#--------------------------------------------------------------------------------------------------
# \file  Common.cmake
# \brief Common source for projects
#--------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.5.0)

#--------------------------------------------------------------------------------------------------
# options
set(CMAKE_AUTOMOC               ON)
set(CMAKE_INCLUDE_CURRENT_DIR   ON)

set(PROJECT_TYPE                "CXX")
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE            "RelWithDebInfo")
    # None (CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used)
    # Debug (CMAKE_C_FLAGS_DEBUG or CMAKE_CXX_FLAGS_DEBUG)
    # Release (CMAKE_C_FLAGS_RELEASE or CMAKE_CXX_FLAGS_RELEASE)
    # RelWithDebInfo (CMAKE_C_FLAGS_RELWITHDEBINFO or CMAKE_CXX_FLAGS_RELWITHDEBINFO
    # MinSizeRel (CMAKE_C_FLAGS_MINSIZEREL or CMAKE_CXX_FLAGS_MINSIZEREL)
set(QT_QMAKE_EXECUTABLE         "") # hide warning
#--------------------------------------------------------------------------------------------------
# Vars
set(DIR_HOME $ENV{HOME})
set(ROOT_DIR "${DIR_HOME}/Projects/iTeacher")

# Qt
set(CMAKE_PREFIX_PATH "${DIR_HOME}/Qt/6.6.1/gcc_64")

# modules
if (EXISTS "${CMAKE_ROOT}/Modules/CMakeLib")
    list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_ROOT}/Modules/CMakeLib"
        "${CMAKE_SOURCE_DIR}")
else()
    list(APPEND CMAKE_MODULE_PATH
        "${DIR_HOME}/Projects/CMakeLib"
        "${CMAKE_SOURCE_DIR}")
endif()

message("")
message("********** Vars **********")
message(STATUS "PROJECT_NAME:      ${PROJECT_NAME}")
message(STATUS "DIR_HOME:          ${DIR_HOME}")
message(STATUS "ROOT_DIR:          ${ROOT_DIR}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message("")
#--------------------------------------------------------------------------------------------------
# find packages
find_package(xLib    REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Qt6     REQUIRED)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
    ${LIBXML2_INCLUDE_DIR}
    ${XLIB_INCLUDES}
    ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
    ${ROOT_DIR}
    ${ROOT_DIR}/QtLib
    ${ROOT_DIR}/QtLib/Ui
    ${ROOT_DIR}/QtLib/Db
    ${ROOT_DIR}/Classes
    ${ROOT_DIR}/Ui)

# Add -fPIE
set(CMAKE_CXX_FLAGS "${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")
#--------------------------------------------------------------------------------------------------
