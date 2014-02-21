PROJECT(openModeller)
SET(OM_MAJOR_VERSION "1")
SET(OM_MINOR_VERSION "4")
SET(OM_PATCH_VERSION "0")

SET(OM_SOVERSION "0")

#############################################################
# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3)

# avoid cmake 2.6 warnings
#CMAKE_POLICY(SET CMP0005 OLD)

SET(CMAKE_COLOR_MAKEFILE ON)

# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# treat all warnings as errors
SET (PEDANTIC TRUE CACHE BOOL "Treat all compiler warnings as errors")

# set if the log level the build binaries should be set to Debug
SET (OM_DEBUG_LOGLEVEL FALSE CACHE BOOL "Force log level to Debug - should leave this off for releases!")

SET (OM_DEBUG_MEMORY FALSE CACHE BOOL "Output log messages to help debugging memory management")

SET (OM_CTEST FALSE CACHE BOOL "Build unit tests")

SET (OM_WITH_PROFILING FALSE CACHE BOOL "Add profiling symbols to build products (-pg)")

SET (OM_WITH_MPI FALSE CACHE BOOL "Determines whether MPI support should be built")

SET (OM_BUILD_BINDINGS FALSE CACHE BOOL "Indicates whether Python bindings should be built")

SET (OM_BUILD_SERVICE FALSE CACHE BOOL "Indicates whether the openModeller Web Service should be built")


#############################################################
IF (OM_CTEST)
  FIND_PACKAGE(CXXTEST)
  #enable unit testing
  ENABLE_TESTING()
  #enable posting test results to dart
  INCLUDE(CTest)
  INCLUDE(Dart)
  SET (EXAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples")
  SET (TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test/ctest/test_data")
  # Enable Dart
  # Include the standard Dart testing module
  #MESSAGE("Dart path: ${CMAKE_ROOT}/Modules/Dart.cmake")
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF (OM_CTEST)


#
# Enable the use of RPATH so that the executeable
# library search paths are relative to the executeable
#

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#############################################################
# Enable packaging

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_NAME "openModeller")
SET(CPACK_PACKAGE_VENDOR "CRIA")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "${OM_MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${OM_MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${OM_PATCH_VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "libopenmodeller-${OM_MAJOR_VERSION}.${OM_MINOR_VERSION}.${OM_PATCH_VERSION}")

IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} openModeller Desktop")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\openmodeller.sf.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\openmodeller.sf.net")
  SET(CPACK_NSIS_CONTACT "tim@linfiniti.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_PACKAGE_FILE_NAME "libopenmodeller-${OM_MAJOR_VERSION}.${OM_MINOR_VERSION}.${OM_PATCH_VERSION}")
  SET(CPACK_GENERATOR "TGZ")
  SET(CPACK_STRIP_FILES "")
  SET(CPACK_SOURCE_PACKAGE_FILE_NAME "libopenmodeller-src-${OM_MAJOR_VERSION}.${OM_MINOR_VERSION}.${OM_PATCH_VERSION}")
  SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP")
  SET(CPACK_SOURCE_STRIP_FILES "")
  SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.svn/;/install_win/;/install_linux/;/install_mac/;/debian/;/windows/;/src/swig/java/;/doc/.*\\\\.txt;/doc/.*\\\\.dia;/doc/.*\\\\.dox;/doc/.*\\\\.png;/doc/.*\\\\.sh;/doc/.*\\\\.t2t;/doc/.*\\\\.py;/doc/.*\\\\.pdf;/doc/.*\\\\.css;/doc/openModeller\\\\.doxygen;/libopenmodeller\\\\.spec;.*~")
ENDIF(WIN32 AND NOT UNIX)

INCLUDE(CPack)


#############################################################
# search for dependencies

# required
FIND_PACKAGE(Proj REQUIRED)
FIND_PACKAGE(GDAL REQUIRED)
FIND_PACKAGE(Expat REQUIRED)
FIND_PACKAGE(GSL REQUIRED)   # For CSM algorithm (version must be >= 1.4)

# optional
FIND_PACKAGE(X11)
FIND_PACKAGE(Sqlite3)
FIND_PACKAGE(TerraLib)  # version >= 3.2.0
FIND_PACKAGE(CURL)      # note: minimum version is 7.15.4

IF (OM_WITH_MPI)
  FIND_PACKAGE(Mpi)
ENDIF (OM_WITH_MPI)

# python support:
IF (OM_BUILD_BINDINGS)
  FIND_PACKAGE(PythonLibs)
ENDIF (OM_BUILD_BINDINGS)

IF (NOT PROJ_FOUND)
  MESSAGE (SEND_ERROR "PROJ4 library was not found!")
ENDIF (NOT PROJ_FOUND)

IF (NOT GDAL_FOUND)
  MESSAGE (SEND_ERROR "GDAL library was not found!")
ENDIF (NOT GDAL_FOUND)

IF (NOT GSL_FOUND)
  MESSAGE (SEND_ERROR "GSL library was not found!")
ENDIF (NOT GSL_FOUND)

#############################################################
# Add g++ directive to create profiling symbols
IF (OM_WITH_PROFILING)
  ADD_DEFINITIONS( -pg )
ENDIF (OM_WITH_PROFILING)

#############################################################
# enable warnings and set compile time defines

IF (PEDANTIC AND NOT MSVC)
  # unfortunately some otherwise valid functions in sample (e.g. += operator)
  # throw unused variable warnings, so I have disabled this warning using
  # -Wno-unused-value - TS
  ADD_DEFINITIONS( -Wall -Werror -Wno-unused-value)
ENDIF (PEDANTIC AND NOT MSVC)

#assume we have escaped compiler directives
#eventually we want to change this to new
#since we don't need to jump through so many
#hoops to escape compiler directives then
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0005 OLD)
ENDIF(COMMAND cmake_policy)

IF (APPLE)
  #xcode doesnt like those double quotes
  ADD_DEFINITIONS(-DCONFIG_FILE='"om.cfg"')
  ADD_DEFINITIONS(-DOMDATAPATH='"share/openmodeller"')
  ADD_DEFINITIONS(-DOM_VERSION='"${OM_MAJOR_VERSION}.${OM_MINOR_VERSION}.${OM_PATCH_VERSION}"')
ELSE (APPLE)
  ADD_DEFINITIONS(-DCONFIG_FILE=\\"om.cfg\\")
  ADD_DEFINITIONS(-DOMDATAPATH=\\"share/openmodeller\\")
  ADD_DEFINITIONS(-DOM_VERSION=\\"${OM_MAJOR_VERSION}.${OM_MINOR_VERSION}.${OM_PATCH_VERSION}\\")
ENDIF (APPLE)

IF (OM_LOGLEVEL_DEBUG)
  ADD_DEFINITIONS(-DLOGLEVELISDEBUG=1)
ENDIF (OM_LOGLEVEL_DEBUG)

IF (OM_DEBUG_MEMORY)
  ADD_DEFINITIONS(-DDEBUG_MEMORY=1)
ENDIF (OM_DEBUG_MEMORY)

IF (CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DOMDEBUG=1)
ENDIF (CMAKE_BUILD_TYPE MATCHES Debug)

IF (TERRALIB_FOUND)
  IF (APPLE)
    ADD_DEFINITIONS(-DTERRALIB_FOUND=\\'TRUE\\')
  ELSE (APPLE)
    ADD_DEFINITIONS(-DTERRALIB_FOUND='"TRUE"')
  ENDIF (APPLE)
ENDIF (TERRALIB_FOUND)

IF (CURL_FOUND)
  IF (APPLE)
    ADD_DEFINITIONS(-DCURL_FOUND=\\'TRUE\\')
  ELSE (APPLE)
    ADD_DEFINITIONS(-DCURL_FOUND='"TRUE"')
  ENDIF (APPLE)
ENDIF (CURL_FOUND)

IF (MPI_FOUND)
  IF (APPLE)
    ADD_DEFINITIONS(-DMPI_FOUND=\\'TRUE\\')
  ELSE (APPLE)
    ADD_DEFINITIONS(-DMPI_FOUND='"TRUE"')
  ENDIF (APPLE)
ENDIF (MPI_FOUND)

#############################################################
# platform specific stuff

IF (WIN32)

  SET (OM_BIN_DIR ${CMAKE_INSTALL_PREFIX})
  SET (OM_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
  SET (OM_DATA_DIR ${CMAKE_INSTALL_PREFIX}/data)
  SET (OM_ALG_DIR ${CMAKE_INSTALL_PREFIX}/algs)
  SET (OM_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
  SET (OM_EXAMPLES_DIR ${CMAKE_INSTALL_PREFIX}/examples)
  # where .lib files should be placed
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OM_LIB_DIR})
  ADD_DEFINITIONS(-DOM_ALGS_DIR=\\"./algs\\")
  ADD_DEFINITIONS(-DOM_DATA_DIR=\\"./data\\")

  IF (MSVC)
    ADD_DEFINITIONS(-DMSVC)
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
    SET(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})
    SET(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR})
    #INSTALL(FILES $ENV{LIB_DIR}/bin/gdal15.dll DESTINATION ${OUTPUT_DIR}/debug)
    EXEC_PROGRAM(${CMAKE_CURRENT_SOURCE_DIR}/install_win/CopyDebugDll.bat)

    # expect that classes are being imported
    # Note: MSVC doesn't like when the macros are quotes
    # and MSYS doesn't like them unqouted (bacause of braces)
    ADD_DEFINITIONS("-Ddllexp=__declspec(dllimport)")
    ADD_DEFINITIONS("-DOM_XML_DLL_EXPORT=__declspec(dllimport)")
    ADD_DEFINITIONS("-DOM_ALG_DLL_EXPORT=__declspec(dllexport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-Ddllexp\"=__declspec(dllimport)\"")
    ADD_DEFINITIONS("-DOM_XML_DLL_EXPORT\"=__declspec(dllimport)\"")
    ADD_DEFINITIONS("-DOM_ALG_DLL_EXPORT\"=__declspec(dllexport)\"")
  ENDIF (MSVC)

ELSE (WIN32)
  INCLUDE(GNUInstallDirs)

  # common for MAC and UNIX
  SET (OM_DATA_DIR     "${CMAKE_INSTALL_PREFIX}/share/openmodeller/data")
  SET (OM_INCLUDE_DIR  "${CMAKE_INSTALL_PREFIX}/include")
  SET (OM_EXAMPLES_DIR "${CMAKE_INSTALL_PREFIX}/share/openmodeller/examples")
  SET (OM_BIN_DIR      "${CMAKE_INSTALL_PREFIX}/bin")
  SET (OM_ALG_DIR      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/openmodeller")
  SET (OM_CONFIG_DIR   "/etc/openmodeller" CACHE STRING "Directory to store openModeller configuration files")

  IF (APPLE)
    #xcode doesnt like those escaped quotes
    ADD_DEFINITIONS(-DPREFIX='"${CMAKE_INSTALL_PREFIX}"')
    ADD_DEFINITIONS(-DOM_ALGS_DIR='"${OM_ALG_DIR}"')
    ADD_DEFINITIONS(-DOM_DATA_DIR='"${OM_DATA_DIR}"')
  ELSE (APPLE)
    ADD_DEFINITIONS(-DPREFIX=\\"${CMAKE_INSTALL_PREFIX}\\")
    ADD_DEFINITIONS(-DOM_ALGS_DIR=\\"${OM_ALG_DIR}\\")
    ADD_DEFINITIONS(-DOM_DATA_DIR=\\"${OM_DATA_DIR}\\")
  ENDIF (APPLE)

  IF (OM_BUNDLE)

    ADD_DEFINITIONS(-DOM_BUNDLE=\\"TRUE\\")

    # for Mac OS X, everything is put inside an application bundle
    SET (CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/MacOS)
    SET (OM_BIN_DIR    ${CMAKE_INSTALL_PREFIX})

    # path for library references
    SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
    SET (OM_ALG_DIR ${CMAKE_INSTALL_PREFIX}/algs)
  ENDIF (OM_BUNDLE)

ENDIF (WIN32)

#############################################################
# process subdirs

SUBDIRS(src examples)
if (SQLITE3_FOUND)
  SUBDIRS(data)
ENDIF(SQLITE3_FOUND)
IF (OM_CTEST)
  SUBDIRS(test)
ENDIF (OM_CTEST)

#############################################################
# Uninstall stuff see: http://www.vtk.org/Wiki/CMake_FAQ
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
