/**
 * \file   CCheckBoxItemDelegate.h
 * \brief
 */


#ifndef iTeacher_CCheckBoxItemDelegate_H
#define iTeacher_CCheckBoxItemDelegate_H
//-------------------------------------------------------------------------------------------------
#include <QStyledItemDelegate>
#include "../QtLib/Common.h"
//-------------------------------------------------------------------------------------------------
class CCheckBoxItemDelegate :
    public QStyledItemDelegate
{
Q_OBJECT
Q_DISABLE_COPY(CCheckBoxItemDelegate)

public:
    explicit CCheckBoxItemDelegate(QObject * parent = NULL);
    virtual ~CCheckBoxItemDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const;
    void     setModelData(QWidget *editor, QAbstractItemModel *model,
                const QModelIndex &index) const;
    void     updateEditorGeometry(QWidget *editor,
                const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void     paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

private Q_SLOTS:
    void     slot_OnToggled();
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_CCheckBoxItemDelegate_H
