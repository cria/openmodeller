SUBDIRS(best_subsets garp dg_garp svm)

########################################################
# Files

SET (LIBENVELOPE_SCORE_SRCS
     envelope_score.cpp
)

SET (LIBBIOCLIM_SRCS
     bioclim.cpp
)

# Replaced by EnvironmentalDistance
#SET (LIBDISTANCETOAVERAGE_SRCS
#     distance_to_average.cpp
#)

# Replaced by EnvironmentalDistance
#SET (LIBMINIMUMDISTANCE_SRCS
#     minimum_distance.cpp
#)

SET (LIBCSMBS_SRCS
     csm.cpp
     csmbs.cpp
)

SET (LIBAQUAMAPS_SRCS
     aquamaps.cpp
)

SET (LIBENVIRONMENTALDISTANCE_SRCS
     environmental_distance.cpp 
     matrix.hh
)

########################################################
# Build

ADD_LIBRARY (envelope_score MODULE ${LIBENVELOPE_SCORE_SRCS})
TARGET_LINK_LIBRARIES(envelope_score openmodeller)

ADD_LIBRARY (bioclim MODULE ${LIBBIOCLIM_SRCS})
TARGET_LINK_LIBRARIES(bioclim openmodeller)

#ADD_LIBRARY (distance_to_average MODULE ${LIBDISTANCETOAVERAGE_SRCS})
#TARGET_LINK_LIBRARIES(distance_to_average openmodeller)

#ADD_LIBRARY (minimum_distance MODULE ${LIBMINIMUMDISTANCE_SRCS})
#TARGET_LINK_LIBRARIES(minimum_distance openmodeller)

ADD_LIBRARY (environmental_distance MODULE ${LIBENVIRONMENTALDISTANCE_SRCS})
TARGET_LINK_LIBRARIES(environmental_distance openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/..
     ${CMAKE_CURRENT_SOURCE_DIR}/../openmodeller
)

if (GSL_FOUND)
  #MESSAGE ("Building with GSL includes at ${GSL_INCLUDE_DIR}...")
  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${GSL_INCLUDE_DIR}
  )
  ADD_LIBRARY (csm_bs MODULE ${LIBCSMBS_SRCS})
  IF (MSVC)
    TARGET_LINK_LIBRARIES(csm_bs gsl cblas openmodeller)
  ELSE (MSVC)
    TARGET_LINK_LIBRARIES(csm_bs gsl gslcblas openmodeller)
  ENDIF (MSVC)
ENDIF(GSL_FOUND)

if (SQLITE3_FOUND)
  MESSAGE ("Building with SQLite3 includes at ${SQLITE3_INCLUDE_DIR}...")
  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${SQLITE3_INCLUDE_DIR}
  )
  ADD_LIBRARY (aquamaps MODULE ${LIBAQUAMAPS_SRCS})
  TARGET_LINK_LIBRARIES(aquamaps sqlite3 openmodeller)
ENDIF(SQLITE3_FOUND)

########################################################
# Install

INSTALL(TARGETS bioclim RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

INSTALL(TARGETS envelope_score RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

#INSTALL(TARGETS distance_to_average RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

#INSTALL(TARGETS minimum_distance RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

if (SQLITE3_FOUND)
  INSTALL(TARGETS aquamaps RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
ENDIF(SQLITE3_FOUND)

INSTALL(TARGETS environmental_distance RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

if (GSL_FOUND)
  INSTALL(TARGETS csm_bs RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
ENDIF(GSL_FOUND)