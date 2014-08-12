#--------------------------------------------------------------------------------------------------
# \file  Pack.cmake
# \brief
#--------------------------------------------------------------------------------------------------


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
