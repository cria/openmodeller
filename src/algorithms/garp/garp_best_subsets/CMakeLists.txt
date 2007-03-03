########################################################
# Files

SET (LIBGARPBS_SRCS
  best_subsets.cpp 
  garp_best_subsets.cpp 
  garp_run.cpp
  ../garp.cpp
  ../ruleset.cpp
  ../bioclim_histogram.cpp
  ../regression.cpp
  ../rules_base.cpp
  ../rules_range.cpp
  ../rules_negrange.cpp
  ../rules_logit.cpp
)
SET (LIBGARPBS_HDRS
  best_subsets.hh 
  bs_algorithm_factory.hh 
  garp_best_subsets.hh 
  garp_run.hh 
  garp_run_thread.hh 
  ../garp.hh 
  ../ruleset.hh
  ../bioclim_histogram.hh
  ../regression.hh
  ../rules_base.hh
  ../rules_range.hh
  ../rules_negrange.hh
  ../rules_logit.hh
)

########################################################
# Build

#we need to prevent the algmetadata definitions from garp.cpp
#being used in this case so we add this define
ADD_DEFINITIONS(-DDONT_EXPORT_GARP_FACTORY=1)


ADD_LIBRARY (garp_best_subsets MODULE ${LIBGARPBS_SRCS})
TARGET_LINK_LIBRARIES(garp_best_subsets openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../
     ${CMAKE_CURRENT_SOURCE_DIR}../../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS garp_best_subsets RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
