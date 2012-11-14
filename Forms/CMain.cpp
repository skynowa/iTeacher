/**
 * \file   CMain.cpp
 * \brief  main widget
 */


#include "CMain.h"

#include "../Classes/CUtils.h"
#include "../Classes/CCheckBoxItemDelegate.h"

#include <xLib/Common/CxString.h>


/****************************************************************************
*   public
*
*****************************************************************************/

//---------------------------------------------------------------------------
CMain::CMain(
    QWidget    *parent,
    Qt::WFlags  flags
) :
    QMainWindow        (parent, flags),
    m_navNavigator     (this),
    _m_dbDatabase      (),
    _m_tmModel         (NULL),
    actFile_Exit       (this),
    actEdit_MovetoFirst(this),
    actEdit_MovetoPrior(this),
    actEdit_MovetoNext (this),
    actEdit_MovetoLast (this),
    actEdit_Insert     (this),
    actEdit_Delete     (this),
    actEdit_Edit       (this),
    actEdit_Post       (this),
    actEdit_Cancel     (this),
    actEdit_Refresh    (this),
    actFind_Search     (this),
    actOptions_Settings(this),
    actHelp_Faq        (this),
    actHelp_About      (this)
{
    _construct();
}
//---------------------------------------------------------------------------
/*virtual*/
CMain::~CMain() {
    _destruct();
}
//---------------------------------------------------------------------------


/****************************************************************************
*   private
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::_construct() {
    _initMain();
    _initModel();
    _initActions();
    _initMenus();
}
//---------------------------------------------------------------------------
void
CMain::_destruct() {
    xPTR_DELETE(_m_tmModel);
}
//---------------------------------------------------------------------------
void
CMain::_initMain() {
    m_Ui.setupUi(this);

    // CMain
    {
        setWindowIcon(QIcon(CONFIG_RES_MAIN_ICON));
        setWindowTitle(CONFIG_APP_NAME);
        setGeometry(0, 0, CONFIG_APP_WIDTH, CONFIG_APP_HEIGHT);
        CUtils::widgetAlignCenter(this);
    }
}
//---------------------------------------------------------------------------
void
CMain::_initModel() {
    //--------------------------------------------------
    // settings DB
    {
        bool bRv = false;

        bRv = QSqlDatabase::isDriverAvailable("QSQLITE");
        qCHECK_DO(false == bRv, qMSG(QSqlDatabase().lastError().text()); return;);

        _m_dbDatabase = QSqlDatabase::addDatabase("QSQLITE");
        _m_dbDatabase.setDatabaseName(QCoreApplication::applicationFilePath() + CONFIG_DB_FILE_EXT);

        bRv = _m_dbDatabase.open();
        qCHECK_REF(bRv, _m_dbDatabase);

        {
            QSqlQuery qryInfo(_m_dbDatabase);

            const QString csSql = \
                    "CREATE TABLE IF NOT EXISTS "
                    "    " CONFIG_DB_T_MAIN
                    "( "
                    "    " CONFIG_DB_F_MAIN_ID         " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                    "    " CONFIG_DB_F_MAIN_TERM       " varchar(255) UNIQUE NOT NULL, "
                    "    " CONFIG_DB_F_MAIN_VALUE      " varchar(255), "
                    "    " CONFIG_DB_F_MAIN_IS_LEARNED " integer NOT NULL DEFAULT 0, "
                    "    " CONFIG_DB_F_MAIN_IS_MARKED  " integer NOT NULL DEFAULT 0 "
                    ")";

            bRv = qryInfo.exec(csSql);
            qCHECK_REF(bRv, qryInfo);
        }
    }

    //--------------------------------------------------
    // _m_tmModel, m_Ui.tabvInfo
    {
        _m_tmModel = new QSqlTableModel(this, _m_dbDatabase);
        Q_ASSERT(NULL != _m_tmModel);

        _m_tmModel->setTable(CONFIG_DB_T_MAIN);
        _m_tmModel->setHeaderData(0, Qt::Horizontal, tr("Id"),      Qt::EditRole);
        _m_tmModel->setHeaderData(1, Qt::Horizontal, tr("Term"),    Qt::EditRole);
        _m_tmModel->setHeaderData(2, Qt::Horizontal, tr("Value"),   Qt::EditRole);
        _m_tmModel->setHeaderData(3, Qt::Horizontal, tr("Learned"), Qt::EditRole);
        _m_tmModel->setHeaderData(4, Qt::Horizontal, tr("Marked"),  Qt::EditRole);
        _m_tmModel->setEditStrategy(QSqlTableModel::OnFieldChange);
        _m_tmModel->select();

        m_Ui.tabvInfo->setModel(_m_tmModel);
        m_Ui.tabvInfo->hideColumn(0); // don't show the CONFIG_DB_F_MAIN_ID
        m_Ui.tabvInfo->verticalHeader()->setVisible(true);
        m_Ui.tabvInfo->verticalHeader()->setDefaultSectionSize(CONFIG_TABLEVIEW_ROW_HEIGHT);
        m_Ui.tabvInfo->setEditTriggers(QAbstractItemView::SelectedClicked);
        m_Ui.tabvInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_Ui.tabvInfo->setSelectionMode(QAbstractItemView::SingleSelection);
        m_Ui.tabvInfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_Ui.tabvInfo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_Ui.tabvInfo->setAlternatingRowColors(true);
        m_Ui.tabvInfo->setItemDelegateForColumn(3, new CCheckBoxItemDelegate(m_Ui.tabvInfo));
        m_Ui.tabvInfo->setItemDelegateForColumn(4, new CCheckBoxItemDelegate(m_Ui.tabvInfo));
        m_Ui.tabvInfo->setSortingEnabled(true);

        m_Ui.tabvInfo->show();
    }

    //--------------------------------------------------
    // slots
    {
        connect(m_Ui.tabvInfo->selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                this,                            SLOT  ( slot_tabvInfo_OnSelectionChanged(const QItemSelection &, const QItemSelection &) ));

        connect(m_Ui.tabvInfo,                   SIGNAL( doubleClicked(const QModelIndex &) ),
                this,                            SLOT  ( slot_tabvInfo_OnDoubleClicked(const QModelIndex &) ));
    }

    //--------------------------------------------------
    // m_navNavigator
    {
        m_navNavigator.construct(_m_tmModel, m_Ui.tabvInfo);

        // go to the last record
        m_navNavigator.last();
        slot_tabvInfo_OnSelectionChanged(QItemSelection(), QItemSelection());
    }
}
//---------------------------------------------------------------------------
void
CMain::_initActions() {
    // group "File"
    {
        actFile_Exit.setText(tr("Exit"));
        connect(&actFile_Exit, SIGNAL( triggered() ),
                this,          SLOT  ( slot_OnExit() ));
        m_Ui.toolBar->addAction(&actFile_Exit);
    }

    // group "Edit"
    {
        actEdit_MovetoFirst.setText(tr("First"));
        connect(&actEdit_MovetoFirst, SIGNAL( triggered() ),
                this,                 SLOT  ( slot_OnFirst() ));
        m_Ui.toolBar->addAction(&actEdit_MovetoFirst);

        actEdit_MovetoPrior.setText(tr("Prior"));
        connect(&actEdit_MovetoPrior, SIGNAL( triggered() ),
                this,                 SLOT  ( slot_OnPrior() ));
        m_Ui.toolBar->addAction(&actEdit_MovetoPrior);

        actEdit_MovetoNext.setText(tr("Next"));
        connect(&actEdit_MovetoNext, SIGNAL( triggered() ),
                this,                SLOT  ( slot_OnNext() ));
        m_Ui.toolBar->addAction(&actEdit_MovetoNext);

        actEdit_MovetoLast.setText(tr("Last"));
        connect(&actEdit_MovetoLast, SIGNAL( triggered() ),
                this,                SLOT  ( slot_OnLast() ));
        m_Ui.toolBar->addAction(&actEdit_MovetoLast);

        actEdit_Insert.setText(tr("Insert"));
        connect(&actEdit_Insert, SIGNAL( triggered() ),
                this,            SLOT  ( slot_OnInsert() ));
        m_Ui.toolBar->addAction(&actEdit_Insert);

        actEdit_Delete.setText(tr("Delete"));
        connect(&actEdit_Delete, SIGNAL( triggered() ),
                this,            SLOT  ( slot_OnRemove() ));
        m_Ui.toolBar->addAction(&actEdit_Delete);

        actEdit_Edit.setText(tr("Edit"));
        connect(&actEdit_Edit, SIGNAL( triggered() ),
                this,          SLOT  ( slot_OnEdit() ));
        m_Ui.toolBar->addAction(&actEdit_Edit);

        actEdit_Post.setText(tr("Post"));
        connect(&actEdit_Post, SIGNAL( triggered() ),
                this,          SLOT  ( slot_OnPost() ));
        m_Ui.toolBar->addAction(&actEdit_Post);

        actEdit_Cancel.setText(tr("Cancel"));
        connect(&actEdit_Cancel, SIGNAL( triggered() ),
                this,            SLOT  ( slot_OnCancel() ));
        m_Ui.toolBar->addAction(&actEdit_Cancel);

        actEdit_Refresh.setText(tr("Refresh"));
        connect(&actEdit_Refresh, SIGNAL( triggered() ),
                this,             SLOT  ( slot_OnRefresh() ));
        m_Ui.toolBar->addAction(&actEdit_Refresh);
    }

    // group "Find"
    {
        actFind_Search.setText(tr("Search"));
        connect(&actFind_Search, SIGNAL( triggered() ),
                this,            SLOT  ( slot_OnSearch() ));
        m_Ui.toolBar->addAction(&actFind_Search);
    }

    // group "View"
    {

    }

    // group "Options"
    {
        actOptions_Settings.setText(tr("Settings"));
        connect(&actOptions_Settings, SIGNAL( triggered() ),
                this,                 SLOT  ( slot_OnSettings() ));
        m_Ui.toolBar->addAction(&actOptions_Settings);
    }

    // group "Help"
    {
        actHelp_Faq.setText(tr("FAQ"));
        connect(&actHelp_Faq, SIGNAL( triggered() ),
                this,         SLOT  ( slot_OnFaq() ));
        m_Ui.toolBar->addAction(&actHelp_Faq);

        actHelp_About.setText(tr("About"));
        connect(&actHelp_About, SIGNAL( triggered() ),
                this,           SLOT  ( slot_OnAbout() ));
        m_Ui.toolBar->addAction(&actHelp_About);
    }
}
//---------------------------------------------------------------------------
void
CMain::_initMenus() {
    // group "File"
    {
        mnuFile.setTitle(tr("File"));

        mnuFile.addAction(&actFile_Exit);

        menuBar()->addMenu(&mnuFile);
    }

    // group "Edit"
    {
        mnuEdit.setTitle(tr("Edit"));

        mnuEdit.addAction(&actEdit_MovetoFirst);
        mnuEdit.addAction(&actEdit_MovetoPrior);
        mnuEdit.addAction(&actEdit_MovetoNext);
        mnuEdit.addAction(&actEdit_MovetoLast);
        mnuEdit.addAction(&actEdit_Insert);
        mnuEdit.addAction(&actEdit_Delete);
        mnuEdit.addAction(&actEdit_Edit);
        mnuEdit.addAction(&actEdit_Post);
        mnuEdit.addAction(&actEdit_Cancel);
        mnuEdit.addAction(&actEdit_Refresh);

        menuBar()->addMenu(&mnuEdit);
    }

    // group "Find"
    {
        mnuFind.setTitle(tr("Find"));

        mnuFind.addAction(&actFind_Search);

        menuBar()->addMenu(&mnuFind);
    }

    // group "View"
    {
        mnuView.setTitle(tr("View"));

        menuBar()->addMenu(&mnuView);
    }

    // group "Options"
    {
        mnuOptions.setTitle(tr("Options"));

        mnuOptions.addAction(&actOptions_Settings);

        menuBar()->addMenu(&mnuOptions);
    }

    // group "Help"
    {
        mnuHelp.setTitle(tr("Help"));

        mnuHelp.addAction(&actHelp_Faq);
        mnuHelp.addAction(&actHelp_About);

        menuBar()->addMenu(&mnuHelp);
    }
}
//---------------------------------------------------------------------------


/****************************************************************************
*   group "File"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnExit() {
    close();
}
//---------------------------------------------------------------------------


/****************************************************************************
*   group "Edit"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnFirst() {
    m_navNavigator.first();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnPrior() {
    m_navNavigator.prior();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnNext() {
    m_navNavigator.next();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnLast() {
    m_navNavigator.last();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnInsert() {
    m_navNavigator.insert();

//    const int   ciCurrentRow = _m_tmModel->rowCount() - 1;
//    CPersonEdit dlgPersonEdit(this, _m_tmModel, ciCurrentRow);

//    dlgPersonEdit.exec();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnRemove() {
    m_navNavigator.remove();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnEdit() {
    m_navNavigator.edit();
}
//---------------------------------------------------------------------------
void
CMain::slot_tabvInfo_OnSelectionChanged(
    const QItemSelection &selected,
    const QItemSelection &deselected
)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}
//---------------------------------------------------------------------------
void
CMain::slot_tabvInfo_OnDoubleClicked(
    const QModelIndex &index
)
{

}
//---------------------------------------------------------------------------
void
CMain::slot_OnPost() {
    m_navNavigator.post();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnCancel() {
    m_navNavigator.cancel();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnRefresh() {
    m_navNavigator.refresh();
}
//---------------------------------------------------------------------------


/****************************************************************************
*   group "Find"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnSearch() {

}
//---------------------------------------------------------------------------


/****************************************************************************
*   group "View"
*
*****************************************************************************/

//---------------------------------------------------------------------------
/*
void
CMain::xxxxxxx() {

}
*/
//---------------------------------------------------------------------------


/****************************************************************************
*   group "Options"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnSettings() {

}
//---------------------------------------------------------------------------


/****************************************************************************
*   group "Help"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnFaq() {

}
//---------------------------------------------------------------------------
void
CMain::slot_OnAbout() {
    QString sMsg = QString(tr(
        "<p>"
        "<b>%1</b> - " CONFIG_APP_COMMENT
        "</p>")
            .arg(CONFIG_APP_NAME)
        );

    QMessageBox::about(this, tr("About ") + CONFIG_APP_NAME, sMsg);
}
//---------------------------------------------------------------------------
