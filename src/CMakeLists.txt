#############################################################
# process subdirs

SUBDIRS(openmodeller algorithms console )

IF (HAVE_PYTHON)
  SUBDIRS (swig)
ENDIF (HAVE_PYTHON)

