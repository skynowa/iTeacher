/**
 * \file   CWordFinder.cpp
 * \brief
 */


#include "CWordFinder.h"

#include "../QtLib/CUtils.h"


/****************************************************************************
*   public
*
*****************************************************************************/

//---------------------------------------------------------------------------
CWordFinder::CWordFinder(
    QWidget        *parent,
    QSqlTableModel *tableModel
) :
    QDialog   (parent),
    _m_tmModel(tableModel)
{
    Q_ASSERT(NULL != _m_tmModel);

    _construct();
}
//---------------------------------------------------------------------------
/*virtual*/
CWordFinder::~CWordFinder() {
    _destruct();
}
//---------------------------------------------------------------------------


/****************************************************************************
*   private
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CWordFinder::_construct() {
    m_Ui.setupUi(this);

    m_Ui.cboWordTerm->setFocus();

    // signals
    {
        connect(m_Ui.bbxButtons, SIGNAL( clicked(QAbstractButton *) ),
                this,            SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));
    }
}
//---------------------------------------------------------------------------
void
CWordFinder::_destruct() {

}
//---------------------------------------------------------------------------


/****************************************************************************
*   private slots
*
*****************************************************************************/

//---------------------------------------------------------------------------
void
CWordFinder::slot_bbxButtons_OnClicked(
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
CWordFinder::_resetAll() {
    m_Ui.cboWordTerm->clear();
    m_Ui.cboWordValue->clear();
    m_Ui.chkWordIsLearned->setChecked(false);
    m_Ui.chkWordIsMarked->setChecked (false);
}
//---------------------------------------------------------------------------
void
CWordFinder::_saveAll() {
    CUtils::db_fields_t fields;
    {
        fields.push_back( QPair<QString, QString>(CONFIG_DB_F_MAIN_TERM,  m_Ui.cboWordTerm->currentText())  );
        fields.push_back( QPair<QString, QString>(CONFIG_DB_F_MAIN_VALUE, m_Ui.cboWordValue->currentText()) );
    }

    QString sqlStrWhere;
    {
        QString sSql1;
        if (true == m_Ui.chkWordIsLearned->isChecked()) {
            sSql1 = QString("%1=%2")
                        .arg(CONFIG_DB_F_MAIN_IS_LEARNED)
                        .arg(1);
        }

        QString sSql2;
        if (true == m_Ui.chkWordIsMarked->isChecked()) {
            sSql2 = QString("%1=%2")
                        .arg(CONFIG_DB_F_MAIN_IS_MARKED)
                        .arg(1);
        }

        bool bCond1 = m_Ui.chkWordIsLearned->isChecked();
        bool bCond2 = m_Ui.chkWordIsMarked->isChecked();

        if      (!bCond1 && !bCond2) {
            sqlStrWhere.clear();
        }
        else if (!bCond1 && bCond2) {
            sqlStrWhere = sSql2;
        }
        else if (bCond1 && !bCond2) {
            sqlStrWhere = sSql1;
        }
        else if (bCond1 && bCond2) {
            sqlStrWhere = QString("%1 AND %2")
                            .arg(sSql1)
                            .arg(sSql2);
        }
        else {
            Q_ASSERT(false);
        }
    }

    CUtils::dbFilter(_m_tmModel, CONFIG_DB_T_MAIN, fields, "", sqlStrWhere, "");
}
//---------------------------------------------------------------------------



