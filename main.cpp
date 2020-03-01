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

    int
    onRun() override
    {
        // Failer().bug();

        return 0;
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
        xl::package::ApplicationInfoData appInfoData;
        appInfoData.name            = APP_NAME;
        appInfoData.decription      = APP_DESCRIPTION;
        appInfoData.copyrightYears  = APP_COPYRIGHT_YEARS;
        appInfoData.usage           = APP_USAGE;
        appInfoData.help            = APP_HELP;
        appInfoData.versionMajor    = APP_VERSION_MAJOR;
        appInfoData.versionMinor    = APP_VERSION_MINOR;
        appInfoData.versionPatch    = APP_VERSION_PATCH;
        appInfoData.versionType     = APP_VERSION_TYPE;
        appInfoData.versionRevision = APP_VERSION_REVISION;
        appInfoData.vendorName      = APP_VENDOR_NAME;
        appInfoData.vendorDomain    = APP_VENDOR_DOMAIN;
        appInfoData.vendorAuthor    = APP_VENDOR_AUTHOR;
        appInfoData.vendorUrl       = APP_VENDOR_URL;
        appInfoData.vendorEmail     = APP_VENDOR_EMAIL;
        appInfoData.vendorSkype     = APP_VENDOR_SKYPE;
        appInfoData.vendorJabber    = APP_VENDOR_JABBER;
        appInfoData.vendorIcq       = APP_VENDOR_ICQ;

        xl::package::ApplicationInfo info(appInfoData);
        xapplication.setInfo(info);
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
