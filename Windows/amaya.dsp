# Microsoft Developer Studio Project File - Name="amaya" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=amaya - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "amaya.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "amaya.mak" CFG="amaya - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "amaya - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "amaya - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
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
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I "..\thotlib\internals\f" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "COUGAR" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I ".\amaya" /I "..\amaya" /I "..\amaya\f" /I "..\thotlib\include" /I "..\..\libwww\Library\src" /I "..\libpng\zlib" /I "..\thotlib\internals\f" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "COUGAR" /D "__STDC__" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /D "_AMAYA_RELEASE_" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "amaya - Win32 Release"
# Name "amaya - Win32 Debug"
# Begin Source File

SOURCE=..\amaya\AHTBridge.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AHTFWrite.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AHTMemConv.c
# End Source File
# Begin Source File

SOURCE=..\amaya\AHTURLTools.c
# End Source File
# Begin Source File

SOURCE=..\amaya\amaya.rc

!IF  "$(CFG)" == "amaya - Win32 Release"

!ELSEIF  "$(CFG)" == "amaya - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\amaya\answer.c
# End Source File
# Begin Source File

SOURCE=..\amaya\appicon.ico
# End Source File
# Begin Source File

SOURCE=..\amaya\bcenter.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\bindent1.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\bindent2.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\bleft.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\bright.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\bslarge.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\bsmedium.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\bssmall.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\css.c
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

SOURCE=..\amaya\gkey001.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey002.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey003.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey004.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey005.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey006.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey007.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey008.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey009.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey010.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey011.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey012.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey013.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey014.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey015.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey016.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey017.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey018.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey019.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey020.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey021.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey022.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey023.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey024.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey025.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey026.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey027.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey028.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey029.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey030.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey031.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey032.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey033.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey034.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey035.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey036.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey037.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey038.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey039.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey040.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey041.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey042.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey043.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey044.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey045.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey046.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey047.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey048.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey049.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey050.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey051.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey052.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey053.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey054.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey055.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey056.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey057.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey058.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey059.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey060.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey061.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey062.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey063.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey064.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey065.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey066.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey067.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey068.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey069.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey070.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey071.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey072.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey073.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey074.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey075.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey076.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey077.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey078.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey079.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey080.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey081.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey082.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey083.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey084.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey085.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey086.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey087.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey088.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey089.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey090.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey091.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey092.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey093.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey094.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey095.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey096.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey097.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey098.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey099.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey100.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey101.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey102.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey103.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey104.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey105.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey106.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey107.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey108.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey109.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey110.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey111.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey112.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey113.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey114.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey115.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey116.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey117.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey118.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey119.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey120.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey121.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey122.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey123.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey124.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey125.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey126.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey127.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey128.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey129.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey130.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey131.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey132.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey133.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey134.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey135.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey136.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey137.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey138.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey139.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey140.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey141.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey142.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey143.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey144.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey145.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey146.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey147.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey148.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey149.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey150.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey151.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey152.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey153.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey154.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey155.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey156.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey157.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey158.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey159.bmp
# End Source File
# Begin Source File

SOURCE=..\amaya\gkey160.bmp
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

SOURCE=.\libThotEditor.lib
# End Source File
# Begin Source File

SOURCE=.\libwww.lib
# End Source File
# Begin Source File

SOURCE=..\amaya\Mathedit.c
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

SOURCE=..\amaya\query.c
# End Source File
# Begin Source File

SOURCE=..\amaya\styleparser.c
# End Source File
# Begin Source File

SOURCE=..\tablelib\tableH.c
# End Source File
# Begin Source File

SOURCE=..\amaya\templates.c
# End Source File
# Begin Source File

SOURCE=.\amaya\TextFileAPP.c
# End Source File
# Begin Source File

SOURCE=..\amaya\toolbar1.bmp
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

SOURCE=..\amaya\wincurso.cur
# End Source File
# Begin Source File

SOURCE=..\amaya\windialogapi.c
# End Source File
# Begin Source File

SOURCE=..\amaya\XMLparser.c
# End Source File
# End Target
# End Project
