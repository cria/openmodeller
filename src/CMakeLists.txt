#############################################################
# process subdirs

SUBDIRS(openmodeller algorithms console soap)

IF (HAVE_PYTHON)
  SUBDIRS (swig)
ENDIF (HAVE_PYTHON)

