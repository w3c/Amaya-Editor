# Microsoft Developer Studio Generated NMAKE File, Based on libpng.dsp
!IF "$(CFG)" == ""
CFG=libpng - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libpng - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "libpng - Win32 Release" && "$(CFG)" != "libpng - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak" CFG="libpng - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libpng - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "libpng - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Libpng.lib"

!ELSE 

ALL : "zlib - Win32 Release" "$(OUTDIR)\Libpng.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\libpng\zlib" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)\libpng.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

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

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libpng.lib" 
LIB32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(OUTDIR)\zlib.lib"

"$(OUTDIR)\Libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Libpng.lib"

!ELSE 

ALL : "zlib - Win32 Debug" "$(OUTDIR)\Libpng.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\libpng\zlib" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\libpng.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

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

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libpng.lib" 
LIB32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(OUTDIR)\zlib.lib"

"$(OUTDIR)\Libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "libpng - Win32 Release" || "$(CFG)" == "libpng - Win32 Debug"
SOURCE=..\..\libpng\png.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNG_C=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNG_C=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\png.obj" : $(SOURCE) $(DEP_CPP_PNG_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNG_C=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\png.obj" : $(SOURCE) $(DEP_CPP_PNG_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngerror.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGER=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGER=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngerror.obj" : $(SOURCE) $(DEP_CPP_PNGER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGER=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngerror.obj" : $(SOURCE) $(DEP_CPP_PNGER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngget.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGGE=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGGE=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngget.obj" : $(SOURCE) $(DEP_CPP_PNGGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGGE=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngget.obj" : $(SOURCE) $(DEP_CPP_PNGGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngmem.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGME=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGME=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngmem.obj" : $(SOURCE) $(DEP_CPP_PNGME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGME=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngmem.obj" : $(SOURCE) $(DEP_CPP_PNGME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngpread.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGPR=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGPR=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngpread.obj" : $(SOURCE) $(DEP_CPP_PNGPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGPR=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngpread.obj" : $(SOURCE) $(DEP_CPP_PNGPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngread.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRE=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRE=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngread.obj" : $(SOURCE) $(DEP_CPP_PNGRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRE=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngread.obj" : $(SOURCE) $(DEP_CPP_PNGRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngrio.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRI=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRI=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngrio.obj" : $(SOURCE) $(DEP_CPP_PNGRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRI=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngrio.obj" : $(SOURCE) $(DEP_CPP_PNGRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngrtran.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRT=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRT=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngrtran.obj" : $(SOURCE) $(DEP_CPP_PNGRT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRT=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngrtran.obj" : $(SOURCE) $(DEP_CPP_PNGRT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngrutil.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRU=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRU=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngrutil.obj" : $(SOURCE) $(DEP_CPP_PNGRU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRU=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngrutil.obj" : $(SOURCE) $(DEP_CPP_PNGRU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngset.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGSE=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGSE=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngset.obj" : $(SOURCE) $(DEP_CPP_PNGSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGSE=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngset.obj" : $(SOURCE) $(DEP_CPP_PNGSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngtrans.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGTR=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGTR=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngtrans.obj" : $(SOURCE) $(DEP_CPP_PNGTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGTR=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngtrans.obj" : $(SOURCE) $(DEP_CPP_PNGTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngwio.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWI=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWI=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngwio.obj" : $(SOURCE) $(DEP_CPP_PNGWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWI=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngwio.obj" : $(SOURCE) $(DEP_CPP_PNGWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngwrite.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWR=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWR=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngwrite.obj" : $(SOURCE) $(DEP_CPP_PNGWR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWR=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngwrite.obj" : $(SOURCE) $(DEP_CPP_PNGWR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngwtran.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWT=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWT=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngwtran.obj" : $(SOURCE) $(DEP_CPP_PNGWT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWT=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngwtran.obj" : $(SOURCE) $(DEP_CPP_PNGWT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\pngwutil.c

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWU=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWU=\
	"..\..\libpng\alloc.h"\
	

"$(INTDIR)\pngwutil.obj" : $(SOURCE) $(DEP_CPP_PNGWU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWU=\
	"..\..\libpng\png.h"\
	"..\..\libpng\pngconf.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\pngwutil.obj" : $(SOURCE) $(DEP_CPP_PNGWU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "libpng - Win32 Release"

"zlib - Win32 Release" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" 
   cd "..\libpng"

"zlib - Win32 ReleaseCLEAN" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\zlib.mak CFG="zlib - Win32 Release"\
 RECURSE=1 
   cd "..\libpng"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

"zlib - Win32 Debug" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" 
   cd "..\libpng"

"zlib - Win32 DebugCLEAN" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\zlib.mak CFG="zlib - Win32 Debug" RECURSE=1\
 
   cd "..\libpng"

!ENDIF 


!ENDIF 

