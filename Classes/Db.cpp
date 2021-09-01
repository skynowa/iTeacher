/**
 * \file
 * \brief
 */

#include "Db.h"


/**************************************************************************************************
*   private: DB
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
Db::Db(
    QObject                          *a_parent,
    cQString                         &a_filePath,
    qtlib::SqlRelationalTableModelEx *a_model,
    QTableView                       *a_tableView
) :
    QObject   (a_parent),
    _filePath {a_filePath},
    _model    {a_model},
    _tableView{a_tableView}
{
    qTEST_PTR(a_parent);
    qTEST(!a_filePath.isEmpty())
    qTEST_PTR(_model);
    qTEST_PTR(a_tableView);
}
//-------------------------------------------------------------------------------------------------
void
Db::open()
{
    qTRACE_FUNC;

    bool bRv {};

    cQString &tableName = QFileInfo(_filePath).baseName();
    qDebug() << qTRACE_VAR(tableName);

    // _db
    {
        // prepare DB directory
        {
            QDir dir;
            dir.setPath( qS2QS(xl::package::Application::dbDirPath()) );

            bRv = dir.exists();
            if (!bRv) {
                bRv = QDir().mkpath( qS2QS(xl::package::Application::dbDirPath()) );
                qTEST(bRv);
            }

            qTEST( dir.exists() );
        }

        qTEST(_db == nullptr);

        bRv = QSqlDatabase::isDriverAvailable("QSQLITE");
        qCHECK_DO(!bRv, qMSG(QSqlDatabase().lastError().text()); return);

        _db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
        _db->setDatabaseName(_filePath);

        bRv = _db->open();
        qCHECK_PTR(bRv, _db);

        // DB pragma
        {
            QSqlQuery qryPragma(*_db);

            cQString sql = "PRAGMA foreign_keys = ON";

            bRv = qryPragma.exec(sql);
            qCHECK_REF(bRv, qryPragma);
        }

        // create DB - DB_T_TAGS
        {
            QSqlQuery qryTags(*_db);

            cQString sql =
                "CREATE TABLE IF NOT EXISTS "
                "    " DB_T_TAGS
                "( "
                "    " DB_F_TAGS_ID   " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                "    " DB_F_TAGS_NAME " varchar(255) DEFAULT '' UNIQUE "
                ")";

            bRv = qryTags.exec(sql);
            qCHECK_REF(bRv, qryTags);
        }

        // create DB
        {
            QSqlQuery qryMain(*_db);

            cQString sql =
                "CREATE TABLE IF NOT EXISTS "
                "    " + tableName +
                "( "
                "    " DB_F_MAIN_ID         " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                "    " DB_F_MAIN_TERM       " varchar(255) UNIQUE, "
                "    " DB_F_MAIN_VALUE      " varchar(255) DEFAULT '', "
                "    " DB_F_MAIN_IS_LEARNED " integer      DEFAULT 0, "
                "    " DB_F_MAIN_IS_MARKED  " integer      DEFAULT 0, "
                "    " DB_F_MAIN_TAG        " integer      DEFAULT 1, "
                " "
                "    FOREIGN KEY (" DB_F_MAIN_TAG ") REFERENCES " DB_T_TAGS "(" DB_F_TAGS_ID ") "
                "    ON UPDATE CASCADE "
                ")";

            bRv = qryMain.exec(sql);
            qCHECK_REF(bRv, qryMain);
        }
    }
}
//-------------------------------------------------------------------------------------------------
void
Db::close()
{
    qTRACE_FUNC;

    if (_db == nullptr) {
        return;
    }

    qTEST(_db->isOpen());

    cQString connectionName = _db->connectionName();

    _db->close();
    qTEST(!_db->isOpen());

    qPTR_DELETE(_db);
    qTEST(_db == nullptr);

    QSqlDatabase::removeDatabase(connectionName);
}
//-------------------------------------------------------------------------------------------------
void
Db::reopen()
{
    qTRACE_FUNC;

    _closeModel();
    qTEST(_model == nullptr);

    close();
    qTEST(_db == nullptr);

    open();
    qTEST_PTR(_db);

    _openModel();
    qTEST_PTR(_model);

    _tableView->setModel(_model);
}
//-------------------------------------------------------------------------------------------------
void
Db::_openModel()
{
    if ( _filePath.isEmpty() ) {
        qTEST(_model == nullptr);
        /// createDb();
        qTEST(_model == nullptr);

        return;
    }

    cQString dictPath = qS2QS(xl::package::Application::dbDirPath()) + QDir::separator() + _filePath;

    // _model
    {
        qTEST(_model == nullptr);

        cQString &tableName = QFileInfo(dictPath).baseName();
        qTEST(!tableName.isEmpty());

        _model = new qtlib::SqlRelationalTableModelEx(this, *_db);
        _model->setTable(tableName);
        _model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
        _model->setRelation(5, QSqlRelation(DB_T_TAGS, DB_F_TAGS_ID, DB_F_TAGS_NAME));

        for (const auto &it_tableViewHeader : ::tableViewHeaders) {
            _model->setHeaderData(it_tableViewHeader.section, Qt::Horizontal,
                it_tableViewHeader.value, Qt::DisplayRole);
        }

        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        _model->select();
    }
}
//-------------------------------------------------------------------------------------------------
void
Db::_closeModel()
{
    if (_model == nullptr) {
        return;
    }

    bool bRv = _model->submitAll();
    if (!bRv &&
        _model->lastError().text().contains("failed", Qt::CaseInsensitive))
    {
        // \see QtLib SqlRelationalTableModelEx::importCsv()

        // lastError(): QSqlError("19", "Unable to fetch row", "UNIQUE constraint failed: t_main.f_main_term")
        // qDebug() << qTRACE_VAR(lastError().text());
    } else {
        qCHECK_PTR(bRv, _model);
    }

    qPTR_DELETE(_model);
}
//-------------------------------------------------------------------------------------------------
