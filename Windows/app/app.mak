# Microsoft Developer Studio Generated NMAKE File, Based on app.dsp
!IF "$(CFG)" == ""
CFG=app - Win32 Debug
!MESSAGE No configuration specified. Defaulting to app - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "app - Win32 Release" && "$(CFG)" != "app - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app.mak" CFG="app - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "app - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "app - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\app.dll"

!ELSE 

ALL : "LibThotKernel - Win32 Release" "cpp - Win32 Release" "$(OUTDIR)\app.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"cpp - Win32 ReleaseCLEAN" "LibThotKernel - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\callbackinit.obj"
	-@erase "$(INTDIR)\compilmsg.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writeapp.obj"
	-@erase "$(OUTDIR)\app.dll"
	-@erase "$(OUTDIR)\app.exp"
	-@erase "$(OUTDIR)\app.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\batch\f" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "WINDOWS_COMPILERS" /Fp"$(INTDIR)\app.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\app.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\app.pdb" /machine:I386 /out:"$(OUTDIR)\app.dll" /implib:"$(OUTDIR)\app.lib" 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\callbackinit.obj" \
	"$(INTDIR)\compilmsg.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\writeapp.obj" \
	"..\LibThotKernel.lib" \
	"$(OUTDIR)\cpp.lib"

"$(OUTDIR)\app.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "app - Win32 Debug"

OUTDIR=.\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\app.dll"

!ELSE 

ALL : "LibThotKernel - Win32 Debug" "cpp - Win32 Debug" "$(OUTDIR)\app.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"cpp - Win32 DebugCLEAN" "LibThotKernel - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\callbackinit.obj"
	-@erase "$(INTDIR)\compilmsg.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\writeapp.obj"
	-@erase "$(OUTDIR)\app.dll"
	-@erase "$(OUTDIR)\app.exp"
	-@erase "$(OUTDIR)\app.ilk"
	-@erase "$(OUTDIR)\app.lib"
	-@erase "$(OUTDIR)\app.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\batch\f" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "WINDOWS_COMPILERS" /Fp"$(INTDIR)\app.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\app.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\app.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"$(OUTDIR)\app.dll" /implib:"$(OUTDIR)\app.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\callbackinit.obj" \
	"$(INTDIR)\compilmsg.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\writeapp.obj" \
	"..\LibThotKernel.lib" \
	"$(OUTDIR)\cpp.lib"

"$(OUTDIR)\app.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("app.dep")
!INCLUDE "app.dep"
!ELSE 
!MESSAGE Warning: cannot find "app.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "app - Win32 Release" || "$(CFG)" == "app - Win32 Debug"

!IF  "$(CFG)" == "app - Win32 Release"

"cpp - Win32 Release" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Release" 
   cd "..\app"

"cpp - Win32 ReleaseCLEAN" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\app"

!ELSEIF  "$(CFG)" == "app - Win32 Debug"

"cpp - Win32 Debug" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Debug" 
   cd "..\app"

"cpp - Win32 DebugCLEAN" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\app"

!ENDIF 

!IF  "$(CFG)" == "app - Win32 Release"

"LibThotKernel - Win32 Release" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Release" 
   cd "..\app"

"LibThotKernel - Win32 ReleaseCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Release" RECURSE=1 CLEAN 
   cd "..\app"

!ELSEIF  "$(CFG)" == "app - Win32 Debug"

"LibThotKernel - Win32 Debug" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Debug" 
   cd "..\app"

"LibThotKernel - Win32 DebugCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\app"

!ENDIF 

SOURCE=..\..\batch\app.c

"$(INTDIR)\app.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\callbackinit.c

"$(INTDIR)\callbackinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\compilmsg.c

"$(INTDIR)\compilmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\batch\parser.c

"$(INTDIR)\parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\batch\writeapp.c

"$(INTDIR)\writeapp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

