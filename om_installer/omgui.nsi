

!macro BIMAGE IMAGE PARMS
	Push $0
	GetTempFileName $0
	File /oname=$0 "${IMAGE}"
	SetBrandingImage ${PARMS} $0
	Delete $0
	Pop $0
!macroend


;--------------------------------

; The name of the installer
Name "openModeller Plugin"

; The file to write
OutFile "omgui_setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\qgis

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\openModeller_Plugin" "Install_Dir"

;--------------------------------

; Adds an XP manifest to the installer
XPStyle on

; Add branding image to the installer (an image placeholder on the side).
; It is not enough to just add the placeholder, we must set the image too...
; We will later set the image in every pre-page function.
; We can also set just one persistent image in .onGUIInit
AddBrandingImage left 100

; Sets the font of the installer
SetFont "Comic Sans MS" 8

; Just to make it three pages...
SubCaption 0 ": openModeller License Agreement"
SubCaption 2 ": Subcaption 1"
LicenseText "License page"
LicenseData "LICENSE.txt"

; This is the text that appears on the directory selection page
DirText "Choose the directory where you wish to install omgui. Note if you already have qgis installed, you should select the SAME directory e.g. c:\program files\qgis"


; Pages

Page license licenseImage
Page custom customPage
Page directory dirImage
Page instfiles instImage

UninstPage uninstConfirm
UninstPage instfiles



Section ""
	; You can also use the BI_NEXT macro here...
	WriteUninstaller uninst.exe
SectionEnd

;--------------------------------

Function licenseImage
	!insertmacro BIMAGE "om_logo.bmp" /RESIZETOFIT
FunctionEnd

Function dirImage
	!insertmacro BIMAGE "om_logo.bmp" /RESIZETOFIT
FunctionEnd

Function instImage
	!insertmacro BIMAGE "om_logo.bmp" /RESIZETOFIT
FunctionEnd

;--------------------------------
Function customPage
	!insertmacro BIMAGE "om_logo.bmp" /RESIZETOFIT
	#insert install options/start menu/<insert plugin name here> here
FunctionEnd

; The stuff to install
Section "openModeller (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there

  File "om_logo.bmp"
  File "..\ecw_cs.dat"
  File "..\gcs.csv"
  File "..\gdal12.dll"
  File "..\gsl.dll"
  File "..\gslcblas.dll"
  File "..\libexif-9.dll"
  File "..\libexpat.dll"
  File "..\libexpatMT.lib"
  File "..\libopenmodeller.dll"
  File "..\libopenmodeller.exp"
  File "..\libopenmodeller.lib"
  File "..\om_config.txt"
  File "..\om_console.exe"
  File "..\omgui.exe"
  File "..\omgui.exp" ;prolly not needed
  File "..\omgui.ilk" ;prolly not needed
  File "..\omgui.pdb" ;prolly not needed
  File "..\pcs.csv"
  File "..\prime_meridian.csv"  
  File "..\proj4.dll"  
  File "..\proj.dll" ; I am sure both are not needed  
  File "..\projop_wparm.csv"  
  File "..\qt-mtnc321.dll" ;QT Non Commercial lib  
  File "..\s57attributes.csv"  
  File "..\s57expectedinput.csv"  
  File "..\s57objectclasses.csv"  
  File "..\unit_of_measure.csv"  
  File "..\wkt_defs.txt"  
 

  ;
  ; Now the algs
  ;
  SetOutPath $INSTDIR\algs
  File "..\algs\*.dll" ;once again I am sure only the dll's need copying  
  ;File "..\"  
  ;File "..\"  
  ;File "..\" 


  ;you could also say just
  ;File "..\"  
  ;and that would include all files in the above directory
  ;but I want to be sure to strip out just the required files
  
   
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\openModeller_Plugin "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openModeller_Plugin" "DisplayName" "openModeller Plugin"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openModeller_Plugin" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openModeller_Plugin" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openModeller_Plugin" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\openModeller Gui"
  CreateShortCut "$SMPROGRAMS\openModeller Gui\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\openModeller Gui\openModeller GUI.lnk" "$INSTDIR\omgui.exe" "" "$INSTDIR\omgui.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\openModeller_Plugin"
  DeleteRegKey HKLM SOFTWARE\NSIS_Example2

  ; Remove files and uninstaller
  Delete $INSTDIR\omgui.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\openModeller Gui\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\openModeller Gui"
  RMDir "$INSTDIR"

SectionEnd