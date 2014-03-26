/**
 * \file   CWordFinder.cpp
 * \brief
 */


#include "CWordFinder.h"

#include "../QtLib/CUtils.h"


/*******************************************************************************
*   public
*
*******************************************************************************/

//-------------------------------------------------------------------------------------------------
CWordFinder::CWordFinder(
    QWidget        *a_parent,
    QSqlTableModel *a_tableModel
) :
    QDialog   (a_parent),
    _tmModel(a_tableModel)
{
    Q_ASSERT(NULL != _tmModel);

    _construct();
}
//-------------------------------------------------------------------------------------------------
/*virtual*/
CWordFinder::~CWordFinder()
{
    _destruct();
}
//-------------------------------------------------------------------------------------------------


/*******************************************************************************
*   private
*
*******************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CWordFinder::_construct()
{
    m_Ui.setupUi(this);

    m_Ui.cboWordTerm->setFocus();

    // tags
    {
        QSqlQuery query;
        query.prepare("SELECT " DB_F_TAGS_NAME " FROM " DB_T_TAGS ";");
        bool rv = query.exec();
        qCHECK_REF(rv, query);

        for ( ; query.next(); ) {
            m_Ui.cboTags->addItem( query.value(0).toString() );
        }

        m_Ui.cboTags->setCurrentText("");
    }

    // signals
    {
        connect(m_Ui.bbxButtons, SIGNAL( clicked(QAbstractButton *) ),
                this,            SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));
    }
}
//-------------------------------------------------------------------------------------------------
void
CWordFinder::_destruct()
{

}
//-------------------------------------------------------------------------------------------------


/*******************************************************************************
*   private slots
*
*******************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CWordFinder::slot_bbxButtons_OnClicked(
    QAbstractButton *a_button
)
{
    QDialogButtonBox::StandardButton sbType = m_Ui.bbxButtons->standardButton(a_button);
    switch (sbType) {
        case QDialogButtonBox::Reset:
            _resetAll();
            break;
        case QDialogButtonBox::Ok:
            _saveAll();
            close();
            break;
        case QDialogButtonBox::Cancel:
            close();
            break;
        case QDialogButtonBox::Apply:
            _saveAll();
            break;
        default:
            Q_ASSERT(false);
            break;
    }
}
//-------------------------------------------------------------------------------------------------


/*******************************************************************************
*   private slots
*
*******************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CWordFinder::_resetAll()
{
    m_Ui.cboWordTerm->clear();
    m_Ui.cboWordValue->clear();
    m_Ui.cboTags->setCurrentText("");
    m_Ui.chkWordNotLearned->setChecked(false);
    m_Ui.chkWordNotMarked->setChecked(false);
    m_Ui.chkWordLearned->setChecked(false);
    m_Ui.chkWordMarked->setChecked (false);
}
//-------------------------------------------------------------------------------------------------
void
CWordFinder::_saveAll()
{
    CUtils::db_fields_t fields;
    {
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TERM,  m_Ui.cboWordTerm->currentText())  );
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_VALUE, m_Ui.cboWordValue->currentText()) );
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TAG,   m_Ui.cboTags->currentText()) );
    }

    QString sqlStrWhere;
    {
        cQString csSeparator = "##";

        bool bCond1 = m_Ui.chkWordNotLearned->isChecked();
        bool bCond2 = m_Ui.chkWordNotMarked->isChecked();
        bool bCond3 = m_Ui.chkWordLearned->isChecked();
        bool bCond4 = m_Ui.chkWordMarked->isChecked();

        QString sSql1;
        if (bCond1) {
            sSql1 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_LEARNED)
                        .arg(0);
            sqlStrWhere += sSql1 + csSeparator;
        }

        QString sSql2;
        if (bCond2) {
            sSql2 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_MARKED)
                        .arg(0);
            sqlStrWhere += sSql2 + csSeparator;
        }

        QString sSql3;
        if (bCond3) {
            sSql3 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_LEARNED)
                        .arg(1);
            sqlStrWhere += sSql3 + csSeparator;
        }

        QString sSql4;
        if (bCond4) {
            sSql4 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_MARKED)
                        .arg(1);
            sqlStrWhere += sSql4 + csSeparator;
        }

        // remove csSeparator from end
        {
            if (csSeparator == sqlStrWhere.right( csSeparator.size() )) {
                sqlStrWhere.truncate( sqlStrWhere.size() - csSeparator.size() );
            }
        }

        // replace csSeparator -> " OR "
        {
            sqlStrWhere.replace(csSeparator, " OR ");
        }

    }

    CUtils::dbFilter(_tmModel, DB_T_MAIN, fields, "", sqlStrWhere, "");
}
//-------------------------------------------------------------------------------------------------
