# Microsoft Developer Studio Generated NMAKE File, Based on tra.dsp
!IF "$(CFG)" == ""
CFG=tra - Win32 Debug
!MESSAGE No configuration specified. Defaulting to tra - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tra - Win32 Release" && "$(CFG)" != "tra - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tra.mak" CFG="tra - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tra - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tra - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "tra - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\tra.dll"

!ELSE 

ALL : "LibThotKernel - Win32 Release" "cpp - Win32 Release" "$(OUTDIR)\tra.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"cpp - Win32 ReleaseCLEAN" "LibThotKernel - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\compilmsg.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\tra.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writetra.obj"
	-@erase "$(OUTDIR)\tra.dll"
	-@erase "$(OUTDIR)\tra.exp"
	-@erase "$(OUTDIR)\tra.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\batch\f" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WINDOWS_COMPILERS" /Fp"$(INTDIR)\tra.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tra.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\tra.pdb" /machine:I386 /nodefaultlib:"libcd.lib" /out:"$(OUTDIR)\tra.dll" /implib:"$(OUTDIR)\tra.lib" 
LINK32_OBJS= \
	"$(INTDIR)\compilmsg.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\tra.obj" \
	"$(INTDIR)\writetra.obj" \
	"..\LibThotKernel.lib" \
	"$(OUTDIR)\cpp.lib"

"$(OUTDIR)\tra.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tra - Win32 Debug"

OUTDIR=.\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\tra.dll"

!ELSE 

ALL : "LibThotKernel - Win32 Debug" "cpp - Win32 Debug" "$(OUTDIR)\tra.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"cpp - Win32 DebugCLEAN" "LibThotKernel - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\compilmsg.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\tra.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\writetra.obj"
	-@erase "$(OUTDIR)\tra.dll"
	-@erase "$(OUTDIR)\tra.exp"
	-@erase "$(OUTDIR)\tra.ilk"
	-@erase "$(OUTDIR)\tra.lib"
	-@erase "$(OUTDIR)\tra.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\batch\f" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WINDOWS_COMPILERS" /Fp"$(INTDIR)\tra.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tra.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\tra.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"$(OUTDIR)\tra.dll" /implib:"$(OUTDIR)\tra.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\compilmsg.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\tra.obj" \
	"$(INTDIR)\writetra.obj" \
	"..\LibThotKernel.lib" \
	"$(OUTDIR)\cpp.lib"

"$(OUTDIR)\tra.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("tra.dep")
!INCLUDE "tra.dep"
!ELSE 
!MESSAGE Warning: cannot find "tra.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tra - Win32 Release" || "$(CFG)" == "tra - Win32 Debug"

!IF  "$(CFG)" == "tra - Win32 Release"

"cpp - Win32 Release" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Release" 
   cd "..\tra"

"cpp - Win32 ReleaseCLEAN" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\tra"

!ELSEIF  "$(CFG)" == "tra - Win32 Debug"

"cpp - Win32 Debug" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Debug" 
   cd "..\tra"

"cpp - Win32 DebugCLEAN" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\tra"

!ENDIF 

!IF  "$(CFG)" == "tra - Win32 Release"

"LibThotKernel - Win32 Release" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Release" 
   cd "..\tra"

"LibThotKernel - Win32 ReleaseCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Release" RECURSE=1 CLEAN 
   cd "..\tra"

!ELSEIF  "$(CFG)" == "tra - Win32 Debug"

"LibThotKernel - Win32 Debug" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Debug" 
   cd "..\tra"

"LibThotKernel - Win32 DebugCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak CFG="LibThotKernel - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\tra"

!ENDIF 

SOURCE=..\..\thotlib\base\compilmsg.c

"$(INTDIR)\compilmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\batch\parser.c

"$(INTDIR)\parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\batch\tra.c

"$(INTDIR)\tra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\batch\writetra.c

"$(INTDIR)\writetra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

