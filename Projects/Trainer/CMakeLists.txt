#--------------------------------------------------------------------------------------------------
# \file  CMakeLists.txt
# \brief
#--------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.5.0)

project(Trainer)
#--------------------------------------------------------------------------------------------------
include(../Common.cmake)

add_executable(${PROJECT_NAME}
    # Main
    Config.h
    main.cpp

    # QtLib
    ${ROOT_DIR}/QtLib/Application.cpp
    ${ROOT_DIR}/QtLib/Db/SqlTableModelEx.cpp
    ${ROOT_DIR}/QtLib/Db/SqlRelationalTableModelEx.cpp
    ${ROOT_DIR}/QtLib/Db/SqlNavigator.cpp
    ${ROOT_DIR}/QtLib/SignalGuard.cpp
    ${ROOT_DIR}/QtLib/Utils.cpp

    # QtLib - GlobalShortcut
    ${ROOT_DIR}/QtLib/GlobalShortcut/GlobalShortcut.cpp
    ${ROOT_DIR}/QtLib/GlobalShortcut/x11/X11ErrorHandler.cpp
    ${ROOT_DIR}/QtLib/GlobalShortcut/x11/X11Data.cpp
    ${ROOT_DIR}/QtLib/GlobalShortcut/x11/ShortcutActivator.cpp

    ## Classes
    ${ROOT_DIR}/Classes/SqliteDb.cpp

    # Ui
    Ui/Main.cpp

    # Resources
    ${ROOT_DIR}/Resources/App.qrc

    # Doc
    Readme.md
    TODO.md)

# TODO: ${XLIB_LIBRARIES}
target_link_libraries(${PROJECT_NAME}
    ${cmXLIB_LIBRARIES}
    ${cmXLIB_SYS_LIBRARIES}
    ${cmXLIB_EXTRA_LIBRARIES}
    -lX11)
#--------------------------------------------------------------------------------------------------
