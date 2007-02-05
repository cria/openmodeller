#############################################################
# sources

SET(OM_CORE_SRCS
  Algorithm.cpp 
  AlgorithmFactory.cpp 
  AlgParameter.cpp 
  AreaStats.cpp 
  Configuration.cpp 
  Configuration_xmlio.cpp 
  ConfusionMatrix.cpp 
  Environment.cpp 
  ignorecase_traits.cpp 
  Log.cpp 
  MapFormat.cpp 
  Occurrence.cpp 
  Occurrences.cpp 
  OpenModeller.cpp 
  os_specific.cpp
  Projector.cpp 
  Random.cpp 
  Sample.cpp 
  Sampler.cpp 
  env_io/GeoTransform.cpp 
  env_io/Header.cpp 
  env_io/Map.cpp 
  env_io/MapIterator.cpp 
  env_io/Raster.cpp 
  env_io/RasterGdal.cpp 
  env_io/RasterFactory.cpp 
  models/AlgoAdapterModel.cpp 
  models/AverageModel.cpp 
  models/ScaledModel.cpp 
  occ_io/OccurrencesFactory.cpp 
  occ_io/OccurrencesFile.cpp 
  occ_io/OccurrencesReader.cpp 
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
  env_io 
  occ_io
  ${CMAKE_CURRENT_SOURCE_DIR}/../
  ${PROJ_INCLUDE_DIR}
  ${SQLITE3_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
)

IF (WIN32)
  IF (MSVC)
    ADD_DEFINITIONS("-DCORE_EXPORT=__declspec(dllexport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UCORE_EXPORT \"-DCORE_EXPORT=__declspec(dllexport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

#############################################################
# openmodeller_core library

ADD_LIBRARY(openmodeller SHARED ${OM_CORE_SRCS})

# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(openmodeller
  ${EXPAT_LIBRARY}
  ${PROJ_LIBRARY}
  ${SQLITE3_LIBRARY}
  ${GDAL_LIBRARY}
  ${PLATFORM_LIBRARIES}
)

INSTALL(TARGETS openmodeller
  RUNTIME DESTINATION ${OM_BIN_DIR}
  LIBRARY DESTINATION lib)


# Added by Tim to install headers

SET(OM_CORE_HDRS
  AlgMetadata.hh 
  AlgorithmFactory.hh  
  Algorithm.hh     
  AlgParameter.hh      
  AreaStats.hh     
  Configurable.hh      
  Configuration.hh     
  ConfusionMatrix.hh 
  Environment.hh   
  Exceptions.hh   
  ignorecase_traits.hh  
  Log.hh   
  MapFormat.hh   
  Model.hh 
  Normalizable.hh 
  Occurrence.hh    
  Occurrences.hh   
  om.hh 
  om_defs.hh    
  OpenModeller.hh  
  os_specific.hh   
  Projector.hh 
  Random.hh
  refcount.hh 
  SampleExpr.hh 
  SampleExprVar.hh 
  Sample.hh 
  Sampler.hh 
)

INSTALL(CODE "MESSAGE(\"Installing CORE headers...\")")
INSTALL(FILES ${OM_CORE_HDRS} DESTINATION ${OM_INCLUDE_DIR})
