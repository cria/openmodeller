
; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "openModeller"
!define PRODUCT_PUBLISHER "openModeller.sf.net"
!define PRODUCT_WEB_SITE "http://openModeller.sf.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\openModeller.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; Defines added by Tim to streamline / softcode install process
!define PRODUCT_VERSION "0.5.3"
; This is where cmake builds and installs to - no space separating name and version
!define BUILD_DIR "c:\Program Files\${PRODUCT_NAME}${PRODUCT_VERSION}"
; This is where the nsis installer will install to. Having the space lets you
; keep dev and inst versions side by side on teh same machine
!define INSTALL_DIR "c:\Program Files\${PRODUCT_NAME} ${PRODUCT_VERSION}"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
InstallDir "${INSTALL_DIR}"
OutFile "openModellerSetup${PRODUCT_VERSION}.exe"
# If this next lineis uncommented the installer will try to install to
# the same dir as any prefvious install of omdesktop
# With it commented it will try to used INSTALL_DIR as defined above
#InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

SetCompressor zlib
; Added by Tim for setting env vars (see this file on disk)
!include WriteEnvStr.nsh
; MUI 1.67 compatible ------
!include "MUI.nsh"
;Added by Tim for a macro that will recursively delete the files in the install dir
!include RecursiveDelete.nsh
;Added by Tim to support unzipping downloaded sample data automatically
!include ZipDLL.nsh

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "openmodeller64x64.ico"
!define MUI_UNICON "openmodeller64x64.ico"
; Added by Tim for side image
!define MUI_WELCOMEFINISHPAGE_BITMAP "om_logo.bmp"
; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "..\COPYING.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "openModeller "
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
;!define MUI_FINISHPAGE_RUN "$INSTDIR\om_console.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "PortugueseBR" 
;!insertmacro MUI_LANGUAGE "French" 
;!insertmacro MUI_LANGUAGE "German" 
;!insertmacro MUI_LANGUAGE "SimpChinese" 
;!insertmacro MUI_LANGUAGE "Japanese" 
;!insertmacro MUI_LANGUAGE "Italian" 
;!insertmacro MUI_LANGUAGE "Swedish" 
;!insertmacro MUI_LANGUAGE "Russian" 
;!insertmacro MUI_LANGUAGE "Portuguese" 
;!insertmacro MUI_LANGUAGE "Polish" 
;!insertmacro MUI_LANGUAGE "Czech" 
;!insertmacro MUI_LANGUAGE "Slovak" 
;!insertmacro MUI_LANGUAGE "Latvian" 
;!insertmacro MUI_LANGUAGE "Indonesian" 

; Initialize language
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd


; MUI end ------

ShowInstDetails show
ShowUnInstDetails show


Section "Application" SEC01
  ;this section is mandatory
  SectionIn RO
  ;Added by Tim to install for all users not just the logged in user..
  ;make sure this is at the top of the section
  SetShellVarContext all
  
  SetOutPath "$INSTDIR"

  FileOpen $4 "$INSTDIR\om.cfg" w
  FileWrite $4 "$INSTDIR\algs\"
  FileClose $4

  SetOverwrite ifnewer
  File "${BUILD_DIR}\*.exe"
  SetOverwrite try
  
  File "${BUILD_DIR}\*.dll"
;------- gdal related
  File "${BUILD_DIR}\cubewerx_extra.wkt"
  File "${BUILD_DIR}\ecw_cs.dat"
  File "${BUILD_DIR}\ellipsoid.csv"
  File "${BUILD_DIR}\epsg.wkt"
  File "${BUILD_DIR}\esri_extra.wkt"
  File "${BUILD_DIR}\gcs.csv"
  File "${BUILD_DIR}\gdal_datum.csv"
  File "${BUILD_DIR}\pcs.csv"
  File "${BUILD_DIR}\prime_meridian.csv"
  File "${BUILD_DIR}\projop_wparm.csv"
  File "${BUILD_DIR}\proj.dll"
  File "${BUILD_DIR}\s57agencies.csv"
  File "${BUILD_DIR}\s57attributes.csv"
  File "${BUILD_DIR}\s57attributes_aml.csv"
  File "${BUILD_DIR}\s57attributes_iw.csv"
  File "${BUILD_DIR}\s57expectedinput.csv"
  File "${BUILD_DIR}\s57objectclasses.csv"
  File "${BUILD_DIR}\s57objectclasses_aml.csv"
  File "${BUILD_DIR}\s57objectclasses_iw.csv"
  File "${BUILD_DIR}\seed_3d.dgn"
  File "${BUILD_DIR}\stateplane.csv"
  File "${BUILD_DIR}\unit_of_measure.csv"
  SetOutPath "$INSTDIR\include\openmodeller"
  File "${BUILD_DIR}\include\openmodeller\*"
  SetOutPath "$INSTDIR\include\openmodeller\env_io\"
  File "${BUILD_DIR}\include\openmodeller\env_io\*"
  SetOutPath "$INSTDIR\include\openmodeller\occ_io\"
  File "${BUILD_DIR}\include\openmodeller\occ_io\*"
  SetOutPath "$INSTDIR\nad"
  File "${BUILD_DIR}\nad\FL.lla"
  File "${BUILD_DIR}\nad\MD.lla"
  File "${BUILD_DIR}\nad\TN.lla"
  File "${BUILD_DIR}\nad\WI.lla"
  File "${BUILD_DIR}\nad\WO.lla"
  File "${BUILD_DIR}\nad\alaska.lla"
  File "${BUILD_DIR}\nad\conus.lla"
  File "${BUILD_DIR}\nad\epsg"
  File "${BUILD_DIR}\nad\esri"
  File "${BUILD_DIR}\nad\hawaii.lla"
  File "${BUILD_DIR}\nad\nad27"
  File "${BUILD_DIR}\nad\nad83"
  File "${BUILD_DIR}\nad\ntv1_can.dat"
  File "${BUILD_DIR}\nad\prvi.lla"
  File "${BUILD_DIR}\nad\stgeorge.lla"
  File "${BUILD_DIR}\nad\stlrnc.lla"
  File "${BUILD_DIR}\nad\stpaul.lla"
  File "${BUILD_DIR}\nad\world"
  SetOutPath "$INSTDIR\algs"
  File "${BUILD_DIR}\algs\*.dll"
  SetOutPath "$INSTDIR\data"
  File "${BUILD_DIR}\data\aquamaps.db"
; Shortcuts
; Next line is important - added by Tim
; if its not there the application working dir will be the last used
;outpath and libom wont be able to find its alg
  SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\openModeller .lnk" "$INSTDIR\openModeller.exe"
  ;CreateShortCut "$DESKTOP\openModeller .lnk" "$INSTDIR\om_console.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
; Added by Tim to set the PROJ_LIB env var so teh nad dir can be located by Proj
  Push PROJ_LIB
  Push "$INSTDIR\nad"
  Call WriteEnvStr

SectionEnd

; /o means unchecked by default
Section /o "Sample Data - CRU CL2 Present" SEC02 
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/installer_sample_data/crucl2.zip crucl2.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\crucl2.zip" "$INSTDIR\SampleData\EnvironmentLayers\" "<ALL>"
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - based on Hadley 2050 A1F" SEC03 
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/installer_sample_data/hadley2050A1F.zip hadley2050A1F.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\hadley2050A1F.zip" "$INSTDIR\SampleData\" "<ALL>"
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - South America" SEC04
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/installer_sample_data/SouthAmerica.zip SouthAmerica.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\SouthAmerica.zip" "$INSTDIR\SampleData\EnvironmentLayers\" "<ALL>"
SectionEnd

; /o means unchecked by default
Section /o "Small examples data" SEC05
  SetOutPath "$INSTDIR\examples"
  File "${BUILD_DIR}\examples\*"
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - Aquamaps" SEC06
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/download/marine2.zip marine2.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\marine2.zip" "$INSTDIR\SampleData\EnvironmentLayers\" "<ALL>"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  ;Added by Tim to install for all users not just the logged in user..
  SetShellVarContext all
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  ; its more consistant to let user remove the app from add/remove progs in control panel
  ;CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst-release.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst-release.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\openModeller.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst-release.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\openModeller.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Main application files - you really need this!"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Sample environment data. Global coverage derived from CRU CL2 present day scenario. About 41mb data will be downloaded from the internet."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "Sample environment data. Global coverage derived from Hadley 2050 A1f scenario. About 4mb data will be downloaded from the internet."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "Sample environment data. South America. About 1mb data will be downloaded from the internet."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC05} "Basic examples (not required if you have your own data already)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC06} "Sample environment data. Aquamaps. About 1.4mb data will be downloaded from the internet."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  # remove the variable
  Push PROJ_LIB
  Call un.DeleteEnvStr

  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst-release.exe"
  Delete "$INSTDIR\*.dll"
;------------------- Sample Data
  Delete "$INSTDIR\examples\*.*"
;---------- Gdal Requirements
  Delete "$INSTDIR\projop_wparm.csv"
  Delete "$INSTDIR\prime_meridian.csv"
  Delete "$INSTDIR\pcs.csv"
  Delete "$INSTDIR\unit_of_measure.csv"
  Delete "$INSTDIR\stateplane.csv"
  Delete "$INSTDIR\seed_3d.dgn"
  Delete "$INSTDIR\s57objectclasses_iw.csv"
  Delete "$INSTDIR\s57objectclasses_aml.csv"
  Delete "$INSTDIR\s57objectclasses.csv"
  Delete "$INSTDIR\s57expectedinput.csv"
  Delete "$INSTDIR\s57attributes_iw.csv"
  Delete "$INSTDIR\s57attributes_aml.csv"
  Delete "$INSTDIR\s57attributes.csv"
  Delete "$INSTDIR\s57agencies.csv"
  Delete "$INSTDIR\gdal_datum.csv"
  Delete "$INSTDIR\ellipsoid.csv"
  Delete "$INSTDIR\nad\FL.lla"
  Delete "$INSTDIR\nad\MD.lla"
  Delete "$INSTDIR\nad\TN.lla"
  Delete "$INSTDIR\nad\WI.lla"
  Delete "$INSTDIR\nad\WO.lla"
  Delete "$INSTDIR\nad\alaska.lla"
  Delete "$INSTDIR\nad\conus.lla"
  Delete "$INSTDIR\nad\epsg"
  Delete "$INSTDIR\nad\esri"
  Delete "$INSTDIR\nad\hawaii.lla"
  Delete "$INSTDIR\nad\nad27"
  Delete "$INSTDIR\nad\nad83"
  Delete "$INSTDIR\nad\ntv1_can.dat"
  Delete "$INSTDIR\nad\prvi.lla"
  Delete "$INSTDIR\nad\stgeorge.lla"
  Delete "$INSTDIR\nad\stlrnc.lla"
  Delete "$INSTDIR\nad\stpaul.lla"
  Delete "$INSTDIR\nad\world"
  Delete "$INSTDIR\gcs.csv"
  Delete "$INSTDIR\esri_extra.wkt"
  Delete "$INSTDIR\epsg.wkt"
  Delete "$INSTDIR\ecw_cs.dat"
  Delete "$INSTDIR\cubewerx_extra.wkt"
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\share"
;---------- openModeller Requirements
  Delete "$INSTDIR\pluginpath.cfg"
  Delete "$INSTDIR\libopenmodeller.a"
  Delete "$INSTDIR\algs\*.dll"
  ;Delete "$INSTDIR\data\aquamaps.db"
;---------------- translations

  Delete "$INSTDIR\*.qm"
;---------------- openModeller desktop related
  Delete "$INSTDIR\*.exe"

  RMDir /r "$INSTDIR\examples"
  RMDir /r "$INSTDIR\algs"
  RMDir /r "$INSTDIR\nad"
  RMDir /r "$INSTDIR"
;----------------- The application dir gets zapped next ...  
  ;I added this recursive delte implementation because
  ; RM -R wasnt working properly
  Push "$INSTDIR"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\"
  RMDir "$INSTDIR"


;----------------- icons and shortcuts
  ;Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  ;Added by Tim to uninstall for all users not just the logged in user..
  SetShellVarContext all
  Delete "$DESKTOP\openModeller .lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\openModeller .lnk"
  RMDir "$SMPROGRAMS\$ICONS_GROUP"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
