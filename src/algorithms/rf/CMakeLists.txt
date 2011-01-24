########################################################
# Files

SET (LIBOMRF_SRCS
  rf_alg.cpp
  librf/random_forest.cc
  librf/instance_set.cc
  librf/discrete_dist.cc
  librf/tree.cc
  librf/tree_node.cc
  librf/weights.cc
)

########################################################
# Build

ADD_LIBRARY (omrf MODULE ${LIBOMRF_SRCS})
TARGET_LINK_LIBRARIES(omrf openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS omrf RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
