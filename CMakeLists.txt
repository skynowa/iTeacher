#--------------------------------------------------------------------------------------------------
# \file  CPackConfig.cmake
# \brief
#--------------------------------------------------------------------------------------------------


project(iTeacher)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_AUTOMOC ON)
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

add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Gui_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Sql_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})
add_definitions(${Qt5PrintSupport_DEFINITIONS})
add_definitions(${Qt5Multimedia_DEFINITIONS})

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
include_directories(${Qt5Multimedia_INCLUDE_DIRS})
# include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

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

# Add -fPIE
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

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
# install
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Bin")

install(
    TARGETS
        ${PROJECT_NAME}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}
)

install(
    FILES
        "${CMAKE_SOURCE_DIR}/Readme.txt"
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}
)

#--------------------------------------------------------------------------------------------------
# CPack
include(InstallRequiredSystemLibraries)

set(CPACK_SET_DESTDIR ON)

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
