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
    public xl::package:: Application
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

//    xl::core::UserApplication::setName(APP_NAME);
//    xl::core::UserApplication::setDecription(APP_DESCRIPTION);
//    xl::core::UserApplication::setCopyrightYears(APP_COPYRIGHT_YEARS);
//    xl::core::UserApplication::setUsage(APP_USAGE);
//    xl::core::UserApplication::setHelp(APP_HELP);
//    xl::core::UserApplication::setVersionMajor(APP_VERSION_MAJOR);
//    xl::core::UserApplication::setVersionMinor(APP_VERSION_MINOR);
//    xl::core::UserApplication::setVersionPatch(APP_VERSION_PATCH);
//    xl::core::UserApplication::setVersionType(APP_VERSION_TYPE);
//    xl::core::UserApplication::setVersionRevision(APP_VERSION_REVISION);
//    xl::core::UserApplication::setVendorName(APP_VENDOR_NAME);
//    xl::core::UserApplication::setVendorDomain(APP_VENDOR_DOMAIN);
//    xl::core::UserApplication::setVendorAuthor(APP_VENDOR_AUTHOR);
//    xl::core::UserApplication::setVendorUrl(APP_VENDOR_URL);
//    xl::core::UserApplication::setVendorEmail(APP_VENDOR_EMAIL);
//    xl::core::UserApplication::setVendorSkype(APP_VENDOR_SKYPE);
//    xl::core::UserApplication::setVendorJabber(APP_VENDOR_JABBER);
//    xlib::core::UserApplication::setVendorIcq(APP_VENDOR_ICQ);

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
