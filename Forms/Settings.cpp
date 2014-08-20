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
    // load settings from INI
    Main::ImportExportOrder importExportOrder = Main::ieoUnknown;
    bool    isHideOnCLose  = false;
    QString shortcutShowHide;
    QString shortcutImportClipboard;
    {
        QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

        settings.beginGroup("file");
        importExportOrder = static_cast<Main::ImportExportOrder>( settings.value("import_export_order", Main::ieoTermValue).toInt() );
        isHideOnCLose     = settings.value("hide_on_close", false).toBool();
        settings.endGroup();

        settings.beginGroup("shortcuts");
        // Sample: Ctrl+Shift+F12
        shortcutShowHide        = settings.value("show_hide", "Shift+F1").toString();
        shortcutImportClipboard = settings.value("clipboard_import", "F1").toString();
        settings.endGroup();
    }

    // apply settings
    {
        // file
        {
            switch (importExportOrder) {
            case Main::ieoTermValue:
                ui.chkImportExportOrder->setChecked(true);
                break;
            case Main::ieoValueTerm:
                ui.chkImportExportOrder->setChecked(false);
                break;
            case Main::ieoUnknown:
            default:
                qTEST(false);
                break;
            }

            ui.chkHideOnClose->setChecked(isHideOnCLose);
        }

        // shortcuts
        {
            ui.kedtAppShowHide->setKeySequence( QKeySequence(shortcutShowHide) );
            ui.kedtImportClipboard->setKeySequence( QKeySequence(shortcutImportClipboard) );
        }
    }
}
//-------------------------------------------------------------------------------------------------
void
Settings::_settingsSave()
{
    QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

    // save settings to INI
    {
        // file
        settings.beginGroup("file");
        {
            Qt::CheckState state = ui.chkImportExportOrder->checkState();
            switch (state) {
            case Qt::Checked:
                settings.setValue("import_export_order", Main::ieoTermValue);
                break;
            case Qt::Unchecked:
                settings.setValue("import_export_order", Main::ieoValueTerm);
                break;
            case Qt::PartiallyChecked:
            default:
                qTEST(false);
                break;
            }
        }
        settings.setValue("hide_on_close", ui.chkHideOnClose->isChecked());
        settings.endGroup();

        // shortcuts
        settings.beginGroup("shortcuts");
        settings.setValue("show_hide",        ui.kedtAppShowHide->keySequence().toString());
        settings.setValue("clipboard_import", ui.kedtImportClipboard->keySequence().toString());
        settings.endGroup();
    }

    // apply settings to UI
    {
        // file
        {
            Qt::CheckState state = ui.chkImportExportOrder->checkState();
            switch (state) {
            case Qt::Checked:
                _wndMain->_importExportOrder = Main::ieoTermValue;
                break;
            case Qt::Unchecked:
                _wndMain->_importExportOrder = Main::ieoValueTerm;
                break;
            case Qt::PartiallyChecked:
            default:
                qTEST(false);
                break;
            }

            _wndMain->_isHideOnCLose = ui.chkHideOnClose->isChecked();
        }

        // shortcuts
        {
            _wndMain->_scShowHide.setShortcut( ui.kedtAppShowHide->keySequence() );
            _wndMain->_scImportClipboard.setShortcut( ui.kedtImportClipboard->keySequence() );
        }
    }
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
