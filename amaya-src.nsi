;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  !define VERSION "9.1-src"
  Name "Amaya ${VERSION}"
  OutFile "Amaya-${VERSION}.exe"
  
  ;Use lzma to compress (better than zip)
  SetCompressor lzma

  ;Default installation folder
  InstallDir "$EXEDIR\src"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Amaya-src" ""


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
  !define MUI_LANGDLL_REGISTRY_KEY "Software\Amaya-src" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "amaya\COPYRIGHT"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Amaya-src" 
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

  ; Amaya sources
  SetOutPath "$INSTDIR\Amaya\WindowsWX"
  File WindowsWX\*.dsw
  File WindowsWX\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\wx-setup"
  File WindowsWX\wx-setup\setup.h
  SetOutPath "$INSTDIR\Amaya\WindowsWX\zlib"
  File WindowsWX\zlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\tra"
  File WindowsWX\tra\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\thotprinter"
  File WindowsWX\thotprinter\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\str"
  File WindowsWX\str\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\prs"
  File WindowsWX\prs\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\printlib"
  File WindowsWX\printlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\libwww"
  File WindowsWX\libwww\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\LibThotKernel"
  File WindowsWX\LibThotKernel\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\LibThotEditor"
  File WindowsWX\LibThotEditor\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\librdf"
  File WindowsWX\librdf\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\libraptor"
  File WindowsWX\libraptor\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\libpng"
  File WindowsWX\libpng\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\libjpeg"
  File WindowsWX\libjpeg\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\davlib"
  File WindowsWX\davlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\cpp"
  File WindowsWX\cpp\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\Compilers"
  File WindowsWX\Compilers\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\app"
  File WindowsWX\app\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\annotlib"
  File WindowsWX\annotlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsWX\amaya"
  File WindowsWX\amaya\amaya.mkf


  SetOutPath "$INSTDIR\Amaya\WindowsGL"
  File WindowsGL\*.dsw
  File WindowsGL\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\zlib"
  File WindowsGL\zlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\tra"
  File WindowsGL\tra\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\thotprinter"
  File WindowsGL\thotprinter\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\str"
  File WindowsGL\str\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\prs"
  File WindowsGL\prs\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\printlib"
  File WindowsGL\printlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\libwww"
  File WindowsGL\libwww\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\LibThotKernel"
  File WindowsGL\LibThotKernel\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\LibThotEditor"
  File WindowsGL\LibThotEditor\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\librdf"
  File WindowsGL\librdf\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\libraptor"
  File WindowsGL\libraptor\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\libpng"
  File WindowsGL\libpng\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\libjpeg"
  File WindowsGL\libjpeg\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\davlib"
  File WindowsGL\davlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\cpp"
  File WindowsGL\cpp\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\Compilers"
  File WindowsGL\Compilers\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\app"
  File WindowsGL\app\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\annotlib"
  File WindowsGL\annotlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\WindowsGL\amaya"
  File WindowsGL\amaya\amaya.mkf


  SetOutPath "$INSTDIR\Amaya\Windows"
  File Windows\*.dsw
  File Windows\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\zlib"
  File Windows\zlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\tra"
  File Windows\tra\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\thotprinter"
  File Windows\thotprinter\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\str"
  File Windows\str\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\prs"
  File Windows\prs\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\printlib"
  File Windows\printlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\libwww"
  File Windows\libwww\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\LibThotKernel"
  File Windows\LibThotKernel\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\LibThotEditor"
  File Windows\LibThotEditor\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\librdf"
  File Windows\librdf\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\libraptor"
  File Windows\libraptor\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\libpng"
  File Windows\libpng\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\libjpeg"
  File Windows\libjpeg\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\davlib"
  File Windows\davlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\cpp"
  File Windows\cpp\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\Compilers"
  File Windows\Compilers\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\app"
  File Windows\app\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\annotlib"
  File Windows\annotlib\*.dsp
  SetOutPath "$INSTDIR\Amaya\Windows\amaya"
  File Windows\amaya\amaya.mkf


  SetOutPath "$INSTDIR\Amaya\tools"
  File /r tools\*
  SetOutPath "$INSTDIR\Amaya\thotlib"
  File /r thotlib\*
  SetOutPath "$INSTDIR\Amaya\templates"
  File /r templates\*
  SetOutPath "$INSTDIR\Amaya\resources"
  File /r resources\*
  SetOutPath "$INSTDIR\Amaya\libpng"
  File /r libpng\*
  SetOutPath "$INSTDIR\Amaya\libjpeg"
  File /r libjpeg\*
  SetOutPath "$INSTDIR\Amaya\Icons"
  File /r Icons\*
  SetOutPath "$INSTDIR\Amaya\fonts"
  File /r fonts\*
  SetOutPath "$INSTDIR\Amaya\doc"
  File /r doc\*
  SetOutPath "$INSTDIR\Amaya\dicopar"
  File /r dicopar\*
  SetOutPath "$INSTDIR\Amaya\davlib"
  File /r davlib\*
  SetOutPath "$INSTDIR\Amaya\cpp"
  File /r cpp\*
  SetOutPath "$INSTDIR\Amaya\config"
  File /r config\*
  SetOutPath "$INSTDIR\Amaya\batch"
  File /r batch\*
  SetOutPath "$INSTDIR\Amaya\annotlib"
  File /r annotlib\*
  SetOutPath "$INSTDIR\Amaya\amaya"
  File /r amaya\*

  SetOutPath "$INSTDIR\Amaya"
  File *.info
  File *.pkg
  File *.spec
  File *.nsi
  File *.in
  File config.guess
  File config.sub
  File configure
  File install-sh
  File README*
  File *.html

  ; libwww sources
  SetOutPath "$INSTDIR\libwww"
  File /r ..\libwww\*

  ; redland sources
  SetOutPath "$INSTDIR\redland"
  File /r ..\redland\*

  ; freetype sources
  SetOutPath "$INSTDIR\freetype-2.1.9"
  File ..\freetype-2.1.9\*
  SetOutPath "$INSTDIR\freetype-2.1.9"
  File /r ..\freetype-2.1.9\builds
  SetOutPath "$INSTDIR\freetype-2.1.9"
  File /r ..\freetype-2.1.9\devel
  SetOutPath "$INSTDIR\freetype-2.1.9"
  File /r ..\freetype-2.1.9\include
  SetOutPath "$INSTDIR\freetype-2.1.9\objs"
  File ..\freetype-2.1.9\objs\README
  SetOutPath "$INSTDIR\freetype-2.1.9"
  File /r ..\freetype-2.1.9\src

  ; wxWidgets sources
  SetOutPath "$INSTDIR\wxWidgets"
  File ..\wxWidgets\*
  SetOutPath "$INSTDIR\wxWidgets"
  File /r ..\wxWidgets\src
  SetOutPath "$INSTDIR\wxWidgets"
  File /r ..\wxWidgets\include
  SetOutPath "$INSTDIR\wxWidgets"
  File /r ..\wxWidgets\locale
  SetOutPath "$INSTDIR\wxWidgets"
  File /r ..\wxWidgets\art
  SetOutPath "$INSTDIR\wxWidgets\lib"
  File ..\wxWidgets\lib\dummy
  SetOutPath "$INSTDIR\wxWidgets\build\msw"
  File ..\wxWidgets\build\msw\*.dsp
  File ..\wxWidgets\build\msw\*.dsw
  File ..\wxWidgets\build\msw\config*
  File ..\wxWidgets\build\msw\makefile*

  ;Store installation folder
  WriteRegStr HKCU "Software\Amaya-src" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Start Menu
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application   
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya-src.lnk" "$INSTDIR\"
  !insertmacro MUI_STARTMENU_WRITE_END

  ;Create desktop link
  CreateShortCut "$DESKTOP\Amaya-src.lnk" "$INSTDIR\"
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
    !insertmacro MUI_DESCRIPTION_TEXT ${SecAmaya} "Amaya sources."
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  ;Start Menu uninstall
;  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP   
;  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
;  ;Delete empty start menu parent diretories
;  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
;  startMenuDeleteLoop:
;    ClearErrors
;    RMDir $MUI_TEMP
;    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."    
;    IfErrors startMenuDeleteLoopDone
;    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
;  startMenuDeleteLoopDone:

  Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Amaya-src.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$STARTMENU_FOLDER"

  DeleteRegKey /ifempty HKCU "Software\Amaya-src"

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd
