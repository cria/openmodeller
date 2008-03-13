########################################################
# Files

SET (LIBMAXIMUMENTROPY_SRCS
  maximum_entropy.cpp
  maxent.cpp
  blmvm.cpp
)

########################################################
# Build

ADD_LIBRARY (maximum_entropy MODULE ${LIBMAXIMUMENTROPY_SRCS})
TARGET_LINK_LIBRARIES(maximum_entropy openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS maximum_entropy RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
