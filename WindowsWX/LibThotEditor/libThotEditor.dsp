# Microsoft Developer Studio Project File - Name="libThotEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libThotEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libThotEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libThotEditor.mak" CFG="libThotEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libThotEditor - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libThotEditor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libThotEditor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\wxWidgets\lib\vc_dll\mswu" /I "..\\" /I "..\..\..\wxWidgets\include" /I "..\..\..\wxWidgets\contrib\include" /I "..\..\..\wxWidgets\src\jpeg" /I "..\..\..\wxWidgets\src\tiff" /I "..\..\..\wxWidgets\src\png" /I "..\..\..\wxWidgets\src\zlib" /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\amaya" /I "..\..\..\freetype\include" /D "WWW_WIN_DLL" /D "SOCKS" /D "__WXMSW__" /D "UNICODE" /D "_UNICODE" /D "STDC_HEADERS" /D "XMD_H" /D "THOT_TOOLTIPS" /D "_WINDOWS" /D "_GL" /D "WIN32" /D WINVER=0x400 /D wxUSE_GUI=1 /D "_WX" /D "WXUSINGDLL" /FD /TP /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Od /I "..\..\..\wxWidgets\lib\vc_dll\mswud" /I "..\\" /I "..\..\..\wxWidgets\include" /I "..\..\..\wxWidgets\contrib\include" /I "..\..\..\wxWidgets\src\jpeg" /I "..\..\..\wxWidgets\src\tiff" /I "..\..\..\wxWidgets\src\png" /I "..\..\..\wxWidgets\src\zlib" /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\amaya" /I "..\..\..\freetype\include" /D "WWW_WIN_DLL" /D "SOCKS" /D "__WXMSW__" /D "UNICODE" /D "_UNICODE" /D "STDC_HEADERS" /D "XMD_H" /D "_STIX" /D "_FONTCONFIG" /D "_GLANIM" /D "_GLTRANSFORMATION" /D "THOT_TOOLTIPS" /D "_SVGLIB" /D "_SVGANIM" /D "_WINDOWS" /D "_GL" /D "WIN32" /D WINVER=0x400 /D wxUSE_GUI=1 /D "_WX" /D "WXUSINGDLL" /FR /FD /TP /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libThotEditor - Win32 Release"
# Name "libThotEditor - Win32 Debug"
# Begin Source File

SOURCE=..\..\thotlib\view\absboxes.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\abspictures.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\actions.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"

!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"

# ADD CPP /MDd /W4

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaActionEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaAddPointEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaAdvancedNotebook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaAdvancedWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\AmayaApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\AmayaAppInstance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaApplyClassPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaAttributePanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaClassicNotebook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaClassicWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaColorButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaConfirmCloseTab.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaCreatePathEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaCreateShapeEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaEditPathEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaEditShapeEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaElementPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaExplorerPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaFileDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaHelpWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\AmayaLogDebug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaMathMLPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaMovePointEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaMovingBoxEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaNormalWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaPage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaPathControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaPopupList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaPrintNotify.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaQuickSplitButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaResizingBoxEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaSimpleWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaSpeCharPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaStatsThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaStylePanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaSVGPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaTemplatePanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaTipOfTheDay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaToolBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaTransformEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaWindowIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaXHTMLPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaXMLPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\animbox.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\appdialogue.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\appdialogue_wx.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\appli.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\applicationapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\archives.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\attributeapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\attributes.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\attrmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\attrpresent.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\base64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxlocate.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxmoves.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxparams.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxpositions.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxrelations.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxselection.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\browser.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\buildboxes.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\buildlines.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\callback.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\callbackinit.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\changeabsbox.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\changepresent.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\checkaccess.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\checkermenu.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\closedoc.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\config.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\containers.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\content.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\contentapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\context.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\createabsbox.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\createpages.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\creationmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\dialogapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\dictionary.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\displaybox.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\displayselect.c
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\View\displayview.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\docs.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\documentapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\editcommands.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\Elemlist.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\email.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\epshandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\exceptions.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\externalref.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\fileaccess.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\font.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\fontconfig.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\fontserver.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\frame.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\geom.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\gifhandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\glbox.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\gldisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\glglyph.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\glgradient.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\global.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\gltimer.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\glwindowdisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\hyphen.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\inites.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\input.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\interface.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\jpeghandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\labelalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\language.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\logdebug.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\memory.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\message.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\message_wx.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\modiftype.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\openglfont.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\pagecommands.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\paginate.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\picture.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\pictureapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\platform.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\pnghandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\presentationapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\presentmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\presrules.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\presvariables.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\printmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\Dialogue\Profiles.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\pschemaapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\readpivot.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\readprs.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\readstr.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\readtra.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\referenceapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\references.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\registry.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\registry_wx.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\res.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\resgen.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\resmatch.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\schemas.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\schtrad.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\scroll.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\search.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\searchmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\searchref.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\selectionapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\SMTP.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\spellchecker.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\spline.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\stix.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\structcommands.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\structcreation.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\structlist.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\structmodif.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\structschema.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\structselect.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\style.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\tableH.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\tesse.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\textcommands.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\include\thot_gui_windows.h
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\thotmsg.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\translation.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\tree.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\treeapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\unicode\uconvert.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\undo.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\undoapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\units.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\unstructchange.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\unstructlocate.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\unicode\ustring.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\viewapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\views.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\word.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\writedoc.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\writepivot.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\writeprs.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\writestr.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\wxAmayaSocketEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\wxAmayaSocketEventLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\wxAmayaTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\xpmhandler.c
# End Source File
# End Target
# End Project
