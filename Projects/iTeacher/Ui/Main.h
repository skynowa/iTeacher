/**
 * \file   Main.h
 * \brief  main widget
 */


#pragma once

#include "../Config.h"

#include <xLib/Core/Core.h>

#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
#include "../QtLib/Db/SqlNavigator.h"
#include "../QtLib/Db/SqlRelationalTableModelEx.h"
#include "../QtLib/GlobalShortcut/GlobalShortcut.h"

#include <QSqlTableModel>
#include <Classes/SqliteDb.h>

#include "ui_Main.h"
//-------------------------------------------------------------------------------------------------
class WordEditor;

class Main :
    public QMainWindow
{
public:
    Main(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Widget);
    ~Main() final;

    Q_DISABLE_COPY(Main)

    QSystemTrayIcon &trayIcon();

public Q_SLOTS:
    void receiveFromOtherApplication(cQStringList &message);

protected:
    bool eventFilter(QObject *object, QEvent *event) final;
    void keyPressEvent(QKeyEvent *event) final;
    void closeEvent(QCloseEvent *event) final;

private:
    enum class ImportExportOrder
    {
        TermValue = 1,
        ValueTerm = 2
    };

    Ui::UiMain      ui;
    WordEditor *    _dlgWordEditorOpened {};
    QSystemTrayIcon _trayIcon;

#if GLOBAL_SHORTCUTS_ALL
    qtlib::GlobalShortcut _scShowHide;
    qtlib::GlobalShortcut _scImportClipboard;
#endif
    qtlib::GlobalShortcut _scQuickClipboardTranslate;

    bool _isHideOnClose {};

    void _initMain();
    void _initActions();
    void _cboDictPath_reload();

    // SQLite DB
    std::unique_ptr<SqliteDb> _sqliteDb;

    // settings
    ImportExportOrder _importExportOrder {ImportExportOrder::TermValue};
    void _settingsLoad();
    void _settingsSave();

    // utils
    QString _exportfileNameBuild(cQString &fileExt);
    bool    _tagsIsEmpty();

    friend class Settings;

private Q_SLOTS:
    // group "File"
    void createDb();
    void quickTranslateClipboard();
    void googleTranslate();
    void deeplTranslate();
    void googleDocs();
    void importCsv();
    void importCsvClipboard();
    void importClipboard();
    void onClipboardChanged();
    void exportCsv();
    void exportPdf();
    void exportClipboard();
    void mute();
    void exit();

    // group "Edit"
    void first();
    void prior();
    void next();
    void last();
    void to();
    void insert();
    void remove();
    void edit();
    void learned();
    void marked();

    // group "Audio"
    void playTerm();
    void playValue();
    void playTermValue();

    // group "Find"
    void search();

    // group "View"
    void showHide();
    void zoomIn();
    void zoomOut();
    void zoomDefault();

    // group "Options"
    void tagsEditor();
    void settings();

    // group "Help"
    void faq();
    void about();

    // tray
    void trayActivated(QSystemTrayIcon::ActivationReason reason);

    // etc
    void cboDictPath_OnCurrentTextChanged(const QString &arg);
};
//-------------------------------------------------------------------------------------------------
