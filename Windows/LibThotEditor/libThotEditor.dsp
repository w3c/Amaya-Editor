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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /D "NDEBUG" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
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
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /D "_DEBUG" /D "_AMAYA_RELEASE_" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /YX /FD /c
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
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\alloca.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\appdialogue.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\appli.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\applicationapi.c
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

SOURCE=..\..\thotlib\dialogue\colors.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\config.c
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

SOURCE=..\..\thotlib\dialogue\createdoc.c
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

SOURCE=..\..\thotlib\tree\draw.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\editcommands.c
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

SOURCE=..\..\thotlib\dialogue\extprintmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\fileaccess.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\font.c
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

SOURCE=..\..\thotlib\dialogue\keyboards.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\labelalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\language.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\lookup.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\memory.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\message.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\modiftype.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\opendoc.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\openview.c
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

SOURCE=..\..\thotlib\image\picturebase.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\picturemenu.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\pivot.c
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

SOURCE=..\..\thotlib\dialogue\quit.c
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

SOURCE=..\..\thotlib\tree\res.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\resgen.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\resmatch.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\savedoc.c
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

SOURCE=..\..\thotlib\dialogue\searchmenustr.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\searchref.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\selectionapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\spellchecker.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\structchange.c
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

SOURCE=..\..\thotlib\view\structlocate.c
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

SOURCE=..\..\thotlib\editing\textcommands.c
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

SOURCE=..\..\thotlib\unicode\uaccess.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\unicode\uconvert.c
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\Unicode\Uio.c
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

SOURCE=..\..\thotlib\editing\viewcommands.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\views.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\visibility.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\windowdisplay.c
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

SOURCE=..\..\thotlib\image\xbmhandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\xpmhandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\zoom.c
# End Source File
# Begin Source File

SOURCE=..\libjpeg.lib
# End Source File
# Begin Source File

SOURCE=..\libpng.lib
# End Source File
# End Target
# End Project
