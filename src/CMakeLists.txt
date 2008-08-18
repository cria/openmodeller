#############################################################
# process subdirs

SUBDIRS(openmodeller algorithms console)

IF (OM_BUILD_SERVICE AND NOT WIN32)
  SUBDIRS (soap)
ENDIF (OM_BUILD_SERVICE AND NOT WIN32)

IF (OM_BUILD_BINDINGS)
  SUBDIRS (swig)
ENDIF (OM_BUILD_BINDINGS)

