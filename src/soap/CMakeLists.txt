
########################################################
# Files

SET (OMSOAP_SRCS
  om_soap_server.cpp 
  om_soap.hh
  stdsoap2.cpp 
  soapServer.cpp 
  soapC.cpp 
  file_parser.cpp
)

SET (OMSOAP_HDRS
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
     ${CMAKE_CURRENT_SOURCE_DIR}/../openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/../console
     ${CMAKE_CURRENT_SOURCE_DIR}/../
     ${GDAL_INCLUDE_DIR}
)

########################################################
# Build the command line apps
IF (WIN32)
  IF (MSVC)
    ADD_DEFINITIONS("-DCORE_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UCORE_EXPORT \"-DCORE_EXPORT=__declspec(dllimport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

ADD_EXECUTABLE (om_soap_server ${OMSOAP_SRCS})
TARGET_LINK_LIBRARIES(om_soap_server openmodeller -lpthread)

########################################################
# Install

INSTALL(TARGETS om_soap_server RUNTIME DESTINATION ${OM_BIN_DIR})

INSTALL(CODE "MESSAGE(\"Installing share data...\")")
INSTALL(FILES server.conf DESTINATION ${OM_CONFIG_DIR})
INSTALL(FILES vrts/bytehfa.vrt DESTINATION ${OM_CONFIG_DIR}/vrts)
INSTALL(FILES scheduler.sh DESTINATION ${OM_CONFIG_DIR})
