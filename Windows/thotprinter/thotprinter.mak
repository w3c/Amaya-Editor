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

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "thotprinter - Win32 Release"

OUTDIR=.\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\thotprinter.dll"

!ELSE 

ALL : "libpng - Win32 Release" "libjpeg - Win32 Release"\
 "printlib - Win32 Release" "libThotTable - Win32 Release"\
 "$(OUTDIR)\thotprinter.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotTable - Win32 ReleaseCLEAN" "printlib - Win32 ReleaseCLEAN"\
 "libjpeg - Win32 ReleaseCLEAN" "libpng - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\css.obj"
	-@erase "$(INTDIR)\fetchHTMLname.obj"
	-@erase "$(INTDIR)\fetchXMLname.obj"
	-@erase "$(INTDIR)\nodialog.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\psdisplay.obj"
	-@erase "$(INTDIR)\styleparser.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\thotprinter.dll"
	-@erase "$(OUTDIR)\thotprinter.exp"
	-@erase "$(OUTDIR)\thotprinter.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\amaya" /I "..\..\amaya" /I\
 "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\thotlib\internals\h"\
 /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I\
 "..\..\tablelib\f" /I "..\..\thotlib\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "__STDC__" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS"\
 /Fp"$(INTDIR)\thotprinter.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL" 
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
	"$(INTDIR)\css.obj" \
	"$(INTDIR)\fetchHTMLname.obj" \
	"$(INTDIR)\fetchXMLname.obj" \
	"$(INTDIR)\nodialog.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\psdisplay.obj" \
	"$(INTDIR)\styleparser.obj" \
	"..\libjpeg.lib" \
	"..\libpng.lib" \
	"..\libThotTable.lib" \
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

ALL : "libpng - Win32 Debug" "libjpeg - Win32 Debug" "printlib - Win32 Debug"\
 "libThotTable - Win32 Debug" "$(OUTDIR)\thotprinter.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libThotTable - Win32 DebugCLEAN" "printlib - Win32 DebugCLEAN"\
 "libjpeg - Win32 DebugCLEAN" "libpng - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\css.obj"
	-@erase "$(INTDIR)\fetchHTMLname.obj"
	-@erase "$(INTDIR)\fetchXMLname.obj"
	-@erase "$(INTDIR)\nodialog.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\psdisplay.obj"
	-@erase "$(INTDIR)\styleparser.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\thotprinter.dll"
	-@erase "$(OUTDIR)\thotprinter.exp"
	-@erase "$(OUTDIR)\thotprinter.ilk"
	-@erase "$(OUTDIR)\thotprinter.lib"
	-@erase "$(OUTDIR)\thotprinter.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GX /Od /I "..\amaya" /I "..\..\amaya" /I\
 "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\tablelib\f" /I\
 "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I\
 "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /D "_DEBUG" /D\
 "_AMAYA_RELEASE_" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "_WIN_PRINT" /D\
 "PAGINEETIMPRIME" /D "STDC_HEADERS" /Fp"$(INTDIR)\thotprinter.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /ZI /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\thotprinter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\thotprinter.pdb" /debug /machine:I386\
 /nodefaultlib:"libcd.lib" /out:"$(OUTDIR)\thotprinter.dll"\
 /implib:"$(OUTDIR)\thotprinter.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\css.obj" \
	"$(INTDIR)\fetchHTMLname.obj" \
	"$(INTDIR)\fetchXMLname.obj" \
	"$(INTDIR)\nodialog.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\psdisplay.obj" \
	"$(INTDIR)\styleparser.obj" \
	"..\libjpeg.lib" \
	"..\libpng.lib" \
	"..\libThotTable.lib" \
	"..\printlib.lib"

"$(OUTDIR)\thotprinter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "thotprinter - Win32 Release" || "$(CFG)" ==\
 "thotprinter - Win32 Debug"

!IF  "$(CFG)" == "thotprinter - Win32 Release"

"libThotTable - Win32 Release" : 
   cd "\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak\
 CFG="libThotTable - Win32 Release" 
   cd "..\thotprinter"

"libThotTable - Win32 ReleaseCLEAN" : 
   cd "\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libThotTable.mak\
 CFG="libThotTable - Win32 Release" RECURSE=1 
   cd "..\thotprinter"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

"libThotTable - Win32 Debug" : 
   cd "\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) /F .\libThotTable.mak CFG="libThotTable - Win32 Debug"\
 
   cd "..\thotprinter"

"libThotTable - Win32 DebugCLEAN" : 
   cd "\Amaya\Windows\libThotTable"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libThotTable.mak\
 CFG="libThotTable - Win32 Debug" RECURSE=1 
   cd "..\thotprinter"

!ENDIF 

!IF  "$(CFG)" == "thotprinter - Win32 Release"

"printlib - Win32 Release" : 
   cd "\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Release" 
   cd "..\thotprinter"

"printlib - Win32 ReleaseCLEAN" : 
   cd "\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\printlib.mak CFG="printlib - Win32 Release"\
 RECURSE=1 
   cd "..\thotprinter"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

"printlib - Win32 Debug" : 
   cd "\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) /F .\printlib.mak CFG="printlib - Win32 Debug" 
   cd "..\thotprinter"

"printlib - Win32 DebugCLEAN" : 
   cd "\Amaya\Windows\printlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\printlib.mak CFG="printlib - Win32 Debug"\
 RECURSE=1 
   cd "..\thotprinter"

!ENDIF 

!IF  "$(CFG)" == "thotprinter - Win32 Release"

"libjpeg - Win32 Release" : 
   cd "\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Release" 
   cd "..\thotprinter"

"libjpeg - Win32 ReleaseCLEAN" : 
   cd "\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libjpeg.mak CFG="libjpeg - Win32 Release"\
 RECURSE=1 
   cd "..\thotprinter"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

"libjpeg - Win32 Debug" : 
   cd "\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Debug" 
   cd "..\thotprinter"

"libjpeg - Win32 DebugCLEAN" : 
   cd "\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libjpeg.mak CFG="libjpeg - Win32 Debug"\
 RECURSE=1 
   cd "..\thotprinter"

!ENDIF 

!IF  "$(CFG)" == "thotprinter - Win32 Release"

"libpng - Win32 Release" : 
   cd "\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Release" 
   cd "..\thotprinter"

"libpng - Win32 ReleaseCLEAN" : 
   cd "\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libpng.mak CFG="libpng - Win32 Release"\
 RECURSE=1 
   cd "..\thotprinter"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

"libpng - Win32 Debug" : 
   cd "\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Debug" 
   cd "..\thotprinter"

"libpng - Win32 DebugCLEAN" : 
   cd "\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libpng.mak CFG="libpng - Win32 Debug"\
 RECURSE=1 
   cd "..\thotprinter"

!ENDIF 

SOURCE=..\..\Amaya\css.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_CSS_C=\
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
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
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
	"..\..\..\libwww\Library\src\HTRDF.h"\
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
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\css.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\css_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\f\styleparser_f.h"\
	"..\..\amaya\f\uicss_f.h"\
	"..\..\amaya\libwww.h"\
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
	"..\..\thotlib\include\style.h"\
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
	"..\amaya\editor.h"\
	"..\amaya\html.h"\
	"..\amaya\textfile.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSS_C=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\css.obj" : $(SOURCE) $(DEP_CPP_CSS_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_CSS_C=\
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
	"..\..\amaya\css.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\css_f.h"\
	"..\..\amaya\f\init_f.h"\
	"..\..\amaya\f\query_f.h"\
	"..\..\amaya\f\styleparser_f.h"\
	"..\..\amaya\f\uicss_f.h"\
	"..\..\amaya\libwww.h"\
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
	"..\..\thotlib\include\style.h"\
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
	"..\amaya\editor.h"\
	"..\amaya\html.h"\
	"..\amaya\textfile.h"\
	

"$(INTDIR)\css.obj" : $(SOURCE) $(DEP_CPP_CSS_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Amaya\fetchHTMLname.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_FETCH=\
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
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
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
	"..\..\..\libwww\Library\src\HTRDF.h"\
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
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\fetchhtmlname_f.h"\
	"..\..\amaya\f\fetchxmlname_f.h"\
	"..\..\amaya\fetchhtmlname.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
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
	"..\amaya\editor.h"\
	"..\amaya\html.h"\
	"..\amaya\textfile.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FETCH=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\fetchHTMLname.obj" : $(SOURCE) $(DEP_CPP_FETCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_FETCH=\
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
	"..\..\amaya\f\fetchhtmlname_f.h"\
	"..\..\amaya\f\fetchxmlname_f.h"\
	"..\..\amaya\fetchhtmlname.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
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
	"..\amaya\editor.h"\
	"..\amaya\html.h"\
	"..\amaya\textfile.h"\
	

"$(INTDIR)\fetchHTMLname.obj" : $(SOURCE) $(DEP_CPP_FETCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Amaya\fetchXMLname.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_FETCHX=\
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
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
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
	"..\..\..\libwww\Library\src\HTRDF.h"\
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
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\f\fetchxmlname_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
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
	"..\amaya\editor.h"\
	"..\amaya\graphml.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FETCHX=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\fetchXMLname.obj" : $(SOURCE) $(DEP_CPP_FETCHX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_FETCHX=\
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
	"..\..\amaya\f\fetchxmlname_f.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
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
	"..\amaya\editor.h"\
	"..\amaya\html.h"\
	"..\amaya\mathml.h"\
	"..\amaya\textfile.h"\
	

"$(INTDIR)\fetchXMLname.obj" : $(SOURCE) $(DEP_CPP_FETCHX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\nodialog.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_NODIA=\
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
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
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
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
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
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
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
	"..\..\thotlib\internals\f\docs_f.h"\
	"..\..\thotlib\internals\f\documentapi_f.h"\
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
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
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
	"..\..\thotlib\internals\f\docs_f.h"\
	"..\..\thotlib\internals\f\documentapi_f.h"\
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
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\actions_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
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
	"..\..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRINT=\
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
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\..\thotlib\include\wininclude.h"\
	"..\..\thotlib\internals\f\absboxes_f.h"\
	"..\..\thotlib\internals\f\actions_f.h"\
	"..\..\thotlib\internals\f\applicationapi_f.h"\
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
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
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
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PSDIS=\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\psdisplay.obj" : $(SOURCE) $(DEP_CPP_PSDIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"


"$(INTDIR)\psdisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Amaya\styleparser.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

DEP_CPP_STYLE=\
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
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
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
	"..\..\..\libwww\Library\src\HTRDF.h"\
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
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	"..\..\amaya\ahtinit.h"\
	"..\..\amaya\amaya.h"\
	"..\..\amaya\amayamsg.h"\
	"..\..\amaya\css.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\css_f.h"\
	"..\..\amaya\f\fetchhtmlname_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlimage_f.h"\
	"..\..\amaya\f\htmlpresentation_f.h"\
	"..\..\amaya\f\styleparser_f.h"\
	"..\..\amaya\f\uicss_f.h"\
	"..\..\amaya\fetchhtmlname.h"\
	"..\..\amaya\htmlstylecolor.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
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
	"..\..\thotlib\include\style.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uaccess.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\amaya\editor.h"\
	"..\amaya\html.h"\
	"..\amaya\textfile.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STYLE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	"..\..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\styleparser.obj" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

DEP_CPP_STYLE=\
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
	"..\..\amaya\css.h"\
	"..\..\amaya\f\ahturltools_f.h"\
	"..\..\amaya\f\css_f.h"\
	"..\..\amaya\f\fetchhtmlname_f.h"\
	"..\..\amaya\f\html2thot_f.h"\
	"..\..\amaya\f\htmlimage_f.h"\
	"..\..\amaya\f\htmlpresentation_f.h"\
	"..\..\amaya\f\styleparser_f.h"\
	"..\..\amaya\f\uicss_f.h"\
	"..\..\amaya\fetchhtmlname.h"\
	"..\..\amaya\htmlstylecolor.h"\
	"..\..\amaya\libwww.h"\
	"..\..\amaya\parser.h"\
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
	"..\..\thotlib\include\style.h"\
	"..\..\thotlib\include\sysdep.h"\
	"..\..\thotlib\include\thot_gui.h"\
	"..\..\thotlib\include\thot_sys.h"\
	"..\..\thotlib\include\thot_uio.h"\
	"..\..\thotlib\include\tree.h"\
	"..\..\thotlib\include\typebase.h"\
	"..\..\thotlib\include\uaccess.h"\
	"..\..\thotlib\include\uconvert.h"\
	"..\..\thotlib\include\undo.h"\
	"..\..\thotlib\include\ustring.h"\
	"..\..\thotlib\include\view.h"\
	"..\amaya\editor.h"\
	"..\amaya\html.h"\
	"..\amaya\textfile.h"\
	

"$(INTDIR)\styleparser.obj" : $(SOURCE) $(DEP_CPP_STYLE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

