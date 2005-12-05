;--------------------------------
;Include Modern UI

  !include "MUI.nsh"


;--------------------------------
;General

  ;Name and file
  !define VERSION "9.2.1"
  Name "Amaya ${VERSION} (debug)"
  OutFile "amaya-WinXP-${VERSION}-debug.exe"
  
  ;Use lzma to compress (better than zip)
  SetCompressor lzma

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Amaya-${VERSION}-debug"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Amaya-${VERSION}-debug" ""

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
  !define MUI_LANGDLL_REGISTRY_KEY "Software\Amaya-${VERSION}-debug" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "amaya\COPYRIGHT"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Amaya-${VERSION}-debug" 
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
;  !insertmacro MUI_LANGUAGE "SimpChinese"
;  !insertmacro MUI_LANGUAGE "TradChinese"
;  !insertmacro MUI_LANGUAGE "Japanese"
;  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "Italian"
;  !insertmacro MUI_LANGUAGE "Dutch"
;  !insertmacro MUI_LANGUAGE "Danish"
;  !insertmacro MUI_LANGUAGE "Swedish"
;  !insertmacro MUI_LANGUAGE "Norwegian"
  !insertmacro MUI_LANGUAGE "Finnish"
;  !insertmacro MUI_LANGUAGE "Greek"
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
 
   StrCpy $R1 $R0 1
   StrCmp $R1 '4' 0 lbl_error
 
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
 
   StrCpy $R1 $R0 1
 
   StrCmp $R1 '3' lbl_winnt_x
   StrCmp $R1 '4' lbl_winnt_x
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '5.0' lbl_winnt_2000
   StrCmp $R1 '5.1' lbl_winnt_XP
   StrCmp $R1 '5.2' lbl_winnt_2003
   Goto lbl_error
 
   lbl_winnt_x:
     StrCpy $R0 "NT $R0" 6
   Goto lbl_done
 
   lbl_winnt_2000:
     Strcpy $R0 '2000'
   Goto lbl_done
 
   lbl_winnt_XP:
     Strcpy $R0 'XP'
   Goto lbl_done
 
   lbl_winnt_2003:
     Strcpy $R0 '2003'
   Goto lbl_done
 
   lbl_error:
     Abort "Only WinXP/2k/NT are supported"
   lbl_done:
  ;XXXXXXXXXXXXXXXXXXXXXXXXXX

  ;This section is required : readonly mode
  SectionIn RO

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya binaries and wxWidgets DLL"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\WindowsWX\bin"
  File WindowsWX\bin\amaya.exe
  File WindowsWX\bin\wxmsw*ud_xrc_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_html_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_core_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_adv_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_gl_vc_custom.dll
  File WindowsWX\bin\wxbase*ud_vc_custom.dll
  File WindowsWX\bin\wxbase*ud_xml_vc_custom.dll
  File WindowsWX\bin\thotprinter.dll
  File WindowsWX\bin\MSVCRTD.DLL

  SetDetailsPrint textonly
  DetailPrint "Installing Amaya resources : icons, dialogues"
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR\resources\icons\misc"
  File resources\icons\misc\*.png
  File resources\icons\misc\*.gif
  File resources\icons\misc\*.ico
  SetOutPath "$INSTDIR\resources\icons\16x16"
  File resources\icons\16x16\*.png
  File resources\icons\16x16\*.gif
  File resources\icons\16x16\*.ico
  SetOutPath "$INSTDIR\resources\icons\22x22"
  File resources\icons\22x22\*.png
  File resources\icons\22x22\*.gif
  File resources\icons\22x22\*.ico
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

  SetOutPath "$INSTDIR\config\libconfig"
  File config\libconfig\*.png
  File config\libconfig\*.svg
  File config\libconfig\*.css
  File config\libconfig\*.lhtml

  SetOutPath "$INSTDIR\config"
  File config\*-amayadialogue
  File config\*-libdialogue
  File config\*-amayamsg
  File config\*-libdialogue
  File config\lib_files.dat
  File config\fonts.*
  File config\amaya.kb
  File config\amaya.profiles
  File config\*.rdf
  File config\annot.schemas
  File config\win-thot.rc

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
  SetOutPath "$INSTDIR\doc\html"
  File doc\html\*
  SetOutPath "$INSTDIR\doc\html\accessibility_in_amaya"
  File doc\html\accessibility_in_amaya\*
  SetOutPath "$INSTDIR\doc\html\attaching_annotations"
  File doc\html\attaching_annotations\*
  SetOutPath "$INSTDIR\doc\html\browsing"
  File doc\html\browsing\*
  SetOutPath "$INSTDIR\doc\html\configuring_amaya"
  File doc\html\configuring_amaya\*
  SetOutPath "$INSTDIR\doc\html\editing_attributes"
  File doc\html\editing_attributes\*
  SetOutPath "$INSTDIR\doc\html\editing_documents"
  File doc\html\editing_documents\*
  SetOutPath "$INSTDIR\doc\html\editing_iso-latin-1_characters"
  File doc\html\editing_iso-latin-1_characters\*
  SetOutPath "$INSTDIR\doc\html\editing_mathematics"
  File doc\html\editing_mathematics\*
  SetOutPath "$INSTDIR\doc\html\editing_tables"
  File doc\html\editing_tables\*
  SetOutPath "$INSTDIR\doc\html\elements"
  File doc\html\elements\*
  SetOutPath "$INSTDIR\doc\html\HTML-elements"
  File doc\html\HTML-elements\*
  SetOutPath "$INSTDIR\doc\html\linking"
  File doc\html\linking\*
  SetOutPath "$INSTDIR\doc\html\printing"
  File doc\html\printing\*
  SetOutPath "$INSTDIR\doc\html\saving_and_publishing_documents"
  File doc\html\saving_and_publishing_documents\*
  SetOutPath "$INSTDIR\doc\html\searching_and_replacing_text"
  File doc\html\searching_and_replacing_text\*
  SetOutPath "$INSTDIR\doc\html\selecting"
  File doc\html\selecting\*
  SetOutPath "$INSTDIR\doc\html\spell_checking"
  File doc\html\spell_checking\*
  SetOutPath "$INSTDIR\doc\html\style_sheets"
  File doc\html\style_sheets\*
  SetOutPath "$INSTDIR\doc\html\using_graphics"
  File doc\html\using_graphics\*
  SetOutPath "$INSTDIR\doc\html\using_image_maps"
  File doc\html\using_image_maps\*
  SetOutPath "$INSTDIR\doc\html\using_the_makebook_function"
  File doc\html\using_the_makebook_function\*
  SetOutPath "$INSTDIR\doc\html\using_xml"
  File doc\html\using_xml\*
  SetOutPath "$INSTDIR\doc\html\viewing"
  File doc\html\viewing\*
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
  WriteRegStr HKCU "Software\Amaya-${VERSION}-debug" "" $INSTDIR 
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya-${VERSION}-debug" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya-${VERSION}-debug" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya-${VERSION}-debug" "DisplayName" "Amaya ${VERSION} (debug)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya-${VERSION}-debug" "DisplayIcon" "$INSTDIR\WindowsWX\bin\amaya.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya-${VERSION}-debug" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya-${VERSION}-debug" "URLInfoAbout" "http://www.w3.org/Amaya"

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
  CreateShortCut "$DESKTOP\Amaya-${VERSION}-debug.lnk" "$INSTDIR\WindowsWX\bin\amaya.exe"
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

  ReadRegStr $STARTMENU_FOLDER HKCU "Software\Amaya-${VERSION}-debug" "Start Menu Folder"
  IfFileExists "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya.lnk" amaya_smp_installed
    Goto amaya_smp_notinstalled
  amaya_smp_installed:
    Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya.lnk"
    Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk"
    RMDir  "$SMPROGRAMS\$STARTMENU_FOLDER"
    Delete "$DESKTOP\Amaya-${VERSION}-debug.lnk"
  amaya_smp_notinstalled:

  RMDir /r "$INSTDIR"

  SetDetailsPrint textonly
  DetailPrint "Deleting Registry Keys..."
  SetDetailsPrint listonly

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Amaya-${VERSION}-debug"
  DeleteRegKey HKLM "Software\Amaya-${VERSION}-debug"
  DeleteRegKey HKCR "Amaya"
  DeleteRegKey HKCU "Software\Amaya-${VERSION}-debug"

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

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd