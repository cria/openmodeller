!include MUI2.nsh
!include LogicLib.nsh
!include nsDialogs.nsh
!include WinMessages.nsh
!include FileFunc.nsh
!include ZipDLL.nsh
!include RecursiveDelete.nsh
!include EnvVarUpdate.nsh
;!include WriteEnvStr.nsh

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "openModeller"
!define PRODUCT_PUBLISHER "openModeller.sf.net"
!define PRODUCT_WEB_SITE "http://openModeller.sf.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\openModeller.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
!define PRODUCT_VERSION "1.3.0"

; This is where cmake builds and installs to - no space separating name and version
!define BUILD_DIR "C:\Arquivos de Programas\${PRODUCT_NAME} ${PRODUCT_VERSION}"
;
; This is where the nsis installer will install to. Having the space lets you
; keep dev and inst versions side by side on the same machine
!define INSTALL_DIR "$PROGRAMFILES\${PRODUCT_NAME} ${PRODUCT_VERSION}"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
InstallDir "${INSTALL_DIR}"
OutFile "openModellerSetup${PRODUCT_VERSION}.exe"
SetCompressor zlib

; Try to declare all variables here
Var DIALOG

Var IDIR 
Var ILABEL
Var IDIRBOX
Var IDIRREQUEST
Var IDIRBROWSE

Var WLABEL
Var WDIRBOX
Var WDIRREQUEST
Var WDIRBROWSE
Var WDIR


; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "openmodeller64x64.ico"
!define MUI_UNICON "openmodeller64x64.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "om_logo.bmp"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "..\COPYING.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

Var WORKINGDIR
; Directory page
Page custom myCustomDirectory myCustomDirectoryLeave

; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME} "
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
;!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "PortugueseBR" 

; Initialize language
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd
; MUI end

ShowInstDetails show
ShowUnInstDetails show

Section "Application" SEC01
  ;this section is mandatory
  SectionIn RO
  ;install for all users not just the logged in user
  ;make sure this is at the top of the section
  SetShellVarContext all
  
  SetOutPath "$INSTDIR"

  FileOpen $4 "$INSTDIR\om.cfg" w
  FileWrite $4 "$INSTDIR\algs\"
  FileClose $4

  SetOverwrite ifnewer
  File "${BUILD_DIR}\*.exe"
  File "${BUILD_DIR}\*.bat"
  File "${BUILD_DIR}\*.html"
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
; if its not there the application working dir will be the last used
; outpath and libom wont be able to find its algorithm
  SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\openModeller README.lnk" "$INSTDIR\README.html"
  SetOutPath "$WORKINGDIR"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_algorithm.lnk" "$INSTDIR\om_cmd" "$WDIR om_algorithm"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_console.lnk" "$INSTDIR\om_cmd" "$WDIR om_console" 
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_model.lnk" "$INSTDIR\om_cmd" "$WDIR om_model"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_points.lnk" "$INSTDIR\om_cmd" "$WDIR om_points"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_project.lnk" "$INSTDIR\om_cmd" "$WDIR om_project"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_pseudo.lnk" "$INSTDIR\om_cmd" "$WDIR om_pseudo"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_sampler.lnk" "$INSTDIR\om_cmd" "$WDIR om_sampler"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\om_test.lnk" "$INSTDIR\om_cmd" "$WDIR om_test"
  SetOutPath "$INSTDIR"
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
  SetOutPath "$WDIR\examples"
  File "${BUILD_DIR}\examples\*"
  SetOutPath "$INSTDIR\examples"
  File "${BUILD_DIR}\examples\*"
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - Aquamaps" SEC06
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/download/marine2.zip marine2.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\marine2.zip" "$INSTDIR\SampleData\EnvironmentLayers\" "<ALL>"
SectionEnd

;Section -AdditionalIcons
;  SetOutPath $INSTDIR
;  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
;  ;install for all users not just the logged in user
;  SetShellVarContext all
;  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
;  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
;  ; its more consistant to let user remove the app from add/remove progs in control panel
;  ;CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst-release.exe"
;  !insertmacro MUI_STARTMENU_WRITE_END
;SectionEnd

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

LangString sec01 ${LANG_ENGLISH} "Main application files (obligatory)." 
LangString sec01 ${LANG_PORTUGUESEBR} "Arquivos principais (obrigatório)."

LangString sec02 ${LANG_ENGLISH} "Global coverage derived from CRU CL2 present day scenario (~41MB will be downloaded from the Internet)."
LangString sec02 ${LANG_PORTUGUESEBR} "CRU CL2 cenário atual - dados climáticos globais (41MB terão que ser baixados da Internet)."

LangString sec03 ${LANG_ENGLISH} "Global coverage derived from Hadley 2050 A1f scenario (~4MB will be downloaded from the Internet)."
LangString sec03 ${LANG_PORTUGUESEBR} "Hadley 2050 cenário A1f - dados climáticos globais (~4MB terão que ser baixados da Internet)."

LangString sec04 ${LANG_ENGLISH} "South America climate data from CIAT (~1MB will be downloaded from the Internet)."
LangString sec04 ${LANG_PORTUGUESEBR} "CIAT - dados climáticos da América do Sul (~1MB terá que ser baixado da Internet)."

LangString sec05 ${LANG_ENGLISH} "Basic examples including a couple of environmental layers and sample request files."
LangString sec05 ${LANG_PORTUGUESEBR} "Arquivos de exemplo (inclui duas camadas ambientais e exemplos de requisição)."

LangString sec06 ${LANG_ENGLISH} "AquaMaps marine layers (~1.4MB will be downloaded from the Internet)."
LangString sec06 ${LANG_PORTUGUESEBR} "AquaMaps - camadas ambientais marinhas (~1.4MB terão que ser baixados da Internet)."
; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(sec01)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} $(sec02) 
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} $(sec03) 
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} $(sec04) 
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC05} $(sec05) 
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC06} $(sec06)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

LangString successfully_removed ${LANG_ENGLISH} "$(^Name) was successfully removed from your computer."
LangString successfully_removed ${LANG_PORTUGUESEBR} "$(^Name) foi removido com sucesso de seu computador."
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK $(successfully_removed) 
FunctionEnd

LangString uninstall_message ${LANG_ENGLISH} "Are you sure you want to completely remove $(^Name) and all of its components?"
LangString uninstall_message ${LANG_PORTUGUESEBR} "Tem certeza que deseja remover $(^Name) e todos os seus componentes?"
Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2  $(uninstall_message) IDYES +2
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
  ;I added this recursive delete implementation because
  ; RM -R wasnt working properly
  Push "$INSTDIR"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\"
  RMDir "$INSTDIR"

;----------------- icons and shortcuts
  ;Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  ;Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  ;uninstall for all users not just the logged in user
  SetShellVarContext all
  ;Delete "$DESKTOP\openModeller .lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\openModeller README.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_algorithm.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_console.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_model.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_points.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_project.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_pseudo.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_sampler.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\om_test.lnk"
  RMDir "$SMPROGRAMS\$ICONS_GROUP"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

LangString header_text ${LANG_ENGLISH} "Choose installation and working directories"
LangString header_text ${LANG_PORTUGUESEBR} "Escolha de diretórios de instalação e trabalho"

LangString header_subtext ${LANG_ENGLISH} "Choose a folder in which to install the ${PRODUCT_NAME} ${PRODUCT_VERSION} software and a different folder where you intend to run and use it."
LangString header_subtext ${LANG_PORTUGUESEBR} "Escolha uma pasta para instalar o programa ${PRODUCT_NAME} ${PRODUCT_VERSION} e outra onde pretende usar e trabalhar com o programa."

LangString ilabel ${LANG_ENGLISH} "Setup will install ${PRODUCT_NAME} ${PRODUCT_VERSION} in the following folder. To install in a different folder, click Browse and select another folder. Click Next to continue."
LangString ilabel ${LANG_PORTUGUESEBR} "Setup vai instalar ${PRODUCT_NAME} ${PRODUCT_VERSION} no seguinte diretório. Para instalar em um diretório diferente, clique Selecionar e selecione outro diretório. Clique Próximo para continuar."

LangString idirbox ${LANG_ENGLISH} "Installation Folder"
LangString idirbox ${LANG_PORTUGUESEBR} "Diretório de Instalação"

LangString dirbrowse ${LANG_ENGLISH} "Browse"
LangString dirbrowse ${LANG_PORTUGUESEBR} "Selecionar"

LangString wlabel ${LANG_ENGLISH} "The Working Folder is where Setup will install examples files and where you should run ${PRODUCT_NAME} from."
LangString wlabel ${LANG_PORTUGUESEBR} "O Diretório de Trabalho é onde o Setup instalará os arquivos de exemplo e onde você deve rodar o ${PRODUCT_NAME}."

LangString wdirbox ${LANG_ENGLISH} "Working Folder"
LangString wdirbox ${LANG_PORTUGUESEBR} "Diretório de Trabalho"

Function myCustomDirectory

        !insertmacro MUI_HEADER_TEXT $(header_text) $(header_subtext)

        nsDialogs::Create 1018
        Pop $DIALOG

        ${If} $DIALOG == error
                Abort
        ${EndIf}

        ${NSD_CreateLabel} 0 0 100% 20% $(ilabel)
        Pop $ILABEL

        ${NSD_CreateGroupBox} 0 20% 100% 25% $(idirbox)
        Pop $IDIRBOX

        ${NSD_CreateText} 5% 30% 70% 10% "$INSTDIR"
        Pop $IDIRREQUEST

        ${NSD_CreateBrowseButton} 75% 30% 20% 10% $(dirbrowse)
        Pop $IDIRBROWSE

        ${NSD_OnClick} $IDIRBROWSE iDirBrowse

        ${NSD_CreateLabel} 0 50% 100% 20% $(wlabel)
        Pop $WLABEL

        ${NSD_CreateGroupBox} 0 70% 100% 25% $(wdirbox)
        Pop $WDIRBOX

        ${NSD_CreateText} 5% 80% 70% 10% "c:\om"
        Pop $WDIRREQUEST

        ${NSD_CreateBrowseButton} 75% 80% 20% 10% $(dirbrowse)
        Pop $WDIRBROWSE

        ${NSD_OnClick} $WDIRBROWSE wDirBrowse

        nsDialogs::Show
FunctionEnd

LangString empty ${LANG_ENGLISH} "Working Folder empty." 
LangString empty ${LANG_PORTUGUESEBR} "Diretório de Trabalho vazio." 

LangString alreadyexists ${LANG_ENGLISH} "Working Folder already exists. Please choose another one." 
LangString alreadyexists ${LANG_PORTUGUESEBR} "Diretório de Trabalho já existe. Por favor selecione outro diretório." 
Function myCustomDirectoryLeave
        ${NSD_GetText} $WDIRREQUEST $WDIR
        
        ${If} $WDIR == ''
            MessageBox MB_OK $(empty)
            Abort
        ${EndIf}

        ${If} ${FileExists} $WDIR
            MessageBox MB_OK $(alreadyexists)
            Abort
        ${EndIf}
FunctionEnd

Function iDirBrowse
        nsDialogs::SelectFolderDialog "" "c:\"
        pop $IDIR

        ${If} $IDIR == error
                Abort
        ${EndIf}

        ${NSD_SetText} $IDIRREQUEST $IDIR
        StrCpy $INSTDIR $IDIR
FunctionEnd

Function wDirBrowse
        nsDialogs::SelectFolderDialog "" "c:\"
        Pop $WDIR

        ${If} $WDIR == error
                Abort
        ${EndIf}

        ${NSD_SetText} $WDIRREQUEST $WDIR
        StrCpy $WORKINGDIR $WDIR
FunctionEnd
