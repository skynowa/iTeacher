/**
 * \file   CCheckBoxItemDelegate.cpp
 * \brief
 */


#include "CCheckBoxItemDelegate.h"

#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QCheckBox>
#include <QDebug>

#include "CCenteredCheckBox.h"


static const bool defaultValue = false;

//------------------------------------------------------------------------------
CCheckBoxItemDelegate::CCheckBoxItemDelegate(
    QObject *a_parent /* = NULL */
) :
    QStyledItemDelegate(a_parent)
{

}
//------------------------------------------------------------------------------
CCheckBoxItemDelegate::~CCheckBoxItemDelegate() {

}
//------------------------------------------------------------------------------
QWidget *
CCheckBoxItemDelegate::createEditor(
    QWidget                    *a_parent,
    const QStyleOptionViewItem &a_option,
    const QModelIndex          &a_index
) const
{
    Q_UNUSED(a_option);
    Q_UNUSED(a_index);

    CCenteredCheckBox *editor = new CCenteredCheckBox(a_parent);

    editor->checkBox()->setChecked(defaultValue);

    QCheckBox *chkEditor = editor->checkBox();

    connect(chkEditor, SIGNAL(pressed()),
            this,      SLOT  (slot_OnToggled()));

    return editor;
}
//------------------------------------------------------------------------------
void
CCheckBoxItemDelegate::setEditorData(
    QWidget           *a_editor,
    const QModelIndex &a_index
) const
{
    QVariant data = a_index.model()->data(a_index, Qt::EditRole);

    bool value;

    if (!data.isNull()){
        value = data.toBool();
    } else {
        value = defaultValue;
    }

    CCenteredCheckBox *checkBoxWidget = static_cast<CCenteredCheckBox*>(a_editor);

    checkBoxWidget->checkBox()->setChecked(value);
}
//------------------------------------------------------------------------------
void
CCheckBoxItemDelegate::setModelData(
    QWidget            *a_editor,
    QAbstractItemModel *a_model,
    const QModelIndex  &a_index
) const
{
    CCenteredCheckBox *checkBoxWidget = static_cast<CCenteredCheckBox*>(a_editor);
    bool value = checkBoxWidget->checkBox()->isChecked();

    a_model->setData(a_index, value, Qt::EditRole);
}
//------------------------------------------------------------------------------
void
CCheckBoxItemDelegate::updateEditorGeometry(
    QWidget                    *a_editor,
    const QStyleOptionViewItem &a_option,
    const QModelIndex          &a_index
) const
{
    Q_UNUSED(a_index);

    CCenteredCheckBox *checkBoxWidget = static_cast<CCenteredCheckBox*>(a_editor);

    QSize size = checkBoxWidget->sizeHint();

    a_editor->setMinimumHeight(size.height());
    a_editor->setMinimumWidth(size.width());
    a_editor->setGeometry(a_option.rect);
}
//------------------------------------------------------------------------------
void
CCheckBoxItemDelegate::paint(
    QPainter                   *a_painter,
    const QStyleOptionViewItem &a_option,
    const QModelIndex          &a_index
) const
{
    const QVariant value = a_index.data();

    if (!value.isValid() || qVariantCanConvert<bool>(value)) {
        bool boolVal = value.isValid() ? value.toBool() : defaultValue;

        QStyle *style        = qApp->style();
        QRect   checkBoxRect = style->subElementRect(QStyle::SE_CheckBoxIndicator, &a_option);

        int chkWidth  = checkBoxRect.width();
        int chkHeight = checkBoxRect.height();

        int centerX   = a_option.rect.left() + qMax(a_option.rect.width()  / 2 - chkWidth  / 2, 0);
        int centerY   = a_option.rect.top()  + qMax(a_option.rect.height() / 2 - chkHeight / 2, 0);

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
//------------------------------------------------------------------------------
void CCheckBoxItemDelegate::slot_OnToggled() {
    qDebug() << "slot_OnToggled";
}
//------------------------------------------------------------------------------
