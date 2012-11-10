/**
 * \file   CSqlNavigator.cpp
 * \brief
 */


#include "CSqlNavigator.h"


/****************************************************************************
*   public
*
*****************************************************************************/

//---------------------------------------------------------------------------
CSqlNavigator::CSqlNavigator(
    QWidget *parent
) :
    QObject         (parent),
    _m_tmModel (NULL),
    _m_tvView(NULL)
{

}
//---------------------------------------------------------------------------
/*virtual*/
CSqlNavigator::~CSqlNavigator() {

}
//---------------------------------------------------------------------------
void
CSqlNavigator::setup(
    QSqlTableModel *tmTableModel,
    QTableView     *tabvTableView
)
{
    Q_ASSERT(NULL != tmTableModel);
    Q_ASSERT(NULL != tabvTableView);

    _m_tmModel  = tmTableModel;
    _m_tvView = tabvTableView;
}
//---------------------------------------------------------------------------
void
CSqlNavigator::first() {
    int iTargetRow = 0;

    _m_tvView->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::prior() {
    int iTargetRow = _m_tvView->currentIndex().row() - 1;

    _m_tvView->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::next() {
    int iTargetRow = _m_tvView->currentIndex().row() + 1;

    _m_tvView->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::last() {
    int iTargetRow = _m_tmModel->rowCount() - 1;

    _m_tvView->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::insert() {
    bool bRes = _m_tmModel->insertRow(_m_tmModel->rowCount());
    qCHECK_PTR(bRes, _m_tmModel);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::remove() {
    int iTargetRow = _m_tvView->currentIndex().row();
    qCHECK_DO(- 1 == iTargetRow, return);

    bool bRes = _m_tvView->model()->removeRow(iTargetRow);
    qCHECK_PTR(bRes, _m_tmModel);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::edit() {
    const int   ciTargetCell = 1;

    int         iTargetRow   = _m_tvView->currentIndex().row();
    QModelIndex miIndex      = _m_tmModel->index(iTargetRow, ciTargetCell);
    qCHECK_DO(- 1 == iTargetRow, return);

    _m_tvView->setCurrentIndex(miIndex);
    _m_tvView->edit(miIndex);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::post() {
    const int   ciTargetCell = 1;

    int         iTargetRow   = _m_tvView->currentIndex().row();
    QModelIndex miIndex      = _m_tmModel->index(iTargetRow, ciTargetCell);

_m_tmModel->submitAll();

    _m_tvView->setCurrentIndex(miIndex);
    _m_tvView->update(miIndex);
}
//---------------------------------------------------------------------------
void
CSqlNavigator::cancel() {

}
//---------------------------------------------------------------------------
void
CSqlNavigator::refresh() {
    bool bRes = _m_tmModel->select();
    qCHECK_PTR(bRes, _m_tmModel);

    int iTargetRow = _m_tvView->currentIndex().row();
    qCHECK_DO(- 1 == iTargetRow, return);

    _m_tvView->selectRow(iTargetRow);
}
//---------------------------------------------------------------------------



