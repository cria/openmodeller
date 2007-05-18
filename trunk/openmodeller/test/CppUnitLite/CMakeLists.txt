########################################################
# Files

SET (LIBCPPUNITLITE_SRCS
    Test.cpp
    TestRegistry.cpp
    Failure.cpp
    SimpleString.cpp
    TestResult.cpp
    )
SET (LIBCPPUNITLITE_HDRS
    TestResult.h
    Test.h
    TestRegistry.h
    Failure.h
    SimpleString.h
    TestHarness.h
    )

########################################################
# Build

ADD_LIBRARY (cppunitlite SHARED ${LIBCPPUNITLITE_SRCS})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
)

########################################################
# Install

#INSTALL(TARGETS cppunitlite RUNTIME DESTINATION ${OM_ALG_DIR} LIBRARY DESTINATION ${OM_ALG_DIR})

