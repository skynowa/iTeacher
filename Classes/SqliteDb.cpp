/**
 * \file  SqliteDb.cpp
 * \brief
 */


#include "SqliteDb.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
SqliteDb::SqliteDb(
    QObject    *a_parent,   ///<
    cQString   &a_filePath, ///<
    QTableView *a_view      ///<
) :
    QObject   (a_parent),
    _filePath {a_filePath},
    _view     {a_view},
    _navigator(a_parent)
{
    qTEST_PTR(a_parent);
    qTEST(!a_filePath.isEmpty())
    qTEST_PTR(a_view);
}
//-------------------------------------------------------------------------------------------------
SqliteDb::~SqliteDb()
{
    _modelClose();
    qTEST(_model == nullptr);

    _dbClose();
    qTEST(_db == nullptr);
}
//-------------------------------------------------------------------------------------------------
QSqlDatabase *
SqliteDb::db()
{
    qTEST(_db);
    return _db.get();
}
//-------------------------------------------------------------------------------------------------
qtlib::SqlRelationalTableModelEx *
SqliteDb::model()
{
    qTEST(_model);
    return _model.get();
}
//-------------------------------------------------------------------------------------------------
QTableView *
SqliteDb::view()
{
    qTEST_PTR(_view);
    return _view;
}
//-------------------------------------------------------------------------------------------------
qtlib::SqlNavigator &
SqliteDb::navigator()
{
    qTEST(_navigator.isValid());
    return _navigator;
}
//-------------------------------------------------------------------------------------------------
void
SqliteDb::reopen()
{
    _modelClose();
    qTEST(_model == nullptr);

    _dbClose();
    qTEST(_db == nullptr);

    _dbOpen();
    qTEST_PTR(_db);

    _modelOpen();
    qTEST_PTR(_model);

    _view->setModel(_model.get());

    _navigator.construct(model(), view());
    _navigator.last();
}
//-------------------------------------------------------------------------------------------------
std::size_t
SqliteDb::wordsAll() const
{
    cQString sql =
        "SELECT count(*) "
        "FROM  " + _model->tableName() + ";";

    return _queryCount(sql);
}
//-------------------------------------------------------------------------------------------------
std::size_t
SqliteDb::wordsLearned() const
{
    cQString sql =
        "SELECT count(*) "
        "FROM  " + _model->tableName() + " "
        "WHERE " DB_F_MAIN_IS_LEARNED " = 1;";

    return _queryCount(sql);
}
//-------------------------------------------------------------------------------------------------
std::size_t
SqliteDb::wordsNotLearned() const
{
    cQString sql =
        "SELECT count(*) "
        "FROM  " + _model->tableName() + " "
        "WHERE " DB_F_MAIN_IS_LEARNED " = 0;";

    return _queryCount(sql);
}
//-------------------------------------------------------------------------------------------------
bool
SqliteDb::isTagsEmpty() const
{
    cQString sql =
        "SELECT count(*) "
        "FROM  " DB_T_TAGS ";";

    return (_queryCount(sql) == 0);
}
//-------------------------------------------------------------------------------------------------
bool
SqliteDb::isTerminExists(
    cQString &a_term
) const
{
    qTEST(!a_term.isEmpty())

    cQString sql =
        "SELECT count(*) "
        "FROM  " + _model->tableName() + " "
        "WHERE " DB_F_MAIN_TERM " = " + a_term.trimmed();

    return (_queryCount(sql) > 0);
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
SqliteDb::_dbOpen()
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

        _db.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE")));
        _db->setDatabaseName(_filePath);

        bRv = _db->open();
        qCHECK_PTR(bRv, _db);

        // DB pragma
        {
            cQString sql = "PRAGMA foreign_keys = ON";

            QSqlQuery query(*_db);
            bRv = query.exec(sql);
            qCHECK_REF(bRv, query);
        }

        // create DB - DB_T_TAGS
        {
            cQString sql =
                "CREATE TABLE IF NOT EXISTS "
                "    " DB_T_TAGS
                "( "
                "    " DB_F_TAGS_ID   " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                "    " DB_F_TAGS_NAME " varchar(255) DEFAULT '' UNIQUE "
                ")";

            QSqlQuery query(*_db);
            bRv = query.exec(sql);
            qCHECK_REF(bRv, query);
        }

        // create DB
        {
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

            QSqlQuery query(*_db);
            bRv = query.exec(sql);
            qCHECK_REF(bRv, query);
        }
    }
}
//-------------------------------------------------------------------------------------------------
void
SqliteDb::_dbClose()
{
    qTRACE_FUNC;

    if (_db == nullptr) {
        return;
    }

    qTEST(_db->isOpen());

    cQString connectionName = _db->connectionName();

    _db->close();
    qTEST(!_db->isOpen());

    _db.reset();
    qTEST(_db.get() == nullptr);

    QSqlDatabase::removeDatabase(connectionName);
}
//-------------------------------------------------------------------------------------------------
void
SqliteDb::_modelOpen()
{
    if ( _filePath.isEmpty() ) {
        qWarning() << qTRACE_VAR(_filePath.isEmpty());
        return;
    }

    // _model
    {
        qTEST(_model == nullptr);

        cQString &tableName = QFileInfo(_filePath).baseName();
        qTEST(!tableName.isEmpty());

        _model.reset(new qtlib::SqlRelationalTableModelEx(this, *_db));
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
SqliteDb::_modelClose()
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

    _model.reset();
}
//-------------------------------------------------------------------------------------------------
std::size_t
SqliteDb::_queryCount(
    cQString &a_sql
) const
{
    bool bRv {};

    QSqlQuery query(*_db);
    bRv = query.exec(a_sql);
    qCHECK_REF(bRv, query);

    bRv = query.next();
    qCHECK_REF(bRv, query);

    return query.value(0).toULongLong();
}
//-------------------------------------------------------------------------------------------------
