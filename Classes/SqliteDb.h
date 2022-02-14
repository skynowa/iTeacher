/**
 * \file  SqliteDb.h
 * \brief SQLite database
 */


#pragma once

//-------------------------------------------------------------------------------------------------
#include <QtLib/Common.h>
#include <QtLib/Utils.h>
#include <QtLib/Db/SqlNavigator.h>
#include <QtLib/Db/SqlRelationalTableModelEx.h>
#include <QObject>

#include <xLib/Core/Core.h>
#include <xLib/Package/Application.h>

#include "../Config.h"
//-------------------------------------------------------------------------------------------------
class SqliteDb :
    public QObject
{
public:
    SqliteDb(QObject *parent, cQString &dbPath);
    SqliteDb(QObject *parent, cQString &dbPath, QTableView *view);
    ~SqliteDb() final;

    QSqlDatabase                     *db();
    qtlib::SqlRelationalTableModelEx *model();
    QTableView                       *view();
    qtlib::SqlNavigator              &navigator();

    void reopen();

    std::size_t wordsAll() const;
    std::size_t wordsLearned() const;
    std::size_t wordsNotLearned() const;
    bool        isTerminExists(cQString &term) const;
    bool        isTagsEmpty() const;
    QSqlRecord  randomRow() const;
    QSqlRecord  findByField(cQString &name, cQString &value) const;

private:
    Q_OBJECT

    cQString                       _dbPath;
    std::unique_ptr<QSqlDatabase>  _db;
    std::unique_ptr<qtlib::SqlRelationalTableModelEx> _model;
    QTableView                    *_view {};
    qtlib::SqlNavigator            _navigator;

    void  _dbOpen();
    void  _dbClose();

    void  _modelOpen();
    void  _modelClose();

    ulong _queryCount(cQString &sql) const;
};
//-------------------------------------------------------------------------------------------------
