########################################################
# Files

SET (LIBENFA_SRCS
  enfa.cpp
)

########################################################
# Build

ADD_LIBRARY (enfa MODULE ${LIBENFA_SRCS})
TARGET_LINK_LIBRARIES(enfa ${GSL_LIBRARIES} openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS enfa RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
