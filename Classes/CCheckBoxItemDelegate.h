#ifndef CCheckBoxItemDelegate_H
#define CCheckBoxItemDelegate_H
//---------------------------------------------------------------------------
#include <QAbstractItemDelegate>
#include <QTableView>
#include "CStateData.h"
//---------------------------------------------------------------------------
/*!
    \class CCheckBoxItemDelegate qivbcheckindicatordelegate.h "qivbcheckindicatordelegate.h"
    \brief Класс-делегат имитирующий поведение виджета QCheckBox.
 */
class CCheckBoxItemDelegate :
    public QAbstractItemDelegate
{
        Q_OBJECT
        Q_DISABLE_COPY(CCheckBoxItemDelegate)

    public:
        explicit      CCheckBoxItemDelegate(QObject * parent = 0);
        virtual      ~CCheckBoxItemDelegate();

        virtual void  paint       (QPainter * painter, const QStyleOptionViewItem & option,
                                   const QModelIndex & index) const;
        virtual QSize sizeHint    (const QStyleOptionViewItem & option,
                                   const QModelIndex & index) const;
        virtual bool  editorEvent (QEvent * event, QAbstractItemModel * model,
                                   const QStyleOptionViewItem & option, const QModelIndex & index);
    signals:
        void          checkChanged(bool state, const QModelIndex &);

    private:
        QTableView   *_m_objParent;
        CStateData    state;

        QRect         checkRect   (const QStyleOptionViewItem & option) const;
};
//---------------------------------------------------------------------------
#endif // CCheckBoxItemDelegate_H
