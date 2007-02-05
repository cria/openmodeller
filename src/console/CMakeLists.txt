
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


SET (OMDUMP_SRCS
     om_dump.cpp 
     request_file.cpp 
     file_parser.cpp
     )
SET (OMSAMPLEDUMP_SRCS
    om_sampledump.cpp 
    request_file.cpp 
    file_parser.cpp
    )
SET (OMPROJECT_SRCS 
     om_project.cpp  
     )
SET (OMCREATE_SRCS
     om_create.cpp
     )
SET (OMMETADUMP_SRCS
     om_metadump.cpp
     )
SET (OMVIEWER_SRCS
     om_viewer.cpp 
     file_parser.cpp 
     graph/color.cpp 
     graph/graphic.cpp 
     graph/graphic_x11.cpp 
     graph/view_model.cpp
     )
SET (OMNICHE_SRCS
     om_niche.cpp 
     request_file.cpp 
     file_parser.cpp 
     graph/color.cpp 
     graph/graphic.cpp 
     graph/graphic_x11.cpp
     )
set (EXTRA_DIST
  file_parser.hh
  request_file.hh
  graph/color.hh
  graph/graphic.hh
  graph/graphic_x11.hh
  graph/list.cpp
  graph/list.hh
  graph/view_model.hh
  )

########################################################
# Build

ADD_LIBRARY (openmodellerxml SHARED ${LIBOPENMODELLERXML_SRCS} ${LIBOPENMODELLERXML_HDRS})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../openmodeller
)

ADD_EXECUTABLE (om_console ${OMCONSOLE_SRCS})
TARGET_LINK_LIBRARIES(om_console openmodeller )

ADD_EXECUTABLE (om_create ${OMCREATE_SRCS})
TARGET_LINK_LIBRARIES(om_create openmodeller openmodellerxml)

ADD_EXECUTABLE (om_project ${OMPROJECT_SRCS})
TARGET_LINK_LIBRARIES(om_project openmodeller openmodellerxml)

ADD_EXECUTABLE (om_dump ${OMDUMP_SRCS})
TARGET_LINK_LIBRARIES(om_dump openmodeller)

ADD_EXECUTABLE (om_sampledump ${OMSAMPLEDUMP_SRCS})
TARGET_LINK_LIBRARIES(om_sampledump openmodeller)

if (X11_FOUND)
  MESSAGE ("Building with X11 includes at ${X11_INCLUDE_DIR}...")
  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/graph
    ${X11_INCLUDE_DIR}
  )
  ADD_EXECUTABLE (om_viewer ${OMVIEWER_SRCS})
  TARGET_LINK_LIBRARIES(om_viewer ${X11_LIBRARIES} openmodeller)

  ADD_EXECUTABLE (om_niche ${OMNICHE_SRCS})
  TARGET_LINK_LIBRARIES(om_niche ${X11_LIBRARIES} openmodeller)
ENDIF(X11_FOUND)
########################################################
# Install

INSTALL(TARGETS openmodellerxml RUNTIME DESTINATION ${OM_BIN_DIR} LIBRARY DESTINATION lib)
INSTALL(TARGETS om_console RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_create RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_project RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_dump RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_sampledump RUNTIME DESTINATION ${OM_BIN_DIR})
if (X11_FOUND)
  INSTALL(TARGETS om_viewer RUNTIME DESTINATION ${OM_BIN_DIR})
  INSTALL(TARGETS om_niche RUNTIME DESTINATION ${OM_BIN_DIR})
ENDIF(X11_FOUND)
