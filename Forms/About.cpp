/**
 * \file   About.cpp
 * \brief  about widget
 */


#include "About.h"

#include <QtLib/Common.h>
#include <QtLib/Utils.h>
#include <QtLib/Application.h>

#include <xLib/xLib.h>
#include <xLib/Core/Application.h>


/**************************************************************************************************
*   public
*
**************************************************************************************************/

xLIB_CORE_APPLICATION_STATIC_DECLARE

//-------------------------------------------------------------------------------------------------
About::About(
    QWidget *a_parent
) :
    QDialog(a_parent)
{
    _construct();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
About::_construct()
{
    _initMain();
}
//-------------------------------------------------------------------------------------------------
void
About::_destruct()
{
}
//-------------------------------------------------------------------------------------------------
void
About::_initMain()
{
    ui.setupUi(this);

    cQString msg = QString(tr("<p><b>%1</b></p>"))
                        .arg( qS2QS(xlib::core::Application::name()) )
                        .arg( qS2QS(xlib::core::Application::versionFull()) );

    ui.lblAppName->setText(msg);

    // build info
    {
        xlib::debug::BuildInfo info;

        cQString painText = QString(
            tr(
            "Build info:\n"
            "Type: %1\n"
            "Date time: %2\n"
            "Language standart: %3\n"
            "OS environment: %4\n"
            "OS: %5\n"
            "Architecture: %6\n"
            "Compiler: %7\n"
            "Character encoding: %8\n"
            "LibC: %9\n"
            "LibC++: %10\n"
            "xLib: %11\n"
            "Qt: %12\n"
            "Binary type: %13\n")
            .arg( info.isRelease() ? "Release" : "Debug" )
            .arg( info.datetime().c_str() )
            .arg( info.langStandart().c_str() )
            .arg( info.osEnvironment().c_str() )
            .arg( info.os().c_str() )
            .arg( info.arch().c_str() )
            .arg( info.compiler().c_str() )
            .arg( info.isUnicodeEncoding() ? "Unicode" : "Ansi" )
            .arg( info.stdLibC().c_str() )
            .arg( info.stdLibCpp().c_str() )
            .arg( info.xlibVersion().c_str() )
            .arg( QT_VERSION_STR )
            .arg( info.binaryType().c_str() ));

        ui.pteAbout->appendPlainText(painText);
    }

    //  signals, slots
    {
        connect(ui.bbxButtons, SIGNAL( clicked() ),
                this,          SLOT  ( slot_bbxButtons_onClicked() ));
    }
}
//-------------------------------------------------------------------------------------------------
void
About::slot_bbxButtons_onClicked(
    QAbstractButton *a_button
)
{
    QDialogButtonBox::StandardButton sbRv = ui.bbxButtons->standardButton(a_button);
    switch (sbRv) {
    case QDialogButtonBox::Ok:
    default:
        close();
        break;
    }
}
//-------------------------------------------------------------------------------------------------
