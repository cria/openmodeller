#############################################################
# process subdirs

SUBDIRS(openmodeller console)

IF (HAVE_PYTHON)
  SUBDIRS (swig)
ENDIF (HAVE_PYTHON)

