# Microsoft Developer Studio Project File - Name="libopenmodeller_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libopenmodeller_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libopenmodeller_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libopenmodeller_static.mak" CFG="libopenmodeller_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libopenmodeller_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libopenmodeller_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libopenmodeller_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../inc" /I "../../inc/env_io" /I "$(GDAL_HOME)/include" /I "../../lib/serialization" /I "$(EXPAT_HOME)/Source/lib" /I "$(JAVA_HOME)/include" /I "$(JAVA_HOME)/bin" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D VERSION=\"0.2\" /D PLUGINPATH=$(OM_PLUGIN_PATH) /D CONFIG_FILE=\"om_config.txt\" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
TargetDir=.\Release
TargetName=libopenmodeller_static
SOURCE="$(InputPath)"
PostBuild_Desc=Install
PostBuild_Cmds=echo on	..\CopyStaticLib.bat build $(TargetDir) $(TargetName)	..\CopyGdal.bat build
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libopenmodeller_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../inc" /I "../../inc/env_io" /I "$(GDAL_HOME)/include" /I "../../lib/serialization" /I "$(EXPAT_HOME)/Source/lib" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D VERSION=\"0.2\" /D PLUGINPATH=$(OM_PLUGIN_PATH) /D CONFIG_FILE=\"om_config.txt\" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
TargetDir=.\Debug
TargetName=libopenmodeller_static
SOURCE="$(InputPath)"
PostBuild_Desc=Install (debug)
PostBuild_Cmds=echo on	..\CopyStaticLib.bat build_debug $(TargetDir) $(TargetName)	..\CopyGdal.bat build_debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libopenmodeller_static - Win32 Release"
# Name "libopenmodeller_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
