# Microsoft Developer Studio Generated NMAKE File, Based on thotprinter.dsp
!IF "$(CFG)" == ""
CFG=thotprinter - Win32 Debug
!MESSAGE No configuration specified. Defaulting to thotprinter - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "thotprinter - Win32 Release" && "$(CFG)" != "thotprinter - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "thotprinter.mak" CFG="thotprinter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "thotprinter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "thotprinter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "thotprinter - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\thotprinter.dll"

!ELSE 

ALL : "printlib - Win32 Release" "libThotTable - Win32 Release" "$(OUTDIR)\thotprinter.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotTable - Win32 ReleaseCLEAN" "printlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\css.obj"
	-@erase "$(INTDIR)\fetchHTMLname.obj"
	-@erase "$(INTDIR)\fetchXMLname.obj"
	-@erase "$(INTDIR)\nodialog.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\psdisplay.obj"
	-@erase "$(INTDIR)\styleparser.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\thotprinter.dll"
	-@erase "$(OUTDIR)\thotprinter.exp"
	-@erase "$(OUTDIR)\thotprinter.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\tablelib\f" /I "..\..\thotlib\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS" /Fp"$(INTDIR)\thotprinter.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\thotprinter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\thotprinter.pdb" /machine:I386 /out:"$(OUTDIR)\thotprinter.dll" /implib:"$(OUTDIR)\thotprinter.lib" 
LINK32_OBJS= \
	"$(INTDIR)\css.obj" \
	"$(INTDIR)\fetchHTMLname.obj" \
	"$(INTDIR)\fetchXMLname.obj" \
	"$(INTDIR)\nodialog.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\psdisplay.obj" \
	"$(INTDIR)\styleparser.obj" \
	"..\printlib.lib" \
	"..\libThotTable.lib"

"$(OUTDIR)\thotprinter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

OUTDIR=.\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\thotprinter.dll"

!ELSE 

ALL : "printlib - Win32 Debug" "libThotTable - Win32 Debug" "$(OUTDIR)\thotprinter.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotTable - Win32 DebugCLEAN" "printlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\css.obj"
	-@erase "$(INTDIR)\fetchHTMLname.obj"
	-@erase "$(INTDIR)\fetchXMLname.obj"
	-@erase "$(INTDIR)\nodialog.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\psdisplay.obj"
	-@erase "$(INTDIR)\styleparser.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\thotprinter.dll"
	-@erase "$(OUTDIR)\thotprinter.exp"
	-@erase "$(OUTDIR)\thotprinter.ilk"
	-@erase "$(OUTDIR)\thotprinter.lib"
	-@erase "$(OUTDIR)\thotprinter.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\tablelib\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /D "_DEBUG" /D "_AMAYA_RELEASE_" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS" /Fp"$(INTDIR)\thotprinter.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\thotprinter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\thotprinter.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"$(OUTDIR)\thotprinter.dll" /implib:"$(OUTDIR)\thotprinter.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\css.obj" \
	"$(INTDIR)\fetchHTMLname.obj" \
	"$(INTDIR)\fetchXMLname.obj" \
	"$(INTDIR)\nodialog.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\psdisplay.obj" \
	"$(INTDIR)\styleparser.obj" \
	"..\printlib.lib" \
	"..\libThotTable.lib"

"$(OUTDIR)\thotprinter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("thotprinter.dep")
!INCLUDE "thotprinter.dep"
!ELSE 
!MESSAGE Warning: cannot find "thotprinter.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "thotprinter - Win32 Release" || "$(CFG)" == "thotprinter - Win32 Debug"

!IF  "$(CFG)" == "thotprinter - Win32 Release"

"libThotTable - Win32 Release" : 
   cd "\users\guetari\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Release" 
   cd "..\thotprinter"

"libThotTable - Win32 ReleaseCLEAN" : 
   cd "\users\guetari\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Release" RECURSE=1 CLEAN 
   cd "..\thotprinter"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

"libThotTable - Win32 Debug" : 
   cd "\users\guetari\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Debug" 
   cd "..\thotprinter"

"libThotTable - Win32 DebugCLEAN" : 
   cd "\users\guetari\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\thotprinter"

!ENDIF 

!IF  "$(CFG)" == "thotprinter - Win32 Release"

"printlib - Win32 Release" : 
   cd "\users\guetari\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Release" 
   cd "..\thotprinter"

"printlib - Win32 ReleaseCLEAN" : 
   cd "\users\guetari\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\thotprinter"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

"printlib - Win32 Debug" : 
   cd "\users\guetari\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Debug" 
   cd "..\thotprinter"

"printlib - Win32 DebugCLEAN" : 
   cd "\users\guetari\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\thotprinter"

!ENDIF 

SOURCE=..\..\Amaya\css.c

"$(INTDIR)\css.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Amaya\fetchHTMLname.c

"$(INTDIR)\fetchHTMLname.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Amaya\fetchXMLname.c

"$(INTDIR)\fetchXMLname.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\nodialog.c

"$(INTDIR)\nodialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\paginate.c

"$(INTDIR)\paginate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\print.c

"$(INTDIR)\print.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\psdisplay.c

"$(INTDIR)\psdisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Amaya\styleparser.c

"$(INTDIR)\styleparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

