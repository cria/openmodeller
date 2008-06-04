#############################################################
# process subdirs

SUBDIRS(openmodeller algorithms console)

IF (NOT WIN32)
  SUBDIRS (soap)
ENDIF (NOT WIN32)

IF (WITH_BINDINGS)
  SUBDIRS (swig)
ENDIF (WITH_BINDINGS)

