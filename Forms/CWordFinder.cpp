/**
 * \file   CWordFinder.cpp
 * \brief
 */


#include "CWordFinder.h"

#include "../QtLib/CUtils.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
CWordFinder::CWordFinder(
    QWidget        *a_parent,
    QSqlTableModel *a_tableModel
) :
    QDialog(a_parent),
    _model (a_tableModel)
{
    qTEST_PTR(_model);

    _construct();
}
//-------------------------------------------------------------------------------------------------
/*virtual*/
CWordFinder::~CWordFinder()
{
    _destruct();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

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


/**************************************************************************************************
*   private slots
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
CWordFinder::slot_bbxButtons_OnClicked(
    QAbstractButton *a_button
)
{
    QDialogButtonBox::StandardButton type = m_Ui.bbxButtons->standardButton(a_button);
    switch (type) {
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
            qTEST(false);
            break;
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private slots
*
**************************************************************************************************/

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
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TERM,  m_Ui.cboWordTerm->currentText()) );
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_VALUE, m_Ui.cboWordValue->currentText()) );
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TAG,   m_Ui.cboTags->currentText()) );
    }

    QString sqlStrWhere;
    {
        cQString separator = "##";

        bool cond1 = m_Ui.chkWordNotLearned->isChecked();
        bool cond2 = m_Ui.chkWordNotMarked->isChecked();
        bool cond3 = m_Ui.chkWordLearned->isChecked();
        bool cond4 = m_Ui.chkWordMarked->isChecked();

        QString sql1;
        if (cond1) {
            sql1 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_LEARNED)
                        .arg(0);
            sqlStrWhere += sql1 + separator;
        }

        QString sql2;
        if (cond2) {
            sql2 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_MARKED)
                        .arg(0);
            sqlStrWhere += sql2 + separator;
        }

        QString sql3;
        if (cond3) {
            sql3 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_LEARNED)
                        .arg(1);
            sqlStrWhere += sql3 + separator;
        }

        QString sql4;
        if (cond4) {
            sql4 = QString("%1=%2")
                        .arg(DB_F_MAIN_IS_MARKED)
                        .arg(1);
            sqlStrWhere += sql4 + separator;
        }

        // remove separator from end
        {
            if (separator == sqlStrWhere.right( separator.size() )) {
                sqlStrWhere.truncate( sqlStrWhere.size() - separator.size() );
            }
        }

        // replace separator -> " OR "
        {
            sqlStrWhere.replace(separator, " OR ");
        }
    }

    CUtils::dbFilter(_model, DB_T_MAIN, fields, "", sqlStrWhere, "ORDER BY Random()");
}
//-------------------------------------------------------------------------------------------------
