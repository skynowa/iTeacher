/**
 * \file   Settings.h
 * \brief  edit person data
 */


#include "Settings.h"

#include <QtLib/Utils.h>
// #include "../Classes/Ini.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
Settings::Settings(
    QWidget *a_parent,
    Main    *a_wndMain
) :
    QDialog (a_parent),
    _wndMain(a_wndMain)

{
    qTEST_PTR(a_parent);
    qTEST_PTR(a_wndMain);

    _construct();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   protected
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
/* virtual */
void
Settings::closeEvent(
    QCloseEvent *a_event
)
{
    _destruct();
    a_event->accept();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Settings::_construct()
{
    _initMain();
    _settingsLoad();
}
//-------------------------------------------------------------------------------------------------
void
Settings::_destruct()
{
    _settingsSave();
}
//-------------------------------------------------------------------------------------------------
void
Settings::_initMain()
{
    ui.setupUi(this);

    // Main
    {
        setWindowIcon( QIcon(RES_MAIN_ICON) );
        setWindowTitle( qS2QS(xlib::core::Application::name()) );
        // setGeometry(0, 0, APP_WIDTH, APP_HEIGHT);
        qtlib::Utils::widgetAlignCenter(this);
    }

    // ui.twGroups
    {
        // select first item
        for (QTreeWidgetItemIterator it(ui.twGroups); *it; ++ it) {
            if ((*it)->text(0) == "Main") {
                (*it)->setSelected(true);
                break;
            }
        }
    }

    // signals
    {
        connect(ui.bbxButtons,               &QDialogButtonBox::clicked,
                this,                        &Settings::bbxButtons_onClicked);
        connect(ui.twGroups,                 &QTreeWidget::currentItemChanged,
                this,                        &Settings::twGroups_onCurrentItemChanged);

        connect(ui.tbtnAppShowHideClear,     &QToolButton::clicked,
                ui.kedtAppShowHide,          &QKeySequenceEdit::clear);

        connect(ui.tbtnImportClipboardClear, &QToolButton::clicked,
                ui.kedtImportClipboard,      &QKeySequenceEdit::clear);
    }
}
//-------------------------------------------------------------------------------------------------
void
Settings::_resetAll()
{

}
//-------------------------------------------------------------------------------------------------
void
Settings::_saveAll()
{

}
//-------------------------------------------------------------------------------------------------
void
Settings::_settingsLoad()
{
    QString shortcutShowHide;
    QString shortcutImportClipboard;
    {
        QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

        settings.beginGroup("shortcuts");
        // Sample: Ctrl+Shift+F12
        shortcutShowHide        = settings.value("show_hide", "Shift+F1").toString();
        shortcutImportClipboard = settings.value("clipboard_import", "F1").toString();
        settings.endGroup();
    }

    // apply settings
    {
        // shortcuts
        {
            ui.kedtAppShowHide->setKeySequence( QKeySequence(shortcutShowHide) );
            ui.kedtImportClipboard->setKeySequence( QKeySequence(shortcutImportClipboard) );
        }

        _wndMain->ui.cboDictPath->setFocus();
    }
}
//-------------------------------------------------------------------------------------------------
void
Settings::_settingsSave()
{
    QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

    // shortcuts
    settings.beginGroup("shortcuts");
    settings.setValue("show_hide",        ui.kedtAppShowHide->keySequence().toString());
    settings.setValue("clipboard_import", ui.kedtImportClipboard->keySequence().toString());
    settings.endGroup();

    // apply settings to UI
    {
        // shortcuts
        {
            _wndMain->_scShowHide.setShortcut( ui.kedtAppShowHide->keySequence() );
            _wndMain->_scImportClipboard.setShortcut( ui.kedtImportClipboard->keySequence() );
        }
    }

    settings.setValue("show_hide",        _wndMain->_scShowHide.shortcut().toString());
    settings.setValue("clipboard_import", _wndMain->_scImportClipboard.shortcut().toString());
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private slots
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Settings::bbxButtons_onClicked(
    QAbstractButton *a_button
)
{
    QDialogButtonBox::StandardButton sbRv = ui.bbxButtons->standardButton(a_button);
    switch (sbRv) {
    case QDialogButtonBox::Reset:
        _resetAll();
        break;
    case QDialogButtonBox::Ok:
        _saveAll();
        close();
        break;
    default:
        qTEST(false);
        break;
    }
}
//-------------------------------------------------------------------------------------------------
void
Settings::twGroups_onCurrentItemChanged(
    QTreeWidgetItem *a_current,
    QTreeWidgetItem *a_previous
)
{
    Q_UNUSED(a_current);
    Q_UNUSED(a_previous);

    cint index = ui.twGroups->currentIndex().row();

    ui.tabwGroupsDetail->setCurrentIndex(index);
}
//-------------------------------------------------------------------------------------------------
