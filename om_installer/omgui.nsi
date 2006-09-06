; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "openModeller"
!define PRODUCT_VERSION "0.3"
!define PRODUCT_PUBLISHER "openModeller Development Team"
!define PRODUCT_WEB_SITE "http://openmodeller.sourceforge.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\omgui.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"


; Side Image for installer -------------
!macro BIMAGE IMAGE PARMS
	Push $0
	GetTempFileName $0
	File /oname=$0 "${IMAGE}"
	SetBrandingImage ${PARMS} $0
	Delete $0
	Pop $0
!macroend

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "om_win322.ico"
!define MUI_UNICON "om_win322_uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\omgui.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "openModeller${PRODUCT_VERSION}_standalone_setup.exe"
InstallDir "$PROGRAMFILES\openModeller"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  CreateDirectory "$SMPROGRAMS\openModeller"
  CreateShortCut "$SMPROGRAMS\openModeller\openModeller.lnk" "$INSTDIR\omgui.exe"
  CreateShortCut "$DESKTOP\openModeller.lnk" "$INSTDIR\omgui.exe"
  
  File "om_logo.bmp"
  File "..\ecw_cs.dat"
  File "..\gcs.csv"
  File "..\gdal12.dll"
  File "..\gsl.dll"
  File "..\gslcblas.dll"
  File "..\libexif-9.dll"
  File "..\libexpat.dll"
  File "..\libopenmodeller.dll"
  File "..\om_config.txt"
  File "..\om_console.exe"
  File "..\omgui.exe"
  File "..\pcs.csv"
  File "..\prime_meridian.csv" 
  File "..\proj.dll"  
  File "..\projop_wparm.csv"  
  File "..\qt-mtnc321.dll" ;QT Non Commercial lib  
  File "..\s57attributes.csv"  
  File "..\s57expectedinput.csv"  
  File "..\s57objectclasses.csv"  
  File "..\stateplane.csv"  
  File "..\unit_of_measure.csv"  
  File "..\wkt_defs.txt"  
  File "..\msvcr80.dll" 
  File "..\msvcp80.dll" 
  File "..\msvcp60.dll" ; <-- needed for qt hopefully this req will go away after i rebuild qt with vc8 

  ;
  ; Now the algs
  ;
  SetOutPath $INSTDIR\algs
  SetOverwrite try
  File "..\algs\om_bioclim.dll"
  File "..\algs\om_csmbs.dll"
  File "..\algs\om_distance_to_average.dll"
  File "..\algs\om_dg_garp.dll"
  File "..\algs\om_dg_garp_bs.dll"
  File "..\algs\om_garp.dll"
  File "..\algs\om_garp_best_subsets.dll"
  File "..\algs\om_mindist.dll"
  
  ;
  ; Now the sample data
  ;
  CreateDirectory  $INSTDIR\sample_data
  SetOutPath $INSTDIR\sample_data
  SetOverwrite try
  File "..\om_sample_data\furcata_boliviana.txt"

  CreateDirectory $INSTDIR\sample_data\rain_coolest
  SetOutPath $INSTDIR\sample_data\rain_coolest
  SetOverWrite try
  File "..\om_sample_data\rain_coolest\dblbnd.adf"
  File "..\om_sample_data\rain_coolest\hdr.adf"
  File "..\om_sample_data\rain_coolest\prj.adf"
  File "..\om_sample_data\rain_coolest\sta.adf"
  File "..\om_sample_data\rain_coolest\w001001.adf"
  File "..\om_sample_data\rain_coolest\w001001x.adf"

  CreateDirectory $INSTDIR\sample_data\rain_hottest
  SetOutPath $INSTDIR\sample_data\rain_hottest
  SetOverWrite try
  File "..\om_sample_data\rain_hottest\dblbnd.adf"
  File "..\om_sample_data\rain_hottest\hdr.adf"
  File "..\om_sample_data\rain_hottest\prj.adf"
  File "..\om_sample_data\rain_hottest\sta.adf"
  File "..\om_sample_data\rain_hottest\w001001.adf"
  File "..\om_sample_data\rain_hottest\w001001x.adf"

  CreateDirectory $INSTDIR\sample_data\rain_tot
  SetOutPath $INSTDIR\sample_data\rain_tot
  SetOverWrite try
  File "..\om_sample_data\rain_tot\dblbnd.adf"
  File "..\om_sample_data\rain_tot\hdr.adf"
  File "..\om_sample_data\rain_tot\prj.adf"
  File "..\om_sample_data\rain_tot\sta.adf"
  File "..\om_sample_data\rain_tot\w001001.adf"
  File "..\om_sample_data\rain_tot\w001001x.adf"

  CreateDirectory $INSTDIR\sample_data\temp_avg
  SetOutPath $INSTDIR\sample_data\temp_avg
  SetOverWrite try
  File "..\om_sample_data\temp_avg\dblbnd.adf"
  File "..\om_sample_data\temp_avg\hdr.adf"
  File "..\om_sample_data\temp_avg\prj.adf"
  File "..\om_sample_data\temp_avg\sta.adf"
  File "..\om_sample_data\temp_avg\w001001.adf"
  File "..\om_sample_data\temp_avg\w001001x.adf"

  CreateDirectory $INSTDIR\sample_data\temp_coolest
  SetOutPath $INSTDIR\sample_data\temp_coolest
  SetOverWrite try
  File "..\om_sample_data\temp_coolest\dblbnd.adf"
  File "..\om_sample_data\temp_coolest\hdr.adf"
  File "..\om_sample_data\temp_coolest\prj.adf"
  File "..\om_sample_data\temp_coolest\sta.adf"
  File "..\om_sample_data\temp_coolest\w001001.adf"
  File "..\om_sample_data\temp_coolest\w001001x.adf"

  CreateDirectory $INSTDIR\sample_data\temp_dryest
  SetOutPath $INSTDIR\sample_data\temp_dryest
  SetOverWrite try
  File "..\om_sample_data\temp_dryest\dblbnd.adf"
  File "..\om_sample_data\temp_dryest\hdr.adf"
  File "..\om_sample_data\temp_dryest\prj.adf"
  File "..\om_sample_data\temp_dryest\sta.adf"
  File "..\om_sample_data\temp_dryest\w001001.adf"
  File "..\om_sample_data\temp_dryest\w001001x.adf"

  CreateDirectory $INSTDIR\sample_data\temp_hotest
  SetOutPath $INSTDIR\sample_data\temp_hotest
  SetOverWrite try
  File "..\om_sample_data\temp_hotest\dblbnd.adf"
  File "..\om_sample_data\temp_hotest\hdr.adf"
  File "..\om_sample_data\temp_hotest\prj.adf"
  File "..\om_sample_data\temp_hotest\sta.adf"
  File "..\om_sample_data\temp_hotest\w001001.adf"
  File "..\om_sample_data\temp_hotest\w001001x.adf"

  CreateDirectory $INSTDIR\sample_data\temp_wettest
  SetOutPath $INSTDIR\sample_data\temp_wettest
  SetOverWrite try
  File "..\om_sample_data\temp_wettest\dblbnd.adf"
  File "..\om_sample_data\temp_wettest\hdr.adf"
  File "..\om_sample_data\temp_wettest\prj.adf"
  File "..\om_sample_data\temp_wettest\sta.adf"
  File "..\om_sample_data\temp_wettest\w001001.adf"
  File "..\om_sample_data\temp_wettest\w001001x.adf"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\openModeller\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\openModeller\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\qgis.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\qgis.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  ;PROGRAM FILES
  Delete /REBOOTOK "$INSTDIR\${PRODUCT_NAME}.url"
  Delete /REBOOTOK "$INSTDIR\uninst.exe"
  Delete /REBOOTOK "$INSTDIR\om_logo.bmp"
  Delete /REBOOTOK "$INSTDIR\ecw_cs.dat"
  Delete /REBOOTOK "$INSTDIR\gcs.csv"
  Delete /REBOOTOK "$INSTDIR\gdal12.dll"
  Delete /REBOOTOK "$INSTDIR\gsl.dll"
  Delete /REBOOTOK "$INSTDIR\gslcblas.dll"
  Delete /REBOOTOK "$INSTDIR\libexif-9.dll"
  Delete /REBOOTOK "$INSTDIR\libexpat.dll"
  Delete /REBOOTOK "$INSTDIR\libexpatMT.lib"
  Delete /REBOOTOK "$INSTDIR\libopenmodeller.dll"
  Delete /REBOOTOK "$INSTDIR\om_config.txt"
  Delete /REBOOTOK "$INSTDIR\om_console.exe"
  Delete /REBOOTOK "$INSTDIR\omgui.exe"
  Delete /REBOOTOK "$INSTDIR\pcs.csv"
  Delete /REBOOTOK "$INSTDIR\prime_meridian.csv" 
  Delete /REBOOTOK "$INSTDIR\proj.dll"  
  Delete /REBOOTOK "$INSTDIR\projop_wparm.csv"  
  Delete /REBOOTOK "$INSTDIR\qt-mtnc321.dll" ;QT Non Commercial lib  
  Delete /REBOOTOK "$INSTDIR\s57attributes.csv"  
  Delete /REBOOTOK "$INSTDIR\s57expectedinput.csv"  
  Delete /REBOOTOK "$INSTDIR\s57objectclasses.csv"  
  Delete /REBOOTOK "$INSTDIR\stateplane.csv"  
  Delete /REBOOTOK "$INSTDIR\unit_of_measure.csv"  
  Delete /REBOOTOK "$INSTDIR\wkt_defs.txt"  
  Delete /REBOOTOK "$INSTDIR\msvcr80.dll" 
  Delete /REBOOTOK "$INSTDIR\msvcp80.dll" 
  Delete /REBOOTOK "$INSTDIR\msvcp60.dll"

  ;ALGORITHMS
  Delete /REBOOTOK "$INSTDIR\algs\om_bioclim.dll"
  Delete /REBOOTOK "$INSTDIR\algs\om_csmbs.dll"
  Delete /REBOOTOK "$INSTDIR\algs\om_csmkg.dll"
  Delete /REBOOTOK "$INSTDIR\algs\om_distance_to_average.dll"
  Delete /REBOOTOK "$INSTDIR\algs\om_garp.dll"
  Delete /REBOOTOK "$INSTDIR\algs\om_garp_best_subsets.dll"
  Delete /REBOOTOK "$INSTDIR\algs\om_mindist.dll"

  ;SAMPLE DATA
  Delete /REBOOTOK "$INSTDIR\sample_data\furcata_boliviana.txt"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_coolest\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_coolest\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_coolest\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_coolest\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_coolest\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_coolest\w001001x.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_hottest\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_hottest\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_hottest\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_hottest\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_hottest\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_hottest\w001001x.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_tot\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_tot\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_tot\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_tot\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_tot\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\rain_tot\w001001x.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_avg\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_avg\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_avg\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_avg\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_avg\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_avg\w001001x.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_coolest\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_coolest\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_coolest\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_coolest\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_coolest\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_coolest\w001001x.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_dryest\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_dryest\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_dryest\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_dryest\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_dryest\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_dryest\w001001x.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_hotest\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_hotest\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_hotest\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_hotest\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_hotest\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_hotest\w001001x.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_wettest\dblbnd.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_wettest\hdr.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_wettest\prj.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_wettest\sta.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_wettest\w001001.adf"
  Delete /REBOOTOK "$INSTDIR\sample_data\temp_wettest\w001001x.adf"

  ;SHORTCUT FILES
  Delete /REBOOTOK "$SMPROGRAMS\openModeller\Uninstall.lnk"
  Delete /REBOOTOK "$SMPROGRAMS\openModeller\openModeller.lnk"
  Delete /REBOOTOK "$SMPROGRAMS\openModeller\Website.lnk"
  Delete /REBOOTOK "$DESKTOP\openModeller.lnk"

  ;REMOVE DIRECTORIES
  RMDir /REBOOTOK "$SMPROGRAMS\openModeller"
  RMDir /REBOOTOK $INSTDIR\algs    
  RMDir /REBOOTOK $INSTDIR          
  RMDir /REBOOTOK $INSTDIR\sample_data\temp_wettest
  RMDir /REBOOTOK $INSTDIR\sample_data\temp_hotest
  RMDir /REBOOTOK $INSTDIR\sample_data\temp_dryest
  RMDir /REBOOTOK $INSTDIR\sample_data\temp_coolest
  RMDir /REBOOTOK $INSTDIR\sample_data\temp_avg
  RMDir /REBOOTOK $INSTDIR\sample_data\rain_tot
  RMDir /REBOOTOK $INSTDIR\sample_data\rain_hottest
  RMDir /REBOOTOK $INSTDIR\sample_data\rain_coolest
  RMDir /REBOOTOK $INSTDIR\sample_data

  ;DELETE REGISTRY ENTRIES
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd


