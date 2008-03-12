########################################################
# Do not need to edit anything in this part - go
# to bottom of this file and copy one of the blocks there
# if you want to add a new test.
########################################################

#######################################################
# Build

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/../../src/openmodeller
     ${CMAKE_CURRENT_SOURCE_DIR}/../../src
     ${CXXTEST_INCLUDE_DIR}
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


########################################################
# Add a compiler define that specifies where test data
# lives in the file system. The cmake variable ${TEST_DATA_DIR}
# is defined in the top level CMakeLists.txt.

#MESSAGE("Test Data Dir: ${TEST_DATA_DIR}")
ADD_DEFINITIONS(-DEXAMPLE_DIR="\\"${EXAMPLE_DIR}\\"")
ADD_DEFINITIONS(-DTEST_DATA_DIR="\\"${TEST_DATA_DIR}\\"")

########################################################
# Add you new tests in here...
########################################################

#Load algs tests
SET (OM_LOAD_ALGS_SRCS om_load_algs.cpp )
ADD_EXECUTABLE (om_load_algs ${OM_LOAD_ALGS_SRCS})
TARGET_LINK_LIBRARIES(om_load_algs openmodeller)
ADD_TEST(om_load_algs ${EXECUTABLE_OUTPUT_PATH}/om_load_algs)

#Log Tests
SET (OM_TEST_LOG_SRCS  om_test_log.cpp )
ADD_EXECUTABLE (om_test_log ${OM_TEST_LOG_SRCS})
TARGET_LINK_LIBRARIES(om_test_log openmodeller)
ADD_TEST(om_test_log ${EXECUTABLE_OUTPUT_PATH}/om_test_log)

#Area Stats
SET (OM_TEST_AREASTATS_SRCS om_test_areastats.cpp)
ADD_EXECUTABLE (om_test_areastats ${OM_TEST_AREASTATS_SRCS})
TARGET_LINK_LIBRARIES(om_test_areastats openmodeller)
ADD_TEST(om_test_areastats ${EXECUTABLE_OUTPUT_PATH}/om_test_areastats)

#Sample Tests
SET (OM_TEST_SAMPLE_SRCS om_test_sample.cpp)
ADD_EXECUTABLE (om_test_sample ${OM_TEST_SAMPLE_SRCS})
TARGET_LINK_LIBRARIES(om_test_sample openmodeller)
ADD_TEST(om_test_sample ${EXECUTABLE_OUTPUT_PATH}/om_test_sample)

#ignorecase_traits Tests
SET (OM_TEST_ICSTRING_SRCS om_test_icstring.cpp)
ADD_EXECUTABLE (om_test_icstring ${OM_TEST_ICSTRING_SRCS})
TARGET_LINK_LIBRARIES(om_test_icstring openmodeller)
ADD_TEST(om_test_icstring ${EXECUTABLE_OUTPUT_PATH}/om_test_icstring)

#SampleExpr Tests
SET (OM_TEST_SAMPLEEXPR_SRCS om_test_sampleexpr.cpp)
ADD_EXECUTABLE (om_test_sampleexpr ${OM_TEST_SAMPLEEXPR_SRCS})
TARGET_LINK_LIBRARIES(om_test_sampleexpr openmodeller)
ADD_TEST(om_test_sampleexpr ${EXECUTABLE_OUTPUT_PATH}/om_test_sampleexpr)

#Random Tests
SET (OM_TEST_RANDOM_SRCS om_test_random.cpp)
ADD_EXECUTABLE (om_test_random ${OM_TEST_RANDOM_SRCS})
TARGET_LINK_LIBRARIES(om_test_random openmodeller)
ADD_TEST(om_test_random ${EXECUTABLE_OUTPUT_PATH}/om_test_random)

#Exceptions Tests
SET (OM_TEST_EXCEPTIONS_SRCS om_test_exceptions.cpp)
ADD_EXECUTABLE (om_test_exceptions ${OM_TEST_EXCEPTIONS_SRCS})
TARGET_LINK_LIBRARIES(om_test_exceptions openmodeller)
ADD_TEST(om_test_exceptions ${EXECUTABLE_OUTPUT_PATH}/om_test_exceptions)

#Configuration Tests
SET (OM_TEST_CONFIGURATION_SRCS om_test_configuration.cpp)
ADD_EXECUTABLE (om_test_configuration ${OM_TEST_CONFIGURATION_SRCS})
TARGET_LINK_LIBRARIES(om_test_configuration openmodeller)
ADD_TEST(om_test_configuration ${EXECUTABLE_OUTPUT_PATH}/om_test_configuration)

#refcount Tests
SET (OM_TEST_REFCOUNT_SRCS om_test_refcount.cpp)
ADD_EXECUTABLE (om_test_refcount ${OM_TEST_REFCOUNT_SRCS})
TARGET_LINK_LIBRARIES(om_test_refcount openmodeller)
ADD_TEST(om_test_refcount ${EXECUTABLE_OUTPUT_PATH}/om_test_refcount)

#MapFormat Tests
SET (OM_TEST_MAPFORMAT_SRCS om_test_mapformat.cpp)
ADD_EXECUTABLE (om_test_mapformat ${OM_TEST_MAPFORMAT_SRCS})
TARGET_LINK_LIBRARIES(om_test_mapformat openmodeller)
ADD_TEST(om_test_mapformat ${EXECUTABLE_OUTPUT_PATH}/om_test_mapformat)

#ScaleNormalizer Tests
SET (OM_TEST_SCALENORMALIZER_SRCS om_test_scalenormalizer.cpp)
ADD_EXECUTABLE (om_test_scalenormalizer ${OM_TEST_SCALENORMALIZER_SRCS})
TARGET_LINK_LIBRARIES(om_test_scalenormalizer openmodeller)
ADD_TEST(om_test_scalenormalizer ${EXECUTABLE_OUTPUT_PATH}/om_test_scalenormalizer)

#SampleExprVar Tests
SET (OM_TEST_SAMPLEEXPRVAR_SRCS om_test_sampleexprvar.cpp)
ADD_EXECUTABLE (om_test_sampleexprvar ${OM_TEST_SAMPLEEXPRVAR_SRCS})
TARGET_LINK_LIBRARIES(om_test_sampleexprvar openmodeller)
ADD_TEST(om_test_sampleexprvar ${EXECUTABLE_OUTPUT_PATH}/om_test_sampleexprvar)

#Occurrence Tests
SET (OM_TEST_OCCURRENCE_SRCS om_test_occurrence.cpp)
ADD_EXECUTABLE (om_test_occurrence ${OM_TEST_OCCURRENCE_SRCS})
TARGET_LINK_LIBRARIES(om_test_occurrence openmodeller)
ADD_TEST(om_test_occurrence ${EXECUTABLE_OUTPUT_PATH}/om_test_occurrence)

#AlgParameter Tests
SET (OM_TEST_ALGPARAMETER_SRCS om_test_algparameter.cpp)
ADD_EXECUTABLE (om_test_algparameter ${OM_TEST_ALGPARAMETER_SRCS})
TARGET_LINK_LIBRARIES(om_test_algparameter openmodeller)
ADD_TEST(om_test_algparameter ${EXECUTABLE_OUTPUT_PATH}/om_test_algparameter)

#AlgorithmFactory Tests
SET (OM_TEST_ALGORITHMFACTORY_SRCS om_test_algorithmfactory.cpp)
ADD_EXECUTABLE (om_test_algorithmfactory ${OM_TEST_ALGORITHMFACTORY_SRCS})
TARGET_LINK_LIBRARIES(om_test_algorithmfactory openmodeller)
ADD_TEST(om_test_algorithmfactory ${EXECUTABLE_OUTPUT_PATH}/om_test_algorithmfactory)

#Jackknife Tests
SET (OM_TEST_JACKKNIFE_SRCS om_test_jackknife.cpp)
ADD_EXECUTABLE (om_test_jackknife ${OM_TEST_JACKKNIFE_SRCS})
TARGET_LINK_LIBRARIES(om_test_jackknife openmodeller)
ADD_TEST(om_test_jackknife ${EXECUTABLE_OUTPUT_PATH}/om_test_jackknife)

#ChiSquare Tests
SET (OM_TEST_CHISQUARE_SRCS om_test_chisquare.cpp)
ADD_EXECUTABLE (om_test_chisquare ${OM_TEST_CHISQUARE_SRCS})
TARGET_LINK_LIBRARIES(om_test_chisquare openmodeller)
ADD_TEST(om_test_chisquare ${EXECUTABLE_OUTPUT_PATH}/om_test_chisquare)
