/**
 * \file   WordEditor.h
 * \brief
 */


#ifndef iTeacher_WordEditorH
#define iTeacher_WordEditorH
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
                         cQString &terminNew = QString());
    virtual         ~WordEditor();

    bool             isConstructed() const;

private:
    bool             _isConstructed;
    QSqlTableModel  *_model;
    qtlib::SqlNavigator *_sqlNavigator;
    cint             _currentRow;
    cbool            _insertMode;
    cQString         _terminNew;

    QPalette         _plInfoDefault;

    void             _construct();
    void             _destruct();

    void             _resetAll();
    void             _saveAll(QDialog::DialogCode *code);

    void             _settingsLoad();
    void             _settingsSave();

    bool             _isTerminExists(cQString &termin);
    void             _languagesAutoDetect();
        ///< auto detect translating languages

    enum Language
        ///< detected language
    {
        lgUnknown,
        lgEn,
        lgRu
    };

    void             _googleLanguagesDetect(cQString &text, WordEditor::Language *langFrom,
                        WordEditor::Language *langTo, QString *langCodeFrom,
                        QString *langCodeTo) const;
        ///< detect languages
    void             _googleTranslate(cQString &textFrom, cQString &langFrom, cQString &langTo,
                         QString *textToBrief, QString *textToDetail, QString *textToRaw) const;
        ///< translate text by Google API

private Q_SLOTS:
    void             slot_pbtnLangsSwap_OnClicked();
    void             slot_terminTranslate();
    bool             slot_terminCheck();
    void             slot_bbxButtons_OnClicked(QAbstractButton *button);
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_WordEditorH
