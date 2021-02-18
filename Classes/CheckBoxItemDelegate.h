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
    explicit         CheckBoxItemDelegate(QObject * parent = nullptr);
    virtual         ~CheckBoxItemDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        cQModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void     setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void     setModelData(QWidget *editor, QAbstractItemModel *model,
                        cQModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                        cQModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void     paint(QPainter *painter, const QStyleOptionViewItem &option,
                        cQModelIndex &index) const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void             toggled();

private:
    Q_OBJECT
    Q_DISABLE_COPY(CheckBoxItemDelegate)
};
//-------------------------------------------------------------------------------------------------
