/**
 * \file   Settings.h
 * \brief  edit person data
 */


#ifndef iTeacher_SettingsH
#define iTeacher_SettingsH
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
                  Settings(QWidget *parent);

protected:
    virtual void  closeEvent(QCloseEvent *event);

private:
    Ui::UiSettings ui;

    void          _construct();
    void          _destruct();
    void          _initMain();

    void          _resetAll();
    void          _saveAll();

    // friend class Ini;

    Q_OBJECT
    Q_DISABLE_COPY(Settings)

private Q_SLOTS:
    void          bbxButtons_onClicked(QAbstractButton *button);
    void          twGroups_onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_SettingsH
