# Microsoft Developer Studio Generated NMAKE File, Based on libThotTable.dsp
!IF "$(CFG)" == ""
CFG=libThotTable - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libThotTable - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libThotTable - Win32 Release" && "$(CFG)" != "libThotTable - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libThotTable.mak" CFG="libThotTable - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libThotTable - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libThotTable - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "libThotTable - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

ALL : "$(OUTDIR)\libThotTable.lib"


CLEAN :
	-@erase "$(INTDIR)\table.obj"
	-@erase "$(INTDIR)\table2.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libThotTable.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\tablelib\f" /I "..\..\schemas" /D "NDEBUG" /D "__STDC__" /D "STDC_HEADERS" /D "_WIN_PRINT" /D "WIN32" /D "_WINDOWS" /D "_AMAYA_RELEASE_" /Fp"$(INTDIR)\libThotTable.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libThotTable.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libThotTable.lib" 
LIB32_OBJS= \
	"$(INTDIR)\table.obj" \
	"$(INTDIR)\table2.obj" \
	"$(INTDIR)\tableH.obj"

"$(OUTDIR)\libThotTable.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libThotTable - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

ALL : "$(OUTDIR)\libThotTable.lib"


CLEAN :
	-@erase "$(INTDIR)\table.obj"
	-@erase "$(INTDIR)\table2.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libThotTable.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\tablelib\f" /I "..\..\schemas" /D "_DEBUG" /D "_AMAYA_RELEASE_" /D "__STDC__" /D "STDC_HEADERS" /D "_WIN_PRINT" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\libThotTable.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libThotTable.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libThotTable.lib" 
LIB32_OBJS= \
	"$(INTDIR)\table.obj" \
	"$(INTDIR)\table2.obj" \
	"$(INTDIR)\tableH.obj"

"$(OUTDIR)\libThotTable.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libThotTable.dep")
!INCLUDE "libThotTable.dep"
!ELSE 
!MESSAGE Warning: cannot find "libThotTable.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libThotTable - Win32 Release" || "$(CFG)" == "libThotTable - Win32 Debug"
SOURCE=..\..\tablelib\table.c

"$(INTDIR)\table.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\tablelib\table2.c

"$(INTDIR)\table2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\tablelib\tableH.c

"$(INTDIR)\tableH.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

