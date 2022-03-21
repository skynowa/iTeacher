/**
 * \file  Main.h
 * \brief
 */


#pragma once

#include "../Config.h"
#include <xLib/Core/Core.h>
#include "../QtLib/Common.h"
#include <Classes/SqliteDb.h>

#include "ui_Main.h"
//-------------------------------------------------------------------------------------------------
class Main :
    public QMainWindow
{
public:
    Main(QWidget *parent, Qt::WindowFlags flags = Qt::Widget);
    ~Main() final;

    Q_DISABLE_COPY(Main)

private:
    Q_OBJECT

    Ui::UiMain                _ui;
    std::unique_ptr<SqliteDb> _sqliteDb;

    void _initMain();
    void _initActions();

    // settings
    void _settingsLoad();
    void _settingsSave();

private Q_SLOTS:
    // group "Main"
    // void xxxxx() const;
    void exit();
};
//-------------------------------------------------------------------------------------------------
