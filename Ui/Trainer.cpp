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

    cQString status = QString("Tag: %1, Marked: %2, Learned: %3\n")
                        .arg(record.value(DB_F_MAIN_TAG).toString())
                        .arg(record.value(DB_F_MAIN_IS_MARKED ).toString())
                        .arg(record.value(DB_F_MAIN_IS_MARKED).toString());

    if (::option_termValueSwap) {
        _ui.lblTerm->setText(value);
        _ui.lblValue->setText(term);
    } else {
        _ui.lblTerm->setText(term);
        _ui.lblValue->setText(value);
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
