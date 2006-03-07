#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

TEMPLATE=subdirs

SUBDIRS=src/openmodeller \
 	src/algorithms \
	src/console

message(Install prefix set to $$PREFIX)
