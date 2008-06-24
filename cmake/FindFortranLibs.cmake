
# CMake module to search for Fortran libraries
#
# If it's found it sets FORTRANLIBS_FOUND to TRUE
# and following variables are set:
#    FORTRAN_INCLUDE_DIR
#    FORTRAN_LIBRARY

# Normally there is no need to specify /usr/... paths because 
# cmake will look there automatically. However the NO_DEFAULT_PATH
# prevents this behaviour allowing you to use no standard file
# locations in preference over standard ones. Note in this case
# you then need to explicitly add /usr and /usr/local prefixes
# to the search list. This applies both to FIND_PATH and FIND_LIBRARY
FIND_PATH(FORTRAN_INCLUDE_DIR g2c.h 
  "$ENV{LIB_DIR}/include/"
  /usr/local/include 
  /usr/include 
  /usr/lib/gcc-lib/i386-redhat-linux/3.2/include 
  c:/msys/local/include
  NO_DEFAULT_PATH
  )

FIND_LIBRARY(FORTRAN_LIBRARY NAMES g2c PATHS 
  "$ENV{LIB_DIR}/lib"
  /usr/local/lib 
  /usr/lib 
  /usr/lib/gcc-lib/i386-redhat-linux/3.2
  c:/msys/local/lib
  NO_DEFAULT_PATH
  )

IF (FORTRAN_INCLUDE_DIR AND FORTRAN_LIBRARY)
   SET(FORTRANLIBS_FOUND TRUE)
ENDIF (FORTRAN_INCLUDE_DIR AND FORTRAN_LIBRARY)


IF (FORTRANLIBS_FOUND)

   IF (NOT FORTRANLIBS_FIND_QUIETLY)
      MESSAGE(STATUS "Found Fortran lib: ${FORTRAN_LIBRARY}")
   ENDIF (NOT FORTRANLIBS_FIND_QUIETLY)

ELSE (FORTRANLIBS_FOUND)

   IF (FORTRANLIBS_FIND_REQUIRED)
     MESSAGE(FATAL_ERROR "Could not find Fortran lib")
   ELSE (FORTRANLIBS_FIND_REQUIRED)
     IF (NOT FORTRANLIBS_FIND_QUIETLY)
        MESSAGE(STATUS "Could not find Fortran lib")
        # Avoid cmake complaints if Fortran libs are not found
        SET(FORTRAN_INCLUDE_DIR "")
        SET(FORTRAN_LIBRARY "")
     ENDIF (NOT FORTRANLIBS_FIND_QUIETLY)
   ENDIF (FORTRANLIBS_FIND_REQUIRED)

ENDIF (FORTRANLIBS_FOUND)
