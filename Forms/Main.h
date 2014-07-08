/**
 * \file   Main.h
 * \brief  main widget
 */


#ifndef iTeacher_CMainH
#define iTeacher_CMainH
//-------------------------------------------------------------------------------------------------
#include "../Config.h"

#if 0

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#undef KeyPress
#undef KeyRelease
#undef Bool
#undef Status
#undef True
#undef False
#undef GrayScale
#undef Unsorted
#undef None
#undef CursorShape
#undef FocusIn
#undef FocusOut
#undef type
#undef FontChange
#undef Expose

#endif

#if HAVE_XLIB
    #include <xLib/Core/Core.h>
#endif

#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
#include "../QtLib/SqlNavigator.h"

#include "ui_Main.h"
//-------------------------------------------------------------------------------------------------
class Main :
    public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(Main)

public:
                    Main(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);
    virtual        ~Main();

protected:
    virtual bool    eventFilter(QObject *object, QEvent *event);
    virtual void    keyPressEvent(QKeyEvent *event);

private:
    Ui::UiMain      ui;
    QString         _appName;
    QString         _appDir;
    QString         _dbDir;
    QString         _dbBackupDir;
    QString         _tempDir;
    QSystemTrayIcon _trayIcon;
    SqlNavigator    _sqlNavigator;

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
    void            _settingsLoad();
    void            _settingsSave();

    // global hotkey
#if defined(Q_OS_UNIX) && 0
    int          iRv;
    int          _keyCode;
    Display     *_display;
#endif

    // utils
    QString         _exportfileNameBuild(cQString &fileExt);
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_CMainH
