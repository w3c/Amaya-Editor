# Microsoft Developer Studio Generated NMAKE File, Based on thotprinter.dsp
!IF "$(CFG)" == ""
CFG=thotprinter - Win32 Debug
!MESSAGE No configuration specified. Defaulting to thotprinter - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "thotprinter - Win32 Release" && "$(CFG)" !=\
 "thotprinter - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "thotprinter.mak" CFG="thotprinter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "thotprinter - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "thotprinter - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "thotprinter - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\thotprinter.dll"

!ELSE 

ALL : "printlib - Win32 Release" "$(OUTDIR)\thotprinter.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"printlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\nodialog.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\psdisplay.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\thotprinter.dll"
	-@erase "$(OUTDIR)\thotprinter.exp"
	-@erase "$(OUTDIR)\thotprinter.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\thotlib\include" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I\
 "..\..\thotlib\internals\var" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "__STDC__" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS"\
 /Fp"$(INTDIR)\thotprinter.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\thotprinter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\thotprinter.pdb" /machine:I386 /out:"$(OUTDIR)\thotprinter.dll"\
 /implib:"$(OUTDIR)\thotprinter.lib" 
LINK32_OBJS= \
	"$(INTDIR)\nodialog.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\psdisplay.obj" \
	"..\printlib.lib"

"$(OUTDIR)\thotprinter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

OUTDIR=.\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\thotprinter.dll"

!ELSE 

ALL : "printlib - Win32 Debug" "$(OUTDIR)\thotprinter.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"printlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\nodialog.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\psdisplay.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\thotprinter.dll"
	-@erase "$(OUTDIR)\thotprinter.exp"
	-@erase "$(OUTDIR)\thotprinter.ilk"
	-@erase "$(OUTDIR)\thotprinter.lib"
	-@erase "$(OUTDIR)\thotprinter.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\thotlib\include" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I\
 "..\..\thotlib\internals\var" /I "..\..\tablelib\f" /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "__STDC__" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS"\
 /Fp"$(INTDIR)\thotprinter.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\thotprinter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\thotprinter.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib"\
 /out:"$(OUTDIR)\thotprinter.dll" /implib:"$(OUTDIR)\thotprinter.lib"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\nodialog.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\psdisplay.obj" \
	"..\printlib.lib"

"$(OUTDIR)\thotprinter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "thotprinter - Win32 Release" || "$(CFG)" ==\
 "thotprinter - Win32 Debug"

!IF  "$(CFG)" == "thotprinter - Win32 Release"

"printlib - Win32 Release" : 
   cd "..\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Release" 
   cd "..\thotprinter"

"printlib - Win32 ReleaseCLEAN" : 
   cd "..\printlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\printlib.mak CFG="printlib - Win32 Release"\
 RECURSE=1 
   cd "..\thotprinter"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

"printlib - Win32 Debug" : 
   cd "..\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Debug" 
   cd "..\thotprinter"

"printlib - Win32 DebugCLEAN" : 
   cd "..\printlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\printlib.mak CFG="printlib - Win32 Debug"\
 RECURSE=1 
   cd "..\thotprinter"

!ENDIF 

SOURCE=..\..\thotlib\dialogue\nodialog.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_NODIA=\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_NODIA=\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\nodialog.obj" : $(SOURCE) $(DEP_CPP_NODIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_NODIA=\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
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
	

"$(INTDIR)\nodialog.obj" : $(SOURCE) $(DEP_CPP_NODIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\paginate.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_PAGIN=\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\boxpositions_f.h"\
	"..\..\thotlib\internals\f\buildboxes_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\createpages_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\frame_f.h"\
	"..\..\thotlib\internals\f\pagecommands_f.h"\
	"..\..\thotlib\internals\f\paginate_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\presvariables_f.h"\
	"..\..\thotlib\internals\f\print_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structlist_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
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
	"..\..\thotlib\internals\var\page_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PAGIN=\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\paginate.obj" : $(SOURCE) $(DEP_CPP_PAGIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_PAGIN=\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
	"..\..\thotlib\internals\f\attributes_f.h"\
	"..\..\thotlib\internals\f\boxpositions_f.h"\
	"..\..\thotlib\internals\f\buildboxes_f.h"\
	"..\..\thotlib\internals\f\callback_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\changepresent_f.h"\
	"..\..\thotlib\internals\f\content_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\createpages_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\frame_f.h"\
	"..\..\thotlib\internals\f\pagecommands_f.h"\
	"..\..\thotlib\internals\f\paginate_f.h"\
	"..\..\thotlib\internals\f\presrules_f.h"\
	"..\..\thotlib\internals\f\presvariables_f.h"\
	"..\..\thotlib\internals\f\print_f.h"\
	"..\..\thotlib\internals\f\structcreation_f.h"\
	"..\..\thotlib\internals\f\structlist_f.h"\
	"..\..\thotlib\internals\f\structmodif_f.h"\
	"..\..\thotlib\internals\f\structselect_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\f\writepivot_f.h"\
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
	"..\..\thotlib\internals\var\page_tv.h"\
	

"$(INTDIR)\paginate.obj" : $(SOURCE) $(DEP_CPP_PAGIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\print.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_PRINT=\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\actions_f.h"\
	"..\..\thotlib\internals\f\attrpresent_f.h"\
	"..\..\thotlib\internals\f\boxmoves_f.h"\
	"..\..\thotlib\internals\f\boxpositions_f.h"\
	"..\..\thotlib\internals\f\buildboxes_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\context_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\dictionary_f.h"\
	"..\..\thotlib\internals\f\displaybox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\frame_f.h"\
	"..\..\thotlib\internals\f\inites_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\nodialog_f.h"\
	"..\..\thotlib\internals\f\pagecommands_f.h"\
	"..\..\thotlib\internals\f\paginate_f.h"\
	"..\..\thotlib\internals\f\picture_f.h"\
	"..\..\thotlib\internals\f\print_f.h"\
	"..\..\thotlib\internals\f\psdisplay_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\f\structlist_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotprinter_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\dictionary.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\printmsg.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\creation_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\font_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\page_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	"..\..\thotlib\internals\var\thotpalette_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRINT=\
	"..\..\thotlib\editing\table2_f.h"\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_PRINT=\
	"..\..\tablelib\f\table2_f.h"\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\actions_f.h"\
	"..\..\thotlib\internals\f\attrpresent_f.h"\
	"..\..\thotlib\internals\f\boxmoves_f.h"\
	"..\..\thotlib\internals\f\boxpositions_f.h"\
	"..\..\thotlib\internals\f\buildboxes_f.h"\
	"..\..\thotlib\internals\f\changeabsbox_f.h"\
	"..\..\thotlib\internals\f\config_f.h"\
	"..\..\thotlib\internals\f\context_f.h"\
	"..\..\thotlib\internals\f\createabsbox_f.h"\
	"..\..\thotlib\internals\f\dictionary_f.h"\
	"..\..\thotlib\internals\f\displaybox_f.h"\
	"..\..\thotlib\internals\f\exceptions_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\frame_f.h"\
	"..\..\thotlib\internals\f\inites_f.h"\
	"..\..\thotlib\internals\f\language_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\nodialog_f.h"\
	"..\..\thotlib\internals\f\pagecommands_f.h"\
	"..\..\thotlib\internals\f\paginate_f.h"\
	"..\..\thotlib\internals\f\picture_f.h"\
	"..\..\thotlib\internals\f\print_f.h"\
	"..\..\thotlib\internals\f\psdisplay_f.h"\
	"..\..\thotlib\internals\f\readpivot_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\f\structlist_f.h"\
	"..\..\thotlib\internals\f\structschema_f.h"\
	"..\..\thotlib\internals\f\thotprinter_f.h"\
	"..\..\thotlib\internals\f\tree_f.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\dictionary.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\printmsg.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\appdialogue_tv.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\creation_tv.h"\
	"..\..\thotlib\internals\var\edit_tv.h"\
	"..\..\thotlib\internals\var\font_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\modif_tv.h"\
	"..\..\thotlib\internals\var\page_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	"..\..\thotlib\internals\var\select_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	"..\..\thotlib\internals\var\thotpalette_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\psdisplay.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_PSDIS=\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\..\thotlib\internals\f\buildlines_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\initpses_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\font_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PSDIS=\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\psdisplay.obj" : $(SOURCE) $(DEP_CPP_PSDIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_PSDIS=\
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
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\uio.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\..\thotlib\internals\f\buildlines_f.h"\
	"..\..\thotlib\internals\f\font_f.h"\
	"..\..\thotlib\internals\f\initpses_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\units_f.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\picture.h"\
	"..\..\thotlib\internals\h\thotcolor.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\boxes_tv.h"\
	"..\..\thotlib\internals\var\font_tv.h"\
	"..\..\thotlib\internals\var\frame_tv.h"\
	"..\..\thotlib\internals\var\thotcolor_tv.h"\
	"..\..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\psdisplay.obj" : $(SOURCE) $(DEP_CPP_PSDIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

