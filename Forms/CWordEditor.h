/**
 * \file   CWordEditor.h
 * \brief
 */


#ifndef CWORDEDITOR_H
#define CWORDEDITOR_H
//---------------------------------------------------------------------------
#include "ui_CWordEditor.h"

#include "../Config.h"
#include "../QtLib/Common.h"
//---------------------------------------------------------------------------
class CWordEditor :
    public QDialog
{
        Q_OBJECT

    public:
        Ui::CWordEditor m_Ui;

                        CWordEditor      (QWidget *parent, QSqlTableModel *tableModel, const int &currentRow);
        virtual        ~CWordEditor      ();

    private:
        QSqlTableModel *_m_tmModel;
        const int       _m_ciCurrentRow;
        QClipboard     *_m_pcbClipboard;    ///< clipboard

        void           _construct        ();
        void           _destruct         ();

        void           _resetAll         ();
        void           _saveAll          ();

    private Q_SLOTS:
        void           slot_textTranslate();
        void           slot_bbxButtons_OnClicked(QAbstractButton *button);
};
//---------------------------------------------------------------------------
#endif // CWORDEDITOR_H
