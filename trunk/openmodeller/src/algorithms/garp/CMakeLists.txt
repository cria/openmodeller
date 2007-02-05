SUBDIRS(garp_best_subsets)
########################################################
# Files

SET (LIBGARP_SRCS
  garp.cpp
  ruleset.cpp
  bioclim_histogram.cpp
  regression.cpp
  rules_base.cpp
  rules_range.cpp
  rules_negrange.cpp
  rules_logit.cpp
)
#these are not needed to build but I specified them in 
#case we want to install the headers
SET (LIBGARP_HDRS
  garp.hh 
  ruleset.hh
  bioclim_histogram.hh
  regression.hh
  rules_base.hh
  rules_range.hh
  rules_negrange.hh
  rules_logit.hh
)

########################################################
# Build

ADD_LIBRARY (dg_garp SHARED ${LIBGARP_SRCS})
TARGET_LINK_LIBRARIES(dg_garp openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS dg_garp RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
