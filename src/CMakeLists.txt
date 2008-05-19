#############################################################
# process subdirs

SUBDIRS(openmodeller algorithms console)

IF (NOT WIN32)
  SUBDIRS (soap)
ENDIF (NOT WIN32)

IF (HAVE_PYTHON)
  SUBDIRS (swig)
ENDIF (HAVE_PYTHON)

