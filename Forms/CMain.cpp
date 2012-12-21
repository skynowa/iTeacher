/**
 * \file   CMain.cpp
 * \brief  main widget
 */


#include "CMain.h"

#include "../QtLib/CUtils.h"
#include "../Classes/CCheckBoxItemDelegate.h"
#include "../Forms/CWordEditor.h"
#include "../Forms/CWordFinder.h"

#include <xLib/Common/CxConst.h>
#include <xLib/Common/CxString.h>
#include <xLib/Filesystem/CxPath.h>
#include <xLib/Filesystem/CxDir.h>


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
    m_sAppName         (),
    m_sAppDir          (),
    m_sDbDir           (),
    m_sDbBackupDir     (),
    m_navNavigator     (this),
    actFile_CreateDb   (this),
    actFile_ImportCsv  (this),
    actFile_ExportCsv  (this),
    actFile_ExportPdf  (this),
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
    actView_ZoomIn     (this),
    actView_ZoomOut    (this),
    actView_ZoomDefault(this),
    actFind_Search     (this),
    actOptions_Settings(this),
    actHelp_Faq        (this),
    actHelp_About      (this),
    _m_dbDatabase      (NULL),
    _m_tmModel         (NULL)
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
*   protected
*
*****************************************************************************/

//---------------------------------------------------------------------------
/* virtual */
bool
CMain::eventFilter(
    QObject *obj,
    QEvent  *ev
)
{
    // table zooming
    if (m_Ui.tabvInfo->viewport() == obj) {
        if (QEvent::Wheel == ev->type()) {
            QWheelEvent *inputEvent = static_cast<QWheelEvent *>( ev );
            if (inputEvent->modifiers() & Qt::ControlModifier) {
                if (inputEvent->delta() > 0) {
                    slot_OnZoomIn();
                } else {
                    slot_OnZoomOut();
                }
            }
        }
    }

    return false;
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
    // _m_dbDatabase disconnect
    dbClose();
}
//---------------------------------------------------------------------------
void
CMain::_initMain() {
    m_Ui.setupUi(this);

    //--------------------------------------------------
    // data
    {
        m_sAppName     = CONFIG_APP_NAME;
        m_sAppDir      = qS2QS( CxPath::sExeDir() );
        m_sDbDir       = m_sAppDir + QDir::separator() + "Db";
        m_sDbBackupDir = m_sDbDir  + QDir::separator() + "Backup";

        CxDir( qQS2S(m_sDbDir) ).vPathCreate();
    }

    //--------------------------------------------------
    // CMain
    {
        setWindowIcon(QIcon(CONFIG_RES_MAIN_ICON));
        setWindowTitle(CONFIG_APP_NAME);
        setGeometry(0, 0, CONFIG_APP_WIDTH, CONFIG_APP_HEIGHT);
        CUtils::widgetAlignCenter(this);
        cboDictionaryPath_reload();
    }

    m_Ui.tabvInfo->viewport()->installEventFilter(this);
}
//---------------------------------------------------------------------------
void
CMain::_initModel() {
    //--------------------------------------------------
    // open DB
    {
        if (false == m_Ui.cboDictionaryPath->currentText().isEmpty()) {
            QString sDictPath = m_sDbDir + QDir::separator() + m_Ui.cboDictionaryPath->currentText();

            dbOpen(sDictPath);
        }
    }

    //--------------------------------------------------
    // _m_tmModel, tabvInfo
    {
        // tabvInfo
        {
            m_Ui.tabvInfo->setModel(_m_tmModel);

            m_Ui.tabvInfo->hideColumn(0); // don't show the CONFIG_DB_F_MAIN_ID
            m_Ui.tabvInfo->setColumnWidth(0, 40);
            m_Ui.tabvInfo->setColumnWidth(1, 100);
            m_Ui.tabvInfo->setColumnWidth(2, 400);
            m_Ui.tabvInfo->setColumnWidth(3, 60);
            m_Ui.tabvInfo->setColumnWidth(4, 60);

            m_Ui.tabvInfo->verticalHeader()->setVisible(true);
            m_Ui.tabvInfo->verticalHeader()->setDefaultSectionSize(CONFIG_TABLEVIEW_ROW_HEIGHT);

            m_Ui.tabvInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
            m_Ui.tabvInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
            m_Ui.tabvInfo->setSelectionMode(QAbstractItemView::ExtendedSelection);
            m_Ui.tabvInfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            m_Ui.tabvInfo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            m_Ui.tabvInfo->setAlternatingRowColors(true);
            m_Ui.tabvInfo->setStyleSheet("alternate-background-color: white; background-color: gray;");
            m_Ui.tabvInfo->setSortingEnabled(true);
            m_Ui.tabvInfo->sortByColumn(0, Qt::AscendingOrder);
            m_Ui.tabvInfo->setItemDelegateForColumn(3, new CCheckBoxItemDelegate(m_Ui.tabvInfo));
            m_Ui.tabvInfo->setItemDelegateForColumn(4, new CCheckBoxItemDelegate(m_Ui.tabvInfo));

            m_Ui.tabvInfo->show();
        }
    }

    //--------------------------------------------------
    // slots
    {
        connect(m_Ui.tabvInfo->selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                this,                            SLOT  ( slot_tabvInfo_OnSelectionChanged(const QItemSelection &, const QItemSelection &) ));

        connect(m_Ui.tabvInfo,                   SIGNAL( doubleClicked(const QModelIndex &) ),
                this,                            SLOT  ( slot_tabvInfo_OnDoubleClicked(const QModelIndex &) ));

        connect(m_Ui.cboDictionaryPath,          SIGNAL( currentIndexChanged(const QString &) ),
                this,                            SLOT  ( slot_cboDictionaryPath_OnCurrentIndexChanged(const QString &) ));
    }

    //--------------------------------------------------
    // fire cboDictionaryPath
    {
        m_Ui.cboDictionaryPath->setCurrentIndex(- 1);
        m_Ui.cboDictionaryPath->setCurrentIndex(0);
    }
}
//---------------------------------------------------------------------------
void
CMain::_initActions() {
    // group "File"
    {
        actFile_CreateDb.setText(tr("Create DB"));
        connect(&actFile_CreateDb, SIGNAL( triggered() ),
                this,              SLOT  ( slot_OnCreateDb() ));
        //// m_Ui.toolBar->addAction(&actFile_CreateDb);

        //// m_Ui.toolBar->addSeparator();

        actFile_ImportCsv.setText(tr("Import CSV"));
        connect(&actFile_ImportCsv, SIGNAL( triggered() ),
                this,               SLOT  ( slot_OnImportCsv() ));
        //// m_Ui.toolBar->addAction(&actFile_ImportCsv);

        //// m_Ui.toolBar->addSeparator();

        actFile_ExportCsv.setText(tr("Export CSV"));
        connect(&actFile_ExportCsv, SIGNAL( triggered() ),
                this,               SLOT  ( slot_OnExportCsv() ));
        //// m_Ui.toolBar->addAction(&actFile_ExportCsv);

        actFile_ExportPdf.setText(tr("Export PDF"));
        connect(&actFile_ExportPdf, SIGNAL( triggered() ),
                this,               SLOT  ( slot_OnExportPdf() ));
        //// m_Ui.toolBar->addAction(&actFile_ExportPdf);

        //// m_Ui.toolBar->addSeparator();

        actFile_Exit.setText(tr("Exit"));
        connect(&actFile_Exit, SIGNAL( triggered() ),
                this,          SLOT  ( slot_OnExit() ));
        //// m_Ui.toolBar->addAction(&actFile_Exit);

        //// m_Ui.toolBar->addSeparator();
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

        m_Ui.toolBar->addSeparator();
    }

    // group "Find"
    {
        actFind_Search.setText(tr("Search"));
        connect(&actFind_Search, SIGNAL( triggered() ),
                this,            SLOT  ( slot_OnSearch() ));
        m_Ui.toolBar->addAction(&actFind_Search);

        m_Ui.toolBar->addSeparator();
    }

    // group "View"
    {
        actView_ZoomIn.setText(tr("Zoom In"));
        connect(&actView_ZoomIn,      SIGNAL( triggered() ),
                this,                 SLOT  ( slot_OnZoomIn() ));
        m_Ui.toolBar->addAction(&actView_ZoomIn);

        actView_ZoomOut.setText(tr("Zoom Out"));
        connect(&actView_ZoomOut,     SIGNAL( triggered() ),
                this,                 SLOT  ( slot_OnZoomOut() ));
        m_Ui.toolBar->addAction(&actView_ZoomOut);

        actView_ZoomDefault.setText(tr("Zoom default"));
        connect(&actView_ZoomDefault, SIGNAL( triggered() ),
                this,                 SLOT  ( slot_OnZoomDefault() ));
        m_Ui.toolBar->addAction(&actView_ZoomDefault);

        m_Ui.toolBar->addSeparator();
    }

    // group "Options"
    {
        actOptions_Settings.setText(tr("Settings"));
        connect(&actOptions_Settings, SIGNAL( triggered() ),
                this,                 SLOT  ( slot_OnSettings() ));
        //// m_Ui.toolBar->addAction(&actOptions_Settings);

        //// m_Ui.toolBar->addSeparator();
    }

    // group "Help"
    {
        actHelp_Faq.setText(tr("FAQ"));
        connect(&actHelp_Faq, SIGNAL( triggered() ),
                this,         SLOT  ( slot_OnFaq() ));
        //// m_Ui.toolBar->addAction(&actHelp_Faq);

        actHelp_About.setText(tr("About"));
        connect(&actHelp_About, SIGNAL( triggered() ),
                this,           SLOT  ( slot_OnAbout() ));
        //// m_Ui.toolBar->addAction(&actHelp_About);
    }
}
//---------------------------------------------------------------------------
void
CMain::_initMenus() {
    // group "File"
    {
        mnuFile.setTitle(tr("File"));

        mnuFile.addAction(&actFile_CreateDb);
        mnuFile.addSeparator();
        mnuFile.addAction(&actFile_ImportCsv);
        mnuFile.addSeparator();
        mnuFile.addAction(&actFile_ExportCsv);
        mnuFile.addAction(&actFile_ExportPdf);
        mnuFile.addSeparator();
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
        mnuEdit.addSeparator();
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

        mnuView.addAction(&actView_ZoomIn);
        mnuView.addAction(&actView_ZoomOut);
        mnuView.addAction(&actView_ZoomDefault);

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
CMain::slot_OnCreateDb() {
    const QString csDbName = QInputDialog::getText(
                                 this,
                                 m_sAppName,
                                 "New DB file path:",
                                 QLineEdit::Normal,
                                 ".db");
    qCHECK_DO(true == csDbName.trimmed().isEmpty(), return);

    QString sDictPath = m_sDbDir + QDir::separator() + csDbName;

    // reopen DB
    {
        dbReopen(sDictPath);
        cboDictionaryPath_reload();
    }

    // activate this DB file name in QComboBox
    {
        int iSectionPos = m_Ui.cboDictionaryPath->findText(csDbName);

        m_Ui.cboDictionaryPath->setCurrentIndex(iSectionPos);
    }
}
//---------------------------------------------------------------------------
void
CMain::slot_OnImportCsv() {
    // choose file path
    QString filePath = QFileDialog::getOpenFileName(
                            this,
                            "Open file",
                            "",
                            "csv files (*.csv)");
    qCHECK_DO(true == filePath.isEmpty(), return);

    // DB field names
    QVector<QString> fieldNames;

    fieldNames.push_back(CONFIG_DB_F_MAIN_TERM);
    fieldNames.push_back(CONFIG_DB_F_MAIN_VALUE);
    fieldNames.push_back(CONFIG_DB_F_MAIN_IS_LEARNED);
    fieldNames.push_back(CONFIG_DB_F_MAIN_IS_MARKED);

    // import
    CUtils::importCsv(filePath, _m_tmModel, fieldNames, "\t");

    // "fire" cboDictionaryPath
    {
        int iCurrent = m_Ui.cboDictionaryPath->currentIndex();
        m_Ui.cboDictionaryPath->setCurrentIndex(- 1);
        m_Ui.cboDictionaryPath->setCurrentIndex(iCurrent);
    }

    // report
    {
        QString sMsg = QString(tr("File: %1\nImport CSV finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), sMsg);
    }
}
//---------------------------------------------------------------------------
void
CMain::slot_OnExportCsv() {
    // choose file path
    QString filePath = QFileDialog::getSaveFileName(
                            this,
                            tr("Save file"),
                            m_Ui.cboDictionaryPath->currentText() + ".csv",
                            tr("CSV document (*.csv)"));
    qCHECK_DO(true == filePath.isEmpty(), return);

    // export
    {
        // DB field names
        QVector<QString> fieldNames;

        fieldNames.push_back(CONFIG_DB_F_MAIN_TERM);
        fieldNames.push_back(CONFIG_DB_F_MAIN_VALUE);
        fieldNames.push_back(CONFIG_DB_F_MAIN_IS_LEARNED);
        fieldNames.push_back(CONFIG_DB_F_MAIN_IS_MARKED);

        // import
        CUtils::exportCsv(filePath, _m_tmModel, fieldNames, "\t");
    }

    // report
    {
        QString sMsg = QString(tr("File: %1\nExport PDF finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), sMsg);
    }
}
//---------------------------------------------------------------------------
void
CMain::slot_OnExportPdf() {
    // choose file path
    QString filePath = QFileDialog::getSaveFileName(
                            this,
                            tr("Save file"),
                            m_Ui.cboDictionaryPath->currentText() + ".pdf",
                            tr("PDF Document (*.pdf)"));
    qCHECK_DO(true == filePath.isEmpty(), return);

    // DB -> text
    QString sHtml;


    // file -> DB
    int iRealRowCount = CUtils::sqlTableModelRowCount(_m_tmModel);

    for (int i = 0; i < iRealRowCount; ++ i) {
        sHtml.push_back( _m_tmModel->record(i).value(CONFIG_DB_F_MAIN_TERM).toString() );
        sHtml.push_back( "\n" );
        sHtml.push_back( " - " );
        sHtml.push_back( _m_tmModel->record(i).value(CONFIG_DB_F_MAIN_VALUE).toString() );
        sHtml.push_back( "<br>" );
    }

    // export
    {
        QPrinter prPrinter;

        prPrinter.setOutputFormat(QPrinter::PdfFormat);
        prPrinter.setOutputFileName(filePath);

        QTextDocument tdDoc;

        tdDoc.setHtml(sHtml);
        tdDoc.print(&prPrinter);
    }

    // report
    {
        QString sMsg = QString(tr("File: %1\nExport PDF finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), sMsg);
    }
}
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
    qCHECK_DO(NULL == _m_tmModel, return);

    m_navNavigator.insert();

    const int   ciCurrentRow = CUtils::sqlTableModelRowCount(_m_tmModel) - 1;
    CWordEditor dlgWordEditor(this, _m_tmModel, ciCurrentRow);

    dlgWordEditor.exec();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnRemove() {
    m_navNavigator.remove();
}
//---------------------------------------------------------------------------
void
CMain::slot_OnEdit() {
    qCHECK_DO(NULL == _m_tmModel, return);

    const int   ciCurrentRow = m_Ui.tabvInfo->currentIndex().row();
    CWordEditor dlgWordEditor(this, _m_tmModel, ciCurrentRow);

    dlgWordEditor.exec();
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
    const int   ciCurrentRow = index.row();
    CWordEditor dlgWordEditor(this, _m_tmModel, ciCurrentRow);

    dlgWordEditor.exec();
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
    qCHECK_DO(NULL == _m_tmModel, return);

    CWordFinder dlgWordFinder(this, _m_tmModel);

    dlgWordFinder.exec();
}
//---------------------------------------------------------------------------


/****************************************************************************
*   group "View"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnZoomIn() {
    // table font
    {
        int iOldSize = m_Ui.tabvInfo->font().pointSize();
        ++ iOldSize;

        QFont font = m_Ui.tabvInfo->font();
        font.setPointSize(iOldSize);

        m_Ui.tabvInfo->setFont(font);
    }

    // table row height
    {
        int iOldSize = m_Ui.tabvInfo->verticalHeader()->defaultSectionSize();
        ++ iOldSize;

        m_Ui.tabvInfo->verticalHeader()->setDefaultSectionSize(iOldSize);
    }
}
//---------------------------------------------------------------------------
void
CMain::slot_OnZoomOut() {
    // table font
    {
        int iOldSize = m_Ui.tabvInfo->font().pointSize();
        if (iOldSize > CONFIG_APP_FONT_SIZE_DEFAULT) {
            -- iOldSize;

            QFont font = m_Ui.tabvInfo->font();
            font.setPointSize(iOldSize);

            m_Ui.tabvInfo->setFont(font);
        }
    }

    // table row height
    {
        int iOldSize = m_Ui.tabvInfo->verticalHeader()->defaultSectionSize();
        if (iOldSize > CONFIG_TABLEVIEW_ROW_HEIGHT) {
            -- iOldSize;

            m_Ui.tabvInfo->verticalHeader()->setDefaultSectionSize(iOldSize);
        }
    }
}
//---------------------------------------------------------------------------
void
CMain::slot_OnZoomDefault() {
    // font
    {
        QFont font = m_Ui.tabvInfo->font();
        font.setPointSize(CONFIG_APP_FONT_SIZE_DEFAULT);

        m_Ui.tabvInfo->setFont(font);
    }

    // row height
    {
        m_Ui.tabvInfo->verticalHeader()->setDefaultSectionSize(CONFIG_TABLEVIEW_ROW_HEIGHT);
    }
}
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
void
CMain::slot_cboDictionaryPath_OnCurrentIndexChanged(
    const QString &arg
)
{
    qCHECK_DO(true == arg.isEmpty(), return);

    // reopen DB
    {
        QString sDictPath = m_sDbDir + QDir::separator() + arg;

        dbReopen(sDictPath);
    }

    // words info
    {
        // iWordsAll
        int iWordsAll = 0;

        {
            QSqlQuery qryWordsAll(*_m_dbDatabase);

            const QString csSql = \
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " CONFIG_DB_T_MAIN ";";

            bool bRv = qryWordsAll.exec(csSql);
            qCHECK_REF(bRv, qryWordsAll);

            bRv = qryWordsAll.next();
            qCHECK_REF(bRv, qryWordsAll);

            iWordsAll = qryWordsAll.value(0).toInt();
        }

        // iWordsLearned
        int iWordsLearned = 0;

        {
            QSqlQuery qryWordsLearned(*_m_dbDatabase);

            const QString csSql = \
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " CONFIG_DB_T_MAIN " "
                        "   WHERE " CONFIG_DB_F_MAIN_IS_LEARNED " = 1;";

            bool bRv = qryWordsLearned.exec(csSql);
            qCHECK_REF(bRv, qryWordsLearned);

            bRv = qryWordsLearned.next();
            qCHECK_REF(bRv, qryWordsLearned);

            iWordsLearned = qryWordsLearned.value(0).toInt();
        }

        // iWordsNotLearned
        int iWordsNotLearned = 0;

        {
            QSqlQuery qryWordsNotLearned(*_m_dbDatabase);

            const QString csSql = \
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " CONFIG_DB_T_MAIN " "
                        "   WHERE " CONFIG_DB_F_MAIN_IS_LEARNED " = 0;";

            bool bRv = qryWordsNotLearned.exec(csSql);
            qCHECK_REF(bRv, qryWordsNotLearned);

            bRv = qryWordsNotLearned.next();
            qCHECK_REF(bRv, qryWordsNotLearned);

            iWordsNotLearned = qryWordsNotLearned.value(0).toInt();
        }

        const QString csDictInfo = \
            QString(tr("&nbsp;&nbsp;&nbsp;<b>All</b>: %1 (%2)"
                       "&nbsp;&nbsp;&nbsp;<b>Learned</b>: %3 (%4)"
                       "&nbsp;&nbsp;&nbsp;<b>Not learned:</b> %5 (%6)"))
                        .arg( iWordsAll )
                        .arg( qS2QS(CxString::sFormatPercentage(iWordsAll, iWordsAll)) )
                        .arg( iWordsLearned )
                        .arg( qS2QS(CxString::sFormatPercentage(iWordsAll, iWordsLearned)) )
                        .arg( iWordsNotLearned )
                        .arg( qS2QS(CxString::sFormatPercentage(iWordsAll, iWordsNotLearned)) );

        m_Ui.lblDictInfo->setText(csDictInfo);
    }
}
//---------------------------------------------------------------------------


/****************************************************************************
*   private
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::cboDictionaryPath_reload() {
    qCHECK_DO(false == CxDir( qQS2S(m_sDbDir) ).bIsExists(), return);

    std::vec_tstring_t vsDicts;

    vsDicts.clear();
    CxDir( qQS2S(m_sDbDir) ).vFilesFind(xT("*.db"), true, &vsDicts);
    qCHECK_DO(true == vsDicts.empty(), return);

    m_Ui.cboDictionaryPath->clear();

    xFOREACH(std::vec_tstring_t, it, vsDicts) {
        QString sDict = qS2QS( (*it).erase(0, (qQS2S(m_sDbDir) + CxConst::xSLASH).size()) );

        m_Ui.cboDictionaryPath->addItem(sDict);
    }
}
//---------------------------------------------------------------------------


/****************************************************************************
*   private: DB
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::dbOpen(
    const QString &filePath
)
{
    // _m_dbDatabase
    {
        Q_ASSERT(NULL == _m_dbDatabase);
        Q_ASSERT(true == CxDir( qQS2S(m_sDbDir) ).bIsExists());

        bool bRv = QSqlDatabase::isDriverAvailable("QSQLITE");
        qCHECK_DO(false == bRv, qMSG(QSqlDatabase().lastError().text()); return);

        _m_dbDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
        _m_dbDatabase->setDatabaseName(filePath);

        bRv = _m_dbDatabase->open();
        qCHECK_PTR(bRv, _m_dbDatabase);

        // create DB
        {
            QSqlQuery qryInfo(*_m_dbDatabase);

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

    // _m_tmModel
    {
        Q_ASSERT(NULL == _m_tmModel);

        _m_tmModel = new QSqlTableModel(this, *_m_dbDatabase);

        _m_tmModel->setTable(CONFIG_DB_T_MAIN);
        _m_tmModel->setHeaderData(0, Qt::Horizontal, tr("Id"),      Qt::DisplayRole);
        _m_tmModel->setHeaderData(1, Qt::Horizontal, tr("Term"),    Qt::DisplayRole);
        _m_tmModel->setHeaderData(2, Qt::Horizontal, tr("Value"),   Qt::DisplayRole);
        _m_tmModel->setHeaderData(3, Qt::Horizontal, tr("Learned"), Qt::DisplayRole);
        _m_tmModel->setHeaderData(4, Qt::Horizontal, tr("Marked"),  Qt::DisplayRole);
        _m_tmModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        _m_tmModel->select();

        #if 0
            QSqlQueryModel *qmModel = dynamic_cast<QSqlQueryModel *>( _m_tmModel );
            Q_ASSERT(NULL != qmModel);

            const QString csSql = \
                    "SELECT * "
                    "   FROM " CONFIG_DB_T_MAIN " "
                    "   ORDER BY " CONFIG_DB_F_MAIN_TERM " DESC";

            qmModel->setQuery(csSql);
        #endif
    }

    //--------------------------------------------------
    // m_navNavigator
    {
        m_navNavigator.construct(_m_tmModel, m_Ui.tabvInfo);

        // go to the last record
        //// m_navNavigator.last();
        //// slot_tabvInfo_OnSelectionChanged(QItemSelection(), QItemSelection());
    }
}
//---------------------------------------------------------------------------
void
CMain::dbReopen(
    const QString &filePath
)
{
    dbClose();
    dbOpen(filePath);

    // _m_tmModel
    _m_tmModel->select();
    m_Ui.tabvInfo->setModel(_m_tmModel);
}
//---------------------------------------------------------------------------
void
CMain::dbClose() {
    // _m_tmModel
    {
        if (NULL != _m_tmModel) {
            xPTR_DELETE(_m_tmModel);
        }
    }

    // _m_dbDatabase
    {
        if (NULL != _m_dbDatabase) {
            Q_ASSERT(true == _m_dbDatabase->isOpen());

            const QString csConnectionName = _m_dbDatabase->connectionName();

            _m_dbDatabase->close();
            Q_ASSERT(false == _m_dbDatabase->isOpen());

            xPTR_DELETE(_m_dbDatabase);

            QSqlDatabase::removeDatabase(csConnectionName);
        }
    }

}
//---------------------------------------------------------------------------
