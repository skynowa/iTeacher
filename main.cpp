/**
 * \file  main.cpp
 * \brief main window
 */


#include <QtGui/QApplication>
#include "Forms/CMain.h"

//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#if !xCOMPILER_MS && !xCOMPILER_GNUC
    #error Start: not supported compiler
#endif

    int iExitCode = EXIT_FAILURE;

#if defined(Q_WS_WIN)
    const QByteArray codecName = "Windows-1251";
#else
    const QByteArray codecName = "UTF-8";
#endif

    QTextCodec *codec = QTextCodec::codecForName(codecName);
    Q_ASSERT(NULL != codec);

    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);

    // activation application window
    CUtils::applicationActivate(CONFIG_APP_WINDOW_CLASS, CONFIG_APP_NAME);

    // set application single inststance
    {
        bool bRv = CUtils::setApplicationSingle(CONFIG_GUID);
        xCHECK_RET(false == bRv, EXIT_SUCCESS);
    }

    // start application
    {
        QApplication apStart(argc, argv);

        QCoreApplication::setOrganizationName(CONFIG_ORG_NAME);
        QCoreApplication::setApplicationName( apStart.applicationFilePath() );

        CMain dlgMain;
        dlgMain.show();

        iExitCode = apStart.exec();
    }

    return iExitCode;
}
//---------------------------------------------------------------------------
