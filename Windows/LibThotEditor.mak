# Microsoft Developer Studio Generated NMAKE File, Based on LibThotEditor.dsp
!IF "$(CFG)" == ""
CFG=LibThotEditor - Win32 Release
!MESSAGE No configuration specified. Defaulting to LibThotEditor - Win32\
 Release.
!ENDIF 

!IF "$(CFG)" != "LibThotEditor - Win32 Release" && "$(CFG)" !=\
 "LibThotEditor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibThotEditor.mak" CFG="LibThotEditor - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibThotEditor - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "LibThotEditor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

OUTDIR=.\.
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\LibThotEditor.lib"

!ELSE 

ALL : "libjpeg - Win32 Release" "libpng - Win32 Release"\
 "$(OUTDIR)\LibThotEditor.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libpng - Win32 ReleaseCLEAN" "libjpeg - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Absboxes.obj"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\Actions.obj"
	-@erase "$(INTDIR)\Alloca.obj"
	-@erase "$(INTDIR)\appdialogue.obj"
	-@erase "$(INTDIR)\Appli.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributeapi.obj"
	-@erase "$(INTDIR)\attributes.obj"
	-@erase "$(INTDIR)\Attrmenu.obj"
	-@erase "$(INTDIR)\attrpresent.obj"
	-@erase "$(INTDIR)\boxlocate.obj"
	-@erase "$(INTDIR)\Boxmoves.obj"
	-@erase "$(INTDIR)\boxparams.obj"
	-@erase "$(INTDIR)\boxpositions.obj"
	-@erase "$(INTDIR)\boxrelations.obj"
	-@erase "$(INTDIR)\boxselection.obj"
	-@erase "$(INTDIR)\Browser.obj"
	-@erase "$(INTDIR)\buildboxes.obj"
	-@erase "$(INTDIR)\buildlines.obj"
	-@erase "$(INTDIR)\Callback.obj"
	-@erase "$(INTDIR)\callbackinit.obj"
	-@erase "$(INTDIR)\changeabsbox.obj"
	-@erase "$(INTDIR)\changepresent.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\checkermenu.obj"
	-@erase "$(INTDIR)\Closedoc.obj"
	-@erase "$(INTDIR)\Colors.obj"
	-@erase "$(INTDIR)\Config.obj"
	-@erase "$(INTDIR)\Content.obj"
	-@erase "$(INTDIR)\contentapi.obj"
	-@erase "$(INTDIR)\Context.obj"
	-@erase "$(INTDIR)\createabsbox.obj"
	-@erase "$(INTDIR)\createdoc.obj"
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\creationmenu.obj"
	-@erase "$(INTDIR)\dialogapi.obj"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displayselect.obj"
	-@erase "$(INTDIR)\Docs.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\Draw.obj"
	-@erase "$(INTDIR)\editcommands.obj"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\extprintmenu.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\Font.obj"
	-@erase "$(INTDIR)\Frame.obj"
	-@erase "$(INTDIR)\genericdriver.obj"
	-@erase "$(INTDIR)\Geom.obj"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\Hyphen.obj"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\Input.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\keyboards.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\Language.obj"
	-@erase "$(INTDIR)\LiteClue.obj"
	-@erase "$(INTDIR)\Lookup.obj"
	-@erase "$(INTDIR)\Memory.obj"
	-@erase "$(INTDIR)\Message.obj"
	-@erase "$(INTDIR)\modiftype.obj"
	-@erase "$(INTDIR)\Opendoc.obj"
	-@erase "$(INTDIR)\Openview.obj"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\Paginate.obj"
	-@erase "$(INTDIR)\parsexml.obj"
	-@erase "$(INTDIR)\Picture.obj"
	-@erase "$(INTDIR)\pictureapi.obj"
	-@erase "$(INTDIR)\picturebase.obj"
	-@erase "$(INTDIR)\picturemenu.obj"
	-@erase "$(INTDIR)\Pivot.obj"
	-@erase "$(INTDIR)\Platform.obj"
	-@erase "$(INTDIR)\pnghandler.obj"
	-@erase "$(INTDIR)\presentationapi.obj"
	-@erase "$(INTDIR)\presentdriver.obj"
	-@erase "$(INTDIR)\presentmenu.obj"
	-@erase "$(INTDIR)\presrules.obj"
	-@erase "$(INTDIR)\presvariables.obj"
	-@erase "$(INTDIR)\printmenu.obj"
	-@erase "$(INTDIR)\pschemaapi.obj"
	-@erase "$(INTDIR)\Quit.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\Readprs.obj"
	-@erase "$(INTDIR)\Readstr.obj"
	-@erase "$(INTDIR)\Readtra.obj"
	-@erase "$(INTDIR)\referenceapi.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\Registry.obj"
	-@erase "$(INTDIR)\Res.obj"
	-@erase "$(INTDIR)\Resgen.obj"
	-@erase "$(INTDIR)\Resmatch.obj"
	-@erase "$(INTDIR)\Savedoc.obj"
	-@erase "$(INTDIR)\savexml.obj"
	-@erase "$(INTDIR)\Schemas.obj"
	-@erase "$(INTDIR)\Schtrad.obj"
	-@erase "$(INTDIR)\Scroll.obj"
	-@erase "$(INTDIR)\Search.obj"
	-@erase "$(INTDIR)\searchmenu.obj"
	-@erase "$(INTDIR)\searchmenustr.obj"
	-@erase "$(INTDIR)\searchref.obj"
	-@erase "$(INTDIR)\selectionapi.obj"
	-@erase "$(INTDIR)\specificdriver.obj"
	-@erase "$(INTDIR)\spellchecker.obj"
	-@erase "$(INTDIR)\structchange.obj"
	-@erase "$(INTDIR)\structcommands.obj"
	-@erase "$(INTDIR)\structcreation.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structlocate.obj"
	-@erase "$(INTDIR)\structmodif.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\structselect.obj"
	-@erase "$(INTDIR)\textcommands.obj"
	-@erase "$(INTDIR)\thotmodule.obj"
	-@erase "$(INTDIR)\Thotmsg.obj"
	-@erase "$(INTDIR)\translatexml.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\Tree.obj"
	-@erase "$(INTDIR)\Treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undoapi.obj"
	-@erase "$(INTDIR)\Units.obj"
	-@erase "$(INTDIR)\unstructchange.obj"
	-@erase "$(INTDIR)\unstructlocate.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\Viewapi.obj"
	-@erase "$(INTDIR)\viewcommands.obj"
	-@erase "$(INTDIR)\Views.obj"
	-@erase "$(INTDIR)\visibility.obj"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\Word.obj"
	-@erase "$(INTDIR)\writedoc.obj"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\writexml.obj"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xmlmodule.obj"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(INTDIR)\Zoom.obj"
	-@erase "$(OUTDIR)\LibThotEditor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /I "..\thotlib\include" /I\
 "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I\
 "..\thotlib\internals\f" /I "..\libjpeg" /I "..\libpng" /I "..\libpng\zlib" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS"\
 /Fp"$(INTDIR)\LibThotEditor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LibThotEditor.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LibThotEditor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\Actions.obj" \
	"$(INTDIR)\Alloca.obj" \
	"$(INTDIR)\appdialogue.obj" \
	"$(INTDIR)\Appli.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributeapi.obj" \
	"$(INTDIR)\attributes.obj" \
	"$(INTDIR)\Attrmenu.obj" \
	"$(INTDIR)\attrpresent.obj" \
	"$(INTDIR)\boxlocate.obj" \
	"$(INTDIR)\Boxmoves.obj" \
	"$(INTDIR)\boxparams.obj" \
	"$(INTDIR)\boxpositions.obj" \
	"$(INTDIR)\boxrelations.obj" \
	"$(INTDIR)\boxselection.obj" \
	"$(INTDIR)\Browser.obj" \
	"$(INTDIR)\buildboxes.obj" \
	"$(INTDIR)\buildlines.obj" \
	"$(INTDIR)\Callback.obj" \
	"$(INTDIR)\callbackinit.obj" \
	"$(INTDIR)\changeabsbox.obj" \
	"$(INTDIR)\changepresent.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\checkermenu.obj" \
	"$(INTDIR)\Closedoc.obj" \
	"$(INTDIR)\Colors.obj" \
	"$(INTDIR)\Config.obj" \
	"$(INTDIR)\Content.obj" \
	"$(INTDIR)\contentapi.obj" \
	"$(INTDIR)\Context.obj" \
	"$(INTDIR)\createabsbox.obj" \
	"$(INTDIR)\createdoc.obj" \
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\creationmenu.obj" \
	"$(INTDIR)\dialogapi.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayselect.obj" \
	"$(INTDIR)\Docs.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\Draw.obj" \
	"$(INTDIR)\editcommands.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\extprintmenu.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\Font.obj" \
	"$(INTDIR)\Frame.obj" \
	"$(INTDIR)\genericdriver.obj" \
	"$(INTDIR)\Geom.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\Hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\Input.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\keyboards.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\Language.obj" \
	"$(INTDIR)\LiteClue.obj" \
	"$(INTDIR)\Lookup.obj" \
	"$(INTDIR)\Memory.obj" \
	"$(INTDIR)\Message.obj" \
	"$(INTDIR)\modiftype.obj" \
	"$(INTDIR)\Opendoc.obj" \
	"$(INTDIR)\Openview.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\Paginate.obj" \
	"$(INTDIR)\parsexml.obj" \
	"$(INTDIR)\Picture.obj" \
	"$(INTDIR)\pictureapi.obj" \
	"$(INTDIR)\picturebase.obj" \
	"$(INTDIR)\picturemenu.obj" \
	"$(INTDIR)\Pivot.obj" \
	"$(INTDIR)\Platform.obj" \
	"$(INTDIR)\pnghandler.obj" \
	"$(INTDIR)\presentationapi.obj" \
	"$(INTDIR)\presentdriver.obj" \
	"$(INTDIR)\presentmenu.obj" \
	"$(INTDIR)\presrules.obj" \
	"$(INTDIR)\presvariables.obj" \
	"$(INTDIR)\printmenu.obj" \
	"$(INTDIR)\pschemaapi.obj" \
	"$(INTDIR)\Quit.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\Readprs.obj" \
	"$(INTDIR)\Readstr.obj" \
	"$(INTDIR)\Readtra.obj" \
	"$(INTDIR)\referenceapi.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\Registry.obj" \
	"$(INTDIR)\Res.obj" \
	"$(INTDIR)\Resgen.obj" \
	"$(INTDIR)\Resmatch.obj" \
	"$(INTDIR)\Savedoc.obj" \
	"$(INTDIR)\savexml.obj" \
	"$(INTDIR)\Schemas.obj" \
	"$(INTDIR)\Schtrad.obj" \
	"$(INTDIR)\Scroll.obj" \
	"$(INTDIR)\Search.obj" \
	"$(INTDIR)\searchmenu.obj" \
	"$(INTDIR)\searchmenustr.obj" \
	"$(INTDIR)\searchref.obj" \
	"$(INTDIR)\selectionapi.obj" \
	"$(INTDIR)\specificdriver.obj" \
	"$(INTDIR)\spellchecker.obj" \
	"$(INTDIR)\structchange.obj" \
	"$(INTDIR)\structcommands.obj" \
	"$(INTDIR)\structcreation.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structlocate.obj" \
	"$(INTDIR)\structmodif.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\structselect.obj" \
	"$(INTDIR)\textcommands.obj" \
	"$(INTDIR)\thotmodule.obj" \
	"$(INTDIR)\Thotmsg.obj" \
	"$(INTDIR)\translatexml.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\Tree.obj" \
	"$(INTDIR)\Treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\undoapi.obj" \
	"$(INTDIR)\Units.obj" \
	"$(INTDIR)\unstructchange.obj" \
	"$(INTDIR)\unstructlocate.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\Viewapi.obj" \
	"$(INTDIR)\viewcommands.obj" \
	"$(INTDIR)\Views.obj" \
	"$(INTDIR)\visibility.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\Word.obj" \
	"$(INTDIR)\writedoc.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\writexml.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xmlmodule.obj" \
	"$(INTDIR)\xpmhandler.obj" \
	"$(INTDIR)\Zoom.obj" \
	"$(OUTDIR)\libjpeg.lib" \
	"$(OUTDIR)\Libpng.lib"

"$(OUTDIR)\LibThotEditor.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

OUTDIR=.\.
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\LibThotEditor.lib"

!ELSE 

ALL : "libjpeg - Win32 Debug" "libpng - Win32 Debug"\
 "$(OUTDIR)\LibThotEditor.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libpng - Win32 DebugCLEAN" "libjpeg - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Absboxes.obj"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\Actions.obj"
	-@erase "$(INTDIR)\Alloca.obj"
	-@erase "$(INTDIR)\appdialogue.obj"
	-@erase "$(INTDIR)\Appli.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributeapi.obj"
	-@erase "$(INTDIR)\attributes.obj"
	-@erase "$(INTDIR)\Attrmenu.obj"
	-@erase "$(INTDIR)\attrpresent.obj"
	-@erase "$(INTDIR)\boxlocate.obj"
	-@erase "$(INTDIR)\Boxmoves.obj"
	-@erase "$(INTDIR)\boxparams.obj"
	-@erase "$(INTDIR)\boxpositions.obj"
	-@erase "$(INTDIR)\boxrelations.obj"
	-@erase "$(INTDIR)\boxselection.obj"
	-@erase "$(INTDIR)\Browser.obj"
	-@erase "$(INTDIR)\buildboxes.obj"
	-@erase "$(INTDIR)\buildlines.obj"
	-@erase "$(INTDIR)\Callback.obj"
	-@erase "$(INTDIR)\callbackinit.obj"
	-@erase "$(INTDIR)\changeabsbox.obj"
	-@erase "$(INTDIR)\changepresent.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\checkermenu.obj"
	-@erase "$(INTDIR)\Closedoc.obj"
	-@erase "$(INTDIR)\Colors.obj"
	-@erase "$(INTDIR)\Config.obj"
	-@erase "$(INTDIR)\Content.obj"
	-@erase "$(INTDIR)\contentapi.obj"
	-@erase "$(INTDIR)\Context.obj"
	-@erase "$(INTDIR)\createabsbox.obj"
	-@erase "$(INTDIR)\createdoc.obj"
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\creationmenu.obj"
	-@erase "$(INTDIR)\dialogapi.obj"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displayselect.obj"
	-@erase "$(INTDIR)\Docs.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\Draw.obj"
	-@erase "$(INTDIR)\editcommands.obj"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\extprintmenu.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\Font.obj"
	-@erase "$(INTDIR)\Frame.obj"
	-@erase "$(INTDIR)\genericdriver.obj"
	-@erase "$(INTDIR)\Geom.obj"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\Hyphen.obj"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\Input.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\keyboards.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\Language.obj"
	-@erase "$(INTDIR)\LiteClue.obj"
	-@erase "$(INTDIR)\Lookup.obj"
	-@erase "$(INTDIR)\Memory.obj"
	-@erase "$(INTDIR)\Message.obj"
	-@erase "$(INTDIR)\modiftype.obj"
	-@erase "$(INTDIR)\Opendoc.obj"
	-@erase "$(INTDIR)\Openview.obj"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\Paginate.obj"
	-@erase "$(INTDIR)\parsexml.obj"
	-@erase "$(INTDIR)\Picture.obj"
	-@erase "$(INTDIR)\pictureapi.obj"
	-@erase "$(INTDIR)\picturebase.obj"
	-@erase "$(INTDIR)\picturemenu.obj"
	-@erase "$(INTDIR)\Pivot.obj"
	-@erase "$(INTDIR)\Platform.obj"
	-@erase "$(INTDIR)\pnghandler.obj"
	-@erase "$(INTDIR)\presentationapi.obj"
	-@erase "$(INTDIR)\presentdriver.obj"
	-@erase "$(INTDIR)\presentmenu.obj"
	-@erase "$(INTDIR)\presrules.obj"
	-@erase "$(INTDIR)\presvariables.obj"
	-@erase "$(INTDIR)\printmenu.obj"
	-@erase "$(INTDIR)\pschemaapi.obj"
	-@erase "$(INTDIR)\Quit.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\Readprs.obj"
	-@erase "$(INTDIR)\Readstr.obj"
	-@erase "$(INTDIR)\Readtra.obj"
	-@erase "$(INTDIR)\referenceapi.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\Registry.obj"
	-@erase "$(INTDIR)\Res.obj"
	-@erase "$(INTDIR)\Resgen.obj"
	-@erase "$(INTDIR)\Resmatch.obj"
	-@erase "$(INTDIR)\Savedoc.obj"
	-@erase "$(INTDIR)\savexml.obj"
	-@erase "$(INTDIR)\Schemas.obj"
	-@erase "$(INTDIR)\Schtrad.obj"
	-@erase "$(INTDIR)\Scroll.obj"
	-@erase "$(INTDIR)\Search.obj"
	-@erase "$(INTDIR)\searchmenu.obj"
	-@erase "$(INTDIR)\searchmenustr.obj"
	-@erase "$(INTDIR)\searchref.obj"
	-@erase "$(INTDIR)\selectionapi.obj"
	-@erase "$(INTDIR)\specificdriver.obj"
	-@erase "$(INTDIR)\spellchecker.obj"
	-@erase "$(INTDIR)\structchange.obj"
	-@erase "$(INTDIR)\structcommands.obj"
	-@erase "$(INTDIR)\structcreation.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structlocate.obj"
	-@erase "$(INTDIR)\structmodif.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\structselect.obj"
	-@erase "$(INTDIR)\textcommands.obj"
	-@erase "$(INTDIR)\thotmodule.obj"
	-@erase "$(INTDIR)\Thotmsg.obj"
	-@erase "$(INTDIR)\translatexml.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\Tree.obj"
	-@erase "$(INTDIR)\Treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undoapi.obj"
	-@erase "$(INTDIR)\Units.obj"
	-@erase "$(INTDIR)\unstructchange.obj"
	-@erase "$(INTDIR)\unstructlocate.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\Viewapi.obj"
	-@erase "$(INTDIR)\viewcommands.obj"
	-@erase "$(INTDIR)\Views.obj"
	-@erase "$(INTDIR)\visibility.obj"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\Word.obj"
	-@erase "$(INTDIR)\writedoc.obj"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\writexml.obj"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xmlmodule.obj"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(INTDIR)\Zoom.obj"
	-@erase "$(OUTDIR)\LibThotEditor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /I "..\thotlib\include" /I\
 "..\thotlib\internals\h" /I "..\thotlib\internals\var" /I\
 "..\thotlib\internals\f" /I "..\libjpeg" /I "..\libpng" /I "..\libpng\zlib" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS"\
 /Fp"$(INTDIR)\LibThotEditor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LibThotEditor.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LibThotEditor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\Actions.obj" \
	"$(INTDIR)\Alloca.obj" \
	"$(INTDIR)\appdialogue.obj" \
	"$(INTDIR)\Appli.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributeapi.obj" \
	"$(INTDIR)\attributes.obj" \
	"$(INTDIR)\Attrmenu.obj" \
	"$(INTDIR)\attrpresent.obj" \
	"$(INTDIR)\boxlocate.obj" \
	"$(INTDIR)\Boxmoves.obj" \
	"$(INTDIR)\boxparams.obj" \
	"$(INTDIR)\boxpositions.obj" \
	"$(INTDIR)\boxrelations.obj" \
	"$(INTDIR)\boxselection.obj" \
	"$(INTDIR)\Browser.obj" \
	"$(INTDIR)\buildboxes.obj" \
	"$(INTDIR)\buildlines.obj" \
	"$(INTDIR)\Callback.obj" \
	"$(INTDIR)\callbackinit.obj" \
	"$(INTDIR)\changeabsbox.obj" \
	"$(INTDIR)\changepresent.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\checkermenu.obj" \
	"$(INTDIR)\Closedoc.obj" \
	"$(INTDIR)\Colors.obj" \
	"$(INTDIR)\Config.obj" \
	"$(INTDIR)\Content.obj" \
	"$(INTDIR)\contentapi.obj" \
	"$(INTDIR)\Context.obj" \
	"$(INTDIR)\createabsbox.obj" \
	"$(INTDIR)\createdoc.obj" \
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\creationmenu.obj" \
	"$(INTDIR)\dialogapi.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayselect.obj" \
	"$(INTDIR)\Docs.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\Draw.obj" \
	"$(INTDIR)\editcommands.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\extprintmenu.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\Font.obj" \
	"$(INTDIR)\Frame.obj" \
	"$(INTDIR)\genericdriver.obj" \
	"$(INTDIR)\Geom.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\Hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\Input.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\keyboards.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\Language.obj" \
	"$(INTDIR)\LiteClue.obj" \
	"$(INTDIR)\Lookup.obj" \
	"$(INTDIR)\Memory.obj" \
	"$(INTDIR)\Message.obj" \
	"$(INTDIR)\modiftype.obj" \
	"$(INTDIR)\Opendoc.obj" \
	"$(INTDIR)\Openview.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\Paginate.obj" \
	"$(INTDIR)\parsexml.obj" \
	"$(INTDIR)\Picture.obj" \
	"$(INTDIR)\pictureapi.obj" \
	"$(INTDIR)\picturebase.obj" \
	"$(INTDIR)\picturemenu.obj" \
	"$(INTDIR)\Pivot.obj" \
	"$(INTDIR)\Platform.obj" \
	"$(INTDIR)\pnghandler.obj" \
	"$(INTDIR)\presentationapi.obj" \
	"$(INTDIR)\presentdriver.obj" \
	"$(INTDIR)\presentmenu.obj" \
	"$(INTDIR)\presrules.obj" \
	"$(INTDIR)\presvariables.obj" \
	"$(INTDIR)\printmenu.obj" \
	"$(INTDIR)\pschemaapi.obj" \
	"$(INTDIR)\Quit.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\Readprs.obj" \
	"$(INTDIR)\Readstr.obj" \
	"$(INTDIR)\Readtra.obj" \
	"$(INTDIR)\referenceapi.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\Registry.obj" \
	"$(INTDIR)\Res.obj" \
	"$(INTDIR)\Resgen.obj" \
	"$(INTDIR)\Resmatch.obj" \
	"$(INTDIR)\Savedoc.obj" \
	"$(INTDIR)\savexml.obj" \
	"$(INTDIR)\Schemas.obj" \
	"$(INTDIR)\Schtrad.obj" \
	"$(INTDIR)\Scroll.obj" \
	"$(INTDIR)\Search.obj" \
	"$(INTDIR)\searchmenu.obj" \
	"$(INTDIR)\searchmenustr.obj" \
	"$(INTDIR)\searchref.obj" \
	"$(INTDIR)\selectionapi.obj" \
	"$(INTDIR)\specificdriver.obj" \
	"$(INTDIR)\spellchecker.obj" \
	"$(INTDIR)\structchange.obj" \
	"$(INTDIR)\structcommands.obj" \
	"$(INTDIR)\structcreation.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structlocate.obj" \
	"$(INTDIR)\structmodif.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\structselect.obj" \
	"$(INTDIR)\textcommands.obj" \
	"$(INTDIR)\thotmodule.obj" \
	"$(INTDIR)\Thotmsg.obj" \
	"$(INTDIR)\translatexml.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\Tree.obj" \
	"$(INTDIR)\Treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\undoapi.obj" \
	"$(INTDIR)\Units.obj" \
	"$(INTDIR)\unstructchange.obj" \
	"$(INTDIR)\unstructlocate.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\Viewapi.obj" \
	"$(INTDIR)\viewcommands.obj" \
	"$(INTDIR)\Views.obj" \
	"$(INTDIR)\visibility.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\Word.obj" \
	"$(INTDIR)\writedoc.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\writexml.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xmlmodule.obj" \
	"$(INTDIR)\xpmhandler.obj" \
	"$(INTDIR)\Zoom.obj" \
	"$(OUTDIR)\libjpeg.lib" \
	"$(OUTDIR)\Libpng.lib"

"$(OUTDIR)\LibThotEditor.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "LibThotEditor - Win32 Release" || "$(CFG)" ==\
 "LibThotEditor - Win32 Debug"
SOURCE=..\Thotlib\View\Absboxes.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ABSBO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\page_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ABSBO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Absboxes.obj" : $(SOURCE) $(DEP_CPP_ABSBO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ABSBO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\page_tv.h"\
	

"$(INTDIR)\Absboxes.obj" : $(SOURCE) $(DEP_CPP_ABSBO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\abspictures.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ABSPI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ABSPI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\abspictures.obj" : $(SOURCE) $(DEP_CPP_ABSPI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ABSPI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\abspictures.obj" : $(SOURCE) $(DEP_CPP_ABSPI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Actions.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ACTIO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ACTIO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Actions.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ACTIO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	

"$(INTDIR)\Actions.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Alloca.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ALLOC=\
	"..\thotlib\include\ustring.h"\
	
NODEP_CPP_ALLOC=\
	"..\thotlib\base\config.h"\
	

"$(INTDIR)\Alloca.obj" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ALLOC=\
	"..\thotlib\include\ustring.h"\
	

"$(INTDIR)\Alloca.obj" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\appdialogue.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_APPDI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\dictionary_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\LiteClue_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\sortmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\LiteClue.h"\
	"..\thotlib\internals\h\logowindow.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_APPDI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\appdialogue.obj" : $(SOURCE) $(DEP_CPP_APPDI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_APPDI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\dictionary_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\sortmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\LiteClue.h"\
	"..\thotlib\internals\h\logowindow.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\appdialogue.obj" : $(SOURCE) $(DEP_CPP_APPDI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Appli.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_APPLI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\interface_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thot_key.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_APPLI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Appli.obj" : $(SOURCE) $(DEP_CPP_APPLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_APPLI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\interface_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thot_key.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Appli.obj" : $(SOURCE) $(DEP_CPP_APPLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\applicationapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_APPLIC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\checkaccess_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\schtrad_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\thotpattern.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\appevents_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	"..\thotlib\internals\var\thotpalette_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_APPLIC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\applicationapi.obj" : $(SOURCE) $(DEP_CPP_APPLIC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_APPLIC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\checkaccess_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\schtrad_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\thotpattern.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\appevents_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	"..\thotlib\internals\var\thotpalette_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\applicationapi.obj" : $(SOURCE) $(DEP_CPP_APPLIC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\attributeapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ATTRI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\attributeapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ATTRI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\attributeapi.obj" : $(SOURCE) $(DEP_CPP_ATTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ATTRI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\attributeapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\attributeapi.obj" : $(SOURCE) $(DEP_CPP_ATTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\attributes.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ATTRIB=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrmenu_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ATTRIB=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\attributes.obj" : $(SOURCE) $(DEP_CPP_ATTRIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ATTRIB=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrmenu_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\attributes.obj" : $(SOURCE) $(DEP_CPP_ATTRIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Attrmenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ATTRM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windialogapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ATTRM=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Attrmenu.obj" : $(SOURCE) $(DEP_CPP_ATTRM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ATTRM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windialogapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\Attrmenu.obj" : $(SOURCE) $(DEP_CPP_ATTRM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\attrpresent.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ATTRP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ATTRP=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\attrpresent.obj" : $(SOURCE) $(DEP_CPP_ATTRP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ATTRP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\attrpresent.obj" : $(SOURCE) $(DEP_CPP_ATTRP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\boxlocate.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BOXLO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BOXLO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\boxlocate.obj" : $(SOURCE) $(DEP_CPP_BOXLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BOXLO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\boxlocate.obj" : $(SOURCE) $(DEP_CPP_BOXLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\Boxmoves.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BOXMO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxrelations_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BOXMO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Boxmoves.obj" : $(SOURCE) $(DEP_CPP_BOXMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BOXMO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxrelations_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	

"$(INTDIR)\Boxmoves.obj" : $(SOURCE) $(DEP_CPP_BOXMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\boxparams.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BOXPA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BOXPA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\boxparams.obj" : $(SOURCE) $(DEP_CPP_BOXPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BOXPA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\boxparams.obj" : $(SOURCE) $(DEP_CPP_BOXPA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\boxpositions.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BOXPO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxrelations_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BOXPO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\boxpositions.obj" : $(SOURCE) $(DEP_CPP_BOXPO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BOXPO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxrelations_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\boxpositions.obj" : $(SOURCE) $(DEP_CPP_BOXPO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\boxrelations.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BOXRE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BOXRE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\boxrelations.obj" : $(SOURCE) $(DEP_CPP_BOXRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BOXRE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	

"$(INTDIR)\boxrelations.obj" : $(SOURCE) $(DEP_CPP_BOXRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\boxselection.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BOXSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BOXSE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\boxselection.obj" : $(SOURCE) $(DEP_CPP_BOXSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BOXSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\boxselection.obj" : $(SOURCE) $(DEP_CPP_BOXSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Browser.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BROWS=\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\thotdir.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BROWS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Browser.obj" : $(SOURCE) $(DEP_CPP_BROWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BROWS=\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\thotdir.h"\
	

"$(INTDIR)\Browser.obj" : $(SOURCE) $(DEP_CPP_BROWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\buildboxes.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BUILD=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxrelations_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BUILD=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\buildboxes.obj" : $(SOURCE) $(DEP_CPP_BUILD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BUILD=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxrelations_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\buildboxes.obj" : $(SOURCE) $(DEP_CPP_BUILD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\buildlines.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_BUILDL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\hyphen_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BUILDL=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\buildlines.obj" : $(SOURCE) $(DEP_CPP_BUILDL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_BUILDL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\hyphen_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	

"$(INTDIR)\buildlines.obj" : $(SOURCE) $(DEP_CPP_BUILDL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Callback.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CALLB=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\callbackinit_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appevents_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CALLB=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Callback.obj" : $(SOURCE) $(DEP_CPP_CALLB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CALLB=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\callbackinit_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appevents_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\Callback.obj" : $(SOURCE) $(DEP_CPP_CALLB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\callbackinit.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CALLBA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appevents_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CALLBA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\callbackinit.obj" : $(SOURCE) $(DEP_CPP_CALLBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CALLBA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appevents_tv.h"\
	

"$(INTDIR)\callbackinit.obj" : $(SOURCE) $(DEP_CPP_CALLBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\changeabsbox.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CHANG=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CHANG=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\changeabsbox.obj" : $(SOURCE) $(DEP_CPP_CHANG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CHANG=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\changeabsbox.obj" : $(SOURCE) $(DEP_CPP_CHANG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\changepresent.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CHANGE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\unstructchange_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CHANGE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\changepresent.obj" : $(SOURCE) $(DEP_CPP_CHANGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CHANGE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\unstructchange_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\changepresent.obj" : $(SOURCE) $(DEP_CPP_CHANGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\checkaccess.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CHECK=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\res.h"\
	"..\thotlib\internals\h\resdynmsg.h"\
	"..\thotlib\internals\h\SelectRestruct.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CHECK=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\checkaccess.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CHECK=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\res.h"\
	"..\thotlib\internals\h\resdynmsg.h"\
	"..\thotlib\internals\h\SelectRestruct.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\checkaccess.obj" : $(SOURCE) $(DEP_CPP_CHECK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\checkermenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CHECKE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\dictionary_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\savedoc_f.h"\
	"..\thotlib\internals\f\spellchecker_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constcorr.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\corrmenu.h"\
	"..\thotlib\internals\h\corrmsg.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\spell_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CHECKE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\checkermenu.obj" : $(SOURCE) $(DEP_CPP_CHECKE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CHECKE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\dictionary_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\savedoc_f.h"\
	"..\thotlib\internals\f\spellchecker_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constcorr.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\corrmenu.h"\
	"..\thotlib\internals\h\corrmsg.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\spell_tv.h"\
	

"$(INTDIR)\checkermenu.obj" : $(SOURCE) $(DEP_CPP_CHECKE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Closedoc.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CLOSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CLOSE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Closedoc.obj" : $(SOURCE) $(DEP_CPP_CLOSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CLOSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\Closedoc.obj" : $(SOURCE) $(DEP_CPP_CLOSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Colors.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_COLOR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_COLOR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Colors.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_COLOR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\Colors.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Config.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CONFI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CONFI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CONFI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\Config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Content\Content.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CONTE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CONTE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Content.obj" : $(SOURCE) $(DEP_CPP_CONTE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CONTE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\Content.obj" : $(SOURCE) $(DEP_CPP_CONTE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Content\contentapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CONTEN=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CONTEN=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\contentapi.obj" : $(SOURCE) $(DEP_CPP_CONTEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CONTEN=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\contentapi.obj" : $(SOURCE) $(DEP_CPP_CONTEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Context.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CONTEX=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\checkermenu_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CONTEX=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Context.obj" : $(SOURCE) $(DEP_CPP_CONTEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CONTEX=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\checkermenu_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\Context.obj" : $(SOURCE) $(DEP_CPP_CONTEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\createabsbox.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CREAT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CREAT=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\createabsbox.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CREAT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\createabsbox.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\createdoc.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CREATE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\opendoc_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CREATE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\createdoc.obj" : $(SOURCE) $(DEP_CPP_CREATE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CREATE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\opendoc_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\createdoc.obj" : $(SOURCE) $(DEP_CPP_CREATE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\createpages.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CREATEP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CREATEP=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\createpages.obj" : $(SOURCE) $(DEP_CPP_CREATEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CREATEP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\createpages.obj" : $(SOURCE) $(DEP_CPP_CREATEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\creationmenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_CREATI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CREATI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\creationmenu.obj" : $(SOURCE) $(DEP_CPP_CREATI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_CREATI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\creationmenu.obj" : $(SOURCE) $(DEP_CPP_CREATI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\dialogapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_DIALO=\
	"..\thotlib\dialogue\winerrdata.c"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\copyright.xbm"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DIALO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\dialogapi.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_DIALO=\
	"..\thotlib\dialogue\winerrdata.c"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\copyright.xbm"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\dialogapi.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Content\dictionary.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_DICTI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DICTI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\dictionary.obj" : $(SOURCE) $(DEP_CPP_DICTI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_DICTI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\dictionary.obj" : $(SOURCE) $(DEP_CPP_DICTI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\displaybox.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_DISPL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\displaybox_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DISPL=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\displaybox.obj" : $(SOURCE) $(DEP_CPP_DISPL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_DISPL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\displaybox_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	

"$(INTDIR)\displaybox.obj" : $(SOURCE) $(DEP_CPP_DISPL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\displayselect.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_DISPLA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DISPLA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\displayselect.obj" : $(SOURCE) $(DEP_CPP_DISPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_DISPLA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\displayselect.obj" : $(SOURCE) $(DEP_CPP_DISPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Docs.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_DOCS_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmodule_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DOCS_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Docs.obj" : $(SOURCE) $(DEP_CPP_DOCS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_DOCS_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmodule_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\Docs.obj" : $(SOURCE) $(DEP_CPP_DOCS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\documentapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_DOCUM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DOCUM=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\documentapi.obj" : $(SOURCE) $(DEP_CPP_DOCUM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_DOCUM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\documentapi.obj" : $(SOURCE) $(DEP_CPP_DOCUM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\Draw.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_DRAW_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DRAW_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_DRAW_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\Draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\editcommands.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_EDITC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\unstructchange_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EDITC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\editcommands.obj" : $(SOURCE) $(DEP_CPP_EDITC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_EDITC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\unstructchange_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\editcommands.obj" : $(SOURCE) $(DEP_CPP_EDITC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\epshandler.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_EPSHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EPSHA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\epshandler.obj" : $(SOURCE) $(DEP_CPP_EPSHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_EPSHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	

"$(INTDIR)\epshandler.obj" : $(SOURCE) $(DEP_CPP_EPSHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\exceptions.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_EXCEP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EXCEP=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\exceptions.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_EXCEP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\exceptions.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\externalref.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_EXTER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EXTER=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\externalref.obj" : $(SOURCE) $(DEP_CPP_EXTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_EXTER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\externalref.obj" : $(SOURCE) $(DEP_CPP_EXTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\extprintmenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_EXTPR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\print.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\printmenu_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EXTPR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\extprintmenu.obj" : $(SOURCE) $(DEP_CPP_EXTPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_EXTPR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\print.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\printmenu_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	

"$(INTDIR)\extprintmenu.obj" : $(SOURCE) $(DEP_CPP_EXTPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\fileaccess.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_FILEA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FILEA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\fileaccess.obj" : $(SOURCE) $(DEP_CPP_FILEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_FILEA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\fileaccess.obj" : $(SOURCE) $(DEP_CPP_FILEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Font.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_FONT_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FONT_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_FONT_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\Font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\Frame.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_FRAME=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\displaybox_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FRAME=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_FRAME=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\displaybox_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\genericdriver.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_GENER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\genericdriver.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\presentdriver.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presentdriver_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GENER=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\genericdriver.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_GENER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\genericdriver.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\presentdriver.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presentdriver_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\genericdriver.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Geom.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_GEOM_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GEOM_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Geom.obj" : $(SOURCE) $(DEP_CPP_GEOM_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_GEOM_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\Geom.obj" : $(SOURCE) $(DEP_CPP_GEOM_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\gifhandler.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_GIFHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GIFHA=\
	"..\thotlib\include\HTVMSUtils.h"\
	"..\thotlib\internals\h\amigax.h"\
	

"$(INTDIR)\gifhandler.obj" : $(SOURCE) $(DEP_CPP_GIFHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_GIFHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\gifhandler.obj" : $(SOURCE) $(DEP_CPP_GIFHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\Hyphen.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_HYPHE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HYPHE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Hyphen.obj" : $(SOURCE) $(DEP_CPP_HYPHE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_HYPHE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	

"$(INTDIR)\Hyphen.obj" : $(SOURCE) $(DEP_CPP_HYPHE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\view\inites.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_INITE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\pattern.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_INITE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\inites.obj" : $(SOURCE) $(DEP_CPP_INITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_INITE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\pattern.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\inites.obj" : $(SOURCE) $(DEP_CPP_INITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Input.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_INPUT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thot_key.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_INPUT=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Input.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_INPUT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thot_key.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\Input.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\interface.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_INTER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\displaybox_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_INTER=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\interface.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_INTER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\displaybox_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\interface.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\jpeghandler.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_JPEGH=\
	"..\libjpeg\jconfig.h"\
	"..\libjpeg\jerror.h"\
	"..\libjpeg\jinclude.h"\
	"..\libjpeg\jmorecfg.h"\
	"..\libjpeg\jpegint.h"\
	"..\libjpeg\jpeglib.h"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_JPEGH=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\jpeghandler.obj" : $(SOURCE) $(DEP_CPP_JPEGH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_JPEGH=\
	"..\libjpeg\jconfig.h"\
	"..\libjpeg\jinclude.h"\
	"..\libjpeg\jmorecfg.h"\
	"..\libjpeg\jpeglib.h"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	

"$(INTDIR)\jpeghandler.obj" : $(SOURCE) $(DEP_CPP_JPEGH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\keyboards.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_KEYBO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_KEYBO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\keyboards.obj" : $(SOURCE) $(DEP_CPP_KEYBO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_KEYBO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\keyboards.obj" : $(SOURCE) $(DEP_CPP_KEYBO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\labelalloc.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_LABEL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LABEL=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\labelalloc.obj" : $(SOURCE) $(DEP_CPP_LABEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_LABEL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\labelalloc.obj" : $(SOURCE) $(DEP_CPP_LABEL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Language.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_LANGU=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LANGU=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_LANGU=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\Language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\LiteClue.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_LITEC=\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\h\LiteClue.h"\
	"..\thotlib\internals\h\LiteClueP.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LITEC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\LiteClue.obj" : $(SOURCE) $(DEP_CPP_LITEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_LITEC=\
	"..\thotlib\include\ustring.h"\
	

"$(INTDIR)\LiteClue.obj" : $(SOURCE) $(DEP_CPP_LITEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Lookup.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_LOOKU=\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LOOKU=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Lookup.obj" : $(SOURCE) $(DEP_CPP_LOOKU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_LOOKU=\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	

"$(INTDIR)\Lookup.obj" : $(SOURCE) $(DEP_CPP_LOOKU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Memory.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_MEMOR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MEMOR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_MEMOR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\Memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Message.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_MESSA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MESSA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_MESSA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\Message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\modiftype.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_MODIF=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\attrmenu_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\creationmenu_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\picturemenu_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MODIF=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\modiftype.obj" : $(SOURCE) $(DEP_CPP_MODIF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_MODIF=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\attrmenu_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\creationmenu_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\input_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\picturemenu_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\modiftype.obj" : $(SOURCE) $(DEP_CPP_MODIF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Opendoc.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_OPEND=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\opendoc_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OPEND=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Opendoc.obj" : $(SOURCE) $(DEP_CPP_OPEND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_OPEND=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\opendoc_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Opendoc.obj" : $(SOURCE) $(DEP_CPP_OPEND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Openview.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_OPENV=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OPENV=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Openview.obj" : $(SOURCE) $(DEP_CPP_OPENV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_OPENV=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\Openview.obj" : $(SOURCE) $(DEP_CPP_OPENV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\pagecommands.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PAGEC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\pagecommands_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\page_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PAGEC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\pagecommands.obj" : $(SOURCE) $(DEP_CPP_PAGEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PAGEC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\pagecommands_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\page_tv.h"\
	

"$(INTDIR)\pagecommands.obj" : $(SOURCE) $(DEP_CPP_PAGEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\Paginate.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PAGIN=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\pagecommands_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\print_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PAGIN=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Paginate.obj" : $(SOURCE) $(DEP_CPP_PAGIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PAGIN=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\pagecommands_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\print_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	

"$(INTDIR)\Paginate.obj" : $(SOURCE) $(DEP_CPP_PAGIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\xml\parsexml.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PARSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\thotmodule_f.h"\
	"..\thotlib\internals\f\translatexml_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PARSE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\parsexml.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PARSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\thotmodule_f.h"\
	"..\thotlib\internals\f\translatexml_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\parsexml.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\Picture.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PICTU=\
	"..\libpng\png.h"\
	"..\libpng\pngconf.h"\
	"..\libpng\zlib\zconf.h"\
	"..\libpng\zlib\zlib.h"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\epshandler_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\jpeghandler_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\pnghandler_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\xbmhandler_f.h"\
	"..\thotlib\internals\f\xpmhandler_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\epsflogo.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\lost.xpm"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PICTU=\
	"..\libpng\alloc.h"\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Picture.obj" : $(SOURCE) $(DEP_CPP_PICTU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PICTU=\
	"..\libpng\png.h"\
	"..\libpng\pngconf.h"\
	"..\libpng\zlib\zconf.h"\
	"..\libpng\zlib\zlib.h"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\epshandler_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\jpeghandler_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\pnghandler_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\xbmhandler_f.h"\
	"..\thotlib\internals\f\xpmhandler_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\epsflogo.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\lost.xpm"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\winsys.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\Picture.obj" : $(SOURCE) $(DEP_CPP_PICTU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\pictureapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PICTUR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PICTUR=\
	"..\thotlib\include\HTVMSUtils.h"\
	"..\thotlib\internals\h\amigax.h"\
	

"$(INTDIR)\pictureapi.obj" : $(SOURCE) $(DEP_CPP_PICTUR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PICTUR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\pictureapi.obj" : $(SOURCE) $(DEP_CPP_PICTUR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\picturebase.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PICTURE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PICTURE=\
	"..\thotlib\include\HTVMSUtils.h"\
	"..\thotlib\internals\h\amigax.h"\
	

"$(INTDIR)\picturebase.obj" : $(SOURCE) $(DEP_CPP_PICTURE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PICTURE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\picturebase.obj" : $(SOURCE) $(DEP_CPP_PICTURE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\picturemenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PICTUREM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\savedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PICTUREM=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\picturemenu.obj" : $(SOURCE) $(DEP_CPP_PICTUREM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PICTUREM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\savedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\picturemenu.obj" : $(SOURCE) $(DEP_CPP_PICTUREM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Pivot.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PIVOT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PIVOT=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Pivot.obj" : $(SOURCE) $(DEP_CPP_PIVOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PIVOT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\Pivot.obj" : $(SOURCE) $(DEP_CPP_PIVOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Platform.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PLATF=\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PLATF=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Platform.obj" : $(SOURCE) $(DEP_CPP_PLATF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PLATF=\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	

"$(INTDIR)\Platform.obj" : $(SOURCE) $(DEP_CPP_PLATF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\pnghandler.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PNGHA=\
	"..\libpng\png.h"\
	"..\libpng\pngconf.h"\
	"..\libpng\zlib\zconf.h"\
	"..\libpng\zlib\zlib.h"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGHA=\
	"..\libpng\alloc.h"\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\pnghandler.obj" : $(SOURCE) $(DEP_CPP_PNGHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PNGHA=\
	"..\libpng\png.h"\
	"..\libpng\pngconf.h"\
	"..\libpng\zlib\zconf.h"\
	"..\libpng\zlib\zlib.h"\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\gifhandler_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	

"$(INTDIR)\pnghandler.obj" : $(SOURCE) $(DEP_CPP_PNGHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\presentationapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PRESE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\compilmsg_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRESE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\presentationapi.obj" : $(SOURCE) $(DEP_CPP_PRESE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PRESE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\compilmsg_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\presentationapi.obj" : $(SOURCE) $(DEP_CPP_PRESE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\presentdriver.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PRESEN=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\presentdriver.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRESEN=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\presentdriver.obj" : $(SOURCE) $(DEP_CPP_PRESEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PRESEN=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\presentdriver.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\presentdriver.obj" : $(SOURCE) $(DEP_CPP_PRESEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\presentmenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PRESENT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\unstructchange_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\windialogapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRESENT=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\presentmenu.obj" : $(SOURCE) $(DEP_CPP_PRESENT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PRESENT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\unstructchange_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\windialogapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\presentmenu.obj" : $(SOURCE) $(DEP_CPP_PRESENT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\presrules.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PRESR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRESR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\presrules.obj" : $(SOURCE) $(DEP_CPP_PRESR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PRESR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\boxpositions_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\presrules.obj" : $(SOURCE) $(DEP_CPP_PRESR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\presvariables.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PRESV=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRESV=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\presvariables.obj" : $(SOURCE) $(DEP_CPP_PRESV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PRESV=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\presvariables.obj" : $(SOURCE) $(DEP_CPP_PRESV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\printmenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PRINT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\print.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attrmenu_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\creationmenu_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presentmenu_f.h"\
	"..\thotlib\internals\f\printmenu_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\thotprinter_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\writedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PRINT=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\printmenu.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PRINT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\print.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attrmenu_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\creationmenu_f.h"\
	"..\thotlib\internals\f\dialogapi_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presentmenu_f.h"\
	"..\thotlib\internals\f\printmenu_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\f\writedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	

"$(INTDIR)\printmenu.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\pschemaapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_PSCHE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\compilmsg_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PSCHE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\pschemaapi.obj" : $(SOURCE) $(DEP_CPP_PSCHE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_PSCHE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\compilmsg_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\pschemaapi.obj" : $(SOURCE) $(DEP_CPP_PSCHE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Quit.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_QUIT_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\closedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_QUIT_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Quit.obj" : $(SOURCE) $(DEP_CPP_QUIT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_QUIT_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\closedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\Quit.obj" : $(SOURCE) $(DEP_CPP_QUIT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\readpivot.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_READP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_READP=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\readpivot.obj" : $(SOURCE) $(DEP_CPP_READP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_READP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\readpivot.obj" : $(SOURCE) $(DEP_CPP_READP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Readprs.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_READPR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readprs_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_READPR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Readprs.obj" : $(SOURCE) $(DEP_CPP_READPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_READPR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readprs_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Readprs.obj" : $(SOURCE) $(DEP_CPP_READPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Readstr.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_READS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_READS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Readstr.obj" : $(SOURCE) $(DEP_CPP_READS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_READS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Readstr.obj" : $(SOURCE) $(DEP_CPP_READS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Readtra.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_READT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\readtra_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_READT=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Readtra.obj" : $(SOURCE) $(DEP_CPP_READT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_READT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\message_f.h"\
	"..\thotlib\internals\f\readtra_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Readtra.obj" : $(SOURCE) $(DEP_CPP_READT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\referenceapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_REFER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\reference.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemastr_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_REFER=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\referenceapi.obj" : $(SOURCE) $(DEP_CPP_REFER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_REFER=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\reference.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemastr_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\referenceapi.obj" : $(SOURCE) $(DEP_CPP_REFER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\references.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_REFERE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_REFERE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\references.obj" : $(SOURCE) $(DEP_CPP_REFERE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_REFERE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\references.obj" : $(SOURCE) $(DEP_CPP_REFERE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Registry.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_REGIS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_REGIS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_REGIS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\Res.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_RES_C=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\resgen_f.h"\
	"..\thotlib\internals\f\resmatch_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typestr.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_RES_C=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Res.obj" : $(SOURCE) $(DEP_CPP_RES_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_RES_C=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\resgen_f.h"\
	"..\thotlib\internals\f\resmatch_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\Res.obj" : $(SOURCE) $(DEP_CPP_RES_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\Resgen.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_RESGE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\resmatch_f.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typestr.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_RESGE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Resgen.obj" : $(SOURCE) $(DEP_CPP_RESGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_RESGE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\resmatch_f.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\Resgen.obj" : $(SOURCE) $(DEP_CPP_RESGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\Resmatch.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_RESMA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typestr.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_RESMA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Resmatch.obj" : $(SOURCE) $(DEP_CPP_RESMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_RESMA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\typestr.h"\
	

"$(INTDIR)\Resmatch.obj" : $(SOURCE) $(DEP_CPP_RESMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Savedoc.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SAVED=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createdoc_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\opendoc_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\writedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SAVED=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Savedoc.obj" : $(SOURCE) $(DEP_CPP_SAVED) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SAVED=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\browser_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createdoc_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\opendoc_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\writedoc_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Savedoc.obj" : $(SOURCE) $(DEP_CPP_SAVED) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\xml\savexml.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SAVEX=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\parsexml_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writexml_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SAVEX=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\savexml.obj" : $(SOURCE) $(DEP_CPP_SAVEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SAVEX=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\parsexml_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writexml_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	

"$(INTDIR)\savexml.obj" : $(SOURCE) $(DEP_CPP_SAVEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Schemas.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SCHEM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readprs_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SCHEM=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Schemas.obj" : $(SOURCE) $(DEP_CPP_SCHEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SCHEM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readprs_f.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\Schemas.obj" : $(SOURCE) $(DEP_CPP_SCHEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Schtrad.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SCHTR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\f\readtra_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\schtrad_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SCHTR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Schtrad.obj" : $(SOURCE) $(DEP_CPP_SCHTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SCHTR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\readstr_f.h"\
	"..\thotlib\internals\f\readtra_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\schtrad_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\Schtrad.obj" : $(SOURCE) $(DEP_CPP_SCHTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\Scroll.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SCROL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SCROL=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Scroll.obj" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SCROL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	

"$(INTDIR)\Scroll.obj" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Content\Search.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SEARC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SEARC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SEARC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\searchmenu.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SEARCH=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\regexp_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SEARCH=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\searchmenu.obj" : $(SOURCE) $(DEP_CPP_SEARCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SEARCH=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\regexp_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\searchmenu.obj" : $(SOURCE) $(DEP_CPP_SEARCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\searchmenustr.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SEARCHM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\regexp_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SEARCHM=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\searchmenustr.obj" : $(SOURCE) $(DEP_CPP_SEARCHM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SEARCHM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\regexp_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\searchmenustr.obj" : $(SOURCE) $(DEP_CPP_SEARCHM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Content\searchref.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SEARCHR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SEARCHR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\searchref.obj" : $(SOURCE) $(DEP_CPP_SEARCHR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SEARCHR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchmenu_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\searchref.obj" : $(SOURCE) $(DEP_CPP_SEARCHR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\selectionapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SELEC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SELEC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\selectionapi.obj" : $(SOURCE) $(DEP_CPP_SELEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SELEC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\selectionapi.obj" : $(SOURCE) $(DEP_CPP_SELEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\presentation\specificdriver.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SPECI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\presentdriver.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\specificdriver.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presentationapi_f.h"\
	"..\thotlib\internals\f\presentdriver_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SPECI=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\specificdriver.obj" : $(SOURCE) $(DEP_CPP_SPECI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SPECI=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\presentdriver.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\specificdriver.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presentationapi_f.h"\
	"..\thotlib\internals\f\presentdriver_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\specificdriver.obj" : $(SOURCE) $(DEP_CPP_SPECI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\spellchecker.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_SPELL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\dictionary_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\hyphen_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\spellchecker_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\constcorr.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmot.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\corrmsg.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\spell_tv.h"\
	"..\thotlib\internals\var\word_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SPELL=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\spellchecker.obj" : $(SOURCE) $(DEP_CPP_SPELL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_SPELL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\dictionary_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\hyphen_f.h"\
	"..\thotlib\internals\f\registry_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\spellchecker_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\word_f.h"\
	"..\thotlib\internals\h\constcorr.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmot.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\corrmsg.h"\
	"..\thotlib\internals\h\dictionary.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\spell_tv.h"\
	"..\thotlib\internals\var\word_tv.h"\
	

"$(INTDIR)\spellchecker.obj" : $(SOURCE) $(DEP_CPP_SPELL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\structchange.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structlocate_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structchange.obj" : $(SOURCE) $(DEP_CPP_STRUC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structlocate_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\structchange.obj" : $(SOURCE) $(DEP_CPP_STRUC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\structcommands.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUCT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\res_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUCT=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structcommands.obj" : $(SOURCE) $(DEP_CPP_STRUCT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUCT=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\res_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\structcommands.obj" : $(SOURCE) $(DEP_CPP_STRUCT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\structcreation.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUCTC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\creationmenu_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUCTC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structcreation.obj" : $(SOURCE) $(DEP_CPP_STRUCTC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUCTC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\creationmenu_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\structcreation.obj" : $(SOURCE) $(DEP_CPP_STRUCTC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\structlist.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUCTL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUCTL=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structlist.obj" : $(SOURCE) $(DEP_CPP_STRUCTL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUCTL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\structlist.obj" : $(SOURCE) $(DEP_CPP_STRUCTL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\structlocate.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUCTLO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUCTLO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structlocate.obj" : $(SOURCE) $(DEP_CPP_STRUCTLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUCTLO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\structlocate.obj" : $(SOURCE) $(DEP_CPP_STRUCTLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\structmodif.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUCTM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\res.h"\
	"..\thotlib\internals\h\resdynmsg.h"\
	"..\thotlib\internals\h\SelectRestruct.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUCTM=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structmodif.obj" : $(SOURCE) $(DEP_CPP_STRUCTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUCTM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\res.h"\
	"..\thotlib\internals\h\resdynmsg.h"\
	"..\thotlib\internals\h\SelectRestruct.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\structmodif.obj" : $(SOURCE) $(DEP_CPP_STRUCTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\structschema.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUCTS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\schemastr_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUCTS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structschema.obj" : $(SOURCE) $(DEP_CPP_STRUCTS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUCTS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\schemastr_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\structschema.obj" : $(SOURCE) $(DEP_CPP_STRUCTS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\structselect.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_STRUCTSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_STRUCTSE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\structselect.obj" : $(SOURCE) $(DEP_CPP_STRUCTSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_STRUCTSE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\displayselect_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\keyboards_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\structselect.obj" : $(SOURCE) $(DEP_CPP_STRUCTSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\textcommands.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_TEXTC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TEXTC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\textcommands.obj" : $(SOURCE) $(DEP_CPP_TEXTC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_TEXTC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\windowdisplay_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\textcommands.obj" : $(SOURCE) $(DEP_CPP_TEXTC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\xml\thotmodule.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_THOTM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\parsexml_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_THOTM=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\thotmodule.obj" : $(SOURCE) $(DEP_CPP_THOTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_THOTM=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\parsexml_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\thotmodule.obj" : $(SOURCE) $(DEP_CPP_THOTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Thotmsg.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_THOTMS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_THOTMS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Thotmsg.obj" : $(SOURCE) $(DEP_CPP_THOTMS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_THOTMS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\Thotmsg.obj" : $(SOURCE) $(DEP_CPP_THOTMS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\xml\translatexml.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_TRANS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typexml.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TRANS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\translatexml.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_TRANS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typexml.h"\
	

"$(INTDIR)\translatexml.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\translation.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_TRANSL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readprs_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\schtrad_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TRANSL=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\translation.obj" : $(SOURCE) $(DEP_CPP_TRANSL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_TRANSL=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\readprs_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\schtrad_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\translation.obj" : $(SOURCE) $(DEP_CPP_TRANSL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\Tree.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_TREE_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\res.h"\
	"..\thotlib\internals\h\resdynmsg.h"\
	"..\thotlib\internals\h\SelectRestruct.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TREE_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_TREE_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\abspictures_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\res.h"\
	"..\thotlib\internals\h\resdynmsg.h"\
	"..\thotlib\internals\h\SelectRestruct.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\Tree.obj" : $(SOURCE) $(DEP_CPP_TREE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Tree\Treeapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_TREEA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributeapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\documentapi_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TREEA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Treeapi.obj" : $(SOURCE) $(DEP_CPP_TREEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_TREEA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributeapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\documentapi_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\translation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\Treeapi.obj" : $(SOURCE) $(DEP_CPP_TREEA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\unicode\uaccess.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_UACCE=\
	"..\thotlib\include\uaccess.h"\
	"..\thotlib\include\ustring.h"\
	

"$(INTDIR)\uaccess.obj" : $(SOURCE) $(DEP_CPP_UACCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_UACCE=\
	"..\thotlib\include\uaccess.h"\
	"..\thotlib\include\ustring.h"\
	

"$(INTDIR)\uaccess.obj" : $(SOURCE) $(DEP_CPP_UACCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\unicode\uconvert.c
DEP_CPP_UCONV=\
	"..\thotlib\include\uconvert.h"\
	"..\thotlib\include\ustring.h"\
	

"$(INTDIR)\uconvert.obj" : $(SOURCE) $(DEP_CPP_UCONV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\thotlib\editing\undo.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_UNDO_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UNDO_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_UNDO_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\constres.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\editing\undoapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_UNDOA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UNDOA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\undoapi.obj" : $(SOURCE) $(DEP_CPP_UNDOA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_UNDOA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	

"$(INTDIR)\undoapi.obj" : $(SOURCE) $(DEP_CPP_UNDOA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Base\Units.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_UNITS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UNITS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Units.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_UNITS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\Units.obj" : $(SOURCE) $(DEP_CPP_UNITS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\unstructchange.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_UNSTR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\unstructlocate_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UNSTR=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\unstructchange.obj" : $(SOURCE) $(DEP_CPP_UNSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_UNSTR=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\contentapi_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\editcommands_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\selectmenu_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\undo_f.h"\
	"..\thotlib\internals\f\unstructlocate_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\modif.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\unstructchange.obj" : $(SOURCE) $(DEP_CPP_UNSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\unstructlocate.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_UNSTRU=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UNSTRU=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\unstructlocate.obj" : $(SOURCE) $(DEP_CPP_UNSTRU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_UNSTRU=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxmoves_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\unstructlocate.obj" : $(SOURCE) $(DEP_CPP_UNSTRU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\unicode\ustring.c
DEP_CPP_USTRI=\
	"..\thotlib\include\ustring.h"\
	

"$(INTDIR)\ustring.obj" : $(SOURCE) $(DEP_CPP_USTRI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Thotlib\View\Viewapi.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_VIEWA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VIEWA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Viewapi.obj" : $(SOURCE) $(DEP_CPP_VIEWA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_VIEWA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\selection.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\attrpresent_f.h"\
	"..\thotlib\internals\f\boxlocate_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\geom_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\presrules_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\scroll_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\boxes_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	

"$(INTDIR)\Viewapi.obj" : $(SOURCE) $(DEP_CPP_VIEWA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Editing\viewcommands.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_VIEWC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\savedoc_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VIEWC=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\viewcommands.obj" : $(SOURCE) $(DEP_CPP_VIEWC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_VIEWC=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\exceptions_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\savedoc_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structlist_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\textcommands_f.h"\
	"..\thotlib\internals\f\thotmsg_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\creation_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\viewcommands.obj" : $(SOURCE) $(DEP_CPP_VIEWC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\Views.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_VIEWS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\closedoc_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VIEWS=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Views.obj" : $(SOURCE) $(DEP_CPP_VIEWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_VIEWS=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\closedoc_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\frame_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\Views.obj" : $(SOURCE) $(DEP_CPP_VIEWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\visibility.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_VISIB=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VISIB=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\visibility.obj" : $(SOURCE) $(DEP_CPP_VISIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_VISIB=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\visibility.obj" : $(SOURCE) $(DEP_CPP_VISIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\View\windowdisplay.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_WINDO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WINDO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\windowdisplay.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_WINDO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\buildlines_f.h"\
	"..\thotlib\internals\f\context_f.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\win_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotcolor.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\font_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\thotcolor_tv.h"\
	"..\thotlib\internals\var\units_tv.h"\
	

"$(INTDIR)\windowdisplay.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Content\Word.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_WORD_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\hyphen_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WORD_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Word.obj" : $(SOURCE) $(DEP_CPP_WORD_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_WORD_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\hyphen_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	

"$(INTDIR)\Word.obj" : $(SOURCE) $(DEP_CPP_WORD_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\document\writedoc.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_WRITE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WRITE=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\writedoc.obj" : $(SOURCE) $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_WRITE=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\absboxes_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\appli_f.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\boxselection_f.h"\
	"..\thotlib\internals\f\buildboxes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changeabsbox_f.h"\
	"..\thotlib\internals\f\config_f.h"\
	"..\thotlib\internals\f\createabsbox_f.h"\
	"..\thotlib\internals\f\createpages_f.h"\
	"..\thotlib\internals\f\docs_f.h"\
	"..\thotlib\internals\f\draw_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\paginate_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\presvariables_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\search_f.h"\
	"..\thotlib\internals\f\searchref_f.h"\
	"..\thotlib\internals\f\structcommands_f.h"\
	"..\thotlib\internals\f\structcreation_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\structselect_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\f\viewcommands_f.h"\
	"..\thotlib\internals\f\views_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\edit_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\page_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	"..\thotlib\internals\var\print_tv.h"\
	"..\thotlib\internals\var\select_tv.h"\
	

"$(INTDIR)\writedoc.obj" : $(SOURCE) $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Document\writepivot.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_WRITEP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WRITEP=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\writepivot.obj" : $(SOURCE) $(DEP_CPP_WRITEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_WRITEP=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\labelAllocator.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\applicationapi_f.h"\
	"..\thotlib\internals\f\attributes_f.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\changepresent_f.h"\
	"..\thotlib\internals\f\content_f.h"\
	"..\thotlib\internals\f\externalref_f.h"\
	"..\thotlib\internals\f\fileaccess_f.h"\
	"..\thotlib\internals\f\labelalloc_f.h"\
	"..\thotlib\internals\f\language_f.h"\
	"..\thotlib\internals\f\memory_f.h"\
	"..\thotlib\internals\f\platform_f.h"\
	"..\thotlib\internals\f\readpivot_f.h"\
	"..\thotlib\internals\f\references_f.h"\
	"..\thotlib\internals\f\schemas_f.h"\
	"..\thotlib\internals\f\structmodif_f.h"\
	"..\thotlib\internals\f\structschema_f.h"\
	"..\thotlib\internals\f\tree_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\f\writepivot_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotdir.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\modif_tv.h"\
	"..\thotlib\internals\var\platform_tv.h"\
	

"$(INTDIR)\writepivot.obj" : $(SOURCE) $(DEP_CPP_WRITEP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\xml\writexml.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_WRITEX=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\reference.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\translatexml_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WRITEX=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\writexml.obj" : $(SOURCE) $(DEP_CPP_WRITEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_WRITEX=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\reference.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\callback_f.h"\
	"..\thotlib\internals\f\translatexml_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constpiv.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	

"$(INTDIR)\writexml.obj" : $(SOURCE) $(DEP_CPP_WRITEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\xbmhandler.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_XBMHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_XBMHA=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\xbmhandler.obj" : $(SOURCE) $(DEP_CPP_XBMHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_XBMHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	

"$(INTDIR)\xbmhandler.obj" : $(SOURCE) $(DEP_CPP_XBMHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thotlib\xml\xmlmodule.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_XMLMO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\reference.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\parsexml_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_XMLMO=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\xmlmodule.obj" : $(SOURCE) $(DEP_CPP_XMLMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_XMLMO=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\application.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\content.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\fileaccess.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\reference.h"\
	"..\thotlib\include\registry.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\parsexml_f.h"\
	"..\thotlib\internals\f\xmlmodule_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\constxml.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\typexml.h"\
	

"$(INTDIR)\xmlmodule.obj" : $(SOURCE) $(DEP_CPP_XMLMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Image\xpmhandler.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_XPMHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_XPMHA=\
	"..\thotlib\include\HTVMSUtils.h"\
	"..\thotlib\internals\h\amigax.h"\
	

"$(INTDIR)\xpmhandler.obj" : $(SOURCE) $(DEP_CPP_XPMHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_XPMHA=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\font_f.h"\
	"..\thotlib\internals\f\inites_f.h"\
	"..\thotlib\internals\f\picture_f.h"\
	"..\thotlib\internals\f\units_f.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\picture.h"\
	"..\thotlib\internals\h\simx.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\h\xpm.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	"..\thotlib\internals\var\picture_tv.h"\
	

"$(INTDIR)\xpmhandler.obj" : $(SOURCE) $(DEP_CPP_XPMHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Thotlib\Dialogue\Zoom.c

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

DEP_CPP_ZOOM_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ZOOM_=\
	"..\thotlib\include\HTVMSUtils.h"\
	

"$(INTDIR)\Zoom.obj" : $(SOURCE) $(DEP_CPP_ZOOM_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

DEP_CPP_ZOOM_=\
	"..\thotlib\include\appaction.h"\
	"..\thotlib\include\appstruct.h"\
	"..\thotlib\include\attribute.h"\
	"..\thotlib\include\dialog.h"\
	"..\thotlib\include\document.h"\
	"..\thotlib\include\interface.h"\
	"..\thotlib\include\language.h"\
	"..\thotlib\include\libmsg.h"\
	"..\thotlib\include\message.h"\
	"..\thotlib\include\presentation.h"\
	"..\thotlib\include\pschema.h"\
	"..\thotlib\include\simx.h"\
	"..\thotlib\include\sysdep.h"\
	"..\thotlib\include\thot_gui.h"\
	"..\thotlib\include\thot_sys.h"\
	"..\thotlib\include\tree.h"\
	"..\thotlib\include\typebase.h"\
	"..\thotlib\include\ustring.h"\
	"..\thotlib\include\view.h"\
	"..\thotlib\internals\f\actions_f.h"\
	"..\thotlib\internals\f\appdialogue_f.h"\
	"..\thotlib\internals\f\boxparams_f.h"\
	"..\thotlib\internals\f\viewapi_f.h"\
	"..\thotlib\internals\h\appdialogue.h"\
	"..\thotlib\internals\h\constint.h"\
	"..\thotlib\internals\h\constmedia.h"\
	"..\thotlib\internals\h\constmenu.h"\
	"..\thotlib\internals\h\constprs.h"\
	"..\thotlib\internals\h\conststr.h"\
	"..\thotlib\internals\h\consttra.h"\
	"..\thotlib\internals\h\frame.h"\
	"..\thotlib\internals\h\thotkey.h"\
	"..\thotlib\internals\h\typecorr.h"\
	"..\thotlib\internals\h\typeint.h"\
	"..\thotlib\internals\h\typemedia.h"\
	"..\thotlib\internals\h\typeprs.h"\
	"..\thotlib\internals\h\typestr.h"\
	"..\thotlib\internals\h\typetra.h"\
	"..\thotlib\internals\var\appdialogue_tv.h"\
	"..\thotlib\internals\var\frame_tv.h"\
	

"$(INTDIR)\Zoom.obj" : $(SOURCE) $(DEP_CPP_ZOOM_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

"libpng - Win32 Release" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Release" 
   cd ".."

"libpng - Win32 ReleaseCLEAN" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libpng.mak CFG="libpng - Win32 Release"\
 RECURSE=1 
   cd ".."

!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

"libpng - Win32 Debug" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Debug" 
   cd ".."

"libpng - Win32 DebugCLEAN" : 
   cd ".\libpng"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libpng.mak CFG="libpng - Win32 Debug"\
 RECURSE=1 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "LibThotEditor - Win32 Release"

"libjpeg - Win32 Release" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Release" 
   cd ".."

"libjpeg - Win32 ReleaseCLEAN" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libjpeg.mak CFG="libjpeg - Win32 Release"\
 RECURSE=1 
   cd ".."

!ELSEIF  "$(CFG)" == "LibThotEditor - Win32 Debug"

"libjpeg - Win32 Debug" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Debug" 
   cd ".."

"libjpeg - Win32 DebugCLEAN" : 
   cd ".\libjpeg"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\libjpeg.mak CFG="libjpeg - Win32 Debug"\
 RECURSE=1 
   cd ".."

!ENDIF 


!ENDIF 

