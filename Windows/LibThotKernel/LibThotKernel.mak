# Microsoft Developer Studio Generated NMAKE File, Based on LibThotKernel.dsp
!IF "$(CFG)" == ""
CFG=LibThotKernel - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LibThotKernel - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LibThotKernel - Win32 Release" && "$(CFG)" != "LibThotKernel - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibThotKernel.mak" CFG="LibThotKernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibThotKernel - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LibThotKernel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

ALL : "$(OUTDIR)\LibThotKernel.lib"


CLEAN :
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributeapi.obj"
	-@erase "$(INTDIR)\batchmessage.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\content.obj"
	-@erase "$(INTDIR)\contentapi.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\draw.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\nodisplay.obj"
	-@erase "$(INTDIR)\pivot.obj"
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\presentationapi.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\readprs.obj"
	-@erase "$(INTDIR)\readstr.obj"
	-@erase "$(INTDIR)\readtra.obj"
	-@erase "$(INTDIR)\referenceapi.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\schemastr.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\Uio.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(OUTDIR)\LibThotKernel.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS" /D "NODISPLAY" /D "_WINDOWS_COMPILERS" /Fp"$(INTDIR)\LibThotKernel.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LibThotKernel.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LibThotKernel.lib" 
LIB32_OBJS= \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributeapi.obj" \
	"$(INTDIR)\batchmessage.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\content.obj" \
	"$(INTDIR)\contentapi.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\draw.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\nodisplay.obj" \
	"$(INTDIR)\pivot.obj" \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\presentationapi.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\readprs.obj" \
	"$(INTDIR)\readstr.obj" \
	"$(INTDIR)\readtra.obj" \
	"$(INTDIR)\referenceapi.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\schemastr.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\Uio.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\writepivot.obj"

"$(OUTDIR)\LibThotKernel.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

ALL : "$(OUTDIR)\LibThotKernel.lib"


CLEAN :
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributeapi.obj"
	-@erase "$(INTDIR)\batchmessage.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\content.obj"
	-@erase "$(INTDIR)\contentapi.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\draw.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\nodisplay.obj"
	-@erase "$(INTDIR)\pivot.obj"
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\presentationapi.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\readprs.obj"
	-@erase "$(INTDIR)\readstr.obj"
	-@erase "$(INTDIR)\readtra.obj"
	-@erase "$(INTDIR)\referenceapi.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\schemastr.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\Uio.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(OUTDIR)\LibThotKernel.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS" /D "NODISPLAY" /D "_WINDOWS_COMPILERS" /Fp"$(INTDIR)\LibThotKernel.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LibThotKernel.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LibThotKernel.lib" 
LIB32_OBJS= \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributeapi.obj" \
	"$(INTDIR)\batchmessage.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\content.obj" \
	"$(INTDIR)\contentapi.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\draw.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\nodisplay.obj" \
	"$(INTDIR)\pivot.obj" \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\presentationapi.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\readprs.obj" \
	"$(INTDIR)\readstr.obj" \
	"$(INTDIR)\readtra.obj" \
	"$(INTDIR)\referenceapi.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\schemastr.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\Uio.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\writepivot.obj"

"$(OUTDIR)\LibThotKernel.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("LibThotKernel.dep")
!INCLUDE "LibThotKernel.dep"
!ELSE 
!MESSAGE Warning: cannot find "LibThotKernel.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LibThotKernel - Win32 Release" || "$(CFG)" == "LibThotKernel - Win32 Debug"
SOURCE=..\..\thotlib\base\actions.c

"$(INTDIR)\actions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\Editing\applicationapi.c

"$(INTDIR)\applicationapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\Tree\attributeapi.c

"$(INTDIR)\attributeapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\batchmessage.c

"$(INTDIR)\batchmessage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\checkaccess.c

"$(INTDIR)\checkaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\content.c

"$(INTDIR)\content.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\contentapi.c

"$(INTDIR)\contentapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\documentapi.c

"$(INTDIR)\documentapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\draw.c

"$(INTDIR)\draw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\exceptions.c

"$(INTDIR)\exceptions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\externalref.c

"$(INTDIR)\externalref.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\fileaccess.c

"$(INTDIR)\fileaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\labelalloc.c

"$(INTDIR)\labelalloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\language.c

"$(INTDIR)\language.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\memory.c

"$(INTDIR)\memory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\message.c

"$(INTDIR)\message.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\nodisplay.c

"$(INTDIR)\nodisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\pivot.c

"$(INTDIR)\pivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\platform.c

"$(INTDIR)\platform.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\presentationapi.c

"$(INTDIR)\presentationapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readpivot.c

"$(INTDIR)\readpivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readprs.c

"$(INTDIR)\readprs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readstr.c

"$(INTDIR)\readstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readtra.c

"$(INTDIR)\readtra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\referenceapi.c

"$(INTDIR)\referenceapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\references.c

"$(INTDIR)\references.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\registry.c

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\schemastr.c

"$(INTDIR)\schemastr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\schtrad.c

"$(INTDIR)\schtrad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\structlist.c

"$(INTDIR)\structlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\structschema.c

"$(INTDIR)\structschema.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\translation.c

"$(INTDIR)\translation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\tree.c

"$(INTDIR)\tree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\treeapi.c

"$(INTDIR)\treeapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\uaccess.c

"$(INTDIR)\uaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\uconvert.c

"$(INTDIR)\uconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\Unicode\Uio.c

"$(INTDIR)\Uio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\units.c

"$(INTDIR)\units.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\ustring.c

"$(INTDIR)\ustring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\writepivot.c

"$(INTDIR)\writepivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

