########################################################
# Files

SET (LIBENFA_SRCS
  enfa.cpp
)

########################################################
# Build

ADD_LIBRARY (enfa MODULE ${LIBENFA_SRCS})
TARGET_LINK_LIBRARIES(enfa openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
)

IF (GSL_FOUND)

  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${GSL_INCLUDE_DIR}
  )

  ADD_LIBRARY (enfa MODULE ${LIBENFA_SRCS})

  TARGET_LINK_LIBRARIES(enfa ${GSL_LIBRARIES} openmodeller)

ENDIF(GSL_FOUND)

########################################################
# Install

INSTALL(TARGETS enfa RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
