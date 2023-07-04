#--------------------------------------------------------------------------------------------------
# \file  CMakeLists.txt
# \brief
#--------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.5.0)

project(iTeacher)
#--------------------------------------------------------------------------------------------------
include(../Common.cmake)

add_executable(${PROJECT_NAME}
    # Main
    Config.h
    main.cpp

    # QtLib
    ${ROOT_DIR}/QtLib/Ui/About.cpp
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
    ${ROOT_DIR}/Classes/CenteredCheckBox.cpp
    ${ROOT_DIR}/Classes/ColorItemDelegate.cpp
    ${ROOT_DIR}/Classes/CheckBoxItemDelegate.cpp
    ${ROOT_DIR}/Classes/ComboBoxItemDelegate.cpp
    ${ROOT_DIR}/Classes/Hint.cpp
    ${ROOT_DIR}/Classes/SqliteDb.cpp

    # Ui
    Ui/Main.cpp
    Ui/WordEditor.cpp
    Ui/WordFinder.cpp
    Ui/TagsEditor.cpp
    Ui/Settings.cpp

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
# install
# if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ".")
# else()
#     set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/Bin)
# endif()

foreach(QT5_MODULE ${_QT5_MODULES})
    GET_TARGET_PROPERTY(DLL Qt5::${QT5_MODULE} LOCATION_${CMAKE_BUILD_TYPE})
    LIST(APPEND DLLS_REQUIRED "${DLL}")

    # message("Qt5::${QT5_MODULE} - ${DLL}\n")
endforeach()

# target
install(
    TARGETS
        ${PROJECT_NAME}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX})

# DLLs
function(install_files)
    foreach(FILE ${ARGV})
        get_filename_component(NAME_NEW ${FILE} NAME_WE)

        if     (ENV_WIN)
            set(NAME_NEW ${NAME_NEW}.dll)
        elseif (ENV_UNIX)
            set(NAME_NEW ${NAME_NEW}.so)
        elseif (ENV_APPLE)
            set(NAME_NEW ${NAME_NEW}.dylib)
        endif()

        install(FILES ${FILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/Libs RENAME ${NAME_NEW})
    endforeach()
endfunction()

install_files(${DLLS_REQUIRED})

# Doc
install(
    FILES
        ${CMAKE_SOURCE_DIR}/Readme.md
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/Doc)

# Resources
install(
    FILES
        ${CMAKE_SOURCE_DIR}/Resources/App.ico
        ${CMAKE_SOURCE_DIR}/Resources/App.png
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/Resources)
#--------------------------------------------------------------------------------------------------
