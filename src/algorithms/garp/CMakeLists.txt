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

#IF (MPI_FOUND)

#  SET (LIBPGARP_SRCS
#    pgarp.cpp
#    ruleset.cpp
#    bioclim_histogram.cpp
#    regression.cpp
#    rules_base.cpp
#    rules_range.cpp
#    rules_negrange.cpp
#    rules_logit.cpp
#  )

#  SET (LIBPGARP_HDRS
#    pgarp.hh
#    ruleset.hh
#    bioclim_histogram.hh
#    regression.hh
#    rules_base.hh
#    rules_range.hh
#    rules_negrange.hh
#    rules_logit.hh
#  )

#ENDIF(MPI_FOUND)

########################################################
# Build

ADD_LIBRARY (garp MODULE ${LIBGARP_SRCS})
TARGET_LINK_LIBRARIES(garp openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../../openmodeller
)

#IF (MPI_FOUND)

#  INCLUDE_DIRECTORIES(
#    ${INCLUDE_DIRECTORIES}
#    ${MPI_INCLUDE_DIR}
#  )

#  ADD_LIBRARY (pgarp MODULE ${LIBPGARP_SRCS})

#  TARGET_LINK_LIBRARIES(pgarp ${MPI_LIBRARY} openmodeller)

#ENDIF(MPI_FOUND)

########################################################
# Install

INSTALL(TARGETS garp RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

#IF (MPI_FOUND)
#  INSTALL(TARGETS pgarp RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
#ENDIF(MPI_FOUND)

