
########################################################
# Files

#SET (OM_HELLO_WORLD_SRCS
#     om_hello_world.cpp
#     )

SET (OM_LOAD_ALGS_SRCS
     om_load_algs.cpp
     )
SET (OM_TEST_LOG_SRCS
     om_test_log.cpp
     )

########################################################
# Build

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../src/openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

########################################################
# Build the varios command line apps
IF (WIN32)
  IF (MSVC)
    ADD_DEFINITIONS("-DCORE_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UCORE_EXPORT \"-DCORE_EXPORT=__declspec(dllimport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

#ADD_EXECUTABLE (om_hello_world ${OM_HELLO_WORLD_SRCS})

ADD_EXECUTABLE (om_load_algs ${OM_LOAD_ALGS_SRCS})
TARGET_LINK_LIBRARIES(om_load_algs openmodeller)

ADD_EXECUTABLE (om_test_log ${OM_TEST_LOG_SRCS})
TARGET_LINK_LIBRARIES(om_test_log openmodeller)
########################################################

#
# Run the hello world test - with custom pass / fail conditions
# The program is passed 'Hello' as a command line parameter
#
# this should pass
#ADD_TEST(om_hello_world_pass ${EXECUTABLE_OUTPUT_PATH}/om_hello_world Hello)
#SET_TESTS_PROPERTIES(om_hello_world_pass PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed;Hellojjj")
#SET_TESTS_PROPERTIES(om_hello_world_pass PROPERTIES PASS_REGULAR_EXPRESSION "Test passed;Hello")

# this should fail
#ADD_TEST(om_hello_world_fail ${EXECUTABLE_OUTPUT_PATH}/om_hello_world Hellojjj)
#SET_TESTS_PROPERTIES(om_hello_world_fail PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed;Hellojjj")
#SET_TESTS_PROPERTIES(om_hello_world_fail PROPERTIES PASS_REGULAR_EXPRESSION "Test passed;Hello")

#
# Run the load algs test with default pass / fail conditions
# where int main return is : 1 = fail and 0 = pass
#

ADD_TEST(om_load_algs ${EXECUTABLE_OUTPUT_PATH}/om_load_algs)
ADD_TEST(om_test_log ${EXECUTABLE_OUTPUT_PATH}/om_test_log)
