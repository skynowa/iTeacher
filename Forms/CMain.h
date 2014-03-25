/**
 * \file   CMain.h
 * \brief  main widget
 */


#ifndef iTeacher_CMainH
#define iTeacher_CMainH
//------------------------------------------------------------------------------
#include "../Config.h"

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

#include <xLib/Core/xCore.h>
#include "../QtLib/Common.h"
#include "../QtLib/CUtils.h"
#include "../QtLib/CSqlNavigator.h"

#include "ui_CMain.h"
//------------------------------------------------------------------------------
class CMain :
    public QMainWindow
{
        Q_OBJECT

    public:
                        CMain(QWidget *parent = 0, Qt::WindowFlags flags = 0);
        virtual        ~CMain();

    protected:
        virtual bool    eventFilter(QObject *object, QEvent *event);
        virtual void    keyPressEvent(QKeyEvent *event);

    private:
        Ui::CMainClass  ui;
        QString         _sAppName;
        QString         _sAppDir;
        QString         _sDbDir;
        QString         _sDbBackupDir;
        QString         _sTempDir;
        QSystemTrayIcon _trayIcon;
        CSqlNavigator   _snSqlNavigator;

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

        void            slot_cboDictPath_OnCurrentIndexChanged(
                           const QString &arg);

    private:
        void            cboDictPath_reload();

        // DB
        QSqlDatabase   *_dbDatabase;
        QSqlTableModel *_tmModel;

        void            dbOpen(cQString &filePath);
        void            dbReopen(cQString &filePath);
        void            dbClose();

        // audio
        static void     _googleSpeech(cQString &text, cQString &lang,
                            cQString &filePath);

        // settings
        void            _settingsLoad();
        void            _settingsSave();

        // global hotkey
    #if defined(Q_OS_UNIX)
        int          iRv;
        int          _keyCode;
        Display     *_display;
    #endif

        // utils
        QString         _exportfileNameBuild(cQString &fileExt);
};
//------------------------------------------------------------------------------
#endif // iTeacher_CMainH
