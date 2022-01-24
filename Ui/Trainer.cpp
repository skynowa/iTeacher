/**
 * \file  Trainer.cpp
 * \brief main widget
 */


#include "Trainer.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
namespace
{

const bool option_termValueSwap {true};

}
//-------------------------------------------------------------------------------------------------
Trainer::Trainer(
    QWidget         *a_parent,
    Qt::WindowFlags  a_flags
) :
    QMainWindow(a_parent, a_flags)
{
    _initMain();
    _initActions();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Trainer::_initMain()
{
    _ui.setupUi(this);

    // Trainer
    {
        setWindowTitle( qS2QS(xl::package::Application::info().name) );
        setGeometry(0, 0, APP_WIDTH, APP_HEIGHT);
        qtlib::Utils::widgetAlignCenter(this);
    }

    // DB
    {
        cQString dbName   = "Words.db";
        cQString dictPath = qS2QS(xl::package::Application::dbDirPath()) + QDir::separator() + dbName;

        _db.reset(new SqliteDb(this, dictPath));
        _db->reopen();
    }

    randomRow();
}
//-------------------------------------------------------------------------------------------------
void
Trainer::_initActions()
{
    // group "Main"
    {
        connect(_ui.actMain_RandomRow, &QAction::triggered,
                this,                  &Trainer::randomRow);
        connect(_ui.actMain_Learn,     &QAction::triggered,
                this,                  &Trainer::learn);
        connect(_ui.actMain_Mark,      &QAction::triggered,
                this,                  &Trainer::mark);
        // connect(_ui.actMain_Xxxx,      &QAction::triggered,
        //         this,                  &Trainer::xxxxx);
        connect(_ui.actMain_Exit,      &QAction::triggered,
                this,                  &Trainer::exit);
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Main"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Trainer::randomRow() const
{
    const QSqlRecord record = _db->randomRow();

    cQString term = QString("%1")
                        .arg(record.value(DB_F_MAIN_TERM).toString());

    cQString value = QString("%1")
                        .arg(record.value(DB_F_MAIN_VALUE).toString());

    cQString tagId   = record.value(DB_F_MAIN_TAG).toString();
    cQString learned = record.value(DB_F_MAIN_IS_LEARNED).toString();
    cQString marked  = record.value(DB_F_MAIN_IS_MARKED).toString();

    struct Separators
    {
        cQString groupWords   {";"};
        cQString groupWordsNl {"\n\n"};

        cQString words        {","};
        cQString wordsNl      {"\n"};
    };

    Separators seps;

    // term
    QString termFromated;
    {
        termFromated = term.split(seps.groupWords).join(seps.groupWordsNl);
        termFromated = termFromated.split(seps.words).join(seps.wordsNl);
    }

    // value
    QString valueFromated;
    {
        valueFromated = value.split(seps.groupWords).join(seps.groupWordsNl);
        valueFromated = valueFromated.split(seps.words).join(seps.wordsNl);
    }

    // status
    QString status;
    {
        const QSqlRecord statusRecord = _db->findByField("ID", tagId);

        QString tagName = statusRecord.value(DB_F_TAGS_NAME).toString();
        if ( tagName.isEmpty() ) {
            tagName = "-";
        }

        status = QString("Learned: %1, Marked: %2, Tag: %3\n")
                    .arg(learned)
                    .arg(marked)
                    .arg(tagName);
    }

    if (::option_termValueSwap) {
        _ui.lblTerm->setText(valueFromated);
        _ui.lblValue->setText(termFromated);
    } else {
        _ui.lblTerm->setText(termFromated);
        _ui.lblValue->setText(valueFromated);
    }

    _ui.lblStatus->setText(status);
}
//-------------------------------------------------------------------------------------------------
void
Trainer::learn() const
{
    qTRACE_SCOPE_FUNC;

    // TODO: impl
}
//-------------------------------------------------------------------------------------------------
void
Trainer::mark() const
{
    qTRACE_SCOPE_FUNC;

    // TODO: impl
}
//-------------------------------------------------------------------------------------------------
//void
//Trainer::xxxxx() const
//{
//    qTRACE_SCOPE_FUNC
//
//    // TODO: impl
//}
//-------------------------------------------------------------------------------------------------
void
Trainer::exit()
{
    qApp->exit();
}
//-------------------------------------------------------------------------------------------------
