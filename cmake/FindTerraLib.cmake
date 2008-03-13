# CMake module to search for TERRALIB library
#
# If it's found it sets TERRALIB_FOUND to TRUE
# and following variables are set:
#    TERRALIB_INCLUDE_DIR
#    TERRALIB_LIBRARY


FIND_PATH(TERRALIB_INCLUDE_DIR TeRaster.h
  /usr/local/include 
  /usr/include 
  "$ENV{LIB_DIR}/include/terralib"
  c:/msys/local/include
  )
  
FIND_LIBRARY(TERRALIB_LIBRARY NAMES terralib PATHS 
  /usr/local/lib 
  /usr/lib 
  "$ENV{LIB_DIR}/lib"
  c:/msys/local/lib
  )
  
IF (TERRALIB_INCLUDE_DIR AND TERRALIB_LIBRARY)
   SET(TERRALIB_FOUND TRUE)
ENDIF (TERRALIB_INCLUDE_DIR AND TERRALIB_LIBRARY)


IF (TERRALIB_FOUND)

   IF (NOT TERRALIB_FIND_QUIETLY)
      MESSAGE(STATUS "Found TERRALIB: ${TERRALIB_LIBRARY}")
   ENDIF (NOT TERRALIB_FIND_QUIETLY)

ELSE (TERRALIB_FOUND)

   IF (TERRALIB_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find TERRALIB")
   ELSE (TERRALIB_FIND_REQUIRED)
      # Avoid cmake complaints if terralib is not found
      SET(TERRALIB_INCLUDE_DIR "")
      SET(TERRALIB_LIBRARY "")
   ENDIF (TERRALIB_FIND_REQUIRED)

ENDIF (TERRALIB_FOUND)
