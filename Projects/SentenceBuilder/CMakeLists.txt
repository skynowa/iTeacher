#--------------------------------------------------------------------------------------------------
# \file  CMakeLists.txt
# \brief
#--------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.5.0)

project(SentenceBuilder)
#--------------------------------------------------------------------------------------------------
include(../Common.cmake)

add_executable(${PROJECT_NAME}
    # Main
    Config.h
    main.cpp

    # QtLib
    ${QTLIB_DIR}/Application.cpp
    ${QTLIB_DIR}/Db/SqlTableModelEx.cpp
    ${QTLIB_DIR}/Db/SqlRelationalTableModelEx.cpp
    ${QTLIB_DIR}/Db/SqlNavigator.cpp
    ${QTLIB_DIR}/SignalGuard.cpp
    ${QTLIB_DIR}/Utils.cpp

    # QtLib - GlobalShortcut
    ${QTLIB_DIR}/GlobalShortcut/GlobalShortcut.cpp
    ${QTLIB_DIR}/GlobalShortcut/x11/X11ErrorHandler.cpp
    ${QTLIB_DIR}/GlobalShortcut/x11/X11Data.cpp
    ${QTLIB_DIR}/GlobalShortcut/x11/ShortcutActivator.cpp

    ## Classes
    ${ROOT_DIR}/Classes/SqliteDb.cpp

    # Ui
    Ui/Main.cpp

    # Resources
    ${ROOT_DIR}/Resources/App.qrc

    # Doc
    Readme.md
    TODO.md)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    QT_NO_EMIT)

# TODO: ${XLIB_LIBRARIES}
target_link_libraries(${PROJECT_NAME}
    ${cmXLIB_LIBRARIES}
    ${cmXLIB_SYS_LIBRARIES}
    ${cmXLIB_EXTRA_LIBRARIES}
    -lX11)
#--------------------------------------------------------------------------------------------------
