# Microsoft Developer Studio Generated NMAKE File, Based on cpp.dsp
!IF "$(CFG)" == ""
CFG=cpp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cpp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cpp - Win32 Release" && "$(CFG)" != "cpp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cpp.mak" CFG="cpp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cpp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cpp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cpp - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\cpp___Wi
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

ALL : "$(OUTDIR)\cpp.dll"


CLEAN :
	-@erase "$(INTDIR)\cppalloc.obj"
	-@erase "$(INTDIR)\cpperror.obj"
	-@erase "$(INTDIR)\cppexp.obj"
	-@erase "$(INTDIR)\cpphash.obj"
	-@erase "$(INTDIR)\cpplib.obj"
	-@erase "$(INTDIR)\cppmain.obj"
	-@erase "$(INTDIR)\obstack.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(OUTDIR)\cpp.dll"
	-@erase "$(OUTDIR)\cpp.exp"
	-@erase "$(OUTDIR)\cpp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\thotlib\internals\f" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDOWS_COMPILERS" /Fp"$(INTDIR)\cpp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cpp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\cpp.pdb" /machine:I386 /out:"$(OUTDIR)\cpp.dll" /implib:"$(OUTDIR)\cpp.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cppalloc.obj" \
	"$(INTDIR)\cpperror.obj" \
	"$(INTDIR)\cppexp.obj" \
	"$(INTDIR)\cpphash.obj" \
	"$(INTDIR)\cpplib.obj" \
	"$(INTDIR)\cppmain.obj" \
	"$(INTDIR)\obstack.obj" \
	"$(INTDIR)\version.obj"

"$(OUTDIR)\cpp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cpp - Win32 Debug"

OUTDIR=.\..\bin
INTDIR=.\cpp___W0
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

ALL : "$(OUTDIR)\cpp.dll"


CLEAN :
	-@erase "$(INTDIR)\cppalloc.obj"
	-@erase "$(INTDIR)\cpperror.obj"
	-@erase "$(INTDIR)\cppexp.obj"
	-@erase "$(INTDIR)\cpphash.obj"
	-@erase "$(INTDIR)\cpplib.obj"
	-@erase "$(INTDIR)\cppmain.obj"
	-@erase "$(INTDIR)\obstack.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(OUTDIR)\cpp.dll"
	-@erase "$(OUTDIR)\cpp.exp"
	-@erase "$(OUTDIR)\cpp.ilk"
	-@erase "$(OUTDIR)\cpp.lib"
	-@erase "$(OUTDIR)\cpp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\thotlib\internals\f" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDOWS_COMPILERS" /Fp"$(INTDIR)\cpp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cpp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cpp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\cpp.dll" /implib:"$(OUTDIR)\cpp.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cppalloc.obj" \
	"$(INTDIR)\cpperror.obj" \
	"$(INTDIR)\cppexp.obj" \
	"$(INTDIR)\cpphash.obj" \
	"$(INTDIR)\cpplib.obj" \
	"$(INTDIR)\cppmain.obj" \
	"$(INTDIR)\obstack.obj" \
	"$(INTDIR)\version.obj"

"$(OUTDIR)\cpp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cpp.dep")
!INCLUDE "cpp.dep"
!ELSE 
!MESSAGE Warning: cannot find "cpp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cpp - Win32 Release" || "$(CFG)" == "cpp - Win32 Debug"
SOURCE=..\..\cpp\cppalloc.c

"$(INTDIR)\cppalloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\cpperror.c

"$(INTDIR)\cpperror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\cppexp.c

"$(INTDIR)\cppexp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\cpphash.c

"$(INTDIR)\cpphash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\cpplib.c

"$(INTDIR)\cpplib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\cppmain.c

"$(INTDIR)\cppmain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\obstack.c

"$(INTDIR)\obstack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\version.c

"$(INTDIR)\version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

