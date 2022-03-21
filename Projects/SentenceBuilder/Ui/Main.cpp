/**
 * \file  Main.cpp
 * \brief main widget
 */


#include "Main.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
namespace
{
}
//-------------------------------------------------------------------------------------------------
Main::Main(
    QWidget         *a_parent,
    Qt::WindowFlags  a_flags
) :
    QMainWindow(a_parent, a_flags)
{
    _initMain();
    _initActions();
    _settingsLoad();
}
//-------------------------------------------------------------------------------------------------
Main::~Main()
{
    _settingsSave();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   protected
*
**************************************************************************************************/


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::_initMain()
{
    _ui.setupUi(this);

    // Main
    {
        setWindowTitle( qS2QS(xl::package::Application::info().name) );
        setGeometry(0, 0, APP_WIDTH, APP_HEIGHT);
        qtlib::Utils::widgetAlignCenter(this);
    }

    // DB
    {
        cQString dictPath = qS2QS(xl::package::Application::dbDirPath()) + QDir::separator() +
            DB_WORDS;

        _sqliteDb.reset(new SqliteDb(this, dictPath));
        _sqliteDb->reopen();
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::_initActions()
{
    // group "Main"
    {
        // connect(_ui.actMain_Xxxx,      &QAction::triggered,
        //         this,                  &Main::xxxxx);
        connect(_ui.actMain_Exit,      &QAction::triggered,
                this,                  &Main::exit);
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   settings
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
Main::_settingsLoad()
{
    // Load
    QSize  size;
    QPoint position;
    {
        QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

        // main
        settings.beginGroup(CFG_TRAINER_GROUP_MAIN);
        size     = settings.value(CFG_TRAINER_SIZE,     QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        position = settings.value(CFG_TRAINER_POSITION, QPoint(200, 200)).toPoint();
        settings.endGroup();
    }

    // Apply
    {
        // main
        resize(size);
        move(position);
    }
}
//-------------------------------------------------------------------------------------------------
void
Main::_settingsSave()
{
    QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

    // main
    settings.beginGroup(CFG_TRAINER_GROUP_MAIN);
    settings.setValue(CFG_TRAINER_SIZE,     size());
    settings.setValue(CFG_TRAINER_POSITION, pos());
    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   group "Main"
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
//void
//Main::xxxxx() const
//{
//    qTRACE_SCOPE_FUNC
//
//    // TODO: impl
//}
//-------------------------------------------------------------------------------------------------
void
Main::exit()
{
    qApp->exit();
}
//-------------------------------------------------------------------------------------------------
