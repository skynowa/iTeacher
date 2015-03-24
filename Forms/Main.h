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
#include "../Classes/GoogleTranslator.h"

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
    enum ImportExportOrder
    {
        ieoUnknown   = 0,
        ieoTermValue = 1,
        ieoValueTerm = 2
    };

    Ui::UiMain      ui;
    QSystemTrayIcon _trayIcon;

    qtlib::GlobalShortcut _scShowHide;
    qtlib::GlobalShortcut _scImportClipboard;
    bool            _isHideOnCLose;

    void            _construct();
    void            _destruct();
    void            _initMain();
    void            _initModel();
    void            _initActions();
    void            _cboDictPath_reload();

    // DB
    QSqlDatabase   *_db;
    QSqlRelationalTableModel *_model;
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
    void            slot_OnCreateDb();
    void            slot_OnImportCsv();
    void            slot_OnImportClipboard();
    void            slot_OnExportCsv();
    void            slot_OnExportPdf();
    void            slot_OnExportClipboard();
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
    void            slot_OnPlayTerm();
    void            slot_OnPlayValue();
    void            slot_OnPlayTermValue();

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
