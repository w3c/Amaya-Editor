# Microsoft Developer Studio Generated NMAKE File, Based on str.dsp
!IF "$(CFG)" == ""
CFG=str - Win32 Debug
!MESSAGE No configuration specified. Defaulting to str - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "str - Win32 Release" && "$(CFG)" != "str - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "str.mak" CFG="str - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "str - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "str - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "str - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\str.dll"

!ELSE 

ALL : "cpp - Win32 Release" "LibThotKernel - Win32 Release" "$(OUTDIR)\str.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"LibThotKernel - Win32 ReleaseCLEAN" "cpp - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\compilmsg.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\str.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\writestr.obj"
	-@erase "$(OUTDIR)\str.dll"
	-@erase "$(OUTDIR)\str.exp"
	-@erase "$(OUTDIR)\str.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\thotlib\include" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I\
 "..\..\thotlib\internals\var" /I "..\..\batch\f" /D "NDEBUG" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WINDOWS_COMPILERS"\
 /Fp"$(INTDIR)\str.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\str.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\str.pdb" /machine:I386 /out:"$(OUTDIR)\str.dll"\
 /implib:"$(OUTDIR)\str.lib" 
LINK32_OBJS= \
	"$(INTDIR)\compilmsg.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\str.obj" \
	"$(INTDIR)\writestr.obj" \
	"$(OUTDIR)\cpp.lib" \
	"..\LibThotKernel.lib"

"$(OUTDIR)\str.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "str - Win32 Debug"

OUTDIR=.\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\str.dll"

!ELSE 

ALL : "cpp - Win32 Debug" "LibThotKernel - Win32 Debug" "$(OUTDIR)\str.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"LibThotKernel - Win32 DebugCLEAN" "cpp - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\compilmsg.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\str.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\writestr.obj"
	-@erase "$(OUTDIR)\str.dll"
	-@erase "$(OUTDIR)\str.exp"
	-@erase "$(OUTDIR)\str.ilk"
	-@erase "$(OUTDIR)\str.lib"
	-@erase "$(OUTDIR)\str.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\batch\f" /I\
 "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I\
 "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /D "_DEBUG" /D\
 "_WINDOWS_COMPILERS" /D "WIN32" /D "_WINDOWS" /D "__STDC__"\
 /Fp"$(INTDIR)\str.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\str.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\str.pdb" /debug /machine:I386 /out:"$(OUTDIR)\str.dll"\
 /implib:"$(OUTDIR)\str.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\compilmsg.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\str.obj" \
	"$(INTDIR)\writestr.obj" \
	"$(OUTDIR)\cpp.lib" \
	"..\LibThotKernel.lib"

"$(OUTDIR)\str.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "str - Win32 Release" || "$(CFG)" == "str - Win32 Debug"

!IF  "$(CFG)" == "str - Win32 Release"

"LibThotKernel - Win32 Release" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak\
 CFG="LibThotKernel - Win32 Release" 
   cd "..\str"

"LibThotKernel - Win32 ReleaseCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\LibThotKernel.mak\
 CFG="LibThotKernel - Win32 Release" RECURSE=1 
   cd "..\str"

!ELSEIF  "$(CFG)" == "str - Win32 Debug"

"LibThotKernel - Win32 Debug" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) /F .\LibThotKernel.mak\
 CFG="LibThotKernel - Win32 Debug" 
   cd "..\str"

"LibThotKernel - Win32 DebugCLEAN" : 
   cd "..\LibThotKernel"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\LibThotKernel.mak\
 CFG="LibThotKernel - Win32 Debug" RECURSE=1 
   cd "..\str"

!ENDIF 

!IF  "$(CFG)" == "str - Win32 Release"

"cpp - Win32 Release" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Release" 
   cd "..\str"

"cpp - Win32 ReleaseCLEAN" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\cpp.mak CFG="cpp - Win32 Release" RECURSE=1\
 
   cd "..\str"

!ELSEIF  "$(CFG)" == "str - Win32 Debug"

"cpp - Win32 Debug" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) /F .\cpp.mak CFG="cpp - Win32 Debug" 
   cd "..\str"

"cpp - Win32 DebugCLEAN" : 
   cd "..\cpp"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\cpp.mak CFG="cpp - Win32 Debug" RECURSE=1 
   cd "..\str"

!ENDIF 

SOURCE=..\..\thotlib\base\compilmsg.c

!IF  "$(CFG)" == "str - Win32 Release"

DEP_CPP_COMPI=\
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
	"..\..\thotlib\internals\h\compilmsg.h"\
	"..\..\thotlib\internals\h\constgrm.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typegrm.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\compil_tv.h"\
	
NODEP_CPP_COMPI=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\compilmsg.obj" : $(SOURCE) $(DEP_CPP_COMPI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "str - Win32 Debug"

DEP_CPP_COMPI=\
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
	"..\..\thotlib\internals\h\compilmsg.h"\
	"..\..\thotlib\internals\h\constgrm.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typegrm.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\compil_tv.h"\
	

"$(INTDIR)\compilmsg.obj" : $(SOURCE) $(DEP_CPP_COMPI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\batch\parser.c

!IF  "$(CFG)" == "str - Win32 Release"

DEP_CPP_PARSE=\
	"..\..\batch\f\parser_f.h"\
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
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\compilmsg_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\compilmsg.h"\
	"..\..\thotlib\internals\h\constgrm.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typegrm.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\analsynt_tv.h"\
	"..\..\thotlib\internals\var\compil_tv.h"\
	
NODEP_CPP_PARSE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "str - Win32 Debug"

DEP_CPP_PARSE=\
	"..\..\batch\f\parser_f.h"\
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
	"..\..\thotlib\include\registry.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\internals\f\compilmsg_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\compilmsg.h"\
	"..\..\thotlib\internals\h\constgrm.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typegrm.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\analsynt_tv.h"\
	"..\..\thotlib\internals\var\compil_tv.h"\
	

"$(INTDIR)\parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\batch\str.c

!IF  "$(CFG)" == "str - Win32 Release"

DEP_CPP_STR_C=\
	"..\..\batch\f\parser_f.h"\
	"..\..\batch\f\writestr_f.h"\
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
	"..\..\thotlib\internals\f\compilers_f.h"\
	"..\..\thotlib\internals\f\compilmsg_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\compilmsg.h"\
	"..\..\thotlib\internals\h\constgrm.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\strdef.h"\
	"..\..\thotlib\internals\h\strmsg.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typegrm.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\analsynt_tv.h"\
	"..\..\thotlib\internals\var\compil_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	
NODEP_CPP_STR_C=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\str.obj" : $(SOURCE) $(DEP_CPP_STR_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "str - Win32 Debug"

DEP_CPP_STR_C=\
	"..\..\batch\f\parser_f.h"\
	"..\..\batch\f\writestr_f.h"\
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
	"..\..\thotlib\internals\f\compilers_f.h"\
	"..\..\thotlib\internals\f\compilmsg_f.h"\
	"..\..\thotlib\internals\f\fileaccess_f.h"\
	"..\..\thotlib\internals\f\memory_f.h"\
	"..\..\thotlib\internals\f\message_f.h"\
	"..\..\thotlib\internals\f\platform_f.h"\
	"..\..\thotlib\internals\f\readstr_f.h"\
	"..\..\thotlib\internals\f\registry_f.h"\
	"..\..\thotlib\internals\h\compilmsg.h"\
	"..\..\thotlib\internals\h\constgrm.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\consttra.h"\
	"..\..\thotlib\internals\h\strdef.h"\
	"..\..\thotlib\internals\h\strmsg.h"\
	"..\..\thotlib\internals\h\thotdir.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\..\thotlib\internals\h\typecorr.h"\
	"..\..\thotlib\internals\h\typegrm.h"\
	"..\..\thotlib\internals\h\typeint.h"\
	"..\..\thotlib\internals\h\typemedia.h"\
	"..\..\thotlib\internals\h\typeprs.h"\
	"..\..\thotlib\internals\h\typestr.h"\
	"..\..\thotlib\internals\h\typetra.h"\
	"..\..\thotlib\internals\var\analsynt_tv.h"\
	"..\..\thotlib\internals\var\compil_tv.h"\
	"..\..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\str.obj" : $(SOURCE) $(DEP_CPP_STR_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\batch\writestr.c

!IF  "$(CFG)" == "str - Win32 Release"

DEP_CPP_WRITE=\
	"..\..\batch\f\writestr_f.h"\
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
	
NODEP_CPP_WRITE=\
	"..\..\thotlib\include\app.h"\
	

"$(INTDIR)\writestr.obj" : $(SOURCE) $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "str - Win32 Debug"

DEP_CPP_WRITE=\
	"..\..\batch\f\writestr_f.h"\
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
	

"$(INTDIR)\writestr.obj" : $(SOURCE) $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

