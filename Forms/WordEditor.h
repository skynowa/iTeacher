/**
 * \file   WordEditor.h
 * \brief
 */


#pragma once

#include "ui_WordEditor.h"

#include "../Config.h"
#include "../QtLib/Common.h"
#include "../Classes/GoogleTranslator.h"

#include "Main.h"
//-------------------------------------------------------------------------------------------------
class WordEditor :
    public QDialog
{
public:
    Ui::UiWordEditor ui;

                     WordEditor(QWidget *parent, QSqlTableModel *tableModel,
                         qtlib::SqlNavigator *sqlNavigator, cbool &inserMode,
                         cQString &termNew = QString());
    virtual         ~WordEditor();

    bool             isConstructed() const;

    static
    void             activateOpened(WordEditor *dlgWordEditor);

private:
    bool             _isConstructed;
    QSqlTableModel  *_model;
    qtlib::SqlNavigator *_sqlNavigator;
    cint             _currentRow;
    cbool            _insertMode;
    QString          _termNew;
    GoogleTranslator _translator;

    QPalette         _plInfoDefault;

    void             _construct();
    void             _destruct();

    void             _resetAll();
    void             _saveAll(QDialog::DialogCode *code);

    void             _settingsLoad();
    void             _settingsSave();

    void             _languagesAutoDetect();
        ///< auto detect translating languages

    Q_OBJECT
    Q_DISABLE_COPY(WordEditor)

private Q_SLOTS:
    void             pbtnLangsSwap_OnClicked();
    void             tbtnTermCopy_OnClicked();
    void             tbtnTermPaste_OnClicked();
    void             tbtnValueCopy_OnClicked();
    void             translate();
    bool             check();
    void             pbtnTagsEdit_OnClicked();
    void             bbxButtons_OnClicked(QAbstractButton *button);
};
//-------------------------------------------------------------------------------------------------
