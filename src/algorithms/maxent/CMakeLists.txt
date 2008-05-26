ADD_DEFINITIONS(-DBOOST_DISABLE_THREADS) # no need to link pthread lib
ADD_DEFINITIONS(-fPIC) # avoid issues in 64bit platform

########################################################
# Fortran library
ENABLE_LANGUAGE(Fortran)

MESSAGE(STATUS "CMAKE_Fortran_FLAGS: ${CMAKE_Fortran_FLAGS}")

IF (CMAKE_Fortran_COMPILER_WORKS)

  FIND_PACKAGE(FortranLibs REQUIRED)

  ADD_DEFINITIONS(-DHAVE_FORTRAN)
  #ADD_LIBRARY (lbfgs SHARED toolkit/src/lbfgs.f)
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
)

IF (CMAKE_Fortran_COMPILER_WORKS)
  SET(LIBMAXENT_SRCS
    ${LIBMAXENT_SRCS}
    toolkit/src/lbfgstrainer.cpp
    toolkit/src/lbfgs_wrapper.c
    toolkit/src/lbfgs.f
  )
ENDIF (CMAKE_Fortran_COMPILER_WORKS)

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

IF (CMAKE_Fortran_COMPILER_WORKS)
  SET(LIBMAXENT_HDRS
    ${LIBMAXENT_HDRS}
    toolkit/src/lbfgs.h
    toolkit/src/lbfgstrainer.hpp
  )
ENDIF (CMAKE_Fortran_COMPILER_WORKS)

########################################################
# Build

ADD_LIBRARY (maximum_entropy MODULE ${LIBMAXENT_SRCS})

IF (CMAKE_Fortran_COMPILER_WORKS)
  SET_TARGET_PROPERTIES(maximum_entropy PROPERTIES LINKER_LANGUAGE CXX)
  TARGET_LINK_LIBRARIES(maximum_entropy ${Boost_LIBRARY_DIRS} openmodeller ${FORTRAN_LIBRARY})
ELSE (CMAKE_Fortran_COMPILER_WORKS)
  TARGET_LINK_LIBRARIES(maximum_entropy ${Boost_LIBRARY_DIRS} openmodeller)
ENDIF (CMAKE_Fortran_COMPILER_WORKS)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/toolkit/src
     ${Boost_INCLUDE_DIR}
)

########################################################
# Install

INSTALL(TARGETS maximum_entropy RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})
