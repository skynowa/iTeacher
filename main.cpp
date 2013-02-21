/**
 * \file  main.cpp
 * \brief main window
 */


#include <QtGui/QApplication>
#include "Forms/CMain.h"

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#if !xCOMPILER_MS && !xCOMPILER_GNUC
    #error Start: not supported compiler
#endif

    int iExitCode = EXIT_FAILURE;

#if defined(Q_WS_WIN)
    cQByteArray codecName = "UTF-8";    // "Windows-1251";
#else
    cQByteArray codecName = "UTF-8";
#endif

    QTextCodec *codec = QTextCodec::codecForName(codecName);
    Q_ASSERT(NULL != codec);

    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);

    // activation application window
    CUtils::applicationActivate(APP_WINDOW_CLASS, APP_NAME);

    // set application single inststance
    {
        bool bRv = CUtils::setApplicationSingle(APP_GUID);
        xCHECK_RET(false == bRv, EXIT_SUCCESS);
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
//------------------------------------------------------------------------------
