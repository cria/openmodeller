
########################################################
# Files

SET (OMSOAP_SRCS
  om_server.cpp 
  stdsoap2.cpp 
  soapServer.cpp 
  soapC.cpp 
  dom.cpp
  omws_utils.cpp
)

SET (OMSOAP_HDRS
)

SET (OMWS_MANAGER_SRCS
  omws_manager.cpp 
  stdsoap2.cpp 
  soapC.cpp 
  dom.cpp
  omws_utils.cpp
  ../../console/getopts/getopts.C
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
# Build the command line apps
IF (WIN32)
  IF (MSVC)
    ADD_DEFINITIONS("-DCORE_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UCORE_EXPORT \"-DCORE_EXPORT=__declspec(dllimport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

ADD_EXECUTABLE (om ${OMSOAP_SRCS})

ADD_EXECUTABLE (omws_manager ${OMWS_MANAGER_SRCS})

IF (ZLIB_FOUND)
  TARGET_LINK_LIBRARIES(om openmodeller -lpthread ${ZLIB_LIBRARIES})
  TARGET_LINK_LIBRARIES(omws_manager openmodeller ${ZLIB_LIBRARIES})
ELSE (ZLIB_FOUND)
  TARGET_LINK_LIBRARIES(om openmodeller -lpthread)
  TARGET_LINK_LIBRARIES(omws_manager openmodeller)
ENDIF (ZLIB_FOUND)

########################################################
# Install

INSTALL(TARGETS omws_manager RUNTIME DESTINATION ${OM_BIN_DIR})

