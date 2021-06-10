/**
 * \file  main.cpp
 * \brief main window
 */


#include <xLib/xLib.h>
#include <xLib/Package/Application.h>
#include <QtLib/Application.h>
#include "Forms/Main.h"
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
    bool bRv = false;

    // xl::core::UserApplication
    UserApplication xapplication(xT(APP_GUID), xT(""));
    xapplication.dirsCreate();

    // info
    {
        xl::package::ApplicationInfo appInfo;
        appInfo.name            = APP_NAME;
        appInfo.decription      = APP_DESCRIPTION;
        appInfo.copyrightYears  = APP_COPYRIGHT_YEARS;
        appInfo.usage           = APP_USAGE;
        appInfo.help            = APP_HELP;
        appInfo.versionMajor    = APP_VERSION_MAJOR;
        appInfo.versionMinor    = APP_VERSION_MINOR;
        appInfo.versionPatch    = APP_VERSION_PATCH;
        appInfo.versionType     = APP_VERSION_TYPE;
        appInfo.versionRevision = APP_VERSION_REVISION;
        appInfo.vendorName      = APP_VENDOR_NAME;
        appInfo.vendorDomain    = APP_VENDOR_DOMAIN;
        appInfo.vendorAuthor    = APP_VENDOR_AUTHOR;
        appInfo.vendorUrl       = APP_VENDOR_URL;
        appInfo.vendorEmail     = APP_VENDOR_EMAIL;
        appInfo.vendorSkype     = APP_VENDOR_SKYPE;
        appInfo.vendorJabber    = APP_VENDOR_JABBER;
        appInfo.vendorIcq       = APP_VENDOR_ICQ;

        xapplication.setInfo(appInfo);
    }

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

    Main dlgMain;
    // show window on CFG_VISIBLE option

    application.connect(&application, &qtlib::Application::sig_messageAvailable,
                        &dlgMain,     &Main::receiveFromOtherApplication);

    int exitCode = application.exec();

    return exitCode;
}
//-------------------------------------------------------------------------------------------------
/**
 * TODO:
 *
 * + Db to Git
 */
//-------------------------------------------------------------------------------------------------
