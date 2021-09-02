/**
 * \file  Db.cpp
 * \brief
 */

#include "Db.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
Db::Db(
    QObject    *a_parent,   ///<
    cQString   &a_filePath, ///<
    QTableView *a_tableView ///<
) :
    QObject      (a_parent),
    _filePath    {a_filePath},
    _tableView   {a_tableView},
    _sqlNavigator(a_parent)
{
    qTEST_PTR(a_parent);
    qTEST(!a_filePath.isEmpty())
    qTEST_PTR(a_tableView);
    qTEST_NA(_sqlNavigator);
}
//-------------------------------------------------------------------------------------------------
Db::~Db()
{
    _closeModel();
    qTEST(_model == nullptr);

    _close();
    qTEST(_db == nullptr);
}
//-------------------------------------------------------------------------------------------------
QSqlDatabase *
Db::db()
{
    qTEST(_db);
    return _db.get();
}
//-------------------------------------------------------------------------------------------------
qtlib::SqlRelationalTableModelEx *
Db::model()
{
    qTEST(_model);
    return _model.get();
}
//-------------------------------------------------------------------------------------------------
QTableView *
Db::view()
{
    qTEST_PTR(_tableView);
    return _tableView;
}
//-------------------------------------------------------------------------------------------------
qtlib::SqlNavigator &
Db::navigator()
{
    qTEST(_sqlNavigator.isValid());
    return _sqlNavigator;
}
//-------------------------------------------------------------------------------------------------
void
Db::reopen()
{
    _closeModel();
    qTEST(_model == nullptr);

    _close();
    qTEST(_db == nullptr);

    _open();
    qTEST_PTR(_db);

    _openModel();
    qTEST_PTR(_model);

    _tableView->setModel(_model.get());

    _sqlNavigator.construct(model(), view());
    _sqlNavigator.last();
}
//-------------------------------------------------------------------------------------------------
std::size_t
Db::wordsAll() const
{
    cQString sql =
        "SELECT COUNT(*) "
        "FROM  " + _model->tableName() + ";";

    return _queryCount(sql);
}
//-------------------------------------------------------------------------------------------------
std::size_t
Db::wordsLearned() const
{
    cQString sql =
        "SELECT COUNT(*) "
        "FROM  " + _model->tableName() + " "
        "WHERE " DB_F_MAIN_IS_LEARNED " = 1;";

    return _queryCount(sql);
}
//-------------------------------------------------------------------------------------------------
std::size_t
Db::wordsNotLearned() const
{
    cQString sql =
        "SELECT COUNT(*) "
        "FROM  " + _model->tableName() + " "
        "WHERE " DB_F_MAIN_IS_LEARNED " = 0;";

    return _queryCount(sql);
}
//-------------------------------------------------------------------------------------------------
bool
Db::isTagsEmpty() const
{
    cQString sql =
        "SELECT COUNT(*) "
        "FROM  " DB_T_TAGS ";";

    return (_queryCount(sql) == 0);
}
//-------------------------------------------------------------------------------------------------
bool
Db::isTerminExists(
    cQString &a_term
) const
{
    qTEST(!a_term.isEmpty())

    cQString sql =
        "SELECT COUNT(*) "
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
Db::_open()
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
            QSqlQuery query(*_db);

            cQString sql = "PRAGMA foreign_keys = ON";

            bRv = query.exec(sql);
            qCHECK_REF(bRv, query);
        }

        // create DB - DB_T_TAGS
        {
            QSqlQuery query(*_db);

            cQString sql =
                "CREATE TABLE IF NOT EXISTS "
                "    " DB_T_TAGS
                "( "
                "    " DB_F_TAGS_ID   " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                "    " DB_F_TAGS_NAME " varchar(255) DEFAULT '' UNIQUE "
                ")";

            bRv = query.exec(sql);
            qCHECK_REF(bRv, query);
        }

        // create DB
        {
            QSqlQuery query(*_db);

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

            bRv = query.exec(sql);
            qCHECK_REF(bRv, query);
        }
    }
}
//-------------------------------------------------------------------------------------------------
void
Db::_close()
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

    _model.reset();
}
//-------------------------------------------------------------------------------------------------
std::size_t
Db::_queryCount(
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
