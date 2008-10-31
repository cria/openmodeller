#SUBDIRS()
########################################################
# Files

SET (LIBOMNEURALNET_SRCS
  nn_alg.cpp
)

#these are not needed to build but I specified them in 
#case we want to install the headers
SET (LIBOMNEURALNET_HDRS
  nn.h
  nn_alg.hh
)

########################################################
# Build

ADD_LIBRARY (omneuralnet MODULE ${LIBOMNEURALNET_SRCS})
TARGET_LINK_LIBRARIES(omneuralnet openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS omneuralnet RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
