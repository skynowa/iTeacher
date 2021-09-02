/**
 * \file   Main.cpp
 * \brief  main widget
 */


#include "Main.h"

#include "../QtLib/Utils.h"
#include "../QtLib/Application.h"
#include "../Classes/CheckBoxItemDelegate.h"
#include "../Classes/ComboBoxItemDelegate.h"
#include "../Classes/Hint.h"
#include "../Forms/WordEditor.h"
#include "../Forms/WordFinder.h"
#include "../Forms/TagsEditor.h"
#include "../Forms/Settings.h"
#include <QtLib/Ui/About.h>

#include <QPrinter>
#include <QFuture>
#include <QToolTip>

#include <xLib/Core/Const.h>
#include <xLib/Core/String.h>
#include <xLib/Package/Application.h>
#include <xLib/Fs/Path.h>
#include <xLib/Fs/Finder.h>


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
Main::Main(
    QWidget         *a_parent,
    Qt::WindowFlags  a_flags
) :
    QMainWindow               (a_parent, a_flags),
    _trayIcon                 (this),
#if GLOBAL_SHORTCUTS_ALL
    _scShowHide               (this),
    _scImportClipboard        (this),
#endif
    _scQuickClipboardTranslate(this)
{
    _initMain();

    cQString dictPath = qS2QS(xl::package::Application::dbDirPath()) + QDir::separator() +
        ui.cboDictPath->currentText();

    _db.reset(new Db(this, dictPath, ui.tvInfo));
    _db->reopen();

    _initActions();
    _settingsLoad();
}
//-------------------------------------------------------------------------------------------------
QSystemTrayIcon &
Main::trayIcon()
{
    return _trayIcon;
}
//-------------------------------------------------------------------------------------------------
/* static */
bool
Main::isTerminExists(
    cQSqlTableModel &a_model,
    cQString        &a_term
)
{
    qTEST_NA(a_model);
    qTEST(!a_term.isEmpty())

    bool      bRv {};
    QSqlQuery qryQuery( a_model.database() );

    cQString sql =
        "SELECT COUNT(1) "
        "FROM  " + a_model.tableName() + " "
        "WHERE " DB_F_MAIN_TERM " LIKE :term";

    qryQuery.prepare(sql);
    qryQuery.bindValue(":term", a_term.trimmed());

    bRv = qryQuery.exec();
    qCHECK_REF(bRv, qryQuery);

    bRv = qryQuery.next();
    qCHECK_REF(bRv, qryQuery);

    bRv = qryQuery.value(0).toBool();

    return bRv;
}
//-------------------------------------------------------------------------------------------------
/*virtual*/
Main::~Main()
{
    _settingsSave();
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
                        zoomIn();
                    } else {
                        zoomOut();
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
    if (_isHideOnClose) {
        a_event->ignore();
        showHide();
    } else {
        QApplication::setQuitOnLastWindowClosed(true);
        a_event->accept();
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

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
            _appDir  = qS2QS(xl::package::Application::dirPath());
        #endif
    #endif
    }

    // Main
    {
        setWindowTitle( qS2QS(xl::package::Application::info().name) );
        setGeometry(0, 0, APP_WIDTH, APP_HEIGHT);
        qtlib::Utils::widgetAlignCenter(this);
        _cboDictPath_reload();
    }

    // tray icon
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << qTRACE_VAR(QSystemTrayIcon::isSystemTrayAvailable());
    } else {
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
        _trayIcon.setIcon( windowIcon() );
        _trayIcon.setToolTip( qS2QS(xl::package::Application::info().name) );
        _trayIcon.setContextMenu(mnuTrayIcon);
        _trayIcon.show();
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::_initActions()
{
    // group "File"
    {
        connect(ui.actFile_CreateDb,        &QAction::triggered,
                this,                       &Main::createDb);

        connect(ui.actFile_QuickTranslateClipboard, &QAction::triggered,
                this,                       &Main::quickTranslateClipboard);

        connect(ui.actFile_ImportCsv,       &QAction::triggered,
                this,                       &Main::importCsv);

        connect(ui.actFile_ImportCsvClipboard, &QAction::triggered,
                this,                       &Main::importCsvClipboard);

        connect(ui.actFile_ImportClipboard, &QAction::triggered,
                this,                       &Main::importClipboard);

        connect(ui.actFile_ExportCsv,       &QAction::triggered,
                this,                       &Main::exportCsv);

        connect(ui.actFile_ExportPdf,       &QAction::triggered,
                this,                       &Main::exportPdf);

        connect(ui.actFile_ExportClipboard, &QAction::triggered,
                this,                       &Main::exportClipboard);

        connect(ui.actFile_Mute,            &QAction::triggered,
                this,                       &Main::mute);

        connect(ui.actFile_Exit,            &QAction::triggered,
                this,                       &Main::exit);
    }

    // group "Edit"
    {
        connect(ui.actEdit_First,           &QAction::triggered,
                this,                       &Main::first);

        connect(ui.actEdit_Prior,           &QAction::triggered,
                this,                       &Main::prior);

        connect(ui.actEdit_Next,            &QAction::triggered,
                this,                       &Main::next);

        connect(ui.actEdit_Last,            &QAction::triggered,
                this,                       &Main::last);

        connect(ui.actEdit_To,              &QAction::triggered,
                this,                       &Main::to);

        connect(ui.actEdit_Insert,          &QAction::triggered,
                this,                       &Main::insert);

        connect(ui.actEdit_Delete,          &QAction::triggered,
                this,                       &Main::remove);

        connect(ui.actEdit_Edit,            &QAction::triggered,
                this,                       &Main::edit);

        connect(ui.actEdit_Learned,         &QAction::triggered,
                this,                       &Main::learned);

        connect(ui.actEdit_Marked,          &QAction::triggered,
                this,                       &Main::marked);
    }

    // audio
    {
        connect(ui.actEdit_PlayTerm,        &QAction::triggered,
                this,                       &Main::playTerm);

        connect(ui.actEdit_PlayValue,       &QAction::triggered,
                this,                       &Main::playValue);

        connect(ui.actEdit_PlayTermValue,   &QAction::triggered,
                this,                       &Main::playTermValue);
    }

    // group "Find"
    {
        connect(ui.actFind_Search,          &QAction::triggered,
                this,                       &Main::search);
    }

    // group "View"
    {
        connect(ui.actView_ShowHide,        &QAction::triggered,
                this,                       &Main::showHide);

        connect(ui.actView_ZoomIn,          &QAction::triggered,
                this,                       &Main::zoomIn);

        connect(ui.actView_ZoomOut,         &QAction::triggered,
                this,                       &Main::zoomOut);

        connect(ui.actView_ZoomDefault,     &QAction::triggered,
                this,                       &Main::zoomDefault);
    }

    // group "Options"
    {
        connect(ui.actOptions_TagsEditor,   &QAction::triggered,
                this,                       &Main::tagsEditor);

        connect(ui.actOptions_Settings,     &QAction::triggered,
                this,                       &Main::settings);
    }

    // group "Help"
    {
        connect(ui.actHelp_Faq,             &QAction::triggered,
                this,                       &Main::faq);

        connect(ui.actHelp_About,           &QAction::triggered,
                this,                       &Main::about);
    }

    // tvInfo
    {
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
        // ui.tvInfo->setItemDelegateForColumn(5, new ComboBoxItemDelegate(ui.tvInfo, _db->model()));

        connect(ui.tvInfo, &QTableView::doubleClicked,
                this,      &Main::edit);

        ui.tvInfo->show();
    }

    // slots
    {
        connect(ui.cboDictPath, qOverload<const QString &>(&QComboBox::currentIndexChanged),
                this,           &Main::cboDictPath_OnCurrentIndexChanged);
    }

    // tray
    {
        connect(&_trayIcon, &QSystemTrayIcon::activated,
                this,       &Main::trayActivated);
        mute();
    }

    // global shortcut
    {
    #if GLOBAL_SHORTCUTS_ALL
        connect(&_scShowHide,                &qtlib::GlobalShortcut::sig_activated,
                this,                        &Main::showHide);

        connect(&_scImportClipboard,         &qtlib::GlobalShortcut::sig_activated,
                this,                        &Main::importClipboard);
    #endif

        connect(&_scQuickClipboardTranslate, &qtlib::GlobalShortcut::sig_activated,
                this,                        &Main::quickTranslateClipboard);
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "File"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::createDb()
{
    cQString dbName = QInputDialog::getText(this, qS2QS(xl::package::Application::info().name),
        tr("New DB file path:"), QLineEdit::Normal, DB_FILE_EXT);
    qCHECK_DO(dbName.trimmed().isEmpty(), return);

    cQString dictPath = qS2QS(xl::package::Application::dbDirPath()) + QDir::separator() + dbName;

    // reopen DB
    {
        _db.reset(new Db(this, dictPath, ui.tvInfo));
        _db->reopen();

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
Main::quickTranslateClipboard()
{
    auto hint = Hint::toolTip( *_db->model() );
    hint.show();
}
//-------------------------------------------------------------------------------------------------
void
Main::importCsv()
{
    qCHECK_DO(_tagsIsEmpty(), return);

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
    QString infoMsg;
    _db->model()->importCsv(filePath, fieldNames, CSV_SEPARATOR, true, &infoMsg);

    // "fire" cboDictPath
    {
        cint currentIndex = ui.cboDictPath->currentIndex();
        ui.cboDictPath->setCurrentIndex(- 1);
        ui.cboDictPath->setCurrentIndex(currentIndex);
    }

    // report
    {
        cQString msg = QString(tr("File: %1\n\n%2\n\nImport CSV finished."))
                            .arg(filePath, infoMsg);

        QMessageBox::information(this, qS2QS(xl::package::Application::info().name), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::importCsvClipboard()
{
    qCHECK_DO(_tagsIsEmpty(), return);

    // DB field names
    QVector<QString> fieldNames;
    fieldNames.push_back(DB_F_MAIN_TERM);
    fieldNames.push_back(DB_F_MAIN_VALUE);
    fieldNames.push_back(DB_F_MAIN_IS_LEARNED);
    fieldNames.push_back(DB_F_MAIN_IS_MARKED);
    fieldNames.push_back(DB_F_MAIN_TAG);

    // import
    QString infoMsg;
    _db->model()->importCsvClipboard(fieldNames, CSV_SEPARATOR, true, &infoMsg);

    // "fire" cboDictPath
    {
        cint currentIndex = ui.cboDictPath->currentIndex();
        ui.cboDictPath->setCurrentIndex(- 1);
        ui.cboDictPath->setCurrentIndex(currentIndex);
    }

    // report
    {
        cQString msg = QString(tr("%1\n\nImport CSV clipboard finished."))
                            .arg(infoMsg);

        QMessageBox::information(this, qS2QS(xl::package::Application::info().name), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::importClipboard()
{
    qCHECK_DO(_tagsIsEmpty(), return);

    bool bRv = false;

    WordEditor::retranslate(_dlgWordEditorOpened);

    // WordEditor - only one instance
    QSharedMemory locker;
    {
        cQString dlgGuid = qS2QS(xl::package::Application::info().name) + "_dlgWordEditor_guid";

        locker.setKey(dlgGuid);

        bRv = locker.attach();
        qCHECK_DO(bRv, return);

        bRv = locker.create(1);
        qCHECK_DO(!bRv, return);
    }

    // insert data
    {
        qCHECK_DO(!_db->navigator().isValid(), return);

        _db->navigator().insert();

        cbool     insertMode {true};
        cQString &data       = QApplication::clipboard()->text();

        WordEditor dlgWordEditor(this, _db->model(), &_db->navigator(), insertMode, data);

        _dlgWordEditorOpened = &dlgWordEditor;
        auto cleanup = xl::core::ScopeExit(
            [&]
            {
                _dlgWordEditorOpened = nullptr;
            });

        bRv = dlgWordEditor.isConstructed();
        qCHECK_DO(!bRv, return);

        QDialog::DialogCode code = static_cast<QDialog::DialogCode>( dlgWordEditor.exec() );
        switch (code) {
        case QDialog::Rejected:
            _db->navigator().remove();
            break;
        default:
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------
// TODO: Main::onClipboardChanged() - unused
void
Main::onClipboardChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard == nullptr) {
        qDebug() << __FUNCTION__ << "clipboard - return";
        return;
    }

    const auto mode = clipboard->supportsSelection() ?
        QClipboard::Mode::Selection : QClipboard::Mode::Clipboard;
    qDebug() << qTRACE_VAR(mode);

    const QMimeData *mime = clipboard->mimeData(mode);
    if (mime == nullptr) {
        qDebug() << __FUNCTION__ << "mime - return";
        return;
    }

    if ( !mime->hasText() ) {
        qDebug() << __FUNCTION__ << "hasText - return";
        return;
    }

    qDebug() << __FUNCTION__ << ": OK";

    // TODO: view text - impl
    this->setWindowTitle( mime->text() );
}
//-------------------------------------------------------------------------------------------------
void
Main::exportCsv()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(this, tr("Save file"),
        _exportfileNameBuild(".csv"), tr("CSV document (*.csv)"));
    qCHECK_DO(filePath.isEmpty(), return);

    // export
    {
        // DB field names
        QVector<QString> fieldNames;

        switch (_importExportOrder) {
        case ImportExportOrder::TermValue:
        default:
            fieldNames.push_back(DB_F_MAIN_TERM);
            fieldNames.push_back(DB_F_MAIN_VALUE);
            break;
        case ImportExportOrder::ValueTerm:
            fieldNames.push_back(DB_F_MAIN_VALUE);
            fieldNames.push_back(DB_F_MAIN_TERM);
            break;
        }

        fieldNames.push_back(DB_F_MAIN_IS_LEARNED);
        fieldNames.push_back(DB_F_MAIN_IS_MARKED);
        fieldNames.push_back(DB_F_MAIN_TAG);

        // import
        _db->model()->exportCsv(filePath, fieldNames, CSV_SEPARATOR, true);
    }

    // report
    {
        cQString msg = QString(tr("File: %1\nExport CSV finished."))
                            .arg(filePath);

        QMessageBox::information(this, qS2QS(xl::package::Application::info().name), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::exportPdf()
{
    // choose file path
    cQString filePath = QFileDialog::getSaveFileName(this, tr("Save file"),
        _exportfileNameBuild(".pdf"), tr("PDF Document (*.pdf)"));
    qCHECK_DO(filePath.isEmpty(), return);

    // DB -> text
    QString html;


    // file -> DB
    cint realRowCount = _db->model()->realRowCount();

    for (int i = 0; i < realRowCount; ++ i) {
        switch (_importExportOrder) {
        case ImportExportOrder::TermValue:
        default:
            html.push_back( _db->model()->record(i).value(DB_F_MAIN_TERM).toString() );
            html.push_back("\n - ");
            html.push_back( _db->model()->record(i).value(DB_F_MAIN_VALUE).toString() );
            html.push_back("<br>");
            break;
        case ImportExportOrder::ValueTerm:
            html.push_back( _db->model()->record(i).value(DB_F_MAIN_VALUE).toString() );
            html.push_back("\n - ");
            html.push_back( _db->model()->record(i).value(DB_F_MAIN_TERM).toString() );
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

        QMessageBox::information(this, qS2QS(xl::package::Application::info().name), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::exportClipboard()
{
    QString         sRv;
    QModelIndexList indexes = _db->view()->selectionModel()->selectedRows();

    for (QModelIndex &index : indexes) {
        _db->view()->setFocus();

        cint targetRow = index.row();

        sRv += _db->model()->record(targetRow).value(DB_F_MAIN_ID).toString() + CSV_SEPARATOR;
        sRv += _db->model()->record(targetRow).value(DB_F_MAIN_TERM).toString() + CSV_SEPARATOR;
        sRv += _db->model()->record(targetRow).value(DB_F_MAIN_VALUE).toString() + CSV_SEPARATOR;
        sRv += _db->model()->record(targetRow).value(DB_F_MAIN_IS_LEARNED).toString() + CSV_SEPARATOR;
        sRv += _db->model()->record(targetRow).value(DB_F_MAIN_IS_MARKED).toString() + CSV_SEPARATOR;
        sRv += _db->model()->record(targetRow).value(DB_F_MAIN_TAG).toString();
        sRv += "\n";
    }

    QApplication::clipboard()->setText(sRv);

    // report
    {
        cQString msg = QString(tr("Text: \n%1\nExport Clipboard finished."))
                            .arg(sRv);

        QMessageBox::information(this, qS2QS(xl::package::Application::info().name), msg);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::mute()
{
    static bool isMute {true};
    isMute = !isMute;

    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard == nullptr) {
        qWarning() << qTRACE_VAR(clipboard);
        return;
    }

    auto *sender       {clipboard};
    auto  senderSignal {&QClipboard::dataChanged};
    auto *reciever     {this};
    auto  recieverSlot {&Main::quickTranslateClipboard};

    if (isMute) {
        disconnect(sender,   senderSignal,
                   reciever, recieverSlot);

        // tray icon - off
        {
            QPixmap pixmapOff = QIcon( windowIcon() )
                    .pixmap(QSYSTEM_TRAYICON_SIZE, QIcon::Mode::Disabled, QIcon::State::Off);
            QIcon iconOff(pixmapOff);
            _trayIcon.setIcon(iconOff);
        }
    } else {
        connect(sender,   senderSignal,
                reciever, recieverSlot);

        // tray icon - on
        {
            QIcon iconOn( windowIcon() );
            _trayIcon.setIcon(iconOn);
        }
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::exit()
{
    qApp->exit();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Edit"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::first()
{
    _db->navigator().first();
}
//-------------------------------------------------------------------------------------------------
void
Main::prior()
{
    _db->navigator().prior();
}
//-------------------------------------------------------------------------------------------------
void
Main::next()
{
    _db->navigator().next();
}
//-------------------------------------------------------------------------------------------------
void
Main::last()
{
    _db->navigator().last();
}
//-------------------------------------------------------------------------------------------------
void
Main::to()
{
    qCHECK_DO(_db->view()->currentIndex().row() < 0, return);

    cint currentRow = _db->view()->currentIndex().row() + 1;
    cint minValue   = 1;
    cint maxValue   = _db->model()->realRowCount();

    cint targetRow  = QInputDialog::getInt(this, qS2QS(xl::package::Application::info().name),
        tr("Go to row:"), currentRow, minValue, maxValue) - 1;

    _db->navigator().goTo(targetRow);
}
//-------------------------------------------------------------------------------------------------
void
Main::insert()
{
    qCHECK_DO(_tagsIsEmpty(),            return);
    qCHECK_DO(!_db->navigator().isValid(), return);

    _db->navigator().insert();

    WordEditor dlgWordEditor(this, _db->model(), &_db->navigator(), true);

    bool bRv = dlgWordEditor.isConstructed();
    qCHECK_DO(!bRv, return);

    QDialog::DialogCode code = static_cast<QDialog::DialogCode>( dlgWordEditor.exec() );
    switch (code) {
    case QDialog::Rejected:
        _db->navigator().remove();
        break;
    default:
        break;
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::remove()
{
    qCHECK_DO(_db->view()->currentIndex().row() < 0, return);

    QString text;
    QString informativeText;
    {
        cint       currentRow = _db->view()->currentIndex().row();
        QSqlRecord record     = _db->model()->record(currentRow);

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
        _db->navigator().remove();
        break;
    case QMessageBox::Cancel:
    default:
        break;
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::edit()
{
    qCHECK_DO(_db->view()->currentIndex().row() < 0, return);

    // show edit dialog
    WordEditor dlgWordEditor(this, _db->model(), &_db->navigator(), false);

    bool bRv = dlgWordEditor.isConstructed();
    qCHECK_DO(!bRv, return);

    (int)dlgWordEditor.exec();
}
//-------------------------------------------------------------------------------------------------
void
Main::learned()
{
    qCHECK_DO(_db->view()->currentIndex().row() < 0, return);

    bool bRv = false;

    cint currentRow = _db->view()->currentIndex().row();

    QSqlRecord record = _db->model()->record(currentRow);
    record.setValue(DB_F_MAIN_IS_LEARNED, !record.value(DB_F_MAIN_IS_LEARNED).toBool() );

    bRv = _db->model()->setRecord(currentRow, record);
    qCHECK_PTR(bRv, _db->model());

    bRv = _db->model()->submitAll();
    qTEST(bRv);

    _db->navigator().goTo(currentRow);
}
//-------------------------------------------------------------------------------------------------
void
Main::marked()
{
    qCHECK_DO(_db->view()->currentIndex().row() < 0, return);

    bool bRv {};

    cint currentRow = _db->view()->currentIndex().row();

    QSqlRecord record = _db->model()->record(currentRow);
    record.setValue(DB_F_MAIN_IS_MARKED,  !record.value(DB_F_MAIN_IS_MARKED).toBool() );

    bRv = _db->model()->setRecord(currentRow, record);
    qCHECK_PTR(bRv, _db->model());

    bRv = _db->model()->submitAll();
    qTEST(bRv);

    _db->navigator().goTo(currentRow);
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Audio"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::playTerm()
{
    qCHECK_DO(_db->view()->currentIndex().row() < 0, return);

    QString text;
    {
        cint       currentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord record     = _db->model()->record(currentRow);

        text = record.value(DB_F_MAIN_TERM).toString();
    }

    QString audioFilePath;
    {
        audioFilePath = qS2QS(xl::package::Application::tempDirPath()) + QDir::separator() +
            AUDIO_TERM_FILE_NAME;
    }

    // TODO: playTerm - impl
#if OPTION_QSOUND
    qTRACE_FUNC;

    /// _translator.speech(text, LANG_EN, audioFilePath);

    audioFilePath = "/home/skynowa/Dropbox/Music/Bomb_Has_Been_Planted_CSGO.wav";
    qTEST(QFile::exists(audioFilePath));
    qTEST(QFileInfo(audioFilePath).completeSuffix() == "wav");

    QSound sound(audioFilePath);
    sound.play();
#else
    Q_UNUSED(text);
    Q_UNUSED(audioFilePath);

    qMSG_NOT_IMPL;
#endif
}
//-------------------------------------------------------------------------------------------------
void
Main::playValue()
{
    qCHECK_DO(_db->view()->currentIndex().row() < 0, return);

    QString text;
    {
        cint       currentRow = ui.tvInfo->currentIndex().row();
        QSqlRecord record     = _db->model()->record(currentRow);

        text = record.value(DB_F_MAIN_VALUE).toString();
    }

    QString audioFilePath;
    {
        audioFilePath = qS2QS(xl::package::Application::tempDirPath()) + QDir::separator() +
            AUDIO_VALUE_FILE_NAME;
    }

    // TODO: playValue - impl
#if OPTION_QSOUND
    qTRACE_FUNC;

    /// _translator.speech(text, LANG_RU, audioFilePath);
#else
    Q_UNUSED(text);
    Q_UNUSED(audioFilePath);

    qMSG_NOT_IMPL;
#endif
}
//-------------------------------------------------------------------------------------------------
void
Main::playTermValue()
{
    playTerm();
    playValue();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Find"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::search()
{
    qCHECK_DO(!_db->navigator().isValid(), return);

    WordFinder dlgWordFinder(this, _db->model());
    dlgWordFinder.exec();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "View"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::showHide()
{
    setVisible( !isVisible() );
}
//-------------------------------------------------------------------------------------------------
void
Main::zoomIn()
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
Main::zoomOut()
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
Main::zoomDefault()
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
Main::tagsEditor()
{
    TagsEditor dlgTagsEditor(this, *_db->db());
    (int)dlgTagsEditor.exec();

    // refresh table
    {
        cint currentRow = _db->view()->currentIndex().row();
        _db->model()->select();
        _db->navigator().goTo(currentRow);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::settings()
{
    Settings dlgSettings(this, this);
    (int)dlgSettings.exec();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Help"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::faq()
{
    // TODO_VER: Faq
}
//-------------------------------------------------------------------------------------------------
void
Main::about()
{
    AboutData aboutData;
    aboutData.appName              = qS2QS(xl::package::Application::info().name);
    aboutData.appVersionFull       = qS2QS(xl::package::Application::info().versionFull());
    aboutData.appDecription        = qS2QS(xl::package::Application::info().decription);
    aboutData.appCopyrightYears    = qS2QS(xl::package::Application::info().copyrightYears);
    aboutData.appUsage             = qS2QS(xl::package::Application::info().usage);
    aboutData.appHelp              = qS2QS(xl::package::Application::info().help);
    aboutData.appVersionMajor      = qS2QS(xl::package::Application::info().versionMajor);
    aboutData.appVersionMinor      = qS2QS(xl::package::Application::info().versionMinor);
    aboutData.appVersionPatch      = qS2QS(xl::package::Application::info().versionPatch);
    aboutData.appVersionType       = qS2QS(xl::package::Application::info().versionType);
    aboutData.appVersionRevision   = qS2QS(xl::package::Application::info().versionRevision);
    aboutData.appVendorName        = qS2QS(xl::package::Application::info().vendorName);
    aboutData.appVendorDomain      = qS2QS(xl::package::Application::info().vendorDomain);
    aboutData.appVendorAuthor      = qS2QS(xl::package::Application::info().vendorAuthor);
    aboutData.appVendorUrl         = qS2QS(xl::package::Application::info().vendorUrl);
    aboutData.appVendorEmail       = qS2QS(xl::package::Application::info().vendorEmail);
    aboutData.appVendorSkype       = qS2QS(xl::package::Application::info().vendorSkype);
    aboutData.appVendorJabber      = qS2QS(xl::package::Application::info().vendorJabber);
    aboutData.appVendorIcq         = qS2QS(xl::package::Application::info().vendorIcq);
    aboutData.appDonatePayPal      = APP_DONATE_PAYPAL;
    aboutData.appDonateWebMoney    = APP_DONATE_WEBMONEY;
    aboutData.appDonateYandexMoney = APP_DONATE_YANDEXMONEY;
    aboutData.appDonatePrivate24   = APP_DONATE_PRIVATE24;

    About dlgAbout(this, aboutData, windowIcon());
    (int)dlgAbout.exec();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   tray
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::trayActivated(
    QSystemTrayIcon::ActivationReason a_reason
)
{
    switch (a_reason) {
    case QSystemTrayIcon::DoubleClick:
        showHide();
        break;
    case QSystemTrayIcon::Trigger:
        mute();
        break;
    case QSystemTrayIcon::MiddleClick:
        importClipboard();
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
Main::cboDictPath_OnCurrentIndexChanged(
    const QString &a_arg
)
{
    qTRACE_SCOPE_FUNC;

    qCHECK_DO(a_arg.isEmpty(), return);

    // reopen DB
    {
        cQString dictPath = qS2QS(xl::package::Application::dbDirPath()) + QDir::separator() + a_arg;

        _db.reset(new Db(this, dictPath, ui.tvInfo));
        _db->reopen();
    }

    // words info
    {
        cint wordsAll        = _db->wordsAll();
        cint wordsLearned    = _db->wordsLearned();
        cint wordsNotLearned = _db->wordsNotLearned();

        cQString dictInfo = QString(
            tr("&nbsp;&nbsp;&nbsp;<b>All</b>: %1 (%2)"
            "&nbsp;&nbsp;&nbsp;<b>Learned</b>: %3 (%4)"
            "&nbsp;&nbsp;&nbsp;<b>Not learned:</b> %5 (%6)"))
            .arg( wordsAll )
            .arg( qS2QS(xl::core::String::formatPercentage(wordsAll, wordsAll)) )
            .arg( wordsLearned )
            .arg( qS2QS(xl::core::String::formatPercentage(wordsAll, wordsLearned)) )
            .arg( wordsNotLearned )
            .arg( qS2QS(xl::core::String::formatPercentage(wordsAll, wordsNotLearned)) );

        ui.lblDictInfo->setText(dictInfo);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::receiveFromOtherApplication(
    cQStringList &a_message
)
{
    // QMessageBox::information(this, "Hey", "Some Apps send:\n" + a_message.join("\n"));

    setVisible(true);
    raise();
    activateWindow();
}
//-------------------------------------------------------------------------------------------------
void
Main::_cboDictPath_reload()
{
    ui.cboDictPath->clear();

    qCHECK_DO(! QDir( qS2QS(xl::package::Application::dbDirPath()) ).exists(), return);

    std::vec_tstring_t dicts;
    xl::fs::Finder::files(xl::package::Application::dbDirPath(), xT("*") xT(DB_FILE_EXT), true, &dicts);

    for (auto &it_dict : dicts) {
        cQString dict = qS2QS( it_dict.erase(0, (xl::package::Application::dbDirPath() +
            xl::core::Const::slash()).size()) );

        ui.cboDictPath->addItem(dict);
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

/**************************************************************************************************
*   settings
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::_settingsLoad()
{
    // load settings from INI
    QSize       size;
    QPoint      position;
    int         dictionaryNum   = 0;
    bool        visible         = true;

    int         tableFontSize   = 0;
    int         tableRowHeight  = 0;
    int         tableCurrentRow = 0;
    int         columnWidth0    = 0;
    int         columnWidth1    = 0;
    int         columnWidth2    = 0;
    int         columnWidth3    = 0;
    int         columnWidth4    = 0;
    int         columnWidth5    = 0;
    ImportExportOrder importExportOrder {ImportExportOrder::Unknown};
    bool        isHideOnCLose   = false;
    QString     shortcutShowHide;
    QString     shortcutQuickClipboardTranslate;
    QString     shortcutClipboardImport;
    {
        QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

        settings.beginGroup(CFG_GROUP_MAIN);
        size           = settings.value(CFG_SIZE,            QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        position       = settings.value(CFG_POSITION,        QPoint(200, 200)).toPoint();
        visible        = settings.value(CFG_VISIBLE,         true).toBool();
        dictionaryNum  = settings.value(CFG_DICTIONARY_NUM,  0).toInt();
        settings.endGroup();

        settings.beginGroup(CFG_GROUP_TABLE);
        tableFontSize   = settings.value(CFG_FONT_SIZE,      APP_FONT_SIZE_DEFAULT).toInt();
        tableRowHeight  = settings.value(CFG_ROW_HEIGHT,     TABLEVIEW_ROW_HEIGHT).toInt();
        tableCurrentRow = settings.value(CFG_CURRENT_ROW,    0).toInt();
        columnWidth0    = settings.value(CFG_COLUMN_WIDTH_0, TVMAIN_COLUMN_WIDTH_0).toInt();
        columnWidth1    = settings.value(CFG_COLUMN_WIDTH_1, TVMAIN_COLUMN_WIDTH_1).toInt();
        columnWidth2    = settings.value(CFG_COLUMN_WIDTH_2, TVMAIN_COLUMN_WIDTH_2).toInt();
        columnWidth3    = settings.value(CFG_COLUMN_WIDTH_3, TVMAIN_COLUMN_WIDTH_3).toInt();
        columnWidth4    = settings.value(CFG_COLUMN_WIDTH_4, TVMAIN_COLUMN_WIDTH_4).toInt();
        columnWidth5    = settings.value(CFG_COLUMN_WIDTH_5, TVMAIN_COLUMN_WIDTH_5).toInt();
        settings.endGroup();

        settings.beginGroup(CFG_GROUP_FILE);
        importExportOrder = static_cast<ImportExportOrder>( settings.value(CFG_IMPORT_EXPORT_ORDER, static_cast<int>(ImportExportOrder::TermValue)).toInt() );
        isHideOnCLose     = settings.value(CFG_HIDE_ON_CLOSE, false).toBool();
        settings.endGroup();

        settings.beginGroup(CFG_GROUP_SHORTCUTS);
        // Sample: Ctrl+Shift+F12
        shortcutShowHide                = settings.value(CFG_SHOW_HIDE, "Ctrl+F1").toString();
        shortcutQuickClipboardTranslate = settings.value(CFG_QUICK_CLIPBOARD_TRANSLATE, "F1").toString();
        shortcutClipboardImport         = settings.value(CFG_CLIPBOARD_IMPORT, "Shift+F1").toString();
        settings.endGroup();
    }

    // apply settings
    {
        // main
        resize(size);
        move(position);
        ui.cboDictPath->setCurrentIndex(dictionaryNum);
        setVisible(visible);

        // table
        {
            QFont font = ui.tvInfo->font();
            font.setPointSize(tableFontSize);

            ui.tvInfo->setFont(font);
        }

        {
            ui.tvInfo->verticalHeader()->setDefaultSectionSize(tableRowHeight);
        }

        _db->navigator().goTo(tableCurrentRow);

        {
            ui.tvInfo->setColumnWidth(0, columnWidth0);
            ui.tvInfo->setColumnWidth(1, columnWidth1);
            ui.tvInfo->setColumnWidth(2, columnWidth2);
            ui.tvInfo->setColumnWidth(3, columnWidth3);
            ui.tvInfo->setColumnWidth(4, columnWidth4);
            ui.tvInfo->setColumnWidth(5, columnWidth5);
        }

        // file
        _importExportOrder = importExportOrder;
        _isHideOnClose     = isHideOnCLose;

        // shortcuts
        {
        #if GLOBAL_SHORTCUTS_ALL
            _scShowHide.set( QKeySequence(shortcutShowHide) );
            _scImportClipboard.set( QKeySequence(shortcutClipboardImport) );
        #endif
            _scQuickClipboardTranslate.set( QKeySequence(shortcutQuickClipboardTranslate) );
        }

        ui.cboDictPath->setFocus();
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::_settingsSave()
{
    QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

    // main
    settings.beginGroup(CFG_GROUP_MAIN);
    settings.setValue(CFG_SIZE,           size());
    settings.setValue(CFG_POSITION,       pos());
    settings.setValue(CFG_VISIBLE,        isVisible());
    settings.setValue(CFG_DICTIONARY_NUM, ui.cboDictPath->currentIndex());
    settings.endGroup();

    // table
    settings.beginGroup(CFG_GROUP_TABLE);
    settings.setValue(CFG_FONT_SIZE,      ui.tvInfo->font().pointSize());
    settings.setValue(CFG_ROW_HEIGHT,     ui.tvInfo->verticalHeader()->defaultSectionSize());
    settings.setValue(CFG_CURRENT_ROW,    ui.tvInfo->currentIndex().row());
    settings.setValue(CFG_COLUMN_WIDTH_0, ui.tvInfo->columnWidth(0));
    settings.setValue(CFG_COLUMN_WIDTH_1, ui.tvInfo->columnWidth(1));
    settings.setValue(CFG_COLUMN_WIDTH_2, ui.tvInfo->columnWidth(2));
    settings.setValue(CFG_COLUMN_WIDTH_3, ui.tvInfo->columnWidth(3));
    settings.setValue(CFG_COLUMN_WIDTH_4, ui.tvInfo->columnWidth(4));
    settings.setValue(CFG_COLUMN_WIDTH_5, ui.tvInfo->columnWidth(5));
    settings.endGroup();

    // file
    settings.beginGroup(CFG_GROUP_FILE);
    settings.setValue(CFG_IMPORT_EXPORT_ORDER, static_cast<int>(_importExportOrder));
    settings.setValue(CFG_HIDE_ON_CLOSE,       _isHideOnClose);
    settings.endGroup();

    // shortcuts
    settings.beginGroup(CFG_GROUP_SHORTCUTS);
#if GLOBAL_SHORTCUTS_ALL
    settings.setValue(CFG_SHOW_HIDE,                 _scShowHide.get().toString());
    settings.setValue(CFG_CLIPBOARD_IMPORT,          _scImportClipboard.get().toString());
#endif
    settings.setValue(CFG_QUICK_CLIPBOARD_TRANSLATE, _scQuickClipboardTranslate.get().toString());
    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------
QString
Main::_exportfileNameBuild(
    cQString &a_fileExt
)
{
    qCHECK_RET(_db->model()->rowCount() <= 0, QString());

    cQString tag = _db->model()->record(0).value(DB_F_MAIN_TAG).toString().trimmed();

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
bool
Main::_tagsIsEmpty()
{
    qCHECK_RET(!_db->tagsIsEmpty(), false);

    // report
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle( qS2QS(xl::package::Application::info().name) );
    msgBox.setText(tr("DB table: " DB_T_TAGS " is empty.\nAdd tags to Tags Editor?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int iRv = msgBox.exec();
    if (iRv == QMessageBox::Yes){
        tagsEditor();
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
