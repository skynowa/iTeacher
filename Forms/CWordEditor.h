/**
 * \file   CWordEditor.h
 * \brief
 */


#ifndef iTeacher_CWordEditorH
#define iTeacher_CWordEditorH
//---------------------------------------------------------------------------
#include "ui_CWordEditor.h"

#include "../Config.h"
#include "../QtLib/Common.h"

#include "CMain.h"
//---------------------------------------------------------------------------
class CWordEditor :
    public QDialog
{
        Q_OBJECT

    public:
        Ui::CWordEditor m_Ui;

                        CWordEditor      (QWidget *parent, QSqlTableModel *tableModel,
                                          const int &currentRow, const QString &newTerm = QString());
        virtual        ~CWordEditor      ();

    private:
        QSqlTableModel *_m_tmModel;
        const int       _m_ciCurrentRow;
        const QString   _m_csNewTerm;

        QStatusBar     *_m_sbInfo;
        QPalette        _m_plInfoDefault;

        void            _construct        ();
        void            _destruct         ();

        void            _resetAll         ();
        void            _saveAll          ();

        void            _settingsLoad     ();
        void            _settingsSave     ();

        bool            _isTermExists     (const QString &term);

    private Q_SLOTS:
        void            slot_termTranslate();
        void            slot_termCheck    ();
        void            slot_bbxButtons_OnClicked(QAbstractButton *button);
        void            slot_WordTermOrValue_OnTextChanged();
};
//---------------------------------------------------------------------------
#endif // iTeacher_CWordEditorH
