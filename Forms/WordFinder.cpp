/**
 * \file   WordFinder.cpp
 * \brief
 */


#include "WordFinder.h"

#include "../QtLib/Utils.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
WordFinder::WordFinder(
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
WordFinder::~WordFinder()
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
WordFinder::_construct()
{
    ui.setupUi(this);

    ui.cboWordTerm->setFocus();

    // tags
    {
        QSqlQuery query;
        query.prepare("SELECT " DB_F_TAGS_NAME " FROM " DB_T_TAGS ";");
        bool rv = query.exec();
        qCHECK_REF(rv, query);

        for ( ; query.next(); ) {
            ui.cboTags->addItem( query.value(0).toString() );
        }

        ui.cboTags->setCurrentText("");
    }

    // signals
    {
        connect(ui.bbxButtons, SIGNAL( clicked(QAbstractButton *) ),
                this,            SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));
    }
}
//-------------------------------------------------------------------------------------------------
void
WordFinder::_destruct()
{

}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private slots
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
WordFinder::slot_bbxButtons_OnClicked(
    QAbstractButton *a_button
)
{
    QDialogButtonBox::StandardButton type = ui.bbxButtons->standardButton(a_button);
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
WordFinder::_resetAll()
{
    ui.cboWordTerm->clear();
    ui.cboWordValue->clear();
    ui.cboTags->setCurrentText("");
    ui.chkWordNotLearned->setChecked(false);
    ui.chkWordNotMarked->setChecked(false);
    ui.chkWordLearned->setChecked(false);
    ui.chkWordMarked->setChecked (false);
}
//-------------------------------------------------------------------------------------------------
void
WordFinder::_saveAll()
{
    ::Utils::db_fields_t fields;
    {
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TERM,  ui.cboWordTerm->currentText()) );
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_VALUE, ui.cboWordValue->currentText()) );
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TAG,   ui.cboTags->currentText()) );
    }

    QString sqlStrWhere;
    {
        cQString separator = "##";

        bool cond1 = ui.chkWordNotLearned->isChecked();
        bool cond2 = ui.chkWordNotMarked->isChecked();
        bool cond3 = ui.chkWordLearned->isChecked();
        bool cond4 = ui.chkWordMarked->isChecked();

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

    ::Utils::dbFilter(_model, DB_T_MAIN, fields, "", sqlStrWhere, "ORDER BY Random()");
}
//-------------------------------------------------------------------------------------------------