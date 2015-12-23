/**
 * \file   Settings.h
 * \brief  edit person data
 */


#pragma once

//-------------------------------------------------------------------------------------------------
#include "ui_Settings.h"

#include <QtLib/Common.h>
#include <xLib/xLib.h>
#include "../Config.h"
#include "Main.h"
//-------------------------------------------------------------------------------------------------
class Settings :
    public QDialog
{
public:
                 Settings(QWidget *parent, Main *wndMain);

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::UiSettings ui;
    Main        *_wndMain;

    void         _construct();
    void         _destruct();
    void         _initMain();

    void         _resetAll();
    void         _saveAll();

    void         _settingsLoad();
    void         _settingsSave();

    // friend class Ini;

    Q_OBJECT
    Q_DISABLE_COPY(Settings)

private Q_SLOTS:
    void         bbxButtons_onClicked(QAbstractButton *button);
    void         twGroups_onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};
//-------------------------------------------------------------------------------------------------
