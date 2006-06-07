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

TARGET = om_console
win32{
  #on windows build libs into bin dir!
  DESTDIR=$${PREFIX}/omgui1
  LIBS += -L$${PREFIX}/omgui1 -lopenmodeller
}else{
  DESTDIR=$${PREFIX}/lib
  LIBS += -L$${PREFIX}/lib -lopenmodeller
}
message("Installing to $${PREFIX}")
TEMPLATE = app

#################################################################

contains(OMG_STATIC,true){
  #This option is used when creating a statically linked
  #lib to prevent double linked libraries. 
  CONFIG += create_prl        
win32{
  LIBS += c:/MinGW/lib/libexpat.a
} else {
}
  LIBS += $${PREFIX}/omgui1/libopenmodeller.a

}else{
}

#################################################################
            
SOURCES += 	om_console.cpp \
		occurrences_file.cpp \
		request_file.cpp \
		file_parser.cpp

