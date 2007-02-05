SUBDIRS(garp)

########################################################
# Files

SET (LIBBIOCLIM_SRCS
     bioclim.cpp
)

SET (LIBDISTANCETOAVERAGE_SRCS
     distance_to_average.cpp
  )

SET (LIBMINIMUMDISTANCE_SRCS
     minimum_distance.cpp
     )
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

ADD_LIBRARY (bioclim SHARED ${LIBBIOCLIM_SRCS})
TARGET_LINK_LIBRARIES(bioclim openmodeller)

ADD_LIBRARY (distance_to_average SHARED ${LIBDISTANCETOAVERAGE_SRCS})
TARGET_LINK_LIBRARIES(distance_to_average openmodeller)

ADD_LIBRARY (minimum_distance SHARED ${LIBMINIMUMDISTANCE_SRCS})
TARGET_LINK_LIBRARIES(minimum_distance openmodeller)

ADD_LIBRARY (aquamaps SHARED ${LIBAQUAMAPS_SRCS})
TARGET_LINK_LIBRARIES(aquamaps openmodeller)

ADD_LIBRARY (environmental_distance SHARED ${LIBENVIRONMENTALDISTANCE_SRCS})
TARGET_LINK_LIBRARIES(environmental_distance openmodeller)


INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}../openmodeller
)

if (GSL_FOUND)
  MESSAGE ("Building with GSL includes at ${GSL_INCLUDE_DIR}...")
  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${GSL_INCLUDE_DIR}
  )
  ADD_LIBRARY (csm_bs SHARED ${LIBCSMBS_SRCS})
  TARGET_LINK_LIBRARIES(csm_bs gsl gslcblas openmodeller)
ENDIF(GSL_FOUND)
########################################################
# Install

INSTALL(TARGETS bioclim RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
INSTALL(TARGETS distance_to_average RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
INSTALL(TARGETS minimum_distance RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
INSTALL(TARGETS aquamaps RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
INSTALL(TARGETS environmental_distance RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
if (GSL_FOUND)
  INSTALL(TARGETS csm_bs RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION algs)
ENDIF(GSL_FOUND)
