/**
 * \file   CMain.h
 * \brief  main widget
 */


#ifndef iTeacher_CMainH
#define iTeacher_CMainH
//---------------------------------------------------------------------------
#include "../Config.h"

#include <xLib/Common/xCommon.h>
#include "../QtLib/Common.h"
#include "../Classes/CSqlNavigator.h"
#include "../Classes/CUtils.h"

#include "ui_CMain.h"
//---------------------------------------------------------------------------
class CMain :
    public QMainWindow
{
        Q_OBJECT

    public:
                               CMain             (QWidget *parent = 0, Qt::WFlags flags = 0);
        virtual               ~CMain             ();

        Ui::CMainClass         m_Ui;
        QString                m_sAppName;
        QString                m_sAppDir;
        QString                m_sDbDir;
        QString                m_sDbBackupDir;
        CSqlNavigator          m_navNavigator;

    private:
        void                   _construct        ();
        void                   _destruct         ();
        void                   _initMain         ();
        void                   _initModel        ();
        void                   _initActions      ();
        void                   _initMenus        ();


        // group "File"
        QAction                actFile_CreateDb;
        QAction                actFile_ImportCsv;
        QAction                actFile_ExportCsv;
        QAction                actFile_ExportPdf;
        QAction                actFile_Exit;

        // group "Edit"
        QAction                actEdit_MovetoFirst;
        QAction                actEdit_MovetoPrior;
        QAction                actEdit_MovetoNext;
        QAction                actEdit_MovetoLast;
        QAction                actEdit_Insert;
        QAction                actEdit_Delete;
        QAction                actEdit_Edit;
        QAction                actEdit_Post;
        QAction                actEdit_Cancel;
        QAction                actEdit_Refresh;

        // group "Find"
        QAction                actFind_Search;

        // group "View"

        // group "Options"
        QAction                actOptions_Settings;

        // group "Help"
        QAction                actHelp_Faq;
        QAction                actHelp_About;

        // menu
        QMenu                  mnuFile;
        QMenu                  mnuEdit;
        QMenu                  mnuFind;
        QMenu                  mnuView;
        QMenu                  mnuOptions;
        QMenu                  mnuHelp;

    private Q_SLOTS:
        // group "File"
        void                   slot_OnCreateDb   ();
        void                   slot_OnImportCsv  ();
        void                   slot_OnExportCsv  ();
        void                   slot_OnExportPdf  ();
        void                   slot_OnExit       ();

        // group "Edit"
        void                   slot_OnFirst      ();
        void                   slot_OnPrior      ();
        void                   slot_OnNext       ();
        void                   slot_OnLast       ();
        void                   slot_OnInsert     ();
        void                   slot_OnRemove     ();
        void                   slot_OnEdit       ();
        void                   slot_OnPost       ();
        void                   slot_OnCancel     ();
        void                   slot_OnRefresh    ();

        // group "Find"
        void                   slot_OnSearch     ();

        // group "View"


        // group "Options"
        void                   slot_OnSettings   ();

        // group "Help"
        void                   slot_OnFaq        ();
        void                   slot_OnAbout      ();

        void                   slot_tabvInfo_OnSelectionChanged            (const QItemSelection &selected, const QItemSelection &deselected);
        void                   slot_tabvInfo_OnDoubleClicked               (const QModelIndex &index);

        void                   slot_cboDictionaryPath_OnCurrentIndexChanged(const QString &arg);

    private:
        void                   cboDictionaryPath_reload();

        // DB
        QSqlDatabase          *_m_dbDatabase;
        QSqlTableModel        *_m_tmModel;

        void                   dbOpen            (const QString &filePath);
        void                   dbReopen          (const QString &filePath);
        void                   dbClose           ();

};
//---------------------------------------------------------------------------
#endif // iTeacher_CMainH
