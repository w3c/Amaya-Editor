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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\wxWidgets\lib\vc_dll\mswu" /I "..\\" /I "..\..\..\wxWidgets\include" /I "..\..\..\wxWidgets\contrib\include" /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\tablelib\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\..\..\freetype-2.1.9\include" /D "__WXMSW__" /D WINVER=0x400 /D wxUSE_GUI=1 /D "UNICODE" /D "_UNICODE" /D "_GLPRINT" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS" /D "_SVG" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D "WXUSINGDLL" /YX /FD /TP /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 wxmsw25u_html.lib wxbase25u_xml.lib wxmsw25u_gl.lib wxmsw25u_adv.lib wxmsw25u_core.lib wxbase25u.lib freetype219.lib wsock32.lib comctl32.lib rpcrt4.lib winmm.lib advapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:yes /map /debug /machine:I386 /libpath:"..\..\..\wxWidgets\lib\vc_dll" /libpath:"..\..\..\freetype-2.1.9\objs"
# SUBTRACT LINK32 /pdb:none

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
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\..\wxWidgets\lib\vc_dll\mswud" /I "..\\" /I "..\..\..\wxWidgets\include" /I "..\..\..\wxWidgets\contrib\include" /I "..\amaya" /I "..\..\amaya" /I "..\..\amaya\f" /I "..\..\..\libwww\Library\src" /I "..\..\tablelib\f" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\thotlib\include" /I "..\..\..\freetype-2.1.9\include" /D "__WXMSW__" /D WINVER=0x400 /D wxUSE_GUI=1 /D "UNICODE" /D "_UNICODE" /D "_GLPRINT" /D "_WIN_PRINT" /D "PAGINEETIMPRIME" /D "STDC_HEADERS" /D "_SVG" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D "WXUSINGDLL" /FR /FD /TP /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmsw25ud_html.lib wxbase25ud_xml.lib wxmsw25ud_gl.lib wxmsw25ud_xrc.lib wxmsw25ud_adv.lib wxmsw25ud_core.lib wxbase25ud.lib freetype219_D.lib wsock32.lib comctl32.lib rpcrt4.lib winmm.lib advapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\wxWidgets\lib\vc_dll" /libpath:"..\..\..\freetype-2.1.9\objs"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "thotprinter - Win32 Release"
# Name "thotprinter - Win32 Debug"
# Begin Source File

SOURCE=..\..\amaya\amaya.rc
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\AmayaApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\AmayaPrintNotify.cpp
# End Source File
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

SOURCE=..\..\thotlib\base\message_wx.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\dialogue\nodialog.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\paginate.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\editing\print.c

!IF  "$(CFG)" == "thotprinter - Win32 Release"

!ELSEIF  "$(CFG)" == "thotprinter - Win32 Debug"

# SUBTRACT CPP /D "UNICODE" /D "_UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\thotlib\view\psdisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\registry_wx.c
# End Source File
# Begin Source File

SOURCE=..\..\Amaya\styleparser.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\wxAmayaTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\printlib.lib
# End Source File
# End Target
# End Project
