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
    bool bRv = false;

    // xlib::core::Application
    xlib::core::Application xapplication(xT(APP_GUID), xT(""));
    xapplication.dirsCreate();

    xlib::core::Application::setName(APP_NAME);
    xlib::core::Application::setDecription(APP_DESCRIPTION);
    xlib::core::Application::setCopyrightYears(APP_COPYRIGHT_YEARS);
    xlib::core::Application::setUsage(APP_USAGE);
    xlib::core::Application::setHelp(APP_HELP);
    xlib::core::Application::setVersionMajor(APP_VERSION_MAJOR);
    xlib::core::Application::setVersionMinor(APP_VERSION_MINOR);
    xlib::core::Application::setVersionPatch(APP_VERSION_PATCH);
    xlib::core::Application::setVersionType(APP_VERSION_TYPE);
    xlib::core::Application::setVersionRevision(APP_VERSION_REVISION);
    xlib::core::Application::setVendorName(APP_VENDOR_NAME);
    xlib::core::Application::setVendorDomain(APP_VENDOR_DOMAIN);
    xlib::core::Application::setVendorAuthor(APP_VENDOR_AUTHOR);
    xlib::core::Application::setVendorUrl(APP_VENDOR_URL);
    xlib::core::Application::setVendorEmail(APP_VENDOR_EMAIL);
    xlib::core::Application::setVendorSkype(APP_VENDOR_SKYPE);

    // qtlib::Application
    qtlib::Application application(argc, argv, APP_GUID);
    bRv = application.isRunnig();
    if (bRv) {
        std::wcerr << "Application is already running." << std::endl;
        application.windowActivate(APP_WINDOW_CLASS, APP_NAME);

        return EXIT_SUCCESS;
    }

    qtlib::Application::setQuitOnLastWindowClosed(false);

    Main dlgMain;
    dlgMain.show();

    int exitCode = application.exec();

    return exitCode;
}
//-------------------------------------------------------------------------------------------------
