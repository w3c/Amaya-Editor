# Microsoft Developer Studio Project File - Name="LibThotKernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LibThotKernel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LibThotKernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibThotKernel.mak" CFG="LibThotKernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibThotKernel - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LibThotKernel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LibThotK"
# PROP BASE Intermediate_Dir "LibThotK"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\wxWidgets\lib\vc_dll\mswu" /I "..\\" /I "..\..\..\wxWidgets\include" /I "..\..\..\wxWidgets\src\zlib" /I "..\..\..\wxWidgets\contrib\include" /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\..\freetype\include" /D "__WXMSW__" /D WINVER=0x0400 /D "UNICODE" /D "_UNICODE" /D "XMD_H" /D "STDC_HEADERS" /D "NODISPLAY" /D "_WINDOWS_COMPILERS" /D "_SVGLIB" /D "_SVGANIM" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D wxUSE_GUI=1 /D "WXUSINGDLL" /YX /FD /TP /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LibThot0"
# PROP BASE Intermediate_Dir "LibThot0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\..\wxWidgets\lib\vc_dll\mswud" /I "..\\" /I "..\..\..\wxWidgets\include" /I "..\..\..\wxWidgets\src\zlib" /I "..\..\..\wxWidgets\contrib\include" /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\..\freetype\include" /D "__WXMSW__" /D WINVER=0x0400 /D "UNICODE" /D "_UNICODE" /D "XMD_H" /D "STDC_HEADERS" /D "NODISPLAY" /D "_WINDOWS_COMPILERS" /D "_SVGLIB" /D "_SVGANIM" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D wxUSE_GUI=1 /D "WXUSINGDLL" /FD /TP /c
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

# Name "LibThotKernel - Win32 Release"
# Name "LibThotKernel - Win32 Debug"
# Begin Source File

SOURCE=..\..\thotlib\base\actions.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\AmayaApp.cpp

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

# ADD CPP /D "WWW_WIN_DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\AmayaAppInstance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\AmayaLogDebug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\Editing\applicationapi.c
# End Source File
# Begin Source File

SOURCE=..\..\Thotlib\Tree\attributeapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\batchmessage.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\checkaccess.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\content.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\content\contentapi.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\documentapi.c
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

SOURCE=..\..\thotlib\base\global.c
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

SOURCE=..\..\thotlib\dialogue\nodisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\platform.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\presentation\presentationapi.c
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

SOURCE=..\..\thotlib\document\schemas.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\document\schtrad.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\structlist.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\tree\structschema.c
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

SOURCE=..\..\thotlib\base\units.c
# End Source File
# Begin Source File

SOURCE=..\..\thotlib\unicode\ustring.c
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

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

# ADD CPP /D "WWW_WIN_DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\wxAmayaSocketEventLoop.cpp

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

# ADD CPP /D "WWW_WIN_DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\thotlib\base\wxAmayaTimer.cpp

!IF  "$(CFG)" == "LibThotKernel - Win32 Release"

!ELSEIF  "$(CFG)" == "LibThotKernel - Win32 Debug"

# ADD CPP /D "WWW_WIN_DLL"

!ENDIF 

# End Source File
# End Target
# End Project
