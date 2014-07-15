#---------------------------------------------------------------------------
# \file  iTeacher.pro
# \brief project file
#---------------------------------------------------------------------------


QT += \
    gui-private

INCLUDEPATH += \
    $$PWD

DEPENDPATH += \
    $$PWD

HEADERS += \
    $$PWD/QxtGlobal.h \
    $$PWD/QxtGlobalShortcut.h \
    $$PWD/QxtGlobalShortcut_p.h

SOURCES += \
    $$PWD/QxtGlobalShortcut.cpp

win32 {
    SOURCES += \
        $$PWD/QxtGlobalShortcut_win.cpp
    LIBS += \
        -luser32
}

unix {
    SOURCES += \
        $$PWD/QxtGlobalShortcut_x11.cpp
}

mac {
    SOURCES += \
        $$PWD/QxtGlobalShortcut_mac.cpp
}
