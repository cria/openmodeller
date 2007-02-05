#############################################################
# process subdirs

SUBDIRS(openmodeller console algorithms)

IF (HAVE_PYTHON)
  SUBDIRS (swig)
ENDIF (HAVE_PYTHON)

