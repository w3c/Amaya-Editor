# Microsoft Developer Studio Generated NMAKE File, Based on zlib.dsp
!IF "$(CFG)" == ""
CFG=zlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to zlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "zlib - Win32 Release" && "$(CFG)" != "zlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "zlib - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\zlib.lib"

!ELSE 

ALL : "$(OUTDIR)\zlib.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "__STDC__" /Fp"$(INTDIR)\zlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\
 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\zlib.lib"

!ELSE 

ALL : "$(OUTDIR)\zlib.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "__STDC__" /Fp"$(INTDIR)\zlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\
 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "zlib - Win32 Release" || "$(CFG)" == "zlib - Win32 Debug"
SOURCE=..\..\libpng\zlib\adler32.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_ADLER=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\adler32.obj" : $(SOURCE) $(DEP_CPP_ADLER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_ADLER=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\adler32.obj" : $(SOURCE) $(DEP_CPP_ADLER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\compress.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_COMPR=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_COMPR=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\crc32.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_CRC32=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_CRC32=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\deflate.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_DEFLA=\
	"..\..\libpng\zlib\deflate.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_DEFLA=\
	"..\..\libpng\zlib\deflate.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\gzio.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_GZIO_=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\gzio.obj" : $(SOURCE) $(DEP_CPP_GZIO_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_GZIO_=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\gzio.obj" : $(SOURCE) $(DEP_CPP_GZIO_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\infblock.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_INFBL=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\infblock.obj" : $(SOURCE) $(DEP_CPP_INFBL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_INFBL=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\infblock.obj" : $(SOURCE) $(DEP_CPP_INFBL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\infcodes.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_INFCO=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inffast.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\infcodes.obj" : $(SOURCE) $(DEP_CPP_INFCO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_INFCO=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inffast.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\infcodes.obj" : $(SOURCE) $(DEP_CPP_INFCO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\inffast.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_INFFA=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inffast.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\inffast.obj" : $(SOURCE) $(DEP_CPP_INFFA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_INFFA=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inffast.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\inffast.obj" : $(SOURCE) $(DEP_CPP_INFFA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\inflate.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_INFLA=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\inflate.obj" : $(SOURCE) $(DEP_CPP_INFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_INFLA=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\inflate.obj" : $(SOURCE) $(DEP_CPP_INFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\inftrees.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_INFTR=\
	"..\..\libpng\zlib\inffixed.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\inftrees.obj" : $(SOURCE) $(DEP_CPP_INFTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_INFTR=\
	"..\..\libpng\zlib\inffixed.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\inftrees.obj" : $(SOURCE) $(DEP_CPP_INFTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\infutil.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_INFUT=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\infutil.obj" : $(SOURCE) $(DEP_CPP_INFUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_INFUT=\
	"..\..\libpng\zlib\infblock.h"\
	"..\..\libpng\zlib\infcodes.h"\
	"..\..\libpng\zlib\inftrees.h"\
	"..\..\libpng\zlib\infutil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\infutil.obj" : $(SOURCE) $(DEP_CPP_INFUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\trees.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_TREES=\
	"..\..\libpng\zlib\deflate.h"\
	"..\..\libpng\zlib\trees.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_TREES=\
	"..\..\libpng\zlib\deflate.h"\
	"..\..\libpng\zlib\trees.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\uncompr.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_UNCOM=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\uncompr.obj" : $(SOURCE) $(DEP_CPP_UNCOM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_UNCOM=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\uncompr.obj" : $(SOURCE) $(DEP_CPP_UNCOM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\libpng\zlib\zutil.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_ZUTIL=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\zutil.obj" : $(SOURCE) $(DEP_CPP_ZUTIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_ZUTIL=\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	"..\..\libpng\zlib\zutil.h"\
	

"$(INTDIR)\zutil.obj" : $(SOURCE) $(DEP_CPP_ZUTIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

