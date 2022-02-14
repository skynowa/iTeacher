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
    explicit  ComboBoxItemDelegate(QObject *parent, QSqlTableModel *sqlModel);
    virtual  ~ComboBoxItemDelegate() = default;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, cQModelIndex &index)
                const override;
    void     setEditorData(QWidget *editor, cQModelIndex &index) const override;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, cQModelIndex &index)
                const override;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                cQModelIndex &index) const override;
    void     paint(QPainter *painter, const QStyleOptionViewItem &option, cQModelIndex &index)
                const override;

private:
    QSqlTableModel *_sqlModel {};

    Q_OBJECT
    Q_DISABLE_COPY(ComboBoxItemDelegate)
};
//-------------------------------------------------------------------------------------------------
