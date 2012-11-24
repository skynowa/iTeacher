/**
 * \file   CMain.h
 * \brief  main widget
 */


#ifndef iTeacher_CMainH
#define iTeacher_CMainH
//---------------------------------------------------------------------------
#undef UNICODE
#undef _UNICODE

#include "ui_CMain.h"

#include <xLib/Common/xCommon.h>
#include "../QtLib/Common.h"

#include "../Config.h"
#include "../Classes/CSqlNavigator.h"
#include "../Classes/CUtils.h"
//---------------------------------------------------------------------------
class CMain :
    public QMainWindow
{
        Q_OBJECT

    public:
                               CMain             (QWidget *parent = 0, Qt::WFlags flags = 0);
        virtual               ~CMain             ();

        Ui::CMainClass         m_Ui;
        CSqlNavigator          m_navNavigator;

    private:
        QSqlDatabase           _m_dbDatabase;
        QSqlTableModel        *_m_tmModel;

        void                   _construct        ();
        void                   _destruct         ();
        void                   _initMain         ();
        void                   _initModel        ();
        void                   _initActions      ();
        void                   _initMenus        ();


        // group "File"
        QAction                actFile_Import;
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

    private slots:
        // group "File"
        void                   slot_OnImport     ();
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

        void                   slot_tabvInfo_OnSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
        void                   slot_tabvInfo_OnDoubleClicked   (const QModelIndex &index);

    private:

        static void            importCsv         (const QString &filePath, QSqlTableModel *sqlTableModel,
                                                  const QVector<QString> &a_fieldNames, const QString &columnSeparator);
            //< import CSV file to DB
};
//---------------------------------------------------------------------------
#endif // iTeacher_CMainH


#if 0
int main2(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTableWidget table(5,5);
    table.setItemDelegateForColumn(1, new CCheckBoxItemDelegate(&table));

    QVariant data;

    for (int i = 0; i != 5; ++ i) {
        data.setValue(CStateData(QStyle::State_Enabled | QStyle::State_On));

        for (int j = 0; j != 5; ++ j) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::UserRole,data);

            table.setItem(i, j, item);
        }
    }

    table.show();

    return a.exec();
}
#endif
