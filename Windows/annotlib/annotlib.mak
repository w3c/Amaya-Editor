# Microsoft Developer Studio Generated NMAKE File, Based on annotlib.dsp
!IF "$(CFG)" == ""
CFG=annotlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to annotlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "annotlib - Win32 Release" && "$(CFG)" != "annotlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "annotlib.mak" CFG="annotlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "annotlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "annotlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "annotlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\annotlib.lib"

!ELSE 

ALL : "libwww - Win32 Release" "libThotEditor - Win32 Release" "$(OUTDIR)\annotlib.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 ReleaseCLEAN" "libwww - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AHTrdf2annot.obj"
	-@erase "$(INTDIR)\AnnotAPP.obj"
	-@erase "$(INTDIR)\ANNOTevent.obj"
	-@erase "$(INTDIR)\ANNOTfiles.obj"
	-@erase "$(INTDIR)\ANNOTlink.obj"
	-@erase "$(INTDIR)\ANNOTmenu.obj"
	-@erase "$(INTDIR)\ANNOTschemas.obj"
	-@erase "$(INTDIR)\ANNOTtools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\annotlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\annotlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\annotlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\annotlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AHTrdf2annot.obj" \
	"$(INTDIR)\AnnotAPP.obj" \
	"$(INTDIR)\ANNOTevent.obj" \
	"$(INTDIR)\ANNOTfiles.obj" \
	"$(INTDIR)\ANNOTlink.obj" \
	"$(INTDIR)\ANNOTmenu.obj" \
	"$(INTDIR)\ANNOTschemas.obj" \
	"$(INTDIR)\ANNOTtools.obj" \
	"..\libThotEditor.lib" \
	"..\libwww.lib"

"$(OUTDIR)\annotlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\annotlib.lib"

!ELSE 

ALL : "libwww - Win32 Debug" "libThotEditor - Win32 Debug" "..\annotlib.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotEditor - Win32 DebugCLEAN" "libwww - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AHTrdf2annot.obj"
	-@erase "$(INTDIR)\AnnotAPP.obj"
	-@erase "$(INTDIR)\ANNOTevent.obj"
	-@erase "$(INTDIR)\ANNOTfiles.obj"
	-@erase "$(INTDIR)\ANNOTlink.obj"
	-@erase "$(INTDIR)\ANNOTmenu.obj"
	-@erase "$(INTDIR)\ANNOTschemas.obj"
	-@erase "$(INTDIR)\ANNOTtools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\annotlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\annotlib" /I "..\..\annotlib\f" /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\thotlib\internals\f" /I "..\..\..\libwww\Library\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__STDC__" /D "ANNOTATIONS" /Fp"$(INTDIR)\annotlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\annotlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\annotlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AHTrdf2annot.obj" \
	"$(INTDIR)\AnnotAPP.obj" \
	"$(INTDIR)\ANNOTevent.obj" \
	"$(INTDIR)\ANNOTfiles.obj" \
	"$(INTDIR)\ANNOTlink.obj" \
	"$(INTDIR)\ANNOTmenu.obj" \
	"$(INTDIR)\ANNOTschemas.obj" \
	"$(INTDIR)\ANNOTtools.obj" \
	"..\libThotEditor.lib" \
	"..\libwww.lib"

"..\annotlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("annotlib.dep")
!INCLUDE "annotlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "annotlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "annotlib - Win32 Release" || "$(CFG)" == "annotlib - Win32 Debug"

!IF  "$(CFG)" == "annotlib - Win32 Release"

"libThotEditor - Win32 Release" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F ".\libThotEditor.mak" CFG="libThotEditor - Win32 Release" 
   cd "..\annotlib"

"libThotEditor - Win32 ReleaseCLEAN" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F ".\libThotEditor.mak" CFG="libThotEditor - Win32 Release" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"libThotEditor - Win32 Debug" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F ".\libThotEditor.mak" CFG="libThotEditor - Win32 Debug" 
   cd "..\annotlib"

"libThotEditor - Win32 DebugCLEAN" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F ".\libThotEditor.mak" CFG="libThotEditor - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ENDIF 

!IF  "$(CFG)" == "annotlib - Win32 Release"

"libwww - Win32 Release" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="libwww - Win32 Release" 
   cd "..\annotlib"

"libwww - Win32 ReleaseCLEAN" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="libwww - Win32 Release" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"libwww - Win32 Debug" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="libwww - Win32 Debug" 
   cd "..\annotlib"

"libwww - Win32 DebugCLEAN" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="libwww - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ENDIF 

SOURCE=..\..\annotlib\AHTrdf2annot.c

"$(INTDIR)\AHTrdf2annot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\amaya\AnnotAPP.c

"$(INTDIR)\AnnotAPP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\annotlib\ANNOTevent.c

"$(INTDIR)\ANNOTevent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\annotlib\ANNOTfiles.c

"$(INTDIR)\ANNOTfiles.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\annotlib\ANNOTlink.c

"$(INTDIR)\ANNOTlink.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\annotlib\ANNOTmenu.c

"$(INTDIR)\ANNOTmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\annotlib\ANNOTschemas.c

"$(INTDIR)\ANNOTschemas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\annotlib\ANNOTtools.c

"$(INTDIR)\ANNOTtools.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

