# Install script for directory: /home/chris/dev/cpp/openmodeller/src/algorithms/enfa

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF(EXISTS "$ENV{DESTDIR}/usr/local/lib/openmodeller/libenfa.so")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/lib/openmodeller/libenfa.so"
         RPATH "/usr/local/lib")
  ENDIF(EXISTS "$ENV{DESTDIR}/usr/local/lib/openmodeller/libenfa.so")
  FILE(INSTALL DESTINATION "/usr/local/lib/openmodeller" TYPE MODULE FILES "/home/chris/dev/cpp/openmodeller/src/algorithms/enfa/libenfa.so")
  IF(EXISTS "$ENV{DESTDIR}/usr/local/lib/openmodeller/libenfa.so")
    FILE(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/lib/openmodeller/libenfa.so"
         OLD_RPATH "/home/chris/dev/cpp/openmodeller/src/openmodeller"
         NEW_RPATH "/usr/local/lib")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/lib/openmodeller/libenfa.so")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF(EXISTS "$ENV{DESTDIR}/usr/local/lib/openmodeller/libenfa.so")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

