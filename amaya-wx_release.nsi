;--------------------------------
;Include Modern UI

  !include "MUI.nsh"


;--------------------------------
;General

  ;Name and file
  !define VERSION "11.3"
  Name "Amaya"
  OutFile "amaya-WinXP-${VERSION}.exe"
  
  ;Use lzma to compress (better than zip)
  SetCompressor lzma

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Amaya"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Amaya" ""

;--------------------------------
;Variables


  Var STARTMENU_FOLDER
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\Amaya" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "amaya\COPYRIGHT"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Amaya" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

  !insertmacro MUI_PAGE_INSTFILES
  
  !define MUI_FINISHPAGE_LINK "Visit the Amaya site for the latest news, FAQs and support"
  !define MUI_FINISHPAGE_LINK_LOCATION "http://www.w3.org/Amaya"

  !define MUI_FINISHPAGE_RUN "$INSTDIR\WindowsWX\bin\amaya.exe"
  !define MUI_FINISHPAGE_NOREBOOTSUPPORT
  
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "SimpChinese"
  !insertmacro MUI_LANGUAGE "TradChinese"
  !insertmacro MUI_LANGUAGE "Japanese"
;  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Dutch"
;  !insertmacro MUI_LANGUAGE "Danish"
;  !insertmacro MUI_LANGUAGE "Swedish"
;  !insertmacro MUI_LANGUAGE "Norwegian"
  !insertmacro MUI_LANGUAGE "Finnish"
  !insertmacro MUI_LANGUAGE "Greek"
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Portuguese"
;  !insertmacro MUI_LANGUAGE "PortugueseBR"
;  !insertmacro MUI_LANGUAGE "Polish"
;  !insertmacro MUI_LANGUAGE "Ukrainian"
;  !insertmacro MUI_LANGUAGE "Czech"
;  !insertmacro MUI_LANGUAGE "Slovak"
;  !insertmacro MUI_LANGUAGE "Croatian"
;  !insertmacro MUI_LANGUAGE "Bulgarian"
  !insertmacro MUI_LANGUAGE "Hungarian"
;  !insertmacro MUI_LANGUAGE "Thai"
;  !insertmacro MUI_LANGUAGE "Romanian"
;  !insertmacro MUI_LANGUAGE "Latvian"
;  !insertmacro MUI_LANGUAGE "Macedonian"
;  !insertmacro MUI_LANGUAGE "Estonian"
  !insertmacro MUI_LANGUAGE "Turkish"
;  !insertmacro MUI_LANGUAGE "Lithuanian"
;  !insertmacro MUI_LANGUAGE "Catalan"
;  !insertmacro MUI_LANGUAGE "Slovenian"
;  !insertmacro MUI_LANGUAGE "Serbian"
;  !insertmacro MUI_LANGUAGE "SerbianLatin"
;  !insertmacro MUI_LANGUAGE "Arabic"
;  !insertmacro MUI_LANGUAGE "Farsi"
;  !insertmacro MUI_LANGUAGE "Hebrew"
;  !insertmacro MUI_LANGUAGE "Indonesian"
;  !insertmacro MUI_LANGUAGE "Mongolian"
;  !insertmacro MUI_LANGUAGE "Luxembourgish"
;  !insertmacro MUI_LANGUAGE "Albanian"

;--------------------------------
;Reserve Files
  
  ;These files should be inserted before other files in the data block
  ;Keep these lines before any File command
  ;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

Section "Amaya" SecAmaya


  SetDetailsPrint textonly
  DetailPrint "Testing supported OS..."
  SetDetailsPrint listonly

 ;XXXXXXXXXXXXXXXXXXXXXXXXXX
 ;Test the platform
 ;XXXXXXXXXXXXXXXXXXXXXXXXXX
   ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
   StrCmp $R0 "" lbl_notwinnt lbl_winnt
   
   ; we are not NT
   lbl_notwinnt:
   ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
 
    
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '4.0' lbl_win32_95
   StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
 
   lbl_win32_95:
     Abort "Win95 not supported"
 
   lbl_win32_98:
     Abort "Win98 not supported"
 
   lbl_win32_ME:
     Abort "WinME not supported"
 
   lbl_winnt:
     StrCpy $R1 $R0 3
     StrCpy $R0 'XP'
     Goto lbl_done
 

   lbl_done:
  ;XXXXXXXXXXXXXXXXXXXXXXXXXX

  ;This section is required : readonly mode
  SectionIn RO

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya binaries and wxWidgets DLL"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\WindowsWX\bin"
  File WindowsWX\bin\amaya.exe
  File WindowsWX\bin\wxmsw*u_xrc_vc_custom.dll
  File WindowsWX\bin\wxmsw*u_html_vc_custom.dll
  File WindowsWX\bin\wxmsw*u_core_vc_custom.dll
  File WindowsWX\bin\wxmsw*u_adv_vc_custom.dll
  File WindowsWX\bin\wxmsw*u_aui_vc_custom.dll
  File WindowsWX\bin\wxmsw*u_gl_vc_custom.dll
  File WindowsWX\bin\wxbase*u_vc_custom.dll
  File WindowsWX\bin\wxbase*u_net_vc_custom.dll
  File WindowsWX\bin\wxbase*u_xml_vc_custom.dll
  File WindowsWX\bin\thotprinter.dll
  ;File WindowsWX\bin\msvc*.dll
  ;File WindowsWX\bin\Microsoft.VC90.CRT.manifest

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya resources : icons, dialogues"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\resources\icons\misc"
  File resources\icons\misc\*.png
  File resources\icons\misc\*.gif
  File resources\icons\misc\*.ico
  SetOutPath "$INSTDIR\resources\icons\16x16"
  File resources\icons\16x16\*.png
  File resources\icons\16x16\*.ico
  SetOutPath "$INSTDIR\resources\icons\22x22"
  File resources\icons\22x22\*.png
  SetOutPath "$INSTDIR\resources\svg"
  File resources\svg\*.png
  File resources\svg\*.svg
  SetOutPath "$INSTDIR\resources\svg\balloons"
  File resources\svg\balloons\*.png
  File resources\svg\balloons\*.svg
  SetOutPath "$INSTDIR\resources\svg\block_arrows"
  File resources\svg\block_arrows\*.png
  File resources\svg\block_arrows\*.svg
  SetOutPath "$INSTDIR\resources\svg\chemistry"
  File resources\svg\chemistry\*.png
  File resources\svg\chemistry\*.svg
  SetOutPath "$INSTDIR\resources\svg\circuit_diagram"
  File resources\svg\circuit_diagram\*.png
  File resources\svg\circuit_diagram\*.svg
  SetOutPath "$INSTDIR\resources\svg\logic_gates"
  File resources\svg\logic_gates\*.png
  File resources\svg\logic_gates\*.svg
  SetOutPath "$INSTDIR\resources\svg\polygons_and_stars"
  File resources\svg\polygons_and_stars\*.png
  File resources\svg\polygons_and_stars\*.svg
  SetOutPath "$INSTDIR\resources\xrc"
  File resources\xrc\*.xrc

  
  SetOutPath "$INSTDIR\amaya\wxdialog"
  File amaya\wxdialog\appicon.ico
  File amaya\wxdialog\amaya.rc

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya schemas"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\amaya"
  File amaya\COPYRIGHT
  File amaya\*.png
  File amaya\*.gif
  File amaya\*.css
  File amaya\*.html*
  File amaya\*.PRS
  File amaya\*.STR
  File amaya\*.TRA
  File amaya\*.conf
  File amaya\*.en
  File amaya\*.trans*
  File amaya\*.ico
  File amaya\*.svg

  SetDetailsPrint textonly
  DetailPrint "Installing annotlib schemas"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\annotlib"
  File annotlib\*.png
  File annotlib\*.gif
  File annotlib\*.PRS
  File annotlib\*.STR
  File annotlib\*.TRA
  File annotlib\*.en

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya config files"
  SetDetailsPrint listonly


  SetOutPath "$INSTDIR\config"
  File config\*-amayadialogue
  File config\*-libdialogue
  File config\*-amayamsg
  File config\*-libdialogue
  File config\fonts.*
  File config\amaya.kb
  File config\amaya.profiles
  File config\*.rdf
  File config\*.css
  File config\annot.schemas
  File config\win-thot.rc
  File config\rdfa.dat
  File config\rdfa_list.dat

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya dictionnaries"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\dicopar"
  File dicopar\alphabet
  File dicopar\*.ptn
  File dicopar\clavier
  File dicopar\*.dic

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya documentation"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\doc\WX"
  File doc\WX\*
  SetOutPath "$INSTDIR\doc\WX\HTML-elements"
  File doc\WX\HTML-elements\*
  SetOutPath "$INSTDIR\doc\images"
  File doc\images\*

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya ttf fonts"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\fonts"
  File fonts\*

  SetDetailsPrint textonly
  DetailPrint "Writting registry keys"
  SetDetailsPrint listonly

  ;Store installation folder
  WriteRegStr HKCU "Software\Amaya" "" $INSTDIR 
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya" "DisplayName" "Amaya"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya" "DisplayIcon" "$INSTDIR\WindowsWX\bin\amaya.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya" "URLInfoAbout" "http://www.w3.org/Amaya"

  ; Associate files to amaya
  WriteRegStr HKCR "Amaya" "" "Amaya Files"
  WriteRegStr HKCR "Amaya\DefaultIcon" "" "$INSTDIR\WindowsWX\bin\amaya.exe"
  ReadRegStr $R0 HKCR "Amaya\shell\open\command" ""
  StrCmp $R0 "" 0 no_amayaopen
    WriteRegStr HKCR "Amaya\shell" "" "open"
    WriteRegStr HKCR "Amaya\shell\open\command" "" '"$INSTDIR\WindowsWX\bin\amaya.exe" "%1"'
  no_amayaopen:
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Install Amaya for all users
  SetShellVarContext all

  ;Start Menu
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application   
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya.lnk" "$INSTDIR\WindowsWX\bin\amaya.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  ;Create desktop link
  CreateShortCut "$DESKTOP\Amaya.lnk" "$INSTDIR\WindowsWX\bin\amaya.exe"

  ;register extension files
  ;WriteRegStr HKCR ".xtd" "" "AM_XTD"
  ;WriteRegStr HKCR ".xtl" "" "AM_XTL"
  ;WriteRegStr HKCR ".svg" "" "AM_SVG"
  ;WriteRegStr HKCR ".mml" "" "AM_MML"
  ;WriteRegStr HKCR ".htm" "" "AM_HTM"
  ;WriteRegStr HKCR ".html" "" "AM_HTML"

  ;WriteRegStr HKCR "XTD\Shell\edit" "" "Edit with Amaya"
  ;WriteRegStr HKCR "XTD\Shell\edit\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'
  ;WriteRegStr HKCR "AM_XTD\Shell\Action2" "" "Edit with Amaya"
  ;WriteRegStr HKCR "AM_XTD\Shell\Action2\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'

  ;WriteRegStr HKCR "XTL\Shell\edit" "" "Edit with Amaya"
  ;WriteRegStr HKCR "XTL\Shell\edit\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'
  ;WriteRegStr HKCR "AM_XTL\Shell\Action2" "" "Edit with Amaya"
  ;WriteRegStr HKCR "AM_XTL\Shell\Action2\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'

  ;WriteRegStr HKCR "SVG\Shell\edit" "" "Edit with Amaya"
  ;WriteRegStr HKCR "SVG\Shell\edit\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'
  ;WriteRegStr HKCR "AM_SVG\Shell\Action2" "" "Edit with Amaya"
  ;WriteRegStr HKCR "AM_SVG\Shell\Action2\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'

  ;WriteRegStr HKCR "MML\Shell\edit" "" "Edit with Amaya"
  ;WriteRegStr HKCR "MML\Shell\edit\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'
  ;WriteRegStr HKCR "AM_MML\Shell\Action2" "" "Edit with Amaya"
  ;WriteRegStr HKCR "AM_MML\Shell\Action2\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'

  ;WriteRegStr HKCR "HTML\Shell\edit" "" "Edit with Amaya"
  ;WriteRegStr HKCR "HTML\Shell\edit\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'
  ;WriteRegStr HKCR "AM_HTML\Shell\Action2" "" "Edit with Amaya"
  ;WriteRegStr HKCR "AM_HTML\Shell\Action2\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'

  ;WriteRegStr HKCR "HTM\Shell\edit" "" "Edit with Amaya"
  ;WriteRegStr HKCR "HTM\Shell\edit\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'
  ;WriteRegStr HKCR "AM_HTM\Shell\Action2" "" "Edit with Amaya"
  ;WriteRegStr HKCR "AM_HTM\Shell\Action2\command" "" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'

SectionEnd


SubSection "File association" SecFileAss

; --> .html
Section /o ".html (HyperText Markup Language)" SecAssHTML
  ReadRegStr $R0 HKCR ".html" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".html" "AM_OLD_VALUE" $R0
  WriteRegStr HKCR ".html" "" "Amaya"
  allready_amaya:
SectionEnd

; --> .htm
Section /o ".htm (HyperText Markup Language)" SecAssHTM
  ReadRegStr $R0 HKCR ".htm" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".htm" "AM_OLD_VALUE" $R0
    WriteRegStr HKCR ".htm" "" "Amaya"
  allready_amaya:
SectionEnd

; --> .xml
Section /o ".xml (eXtensible Markup Language)" SecAssXML
  ReadRegStr $R0 HKCR ".xml" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".xml" "AM_OLD_VALUE" $R0
  WriteRegStr HKCR ".xml" "" "Amaya"
  allready_amaya:
SectionEnd

; --> .svg
Section /o ".svg (Scalable Vector Graphics)" SecAssSVG
  ReadRegStr $R0 HKCR ".svg" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".svg" "AM_OLD_VALUE" $R0
  WriteRegStr HKCR ".svg" "" "Amaya"
  allready_amaya:
SectionEnd

; --> .mml
Section /o ".mml (MathML)" SecAssMML
  ReadRegStr $R0 HKCR ".mml" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".mml" "AM_OLD_VALUE" $R0
  WriteRegStr HKCR ".mml" "" "Amaya"
  allready_amaya:
SectionEnd

; --> .css
Section /o ".css (Cascading Style Sheets)" SecAssCSS
  ReadRegStr $R0 HKCR ".css" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".css" "AM_OLD_VALUE" $R0
  WriteRegStr HKCR ".css" "" "Amaya"
  allready_amaya:
SectionEnd

; --> .xtd
Section /o ".xtd (XTiger Template)" SecAssXTD
  ReadRegStr $R0 HKCR ".xtd" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".xtd" "AM_OLD_VALUE" $R0
  WriteRegStr HKCR ".xtd" "" "Amaya"
  allready_amaya:
SectionEnd

; --> .xtl
Section /o ".xtl (XTiger Library)" SecAssXTL
  ReadRegStr $R0 HKCR ".xtl" ""
  StrCmp $R0 "Amaya" allready_amaya no_amaya
  no_amaya:
    WriteRegStr HKCR ".xtl" "AM_OLD_VALUE" $R0
  WriteRegStr HKCR ".xtl" "" "Amaya"
  allready_amaya:
SectionEnd

SubSectionEnd

;--------------------------------
;Installer Functions

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
;Descriptions

  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC

  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecAmaya} "Install main Amaya program (mandatory)."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecFileAss} "Selects Amaya as the default application for files of these types."
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  SetDetailsPrint textonly
  DetailPrint "Uninstalling Amaya..."
  SetDetailsPrint listonly

  IfFileExists $INSTDIR\WindowsWX\bin\amaya.exe amaya_installed
    MessageBox MB_YESNO "It does not appear that Amaya is installed in the directory '$INSTDIR'.$\r$\nContinue anyway (not recommended)?" IDYES amaya_installed
    Abort "Uninstall aborted by user"
  amaya_installed:


  SetDetailsPrint textonly
  DetailPrint "Deleting Files..."
  SetDetailsPrint listonly
  ;Uninstall Amaya for all users
  SetShellVarContext all
  
  ReadRegStr $STARTMENU_FOLDER HKCU "Software\Amaya" "Start Menu Folder"
  IfFileExists "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya.lnk" amaya_smp_installed
    Goto amaya_smp_notinstalled
  amaya_smp_installed:
  Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$STARTMENU_FOLDER"
  Delete "$DESKTOP\Amaya.lnk"
  amaya_smp_notinstalled:

  RMDir /r "$INSTDIR"

  SetDetailsPrint textonly
  DetailPrint "Deleting Registry Keys..."
  SetDetailsPrint listonly

  DeleteRegKey HKCR "AM_XTD\Shell\Action2"
  DeleteRegKey HKCR "AM_XTL\Shell\Action2"
  DeleteRegKey HKCR "AM_SVG\Shell\Action2"
  DeleteRegKey HKCR "AM_MML\Shell\Action2"
  DeleteRegKey HKCR "AM_HTM\Shell\Action2"
  DeleteRegKey HKCR "AM_HTML\Shell\Action2"

  DeleteRegKey HKCR "XTD\Shell\Action2"
  DeleteRegKey HKCR "XTL\Shell\Action2"
  DeleteRegKey HKCR "SVG\Shell\Action2"
  DeleteRegKey HKCR "MML\Shell\Action2"
  DeleteRegKey HKCR "HTM\Shell\Action2"
  DeleteRegKey HKCR "HTML\Shell\Action2"

  ;DeleteRegKey HKCR "HTML\Shell\edit"

  ;DeleteRegValue HKCR "XTD\Shell" "edit"
  ;DeleteRegKey HKCR "XTD\Shell"
  ;DeleteRegValue HKCR "XTD\Shell\Action2" "Edit with Amaya"
  ;DeleteRegValue HKCR "XTD\Shell\Action2\command" "Edit with Amaya"
  ;DeleteRegValue HKCR "XTD\Shell\Action2" "Edit with Amaya"
  ;DeleteRegValue HKCR "XTD\Shell\Action2\command" '$INSTDIR\WindowsWX\bin\amaya.exe "%1"'
  ;DeleteRegValue HKCR "XTL\Shell\Action2" "Edit with Amaya"
  ;DeleteRegValue HKCR "XTL\Shell\Action2\command" "Edit with Amaya"
  ;DeleteRegValue HKCR "SVG\Shell\Action2" "Edit with Amaya"
  ;DeleteRegValue HKCR "SVG\Shell\Action2\command" "Edit with Amaya"
  ;DeleteRegValue HKCR "MML\Shell\Action2" "Edit with Amaya"
  ;DeleteRegValue HKCR "MML\Shell\Action2\command" "Edit with Amaya"
  ;DeleteRegValue HKCR "HTM\Shell\Action2" "Edit with Amaya"
  ;DeleteRegValue HKCR "HTM\Shell\Action2\command" "Edit with Amaya"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya"
  DeleteRegKey HKLM "Software\Amaya"
  DeleteRegKey HKCR "Amaya"
  DeleteRegKey HKCU "Software\Amaya"

; uninstall files associations
  ; --> .html
  ReadRegStr $R0 HKCR ".html" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".html" "AM_OLD_VALUE"
    WriteRegStr HKCR ".html" "" $R0
  ; --> .htm
  ReadRegStr $R0 HKCR ".htm" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".htm" "AM_OLD_VALUE"
    WriteRegStr HKCR ".htm" "" $R0
  ; --> .css
  ReadRegStr $R0 HKCR ".css" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".css" "AM_OLD_VALUE"
    WriteRegStr HKCR ".css" "" $R0
  ; --> .svg
  ReadRegStr $R0 HKCR ".svg" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".svg" "AM_OLD_VALUE"
    WriteRegStr HKCR ".svg" "" $R0
  ; --> .mml
  ReadRegStr $R0 HKCR ".mml" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".mml" "AM_OLD_VALUE"
    WriteRegStr HKCR ".mml" "" $R0
  ; --> .xml
  ReadRegStr $R0 HKCR ".xml" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".xml" "AM_OLD_VALUE"
    WriteRegStr HKCR ".xml" "" $R0
  ; --> .xtd
  ReadRegStr $R0 HKCR ".xtd" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".xtd" "AM_OLD_VALUE"
    WriteRegStr HKCR ".xtd" "" $R0
  ; --> .xtl
  ReadRegStr $R0 HKCR ".xtl" ""
  StrCmp $R0 "Amaya" 0 +3
    ReadRegStr $R0 HKCR ".xtl" "AM_OLD_VALUE"
    WriteRegStr HKCR ".xtl" "" $R0

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd
