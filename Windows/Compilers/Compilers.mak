# Microsoft Developer Studio Generated NMAKE File, Based on Compilers.dsp
!IF "$(CFG)" == ""
CFG=Compilers - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Compilers - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Compilers - Win32 Release" && "$(CFG)" != "Compilers - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Compilers.mak" CFG="Compilers - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Compilers - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Compilers - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Compilers - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Compilers.exe"

!ELSE 

ALL : "LibThotKernel - Win32 Release" "tra - Win32 Release" "str - Win32 Release" "prs - Win32 Release" "app - Win32 Release" "$(OUTDIR)\Compilers.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"app - Win32 ReleaseCLEAN" "prs - Win32 ReleaseCLEAN" "str - Win32 ReleaseCLEAN" "tra - Win32 ReleaseCLEAN" "LibThotKernel - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\winthotcompilers.obj"
	-@erase "$(OUTDIR)\Compilers.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\f" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WINDOWS_COMPILERS" /Fp"$(INTDIR)\Compilers.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Compilers.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Compilers.pdb" /machine:I386 /out:"$(OUTDIR)\Compilers.exe" 
LINK32_OBJS= \
	"$(INTDIR)\winthotcompilers.obj" \
	"..\LibThotKernel.lib" \
	"$(OUTDIR)\app.lib" \
	"$(OUTDIR)\prs.lib" \
	"$(OUTDIR)\str.lib" \
	"$(OUTDIR)\tra.lib"

"$(OUTDIR)\Compilers.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"

OUTDIR=.\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Compilers.exe" "$(OUTDIR)\Compilers.bsc"

!ELSE 

ALL : "LibThotKernel - Win32 Debug" "tra - Win32 Debug" "str - Win32 Debug" "prs - Win32 Debug" "app - Win32 Debug" "$(OUTDIR)\Compilers.exe" "$(OUTDIR)\Compilers.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"app - Win32 DebugCLEAN" "prs - Win32 DebugCLEAN" "str - Win32 DebugCLEAN" "tra - Win32 DebugCLEAN" "LibThotKernel - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\winthotcompilers.obj"
	-@erase "$(INTDIR)\winthotcompilers.sbr"
	-@erase "$(OUTDIR)\Compilers.bsc"
	-@erase "$(OUTDIR)\Compilers.exe"
	-@erase "$(OUTDIR)\Compilers.ilk"
	-@erase "$(OUTDIR)\Compilers.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\f" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WINDOWS_COMPILERS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Compilers.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Compilers.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\winthotcompilers.sbr"

"$(OUTDIR)\Compilers.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Compilers.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Compilers.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\winthotcompilers.obj" \
	"..\LibThotKernel.lib" \
	"$(OUTDIR)\app.lib" \
	"$(OUTDIR)\prs.lib" \
	"$(OUTDIR)\str.lib" \
	"$(OUTDIR)\tra.lib"

"$(OUTDIR)\Compilers.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Compilers.dep")
!INCLUDE "Compilers.dep"
!ELSE 
!MESSAGE Warning: cannot find "Compilers.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Compilers - Win32 Release" || "$(CFG)" == "Compilers - Win32 Debug"

!IF  "$(CFG)" == "Compilers - Win32 Release"

"app - Win32 Release" : 
   cd "..\app"
   $(MAKE) /$(MAKEFLAGS) /F .\app.mak CFG="app - Win32 Release" 
   cd "..\Compilers"

"app - Win32 ReleaseCLEAN" : 
   cd "..\app"
   $(MAKE) /$(MAKEFLAGS) /F .\app.mak CFG="app - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"

"app - Win32 Debug" : 
   cd "..\app"
   $(MAKE) /$(MAKEFLAGS) /F .\app.mak CFG="app - Win32 Debug" 
   cd "..\Compilers"

"app - Win32 DebugCLEAN" : 
   cd "..\app"
   $(MAKE) /$(MAKEFLAGS) /F .\app.mak CFG="app - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ENDIF 

!IF  "$(CFG)" == "Compilers - Win32 Release"

"prs - Win32 Release" : 
   cd "..\prs"
   $(MAKE) /$(MAKEFLAGS) /F .\prs.mak CFG="prs - Win32 Release" 
   cd "..\Compilers"

"prs - Win32 ReleaseCLEAN" : 
   cd "..\prs"
   $(MAKE) /$(MAKEFLAGS) /F .\prs.mak CFG="prs - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"

"prs - Win32 Debug" : 
   cd "..\prs"
   $(MAKE) /$(MAKEFLAGS) /F .\prs.mak CFG="prs - Win32 Debug" 
   cd "..\Compilers"

"prs - Win32 DebugCLEAN" : 
   cd "..\prs"
   $(MAKE) /$(MAKEFLAGS) /F .\prs.mak CFG="prs - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ENDIF 

!IF  "$(CFG)" == "Compilers - Win32 Release"

"str - Win32 Release" : 
   cd "..\str"
   $(MAKE) /$(MAKEFLAGS) /F .\str.mak CFG="str - Win32 Release" 
   cd "..\Compilers"

"str - Win32 ReleaseCLEAN" : 
   cd "..\str"
   $(MAKE) /$(MAKEFLAGS) /F .\str.mak CFG="str - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"

"str - Win32 Debug" : 
   cd "..\str"
   $(MAKE) /$(MAKEFLAGS) /F .\str.mak CFG="str - Win32 Debug" 
   cd "..\Compilers"

"str - Win32 DebugCLEAN" : 
   cd "..\str"
   $(MAKE) /$(MAKEFLAGS) /F .\str.mak CFG="str - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ENDIF 

!IF  "$(CFG)" == "Compilers - Win32 Release"

"tra - Win32 Release" : 
   cd "..\tra"
   $(MAKE) /$(MAKEFLAGS) /F .\tra.mak CFG="tra - Win32 Release" 
   cd "..\Compilers"

"tra - Win32 ReleaseCLEAN" : 
   cd "..\tra"
   $(MAKE) /$(MAKEFLAGS) /F .\tra.mak CFG="tra - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"

"tra - Win32 Debug" : 
   cd "..\tra"
   $(MAKE) /$(MAKEFLAGS) /F .\tra.mak CFG="tra - Win32 Debug" 
   cd "..\Compilers"

"tra - Win32 DebugCLEAN" : 
   cd "..\tra"
   $(MAKE) /$(MAKEFLAGS) /F .\tra.mak CFG="tra - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ENDIF 

!IF  "$(CFG)" == "Compilers - Win32 Release"

"LibThotKernel - Win32 Release" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Release" 
   cd "..\Compilers"

"LibThotKernel - Win32 ReleaseCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"

"LibThotKernel - Win32 Debug" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Debug" 
   cd "..\Compilers"

"LibThotKernel - Win32 DebugCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Compilers"

!ENDIF 

SOURCE=..\..\batch\winthotcompilers.c

!IF  "$(CFG)" == "Compilers - Win32 Release"


"$(INTDIR)\winthotcompilers.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"


"$(INTDIR)\winthotcompilers.obj"	"$(INTDIR)\winthotcompilers.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

