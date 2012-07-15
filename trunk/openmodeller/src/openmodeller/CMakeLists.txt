#############################################################
# sources

SET(OM_CORE_SRCS
  Algorithm.cpp 
  AlgorithmFactory.cpp 
  AlgParameter.cpp 
  AreaStats.cpp 
  CacheManager.cpp
  CallbackWrapper.cpp
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
  env_io/GdalRaster.cpp 
  env_io/WcsProxyRaster.cpp 
  env_io/RasterFactory.cpp 
  models/AlgoAdapterModel.cpp 
  models/AverageModel.cpp 
  models/ScaledModel.cpp 
  occ_io/OccurrencesFactory.cpp 
  occ_io/OccurrencesReader.cpp 
  occ_io/DelimitedTextOccurrences.cpp 
  occ_io/SerializedXmlOccurrences.cpp 
  pre/PreParameters.cpp
  pre/PreAlgorithm.cpp
  pre/PreJackknife.cpp
  pre/PreChiSquare.cpp
  pre/PreAlgorithmFactory.cpp
  pre/PreChiSquareFactory.cpp
  pre/PreJackknifeFactory.cpp
  ext/md5/md5.c
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

  IF (MSVC)
    # TerraLib has references to the wchar_t datatype, and 
    # depending on the MSVC version being used it may
    # be necessary to disable the use of the built-in datatype.
    # You can do this manually too in MSVC express by clicking 
    # on the openmodeller project, then from the top menu go:
    # Project -> Properties -> Configuration Properties ->
    # C/C++ -> Language -> Treat wchar_t as Built-in type
    # and change it to No (/Zc:wchar_t)
    IF (MSVC80)
      ADD_DEFINITIONS("/Zc:wchar_t")
    ELSE (MSVC80)
      ADD_DEFINITIONS("/Zc:wchar_t-")
    ENDIF (MSVC80)
  ENDIF (MSVC)

  SET(OM_CORE_SRCS
      ${OM_CORE_SRCS}
      TeDatabaseManager.cpp
      TeStringParser.cpp
      env_io/TerralibRaster.cpp
      occ_io/TeOccurrences.cpp
	  # Ado
      ${TERRALIB_INCLUDE_DIR}/TeAdoDB.cpp
	  ${TERRALIB_INCLUDE_DIR}/TeAdoOracle.cpp
	  ${TERRALIB_INCLUDE_DIR}/TeAdoSqlServer.cpp
	  # MrSID
	  #${TERRALIB_INCLUDE_DIR}/TeDecoderMrSID.cpp
	  #${TERRALIB_INCLUDE_DIR}/TeMrSID.cpp
	  # MySQL
	  ${TERRALIB_INCLUDE_DIR}/TeMySQL.cpp
	  # ORACLE
	  ${TERRALIB_INCLUDE_DIR}/TeOCIConnect.cpp
	  ${TERRALIB_INCLUDE_DIR}/TeOCICursor.cpp
	  ${TERRALIB_INCLUDE_DIR}/TeOCIOracle.cpp
	  ${TERRALIB_INCLUDE_DIR}/TeOCISDO.cpp
	  ${TERRALIB_INCLUDE_DIR}/TeOracleSpatial.cpp
	  # PostgreSQL
	  ${TERRALIB_INCLUDE_DIR}/TePGInterface.cpp
	  ${TERRALIB_INCLUDE_DIR}/TePGUtils.cpp
	  ${TERRALIB_INCLUDE_DIR}/TePostgreSQL.cpp
	  ${TERRALIB_INCLUDE_DIR}/TePostGIS.cpp
    )
	
	IF (WIN32)
  		SET(TERRALIB_LIBRARY
			${TERRALIB_LIBRARY}
			"$ENV{LIB_DIR}/lib/libmysql.lib"
			"$ENV{LIB_DIR}/lib/oci.lib"
			"$ENV{LIB_DIR}/lib/libpq.lib"
		)
	ENDIF (WIN32)
	
ENDIF (TERRALIB_FOUND)

IF (CURL_FOUND)

  SET(OM_CORE_SRCS
      ${OM_CORE_SRCS}
      occ_io/TapirOccurrences.cpp
      occ_io/GbifOccurrences.cpp
    )
ENDIF (CURL_FOUND)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
  env_io 
  occ_io
  pre
  ext/md5
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
    #explicitly remove the defs first otherwise msvc lists them twice TS
    REMOVE_DEFINITIONS("-Ddllexp=__declspec(dllimport)")
    REMOVE_DEFINITIONS("-DOM_XML_DLL_EXPORT=__declspec(dllimport)")
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
  
IF (MSVC)
  # SET_TARGET_PROPERTIES(openmodeller PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OM_LIB_DIR})
ENDIF (MSVC)

INSTALL(TARGETS openmodeller
  RUNTIME DESTINATION ${OM_BIN_DIR}
  LIBRARY DESTINATION lib)

SET(OM_CORE_HDRS
  AlgMetadata.hh 
  AlgorithmFactory.hh  
  Algorithm.hh     
  AlgParameter.hh      
  AreaStats.hh     
  CacheManager.hh
  CallbackWrapper.hh
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
occ_io/OccurrencesReader.hh
occ_io/DelimitedTextOccurrences.hh
occ_io/SerializedXmlOccurrences.hh
)

IF (CURL_FOUND)
  SET(OM_OCCIO_HDRS
      ${OM_OCCIO_HDRS}
      occ_io/TapirOccurrences.hh
      occ_io/GbifOccurrences.hh
     )
ENDIF (CURL_FOUND)

SET (OM_ENVIO_HDRS
env_io/GeoTransform.hh
env_io/Header.hh
env_io/Map.hh
env_io/MapIterator.hh
env_io/Raster.hh
env_io/RasterFactory.hh
env_io/GdalRaster.hh
env_io/WcsProxyRaster.hh
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
      env_io/TerralibRaster.hh
    )
ENDIF (TERRALIB_FOUND)

SET (OM_PRE_HDRS
pre/PreMultiContainer.hh
pre/PreParameters.hh
pre/PreAlgorithm.hh
pre/PreJackknife.hh
pre/PreChiSquare.hh
pre/PreFactory.hh
pre/PreAlgorithmFactory.hh
pre/PreChiSquareFactory.hh
pre/PreJackknifeFactory.hh
)


########################################################
# Make headers appear in MSVC nicely
IF (MSVC)
  SOURCE_GROUP( "Header Files" FILES ${OM_CORE_HEADERS} ${OM_OCCIO_HDRS} ${OM_ENVIO_HDRS})
ENDIF (MSVC)


########################################################
# Install headers
INSTALL(CODE "MESSAGE(\"Installing CORE headers...\")")
INSTALL(FILES ${LIBOPENMODELLERXML_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller)
INSTALL(FILES ${OM_CORE_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller)
INSTALL(FILES ${OM_OCCIO_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/occ_io)
INSTALL(FILES ${OM_ENVIO_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/env_io)
INSTALL(FILES ${OM_PRE_HDRS} DESTINATION ${OM_INCLUDE_DIR}/openmodeller/pre)
