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
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\libwww.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\libwww\Library\src" /D "NDEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\libwww.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\libwww.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\..\libwww\modules\md5" /I\
 "..\..\..\libwww\modules\expat\xmlparse" /I\
 "..\..\..\libwww\modules\expat\xmltok" /I "..\..\..\libwww\Library\src" /I\
 "..\..\libpng\zlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D\
 "HT_ZLIB" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
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

SOURCE=..\..\..\libwww\Library\src\HTAABrow.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTAAB=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdigest.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTAAB=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	"..\..\..\libwww\library\src\md5.h"\
	

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) $(DEP_CPP_HTAAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTAAB=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdigest.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\modules\md5\md5.h"\
	

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) $(DEP_CPP_HTAAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAAUtil.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTAAU=\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTAAU=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) $(DEP_CPP_HTAAU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTAAU=\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) $(DEP_CPP_HTAAU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAccess.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTACC=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTACC=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAccess.obj" : $(SOURCE) $(DEP_CPP_HTACC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTACC=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTAccess.obj" : $(SOURCE) $(DEP_CPP_HTACC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAlert.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTALE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTALE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAlert.obj" : $(SOURCE) $(DEP_CPP_HTALE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTALE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTAlert.obj" : $(SOURCE) $(DEP_CPP_HTALE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAnchor.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTANC=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTANC=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) $(DEP_CPP_HTANC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTANC=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) $(DEP_CPP_HTANC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTANSI.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTANS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTANS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTANSI.obj" : $(SOURCE) $(DEP_CPP_HTANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTANS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTANSI.obj" : $(SOURCE) $(DEP_CPP_HTANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTArray.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTARR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTARR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTArray.obj" : $(SOURCE) $(DEP_CPP_HTARR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTARR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTArray.obj" : $(SOURCE) $(DEP_CPP_HTARR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAssoc.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTASS=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTASS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAssoc.obj" : $(SOURCE) $(DEP_CPP_HTASS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTASS=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTAssoc.obj" : $(SOURCE) $(DEP_CPP_HTASS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTAtom.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTATO=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTATO=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTAtom.obj" : $(SOURCE) $(DEP_CPP_HTATO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTATO=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTAtom.obj" : $(SOURCE) $(DEP_CPP_HTATO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBind.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBIN=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBIN=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBind.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBIN=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTBind.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBINI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBINI=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBINI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBound.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBOU=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBOU=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBound.obj" : $(SOURCE) $(DEP_CPP_HTBOU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBOU=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbound.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTBound.obj" : $(SOURCE) $(DEP_CPP_HTBOU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBTree.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBTR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htbtree.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBTR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBTree.obj" : $(SOURCE) $(DEP_CPP_HTBTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBTR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htbtree.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTBTree.obj" : $(SOURCE) $(DEP_CPP_HTBTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTBufWrt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTBUF=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBUF=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBufWrt.obj" : $(SOURCE) $(DEP_CPP_HTBUF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTBUF=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTBufWrt.obj" : $(SOURCE) $(DEP_CPP_HTBUF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTCache.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCAC=\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCAC=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTCache.obj" : $(SOURCE) $(DEP_CPP_HTCAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCAC=\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTCache.obj" : $(SOURCE) $(DEP_CPP_HTCAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTChannl.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCHA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdemux.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htmuxtx.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\Library\src\WWWMux.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCHA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTChannl.obj" : $(SOURCE) $(DEP_CPP_HTCHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCHA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTChannl.obj" : $(SOURCE) $(DEP_CPP_HTCHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTChunk.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCHU=\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCHU=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTChunk.obj" : $(SOURCE) $(DEP_CPP_HTCHU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCHU=\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTChunk.obj" : $(SOURCE) $(DEP_CPP_HTCHU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTConLen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTCON=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTCON=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTConLen.obj" : $(SOURCE) $(DEP_CPP_HTCON) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTCON=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTConLen.obj" : $(SOURCE) $(DEP_CPP_HTCON) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDemux.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDEM=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdemux.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDEM=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDemux.obj" : $(SOURCE) $(DEP_CPP_HTDEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDEM=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdemux.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTDemux.obj" : $(SOURCE) $(DEP_CPP_HTDEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDescpt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDES=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDES=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDescpt.obj" : $(SOURCE) $(DEP_CPP_HTDES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDES=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTDescpt.obj" : $(SOURCE) $(DEP_CPP_HTDES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDialog.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDIA=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
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
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDIA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDialog.obj" : $(SOURCE) $(DEP_CPP_HTDIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDIA=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
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
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTDialog.obj" : $(SOURCE) $(DEP_CPP_HTDIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDigest.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDIG=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdigest.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDIG=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	"..\..\..\libwww\library\src\md5.h"\
	

"$(INTDIR)\HTDigest.obj" : $(SOURCE) $(DEP_CPP_HTDIG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDIG=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdigest.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\modules\md5\md5.h"\
	

"$(INTDIR)\HTDigest.obj" : $(SOURCE) $(DEP_CPP_HTDIG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDir.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDIR=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDIR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDir.obj" : $(SOURCE) $(DEP_CPP_HTDIR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDIR=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTDir.obj" : $(SOURCE) $(DEP_CPP_HTDIR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTDNS.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTDNS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTDNS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTDNS.obj" : $(SOURCE) $(DEP_CPP_HTDNS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTDNS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTDNS.obj" : $(SOURCE) $(DEP_CPP_HTDNS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEPtoCl.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTEPT=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTEPT=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEPtoCl.obj" : $(SOURCE) $(DEP_CPP_HTEPT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTEPT=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTEPtoCl.obj" : $(SOURCE) $(DEP_CPP_HTEPT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTError.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTERR=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTERR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTError.obj" : $(SOURCE) $(DEP_CPP_HTERR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTERR=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTError.obj" : $(SOURCE) $(DEP_CPP_HTERR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEscape.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTESC=\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTESC=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEscape.obj" : $(SOURCE) $(DEP_CPP_HTESC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTESC=\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTEscape.obj" : $(SOURCE) $(DEP_CPP_HTESC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEvent.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTEVE=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTEVE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvent.obj" : $(SOURCE) $(DEP_CPP_HTEVE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTEVE=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTEvent.obj" : $(SOURCE) $(DEP_CPP_HTEVE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTEvtLst.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTEVT=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTEVT=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvtLst.obj" : $(SOURCE) $(DEP_CPP_HTEVT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTEVT=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTEvtLst.obj" : $(SOURCE) $(DEP_CPP_HTEVT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFile.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFIL=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFIL=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFile.obj" : $(SOURCE) $(DEP_CPP_HTFIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFIL=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTFile.obj" : $(SOURCE) $(DEP_CPP_HTFIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFilter.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFILT=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
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
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFILT=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFilter.obj" : $(SOURCE) $(DEP_CPP_HTFILT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFILT=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
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
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTFilter.obj" : $(SOURCE) $(DEP_CPP_HTFILT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFormat.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFOR=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFOR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFormat.obj" : $(SOURCE) $(DEP_CPP_HTFOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFOR=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTFormat.obj" : $(SOURCE) $(DEP_CPP_HTFOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFSave.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFSA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFSA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFSave.obj" : $(SOURCE) $(DEP_CPP_HTFSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFSA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTFSave.obj" : $(SOURCE) $(DEP_CPP_HTFSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFTP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFTP=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFTP=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTP.obj" : $(SOURCE) $(DEP_CPP_HTFTP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFTP=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTFTP.obj" : $(SOURCE) $(DEP_CPP_HTFTP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFTPDir.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFTPD=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFTPD=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTPDir.obj" : $(SOURCE) $(DEP_CPP_HTFTPD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFTPD=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htftp.h"\
	"..\..\..\libwww\library\src\htftpdir.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTFTPDir.obj" : $(SOURCE) $(DEP_CPP_HTFTPD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTFWrite.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTFWR=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTFWR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) $(DEP_CPP_HTFWR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTFWR=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) $(DEP_CPP_HTFWR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTGopher.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTGOP=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htgopher.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTGOP=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTGopher.obj" : $(SOURCE) $(DEP_CPP_HTGOP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTGOP=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdescpt.h"\
	"..\..\..\libwww\library\src\htdir.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htgopher.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTGopher.obj" : $(SOURCE) $(DEP_CPP_HTGOP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTGuess.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTGUE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTGUE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTGuess.obj" : $(SOURCE) $(DEP_CPP_HTGUE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTGUE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTGuess.obj" : $(SOURCE) $(DEP_CPP_HTGUE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHeader.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHEA=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHEA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHeader.obj" : $(SOURCE) $(DEP_CPP_HTHEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHEA=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTHeader.obj" : $(SOURCE) $(DEP_CPP_HTHEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHIN=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHIN=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHIN=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHist.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHIS=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHIS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHist.obj" : $(SOURCE) $(DEP_CPP_HTHIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHIS=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTHist.obj" : $(SOURCE) $(DEP_CPP_HTHIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHome.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHOM=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHOM=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHome.obj" : $(SOURCE) $(DEP_CPP_HTHOM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHOM=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTHome.obj" : $(SOURCE) $(DEP_CPP_HTHOM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTHost.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTHOS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHOS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHost.obj" : $(SOURCE) $(DEP_CPP_HTHOS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTHOS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTHost.obj" : $(SOURCE) $(DEP_CPP_HTHOS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTIcons.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTICO=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTICO=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTIcons.obj" : $(SOURCE) $(DEP_CPP_HTICO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTICO=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTIcons.obj" : $(SOURCE) $(DEP_CPP_HTICO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTInet.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTINE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTINE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTInet.obj" : $(SOURCE) $(DEP_CPP_HTINE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTINE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTInet.obj" : $(SOURCE) $(DEP_CPP_HTINE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTINI=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
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
	"..\..\..\libwww\library\src\htdemux.h"\
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
	"..\..\..\libwww\library\src\htgopher.h"\
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
	"..\..\..\libwww\library\src\htinit.h"\
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
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htmuxtx.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
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
	"..\..\..\libwww\library\src\httelnet.h"\
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
	"..\..\..\libwww\library\src\HTWAIS.h"\
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
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwgophe.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\Library\src\WWWMux.h"\
	"..\..\..\libwww\library\src\wwwnews.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtelnt.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\WWWWAIS.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTINI=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	
CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\..\libwww\Library\src" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\libwww.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTINI=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
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
	"..\..\..\libwww\library\src\htgopher.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htinit.h"\
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
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
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
	"..\..\..\libwww\library\src\httelnet.h"\
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
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwgophe.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwnews.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtelnt.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	
CPP_SWITCHES=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\..\libwww\modules\md5" /I\
 "..\..\..\libwww\modules\expat\xmlparse" /I\
 "..\..\..\libwww\modules\expat\xmltok" /I "..\..\libpng\zlib" /I\
 "..\..\..\..\libwww\Library\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_ASYNC" /D "HT_ZLIB" /Fp"$(INTDIR)\libwww.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLib.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLIB=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIB=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLib.obj" : $(SOURCE) $(DEP_CPP_HTLIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLIB=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTLib.obj" : $(SOURCE) $(DEP_CPP_HTLIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLink.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLIN=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIN=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLink.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLIN=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTLink.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTList.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLIS=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTList.obj" : $(SOURCE) $(DEP_CPP_HTLIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLIS=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTList.obj" : $(SOURCE) $(DEP_CPP_HTLIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLocal.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLOC=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLOC=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLocal.obj" : $(SOURCE) $(DEP_CPP_HTLOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLOC=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTLocal.obj" : $(SOURCE) $(DEP_CPP_HTLOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTLog.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTLOG=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLOG=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLog.obj" : $(SOURCE) $(DEP_CPP_HTLOG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTLOG=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTLog.obj" : $(SOURCE) $(DEP_CPP_HTLOG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMemLog.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMEM=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMEM=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemLog.obj" : $(SOURCE) $(DEP_CPP_HTMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMEM=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMemLog.obj" : $(SOURCE) $(DEP_CPP_HTMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMemory.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMEMO=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMEMO=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemory.obj" : $(SOURCE) $(DEP_CPP_HTMEMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMEMO=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTMemory.obj" : $(SOURCE) $(DEP_CPP_HTMEMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMerge.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMER=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMER=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMerge.obj" : $(SOURCE) $(DEP_CPP_HTMER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMER=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMerge.obj" : $(SOURCE) $(DEP_CPP_HTMER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMethod.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMET=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMET=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMethod.obj" : $(SOURCE) $(DEP_CPP_HTMET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMET=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTMethod.obj" : $(SOURCE) $(DEP_CPP_HTMET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIME.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIM=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIM=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIME.obj" : $(SOURCE) $(DEP_CPP_HTMIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIM=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htcache.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmime.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMIME.obj" : $(SOURCE) $(DEP_CPP_HTMIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIMERq.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIME=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIME=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) $(DEP_CPP_HTMIME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIME=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htancman.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) $(DEP_CPP_HTMIME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIMImp.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIMI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIMI=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) $(DEP_CPP_HTMIMI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIMI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimimp.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) $(DEP_CPP_HTMIMI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMIMPrs.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMIMP=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMIMP=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) $(DEP_CPP_HTMIMP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMIMP=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) $(DEP_CPP_HTMIMP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTML.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTML_=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htextimp.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTML_=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTML_=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htextimp.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMLGen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMLG=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMLG=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLGen.obj" : $(SOURCE) $(DEP_CPP_HTMLG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMLG=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMLGen.obj" : $(SOURCE) $(DEP_CPP_HTMLG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMLPDTD.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMLP=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMLP=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMLP=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMulti.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMUL=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMUL=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMulti.obj" : $(SOURCE) $(DEP_CPP_HTMUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMUL=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMulti.obj" : $(SOURCE) $(DEP_CPP_HTMUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMuxCh.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMUX=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdemux.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htmuxtx.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMUX=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMuxCh.obj" : $(SOURCE) $(DEP_CPP_HTMUX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMUX=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdemux.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htmuxtx.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMuxCh.obj" : $(SOURCE) $(DEP_CPP_HTMUX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTMuxTx.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTMUXT=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htmuxtx.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMUXT=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMuxTx.obj" : $(SOURCE) $(DEP_CPP_HTMUXT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTMUXT=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmuxch.h"\
	"..\..\..\libwww\library\src\htmuxheader.h"\
	"..\..\..\libwww\library\src\htmuxtx.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTMuxTx.obj" : $(SOURCE) $(DEP_CPP_HTMUXT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNDir.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNDI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNDI=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNDir.obj" : $(SOURCE) $(DEP_CPP_HTNDI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNDI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTNDir.obj" : $(SOURCE) $(DEP_CPP_HTNDI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNet.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNET=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNET=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNet.obj" : $(SOURCE) $(DEP_CPP_HTNET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNET=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTNet.obj" : $(SOURCE) $(DEP_CPP_HTNET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNetTxt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNETT=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNETT=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNetTxt.obj" : $(SOURCE) $(DEP_CPP_HTNETT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNETT=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTNetTxt.obj" : $(SOURCE) $(DEP_CPP_HTNETT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNews.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNEW=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNEW=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNews.obj" : $(SOURCE) $(DEP_CPP_HTNEW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNEW=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTNews.obj" : $(SOURCE) $(DEP_CPP_HTNEW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNewsLs.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNEWS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNEWS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) $(DEP_CPP_HTNEWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNEWS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) $(DEP_CPP_HTNEWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNewsRq.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNEWSR=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNEWSR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) $(DEP_CPP_HTNEWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNEWSR=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) $(DEP_CPP_HTNEWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTNoFree.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTNOF=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTNOF=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTNoFree.obj" : $(SOURCE) $(DEP_CPP_HTNOF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTNOF=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTNoFree.obj" : $(SOURCE) $(DEP_CPP_HTNOF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTParse.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPAR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPAR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTParse.obj" : $(SOURCE) $(DEP_CPP_HTPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPAR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTParse.obj" : $(SOURCE) $(DEP_CPP_HTPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTPEP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPEP=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPEP=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTPEP.obj" : $(SOURCE) $(DEP_CPP_HTPEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPEP=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTPEP.obj" : $(SOURCE) $(DEP_CPP_HTPEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTPlain.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPLA=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htextimp.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPLA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPLA=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htext.h"\
	"..\..\..\libwww\library\src\htextimp.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTProfil.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPRO=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
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
	"..\..\..\libwww\library\src\htgopher.h"\
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
	"..\..\..\libwww\library\src\htinit.h"\
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
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprofil.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\Library\src\HTRDF.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
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
	"..\..\..\libwww\library\src\httelnet.h"\
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
	"..\..\..\libwww\library\src\HTWAIS.h"\
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
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwgophe.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwinit.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwnews.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtelnt.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\WWWWAIS.h"\
	"..\..\..\libwww\Library\src\WWWXML.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPRO=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPRO=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
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
	"..\..\..\libwww\library\src\htgopher.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthinit.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\hticons.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htinit.h"\
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
	"..\..\..\libwww\library\src\html.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htmulpar.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htndir.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnews.h"\
	"..\..\..\libwww\library\src\htnewsls.h"\
	"..\..\..\libwww\library\src\htnewsrq.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htplain.h"\
	"..\..\..\libwww\library\src\htprofil.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
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
	"..\..\..\libwww\library\src\httelnet.h"\
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
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcache.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwdir.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwftp.h"\
	"..\..\..\libwww\library\src\wwwgophe.h"\
	"..\..\..\libwww\library\src\wwwhtml.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwinit.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwmime.h"\
	"..\..\..\libwww\library\src\wwwnews.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtelnt.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	"..\..\..\libwww\library\src\wwwzip.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTProt.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPROT=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPROT=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProt.obj" : $(SOURCE) $(DEP_CPP_HTPROT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPROT=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTProt.obj" : $(SOURCE) $(DEP_CPP_HTPROT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTProxy.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTPROX=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
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
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPROX=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProxy.obj" : $(SOURCE) $(DEP_CPP_HTPROX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTPROX=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htaautil.h"\
	"..\..\..\libwww\library\src\htaccess.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htcookie.h"\
	"..\..\..\libwww\library\src\htdialog.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htevtlst.h"\
	"..\..\..\libwww\library\src\htfilter.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthist.h"\
	"..\..\..\libwww\library\src\hthome.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlog.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htpep.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
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
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwapp.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwhttp.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTProxy.obj" : $(SOURCE) $(DEP_CPP_HTPROX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTReader.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTREA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTREA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTReader.obj" : $(SOURCE) $(DEP_CPP_HTREA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTREA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTReader.obj" : $(SOURCE) $(DEP_CPP_HTREA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTReqMan.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTREQ=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTREQ=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) $(DEP_CPP_HTREQ) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTREQ=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) $(DEP_CPP_HTREQ) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTResponse.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTRES=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTRES=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTResponse.obj" : $(SOURCE) $(DEP_CPP_HTRES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTRES=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTResponse.obj" : $(SOURCE) $(DEP_CPP_HTRES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTRules.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTRUL=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTRUL=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTRules.obj" : $(SOURCE) $(DEP_CPP_HTRUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTRUL=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbind.h"\
	"..\..\..\libwww\library\src\htbinit.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htfile.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmulti.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htrules.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwfile.h"\
	"..\..\..\libwww\library\src\wwwlib.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTRules.obj" : $(SOURCE) $(DEP_CPP_HTRUL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTSChunk.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSCH=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSCH=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTSChunk.obj" : $(SOURCE) $(DEP_CPP_HTSCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSCH=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTSChunk.obj" : $(SOURCE) $(DEP_CPP_HTSCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTSocket.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSOC=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSOC=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTSocket.obj" : $(SOURCE) $(DEP_CPP_HTSOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSOC=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htansi.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htbufwrt.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htlocal.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htsocket.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwtrans.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTSocket.obj" : $(SOURCE) $(DEP_CPP_HTSOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTStream.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSTR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSTR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTStream.obj" : $(SOURCE) $(DEP_CPP_HTSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSTR=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTStream.obj" : $(SOURCE) $(DEP_CPP_HTSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTString.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSTRI=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSTRI=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTString.obj" : $(SOURCE) $(DEP_CPP_HTSTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSTRI=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTString.obj" : $(SOURCE) $(DEP_CPP_HTSTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTStyle.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTSTY=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTSTY=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTSTY=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstyle.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTChunk.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTCH=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTCH=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) $(DEP_CPP_HTTCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTCH=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httchunk.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) $(DEP_CPP_HTTCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTCP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTCP=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTCP=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTCP.obj" : $(SOURCE) $(DEP_CPP_HTTCP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTCP=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httcp.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTCP.obj" : $(SOURCE) $(DEP_CPP_HTTCP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTee.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTEE=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTEE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTee.obj" : $(SOURCE) $(DEP_CPP_HTTEE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTEE=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTee.obj" : $(SOURCE) $(DEP_CPP_HTTEE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTelnet.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTEL=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httelnet.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTEL=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTelnet.obj" : $(SOURCE) $(DEP_CPP_HTTEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTEL=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httelnet.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTelnet.obj" : $(SOURCE) $(DEP_CPP_HTTEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTeXGen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTEX=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTEX=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTeXGen.obj" : $(SOURCE) $(DEP_CPP_HTTEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTEX=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httexgen.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTeXGen.obj" : $(SOURCE) $(DEP_CPP_HTTEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTimer.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTIM=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTIM=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTimer.obj" : $(SOURCE) $(DEP_CPP_HTTIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTIM=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTimer.obj" : $(SOURCE) $(DEP_CPP_HTTIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTP.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTP_=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTP_=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTP.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTP_=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htconlen.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htfsave.h"\
	"..\..\..\libwww\library\src\htfwrite.h"\
	"..\..\..\libwww\library\src\htguess.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmerge.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnettxt.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htschunk.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httee.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\http.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwstream.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTP.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPGen.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPG=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPG=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) $(DEP_CPP_HTTPG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPG=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) $(DEP_CPP_HTTPG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPReq.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPR=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPR=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpreq.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPRes.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPRE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPRE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) $(DEP_CPP_HTTPRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPRE=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpgen.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) $(DEP_CPP_HTTPRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTPServ.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTPS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTPS=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTPS=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimerq.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httpres.h"\
	"..\..\..\libwww\library\src\httpserv.h"\
	"..\..\..\libwww\library\src\httputil.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTrace.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTRA=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTRA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrace.obj" : $(SOURCE) $(DEP_CPP_HTTRA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTRA=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTTrace.obj" : $(SOURCE) $(DEP_CPP_HTTRA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTTrans.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTTRAN=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTTRAN=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrans.obj" : $(SOURCE) $(DEP_CPP_HTTRAN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTTRAN=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTTrans.obj" : $(SOURCE) $(DEP_CPP_HTTRAN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTUser.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTUSE=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTUSE=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTUser.obj" : $(SOURCE) $(DEP_CPP_HTUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTUSE=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTUser.obj" : $(SOURCE) $(DEP_CPP_HTUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTUTree.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTUTR=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTUTR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTUTree.obj" : $(SOURCE) $(DEP_CPP_HTUTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTUTR=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTUTree.obj" : $(SOURCE) $(DEP_CPP_HTUTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTUU.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTUU_=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTUU_=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTUU.obj" : $(SOURCE) $(DEP_CPP_HTUU_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTUU_=\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\HTUU.obj" : $(SOURCE) $(DEP_CPP_HTUU_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTWriter.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTWRI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTWRI=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTWriter.obj" : $(SOURCE) $(DEP_CPP_HTWRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTWRI=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnetman.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwriter.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTWriter.obj" : $(SOURCE) $(DEP_CPP_HTWRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTWSRC.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTWSR=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwsrc.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTWSR=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) $(DEP_CPP_HTWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTWSR=\
	"..\..\..\libwww\library\src\htaabrow.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\htheader.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htmimprs.h"\
	"..\..\..\libwww\library\src\htmlgen.h"\
	"..\..\..\libwww\library\src\htmlpdtd.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htproxy.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htreqman.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwsrc.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) $(DEP_CPP_HTWSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTWWWStr.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTWWW=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTWWW=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) $(DEP_CPP_HTWWW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTWWW=\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) $(DEP_CPP_HTWWW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTXParse.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTXPA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTXPA=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTXParse.obj" : $(SOURCE) $(DEP_CPP_HTXPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTXPA=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hteptocl.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreader.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htxparse.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	

"$(INTDIR)\HTXParse.obj" : $(SOURCE) $(DEP_CPP_HTXPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\libwww\Library\src\HTZip.c

!IF  "$(CFG)" == "libwww - Win32 Release"

DEP_CPP_HTZIP=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTZIP=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTZip.obj" : $(SOURCE) $(DEP_CPP_HTZIP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_HTZIP=\
	"..\..\..\libwww\library\src\htalert.h"\
	"..\..\..\libwww\library\src\htanchor.h"\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htassoc.h"\
	"..\..\..\libwww\library\src\htatom.h"\
	"..\..\..\libwww\library\src\htchannl.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htdns.h"\
	"..\..\..\libwww\library\src\hterror.h"\
	"..\..\..\libwww\library\src\htescape.h"\
	"..\..\..\libwww\library\src\htevent.h"\
	"..\..\..\libwww\library\src\htformat.h"\
	"..\..\..\libwww\library\src\hthost.h"\
	"..\..\..\libwww\library\src\hthstman.h"\
	"..\..\..\libwww\library\src\htinet.h"\
	"..\..\..\libwww\library\src\htiostream.h"\
	"..\..\..\libwww\library\src\htlib.h"\
	"..\..\..\libwww\library\src\htlink.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemlog.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htmethod.h"\
	"..\..\..\libwww\library\src\htnet.h"\
	"..\..\..\libwww\library\src\htnofree.h"\
	"..\..\..\libwww\library\src\htparse.h"\
	"..\..\..\libwww\library\src\htprot.h"\
	"..\..\..\libwww\library\src\htreq.h"\
	"..\..\..\libwww\library\src\htresponse.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\httimer.h"\
	"..\..\..\libwww\library\src\httrans.h"\
	"..\..\..\libwww\library\src\htuser.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\htutree.h"\
	"..\..\..\libwww\library\src\htuu.h"\
	"..\..\..\libwww\library\src\htwwwstr.h"\
	"..\..\..\libwww\library\src\htzip.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwcore.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	"..\..\..\libwww\library\src\wwwutil.h"\
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
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SGML_=\
	"..\..\..\libwww\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

DEP_CPP_SGML_=\
	"..\..\..\libwww\library\src\htarray.h"\
	"..\..\..\libwww\library\src\htchunk.h"\
	"..\..\..\libwww\library\src\htlist.h"\
	"..\..\..\libwww\library\src\htmemory.h"\
	"..\..\..\libwww\library\src\htstream.h"\
	"..\..\..\libwww\library\src\htstring.h"\
	"..\..\..\libwww\library\src\htstruct.h"\
	"..\..\..\libwww\library\src\htutils.h"\
	"..\..\..\libwww\library\src\sgml.h"\
	"..\..\..\libwww\library\src\windows\config.h"\
	"..\..\..\libwww\library\src\wwwsys.h"\
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

