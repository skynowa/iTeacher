/**
 * \file
 * \brief
 */


#include "QxtGlobalShortcut.h"

#include "QxtGlobalShortcut_p.h"
#include <QAbstractEventDispatcher>
#include <QtDebug>


//-------------------------------------------------------------------------------------------------
#ifndef Q_OS_MAC
    int QxtGlobalShortcutPrivate::ref = 0;
#endif

QHash<QPair<quint32, quint32>, QxtGlobalShortcut*> QxtGlobalShortcutPrivate::shortcuts;
//-------------------------------------------------------------------------------------------------
QxtGlobalShortcutPrivate::QxtGlobalShortcutPrivate() :
    enabled(true),
    key    (Qt::Key(0)),
    mods   (Qt::NoModifier)
{
#ifndef Q_OS_MAC
    if (ref == 0) {
        QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
    }

    ++ ref;
#endif
}
//-------------------------------------------------------------------------------------------------
QxtGlobalShortcutPrivate::~QxtGlobalShortcutPrivate()
{
#ifndef Q_OS_MAC
    -- ref;

    if (ref == 0) {
        QAbstractEventDispatcher *ed = QAbstractEventDispatcher::instance();
        if (ed != 0) {
            ed->removeNativeEventFilter(this);
        }
    }
#endif
}
//-------------------------------------------------------------------------------------------------
bool QxtGlobalShortcutPrivate::setShortcut(const QKeySequence& shortcut)
{
    Qt::KeyboardModifiers allMods = Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier;
    key = shortcut.isEmpty() ? Qt::Key(0) : Qt::Key((shortcut[0] ^ allMods) & shortcut[0]);
    mods = shortcut.isEmpty() ? Qt::KeyboardModifiers(0) : Qt::KeyboardModifiers(shortcut[0] & allMods);

    const quint32 nativeKey = nativeKeycode(key);
    const quint32 nativeMods = nativeModifiers(mods);

    const bool res = registerShortcut(nativeKey, nativeMods);
    if (res) {
        shortcuts.insert(qMakePair(nativeKey, nativeMods), &qxt_p());
        qWarning() << "QxtGlobalShortcut - OK:" << QKeySequence(key + mods).toString();
    } else {
        qWarning() << "QxtGlobalShortcut failed to register:" << QKeySequence(key + mods).toString();
    }

    return res;
}
//-------------------------------------------------------------------------------------------------
bool QxtGlobalShortcutPrivate::unsetShortcut()
{
    bool res = false;
    const quint32 nativeKey = nativeKeycode(key);
    const quint32 nativeMods = nativeModifiers(mods);

    if (shortcuts.value(qMakePair(nativeKey, nativeMods)) == &qxt_p()) {
        res = unregisterShortcut(nativeKey, nativeMods);
    }

    if (res) {
        shortcuts.remove(qMakePair(nativeKey, nativeMods));
    } else {
        qWarning() << "QxtGlobalShortcut failed to unregister:" << QKeySequence(key + mods).toString();
    }

    key = Qt::Key(0);
    mods = Qt::KeyboardModifiers(0);

    return res;
}
//-------------------------------------------------------------------------------------------------
void QxtGlobalShortcutPrivate::activateShortcut(quint32 nativeKey, quint32 nativeMods)
{
    QxtGlobalShortcut* shortcut = shortcuts.value(qMakePair(nativeKey, nativeMods));
    if (shortcut && shortcut->isEnabled()) {
        emit shortcut->activated();
    }
}
//-------------------------------------------------------------------------------------------------
/*!
    \class QxtGlobalShortcut
    \inmodule QxtWidgets
    \brief The QxtGlobalShortcut class provides a global shortcut aka "hotkey".

    A global shortcut triggers even if the application is not active. This
    makes it easy to implement applications that react to certain shortcuts
    still if some other application is active or if the application is for
    example minimized to the system tray.

    Example usage:
    \code
    QxtGlobalShortcut* shortcut = new QxtGlobalShortcut(window);
    connect(shortcut, SIGNAL(activated()), window, SLOT(toggleVisibility()));
    shortcut->setShortcut(QKeySequence("Ctrl+Shift+F12"));
    \endcode

    \bold {Note:} Since Qxt 0.6 QxtGlobalShortcut no more requires QxtApplication.
 */

/*!
    \fn QxtGlobalShortcut::activated()

    This signal is emitted when the user types the shortcut's key sequence.

    \sa shortcut
 */

/*!
    Constructs a new QxtGlobalShortcut with \a parent.
 */
QxtGlobalShortcut::QxtGlobalShortcut(QObject* parent)
        : QObject(parent)
{
    QXT_INIT_PRIVATE(QxtGlobalShortcut);
}
//-------------------------------------------------------------------------------------------------
/*!
    Constructs a new QxtGlobalShortcut with \a shortcut and \a parent.
 */
QxtGlobalShortcut::QxtGlobalShortcut(const QKeySequence& shortcut, QObject* parent)
        : QObject(parent)
{
    QXT_INIT_PRIVATE(QxtGlobalShortcut);

    setShortcut(shortcut);
}
//-------------------------------------------------------------------------------------------------
/*!
    Destructs the QxtGlobalShortcut.
 */
QxtGlobalShortcut::~QxtGlobalShortcut()
{
    if (qxt_d().key != 0) {
        qxt_d().unsetShortcut();
    }
}
//-------------------------------------------------------------------------------------------------
/*!
    \property QxtGlobalShortcut::shortcut
    \brief the shortcut key sequence

    \bold {Note:} Notice that corresponding key press and release events are not
    delivered for registered global shortcuts even if they are disabled.
    Also, comma separated key sequences are not supported.
    Only the first part is used:

    \code
    qxtShortcut->setShortcut(QKeySequence("Ctrl+Alt+A,Ctrl+Alt+B"));
    Q_ASSERT(qxtShortcut->shortcut() == QKeySequence("Ctrl+Alt+A"));
    \endcode
 */
QKeySequence QxtGlobalShortcut::shortcut() const
{
    return QKeySequence(qxt_d().key | qxt_d().mods);
}
//-------------------------------------------------------------------------------------------------
bool QxtGlobalShortcut::setShortcut(const QKeySequence& shortcut)
{
    if (qxt_d().key != 0)
        qxt_d().unsetShortcut();

    return qxt_d().setShortcut(shortcut);
}
//-------------------------------------------------------------------------------------------------
/*!
    \property QxtGlobalShortcut::enabled
    \brief whether the shortcut is enabled

    A disabled shortcut does not get activated.

    The default value is \c true.

    \sa setDisabled()
 */
bool QxtGlobalShortcut::isEnabled() const
{
    return qxt_d().enabled;
}
//-------------------------------------------------------------------------------------------------
void QxtGlobalShortcut::setEnabled(bool enabled)
{
    qxt_d().enabled = enabled;
}
//-------------------------------------------------------------------------------------------------
/*!
    Sets the shortcut \a disabled.

    \sa enabled
 */
void QxtGlobalShortcut::setDisabled(bool disabled)
{
    qxt_d().enabled = !disabled;
}
//-------------------------------------------------------------------------------------------------
