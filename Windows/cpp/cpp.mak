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

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cpp - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\cpp___Wi
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cpp.dll"

!ELSE 

ALL : "$(OUTDIR)\cpp.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\cppalloc.obj"
	-@erase "$(INTDIR)\cpperror.obj"
	-@erase "$(INTDIR)\cppexp.obj"
	-@erase "$(INTDIR)\cpphash.obj"
	-@erase "$(INTDIR)\cpplib.obj"
	-@erase "$(INTDIR)\cppmain.obj"
	-@erase "$(INTDIR)\obstack.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(OUTDIR)\cpp.dll"
	-@erase "$(OUTDIR)\cpp.exp"
	-@erase "$(OUTDIR)\cpp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\thotlib\internals\f" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "WINDOWS_COMPILERS" /Fp"$(INTDIR)\cpp.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\cpp___Wi/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cpp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\cpp.pdb" /machine:I386 /out:"$(OUTDIR)\cpp.dll"\
 /implib:"$(OUTDIR)\cpp.lib" 
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

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cpp.dll"

!ELSE 

ALL : "$(OUTDIR)\cpp.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\cppalloc.obj"
	-@erase "$(INTDIR)\cpperror.obj"
	-@erase "$(INTDIR)\cppexp.obj"
	-@erase "$(INTDIR)\cpphash.obj"
	-@erase "$(INTDIR)\cpplib.obj"
	-@erase "$(INTDIR)\cppmain.obj"
	-@erase "$(INTDIR)\obstack.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
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

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\thotlib\internals\f" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDOWS_COMPILERS" /Fp"$(INTDIR)\cpp.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\cpp___W0/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cpp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\cpp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\cpp.dll"\
 /implib:"$(OUTDIR)\cpp.lib" /pdbtype:sept 
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

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "cpp - Win32 Release" || "$(CFG)" == "cpp - Win32 Debug"
SOURCE=..\..\cpp\cppalloc.c

!IF  "$(CFG)" == "cpp - Win32 Release"

DEP_CPP_CPPAL=\
	"..\..\cpp\config.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	

"$(INTDIR)\cppalloc.obj" : $(SOURCE) $(DEP_CPP_CPPAL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cpp - Win32 Debug"

DEP_CPP_CPPAL=\
	"..\..\cpp\config.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	

"$(INTDIR)\cppalloc.obj" : $(SOURCE) $(DEP_CPP_CPPAL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\cpp\cpperror.c

!IF  "$(CFG)" == "cpp - Win32 Release"

DEP_CPP_CPPER=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\cpperror.obj" : $(SOURCE) $(DEP_CPP_CPPER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cpp - Win32 Debug"

DEP_CPP_CPPER=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	

"$(INTDIR)\cpperror.obj" : $(SOURCE) $(DEP_CPP_CPPER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\cpp\cppexp.c

!IF  "$(CFG)" == "cpp - Win32 Release"

DEP_CPP_CPPEX=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\cppexp.obj" : $(SOURCE) $(DEP_CPP_CPPEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cpp - Win32 Debug"

DEP_CPP_CPPEX=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	

"$(INTDIR)\cppexp.obj" : $(SOURCE) $(DEP_CPP_CPPEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\cpp\cpphash.c

!IF  "$(CFG)" == "cpp - Win32 Release"

DEP_CPP_CPPHA=\
	"..\..\cpp\cpphash.h"\
	"..\..\cpp\cpplib.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\cpphash.obj" : $(SOURCE) $(DEP_CPP_CPPHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cpp - Win32 Debug"

DEP_CPP_CPPHA=\
	"..\..\cpp\cpphash.h"\
	"..\..\cpp\cpplib.h"\
	

"$(INTDIR)\cpphash.obj" : $(SOURCE) $(DEP_CPP_CPPHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\cpp\cpplib.c

!IF  "$(CFG)" == "cpp - Win32 Release"

DEP_CPP_CPPLI=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpphash.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CPPLI=\
	"..\..\src\config.h"\
	

"$(INTDIR)\cpplib.obj" : $(SOURCE) $(DEP_CPP_CPPLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cpp - Win32 Debug"

DEP_CPP_CPPLI=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpphash.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	

"$(INTDIR)\cpplib.obj" : $(SOURCE) $(DEP_CPP_CPPLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\cpp\cppmain.c

!IF  "$(CFG)" == "cpp - Win32 Release"

DEP_CPP_CPPMA=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	"..\..\thotlib\internals\f\compilers_f.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\cppmain.obj" : $(SOURCE) $(DEP_CPP_CPPMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cpp - Win32 Debug"

DEP_CPP_CPPMA=\
	"..\..\cpp\config.h"\
	"..\..\cpp\cpplib.h"\
	"..\..\cpp\i386\bsd.h"\
	"..\..\cpp\i386\gas.h"\
	"..\..\cpp\i386\i386.h"\
	"..\..\cpp\i386\unix.h"\
	"..\..\cpp\i386\win-nt.h"\
	"..\..\cpp\i386\xm-i386.h"\
	"..\..\cpp\i386\xm-winnt.h"\
	"..\..\cpp\tm.h"\
	"..\..\cpp\winnt\win-nt.h"\
	"..\..\cpp\winnt\xm-winnt.h"\
	"..\..\thotlib\internals\f\compilers_f.h"\
	

"$(INTDIR)\cppmain.obj" : $(SOURCE) $(DEP_CPP_CPPMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\cpp\obstack.c
DEP_CPP_OBSTA=\
	"..\..\cpp\obstack.h"\
	

"$(INTDIR)\obstack.obj" : $(SOURCE) $(DEP_CPP_OBSTA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cpp\version.c

"$(INTDIR)\version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

