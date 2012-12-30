#---------------------------------------------------------------------------
#
# \file  iTeacher.pro
# \brief project file
#
#---------------------------------------------------------------------------


TARGET              = iTeacher
TEMPLATE            = app
QT                  = core gui sql network xml phonon
CONFIG             += warn_on

win32 {
    QMAKE_CXXFLAGS -= -Zc:wchar_t-
    QMAKE_CXXFLAGS += -Zc:wchar_t
    QMAKE_CFLAGS   -= -Zc:wchar_t-
    QMAKE_CFLAGS   += -Zc:wchar_t

    INCLUDEPATH     = "$$(xLib)/Project/Include"

    CONFIG(debug, debug|release) {
        LIBS        = "$$(xLib)/Build/Libs/VC++2010/Debug_Unicode/StaticLib_VC++2010.lib"
    } else {
        LIBS        = "$$(xLib)/Build/Libs/VC++2010/Release_Unicode/StaticLib_VC++2010.lib"
    }

    LIBS           += User32.lib Ole32.lib Advapi32.lib shell32.lib
}
unix {
    INCLUDEPATH     =

    CONFIG(debug, debug|release) {
        LIBS        = -lxlib_d
    } else {
        LIBS        = -lxlib_r
    }

    LIBS           +=
}

HEADERS             = Config.h \
                      QtLib/Common.h \
                      QtLib/CUtils.h \
                      QtLib/CSqlNavigator.h \
                      Classes/CCheckBoxItemDelegate.h \
                      Classes/CCenteredCheckBox.h \
                      Forms/CMain.h \
                      Forms/CWordEditor.h \
                      Forms/CWordFinder.h

SOURCES             = main.cpp \
                      QtLib/CUtils.cpp \
                      QtLib/CSqlNavigator.cpp \
                      Classes/CCheckBoxItemDelegate.cpp \
                      Classes/CCenteredCheckBox.cpp \
                      Forms/CMain.cpp \
                      Forms/CWordEditor.cpp \
                      Forms/CWordFinder.cpp

FORMS               = Forms/CMain.ui \
                      Forms/CWordEditor.ui \
                      Forms/CWordFinder.ui

RESOURCES           = Resources/App.qrc

win32 {
    RC_FILE         = Resources/App_win.rc
}
unix {
    RC_FILE         = # n/a
}

OUT_DIR             =

CONFIG(debug, debug|release) {
    OUT_DIR         = ./Build/Debug
} else {
    OUT_DIR         = ./Build/Release
}

DESTDIR             = "$$OUT_DIR/Distr"
MOC_DIR             = "$$OUT_DIR/Temp"
OBJECTS_DIR         = "$$OUT_DIR/Temp"
RCC_DIR             = "$$OUT_DIR/Temp"
UI_DIR              = "$$OUT_DIR/Ui"

