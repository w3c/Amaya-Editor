# Microsoft Developer Studio Generated NMAKE File, Based on amaya.dsp
!IF "$(CFG)" == ""
CFG=AMAYA - WIN32 DEBUG
!MESSAGE No configuration specified. Defaulting to AMAYA - WIN32 DEBUG.
!ENDIF 

!IF "$(CFG)" != "amaya - Win32 Release" && "$(CFG)" != "amaya - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "amaya.mak" CFG="AMAYA - WIN32 DEBUG"
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

!IF  "$(CFG)" == "amaya - Win32 Release"

OUTDIR=.\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\amaya.exe"

!ELSE 

ALL : "davlib - Win32 Release" "Compilers - Win32 Release" "annotlib - Win32 Release" "libThotTable - Win32 Release" "libpng - Win32 Release" "libjpeg - Win32 Release" "thotprinter - Win32 Release" "libwww - Win32 Release" "libThotEditor - Win32 Release" "$(OUTDIR)\amaya.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 ReleaseCLEAN" "libwww - Win32 ReleaseCLEAN" "thotprinter - Win32 ReleaseCLEAN" "libjpeg - Win32 ReleaseCLEAN" "libpng - Win32 ReleaseCLEAN" "libThotTable - Win32 ReleaseCLEAN" "annotlib - Win32 ReleaseCLEAN" "Compilers - Win32 ReleaseCLEAN" "davlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AHTBridge.obj"
	-@erase "$(INTDIR)\AHTFWrite.obj"
	-@erase "$(INTDIR)\AHTMemConv.obj"
	-@erase "$(INTDIR)\AHTURLTools.obj"
	-@erase "$(INTDIR)\amaya.res"
	-@erase "$(INTDIR)\anim.obj"
	-@erase "$(INTDIR)\animbuilder.obj"
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
	-@erase "$(INTDIR)\libmanag.obj"
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
	-@erase "$(INTDIR)\TimelineAPP.obj"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\transparse.obj"
	-@erase "$(INTDIR)\UIcss.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\windialogapi.obj"
	-@erase "$(INTDIR)\windowdisplay.obj"
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

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\davlib" /I "..\davlib\f" /I "..\davlib\tree\h" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\f" /I "..\annotlib" /I "..\annotlib\f" /I "..\davlib\h" /D "NDEBUG" /D "XML_DTD" /D "XML_NS" /D "_SVG" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "THOT_TOOLTIPS" /D "ANNOTATIONS" /D "XML_GENERIC" /D "_I18N_" /D "WIN32" /D "_WINDOWS" /D "DAV" /Fp"$(INTDIR)\amaya.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL" 
RSC=rc.exe
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
	"$(INTDIR)\anim.obj" \
	"$(INTDIR)\animbuilder.obj" \
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
	"$(INTDIR)\libmanag.obj" \
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
	"$(INTDIR)\TimelineAPP.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\transparse.obj" \
	"$(INTDIR)\UIcss.obj" \
	"$(INTDIR)\windialogapi.obj" \
	"$(INTDIR)\windowdisplay.obj" \
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
	".\libraptor.lib" \
	"$(OUTDIR)\thotprinter.lib" \
	".\libjpeg.lib" \
	".\libpng.lib" \
	".\libThotTable.lib" \
	".\annotlib.lib" \
	".\davlib\Release\davlib.lib"

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

ALL : "$(OUTDIR)\amaya.exe" "$(OUTDIR)\amaya.bsc"

!ELSE 

ALL : "davlib - Win32 Debug" "Compilers - Win32 Debug" "annotlib - Win32 Debug" "libThotTable - Win32 Debug" "libpng - Win32 Debug" "libjpeg - Win32 Debug" "thotprinter - Win32 Debug" "libwww - Win32 Debug" "libThotEditor - Win32 Debug" "$(OUTDIR)\amaya.exe" "$(OUTDIR)\amaya.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 DebugCLEAN" "libwww - Win32 DebugCLEAN" "thotprinter - Win32 DebugCLEAN" "libjpeg - Win32 DebugCLEAN" "libpng - Win32 DebugCLEAN" "libThotTable - Win32 DebugCLEAN" "annotlib - Win32 DebugCLEAN" "Compilers - Win32 DebugCLEAN" "davlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AHTBridge.obj"
	-@erase "$(INTDIR)\AHTBridge.sbr"
	-@erase "$(INTDIR)\AHTFWrite.obj"
	-@erase "$(INTDIR)\AHTFWrite.sbr"
	-@erase "$(INTDIR)\AHTMemConv.obj"
	-@erase "$(INTDIR)\AHTMemConv.sbr"
	-@erase "$(INTDIR)\AHTURLTools.obj"
	-@erase "$(INTDIR)\AHTURLTools.sbr"
	-@erase "$(INTDIR)\amaya.res"
	-@erase "$(INTDIR)\anim.obj"
	-@erase "$(INTDIR)\anim.sbr"
	-@erase "$(INTDIR)\animbuilder.obj"
	-@erase "$(INTDIR)\animbuilder.sbr"
	-@erase "$(INTDIR)\answer.obj"
	-@erase "$(INTDIR)\answer.sbr"
	-@erase "$(INTDIR)\css.obj"
	-@erase "$(INTDIR)\css.sbr"
	-@erase "$(INTDIR)\EDITimage.obj"
	-@erase "$(INTDIR)\EDITimage.sbr"
	-@erase "$(INTDIR)\EDITORactions.obj"
	-@erase "$(INTDIR)\EDITORactions.sbr"
	-@erase "$(INTDIR)\EDITORAPP.obj"
	-@erase "$(INTDIR)\EDITORAPP.sbr"
	-@erase "$(INTDIR)\EDITstyle.obj"
	-@erase "$(INTDIR)\EDITstyle.sbr"
	-@erase "$(INTDIR)\fetchHTMLname.obj"
	-@erase "$(INTDIR)\fetchHTMLname.sbr"
	-@erase "$(INTDIR)\fetchXMLname.obj"
	-@erase "$(INTDIR)\fetchXMLname.sbr"
	-@erase "$(INTDIR)\html2thot.obj"
	-@erase "$(INTDIR)\html2thot.sbr"
	-@erase "$(INTDIR)\HTMLactions.obj"
	-@erase "$(INTDIR)\HTMLactions.sbr"
	-@erase "$(INTDIR)\HTMLAPP.obj"
	-@erase "$(INTDIR)\HTMLAPP.sbr"
	-@erase "$(INTDIR)\HTMLbook.obj"
	-@erase "$(INTDIR)\HTMLbook.sbr"
	-@erase "$(INTDIR)\HTMLedit.obj"
	-@erase "$(INTDIR)\HTMLedit.sbr"
	-@erase "$(INTDIR)\HTMLform.obj"
	-@erase "$(INTDIR)\HTMLform.sbr"
	-@erase "$(INTDIR)\HTMLhistory.obj"
	-@erase "$(INTDIR)\HTMLhistory.sbr"
	-@erase "$(INTDIR)\HTMLimage.obj"
	-@erase "$(INTDIR)\HTMLimage.sbr"
	-@erase "$(INTDIR)\HTMLpresentation.obj"
	-@erase "$(INTDIR)\HTMLpresentation.sbr"
	-@erase "$(INTDIR)\HTMLsave.obj"
	-@erase "$(INTDIR)\HTMLsave.sbr"
	-@erase "$(INTDIR)\HTMLtable.obj"
	-@erase "$(INTDIR)\HTMLtable.sbr"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\init.sbr"
	-@erase "$(INTDIR)\libmanag.obj"
	-@erase "$(INTDIR)\libmanag.sbr"
	-@erase "$(INTDIR)\Mathedit.obj"
	-@erase "$(INTDIR)\Mathedit.sbr"
	-@erase "$(INTDIR)\MathMLAPP.obj"
	-@erase "$(INTDIR)\MathMLAPP.sbr"
	-@erase "$(INTDIR)\MathMLbuilder.obj"
	-@erase "$(INTDIR)\MathMLbuilder.sbr"
	-@erase "$(INTDIR)\MENUconf.obj"
	-@erase "$(INTDIR)\MENUconf.sbr"
	-@erase "$(INTDIR)\query.obj"
	-@erase "$(INTDIR)\query.sbr"
	-@erase "$(INTDIR)\styleparser.obj"
	-@erase "$(INTDIR)\styleparser.sbr"
	-@erase "$(INTDIR)\SVGAPP.obj"
	-@erase "$(INTDIR)\SVGAPP.sbr"
	-@erase "$(INTDIR)\SVGbuilder.obj"
	-@erase "$(INTDIR)\SVGbuilder.sbr"
	-@erase "$(INTDIR)\SVGedit.obj"
	-@erase "$(INTDIR)\SVGedit.sbr"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\tableH.sbr"
	-@erase "$(INTDIR)\templates.obj"
	-@erase "$(INTDIR)\templates.sbr"
	-@erase "$(INTDIR)\TextFileAPP.obj"
	-@erase "$(INTDIR)\TextFileAPP.sbr"
	-@erase "$(INTDIR)\TimelineAPP.obj"
	-@erase "$(INTDIR)\TimelineAPP.sbr"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\trans.sbr"
	-@erase "$(INTDIR)\transparse.obj"
	-@erase "$(INTDIR)\transparse.sbr"
	-@erase "$(INTDIR)\UIcss.obj"
	-@erase "$(INTDIR)\UIcss.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\windialogapi.obj"
	-@erase "$(INTDIR)\windialogapi.sbr"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\windowdisplay.sbr"
	-@erase "$(INTDIR)\XHTMLbuilder.obj"
	-@erase "$(INTDIR)\XHTMLbuilder.sbr"
	-@erase "$(INTDIR)\XLinkAPP.obj"
	-@erase "$(INTDIR)\XLinkAPP.sbr"
	-@erase "$(INTDIR)\XLinkbuilder.obj"
	-@erase "$(INTDIR)\XLinkbuilder.sbr"
	-@erase "$(INTDIR)\XLinkedit.obj"
	-@erase "$(INTDIR)\XLinkedit.sbr"
	-@erase "$(INTDIR)\Xml2thot.obj"
	-@erase "$(INTDIR)\Xml2thot.sbr"
	-@erase "$(INTDIR)\XMLAPP.obj"
	-@erase "$(INTDIR)\XMLAPP.sbr"
	-@erase "$(INTDIR)\Xmlbuilder.obj"
	-@erase "$(INTDIR)\Xmlbuilder.sbr"
	-@erase "$(INTDIR)\XPointer.obj"
	-@erase "$(INTDIR)\XPointer.sbr"
	-@erase "$(INTDIR)\XPointerparse.obj"
	-@erase "$(INTDIR)\XPointerparse.sbr"
	-@erase "$(OUTDIR)\amaya.bsc"
	-@erase "$(OUTDIR)\amaya.exe"
	-@erase "$(OUTDIR)\amaya.ilk"
	-@erase "$(OUTDIR)\amaya.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /ZI /Od /I "..\..\..\libwww\modules\expat\xmlparse" /I "..\..\..\libwww\modules\expat\xmltok" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\f" /I "..\annotlib" /I "..\annotlib\f" /I "..\davlib\h" /I "..\davlib\f" /I "..\davlib\tree\h" /D "EXPAT_PARSER" /D "ANNOT_ON_ANNOT" /D "XML_DTD" /D "XML_NS" /D "_SVG" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "THOT_TOOLTIPS" /D "ANNOTATIONS" /D "XML_GENERIC" /D "_I18N_" /D "DAV" /D "_SVGLIB" /D "_SVGANIM" /D "_FONTCONFIG" /D "_STIX" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\amaya.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL" 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\amaya.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\amaya.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AHTBridge.sbr" \
	"$(INTDIR)\AHTFWrite.sbr" \
	"$(INTDIR)\AHTMemConv.sbr" \
	"$(INTDIR)\AHTURLTools.sbr" \
	"$(INTDIR)\anim.sbr" \
	"$(INTDIR)\animbuilder.sbr" \
	"$(INTDIR)\answer.sbr" \
	"$(INTDIR)\css.sbr" \
	"$(INTDIR)\EDITimage.sbr" \
	"$(INTDIR)\EDITORactions.sbr" \
	"$(INTDIR)\EDITORAPP.sbr" \
	"$(INTDIR)\EDITstyle.sbr" \
	"$(INTDIR)\fetchHTMLname.sbr" \
	"$(INTDIR)\fetchXMLname.sbr" \
	"$(INTDIR)\html2thot.sbr" \
	"$(INTDIR)\HTMLactions.sbr" \
	"$(INTDIR)\HTMLAPP.sbr" \
	"$(INTDIR)\HTMLbook.sbr" \
	"$(INTDIR)\HTMLedit.sbr" \
	"$(INTDIR)\HTMLform.sbr" \
	"$(INTDIR)\HTMLhistory.sbr" \
	"$(INTDIR)\HTMLimage.sbr" \
	"$(INTDIR)\HTMLpresentation.sbr" \
	"$(INTDIR)\HTMLsave.sbr" \
	"$(INTDIR)\HTMLtable.sbr" \
	"$(INTDIR)\init.sbr" \
	"$(INTDIR)\libmanag.sbr" \
	"$(INTDIR)\Mathedit.sbr" \
	"$(INTDIR)\MathMLAPP.sbr" \
	"$(INTDIR)\MathMLbuilder.sbr" \
	"$(INTDIR)\MENUconf.sbr" \
	"$(INTDIR)\query.sbr" \
	"$(INTDIR)\styleparser.sbr" \
	"$(INTDIR)\SVGAPP.sbr" \
	"$(INTDIR)\SVGbuilder.sbr" \
	"$(INTDIR)\SVGedit.sbr" \
	"$(INTDIR)\tableH.sbr" \
	"$(INTDIR)\templates.sbr" \
	"$(INTDIR)\TextFileAPP.sbr" \
	"$(INTDIR)\TimelineAPP.sbr" \
	"$(INTDIR)\trans.sbr" \
	"$(INTDIR)\transparse.sbr" \
	"$(INTDIR)\UIcss.sbr" \
	"$(INTDIR)\windialogapi.sbr" \
	"$(INTDIR)\windowdisplay.sbr" \
	"$(INTDIR)\XHTMLbuilder.sbr" \
	"$(INTDIR)\XLinkAPP.sbr" \
	"$(INTDIR)\XLinkbuilder.sbr" \
	"$(INTDIR)\XLinkedit.sbr" \
	"$(INTDIR)\Xml2thot.sbr" \
	"$(INTDIR)\XMLAPP.sbr" \
	"$(INTDIR)\Xmlbuilder.sbr" \
	"$(INTDIR)\XPointer.sbr" \
	"$(INTDIR)\XPointerparse.sbr"

"$(OUTDIR)\amaya.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=comctl32.lib wsock32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib winspool.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\amaya.pdb" /debug /machine:I386 /out:"$(OUTDIR)\amaya.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AHTBridge.obj" \
	"$(INTDIR)\AHTFWrite.obj" \
	"$(INTDIR)\AHTMemConv.obj" \
	"$(INTDIR)\AHTURLTools.obj" \
	"$(INTDIR)\anim.obj" \
	"$(INTDIR)\animbuilder.obj" \
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
	"$(INTDIR)\libmanag.obj" \
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
	"$(INTDIR)\TimelineAPP.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\transparse.obj" \
	"$(INTDIR)\UIcss.obj" \
	"$(INTDIR)\windialogapi.obj" \
	"$(INTDIR)\windowdisplay.obj" \
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
	".\libraptor.lib" \
	"$(OUTDIR)\thotprinter.lib" \
	".\libjpeg.lib" \
	".\libpng.lib" \
	".\libThotTable.lib" \
	".\annotlib.lib" \
	".\davlib\Debug\davlib.lib"

"$(OUTDIR)\amaya.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


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

!IF  "$(CFG)" == "amaya - Win32 Release"

"davlib - Win32 Release" : 
   cd ".\davlib"
   $(MAKE) /$(MAKEFLAGS) /F .\davlib.mak CFG="davlib - Win32 Release" 
   cd ".."

"davlib - Win32 ReleaseCLEAN" : 
   cd ".\davlib"
   $(MAKE) /$(MAKEFLAGS) /F .\davlib.mak CFG="davlib - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

"davlib - Win32 Debug" : 
   cd ".\davlib"
   $(MAKE) /$(MAKEFLAGS) /F .\davlib.mak CFG="davlib - Win32 Debug" 
   cd ".."

"davlib - Win32 DebugCLEAN" : 
   cd ".\davlib"
   $(MAKE) /$(MAKEFLAGS) /F .\davlib.mak CFG="davlib - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

SOURCE=..\amaya\AHTBridge.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\AHTBridge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\AHTBridge.obj"	"$(INTDIR)\AHTBridge.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\AHTFWrite.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\AHTFWrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\AHTFWrite.obj"	"$(INTDIR)\AHTFWrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\AHTMemConv.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\AHTMemConv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\AHTMemConv.obj"	"$(INTDIR)\AHTMemConv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\AHTURLTools.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\AHTURLTools.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\AHTURLTools.obj"	"$(INTDIR)\AHTURLTools.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\amaya.rc

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\amaya.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\amaya.res" /i "\src\Amaya\amaya" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\amaya.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\amaya.res" /i "\src\Amaya\amaya" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\amaya\anim.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\anim.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\anim.obj"	"$(INTDIR)\anim.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\animbuilder.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\animbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\animbuilder.obj"	"$(INTDIR)\animbuilder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\answer.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\answer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\answer.obj"	"$(INTDIR)\answer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\css.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\css.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\css.obj"	"$(INTDIR)\css.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\EDITimage.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\EDITimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\EDITimage.obj"	"$(INTDIR)\EDITimage.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\EDITORactions.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\EDITORactions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\EDITORactions.obj"	"$(INTDIR)\EDITORactions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\EDITORAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\EDITORAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\EDITORAPP.obj"	"$(INTDIR)\EDITORAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\EDITstyle.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\EDITstyle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\EDITstyle.obj"	"$(INTDIR)\EDITstyle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Amaya\fetchHTMLname.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\fetchHTMLname.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\fetchHTMLname.obj"	"$(INTDIR)\fetchHTMLname.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Amaya\fetchXMLname.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\fetchXMLname.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\fetchXMLname.obj"	"$(INTDIR)\fetchXMLname.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\html2thot.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\html2thot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\html2thot.obj"	"$(INTDIR)\html2thot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLactions.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLactions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLactions.obj"	"$(INTDIR)\HTMLactions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\HTMLAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLAPP.obj"	"$(INTDIR)\HTMLAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLbook.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLbook.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLbook.obj"	"$(INTDIR)\HTMLbook.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLedit.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLedit.obj"	"$(INTDIR)\HTMLedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLform.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLform.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLform.obj"	"$(INTDIR)\HTMLform.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLhistory.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLhistory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLhistory.obj"	"$(INTDIR)\HTMLhistory.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLimage.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLimage.obj"	"$(INTDIR)\HTMLimage.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLpresentation.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLpresentation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLpresentation.obj"	"$(INTDIR)\HTMLpresentation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLsave.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLsave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLsave.obj"	"$(INTDIR)\HTMLsave.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\HTMLtable.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\HTMLtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\HTMLtable.obj"	"$(INTDIR)\HTMLtable.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\init.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\init.obj"	"$(INTDIR)\init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\libmanag.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\libmanag.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\libmanag.obj"	"$(INTDIR)\libmanag.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\Mathedit.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\Mathedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\Mathedit.obj"	"$(INTDIR)\Mathedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\MathMLAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\MathMLAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\MathMLAPP.obj"	"$(INTDIR)\MathMLAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\MathMLbuilder.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\MathMLbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\MathMLbuilder.obj"	"$(INTDIR)\MathMLbuilder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\MENUconf.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\MENUconf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\MENUconf.obj"	"$(INTDIR)\MENUconf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\query.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\query.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\query.obj"	"$(INTDIR)\query.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\styleparser.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\styleparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\styleparser.obj"	"$(INTDIR)\styleparser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\SVGAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\SVGAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\SVGAPP.obj"	"$(INTDIR)\SVGAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\SVGbuilder.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\SVGbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\SVGbuilder.obj"	"$(INTDIR)\SVGbuilder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\SVGedit.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\SVGedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\SVGedit.obj"	"$(INTDIR)\SVGedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\tablelib\tableH.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\tableH.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\tableH.obj"	"$(INTDIR)\tableH.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\templates.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\templates.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\templates.obj"	"$(INTDIR)\templates.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\TextFileAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\TextFileAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\TextFileAPP.obj"	"$(INTDIR)\TextFileAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\TimelineAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\TimelineAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\TimelineAPP.obj"	"$(INTDIR)\TimelineAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\trans.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\trans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\trans.obj"	"$(INTDIR)\trans.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\transparse.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\transparse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\transparse.obj"	"$(INTDIR)\transparse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\UIcss.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\UIcss.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\UIcss.obj"	"$(INTDIR)\UIcss.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\windialogapi.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\windialogapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\windialogapi.obj"	"$(INTDIR)\windialogapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\view\windowdisplay.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\windowdisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\windowdisplay.obj"	"$(INTDIR)\windowdisplay.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\XHTMLbuilder.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\XHTMLbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\XHTMLbuilder.obj"	"$(INTDIR)\XHTMLbuilder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\XLinkAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\XLinkAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\XLinkAPP.obj"	"$(INTDIR)\XLinkAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\XLinkbuilder.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\XLinkbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\XLinkbuilder.obj"	"$(INTDIR)\XLinkbuilder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\XLinkedit.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\XLinkedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\XLinkedit.obj"	"$(INTDIR)\XLinkedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\Xml2thot.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\Xml2thot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\Xml2thot.obj"	"$(INTDIR)\Xml2thot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\amaya\XMLAPP.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\XMLAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\XMLAPP.obj"	"$(INTDIR)\XMLAPP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\Xmlbuilder.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\Xmlbuilder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\Xmlbuilder.obj"	"$(INTDIR)\Xmlbuilder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\XPointer.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\XPointer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\XPointer.obj"	"$(INTDIR)\XPointer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\XPointerparse.c

!IF  "$(CFG)" == "amaya - Win32 Release"


"$(INTDIR)\XPointerparse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\XPointerparse.obj"	"$(INTDIR)\XPointerparse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

