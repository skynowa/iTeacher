/**
 * \file
 * \brief
 */


#ifndef QXTGLOBALSHORTCUT_P_H
#define QXTGLOBALSHORTCUT_P_H
//-------------------------------------------------------------------------------------------------
#include "QxtGlobalShortcut.h"
#include <QAbstractEventDispatcher>
#include <QKeySequence>
#include <QHash>
#include <QAbstractNativeEventFilter>
//-------------------------------------------------------------------------------------------------
class QxtGlobalShortcutPrivate :
        public QxtPrivate<QxtGlobalShortcut>
    #if !defined(Q_OS_MAC)
        , public QAbstractNativeEventFilter
    #endif
{
public:
    QXT_DECLARE_PUBLIC(QxtGlobalShortcut)

    QxtGlobalShortcutPrivate();
    ~QxtGlobalShortcutPrivate();

    bool enabled;
    Qt::Key key;
    Qt::KeyboardModifiers mods;

    bool setShortcut(const QKeySequence& shortcut);
    bool unsetShortcut();

    static bool error;

#ifndef Q_OS_MAC
    static int ref;

    virtual bool nativeEventFilter(const QByteArray & eventType, void * message, long * result);
#endif // Q_OS_MAC

    static void activateShortcut(quint32 nativeKey, quint32 nativeMods);

private:
    static quint32 nativeKeycode(Qt::Key keycode);
    static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);

    static bool registerShortcut(quint32 nativeKey, quint32 nativeMods);
    static bool unregisterShortcut(quint32 nativeKey, quint32 nativeMods);

    static QHash<QPair<quint32, quint32>, QxtGlobalShortcut*> shortcuts;
};
//-------------------------------------------------------------------------------------------------
#endif // QXTGLOBALSHORTCUT_P_H
