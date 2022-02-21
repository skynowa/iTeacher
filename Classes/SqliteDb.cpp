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
    QObject   *a_parent,    ///< Parent object
    cQString  &a_dbPath     ///< DB file path
) :
    QObject   (a_parent),
    _dbPath   {a_dbPath},
    _view     {nullptr},
    _navigator(a_parent)
{
    qTEST_PTR(a_parent);
    qTEST(!a_dbPath.isEmpty());
    qTEST(QFile(a_dbPath).exists());
}
//-------------------------------------------------------------------------------------------------
SqliteDb::SqliteDb(
    QObject    *a_parent,   ///<
    cQString   &a_dbPath,   ///<
    QTableView *a_view      ///<
) :
    QObject   (a_parent),
    _dbPath   {a_dbPath},
    _view     {a_view},
    _navigator(a_parent)
{
    qTEST_PTR(a_parent);
    qTEST(!a_dbPath.isEmpty());
    qTEST(QFile(a_dbPath).exists());
    qTEST_PTR(a_view);
}
//-------------------------------------------------------------------------------------------------
SqliteDb::SqliteDb(
    QObject                                *a_parent,
    const QSqlDatabase                     *a_db,
    const qtlib::SqlRelationalTableModelEx &a_model
) :
    QObject   (a_parent),
    _dbPath   {},
    _db       ( const_cast<QSqlDatabase *>(a_db) ),
    _view     {},
    _navigator(a_parent),
    _model    ( const_cast<qtlib::SqlRelationalTableModelEx *>(&a_model) )
{
}
//-------------------------------------------------------------------------------------------------
SqliteDb::~SqliteDb()
{
    if ( _dbPath.isEmpty() ) {
        _model.release();
        _db.release();
        return;
    }

    _modelClose();
    qTEST(_model == nullptr);

    _dbClose();
    qTEST(_db == nullptr);
}
//-------------------------------------------------------------------------------------------------
QSqlDatabase *
SqliteDb::db() const
{
    qTEST(_db);
    return _db.get();
}
//-------------------------------------------------------------------------------------------------
qtlib::SqlRelationalTableModelEx *
SqliteDb::model() const
{
    qTEST(_model);
    return _model.get();
}
//-------------------------------------------------------------------------------------------------
QTableView *
SqliteDb::view() const
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
    if ( _dbPath.isEmpty() ) {
        return;
    }

    _modelClose();
    qTEST(_model == nullptr);

    _dbClose();
    qTEST(_db == nullptr);

    _dbOpen();
    qTEST_PTR(_db);

    _modelOpen();
    qTEST_PTR(_model);

    if (_view != nullptr) {
        view()->setModel(_model.get());

        _navigator.construct(model(), view());
        _navigator.last();
    }
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
#if 0
    qTEST(!a_term.isEmpty())
    qTEST(_model);

    cQString sql =
        "SELECT count(*) "
        "FROM  " + _model->tableName() + " "
        "WHERE " DB_F_MAIN_TERM " = " + a_term.trimmed();

    return (_queryCount(sql) > 0);
#else
    qTEST(_db);
    qTEST(_model);
    qTEST(!a_term.isEmpty())

    bool      bRv {};
    QSqlQuery query( *db() );

    cQString sql =
        "SELECT count(1) "
        "FROM  " + _model->tableName() + " "
        "WHERE " DB_F_MAIN_TERM " = :term";

    query.prepare(sql);
    query.bindValue(":term", a_term.trimmed());

    bRv = query.exec();
    qCHECK_REF(bRv, query);

    bRv = query.next();
    qCHECK_REF(bRv, query);

    bRv = query.value(0).toBool();

    return bRv;
#endif
}
//-------------------------------------------------------------------------------------------------
QSqlRecord
SqliteDb::randomRow() const
{
    const bool option_notLearned {true};

    bool bRv {};

    cQString sql =
        "SELECT * "
        "FROM  " + _model->tableName() + " " +
        (option_notLearned ? "WHERE " DB_F_MAIN_IS_LEARNED " = 0 " : "") +
        "ORDER BY RANDOM() "
        "LIMIT 1;";

    QSqlQuery query(*_db);
    bRv = query.exec(sql);
    qCHECK_REF(bRv, query);

    bRv = query.next();
    qCHECK_REF(bRv, query);

    return query.record();
}
//-------------------------------------------------------------------------------------------------
QSqlRecord
SqliteDb::findByField(
    cQString &a_name,
    cQString &a_value
) const
{
    bool bRv {};

    cQString sql =
        "SELECT * "
        "FROM  " DB_T_TAGS " "
        "WHERE " + a_name + "=" + a_value + " " +
        "LIMIT 1";

    QSqlQuery query(*_db);
    bRv = query.exec(sql);
    qCHECK_REF(bRv, query);

    bRv = query.next();
    qCHECK_REF(bRv, query);

    return query.record();
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

    cQString &tableName = QFileInfo(_dbPath).baseName();
    // qDebug() << qTRACE_VAR(tableName);

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
        _db->setDatabaseName(_dbPath);

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
    if ( _dbPath.isEmpty() ) {
        qWarning() << qTRACE_VAR(_dbPath.isEmpty());
        return;
    }

    // _model
    {
        qTEST(_model == nullptr);

        cQString &tableName = QFileInfo(_dbPath).baseName();
        qTEST(!tableName.isEmpty());

        _model.reset(new qtlib::SqlRelationalTableModelEx(this, *_db));
        _model->setTable(tableName);
        _model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
        _model->setRelation(5, QSqlRelation(DB_T_TAGS, DB_F_TAGS_ID, DB_F_TAGS_NAME));

        for (const auto &[it_section, it_value] : ::tableViewHeaders) {
            _model->setHeaderData(it_section, Qt::Horizontal, it_value, Qt::DisplayRole);
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
    qTEST_PTR(_db);

    bool bRv {};

    QSqlQuery query( *_db.get() );
    bRv = query.exec(a_sql);
    qCHECK_REF(bRv, query);

    bRv = query.next();
    qCHECK_REF(bRv, query);

    return query.value(0).toULongLong();
}
//-------------------------------------------------------------------------------------------------
