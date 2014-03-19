#---------------------------------------------------------------------------
#
# \file  iTeacher.pro
# \brief project file
#
#---------------------------------------------------------------------------


TARGET              = iTeacher
TEMPLATE            = app
QT                  = core gui widgets sql network xml printsupport
CONFIG             += debug warn_on

win32 {
    #QMAKE_CXXFLAGS -= -Zc:wchar_t-
    #QMAKE_CXXFLAGS += -Zc:wchar_t
    #QMAKE_CFLAGS   -= -Zc:wchar_t-
    #QMAKE_CFLAGS   += -Zc:wchar_t

    INCLUDEPATH     =  xLib/Include
    #LIBS           += User32.lib Ole32.lib Advapi32.lib shell32.lib
    LIBS            += -lpsapi -luuid -lole32 -lmpr
}
unix {
    INCLUDEPATH     = xLib/Include
    LIBS           += -lX11 -ldl #-lc (FreeBSD)
}

HEADERS             = Config.h \
                      QtLib/Common.h \
                      QtLib/CApplication.h \
                      QtLib/CUtils.h \
                      QtLib/CSqlNavigator.h \
                      QtLib/QxGlobalShortcut_x11.h \
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
                      QtLib/QxGlobalShortcut_x11.cpp \
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

CONFIG(debug, debug | release) {
    DISTR_DIR       = Distr/Debug
    OUT_DIR         = Build/Debug
} else {
    DISTR_DIR       = Distr/Release
    OUT_DIR         = Build/Release
}

DESTDIR             = $$DISTR_DIR
MOC_DIR             = $$OUT_DIR/Temp
OBJECTS_DIR         = $$OUT_DIR/Temp
RCC_DIR             = $$OUT_DIR/Temp
UI_DIR              = $$OUT_DIR/Ui
