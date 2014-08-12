#--------------------------------------------------------------------------------------------------
# \file  Package.cmake
# \brief CPack config
#        http://www.cmake.org/Wiki/CMake:CPackConfiguration
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
# options
include(InstallRequiredSystemLibraries)
set(CPACK_SET_DESTDIR ON)

#--------------------------------------------------------------------------------------------------
# Basic settings
set(CPACK_GENERATOR "ZIP")
    # CPack generator to be used:
    # DEB    = Debian packages
    # NSIS   = Null Soft Installer
    # NSIS64 = Null Soft Installer (64-bit)
    # RPM    = RPM packages
    # STGZ   = Self extracting Tar GZip compression
    # TBZ2   = Tar BZip2 compression
    # TGZ    = Tar GZip compression
    # TZ     = Tar Compress compression
    # ZIP    = ZIP file format
    # Example: STGZ;TGZ;TZ
# set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY xxxxxxxxxxxxxxx)
    # Controls whether CPack adds a top-level directory, usually of the form ProjectName-Version-OS,
    # to the top of package tree.
    # Example: 0 to disable, 1 to enable
# set(CPACK_INSTALL_CMAKE_PROJECTS xxxxxxxxxxxxxxx)
    # List of four values: Build directory, Project Name, Project Component, Directory in the
    # package
    # Example: /home/andy/vtk/CMake-bin;CMake;ALL;/
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/Readme.txt)
    # File used as a description of a project
    # Example: /path/to/project/ReadMe.txt
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Learning foreign words")
    # Description summary of a project
    # Example: CMake is a build tool
set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})
    # List of pairs of executables and labels. Used by the NSIS generator to create Start Menu
    # shortcuts.
    # Example: ccmake;CMake
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CMAKE_SYSTEM_PROCESSOR})
    # Package file name without extension. Also a directory of installer
    # Example: cmake-2.5.0-Linux-i686
# set(CPACK_PACKAGE_INSTALL_DIRECTORY xxxxxxxxxxxxxxx)
    # Installation directory on the target system
    # Example: CMake 2.5
# set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY xxxxxxxxxxxxxxx)
    # Registry key used when installing this project
    # Example: CMake 2.5.0
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    # Package name, defaults to the project name.
    # Example: CMake
set(CPACK_PACKAGE_VENDOR "Skynowa Studio")
    # Package vendor name
    # Example: Kitware
set(CPACK_PACKAGE_VERSION_MAJOR 1)
    # Package Major Version
    # Example: 2
set(CPACK_PACKAGE_VERSION_MINOR 0)
    # Package Minor Version
    # Example: 5
set(CPACK_PACKAGE_VERSION_PATCH 0)
    # Package Patch Version
    # Example: 0
# set(CPACK_PROJECT_CONFIG_FILE xxxxxxxxxxxxxxx)
    # File included at cpack time, once per generator after setting CPACK_GENERATOR to the actual
    # generator being used; allows per-generator setting of CPACK_* variables at cpack time.
    # Example: ${PROJECT_BINARY_DIR}/CPackOptions.cmake
set(CPACK_SOURCE_GENERATOR "ZIP")
    # List of generators used for the source package
    # Example: TGZ;TZ
set(CPACK_SOURCE_IGNORE_FILES ".git;")
    # Pattern of files in the source tree that won't be packaged
    # Example: /CVS/;/\\.svn/;\\.swp$;\\.#;/#;.*~;cscope.*
set(CPACK_SOURCE_PACKAGE_FILE_NAME xxxxxxxxxxxxxxx)
    # Name of the source package
    # Example: cmake-2.5.0
# set(CPACK_SOURCE_STRIP_FILES xxxxxxxxxxxxxxx)
    # List of files in the source tree that will be stripped. Starting with CMake 2.6.0
    # CPACK_SOURCE_STRIP_FILES will be a boolean variable which enables stripping of all files
    # (a list of files evaluates to TRUE in CMake, so this change is compatible).
    # Example: -
# set(CPACK_STRIP_FILES xxxxxxxxxxxxxxx)
    # List of files to be stripped. Starting with CMake 2.6.0 CPACK_STRIP_FILES will be a boolean
    # variable which enables stripping of all files (a list of files evaluates to TRUE in CMake,
    # so this change is compatible).
    # Example: bin/ccmake;bin/cmake;bin/cpack;bin/ctest
set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
    # System name, defaults to the value of ${CMAKE_SYSTEM_NAME}.
    # Example: Linux-i686
#--------------------------------------------------------------------------------------------------
# Advanced settings
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
    # What CMake generator should be used if the project is CMake project.
    # Defaults to the value of CMAKE_GENERATOR.
    # Example: Unix Makefiles
# set(CPACK_RESOURCE_FILE_LICENSE xxxxxxxxxxxxxxx)
    # License file for the project, used by the STGZ, NSIS, and PackageMaker generators.
    # Example: /home/andy/vtk/CMake/Copyright.txt
# set(CPACK_RESOURCE_FILE_README xxxxxxxxxxxxxxx)
    # ReadMe file for the project, used by PackageMaker generator.
    # Example: /home/andy/vtk/CMake/Templates/CPack.GenericDescription.txt
# set(CPACK_RESOURCE_FILE_WELCOME xxxxxxxxxxxxxxx)
    # Welcome file for the project, used by PackageMaker generator.
    # Example: /home/andy/vtk/CMake/Templates/CPack.GenericWelcome.txt
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
    # Package full version, used internally.
    # Example: 2.5.0
# set(CPACK_TOPLEVEL_TAG xxxxxxxxxxxxxxx)
    # Directory for the installed files.
    # Example: Linux-i686
# set(CPACK_INSTALL_COMMANDS xxxxxxxxxxxxxxx)
    # Extra commands to install components. Executed in addition to ..._SCRIPT and
    # CPACK_INSTALLED_DIRECTORIES.
# set(CPACK_INSTALL_SCRIPT xxxxxxxxxxxxxxx)
    # Extra script to install components. Executed in addition to related variables.
# set(CPACK_INSTALLED_DIRECTORIES xxxxxxxxxxxxxxx)
    # Extra directories to install. Note that spelling is CPACK_INSTALLED_DIRECTORIES
    # (wrong docs in CPack.cmake). Executed in addition to related variables.
# set(CPACK_MONOLITHIC_INSTALL xxxxxxxxxxxxxxx)
    # When set disables the component-based installer.
# set(CPACK_PACKAGING_INSTALL_PREFIX xxxxxxxxxxxxxxx)
    # Sets the default root that the generated package installs into, '/usr' is the default for
    # the debian and redhat generators
    # Example: /usr/local
set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_SOURCE_DIR}/Distr/${CMAKE_BUILD_TYPE})
    # Available in CMake >= 2.8.3 only (for older versions a find . -maxdepth 1 -type f -ctime....
    # might prove useful to gather the set of packages to be moved away). Can be used to specify
    # the destination output prefix directory that the final package will be stored in.
    # Since CPack will combine this argument with a trailing slash, it unfortunately cannot be used
    # to specify a filename-only prefix part.
    # Example: /tmp/my_cpack_generated_packages
#--------------------------------------------------------------------------------------------------

include(CPack)


# sudo apt-get install libegl1-mesa-dev libgles2-mesa-dev
# cd ..
# mkdir ../iTeacher_build
# cmake ../iTeacher
# cpack ../iTeacher (or make package)
