#
# This is the DartConfig.cmake file for the Insight Toolkit.
# The variables here would be set by the software project administrator
# and should not be modified by the user.
#

# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "1:00:00 EST")

# Dart server to submit results (used by client)
SET (DROP_METHOD "ftp")
SET (DROP_SITE "test.cria.org.br")
SET (DROP_LOCATION "/incoming")
SET (DROP_SITE_USER "anonymous")
SET (DROP_SITE_PASSWORD "")
SET (TRIGGER_SITE 
       "http://${DROP_SITE}/Dart/cgi-bin/TriggerDart.cgi")

# Dart server configuration 
SET (CVS_WEB_URL "http://${DROP_SITE}/cgi-bin/cvsweb.cgi/Insight/")
SET (CVS_WEB_CVSROOT "Insight")

OPTION(BUILD_DOXYGEN "Build source documentation using doxygen" "On")
SET (DOXYGEN_URL "http://${DROP_SITE}/Insight/Doxygen/html/" )
SET (USE_DOXYGEN "On")
SET (DOXYGEN_CONFIG "${PROJECT_BINARY_DIR}/doxygen.config" )

SET (USE_GNATS "On")
SET (GNATS_WEB_URL "http://${DROP_SITE}/cgi-bin/gnatsweb.pl/Insight/")

# Problem build email delivery variables
SET (DELIVER_BROKEN_BUILD_EMAIL "Continuous")
SET (EMAIL_FROM "someone@somewhere.com")
SET (DARTBOARD_BASE_URL "http://www.itk.org/Testing")
SET (EMAIL_PROJECT_NAME "Insight")
SET (BUILD_MONITORS "{.* monitor@aaa.com}")
SET (CVS_IDENT_TO_EMAIL "{cvsuser1 aa@aaaa.com} {cvsuser2 bb@bbbb.com}")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_CONFIGURE_FAILURES "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_BUILD_ERRORS "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_BUILD_WARNINGS "0")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_TEST_NOT_RUNS "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_TEST_FAILURES "0")

# Copy over the testing logo
#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/TestingLogo.gif 
#     ${PROJECT_BINARY_DIR}/Testing/HTML/TestingResults/Icons/Logo.gif COPYONLY)

# If DROP_METHOD is set to "scp", then add this FIND_PROGRAM to your DartConfig
#FIND_PROGRAM(SCPCOMMAND scp DOC 
#     "Path to scp command, sometimes used for submitting Dart results.")


#
# End of DartConfig.cmake
#