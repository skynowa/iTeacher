/**
 * \file  main.cpp
 * \brief main window
 */


#include <QApplication>
#include "Forms/Main.h"

//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int exitCode = EXIT_FAILURE;

    // activation application window
    qtlib::Utils::applicationActivate(APP_WINDOW_CLASS, APP_NAME);

    // set application single inststance
    {
    #if 0
        bool bRv = qtlib::CUtils::setApplicationSingle(APP_GUID);
        qCHECK_RET(false == bRv, EXIT_SUCCESS);
    #endif
    }

    // start application
    {
        QApplication application(argc, argv);

        QCoreApplication::setOrganizationName(ORG_NAME);
        QCoreApplication::setApplicationName(APP_NAME);
        QApplication::setQuitOnLastWindowClosed(false);

        Main dlgMain;
        dlgMain.show();

        exitCode = application.exec();
    }

    return exitCode;
}
//-------------------------------------------------------------------------------------------------
