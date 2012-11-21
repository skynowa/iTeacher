/**
 * \file   CWordFinder.h
 * \brief
 */


#ifndef CWORDFINDER_H
#define CWORDFINDER_H
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
        QSqlTableModel *_m_tmModel;

        void            _construct ();
        void            _destruct  ();

        void            _resetAll  ();
        void            _saveAll   ();

    private Q_SLOTS:
        void            slot_bbxButtons_OnClicked(QAbstractButton *button);

};
//---------------------------------------------------------------------------
#endif // CWORDFINDER_H
