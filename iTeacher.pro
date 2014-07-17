#---------------------------------------------------------------------------
#
# \file  iTeacher.pro
# \brief project file
#
#---------------------------------------------------------------------------


# config
HAVE_XLIB       = 1

# project
TARGET          = iTeacher
TEMPLATE        = app
QT              = core gui widgets sql network xml printsupport multimedia
CONFIG         += debug warn_on

# xLib
!isEmpty(HAVE_XLIB) {
    INCLUDEPATH = xLib/Include

    win32 {
        LIBS   += User32.lib Ole32.lib Advapi32.lib shell32.lib
        LIB    += -lpsapi -luuid -lole32 -lmpr
    }
    unix {
        LIBS   += -ldl #-lc (FreeBSD)
    }
}

HEADERS = \
    Config.h \
    QtLib/Common.h \
    Forms/Main.h \
    Forms/WordEditor.h \
    Forms/WordFinder.h \
    Classes/CenteredCheckBox.h \
    Classes/CheckBoxItemDelegate.h \
    Classes/ComboBoxItemDelegate.h \
    QtLib/Application.h \
    QtLib/SqlNavigator.h \
    QtLib/Utils.h

SOURCES = \
    main.cpp \
    Forms/Main.cpp \
    Forms/WordEditor.cpp \
    Forms/WordFinder.cpp \
    Classes/CenteredCheckBox.cpp \
    Classes/CheckBoxItemDelegate.cpp \
    Classes/ComboBoxItemDelegate.cpp \
    QtLib/Application.cpp \
    QtLib/SqlNavigator.cpp \
    QtLib/Utils.cpp

# includes
include(QtLib/QxtGlobalShortcut/QxtGlobalShortcut.pri)

FORMS = \
    Forms/Main.ui \
    Forms/WordEditor.ui \
    Forms/WordFinder.ui

RESOURCES = \
    Resources/App.qrc

win32 {
    RC_FILE = \
        Resources/App_win.rc
}

CONFIG(debug, debug | release) {
    DISTR_DIR   = Distr/Debug
    OUT_DIR     = Build/Debug
} else {
    DISTR_DIR   = Distr/Release
    OUT_DIR     = Build/Release
}

DESTDIR         = $$DISTR_DIR
MOC_DIR         = $$OUT_DIR/Temp
OBJECTS_DIR     = $$OUT_DIR/Temp
RCC_DIR         = $$OUT_DIR/Temp
UI_DIR          = $$OUT_DIR/Ui
