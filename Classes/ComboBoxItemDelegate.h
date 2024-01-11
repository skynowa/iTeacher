/**
 * \file   ComboBoxItemDelegate.h
 * \brief
 */


#pragma once

#include <QStyledItemDelegate>
#include "../QtLib/Common.h"
#include "Config.h"
//-------------------------------------------------------------------------------------------------
class ComboBoxItemDelegate :
    public QStyledItemDelegate
{
public:
///@name ctors, dtor
///@{
    explicit  ComboBoxItemDelegate(QObject *parent, QSqlTableModel *sqlModel);
    virtual  ~ComboBoxItemDelegate() = default;

    Q_DISABLE_COPY(ComboBoxItemDelegate)
///@}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, cQModelIndex &index)
                const final;
    void     setEditorData(QWidget *editor, cQModelIndex &index) const final;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, cQModelIndex &index)
                const final;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                cQModelIndex &index) const final;
    void     paint(QPainter *painter, const QStyleOptionViewItem &option, cQModelIndex &index)
                const final;

private:
    Q_OBJECT

    QSqlTableModel *_sqlModel {};
};
//-------------------------------------------------------------------------------------------------
