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

CPP=cl.exe

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
	-@erase "$(INTDIR)\hashtable.obj"
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
	-@erase "$(INTDIR)\HTRDF.obj"
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
	-@erase "$(INTDIR)\HTXML.obj"
	-@erase "$(INTDIR)\HTXParse.obj"
	-@erase "$(INTDIR)\HTZip.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(OUTDIR)\libwww.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

RSC=rc.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\libwww\modules\md5" /I\
 "..\..\..\libwww\modules\expat\xmlparse" /I\
 "..\..\..\libwww\modules\expat\xmltok" /I "..\..\..\libwww\Library\src" /I\
 "..\..\libpng\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D\
 "HT_ZLIB" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libwww.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libwww.lib" 
LIB32_OBJS= \
	"$(INTDIR)\hashtable.obj" \
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
	"$(INTDIR)\HTRDF.obj" \
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
	"$(INTDIR)\HTXML.obj" \
	"$(INTDIR)\HTXParse.obj" \
	"$(INTDIR)\HTZip.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\SGML.obj" \
	"$(INTDIR)\xmlparse.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj" \
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
	-@erase "$(INTDIR)\hashtable.obj"
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
	-@erase "$(INTDIR)\HTRDF.obj"
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
	-@erase "$(INTDIR)\HTXML.obj"
	-@erase "$(INTDIR)\HTXParse.obj"
	-@erase "$(INTDIR)\HTZip.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(OUTDIR)\libwww.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

RSC=rc.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\..\libwww\modules\md5" /I\
 "..\..\..\libwww\modules\expat\xmlparse" /I\
 "..\..\..\libwww\modules\expat\xmltok" /I "..\..\libpng\zlib" /I\
 "..\..\..\libwww\Library\src" /D "_DEBUG" /D "HT_ZLIB" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libwww.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libwww.lib" 
LIB32_OBJS= \
	"$(INTDIR)\hashtable.obj" \
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
	"$(INTDIR)\HTRDF.obj" \
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
	"$(INTDIR)\HTXML.obj" \
	"$(INTDIR)\HTXParse.obj" \
	"$(INTDIR)\HTZip.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\SGML.obj" \
	"$(INTDIR)\xmlparse.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj" \
	"$(OUTDIR)\zlib.lib"

"$(OUTDIR)\libwww.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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


!IF "$(CFG)" == "libwww - Win32 Release" || "$(CFG)" == "libwww - Win32 Debug"

!IF  "$(CFG)" == "libwww - Win32 Release"

"zlib - Win32 Release" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" 
   cd "..\libwww"

"zlib - Win32 ReleaseCLEAN" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\zlib.mak CFG="zlib - Win32 Release"\
 RECURSE=1 
   cd "..\libwww"

!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

"zlib - Win32 Debug" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" 
   cd "..\libwww"

"zlib - Win32 DebugCLEAN" : 
   cd "..\zlib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\zlib.mak CFG="zlib - Win32 Debug" RECURSE=1\
 
   cd "..\libwww"

!ENDIF 

SOURCE=..\..\..\Libwww\Modules\Expat\Xmlparse\hashtable.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HASHT=\
	"..\..\..\Libwww\Modules\Expat\Xmlparse\hashtable.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	
NODEP_CPP_HASHT=\
	"..\..\..\libwww\modules\expat\xmltok\nspr.h"\
	

"$(INTDIR)\hashtable.obj" : $(SOURCE) $(DEP_CPP_HASHT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HASHT=\
	"..\..\..\Libwww\Modules\Expat\Xmlparse\hashtable.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	

"$(INTDIR)\hashtable.obj" : $(SOURCE) $(DEP_CPP_HASHT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAABrow.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTAAB=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDigest.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\modules\md5\md5.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTAAB=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) $(DEP_CPP_HTAAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTAAB=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDigest.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\modules\md5\md5.h"\
	

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) $(DEP_CPP_HTAAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAAUtil.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTAAU=\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTAAU=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) $(DEP_CPP_HTAAU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTAAU=\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) $(DEP_CPP_HTAAU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAccess.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTACC=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTACC=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAccess.obj" : $(SOURCE) $(DEP_CPP_HTACC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTACC=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTAccess.obj" : $(SOURCE) $(DEP_CPP_HTACC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAlert.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTALE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTALE=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAlert.obj" : $(SOURCE) $(DEP_CPP_HTALE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTALE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTAlert.obj" : $(SOURCE) $(DEP_CPP_HTALE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAnchor.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTANC=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTAncMan.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTANC=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) $(DEP_CPP_HTANC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTANC=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTAncMan.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) $(DEP_CPP_HTANC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTANSI.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTANS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTANS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTANSI.obj" : $(SOURCE) $(DEP_CPP_HTANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTANS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTANSI.obj" : $(SOURCE) $(DEP_CPP_HTANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTArray.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTARR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTARR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTArray.obj" : $(SOURCE) $(DEP_CPP_HTARR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTARR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTArray.obj" : $(SOURCE) $(DEP_CPP_HTARR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAssoc.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTASS=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTASS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAssoc.obj" : $(SOURCE) $(DEP_CPP_HTASS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTASS=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTAssoc.obj" : $(SOURCE) $(DEP_CPP_HTASS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAtom.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTATO=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTATO=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAtom.obj" : $(SOURCE) $(DEP_CPP_HTATO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTATO=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTAtom.obj" : $(SOURCE) $(DEP_CPP_HTATO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBind.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBIN=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBIN=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBind.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBIN=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTBind.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBINI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBINI=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBINI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBound.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBOU=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBOU=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBound.obj" : $(SOURCE) $(DEP_CPP_HTBOU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBOU=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTBound.obj" : $(SOURCE) $(DEP_CPP_HTBOU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBTree.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBTR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTBTree.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBTR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBTree.obj" : $(SOURCE) $(DEP_CPP_HTBTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBTR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTBTree.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTBTree.obj" : $(SOURCE) $(DEP_CPP_HTBTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBufWrt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBUF=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBUF=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBufWrt.obj" : $(SOURCE) $(DEP_CPP_HTBUF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBUF=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTBufWrt.obj" : $(SOURCE) $(DEP_CPP_HTBUF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTCache.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCAC=\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCAC=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTCache.obj" : $(SOURCE) $(DEP_CPP_HTCAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCAC=\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTCache.obj" : $(SOURCE) $(DEP_CPP_HTCAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTChannl.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCHA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDemux.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTMuxTx.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWMux.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCHA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTChannl.obj" : $(SOURCE) $(DEP_CPP_HTCHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCHA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTChannl.obj" : $(SOURCE) $(DEP_CPP_HTCHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTChunk.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCHU=\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCHU=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTChunk.obj" : $(SOURCE) $(DEP_CPP_HTCHU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCHU=\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTChunk.obj" : $(SOURCE) $(DEP_CPP_HTCHU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTConLen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCON=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCON=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTConLen.obj" : $(SOURCE) $(DEP_CPP_HTCON) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCON=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTConLen.obj" : $(SOURCE) $(DEP_CPP_HTCON) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDemux.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDEM=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDemux.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDEM=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDemux.obj" : $(SOURCE) $(DEP_CPP_HTDEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDEM=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDemux.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTDemux.obj" : $(SOURCE) $(DEP_CPP_HTDEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDescpt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDES=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDES=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDescpt.obj" : $(SOURCE) $(DEP_CPP_HTDES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDES=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTDescpt.obj" : $(SOURCE) $(DEP_CPP_HTDES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDialog.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDIA=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDIA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDialog.obj" : $(SOURCE) $(DEP_CPP_HTDIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDIA=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTDialog.obj" : $(SOURCE) $(DEP_CPP_HTDIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDigest.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDIG=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDigest.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\modules\md5\md5.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDIG=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDigest.obj" : $(SOURCE) $(DEP_CPP_HTDIG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDIG=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDigest.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\modules\md5\md5.h"\
	

"$(INTDIR)\HTDigest.obj" : $(SOURCE) $(DEP_CPP_HTDIG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDir.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDIR=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDIR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDir.obj" : $(SOURCE) $(DEP_CPP_HTDIR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDIR=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTDir.obj" : $(SOURCE) $(DEP_CPP_HTDIR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDNS.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDNS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDNS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDNS.obj" : $(SOURCE) $(DEP_CPP_HTDNS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDNS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTDNS.obj" : $(SOURCE) $(DEP_CPP_HTDNS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEPtoCl.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTEPT=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTEPT=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEPtoCl.obj" : $(SOURCE) $(DEP_CPP_HTEPT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTEPT=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTEPtoCl.obj" : $(SOURCE) $(DEP_CPP_HTEPT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTError.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTERR=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTERR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTError.obj" : $(SOURCE) $(DEP_CPP_HTERR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTERR=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTError.obj" : $(SOURCE) $(DEP_CPP_HTERR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEscape.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTESC=\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTESC=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEscape.obj" : $(SOURCE) $(DEP_CPP_HTESC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTESC=\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTEscape.obj" : $(SOURCE) $(DEP_CPP_HTESC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEvent.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTEVE=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTEVE=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvent.obj" : $(SOURCE) $(DEP_CPP_HTEVE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTEVE=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTEvent.obj" : $(SOURCE) $(DEP_CPP_HTEVE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEvtLst.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTEVT=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTEVT=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvtLst.obj" : $(SOURCE) $(DEP_CPP_HTEVT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTEVT=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTEvtLst.obj" : $(SOURCE) $(DEP_CPP_HTEVT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFile.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFIL=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFIL=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFile.obj" : $(SOURCE) $(DEP_CPP_HTFIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFIL=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTFile.obj" : $(SOURCE) $(DEP_CPP_HTFIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFilter.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFILT=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFILT=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFilter.obj" : $(SOURCE) $(DEP_CPP_HTFILT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFILT=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTFilter.obj" : $(SOURCE) $(DEP_CPP_HTFILT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFormat.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFOR=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFOR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFormat.obj" : $(SOURCE) $(DEP_CPP_HTFOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFOR=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTFormat.obj" : $(SOURCE) $(DEP_CPP_HTFOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFSave.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFSA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFSA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFSave.obj" : $(SOURCE) $(DEP_CPP_HTFSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFSA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTFSave.obj" : $(SOURCE) $(DEP_CPP_HTFSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFTP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFTP=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFTP=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTP.obj" : $(SOURCE) $(DEP_CPP_HTFTP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFTP=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTFTP.obj" : $(SOURCE) $(DEP_CPP_HTFTP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFTPDir.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFTPD=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFTPD=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTPDir.obj" : $(SOURCE) $(DEP_CPP_HTFTPD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFTPD=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTFTPDir.obj" : $(SOURCE) $(DEP_CPP_HTFTPD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFWrite.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFWR=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFWR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) $(DEP_CPP_HTFWR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFWR=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) $(DEP_CPP_HTFWR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTGopher.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTGOP=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTGOP=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTGopher.obj" : $(SOURCE) $(DEP_CPP_HTGOP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTGOP=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTGopher.obj" : $(SOURCE) $(DEP_CPP_HTGOP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTGuess.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTGUE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTGUE=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTGuess.obj" : $(SOURCE) $(DEP_CPP_HTGUE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTGUE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTGuess.obj" : $(SOURCE) $(DEP_CPP_HTGUE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHeader.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHEA=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHEA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHeader.obj" : $(SOURCE) $(DEP_CPP_HTHEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHEA=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTHeader.obj" : $(SOURCE) $(DEP_CPP_HTHEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHIN=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHIN=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHIN=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHist.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHIS=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHIS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHist.obj" : $(SOURCE) $(DEP_CPP_HTHIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHIS=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTHist.obj" : $(SOURCE) $(DEP_CPP_HTHIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHome.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHOM=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHOM=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHome.obj" : $(SOURCE) $(DEP_CPP_HTHOM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHOM=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTHome.obj" : $(SOURCE) $(DEP_CPP_HTHOM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHost.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHOS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHOS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHost.obj" : $(SOURCE) $(DEP_CPP_HTHOS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHOS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTHost.obj" : $(SOURCE) $(DEP_CPP_HTHOS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTIcons.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTICO=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTICO=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTIcons.obj" : $(SOURCE) $(DEP_CPP_HTICO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTICO=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTIcons.obj" : $(SOURCE) $(DEP_CPP_HTICO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTInet.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTINE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTINE=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTInet.obj" : $(SOURCE) $(DEP_CPP_HTINE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTINE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTInet.obj" : $(SOURCE) $(DEP_CPP_HTINE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTINI=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDemux.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTInit.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulpar.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTMuxTx.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWAIS.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWFTP.h"\
	"..\..\..\libwww\Library\src\WWWGophe.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\WWWMIME.h"\
	"..\..\..\libwww\Library\src\WWWMux.h"\
	"..\..\..\libwww\Library\src\WWWNews.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTelnt.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\Library\src\WWWWAIS.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\Library\src\WWWZip.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTINI=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	
CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\..\libwww\modules\md5" /I\
 "..\..\..\libwww\modules\expat\xmlparse" /I\
 "..\..\..\libwww\modules\expat\xmltok" /I "..\..\..\libwww\Library\src" /I\
 "..\..\libpng\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D\
 "HT_ZLIB" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTINI=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTInit.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulpar.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWFTP.h"\
	"..\..\..\libwww\Library\src\WWWGophe.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\WWWMIME.h"\
	"..\..\..\libwww\Library\src\WWWNews.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTelnt.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\Library\src\WWWZip.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	
CPP_SWITCHES=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\..\libwww\modules\md5" /I\
 "..\..\..\libwww\modules\expat\xmlparse" /I\
 "..\..\..\libwww\modules\expat\xmltok" /I "..\..\libpng\zlib" /I\
 "..\..\..\..\libwww\Library\src" /D "_DEBUG" /D "HT_ZLIB" /D "WIN32" /D\
 "_WINDOWS" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLib.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLIB=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIB=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLib.obj" : $(SOURCE) $(DEP_CPP_HTLIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLIB=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTLib.obj" : $(SOURCE) $(DEP_CPP_HTLIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLink.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLIN=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTAncMan.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIN=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLink.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLIN=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTAncMan.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTLink.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTList.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLIS=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTList.obj" : $(SOURCE) $(DEP_CPP_HTLIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLIS=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTList.obj" : $(SOURCE) $(DEP_CPP_HTLIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLocal.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLOC=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLOC=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLocal.obj" : $(SOURCE) $(DEP_CPP_HTLOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLOC=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTLocal.obj" : $(SOURCE) $(DEP_CPP_HTLOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLog.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLOG=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLOG=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLog.obj" : $(SOURCE) $(DEP_CPP_HTLOG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLOG=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTLog.obj" : $(SOURCE) $(DEP_CPP_HTLOG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMemLog.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMEM=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMEM=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemLog.obj" : $(SOURCE) $(DEP_CPP_HTMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMEM=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMemLog.obj" : $(SOURCE) $(DEP_CPP_HTMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMemory.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMEMO=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMEMO=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemory.obj" : $(SOURCE) $(DEP_CPP_HTMEMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMEMO=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTMemory.obj" : $(SOURCE) $(DEP_CPP_HTMEMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMerge.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMER=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMER=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMerge.obj" : $(SOURCE) $(DEP_CPP_HTMER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMER=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMerge.obj" : $(SOURCE) $(DEP_CPP_HTMER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMethod.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMET=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMET=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMethod.obj" : $(SOURCE) $(DEP_CPP_HTMET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMET=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTMethod.obj" : $(SOURCE) $(DEP_CPP_HTMET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIME.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIM=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIM=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIME.obj" : $(SOURCE) $(DEP_CPP_HTMIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIM=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMIME.obj" : $(SOURCE) $(DEP_CPP_HTMIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIMERq.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIME=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTAncMan.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIME=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) $(DEP_CPP_HTMIME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIME=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTAncMan.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) $(DEP_CPP_HTMIME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIMImp.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIMI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIMI=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) $(DEP_CPP_HTMIMI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIMI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) $(DEP_CPP_HTMIMI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIMPrs.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIMP=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIMP=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) $(DEP_CPP_HTMIMP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIMP=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) $(DEP_CPP_HTMIMP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTML.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTML_=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTextImp.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTML_=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTML_=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTextImp.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMLGen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMLG=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMLG=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLGen.obj" : $(SOURCE) $(DEP_CPP_HTMLG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMLG=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMLGen.obj" : $(SOURCE) $(DEP_CPP_HTMLG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMLPDTD.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMLP=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMLP=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMLP=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMulti.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMUL=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMUL=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMulti.obj" : $(SOURCE) $(DEP_CPP_HTMUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMUL=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMulti.obj" : $(SOURCE) $(DEP_CPP_HTMUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMuxCh.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMUX=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDemux.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTMuxTx.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMUX=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMuxCh.obj" : $(SOURCE) $(DEP_CPP_HTMUX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMUX=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDemux.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTMuxTx.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMuxCh.obj" : $(SOURCE) $(DEP_CPP_HTMUX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMuxTx.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMUXT=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTMuxTx.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMUXT=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMuxTx.obj" : $(SOURCE) $(DEP_CPP_HTMUXT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMUXT=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMuxCh.h"\
	"..\..\..\libwww\Library\src\HTMuxHeader.h"\
	"..\..\..\libwww\Library\src\HTMuxTx.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTMuxTx.obj" : $(SOURCE) $(DEP_CPP_HTMUXT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNDir.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNDI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNDI=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNDir.obj" : $(SOURCE) $(DEP_CPP_HTNDI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNDI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTNDir.obj" : $(SOURCE) $(DEP_CPP_HTNDI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNet.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNET=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNET=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNet.obj" : $(SOURCE) $(DEP_CPP_HTNET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNET=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTNet.obj" : $(SOURCE) $(DEP_CPP_HTNET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNetTxt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNETT=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNETT=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNetTxt.obj" : $(SOURCE) $(DEP_CPP_HTNETT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNETT=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTNetTxt.obj" : $(SOURCE) $(DEP_CPP_HTNETT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNews.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNEW=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNEW=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNews.obj" : $(SOURCE) $(DEP_CPP_HTNEW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNEW=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTNews.obj" : $(SOURCE) $(DEP_CPP_HTNEW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNewsLs.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNEWS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNEWS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) $(DEP_CPP_HTNEWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNEWS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) $(DEP_CPP_HTNEWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNewsRq.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNEWSR=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNEWSR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) $(DEP_CPP_HTNEWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNEWSR=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) $(DEP_CPP_HTNEWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNoFree.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNOF=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNOF=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNoFree.obj" : $(SOURCE) $(DEP_CPP_HTNOF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNOF=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTNoFree.obj" : $(SOURCE) $(DEP_CPP_HTNOF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTParse.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPAR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPAR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTParse.obj" : $(SOURCE) $(DEP_CPP_HTPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPAR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTParse.obj" : $(SOURCE) $(DEP_CPP_HTPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTPEP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPEP=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPEP=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTPEP.obj" : $(SOURCE) $(DEP_CPP_HTPEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPEP=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTPEP.obj" : $(SOURCE) $(DEP_CPP_HTPEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTPlain.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPLA=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTextImp.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPLA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPLA=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTextImp.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTProfil.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPRO=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTInit.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulpar.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProfil.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWAIS.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWFTP.h"\
	"..\..\..\libwww\Library\src\WWWGophe.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWInit.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\WWWMIME.h"\
	"..\..\..\libwww\Library\src\WWWNews.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTelnt.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\Library\src\WWWWAIS.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\Library\src\WWWZip.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPRO=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPRO=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTInit.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulpar.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProfil.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWFTP.h"\
	"..\..\..\libwww\Library\src\WWWGophe.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWInit.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\WWWMIME.h"\
	"..\..\..\libwww\Library\src\WWWNews.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTelnt.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\Library\src\WWWZip.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTProt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPROT=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPROT=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProt.obj" : $(SOURCE) $(DEP_CPP_HTPROT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPROT=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTProt.obj" : $(SOURCE) $(DEP_CPP_HTPROT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTProxy.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPROX=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPROX=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProxy.obj" : $(SOURCE) $(DEP_CPP_HTPROX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPROX=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTProxy.obj" : $(SOURCE) $(DEP_CPP_HTPROX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Libwww\Library\Src\HTRDF.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTRDF=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTInit.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulpar.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProfil.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWAIS.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWFTP.h"\
	"..\..\..\libwww\Library\src\WWWGophe.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWInit.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\WWWMIME.h"\
	"..\..\..\libwww\Library\src\WWWNews.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTelnt.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\Library\src\WWWWAIS.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\Library\src\WWWZip.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTRDF=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTRDF.obj" : $(SOURCE) $(DEP_CPP_HTRDF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTRDF=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAAUtil.h"\
	"..\..\..\libwww\Library\src\HTAccess.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTBound.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTCache.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTCookie.h"\
	"..\..\..\libwww\Library\src\HTDescpt.h"\
	"..\..\..\libwww\Library\src\HTDialog.h"\
	"..\..\..\libwww\Library\src\HTDir.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTEvtLst.h"\
	"..\..\..\libwww\Library\src\HText.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFilter.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFTP.h"\
	"..\..\..\libwww\Library\src\HTFTPDir.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGopher.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHash.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHInit.h"\
	"..\..\..\libwww\Library\src\HTHist.h"\
	"..\..\..\libwww\Library\src\HTHome.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTIcons.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTInit.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTLog.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIME.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMImp.h"\
	"..\..\..\libwww\Library\src\HTML.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTMulpar.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNDir.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNews.h"\
	"..\..\..\libwww\Library\src\HTNewsLs.h"\
	"..\..\..\libwww\Library\src\HTNewsRq.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTPEP.h"\
	"..\..\..\libwww\Library\src\HTPlain.h"\
	"..\..\..\libwww\Library\src\HTProfil.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWApp.h"\
	"..\..\..\libwww\Library\src\WWWCache.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWDir.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWFTP.h"\
	"..\..\..\libwww\Library\src\WWWGophe.h"\
	"..\..\..\libwww\Library\src\WWWHTML.h"\
	"..\..\..\libwww\Library\src\WWWHTTP.h"\
	"..\..\..\libwww\Library\src\WWWInit.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\WWWMIME.h"\
	"..\..\..\libwww\Library\src\WWWNews.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTelnt.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\Library\src\WWWZip.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	

"$(INTDIR)\HTRDF.obj" : $(SOURCE) $(DEP_CPP_HTRDF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTReader.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTREA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTREA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTReader.obj" : $(SOURCE) $(DEP_CPP_HTREA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTREA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTReader.obj" : $(SOURCE) $(DEP_CPP_HTREA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTReqMan.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTREQ=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTREQ=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) $(DEP_CPP_HTREQ) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTREQ=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) $(DEP_CPP_HTREQ) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTResponse.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTRES=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTRES=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTResponse.obj" : $(SOURCE) $(DEP_CPP_HTRES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTRES=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTResponse.obj" : $(SOURCE) $(DEP_CPP_HTRES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTRules.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTRUL=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTRUL=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTRules.obj" : $(SOURCE) $(DEP_CPP_HTRUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTRUL=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBind.h"\
	"..\..\..\libwww\Library\src\HTBInit.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFile.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMulti.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTRules.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWFile.h"\
	"..\..\..\libwww\Library\src\WWWLib.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTRules.obj" : $(SOURCE) $(DEP_CPP_HTRUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTSChunk.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSCH=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSCH=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTSChunk.obj" : $(SOURCE) $(DEP_CPP_HTSCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSCH=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTSChunk.obj" : $(SOURCE) $(DEP_CPP_HTSCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTSocket.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSOC=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSOC=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTSocket.obj" : $(SOURCE) $(DEP_CPP_HTSOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSOC=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTANSI.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTBufWrt.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTLocal.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSocket.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWTrans.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTSocket.obj" : $(SOURCE) $(DEP_CPP_HTSOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTStream.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSTR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSTR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTStream.obj" : $(SOURCE) $(DEP_CPP_HTSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSTR=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTStream.obj" : $(SOURCE) $(DEP_CPP_HTSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTString.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSTRI=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSTRI=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTString.obj" : $(SOURCE) $(DEP_CPP_HTSTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSTRI=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTString.obj" : $(SOURCE) $(DEP_CPP_HTSTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTStyle.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSTY=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSTY=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSTY=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStyle.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTChunk.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTCH=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTCH=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) $(DEP_CPP_HTTCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTCH=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTChunk.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) $(DEP_CPP_HTTCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTCP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTCP=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTCP=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTCP.obj" : $(SOURCE) $(DEP_CPP_HTTCP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTCP=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTCP.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTCP.obj" : $(SOURCE) $(DEP_CPP_HTTCP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTee.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTEE=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTEE=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTee.obj" : $(SOURCE) $(DEP_CPP_HTTEE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTEE=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTee.obj" : $(SOURCE) $(DEP_CPP_HTTEE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTelnet.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTEL=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTEL=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTelnet.obj" : $(SOURCE) $(DEP_CPP_HTTEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTEL=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTelnet.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTelnet.obj" : $(SOURCE) $(DEP_CPP_HTTEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTeXGen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTEX=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTEX=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTeXGen.obj" : $(SOURCE) $(DEP_CPP_HTTEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTEX=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTeXGen.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTeXGen.obj" : $(SOURCE) $(DEP_CPP_HTTEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTimer.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTIM=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTIM=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTimer.obj" : $(SOURCE) $(DEP_CPP_HTTIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTIM=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTimer.obj" : $(SOURCE) $(DEP_CPP_HTTIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTP_=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTP_=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTP.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTP_=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTConLen.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTFSave.h"\
	"..\..\..\libwww\Library\src\HTFWrite.h"\
	"..\..\..\libwww\Library\src\HTGuess.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMerge.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNetTxt.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTSChunk.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTee.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTP.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\WWWStream.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTP.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPGen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPG=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPG=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) $(DEP_CPP_HTTPG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPG=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) $(DEP_CPP_HTTPG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPReq.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPR=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPR=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPReq.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPRes.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPRE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPRE=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) $(DEP_CPP_HTTPRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPRE=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPGen.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) $(DEP_CPP_HTTPRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPServ.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPS=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPS=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMERq.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTPRes.h"\
	"..\..\..\libwww\Library\src\HTTPServ.h"\
	"..\..\..\libwww\Library\src\HTTPUtil.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTrace.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTRA=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTRA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrace.obj" : $(SOURCE) $(DEP_CPP_HTTRA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTRA=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTTrace.obj" : $(SOURCE) $(DEP_CPP_HTTRA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTrans.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTRAN=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTRAN=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrans.obj" : $(SOURCE) $(DEP_CPP_HTTRAN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTRAN=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTTrans.obj" : $(SOURCE) $(DEP_CPP_HTTRAN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTUser.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTUSE=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTUSE=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTUser.obj" : $(SOURCE) $(DEP_CPP_HTUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTUSE=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTUser.obj" : $(SOURCE) $(DEP_CPP_HTUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTUTree.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTUTR=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTUTR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTUTree.obj" : $(SOURCE) $(DEP_CPP_HTUTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTUTR=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTUTree.obj" : $(SOURCE) $(DEP_CPP_HTUTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTUU.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTUU_=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTUU_=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTUU.obj" : $(SOURCE) $(DEP_CPP_HTUU_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTUU_=\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\HTUU.obj" : $(SOURCE) $(DEP_CPP_HTUU_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTWriter.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTWRI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTWRI=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTWriter.obj" : $(SOURCE) $(DEP_CPP_HTWRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTWRI=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNetMan.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWriter.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTWriter.obj" : $(SOURCE) $(DEP_CPP_HTWRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTWSRC.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTWSR=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWSRC.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTWSR=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) $(DEP_CPP_HTWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTWSR=\
	"..\..\..\libwww\Library\src\HTAABrow.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHeader.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTMIMPrs.h"\
	"..\..\..\libwww\Library\src\HTMLGen.h"\
	"..\..\..\libwww\Library\src\HTMLPDTD.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTProxy.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTReqMan.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWSRC.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) $(DEP_CPP_HTWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTWWWStr.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTWWW=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTWWW=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) $(DEP_CPP_HTWWW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTWWW=\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) $(DEP_CPP_HTWWW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Libwww\Library\Src\HTXML.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTXML=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTXML=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTXML.obj" : $(SOURCE) $(DEP_CPP_HTXML) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTXML=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXML.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	

"$(INTDIR)\HTXML.obj" : $(SOURCE) $(DEP_CPP_HTXML) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTXParse.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTXPA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTXPA=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTXParse.obj" : $(SOURCE) $(DEP_CPP_HTXPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTXPA=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTEPtoCl.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReader.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTXParse.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	

"$(INTDIR)\HTXParse.obj" : $(SOURCE) $(DEP_CPP_HTXPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTZip.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTZIP=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTZIP=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTZip.obj" : $(SOURCE) $(DEP_CPP_HTZIP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTZIP=\
	"..\..\..\libwww\Library\src\HTAlert.h"\
	"..\..\..\libwww\Library\src\HTAnchor.h"\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTAssoc.h"\
	"..\..\..\libwww\Library\src\HTAtom.h"\
	"..\..\..\libwww\Library\src\HTChannl.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTDNS.h"\
	"..\..\..\libwww\Library\src\HTError.h"\
	"..\..\..\libwww\Library\src\HTEscape.h"\
	"..\..\..\libwww\Library\src\HTEvent.h"\
	"..\..\..\libwww\Library\src\HTFormat.h"\
	"..\..\..\libwww\Library\src\HTHost.h"\
	"..\..\..\libwww\Library\src\HTHstMan.h"\
	"..\..\..\libwww\Library\src\HTInet.h"\
	"..\..\..\libwww\Library\src\HTIOStream.h"\
	"..\..\..\libwww\Library\src\HTLib.h"\
	"..\..\..\libwww\Library\src\HTLink.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemLog.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTMethod.h"\
	"..\..\..\libwww\Library\src\HTNet.h"\
	"..\..\..\libwww\Library\src\HTNoFree.h"\
	"..\..\..\libwww\Library\src\HTParse.h"\
	"..\..\..\libwww\Library\src\HTProt.h"\
	"..\..\..\libwww\Library\src\HTReq.h"\
	"..\..\..\libwww\Library\src\HTResponse.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTTimer.h"\
	"..\..\..\libwww\Library\src\HTTrans.h"\
	"..\..\..\libwww\Library\src\HTUser.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\HTUTree.h"\
	"..\..\..\libwww\Library\src\HTUU.h"\
	"..\..\..\libwww\Library\src\HTWWWStr.h"\
	"..\..\..\libwww\Library\src\HTZip.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWCore.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	"..\..\..\libwww\Library\src\WWWUtil.h"\
	"..\..\libpng\zlib\zconf.h"\
	"..\..\libpng\zlib\zlib.h"\
	

"$(INTDIR)\HTZip.obj" : $(SOURCE) $(DEP_CPP_HTZIP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\modules\md5\md5.c
DEP_CPP_MD5_C=\
	"..\..\..\libwww\modules\md5\md5.h"\
	

"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\libwww\Library\src\SGML.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_SGML_=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SGML_=\
	"..\..\..\libwww\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_SGML_=\
	"..\..\..\libwww\Library\src\HTArray.h"\
	"..\..\..\libwww\Library\src\HTChunk.h"\
	"..\..\..\libwww\Library\src\HTList.h"\
	"..\..\..\libwww\Library\src\HTMemory.h"\
	"..\..\..\libwww\Library\src\HTStream.h"\
	"..\..\..\libwww\Library\src\HTString.h"\
	"..\..\..\libwww\Library\src\HTStruct.h"\
	"..\..\..\libwww\Library\src\HTUtils.h"\
	"..\..\..\libwww\Library\src\SGML.h"\
	"..\..\..\libwww\Library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\wwwsys.h"\
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Libwww\Modules\Expat\Xmlparse\xmlparse.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_XMLPA=\
	"..\..\..\Libwww\Modules\Expat\Xmlparse\hashtable.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmlrole.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmltok.h"\
	
NODEP_CPP_XMLPA=\
	"..\..\..\libwww\modules\expat\xmltok\nspr.h"\
	

"$(INTDIR)\xmlparse.obj" : $(SOURCE) $(DEP_CPP_XMLPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_XMLPA=\
	"..\..\..\Libwww\Modules\Expat\Xmlparse\hashtable.h"\
	"..\..\..\libwww\modules\expat\xmlparse\xmlparse.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmlrole.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmltok.h"\
	

"$(INTDIR)\xmlparse.obj" : $(SOURCE) $(DEP_CPP_XMLPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Libwww\Modules\Expat\Xmltok\xmlrole.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_XMLRO=\
	"..\..\..\Libwww\Modules\Expat\Xmltok\ascii.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmlrole.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmltok.h"\
	
NODEP_CPP_XMLRO=\
	"..\..\..\libwww\modules\expat\xmltok\nspr.h"\
	

"$(INTDIR)\xmlrole.obj" : $(SOURCE) $(DEP_CPP_XMLRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_XMLRO=\
	"..\..\..\Libwww\Modules\Expat\Xmltok\ascii.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmlrole.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmltok.h"\
	

"$(INTDIR)\xmlrole.obj" : $(SOURCE) $(DEP_CPP_XMLRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Libwww\Modules\Expat\Xmltok\xmltok.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_XMLTO=\
	"..\..\..\Libwww\Modules\Expat\Xmltok\ascii.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\asciitab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\iasciitab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\latin1tab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\nametab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\utf8tab.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmltok.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\xmltok_impl.c"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\xmltok_impl.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\xmltok_ns.c"\
	
NODEP_CPP_XMLTO=\
	"..\..\..\libwww\modules\expat\xmltok\nspr.h"\
	

"$(INTDIR)\xmltok.obj" : $(SOURCE) $(DEP_CPP_XMLTO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_XMLTO=\
	"..\..\..\Libwww\Modules\Expat\Xmltok\ascii.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\asciitab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\iasciitab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\latin1tab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\nametab.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\utf8tab.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmldef.h"\
	"..\..\..\libwww\modules\expat\xmltok\xmltok.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\xmltok_impl.c"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\xmltok_impl.h"\
	"..\..\..\Libwww\Modules\Expat\Xmltok\xmltok_ns.c"\
	

"$(INTDIR)\xmltok.obj" : $(SOURCE) $(DEP_CPP_XMLTO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

