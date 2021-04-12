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

#include "ui_Main.h"
//-------------------------------------------------------------------------------------------------
#define GLOBAL_SHORTCUTS_ALL 0
//-------------------------------------------------------------------------------------------------
class WordEditor;

class Main :
    public QMainWindow
{
public:
             Main(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Widget);
    virtual ~Main();

public Q_SLOTS:
    void receiveFromOtherApplication(cQStringList &message);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    enum class ImportExportOrder
    {
        ieoUnknown   = 0,
        ieoTermValue = 1,
        ieoValueTerm = 2
    };

    Ui::UiMain      ui;
    WordEditor *    _dlgWordEditorOpened {};
    QSystemTrayIcon _trayIcon;

#if GLOBAL_SHORTCUTS_ALL
    qtlib::GlobalShortcut _scShowHide;
    qtlib::GlobalShortcut _scImportClipboard;
#endif
    qtlib::GlobalShortcut _scQuickClipboardTranslate;

    bool            _isHideOnClose {};

    void _initMain();
    void _initModel();
    void _initActions();
    void _cboDictPath_reload();

    // DB
    QSqlDatabase                     *_db {};
    qtlib::SqlRelationalTableModelEx *_model {};
    qtlib::SqlNavigator               _sqlNavigator;

    void _dbOpen(cQString &filePath);
    void _dbClose();
    void _dbReopen(cQString &filePath);

    // settings
    ImportExportOrder _importExportOrder {ImportExportOrder::ieoUnknown};
    void _settingsLoad();
    void _settingsSave();

    // utils
    QString _exportfileNameBuild(cQString &fileExt);
    bool    _tagsIsEmpty();

    Q_OBJECT
    Q_DISABLE_COPY(Main)

    friend class Settings;

private Q_SLOTS:
    // group "File"
    void createDb();
    void quickTranslateClipboard();
    void importCsv();
    void importClipboard();
    void onClipboardChanged();
    void exportCsv();
    void exportPdf();
    void exportClipboard();
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
    void cboDictPath_OnCurrentIndexChanged(const QString &arg);
};
//-------------------------------------------------------------------------------------------------
