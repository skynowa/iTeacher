/**
 * \file  Db.h
 * \brief
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
class Db :
    public QObject
{
public:
    Db(QObject *parent, cQString &filePath, QTableView *tableView);
   ~Db();

    QSqlDatabase                     *db();
    qtlib::SqlRelationalTableModelEx *model();
    QTableView                       *view();
    qtlib::SqlNavigator              &navigator();

    void reopen();

    std::size_t wordsAll() const;
    std::size_t wordsLearned() const;
    std::size_t wordsNotLearned() const;
    bool        tagsIsEmpty() const;

private:
    Q_OBJECT

    cQString                          _filePath;
    std::unique_ptr<QSqlDatabase>                     _db;
    std::unique_ptr<qtlib::SqlRelationalTableModelEx> _model;
    QTableView                       *_tableView {};
    qtlib::SqlNavigator               _sqlNavigator;

    void  _open();
    void  _close();

    void  _openModel();
    void  _closeModel();

    ulong _queryCount(cQString &sql) const;
};
//-------------------------------------------------------------------------------------------------
