# Microsoft Developer Studio Project File - Name="om_garp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=om_garp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "om_garp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "om_garp.mak" CFG="om_garp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "om_garp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "om_garp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "om_garp - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OM_GARP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../algorithms/garp" /I "../../inc" /I "../../inc/serialization" /I "$(GDAL_HOME)/include" /I "$(JAVA_HOME)/include" /I "$(JAVA_HOME)/bin" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "OM_GARP_EXPORTS" /D "CORE_DLL_IMPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 gdal_i.lib libexpat.lib libopenmodeller.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /nodefaultlib:"scew_.lib" /libpath:"$(GDAL_HOME)\lib" /libpath:"../build/lib" /libpath:"$(EXPAT_HOME)/libs"
# Begin Special Build Tool
TargetDir=.\Release
TargetName=om_garp
SOURCE="$(InputPath)"
PostBuild_Desc=Install
PostBuild_Cmds=echo on	..\CopyDLL.bat build $(TargetDir) $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "om_garp - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OM_GARP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../algorithms/garp" /I "../../inc" /I "../../inc/serialization" /I "$(GDAL_HOME)/include" /D "_WINDOWS" /D "_USRDLL" /D "OM_GARP_EXPORTS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D VERSION=\"0.2\" /D PLUGINPATH=$(OM_PLUGIN_PATH) /D "CORE_DLL_IMPORT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdal_i.lib libexpat.lib libopenmodeller.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(GDAL_HOME)\lib" /libpath:"../build_debug/lib" /libpath:"$(EXPAT_HOME)/libs"
# Begin Special Build Tool
TargetDir=.\Debug
TargetName=om_garp
SOURCE="$(InputPath)"
PostBuild_Desc=Install (debug)
PostBuild_Cmds=echo on	..\CopyDll.bat build_debug $(TargetDir) $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "om_garp - Win32 Release"
# Name "om_garp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\algorithms\garp\garp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\garp_sampler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_atomic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_logit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_negrange.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_range.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\ruleset.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\algorithms\garp\garp.hh
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\garp_sampler.hh
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_atomic.hh
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_base.hh
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_logit.hh
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_negrange.hh
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\rules_range.hh
# End Source File
# Begin Source File

SOURCE=..\..\..\algorithms\garp\ruleset.hh
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
