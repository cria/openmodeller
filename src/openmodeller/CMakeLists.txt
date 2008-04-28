#############################################################
# sources
SET (LIBOPENMODELLERXML_SRCS
  ../console/consolexml.cpp
)

SET (LIBOPENMODELLERXML_HDRS
  ../console/consolexml.hh
)

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
  MeanVarianceNormalizer.cpp
  Occurrence.cpp 
  Occurrences.cpp 
  OpenModeller.cpp 
  Projector.cpp 
  Random.cpp 
  RocCurve.cpp
  Sample.cpp 
  Sampler.cpp 
  ScaleNormalizer.cpp
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
  pre/PreParameters.cpp
  pre/PreAlgorithm.cpp
  pre/Jackknife.cpp
  pre/ChiSquare.cpp
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

IF (TERRALIB_FOUND)

  SET(OM_CORE_SRCS
      ${OM_CORE_SRCS}
      TeDatabaseManager.cpp
      TeStringParser.cpp
      env_io/TeOMRaster.cpp
      occ_io/TeOccurrences.cpp
      ${TERRALIB_INCLUDE_DIR}/TeAdoDB.cpp
    )
ENDIF (TERRALIB_FOUND)

IF (CURL_FOUND AND WITH_TAPIR_CLIENT)

  SET(OM_CORE_SRCS
      ${OM_CORE_SRCS}
      occ_io/TapirOccurrences.cpp
      occ_io/GbifOccurrences.cpp
    )
ENDIF (CURL_FOUND AND WITH_TAPIR_CLIENT) 

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
  env_io 
  occ_io
  pre
  ${CMAKE_CURRENT_SOURCE_DIR}/../
  ${CMAKE_CURRENT_SOURCE_DIR}/../console
  ${PROJ_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
  ${EXPAT_INCLUDE_DIR}
  ${TERRALIB_INCLUDE_DIR}
  ${CURL_INCLUDE_DIR}
)

IF (WIN32)
  IF (MSVC)
    ADD_DEFINITIONS("-Ddllexp=__declspec(dllexport)")
    ADD_DEFINITIONS("-DOM_XML_DLL_EXPORT=__declspec(dllexport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-Udllexp \"-Ddllexp=__declspec(dllexport)\"")
    ADD_DEFINITIONS("-UOM_XML_DLL_EXPORT \"-DOM_XML_DLL_EXPORT=__declspec(dllexport)\"")
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
  ${GDAL_LIBRARY}
  ${PLATFORM_LIBRARIES}
  ${TERRALIB_LIBRARY}
  ${CURL_LIBRARY}
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
  AbortionCommand.hh
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
  MeanVarianceNormalizer.hh 
  Model.hh 
  Normalizable.hh 
  Normalizer.hh 
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
  ScaleNormalizer.hh
)

SET (OM_OCCIO_HDRS
occ_io/OccurrencesFactory.hh
occ_io/OccurrencesFile.hh
occ_io/OccurrencesReader.hh
)

IF (CURL_FOUND AND WITH_TAPIR_CLIENT)
  SET(OM_OCCIO_HDRS
      ${OM_OCCIO_HDRS}
      occ_io/TapirOccurrences.hh
      occ_io/GbifOccurrences.hh
     )
ENDIF (CURL_FOUND AND WITH_TAPIR_CLIENT)

SET (OM_ENVIO_HDRS
env_io/GeoTransform.hh
env_io/Header.hh
env_io/Map.hh
env_io/MapIterator.hh
env_io/Raster.hh
env_io/RasterFactory.hh
env_io/RasterGdal.hh
)

IF (TERRALIB_FOUND)

  SET(OM_CORE_HDRS
      ${OM_CORE_HDRS}
      TeDatabaseManager.hh
      TeStringParser.hh
     )

  SET(OM_OCCIO_HDRS
      ${OM_OCCIO_HDRS}
      occ_io/TeOccurrences.hh
     )

  SET(OM_ENVIO_HDRS
      ${OM_ENVIO_HDRS}
      env_io/TeOMRaster.hh
    )
ENDIF (TERRALIB_FOUND)

SET (OM_PRE_HDRS
pre/PreMultiContainer.hh
pre/PreParameters.hh
pre/PreAlgorithm.hh
pre/Jackknife.hh
pre/ChiSquare.hh
)


########################################################
# Make headers appear in MSVC nicely
IF (MSVC)
  SOURCE_GROUP( "Header Files" FILES ${OM_CORE_HEADERS} ${OM_OCCIO_HDRS} ${OM_ENVIO_HDRS})
ENDIF (MSVC)


########################################################
# Build openmodellerxml lib
ADD_LIBRARY (openmodellerxml SHARED ${LIBOPENMODELLERXML_SRCS})
TARGET_LINK_LIBRARIES(openmodellerxml openmodeller )
INSTALL(TARGETS openmodellerxml RUNTIME DESTINATION ${OM_BIN_DIR} LIBRARY DESTINATION lib)


########################################################
# Install headers
INSTALL(CODE "MESSAGE(\"Installing CORE headers...\")")
INSTALL(FILES ${LIBOPENMODELLERXML_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller)
INSTALL(FILES ${OM_CORE_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller)
INSTALL(FILES ${OM_OCCIO_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/occ_io)
INSTALL(FILES ${OM_ENVIO_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/env_io)
INSTALL(FILES ${OM_PRE_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/pre)
