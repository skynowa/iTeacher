/**
 * \file   WordEditor.h
 * \brief
 */


#ifndef iTeacher_CWordEditorH
#define iTeacher_CWordEditorH
//-------------------------------------------------------------------------------------------------
#include "ui_WordEditor.h"

#include "../Config.h"
#include "../QtLib/Common.h"

#include "Main.h"
//-------------------------------------------------------------------------------------------------
class WordEditor :
    public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(WordEditor)

public:
    Ui::UiWordEditor ui;

                    WordEditor(QWidget *parent, QSqlTableModel *tableModel,
                        qtlib::SqlNavigator *sqlNavigator, cbool &inserMode,
                        cQString &newTerm = QString());
    virtual        ~WordEditor();

    bool            isConstructed() const;

private:
    bool            _isConstructed;
    QSqlTableModel *_model;
    qtlib::SqlNavigator *_sqlNavigator;
    cint            _currentRow;
    cbool           _insertMode;
    cQString        _termNew;

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
