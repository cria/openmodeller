########################################################
# C++ files

SET (LIBMAXENT_SRCS
    maximum_entropy.cpp
    linear_feature.cpp
    quadratic_feature.cpp
    product_feature.cpp
)

#these are not needed to build but I specified them in 
#case we want to install the headers
SET (LIBMAXENT_HDRS
    maximum_entropy.hh
)

########################################################
# Build

ADD_LIBRARY (maximum_entropy MODULE ${LIBMAXENT_SRCS})
TARGET_LINK_LIBRARIES(maximum_entropy openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS maximum_entropy RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
