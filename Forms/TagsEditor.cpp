/**
 * \file   TagsEditor.cpp
 * \brief
 */


#include "TagsEditor.h"

#include "../QtLib/Utils.h"
#include <xLib/Package/Application.h>


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
TagsEditor::TagsEditor(
    QWidget            *a_parent,
    const QSqlDatabase &a_db
) :
    QDialog      (a_parent),
    _db          (a_db),
    _model       (this, a_db),
    _sqlNavigator(this)
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

    // slots
    {
        connect(ui.tbtnEditInsert, &QToolButton::clicked,
                this,              &TagsEditor::insert);

        connect(ui.tbtnEditRemove, &QToolButton::clicked,
                this,              &TagsEditor::remove);
    }

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

        ui.tvTags->horizontalHeader()->setStretchLastSection(true);
        ui.tvTags->setColumnWidth(0, TVTAGS_COLUMN_WIDTH_0);
        ui.tvTags->setColumnWidth(1, TVTAGS_COLUMN_WIDTH_1);
        ui.tvTags->verticalHeader()->setVisible(true);
        ui.tvTags->verticalHeader()->setDefaultSectionSize(TABLEVIEW_ROW_HEIGHT);

        ui.tvTags->setEditTriggers(QAbstractItemView::DoubleClicked |
            QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
        ui.tvTags->setSelectionBehavior(QAbstractItemView::SelectItems);
        ui.tvTags->setSelectionMode(QAbstractItemView::SingleSelection);
        ui.tvTags->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        ui.tvTags->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        ui.tvTags->setAlternatingRowColors(true);
        ui.tvTags->setStyleSheet("alternate-background-color: white; background-color: gray;");
        ui.tvTags->setSortingEnabled(true);
        ui.tvTags->sortByColumn(0, Qt::AscendingOrder);

        ui.tvTags->show();
    }

    // _sqlNavigator
    {
        /// TODO:  _sqlNavigator.construct(&_model, ui.tvTags);
        _sqlNavigator.last();
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

//-------------------------------------------------------------------------------------------------
void
TagsEditor::insert()
{
    qCHECK_DO(!_sqlNavigator.isValid(), return);

    _sqlNavigator.insert();
}
//-------------------------------------------------------------------------------------------------
void
TagsEditor::remove()
{
    qCHECK_DO(_sqlNavigator.view()->currentIndex().row() < 0, return);

    QString text;
    QString informativeText;
    {
        cint       currentRow = _sqlNavigator.view()->currentIndex().row();
        QSqlRecord record     = _sqlNavigator.model()->record(currentRow);

        cQString   wordTerm   = record.value(DB_F_TAGS_ID).toString();
        cQString   wordValue  = record.value(DB_F_TAGS_NAME).toString();

        text            = QString(tr("Remove record number %1?"))
                            .arg(currentRow + 1);
        informativeText = QString(tr("%2 - %3"))
                            .arg(wordTerm)
                            .arg(wordValue);
    }

    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int iRv = msgBox.exec();
    switch (iRv) {
    case QMessageBox::Yes:
        _sqlNavigator.remove();
        break;
    case QMessageBox::Cancel:
    default:
        break;
    }
}
//-------------------------------------------------------------------------------------------------
