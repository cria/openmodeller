
; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "openModeller"
!define PRODUCT_PUBLISHER "openModeller.sf.net"
!define PRODUCT_WEB_SITE "http://openModeller.sf.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\openModeller.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; Defines added by Tim to streamline / softcode install process
!define PRODUCT_VERSION "1.1.0"
; This is where cmake builds and installs to - no space separating name and version
!define BUILD_DIR "C:\Program files\${PRODUCT_NAME}${PRODUCT_VERSION}"
; This is where the nsis installer will install to. Having the space lets you
; keep dev and inst versions side by side on the same machine
!define INSTALL_DIR "$PROGRAMFILES\${PRODUCT_NAME} ${PRODUCT_VERSION}"

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
!include EnvVarUpdate.nsh
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
 
  File "${BUILD_DIR}\*.txt" 
  File "${BUILD_DIR}\*.dll"

  SetOutPath "$INSTDIR\include\openmodeller"
  File "${BUILD_DIR}\include\openmodeller\*"
  SetOutPath "$INSTDIR\include\openmodeller\env_io\"
  File "${BUILD_DIR}\include\openmodeller\env_io\*"
  SetOutPath "$INSTDIR\include\openmodeller\occ_io\"
  File "${BUILD_DIR}\include\openmodeller\occ_io\*"
  SetOutPath "$INSTDIR\include\openmodeller\pre\"
  File "${BUILD_DIR}\include\openmodeller\pre\*"

; gdal related
  SetOutPath "$INSTDIR\gdal"
  File "${BUILD_DIR}\gdal\*.wkt"
  File "${BUILD_DIR}\gdal\*.svg"
  File "${BUILD_DIR}\gdal\*.csv"
  File "${BUILD_DIR}\gdal\*.dgn"

; proj4 related
  SetOutPath "$INSTDIR\nad"
  File "${BUILD_DIR}\nad\epsg"
  File "${BUILD_DIR}\nad\esri"
  File "${BUILD_DIR}\nad\esri.extra"
  File "${BUILD_DIR}\nad\GL27"
  File "${BUILD_DIR}\nad\IGNF"
  File "${BUILD_DIR}\nad\nad.lst"
  File "${BUILD_DIR}\nad\nad27"
  File "${BUILD_DIR}\nad\nad83"
  File "${BUILD_DIR}\nad\other.extra"
  File "${BUILD_DIR}\nad\proj_def.dat"
  File "${BUILD_DIR}\nad\world"

  SetOutPath "$INSTDIR\algs"
  File "${BUILD_DIR}\algs\*.dll"
  SetOutPath "$INSTDIR\data"
  File "${BUILD_DIR}\data\aquamaps.db"

; Shortcuts
; Next line is important - added by Tim
; if its not there the application working dir will be the last used
; outpath and libom wont be able to find its alg
  SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\openModeller .lnk" "$INSTDIR\README.txt"
  !insertmacro MUI_STARTMENU_WRITE_END
; Add path
  Push "Path"
  Push "A"
  Push "HKLM"
  Push "$INSTDIR"
  Call EnvVarUpdate 

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

Section "Small examples data" SEC05
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
  Push "Path"
  Push "R"
  Push "HKLM"
  Push "$INSTDIR"
  Call un.EnvVarUpdate

  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst-release.exe"
  Delete "$INSTDIR\*.dll"

;---------- Sample data
  Delete "$INSTDIR\examples\*.*"

;---------- GDAL data
  Delete "$INSTDIR\gdal\*.wkt"
  Delete "$INSTDIR\gdal\*.svg"
  Delete "$INSTDIR\gdal\*.csv"
  Delete "$INSTDIR\gdal\*.dgn"

;---------- proj4 data
  Delete "$INSTDIR\nad\epsg"
  Delete "$INSTDIR\nad\esri"
  Delete "$INSTDIR\nad\esri.extra"
  Delete "$INSTDIR\nad\GL27"
  Delete "$INSTDIR\nad\IGNF"
  Delete "$INSTDIR\nad\nad.lst"
  Delete "$INSTDIR\nad\nad27"
  Delete "$INSTDIR\nad\nad83"
  Delete "$INSTDIR\nad\other.extra"
  Delete "$INSTDIR\nad\proj_def.dat"
  Delete "$INSTDIR\nad\world"

;---------- openModeller stuff
  Delete "$INSTDIR\om.cfg"
  Delete "$INSTDIR\libopenmodeller.a"
  Delete "$INSTDIR\algs\*.dll"
  Delete "$INSTDIR\data\aquamaps.db"
  Delete "$INSTDIR\*.exe"

  RMDir /r "$INSTDIR\examples"
  RMDir /r "$INSTDIR\algs"
  RMDir /r "$INSTDIR\gdal"
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
