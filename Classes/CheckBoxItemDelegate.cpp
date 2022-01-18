/**
 * \file   CheckBoxItemDelegate.cpp
 * \brief
 */


#include "CheckBoxItemDelegate.h"

#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QCheckBox>
#include <QDebug>

#include "CenteredCheckBox.h"


//-------------------------------------------------------------------------------------------------
namespace
{

const bool defaultValue = false;

}
//-------------------------------------------------------------------------------------------------
CheckBoxItemDelegate::CheckBoxItemDelegate(
    QObject *a_parent
) :
    QStyledItemDelegate(a_parent)
{
}
//-------------------------------------------------------------------------------------------------
QWidget *
CheckBoxItemDelegate::createEditor(
    QWidget                    *a_parent,
    const QStyleOptionViewItem &a_option,
    cQModelIndex               &a_index
) const
{
    Q_UNUSED(a_option);
    Q_UNUSED(a_index);

    CenteredCheckBox *editor = new CenteredCheckBox(a_parent);
    editor->checkBox()->setChecked(::defaultValue);

    QCheckBox *chkEditor = editor->checkBox();
    connect(chkEditor, &QCheckBox::pressed,
            this,      &CheckBoxItemDelegate::toggled);

    return editor;
}
//-------------------------------------------------------------------------------------------------
void
CheckBoxItemDelegate::setEditorData(
    QWidget      *a_editor,
    cQModelIndex &a_index
) const
{
    cQVariant data = a_index.model()->data(a_index, Qt::EditRole);

    bool value = false;

    if (!data.isNull()){
        value = data.toBool();
    } else {
        value = defaultValue;
    }

    CenteredCheckBox *checkBoxWidget = static_cast<CenteredCheckBox*>(a_editor);
    checkBoxWidget->checkBox()->setChecked(value);
}
//-------------------------------------------------------------------------------------------------
void
CheckBoxItemDelegate::setModelData(
    QWidget            *a_editor,
    QAbstractItemModel *a_model,
    cQModelIndex       &a_index
) const
{
    CenteredCheckBox *checkBoxWidget = static_cast<CenteredCheckBox*>(a_editor);
    cbool value = checkBoxWidget->checkBox()->isChecked();

    a_model->setData(a_index, value, Qt::EditRole);
}
//-------------------------------------------------------------------------------------------------
void
CheckBoxItemDelegate::updateEditorGeometry(
    QWidget                    *a_editor,
    const QStyleOptionViewItem &a_option,
    cQModelIndex               &a_index
) const
{
    Q_UNUSED(a_index);

    CenteredCheckBox *checkBoxWidget = static_cast<CenteredCheckBox*>(a_editor);

    cQSize size = checkBoxWidget->sizeHint();

    a_editor->setMinimumHeight(size.height());
    a_editor->setMinimumWidth(size.width());
    a_editor->setGeometry(a_option.rect);
}
//-------------------------------------------------------------------------------------------------
void
CheckBoxItemDelegate::paint(
    QPainter                   *a_painter,
    const QStyleOptionViewItem &a_option,
    cQModelIndex               &a_index
) const
{
    cQVariant value = a_index.data();

    if (!value.isValid() || value.canConvert<bool>()) {
        cbool boolVal = value.isValid() ? value.toBool() : defaultValue;

        QStyle *style        = qApp->style();
        QRect   checkBoxRect = style->subElementRect(QStyle::SE_CheckBoxIndicator, &a_option);

        cint chkWidth  = checkBoxRect.width();
        cint chkHeight = checkBoxRect.height();

        cint centerX   = a_option.rect.left() + qMax(a_option.rect.width()  / 2 - chkWidth  / 2, 0);
        cint centerY   = a_option.rect.top()  + qMax(a_option.rect.height() / 2 - chkHeight / 2, 0);

        QStyleOptionViewItem modifiedOption(a_option);
        modifiedOption.rect.moveTo(centerX, centerY);
        modifiedOption.rect.setSize(QSize(chkWidth, chkHeight));

        if (boolVal) {
            modifiedOption.state |= QStyle::State_On;
        }

        style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &modifiedOption, a_painter);
    } else {
        QStyledItemDelegate::paint(a_painter, a_option, a_index);
    }
}
//-------------------------------------------------------------------------------------------------
void
CheckBoxItemDelegate::toggled()
{
    qTRACE_FUNC;
}
//-------------------------------------------------------------------------------------------------
