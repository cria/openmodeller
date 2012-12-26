#######################################################
#  MSVC DEFINITIONS
IF (WIN32)
	IF (MSVC)
		SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR}/algs)
		SET(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR}/algs)
	ENDIF (MSVC)
ENDIF(WIN32)

########################################################
# Fix for LNK2001: unresolved external symbol _gsl_rng_default under msvc 2008
IF (WIN32)
    ADD_DEFINITIONS("-DGSL_DLL")
ENDIF (WIN32)  


########################################################
# Subdirectories
SUBDIRS(best_subsets garp dg_garp svm neural_networks maxent rf consensus enfa)

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

SET (LIBNICHEMOSAIC_SRCS
     niche_mosaic.cpp 
     niche_mosaic.hh
)

#SET (LIBVIRTUALNICHE_SRCS
#     virtual_niche.cpp 
#     virtual_niche.hh
#)

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

ADD_LIBRARY (niche_mosaic MODULE ${LIBNICHEMOSAIC_SRCS})
TARGET_LINK_LIBRARIES(niche_mosaic openmodeller)

ADD_LIBRARY (csm_bs MODULE ${LIBCSMBS_SRCS})
TARGET_LINK_LIBRARIES(csm_bs ${GSL_LIBRARIES} openmodeller)

#ADD_LIBRARY (virtual_niche MODULE ${LIBVIRTUALNICHE_SRCS})
#TARGET_LINK_LIBRARIES(virtual_niche openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/..
     ${CMAKE_CURRENT_SOURCE_DIR}/../openmodeller
     ${GSL_INCLUDE_DIR}
)

IF (CURL_FOUND)

  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${EXPAT_INCLUDE_DIR}
  )
  
ENDIF(CURL_FOUND)

IF (SQLITE3_FOUND)

  INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${SQLITE3_INCLUDE_DIR}
  )

  IF(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
  ENDIF(COMMAND cmake_policy)

  ADD_LIBRARY (aquamaps MODULE ${LIBAQUAMAPS_SRCS})

  TARGET_LINK_LIBRARIES(aquamaps ${SQLITE3_LIBRARY} openmodeller)

ENDIF(SQLITE3_FOUND)

########################################################
# Install

INSTALL(TARGETS bioclim RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

INSTALL(TARGETS envelope_score RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

#INSTALL(TARGETS distance_to_average RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

#INSTALL(TARGETS minimum_distance RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

IF (SQLITE3_FOUND)
  INSTALL(TARGETS aquamaps RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
ENDIF(SQLITE3_FOUND)

INSTALL(TARGETS environmental_distance RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

INSTALL(TARGETS niche_mosaic RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

#INSTALL(TARGETS virtual_niche RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

INSTALL(TARGETS csm_bs RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
