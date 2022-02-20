/**
 * \file   CheckBoxItemDelegate.h
 * \brief
 */


#pragma once

#include <QStyledItemDelegate>
#include "../QtLib/Common.h"
//-------------------------------------------------------------------------------------------------
class CheckBoxItemDelegate :
    public QStyledItemDelegate
{
public:
///@name ctors, dtor
///@{
    explicit  CheckBoxItemDelegate(QObject * parent);
    virtual  ~CheckBoxItemDelegate() = default;

    Q_DISABLE_COPY(CheckBoxItemDelegate);
///@}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, cQModelIndex &index)
                const final;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, cQModelIndex &index)
                const final;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                cQModelIndex &index) const final;
    void     paint(QPainter *painter, const QStyleOptionViewItem &option, cQModelIndex &index)
                const final;

private Q_SLOTS:
    void toggled();

private:
    Q_OBJECT
};
//-------------------------------------------------------------------------------------------------
