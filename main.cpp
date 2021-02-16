/**
 * \file  main.cpp
 * \brief main window
 */


#include <xLib/xLib.h>
#include <xLib/Package/Application.h>
#include <QtLib/Application.h>
#include "Forms/Main.h"

#include <X11/Xlib.h>
#include <xcb/xcb.h>
//-------------------------------------------------------------------------------------------------
class NativeFilter : public QAbstractNativeEventFilter {
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
};

bool NativeFilter::nativeEventFilter(const QByteArray &a_eventType, void *a_message, long *a_result)
{
/* On Windows we interceot the click in the title bar. */
/* If we wait for the minimize event, it is already too late. */
#ifdef Q_OS_WIN
    auto msg = static_cast<MSG *>(message);
    // Filter out the event when the minimize button is pressed.
    if (msg->message == WM_NCLBUTTONDOWN && msg->wParam == HTREDUCE)
        return true;
#endif

/* Example macOS code from Qt doc, adapt to your need */
#ifdef Q_OS_MACOS
    if (eventType == "mac_generic_NSEvent") {
        NSEvent *event = static_cast<NSEvent *>(message);
        if ([event type] == NSKeyDown) {
            // Handle key event
            qDebug() << QString::fromNSString([event characters]);
        }
}
#endif

    qTRACE_SCOPE_FUNC;

    qTEST(a_eventType.size() > 0);
    qTEST_PTR(a_message);
    qTEST_PTR(a_result);

    Q_UNUSED(a_result);

    xcb_key_press_event_t *kev {};

    // qDebug() << qTRACE_VAR(a_eventType);

    if (a_eventType == "xcb_generic_event_t") {
        auto *event = static_cast<xcb_generic_event_t *>(a_message);
        // qDebug() << qTRACE_VAR(event->response_type);
        if ((event->response_type & 127) == XCB_KEY_PRESS) {
            kev = static_cast<xcb_key_press_event_t *>(a_message);
        }
    }

    if (kev != Q_NULLPTR) {
        unsigned int keycode  {kev->detail};
        unsigned int keystate {0};

        if (kev->state & XCB_MOD_MASK_1) {
            keystate |= Mod1Mask;
        }
        if (kev->state & XCB_MOD_MASK_CONTROL) {
            keystate |= ControlMask;
        }
        if (kev->state & XCB_MOD_MASK_4) {
            keystate |= Mod4Mask;
        }
        if (kev->state & XCB_MOD_MASK_SHIFT) {
            keystate |= ShiftMask;
        }

        qDebug() << ">>> activateShortcut <<<";
    }

    return false;

    return false;
}
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

        return static_cast<ExitCode>(0);
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


    application.installNativeEventFilter(new NativeFilter());

    qtlib::Application::setQuitOnLastWindowClosed(false);

    Main dlgMain;
    // show window on CFG_VISIBLE option

    application.connect(&application, &qtlib::Application::sig_messageAvailable,
                        &dlgMain,     &Main::receiveFromOtherApplication);

    int exitCode = application.exec();

    return exitCode;
}
//-------------------------------------------------------------------------------------------------
