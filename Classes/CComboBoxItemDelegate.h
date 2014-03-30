/**
 * \file   CComboBoxItemDelegate.h
 * \brief
 */


#ifndef iTeacher_CComboBoxItemDelegate_H
#define iTeacher_CComboBoxItemDelegate_H
//-------------------------------------------------------------------------------------------------
#include <QStyledItemDelegate>
#include "../QtLib/Common.h"
#include "../Config.h"
//-------------------------------------------------------------------------------------------------
class CComboBoxItemDelegate :
    public QStyledItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(CComboBoxItemDelegate)

public:
    explicit CComboBoxItemDelegate(QObject * parent, QSqlTableModel *sqlModel);
    virtual ~CComboBoxItemDelegate();

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
#endif // iTeacher_CComboBoxItemDelegate_H
