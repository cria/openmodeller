
########################################################
# Files

SET (LIBOPENMODELLERXML_SRCS
  consolexml.cpp
)

SET (LIBOPENMODELLERXML_HDRS
  consolexml.hh
)

SET (OMCONSOLE_SRCS
    om_console.cpp
    request_file.cpp
    file_parser.cpp
)


########################################################
# Build

ADD_LIBRARY (openmodellerxml MODULE ${LIBOPENMODELLERXML_SRCS} ${LIBOPENMODELLERXML_HDRS})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../openmodeller
)

ADD_EXECUTABLE (om_console ${OMCONSOLE_SRCS})
TARGET_LINK_LIBRARIES(om_console
 openmodeller 
)


########################################################
# Install

INSTALL(TARGETS openmodellerxml
  RUNTIME DESTINATION ${OM_BIN_DIR}
  LIBRARY DESTINATION lib)

INSTALL(TARGETS om_console RUNTIME DESTINATION ${OM_BIN_DIR})
