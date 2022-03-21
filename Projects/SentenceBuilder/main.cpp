/**
 * \file  Main.cpp
 * \brief iTeacher application
 */


#include <xLib/xLib.h>
#include <xLib/Package/Application.h>
#include <QtLib/Application.h>
#include "Ui/Main.h"
//-------------------------------------------------------------------------------------------------
class UserApplication :
    public xl::package::Application
    /// user application
{
public:
    UserApplication(std::ctstring_t &a_appGuid, std::ctstring_t &a_locale) :
        Application(a_appGuid, a_locale)
    {
    }

    ExitCode
    onRun() override
    {
        // Failer().bug();

        return ExitCode::Success;
    }

private:
    xNO_COPY_ASSIGN(UserApplication)
};
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool bRv {};

    // xl::core::UserApplication
    UserApplication xapplication(xT(APP_GUID), xT(""));
    xapplication.dirsCreate();

    // qtlib::UserApplication
    qtlib::Application application(argc, argv, APP_GUID);
    bRv = application.isRunnig();
    if (bRv) {
        std::wcerr << "Application is already running." << std::endl;

        application.sendMessage("I am another instance");
    #if 0
        application.sendMessage("I want to send several message like argv if needed");
    #endif

        return EXIT_SUCCESS;
    }

    qtlib::Application::setQuitOnLastWindowClosed(false);

    // Main
    Main wndMain(nullptr);
    wndMain.show();

    return application.exec();
}
//-------------------------------------------------------------------------------------------------
