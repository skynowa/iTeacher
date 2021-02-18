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
    explicit  CheckBoxItemDelegate(QObject * parent = nullptr);
    virtual  ~CheckBoxItemDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, cQModelIndex &index)
                const override;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, cQModelIndex &index)
                const override;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                cQModelIndex &index) const override;
    void     paint(QPainter *painter, const QStyleOptionViewItem &option, cQModelIndex &index)
                const override;

private Q_SLOTS:
    void toggled();

private:
    Q_OBJECT
    Q_DISABLE_COPY(CheckBoxItemDelegate)
};
//-------------------------------------------------------------------------------------------------
