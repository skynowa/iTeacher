/**
 * \file   TagsEditor.cpp
 * \brief
 */


#include "TagsEditor.h"

#include "../QtLib/Utils.h"

#include <xLib/Core/Application.h>


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
TagsEditor::TagsEditor(
    QWidget            *a_parent,
    const QSqlDatabase &a_db
) :
    QDialog(a_parent),
    _db    (a_db),
    _model (this, a_db)
{
    qTEST_PTR(a_parent);
    qTEST_NA(a_db);

    _construct();
}
//-------------------------------------------------------------------------------------------------
/* virtual */
TagsEditor::~TagsEditor()
{
    _destruct();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
TagsEditor::_construct()
{
    ui.setupUi(this);

    // _model
    {
        _model.setTable(DB_T_TAGS);
        _model.setHeaderData(0, Qt::Horizontal, tr("Id"),   Qt::DisplayRole);
        _model.setHeaderData(1, Qt::Horizontal, tr("Name"), Qt::DisplayRole);
        _model.setEditStrategy(QSqlTableModel::OnFieldChange);

        bool bRv = _model.select();
        qTEST(bRv);
    }

    // tvTags
    {
        ui.tvTags->setModel(&_model);
        ui.tvTags->viewport()->installEventFilter(this);

        ui.tvTags->setColumnWidth(0, TVTAGS_COLUMN_WIDTH_0);
        ui.tvTags->setColumnWidth(1, TVTAGS_COLUMN_WIDTH_1);
        ui.tvTags->verticalHeader()->setVisible(true);
        ui.tvTags->verticalHeader()->setDefaultSectionSize(TABLEVIEW_ROW_HEIGHT);

        ui.tvTags->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui.tvTags->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui.tvTags->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui.tvTags->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        ui.tvTags->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        ui.tvTags->setAlternatingRowColors(true);
        ui.tvTags->setStyleSheet("alternate-background-color: white; background-color: gray;");
        ui.tvTags->setSortingEnabled(true);
        ui.tvTags->sortByColumn(0, Qt::AscendingOrder);

        ui.tvTags->show();
    }
}
//-------------------------------------------------------------------------------------------------
void
TagsEditor::_destruct()
{
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private slots
*
**************************************************************************************************/
