#---------------------------------------------------------------------------
#
# \file  iTeacher.pro
# \brief project file
#
#---------------------------------------------------------------------------


TARGET              = iTeacher
TEMPLATE            = app
QT                  = core gui widgets sql network xml printsupport
CONFIG             += warn_on

win32 {
    #QMAKE_CXXFLAGS -= -Zc:wchar_t-
    #QMAKE_CXXFLAGS += -Zc:wchar_t
    #QMAKE_CFLAGS   -= -Zc:wchar_t-
    #QMAKE_CFLAGS   += -Zc:wchar_t

    INCLUDEPATH     = "$$(xLib)/Project/Include"

    CONFIG(debug, debug|release) {
        LIBS        = "C:/Program Files (x86)/xLib/bin/libxLib_static.a"
    } else {
        LIBS        = "C:/Program Files (x86)/xLib/bin/libxLib_static.a"
    }

    #LIBS           += User32.lib Ole32.lib Advapi32.lib shell32.lib
    LIBS            += -lpsapi -luuid -lole32 -lmpr
}
unix {
    INCLUDEPATH     =

    CONFIG(debug, debug|release) {
        LIBS        = /usr/local/bin/libxLib_static.a -ldl
    } else {
        LIBS        = /usr/local/bin/libxLib_static.a -ldl
    }

    LIBS           +=
}

HEADERS             = Config.h \
                      QtLib/Common.h \
                      QtLib/CApplication.h \
                      QtLib/CUtils.h \
                      QtLib/CSqlNavigator.h \
                      Classes/CCenteredCheckBox.h \
                      Classes/CCheckBoxItemDelegate.h \
                      Classes/CComboBoxItemDelegate.h \
                      Forms/CMain.h \
                      Forms/CWordEditor.h \
                      Forms/CWordFinder.h

SOURCES             = main.cpp \
                      QtLib/CApplication.cpp \
                      QtLib/CUtils.cpp \
                      QtLib/CSqlNavigator.cpp \
                      Classes/CCenteredCheckBox.cpp \
                      Classes/CCheckBoxItemDelegate.cpp \
                      Classes/CComboBoxItemDelegate.cpp \
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

