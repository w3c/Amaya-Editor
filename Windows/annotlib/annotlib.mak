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

!IF  "$(CFG)" == "annotlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\annotlib.lib"

!ELSE 

ALL : "libraptor - Win32 Release" "libwww - Win32 Release" "libThotEditor - Win32 Release" "Compilers - Win32 Release" "..\annotlib.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Compilers - Win32 ReleaseCLEAN" "libThotEditor - Win32 ReleaseCLEAN" "libwww - Win32 ReleaseCLEAN" "libraptor - Win32 ReleaseCLEAN" 
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

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\annotlib" /I "..\..\annotlib\f" /I "..\..\libraptor" /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\thotlib\internals\f" /I "..\..\..\libwww\Library\src" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\annotlib" /I "..\annotlib\f" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__STDC__" /D "ANNOTATIONS" /D "RAPTOR_RDF_PARSER" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /Fp"$(INTDIR)\annotlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
	"..\libwww.lib" \
	"..\libraptor.lib"

"..\annotlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\annotlib.lib"

!ELSE 

ALL : "libraptor - Win32 Debug" "libwww - Win32 Debug" "libThotEditor - Win32 Debug" "Compilers - Win32 Debug" "..\annotlib.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Compilers - Win32 DebugCLEAN" "libThotEditor - Win32 DebugCLEAN" "libwww - Win32 DebugCLEAN" "libraptor - Win32 DebugCLEAN" 
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

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\annotlib" /I "..\..\annotlib\f" /I "..\..\libraptor" /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\thotlib\internals\f" /I "..\..\..\libwww\Library\src" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\annotlib" /I "..\annotlib\f" /D "_DEBUG" /D "__STDC__" /D "ANNOTATIONS" /D "RAPTOR_RDF_PARSER" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /Fp"$(INTDIR)\annotlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
	"..\libwww.lib" \
	"..\libraptor.lib"

"..\annotlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("annotlib.dep")
!INCLUDE "annotlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "annotlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "annotlib - Win32 Release" || "$(CFG)" == "annotlib - Win32 Debug"

!IF  "$(CFG)" == "annotlib - Win32 Release"

"Compilers - Win32 Release" : 
   cd "\src\Amaya\Windows\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Release" 
   cd "..\annotlib"

"Compilers - Win32 ReleaseCLEAN" : 
   cd "\src\Amaya\Windows\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Release" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"Compilers - Win32 Debug" : 
   cd "\src\Amaya\Windows\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Debug" 
   cd "..\annotlib"

"Compilers - Win32 DebugCLEAN" : 
   cd "\src\Amaya\Windows\Compilers"
   $(MAKE) /$(MAKEFLAGS) /F .\Compilers.mak CFG="Compilers - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ENDIF 

!IF  "$(CFG)" == "annotlib - Win32 Release"

"libThotEditor - Win32 Release" : 
   cd "\src\Amaya\Windows\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Release" 
   cd "..\annotlib"

"libThotEditor - Win32 ReleaseCLEAN" : 
   cd "\src\Amaya\Windows\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Release" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"libThotEditor - Win32 Debug" : 
   cd "\src\Amaya\Windows\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Debug" 
   cd "..\annotlib"

"libThotEditor - Win32 DebugCLEAN" : 
   cd "\src\Amaya\Windows\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak CFG="libThotEditor - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ENDIF 

!IF  "$(CFG)" == "annotlib - Win32 Release"

"libwww - Win32 Release" : 
   cd "\src\Amaya\Windows\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Release" 
   cd "..\annotlib"

"libwww - Win32 ReleaseCLEAN" : 
   cd "\src\Amaya\Windows\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Release" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"libwww - Win32 Debug" : 
   cd "\src\Amaya\Windows\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Debug" 
   cd "..\annotlib"

"libwww - Win32 DebugCLEAN" : 
   cd "\src\Amaya\Windows\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ENDIF 

!IF  "$(CFG)" == "annotlib - Win32 Release"

"libraptor - Win32 Release" : 
   cd "\src\Amaya\Windows\libraptor"
   $(MAKE) /$(MAKEFLAGS) /F .\libraptor.mak CFG="libraptor - Win32 Release" 
   cd "..\annotlib"

"libraptor - Win32 ReleaseCLEAN" : 
   cd "\src\Amaya\Windows\libraptor"
   $(MAKE) /$(MAKEFLAGS) /F .\libraptor.mak CFG="libraptor - Win32 Release" RECURSE=1 CLEAN 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"libraptor - Win32 Debug" : 
   cd "\src\Amaya\Windows\libraptor"
   $(MAKE) /$(MAKEFLAGS) /F .\libraptor.mak CFG="libraptor - Win32 Debug" 
   cd "..\annotlib"

"libraptor - Win32 DebugCLEAN" : 
   cd "\src\Amaya\Windows\libraptor"
   $(MAKE) /$(MAKEFLAGS) /F .\libraptor.mak CFG="libraptor - Win32 Debug" RECURSE=1 CLEAN 
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

