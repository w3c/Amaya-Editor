# Microsoft Developer Studio Project File - Name="libwww" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libwww - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libwww.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libwww - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".."
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\libpng\zlib" /I "..\..\..\wxWidgets\src\zlib" /I "..\..\..\libwww\modules\md5" /I "..\..\..\libwww\modules\expat\lib" /I "..\..\..\libwww\Library\src" /I "..\..\..\libwww\modules" /I "..\..\..\libwww\Library\src\windows" /I "..\..\..\libwww\Library\src\SSL" /I "..\..\..\openssl\inc32" /D "HT_DAV" /D "HT_ZLIB" /D "_GL" /D "WIN32" /D "_WINDOWS" /D "_SVGLIB" /D "_SVGANIM" /D "_STIX" /D "_FONTCONFIG" /D "_GLANIM" /D "_GLTRANSFORMATION" /D "LIBWWW_USEIDN" /D "WITHOUT_ICONV" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".."
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\..\wxWidgets\src\zlib" /I "..\..\..\libwww\modules\md5" /I "..\..\..\libwww\modules\expat\lib" /I "..\..\..\libwww\Library\src" /I "..\..\..\libwww\modules" /I "..\..\..\libwww\Library\src\windows" /I "..\..\..\libwww\Library\src\SSL" /I "..\..\..\openssl\inc32" /D "HT_DAV" /D "HT_ZLIB" /D "_GL" /D "WIN32" /D "_WINDOWS" /D "_SVGLIB" /D "_SVGANIM" /D "_STIX" /D "_FONTCONFIG" /D "_GLANIM" /D "_GLTRANSFORMATION" /D "LIBWWW_USEIDN" /D "WITHOUT_ICONV" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libwww - Win32 Release"
# Name "libwww - Win32 Debug"
# Begin Group ".c"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\aliaslist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\checker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\converter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\debug.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\delimitermap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\filechecker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\filemapper.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTAABrow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTAAUtil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTAccess.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTAlert.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTAnchor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTANSI.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTArray.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTAssoc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTAtom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTBind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTBInit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTBound.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTBTree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTBufWrt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTCache.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTChannl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTChunk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTConLen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTDAV.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTDemux.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTDescpt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTDigest.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTDir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTDNS.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTEPtoCl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTError.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTEscape.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTEvent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTEvtLst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTFile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTFilter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTFormat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTFSave.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTFTP.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTFTPDir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTFWrite.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTGopher.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTGuess.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Libwww\Library\Src\HTHash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTHeader.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTHInit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTHist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTHome.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTHost.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTIcons.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTInet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTInit.c

!IF  "$(CFG)" == "libwww - Win32 Release"

!ELSEIF  "$(CFG)" == "libwww - Win32 Debug"

# ADD CPP /I "..\..\..\..\libwww\Library\src"
# SUBTRACT CPP /I "..\..\..\libwww\Library\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTLib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTLink.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTList.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTLocal.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTLog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMemLog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMemory.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMerge.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMethod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMIME.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMIMERq.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMIMImp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMIMPrs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTML.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMLGen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMLPDTD.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMulti.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMuxCh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTMuxTx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTNDir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTNet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTNetTxt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTNews.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTNewsLs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTNewsRq.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTNoFree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTParse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTPEP.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTPlain.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTProfil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTProt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTProxy.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTReader.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTReqMan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTResponse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTRules.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTSChunk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTSocket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\SSL\HTSSL.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\SSL\HTSSLhttps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\SSL\HTSSLReader.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\SSL\HTSSLWriter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTStream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTString.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTStyle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTChunk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTCP.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTee.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTelnet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTeXGen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTimer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTP.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTPGen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTPReq.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTPRes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTPServ.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTrace.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTTrans.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTUser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTUTree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTUU.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTWriter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTWSRC.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTWWWStr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\Src\HTXML.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTXParse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\HTZip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\mapper.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\mapselector.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\md5\md5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\nameprep.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\nameprep_template.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\nameprepdata.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\normalizer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\punycode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\res.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\resconf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\result.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\Library\src\SGML.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\strhash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\ucs4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\ucsmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\ucsset.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\unicode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\unicode_template.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\unicodedata_320.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\unormalize.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\utf8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\util.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\version.c
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\expat\lib\xmlparse.c
# ADD CPP /D "COMPILED_FROM_DSP"
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\expat\lib\xmlrole.c
# ADD CPP /D "COMPILED_FROM_DSP"
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\expat\lib\xmltok.c
# ADD CPP /D "COMPILED_FROM_DSP"
# End Source File
# End Group
# Begin Group ".h"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\aliaslist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\api.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\assert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\checker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\converter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\delimitermap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\export.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\filechecker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\filemapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\logmacro.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\mapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\mapselector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\nameprep.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\normalizer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\punycode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\res.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\resconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\result.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\strhash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\ucs4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\ucsmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\ucsset.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\unicode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\unormalize.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\utf8.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\util.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libwww\modules\idn\version.h
# End Source File
# End Group
# End Target
# End Project
