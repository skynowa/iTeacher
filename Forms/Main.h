/**
 * \file   Main.h
 * \brief  main widget
 */


#ifndef iTeacher_CMainH
#define iTeacher_CMainH
//-------------------------------------------------------------------------------------------------
#include "../Config.h"

#if HAVE_XLIB
    #include <xLib/Core/Core.h>
#endif

#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
#include "../QtLib/SqlNavigator.h"
#include "Etc/QxtGlobalShortcut/QxtGlobalShortcut.h"

#include "ui_Main.h"
//-------------------------------------------------------------------------------------------------
class Main :
    public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(Main)

public:
                    Main(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::Widget);
    virtual        ~Main();

protected:
    virtual bool    eventFilter(QObject *object, QEvent *event);
    virtual void    keyPressEvent(QKeyEvent *event);

private:
    enum ExportOrder
    {
        eoUnknown     = 0,
        eoTerminValue = 1,
        eoValueTermin = 2
    };

    Ui::UiMain      ui;
    QString         _appName;
    QString         _appDir;
    QString         _dbDir;
    QString         _dbBackupDir;
    QString         _tempDir;
    QSystemTrayIcon _trayIcon;

    qtlib::SqlNavigator _sqlNavigator;
    QxtGlobalShortcut _shortcut;

    void            _construct();
    void            _destruct();
    void            _initMain();
    void            _initModel();
    void            _initActions();

private Q_SLOTS:
    // group "File"
    void            slot_OnCreateDb();
    void            slot_OnImportCsv();
    void            slot_OnImportClipboard();
    void            slot_OnExportCsv();
    void            slot_OnExportPdf();
    void            slot_OnExit();

    // group "Edit"
    void            slot_OnFirst();
    void            slot_OnPrior();
    void            slot_OnNext();
    void            slot_OnLast();
    void            slot_OnTo();
    void            slot_OnInsert();
    void            slot_OnRemove();
    void            slot_OnEdit();
    void            slot_OnLearned();
    void            slot_OnMarked();

    // group "Audio"
    void            slot_OnPlayWord();
    void            slot_OnPlayTranslation();
    void            slot_OnPlayWordTranslation();

    // group "Find"
    void            slot_OnSearch();

    // group "View"
    void            slot_OnZoomIn();
    void            slot_OnZoomOut();
    void            slot_OnZoomDefault();

    // group "Options"
    void            slot_OnSettings();

    // group "Help"
    void            slot_OnFaq();
    void            slot_OnAbout();

    // tray
    void            slot_OnTrayActivated(QSystemTrayIcon::ActivationReason);

    // etc
    void            slot_cboDictPath_OnCurrentIndexChanged(const QString &arg);

private:
    void            cboDictPath_reload();

    // DB
    QSqlDatabase   *_dbDatabase;
    QSqlTableModel *_model;

    void            dbOpen(cQString &filePath);
    void            dbReopen(cQString &filePath);
    void            dbClose();

    // audio
    static void     _googleSpeech(cQString &text, cQString &lang, cQString &filePath);

    // settings
    ExportOrder     _exportOrder;
    void            _settingsLoad();
    void            _settingsSave();

    // utils
    QString         _exportfileNameBuild(cQString &fileExt);
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_CMainH
