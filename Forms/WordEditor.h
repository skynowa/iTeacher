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

private:
    bool             _isConstructed;
    QSqlTableModel  *_model;
    qtlib::SqlNavigator *_sqlNavigator;
    cint             _currentRow;
    cbool            _insertMode;
    cQString         _termNew;
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
    void             slot_pbtnLangsSwap_OnClicked();
    void             slot_translate();
    bool             slot_check();
    void             slot_pbtnTagsEdit_OnClicked();
    void             slot_bbxButtons_OnClicked(QAbstractButton *button);
};
//-------------------------------------------------------------------------------------------------
