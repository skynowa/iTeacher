/**
 * \file   CWordEditor.cpp
 * \brief
 */


#include "CWordEditor.h"

#include "../Classes/CUtils.h"


/****************************************************************************
*   public
*
*****************************************************************************/

//---------------------------------------------------------------------------
CWordEditor::CWordEditor(
    QWidget        *parent,
    QSqlTableModel *tableModel,
    const int      &currentRow
) :
    QDialog        (parent),
    _m_tmModel     (tableModel),
    _m_ciCurrentRow(currentRow),
    _m_pcbClipboard(QApplication::clipboard())
{
    Q_ASSERT(NULL != _m_tmModel);
    Q_ASSERT(- 1  <  _m_ciCurrentRow);

    _construct();
}
//---------------------------------------------------------------------------
/*virtual*/
CWordEditor::~CWordEditor() {
    _destruct();
}
//---------------------------------------------------------------------------


/****************************************************************************
*   private
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CWordEditor::_construct() {
    m_Ui.setupUi(this);

    // "Main" group
    {
        QSqlRecord srRecord = _m_tmModel->record(_m_ciCurrentRow);

        // lables
        m_Ui.tedtWordTerm->setText       ( srRecord.value(CONFIG_DB_F_MAIN_TERM).toString() );
        m_Ui.tedtWordValue->setText      ( srRecord.value(CONFIG_DB_F_MAIN_VALUE).toString() );
        m_Ui.chkWordIsLearned->setChecked( srRecord.value(CONFIG_DB_F_MAIN_IS_LEARNED).toBool() );
        m_Ui.chkWordIsMarked->setChecked ( srRecord.value(CONFIG_DB_F_MAIN_IS_MARKED).toBool() );
    }

    // signals
    {
        connect(m_Ui.pbtnTranslate, SIGNAL( clicked() ),
                 this,              SLOT  ( slot_textTranslate() ));

        connect(m_Ui.bbxButtons,    SIGNAL( clicked(QAbstractButton *) ),
                this,               SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));

        connect(m_Ui.tedtWordTerm,  SIGNAL( textChanged()),
                this,               SLOT  ( slot_WordTermOrValue_OnTextChanged() ));

        connect(m_Ui.tedtWordValue, SIGNAL( textChanged()),
                this,               SLOT  ( slot_WordTermOrValue_OnTextChanged() ));
    }
}
//---------------------------------------------------------------------------
void
CWordEditor::_destruct() {

}
//---------------------------------------------------------------------------


/****************************************************************************
*   private
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CWordEditor::_resetAll() {
    m_Ui.tedtWordTerm->clear();
    m_Ui.tedtWordValue->clear();
    m_Ui.chkWordIsLearned->setChecked(false);
    m_Ui.chkWordIsMarked->setChecked(false);
}
//---------------------------------------------------------------------------
void
CWordEditor::_saveAll() {
    QSqlRecord srRecord = _m_tmModel->record(_m_ciCurrentRow);

    {
        srRecord.setValue(CONFIG_DB_F_MAIN_TERM,       m_Ui.tedtWordTerm->toPlainText());
        srRecord.setValue(CONFIG_DB_F_MAIN_VALUE,      m_Ui.tedtWordValue->toPlainText());
        srRecord.setValue(CONFIG_DB_F_MAIN_IS_LEARNED, m_Ui.chkWordIsLearned->isChecked());
        srRecord.setValue(CONFIG_DB_F_MAIN_IS_MARKED,  m_Ui.chkWordIsMarked->isChecked());
    }

    _m_tmModel->setRecord(_m_ciCurrentRow, srRecord);
    _m_tmModel->submitAll();
}
//---------------------------------------------------------------------------



/****************************************************************************
*   private slots
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CWordEditor::slot_textTranslate() {
    m_Ui.tedtWordValue->clear();

    qCHECK_DO(true == m_Ui.tedtWordTerm->toPlainText().isEmpty(), return);

    const QString sTextFrom = m_Ui.tedtWordTerm->toPlainText().toUtf8();
    QString       sTextTo;
    const QString sLangTo   = QString("ru").toUtf8();

    sTextTo = CUtils::googleTranslate(sTextFrom, sLangTo);

    m_Ui.tedtWordValue->setText( sTextTo );
}
//---------------------------------------------------------------------------
void
CWordEditor::slot_bbxButtons_OnClicked(
    QAbstractButton *button
)
{
    QDialogButtonBox::StandardButton sbType = m_Ui.bbxButtons->standardButton(button);
    switch (sbType) {
        case QDialogButtonBox::Reset: {
                _resetAll();
            }
            break;

        case QDialogButtonBox::Ok: {
                _saveAll();
                close();
            }
            break;

        case QDialogButtonBox::Cancel: {
                close();
            }
            break;

        case QDialogButtonBox::Apply: {
                _saveAll();
            }
            break;

        default: {
                Q_ASSERT(false);
            }
            break;
    }
}
//---------------------------------------------------------------------------
void
CWordEditor::slot_WordTermOrValue_OnTextChanged() {
    const QString csDirtyChar = "*";

    // check "dirty char"
    {
        QString sDirtyChar = windowTitle().right( csDirtyChar.size() );
        qCHECK_DO(csDirtyChar == sDirtyChar, return);
    }

    QString sDirtyTitle = QString(tr("%1 %2")
                                .arg(windowTitle())
                                .arg(csDirtyChar));

    setWindowTitle(sDirtyTitle);
}
//---------------------------------------------------------------------------
