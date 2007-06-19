########################################################
# Files

SET (LIBOMSVM_SRCS
  svm_alg.cpp
  svm.cpp
)

########################################################
# Build

ADD_LIBRARY (omsvm MODULE ${LIBOMSVM_SRCS})
TARGET_LINK_LIBRARIES(omsvm openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS omsvm RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
