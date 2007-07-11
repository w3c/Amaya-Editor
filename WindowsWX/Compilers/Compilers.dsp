# Microsoft Developer Studio Project File - Name="Compilers" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Compilers - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Compilers.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Compilers.mak" CFG="Compilers - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Compilers - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Compilers - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Compilers - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\f" /D "WINDOWS_COMPILERS" /D "NODISPLAY" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WINGUI" /YX /FD /TP /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxbase28u.lib wsock32.lib comctl32.lib rpcrt4.lib winmm.lib advapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /machine:I386 /nodefaultlib:"libc" /libpath:"..\..\..\wxWidgets\lib\vc_dll"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=copy      ..\..\..\wxWidgets\lib\vc_dll\*.dll      ..\bin\ 
PostBuild_Cmds=echo "Before running Compilers.exe"	..\bin\Compilers.exe	echo "After running Compilers.exe"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Compilers - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /ZI /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\f" /D "WINDOWS_COMPILERS" /D "NODISPLAY" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WINGUI" /FR /FD /TP /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxbase28ud.lib wsock32.lib comctl32.lib rpcrt4.lib winmm.lib advapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /pdbtype:sept /libpath:"..\..\..\wxWidgets\lib\vc_dll"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=copy      ..\..\..\wxWidgets\lib\vc_dll\*.dll      ..\bin\ 
PostBuild_Cmds=echo "Before running Compilers.exe"	..\bin\Compilers.exe	echo "After running Compilers.exe"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Compilers - Win32 Release"
# Name "Compilers - Win32 Debug"
# Begin Source File

SOURCE=..\..\batch\comp_ico.ico
# End Source File
# Begin Source File

SOURCE=..\..\batch\comp_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\batch\Compilers.rc
# End Source File
# Begin Source File

SOURCE=..\..\batch\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\batch\winthotcompilers.c
# End Source File
# Begin Source File

SOURCE=..\LibThotKernel.lib
# End Source File
# End Target
# End Project
