# Microsoft Developer Studio Generated NMAKE File, Based on libpng.dsp
!IF "$(CFG)" == ""
CFG=libpng - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libpng - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libpng - Win32 Release" && "$(CFG)" != "libpng - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak" CFG="libpng - Win32 Debug"
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

ALL : "$(OUTDIR)\libpng.lib"

!ELSE 

ALL : "zlib - Win32 Release" "$(OUTDIR)\libpng.lib"

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
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\libpng\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\libpng.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

"$(OUTDIR)\libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

ALL : "$(OUTDIR)\libpng.lib"

!ELSE 

ALL : "zlib - Win32 Debug" "$(OUTDIR)\libpng.lib"

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
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\libpng\zlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\libpng.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

"$(OUTDIR)\libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libpng.dep")
!INCLUDE "libpng.dep"
!ELSE 
!MESSAGE Warning: cannot find "libpng.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libpng - Win32 Release" || "$(CFG)" == "libpng - Win32 Debug"

!IF  "$(CFG)" == "libpng - Win32 Release"

"zlib - Win32 Release" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" 
   cd "..\libpng"

"zlib - Win32 ReleaseCLEAN" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\libpng"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

"zlib - Win32 Debug" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" 
   cd "..\libpng"

"zlib - Win32 DebugCLEAN" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\libpng"

!ENDIF 

SOURCE=..\..\libpng\png.c

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngerror.c

"$(INTDIR)\pngerror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngget.c

"$(INTDIR)\pngget.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngmem.c

"$(INTDIR)\pngmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngpread.c

"$(INTDIR)\pngpread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngread.c

"$(INTDIR)\pngread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngrio.c

"$(INTDIR)\pngrio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngrtran.c

"$(INTDIR)\pngrtran.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngrutil.c

"$(INTDIR)\pngrutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngset.c

"$(INTDIR)\pngset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngtrans.c

"$(INTDIR)\pngtrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngwio.c

"$(INTDIR)\pngwio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngwrite.c

"$(INTDIR)\pngwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngwtran.c

"$(INTDIR)\pngwtran.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libpng\pngwutil.c

"$(INTDIR)\pngwutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

