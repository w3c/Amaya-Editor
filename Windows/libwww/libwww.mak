# Microsoft Developer Studio Generated NMAKE File, Based on libwww.dsp
!IF "$(CFG)" == ""
CFG=libwww - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libwww - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libwww - Win32 Release" && "$(CFG)" != "libwww - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libwww.mak" CFG="libwww - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libwww - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libwww - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "libwww - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libwww.lib"

!ELSE 

ALL : "zlib - Win32 Release" "$(OUTDIR)\libwww.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\HTAABrow.obj"
	-@erase "$(INTDIR)\HTAAUtil.obj"
	-@erase "$(INTDIR)\HTAccess.obj"
	-@erase "$(INTDIR)\HTAlert.obj"
	-@erase "$(INTDIR)\HTAnchor.obj"
	-@erase "$(INTDIR)\HTANSI.obj"
	-@erase "$(INTDIR)\HTArray.obj"
	-@erase "$(INTDIR)\HTAssoc.obj"
	-@erase "$(INTDIR)\HTAtom.obj"
	-@erase "$(INTDIR)\HTBind.obj"
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTBound.obj"
	-@erase "$(INTDIR)\HTBTree.obj"
	-@erase "$(INTDIR)\HTBufWrt.obj"
	-@erase "$(INTDIR)\HTCache.obj"
	-@erase "$(INTDIR)\HTChannl.obj"
	-@erase "$(INTDIR)\HTChunk.obj"
	-@erase "$(INTDIR)\HTConLen.obj"
	-@erase "$(INTDIR)\HTDemux.obj"
	-@erase "$(INTDIR)\HTDescpt.obj"
	-@erase "$(INTDIR)\HTDialog.obj"
	-@erase "$(INTDIR)\HTDigest.obj"
	-@erase "$(INTDIR)\HTDir.obj"
	-@erase "$(INTDIR)\HTDNS.obj"
	-@erase "$(INTDIR)\HTEPtoCl.obj"
	-@erase "$(INTDIR)\HTError.obj"
	-@erase "$(INTDIR)\HTEscape.obj"
	-@erase "$(INTDIR)\HTEvent.obj"
	-@erase "$(INTDIR)\HTEvtLst.obj"
	-@erase "$(INTDIR)\HTFile.obj"
	-@erase "$(INTDIR)\HTFilter.obj"
	-@erase "$(INTDIR)\HTFormat.obj"
	-@erase "$(INTDIR)\HTFSave.obj"
	-@erase "$(INTDIR)\HTFTP.obj"
	-@erase "$(INTDIR)\HTFTPDir.obj"
	-@erase "$(INTDIR)\HTFWrite.obj"
	-@erase "$(INTDIR)\HTGopher.obj"
	-@erase "$(INTDIR)\HTGuess.obj"
	-@erase "$(INTDIR)\HTHeader.obj"
	-@erase "$(INTDIR)\HTHInit.obj"
	-@erase "$(INTDIR)\HTHist.obj"
	-@erase "$(INTDIR)\HTHome.obj"
	-@erase "$(INTDIR)\HTHost.obj"
	-@erase "$(INTDIR)\HTIcons.obj"
	-@erase "$(INTDIR)\HTInet.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTLib.obj"
	-@erase "$(INTDIR)\HTLink.obj"
	-@erase "$(INTDIR)\HTList.obj"
	-@erase "$(INTDIR)\HTLocal.obj"
	-@erase "$(INTDIR)\HTLog.obj"
	-@erase "$(INTDIR)\HTMemLog.obj"
	-@erase "$(INTDIR)\HTMemory.obj"
	-@erase "$(INTDIR)\HTMerge.obj"
	-@erase "$(INTDIR)\HTMethod.obj"
	-@erase "$(INTDIR)\HTMIME.obj"
	-@erase "$(INTDIR)\HTMIMERq.obj"
	-@erase "$(INTDIR)\HTMIMImp.obj"
	-@erase "$(INTDIR)\HTMIMPrs.obj"
	-@erase "$(INTDIR)\HTML.obj"
	-@erase "$(INTDIR)\HTMLGen.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTMulti.obj"
	-@erase "$(INTDIR)\HTMuxCh.obj"
	-@erase "$(INTDIR)\HTMuxTx.obj"
	-@erase "$(INTDIR)\HTNDir.obj"
	-@erase "$(INTDIR)\HTNet.obj"
	-@erase "$(INTDIR)\HTNetTxt.obj"
	-@erase "$(INTDIR)\HTNews.obj"
	-@erase "$(INTDIR)\HTNewsLs.obj"
	-@erase "$(INTDIR)\HTNewsRq.obj"
	-@erase "$(INTDIR)\HTNoFree.obj"
	-@erase "$(INTDIR)\HTParse.obj"
	-@erase "$(INTDIR)\HTPEP.obj"
	-@erase "$(INTDIR)\HTPlain.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\HTProt.obj"
	-@erase "$(INTDIR)\HTProxy.obj"
	-@erase "$(INTDIR)\HTReader.obj"
	-@erase "$(INTDIR)\HTReqMan.obj"
	-@erase "$(INTDIR)\HTResponse.obj"
	-@erase "$(INTDIR)\HTRules.obj"
	-@erase "$(INTDIR)\HTSChunk.obj"
	-@erase "$(INTDIR)\HTSocket.obj"
	-@erase "$(INTDIR)\HTStream.obj"
	-@erase "$(INTDIR)\HTString.obj"
	-@erase "$(INTDIR)\HTStyle.obj"
	-@erase "$(INTDIR)\HTTChunk.obj"
	-@erase "$(INTDIR)\HTTCP.obj"
	-@erase "$(INTDIR)\HTTee.obj"
	-@erase "$(INTDIR)\HTTelnet.obj"
	-@erase "$(INTDIR)\HTTeXGen.obj"
	-@erase "$(INTDIR)\HTTimer.obj"
	-@erase "$(INTDIR)\HTTP.obj"
	-@erase "$(INTDIR)\HTTPGen.obj"
	-@erase "$(INTDIR)\HTTPReq.obj"
	-@erase "$(INTDIR)\HTTPRes.obj"
	-@erase "$(INTDIR)\HTTPServ.obj"
	-@erase "$(INTDIR)\HTTrace.obj"
	-@erase "$(INTDIR)\HTTrans.obj"
	-@erase "$(INTDIR)\HTUser.obj"
	-@erase "$(INTDIR)\HTUTree.obj"
	-@erase "$(INTDIR)\HTUU.obj"
	-@erase "$(INTDIR)\HTWriter.obj"
	-@erase "$(INTDIR)\HTWSRC.obj"
	-@erase "$(INTDIR)\HTWWWStr.obj"
	-@erase "$(INTDIR)\HTXParse.obj"
	-@erase "$(INTDIR)\HTZip.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libwww.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\libwww\modules\md5" /I "..\..\..\libwww\modules\expat\xmlparse" /I "..\..\..\libwww\modules\expat\xmltok" /I "..\..\..\libwww\Library\src" /I "..\..\libpng\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "HT_ZLIB" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libwww.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libwww.lib" 
LIB32_OBJS= \
	"$(INTDIR)\HTAABrow.obj" \
	"$(INTDIR)\HTAAUtil.obj" \
	"$(INTDIR)\HTAccess.obj" \
	"$(INTDIR)\HTAlert.obj" \
	"$(INTDIR)\HTAnchor.obj" \
	"$(INTDIR)\HTANSI.obj" \
	"$(INTDIR)\HTArray.obj" \
	"$(INTDIR)\HTAssoc.obj" \
	"$(INTDIR)\HTAtom.obj" \
	"$(INTDIR)\HTBind.obj" \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTBound.obj" \
	"$(INTDIR)\HTBTree.obj" \
	"$(INTDIR)\HTBufWrt.obj" \
	"$(INTDIR)\HTCache.obj" \
	"$(INTDIR)\HTChannl.obj" \
	"$(INTDIR)\HTChunk.obj" \
	"$(INTDIR)\HTConLen.obj" \
	"$(INTDIR)\HTDemux.obj" \
	"$(INTDIR)\HTDescpt.obj" \
	"$(INTDIR)\HTDialog.obj" \
	"$(INTDIR)\HTDigest.obj" \
	"$(INTDIR)\HTDir.obj" \
	"$(INTDIR)\HTDNS.obj" \
	"$(INTDIR)\HTEPtoCl.obj" \
	"$(INTDIR)\HTError.obj" \
	"$(INTDIR)\HTEscape.obj" \
	"$(INTDIR)\HTEvent.obj" \
	"$(INTDIR)\HTEvtLst.obj" \
	"$(INTDIR)\HTFile.obj" \
	"$(INTDIR)\HTFilter.obj" \
	"$(INTDIR)\HTFormat.obj" \
	"$(INTDIR)\HTFSave.obj" \
	"$(INTDIR)\HTFTP.obj" \
	"$(INTDIR)\HTFTPDir.obj" \
	"$(INTDIR)\HTFWrite.obj" \
	"$(INTDIR)\HTGopher.obj" \
	"$(INTDIR)\HTGuess.obj" \
	"$(INTDIR)\HTHeader.obj" \
	"$(INTDIR)\HTHInit.obj" \
	"$(INTDIR)\HTHist.obj" \
	"$(INTDIR)\HTHome.obj" \
	"$(INTDIR)\HTHost.obj" \
	"$(INTDIR)\HTIcons.obj" \
	"$(INTDIR)\HTInet.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTLib.obj" \
	"$(INTDIR)\HTLink.obj" \
	"$(INTDIR)\HTList.obj" \
	"$(INTDIR)\HTLocal.obj" \
	"$(INTDIR)\HTLog.obj" \
	"$(INTDIR)\HTMemLog.obj" \
	"$(INTDIR)\HTMemory.obj" \
	"$(INTDIR)\HTMerge.obj" \
	"$(INTDIR)\HTMethod.obj" \
	"$(INTDIR)\HTMIME.obj" \
	"$(INTDIR)\HTMIMERq.obj" \
	"$(INTDIR)\HTMIMImp.obj" \
	"$(INTDIR)\HTMIMPrs.obj" \
	"$(INTDIR)\HTML.obj" \
	"$(INTDIR)\HTMLGen.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTMulti.obj" \
	"$(INTDIR)\HTMuxCh.obj" \
	"$(INTDIR)\HTMuxTx.obj" \
	"$(INTDIR)\HTNDir.obj" \
	"$(INTDIR)\HTNet.obj" \
	"$(INTDIR)\HTNetTxt.obj" \
	"$(INTDIR)\HTNews.obj" \
	"$(INTDIR)\HTNewsLs.obj" \
	"$(INTDIR)\HTNewsRq.obj" \
	"$(INTDIR)\HTNoFree.obj" \
	"$(INTDIR)\HTParse.obj" \
	"$(INTDIR)\HTPEP.obj" \
	"$(INTDIR)\HTPlain.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\HTProt.obj" \
	"$(INTDIR)\HTProxy.obj" \
	"$(INTDIR)\HTReader.obj" \
	"$(INTDIR)\HTReqMan.obj" \
	"$(INTDIR)\HTResponse.obj" \
	"$(INTDIR)\HTRules.obj" \
	"$(INTDIR)\HTSChunk.obj" \
	"$(INTDIR)\HTSocket.obj" \
	"$(INTDIR)\HTStream.obj" \
	"$(INTDIR)\HTString.obj" \
	"$(INTDIR)\HTStyle.obj" \
	"$(INTDIR)\HTTChunk.obj" \
	"$(INTDIR)\HTTCP.obj" \
	"$(INTDIR)\HTTee.obj" \
	"$(INTDIR)\HTTelnet.obj" \
	"$(INTDIR)\HTTeXGen.obj" \
	"$(INTDIR)\HTTimer.obj" \
	"$(INTDIR)\HTTP.obj" \
	"$(INTDIR)\HTTPGen.obj" \
	"$(INTDIR)\HTTPReq.obj" \
	"$(INTDIR)\HTTPRes.obj" \
	"$(INTDIR)\HTTPServ.obj" \
	"$(INTDIR)\HTTrace.obj" \
	"$(INTDIR)\HTTrans.obj" \
	"$(INTDIR)\HTUser.obj" \
	"$(INTDIR)\HTUTree.obj" \
	"$(INTDIR)\HTUU.obj" \
	"$(INTDIR)\HTWriter.obj" \
	"$(INTDIR)\HTWSRC.obj" \
	"$(INTDIR)\HTWWWStr.obj" \
	"$(INTDIR)\HTXParse.obj" \
	"$(INTDIR)\HTZip.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\SGML.obj" \
	"$(OUTDIR)\zlib.lib"

"$(OUTDIR)\libwww.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libwww.lib"

!ELSE 

ALL : "zlib - Win32 Debug" "$(OUTDIR)\libwww.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\HTAABrow.obj"
	-@erase "$(INTDIR)\HTAAUtil.obj"
	-@erase "$(INTDIR)\HTAccess.obj"
	-@erase "$(INTDIR)\HTAlert.obj"
	-@erase "$(INTDIR)\HTAnchor.obj"
	-@erase "$(INTDIR)\HTANSI.obj"
	-@erase "$(INTDIR)\HTArray.obj"
	-@erase "$(INTDIR)\HTAssoc.obj"
	-@erase "$(INTDIR)\HTAtom.obj"
	-@erase "$(INTDIR)\HTBind.obj"
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTBound.obj"
	-@erase "$(INTDIR)\HTBTree.obj"
	-@erase "$(INTDIR)\HTBufWrt.obj"
	-@erase "$(INTDIR)\HTCache.obj"
	-@erase "$(INTDIR)\HTChannl.obj"
	-@erase "$(INTDIR)\HTChunk.obj"
	-@erase "$(INTDIR)\HTConLen.obj"
	-@erase "$(INTDIR)\HTDemux.obj"
	-@erase "$(INTDIR)\HTDescpt.obj"
	-@erase "$(INTDIR)\HTDialog.obj"
	-@erase "$(INTDIR)\HTDigest.obj"
	-@erase "$(INTDIR)\HTDir.obj"
	-@erase "$(INTDIR)\HTDNS.obj"
	-@erase "$(INTDIR)\HTEPtoCl.obj"
	-@erase "$(INTDIR)\HTError.obj"
	-@erase "$(INTDIR)\HTEscape.obj"
	-@erase "$(INTDIR)\HTEvent.obj"
	-@erase "$(INTDIR)\HTEvtLst.obj"
	-@erase "$(INTDIR)\HTFile.obj"
	-@erase "$(INTDIR)\HTFilter.obj"
	-@erase "$(INTDIR)\HTFormat.obj"
	-@erase "$(INTDIR)\HTFSave.obj"
	-@erase "$(INTDIR)\HTFTP.obj"
	-@erase "$(INTDIR)\HTFTPDir.obj"
	-@erase "$(INTDIR)\HTFWrite.obj"
	-@erase "$(INTDIR)\HTGopher.obj"
	-@erase "$(INTDIR)\HTGuess.obj"
	-@erase "$(INTDIR)\HTHeader.obj"
	-@erase "$(INTDIR)\HTHInit.obj"
	-@erase "$(INTDIR)\HTHist.obj"
	-@erase "$(INTDIR)\HTHome.obj"
	-@erase "$(INTDIR)\HTHost.obj"
	-@erase "$(INTDIR)\HTIcons.obj"
	-@erase "$(INTDIR)\HTInet.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTLib.obj"
	-@erase "$(INTDIR)\HTLink.obj"
	-@erase "$(INTDIR)\HTList.obj"
	-@erase "$(INTDIR)\HTLocal.obj"
	-@erase "$(INTDIR)\HTLog.obj"
	-@erase "$(INTDIR)\HTMemLog.obj"
	-@erase "$(INTDIR)\HTMemory.obj"
	-@erase "$(INTDIR)\HTMerge.obj"
	-@erase "$(INTDIR)\HTMethod.obj"
	-@erase "$(INTDIR)\HTMIME.obj"
	-@erase "$(INTDIR)\HTMIMERq.obj"
	-@erase "$(INTDIR)\HTMIMImp.obj"
	-@erase "$(INTDIR)\HTMIMPrs.obj"
	-@erase "$(INTDIR)\HTML.obj"
	-@erase "$(INTDIR)\HTMLGen.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTMulti.obj"
	-@erase "$(INTDIR)\HTMuxCh.obj"
	-@erase "$(INTDIR)\HTMuxTx.obj"
	-@erase "$(INTDIR)\HTNDir.obj"
	-@erase "$(INTDIR)\HTNet.obj"
	-@erase "$(INTDIR)\HTNetTxt.obj"
	-@erase "$(INTDIR)\HTNews.obj"
	-@erase "$(INTDIR)\HTNewsLs.obj"
	-@erase "$(INTDIR)\HTNewsRq.obj"
	-@erase "$(INTDIR)\HTNoFree.obj"
	-@erase "$(INTDIR)\HTParse.obj"
	-@erase "$(INTDIR)\HTPEP.obj"
	-@erase "$(INTDIR)\HTPlain.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\HTProt.obj"
	-@erase "$(INTDIR)\HTProxy.obj"
	-@erase "$(INTDIR)\HTReader.obj"
	-@erase "$(INTDIR)\HTReqMan.obj"
	-@erase "$(INTDIR)\HTResponse.obj"
	-@erase "$(INTDIR)\HTRules.obj"
	-@erase "$(INTDIR)\HTSChunk.obj"
	-@erase "$(INTDIR)\HTSocket.obj"
	-@erase "$(INTDIR)\HTStream.obj"
	-@erase "$(INTDIR)\HTString.obj"
	-@erase "$(INTDIR)\HTStyle.obj"
	-@erase "$(INTDIR)\HTTChunk.obj"
	-@erase "$(INTDIR)\HTTCP.obj"
	-@erase "$(INTDIR)\HTTee.obj"
	-@erase "$(INTDIR)\HTTelnet.obj"
	-@erase "$(INTDIR)\HTTeXGen.obj"
	-@erase "$(INTDIR)\HTTimer.obj"
	-@erase "$(INTDIR)\HTTP.obj"
	-@erase "$(INTDIR)\HTTPGen.obj"
	-@erase "$(INTDIR)\HTTPReq.obj"
	-@erase "$(INTDIR)\HTTPRes.obj"
	-@erase "$(INTDIR)\HTTPServ.obj"
	-@erase "$(INTDIR)\HTTrace.obj"
	-@erase "$(INTDIR)\HTTrans.obj"
	-@erase "$(INTDIR)\HTUser.obj"
	-@erase "$(INTDIR)\HTUTree.obj"
	-@erase "$(INTDIR)\HTUU.obj"
	-@erase "$(INTDIR)\HTWriter.obj"
	-@erase "$(INTDIR)\HTWSRC.obj"
	-@erase "$(INTDIR)\HTWWWStr.obj"
	-@erase "$(INTDIR)\HTXParse.obj"
	-@erase "$(INTDIR)\HTZip.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libwww.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\..\libwww\modules\md5" /I "..\..\..\libwww\modules\expat\xmlparse" /I "..\..\..\libwww\modules\expat\xmltok" /I "..\..\libpng\zlib" /I "..\..\..\libwww\Library\src" /D "_DEBUG" /D "HT_ZLIB" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libwww.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libwww.lib" 
LIB32_OBJS= \
	"$(INTDIR)\HTAABrow.obj" \
	"$(INTDIR)\HTAAUtil.obj" \
	"$(INTDIR)\HTAccess.obj" \
	"$(INTDIR)\HTAlert.obj" \
	"$(INTDIR)\HTAnchor.obj" \
	"$(INTDIR)\HTANSI.obj" \
	"$(INTDIR)\HTArray.obj" \
	"$(INTDIR)\HTAssoc.obj" \
	"$(INTDIR)\HTAtom.obj" \
	"$(INTDIR)\HTBind.obj" \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTBound.obj" \
	"$(INTDIR)\HTBTree.obj" \
	"$(INTDIR)\HTBufWrt.obj" \
	"$(INTDIR)\HTCache.obj" \
	"$(INTDIR)\HTChannl.obj" \
	"$(INTDIR)\HTChunk.obj" \
	"$(INTDIR)\HTConLen.obj" \
	"$(INTDIR)\HTDemux.obj" \
	"$(INTDIR)\HTDescpt.obj" \
	"$(INTDIR)\HTDialog.obj" \
	"$(INTDIR)\HTDigest.obj" \
	"$(INTDIR)\HTDir.obj" \
	"$(INTDIR)\HTDNS.obj" \
	"$(INTDIR)\HTEPtoCl.obj" \
	"$(INTDIR)\HTError.obj" \
	"$(INTDIR)\HTEscape.obj" \
	"$(INTDIR)\HTEvent.obj" \
	"$(INTDIR)\HTEvtLst.obj" \
	"$(INTDIR)\HTFile.obj" \
	"$(INTDIR)\HTFilter.obj" \
	"$(INTDIR)\HTFormat.obj" \
	"$(INTDIR)\HTFSave.obj" \
	"$(INTDIR)\HTFTP.obj" \
	"$(INTDIR)\HTFTPDir.obj" \
	"$(INTDIR)\HTFWrite.obj" \
	"$(INTDIR)\HTGopher.obj" \
	"$(INTDIR)\HTGuess.obj" \
	"$(INTDIR)\HTHeader.obj" \
	"$(INTDIR)\HTHInit.obj" \
	"$(INTDIR)\HTHist.obj" \
	"$(INTDIR)\HTHome.obj" \
	"$(INTDIR)\HTHost.obj" \
	"$(INTDIR)\HTIcons.obj" \
	"$(INTDIR)\HTInet.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTLib.obj" \
	"$(INTDIR)\HTLink.obj" \
	"$(INTDIR)\HTList.obj" \
	"$(INTDIR)\HTLocal.obj" \
	"$(INTDIR)\HTLog.obj" \
	"$(INTDIR)\HTMemLog.obj" \
	"$(INTDIR)\HTMemory.obj" \
	"$(INTDIR)\HTMerge.obj" \
	"$(INTDIR)\HTMethod.obj" \
	"$(INTDIR)\HTMIME.obj" \
	"$(INTDIR)\HTMIMERq.obj" \
	"$(INTDIR)\HTMIMImp.obj" \
	"$(INTDIR)\HTMIMPrs.obj" \
	"$(INTDIR)\HTML.obj" \
	"$(INTDIR)\HTMLGen.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTMulti.obj" \
	"$(INTDIR)\HTMuxCh.obj" \
	"$(INTDIR)\HTMuxTx.obj" \
	"$(INTDIR)\HTNDir.obj" \
	"$(INTDIR)\HTNet.obj" \
	"$(INTDIR)\HTNetTxt.obj" \
	"$(INTDIR)\HTNews.obj" \
	"$(INTDIR)\HTNewsLs.obj" \
	"$(INTDIR)\HTNewsRq.obj" \
	"$(INTDIR)\HTNoFree.obj" \
	"$(INTDIR)\HTParse.obj" \
	"$(INTDIR)\HTPEP.obj" \
	"$(INTDIR)\HTPlain.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\HTProt.obj" \
	"$(INTDIR)\HTProxy.obj" \
	"$(INTDIR)\HTReader.obj" \
	"$(INTDIR)\HTReqMan.obj" \
	"$(INTDIR)\HTResponse.obj" \
	"$(INTDIR)\HTRules.obj" \
	"$(INTDIR)\HTSChunk.obj" \
	"$(INTDIR)\HTSocket.obj" \
	"$(INTDIR)\HTStream.obj" \
	"$(INTDIR)\HTString.obj" \
	"$(INTDIR)\HTStyle.obj" \
	"$(INTDIR)\HTTChunk.obj" \
	"$(INTDIR)\HTTCP.obj" \
	"$(INTDIR)\HTTee.obj" \
	"$(INTDIR)\HTTelnet.obj" \
	"$(INTDIR)\HTTeXGen.obj" \
	"$(INTDIR)\HTTimer.obj" \
	"$(INTDIR)\HTTP.obj" \
	"$(INTDIR)\HTTPGen.obj" \
	"$(INTDIR)\HTTPReq.obj" \
	"$(INTDIR)\HTTPRes.obj" \
	"$(INTDIR)\HTTPServ.obj" \
	"$(INTDIR)\HTTrace.obj" \
	"$(INTDIR)\HTTrans.obj" \
	"$(INTDIR)\HTUser.obj" \
	"$(INTDIR)\HTUTree.obj" \
	"$(INTDIR)\HTUU.obj" \
	"$(INTDIR)\HTWriter.obj" \
	"$(INTDIR)\HTWSRC.obj" \
	"$(INTDIR)\HTWWWStr.obj" \
	"$(INTDIR)\HTXParse.obj" \
	"$(INTDIR)\HTZip.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\SGML.obj" \
	"$(OUTDIR)\zlib.lib"

"$(OUTDIR)\libwww.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libwww.dep")
!INCLUDE "libwww.dep"
!ELSE 
!MESSAGE Warning: cannot find "libwww.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libwww - Win32 Release" || "$(CFG)" == "libwww - Win32 Debug"

!IF  "$(CFG)" == "libwww - Win32 Release"

"zlib - Win32 Release" : 
   cd "\users\guetari\Amaya\Windows\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" 
   cd "..\libwww"

"zlib - Win32 ReleaseCLEAN" : 
   cd "\users\guetari\Amaya\Windows\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\libwww"

!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

"zlib - Win32 Debug" : 
   cd "\users\guetari\Amaya\Windows\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" 
   cd "..\libwww"

"zlib - Win32 DebugCLEAN" : 
   cd "\users\guetari\Amaya\Windows\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\libwww"

!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAABrow.c

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTAAUtil.c

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTAccess.c

"$(INTDIR)\HTAccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTAlert.c

"$(INTDIR)\HTAlert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTAnchor.c

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTANSI.c

"$(INTDIR)\HTANSI.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTArray.c

"$(INTDIR)\HTArray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTAssoc.c

"$(INTDIR)\HTAssoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTAtom.c

"$(INTDIR)\HTAtom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTBind.c

"$(INTDIR)\HTBind.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTBInit.c

"$(INTDIR)\HTBInit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTBound.c

"$(INTDIR)\HTBound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTBTree.c

"$(INTDIR)\HTBTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTBufWrt.c

"$(INTDIR)\HTBufWrt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTCache.c

"$(INTDIR)\HTCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTChannl.c

"$(INTDIR)\HTChannl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTChunk.c

"$(INTDIR)\HTChunk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTConLen.c

"$(INTDIR)\HTConLen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTDemux.c

"$(INTDIR)\HTDemux.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTDescpt.c

"$(INTDIR)\HTDescpt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTDialog.c

"$(INTDIR)\HTDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTDigest.c

"$(INTDIR)\HTDigest.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTDir.c

"$(INTDIR)\HTDir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTDNS.c

"$(INTDIR)\HTDNS.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTEPtoCl.c

"$(INTDIR)\HTEPtoCl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTError.c

"$(INTDIR)\HTError.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTEscape.c

"$(INTDIR)\HTEscape.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTEvent.c

"$(INTDIR)\HTEvent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTEvtLst.c

"$(INTDIR)\HTEvtLst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTFile.c

"$(INTDIR)\HTFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTFilter.c

"$(INTDIR)\HTFilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTFormat.c

"$(INTDIR)\HTFormat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTFSave.c

"$(INTDIR)\HTFSave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTFTP.c

"$(INTDIR)\HTFTP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTFTPDir.c

"$(INTDIR)\HTFTPDir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTFWrite.c

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTGopher.c

"$(INTDIR)\HTGopher.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTGuess.c

"$(INTDIR)\HTGuess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTHeader.c

"$(INTDIR)\HTHeader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTHInit.c

"$(INTDIR)\HTHInit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTHist.c

"$(INTDIR)\HTHist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTHome.c

"$(INTDIR)\HTHome.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTHost.c

"$(INTDIR)\HTHost.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTIcons.c

"$(INTDIR)\HTIcons.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTInet.c

"$(INTDIR)\HTInet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\..\libwww\modules\md5" /I "..\..\..\libwww\modules\expat\xmlparse" /I "..\..\..\libwww\modules\expat\xmltok" /I "..\..\..\libwww\Library\src" /I "..\..\libpng\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "HT_ZLIB" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\HTInit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\..\libwww\modules\md5" /I "..\..\..\libwww\modules\expat\xmlparse" /I "..\..\..\libwww\modules\expat\xmltok" /I "..\..\libpng\zlib" /I "..\..\..\..\libwww\Library\src" /D "_DEBUG" /D "HT_ZLIB" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\HTInit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLib.c

"$(INTDIR)\HTLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTLink.c

"$(INTDIR)\HTLink.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTList.c

"$(INTDIR)\HTList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTLocal.c

"$(INTDIR)\HTLocal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTLog.c

"$(INTDIR)\HTLog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMemLog.c

"$(INTDIR)\HTMemLog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMemory.c

"$(INTDIR)\HTMemory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMerge.c

"$(INTDIR)\HTMerge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMethod.c

"$(INTDIR)\HTMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMIME.c

"$(INTDIR)\HTMIME.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMIMERq.c

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMIMImp.c

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMIMPrs.c

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTML.c

"$(INTDIR)\HTML.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMLGen.c

"$(INTDIR)\HTMLGen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMLPDTD.c

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMulti.c

"$(INTDIR)\HTMulti.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMuxCh.c

"$(INTDIR)\HTMuxCh.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTMuxTx.c

"$(INTDIR)\HTMuxTx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTNDir.c

"$(INTDIR)\HTNDir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTNet.c

"$(INTDIR)\HTNet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTNetTxt.c

"$(INTDIR)\HTNetTxt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTNews.c

"$(INTDIR)\HTNews.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTNewsLs.c

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTNewsRq.c

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTNoFree.c

"$(INTDIR)\HTNoFree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTParse.c

"$(INTDIR)\HTParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTPEP.c

"$(INTDIR)\HTPEP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTPlain.c

"$(INTDIR)\HTPlain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTProfil.c

"$(INTDIR)\HTProfil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTProt.c

"$(INTDIR)\HTProt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTProxy.c

"$(INTDIR)\HTProxy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTReader.c

"$(INTDIR)\HTReader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTReqMan.c

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTResponse.c

"$(INTDIR)\HTResponse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTRules.c

"$(INTDIR)\HTRules.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTSChunk.c

"$(INTDIR)\HTSChunk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTSocket.c

"$(INTDIR)\HTSocket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTStream.c

"$(INTDIR)\HTStream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTString.c

"$(INTDIR)\HTString.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTStyle.c

"$(INTDIR)\HTStyle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTChunk.c

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTCP.c

"$(INTDIR)\HTTCP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTee.c

"$(INTDIR)\HTTee.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTelnet.c

"$(INTDIR)\HTTelnet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTeXGen.c

"$(INTDIR)\HTTeXGen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTimer.c

"$(INTDIR)\HTTimer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTP.c

"$(INTDIR)\HTTP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTPGen.c

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTPReq.c

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTPRes.c

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTPServ.c

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTrace.c

"$(INTDIR)\HTTrace.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTTrans.c

"$(INTDIR)\HTTrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTUser.c

"$(INTDIR)\HTUser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTUTree.c

"$(INTDIR)\HTUTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTUU.c

"$(INTDIR)\HTUU.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTWriter.c

"$(INTDIR)\HTWriter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTWSRC.c

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTWWWStr.c

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTXParse.c

"$(INTDIR)\HTXParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\HTZip.c

"$(INTDIR)\HTZip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\modules\md5\md5.c

"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\SGML.c

"$(INTDIR)\SGML.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

