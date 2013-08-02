/**
 * \file   CWordFinder.h
 * \brief
 */


#ifndef iTeacher_CWordFinderH
#define iTeacher_CWordFinderH
//---------------------------------------------------------------------------
#include "ui_CWordFinder.h"

#include "../Config.h"
#include "../QtLib/Common.h"
//---------------------------------------------------------------------------
class CWordFinder :
    public QDialog
{
        Q_OBJECT

    public:
        Ui::CWordFinder m_Ui;

                        CWordFinder(QWidget *parent, QSqlTableModel *tableModel);
        virtual        ~CWordFinder();

    private:
        QSqlTableModel *_tmModel;

        void            _construct();
        void            _destruct();

        void            _resetAll();
        void            _saveAll();

    private Q_SLOTS:
        void            slot_bbxButtons_OnClicked(QAbstractButton *button);

};
//---------------------------------------------------------------------------
#endif // iTeacher_CWordFinderH
