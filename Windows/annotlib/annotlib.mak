# Microsoft Developer Studio Generated NMAKE File, Based on annotlib.dsp
!IF "$(CFG)" == ""
CFG=annotlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to annotlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "annotlib - Win32 Release" && "$(CFG)" !=\
 "annotlib - Win32 Debug"
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

!IF  "$(CFG)" == "annotlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\annotlib.lib"

!ELSE 

ALL : "libwww - Win32 Release" "libThotEditor - Win32 Release"\
 "$(OUTDIR)\annotlib.lib"

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
	-@erase "$(INTDIR)\ANNOTnotif.obj"
	-@erase "$(INTDIR)\ANNOTschemas.obj"
	-@erase "$(INTDIR)\ANNOTtools.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\annotlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

RSC=rc.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\thotlib\internals\h" /I\
 "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I\
 "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I\
 "..\thotlib\internals\f" /I "..\annotlib" /I "..\annotlib\f" /D "NDEBUG" /D\
 "_MBCS" /D "_LIB" /D "_WINDOWS" /D "__STDC__" /D "WIN32" /D "ANNOTATIONS"\
 /Fp"$(INTDIR)\annotlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
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
	"$(INTDIR)\ANNOTnotif.obj" \
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
	-@erase "$(INTDIR)\ANNOTnotif.obj"
	-@erase "$(INTDIR)\ANNOTschemas.obj"
	-@erase "$(INTDIR)\ANNOTtools.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "..\annotlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

RSC=rc.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\annotlib" /I "..\..\annotlib\f"\
 /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I\
 "..\..\thotlib\include" /I "..\thotlib\internals\f" /I\
 "..\..\..\libwww\Library\src" /I "..\thotlib\internals\h" /I\
 "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya\f" /I "..\thotlib\include"\
 /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\annotlib" /I\
 "..\annotlib\f" /D "_DEBUG" /D "_WINDOWS" /D "__STDC__" /D "WIN32" /D\
 "ANNOTATIONS" /Fp"$(INTDIR)\annotlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /GZ /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
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
	"$(INTDIR)\ANNOTnotif.obj" \
	"$(INTDIR)\ANNOTschemas.obj" \
	"$(INTDIR)\ANNOTtools.obj" \
	"..\libThotEditor.lib" \
	"..\libwww.lib"

"..\annotlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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


!IF "$(CFG)" == "annotlib - Win32 Release" || "$(CFG)" ==\
 "annotlib - Win32 Debug"

!IF  "$(CFG)" == "annotlib - Win32 Release"

"libThotEditor - Win32 Release" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak\
 CFG="libThotEditor - Win32 Release" 
   cd "..\annotlib"

"libThotEditor - Win32 ReleaseCLEAN" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libThotEditor.mak\
 CFG="libThotEditor - Win32 Release" RECURSE=1 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"libThotEditor - Win32 Debug" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotEditor.mak\
 CFG="libThotEditor - Win32 Debug" 
   cd "..\annotlib"

"libThotEditor - Win32 DebugCLEAN" : 
   cd "..\LibThotEditor"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libThotEditor.mak\
 CFG="libThotEditor - Win32 Debug" RECURSE=1 
   cd "..\annotlib"

!ENDIF 

!IF  "$(CFG)" == "annotlib - Win32 Release"

"libwww - Win32 Release" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Release" 
   cd "..\annotlib"

"libwww - Win32 ReleaseCLEAN" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libwww.mak CFG="libwww - Win32 Release"\
 RECURSE=1 
   cd "..\annotlib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

"libwww - Win32 Debug" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="libwww - Win32 Debug" 
   cd "..\annotlib"

"libwww - Win32 DebugCLEAN" : 
   cd "..\libwww"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libwww.mak CFG="libwww - Win32 Debug"\
 RECURSE=1 
   cd "..\annotlib"

!ENDIF 

SOURCE=..\..\annotlib\AHTrdf2annot.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_AHTRD=\
	"..\..\annotlib\annotlib.h"\
	"..\amaya\annot.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\xlink.h"\
	
NODEP_CPP_AHTRD=\
	"..\..\annotlib\amaya.h"\
	"..\..\annotlib\ANNOTevent_f.h"\
	"..\..\annotlib\ANNOTfiles_f.h"\
	"..\..\annotlib\ANNOTlink_f.h"\
	"..\..\annotlib\ANNOTmenu_f.h"\
	"..\..\annotlib\ANNOTschemas_f.h"\
	"..\..\annotlib\ANNOTtools_f.h"\
	"..\..\annotlib\EDITORactions_f.h"\
	"..\..\annotlib\html2thot_f.h"\
	"..\..\annotlib\HTMLactions_f.h"\
	"..\..\annotlib\HTMLedit_f.h"\
	"..\..\annotlib\HTMLtable_f.h"\
	"..\..\annotlib\HTRDF.h"\
	"..\..\annotlib\init_f.h"\
	"..\..\annotlib\query_f.h"\
	"..\..\annotlib\rdf2annot_f.h"\
	"..\..\annotlib\wininclude.h"\
	"..\..\annotlib\xmlparse.h"\
	

"$(INTDIR)\AHTrdf2annot.obj" : $(SOURCE) $(DEP_CPP_AHTRD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_AHTRD=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\hthash.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htrdf.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\editoractions_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlactions_f.h"\
	"..\..\amaya\f\htmledit_f.h"\
	"..\..\amaya\f\htmltable_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
	"..\..\amaya\xmlparse.h"\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\f\annotevent_f.h"\
	"..\..\annotlib\f\annotfiles_f.h"\
	"..\..\annotlib\f\annotlink_f.h"\
	"..\..\annotlib\f\annotmenu_f.h"\
	"..\..\annotlib\f\annotschemas_f.h"\
	"..\..\annotlib\f\annottools_f.h"\
	"..\..\annotlib\f\rdf2annot_f.h"\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\browser.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\dialog.h"\
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
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\amaya\annot.h"\
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	"..\amaya\xlink.h"\
	

"$(INTDIR)\AHTrdf2annot.obj" : $(SOURCE) $(DEP_CPP_AHTRD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\amaya\AnnotAPP.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOT=\
	"..\amaya\annot.h"\
	
NODEP_CPP_ANNOT=\
	"..\amaya\application.h"\
	"..\amaya\appstruct.h"\
	"..\amaya\interface.h"\
	"..\amaya\thot_gui.h"\
	"..\amaya\thot_sys.h"\
	"..\amaya\ustring.h"\
	

"$(INTDIR)\AnnotAPP.obj" : $(SOURCE) $(DEP_CPP_ANNOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_ANNOT=\
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
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\amaya\annot.h"\
	

"$(INTDIR)\AnnotAPP.obj" : $(SOURCE) $(DEP_CPP_ANNOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\annotlib\ANNOTevent.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOTE=\
	"..\..\annotlib\annotlib.h"\
	"..\amaya\annot.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\xlink.h"\
	
NODEP_CPP_ANNOTE=\
	"..\..\annotlib\AHTURLTools_f.h"\
	"..\..\annotlib\amaya.h"\
	"..\..\annotlib\ANNOTevent_f.h"\
	"..\..\annotlib\ANNOTfiles_f.h"\
	"..\..\annotlib\ANNOTlink_f.h"\
	"..\..\annotlib\ANNOTmenu_f.h"\
	"..\..\annotlib\ANNOTschemas_f.h"\
	"..\..\annotlib\ANNOTtools_f.h"\
	"..\..\annotlib\EDITORactions_f.h"\
	"..\..\annotlib\html2thot_f.h"\
	"..\..\annotlib\HTMLactions_f.h"\
	"..\..\annotlib\HTMLedit_f.h"\
	"..\..\annotlib\HTMLtable_f.h"\
	"..\..\annotlib\init_f.h"\
	"..\..\annotlib\query_f.h"\
	"..\..\annotlib\rdf2annot_f.h"\
	"..\..\annotlib\wininclude.h"\
	"..\..\annotlib\XPointer.h"\
	"..\..\annotlib\XPointer_f.h"\
	"..\..\annotlib\XPointerparse_f.h"\
	

"$(INTDIR)\ANNOTevent.obj" : $(SOURCE) $(DEP_CPP_ANNOTE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_ANNOTE=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\editoractions_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlactions_f.h"\
	"..\..\amaya\f\htmledit_f.h"\
	"..\..\amaya\f\htmltable_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\f\xpointer_f.h"\
	"..\..\amaya\f\xpointerparse_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
	"..\..\amaya\xpointer.h"\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\f\annotevent_f.h"\
	"..\..\annotlib\f\annotfiles_f.h"\
	"..\..\annotlib\f\annotlink_f.h"\
	"..\..\annotlib\f\annotmenu_f.h"\
	"..\..\annotlib\f\annotschemas_f.h"\
	"..\..\annotlib\f\annottools_f.h"\
	"..\..\annotlib\f\rdf2annot_f.h"\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\browser.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\dialog.h"\
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
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\amaya\annot.h"\
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	"..\amaya\xlink.h"\
	

"$(INTDIR)\ANNOTevent.obj" : $(SOURCE) $(DEP_CPP_ANNOTE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\annotlib\ANNOTfiles.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOTF=\
	"..\..\annotlib\annotlib.h"\
	"..\amaya\annot.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\xlink.h"\
	
NODEP_CPP_ANNOTF=\
	"..\..\annotlib\AHTURLTools_f.h"\
	"..\..\annotlib\amaya.h"\
	"..\..\annotlib\ANNOTevent_f.h"\
	"..\..\annotlib\ANNOTfiles_f.h"\
	"..\..\annotlib\ANNOTlink_f.h"\
	"..\..\annotlib\ANNOTmenu_f.h"\
	"..\..\annotlib\ANNOTschemas_f.h"\
	"..\..\annotlib\ANNOTtools_f.h"\
	"..\..\annotlib\EDITORactions_f.h"\
	"..\..\annotlib\html2thot_f.h"\
	"..\..\annotlib\HTMLactions_f.h"\
	"..\..\annotlib\HTMLedit_f.h"\
	"..\..\annotlib\HTMLtable_f.h"\
	"..\..\annotlib\init_f.h"\
	"..\..\annotlib\query_f.h"\
	"..\..\annotlib\rdf2annot_f.h"\
	"..\..\annotlib\wininclude.h"\
	

"$(INTDIR)\ANNOTfiles.obj" : $(SOURCE) $(DEP_CPP_ANNOTF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_ANNOTF=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\editoractions_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlactions_f.h"\
	"..\..\amaya\f\htmledit_f.h"\
	"..\..\amaya\f\htmltable_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\f\annotevent_f.h"\
	"..\..\annotlib\f\annotfiles_f.h"\
	"..\..\annotlib\f\annotlink_f.h"\
	"..\..\annotlib\f\annotmenu_f.h"\
	"..\..\annotlib\f\annotschemas_f.h"\
	"..\..\annotlib\f\annottools_f.h"\
	"..\..\annotlib\f\rdf2annot_f.h"\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\browser.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\dialog.h"\
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
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\amaya\annot.h"\
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	"..\amaya\xlink.h"\
	

"$(INTDIR)\ANNOTfiles.obj" : $(SOURCE) $(DEP_CPP_ANNOTF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\annotlib\ANNOTlink.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOTL=\
	"..\..\annotlib\annotlib.h"\
	"..\amaya\annot.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\xlink.h"\
	
NODEP_CPP_ANNOTL=\
	"..\..\annotlib\AHTURLTools_f.h"\
	"..\..\annotlib\amaya.h"\
	"..\..\annotlib\ANNOTevent_f.h"\
	"..\..\annotlib\ANNOTfiles_f.h"\
	"..\..\annotlib\ANNOTlink_f.h"\
	"..\..\annotlib\ANNOTmenu_f.h"\
	"..\..\annotlib\ANNOTschemas_f.h"\
	"..\..\annotlib\ANNOTtools_f.h"\
	"..\..\annotlib\EDITORactions_f.h"\
	"..\..\annotlib\fetchXMLname_f.h"\
	"..\..\annotlib\html2thot_f.h"\
	"..\..\annotlib\HTMLactions_f.h"\
	"..\..\annotlib\HTMLedit_f.h"\
	"..\..\annotlib\HTMLtable_f.h"\
	"..\..\annotlib\init_f.h"\
	"..\..\annotlib\query_f.h"\
	"..\..\annotlib\rdf2annot_f.h"\
	"..\..\annotlib\wininclude.h"\
	"..\..\annotlib\XLinkedit_f.h"\
	"..\..\annotlib\XPointer.h"\
	"..\..\annotlib\XPointer_f.h"\
	"..\..\annotlib\XPointerparse_f.h"\
	

"$(INTDIR)\ANNOTlink.obj" : $(SOURCE) $(DEP_CPP_ANNOTL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_ANNOTL=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\editoractions_f.h"\
	"..\..\amaya\f\fetchxmlname_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlactions_f.h"\
	"..\..\amaya\f\htmledit_f.h"\
	"..\..\amaya\f\htmltable_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\f\xlinkedit_f.h"\
	"..\..\amaya\f\xpointer_f.h"\
	"..\..\amaya\f\xpointerparse_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
	"..\..\amaya\xpointer.h"\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\f\annotevent_f.h"\
	"..\..\annotlib\f\annotfiles_f.h"\
	"..\..\annotlib\f\annotlink_f.h"\
	"..\..\annotlib\f\annotmenu_f.h"\
	"..\..\annotlib\f\annotschemas_f.h"\
	"..\..\annotlib\f\annottools_f.h"\
	"..\..\annotlib\f\rdf2annot_f.h"\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\browser.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\dialog.h"\
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
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\amaya\annot.h"\
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	"..\amaya\xlink.h"\
	

"$(INTDIR)\ANNOTlink.obj" : $(SOURCE) $(DEP_CPP_ANNOTL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\annotlib\ANNOTmenu.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOTM=\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\annotmenu.h"\
	"..\amaya\annot.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\xlink.h"\
	
NODEP_CPP_ANNOTM=\
	"..\..\annotlib\amaya.h"\
	"..\..\annotlib\ANNOTevent_f.h"\
	"..\..\annotlib\ANNOTfiles_f.h"\
	"..\..\annotlib\ANNOTlink_f.h"\
	"..\..\annotlib\ANNOTmenu_f.h"\
	"..\..\annotlib\ANNOTschemas_f.h"\
	"..\..\annotlib\ANNOTtools_f.h"\
	"..\..\annotlib\appdialogue.h"\
	"..\..\annotlib\constmedia.h"\
	"..\..\annotlib\EDITORactions_f.h"\
	"..\..\annotlib\html2thot_f.h"\
	"..\..\annotlib\HTMLactions_f.h"\
	"..\..\annotlib\HTMLedit_f.h"\
	"..\..\annotlib\HTMLtable_f.h"\
	"..\..\annotlib\init_f.h"\
	"..\..\annotlib\query_f.h"\
	"..\..\annotlib\rdf2annot_f.h"\
	"..\..\annotlib\resource.h"\
	"..\..\annotlib\wininclude.h"\
	

"$(INTDIR)\ANNOTmenu.obj" : $(SOURCE) $(DEP_CPP_ANNOTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_ANNOTM=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\editoractions_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlactions_f.h"\
	"..\..\amaya\f\htmledit_f.h"\
	"..\..\amaya\f\htmltable_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\annotmenu.h"\
	"..\..\annotlib\f\annotevent_f.h"\
	"..\..\annotlib\f\annotfiles_f.h"\
	"..\..\annotlib\f\annotlink_f.h"\
	"..\..\annotlib\f\annotmenu_f.h"\
	"..\..\annotlib\f\annotschemas_f.h"\
	"..\..\annotlib\f\annottools_f.h"\
	"..\..\annotlib\f\rdf2annot_f.h"\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\browser.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\dialog.h"\
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
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\..\thotlib\internals\h\appdialogue.h"\
	"..\..\thotlib\internals\h\constint.h"\
	"..\..\thotlib\internals\h\constmedia.h"\
	"..\..\thotlib\internals\h\constmenu.h"\
	"..\..\thotlib\internals\h\constprs.h"\
	"..\..\thotlib\internals\h\conststr.h"\
	"..\..\thotlib\internals\h\frame.h"\
	"..\..\thotlib\internals\h\thotkey.h"\
	"..\amaya\annot.h"\
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	"..\amaya\xlink.h"\
	

"$(INTDIR)\ANNOTmenu.obj" : $(SOURCE) $(DEP_CPP_ANNOTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\annotlib\ANNOTnotif.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOTN=\
	"..\amaya\annot.h"\
	
NODEP_CPP_ANNOTN=\
	"..\..\annotlib\alliance_assistant.h"\
	"..\..\annotlib\editorComs.h"\
	

"$(INTDIR)\ANNOTnotif.obj" : $(SOURCE) $(DEP_CPP_ANNOTN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"


"$(INTDIR)\ANNOTnotif.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\annotlib\ANNOTschemas.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOTS=\
	"..\..\annotlib\annotlib.h"\
	"..\amaya\annot.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\xlink.h"\
	
NODEP_CPP_ANNOTS=\
	"..\..\annotlib\AHTURLTools_f.h"\
	"..\..\annotlib\amaya.h"\
	"..\..\annotlib\ANNOTevent_f.h"\
	"..\..\annotlib\ANNOTfiles_f.h"\
	"..\..\annotlib\ANNOTlink_f.h"\
	"..\..\annotlib\ANNOTmenu_f.h"\
	"..\..\annotlib\ANNOTschemas_f.h"\
	"..\..\annotlib\ANNOTtools_f.h"\
	"..\..\annotlib\EDITORactions_f.h"\
	"..\..\annotlib\html2thot_f.h"\
	"..\..\annotlib\HTMLactions_f.h"\
	"..\..\annotlib\HTMLedit_f.h"\
	"..\..\annotlib\HTMLtable_f.h"\
	"..\..\annotlib\HTRDF.h"\
	"..\..\annotlib\init_f.h"\
	"..\..\annotlib\query_f.h"\
	"..\..\annotlib\rdf2annot_f.h"\
	"..\..\annotlib\wininclude.h"\
	"..\..\annotlib\xmlparse.h"\
	

"$(INTDIR)\ANNOTschemas.obj" : $(SOURCE) $(DEP_CPP_ANNOTS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_ANNOTS=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\hthash.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htrdf.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\editoractions_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlactions_f.h"\
	"..\..\amaya\f\htmledit_f.h"\
	"..\..\amaya\f\htmltable_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
	"..\..\amaya\xmlparse.h"\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\f\annotevent_f.h"\
	"..\..\annotlib\f\annotfiles_f.h"\
	"..\..\annotlib\f\annotlink_f.h"\
	"..\..\annotlib\f\annotmenu_f.h"\
	"..\..\annotlib\f\annotschemas_f.h"\
	"..\..\annotlib\f\annottools_f.h"\
	"..\..\annotlib\f\rdf2annot_f.h"\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\browser.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\dialog.h"\
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
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\amaya\annot.h"\
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	"..\amaya\xlink.h"\
	

"$(INTDIR)\ANNOTschemas.obj" : $(SOURCE) $(DEP_CPP_ANNOTS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\annotlib\ANNOTtools.c

!IF  "$(CFG)" == "annotlib - Win32 Release"

DEP_CPP_ANNOTT=\
	"..\..\annotlib\annotlib.h"\
	"..\amaya\annot.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\xlink.h"\
	
NODEP_CPP_ANNOTT=\
	"..\..\annotlib\AHTURLTools_f.h"\
	"..\..\annotlib\amaya.h"\
	"..\..\annotlib\ANNOTevent_f.h"\
	"..\..\annotlib\ANNOTfiles_f.h"\
	"..\..\annotlib\ANNOTlink_f.h"\
	"..\..\annotlib\ANNOTmenu_f.h"\
	"..\..\annotlib\ANNOTschemas_f.h"\
	"..\..\annotlib\ANNOTtools_f.h"\
	"..\..\annotlib\EDITORactions_f.h"\
	"..\..\annotlib\fetchXMLname_f.h"\
	"..\..\annotlib\html2thot_f.h"\
	"..\..\annotlib\HTMLactions_f.h"\
	"..\..\annotlib\HTMLedit_f.h"\
	"..\..\annotlib\HTMLtable_f.h"\
	"..\..\annotlib\init_f.h"\
	"..\..\annotlib\query_f.h"\
	"..\..\annotlib\rdf2annot_f.h"\
	"..\..\annotlib\wininclude.h"\
	

"$(INTDIR)\ANNOTtools.obj" : $(SOURCE) $(DEP_CPP_ANNOTT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

DEP_CPP_ANNOTT=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\editoractions_f.h"\
	"..\..\amaya\f\fetchxmlname_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlactions_f.h"\
	"..\..\amaya\f\htmledit_f.h"\
	"..\..\amaya\f\htmltable_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
	"..\..\annotlib\annotlib.h"\
	"..\..\annotlib\f\annotevent_f.h"\
	"..\..\annotlib\f\annotfiles_f.h"\
	"..\..\annotlib\f\annotlink_f.h"\
	"..\..\annotlib\f\annotmenu_f.h"\
	"..\..\annotlib\f\annotschemas_f.h"\
	"..\..\annotlib\f\annottools_f.h"\
	"..\..\annotlib\f\rdf2annot_f.h"\
	"..\..\thotlib\include\appaction.h"\
	"..\..\thotlib\include\application.h"\
	"..\..\thotlib\include\appstruct.h"\
	"..\..\thotlib\include\attribute.h"\
	"..\..\thotlib\include\browser.h"\
	"..\..\thotlib\include\content.h"\
	"..\..\thotlib\include\dialog.h"\
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
	"..\..\thotlib\include\selection.h"\
	"..\..\thotlib\include\simx.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\amaya\annot.h"\
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	"..\amaya\xlink.h"\
	

"$(INTDIR)\ANNOTtools.obj" : $(SOURCE) $(DEP_CPP_ANNOTT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

