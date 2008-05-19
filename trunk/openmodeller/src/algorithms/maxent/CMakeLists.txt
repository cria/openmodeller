ADD_DEFINITIONS(-DBOOST_DISABLE_THREADS) # no need to link pthread lib
ADD_DEFINITIONS(-fPIC) # avoid issues in 64bit platform

########################################################
# Files

SET (LIBMAXENT_SRCS
    maximum_entropy.cpp
    toolkit/src/display.cpp
    toolkit/src/eventspace.tcc
    toolkit/src/gistrainer.cpp
    toolkit/src/itemmap.tcc
    toolkit/src/maxentmodel.cpp
    toolkit/src/modelfile.cpp
    toolkit/src/trainer.cpp
)
#these are not needed to build but I specified them in 
#case we want to install the headers
SET (LIBMAXENT_HDRS
    maximum_entropy.hh
    toolkit/src/display.hpp
    toolkit/src/eventspace.hpp
    toolkit/src/finite.h
    toolkit/src/gistrainer.hpp
    toolkit/src/ext_algorithm.hpp
    toolkit/src/hash_map.hpp
    toolkit/src/itemmap.hpp
    toolkit/src/maxentmodel.hpp
    toolkit/src/meevent.hpp
    toolkit/src/modelfile.hpp
    toolkit/src/rfevent.hpp
    toolkit/src/trainer.hpp
)

########################################################
# Build

ADD_LIBRARY (maximum_entropy MODULE ${LIBMAXENT_SRCS})
TARGET_LINK_LIBRARIES(maximum_entropy ${Boost_LIBRARY_DIRS} openmodeller)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/toolkit/src
     ${Boost_INICLUDE_DIR}
)

########################################################
# Install

INSTALL(TARGETS maximum_entropy RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
