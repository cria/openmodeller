#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

message("**** Building om_console ****")

# include global settings from the top level dir
include (../../settings.pro)
TEMPLATE=app
TARGET = om_console
LIBS += $${OMLIBADD}
unix:DESTDIR=$${DESTDIR}/bin
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")

#################################################################


SOURCES += 	om_console.cpp \
		request_file.cpp \
		file_parser.cpp

