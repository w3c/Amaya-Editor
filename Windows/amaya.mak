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

!IF  "$(CFG)" == "amaya - Win32 Release"

OUTDIR=.\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\amaya.exe"

!ELSE 

ALL : "thotprinter - Win32 Release" "libwww - Win32 Release" "libThotEditor - Win32 Release" "$(OUTDIR)\amaya.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 ReleaseCLEAN" "libwww - Win32 ReleaseCLEAN" "thotprinter - Win32 ReleaseCLEAN" 
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
	-@erase "$(INTDIR)\Graphedit.obj"
	-@erase "$(INTDIR)\GraphMLAPP.obj"
	-@erase "$(INTDIR)\GraphMLbuilder.obj"
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
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\templates.obj"
	-@erase "$(INTDIR)\TextFileAPP.obj"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\transparse.obj"
	-@erase "$(INTDIR)\UIcss.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\windialogapi.obj"
	-@erase "$(INTDIR)\XMLparser.obj"
	-@erase "$(OUTDIR)\amaya.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I "..\thotlib\internals\f" /D "NDEBUG" /D "COUGAR" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /D "GRAPHML" /Fp"$(INTDIR)\amaya.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
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
	"$(INTDIR)\answer.obj" \
	"$(INTDIR)\css.obj" \
	"$(INTDIR)\EDITimage.obj" \
	"$(INTDIR)\EDITORactions.obj" \
	"$(INTDIR)\EDITORAPP.obj" \
	"$(INTDIR)\EDITstyle.obj" \
	"$(INTDIR)\fetchHTMLname.obj" \
	"$(INTDIR)\fetchXMLname.obj" \
	"$(INTDIR)\Graphedit.obj" \
	"$(INTDIR)\GraphMLAPP.obj" \
	"$(INTDIR)\GraphMLbuilder.obj" \
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
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\TextFileAPP.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\transparse.obj" \
	"$(INTDIR)\UIcss.obj" \
	"$(INTDIR)\windialogapi.obj" \
	"$(INTDIR)\XMLparser.obj" \
	"$(INTDIR)\amaya.res" \
	".\libThotEditor.lib" \
	".\libwww.lib" \
	".\Plugins.lib" \
	"$(OUTDIR)\thotprinter.lib"

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

ALL : "thotprinter - Win32 Debug" "libwww - Win32 Debug" "libThotEditor - Win32 Debug" "$(OUTDIR)\amaya.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 DebugCLEAN" "libwww - Win32 DebugCLEAN" "thotprinter - Win32 DebugCLEAN" 
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
	-@erase "$(INTDIR)\Graphedit.obj"
	-@erase "$(INTDIR)\GraphMLAPP.obj"
	-@erase "$(INTDIR)\GraphMLbuilder.obj"
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
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\templates.obj"
	-@erase "$(INTDIR)\TextFileAPP.obj"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\transparse.obj"
	-@erase "$(INTDIR)\UIcss.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\windialogapi.obj"
	-@erase "$(INTDIR)\XMLparser.obj"
	-@erase "$(OUTDIR)\amaya.exe"
	-@erase "$(OUTDIR)\amaya.ilk"
	-@erase "$(OUTDIR)\amaya.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\f" /D "_DEBUG" /D "_AMAYA_RELEASE_" /D "GRAPHML" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /Fp"$(INTDIR)\amaya.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
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
	"$(INTDIR)\Graphedit.obj" \
	"$(INTDIR)\GraphMLAPP.obj" \
	"$(INTDIR)\GraphMLbuilder.obj" \
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
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\TextFileAPP.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\transparse.obj" \
	"$(INTDIR)\UIcss.obj" \
	"$(INTDIR)\windialogapi.obj" \
	"$(INTDIR)\XMLparser.obj" \
	"$(INTDIR)\amaya.res" \
	".\libThotEditor.lib" \
	".\libwww.lib" \
	".\Plugins.lib" \
	"$(OUTDIR)\thotprinter.lib"

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
	$(RSC) /l 0x409 /fo"$(INTDIR)\amaya.res" /i "\users\guetari\Amaya\amaya" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"


"$(INTDIR)\amaya.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\amaya.res" /i "\users\guetari\Amaya\amaya" /d "_DEBUG" $(SOURCE)


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


SOURCE=..\amaya\Graphedit.c

"$(INTDIR)\Graphedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\amaya\GraphMLAPP.c

"$(INTDIR)\GraphMLAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\GraphMLbuilder.c

"$(INTDIR)\GraphMLbuilder.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=..\amaya\XMLparser.c

"$(INTDIR)\XMLparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

