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
  Projector.cpp 
  Random.cpp 
  RocCurve.cpp
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
  IF (WIN32)
    SET(OM_CORE_SRCS
      ${OM_CORE_SRCS}
      os_specific_win.cpp
    )
  ELSE (WIN32)
    SET(OM_CORE_SRCS
      ${OM_CORE_SRCS}
      os_specific.cpp
    )
  ENDIF (WIN32)

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
    ADD_DEFINITIONS("-DCORE_EXPORT \"-DCORE_EXPORT=__declspec(dllexport)\"")
    ADD_DEFINITIONS( -Wl,--enable-auto-export )
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
IF (APPLE)
 TARGET_LINK_LIBRARIES(openmodeller
   "-framework CoreFoundation"
   "-framework Cocoa"
  )
ENDIF (APPLE)
  
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
  RocCurve.hh
  refcount.hh 
  SampleExpr.hh 
  SampleExprVar.hh 
  Sample.hh 
  Sampler.hh 
)

SET (OM_OCCIO_HDRS
occ_io/OccurrencesFactory.hh
occ_io/OccurrencesFile.hh
occ_io/OccurrencesReader.hh
occ_io/TeOccurrences.hh
)

SET (OM_ENVIO_HDRS
env_io/GeoTransform.hh
env_io/Header.hh
env_io/Map.hh
env_io/MapIterator.hh
env_io/Raster.hh
env_io/RasterFactory.hh
env_io/RasterGdal.hh
env_io/TeOMRaster.hh
)

INSTALL(CODE "MESSAGE(\"Installing CORE headers...\")")
INSTALL(FILES ${OM_CORE_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller)
INSTALL(FILES ${OM_OCCIO_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/occ_io)
INSTALL(FILES ${OM_ENVIO_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/env_io)
