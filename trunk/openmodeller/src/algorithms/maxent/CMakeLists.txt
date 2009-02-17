ADD_DEFINITIONS(-DBOOST_DISABLE_THREADS) # no need to link pthread lib
ADD_DEFINITIONS(-fPIC) # avoid issues in 64bit platform

########################################################
# Fortran library

IF (OM_USE_FORTRAN)
  # Only tested with g77
  SET(CMAKE_GENERATOR_FC "g77")
  ENABLE_LANGUAGE(Fortran)
ENDIF (OM_USE_FORTRAN)

IF (CMAKE_Fortran_COMPILER_WORKS)

  MESSAGE(STATUS "Fortran compiler: ${CMAKE_Fortran_COMPILER}")

  FIND_PACKAGE(FortranLibs)

  IF (FORTRANLIBS_FOUND)
    ADD_DEFINITIONS(-DHAVE_FORTRAN)
  ENDIF (FORTRANLIBS_FOUND)

ENDIF (CMAKE_Fortran_COMPILER_WORKS)

########################################################
# C++ files

SET (LIBMAXENT_SRCS
    maximum_entropy.cpp
    toolkit/src/display.cpp
    toolkit/src/eventspace.tcc
    toolkit/src/gistrainer.cpp
    toolkit/src/itemmap.tcc
    toolkit/src/maxentmodel.cpp
    toolkit/src/modelfile.cpp
    toolkit/src/trainer.cpp
    toolkit/src/seqtrainer.cpp
)

IF (CMAKE_Fortran_COMPILER_WORKS AND FORTRANLIBS_FOUND)
  SET(LIBMAXENT_SRCS
    ${LIBMAXENT_SRCS}
    toolkit/src/lbfgstrainer.cpp
    toolkit/src/lbfgs_wrapper.c
    toolkit/src/lbfgs.f
  )
ENDIF (CMAKE_Fortran_COMPILER_WORKS AND FORTRANLIBS_FOUND)

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
    toolkit/src/seqtrainer.hpp
)

IF (CMAKE_Fortran_COMPILER_WORKS AND FORTRANLIBS_FOUND)
  SET(LIBMAXENT_HDRS
    ${LIBMAXENT_HDRS}
    toolkit/src/lbfgs.h
    toolkit/src/lbfgstrainer.hpp
  )
ENDIF (CMAKE_Fortran_COMPILER_WORKS AND FORTRANLIBS_FOUND)

########################################################
# Build

ADD_LIBRARY (maximum_entropy MODULE ${LIBMAXENT_SRCS})

IF (CMAKE_Fortran_COMPILER_WORKS AND FORTRANLIBS_FOUND)
  SET_TARGET_PROPERTIES(maximum_entropy PROPERTIES LINKER_LANGUAGE CXX)
  TARGET_LINK_LIBRARIES(maximum_entropy openmodeller ${FORTRAN_LIBRARY})
ELSE (CMAKE_Fortran_COMPILER_WORKS AND FORTRANLIBS_FOUND)
  TARGET_LINK_LIBRARIES(maximum_entropy openmodeller)
ENDIF (CMAKE_Fortran_COMPILER_WORKS AND FORTRANLIBS_FOUND)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/toolkit/src
     ${Boost_INCLUDE_DIR}
)

########################################################
# Install

INSTALL(TARGETS maximum_entropy RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
