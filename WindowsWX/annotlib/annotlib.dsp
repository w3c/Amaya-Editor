# Microsoft Developer Studio Project File - Name="annotlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=annotlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "annotlib.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "annotlib - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\wxWidgets\lib\vc_dll\mswu" /I "..\..\..\wxWidgets\include" /I "..\annotlib" /I "..\annotlib\f" /I "..\..\annotlib" /I "..\..\annotlib\f" /I "..\..\..\redland\raptor" /I "..\..\..\redland\librdf" /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\thotlib\internals\f" /I "..\..\..\libwww\Library\src" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\..\..\wxWidgets\src\zlib" /D "_WINDOWS" /D "ANNOTATIONS" /D "RAPTOR_RDF_PARSER" /D "AM_REDLAND" /D "RAPTOR_INTERNAL" /D "_GL" /D "WIN32" /D "_WX" /YX /FD /TP /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\annotlib.lib"

!ELSEIF  "$(CFG)" == "annotlib - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\..\wxWidgets\lib\vc_dll\mswud" /I "..\..\..\wxWidgets\include" /I "..\annotlib" /I "..\annotlib\f" /I "..\..\annotlib" /I "..\..\annotlib\f" /I "..\..\..\redland\raptor" /I "..\..\..\redland\librdf" /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\thotlib\internals\f" /I "..\..\..\libwww\Library\src" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\..\..\wxWidgets\src\zlib" /D "_WINDOWS" /D "ANNOTATIONS" /D "RAPTOR_RDF_PARSER" /D "AM_REDLAND" /D "RAPTOR_INTERNAL" /D "_GL" /D "WIN32" /D "_WX" /FD /GZ /TP /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\annotlib.lib"

!ENDIF 

# Begin Target

# Name "annotlib - Win32 Release"
# Name "annotlib - Win32 Debug"
# Begin Source File

SOURCE=..\..\annotlib\AHTrdf2annot.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AnnotAPP.c
# End Source File
# Begin Source File

SOURCE=..\..\annotlib\ANNOTevent.c
# End Source File
# Begin Source File

SOURCE=..\..\annotlib\ANNOTfiles.c
# End Source File
# Begin Source File

SOURCE=..\..\annotlib\ANNOTlink.c
# End Source File
# Begin Source File

SOURCE=..\..\annotlib\ANNOTmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\annotlib\ANNOTschemas.c
# End Source File
# Begin Source File

SOURCE=..\..\annotlib\ANNOTtools.c
# End Source File
# End Target
# End Project
