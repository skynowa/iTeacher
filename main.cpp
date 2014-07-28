/**
 * \file  main.cpp
 * \brief main window
 */


#include <xLib/Core/Application.h>
#include <QtLib/Application.h>
#include "Forms/Main.h"


//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int exitCode = EXIT_FAILURE;

    // activation application window
    qtlib::Application::windowActivate(APP_WINDOW_CLASS, APP_NAME);

    // set application single inststance
    {
        bool bRv = qtlib::Application::setSingle(APP_GUID);
        qCHECK_RET(!bRv, EXIT_SUCCESS);
    }

    // start application
    {
        // xlib::core::Application
        xlib::core::Application xapplication;
        xapplication.dirsCreate();

        // qtlib::Application
        qtlib::Application application(argc, argv);

        qtlib::Application::setOrganizationName(ORG_NAME);
        qtlib::Application::setApplicationName(APP_NAME);
        qtlib::Application::setQuitOnLastWindowClosed(false);

        Main dlgMain;
        dlgMain.show();

        exitCode = application.exec();
    }

    return exitCode;
}
//-------------------------------------------------------------------------------------------------
