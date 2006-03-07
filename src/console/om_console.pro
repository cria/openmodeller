#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../settings.pro)

TARGET = om_console
win32{
  #on windows build libs into bin dir!
  DESTDIR=$${PREFIX}/omgui1
}else{
  DESTDIR=$${PREFIX}/lib
}
message("Installing to $${PREFIX}")
TEMPLATE = app
LIBS += -L$${PREFIX}/omgui1 -lopenmodeller

#################################################################

contains(OMG_STATIC,true){
  #This option is used when creating a statically linked
  #lib to prevent double linked libraries. 
  CONFIG += create_prl        
}else{
}

#################################################################
            
SOURCES += 	om_console.cpp \
		occurrences_file.cpp \
		request_file.cpp \
		file_parser.cpp

