# Microsoft Developer Studio Project File - Name="libraptor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libraptor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libraptor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libraptor.mak" CFG="libraptor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libraptor - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libraptor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libraptor - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\libwww\modules\r" /I "..\..\..\redland\librdf" /I "..\..\..\redland\raptor" /I "..\..\..\libwww\modules\expat\lib" /D "_MBCS" /D "__STDC__" /D "HAVE_STDLIB_H" /D "HAVE_STDARG_H" /D "WIN32" /D "_WINDOWS" /D "RAPTOR_INTERNAL" /D "LIBRDF_INTERNAL" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libraptor - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\libwww\modules\r" /I "..\..\..\redland\librdf" /I "..\..\..\redland\raptor" /I "..\..\..\libwww\modules\expat\lib" /D "_DEBUG" /D "_MBCS" /D "__STDC__" /D "HAVE_STDLIB_H" /D "HAVE_STDARG_H" /D "WIN32" /D "_WINDOWS" /D "RAPTOR_INTERNAL" /D "LIBRDF_INTERNAL" /FD /GZ /c
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

# Name "libraptor - Win32 Release"
# Name "libraptor - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\redland\raptor\getopt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\ntriples_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_general.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_identifier.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_locator.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_namespace.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_qname.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_sax2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_sequence.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_set.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_stringbuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_uri.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_utf8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_win32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_www.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_xml.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\raptor_xml_writer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\strcasecmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\raptor\win32_config.h
# End Source File
# End Target
# End Project
