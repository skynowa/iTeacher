/**
 * \file  main.cpp
 * \brief main window
 */


#include <QApplication>
#include "Forms/CMain.h"

//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int iExitCode = EXIT_FAILURE;

#if defined(Q_WS_WIN)
    cQByteArray codecName = "UTF-8";    // "Windows-1251";
#else
    cQByteArray codecName = "UTF-8";
#endif

    // activation application window
    CUtils::applicationActivate(APP_WINDOW_CLASS, APP_NAME);

    // set application single inststance
    {
        bool bRv = CUtils::setApplicationSingle(APP_GUID);
        qCHECK_RET(false == bRv, EXIT_SUCCESS);
    }

    // start application
    {
        QApplication apStart(argc, argv);

        QCoreApplication::setOrganizationName(ORG_NAME);
        QCoreApplication::setApplicationName(APP_NAME);

        CMain dlgMain;
        dlgMain.show();

        iExitCode = apStart.exec();
    }

    return iExitCode;
}
//-------------------------------------------------------------------------------------------------
