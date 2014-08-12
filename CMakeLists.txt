#--------------------------------------------------------------------------------------------------
# \file  CPackConfig.cmake
# \brief
#--------------------------------------------------------------------------------------------------


project(iTeacher)
cmake_minimum_required(VERSION 2.8)

#--------------------------------------------------------------------------------------------------
# options
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#--------------------------------------------------------------------------------------------------
# modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/xLib/CMake")
include(FindxLib)

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

foreach(COMPONENT ${_QT5_MODULES})
    find_package(Qt5${COMPONENT} REQUIRED)
    include_directories(${Qt5${COMPONENT}_INCLUDE_DIRS})
    add_definitions(${Qt5${COMPONENT}_DEFINITIONS})
    link_libraries(${Qt5${COMPONENT}_LIBRARIES})
endforeach()

include_directories(
    ${XLIB_INCLUDES}
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

target_link_libraries(${PROJECT_NAME} ${XLIB_LIBRARIES})

#--------------------------------------------------------------------------------------------------
# install
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Bin")

foreach(QT5_MODULE ${_QT5_MODULES})
    GET_TARGET_PROPERTY(dll Qt5::${QT5_MODULE} LOCATION_${CMAKE_BUILD_TYPE})
    LIST(APPEND DLLS_REQUIRED "${dll}")

    # message("Qt5::${QT5_MODULE} - ${dll}")
endforeach()

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

include(Package.cmake)
