# Microsoft Developer Studio Generated NMAKE File, Based on LibThotKernel.dsp
!IF "$(CFG)" == ""
CFG=LibThotKernel - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LibThotKernel - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LibThotKernel - Win32 Release" && "$(CFG)" !=\
 "LibThotKernel - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibThotKernel.mak" CFG="LibThotKernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibThotKernel - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "LibThotKernel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\LibThotKernel.lib"

!ELSE 

ALL : "$(OUTDIR)\LibThotKernel.lib"

!ENDIF 

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
	-@erase "$(INTDIR)\parsexml.obj"
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
	-@erase "$(INTDIR)\res.obj"
	-@erase "$(INTDIR)\resgen.obj"
	-@erase "$(INTDIR)\resmatch.obj"
	-@erase "$(INTDIR)\savexml.obj"
	-@erase "$(INTDIR)\schemastr.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\thotmodule.obj"
	-@erase "$(INTDIR)\translatexml.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\writexml.obj"
	-@erase "$(INTDIR)\xmlmodule.obj"
	-@erase "$(OUTDIR)\LibThotKernel.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I\
 "..\..\thotlib\internals\f" /D "NDEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "NODISPLAY" /D "__STDC__" /D "STDC_HEADERS" /D "_WINDOWS_COMPILERS"\
 /Fp"$(INTDIR)\LibThotKernel.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
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
	"$(INTDIR)\parsexml.obj" \
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
	"$(INTDIR)\res.obj" \
	"$(INTDIR)\resgen.obj" \
	"$(INTDIR)\resmatch.obj" \
	"$(INTDIR)\savexml.obj" \
	"$(INTDIR)\schemastr.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\thotmodule.obj" \
	"$(INTDIR)\translatexml.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\writexml.obj" \
	"$(INTDIR)\xmlmodule.obj"

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

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\LibThotKernel.lib"

!ELSE 

ALL : "$(OUTDIR)\LibThotKernel.lib"

!ENDIF 

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
	-@erase "$(INTDIR)\parsexml.obj"
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
	-@erase "$(INTDIR)\res.obj"
	-@erase "$(INTDIR)\resgen.obj"
	-@erase "$(INTDIR)\resmatch.obj"
	-@erase "$(INTDIR)\savexml.obj"
	-@erase "$(INTDIR)\schemastr.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\thotmodule.obj"
	-@erase "$(INTDIR)\translatexml.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\writexml.obj"
	-@erase "$(INTDIR)\xmlmodule.obj"
	-@erase "$(OUTDIR)\LibThotKernel.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\thotlib\include" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I\
 "..\..\thotlib\internals\f" /D "_DEBUG" /D "STDC_HEADERS" /D\
 "_WINDOWS_COMPILERS" /D "WIN32" /D "_WINDOWS" /D "NODISPLAY" /D "__STDC__"\
 /Fp"$(INTDIR)\LibThotKernel.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
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
	"$(INTDIR)\parsexml.obj" \
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
	"$(INTDIR)\res.obj" \
	"$(INTDIR)\resgen.obj" \
	"$(INTDIR)\resmatch.obj" \
	"$(INTDIR)\savexml.obj" \
	"$(INTDIR)\schemastr.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\thotmodule.obj" \
	"$(INTDIR)\translatexml.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\writexml.obj" \
	"$(INTDIR)\xmlmodule.obj"

"$(OUTDIR)\LibThotKernel.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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


!IF "$(CFG)" == "LibThotKernel - Win32 Release" || "$(CFG)" ==\
 "LibThotKernel - Win32 Debug"
SOURCE=..\..\thotlib\base\actions.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_ACTIO=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	
NODEP_CPP_ACTIO=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\actions.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_ACTIO=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	

"$(INTDIR)\actions.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\applicationapi.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_APPLI=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\dialog.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\checkaccess_f.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\dialogapi_f.h"\
	"..\..\thotlib\internals\f\input_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\schtrad_f.h"\
	"..\..\thotlib\internals\f\searchmenu_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\wininclude.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\dictionary.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\thotpattern.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\appevents_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\print_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	"..\..\thotlib\internals\var\thotpalette_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	
NODEP_CPP_APPLI=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\applicationapi.obj" : $(SOURCE) $(DEP_CPP_APPLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_APPLI=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\dialog.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\checkaccess_f.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\dialogapi_f.h"\
	"..\..\thotlib\internals\f\input_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\schtrad_f.h"\
	"..\..\thotlib\internals\f\searchmenu_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\wininclude.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\dictionary.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\thotpattern.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\appevents_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\print_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	"..\..\thotlib\internals\var\thotpalette_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\applicationapi.obj" : $(SOURCE) $(DEP_CPP_APPLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\attributeapi.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_ATTRI=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\attributeapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	
NODEP_CPP_ATTRI=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\attributeapi.obj" : $(SOURCE) $(DEP_CPP_ATTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_ATTRI=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\attributeapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\attributeapi.obj" : $(SOURCE) $(DEP_CPP_ATTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\batchmessage.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_BATCH=\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_sys.h"\
	

"$(INTDIR)\batchmessage.obj" : $(SOURCE) $(DEP_CPP_BATCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_BATCH=\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\ustring.h"\
	

"$(INTDIR)\batchmessage.obj" : $(SOURCE) $(DEP_CPP_BATCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\checkaccess.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_CHECK=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\abspictures_f.h"\
	"..\..\thotlib\internals\f\actions_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\res.h"\
	"..\..\thotlib\internals\h\resdynmsg.h"\
	"..\..\thotlib\internals\h\selectrestruct.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	
NODEP_CPP_CHECK=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\checkaccess.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_CHECK=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\abspictures_f.h"\
	"..\..\thotlib\internals\f\actions_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\res.h"\
	"..\..\thotlib\internals\h\resdynmsg.h"\
	"..\..\thotlib\internals\h\selectrestruct.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\checkaccess.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\content.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_CONTE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	
NODEP_CPP_CONTE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\content.obj" : $(SOURCE) $(DEP_CPP_CONTE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_CONTE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\content.obj" : $(SOURCE) $(DEP_CPP_CONTE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\contentapi.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_CONTEN=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\picture_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_CONTEN=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\contentapi.obj" : $(SOURCE) $(DEP_CPP_CONTEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_CONTEN=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\picture_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\contentapi.obj" : $(SOURCE) $(DEP_CPP_CONTEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\documentapi.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_DOCUM=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\appdialogue_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\docs_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\translation_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\f\views_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	
NODEP_CPP_DOCUM=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\documentapi.obj" : $(SOURCE) $(DEP_CPP_DOCUM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_DOCUM=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\appdialogue_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\docs_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\translation_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\f\views_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\documentapi.obj" : $(SOURCE) $(DEP_CPP_DOCUM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\draw.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_DRAW_=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	
NODEP_CPP_DRAW_=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_DRAW_=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\exceptions.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_EXCEP=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_EXCEP=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\exceptions.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_EXCEP=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\exceptions.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\externalref.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_EXTER=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_EXTER=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\externalref.obj" : $(SOURCE) $(DEP_CPP_EXTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_EXTER=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\externalref.obj" : $(SOURCE) $(DEP_CPP_EXTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\fileaccess.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_FILEA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_FILEA=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\fileaccess.obj" : $(SOURCE) $(DEP_CPP_FILEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_FILEA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\fileaccess.obj" : $(SOURCE) $(DEP_CPP_FILEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\labelalloc.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_LABEL=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	
NODEP_CPP_LABEL=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\labelalloc.obj" : $(SOURCE) $(DEP_CPP_LABEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_LABEL=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\labelalloc.obj" : $(SOURCE) $(DEP_CPP_LABEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\language.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_LANGU=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\dictionary.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	
NODEP_CPP_LANGU=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_LANGU=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\dictionary.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\memory.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_MEMOR=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_MEMOR=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_MEMOR=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\message.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_MESSA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\dialog.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\dialogapi_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	
NODEP_CPP_MESSA=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_MESSA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\dialog.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\dialogapi_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\nodisplay.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_NODIS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_NODIS=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\nodisplay.obj" : $(SOURCE) $(DEP_CPP_NODIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_NODIS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\nodisplay.obj" : $(SOURCE) $(DEP_CPP_NODIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\xml\parsexml.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_PARSE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\thotmodule_f.h"\
	"..\..\thotlib\internals\f\translatexml_f.h"\
	"..\..\thotlib\internals\f\xmlmodule_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_PARSE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\parsexml.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_PARSE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\thotmodule_f.h"\
	"..\..\thotlib\internals\f\translatexml_f.h"\
	"..\..\thotlib\internals\f\xmlmodule_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\parsexml.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\pivot.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_PIVOT=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_PIVOT=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\pivot.obj" : $(SOURCE) $(DEP_CPP_PIVOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_PIVOT=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\pivot.obj" : $(SOURCE) $(DEP_CPP_PIVOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\platform.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_PLATF=\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	

"$(INTDIR)\platform.obj" : $(SOURCE) $(DEP_CPP_PLATF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_PLATF=\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	

"$(INTDIR)\platform.obj" : $(SOURCE) $(DEP_CPP_PLATF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\presentationapi.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_PRESE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\appli_f.h"\
	"..\..\thotlib\internals\f\boxpositions_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\compilmsg_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_PRESE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\presentationapi.obj" : $(SOURCE) $(DEP_CPP_PRESE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_PRESE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\appli_f.h"\
	"..\..\thotlib\internals\f\boxpositions_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\compilmsg_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\presentationapi.obj" : $(SOURCE) $(DEP_CPP_PRESE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readpivot.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_READP=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\abspictures_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\viewcommands_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_READP=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\readpivot.obj" : $(SOURCE) $(DEP_CPP_READP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_READP=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\abspictures_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\viewcommands_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\readpivot.obj" : $(SOURCE) $(DEP_CPP_READP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readprs.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_READPR=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readprs_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_READPR=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\readprs.obj" : $(SOURCE) $(DEP_CPP_READPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_READPR=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readprs_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\readprs.obj" : $(SOURCE) $(DEP_CPP_READPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readstr.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_READS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_READS=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\readstr.obj" : $(SOURCE) $(DEP_CPP_READS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_READS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\readstr.obj" : $(SOURCE) $(DEP_CPP_READS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readtra.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_READT=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\readtra_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_READT=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\readtra.obj" : $(SOURCE) $(DEP_CPP_READT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_READT=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\readtra_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\readtra.obj" : $(SOURCE) $(DEP_CPP_READT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\referenceapi.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_REFER=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\reference.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemastr_f.h"\
	"..\..\thotlib\internals\f\search_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_REFER=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\referenceapi.obj" : $(SOURCE) $(DEP_CPP_REFER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_REFER=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\reference.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemastr_f.h"\
	"..\..\thotlib\internals\f\search_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\referenceapi.obj" : $(SOURCE) $(DEP_CPP_REFER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\references.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_REFERE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_REFERE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\references.obj" : $(SOURCE) $(DEP_CPP_REFERE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_REFERE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\references.obj" : $(SOURCE) $(DEP_CPP_REFERE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\registry.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_REGIS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_REGIS=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_REGIS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\res.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_RES_C=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\resgen_f.h"\
	"..\..\thotlib\internals\f\resmatch_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\constres.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	
NODEP_CPP_RES_C=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\res.obj" : $(SOURCE) $(DEP_CPP_RES_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_RES_C=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\resgen_f.h"\
	"..\..\thotlib\internals\f\resmatch_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\constres.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\res.obj" : $(SOURCE) $(DEP_CPP_RES_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\resgen.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_RESGE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\resmatch_f.h"\
	"..\..\thotlib\internals\h\constres.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	
NODEP_CPP_RESGE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\resgen.obj" : $(SOURCE) $(DEP_CPP_RESGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_RESGE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\resmatch_f.h"\
	"..\..\thotlib\internals\h\constres.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\resgen.obj" : $(SOURCE) $(DEP_CPP_RESGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\resmatch.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_RESMA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\h\constres.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	
NODEP_CPP_RESMA=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\resmatch.obj" : $(SOURCE) $(DEP_CPP_RESMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_RESMA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\h\constres.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\resmatch.obj" : $(SOURCE) $(DEP_CPP_RESMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\xml\savexml.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_SAVEX=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\parsexml_f.h"\
	"..\..\thotlib\internals\f\views_f.h"\
	"..\..\thotlib\internals\f\writexml_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	
NODEP_CPP_SAVEX=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\savexml.obj" : $(SOURCE) $(DEP_CPP_SAVEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_SAVEX=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\parsexml_f.h"\
	"..\..\thotlib\internals\f\views_f.h"\
	"..\..\thotlib\internals\f\writexml_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	

"$(INTDIR)\savexml.obj" : $(SOURCE) $(DEP_CPP_SAVEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\schemastr.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_SCHEM=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_SCHEM=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\schemastr.obj" : $(SOURCE) $(DEP_CPP_SCHEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_SCHEM=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\schemastr.obj" : $(SOURCE) $(DEP_CPP_SCHEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\schtrad.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_SCHTR=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\readtra_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\schtrad_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_SCHTR=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\schtrad.obj" : $(SOURCE) $(DEP_CPP_SCHTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_SCHTR=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\readtra_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\schtrad_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\schtrad.obj" : $(SOURCE) $(DEP_CPP_SCHTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\structlist.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_STRUC=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\structlist_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_STRUC=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\structlist.obj" : $(SOURCE) $(DEP_CPP_STRUC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_STRUC=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\structlist_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\structlist.obj" : $(SOURCE) $(DEP_CPP_STRUC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\structschema.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_STRUCT=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\schemastr_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_STRUCT=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\structschema.obj" : $(SOURCE) $(DEP_CPP_STRUCT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_STRUCT=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\schemastr_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\structschema.obj" : $(SOURCE) $(DEP_CPP_STRUCT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\xml\thotmodule.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_THOTM=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\nodisplay_f.h"\
	"..\..\thotlib\internals\f\parsexml_f.h"\
	"..\..\thotlib\internals\f\xmlmodule_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	
NODEP_CPP_THOTM=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\thotmodule.obj" : $(SOURCE) $(DEP_CPP_THOTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_THOTM=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\nodisplay_f.h"\
	"..\..\thotlib\internals\f\parsexml_f.h"\
	"..\..\thotlib\internals\f\xmlmodule_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\thotmodule.obj" : $(SOURCE) $(DEP_CPP_THOTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\xml\translatexml.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_TRANS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	
NODEP_CPP_TRANS=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\translatexml.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_TRANS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	

"$(INTDIR)\translatexml.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\translation.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_TRANSL=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readprs_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\schtrad_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\translation_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	
NODEP_CPP_TRANSL=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\translation.obj" : $(SOURCE) $(DEP_CPP_TRANSL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_TRANSL=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\readprs_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\schtrad_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\translation_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\translation.obj" : $(SOURCE) $(DEP_CPP_TRANSL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\tree.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_TREE_=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\abspictures_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\res.h"\
	"..\..\thotlib\internals\h\resdynmsg.h"\
	"..\..\thotlib\internals\h\selectrestruct.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	
NODEP_CPP_TREE_=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_TREE_=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\abspictures_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\draw_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\res.h"\
	"..\..\thotlib\internals\h\resdynmsg.h"\
	"..\..\thotlib\internals\h\selectrestruct.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\treeapi.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_TREEA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributeapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\documentapi_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\translation_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_TREEA=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\treeapi.obj" : $(SOURCE) $(DEP_CPP_TREEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_TREEA=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributeapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\documentapi_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotmsg_f.h"\
	"..\..\thotlib\internals\f\translation_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\viewapi_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\treeapi.obj" : $(SOURCE) $(DEP_CPP_TREEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\unicode\uaccess.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_UACCE=\
	"..\..\thotlib\include\uaccess.h"\
	"..\..\thotlib\include\ustring.h"\
	

"$(INTDIR)\uaccess.obj" : $(SOURCE) $(DEP_CPP_UACCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_UACCE=\
	"..\..\thotlib\include\uaccess.h"\
	"..\..\thotlib\include\ustring.h"\
	

"$(INTDIR)\uaccess.obj" : $(SOURCE) $(DEP_CPP_UACCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\unicode\uconvert.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_UCONV=\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\ustring.h"\
	

"$(INTDIR)\uconvert.obj" : $(SOURCE) $(DEP_CPP_UCONV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_UCONV=\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\ustring.h"\
	

"$(INTDIR)\uconvert.obj" : $(SOURCE) $(DEP_CPP_UCONV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\units.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_UNITS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\wininclude.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	
NODEP_CPP_UNITS=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\units.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_UNITS=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\wininclude.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\units.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\unicode\ustring.c
DEP_CPP_USTRI=\
	"..\..\thotlib\include\ustring.h"\
	

"$(INTDIR)\ustring.obj" : $(SOURCE) $(DEP_CPP_USTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\writepivot.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_WRITE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_WRITE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\writepivot.obj" : $(SOURCE) $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_WRITE=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\labelallocator.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\externalref_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\labelalloc_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\schemas_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\writepivot.obj" : $(SOURCE) $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\xml\writexml.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_WRITEX=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\reference.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\translatexml_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	
NODEP_CPP_WRITEX=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\writexml.obj" : $(SOURCE) $(DEP_CPP_WRITEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_WRITEX=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\libmsg.h"\
	"..\..\thotlib\include\message.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\reference.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\translatexml_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constpiv.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	

"$(INTDIR)\writexml.obj" : $(SOURCE) $(DEP_CPP_WRITEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\xml\xmlmodule.c

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

DEP_CPP_XMLMO=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\reference.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\parsexml_f.h"\
	"..\..\thotlib\internals\f\xmlmodule_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	
NODEP_CPP_XMLMO=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\xmlmodule.obj" : $(SOURCE) $(DEP_CPP_XMLMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

DEP_CPP_XMLMO=\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\document.h"\
	"..\..\thotlib\include\fileaccess.h"\
	"..\..\thotlib\include\interface.h"\
	"..\..\thotlib\include\language.h"\
	"..\..\thotlib\include\presentation.h"\
	"..\..\thotlib\include\pschema.h"\
	"..\..\thotlib\include\reference.h"\
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\parsexml_f.h"\
	"..\..\thotlib\internals\f\xmlmodule_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\constxml.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\h\typexml.h"\
	

"$(INTDIR)\xmlmodule.obj" : $(SOURCE) $(DEP_CPP_XMLMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

