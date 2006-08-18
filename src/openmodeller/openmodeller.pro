#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../settings.pro)

win32{
  #on windows build libs into bin dir!
  CONFIG(debug, debug|release){
    DESTDIR=$${PREFIX}/omgui1-debug
    TARGET = openmodeller-debug
  }else{
    DESTDIR=$${PREFIX}/omgui1
    TARGET = openmodeller
  }
}else{
  DESTDIR=$${PREFIX}/lib
  TARGET = openmodeller
}

TEMPLATE = lib


win32{
      SOURCES += os_specific_win.cpp 
} else {
      SOURCES += os_specific.cpp 
}

#################################################################

contains(OMG_STATIC,true){
  #This option is used when creating a statically linked
  #lib to prevent double linked libraries. 
  CONFIG += create_prl        
}else{
}

#################################################################

#
# env_io
#

HEADERS +=  env_io/GeoTransform.hh \
            env_io/Header.hh \
            env_io/Map.hh \
            env_io/MapIterator.hh \
            env_io/Raster.hh \
            env_io/RasterFactory.hh \
            env_io/RasterGdal.hh 
            
SOURCES +=  env_io/GeoTransform.cpp \
            env_io/Header.cpp \
            env_io/Map.cpp \
            env_io/MapIterator.cpp \
            env_io/Raster.cpp \
            env_io/RasterFactory.cpp \
            env_io/RasterGdal.cpp 

#
# models
#
            
HEADERS +=  models/AlgoAdapterModel.hh \
            models/AverageModel.hh \
            models/ScaledModel.hh 
            
SOURCES +=  models/AlgoAdapterModel.cpp \
            models/AverageModel.cpp \
            models/ScaledModel.cpp 

#
# openmodeller
# 
HEADERS +=  AlgMetadata.hh \
            AlgParameter.hh \
            Algorithm.hh \
            AlgorithmFactory.hh \
            AreaStats.hh \
            Configurable.hh \
            Configuration.hh \
            ConfusionMatrix.hh \
            Environment.hh \
            Exceptions.hh \
            Log.hh \
            MapFormat.hh \
            Model.hh \
            Normalizable.hh \
            Occurrence.hh \
            Occurrences.hh \
            OpenModeller.hh \
            Projector.hh \
            Random.hh \
            Sample.hh \
            SampleExpr.hh \
            SampleExprVar.hh \
            Sampler.hh \
            ignorecase_traits.hh \
            om.hh \
            om_defs.hh \
            os_specific.hh \
            refcount.hh 
            
SOURCES +=  AlgParameter.cpp \
            Algorithm.cpp \
            AlgorithmFactory.cpp \
            AreaStats.cpp \
            Configuration.cpp \
            Configuration_xmlio.cpp \
            ConfusionMatrix.cpp \
            Environment.cpp \
            Log.cpp \
            Occurrence.cpp \
            Occurrences.cpp \
            OpenModeller.cpp \
            MapFormat.cpp \
            #Model.cpp \
            ignorecase_traits.cpp \
            Projector.cpp \
            Random.cpp \
            Sample.cpp \
            Sampler.cpp 

