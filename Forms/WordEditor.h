/**
 * \file   WordEditor.h
 * \brief
 */


#ifndef iTeacher_CWordEditorH
#define iTeacher_CWordEditorH
//-------------------------------------------------------------------------------------------------
#include "ui_WordEditorUi.h"

#include <QStatusBar>

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
    Ui::WordEditorUi m_Ui;

                    WordEditor(QWidget *parent, QSqlTableModel *tableModel,
                        SqlNavigator *sqlNavigator, cQString &newTerm = QString());
    virtual        ~WordEditor();

private:
    QSqlTableModel *_model;
    SqlNavigator   *_sqlNavigator;
    cint            _currentRow;
    cQString        _termNew;

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
