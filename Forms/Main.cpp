/**
 * \file   Main.cpp
 * \brief  main widget
 */


#include "Main.h"

#include "../QtLib/Utils.h"
#include "../QtLib/Application.h"
#include "../Classes/CheckBoxItemDelegate.h"
#include "../Classes/ComboBoxItemDelegate.h"
#include "../Forms/WordEditor.h"
#include "../Forms/WordFinder.h"
#include "../Forms/TagsEditor.h"
#include "../Forms/About.h"

#include <QPrinter>
#include <QMediaPlayer>

#include <xLib/Core/Const.h>
#include <xLib/Core/String.h>
#include <xLib/Core/Application.h>
#include <xLib/IO/Path.h>
#include <xLib/IO/Finder.h>


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
Main::Main(
    QWidget         *a_parent,
    Qt::WindowFlags  a_flags
) :
    QMainWindow       (a_parent, a_flags),
    _trayIcon         (this),
    _scShowHide       (this),
    _scClipboardImport(this),
    _db               (Q_NULLPTR),
    _model            (Q_NULLPTR),
    _sqlNavigator     (this),
    _exportOrder      (eoUnknown)
{
    _construct();
}
//-------------------------------------------------------------------------------------------------
/*virtual*/
Main::~Main()
{
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
Main::eventFilter(
    QObject *a_object,
    QEvent  *a_event
)
{
    // table zooming
    if (ui.tvInfo->viewport() == a_object) {
        switch (a_event->type()) {
        case QEvent::Wheel:
            {
                QWheelEvent *inputEvent = static_cast<QWheelEvent *>( a_event );
                if (inputEvent->modifiers() & Qt::ControlModifier) {
                    if (inputEvent->delta() > 0) {
                        slot_OnZoomIn();
                    } else {
                        slot_OnZoomOut();
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    return false;
}
//-------------------------------------------------------------------------------------------------
/* virtual */
void
Main::keyPressEvent(
    QKeyEvent *a_event
)
{
    switch (a_event->key()) {
    case Qt::Key_Escape:
        setWindowState(Qt::WindowMinimized);
        break;
    default:
        QMainWindow::keyPressEvent(a_event);
        break;
    }
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void
Main::closeEvent(
    QCloseEvent *a_event
)
{
    if (true) {
        QApplication::setQuitOnLastWindowClosed(true);
        a_event->accept();
    } else {
        a_event->ignore();
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::_construct()
{
    _initMain();
    _initModel();
    _initActions();
    _settingsLoad();
}
//-------------------------------------------------------------------------------------------------
void
Main::_destruct()
{
    _settingsSave();
    _dbClose();
}
//-------------------------------------------------------------------------------------------------
void
Main::_initMain()
{
    ui.setupUi(this);

    // data
    {
    #if 0
        // TODO: _appDir (Android)

        #if defined(Q_OS_ANDROID)
            _appDir = QDir::homePath();
        #else
            _appDir  = qS2QS(xlib::core::Application::dirPath());
        #endif
    #endif
    }

    // Main
    {
        setWindowIcon( QIcon(RES_MAIN_ICON) );
        setWindowTitle( qS2QS(xlib::core::Application::name()) );
        setGeometry(0, 0, APP_WIDTH, APP_HEIGHT);
        qtlib::Utils::widgetAlignCenter(this);
        _cboDictPath_reload();
    }

    // tray icon
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        // mnuTrayIcon
        QMenu *mnuTrayIcon = new QMenu(this);
        mnuTrayIcon->addMenu(ui.menuFile);
        mnuTrayIcon->addMenu(ui.menuEdit);
        mnuTrayIcon->addMenu(ui.menuAudio);
        mnuTrayIcon->addMenu(ui.menuFind);
        mnuTrayIcon->addMenu(ui.menuView);
        mnuTrayIcon->addMenu(ui.menuOptions);
        mnuTrayIcon->addMenu(ui.menuHelp);
        mnuTrayIcon->addSeparator();

        // _trayIcon
        _trayIcon.setIcon( QIcon(RES_MAIN_PNG) );
        _trayIcon.setToolTip( qlApp->applicationName() );
        _trayIcon.setContextMenu(mnuTrayIcon);
        _trayIcon.show();
    } else {
        qDebug() << qDEBUG_VAR(QSystemTrayIcon::isSystemTrayAvailable());
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::_initModel()
{
    // open DB
    {
        if (ui.cboDictPath->currentText().isEmpty()) {
            cQString dictPath = qS2QS(xlib::core::Application::dbDirPath()) + QDir::separator() +
                DB_FILE_NAME_EMPTY;

            _dbOpen(dictPath);
            _cboDictPath_reload();
        } else {
            cQString dictPath = qS2QS(xlib::core::Application::dbDirPath()) + QDir::separator() +
                ui.cboDictPath->currentText();

            _dbOpen(dictPath);
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

        ui.tvInfo->horizontalHeader()->setStretchLastSection(true);
        ui.tvInfo->hideColumn(0); // don't show the DB_F_MAIN_ID
        ui.tvInfo->setColumnWidth(0, TVMAIN_COLUMN_WIDTH_0);
        ui.tvInfo->setColumnWidth(1, TVMAIN_COLUMN_WIDTH_1);
        ui.tvInfo->setColumnWidth(2, TVMAIN_COLUMN_WIDTH_2);
        ui.tvInfo->setColumnWidth(3, TVMAIN_COLUMN_WIDTH_3);
        ui.tvInfo->setColumnWidth(4, TVMAIN_COLUMN_WIDTH_4);
        ui.tvInfo->setColumnWidth(5, TVMAIN_COLUMN_WIDTH_5);
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
        ui.tvInfo->setItemDelegateForColumn(3, new CheckBoxItemDelegate(ui.tvInfo));
        ui.tvInfo->setItemDelegateForColumn(4, new CheckBoxItemDelegate(ui.tvInfo));
        // ui.tvInfo->setItemDelegateForColumn(5, new ComboBoxItemDelegate(ui.tvInfo, _model));

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
Main::_initActions()
{
    // group "File"
    {
        connect(ui.actFile_CreateDb,        SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnCreateDb() ));

        connect(ui.actFile_ImportCsv,       SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnImportCsv() ));

        connect(ui.actFile_ClipboardImport, SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnClipboardImport() ));

        connect(ui.actFile_ExportCsv,       SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnExportCsv() ));

        connect(ui.actFile_ExportPdf,       SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnExportPdf() ));

        connect(ui.actFile_ExportClipboard, SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnExportClipboard() ));

        connect(ui.actFile_Exit,            SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnExit() ));
    }

    // group "Edit"
    {
        connect(ui.actEdit_First,           SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnFirst() ));

        connect(ui.actEdit_Prior,           SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnPrior() ));

        connect(ui.actEdit_Next,            SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnNext() ));

        connect(ui.actEdit_Last,            SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnLast() ));

        connect(ui.actEdit_To,              SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnTo() ));

        connect(ui.actEdit_Insert,          SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnInsert() ));

        connect(ui.actEdit_Delete,          SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnRemove() ));

        connect(ui.actEdit_Edit,            SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnEdit() ));

        connect(ui.actEdit_Learned,         SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnLearned() ));

        connect(ui.actEdit_Marked,          SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnMarked() ));
    }

    // audio
    {
        connect(ui.actEdit_PlayTerm,        SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnPlayTerm() ));

        connect(ui.actEdit_PlayValue,       SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnPlayValue() ));

        connect(ui.actEdit_PlayTermValue,   SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnPlayTermValue() ));
    }

    // group "Find"
    {
        connect(ui.actFind_Search,          SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnSearch() ));
    }

    // group "View"
    {
        connect(ui.actView_ShowHide,        SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnShowHide() ));

        connect(ui.actView_ZoomIn,          SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnZoomIn() ));

        connect(ui.actView_ZoomOut,         SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnZoomOut() ));

        connect(ui.actView_ZoomDefault,     SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnZoomDefault() ));
    }

    // group "Options"
    {
        connect(ui.actOptions_TagsEditor,   SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnTagsEditor() ));

        connect(ui.actOptions_Settings,     SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnSettings() ));
    }

    // group "Help"
    {
        connect(ui.actHelp_Faq,             SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnFaq() ));

        connect(ui.actHelp_About,           SIGNAL( triggered() ),
                this,                       SLOT  ( slot_OnAbout() ));
    }

    // tray
    {
        connect(&_trayIcon,                 SIGNAL( activated(QSystemTrayIcon::ActivationReason) ),
                this,                       SLOT  ( slot_OnTrayActivated(QSystemTrayIcon::ActivationReason) ));
    }

    // global shortcut
    {
        connect(&_scShowHide,               SIGNAL( activated() ),
                this,                       SLOT  ( slot_OnShowHide() ));

        connect(&_scClipboardImport,        SIGNAL( activated() ),
                this,                       SLOT  ( slot_OnClipboardImport() ));
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "File"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::slot_OnCreateDb()
{
    cQString dbName = QInputDialog::getText(this, qS2QS(xlib::core::Application::name()),
        tr("New DB file path:"), QLineEdit::Normal, DB_FILE_EXT);
    qCHECK_DO(dbName.trimmed().isEmpty(), return);

    cQString dictPath = qS2QS(xlib::core::Application::dbDirPath()) + QDir::separator() + dbName;

    // reopen DB
    {
        _dbReopen(dictPath);
        _cboDictPath_reload();
    }

    // activate this DB file name in QComboBox
    {
        cint sectionPos = ui.cboDictPath->findText(dbName);

        ui.cboDictPath->setCurrentIndex(sectionPos);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnImportCsv()
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
    qtlib::Utils::importCsv(filePath, _model, fieldNames, "\t");

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

        QMessageBox::information(this, qlApp->applicationName(), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnClipboardImport()
{
    bool bRv = false;

    // WordEditor - only one instance
    QSharedMemory locker;
    {
        cQString dlgGuid = qS2QS(xlib::core::Application::name()) + "_dlgWordEditor_guid";

        locker.setKey(dlgGuid);

        bRv = locker.attach();
        qCHECK_DO(bRv, return);

        bRv = locker.create(1);
        qCHECK_DO(!bRv, return);
    }

    qCHECK_DO(!_sqlNavigator.isValid(), return);

    _sqlNavigator.insert();

    cQString   data = QApplication::clipboard()->text();
    WordEditor dlgWordEditor(this, _model, &_sqlNavigator, true, data);

    bRv = dlgWordEditor.isConstructed();
    qCHECK_DO(!bRv, return);

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
Main::slot_OnExportCsv()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(this, tr("Save file"),
        _exportfileNameBuild(".csv"), tr("CSV document (*.csv)"));
    qCHECK_DO(filePath.isEmpty(), return);

    // export
    {
        // DB field names
        QVector<QString> fieldNames;

        switch (_exportOrder) {
        case eoTermValue:
        default:
            fieldNames.push_back(DB_F_MAIN_TERM);
            fieldNames.push_back(DB_F_MAIN_VALUE);
            break;
        case eoValueTerm:
            fieldNames.push_back(DB_F_MAIN_VALUE);
            fieldNames.push_back(DB_F_MAIN_TERM);
            break;
        }

        fieldNames.push_back(DB_F_MAIN_IS_LEARNED);
        fieldNames.push_back(DB_F_MAIN_IS_MARKED);
        fieldNames.push_back(DB_F_MAIN_TAG);

        // import
        qtlib::Utils::exportCsv(filePath, _model, fieldNames, "\t");
    }

    // report
    {
        cQString msg = QString(tr("File: %1\nExport CSV finished."))
                            .arg(filePath);

        QMessageBox::information(this, qlApp->applicationName(), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnExportPdf()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(this, tr("Save file"),
        _exportfileNameBuild(".pdf"), tr("PDF Document (*.pdf)"));
    qCHECK_DO(filePath.isEmpty(), return);

    // DB -> text
    QString html;


    // file -> DB
    cint realRowCount = qtlib::Utils::sqlTableModelRowCount(_model);

    for (int i = 0; i < realRowCount; ++ i) {
        switch (_exportOrder) {
        case eoTermValue:
        default:
            html.push_back( _model->record(i).value(DB_F_MAIN_TERM).toString() );
            html.push_back("\n - ");
            html.push_back( _model->record(i).value(DB_F_MAIN_VALUE).toString() );
            html.push_back("<br>");
            break;
        case eoValueTerm:
            html.push_back( _model->record(i).value(DB_F_MAIN_VALUE).toString() );
            html.push_back("\n - ");
            html.push_back( _model->record(i).value(DB_F_MAIN_TERM).toString() );
            html.push_back("<br>");
            break;
        }
    }

    // export
    {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filePath);

        QTextDocument document;
        document.setHtml(html);
        document.print(&printer);
    }

    // report
    {
        cQString msg = QString(tr("File: %1\nExport PDF finished."))
                            .arg(filePath);

        QMessageBox::information(this, qlApp->applicationName(), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnExportClipboard()
{
    QString         sRv;
    cQString        separator = ",";
    QModelIndexList indexes   = _sqlNavigator.view()->selectionModel()->selectedIndexes();

    Q_FOREACH (QModelIndex index, indexes) {
        _sqlNavigator.view()->setFocus();

        cint targetRow = index.row();

        sRv += _model->record(targetRow).value(DB_F_MAIN_ID).toString() + separator;
        sRv += _model->record(targetRow).value(DB_F_MAIN_TERM).toString() + separator;
        sRv += _model->record(targetRow).value(DB_F_MAIN_VALUE).toString() + separator;
        sRv += _model->record(targetRow).value(DB_F_MAIN_IS_LEARNED).toString() + separator;
        sRv += _model->record(targetRow).value(DB_F_MAIN_IS_MARKED).toString() + separator;
        sRv += _model->record(targetRow).value(DB_F_MAIN_TAG).toString();
        sRv += "\n";
    }

    QApplication::clipboard()->setText(sRv);

    // report
    {
        cQString msg = QString(tr("Text: \n%1\nExport Clipboard finished."))
                            .arg(sRv);

        QMessageBox::information(this, qlApp->applicationName(), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnExit()
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
Main::slot_OnFirst()
{
    _sqlNavigator.first();
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnPrior()
{
    _sqlNavigator.prior();
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnNext()
{
    _sqlNavigator.next();
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnLast()
{
    _sqlNavigator.last();
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnTo()
{
    cint currentRow = _sqlNavigator.view()->currentIndex().row() + 1;
    cint minValue   = 1;
    cint maxValue   = qtlib::Utils::sqlTableModelRowCount(_model);

    cint targetRow  = QInputDialog::getInt(this, qS2QS(xlib::core::Application::name()),
        tr("Go to row:"), currentRow, minValue, maxValue) - 1;

    _sqlNavigator.goTo(targetRow);
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnInsert()
{
    qCHECK_DO(!_sqlNavigator.isValid(), return);

    _sqlNavigator.insert();

    WordEditor dlgWordEditor(this, _model, &_sqlNavigator, true);

    bool bRv = dlgWordEditor.isConstructed();
    qCHECK_DO(!bRv, return);

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
Main::slot_OnRemove()
{
    qCHECK_DO(_sqlNavigator.view()->currentIndex().row() < 0, return);

    QString text;
    QString informativeText;
    {
        cint       currentRow = _sqlNavigator.view()->currentIndex().row();
        QSqlRecord record     = _sqlNavigator.model()->record(currentRow);

        cQString   wordTerm   = record.value(DB_F_MAIN_TERM).toString();
        cQString   wordValue  = record.value(DB_F_MAIN_VALUE).toString();

        text            = QString(tr("Remove record number %1?"))
                            .arg(currentRow + 1);
        informativeText = QString(tr("%2 - %3"))
                            .arg(wordTerm)
                            .arg(wordValue);
    }

    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int iRv = msgBox.exec();
    switch (iRv) {
    case QMessageBox::Yes:
        _sqlNavigator.remove();
        break;
    case QMessageBox::Cancel:
    default:
        break;
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnEdit()
{
    // show edit dialog
    WordEditor dlgWordEditor(this, _model, &_sqlNavigator, false);

    bool bRv = dlgWordEditor.isConstructed();
    qCHECK_DO(!bRv, return);

    (int)dlgWordEditor.exec();
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnLearned()
{
    bool bRv = false;

    cint currentRow = _sqlNavigator.view()->currentIndex().row();

    QSqlRecord record = _model->record(currentRow);
    record.setValue(DB_F_MAIN_IS_LEARNED, !record.value(DB_F_MAIN_IS_LEARNED).toBool() );

    bRv = _model->setRecord(currentRow, record);
    qCHECK_PTR(bRv, _model);

    bRv = _model->submitAll();
    qTEST(bRv);

    _sqlNavigator.goTo(currentRow);
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnMarked()
{
    bool bRv = false;

    cint currentRow = _sqlNavigator.view()->currentIndex().row();

    QSqlRecord record = _model->record(currentRow);
    record.setValue(DB_F_MAIN_IS_MARKED,  !record.value(DB_F_MAIN_IS_MARKED).toBool() );

    bRv = _model->setRecord(currentRow, record);
    qCHECK_PTR(bRv, _model);

    bRv = _model->submitAll();
    qTEST(bRv);

    _sqlNavigator.goTo(currentRow);
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Audio"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::slot_OnPlayTerm()
{
    QString text;
    {
        cint       currentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord record     = _model->record(currentRow);

        text = record.value(DB_F_MAIN_TERM).toString();
    }

    QString audioFilePath;
    {
        audioFilePath = qS2QS(xlib::core::Application::tempDirPath()) + QDir::separator() +
            AUDIO_TERM_FILE_NAME;
    }

    _googleSpeech(text, LANG_EN, audioFilePath);
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnPlayValue()
{
    QString text;
    {
        cint       currentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord record     = _model->record(currentRow);

        text = record.value(DB_F_MAIN_VALUE).toString();
    }

    QString audioFilePath;
    {
        audioFilePath = qS2QS(xlib::core::Application::tempDirPath()) + QDir::separator() +
            AUDIO_VALUE_FILE_NAME;
    }

    _googleSpeech(text, LANG_RU, audioFilePath);
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnPlayTermValue()
{
    slot_OnPlayTerm();
    slot_OnPlayValue();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Find"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::slot_OnSearch()
{
    qCHECK_DO(!_sqlNavigator.isValid(), return);

    WordFinder dlgWordFinder(this, _model);

    dlgWordFinder.exec();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "View"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::slot_OnShowHide()
{
    setVisible( !isVisible() );
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnZoomIn()
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
Main::slot_OnZoomOut()
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
Main::slot_OnZoomDefault()
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
Main::slot_OnTagsEditor()
{
    TagsEditor dlgTagsEditor(this, *_db);

    (int)dlgTagsEditor.exec();
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnSettings()
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
Main::slot_OnFaq()
{
    // TODO: slot_OnFaq
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_OnAbout()
{
    About dlgAbout(this, windowIcon());

    (int)dlgAbout.exec();
}
//-------------------------------------------------------------------------------------------------
void
Main::slot_cboDictPath_OnCurrentIndexChanged(
    const QString &a_arg
)
{
    qCHECK_DO(a_arg.isEmpty(), return);

    // reopen DB
    {
        cQString dictPath = qS2QS(xlib::core::Application::dbDirPath()) + QDir::separator() + a_arg;

        _dbReopen(dictPath);
    }

    // words info
    {
        int wordsAll = 0;
        {
            QSqlQuery qryWordsAll(*_db);

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
            QSqlQuery qryWordsLearned(*_db);

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
            QSqlQuery qryWordsNotLearned(*_db);

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

        cQString dictInfo = QString(
            tr("&nbsp;&nbsp;&nbsp;<b>All</b>: %1 (%2)"
            "&nbsp;&nbsp;&nbsp;<b>Learned</b>: %3 (%4)"
            "&nbsp;&nbsp;&nbsp;<b>Not learned:</b> %5 (%6)"))
            .arg( wordsAll )
            .arg( qS2QS(xlib::core::String::formatPercentage(wordsAll, wordsAll)) )
            .arg( wordsLearned )
            .arg( qS2QS(xlib::core::String::formatPercentage(wordsAll, wordsLearned)) )
            .arg( wordsNotLearned )
            .arg( qS2QS(xlib::core::String::formatPercentage(wordsAll, wordsNotLearned)) );

        ui.lblDictInfo->setText(dictInfo);
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   tray
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::slot_OnTrayActivated(
    QSystemTrayIcon::ActivationReason a_reason
)
{
    switch (a_reason) {
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
        slot_OnShowHide();
        break;
    case QSystemTrayIcon::MiddleClick:
        slot_OnClipboardImport();
        break;
    default:
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
Main::_cboDictPath_reload()
{
    ui.cboDictPath->clear();

    qCHECK_DO(! QDir( qS2QS(xlib::core::Application::dbDirPath()) ).exists(), return);

    std::vec_tstring_t dicts;

    xlib::io::Finder::files(xlib::core::Application::dbDirPath(),
        xlib::core::String::format(xT("*%s"), xT(DB_FILE_EXT)), true, &dicts);

    xFOREACH(std::vec_tstring_t, it, dicts) {
        cQString dict = qS2QS( it->erase(0, (xlib::core::Application::dbDirPath() +
            xlib::core::Const::slash()).size()) );

        ui.cboDictPath->addItem(dict);
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private: DB
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::_dbOpen(
    cQString &a_filePath
)
{
    bool bRv = false;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // _db
    {
        // prepare DB directory
        {
            QDir dir;
            dir.setPath( qS2QS(xlib::core::Application::dbDirPath()) );

            bRv = dir.exists();
            if (!bRv) {
                bRv = QDir().mkpath( qS2QS(xlib::core::Application::dbDirPath()) );
                qTEST(bRv);
            }

            qTEST( dir.exists() );
        }

        qTEST(_db == Q_NULLPTR);

        bool bRv = QSqlDatabase::isDriverAvailable("QSQLITE");
        qCHECK_DO(!bRv, qMSG(QSqlDatabase().lastError().text()); return);

        _db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
        _db->setDatabaseName(a_filePath);

        bRv = _db->open();
        qCHECK_PTR(bRv, _db);

        // create DB - DB_T_TAGS
        {
            QSqlQuery qryPragma(*_db);

            cQString sql =
                "PRAGMA foreign_keys = ON";

            bRv = qryPragma.exec(sql);
            qCHECK_REF(bRv, qryPragma);
        }

        // DB pragma
        {
            QSqlQuery qryTags(*_db);

            cQString sql =
                "CREATE TABLE IF NOT EXISTS "
                "    " DB_T_TAGS
                "( "
                "    " DB_F_TAGS_ID   " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                "    " DB_F_TAGS_NAME " varchar(255) DEFAULT '' UNIQUE"
                ")";

            bRv = qryTags.exec(sql);
            qCHECK_REF(bRv, qryTags);
        }

        // create DB - DB_T_MAIN
        {
            QSqlQuery qryMain(*_db);

            cQString sql =
                "CREATE TABLE IF NOT EXISTS "
                "    " DB_T_MAIN
                "( "
                "    " DB_F_MAIN_ID         " integer PRIMARY KEY AUTOINCREMENT UNIQUE, "
                "    " DB_F_MAIN_TERM       " varchar(255) UNIQUE, "
                "    " DB_F_MAIN_VALUE      " varchar(255) DEFAULT '', "
                "    " DB_F_MAIN_IS_LEARNED " integer      DEFAULT 0, "
                "    " DB_F_MAIN_IS_MARKED  " integer      DEFAULT 0, "
                "    " DB_F_MAIN_TAG        " integer      DEFAULT 1, "
                " "
                "    FOREIGN KEY (" DB_F_MAIN_TAG ") REFERENCES " DB_T_TAGS "(" DB_F_TAGS_ID ") "
                "    ON UPDATE CASCADE "
                ")";

            bRv = qryMain.exec(sql);
            qCHECK_REF(bRv, qryMain);
        }
    }

    // _model
    {
        qTEST(_model == Q_NULLPTR);

        _model = new QSqlRelationalTableModel(this, *_db);
        _model->setTable(DB_T_MAIN);
        _model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
        _model->setRelation(5, QSqlRelation(DB_T_TAGS, DB_F_TAGS_ID, DB_F_TAGS_NAME));
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
Main::_dbReopen(
    cQString &a_filePath
)
{
    _dbClose();
    _dbOpen(a_filePath);

    // _model
    _model->select();
    ui.tvInfo->setModel(_model);
}
//-------------------------------------------------------------------------------------------------
void
Main::_dbClose()
{
    // _model
    {
        bool bRv = _model->submitAll();
        qCHECK_PTR(bRv, _model);

        qPTR_DELETE(_model);
    }

    // _db
    {
        if (_db != Q_NULLPTR) {
            qTEST(_db->isOpen());

            cQString connectionName = _db->connectionName();

            _db->close();
            qTEST(!_db->isOpen());

            qPTR_DELETE(_db);

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
void
Main::_googleSpeech(
    cQString &a_text,
    cQString &a_lang,
    cQString &a_filePath
)
{
    bool bRv = false;

    // request to Google
    {
        cQString              urlStr = "http://translate.google.ru/translate_tts?&q=" +
                                       a_text + "&tl=" + a_lang;
        const QUrl            url(urlStr);
        QNetworkAccessManager manager;
        const QNetworkRequest request(url);

        QNetworkReply *reply = manager.get(request);
        qTEST_PTR(reply);

        for ( ; ; ) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            qCHECK_DO( reply->isFinished(), break);
        }

        // write to audio file
        {
            QFile file(a_filePath);
            bRv = file.open(QIODevice::WriteOnly);
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

            ::Utils::sleep(100);

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
Main::_settingsLoad()
{
    QSize       size;
    QPoint      position;
    int         dictionaryNum   = 0;
    int         tableFontSize   = 0;
    int         tableRowHeight  = 0;
    int         tableCurrentRow = 0;
    int         columnWidth0    = 0;
    int         columnWidth1    = 0;
    int         columnWidth2    = 0;
    int         columnWidth3    = 0;
    int         columnWidth4    = 0;
    int         columnWidth5    = 0;
    ExportOrder exportOrder     = eoUnknown;
    QString     shortcutShowHide;
    QString     shortcutClipboardImport;
    {
        QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

        settings.beginGroup("main");
        size           = settings.value("size",            QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        position       = settings.value("position",        QPoint(200, 200)).toPoint();
        dictionaryNum  = settings.value("dictionary_num",  0).toInt();
        settings.endGroup();

        settings.beginGroup("table");
        tableFontSize   = settings.value("font_size",      APP_FONT_SIZE_DEFAULT).toInt();
        tableRowHeight  = settings.value("row_height",     TABLEVIEW_ROW_HEIGHT).toInt();
        tableCurrentRow = settings.value("current_row",    0).toInt();
        columnWidth0    = settings.value("column_width_0", TVMAIN_COLUMN_WIDTH_0).toInt();
        columnWidth1    = settings.value("column_width_1", TVMAIN_COLUMN_WIDTH_1).toInt();
        columnWidth2    = settings.value("column_width_2", TVMAIN_COLUMN_WIDTH_2).toInt();
        columnWidth3    = settings.value("column_width_3", TVMAIN_COLUMN_WIDTH_3).toInt();
        columnWidth4    = settings.value("column_width_4", TVMAIN_COLUMN_WIDTH_4).toInt();
        columnWidth5    = settings.value("column_width_5", TVMAIN_COLUMN_WIDTH_5).toInt();
        settings.endGroup();

        settings.beginGroup("file");
        exportOrder     = static_cast<ExportOrder>( settings.value("export_order", eoTermValue).toInt() );
        settings.endGroup();

        settings.beginGroup("shortcuts");
        // Sample: Ctrl+Shift+F12
        shortcutShowHide = settings.value("show_hide", "Shift+F1").toString();
        shortcutClipboardImport = settings.value("clipboard_import", "F1").toString();
        settings.endGroup();
    }

    // apply settings
    {
        // main
        resize(size);
        move(position);
        ui.cboDictPath->setCurrentIndex(dictionaryNum);

        // table
        {
            QFont font = ui.tvInfo->font();
            font.setPointSize(tableFontSize);

            ui.tvInfo->setFont(font);
        }
        {
            ui.tvInfo->verticalHeader()->setDefaultSectionSize(tableRowHeight);
        }
        _sqlNavigator.goTo(tableCurrentRow);
        {
            ui.tvInfo->setColumnWidth(0, columnWidth0);
            ui.tvInfo->setColumnWidth(1, columnWidth1);
            ui.tvInfo->setColumnWidth(2, columnWidth2);
            ui.tvInfo->setColumnWidth(3, columnWidth3);
            ui.tvInfo->setColumnWidth(4, columnWidth4);
            ui.tvInfo->setColumnWidth(5, columnWidth5);
        }

        // file
        _exportOrder = exportOrder;

        // shortcuts
        {
            _scShowHide.setShortcut( QKeySequence(shortcutShowHide) );
            _scClipboardImport.setShortcut( QKeySequence(shortcutClipboardImport) );
        }

        ui.cboDictPath->setFocus();
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::_settingsSave()
{
    QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

    // main
    settings.beginGroup("main");
    settings.setValue("position",       pos());
    settings.setValue("size",           size());
    settings.setValue("dictionary_num", ui.cboDictPath->currentIndex());
    settings.endGroup();

    // table
    settings.beginGroup("table");
    settings.setValue("font_size",      ui.tvInfo->font().pointSize());
    settings.setValue("row_height",     ui.tvInfo->verticalHeader()->defaultSectionSize());
    settings.setValue("current_row",    ui.tvInfo->currentIndex().row());
    settings.setValue("column_width_0", ui.tvInfo->columnWidth(0));
    settings.setValue("column_width_1", ui.tvInfo->columnWidth(1));
    settings.setValue("column_width_2", ui.tvInfo->columnWidth(2));
    settings.setValue("column_width_3", ui.tvInfo->columnWidth(3));
    settings.setValue("column_width_4", ui.tvInfo->columnWidth(4));
    settings.setValue("column_width_5", ui.tvInfo->columnWidth(5));
    settings.endGroup();

    // file
    settings.beginGroup("file");
    settings.setValue("export_order",   _exportOrder);
    settings.endGroup();

    // shortcuts
    settings.beginGroup("shortcuts");
    settings.setValue("show_hide",        _scShowHide.shortcut().toString());
    settings.setValue("clipboard_import", _scClipboardImport.shortcut().toString());
    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------
QString
Main::_exportfileNameBuild(
    cQString &a_fileExt
)
{
    qCHECK_RET(_model->rowCount() <= 0, QString());

    cQString tag = _model->record(0).value(DB_F_MAIN_TAG).toString().trimmed();

    QString baseName;
    {
        baseName = QFileInfo( ui.cboDictPath->currentText() ).baseName().trimmed();
        if (!tag.isEmpty()) {
            baseName.append("-");
        }
    }

    QString sRv = baseName + tag + a_fileExt;

    return sRv;
}
//-------------------------------------------------------------------------------------------------
