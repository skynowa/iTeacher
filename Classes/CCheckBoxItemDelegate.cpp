#include "CCheckBoxItemDelegate.h"

#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>


//---------------------------------------------------------------------------
CCheckBoxItemDelegate::CCheckBoxItemDelegate(
    QObject *parent
) :
    QAbstractItemDelegate(parent),
    _m_objParent         (static_cast<QTableView *>( parent ))
{
    Q_ASSERT(NULL != _m_objParent);
}
//---------------------------------------------------------------------------
CCheckBoxItemDelegate::~CCheckBoxItemDelegate()
{

}
//---------------------------------------------------------------------------
QSize
CCheckBoxItemDelegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex          &index
) const
{
    Q_UNUSED(index);

    return option.rect.size();
}
//---------------------------------------------------------------------------
/*!
  Возвращает область занимаемую индикатором.
 */
QRect
CCheckBoxItemDelegate::checkRect(
    const QStyleOptionViewItem &option
) const
{
    QStyleOptionButton styleOptionButton;

    QRect rect = QApplication::style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &styleOptionButton);
    rect.moveCenter(option.rect.center());

    return rect;
}
//---------------------------------------------------------------------------
void
CCheckBoxItemDelegate::paint(
    QPainter                   *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex          &index
) const
{
    QStyle::State state = index.data(Qt::UserRole).value<CStateData>();
    if (QStyle::State_None == state) {
        return;
    }

    index.data(Qt::UserRole + 1).value<CStateData>() & QStyle::State_MouseOver ?
               state |=  QStyle::State_MouseOver :
               state &=~ QStyle::State_MouseOver;

    QStyleOptionButton styleOptionButton;
    styleOptionButton.rect  = checkRect(option);
    styleOptionButton.state = state;

    QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &styleOptionButton, painter);
}
//---------------------------------------------------------------------------
/*!
    Всякий раз, когда происходит событие, эта функция вызывается с вариантом модели, событий и индекс,
    который соответствует пункту время редактируется.
 */
bool
CCheckBoxItemDelegate::editorEvent(
    QEvent                     *event,
    QAbstractItemModel         *model,
    const QStyleOptionViewItem &option,
    const QModelIndex          &index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    Qt::ItemFlags flags = model->flags(index);
    if (!(option.state & QStyle::State_Enabled) || !(flags & Qt::ItemIsEnabled)) {
        return false;
    }

    state = index.data(Qt::UserRole).value<CStateData>();
    if (state.testFlag(QStyle::State_NoChange) || state == QStyle::State_None) {
        return false;
    }

    switch(event->type()) {
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick: {
            QRect cr(checkRect(option));

            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            if (event->type() == QEvent::MouseButtonDblClick  || !cr.contains(me->pos())) {
                return false;
            }

            if (me->button() != Qt::LeftButton) {
                return false;
            }

            QVariant variant;
            state ^= (QStyle::State_On | QStyle::State_Off);
            variant.setValue(state);
            model->setData(index, variant, Qt::UserRole);

            emit checkChanged(state.testFlag(QStyle::State_On), index);

            return true;
        }

        default: {
            ;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
