;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  !define VERSION "9.0"
  Name "Amaya WX ${VERSION} (debug)"
  OutFile "Amaya-WX-${VERSION}-debug.exe"
  
  ;Use lzma to compress (better than zip)
  SetCompressor lzma

  ;Default installation folder
  InstallDir "$PROGRAMFILES\AmayaWX-debug"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\AmayaWX-debug" ""


;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\AmayaWX-debug" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "amaya\COPYRIGHT"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\AmayaWX-debug" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

  !insertmacro MUI_PAGE_INSTFILES
  
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
;  !insertmacro MUI_LANGUAGE "Italian"
;  !insertmacro MUI_LANGUAGE "Dutch"
;  !insertmacro MUI_LANGUAGE "Danish"
;  !insertmacro MUI_LANGUAGE "Swedish"
;  !insertmacro MUI_LANGUAGE "Norwegian"
;  !insertmacro MUI_LANGUAGE "Finnish"
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
;  !insertmacro MUI_LANGUAGE "Hungarian"
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

  ;This section is required : readonly mode
  SectionIn RO

  SetOutPath "$INSTDIR\WindowsWX\bin"
  File WindowsWX\bin\amaya.exe
  File WindowsWX\bin\wxmsw*ud_xrc_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_html_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_core_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_adv_vc_custom.dll
  File WindowsWX\bin\wxmsw*ud_gl_vc_custom.dll
  File WindowsWX\bin\wxbase*ud_vc_custom.dll
  File WindowsWX\bin\wxbase*ud_xml_vc_custom.dll
;  File WindowsWX\bin\thotprinter.exe


  SetOutPath "$INSTDIR\resources\icons"
  File resources\icons\*.png
  SetOutPath "$INSTDIR\resources\xrc"
  File resources\xrc\*.xrc


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

  SetOutPath "$INSTDIR\annotlib"
  File annotlib\*.png
  File annotlib\*.gif
  File annotlib\*.PRS
  File annotlib\*.STR
  File annotlib\*.TRA
  File annotlib\*.en

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

  SetOutPath "$INSTDIR\dicopar"
  File dicopar\alphabet
  File dicopar\*.ptn
  File dicopar\clavier
  File dicopar\*.dic

  SetOutPath "$INSTDIR\doc\html"
  File doc\html\*
  SetOutPath "$INSTDIR\doc\accessibility_in_amaya"
  File doc\html\accessibility_in_amaya\*
  SetOutPath "$INSTDIR\doc\attaching_annotations"
  File doc\html\attaching_annotations\*
  SetOutPath "$INSTDIR\doc\browsing"
  File doc\html\browsing\*
  SetOutPath "$INSTDIR\doc\configuring_amaya"
  File doc\html\configuring_amaya\*
  SetOutPath "$INSTDIR\doc\editing_attributes"
  File doc\html\editing_attributes\*
  SetOutPath "$INSTDIR\doc\editing_documents"
  File doc\html\editing_documents\*
  SetOutPath "$INSTDIR\doc\editing_iso-latin-1_characters"
  File doc\html\editing_iso-latin-1_characters\*
  SetOutPath "$INSTDIR\doc\editing_mathematics"
  File doc\html\editing_mathematics\*
  SetOutPath "$INSTDIR\doc\editing_tables"
  File doc\html\editing_tables\*
  SetOutPath "$INSTDIR\doc\elements"
  File doc\html\elements\*
  SetOutPath "$INSTDIR\doc\HTML-elements"
  File doc\html\HTML-elements\*
  SetOutPath "$INSTDIR\doc\linking"
  File doc\html\linking\*
  SetOutPath "$INSTDIR\doc\printing"
  File doc\html\printing\*
  SetOutPath "$INSTDIR\doc\saving_and_publishing_documents"
  File doc\html\saving_and_publishing_documents\*
  SetOutPath "$INSTDIR\doc\searching_and_replacing_text"
  File doc\html\searching_and_replacing_text\*
  SetOutPath "$INSTDIR\doc\selecting"
  File doc\html\selecting\*
  SetOutPath "$INSTDIR\doc\spell_checking"
  File doc\html\spell_checking\*
  SetOutPath "$INSTDIR\doc\style_sheets"
  File doc\html\style_sheets\*
  SetOutPath "$INSTDIR\doc\using_graphics"
  File doc\html\using_graphics\*
  SetOutPath "$INSTDIR\doc\using_image_maps"
  File doc\html\using_image_maps\*
  SetOutPath "$INSTDIR\doc\using_the_makebook_function"
  File doc\html\using_the_makebook_function\*
  SetOutPath "$INSTDIR\doc\using_xml"
  File doc\html\using_xml\*
  SetOutPath "$INSTDIR\doc\viewing"
  File doc\html\viewing\*

  SetOutPath "$INSTDIR\doc\images"
  File doc\images\*

  SetOutPath "$INSTDIR\fonts"
  File fonts\*

  ;Store installation folder
  WriteRegStr HKCU "Software\AmayaWX-debug" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Start Menu
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application   
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya.lnk" "$INSTDIR\WindowsWX\bin\amaya.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

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
    !insertmacro MUI_DESCRIPTION_TEXT ${SecAmaya} "Amaya."
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  ;Start Menu uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP   
  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  ;Delete empty start menu parent diretories
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
  startMenuDeleteLoop:
    ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."    
    IfErrors startMenuDeleteLoopDone
    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:


  DeleteRegKey /ifempty HKCU "Software\AmayaWX-debug"

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd