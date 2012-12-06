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

    // signals
    {
        connect(m_Ui.bbxButtons,       SIGNAL( clicked(QAbstractButton *) ),
                this,                  SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));
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
    // fill vsFilters
    QVector<QString> vsFilters;
    {
        if (! m_Ui.cboWordTerm->currentText().isEmpty()) {
            QString filter = QString("%2 = '%1'")
                                .arg(m_Ui.cboWordTerm->currentText())
                                .arg(CONFIG_DB_F_MAIN_TERM);

            vsFilters.push_back(filter);
        }

        if (! m_Ui.cboWordValue->currentText().isEmpty()) {
            QString filter = QString("%2 = '%1'")
                                .arg(m_Ui.cboWordValue->currentText())
                                .arg(CONFIG_DB_F_MAIN_VALUE);

            vsFilters.push_back(filter);
        }

        {
            QString filter = QString("%2 = %1")
                                .arg(m_Ui.chkWordIsLearned->isChecked())
                                .arg(CONFIG_DB_F_MAIN_IS_LEARNED);

            vsFilters.push_back(filter);
        }

        {
            QString filter = QString("%2 = %1")
                                .arg(m_Ui.chkWordIsMarked->isChecked())
                                .arg(CONFIG_DB_F_MAIN_IS_MARKED);

            vsFilters.push_back(filter);
        }
    }

    // fill slFilterAll
    QStringList slFilterAll;

    for (QVector<QString>::ConstIterator it = vsFilters.constBegin();
         it != vsFilters.constEnd();
         ++ it)
    {
        qCHECK_DO((*it).isEmpty(), continue);

        slFilterAll.append(*it);
    }

    // fill sFilterAll
    QString sFilterAll;

    sFilterAll = slFilterAll.join(" or ");
    qDebug() << sFilterAll;

    // set filter
    _m_tmModel->setFilter(sFilterAll);

#if 0
    QSqlQueryModel *qmModel = dynamic_cast<QSqlQueryModel *>( _m_tmModel );
    Q_ASSERT(NULL != qmModel);

    const QString csSql = \
            "SELECT * "
            "   FROM " CONFIG_DB_T_MAIN " "
            "   ORDER BY " CONFIG_DB_F_MAIN_TERM " DESC";

    qmModel->setQuery(csSql);
#endif
}
//---------------------------------------------------------------------------



