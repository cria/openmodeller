########################################################
# Files

SET (LIBOMCONSENSUS_SRCS
  consensus.cpp
)

########################################################
# Build

ADD_LIBRARY (omconsensus MODULE ${LIBOMCONSENSUS_SRCS})
TARGET_LINK_LIBRARIES(omconsensus openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS omconsensus RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
