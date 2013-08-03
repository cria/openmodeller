
########################################################
# Files

# Use the following configuration to create omws and om parser DLLs
#
#SET(OM_PARSER_SRCS
#  omC.cpp 
#  envC.cpp
#  dom.cpp
#  stdsoap2.cpp 
#)
#
#SET (OM_PARSER_HDRS
#  omH.h 
#)
#
#SET(OMWS_LIB_SRCS
#  omwsC.cpp 
#  envC.cpp
#  stdsoap2.cpp 
#)
#
#SET (OMWS_LIB_HDRS
#  omwsH.h 
#)
#
#SET (OM_SERVER_SRCS
#  om_server.cpp 
#  omwsServer.cpp 
#  omws_utils.cpp
#)
#
#SET (OMWS_MANAGER_SRCS
#  omws_manager.cpp 
#  omws_utils.cpp
#  ../../console/getopts/getopts.C
#)

SET (OM_SERVER_SRCS
  om_server.cpp 
  omwsServer.cpp 
  omws_utils.cpp
  omwsC.cpp 
  envC.cpp
  omC.cpp 
  dom.cpp
  stdsoap2.cpp 
)

SET (OMWS_MANAGER_SRCS
  omws_manager.cpp 
  omws_utils.cpp
  ../../console/getopts/getopts.C
  envC.cpp
  omC.cpp 
  dom.cpp
  stdsoap2.cpp 
)

########################################################
# Build
IF (PEDANTIC)
  # The warnings are caused by generated soap sources
  # I dont know if it can be fixed really
  #MESSAGE("src/soap : -Werror removed for soap for now - please get rid of any compiler warnings!")
  FILE (GLOB files *.cpp)
  SET_SOURCE_FILES_PROPERTIES(${files} PROPERTIES COMPILE_FLAGS -Wno-error )
ENDIF (PEDANTIC)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/../../console
     ${CMAKE_CURRENT_SOURCE_DIR}/../../
     ${GDAL_INCLUDE_DIR}
)

# Check zlib presence to allow http compression
FIND_PACKAGE(ZLIB)
IF (ZLIB_FOUND)
  MESSAGE(STATUS "Found ZLIB. Enabling compression in the SOAP server")
  ADD_DEFINITIONS("-DWITH_GZIP")
  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${ZLIB_INCLUDE_DIR}
  )
ELSE (ZLIB_FOUND)
  MESSAGE(STATUS "ZLIB not found. Compression disabled in the SOAP server")
ENDIF (ZLIB_FOUND)

########################################################
# Build DLLs and BINARIES
IF (WIN32)
  IF (MSVC)
    ADD_DEFINITIONS("-DCORE_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UCORE_EXPORT \"-DCORE_EXPORT=__declspec(dllimport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

#Use the following commands to define the omws and om parser DLLS
#
#ADD_LIBRARY(om_parser SHARED ${OM_PARSER_SRCS})
#
#ADD_LIBRARY(omws SHARED ${OMWS_LIB_SRCS})

ADD_EXECUTABLE (om ${OM_SERVER_SRCS})

ADD_EXECUTABLE (omws_manager ${OMWS_MANAGER_SRCS})

IF (ZLIB_FOUND)
  #TARGET_LINK_LIBRARIES(om openmodeller omws om_parser -lpthread ${ZLIB_LIBRARIES})
  #TARGET_LINK_LIBRARIES(omws_manager openmodeller om_parser ${ZLIB_LIBRARIES})
  TARGET_LINK_LIBRARIES(om openmodeller -lpthread ${ZLIB_LIBRARIES})
  TARGET_LINK_LIBRARIES(omws_manager openmodeller ${ZLIB_LIBRARIES})
ELSE (ZLIB_FOUND)
  #TARGET_LINK_LIBRARIES(om openmodeller omws om_parser -lpthread)
  #TARGET_LINK_LIBRARIES(omws_manager openmodeller om_parser)
  TARGET_LINK_LIBRARIES(om openmodeller -lpthread)
  TARGET_LINK_LIBRARIES(omws_manager openmodeller)
ENDIF (ZLIB_FOUND)

########################################################
# Install

# Use the following commands to install omws and om parser DLLs
#
#INSTALL(TARGETS om_parser RUNTIME DESTINATION ${OM_BIN_DIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
#
#INSTALL(TARGETS omws RUNTIME DESTINATION ${OM_BIN_DIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

INSTALL(TARGETS omws_manager RUNTIME DESTINATION ${OM_BIN_DIR})

