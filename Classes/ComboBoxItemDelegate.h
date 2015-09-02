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
    Q_OBJECT
    Q_DISABLE_COPY(ComboBoxItemDelegate)

public:
    explicit ComboBoxItemDelegate(QObject * parent, QSqlTableModel *sqlModel);
    virtual ~ComboBoxItemDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const;
    void     setModelData(QWidget *editor, QAbstractItemModel *model,
                const QModelIndex &index) const;
    void     updateEditorGeometry(QWidget *editor,
                const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void     paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

private:
    QSqlTableModel *_sqlModel;

private Q_SLOTS:

};
//-------------------------------------------------------------------------------------------------
