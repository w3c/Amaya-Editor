# Microsoft Developer Studio Project File - Name="davlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=davlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "davlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "davlib.mak" CFG="davlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "davlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "davlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "davlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\amaya" /I "..\..\davlib\h" /I "..\..\davlib\f" /I "..\..\davlib\tree\h" /I "..\..\..\libwww\Library\src" /I ".\..\..\libwww\modules\expat" /I "..\..\thotlib\include" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\wxWidgets\src\zlib" /D "_STIX" /D "_FONTCONFIG" /D "_GLANIM" /D "_GLTRANSFORMATION" /D "_MBCS" /D "_LIB" /D "DAV" /D "HT_DAV" /D "_SVGLIB" /D "_SVGANIM" /D "__WXMSW__" /D WINVER=0x0400 /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /YX /FD /TP /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "davlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\amaya" /I "..\..\davlib\h" /I "..\..\davlib\f" /I "..\..\davlib\tree\h" /I "..\..\..\libwww\Library\src" /I ".\..\..\libwww\modules\expat" /I "..\..\thotlib\include" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\wxWidgets\src\zlib" /D "_STIX" /D "_FONTCONFIG" /D "_GLANIM" /D "_GLTRANSFORMATION" /D "_MBCS" /D "_LIB" /D "DAV" /D "HT_DAV" /D "_SVGLIB" /D "_SVGANIM" /D "__WXMSW__" /D WINVER=0x0400 /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /FD /GZ /TP /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "davlib - Win32 Release"
# Name "davlib - Win32 Debug"
# Begin Source File

SOURCE=..\..\davlib\AHTLockBase.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\tree\awnode.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\tree\awpair.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\awparser.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\tree\awstring.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\tree\awtree.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\davlib.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\davlibCommon.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\davlibRequests.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\davlibUI.c
# End Source File
# Begin Source File

SOURCE=..\..\davlib\tree\list.c
# End Source File
# End Target
# End Project
