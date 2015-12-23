/**
 * \file   ComboBoxItemDelegate.cpp
 * \brief
 */


#include "ComboBoxItemDelegate.h"

#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QComboBox>
#include <QDebug>


//-------------------------------------------------------------------------------------------------
ComboBoxItemDelegate::ComboBoxItemDelegate(
    QObject        *a_parent /* = Q_NULLPTR */,
    QSqlTableModel *a_sqlModel
) :
    QStyledItemDelegate(a_parent),
    _sqlModel          (a_sqlModel)
{
}
//-------------------------------------------------------------------------------------------------
ComboBoxItemDelegate::~ComboBoxItemDelegate()
{
}
//-------------------------------------------------------------------------------------------------
QWidget *
ComboBoxItemDelegate::createEditor(
    QWidget                    *a_parent,
    const QStyleOptionViewItem &a_option,
    cQModelIndex               &a_index
) const
{
    Q_UNUSED(a_option);
    Q_UNUSED(a_index);

    // ComboBox ony in column 2
#if 0
    if (a_index.column() != 1) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
#endif

    QComboBox *comboBox = new QComboBox(a_parent);

    QSqlQuery query;
    query.prepare("SELECT " DB_F_TAGS_NAME " FROM " DB_T_TAGS ";");
    bool rv = query.exec();
    qCHECK_REF(rv, query);

    for ( ; query.next(); ) {
        comboBox->addItem( query.value(0).toString() );
    }

    // comboBox->addItem("Add tag");

    return comboBox;
}
//-------------------------------------------------------------------------------------------------
void
ComboBoxItemDelegate::setEditorData(
    QWidget      *a_editor,
    cQModelIndex &a_index
) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(a_editor);
    if (comboBox != Q_NULLPTR) {
        cQString currentText = a_index.data(Qt::EditRole).toString();

        cint currentIndex = comboBox->findText(currentText);
        if (currentIndex >= 0) {
            comboBox->setCurrentIndex(currentIndex);
        }
    } else {
        QStyledItemDelegate::setEditorData(a_editor, a_index);
    }
}
//-------------------------------------------------------------------------------------------------
void
ComboBoxItemDelegate::setModelData(
    QWidget            *a_editor,
    QAbstractItemModel *a_model,
    cQModelIndex       &a_index
) const
{
    QComboBox *comboBox = static_cast<QComboBox *>( a_editor );
    if (comboBox != Q_NULLPTR) {
        a_model->setData(a_index, comboBox->currentText(), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(a_editor, a_model, a_index);
    }
}
//-------------------------------------------------------------------------------------------------
void
ComboBoxItemDelegate::updateEditorGeometry(
    QWidget                    *a_editor,
    const QStyleOptionViewItem &a_option,
    cQModelIndex               &a_index
) const
{
    Q_UNUSED(a_index);

    QComboBox *cb = static_cast<QComboBox *>(a_editor);

    cQSize size = cb->sizeHint();

    a_editor->setMinimumHeight(size.height());
    a_editor->setMinimumWidth(size.width());
    a_editor->setGeometry(a_option.rect);
}
//-------------------------------------------------------------------------------------------------
void
ComboBoxItemDelegate::paint(
    QPainter                   *a_painter,
    const QStyleOptionViewItem &a_option,
    cQModelIndex               &a_index
) const
{
    cQString value = a_index.data().toString();

    QStyleOptionViewItemV4 myOption = a_option;
    myOption.text = value;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, a_painter);
}
//-------------------------------------------------------------------------------------------------
