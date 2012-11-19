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

//---------------------------------------------------------------------------
CCheckBoxItemDelegate::CCheckBoxItemDelegate(
    QObject *parent /* = NULL */
) :
    QStyledItemDelegate(parent)
{

}
//---------------------------------------------------------------------------
CCheckBoxItemDelegate::~CCheckBoxItemDelegate() {

}
//---------------------------------------------------------------------------
QWidget *
CCheckBoxItemDelegate::createEditor(
    QWidget                    *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex          &index
) const
{
    CCenteredCheckBox *editor = new CCenteredCheckBox(parent);

    editor->checkBox()->setChecked(defaultValue);

    QCheckBox *chk = editor->checkBox();

    connect(chk,                 SIGNAL(pressed()),
            this,                SLOT  (slot_OnToggled()));

    return editor;
}
//---------------------------------------------------------------------------
void
CCheckBoxItemDelegate::setEditorData(
    QWidget           *editor,
    const QModelIndex &index
) const
{
    QVariant data = index.model()->data(index, Qt::EditRole);

    bool value;

    if (!data.isNull()){
        value = data.toBool();
    } else {
        value = defaultValue;
    }

    CCenteredCheckBox *checkBoxWidget = static_cast<CCenteredCheckBox*>(editor);

    checkBoxWidget->checkBox()->setChecked(value);
}
//---------------------------------------------------------------------------
void
CCheckBoxItemDelegate::setModelData(
    QWidget            *editor,
    QAbstractItemModel *model,
    const QModelIndex  &index
) const
{
    CCenteredCheckBox *checkBoxWidget = static_cast<CCenteredCheckBox*>(editor);
    bool value = checkBoxWidget->checkBox()->isChecked();

    model->setData(index, value, Qt::EditRole);
}
//---------------------------------------------------------------------------
void
CCheckBoxItemDelegate::updateEditorGeometry(
    QWidget                    *editor,
    const QStyleOptionViewItem &option,
    const QModelIndex          &index
) const
{
    CCenteredCheckBox *checkBoxWidget = static_cast<CCenteredCheckBox*>(editor);

    QSize size = checkBoxWidget->sizeHint();

    editor->setMinimumHeight(size.height());
    editor->setMinimumWidth(size.width());
    editor->setGeometry(option.rect);
}
//---------------------------------------------------------------------------
void
CCheckBoxItemDelegate::paint(
    QPainter                   *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex          &index
) const
{
    const QVariant value = index.data();

    if (!value.isValid() || qVariantCanConvert<bool>(value)) {
        bool boolVal = value.isValid() ? value.toBool() : defaultValue;

        QStyle *style        = qApp->style();
        QRect   checkBoxRect = style->subElementRect(QStyle::SE_CheckBoxIndicator, &option);

        int chkWidth  = checkBoxRect.width();
        int chkHeight = checkBoxRect.height();

        int centerX   = option.rect.left() + qMax(option.rect.width()  / 2 - chkWidth  / 2, 0);
        int centerY   = option.rect.top()  + qMax(option.rect.height() / 2 - chkHeight / 2, 0);

        QStyleOptionViewItem modifiedOption(option);
        modifiedOption.rect.moveTo(centerX, centerY);
        modifiedOption.rect.setSize(QSize(chkWidth, chkHeight));

        if (boolVal) {
            modifiedOption.state |= QStyle::State_On;
        }

        style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &modifiedOption, painter);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
//---------------------------------------------------------------------------
void CCheckBoxItemDelegate::slot_OnToggled()
{
    qDebug() << "slot_OnToggled";
}
//---------------------------------------------------------------------------
