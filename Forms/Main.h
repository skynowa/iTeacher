/**
 * \file   Main.h
 * \brief  main widget
 */


#ifndef iTeacher_MainH
#define iTeacher_MainH
//-------------------------------------------------------------------------------------------------
#include "../Config.h"

#include <xLib/Core/Core.h>

#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
#include "../QtLib/SqlNavigator.h"
#include "../QtLib/GlobalShortcut/GlobalShortcut.h"

#include "ui_Main.h"
//-------------------------------------------------------------------------------------------------
class Main :
    public QMainWindow
{
public:
                    Main(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::Widget);
    virtual        ~Main();

protected:
    virtual bool    eventFilter(QObject *object, QEvent *event);
    virtual void    keyPressEvent(QKeyEvent *event);
    virtual void    closeEvent(QCloseEvent *event);

private:
    enum ExportOrder
    {
        eoUnknown   = 0,
        eoTermValue = 1,
        eoValueTerm = 2
    };

    Ui::UiMain      ui;
    QSystemTrayIcon _trayIcon;

    qtlib::GlobalShortcut _scShowHide;
    qtlib::GlobalShortcut _scImportClipboard;

    void            _construct();
    void            _destruct();
    void            _initMain();
    void            _initModel();
    void            _initActions();
    void            _cboDictPath_reload();

    // DB
    QSqlDatabase   *_dbDatabase;
    QSqlTableModel *_model;
    qtlib::SqlNavigator _sqlNavigator;

    void            _dbOpen(cQString &filePath);
    void            _dbReopen(cQString &filePath);
    void            _dbClose();

    // audio
    void            _googleSpeech(cQString &text, cQString &lang, cQString &filePath);

    // settings
    ExportOrder     _exportOrder;
    void            _settingsLoad();
    void            _settingsSave();

    // utils
    QString         _exportfileNameBuild(cQString &fileExt);

    Q_OBJECT
    Q_DISABLE_COPY(Main)

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
    void            slot_OnShowHide();
    void            slot_OnZoomIn();
    void            slot_OnZoomOut();
    void            slot_OnZoomDefault();

    // group "Options"
    void            slot_OnTagsEditor();
    void            slot_OnSettings();

    // group "Help"
    void            slot_OnFaq();
    void            slot_OnAbout();

    // tray
    void            slot_OnTrayActivated(QSystemTrayIcon::ActivationReason);

    // etc
    void            slot_cboDictPath_OnCurrentIndexChanged(const QString &arg);
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_MainH
