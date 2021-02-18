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
void
Settings::closeEvent(
    QCloseEvent *a_event
) /* override */
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
        setWindowTitle( qS2QS(xl::package::Application::info().name) );
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
        connect(ui.bbxButtons,                  &QDialogButtonBox::clicked,
                this,                           &Settings::bbxButtons_onClicked);
        connect(ui.twGroups,                    &QTreeWidget::currentItemChanged,
                this,                           &Settings::twGroups_onCurrentItemChanged);

        connect(ui.tbtnAppShowHideClear,        &QToolButton::clicked,
                ui.kedtAppShowHide,             &QKeySequenceEdit::clear);

        connect(ui.tbtnQuickClipboardTranslate, &QToolButton::clicked,
                ui.kedtQuickClipboardTranslate, &QKeySequenceEdit::clear);

        connect(ui.tbtnImportClipboardClear,    &QToolButton::clicked,
                ui.kedtImportClipboard,         &QKeySequenceEdit::clear);
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
    QString shortcutQuickClipboardTranslate;
    QString shortcutImportClipboard;
    {
        QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

        settings.beginGroup(CFG_GROUP_FILE);
        importExportOrder = static_cast<Main::ImportExportOrder>( settings.value(CFG_IMPORT_EXPORT_ORDER, Main::ieoTermValue).toInt() );
        isHideOnCLose     = settings.value(CFG_HIDE_ON_CLOSE, false).toBool();
        settings.endGroup();

        settings.beginGroup(CFG_GROUP_SHORTCUTS);
        // Sample: Ctrl+Shift+F12
        shortcutShowHide                = settings.value(CFG_SHOW_HIDE, "Ctrl+F1").toString();
        shortcutQuickClipboardTranslate = settings.value(CFG_QUICK_CLIPBOARD_TRANSLATE, "F1").toString();
        shortcutImportClipboard         = settings.value(CFG_CLIPBOARD_IMPORT, "Shift+F1").toString();
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
            ui.kedtQuickClipboardTranslate->setKeySequence( QKeySequence(shortcutQuickClipboardTranslate) );
            ui.kedtImportClipboard->setKeySequence( QKeySequence(shortcutImportClipboard) );
        }
    }
}
//-------------------------------------------------------------------------------------------------
void
Settings::_settingsSave()
{
    QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

    // save settings to INI
    {
        // file
        settings.beginGroup(CFG_GROUP_FILE);
        {
            Qt::CheckState state = ui.chkImportExportOrder->checkState();
            switch (state) {
            case Qt::Checked:
                settings.setValue(CFG_IMPORT_EXPORT_ORDER, Main::ieoTermValue);
                break;
            case Qt::Unchecked:
                settings.setValue(CFG_IMPORT_EXPORT_ORDER, Main::ieoValueTerm);
                break;
            case Qt::PartiallyChecked:
            default:
                qTEST(false);
                break;
            }
        }
        settings.setValue(CFG_HIDE_ON_CLOSE, ui.chkHideOnClose->isChecked());
        settings.endGroup();

        // shortcuts
        settings.beginGroup(CFG_GROUP_SHORTCUTS);
        settings.setValue(CFG_SHOW_HIDE,                 ui.kedtAppShowHide->keySequence().toString());
        settings.setValue(CFG_QUICK_CLIPBOARD_TRANSLATE, ui.kedtQuickClipboardTranslate->keySequence().toString());
        settings.setValue(CFG_CLIPBOARD_IMPORT,          ui.kedtImportClipboard->keySequence().toString());
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
            _wndMain->_scShowHide.set( ui.kedtAppShowHide->keySequence() );
            _wndMain->_scQuickClipboardTranslate.set( ui.kedtQuickClipboardTranslate->keySequence() );
            _wndMain->_scImportClipboard.set( ui.kedtImportClipboard->keySequence() );
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
