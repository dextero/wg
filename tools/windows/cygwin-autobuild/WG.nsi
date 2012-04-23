  SetCompressor /SOLID lzma
  
  !define VERSION "1.0RC3"
 
  !define MULTIUSER_EXECUTIONLEVEL Highest
  !define MULTIUSER_MUI
  !define MULTIUSER_INSTALLMODE_COMMANDLINE
  !define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY "Software\Warlock's Gauntlet\${VERSION}"
  !define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME ""
  !define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_KEY "Software\Warlock's Gauntlet\${VERSION}"
  !define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUENAME ""
  !define MULTIUSER_INSTALLMODE_INSTDIR "Warlock's Gauntlet"
  !include "MultiUser.nsh"
  !include "MUI2.nsh"
  !include "LogicLib.nsh"
 
;--------------------------------
;General
 
  ;Name and file
  Name "Warlock's Gauntlet"
  OutFile "WG-${VERSION}-win32.exe"
 
;--------------------------------
;Variables
 
  Var StartMenuFolder
  !define UserdataDir "Warlock's Gauntlet"
 
;--------------------------------
;Interface Settings
 
  !define MUI_WELCOMEFINISHPAGE_BITMAP "WG-installer-graphic.bmp"
  !define MUI_ABORTWARNING
 
;--------------------------------
;Language Selection Dialog Settings
 
  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "SHCTX" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\Warlock's Gauntlet\${VERSION}" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"
 
;--------------------------------
;Pages
 
  !insertmacro MUI_PAGE_WELCOME
  ;!insertmacro MUI_PAGE_LICENSE "README.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MULTIUSER_PAGE_INSTALLMODE
  !insertmacro MUI_PAGE_DIRECTORY
 
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "SHCTX" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Warlock's Gauntlet\${VERSION}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
 
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
 
  !insertmacro MUI_PAGE_INSTFILES
 
  !define MUI_FINISHPAGE_RUN $INSTDIR\WarlocksGauntlet.exe
  !insertmacro MUI_PAGE_FINISH
 
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
 
;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_LANGUAGE "English"  ;first language is the default language

  LangString COMPONENT_DESKTOP_SHORTCUT ${LANG_POLISH} "Skrót na pulpicie"
  LangString COMPONENT_DESKTOP_SHORTCUT ${LANG_ENGLISH} "Desktop shortcut"
 
;--------------------------------
;Reserve Files
 
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
 
  !insertmacro MUI_RESERVEFILE_LANGDLL
  ReserveFile "${NSISDIR}\Plugins\*.dll"
 
;--------------------------------
;Installer Sections
 
Section "-Warlock's Gauntlet" WMSection
  
  SetOutPath "$INSTDIR"
  File /r /x .* ".\WG\"
 
  ;Store installation folder
  WriteRegStr SHCTX "Software\Warlock's Gauntlet\${VERSION}" "" $INSTDIR
 
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; ustaw jezyk
  ; usun config.xml jesli istnieje
  Delete "$INSTDIR\data\config.xml"
  
  ; przenies odpowiedni config.xml
  ${If} $LANGUAGE == "1045"		; polski
	Rename "$INSTDIR\data\configs\pl.xml" "$INSTDIR\data\config.xml"
  ${Else}						; $LANGUAGE == "1033", angielski
	Rename "$INSTDIR\data\configs\en.xml" "$INSTDIR\data\config.xml"
  ${EndIf}
  
  ; usun folder z configami
  RMDir /r "$INSTDIR\data\configs"
 
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
 
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Warlock's Gauntlet.lnk" "$INSTDIR\WarlocksGauntlet.exe" "" "$INSTDIR\wg64.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\WGEditor.lnk" "$INSTDIR\WGEditor.exe" "" "$INSTDIR\wg64.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"    
 
  !insertmacro MUI_STARTMENU_WRITE_END
 
SectionEnd

;Section "Redisty" RedistSection
;  SetOutPath "$TEMP"
;  File "vcredist_x86.exe"
;  ExecWait "$TEMP\vcredist_x86.exe"
;SectionEnd

Section $(COMPONENT_DESKTOP_SHORTCUT) LnkSection
  SetOutPath "$INSTDIR"
  CreateShortCut "$DESKTOP\Warlock's Gauntlet.lnk" "$INSTDIR\WarlocksGauntlet.exe" "" "$INSTDIR\wg64.ico"
  
SectionEnd
 
;--------------------------------
;Installer Functions
 
Function .onInit
 
  !insertmacro MULTIUSER_INIT
  !insertmacro MUI_LANGDLL_DISPLAY
 
FunctionEnd
 
;--------------------------------
;Descriptions
 
  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC
 
  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${WMSection} "Warlock's Gauntlet - gra"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
 
;--------------------------------
;Uninstaller Section
 
Section "Uninstall"
 
  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\editor"
  RMDir /r "$INSTDIR\user"
  Delete "$INSTDIR\WGEditor.exe.manifest"
  Delete "$INSTDIR\WGEditor.exe"
  Delete "$INSTDIR\WarlocksGauntlet.exe.manifest"
  Delete "$INSTDIR\WarlocksGauntlet.exe"
  Delete "$INSTDIR\wg64.ico"
  Delete "$INSTDIR\openal32.dll"
  Delete "$INSTDIR\msvcr90.dll"
  Delete "$INSTDIR\msvcp90.dll"
  Delete "$INSTDIR\Microsoft.VC90.CRT.manifest"
  Delete "$INSTDIR\libsndfile-1.dll"
  Delete "$INSTDIR\data.vfs"
  Delete "$INSTDIR\Uninstall.exe" ; delete self - dziala, bo uninstaller jest kopiowany do tempa
  
  Delete "$DESKTOP\Warlock's Gauntlet.lnk"
 
  RMDir "$INSTDIR" ; bez /r - usunie tylko, jesli katalog jest pusty
 
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
 
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Warlock's Gauntlet.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\WGEditor.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty SHCTX "Software\Warlock's Gauntlet\${VERSION}"
  DeleteRegKey /ifempty SHCTX "Software\Warlock's Gauntlet"
 
SectionEnd
 
;--------------------------------
;Uninstaller Functions
 
Function un.onInit
 
  !insertmacro MULTIUSER_UNINIT
  !insertmacro MUI_UNGETLANGUAGE
 
FunctionEnd