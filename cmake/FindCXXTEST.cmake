# CMake module to search for Expat library
# (library for parsing XML files)
#
# If it's found it sets CXXTEST_FOUND to TRUE
# and following variables are set:
#    CXXTEST_INCLUDE_DIR


FIND_PATH(CXXTEST_INCLUDE_DIR cxxtest/TestSuite.h 
        /usr/local/include 
        /usr/include 
        c:/msys/local/include
        )


IF (CXXTEST_INCLUDE_DIR)
   SET(CXXTEST_FOUND TRUE)
ENDIF (CXXTEST_INCLUDE_DIR)


IF (CXXTEST_FOUND)

   IF (NOT CXXTEST_FIND_QUIETLY)
      MESSAGE(STATUS "Found CXXTEST: ${CXXTEST_LIBRARY}")
   ENDIF (NOT CXXTEST_FIND_QUIETLY)

ELSE (CXXTEST_FOUND)

   IF (CXXTEST_FIND_REQUIRED)
     MESSAGE(FATAL_ERROR "Could not find CXXTEST")
   ELSE (CXXTEST_FIND_REQUIRED)
     IF (NOT CXXTEST_FIND_QUIETLY)
        MESSAGE(STATUS "Could not find CXXTEST")
     ENDIF (NOT CXXTEST_FIND_QUIETLY)
   ENDIF (CXXTEST_FIND_REQUIRED)

ENDIF (CXXTEST_FOUND)

