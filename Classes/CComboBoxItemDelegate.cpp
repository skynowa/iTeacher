/**
 * \file   CComboBoxItemDelegate.cpp
 * \brief
 */


#include "CComboBoxItemDelegate.h"

#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QComboBox>
#include <QDebug>


//------------------------------------------------------------------------------
CComboBoxItemDelegate::CComboBoxItemDelegate(
    QObject *a_parent /* = NULL */
) :
    QStyledItemDelegate(a_parent)
{
}
//------------------------------------------------------------------------------
CComboBoxItemDelegate::~CComboBoxItemDelegate()
{
}
//------------------------------------------------------------------------------
QWidget *
CComboBoxItemDelegate::createEditor(
    QWidget                    *a_parent,
    const QStyleOptionViewItem &a_option,
    const QModelIndex          &a_index
) const
{
    Q_UNUSED(a_option);
    Q_UNUSED(a_index);

    QComboBox *comboBox = new QComboBox(a_parent);

    for (int i = 0; i < 10; ++ i) {
        comboBox->addItem( QString("item_%1").arg(i) );
    }

    return comboBox;
}
//------------------------------------------------------------------------------
void
CComboBoxItemDelegate::setEditorData(
    QWidget           *a_editor,
    const QModelIndex &a_index
) const
{
    cQVariant data  = a_index.model()->data(a_index, Qt::EditRole);
    cint      value = data.toInt();

    QComboBox *comboBox = static_cast<QComboBox *>( a_editor );
    comboBox->setCurrentIndex(value);
}
//------------------------------------------------------------------------------
void
CComboBoxItemDelegate::setModelData(
    QWidget            *a_editor,
    QAbstractItemModel *a_model,
    const QModelIndex  &a_index
) const
{
    QComboBox *comboBox = static_cast<QComboBox *>( a_editor );
    cint       value    = comboBox->currentIndex();

    a_model->setData(a_index, value, Qt::EditRole);
}
//------------------------------------------------------------------------------
void
CComboBoxItemDelegate::updateEditorGeometry(
    QWidget                    *a_editor,
    const QStyleOptionViewItem &a_option,
    const QModelIndex          &a_index
) const
{
    Q_UNUSED(a_index);

    QComboBox *cb = static_cast<QComboBox *>(a_editor);

    cQSize size = cb->sizeHint();

    a_editor->setMinimumHeight(size.height());
    a_editor->setMinimumWidth(size.width());
    a_editor->setGeometry(a_option.rect);
}
//------------------------------------------------------------------------------
void
CComboBoxItemDelegate::paint(
    QPainter                   *a_painter,
    const QStyleOptionViewItem &a_option,
    const QModelIndex          &a_index
) const
{
#if 0
    QStyleOptionViewItemV4 myOption = a_option;
    QString                text     = Items[a_index.row()].c_str();

    myOption.text = text;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, a_painter);
#endif
}
//------------------------------------------------------------------------------
