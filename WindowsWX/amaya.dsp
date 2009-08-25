# Microsoft Developer Studio Project File - Name="amaya" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AMAYA - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "amaya.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "amaya.mak" CFG="AMAYA - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "amaya - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "amaya - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "amaya - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\wxWidgets\lib\vc_dll\mswu" /I ".\\" /I "..\..\wxWidgets\include" /I "..\..\wxWidgets\contrib\include" /I "..\..\wxWidgets\src\jpeg" /I "..\..\wxWidgets\src\tiff" /I "..\..\wxWidgets\src\png" /I "..\..\wxWidgets\src\zlib" /I "..\..\freetype\include" /I "..\..\libwww\modules\expat\lib" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\..\libwww\Library\src\SSL" /I "..\thotlib\internals\f" /I "..\annotlib" /I "..\annotlib\f" /I "..\davlib\h" /I "..\davlib\f" /I "..\davlib\tree\h" /I "..\..\wxWidgets\build\wince\missing" /D "WWW_WIN_DLL" /D "SOCKS" /D "STDC_HEADERS" /D "XMD_H" /D "EXPAT_PARSER" /D "XML_DTD" /D "XML_NS" /D "_SVG" /D "TEMPLATES" /D "THOT_TOOLTIPS" /D "ANNOTATIONS" /D "XML_GENERIC" /D "DAV" /D "__WXMSW__" /D WINVER=0x0400 /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D wxUSE_GUI=1 /D "WXUSINGDLL" /D "SSL" /YX /FD /TP /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\wxWidgets\include" /i "..\..\wxWidgets\include" /d "NDEBUG" /d "__WXMSW__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxjpeg.lib wxpng.lib wxzlib.lib wxmsw28u_aui.lib wxbase28u_xml.lib wxmsw28u_gl.lib wxmsw28u_xrc.lib wxmsw28u_adv.lib wxmsw28u_core.lib wxbase28u.lib wxbase28u_net.lib freetype2110.lib ssleay32.lib libeay32.lib wsock32.lib comctl32.lib rpcrt4.lib winmm.lib advapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:yes /machine:I386 /nodefaultlib:"LIBC.lib" /libpath:"..\..\wxWidgets\lib\vc_dll" /libpath:"..\..\freetype\objs" /libpath:"..\..\openssl\out32"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=copy                  ..\..\wxWidgets\lib\vc_dll\*.dll                  bin\                 	copy                  ..\..\wxWidgets\lib\vc_dll\wxjpeg.lib                  bin\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\wxWidgets\lib\vc_dll\mswud" /I ".\\" /I "..\..\wxWidgets\include" /I "..\..\wxWidgets\contrib\include" /I "..\..\wxWidgets\src\jpeg" /I "..\..\wxWidgets\src\tiff" /I "..\..\wxWidgets\src\png" /I "..\..\wxWidgets\src\zlib" /I "..\..\freetype\include" /I "..\..\libwww\modules\expat\lib" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\..\libwww\Library\src\SSL" /I "..\thotlib\internals\f" /I "..\annotlib" /I "..\annotlib\f" /I "..\davlib\h" /I "..\davlib\f" /I "..\davlib\tree\h" /I "..\..\wxWidgets\build\wince\missing" /D "WWW_WIN_DLL" /D "SOCKS" /D "STDC_HEADERS" /D "XMD_H" /D "EXPAT_PARSER" /D "XML_DTD" /D "XML_NS" /D "_SVG" /D "TEMPLATES" /D "THOT_TOOLTIPS" /D "ANNOTATIONS" /D "XML_GENERIC" /D "DAV" /D "__WXMSW__" /D WINVER=0x0400 /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_GL" /D "WIN32" /D "_WX" /D wxUSE_GUI=1 /D "WXUSINGDLL" /D "SSL" /FR /YX /FD /GZ /TP /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\wxWidgets\include" /i "..\..\wxWidgets\include" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxjpeg.lib wxpng.lib wxzlib.lib wxmsw28ud_aui.lib wxbase28ud_xml.lib wxmsw28ud_gl.lib wxmsw28ud_xrc.lib wxmsw28ud_adv.lib wxmsw28ud_core.lib wxbase28ud.lib wxbase28ud_net.lib freetype2110_D.lib ssleay32.lib libeay32.lib wsock32.lib comctl32.lib rpcrt4.lib winmm.lib advapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCD.lib" /nodefaultlib:"LIBCMTD.lib" /libpath:"..\..\wxWidgets\lib\vc_dll" /libpath:"..\..\freetype\objs" /libpath:"..\..\openssl\out32"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=copy                   ..\..\wxWidgets\lib\vc_dll\*.dll                   bin\                 	copy                  ..\..\wxWidgets\lib\vc_dll\wxjpeg.lib                  bin\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "amaya - Win32 Release"
# Name "amaya - Win32 Debug"
# Begin Group "wxdialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\amaya\wxdialog\amaya.rc
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\AuthentDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\CheckedListDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\CreateTableDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\DocInfoDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\FontDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\HRefDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\ImageDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\InitConfirmDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\ListBoxBook.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\ListDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\ListEditDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\ListNSDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\MakeIdDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\MetaDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\ObjectDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\OpenDocDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\PreferenceDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\PrintDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\SaveAsDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\SearchDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\SelectFenceAttributesDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\SelectIntegralDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\SelectOperatorDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\SendByMailDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\SpellCheckDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\StyleDlgWX.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\TitleDlgWX.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\amaya\AHTBridge.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AHTFWrite.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AHTInit.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AHTMemConv.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AHTURLTools.c
# End Source File
# Begin Source File

SOURCE=..\amaya\anim.c
# End Source File
# Begin Source File

SOURCE=..\amaya\animbuilder.c
# End Source File
# Begin Source File

SOURCE=..\amaya\answer.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\appicon.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\cross.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\css.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\EDITimage.c
# End Source File
# Begin Source File

SOURCE=..\amaya\EDITORactions.c
# End Source File
# Begin Source File

SOURCE=.\amaya\EDITORAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\EDITstyle.c
# End Source File
# Begin Source File

SOURCE=..\Amaya\fetchHTMLname.c
# End Source File
# Begin Source File

SOURCE=..\Amaya\fetchXMLname.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=..\thotlib\view\gldisplay.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\html2thot.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLactions.c
# End Source File
# Begin Source File

SOURCE=.\amaya\HTMLAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLbook.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLedit.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLform.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLhistory.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLimage.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLpresentation.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLsave.c
# End Source File
# Begin Source File

SOURCE=..\amaya\HTMLtable.c
# End Source File
# Begin Source File

SOURCE=..\amaya\init.c
# End Source File
# Begin Source File

SOURCE=..\amaya\insertelem.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\Mathedit.c
# End Source File
# Begin Source File

SOURCE=..\amaya\Mathedit_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\amaya\MathMLAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\MathMLbuilder.c
# End Source File
# Begin Source File

SOURCE=..\amaya\MENUconf.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\query.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\wx\msw\std.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialog\StyleListToolPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\amaya\styleparser.c
# End Source File
# Begin Source File

SOURCE=.\amaya\SVGAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\SVGbuilder.c
# End Source File
# Begin Source File

SOURCE=..\amaya\SVGedit.c
# End Source File
# Begin Source File

SOURCE=.\amaya\TemplateAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\Templatebuilder.c
# End Source File
# Begin Source File

SOURCE=..\amaya\templateDeclarations.c
# End Source File
# Begin Source File

SOURCE=..\amaya\templateInstantiate.c
# End Source File
# Begin Source File

SOURCE=..\amaya\templateLoad.c
# End Source File
# Begin Source File

SOURCE=..\amaya\templates.c
# End Source File
# Begin Source File

SOURCE=..\amaya\templateUtils.c
# End Source File
# Begin Source File

SOURCE=..\thotlib\base\testcase.c
# End Source File
# Begin Source File

SOURCE=.\amaya\TextFileAPP.c
# End Source File
# Begin Source File

SOURCE=.\amaya\TimelineAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\trans.c
# End Source File
# Begin Source File

SOURCE=..\amaya\transparse.c
# End Source File
# Begin Source File

SOURCE=..\amaya\UIcss.c
# End Source File
# Begin Source File

SOURCE=..\amaya\wxdialogapi.c
# End Source File
# Begin Source File

SOURCE=..\amaya\XHTMLbuilder.c
# End Source File
# Begin Source File

SOURCE=.\amaya\XLinkAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\XLinkbuilder.c
# End Source File
# Begin Source File

SOURCE=..\amaya\XLinkedit.c
# End Source File
# Begin Source File

SOURCE=..\amaya\Xml2thot.c
# End Source File
# Begin Source File

SOURCE=.\amaya\XMLAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\Xmlbuilder.c
# End Source File
# Begin Source File

SOURCE=..\amaya\XPointer.c
# End Source File
# Begin Source File

SOURCE=..\amaya\XPointerparse.c
# End Source File
# Begin Source File

SOURCE=.\libThotEditor.lib
# End Source File
# Begin Source File

SOURCE=.\libwww.lib
# End Source File
# Begin Source File

SOURCE=.\libraptor.lib
# End Source File
# End Target
# End Project
