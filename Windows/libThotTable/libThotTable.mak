# Microsoft Developer Studio Generated NMAKE File, Based on libThotTable.dsp
!IF "$(CFG)" == ""
CFG=libThotTable - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libThotTable - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libThotTable - Win32 Release" && "$(CFG)" !=\
 "libThotTable - Win32 Debug"
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
OutDir=.\..\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libThotTable.lib"

!ELSE 

ALL : "$(OUTDIR)\libThotTable.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\table.obj"
	-@erase "$(INTDIR)\table2.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\libThotTable.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I\
 "..\..\thotlib\internals\var" /I "..\..\tablelib\f" /I "..\..\schemas" /D\
 "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__STDC__" /D "_WIN_PRINT" /D\
 "STDC_HEADERS" /Fp"$(INTDIR)\libThotTable.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
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
OutDir=.\..\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libThotTable.lib"

!ELSE 

ALL : "$(OUTDIR)\libThotTable.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\table.obj"
	-@erase "$(INTDIR)\table2.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\libThotTable.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\thotlib\include" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I\
 "..\..\thotlib\internals\var" /I "..\..\tablelib\f" /I "..\..\schemas" /D\
 "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__STDC__" /D "_WIN_PRINT" /D\
 "STDC_HEADERS" /D "AMAYA_DEBUG" /Fp"$(INTDIR)\libThotTable.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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


!IF "$(CFG)" == "libThotTable - Win32 Release" || "$(CFG)" ==\
 "libThotTable - Win32 Debug"
SOURCE=..\..\tablelib\table.c

!IF  "$(CFG)" == "libThotTable - Win32 Release"

DEP_CPP_TABLE=\
	"..\..\tablelib\exc_Table.h"\
	"..\..\tablelib\f\table2_f.h"\
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
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\attrpresent_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	"..\..\thotlib\internals\var\creation_tv.h"\
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TABLE=\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotTable - Win32 Debug"

DEP_CPP_TABLE=\
	"..\..\tablelib\exc_Table.h"\
	"..\..\tablelib\f\table2_f.h"\
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
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\attrpresent_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	"..\..\thotlib\internals\var\creation_tv.h"\
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\tablelib\table2.c

!IF  "$(CFG)" == "libThotTable - Win32 Release"

DEP_CPP_TABLE2=\
	"..\..\tablelib\exc_Table.h"\
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
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\attrpresent_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TABLE2=\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\table2.obj" : $(SOURCE) $(DEP_CPP_TABLE2) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotTable - Win32 Debug"

DEP_CPP_TABLE2=\
	"..\..\tablelib\exc_Table.h"\
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
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\attrpresent_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\references_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\table2.obj" : $(SOURCE) $(DEP_CPP_TABLE2) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\tablelib\tableH.c

!IF  "$(CFG)" == "libThotTable - Win32 Release"

DEP_CPP_TABLEH=\
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
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\boxmoves_f.h"\
	"..\..\thotlib\internals\f\boxrelations_f.h"\
	"..\..\thotlib\internals\f\buildboxes_f.h"\
	"..\..\thotlib\internals\f\buildlines_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TABLEH=\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\tableH.obj" : $(SOURCE) $(DEP_CPP_TABLEH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotTable - Win32 Debug"

DEP_CPP_TABLEH=\
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
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\boxmoves_f.h"\
	"..\..\thotlib\internals\f\boxrelations_f.h"\
	"..\..\thotlib\internals\f\buildboxes_f.h"\
	"..\..\thotlib\internals\f\buildlines_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\tableH.obj" : $(SOURCE) $(DEP_CPP_TABLEH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

