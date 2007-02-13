PROJECT(openModeller)

#############################################################
# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3)

SET(CMAKE_COLOR_MAKEFILE ON)

# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# try to configure and build python bindings by default
SET (WITH_BINDINGS TRUE CACHE BOOL "Determines whether python bindings should be built")

#############################################################
# search for dependencies

# required
FIND_PACKAGE(Proj)
FIND_PACKAGE(Sqlite3)
FIND_PACKAGE(GDAL)
FIND_PACKAGE(Expat)

# optional
FIND_PACKAGE(GSL)      # Georeferencer
FIND_PACKAGE(X11)
# python support:
#INCLUDE (Python) # file cmake/Python.cmake


IF (NOT PROJ_FOUND OR NOT SQLITE3_FOUND OR NOT GDAL_FOUND)
  MESSAGE (SEND_ERROR "Some dependencies were not found!")
ENDIF (NOT PROJ_FOUND OR NOT SQLITE3_FOUND OR NOT GDAL_FOUND)

#############################################################
# enable warnings and set compile time defines

ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(-DVERSION=\\"0.4\\")
ADD_DEFINITIONS(-DCONFIG_FILE=\\"om.cfg\\")
ADD_DEFINITIONS(-DPLUGINPATH=\\"algs\\")
ADD_DEFINITIONS(-DOMDATAPATH=\\"share/openmodeller\\")

IF (CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DOMDEBUG=1)
ENDIF (CMAKE_BUILD_TYPE MATCHES Debug)

#############################################################
# platform specific stuff

IF (WIN32)
  SET (OM_BIN_DIR    ${CMAKE_INSTALL_PREFIX})
  SET (OM_DATA_DIR   ${CMAKE_INSTALL_PREFIX})
  SET (OM_ALG_DIR ${CMAKE_INSTALL_PREFIX}/algs)
  SET (OM_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
  SET (OM_EXAMPLES_DIR ${CMAKE_INSTALL_PREFIX}/examples)
ELSE (WIN32)
  IF (APPLE)
    # for Mac OS X, everything is put inside an application bundle
    SET (CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/MacOS)
    SET (OM_BIN_DIR    ${CMAKE_INSTALL_PREFIX})
    # path for library references
    SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
  ENDIF (APPLE)

  # common for MAC and UNIX
  SET (OM_DATA_DIR   ${CMAKE_INSTALL_PREFIX}/share/qgis)
  SET (OM_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/qgis)
  SET(OM_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/openmodeller")
  SET(OM_EXAMPLES_DIR "${CMAKE_INSTALL_PREFIX}/share/openmodeller/examples")

  IF (UNIX AND NOT APPLE)
    SET (OM_BIN_DIR    ${CMAKE_INSTALL_PREFIX}/bin)
    ADD_DEFINITIONS(-DPREFIX=\\"${CMAKE_INSTALL_PREFIX}\\")
    ADD_DEFINITIONS(-DALGPATH=\\"${OM_ALG_DIR}\\")
    ADD_DEFINITIONS(-DPKGDATAPATH=\\"${OM_DATA_DIR}\\")
  ENDIF (UNIX AND NOT APPLE)

ENDIF (WIN32)


IF (WIN32)
  # expect that classes are being imported
  # Note: MSVC doesn't like when the macros are quotes
  # and MSYS doesn't like them unqouted (bacause of braces)
  IF (MSVC)
    ADD_DEFINITIONS(-DUSE_PRAGMA)
    ADD_DEFINITIONS(-DDLL_CORE_IMPORT)
  ELSE (MSVC)
ENDIF (WIN32)

#############################################################
# process subdirs

SUBDIRS(src examples)

#############################################################
# install stuff

#INSTALL (FILES AUTHORS SPONSORS
 #        DESTINATION ${QGIS_DATA_DIR}/doc)

# manual page... install also on windows?
#INSTALL (FILES qgis.man
 #        DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)
