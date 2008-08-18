#############################################################
# process subdirs

SUBDIRS(openmodeller algorithms console)

IF (OM_BUILD_SERVICE AND NOT WIN32)
  SUBDIRS (soap)
ENDIF (OM_BUILD_SERVICE AND NOT WIN32)

IF (WITH_BINDINGS)
  SUBDIRS (swig)
ENDIF (WITH_BINDINGS)

