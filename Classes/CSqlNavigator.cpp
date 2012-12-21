/**
 * \file   CSqlNavigator.cpp
 * \brief
 */


#include "CSqlNavigator.h"

#include "../QtLib/CUtils.h"


/****************************************************************************
*   public
*
*****************************************************************************/

//---------------------------------------------------------------------------
CSqlNavigator::CSqlNavigator(
    QWidget *a_parent
) :
    QObject   (a_parent),
    _m_tmModel(NULL),
    _m_tvView (NULL)
{

}
//---------------------------------------------------------------------------
/*virtual*/
CSqlNavigator::~CSqlNavigator() {

}
//---------------------------------------------------------------------------
void
CSqlNavigator::construct(
    QSqlTableModel *a_tmTableModel,
    QTableView     *a_tabvTableView
)
{
    Q_ASSERT(NULL != a_tmTableModel);
    Q_ASSERT(NULL != a_tabvTableView);

    _m_tmModel = a_tmTableModel;
    _m_tvView  = a_tabvTableView;
}
//---------------------------------------------------------------------------
QSqlTableModel *
CSqlNavigator::model() {
    Q_ASSERT(NULL != _m_tmModel);

    return _m_tmModel;
}
//---------------------------------------------------------------------------
QTableView *
CSqlNavigator::view() {
    Q_ASSERT(NULL != _m_tvView);

    return _m_tvView;
}
//---------------------------------------------------------------------------
bool
CSqlNavigator::isValid() const {
    return ( (NULL != _m_tmModel) && (NULL != _m_tvView) );
}
//---------------------------------------------------------------------------
void
CSqlNavigator::first() {
    qCHECK_DO(false == isValid(), return);

    int iTargetRow = 0;

    view()->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::prior() {
    qCHECK_DO(false == isValid(), return);

    int iTargetRow = view()->currentIndex().row() - 1;

    view()->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::next() {
    qCHECK_DO(false == isValid(), return);

    int iTargetRow = view()->currentIndex().row() + 1;

    view()->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::last() {
    qCHECK_DO(false == isValid(), return);

    int iTargetRow = CUtils::sqlTableModelRowCount( model() ) - 1;
    qCHECK_DO(- 1 >= iTargetRow, iTargetRow = 0);

    view()->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::insert() {
    qCHECK_DO(false == isValid(), return);

    bool bRv = model()->insertRow(CUtils::sqlTableModelRowCount(model()), QModelIndex());
    qCHECK_PTR(bRv, model());

    last();
}
//---------------------------------------------------------------------------
void
CSqlNavigator::remove() {
    qCHECK_DO(false == isValid(), return);

    QModelIndexList ilList = view()->selectionModel()->selectedIndexes();
    foreach (QModelIndex index, ilList) {
        int iTargetRow = index.row();

        bool bRv = view()->model()->removeRow(iTargetRow, QModelIndex());
        qCHECK_PTR(bRv, model());
    }
}
//---------------------------------------------------------------------------
void
CSqlNavigator::edit() {
    qCHECK_DO(false == isValid(), return);

    const int   ciTargetCell = 1;

    int         iTargetRow   = view()->currentIndex().row();
    QModelIndex miIndex      = model()->index(iTargetRow, ciTargetCell);
    qCHECK_DO(- 1 == iTargetRow, return);

    view()->setCurrentIndex(miIndex);
    view()->edit(miIndex);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::post() {
    qCHECK_DO(false == isValid(), return);

    const int   ciTargetCell = 1;

    int         iTargetRow   = view()->currentIndex().row();
    QModelIndex miIndex      = model()->index(iTargetRow, ciTargetCell);

    model()->submitAll();

    view()->setCurrentIndex(miIndex);
    view()->update(miIndex);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::cancel() {
    qCHECK_DO(false == isValid(), return);

}
//---------------------------------------------------------------------------
void
CSqlNavigator::refresh() {
    qCHECK_DO(false == isValid(), return);

    int iTargetRow = view()->currentIndex().row();
    qCHECK_DO(- 1 == iTargetRow, return);

    bool bRv = model()->select();
    qCHECK_PTR(bRv, model());

    view()->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
