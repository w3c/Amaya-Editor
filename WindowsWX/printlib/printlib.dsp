# Microsoft Developer Studio Project File - Name="printlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=printlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "printlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "printlib.mak" CFG="printlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "printlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "printlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "printlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "printlib"
# PROP BASE Intermediate_Dir "printlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\..\wxWidgets\src\jpeg" /I "..\..\..\wxWidgets\src\png" /I "..\..\..\wxWidgets\src\zlib" /D "NDEBUG" /D "__STDC_HEADERS" /D "STDC_HEADERS" /D "__STDC__" /D "_WIN_PRINT" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /D "JCONFIG_INCLUDED" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "printlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "printli0"
# PROP BASE Intermediate_Dir "printli0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\..\wxWidgets\src\jpeg" /I "..\..\..\wxWidgets\src\png" /I "..\..\..\wxWidgets\src\zlib" /D "_DEBUG" /D "_WINDOWS_DLL" /D "STDC_HEADERS" /D "__STDC__" /D "_WIN_PRINT" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /YX /FD /c
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

# Name "printlib - Win32 Release"
# Name "printlib - Win32 Debug"
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

SOURCE=..\..\Thotlib\Editing\applicationapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\attributes.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\attrpresent.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxmoves.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxpositions.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\boxrelations.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\buildboxes.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\buildlines.c
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

SOURCE=..\..\thotlib\content\content.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\createabsbox.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\createpages.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\dictionary.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\displaybox.c
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\View\displayview.c
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\Document\documentapi.c
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

SOURCE=..\..\thotlib\image\gifhandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\global.c
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

SOURCE=..\..\thotlib\image\jpeghandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\labelalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\language.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\memory.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\message.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\openglfont.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\pagecommands.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\picture.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\platform.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\pnghandler.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\presrules.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\presvariables.c
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

SOURCE=..\..\thotlib\tree\references.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\registry.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\schemas.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\schtrad.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\stix.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\structschema.c
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\presentation\Style.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\tableH.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\tree.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\unicode\uconvert.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\units.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\unicode\ustring.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\windowdisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\image\xpmhandler.c
# End Source File
# End Target
# End Project
