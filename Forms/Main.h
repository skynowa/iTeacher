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
#include "../Classes/GoogleTranslator.h"

#include "ui_Main.h"
//-------------------------------------------------------------------------------------------------
class WordEditor;

class Main :
    public QMainWindow
{
public:
                    Main(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::Widget);
    virtual        ~Main();

public Q_SLOTS:
    void            receiveFromOtherApplication(cQStringList &message);

protected:
    virtual bool    eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;
    virtual void    keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void    closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    enum ImportExportOrder
    {
        ieoUnknown   = 0,
        ieoTermValue = 1,
        ieoValueTerm = 2
    };

    Ui::UiMain      ui;
    WordEditor *    _dlgWordEditorOpened;
    QSystemTrayIcon _trayIcon;

    /// qtlib::GlobalShortcut _scShowHide;
    qtlib::GlobalShortcut _scQuickClipboardTranslate;
    /// qtlib::GlobalShortcut _scImportClipboard;
    bool            _isHideOnCLose;

    void            _construct();
    void            _destruct();
    void            _initMain();
    void            _initModel();
    void            _initActions();
    void            _cboDictPath_reload();

    // DB
    QSqlDatabase   *_db;
    qtlib::SqlRelationalTableModelEx *_model;
    qtlib::SqlNavigator _sqlNavigator;

    void            _dbOpen(cQString &filePath);
    void            _dbClose();
    void            _dbReopen(cQString &filePath);

    // audio
    GoogleTranslator _translator;

    // settings
    ImportExportOrder _importExportOrder;
    void            _settingsLoad();
    void            _settingsSave();

    // utils
    QString         _exportfileNameBuild(cQString &fileExt);
    bool            _tagsIsEmpty();

    Q_OBJECT
    Q_DISABLE_COPY(Main)

    friend class Settings;

private Q_SLOTS:
    // group "File"
    void            createDb();
    void            quickTranslateClipboard();
    void            importCsv();
    void            importClipboard();
    void            exportCsv();
    void            exportPdf();
    void            exportClipboard();
    void            exit();

    // group "Edit"
    void            first();
    void            prior();
    void            next();
    void            last();
    void            to();
    void            insert();
    void            remove();
    void            edit();
    void            learned();
    void            marked();

    // group "Audio"
    void            playTerm();
    void            playValue();
    void            playTermValue();

    // group "Find"
    void            search();

    // group "View"
    void            showHide();
    void            zoomIn();
    void            zoomOut();
    void            zoomDefault();

    // group "Options"
    void            tagsEditor();
    void            settings();

    // group "Help"
    void            faq();
    void            about();

    // tray
    void            trayActivated(QSystemTrayIcon::ActivationReason reason);

    // etc
    void            cboDictPath_OnCurrentIndexChanged(const QString &arg);
};
//-------------------------------------------------------------------------------------------------
