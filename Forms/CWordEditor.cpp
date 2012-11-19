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
    _m_ciCurrentRow(currentRow)
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
        connect(m_Ui.pbtnTranslate,    SIGNAL( clicked() ),
                 this,                 SLOT  ( slot_OnTranslate() ));

        connect(m_Ui.bbxButtons,       SIGNAL( clicked(QAbstractButton *) ),
                this,                  SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));
    }
}
//---------------------------------------------------------------------------
void
CWordEditor::_destruct() {

}
//---------------------------------------------------------------------------


/****************************************************************************
*   private slots
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CWordEditor::slot_OnTranslate() {
    qDebug() << "slot_OnTranslate";
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


/****************************************************************************
*   private slots
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CWordEditor::_resetAll() {
    m_Ui.tedtWordTerm->clear();
    m_Ui.tedtWordValue->clear();
    m_Ui.chkWordIsLearned->setChecked(false);
    m_Ui.chkWordIsMarked->setChecked (false);
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


