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
    QObject *parent
) :
    QObject      (parent),
    _sqlNavigator(this)
{
}
//-------------------------------------------------------------------------------------------------
void
Db::open(
    cQString &a_filePath
)
{
    bool bRv {};

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    cQString &tableName = QFileInfo(a_filePath).baseName();
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
        _db->setDatabaseName(a_filePath);

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
            qTEST(_model == nullptr);

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

    // _model
    {
        qTEST(_model == nullptr);

        _model = new qtlib::SqlRelationalTableModelEx(this, *_db);
        _model->setTable(tableName);
        _model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
        _model->setRelation(5, QSqlRelation(DB_T_TAGS, DB_F_TAGS_ID, DB_F_TAGS_NAME));

        for (size_t i = 0; i < qARRAY_LENGTH(::tableViewHeaders); ++ i) {
            _model->setHeaderData(::tableViewHeaders[i].section, Qt::Horizontal,
                ::tableViewHeaders[i].value, Qt::DisplayRole);
        }

        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        _model->select();
    }

    // _sqlNavigator
    {
        /// _sqlNavigator.construct(_model, ui.tvInfo);
        _sqlNavigator.last();
    }

    QApplication::restoreOverrideCursor();
}
//-------------------------------------------------------------------------------------------------
void
Db::close()
{
    // _model
    if (_model != nullptr) {
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

    // _db
    if (_db != nullptr) {
        qTEST(_db->isOpen());

        cQString connectionName = _db->connectionName();

        _db->close();
        qTEST(!_db->isOpen());

        qPTR_DELETE(_db);

        QSqlDatabase::removeDatabase(connectionName);
    }
}
//-------------------------------------------------------------------------------------------------
void
Db::reopen(
    cQString &a_filePath
)
{
    close();
    open(a_filePath);

    // _model
    _model->select();
    /// ui.tvInfo->setModel(_model);
}
//-------------------------------------------------------------------------------------------------
