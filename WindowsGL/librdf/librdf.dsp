# Microsoft Developer Studio Project File - Name="librdf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=librdf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "librdf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "librdf.mak" CFG="librdf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "librdf - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "librdf - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "librdf - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "librdf - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\libwww\modules\r" /I "..\..\..\redland\librdf" /I "..\..\..\redland\raptor" /I "..\..\..\libwww\modules\expat\lib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__STDC__" /D "HAVE_STDLIB_H" /D "HAVE_STDARG_H" /D "_WINDOWS" /D "LIBRDF_INTERNAL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\librdf.lib"

!ENDIF 

# Begin Target

# Name "librdf - Win32 Release"
# Name "librdf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\redland\librdf\memcmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_concepts.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_digest.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_digest_md5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_digest_sha1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_files.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_hash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_hash_cursor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_hash_memory.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_heuristics.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_iterator.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_model.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_model_storage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_node.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_parser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_parser_raptor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_query.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_query_triples.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_serializer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_serializer_raptor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_serializer_rdfxml.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_statement.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_storage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_storage_file.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_storage_hashes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_storage_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_stream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_uri.c
# End Source File
# Begin Source File

SOURCE=..\..\..\redland\librdf\rdf_utf8.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
