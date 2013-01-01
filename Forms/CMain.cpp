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

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>


/****************************************************************************
*   public
*
*****************************************************************************/

//---------------------------------------------------------------------------
CMain::CMain(
    QWidget    *parent,
    Qt::WFlags  flags
) :
    QMainWindow   (parent, flags),
    m_sAppName    (),
    m_sAppDir     (),
    m_sDbDir      (),
    m_sDbBackupDir(),
    m_sTempDir    (),
    m_navNavigator(this),
    _m_dbDatabase (NULL),
    _m_tmModel    (NULL)
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
    QObject *a_obj,
    QEvent  *a_ev
)
{
    // table zooming
    if (m_Ui.tabvInfo->viewport() == a_obj) {
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
//---------------------------------------------------------------------------
/* virtual */
void
CMain::keyPressEvent(
    QKeyEvent *a_ev
)
{
    switch (a_ev->key()) {
        // minimize on pressing escape
        case Qt::Key_Escape: {
                setWindowState(Qt::WindowMinimized);;
            }
            break;

        default: {
                QMainWindow::keyPressEvent(a_ev);
            }
            break;
    }
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

    _settingsLoad();
}
//---------------------------------------------------------------------------
void
CMain::_destruct() {
    _settingsSave();

    dbClose();
}
//---------------------------------------------------------------------------
void
CMain::_initMain() {
    m_Ui.setupUi(this);

    //--------------------------------------------------
    // data
    {
        m_sAppName     = QCoreApplication::applicationName();;
        m_sAppDir      = QCoreApplication::applicationDirPath();
        m_sDbDir       = m_sAppDir + QDir::separator() + CONFIG_DB_DIR_NAME;
        m_sDbBackupDir = m_sDbDir  + QDir::separator() + CONFIG_BACKUP_DIR_NAME;
        m_sTempDir     = m_sAppDir + QDir::separator() + CONFIG_TEMP_DIR_NAME;

        QDir().mkpath(m_sDbDir);
        QDir().mkpath(m_sDbBackupDir);
        QDir().mkpath(m_sTempDir);
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
            m_Ui.tabvInfo->viewport()->installEventFilter(this);

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
        connect(m_Ui.tabvInfo,          SIGNAL( doubleClicked(const QModelIndex &) ),
                this,                   SLOT  ( slot_OnEdit() ));

        connect(m_Ui.cboDictionaryPath, SIGNAL( currentIndexChanged(const QString &) ),
                this,                   SLOT  ( slot_cboDictionaryPath_OnCurrentIndexChanged(const QString &) ));
    }

    //--------------------------------------------------
    // fire cboDictionaryPath
    {
        m_Ui.cboDictionaryPath->setCurrentIndex(- 1);
        m_Ui.cboDictionaryPath->setCurrentIndex(0);
    }

    //--------------------------------------------------
    // m_navNavigator
    {
        m_navNavigator.last();
    }
}
//---------------------------------------------------------------------------
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

        connect(m_Ui.actEdit_Post,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPost() ));

        connect(m_Ui.actEdit_Cancel,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnCancel() ));

        connect(m_Ui.actEdit_Refresh,             SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnRefresh() ));
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
CMain::slot_OnImportClipboard() {
    qCHECK_DO(NULL == _m_tmModel, return);

    m_navNavigator.insert();

    const QString csData       = QApplication::clipboard()->text();
    const int     ciCurrentRow = CUtils::sqlTableModelRowCount(_m_tmModel) - 1;
    CWordEditor   dlgWordEditor(this, _m_tmModel, ciCurrentRow, csData);

    dlgWordEditor.exec();
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
CMain::slot_OnTo() {
    const int ciCurrentRow = m_Ui.tabvInfo->currentIndex().row() + 1;
    const int ciMinValue   = 1;
    const int ciMaxValue   = CUtils::sqlTableModelRowCount(_m_tmModel);

    int iTargetRow = QInputDialog::getInt(
                        this, CONFIG_APP_NAME, "Go to row:", ciCurrentRow, ciMinValue, ciMaxValue) - 1;

    m_navNavigator.to(iTargetRow);
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
*   group "Audio"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnPlayWord() {
    QString sText;
    {
        const int  ciCurrentRow = m_Ui.tabvInfo->currentIndex().row();
        QSqlRecord srRecord     = _m_tmModel->record(ciCurrentRow);

        sText = srRecord.value(CONFIG_DB_F_MAIN_TERM).toString();
    }

    QString sAudioFilePath;
    {
        sAudioFilePath = m_sTempDir + QDir::separator() + CONFIG_AUDIO_WORD_FILE_NAME;
    }

    _googleSpeech(sText, CONFIG_TRANSLATION_LANG_ENGLISH, sAudioFilePath);
}
//---------------------------------------------------------------------------
void
CMain::slot_OnPlayTranslation() {
    QString sText;
    {
        const int  ciCurrentRow = m_Ui.tabvInfo->currentIndex().row();
        QSqlRecord srRecord     = _m_tmModel->record(ciCurrentRow);

        sText = srRecord.value(CONFIG_DB_F_MAIN_VALUE).toString();
    }

    QString sAudioFilePath;
    {
        sAudioFilePath = m_sTempDir + QDir::separator() + CONFIG_AUDIO_TRANSLATION_FILE_NAME;
    }

    _googleSpeech(sText, CONFIG_TRANSLATION_LANG_RUSSIAN, sAudioFilePath);
}
//---------------------------------------------------------------------------
void
CMain::slot_OnPlayWordTranslation() {
    slot_OnPlayWord();
    slot_OnPlayTranslation();
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
    // TODO: slot_OnSettings
}
//---------------------------------------------------------------------------


/****************************************************************************
*   group "Help"
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::slot_OnFaq() {
    // TODO: slot_OnFaq
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
    const QString &a_arg
)
{
    qCHECK_DO(true == a_arg.isEmpty(), return);

    // reopen DB
    {
        QString sDictPath = m_sDbDir + QDir::separator() + a_arg;

        dbReopen(sDictPath);
    }

    // words info
    {
        // iWordsAll
        int iWordsAll = 0;

        {
            QSqlQuery qryWordsAll(*_m_dbDatabase);

            const QString csSql =
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

            const QString csSql =
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

            const QString csSql =
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " CONFIG_DB_T_MAIN " "
                        "   WHERE " CONFIG_DB_F_MAIN_IS_LEARNED " = 0;";

            bool bRv = qryWordsNotLearned.exec(csSql);
            qCHECK_REF(bRv, qryWordsNotLearned);

            bRv = qryWordsNotLearned.next();
            qCHECK_REF(bRv, qryWordsNotLearned);

            iWordsNotLearned = qryWordsNotLearned.value(0).toInt();
        }

        const QString csDictInfo =
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
    qCHECK_DO(! QDir(m_sDbDir).exists(), return);

    std::vec_tstring_t vsDicts;

    vsDicts.clear();

    CxDir( qQS2S(m_sDbDir) ).vFilesFind( CxString::sFormat(xT("*%s"), xT(CONFIG_DB_FILE_EXT) ), true, &vsDicts);
    qCHECK_DO(vsDicts.empty(), return);

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
    const QString &a_filePath
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
        _m_tmModel->setEditStrategy(QSqlTableModel::OnRowChange);

        _m_tmModel->select();
    }

    //--------------------------------------------------
    // m_navNavigator
    {
        m_navNavigator.construct(_m_tmModel, m_Ui.tabvInfo);
        m_navNavigator.last();
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
        xPTR_DELETE(_m_tmModel);
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


/****************************************************************************
*   private
*
*****************************************************************************/

/****************************************************************************
*   audio
*
*****************************************************************************/

//---------------------------------------------------------------------------
/* static */
void
CMain::_googleSpeech(
    const QString &a_text,
    const QString &a_lang,
    const QString &a_filePath
)
{
    // request to Google
    {
        const QString         csUrl = "http://translate.google.ru/translate_tts?&q=" + a_text + "&tl=" + a_lang;
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
        Phonon::MediaObject *moPlayer = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource(a_filePath));
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
    }
}
//---------------------------------------------------------------------------


/****************************************************************************
*   settings
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CMain::_settingsLoad() {
    QSize  szSize;
    QPoint pnPosition;
    int    iTableCurrentRow = 0;

    {
        QSettings stSettings(QCoreApplication::applicationName() + CONFIG_APP_SETTINGS_FIE_EXT,
                             QSettings::IniFormat, this);

        stSettings.beginGroup("main");
        szSize           = stSettings.value("size",     QSize(CONFIG_APP_WIDTH, CONFIG_APP_HEIGHT)).toSize();
        pnPosition       = stSettings.value("position", QPoint(200, 200)).toPoint();
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

        // table
        m_Ui.tabvInfo->setFocus();
        m_Ui.tabvInfo->selectRow(iTableCurrentRow);
    }
}
//---------------------------------------------------------------------------
void
CMain::_settingsSave() {
    QSettings stSettings(QCoreApplication::applicationName() + CONFIG_APP_SETTINGS_FIE_EXT,
                         QSettings::IniFormat, this);

    // main
    stSettings.beginGroup("main");
    stSettings.setValue("position", pos());
    stSettings.setValue("size",     size());
    stSettings.endGroup();

    // table
    stSettings.beginGroup("table");
    stSettings.setValue("current_row", m_Ui.tabvInfo->currentIndex().row());
    stSettings.endGroup();
}
//---------------------------------------------------------------------------
