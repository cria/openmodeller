
########################################################
# Files

SET (OMCONSOLE_SRCS
     om_console.cpp
     request_file.cpp
     om_cmd_utils.cpp 
    )
SET (OMSAMPLER_SRCS
     om_sampler.cpp 
     request_file.cpp 
     getopts/getopts.C 
     om_cmd_utils.cpp 
    )
SET (OMTEST_SRCS
     om_test.cpp 
     getopts/getopts.C 
     om_cmd_utils.cpp 
    )
SET (OMMODEL_SRCS
     om_model.cpp
     getopts/getopts.C 
     om_cmd_utils.cpp 
     )
SET (OMPROJECT_SRCS 
     om_project.cpp  
     getopts/getopts.C 
     om_cmd_utils.cpp 
     )
SET (OMVIEWER_SRCS
     om_viewer.cpp 
     graph/color.cpp 
     graph/graphic.cpp 
     graph/graphic_x11.cpp 
     graph/view_model.cpp
     )
SET (OMNICHE_SRCS
     om_niche.cpp 
     getopts/getopts.C
     om_cmd_utils.cpp 
     graph/color.cpp 
     graph/graphic.cpp 
     graph/graphic_x11.cpp
     )
SET (OMPSEUDO_SRCS
     om_pseudo.cpp 
     getopts/getopts.C 
     om_cmd_utils.cpp 
     )
SET (OMALGORITHM_SRCS
     om_algorithm.cpp 
     getopts/getopts.C 
     om_cmd_utils.cpp 
     )
SET (OMPOINTS_SRCS
     om_points.cpp 
     getopts/getopts.C 
     om_cmd_utils.cpp 
     )
SET (OMEVALUATE_SRCS
     om_evaluate.cpp 
     getopts/getopts.C 
     om_cmd_utils.cpp 
    )
SET (OMLAYER_SRCS
     om_layer.cpp 
     getopts/getopts.C 
     om_cmd_utils.cpp 
     om_layer_utils.cpp 
     )

IF (WIN32)
  SET(OMLAYER_SRCS
    ${OMLAYER_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../openmodeller/ext/dirent.c
  )
ENDIF (WIN32)

set (EXTRA_DIST
  request_file.hh
  om_cmd_utils.hh
  graph/color.hh
  graph/graphic.hh
  graph/graphic_x11.hh
  graph/list.cpp
  graph/list.hh
  graph/view_model.hh
  getopts/getopts.C
  getopts/getopts.h
  getopts/LICENSE
  getopts/README
  )

########################################################
# Build


INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/../console
     ${CMAKE_CURRENT_SOURCE_DIR}/../
     ${EXPAT_INCLUDE_DIR}
     ${TERRALIB_INCLUDE_DIR}
     ${CURL_INCLUDE_DIR}
     ${GDAL_INCLUDE_DIR}
  )

########################################################
# Build the varios command line apps
ADD_EXECUTABLE (om_console ${OMCONSOLE_SRCS})
TARGET_LINK_LIBRARIES(om_console openmodeller)

ADD_EXECUTABLE (om_model ${OMMODEL_SRCS})
TARGET_LINK_LIBRARIES(om_model openmodeller)

ADD_EXECUTABLE (om_project ${OMPROJECT_SRCS})
TARGET_LINK_LIBRARIES(om_project openmodeller)

ADD_EXECUTABLE (om_sampler ${OMSAMPLER_SRCS})
TARGET_LINK_LIBRARIES(om_sampler openmodeller)

ADD_EXECUTABLE (om_test ${OMTEST_SRCS})
TARGET_LINK_LIBRARIES(om_test openmodeller)

IF (X11_FOUND)
  #MESSAGE ("Building with X11 includes at ${X11_INCLUDE_DIR}...")
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

ADD_EXECUTABLE (om_pseudo ${OMPSEUDO_SRCS})
TARGET_LINK_LIBRARIES(om_pseudo openmodeller)

ADD_EXECUTABLE (om_algorithm ${OMALGORITHM_SRCS})
TARGET_LINK_LIBRARIES(om_algorithm openmodeller)

ADD_EXECUTABLE (om_points ${OMPOINTS_SRCS})
TARGET_LINK_LIBRARIES(om_points openmodeller)

ADD_EXECUTABLE (om_evaluate ${OMEVALUATE_SRCS})
TARGET_LINK_LIBRARIES(om_evaluate openmodeller)

ADD_EXECUTABLE (om_layer ${OMLAYER_SRCS})
TARGET_LINK_LIBRARIES(om_layer openmodeller)

########################################################
# Install

INSTALL(TARGETS om_console RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_algorithm RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_points RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_pseudo RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_model RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_project RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_sampler RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_test RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_evaluate RUNTIME DESTINATION ${OM_BIN_DIR})
INSTALL(TARGETS om_layer RUNTIME DESTINATION ${OM_BIN_DIR})
IF (X11_FOUND)
  INSTALL(TARGETS om_viewer RUNTIME DESTINATION ${OM_BIN_DIR})
  INSTALL(TARGETS om_niche RUNTIME DESTINATION ${OM_BIN_DIR})
ENDIF(X11_FOUND)

IF (UNIX AND NOT OM_BUNDLE)
  INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/man/om_console.1
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_algorithm.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_points.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_pseudo.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_model.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_sampler.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_test.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_project.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_evaluate.1 
                 ${CMAKE_CURRENT_SOURCE_DIR}/man/om_layer.1 
           DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)

  IF (X11_FOUND)
    INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/man/om_niche.1
                   ${CMAKE_CURRENT_SOURCE_DIR}/man/om_viewer.1
             DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)
  ENDIF(X11_FOUND)

ENDIF (UNIX AND NOT OM_BUNDLE)
