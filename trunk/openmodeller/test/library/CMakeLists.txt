########################################################
# Files

SET (LIBUNITTEST_SRCS
     #test_icstring.cpp
     test_sample.cpp
     #test_sampleexprvar.cpp
     #test_configuration.cpp
     #test_sampleexpr.cpp
)

SET (CPPTEST_SRCS
     test_main.cpp
     )

########################################################
# Build

ADD_LIBRARY (unittest SHARED ${LIBUNITTEST_SRCS})
TARGET_LINK_LIBRARIES(unittest openmodeller cppunitlite)

ADD_EXECUTABLE(cpptest ${CPPTEST_SRCS})
TARGET_LINK_LIBRARIES(cpptest unittest)

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/..
     ${CMAKE_CURRENT_SOURCE_DIR}/../CppUnitLite
     ${CMAKE_CURRENT_SOURCE_DIR}/../../src/openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

########################################################
# Install


ADD_TEST(cpptest ${EXECUTABLE_OUTPUT_PATH}/cpptest)
