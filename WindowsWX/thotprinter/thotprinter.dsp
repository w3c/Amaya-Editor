# Microsoft Developer Studio Project File - Name="thotprinter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=thotprinter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "thotprinter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "thotprinter.mak" CFG="thotprinter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "thotprinter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "thotprinter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "thotprinter - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "thotprin"
# PROP BASE Intermediate_Dir "thotprin"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\..\..\wxWidgets\src\zlib" /D "__STDC__" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS" /D "_SVG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDOWS_DLL" /D "_WINGUI" /D "_WINGUI_WX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 wxjpeg.lib wxpng.lib wxzlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"LIBCMT.lib" /libpath:"..\..\..\wxWidgets\lib\vc_dll"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "thotpri0"
# PROP BASE Intermediate_Dir "thotpri0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\..\..\wxWidgets\src\zlib" /D "__STDC__" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS" /D "_SVG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDOWS_DLL" /D "_WINGUI" /D "_WINGUI_WX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxjpeg.lib wxpng.lib wxzlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCD.lib" /nodefaultlib:"LIBCMTD.lib" /pdbtype:sept /libpath:"..\..\..\wxWidgets\lib\vc_dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "thotprinter - Win32 Release"
# Name "thotprinter - Win32 Debug"
# Begin Source File

SOURCE=..\..\Amaya\css.c
# End Source File
# Begin Source File

SOURCE=..\..\Amaya\fetchHTMLname.c
# End Source File
# Begin Source File

SOURCE=..\..\Amaya\fetchXMLname.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\nodialog.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\paginate.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\print.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\psdisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\Amaya\styleparser.c
# End Source File
# End Target
# End Project
