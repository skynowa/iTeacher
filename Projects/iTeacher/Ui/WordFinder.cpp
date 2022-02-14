/**
 * \file   WordFinder.cpp
 * \brief
 */


#include "WordFinder.h"

#include "../QtLib/Utils.h"
#include "../QtLib/Db/SqlTableModelEx.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
WordFinder::WordFinder(
    QWidget                          *a_parent,
    qtlib::SqlRelationalTableModelEx *a_tableModel
) :
    QDialog(a_parent),
    _model (a_tableModel)
{
    qTEST_PTR(_model);

    _construct();
}
//-------------------------------------------------------------------------------------------------
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

    // Main
    {
        // ui.bbxButtons
        {
            ui.bbxButtons->setMinimumWidth(BUTTON_BOX_WIDTH);

            QList<QAbstractButton *> buttons = ui.bbxButtons->buttons();

            while ( !buttons.empty() ) {
                buttons.takeFirst()->setMinimumWidth( ui.bbxButtons->minimumWidth() );
            }
        }

        ui.cboTerm->setFocus();
    }

    // tags
    {
        QSqlQuery query;
        query.prepare("SELECT " DB_F_TAGS_NAME " FROM " DB_T_TAGS ";");
        bool bRv = query.exec();
        qCHECK_REF(bRv, query);

        for ( ; query.next(); ) {
            ui.cboTags->addItem( query.value(0).toString() );
        }

        ui.cboTags->setCurrentText("");
    }

    // signals
    {
        connect(ui.bbxButtons, &QDialogButtonBox::clicked,
                this,          &WordFinder::bbxButtons_OnClicked);
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
WordFinder::bbxButtons_OnClicked(
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
    ui.cboTerm->setCurrentText("");
    ui.cboValue->setCurrentText("");
    ui.cboTags->setCurrentText("");
    ui.chkNotLearned->setChecked(false);
    ui.chkNotMarked->setChecked(false);
    ui.chkLearned->setChecked(false);
    ui.chkMarked->setChecked (false);
}
//-------------------------------------------------------------------------------------------------
void
WordFinder::_saveAll()
{
    bool bRv = false;

    qtlib::SqlTableModelEx::db_fields_t fields;
    {
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TERM,  ui.cboTerm->currentText()) );
        fields.push_back( QPair<QString, QString>(DB_F_MAIN_VALUE, ui.cboValue->currentText()) );

        QString tag_id;
        {
            QMap<QString, QString> nameToId;
            {
                QSqlQuery qryQuery(_model->database());

                cQString sql = "SELECT * FROM " DB_T_TAGS ";";

                bRv = qryQuery.exec(sql);
                qCHECK_REF(bRv, qryQuery);

                for ( ; ; ) {
                    bRv = qryQuery.next();
                    qCHECK_DO(!bRv, break);

                    cQString id   = qryQuery.value(0).toString();
                    cQString name = qryQuery.value(1).toString();

                    nameToId.insert(name, id);
                }
            }

            tag_id = nameToId[ ui.cboTags->currentText() ];
            if (tag_id == DB_ALL_TAGS_ID) {
                tag_id.clear();
            }
        }

        fields.push_back( QPair<QString, QString>(DB_F_MAIN_TAG, tag_id) );
    }

    QString sqlStrWhere;
    {
        cQString separator = "##";

        cbool isNotLearned = ui.chkNotLearned->isChecked();
        cbool isNotMarked  = ui.chkNotMarked->isChecked();
        cbool isLearned    = ui.chkLearned->isChecked();
        cbool isMarked     = ui.chkMarked->isChecked();

        if (isNotLearned) {
            cQString sql = QString("%1=%2")
                                .arg(DB_F_MAIN_IS_LEARNED)
                                .arg(0);
            sqlStrWhere += sql + separator;
        }

        if (isNotMarked) {
            cQString sql = QString("%1=%2")
                                .arg(DB_F_MAIN_IS_MARKED)
                                .arg(0);
            sqlStrWhere += sql + separator;
        }

        if (isLearned) {
            cQString sql = QString("%1=%2")
                                .arg(DB_F_MAIN_IS_LEARNED)
                                .arg(1);
            sqlStrWhere += sql + separator;
        }

        if (isMarked) {
            cQString sql = QString("%1=%2")
                                .arg(DB_F_MAIN_IS_MARKED)
                                .arg(1);
            sqlStrWhere += sql + separator;
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

    QString sqlStrOrderBy;
    {
        bool isRandomized = ui.chkRandomized->isChecked();
        if (isRandomized) {
            sqlStrOrderBy = "ORDER BY Random()";
        }
    }

    _model->filter(fields, "", sqlStrWhere, sqlStrOrderBy);
}
//-------------------------------------------------------------------------------------------------
