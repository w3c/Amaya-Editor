# Microsoft Developer Studio Generated NMAKE File, Based on amaya.dsp
!IF "$(CFG)" == ""
CFG=amaya - Win32 Debug
!MESSAGE No configuration specified. Defaulting to amaya - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "amaya - Win32 Release" && "$(CFG)" != "amaya - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "amaya.mak" CFG="amaya - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "amaya - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "amaya - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "amaya - Win32 Release"

OUTDIR=.\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\amaya.exe"

!ELSE 

ALL : "Compilers - Win32 Release" "annotlib - Win32 Release" "libThotTable - Win32 Release" "libpng - Win32 Release" "libjpeg - Win32 Release" "thotprinter - Win32 Release" "libwww - Win32 Release" "libThotEditor - Win32 Release" "$(OUTDIR)\amaya.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 ReleaseCLEAN" "libwww - Win32 ReleaseCLEAN" "thotprinter - Win32 ReleaseCLEAN" "libjpeg - Win32 ReleaseCLEAN" "libpng - Win32 ReleaseCLEAN" "libThotTable - Win32 ReleaseCLEAN" "annotlib - Win32 ReleaseCLEAN" "Compilers - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AHTBridge.obj"
	-@erase "$(INTDIR)\AHTFWrite.obj"
	-@erase "$(INTDIR)\AHTMemConv.obj"
	-@erase "$(INTDIR)\AHTURLTools.obj"
	-@erase "$(INTDIR)\amaya.res"
	-@erase "$(INTDIR)\answer.obj"
	-@erase "$(INTDIR)\css.obj"
	-@erase "$(INTDIR)\EDITimage.obj"
	-@erase "$(INTDIR)\EDITORactions.obj"
	-@erase "$(INTDIR)\EDITORAPP.obj"
	-@erase "$(INTDIR)\EDITstyle.obj"
	-@erase "$(INTDIR)\fetchHTMLname.obj"
	-@erase "$(INTDIR)\fetchXMLname.obj"
	-@erase "$(INTDIR)\html2thot.obj"
	-@erase "$(INTDIR)\HTMLactions.obj"
	-@erase "$(INTDIR)\HTMLAPP.obj"
	-@erase "$(INTDIR)\HTMLbook.obj"
	-@erase "$(INTDIR)\HTMLedit.obj"
	-@erase "$(INTDIR)\HTMLform.obj"
	-@erase "$(INTDIR)\HTMLhistory.obj"
	-@erase "$(INTDIR)\HTMLimage.obj"
	-@erase "$(INTDIR)\HTMLpresentation.obj"
	-@erase "$(INTDIR)\HTMLsave.obj"
	-@erase "$(INTDIR)\HTMLtable.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\Mathedit.obj"
	-@erase "$(INTDIR)\MathMLAPP.obj"
	-@erase "$(INTDIR)\MathMLbuilder.obj"
	-@erase "$(INTDIR)\MENUconf.obj"
	-@erase "$(INTDIR)\query.obj"
	-@erase "$(INTDIR)\styleparser.obj"
	-@erase "$(INTDIR)\SVGAPP.obj"
	-@erase "$(INTDIR)\SVGbuilder.obj"
	-@erase "$(INTDIR)\SVGedit.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\templates.obj"
	-@erase "$(INTDIR)\TextFileAPP.obj"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\transparse.obj"
	-@erase "$(INTDIR)\UIcss.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\windialogapi.obj"
	-@erase "$(INTDIR)\XHTMLbuilder.obj"
	-@erase "$(INTDIR)\XLinkAPP.obj"
	-@erase "$(INTDIR)\XLinkbuilder.obj"
	-@erase "$(INTDIR)\XLinkedit.obj"
	-@erase "$(INTDIR)\Xml2thot.obj"
	-@erase "$(INTDIR)\XMLAPP.obj"
	-@erase "$(INTDIR)\Xmlbuilder.obj"
	-@erase "$(INTDIR)\XPointer.obj"
	-@erase "$(INTDIR)\XPointerparse.obj"
	-@erase "$(OUTDIR)\amaya.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\f" /I "..\annotlib" /I "..\annotlib\f" /D "NDEBUG" /D "XML_DTD" /D "XML_NS" /D "_SVG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "THOT_TOOLTIPS" /D "ANNOTATIONS" /D "XML_GENERIC" /D "ANNOT_ON_ANNOT" /Fp"$(INTDIR)\amaya.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\amaya.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\amaya.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib wsock32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib winspool.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\amaya.pdb" /machine:I386 /out:"$(OUTDIR)\amaya.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AHTBridge.obj" \
	"$(INTDIR)\AHTFWrite.obj" \
	"$(INTDIR)\AHTMemConv.obj" \
	"$(INTDIR)\AHTURLTools.obj" \
	"$(INTDIR)\answer.obj" \
	"$(INTDIR)\css.obj" \
	"$(INTDIR)\EDITimage.obj" \
	"$(INTDIR)\EDITORactions.obj" \
	"$(INTDIR)\EDITORAPP.obj" \
	"$(INTDIR)\EDITstyle.obj" \
	"$(INTDIR)\fetchHTMLname.obj" \
	"$(INTDIR)\fetchXMLname.obj" \
	"$(INTDIR)\html2thot.obj" \
	"$(INTDIR)\HTMLactions.obj" \
	"$(INTDIR)\HTMLAPP.obj" \
	"$(INTDIR)\HTMLbook.obj" \
	"$(INTDIR)\HTMLedit.obj" \
	"$(INTDIR)\HTMLform.obj" \
	"$(INTDIR)\HTMLhistory.obj" \
	"$(INTDIR)\HTMLimage.obj" \
	"$(INTDIR)\HTMLpresentation.obj" \
	"$(INTDIR)\HTMLsave.obj" \
	"$(INTDIR)\HTMLtable.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\Mathedit.obj" \
	"$(INTDIR)\MathMLAPP.obj" \
	"$(INTDIR)\MathMLbuilder.obj" \
	"$(INTDIR)\MENUconf.obj" \
	"$(INTDIR)\query.obj" \
	"$(INTDIR)\styleparser.obj" \
	"$(INTDIR)\SVGAPP.obj" \
	"$(INTDIR)\SVGbuilder.obj" \
	"$(INTDIR)\SVGedit.obj" \
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\TextFileAPP.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\transparse.obj" \
	"$(INTDIR)\UIcss.obj" \
	"$(INTDIR)\windialogapi.obj" \
	"$(INTDIR)\XHTMLbuilder.obj" \
	"$(INTDIR)\XLinkAPP.obj" \
	"$(INTDIR)\XLinkbuilder.obj" \
	"$(INTDIR)\XLinkedit.obj" \
	"$(INTDIR)\Xml2thot.obj" \
	"$(INTDIR)\XMLAPP.obj" \
	"$(INTDIR)\Xmlbuilder.obj" \
	"$(INTDIR)\XPointer.obj" \
	"$(INTDIR)\XPointerparse.obj" \
	"$(INTDIR)\amaya.res" \
	".\libThotEditor.lib" \
	".\libwww.lib" \
	"$(OUTDIR)\thotprinter.lib" \
	".\libjpeg.lib" \
	".\libpng.lib" \
	".\libThotTable.lib" \
	".\annotlib.lib"

"$(OUTDIR)\amaya.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

OUTDIR=.\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\amaya.exe"

!ELSE 

ALL : "Compilers - Win32 Debug" "annotlib - Win32 Debug" "libThotTable - Win32 Debug" "libpng - Win32 Debug" "libjpeg - Win32 Debug" "thotprinter - Win32 Debug" "libwww - Win32 Debug" "libThotEditor - Win32 Debug" "$(OUTDIR)\amaya.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 DebugCLEAN" "libwww - Win32 DebugCLEAN" "thotprinter - Win32 DebugCLEAN" "libjpeg - Win32 DebugCLEAN" "libpng - Win32 DebugCLEAN" "libThotTable - Win32 DebugCLEAN" "annotlib - Win32 DebugCLEAN" "Compilers - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AHTBridge.obj"
	-@erase "$(INTDIR)\AHTFWrite.obj"
	-@erase "$(INTDIR)\AHTMemConv.obj"
	-@erase "$(INTDIR)\AHTURLTools.obj"
	-@erase "$(INTDIR)\amaya.res"
	-@erase "$(INTDIR)\answer.obj"
	-@erase "$(INTDIR)\css.obj"
	-@erase "$(INTDIR)\EDITimage.obj"
	-@erase "$(INTDIR)\EDITORactions.obj"
	-@erase "$(INTDIR)\EDITORAPP.obj"
	-@erase "$(INTDIR)\EDITstyle.obj"
	-@erase "$(INTDIR)\fetchHTMLname.obj"
	-@erase "$(INTDIR)\fetchXMLname.obj"
	-@erase "$(INTDIR)\html2thot.obj"
	-@erase "$(INTDIR)\HTMLactions.obj"
	-@erase "$(INTDIR)\HTMLAPP.obj"
	-@erase "$(INTDIR)\HTMLbook.obj"
	-@erase "$(INTDIR)\HTMLedit.obj"
	-@erase "$(INTDIR)\HTMLform.obj"
	-@erase "$(INTDIR)\HTMLhistory.obj"
	-@erase "$(INTDIR)\HTMLimage.obj"
	-@erase "$(INTDIR)\HTMLpresentation.obj"
	-@erase "$(INTDIR)\HTMLsave.obj"
	-@erase "$(INTDIR)\HTMLtable.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\Mathedit.obj"
	-@erase "$(INTDIR)\MathMLAPP.obj"
	-@erase "$(INTDIR)\MathMLbuilder.obj"
	-@erase "$(INTDIR)\MENUconf.obj"
	-@erase "$(INTDIR)\query.obj"
	-@erase "$(INTDIR)\styleparser.obj"
	-@erase "$(INTDIR)\SVGAPP.obj"
	-@erase "$(INTDIR)\SVGbuilder.obj"
	-@erase "$(INTDIR)\SVGedit.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\templates.obj"
	-@erase "$(INTDIR)\TextFileAPP.obj"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\transparse.obj"
	-@erase "$(INTDIR)\UIcss.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\windialogapi.obj"
	-@erase "$(INTDIR)\XHTMLbuilder.obj"
	-@erase "$(INTDIR)\XLinkAPP.obj"
	-@erase "$(INTDIR)\XLinkbuilder.obj"
	-@erase "$(INTDIR)\XLinkedit.obj"
	-@erase "$(INTDIR)\Xml2thot.obj"
	-@erase "$(INTDIR)\XMLAPP.obj"
	-@erase "$(INTDIR)\Xmlbuilder.obj"
	-@erase "$(INTDIR)\XPointer.obj"
	-@erase "$(INTDIR)\XPointerparse.obj"
	-@erase "$(OUTDIR)\amaya.exe"
	-@erase "$(OUTDIR)\amaya.ilk"
	-@erase "$(OUTDIR)\amaya.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /ZI /Od /I "..\..\..\libwww\modules\expat\xmlparse" /I "..\..\..\libwww\modules\expat\xmltok" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\f" /I "..\annotlib" /I "..\annotlib\f" /D "_DEBUG" /D "EXPAT_PARSER" /D "XML_DTD" /D "XML_NS" /D "_SVG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "THOT_TOOLTIPS" /D "ANNOTATIONS" /D "XML_GENERIC" /D "ANNOT_ON_ANNOT" /Fp"$(INTDIR)\amaya.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\amaya.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\amaya.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib wsock32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib winspool.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\amaya.pdb" /debug /machine:I386 /out:"$(OUTDIR)\amaya.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AHTBridge.obj" \
	"$(INTDIR)\AHTFWrite.obj" \
	"$(INTDIR)\AHTMemConv.obj" \
	"$(INTDIR)\AHTURLTools.obj" \
	"$(INTDIR)\answer.obj" \
	"$(INTDIR)\css.obj" \
	"$(INTDIR)\EDITimage.obj" \
	"$(INTDIR)\EDITORactions.obj" \
	"$(INTDIR)\EDITORAPP.obj" \
	"$(INTDIR)\EDITstyle.obj" \
	"$(INTDIR)\fetchHTMLname.obj" \
	"$(INTDIR)\fetchXMLname.obj" \
	"$(INTDIR)\html2thot.obj" \
	"$(INTDIR)\HTMLactions.obj" \
	"$(INTDIR)\HTMLAPP.obj" \
	"$(INTDIR)\HTMLbook.obj" \
	"$(INTDIR)\HTMLedit.obj" \
	"$(INTDIR)\HTMLform.obj" \
	"$(INTDIR)\HTMLhistory.obj" \
	"$(INTDIR)\HTMLimage.obj" \
	"$(INTDIR)\HTMLpresentation.obj" \
	"$(INTDIR)\HTMLsave.obj" \
	"$(INTDIR)\HTMLtable.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\Mathedit.obj" \
	"$(INTDIR)\MathMLAPP.obj" \
	"$(INTDIR)\MathMLbuilder.obj" \
	"$(INTDIR)\MENUconf.obj" \
	"$(INTDIR)\query.obj" \
	"$(INTDIR)\styleparser.obj" \
	"$(INTDIR)\SVGAPP.obj" \
	"$(INTDIR)\SVGbuilder.obj" \
	"$(INTDIR)\SVGedit.obj" \
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\TextFileAPP.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\transparse.obj" \
	"$(INTDIR)\UIcss.obj" \
	"$(INTDIR)\windialogapi.obj" \
	"$(INTDIR)\XHTMLbuilder.obj" \
	"$(INTDIR)\XLinkAPP.obj" \
	"$(INTDIR)\XLinkbuilder.obj" \
	"$(INTDIR)\XLinkedit.obj" \
	"$(INTDIR)\Xml2thot.obj" \
	"$(INTDIR)\XMLAPP.obj" \
	"$(INTDIR)\Xmlbuilder.obj" \
	"$(INTDIR)\XPointer.obj" \
	"$(INTDIR)\XPointerparse.obj" \
	"$(INTDIR)\amaya.res" \
	".\libThotEditor.lib" \
	".\libwww.lib" \
	"$(OUTDIR)\thotprinter.lib" \
	".\libjpeg.lib" \
	".\libpng.lib" \
	".\libThotTable.lib" \
	".\annotlib.lib"

"$(OUTDIR)\amaya.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("amaya.dep")
!INCLUDE "amaya.dep"
!ELSE 
!MESSAGE Warning: cannot find "amaya.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "amaya - Win32 Release" || "$(CFG)" == "amaya - Win32 Debug"

!IF  "$(CFG)" == "amaya - Win32 Release"

"libThotEditor - Win32 Release" : 
   cd ".\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Release" 
   cd ".."

"libThotEditor - Win32 ReleaseCLEAN" : 
   cd ".\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"libThotEditor - Win32 Debug" : 
   cd ".\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Debug" 
   cd ".."

"libThotEditor - Win32 DebugCLEAN" : 
   cd ".\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "amaya - Win32 Release"

"libwww - Win32 Release" : 
   cd ".\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Release" 
   cd ".."

"libwww - Win32 ReleaseCLEAN" : 
   cd ".\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"libwww - Win32 Debug" : 
   cd ".\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Debug" 
   cd ".."

"libwww - Win32 DebugCLEAN" : 
   cd ".\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "amaya - Win32 Release"

"thotprinter - Win32 Release" : 
   cd ".\thotprinter"
   $(MAKE) /$(MAKEFLAGS) /F .\thotprinter.mak CFG="thotprinter - Win32 Release" 
   cd ".."

"thotprinter - Win32 ReleaseCLEAN" : 
   cd ".\thotprinter"
   $(MAKE) /$(MAKEFLAGS) /F .\thotprinter.mak CFG="thotprinter - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"thotprinter - Win32 Debug" : 
   cd ".\thotprinter"
   $(MAKE) /$(MAKEFLAGS) /F .\thotprinter.mak CFG="thotprinter - Win32 Debug" 
   cd ".."

"thotprinter - Win32 DebugCLEAN" : 
   cd ".\thotprinter"
   $(MAKE) /$(MAKEFLAGS) /F .\thotprinter.mak CFG="thotprinter - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "amaya - Win32 Release"

"libjpeg - Win32 Release" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Release" 
   cd ".."

"libjpeg - Win32 ReleaseCLEAN" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"libjpeg - Win32 Debug" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Debug" 
   cd ".."

"libjpeg - Win32 DebugCLEAN" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "amaya - Win32 Release"

"libpng - Win32 Release" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Release" 
   cd ".."

"libpng - Win32 ReleaseCLEAN" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"libpng - Win32 Debug" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Debug" 
   cd ".."

"libpng - Win32 DebugCLEAN" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "amaya - Win32 Release"

"libThotTable - Win32 Release" : 
   cd ".\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Release" 
   cd ".."

"libThotTable - Win32 ReleaseCLEAN" : 
   cd ".\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"libThotTable - Win32 Debug" : 
   cd ".\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Debug" 
   cd ".."

"libThotTable - Win32 DebugCLEAN" : 
   cd ".\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "amaya - Win32 Release"

"annotlib - Win32 Release" : 
   cd ".\annotlib"
   $(MAKE) /$(MAKEFLAGS) /F .\annotlib.mak CFG="annotlib - Win32 Release" 
   cd ".."

"annotlib - Win32 ReleaseCLEAN" : 
   cd ".\annotlib"
   $(MAKE) /$(MAKEFLAGS) /F .\annotlib.mak CFG="annotlib - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"annotlib - Win32 Debug" : 
   cd ".\annotlib"
   $(MAKE) /$(MAKEFLAGS) /F .\annotlib.mak CFG="annotlib - Win32 Debug" 
   cd ".."

"annotlib - Win32 DebugCLEAN" : 
   cd ".\annotlib"
   $(MAKE) /$(MAKEFLAGS) /F .\annotlib.mak CFG="annotlib - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "amaya - Win32 Release"

"Compilers - Win32 Release" : 
   cd ".\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Release" 
   cd ".."

"Compilers - Win32 ReleaseCLEAN" : 
   cd ".\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"Compilers - Win32 Debug" : 
   cd ".\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Debug" 
   cd ".."

"Compilers - Win32 DebugCLEAN" : 
   cd ".\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

SOURCE=..\amaya\AHTBridge.c

"$(INTDIR)\AHTBridge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\AHTFWrite.c

"$(INTDIR)\AHTFWrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\AHTMemConv.c

"$(INTDIR)\AHTMemConv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\AHTURLTools.c

"$(INTDIR)\AHTURLTools.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\amaya.rc

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\amaya.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\amaya.res" /i "\src\Amaya\amaya" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\amaya.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\amaya.res" /i "\src\Amaya\amaya" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\amaya\answer.c

"$(INTDIR)\answer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\css.c

"$(INTDIR)\css.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\EDITimage.c

"$(INTDIR)\EDITimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\EDITORactions.c

"$(INTDIR)\EDITORactions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\EDITORAPP.c

"$(INTDIR)\EDITORAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\EDITstyle.c

"$(INTDIR)\EDITstyle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Amaya\fetchHTMLname.c

"$(INTDIR)\fetchHTMLname.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Amaya\fetchXMLname.c

"$(INTDIR)\fetchXMLname.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\html2thot.c

"$(INTDIR)\html2thot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLactions.c

"$(INTDIR)\HTMLactions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\HTMLAPP.c

"$(INTDIR)\HTMLAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLbook.c

"$(INTDIR)\HTMLbook.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLedit.c

"$(INTDIR)\HTMLedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLform.c

"$(INTDIR)\HTMLform.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLhistory.c

"$(INTDIR)\HTMLhistory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLimage.c

"$(INTDIR)\HTMLimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLpresentation.c

"$(INTDIR)\HTMLpresentation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLsave.c

"$(INTDIR)\HTMLsave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\HTMLtable.c

"$(INTDIR)\HTMLtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\init.c

"$(INTDIR)\init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\Mathedit.c

"$(INTDIR)\Mathedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\MathMLAPP.c

"$(INTDIR)\MathMLAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\MathMLbuilder.c

"$(INTDIR)\MathMLbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\MENUconf.c

"$(INTDIR)\MENUconf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\query.c

"$(INTDIR)\query.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\styleparser.c

"$(INTDIR)\styleparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\SVGAPP.c

"$(INTDIR)\SVGAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\SVGbuilder.c

"$(INTDIR)\SVGbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\SVGedit.c

"$(INTDIR)\SVGedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tablelib\tableH.c

"$(INTDIR)\tableH.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\templates.c

"$(INTDIR)\templates.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\TextFileAPP.c

"$(INTDIR)\TextFileAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\trans.c

"$(INTDIR)\trans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\transparse.c

"$(INTDIR)\transparse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\UIcss.c

"$(INTDIR)\UIcss.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\windialogapi.c

"$(INTDIR)\windialogapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\XHTMLbuilder.c

"$(INTDIR)\XHTMLbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\XLinkAPP.c

"$(INTDIR)\XLinkAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\XLinkbuilder.c

"$(INTDIR)\XLinkbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\XLinkedit.c

"$(INTDIR)\XLinkedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\Xml2thot.c

"$(INTDIR)\Xml2thot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\XMLAPP.c

"$(INTDIR)\XMLAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\Xmlbuilder.c

"$(INTDIR)\Xmlbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\XPointer.c

"$(INTDIR)\XPointer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\XPointerparse.c

"$(INTDIR)\XPointerparse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

