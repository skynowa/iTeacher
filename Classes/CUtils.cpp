/**
 * \file   CUtils.cpp
 * \brief
 */


#include "CUtils.h"


#if defined(Q_WS_WIN)
    #include <windows.h>
#else

#endif

#include <QImageWriter>



/****************************************************************************
*   public
*
*****************************************************************************/

//---------------------------------------------------------------------------
/* static */
bool
CUtils::setApplicationSingle(
    const QString &a_applicationGuid
)
{
    bool bRv = false;

    static QSharedMemory smLocker(a_applicationGuid);

    bRv = smLocker.attach();
    qCHECK_RET(true == bRv, false);

    bRv = smLocker.create(1);
    qCHECK_RET(false == bRv, false);

    return true;
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::widgetAlignCenter(
    QWidget *a_widget
)
{
    Q_ASSERT(NULL != a_widget);

#if 0
    QDesktopWidget *desktop = QApplication::desktop();
    Q_ASSERT(NULL != desktop);

    const int x = (desktop->width()  - a_widget->width())  / 2;
    const int y = (desktop->height() - a_widget->height()) / 2;

    a_widget->setGeometry(x, y, a_widget->width(), a_widget->height());
#endif

    QRect rcRect = QStyle::alignedRect(
                        Qt::LeftToRight, Qt::AlignCenter, a_widget->size(),
                        qApp->desktop()->availableGeometry());

    a_widget->setGeometry(rcRect);
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::widgetAlignTopCenter(
    QWidget *a_widget
)
{
    Q_ASSERT(NULL != a_widget);

    QDesktopWidget *desktop = QApplication::desktop();
    Q_ASSERT(NULL != desktop);

    const int x = (desktop->width() - a_widget->width()) / 2;
    const int y = 0;

    a_widget->setGeometry(x, y, a_widget->width(), a_widget->height());
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::applicationActivate(
    const QString &a_className,
    const QString &a_windowName
)
{
#if defined(Q_WS_WIN)
    HWND hWnd = ::FindWindowW(a_className.toStdWString().c_str(), a_windowName.toStdWString().c_str());
    if (NULL != hWnd) {
        BOOL blRv = ::SetForegroundWindow(hWnd);
        Q_ASSERT((BOOL)FALSE != blRv);

        ::Beep(400, 400);
    }
#else
    // TODO: activation application window
#endif
}
//---------------------------------------------------------------------------


/****************************************************************************
*   private
*
*****************************************************************************/

//---------------------------------------------------------------------------
CUtils::CUtils() {

}
//---------------------------------------------------------------------------
CUtils::~CUtils() {

}
//---------------------------------------------------------------------------
