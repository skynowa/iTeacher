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
#include <QMediaPlayer>

#if HAVE_XLIB
    #include <xLib/Core/CxConst.h>
    #include <xLib/Core/CxString.h>
    #include <xLib/Filesystem/CxPath.h>
    #include <xLib/Filesystem/CxFinder.h>
#endif


//-------------------------------------------------------------------------------------------------
#if 0

xNAMESPACE_ANONYM_BEGIN

std::ctstring_t
xlib_errorFormat
(
    Display *a_display,
    cint_t  &a_code
)
{
    std::tstring_t sRv;
    tchar_t        buff[1024 + 1] = {0};

    int_t iRv = ::XGetErrorText(a_display, a_code, buff, static_cast<int_t>( sizeof(buff) ) - 1);
    xTEST_DIFF(iRv, 0);

    sRv.assign(buff);

    return sRv;
}

int
xlib_errorHandler(
    Display     *a_display,
    XErrorEvent *a_errorEvent
)
{
    std::ctstring_t errorStr = ::xlib_errorFormat(a_display, a_errorEvent->error_code);

    CxTrace() << xT("xLib: XLIB error - ") << xTRACE_VAR7(a_errorEvent->type,
        a_errorEvent->resourceid, a_errorEvent->serial, a_errorEvent->error_code, errorStr,
        a_errorEvent->request_code, a_errorEvent->minor_code);

    return 0;
}

xNAMESPACE_ANONYM_END

#endif
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
CMain::CMain(
    QWidget         *a_parent,
    Qt::WindowFlags  a_flags
) :
    QMainWindow  (a_parent, a_flags),
    _appName     (),
    _appDir      (),
    _dbDir       (),
    _dbBackupDir (),
    _tempDir     (),
    _trayIcon    (this),
    _sqlNavigator(this),
    _dbDatabase  (Q_NULLPTR),
    _model       (Q_NULLPTR)
#if defined(Q_OS_UNIX) && 0
    // global hotkey
    ,
    _keyCode       (0),
    _display       (Q_NULLPTR)
#endif
{
    _construct();

#if defined(Q_OS_UNIX) && 0
    qCApp->installEventFilter(this);

    // global hotkey
    _display = ::XOpenDisplay(Q_NULLPTR);
    qTEST_PTR(_display);

    // handle errors is on
    ::XSynchronize(_display, 1 /*True*/);
    ::XSetErrorHandler(::xlib_errorHandler);

    _keyCode = ::XKeysymToKeycode(_display, XK_F1);
    iRv = ::XGrabKey(_display, _keyCode, ControlMask | ShiftMask, RootWindow(_display, 0),
        /*False*/1, GrabModeAsync, GrabModeAsync);
    xTEST_DIFF(iRv, 0);

    iRv = ::XFlush(_display);
    xTEST_DIFF(iRv, 0);
#endif
}
//-------------------------------------------------------------------------------------------------
/*virtual*/
CMain::~CMain()
{
#if defined(Q_OS_UNIX) && 0
    // global hotkey
    iRv = ::XUngrabKey(_display, _keyCode, ControlMask | ShiftMask, RootWindow(_display, 0));
    xTEST_DIFF(iRv, 0);

    iRv = ::XCloseDisplay(_display);  _display = Q_NULLPTR;
    xTEST_DIFF(iRv, 0);
#endif

    _destruct();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   protected
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
/* virtual */
bool
CMain::eventFilter(
    QObject *a_object,
    QEvent  *a_event
)
{
    // table zooming
    if (ui.tvInfo->viewport() == a_object) {
        if (QEvent::Wheel == a_event->type()) {
            QWheelEvent *inputEvent = static_cast<QWheelEvent *>( a_event );
            if (inputEvent->modifiers() & Qt::ControlModifier) {
                if (inputEvent->delta() > 0) {
                    slot_OnZoomIn();
                } else {
                    slot_OnZoomOut();
                }
            }
        }
    }

#if defined(Q_OS_UNIX) && 0
    // global hotkey
    if (a_event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(a_event);

        if (keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) &&
            keyEvent->key() == Qt::Key_F1)
        {
            qDebug() << "QxGlobalShortcut_x11 event";
            return false;
        }
    }
#endif
    return false;
}
//-------------------------------------------------------------------------------------------------
/* virtual */
void
CMain::keyPressEvent(
    QKeyEvent *a_event
)
{
    switch (a_event->key()) {
        // minimize on pressing escape
        case Qt::Key_Escape:
            setWindowState(Qt::WindowMinimized);;
            break;
        default:
            QMainWindow::keyPressEvent(a_event);
            break;
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::_construct()
{
    _initMain();
    _initModel();
    _initActions();

    _settingsLoad();
}
//-------------------------------------------------------------------------------------------------
void
CMain::_destruct()
{
    _settingsSave();

    dbClose();
}
//-------------------------------------------------------------------------------------------------
void
CMain::_initMain()
{
    ui.setupUi(this);

    // data
    {
        _appName     = QCoreApplication::applicationName();
    #if defined(Q_OS_ANDROID)
        _appDir      = QDir::homePath();
    #else
        _appDir      = QCoreApplication::applicationDirPath();
    #endif
        _dbDir       = _appDir + QDir::separator() + DB_DIR_NAME;
        _dbBackupDir = _appDir + QDir::separator() + BACKUP_DIR_NAME;
        _tempDir     = _appDir + QDir::separator() + TEMP_DIR_NAME;

        QDir().mkpath(_dbDir);
        QDir().mkpath(_dbBackupDir);
        QDir().mkpath(_tempDir);
    }

    // CMain
    {
        setWindowIcon( QIcon(RES_MAIN_ICON) );
        setWindowTitle(APP_NAME);
        setGeometry(0, 0, APP_WIDTH, APP_HEIGHT);
        CUtils::widgetAlignCenter(this);
        cboDictPath_reload();
    }

    // tray icon
    {
        qDebug() << qDEBUG_VAR(QSystemTrayIcon::isSystemTrayAvailable());

        _trayIcon.setIcon( QIcon(RES_MAIN_ICON) );
        _trayIcon.setVisible(true);
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::_initModel()
{
    // open DB
    {
        if (ui.cboDictPath->currentText().isEmpty()) {
            cQString dictPath = _dbDir + QDir::separator() + DB_FILE_NAME__NEW;

            dbOpen(dictPath);
            cboDictPath_reload();
        } else {
            cQString dictPath = _dbDir + QDir::separator() + ui.cboDictPath->currentText();

            dbOpen(dictPath);
        }
    }

    // _model
    {
        qTEST_PTR(_model);
    }

    // tvInfo
    {
        ui.tvInfo->setModel(_model);
        ui.tvInfo->viewport()->installEventFilter(this);

        ui.tvInfo->hideColumn(0); // don't show the DB_F_MAIN_ID
        ui.tvInfo->setColumnWidth(0, TABLEVIEW_COLUMN_WIDTH_0);
        ui.tvInfo->setColumnWidth(1, TABLEVIEW_COLUMN_WIDTH_1);
        ui.tvInfo->setColumnWidth(2, TABLEVIEW_COLUMN_WIDTH_2);
        ui.tvInfo->setColumnWidth(3, TABLEVIEW_COLUMN_WIDTH_3);
        ui.tvInfo->setColumnWidth(4, TABLEVIEW_COLUMN_WIDTH_4);
        ui.tvInfo->setColumnWidth(5, TABLEVIEW_COLUMN_WIDTH_5);

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
        // ui.tvInfo->setItemDelegateForColumn(5, new CComboBoxItemDelegate(ui.tvInfo, _model));

        ui.tvInfo->show();
    }

    // slots
    {
        connect(ui.tvInfo,      SIGNAL( doubleClicked(const QModelIndex &) ),
                this,           SLOT  ( slot_OnEdit() ));

        connect(ui.cboDictPath, SIGNAL( currentIndexChanged(const QString &) ),
                this,           SLOT  ( slot_cboDictPath_OnCurrentIndexChanged(const QString &) ));
    }

    // fire cboDictPath
    {
        ui.cboDictPath->setCurrentIndex(- 1);
        ui.cboDictPath->setCurrentIndex(0);
    }

    // _sqlNavigator
    {
        _sqlNavigator.construct(_model, ui.tvInfo);
        _sqlNavigator.last();
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::_initActions()
{
    // group "File"
    {
        connect(ui.actFile_CreateDb,            SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnCreateDb() ));

        connect(ui.actFile_ImportCsv,           SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnImportCsv() ));

        connect(ui.actFile_ImportClipboard,     SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnImportClipboard() ));

        connect(ui.actFile_ExportCsv,           SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnExportCsv() ));

        connect(ui.actFile_ExportPdf,           SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnExportPdf() ));

        connect(ui.actFile_Exit,                SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnExit() ));
    }

    // group "Edit"
    {
        connect(ui.actEdit_First,               SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnFirst() ));

        connect(ui.actEdit_Prior,               SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnPrior() ));

        connect(ui.actEdit_Next,                SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnNext() ));

        connect(ui.actEdit_Last,                SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnLast() ));

        connect(ui.actEdit_To,                  SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnTo() ));

        connect(ui.actEdit_Insert,              SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnInsert() ));

        connect(ui.actEdit_Delete,              SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnRemove() ));

        connect(ui.actEdit_Edit,                SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnEdit() ));
    }

    // audio
    {
        connect(ui.actEdit_PlayWord,            SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnPlayWord() ));

        connect(ui.actEdit_PlayTranslation,     SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnPlayTranslation() ));

        connect(ui.actEdit_PlayWordTranslation, SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnPlayWordTranslation() ));
    }

    // group "Find"
    {
        connect(ui.actFind_Search,              SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnSearch() ));
    }

    // group "View"
    {
        connect(ui.actView_ZoomIn,              SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnZoomIn() ));

        connect(ui.actView_ZoomOut,             SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnZoomOut() ));

        connect(ui.actView_ZoomDefault,         SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnZoomDefault() ));
    }

    // group "Options"
    {
        connect(ui.actOptions_Settings,         SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnSettings() ));
    }

    // group "Help"
    {
        connect(ui.actHelp_Faq,                 SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnFaq() ));

        connect(ui.actHelp_About,               SIGNAL( triggered() ),
                this,                           SLOT  ( slot_OnAbout() ));
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "File"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnCreateDb()
{
    cQString dbName = QInputDialog::getText(this, _appName, tr("New DB file path:"),
        QLineEdit::Normal, DB_FILE_EXT);
    qCHECK_DO(dbName.trimmed().isEmpty(), return);

    cQString dictPath = _dbDir + QDir::separator() + dbName;

    // reopen DB
    {
        dbReopen(dictPath);
        cboDictPath_reload();
    }

    // activate this DB file name in QComboBox
    {
        cint sectionPos = ui.cboDictPath->findText(dbName);

        ui.cboDictPath->setCurrentIndex(sectionPos);
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnImportCsv()
{
    // choose file path
    cQString filePath = QFileDialog::getOpenFileName(this, tr("Open file"), tr(""),
        tr("csv files (*.csv)"));
    qCHECK_DO(filePath.isEmpty(), return);

    // DB field names
    QVector<QString> fieldNames;

    fieldNames.push_back(DB_F_MAIN_TERM);
    fieldNames.push_back(DB_F_MAIN_VALUE);
    fieldNames.push_back(DB_F_MAIN_IS_LEARNED);
    fieldNames.push_back(DB_F_MAIN_IS_MARKED);
    fieldNames.push_back(DB_F_MAIN_TAG);

    // import
    CUtils::importCsv(filePath, _model, fieldNames, "\t");

    // "fire" cboDictPath
    {
        cint currentIndex = ui.cboDictPath->currentIndex();
        ui.cboDictPath->setCurrentIndex(- 1);
        ui.cboDictPath->setCurrentIndex(currentIndex);
    }

    // report
    {
        cQString msg = QString(tr("File: %1\nImport CSV finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnImportClipboard()
{
    qCHECK_DO(!_sqlNavigator.isValid(), return);

    _sqlNavigator.insert();

    cQString    data = QApplication::clipboard()->text();
    CWordEditor dlgWordEditor(this, _model, &_sqlNavigator, data);

    QDialog::DialogCode code = static_cast<QDialog::DialogCode>( dlgWordEditor.exec() );
    switch (code) {
        case QDialog::Rejected:
            _sqlNavigator.remove();
            break;
        default:
            break;
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnExportCsv()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(this, tr("Save file"),
        _exportfileNameBuild(".csv"), tr("CSV document (*.csv)"));
    qCHECK_DO(filePath.isEmpty(), return);

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
        CUtils::exportCsv(filePath, _model, fieldNames, "\t");
    }

    // report
    {
        cQString msg = QString(tr("File: %1\nExport CSV finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnExportPdf()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(this, tr("Save file"),
        _exportfileNameBuild(".pdf"), tr("PDF Document (*.pdf)"));
    qCHECK_DO(filePath.isEmpty(), return);

    // DB -> text
    QString sHtml;


    // file -> DB
    cint realRowCount = CUtils::sqlTableModelRowCount(_model);

    for (int i = 0; i < realRowCount; ++ i) {
        sHtml.push_back( _model->record(i).value(DB_F_MAIN_TERM).toString() );
        sHtml.push_back("\n");
        sHtml.push_back(" - ");
        sHtml.push_back( _model->record(i).value(DB_F_MAIN_VALUE).toString() );
        sHtml.push_back("<br>");
    }

    // export
    {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filePath);

        QTextDocument document;
        document.setHtml(sHtml);
        document.print(&printer);
    }

    // report
    {
        cQString msg = QString(tr("File: %1\nExport PDF finished."))
                            .arg(filePath);

        QMessageBox::information(this, qApp->applicationName(), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnExit()
{
    close();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Edit"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnFirst()
{
    _sqlNavigator.first();
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnPrior()
{
    _sqlNavigator.prior();
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnNext()
{
    _sqlNavigator.next();
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnLast()
{
    _sqlNavigator.last();
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnTo()
{
    cint currentRow = _sqlNavigator.view()->currentIndex().row() + 1;
    cint minValue   = 1;
    cint maxValue   = CUtils::sqlTableModelRowCount(_model);

    cint targetRow  = QInputDialog::getInt(this, APP_NAME, tr("Go to row:"), currentRow, minValue,
        maxValue) - 1;

    _sqlNavigator.goTo(targetRow);
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnInsert()
{
    qCHECK_DO(!_sqlNavigator.isValid(), return);

    _sqlNavigator.insert();

    CWordEditor dlgWordEditor(this, _model, &_sqlNavigator);

    QDialog::DialogCode code = static_cast<QDialog::DialogCode>( dlgWordEditor.exec() );
    switch (code) {
        case QDialog::Rejected:
            _sqlNavigator.remove();
            break;
        default:
            break;
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnRemove()
{
    _sqlNavigator.remove();
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnEdit()
{
    // show edit dialog
    CWordEditor dlgWordEditor(this, _model, &_sqlNavigator);

    (int)dlgWordEditor.exec();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Audio"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnPlayWord()
{
    QString text;
    {
        cint       currentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord record     = _model->record(currentRow);

        text = record.value(DB_F_MAIN_TERM).toString();
    }

    QString audioFilePath;
    {
        audioFilePath = _tempDir + QDir::separator() + AUDIO_WORD_FILE_NAME;
    }

    _googleSpeech(text, TRANSLATION_LANG_ENGLISH, audioFilePath);
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnPlayTranslation()
{
    QString text;
    {
        cint       currentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord record     = _model->record(currentRow);

        text = record.value(DB_F_MAIN_VALUE).toString();
    }

    QString audioFilePath;
    {
        audioFilePath = _tempDir + QDir::separator() + AUDIO_TRANSLATION_FILE_NAME;
    }

    _googleSpeech(text, TRANSLATION_LANG_RUSSIAN, audioFilePath);
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnPlayWordTranslation()
{
    slot_OnPlayWord();
    slot_OnPlayTranslation();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Find"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnSearch()
{
    qCHECK_DO(_model == Q_NULLPTR, return);

    CWordFinder dlgWordFinder(this, _model);

    dlgWordFinder.exec();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "View"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnZoomIn()
{
    // table font
    {
        cint sizeNew = ui.tvInfo->font().pointSize() + 1;

        QFont font = ui.tvInfo->font();
        font.setPointSize(sizeNew);

        ui.tvInfo->setFont(font);
    }

    // table row height
    {
        cint sizeNew = ui.tvInfo->verticalHeader()->defaultSectionSize() + 1;

        ui.tvInfo->verticalHeader()->setDefaultSectionSize(sizeNew);
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnZoomOut()
{
    // table font
    {
        int sizeNew = ui.tvInfo->font().pointSize();
        if (sizeNew > APP_FONT_SIZE_DEFAULT) {
            -- sizeNew;

            QFont font = ui.tvInfo->font();
            font.setPointSize(sizeNew);

            ui.tvInfo->setFont(font);
        }
    }

    // table row height
    {
        int sizeNew = ui.tvInfo->verticalHeader()->defaultSectionSize();
        if (sizeNew > TABLEVIEW_ROW_HEIGHT) {
            -- sizeNew;

            ui.tvInfo->verticalHeader()->setDefaultSectionSize(sizeNew);
        }
    }
}
//-------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Options"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnSettings()
{
    // TODO: slot_OnSettings
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Help"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnFaq()
{
    // TODO: slot_OnFaq
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_OnAbout()
{
    cQString msg = QString(tr(
        "<p>"
        "<b>%1</b> - " APP_COMMENT
        "</p>"))
            .arg(APP_NAME);

    QMessageBox::about(this, tr("About ") + APP_NAME, msg);
}
//-------------------------------------------------------------------------------------------------
void
CMain::slot_cboDictPath_OnCurrentIndexChanged(
    const QString &a_arg
)
{
    qCHECK_DO(a_arg.isEmpty(), return);

    // reopen DB
    {
        cQString dictPath = _dbDir + QDir::separator() + a_arg;

        dbReopen(dictPath);
    }

    // words info
    {
        int wordsAll = 0;
        {
            QSqlQuery qryWordsAll(*_dbDatabase);

            cQString sql =
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " DB_T_MAIN ";";

            bool bRv = qryWordsAll.exec(sql);
            qCHECK_REF(bRv, qryWordsAll);

            bRv = qryWordsAll.next();
            qCHECK_REF(bRv, qryWordsAll);

            wordsAll = qryWordsAll.value(0).toInt();
        }

        int wordsLearned = 0;
        {
            QSqlQuery qryWordsLearned(*_dbDatabase);

            cQString sql =
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " DB_T_MAIN " "
                        "   WHERE " DB_F_MAIN_IS_LEARNED " = 1;";

            bool bRv = qryWordsLearned.exec(sql);
            qCHECK_REF(bRv, qryWordsLearned);

            bRv = qryWordsLearned.next();
            qCHECK_REF(bRv, qryWordsLearned);

            wordsLearned = qryWordsLearned.value(0).toInt();
        }

        int wordsNotLearned = 0;
        {
            QSqlQuery qryWordsNotLearned(*_dbDatabase);

            cQString sql =
                        "SELECT COUNT(*) AS f_records_count "
                        "   FROM  " DB_T_MAIN " "
                        "   WHERE " DB_F_MAIN_IS_LEARNED " = 0;";

            bool bRv = qryWordsNotLearned.exec(sql);
            qCHECK_REF(bRv, qryWordsNotLearned);

            bRv = qryWordsNotLearned.next();
            qCHECK_REF(bRv, qryWordsNotLearned);

            wordsNotLearned = qryWordsNotLearned.value(0).toInt();
        }

        cQString dictInfo =
    #if HAVE_XLIB
            QString(tr("&nbsp;&nbsp;&nbsp;<b>All</b>: %1 (%2)"
                       "&nbsp;&nbsp;&nbsp;<b>Learned</b>: %3 (%4)"
                       "&nbsp;&nbsp;&nbsp;<b>Not learned:</b> %5 (%6)"))
                        .arg( wordsAll )
                        .arg( qS2QS(CxString::formatPercentage(wordsAll, wordsAll)) )
                        .arg( wordsLearned )
                        .arg( qS2QS(CxString::formatPercentage(wordsAll, wordsLearned)) )
                        .arg( wordsNotLearned )
                        .arg( qS2QS(CxString::formatPercentage(wordsAll, wordsNotLearned)) );

        ui.lblDictInfo->setText(dictInfo);
    #else
        // TODO: CMain::slot_cboDictPath_OnCurrentIndexChanged()
        "";

        ui.lblDictInfo->setText(dictInfo);
    #endif
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::cboDictPath_reload()
{
#if HAVE_XLIB
    qCHECK_DO(! QDir(_dbDir).exists(), return);

    std::vec_tstring_t dicts;

    CxFinder::files(qQS2S(_dbDir), CxString::format(xT("*%s"), xT(DB_FILE_EXT) ), true, &dicts);
    qCHECK_DO(dicts.empty(), return);

    ui.cboDictPath->clear();

    xFOREACH(std::vec_tstring_t, it, dicts) {
        QString dict = qS2QS( (*it).erase(0, (qQS2S(_dbDir) + CxConst::slash()).size()) );

        ui.cboDictPath->addItem(dict);
    }
#else
    // TODO: CMain::cboDictPath_reload()
#endif
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private: DB
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::dbOpen(
    cQString &a_filePath
)
{
    bool bRv = false;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // _dbDatabase
    {
        // prepare DB directory
        {
            QDir dir;
            dir.setPath(_dbDir);

            bRv = dir.exists();
            if (!bRv) {
                bRv = QDir().mkpath(_dbDir);
                qTEST(bRv);
            }

            qTEST( dir.exists() );
        }

        qTEST(_dbDatabase == Q_NULLPTR);

        bool bRv = QSqlDatabase::isDriverAvailable("QSQLITE");
        qCHECK_DO(!bRv, qMSG(QSqlDatabase().lastError().text()); return);

        _dbDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
        _dbDatabase->setDatabaseName(a_filePath);

        bRv = _dbDatabase->open();
        qCHECK_PTR(bRv, _dbDatabase);

        // create DB - DB_T_TAGS
        {
            QSqlQuery qryTags(*_dbDatabase);

            cQString sql =
                    "CREATE TABLE IF NOT EXISTS "
                    "    " DB_T_TAGS
                    "( "
                    "    " DB_F_TAGS_ID   " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                    "    " DB_F_TAGS_NAME " varchar(255) DEFAULT '' UNIQUE "
                    ")";

            bRv = qryTags.exec(sql);
            qCHECK_REF(bRv, qryTags);
        }

        // create DB - DB_T_MAIN
        {
            QSqlQuery qryMain(*_dbDatabase);

            cQString sql =
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

            bRv = qryMain.exec(sql);
            qCHECK_REF(bRv, qryMain);
        }
    }

    // _model
    {
        qTEST(_model == Q_NULLPTR);

        _model = new QSqlTableModel(this, *_dbDatabase);

        _model->setTable(DB_T_MAIN);
        _model->setHeaderData(0, Qt::Horizontal, tr("Id"),      Qt::DisplayRole);
        _model->setHeaderData(1, Qt::Horizontal, tr("Term"),    Qt::DisplayRole);
        _model->setHeaderData(2, Qt::Horizontal, tr("Value"),   Qt::DisplayRole);
        _model->setHeaderData(3, Qt::Horizontal, tr("Learned"), Qt::DisplayRole);
        _model->setHeaderData(4, Qt::Horizontal, tr("Marked"),  Qt::DisplayRole);
        _model->setHeaderData(5, Qt::Horizontal, tr("Tag"),     Qt::DisplayRole);
        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        _model->select();
    }

    // _sqlNavigator
    {
        _sqlNavigator.construct(_model, ui.tvInfo);
        _sqlNavigator.last();
    }

    QApplication::restoreOverrideCursor();
}
//-------------------------------------------------------------------------------------------------
void
CMain::dbReopen(
    cQString &a_filePath
)
{
    dbClose();
    dbOpen(a_filePath);

    // _model
    _model->select();
    ui.tvInfo->setModel(_model);
}
//-------------------------------------------------------------------------------------------------
void
CMain::dbClose()
{
    // _model
    {
        bool bRv = _model->submitAll();
        qCHECK_PTR(bRv, _model);

        qPTR_DELETE(_model);
    }

    // _dbDatabase
    {
        if (_dbDatabase != Q_NULLPTR) {
            qTEST(_dbDatabase->isOpen());

            cQString connectionName = _dbDatabase->connectionName();

            _dbDatabase->close();
            qTEST(!_dbDatabase->isOpen());

            qPTR_DELETE(_dbDatabase);

            QSqlDatabase::removeDatabase(connectionName);
        }
    }

}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

/**************************************************************************************************
*   audio
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
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
        cQString              urlStr = "http://translate.google.ru/translate_tts?&q=" +
                                       a_text + "&tl=" + a_lang;
        const QUrl            url(urlStr);
        QNetworkAccessManager manager;
        const QNetworkRequest request(url);

        QNetworkReply *reply = manager.get(request);
        qTEST_PTR(reply);

        for ( ;; ) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            qCHECK_DO( reply->isFinished(), break);
        }

        // write to audio file
        {
            QFile file(a_filePath);

            bool bRv = file.open(QIODevice::WriteOnly);
            qTEST(bRv);

            file.write(reply->readAll());
        }

        reply->close();
        qPTR_DELETE(reply);
    }

    // play audio file
    {
    #if 0
        Phonon::MediaObject *player = Phonon::createPlayer(
                                            Phonon::MusicCategory,
                                            Phonon::MediaSource(a_filePath));
        qTEST_PTR(player);

        // for signal slot mechanism
        // connect(player, SIGNAL( finished() ),
        //         player, SLOT  ( deleteLater() ));

        player->play();

        // wait for finish
        for (bool bRv = true; bRv; ) {
            Phonon::State state = player->state();
            if (Phonon::LoadingState == state ||
                Phonon::PlayingState == state)
            {
                bRv = true;
            } else {
                qTEST(Phonon::PausedState == state || Phonon::StoppedState == state);

                bRv = false;
            }

            CUtils::sleep(100);

            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        qPTR_DELETE(player);
    #else
        QMediaPlayer player;
        qCHECK_DO(!player.isAudioAvailable(),
            qDebug() << "QMediaPlayer: audio is not available"; return);

        player.setMedia(QUrl::fromLocalFile(a_filePath));
        player.setVolume(35);
        player.play();
    #endif
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   settings
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CMain::_settingsLoad()
{
    QSize  size;
    QPoint position;
    int    dictionary = 0;
    int    tableCurrentRow = 0;

    {
        QSettings settings(CApplication::configFilePath(), QSettings::IniFormat, this);

        settings.beginGroup("main");
        size           = settings.value("size",        QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        position       = settings.value("position",    QPoint(200, 200)).toPoint();
        dictionary     = settings.value("dictionary",  0).toInt();
        settings.endGroup();

        settings.beginGroup("table");
        tableCurrentRow = settings.value("current_row", 0).toInt();
        settings.endGroup();
    }

    // apply settings
    {
        // main
        resize(size);
        move(position);
        ui.cboDictPath->setCurrentIndex(dictionary);

        // table
        _sqlNavigator.goTo(tableCurrentRow);
    }
}
//-------------------------------------------------------------------------------------------------
void
CMain::_settingsSave()
{
    QSettings settings(CApplication::configFilePath(), QSettings::IniFormat, this);

    // main
    settings.beginGroup("main");
    settings.setValue("position",    pos());
    settings.setValue("size",        size());
    settings.setValue("dictionary",  ui.cboDictPath->currentIndex());
    settings.endGroup();

    // table
    settings.beginGroup("table");
    settings.setValue("current_row", ui.tvInfo->currentIndex().row());
    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------
QString
CMain::_exportfileNameBuild(
    cQString &a_fileExt
)
{
    qCHECK_RET(_model->rowCount() <= 0, QString());

    QString sRv;

    cQString tag = _model->record(0).value(DB_F_MAIN_TAG).toString().trimmed();

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
//-------------------------------------------------------------------------------------------------
