#--------------------------------------------------------------------------------------------------
# \file  CMakeLists.txt
# \brief
#--------------------------------------------------------------------------------------------------


project(iTeacher)
cmake_minimum_required(VERSION 2.8)

#--------------------------------------------------------------------------------------------------
# options
set(CMAKE_AUTOMOC               ON)
set(CMAKE_INCLUDE_CURRENT_DIR   ON)

set(PROJECT_TYPE                "CXX")
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE            "RelWithDebInfo")
    # None (CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used)
    # Debug (CMAKE_C_FLAGS_DEBUG or CMAKE_CXX_FLAGS_DEBUG)
    # Release (CMAKE_C_FLAGS_RELEASE or CMAKE_CXX_FLAGS_RELEASE)
    # RelWithDebInfo (CMAKE_C_FLAGS_RELWITHDEBINFO or CMAKE_CXX_FLAGS_RELWITHDEBINFO
    # MinSizeRel (CMAKE_C_FLAGS_MINSIZEREL or CMAKE_CXX_FLAGS_MINSIZEREL)
set(QT_QMAKE_EXECUTABLE         "") # hide warning
#--------------------------------------------------------------------------------------------------
# modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

# CMakeLib - CMAKE_MODULE_PATH
set(CMAKE_MAJOR_MINOR_VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
set(CMAKE_DATA_DIR_DEFAULT    "/usr/share/cmake-${CMAKE_MAJOR_MINOR_VERSION}/Modules")
set(CMAKE_MODULE_PATH
    "${CMAKE_DATA_DIR_DEFAULT}/CMakeLib;"
    "${CMAKE_DATA_DIR_DEFAULT}/CMakeLib/Qt;"
    "${CMAKE_MODULE_PATH}")
#--------------------------------------------------------------------------------------------------
# checks
# Check if Qtversion is >=QT_OFFICIAL_VERSION.
# If so, we are good. Otherwise we will post a warning of versions (<QT_OFFICIAL_VERSION)
string(REGEX MATCH "^5\\.[3]\\.[0-1]+" QT_VERSION_MATCH "${Qt5Core_VERSION_STRING}")
    if (QT_VERSION_MATCH)
    message(WARNING "Warning: You are using Qt ${Qt5Core_VERSION_STRING}. "
                    "Officially supported version is Qt ${QT_OFFICIAL_VERSION}")
endif()
#--------------------------------------------------------------------------------------------------
# find packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules/CMakeLib")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(xLib REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Qt5 REQUIRED)

include_directories(
    ${LIBXML2_INCLUDE_DIR}
    ${XLIB_INCLUDES}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/QtLib
    ${CMAKE_SOURCE_DIR}/QtLib/Ui
    ${CMAKE_SOURCE_DIR}/QtLib/Db
    ${CMAKE_SOURCE_DIR}/Classes
    ${CMAKE_SOURCE_DIR}/Ui)

qt5_wrap_ui(MOC_UI
    QtLib/Ui/About.ui
    Ui/Main.ui
    Ui/WordEditor.ui
    Ui/WordFinder.ui
    Ui/TagsEditor.ui
    Ui/Settings.ui)

qt5_wrap_cpp(MOC_CPP)

qt5_add_resources(RESOURCES Resources/App.qrc)

# Add -fPIE
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_executable(${PROJECT_NAME}
    # Main
    Config.h
    ${PROJECT_NAME}.cpp

    # QtLib
    QtLib/Ui/About.cpp
    QtLib/Application.cpp
    QtLib/Db/SqlTableModelEx.cpp
    QtLib/Db/SqlRelationalTableModelEx.cpp
    QtLib/Db/SqlNavigator.cpp
    QtLib/SignalGuard.cpp
    QtLib/Utils.cpp

    # QtLib - GlobalShortcut
    QtLib/GlobalShortcut/GlobalShortcut.cpp
    QtLib/GlobalShortcut/x11/X11ErrorHandler.cpp
    QtLib/GlobalShortcut/x11/X11Data.cpp
    QtLib/GlobalShortcut/x11/ShortcutActivator.cpp

    ## Classes
    Classes/CenteredCheckBox.cpp
    Classes/ColorItemDelegate.cpp
    Classes/CheckBoxItemDelegate.cpp
    Classes/ComboBoxItemDelegate.cpp
    Classes/Hint.cpp
    Classes/SqliteDb.cpp

    # Ui
    Ui/Main.cpp
    Ui/WordEditor.cpp
    Ui/WordFinder.cpp
    Ui/TagsEditor.cpp
    Ui/Settings.cpp

    # MOCs
    ${MOC_UI}
    ${MOC_CPP}
    ${RESOURCES}

    # Doc
    Readme.md
    TODO.md)

# TODO: ${XLIB_LIBRARIES}
target_link_libraries(${PROJECT_NAME} ${cmXLIB_LIBRARIES} -lX11)
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
# Targets
include(Trainer.cmake)
