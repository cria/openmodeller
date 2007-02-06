########################################################
# Files

SET (LIBDGARPBESTSUBSETS_SRCS
  AlgorithmRun.cpp
  AbstractBestSubsets.cpp
  DgGarpBestSubsets.cpp
  #GarpAlgorithm.cpp 
  #EnvCell.cpp 
  #EnvCellSet.cpp 
  #Rule.cpp 
  #RuleSet.cpp 
  #Utilities.cpp 
)
#these are not needed to build but I specified them in 
#case we want to install the headers
SET (LIBDGARPBESTSUBSETS_HDRS
  DgGarpBestSubsets.hh
  #GarpAlgorithm.h 
  #EnvCell.h 
  #EnvCellSet.h 
  #Rule.h 
  #RuleSet.h 
  #Utilities.h
)

########################################################
# Build

ADD_LIBRARY (desktop_garp_best_subsets SHARED ${LIBDGARPBESTSUBSETS_SRCS})
TARGET_LINK_LIBRARIES(desktop_garp_best_subsets openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../garp
     ${CMAKE_CURRENT_SOURCE_DIR}../../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS desktop_garp_best_subsets RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
