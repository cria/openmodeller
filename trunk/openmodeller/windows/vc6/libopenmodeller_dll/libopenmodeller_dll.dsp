# Microsoft Developer Studio Project File - Name="libopenmodeller_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libopenmodeller_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libopenmodeller_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libopenmodeller_dll.mak" CFG="libopenmodeller_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libopenmodeller_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libopenmodeller_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libopenmodeller_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOPENMODELLER_DLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../inc" /I "../../inc/serialization" /I "$(GDAL_HOME)/include" /I "../../lib/serialization" /I "$(EXPAT_HOME)/Source/lib" /I "$(JAVA_HOME)/include" /I "$(JAVA_HOME)/bin" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOPENMODELLER_DLL_EXPORTS" /D VERSION=\"0.2\" /D PLUGINPATH=$(OM_PLUGIN_PATH) /D CONFIG_FILE=\"om_config.txt\" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 libexpat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gdal_i.lib /nologo /dll /machine:I386 /out:"Release/libopenmodeller.dll" /libpath:"$(GDAL_HOME)\lib" /libpath:"$(EXPAT_HOME)/libs"
# Begin Special Build Tool
TargetDir=.\Release
TargetName=libopenmodeller
SOURCE="$(InputPath)"
PostBuild_Desc=Install
PostBuild_Cmds=echo on	..\CopyDll.bat build $(TargetDir) $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libopenmodeller_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOPENMODELLER_DLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../inc" /I "../../inc/serialization" /I "$(GDAL_HOME)/include" /I "../../lib/serialization" /I "$(EXPAT_HOME)/Source/lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOPENMODELLER_DLL_EXPORTS" /D VERSION=\"0.2\" /D PLUGINPATH=$(OM_PLUGIN_PATH) /D CONFIG_FILE=\"om_config.txt\" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libexpat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gdal_i.lib /nologo /dll /map /debug /machine:I386 /out:"Debug/libopenmodeller.dll" /pdbtype:sept /libpath:"$(GDAL_HOME)\lib" /libpath:"$(EXPAT_HOME)/libs"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=.\Debug
TargetName=libopenmodeller
SOURCE="$(InputPath)"
PostBuild_Desc=Install (debug)
PostBuild_Cmds=echo on	..\CopyDll.bat build_debug $(TargetDir) $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libopenmodeller_dll - Win32 Release"
# Name "libopenmodeller_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\alg_parameter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\algorithm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\algorithm_factory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\area_stats.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\conf_matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\control.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\environment.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\file_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\map_format.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\occurrence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\occurrences.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\os_specific_win.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\random.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\sampled_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\sampler.cpp
# End Source File
# End Group
# Begin Group "scew"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\serialization\scew\attribute.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\element.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\error.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\parser.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\str.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\tree.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\writer.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\xattribute.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\xerror.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\xhandler.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\xparser.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\scew\xprint.c
# End Source File
# End Group
# Begin Group "env_io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\env_io\geo_transform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\env_io\header.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\env_io\map.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\env_io\raster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\env_io\raster_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\env_io\raster_gdal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\env_io\raster_mem.cpp
# End Source File
# End Group
# Begin Group "serialization"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\serialization\ascii_deserializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\ascii_serializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\deserializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\serializable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\serializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\serializer_factory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\xml_deserializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\serialization\xml_serializer.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "lib_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\inc\algorithm_factory.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\environment.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\file_parser.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\list.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\map_format.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\occurrence.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_alg_parameter.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_algorithm.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_algorithm_metadata.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_area_stats.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_conf_matrix.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_control.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_defs.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_log.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_occurrences.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_sampled_data.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_sampler.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\os_specific.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\random.hh
# End Source File
# End Group
# Begin Group "env_io_h"

# PROP Default_Filter ""
# End Group
# Begin Group "serialization_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\inc\serialization\ascii_deserializer.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\serialization\ascii_serializer.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\serialization\om_deserializer.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\om_serializable.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\serialization\om_serializer.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\serialization\om_serializer_factory.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\serialization\stack.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\serialization\xml_deserializer.hh
# End Source File
# Begin Source File

SOURCE=..\..\inc\serialization\xml_serializer.hh
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
