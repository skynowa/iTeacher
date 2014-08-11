#--------------------------------------------------------------------------------------------------
# \file  CPackConfig.cmake
# \brief
#--------------------------------------------------------------------------------------------------


project(iTeacher)
cmake_minimum_required(VERSION 2.8)

#--------------------------------------------------------------------------------------------------
# options
set(CMAKE_AUTOMOC ON)
# AUTOMOC_TARGETS_FOLDER
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
#
set(_QT5_MODULES Core Gui Widgets Sql Network Xml PrintSupport Multimedia)

FOREACH(COMPONENT ${_QT5_MODULES})
    find_package(Qt5${COMPONENT} REQUIRED)
    include_directories(${Qt5${COMPONENT}_INCLUDE_DIRS})
    add_definitions(${Qt5${COMPONENT}_DEFINITIONS})
ENDFOREACH()

include_directories(
    "${CMAKE_SOURCE_DIR}/xLib/Include"
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    "${CMAKE_SOURCE_DIR}/QtLib"
    "${CMAKE_SOURCE_DIR}/QtLib/Forms"
    "${CMAKE_SOURCE_DIR}/Classes"
    "${CMAKE_SOURCE_DIR}/Forms"
)

qt5_wrap_ui(MOC_UI
    QtLib/Forms/About.ui
    Forms/Main.ui
    Forms/WordEditor.ui
    Forms/WordFinder.ui
    Forms/TagsEditor.ui
    Forms/Settings.ui
)

qt5_wrap_cpp(MOC_CPP
    QtLib/Forms/About.cpp
    Forms/Main.cpp
    Forms/WordEditor.cpp
    Forms/WordFinder.cpp
    Forms/TagsEditor.cpp
    Forms/Settings.cpp
)

qt5_add_resources(RESOURCES Resources/App.qrc)

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
    ${RESOURCES}
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

FOREACH(QT5_MODULE ${_QT5_MODULES})
    GET_TARGET_PROPERTY(dll Qt5::${QT5_MODULE} LOCATION_${CMAKE_BUILD_TYPE})
    LIST(APPEND DLLS_REQUIRED "${dll}")

    message("Qt5::${QT5_MODULE} - ${dll}")
ENDFOREACH()

# target
install(
    TARGETS
        ${PROJECT_NAME}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}
)

# DLLs
install(
    FILES
        ${DLLS_REQUIRED}
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/Libs
)

# Doc
install(
    FILES
        ${CMAKE_SOURCE_DIR}/Readme.txt
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/Doc
)

# Resources
install(
    FILES
        ${CMAKE_SOURCE_DIR}/Resources/App.ico
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/Resources
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


# sudo apt-get install libegl1-mesa-dev libgles2-mesa-dev
# cd ..
# mkdir ../iTeacher_build
# cmake ../iTeacher
# cpack ../iTeacher


# https://github.com/Kitware/ParaView/blob/master/CMake/ParaViewQt5.cmake
