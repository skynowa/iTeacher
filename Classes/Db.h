/**
 * \file
 * \brief
 */


#pragma once

//-------------------------------------------------------------------------------------------------
#include <QtLib/Common.h>
#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
#include "../QtLib/Db/SqlNavigator.h"
#include "../QtLib/Db/SqlRelationalTableModelEx.h"
#include "../QtLib/GlobalShortcut/GlobalShortcut.h"
#include <QObject>

#include <xLib/Core/Core.h>
#include <xLib/Core/Const.h>
#include <xLib/Core/String.h>
#include <xLib/Package/Application.h>
#include <xLib/Fs/Path.h>
#include <xLib/Fs/Finder.h>

#include "../Config.h"
//-------------------------------------------------------------------------------------------------
class Db :
    public QObject
{
public:
    Db(QObject *parent, cQString &filePath, qtlib::SqlRelationalTableModelEx *model,
        QTableView *tableView);

    void reopen();

signals:

public slots:

private:
    Q_OBJECT

    cQString                          _filePath;
    QSqlDatabase                     *_db {};
    qtlib::SqlRelationalTableModelEx *_model {};
    QTableView                       *_tableView {};

    void open();
    void close();

    void _openModel();
    void _closeModel();
};
//-------------------------------------------------------------------------------------------------
