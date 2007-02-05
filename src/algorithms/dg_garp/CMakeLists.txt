#SUBDIRS()
########################################################
# Files


SET (LIBDGARP_SRCS
  GarpAlgorithm.cpp 
  EnvCell.cpp 
  EnvCellSet.cpp 
  Rule.cpp 
  RuleSet.cpp 
  Utilities.cpp 
)
#these are not needed to build but I specified them in 
#case we want to install the headers
SET (LIBDGARP_HDRS
  GarpAlgorithm.h 
  EnvCell.h 
  EnvCellSet.h 
  Rule.h 
  RuleSet.h 
  Utilities.h
)

########################################################
# Build

ADD_LIBRARY (desktop_garp SHARED ${LIBDGARP_SRCS})
TARGET_LINK_LIBRARIES(desktop_garp openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../../openmodeller
)

########################################################
# Install

INSTALL(TARGETS desktop_garp RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
