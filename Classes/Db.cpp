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
    QObject  *a_parent,
    cQString &a_filePath
) :
    QObject  (a_parent),
    _filePath{a_filePath}
{
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
        qTEST(_db == nullptr);

        QSqlDatabase::removeDatabase(connectionName);
    }
}
//-------------------------------------------------------------------------------------------------
void
Db::reopen()
{
    qTRACE_FUNC;

    close();
    open();

    // _model
    _model->select();
    /// ui.tvInfo->setModel(_model);
}
//-------------------------------------------------------------------------------------------------
