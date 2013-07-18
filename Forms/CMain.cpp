/**
 * \file   CMain.cpp
 * \brief  main widget
 */


#include "CMain.h"

#include "../QtLib/CUtils.h"
#include "../Classes/CCheckBoxItemDelegate.h"
#include "../Classes/CComboBoxItemDelegate.h"
#include "../Forms/CWordEditor.h"
#include "../Forms/CWordFinder.h"

#include <QPrinter>

#include <xLib/Common/CxConst.h>
#include <xLib/Common/CxString.h>
#include <xLib/Filesystem/CxPath.h>
#include <xLib/Filesystem/CxFinder.h>


/*******************************************************************************
*   public
*
*******************************************************************************/

//------------------------------------------------------------------------------
CMain::CMain(
    QWidget         *a_parent,
    Qt::WindowFlags  a_flags
) :
    QMainWindow     (a_parent, a_flags),
    m_sAppName      (),
    m_sAppDir       (),
    m_sDbDir        (),
    m_sDbBackupDir  (),
    m_sTempDir      (),
    m_snSqlNavigator(this),
    _m_dbDatabase   (NULL),
    _m_tmModel      (NULL)
{
    _construct();
}
//------------------------------------------------------------------------------
/*virtual*/
CMain::~CMain() {
    _destruct();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   protected
*
*******************************************************************************/

//------------------------------------------------------------------------------
/* virtual */
bool
CMain::eventFilter(
    QObject *a_obj,
    QEvent  *a_ev
)
{
    // table zooming
    if (m_Ui.tvInfo->viewport() == a_obj) {
        if (QEvent::Wheel == a_ev->type()) {
            QWheelEvent *inputEvent = static_cast<QWheelEvent *>( a_ev );
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
//------------------------------------------------------------------------------
/* virtual */
void
CMain::keyPressEvent(
    QKeyEvent *a_ev
)
{
    switch (a_ev->key()) {
        // minimize on pressing escape
        case Qt::Key_Escape:
            setWindowState(Qt::WindowMinimized);;
            break;
        default:
            QMainWindow::keyPressEvent(a_ev);
            break;
    }
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   private
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::_construct() {
    _initMain();
    _initModel();
    _initActions();

    _settingsLoad();
}
//------------------------------------------------------------------------------
void
CMain::_destruct() {
    _settingsSave();

    dbClose();
}
//------------------------------------------------------------------------------
void
CMain::_initMain() {
    m_Ui.setupUi(this);

    //--------------------------------------------------
    // data
    {
        m_sAppName     = QCoreApplication::applicationName();;
        m_sAppDir      = QCoreApplication::applicationDirPath();
        m_sDbDir       = m_sAppDir + QDir::separator() + DB_DIR_NAME;
        m_sDbBackupDir = m_sDbDir  + QDir::separator() + BACKUP_DIR_NAME;
        m_sTempDir     = m_sAppDir + QDir::separator() + TEMP_DIR_NAME;

        QDir().mkpath(m_sDbDir);
        QDir().mkpath(m_sDbBackupDir);
        QDir().mkpath(m_sTempDir);
    }

    //--------------------------------------------------
    // CMain
    {
        setWindowIcon(QIcon(RES_MAIN_ICON));
        setWindowTitle(APP_NAME);
        setGeometry(0, 0, APP_WIDTH, APP_HEIGHT);
        CUtils::widgetAlignCenter(this);
        cboDictPath_reload();
    }
}
//------------------------------------------------------------------------------
void
CMain::_initModel() {
    //--------------------------------------------------
    // open DB
    {
        if (m_Ui.cboDictPath->currentText().isEmpty()) {
            cQString csDictPath = m_sDbDir + QDir::separator() + DB_FILE_NAME__NEW;

            dbOpen(csDictPath);
        } else {
            cQString csDictPath = m_sDbDir + QDir::separator() +
                                  m_Ui.cboDictPath->currentText();

            dbOpen(csDictPath);
        }
    }

    //--------------------------------------------------
    // _m_tmModel, tvInfo
    {
        // tvInfo
        {
            Q_ASSERT(_m_tmModel != NULL);

            m_Ui.tvInfo->setModel(_m_tmModel);
            m_Ui.tvInfo->viewport()->installEventFilter(this);

            m_Ui.tvInfo->hideColumn(0); // don't show the DB_F_MAIN_ID
            m_Ui.tvInfo->setColumnWidth(0, 40);
            m_Ui.tvInfo->setColumnWidth(1, 100);
            m_Ui.tvInfo->setColumnWidth(2, 400);
            m_Ui.tvInfo->setColumnWidth(3, 60);
            m_Ui.tvInfo->setColumnWidth(4, 60);
            m_Ui.tvInfo->setColumnWidth(5, 100);

            m_Ui.tvInfo->verticalHeader()->setVisible(true);
            m_Ui.tvInfo->verticalHeader()->setDefaultSectionSize(TABLEVIEW_ROW_HEIGHT);

            //m_Ui.tvInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
            //m_Ui.tvInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
            m_Ui.tvInfo->setSelectionMode(QAbstractItemView::ExtendedSelection);
            m_Ui.tvInfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            m_Ui.tvInfo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            m_Ui.tvInfo->setAlternatingRowColors(true);
            m_Ui.tvInfo->setStyleSheet("alternate-background-color: white; background-color: gray;");
            m_Ui.tvInfo->setSortingEnabled(true);
            m_Ui.tvInfo->sortByColumn(0, Qt::AscendingOrder);
            m_Ui.tvInfo->setItemDelegateForColumn(3, new CCheckBoxItemDelegate(m_Ui.tvInfo));
            m_Ui.tvInfo->setItemDelegateForColumn(4, new CCheckBoxItemDelegate(m_Ui.tvInfo));
            m_Ui.tvInfo->setItemDelegateForColumn(5, new CComboBoxItemDelegate(m_Ui.tvInfo, _m_tmModel));

            m_Ui.tvInfo->show();
        }
    }

    //--------------------------------------------------
    // slots
    {
        connect(m_Ui.tvInfo,      SIGNAL( doubleClicked(const QModelIndex &) ),
                this,             SLOT  ( slot_OnEdit() ));

        connect(m_Ui.cboDictPath, SIGNAL( currentIndexChanged(const QString &) ),
                this,             SLOT  ( slot_cboDictPath_OnCurrentIndexChanged(const QString &) ));
    }

    //--------------------------------------------------
    // fire cboDictPath
    {
        m_Ui.cboDictPath->setCurrentIndex(- 1);
        m_Ui.cboDictPath->setCurrentIndex(0);
    }

    //--------------------------------------------------
    // m_snSqlNavigator
    {
        m_snSqlNavigator.construct(_m_tmModel, m_Ui.tvInfo);
        m_snSqlNavigator.last();
    }
}
//------------------------------------------------------------------------------
void
CMain::_initActions() {
    // group "File"
    {
        connect(m_Ui.actFile_CreateDb,            SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnCreateDb() ));

        connect(m_Ui.actFile_ImportCsv,           SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnImportCsv() ));

        connect(m_Ui.actFile_ImportClipboard,     SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnImportClipboard() ));

        connect(m_Ui.actFile_ExportCsv,           SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnExportCsv() ));

        connect(m_Ui.actFile_ExportPdf,           SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnExportPdf() ));

        connect(m_Ui.actFile_Exit,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnExit() ));
    }

    // group "Edit"
    {
        connect(m_Ui.actEdit_First,               SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnFirst() ));

        connect(m_Ui.actEdit_Prior,               SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPrior() ));

        connect(m_Ui.actEdit_Next,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnNext() ));

        connect(m_Ui.actEdit_Last,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnLast() ));

        connect(m_Ui.actEdit_To,                  SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnTo() ));

        connect(m_Ui.actEdit_Insert,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnInsert() ));

        connect(m_Ui.actEdit_Delete,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnRemove() ));

        connect(m_Ui.actEdit_Edit,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnEdit() ));
    }

    // audio
    {
        connect(m_Ui.actEdit_PlayWord,            SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPlayWord() ));

        connect(m_Ui.actEdit_PlayTranslation,     SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPlayTranslation() ));

        connect(m_Ui.actEdit_PlayWordTranslation, SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPlayWordTranslation() ));
    }

    // group "Find"
    {
        connect(m_Ui.actFind_Search,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnSearch() ));
    }

    // group "View"
    {
        connect(m_Ui.actView_ZoomIn,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnZoomIn() ));

        connect(m_Ui.actView_ZoomOut,             SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnZoomOut() ));

        connect(m_Ui.actView_ZoomDefault,         SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnZoomDefault() ));
    }

    // group "Options"
    {
        connect(m_Ui.actOptions_Settings,         SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnSettings() ));
    }

    // group "Help"
    {
        connect(m_Ui.actHelp_Faq,                 SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnFaq() ));

        connect(m_Ui.actHelp_About,               SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnAbout() ));
    }
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "File"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnCreateDb() {
    cQString csDbName = QInputDialog::getText(
                                 this,
                                 m_sAppName,
                                 "New DB file path:",
                                 QLineEdit::Normal,
                                 DB_FILE_EXT);
    qCHECK_DO(true == csDbName.trimmed().isEmpty(), return);

    cQString csDictPath = m_sDbDir + QDir::separator() + csDbName;

    // reopen DB
    {
        dbReopen(csDictPath);
        cboDictPath_reload();
    }

    // activate this DB file name in QComboBox
    {
        cint ciSectionPos = m_Ui.cboDictPath->findText(csDbName);

        m_Ui.cboDictPath->setCurrentIndex(ciSectionPos);
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnImportCsv() {
    // choose file path
    cQString filePath = QFileDialog::getOpenFileName(
                            this,
                            "Open file",
                            "",
                            "csv files (*.csv)");
    qCHECK_DO(true == filePath.isEmpty(), return);

    // DB field names
    QVector<QString> fieldNames;

    fieldNames.push_back(DB_F_MAIN_TERM);
    fieldNames.push_back(DB_F_MAIN_VALUE);
    fieldNames.push_back(DB_F_MAIN_IS_LEARNED);
    fieldNames.push_back(DB_F_MAIN_IS_MARKED);
    fieldNames.push_back(DB_F_MAIN_TAG);

    // import
    CUtils::importCsv(filePath, _m_tmModel, fieldNames, "\t");

    // "fire" cboDictPath
    {
        cint iCurrent = m_Ui.cboDictPath->currentIndex();
        m_Ui.cboDictPath->setCurrentIndex(- 1);
        m_Ui.cboDictPath->setCurrentIndex(iCurrent);
    }

    // report
    {
        cQString csMsg = QString(tr("File: %1\nImport CSV finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), csMsg);
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnImportClipboard() {
    qCHECK_DO(NULL == _m_tmModel, return);

    m_snSqlNavigator.insert();

    cQString    csData       = QApplication::clipboard()->text();
    CWordEditor dlgWordEditor(this, _m_tmModel, &m_snSqlNavigator, csData);

    (int)dlgWordEditor.exec();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnExportCsv() {
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(
                            this,
                            tr("Save file"),
                            m_Ui.cboDictPath->currentText() + ".csv",
                            tr("CSV document (*.csv)"));
    qCHECK_DO(true == filePath.isEmpty(), return);

    // export
    {
        // DB field names
        QVector<QString> fieldNames;

        fieldNames.push_back(DB_F_MAIN_TERM);
        fieldNames.push_back(DB_F_MAIN_VALUE);
        fieldNames.push_back(DB_F_MAIN_IS_LEARNED);
        fieldNames.push_back(DB_F_MAIN_IS_MARKED);
        fieldNames.push_back(DB_F_MAIN_TAG);

        // import
        CUtils::exportCsv(filePath, _m_tmModel, fieldNames, "\t");
    }

    // report
    {
        cQString csMsg = QString(tr("File: %1\nExport PDF finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), csMsg);
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnExportPdf() {
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(
                            this,
                            tr("Save file"),
                            m_Ui.cboDictPath->currentText() + ".pdf",
                            tr("PDF Document (*.pdf)"));
    qCHECK_DO(true == filePath.isEmpty(), return);

    // DB -> text
    QString sHtml;


    // file -> DB
    cint ciRealRowCount = CUtils::sqlTableModelRowCount(_m_tmModel);

    for (int i = 0; i < ciRealRowCount; ++ i) {
        sHtml.push_back( _m_tmModel->record(i).value(DB_F_MAIN_TERM).toString() );
        sHtml.push_back( "\n" );
        sHtml.push_back( " - " );
        sHtml.push_back( _m_tmModel->record(i).value(DB_F_MAIN_VALUE).toString() );
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
        cQString csMsg = QString(tr("File: %1\nExport PDF finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), csMsg);
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnExit() {
    close();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "Edit"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnFirst() {
    m_snSqlNavigator.first();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnPrior() {
    m_snSqlNavigator.prior();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnNext() {
    m_snSqlNavigator.next();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnLast() {
    m_snSqlNavigator.last();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnTo() {
    cint ciCurrentRow = m_snSqlNavigator.view()->currentIndex().row() + 1;
    cint ciMinValue   = 1;
    cint ciMaxValue   = CUtils::sqlTableModelRowCount(_m_tmModel);

    cint ciTargetRow  = QInputDialog::getInt(
                            this, APP_NAME, "Go to row:",
                            ciCurrentRow, ciMinValue, ciMaxValue) - 1;

    m_snSqlNavigator.goTo(ciTargetRow);
}
//------------------------------------------------------------------------------
void
CMain::slot_OnInsert() {
    m_snSqlNavigator.insert();

    // show edit dialog
    {
        CWordEditor dlgWordEditor(this, _m_tmModel, &m_snSqlNavigator);

        (int)dlgWordEditor.exec();
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnRemove() {
    m_snSqlNavigator.remove();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnEdit() {
    // show edit dialog
    CWordEditor dlgWordEditor(this, _m_tmModel, &m_snSqlNavigator);

    (int)dlgWordEditor.exec();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "Audio"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnPlayWord() {
    QString sText;
    {
        cint       ciCurrentRow = m_Ui.tvInfo->currentIndex().row();
        QSqlRecord srRecord     = _m_tmModel->record(ciCurrentRow);

        sText = srRecord.value(DB_F_MAIN_TERM).toString();
    }

    QString sAudioFilePath;
    {
        sAudioFilePath = m_sTempDir + QDir::separator() + AUDIO_WORD_FILE_NAME;
    }

    _googleSpeech(sText, TRANSLATION_LANG_ENGLISH, sAudioFilePath);
}
//------------------------------------------------------------------------------
void
CMain::slot_OnPlayTranslation() {
    QString sText;
    {
        cint       ciCurrentRow = m_Ui.tvInfo->currentIndex().row();
        QSqlRecord srRecord     = _m_tmModel->record(ciCurrentRow);

        sText = srRecord.value(DB_F_MAIN_VALUE).toString();
    }

    QString sAudioFilePath;
    {
        sAudioFilePath = m_sTempDir + QDir::separator() + AUDIO_TRANSLATION_FILE_NAME;
    }

    _googleSpeech(sText, TRANSLATION_LANG_RUSSIAN, sAudioFilePath);
}
//------------------------------------------------------------------------------
void
CMain::slot_OnPlayWordTranslation() {
    slot_OnPlayWord();
    slot_OnPlayTranslation();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "Find"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnSearch() {
    qCHECK_DO(NULL == _m_tmModel, return);

    CWordFinder dlgWordFinder(this, _m_tmModel);

    dlgWordFinder.exec();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "View"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnZoomIn() {
    // table font
    {
        int iOldSize = m_Ui.tvInfo->font().pointSize();
        ++ iOldSize;

        QFont font = m_Ui.tvInfo->font();
        font.setPointSize(iOldSize);

        m_Ui.tvInfo->setFont(font);
    }

    // table row height
    {
        int iOldSize = m_Ui.tvInfo->verticalHeader()->defaultSectionSize();
        ++ iOldSize;

        m_Ui.tvInfo->verticalHeader()->setDefaultSectionSize(iOldSize);
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnZoomOut() {
    // table font
    {
        int iOldSize = m_Ui.tvInfo->font().pointSize();
        if (iOldSize > APP_FONT_SIZE_DEFAULT) {
            -- iOldSize;

            QFont font = m_Ui.tvInfo->font();
            font.setPointSize(iOldSize);

            m_Ui.tvInfo->setFont(font);
        }
    }

    // table row height
    {
        int iOldSize = m_Ui.tvInfo->verticalHeader()->defaultSectionSize();
        if (iOldSize > TABLEVIEW_ROW_HEIGHT) {
            -- iOldSize;

            m_Ui.tvInfo->verticalHeader()->setDefaultSectionSize(iOldSize);
        }
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnZoomDefault() {
    // font
    {
        QFont font = m_Ui.tvInfo->font();
        font.setPointSize(APP_FONT_SIZE_DEFAULT);

        m_Ui.tvInfo->setFont(font);
    }

    // row height
    {
        m_Ui.tvInfo->verticalHeader()->setDefaultSectionSize(TABLEVIEW_ROW_HEIGHT);
    }
}
//------------------------------------------------------------------------------

/*******************************************************************************
*   group "Options"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnSettings() {
    // TODO: slot_OnSettings
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "Help"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnFaq() {
    // TODO: slot_OnFaq
}
//------------------------------------------------------------------------------
void
CMain::slot_OnAbout() {
    cQString csMsg = QString(tr(
        "<p>"
        "<b>%1</b> - " APP_COMMENT
        "</p>"))
            .arg(APP_NAME);

    QMessageBox::about(this, tr("About ") + APP_NAME, csMsg);
}
//------------------------------------------------------------------------------
void
CMain::slot_cboDictPath_OnCurrentIndexChanged(
    const QString &a_arg
)
{
    qCHECK_DO(true == a_arg.isEmpty(), return);

    // reopen DB
    {
        cQString csDictPath = m_sDbDir + QDir::separator() + a_arg;

        dbReopen(csDictPath);
    }

    // words info
    {
        // iWordsAll
        int iWordsAll = 0;

        {
            QSqlQuery qryWordsAll(*_m_dbDatabase);

            cQString csSql =
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " DB_T_MAIN ";";

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

            cQString csSql =
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " DB_T_MAIN " "
                        "   WHERE " DB_F_MAIN_IS_LEARNED " = 1;";

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

            cQString csSql =
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " DB_T_MAIN " "
                        "   WHERE " DB_F_MAIN_IS_LEARNED " = 0;";

            bool bRv = qryWordsNotLearned.exec(csSql);
            qCHECK_REF(bRv, qryWordsNotLearned);

            bRv = qryWordsNotLearned.next();
            qCHECK_REF(bRv, qryWordsNotLearned);

            iWordsNotLearned = qryWordsNotLearned.value(0).toInt();
        }

        cQString csDictInfo =
            QString(tr("&nbsp;&nbsp;&nbsp;<b>All</b>: %1 (%2)"
                       "&nbsp;&nbsp;&nbsp;<b>Learned</b>: %3 (%4)"
                       "&nbsp;&nbsp;&nbsp;<b>Not learned:</b> %5 (%6)"))
                        .arg( iWordsAll )
                        .arg( qS2QS(CxString::formatPercentage(iWordsAll, iWordsAll)) )
                        .arg( iWordsLearned )
                        .arg( qS2QS(CxString::formatPercentage(iWordsAll, iWordsLearned)) )
                        .arg( iWordsNotLearned )
                        .arg( qS2QS(CxString::formatPercentage(iWordsAll, iWordsNotLearned)) );

        m_Ui.lblDictInfo->setText(csDictInfo);
    }
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   private
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::cboDictPath_reload() {
    qCHECK_DO(! QDir(m_sDbDir).exists(), return);

    std::vec_tstring_t vsDicts;

    vsDicts.clear();

    CxFinder::files(qQS2S(m_sDbDir), CxString::format(xT("*%s"), xT(DB_FILE_EXT) ), true, &vsDicts);
    qCHECK_DO(vsDicts.empty(), return);

    m_Ui.cboDictPath->clear();

    xFOREACH(std::vec_tstring_t, it, vsDicts) {
        QString sDict = qS2QS( (*it).erase(0, (qQS2S(m_sDbDir) + CxConst::xSLASH).size()) );

        m_Ui.cboDictPath->addItem(sDict);
    }
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   private: DB
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::dbOpen(
    cQString &a_filePath
)
{
    // _m_dbDatabase
    {
        Q_ASSERT(NULL == _m_dbDatabase);
        Q_ASSERT(true == QDir(m_sDbDir).exists());

        bool bRv = QSqlDatabase::isDriverAvailable("QSQLITE");
        qCHECK_DO(false == bRv, qMSG(QSqlDatabase().lastError().text()); return);

        _m_dbDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
        _m_dbDatabase->setDatabaseName(a_filePath);

        bRv = _m_dbDatabase->open();
        qCHECK_PTR(bRv, _m_dbDatabase);

        // create DB - DB_T_TAGS
        {
            QSqlQuery qryTags(*_m_dbDatabase);

            cQString csSql =
                    "CREATE TABLE IF NOT EXISTS "
                    "    " DB_T_TAGS
                    "( "
                    "    " DB_F_TAGS_ID   " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                    "    " DB_F_TAGS_NAME " varchar(255) DEFAULT '' UNIQUE "
                    ")";

            bRv = qryTags.exec(csSql);
            qCHECK_REF(bRv, qryTags);
        }

        // create DB - DB_T_MAIN
        {
            QSqlQuery qryMain(*_m_dbDatabase);

            cQString csSql =
                    "CREATE TABLE IF NOT EXISTS "
                    "    " DB_T_MAIN
                    "( "
                    "    " DB_F_MAIN_ID         " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                    "    " DB_F_MAIN_TERM       " varchar(255) UNIQUE, "
                    "    " DB_F_MAIN_VALUE      " varchar(255) DEFAULT '', "
                    "    " DB_F_MAIN_IS_LEARNED " integer      DEFAULT 0, "
                    "    " DB_F_MAIN_IS_MARKED  " integer      DEFAULT 0, "
                    "    " DB_F_MAIN_TAG        " varchar(255) DEFAULT '' UNIQUE, "
                    "    FOREIGN KEY(" DB_F_MAIN_TAG ") REFERENCES " DB_T_TAGS "(" DB_F_TAGS_NAME ")"
                    ")";

            bRv = qryMain.exec(csSql);
            qCHECK_REF(bRv, qryMain);
        }
    }

    // _m_tmModel
    {
        Q_ASSERT(NULL == _m_tmModel);

        _m_tmModel = new QSqlTableModel(this, *_m_dbDatabase);

        _m_tmModel->setTable(DB_T_MAIN);
        _m_tmModel->setHeaderData(0, Qt::Horizontal, tr("Id"),      Qt::DisplayRole);
        _m_tmModel->setHeaderData(1, Qt::Horizontal, tr("Term"),    Qt::DisplayRole);
        _m_tmModel->setHeaderData(2, Qt::Horizontal, tr("Value"),   Qt::DisplayRole);
        _m_tmModel->setHeaderData(3, Qt::Horizontal, tr("Learned"), Qt::DisplayRole);
        _m_tmModel->setHeaderData(4, Qt::Horizontal, tr("Marked"),  Qt::DisplayRole);
        _m_tmModel->setHeaderData(5, Qt::Horizontal, tr("Tag"),     Qt::DisplayRole);
        _m_tmModel->setEditStrategy(QSqlTableModel::OnFieldChange);

        _m_tmModel->select();
    }

    //--------------------------------------------------
    // m_snSqlNavigator
    {
        m_snSqlNavigator.construct(_m_tmModel, m_Ui.tvInfo);
        m_snSqlNavigator.last();
    }
}
//------------------------------------------------------------------------------
void
CMain::dbReopen(
    cQString &a_filePath
)
{
    dbClose();
    dbOpen(a_filePath);

    // _m_tmModel
    _m_tmModel->select();
    m_Ui.tvInfo->setModel(_m_tmModel);
}
//------------------------------------------------------------------------------
void
CMain::dbClose() {
    // _m_tmModel
    {
        xPTR_DELETE(_m_tmModel);
    }

    // _m_dbDatabase
    {
        if (NULL != _m_dbDatabase) {
            Q_ASSERT(true == _m_dbDatabase->isOpen());

            cQString csConnectionName = _m_dbDatabase->connectionName();

            _m_dbDatabase->close();
            Q_ASSERT(false == _m_dbDatabase->isOpen());

            xPTR_DELETE(_m_dbDatabase);

            QSqlDatabase::removeDatabase(csConnectionName);
        }
    }

}
//------------------------------------------------------------------------------


/*******************************************************************************
*   private
*
*******************************************************************************/

/*******************************************************************************
*   audio
*
*******************************************************************************/

//------------------------------------------------------------------------------
/* static */
void
CMain::_googleSpeech(
    cQString &a_text,
    cQString &a_lang,
    cQString &a_filePath
)
{
    // request to Google
    {
        cQString              csUrl = "http://translate.google.ru/translate_tts?&q=" +
                                       a_text + "&tl=" + a_lang;
        const QUrl            curUrl(csUrl);
        QNetworkAccessManager nmManager;
        const QNetworkRequest cnrRequest(curUrl);

        QNetworkReply *nrReply = nmManager.get(cnrRequest);
        Q_ASSERT(NULL != nrReply);

        do {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        while (! nrReply->isFinished());

        // write to audio file
        {
            QFile file(a_filePath);

            bool bRv = file.open(QIODevice::WriteOnly);
            Q_ASSERT(bRv);

            file.write(nrReply->readAll());
        }

        nrReply->close();
        delete nrReply; nrReply = NULL;
    }

    // play audio file
    {
    #if 0
        Phonon::MediaObject *moPlayer = Phonon::createPlayer(
                                            Phonon::MusicCategory,
                                            Phonon::MediaSource(a_filePath));
        Q_ASSERT(NULL != moPlayer);

        // for signal slot mechanism
        // connect(moPlayer, SIGNAL( finished() ),
        //         moPlayer, SLOT  ( deleteLater() ));

        moPlayer->play();

        // wait for finish
        for (bool bRv = true; bRv; ) {
            Phonon::State stState = moPlayer->state();
            if (Phonon::LoadingState == stState ||
                Phonon::PlayingState == stState)
            {
                bRv = true;
            } else {
                Q_ASSERT(Phonon::PausedState == stState || Phonon::StoppedState == stState);

                bRv = false;
            }

            CUtils::sleep(100);

            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        delete moPlayer;    moPlayer = NULL;
    #endif
    }
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   settings
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::_settingsLoad() {
    QSize  szSize;
    QPoint pnPosition;
    int    iDictionary = 0;
    int    iTableCurrentRow = 0;

    {
        QSettings stSettings(QCoreApplication::applicationName() + APP_SETTINGS_FILE_EXT,
                             QSettings::IniFormat, this);

        stSettings.beginGroup("main");
        szSize           = stSettings.value("size",        QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        pnPosition       = stSettings.value("position",    QPoint(200, 200)).toPoint();
        iDictionary      = stSettings.value("dictionary",  0).toInt();
        stSettings.endGroup();

        stSettings.beginGroup("table");
        iTableCurrentRow = stSettings.value("current_row", 0).toInt();
        stSettings.endGroup();
    }

    // apply settings
    {
        // main
        resize(szSize);
        move(pnPosition);
        m_Ui.cboDictPath->setCurrentIndex(iDictionary);

        // table
        m_snSqlNavigator.goTo(iTableCurrentRow);
    }
}
//------------------------------------------------------------------------------
void
CMain::_settingsSave() {
    QSettings stSettings(QCoreApplication::applicationName() + APP_SETTINGS_FILE_EXT,
                         QSettings::IniFormat, this);

    // main
    stSettings.beginGroup("main");
    stSettings.setValue("position",    pos());
    stSettings.setValue("size",        size());
    stSettings.setValue("dictionary",  m_Ui.cboDictPath->currentIndex());
    stSettings.endGroup();

    // table
    stSettings.beginGroup("table");
    stSettings.setValue("current_row", m_Ui.tvInfo->currentIndex().row());
    stSettings.endGroup();
}
//------------------------------------------------------------------------------
