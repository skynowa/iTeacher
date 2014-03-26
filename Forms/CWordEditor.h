/**
 * \file   CWordEditor.h
 * \brief
 */


#ifndef iTeacher_CWordEditorH
#define iTeacher_CWordEditorH
//-------------------------------------------------------------------------------------------------
#include "ui_CWordEditor.h"

#include <QStatusBar>

#include "../Config.h"
#include "../QtLib/Common.h"

#include "CMain.h"
//-------------------------------------------------------------------------------------------------
class CWordEditor :
    public QDialog
{
        Q_OBJECT

    public:
        Ui::CWordEditor m_Ui;

                        CWordEditor(QWidget *parent, QSqlTableModel *tableModel,
                            CSqlNavigator *sqlNavigator,
                            cQString &newTerm = QString());
        virtual        ~CWordEditor();

    private:
        QSqlTableModel *_tmModel;
        CSqlNavigator  *_snSqlNavigator;
        cint            _ciCurrentRow;
        cQString        _csNewTerm;

        QStatusBar     *_sbInfo;
        QPalette        _plInfoDefault;

        void            _construct();
        void            _destruct();

        void            _resetAll();
        void            _saveAll(QDialog::DialogCode *code);

        void            _settingsLoad();
        void            _settingsSave();

        bool            _isTermExists(cQString &term);

    private Q_SLOTS:
        void            slot_termTranslate();
        bool            slot_termCheck();
        void            slot_bbxButtons_OnClicked(QAbstractButton *button);
        void            slot_WordTermOrValue_OnTextChanged();
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_CWordEditorH
