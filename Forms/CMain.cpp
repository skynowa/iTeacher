/**
 * \file   CMain.cpp
 * \brief  main widget
 */


#include "CMain.h"

#include "../QtLib/CUtils.h"
#include "../QtLib/CApplication.h"
#include "../Classes/CCheckBoxItemDelegate.h"
#include "../Classes/CComboBoxItemDelegate.h"
#include "../Forms/CWordEditor.h"
#include "../Forms/CWordFinder.h"

#include <QPrinter>

#include <xLib/Core/CxConst.h>
#include <xLib/Core/CxString.h>
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
    QMainWindow    (a_parent, a_flags),
    _sAppName      (),
    _sAppDir       (),
    _sDbDir        (),
    _sDbBackupDir  (),
    _sTempDir      (),
    _snSqlNavigator(this),
    _dbDatabase    (NULL),
    _tmModel       (NULL)
{
    _construct();
}
//------------------------------------------------------------------------------
/*virtual*/
CMain::~CMain()
{
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
    if (ui.tvInfo->viewport() == a_obj) {
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
CMain::_construct()
{
    _initMain();
    _initModel();
    _initActions();

    _settingsLoad();
}
//------------------------------------------------------------------------------
void
CMain::_destruct()
{
    _settingsSave();

    dbClose();
}
//------------------------------------------------------------------------------
void
CMain::_initMain()
{
    ui.setupUi(this);

    //--------------------------------------------------
    // data
    {
        _sAppName     = QCoreApplication::applicationName();;
        _sAppDir      = QCoreApplication::applicationDirPath();
        _sDbDir       = _sAppDir + QDir::separator() + DB_DIR_NAME;
        _sDbBackupDir = _sDbDir  + QDir::separator() + BACKUP_DIR_NAME;
        _sTempDir     = _sAppDir + QDir::separator() + TEMP_DIR_NAME;

        QDir().mkpath(_sDbDir);
        QDir().mkpath(_sDbBackupDir);
        QDir().mkpath(_sTempDir);
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
CMain::_initModel()
{
    //--------------------------------------------------
    // open DB
    {
        if (ui.cboDictPath->currentText().isEmpty()) {
            cQString csDictPath = _sDbDir + QDir::separator() + DB_FILE_NAME__NEW;

            dbOpen(csDictPath);
            cboDictPath_reload();
        } else {
            cQString csDictPath = _sDbDir + QDir::separator() +
                                  ui.cboDictPath->currentText();

            dbOpen(csDictPath);
        }
    }

    //--------------------------------------------------
    // _tmModel, tvInfo
    {
        // tvInfo
        {
            Q_ASSERT(_tmModel != NULL);

            ui.tvInfo->setModel(_tmModel);
            ui.tvInfo->viewport()->installEventFilter(this);

            ui.tvInfo->hideColumn(0); // don't show the DB_F_MAIN_ID
            ui.tvInfo->setColumnWidth(0, 40);
            ui.tvInfo->setColumnWidth(1, 120);
            ui.tvInfo->setColumnWidth(2, 400);
            ui.tvInfo->setColumnWidth(3, 60);
            ui.tvInfo->setColumnWidth(4, 60);
            ui.tvInfo->setColumnWidth(5, 80);

            ui.tvInfo->verticalHeader()->setVisible(true);
            ui.tvInfo->verticalHeader()->setDefaultSectionSize(TABLEVIEW_ROW_HEIGHT);

            ui.tvInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
            ui.tvInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
            ui.tvInfo->setSelectionMode(QAbstractItemView::ExtendedSelection);
            ui.tvInfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            ui.tvInfo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            ui.tvInfo->setAlternatingRowColors(true);
            ui.tvInfo->setStyleSheet("alternate-background-color: white; background-color: gray;");
            ui.tvInfo->setSortingEnabled(true);
            ui.tvInfo->sortByColumn(0, Qt::AscendingOrder);
            ui.tvInfo->setItemDelegateForColumn(3, new CCheckBoxItemDelegate(ui.tvInfo));
            ui.tvInfo->setItemDelegateForColumn(4, new CCheckBoxItemDelegate(ui.tvInfo));
            //ui.tvInfo->setItemDelegateForColumn(5, new CComboBoxItemDelegate(ui.tvInfo, _tmModel));

            ui.tvInfo->show();
        }
    }

    //--------------------------------------------------
    // slots
    {
        connect(ui.tvInfo,      SIGNAL( doubleClicked(const QModelIndex &) ),
                this,             SLOT  ( slot_OnEdit() ));

        connect(ui.cboDictPath, SIGNAL( currentIndexChanged(const QString &) ),
                this,             SLOT  ( slot_cboDictPath_OnCurrentIndexChanged(const QString &) ));
    }

    //--------------------------------------------------
    // fire cboDictPath
    {
        ui.cboDictPath->setCurrentIndex(- 1);
        ui.cboDictPath->setCurrentIndex(0);
    }

    //--------------------------------------------------
    // _snSqlNavigator
    {
        _snSqlNavigator.construct(_tmModel, ui.tvInfo);
        _snSqlNavigator.last();
    }
}
//------------------------------------------------------------------------------
void
CMain::_initActions()
{
    // group "File"
    {
        connect(ui.actFile_CreateDb,            SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnCreateDb() ));

        connect(ui.actFile_ImportCsv,           SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnImportCsv() ));

        connect(ui.actFile_ImportClipboard,     SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnImportClipboard() ));

        connect(ui.actFile_ExportCsv,           SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnExportCsv() ));

        connect(ui.actFile_ExportPdf,           SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnExportPdf() ));

        connect(ui.actFile_Exit,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnExit() ));
    }

    // group "Edit"
    {
        connect(ui.actEdit_First,               SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnFirst() ));

        connect(ui.actEdit_Prior,               SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPrior() ));

        connect(ui.actEdit_Next,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnNext() ));

        connect(ui.actEdit_Last,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnLast() ));

        connect(ui.actEdit_To,                  SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnTo() ));

        connect(ui.actEdit_Insert,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnInsert() ));

        connect(ui.actEdit_Delete,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnRemove() ));

        connect(ui.actEdit_Edit,                SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnEdit() ));
    }

    // audio
    {
        connect(ui.actEdit_PlayWord,            SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPlayWord() ));

        connect(ui.actEdit_PlayTranslation,     SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPlayTranslation() ));

        connect(ui.actEdit_PlayWordTranslation, SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnPlayWordTranslation() ));
    }

    // group "Find"
    {
        connect(ui.actFind_Search,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnSearch() ));
    }

    // group "View"
    {
        connect(ui.actView_ZoomIn,              SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnZoomIn() ));

        connect(ui.actView_ZoomOut,             SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnZoomOut() ));

        connect(ui.actView_ZoomDefault,         SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnZoomDefault() ));
    }

    // group "Options"
    {
        connect(ui.actOptions_Settings,         SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnSettings() ));
    }

    // group "Help"
    {
        connect(ui.actHelp_Faq,                 SIGNAL( triggered() ),
                this,                             SLOT  ( slot_OnFaq() ));

        connect(ui.actHelp_About,               SIGNAL( triggered() ),
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
CMain::slot_OnCreateDb()
{
    cQString csDbName = QInputDialog::getText(
                                 this,
                                 _sAppName,
                                 "New DB file path:",
                                 QLineEdit::Normal,
                                 DB_FILE_EXT);
    qCHECK_DO(true == csDbName.trimmed().isEmpty(), return);

    cQString csDictPath = _sDbDir + QDir::separator() + csDbName;

    // reopen DB
    {
        dbReopen(csDictPath);
        cboDictPath_reload();
    }

    // activate this DB file name in QComboBox
    {
        cint ciSectionPos = ui.cboDictPath->findText(csDbName);

        ui.cboDictPath->setCurrentIndex(ciSectionPos);
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnImportCsv()
{
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
    CUtils::importCsv(filePath, _tmModel, fieldNames, "\t");

    // "fire" cboDictPath
    {
        cint iCurrent = ui.cboDictPath->currentIndex();
        ui.cboDictPath->setCurrentIndex(- 1);
        ui.cboDictPath->setCurrentIndex(iCurrent);
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
CMain::slot_OnImportClipboard()
{
    qCHECK_DO(!_snSqlNavigator.isValid(), return);

    _snSqlNavigator.insert();

    cQString    csData = QApplication::clipboard()->text();
    CWordEditor dlgWordEditor(this, _tmModel, &_snSqlNavigator, csData);

    QDialog::DialogCode code = static_cast<QDialog::DialogCode>( dlgWordEditor.exec() );
    switch (code) {
        case QDialog::Rejected:
            _snSqlNavigator.remove();
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnExportCsv()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(
                            this,
                            tr("Save file"),
                            _exportfileNameBuild(".csv"),
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
        CUtils::exportCsv(filePath, _tmModel, fieldNames, "\t");
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
CMain::slot_OnExportPdf()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(
                            this,
                            tr("Save file"),
                            _exportfileNameBuild(".pdf"),
                            tr("PDF Document (*.pdf)"));
    qCHECK_DO(true == filePath.isEmpty(), return);

    // DB -> text
    QString sHtml;


    // file -> DB
    cint ciRealRowCount = CUtils::sqlTableModelRowCount(_tmModel);

    for (int i = 0; i < ciRealRowCount; ++ i) {
        sHtml.push_back( _tmModel->record(i).value(DB_F_MAIN_TERM).toString() );
        sHtml.push_back( "\n" );
        sHtml.push_back( " - " );
        sHtml.push_back( _tmModel->record(i).value(DB_F_MAIN_VALUE).toString() );
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
CMain::slot_OnExit()
{
    close();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "Edit"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnFirst()
{
    _snSqlNavigator.first();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnPrior()
{
    _snSqlNavigator.prior();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnNext()
{
    _snSqlNavigator.next();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnLast()
{
    _snSqlNavigator.last();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnTo()
{
    cint ciCurrentRow = _snSqlNavigator.view()->currentIndex().row() + 1;
    cint ciMinValue   = 1;
    cint ciMaxValue   = CUtils::sqlTableModelRowCount(_tmModel);

    cint ciTargetRow  = QInputDialog::getInt(
                            this, APP_NAME, "Go to row:",
                            ciCurrentRow, ciMinValue, ciMaxValue) - 1;

    _snSqlNavigator.goTo(ciTargetRow);
}
//------------------------------------------------------------------------------
void
CMain::slot_OnInsert()
{
    qCHECK_DO(!_snSqlNavigator.isValid(), return);

    _snSqlNavigator.insert();

    CWordEditor dlgWordEditor(this, _tmModel, &_snSqlNavigator);

    QDialog::DialogCode code = static_cast<QDialog::DialogCode>( dlgWordEditor.exec() );
    switch (code) {
        case QDialog::Rejected:
            _snSqlNavigator.remove();
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnRemove()
{
    _snSqlNavigator.remove();
}
//------------------------------------------------------------------------------
void
CMain::slot_OnEdit()
{
    // show edit dialog
    CWordEditor dlgWordEditor(this, _tmModel, &_snSqlNavigator);

    (int)dlgWordEditor.exec();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "Audio"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnPlayWord()
{
    QString sText;
    {
        cint       ciCurrentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord srRecord     = _tmModel->record(ciCurrentRow);

        sText = srRecord.value(DB_F_MAIN_TERM).toString();
    }

    QString sAudioFilePath;
    {
        sAudioFilePath = _sTempDir + QDir::separator() + AUDIO_WORD_FILE_NAME;
    }

    _googleSpeech(sText, TRANSLATION_LANG_ENGLISH, sAudioFilePath);
}
//------------------------------------------------------------------------------
void
CMain::slot_OnPlayTranslation()
{
    QString sText;
    {
        cint       ciCurrentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord srRecord     = _tmModel->record(ciCurrentRow);

        sText = srRecord.value(DB_F_MAIN_VALUE).toString();
    }

    QString sAudioFilePath;
    {
        sAudioFilePath = _sTempDir + QDir::separator() + AUDIO_TRANSLATION_FILE_NAME;
    }

    _googleSpeech(sText, TRANSLATION_LANG_RUSSIAN, sAudioFilePath);
}
//------------------------------------------------------------------------------
void
CMain::slot_OnPlayWordTranslation()
{
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
CMain::slot_OnSearch()
{
    qCHECK_DO(NULL == _tmModel, return);

    CWordFinder dlgWordFinder(this, _tmModel);

    dlgWordFinder.exec();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "View"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnZoomIn()
{
    // table font
    {
        int iOldSize = ui.tvInfo->font().pointSize();
        ++ iOldSize;

        QFont font = ui.tvInfo->font();
        font.setPointSize(iOldSize);

        ui.tvInfo->setFont(font);
    }

    // table row height
    {
        int iOldSize = ui.tvInfo->verticalHeader()->defaultSectionSize();
        ++ iOldSize;

        ui.tvInfo->verticalHeader()->setDefaultSectionSize(iOldSize);
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnZoomOut()
{
    // table font
    {
        int iOldSize = ui.tvInfo->font().pointSize();
        if (iOldSize > APP_FONT_SIZE_DEFAULT) {
            -- iOldSize;

            QFont font = ui.tvInfo->font();
            font.setPointSize(iOldSize);

            ui.tvInfo->setFont(font);
        }
    }

    // table row height
    {
        int iOldSize = ui.tvInfo->verticalHeader()->defaultSectionSize();
        if (iOldSize > TABLEVIEW_ROW_HEIGHT) {
            -- iOldSize;

            ui.tvInfo->verticalHeader()->setDefaultSectionSize(iOldSize);
        }
    }
}
//------------------------------------------------------------------------------
void
CMain::slot_OnZoomDefault()
{
    // font
    {
        QFont font = ui.tvInfo->font();
        font.setPointSize(APP_FONT_SIZE_DEFAULT);

        ui.tvInfo->setFont(font);
    }

    // row height
    {
        ui.tvInfo->verticalHeader()->setDefaultSectionSize(TABLEVIEW_ROW_HEIGHT);
    }
}
//------------------------------------------------------------------------------

/*******************************************************************************
*   group "Options"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnSettings()
{
    // TODO: slot_OnSettings
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   group "Help"
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::slot_OnFaq()
{
    // TODO: slot_OnFaq
}
//------------------------------------------------------------------------------
void
CMain::slot_OnAbout()
{
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
        cQString csDictPath = _sDbDir + QDir::separator() + a_arg;

        dbReopen(csDictPath);
    }

    // words info
    {
        // iWordsAll
        int iWordsAll = 0;

        {
            QSqlQuery qryWordsAll(*_dbDatabase);

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
            QSqlQuery qryWordsLearned(*_dbDatabase);

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
            QSqlQuery qryWordsNotLearned(*_dbDatabase);

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

        ui.lblDictInfo->setText(csDictInfo);
    }
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   private
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CMain::cboDictPath_reload()
{
    qCHECK_DO(! QDir(_sDbDir).exists(), return);

    std::vec_tstring_t vsDicts;

    vsDicts.clear();

    CxFinder::files(qQS2S(_sDbDir), CxString::format(xT("*%s"), xT(DB_FILE_EXT) ), true, &vsDicts);
    qCHECK_DO(vsDicts.empty(), return);

    ui.cboDictPath->clear();

    xFOREACH(std::vec_tstring_t, it, vsDicts) {
        QString sDict = qS2QS( (*it).erase(0, (qQS2S(_sDbDir) + CxConst::slash()).size()) );

        ui.cboDictPath->addItem(sDict);
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
    // _dbDatabase
    {
        Q_ASSERT(NULL == _dbDatabase);
        Q_ASSERT(true == QDir(_sDbDir).exists());

        bool bRv = QSqlDatabase::isDriverAvailable("QSQLITE");
        qCHECK_DO(false == bRv, qMSG(QSqlDatabase().lastError().text()); return);

        _dbDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
        _dbDatabase->setDatabaseName(a_filePath);

        bRv = _dbDatabase->open();
        qCHECK_PTR(bRv, _dbDatabase);

        // create DB - DB_T_TAGS
        {
            QSqlQuery qryTags(*_dbDatabase);

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
            QSqlQuery qryMain(*_dbDatabase);

            cQString csSql =
                    "CREATE TABLE IF NOT EXISTS "
                    "    " DB_T_MAIN
                    "( "
                    "    " DB_F_MAIN_ID         " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                    "    " DB_F_MAIN_TERM       " varchar(255) UNIQUE, "
                    "    " DB_F_MAIN_VALUE      " varchar(255) DEFAULT '', "
                    "    " DB_F_MAIN_IS_LEARNED " integer      DEFAULT 0, "
                    "    " DB_F_MAIN_IS_MARKED  " integer      DEFAULT 0, "
                    "    " DB_F_MAIN_TAG        " varchar(255) DEFAULT '', "
                    " "
                    "    FOREIGN KEY (" DB_F_MAIN_TAG ") REFERENCES " DB_T_TAGS "(" DB_F_TAGS_NAME ")"
                    ")";

            bRv = qryMain.exec(csSql);
            qCHECK_REF(bRv, qryMain);
        }
    }

    // _tmModel
    {
        Q_ASSERT(NULL == _tmModel);

        _tmModel = new QSqlTableModel(this, *_dbDatabase);

        _tmModel->setTable(DB_T_MAIN);
        _tmModel->setHeaderData(0, Qt::Horizontal, tr("Id"),      Qt::DisplayRole);
        _tmModel->setHeaderData(1, Qt::Horizontal, tr("Term"),    Qt::DisplayRole);
        _tmModel->setHeaderData(2, Qt::Horizontal, tr("Value"),   Qt::DisplayRole);
        _tmModel->setHeaderData(3, Qt::Horizontal, tr("Learned"), Qt::DisplayRole);
        _tmModel->setHeaderData(4, Qt::Horizontal, tr("Marked"),  Qt::DisplayRole);
        _tmModel->setHeaderData(5, Qt::Horizontal, tr("Tag"),     Qt::DisplayRole);
        _tmModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        _tmModel->select();
    }

    //--------------------------------------------------
    // _snSqlNavigator
    {
        _snSqlNavigator.construct(_tmModel, ui.tvInfo);
        _snSqlNavigator.last();
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

    // _tmModel
    _tmModel->select();
    ui.tvInfo->setModel(_tmModel);
}
//------------------------------------------------------------------------------
void
CMain::dbClose()
{
    // _tmModel
    {
        bool bRv = _tmModel->submitAll();
        qCHECK_PTR(bRv, _tmModel);

        qPTR_DELETE(_tmModel);
    }

    // _dbDatabase
    {
        if (NULL != _dbDatabase) {
            Q_ASSERT(true == _dbDatabase->isOpen());

            cQString csConnectionName = _dbDatabase->connectionName();

            _dbDatabase->close();
            Q_ASSERT(false == _dbDatabase->isOpen());

            qPTR_DELETE(_dbDatabase);

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
CMain::_settingsLoad()
{
    QSize  szSize;
    QPoint pnPosition;
    int    iDictionary = 0;
    int    iTableCurrentRow = 0;

    {
        QSettings stSettings(CApplication::iniFilePath(), QSettings::IniFormat, this);

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
        ui.cboDictPath->setCurrentIndex(iDictionary);

        // table
        _snSqlNavigator.goTo(iTableCurrentRow);
    }
}
//------------------------------------------------------------------------------
void
CMain::_settingsSave()
{
    QSettings stSettings(CApplication::iniFilePath(), QSettings::IniFormat, this);

    // main
    stSettings.beginGroup("main");
    stSettings.setValue("position",    pos());
    stSettings.setValue("size",        size());
    stSettings.setValue("dictionary",  ui.cboDictPath->currentIndex());
    stSettings.endGroup();

    // table
    stSettings.beginGroup("table");
    stSettings.setValue("current_row", ui.tvInfo->currentIndex().row());
    stSettings.endGroup();
}
//------------------------------------------------------------------------------
QString
CMain::_exportfileNameBuild(
    cQString &a_fileExt
)
{
    qCHECK_RET(_tmModel->rowCount() <= 0, QString());

    QString sRv;

    cQString tag = _tmModel->record(0).value(DB_F_MAIN_TAG).toString().trimmed();

    QString baseName;
    {
        baseName = QFileInfo( ui.cboDictPath->currentText() ).baseName().trimmed();
        if (!tag.isEmpty()) {
            baseName.append("-");
        }
    }

    sRv = baseName + tag + a_fileExt;

    return sRv;
}
//------------------------------------------------------------------------------
