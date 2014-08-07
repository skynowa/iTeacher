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
    QWidget *a_parent
) :
    QDialog(a_parent)

{
    qTEST_PTR(a_parent);

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

    // ui.chkPhotos_IsDeleteFromDisk->setChecked( Ini::photos_isDeleteFromDisk() );
}
//-------------------------------------------------------------------------------------------------
void
Settings::_destruct()
{
    // Ini::setPhotos_isDeleteFromDisk( ui.chkPhotos_IsDeleteFromDisk->isChecked() );
}
//-------------------------------------------------------------------------------------------------
void
Settings::_initMain()
{
    ui.setupUi(this);

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
                ui.kedtImportClipboar,       &QKeySequenceEdit::clear);
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
