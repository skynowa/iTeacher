/**
 * \file   ComboBoxItemDelegate.h
 * \brief
 */


#pragma once

#include <QStyledItemDelegate>
#include "../QtLib/Common.h"
#include "../Config.h"
//-------------------------------------------------------------------------------------------------
class ComboBoxItemDelegate :
    public QStyledItemDelegate
{
public:
    explicit         ComboBoxItemDelegate(QObject * parent, QSqlTableModel *sqlModel);
    virtual         ~ComboBoxItemDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void     setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void     setModelData(QWidget *editor, QAbstractItemModel *model,
                        const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void     updateEditorGeometry(QWidget *editor,
                        const QStyleOptionViewItem &option, const QModelIndex &index) const
                        Q_DECL_OVERRIDE;
    virtual void     paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QSqlTableModel  *_sqlModel;

    Q_OBJECT
    Q_DISABLE_COPY(ComboBoxItemDelegate)
};
//-------------------------------------------------------------------------------------------------
