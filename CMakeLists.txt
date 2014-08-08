#--------------------------------------------------------------------------------------------------
# \file  CPackConfig.cmake
# \brief
#--------------------------------------------------------------------------------------------------


# SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
# SET(CPACK_GENERATOR "STGZ;TGZ;TZ;DEB")
# SET(CPACK_INSTALL_CMAKE_PROJECTS "Distr;CMake;ALL") # ;/
# SET(CPACK_NSIS_DISPLAY_NAME "CMake 2.5")
# #SET(CPACK_OUTPUT_CONFIG_FILE "/home/andy/vtk/CMake-bin/CPackConfig.cmake")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "Readme.txt")
# SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMake is a build tool")
# SET(CPACK_PACKAGE_EXECUTABLES "ccmake;CMake")
# SET(CPACK_PACKAGE_FILE_NAME "cmake-2.5.0-Linux-i686")
# SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake 2.5")
# SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "CMake 2.5.0")
# SET(CPACK_PACKAGE_NAME "CMake")
# SET(CPACK_PACKAGE_VENDOR "Kitware")
# SET(CPACK_PACKAGE_VERSION "2.5.0")
# SET(CPACK_PACKAGE_VERSION_MAJOR "2")
# SET(CPACK_PACKAGE_VERSION_MINOR "5")
# SET(CPACK_PACKAGE_VERSION_PATCH "0")
# SET(CPACK_RESOURCE_FILE_LICENSE "Readme.txt")
# SET(CPACK_RESOURCE_FILE_README "Readme.txt")
# #SET(CPACK_RESOURCE_FILE_WELCOME "/home/andy/vtk/CMake/Templates/CPack.GenericWelcome.txt")
# SET(CPACK_SOURCE_GENERATOR "TGZ;TZ")
# #SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/home/andy/vtk/CMake-bin/CPackSourceConfig.cmake")
# SET(CPACK_SOURCE_PACKAGE_FILE_NAME "cmake-2.5.0")
# SET(CPACK_SOURCE_STRIP_FILES "")
# SET(CPACK_STRIP_FILES "bin/ccmake;bin/cmake;bin/cpack;bin/ctest")
# SET(CPACK_SYSTEM_NAME "Linux-i686")
# SET(CPACK_TOPLEVEL_TAG "Linux-i686")

# include(CPack)

########################################################################
# CPack configuration
########################################################################

# set(CPACK_PACKAGE_NAME "CPackExampleInstaller")
# set(CPACK_PACKAGE_VENDOR "Ziv Yaniv")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Example showing CPack in action")
# set(CPACK_PACKAGE_VERSION_MAJOR "1")
# set(CPACK_PACKAGE_VERSION_MINOR "0")
# set(CPACK_PACKAGE_VERSION_PATCH "0")
# set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "CPackExample")

# #
# # Tell CPack about the components and group the data components together (CPACK_COMPONENT_${COMPONENT_NAME_ALL_CAPS}_GROUP).
# #
# set(CPACK_COMPONENTS_ALL applications CTData instructionData)
# set(CPACK_COMPONENT_CTDATA_GROUP "data")
# set(CPACK_COMPONENT_INSTRUCTIONDATA_GROUP "data")

# #
# # More descriptive names for each of the components, and component groups
# #
# set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "applications (hello & world)")
# set(CPACK_COMPONENT_GROUP_DATA_DESCRIPTION "data (CT and instructions)")


# #
# # Text from "license.txt" is displayed in the installer's license tab
# #
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/Readme.txt")

# #
# # Set the icon used inside the installer wizard and the icons for the installer and un-installer
# #
# # set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\hw.bmp")
# # set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/hw.ico")
# # set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/hw.ico")


# #
# # Set the programs displayed under the Start menu shortcut
# # We do not use CPACK_PACKAGE_EXECUTABLES because it expects the binaries to be in a
# # directory named "bin" (CMake bug tracker ID: 0007829).
# #
# # set(CPACK_NSIS_MENU_LINKS "programs\\hello" "Hello"
# #                           "programs\\world" "World"
# #                           ${CT_DATA_FILE_DIR} "CT Data"
# #                           "data\\example.pdf" "documentation")

# include(CPack)



project(iTeacher)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_RELAXED_MODE ON)
# AUTOMOC_TARGETS_FOLDER
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Multimedia REQUIRED)
#find_package(Qt5GuiPrivate REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
include_directories(${Qt5Multimedia_INCLUDE_DIRS})


#INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )


# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Gui_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Sql_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})
add_definitions(${Qt5PrintSupport_DEFINITIONS})
add_definitions(${Qt5Multimedia_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration without -fPIE. We add that here
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")


include_directories(
    "${CMAKE_SOURCE_DIR}/QtLib"
    "${CMAKE_SOURCE_DIR}/QtLib/Forms"
    "${CMAKE_SOURCE_DIR}/Classes"
    "${CMAKE_SOURCE_DIR}/Forms"
)

QT5_WRAP_UI(MOC_UI
    QtLib/Forms/About.ui
    Forms/Main.ui
    Forms/WordEditor.ui
    Forms/WordFinder.ui
    Forms/TagsEditor.ui
    Forms/Settings.ui
)

QT5_WRAP_CPP(MOC_CPP
    QtLib/Forms/About.cpp
    Forms/Main.cpp
    Forms/WordEditor.cpp
    Forms/WordFinder.cpp
    Forms/TagsEditor.cpp
    Forms/Settings.cpp
)

add_executable(${PROJECT_NAME}
    main.cpp
    QtLib/Forms/About.cpp
    QtLib/Application.cpp
    QtLib/SqlNavigator.cpp
    QtLib/SignalGuard.cpp

    QtLib/GlobalShortcut/GlobalShortcut.cpp
    # QtLib/GlobalShortcut/GlobalShortcut_mac.cpp
    # QtLib/GlobalShortcut/GlobalShortcut_win.cpp
    QtLib/GlobalShortcut/GlobalShortcut_x11.cpp

    QtLib/Utils.cpp
    Classes/CenteredCheckBox.cpp
    Classes/CheckBoxItemDelegate.cpp
    Classes/ComboBoxItemDelegate.cpp
    Classes/GoogleTranslator.cpp
    Forms/Main.cpp
    Forms/WordEditor.cpp
    Forms/WordFinder.cpp
    Forms/TagsEditor.cpp
    Forms/Settings.cpp
    ${MOC_UI}
    ${MOC_CPP}
)
target_link_libraries(${PROJECT_NAME}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Multimedia_LIBRARIES}
)

#--------------------------------------------------------------------------------------------------
# CPack
include(InstallRequiredSystemLibraries)

set(VERSION "1.0.0")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_GENERATOR "ZIP")

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_RELEASE 0)

set(CPACK_PACKAGE_CONTACT "skynowa")
set(CPACK_PACKAGE_VENDOR "Skynowa Studio")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})

include(CPack)


# sudo apt-get install libegl1-mesa-dev
# cd ..
# mkdir ../iTeacher_build
# cmake ../iTeacher
# cpack ../iTeacher
