# Microsoft Developer Studio Generated NMAKE File, Based on libThotEditor.dsp
!IF "$(CFG)" == ""
CFG=libThotEditor - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libThotEditor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libThotEditor - Win32 Release" && "$(CFG)" != "libThotEditor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libThotEditor.mak" CFG="libThotEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libThotEditor - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libThotEditor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libThotEditor - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libThotEditor.lib"

!ELSE 

ALL : "libpng - Win32 Release" "libjpeg - Win32 Release" "$(OUTDIR)\libThotEditor.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libjpeg - Win32 ReleaseCLEAN" "libpng - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\absboxes.obj"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\alloca.obj"
	-@erase "$(INTDIR)\appdialogue.obj"
	-@erase "$(INTDIR)\appli.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributeapi.obj"
	-@erase "$(INTDIR)\attributes.obj"
	-@erase "$(INTDIR)\attrmenu.obj"
	-@erase "$(INTDIR)\attrpresent.obj"
	-@erase "$(INTDIR)\boxlocate.obj"
	-@erase "$(INTDIR)\boxmoves.obj"
	-@erase "$(INTDIR)\boxparams.obj"
	-@erase "$(INTDIR)\boxpositions.obj"
	-@erase "$(INTDIR)\boxrelations.obj"
	-@erase "$(INTDIR)\boxselection.obj"
	-@erase "$(INTDIR)\browser.obj"
	-@erase "$(INTDIR)\buildboxes.obj"
	-@erase "$(INTDIR)\buildlines.obj"
	-@erase "$(INTDIR)\callback.obj"
	-@erase "$(INTDIR)\callbackinit.obj"
	-@erase "$(INTDIR)\changeabsbox.obj"
	-@erase "$(INTDIR)\changepresent.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\checkermenu.obj"
	-@erase "$(INTDIR)\closedoc.obj"
	-@erase "$(INTDIR)\colors.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\content.obj"
	-@erase "$(INTDIR)\contentapi.obj"
	-@erase "$(INTDIR)\context.obj"
	-@erase "$(INTDIR)\createabsbox.obj"
	-@erase "$(INTDIR)\createdoc.obj"
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\creationmenu.obj"
	-@erase "$(INTDIR)\dialogapi.obj"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displayselect.obj"
	-@erase "$(INTDIR)\displayview.obj"
	-@erase "$(INTDIR)\docs.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\draw.obj"
	-@erase "$(INTDIR)\editcommands.obj"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\extprintmenu.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\geom.obj"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\hyphen.obj"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\keyboards.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\lookup.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\modiftype.obj"
	-@erase "$(INTDIR)\opendoc.obj"
	-@erase "$(INTDIR)\openview.obj"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\picture.obj"
	-@erase "$(INTDIR)\pictureapi.obj"
	-@erase "$(INTDIR)\picturebase.obj"
	-@erase "$(INTDIR)\picturemenu.obj"
	-@erase "$(INTDIR)\pivot.obj"
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\pnghandler.obj"
	-@erase "$(INTDIR)\presentationapi.obj"
	-@erase "$(INTDIR)\presentmenu.obj"
	-@erase "$(INTDIR)\presrules.obj"
	-@erase "$(INTDIR)\presvariables.obj"
	-@erase "$(INTDIR)\printmenu.obj"
	-@erase "$(INTDIR)\Profiles.obj"
	-@erase "$(INTDIR)\pschemaapi.obj"
	-@erase "$(INTDIR)\quit.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\readprs.obj"
	-@erase "$(INTDIR)\readstr.obj"
	-@erase "$(INTDIR)\readtra.obj"
	-@erase "$(INTDIR)\referenceapi.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\res.obj"
	-@erase "$(INTDIR)\resgen.obj"
	-@erase "$(INTDIR)\resmatch.obj"
	-@erase "$(INTDIR)\savedoc.obj"
	-@erase "$(INTDIR)\schemas.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\searchmenu.obj"
	-@erase "$(INTDIR)\searchmenustr.obj"
	-@erase "$(INTDIR)\searchref.obj"
	-@erase "$(INTDIR)\selectionapi.obj"
	-@erase "$(INTDIR)\spellchecker.obj"
	-@erase "$(INTDIR)\structchange.obj"
	-@erase "$(INTDIR)\structcommands.obj"
	-@erase "$(INTDIR)\structcreation.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structlocate.obj"
	-@erase "$(INTDIR)\structmodif.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\structselect.obj"
	-@erase "$(INTDIR)\style.obj"
	-@erase "$(INTDIR)\textcommands.obj"
	-@erase "$(INTDIR)\thotmsg.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\Uio.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undoapi.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\unstructchange.obj"
	-@erase "$(INTDIR)\unstructlocate.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\viewapi.obj"
	-@erase "$(INTDIR)\viewcommands.obj"
	-@erase "$(INTDIR)\views.obj"
	-@erase "$(INTDIR)\visibility.obj"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(INTDIR)\writedoc.obj"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(INTDIR)\zoom.obj"
	-@erase "$(OUTDIR)\libThotEditor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /D "NDEBUG" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /Fp"$(INTDIR)\libThotEditor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libThotEditor.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libThotEditor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\alloca.obj" \
	"$(INTDIR)\appdialogue.obj" \
	"$(INTDIR)\appli.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributeapi.obj" \
	"$(INTDIR)\attributes.obj" \
	"$(INTDIR)\attrmenu.obj" \
	"$(INTDIR)\attrpresent.obj" \
	"$(INTDIR)\boxlocate.obj" \
	"$(INTDIR)\boxmoves.obj" \
	"$(INTDIR)\boxparams.obj" \
	"$(INTDIR)\boxpositions.obj" \
	"$(INTDIR)\boxrelations.obj" \
	"$(INTDIR)\boxselection.obj" \
	"$(INTDIR)\browser.obj" \
	"$(INTDIR)\buildboxes.obj" \
	"$(INTDIR)\buildlines.obj" \
	"$(INTDIR)\callback.obj" \
	"$(INTDIR)\callbackinit.obj" \
	"$(INTDIR)\changeabsbox.obj" \
	"$(INTDIR)\changepresent.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\checkermenu.obj" \
	"$(INTDIR)\closedoc.obj" \
	"$(INTDIR)\colors.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\content.obj" \
	"$(INTDIR)\contentapi.obj" \
	"$(INTDIR)\context.obj" \
	"$(INTDIR)\createabsbox.obj" \
	"$(INTDIR)\createdoc.obj" \
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\creationmenu.obj" \
	"$(INTDIR)\dialogapi.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayselect.obj" \
	"$(INTDIR)\displayview.obj" \
	"$(INTDIR)\docs.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\draw.obj" \
	"$(INTDIR)\editcommands.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\extprintmenu.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\geom.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\keyboards.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\lookup.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\modiftype.obj" \
	"$(INTDIR)\opendoc.obj" \
	"$(INTDIR)\openview.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\picture.obj" \
	"$(INTDIR)\pictureapi.obj" \
	"$(INTDIR)\picturebase.obj" \
	"$(INTDIR)\picturemenu.obj" \
	"$(INTDIR)\pivot.obj" \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\pnghandler.obj" \
	"$(INTDIR)\presentationapi.obj" \
	"$(INTDIR)\presentmenu.obj" \
	"$(INTDIR)\presrules.obj" \
	"$(INTDIR)\presvariables.obj" \
	"$(INTDIR)\printmenu.obj" \
	"$(INTDIR)\Profiles.obj" \
	"$(INTDIR)\pschemaapi.obj" \
	"$(INTDIR)\quit.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\readprs.obj" \
	"$(INTDIR)\readstr.obj" \
	"$(INTDIR)\readtra.obj" \
	"$(INTDIR)\referenceapi.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\res.obj" \
	"$(INTDIR)\resgen.obj" \
	"$(INTDIR)\resmatch.obj" \
	"$(INTDIR)\savedoc.obj" \
	"$(INTDIR)\schemas.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\searchmenu.obj" \
	"$(INTDIR)\searchmenustr.obj" \
	"$(INTDIR)\searchref.obj" \
	"$(INTDIR)\selectionapi.obj" \
	"$(INTDIR)\spellchecker.obj" \
	"$(INTDIR)\structchange.obj" \
	"$(INTDIR)\structcommands.obj" \
	"$(INTDIR)\structcreation.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structlocate.obj" \
	"$(INTDIR)\structmodif.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\structselect.obj" \
	"$(INTDIR)\style.obj" \
	"$(INTDIR)\textcommands.obj" \
	"$(INTDIR)\thotmsg.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\Uio.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\undoapi.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\unstructchange.obj" \
	"$(INTDIR)\unstructlocate.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\viewapi.obj" \
	"$(INTDIR)\viewcommands.obj" \
	"$(INTDIR)\views.obj" \
	"$(INTDIR)\visibility.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\writedoc.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xpmhandler.obj" \
	"$(INTDIR)\zoom.obj" \
	"$(OUTDIR)\libjpeg.lib" \
	"$(OUTDIR)\libpng.lib"

"$(OUTDIR)\libThotEditor.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libThotEditor.lib"

!ELSE 

ALL : "libpng - Win32 Debug" "libjpeg - Win32 Debug" "$(OUTDIR)\libThotEditor.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libjpeg - Win32 DebugCLEAN" "libpng - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\absboxes.obj"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\alloca.obj"
	-@erase "$(INTDIR)\appdialogue.obj"
	-@erase "$(INTDIR)\appli.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributeapi.obj"
	-@erase "$(INTDIR)\attributes.obj"
	-@erase "$(INTDIR)\attrmenu.obj"
	-@erase "$(INTDIR)\attrpresent.obj"
	-@erase "$(INTDIR)\boxlocate.obj"
	-@erase "$(INTDIR)\boxmoves.obj"
	-@erase "$(INTDIR)\boxparams.obj"
	-@erase "$(INTDIR)\boxpositions.obj"
	-@erase "$(INTDIR)\boxrelations.obj"
	-@erase "$(INTDIR)\boxselection.obj"
	-@erase "$(INTDIR)\browser.obj"
	-@erase "$(INTDIR)\buildboxes.obj"
	-@erase "$(INTDIR)\buildlines.obj"
	-@erase "$(INTDIR)\callback.obj"
	-@erase "$(INTDIR)\callbackinit.obj"
	-@erase "$(INTDIR)\changeabsbox.obj"
	-@erase "$(INTDIR)\changepresent.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\checkermenu.obj"
	-@erase "$(INTDIR)\closedoc.obj"
	-@erase "$(INTDIR)\colors.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\content.obj"
	-@erase "$(INTDIR)\contentapi.obj"
	-@erase "$(INTDIR)\context.obj"
	-@erase "$(INTDIR)\createabsbox.obj"
	-@erase "$(INTDIR)\createdoc.obj"
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\creationmenu.obj"
	-@erase "$(INTDIR)\dialogapi.obj"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displayselect.obj"
	-@erase "$(INTDIR)\displayview.obj"
	-@erase "$(INTDIR)\docs.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\draw.obj"
	-@erase "$(INTDIR)\editcommands.obj"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\extprintmenu.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\geom.obj"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\hyphen.obj"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\keyboards.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\lookup.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\modiftype.obj"
	-@erase "$(INTDIR)\opendoc.obj"
	-@erase "$(INTDIR)\openview.obj"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\picture.obj"
	-@erase "$(INTDIR)\pictureapi.obj"
	-@erase "$(INTDIR)\picturebase.obj"
	-@erase "$(INTDIR)\picturemenu.obj"
	-@erase "$(INTDIR)\pivot.obj"
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\pnghandler.obj"
	-@erase "$(INTDIR)\presentationapi.obj"
	-@erase "$(INTDIR)\presentmenu.obj"
	-@erase "$(INTDIR)\presrules.obj"
	-@erase "$(INTDIR)\presvariables.obj"
	-@erase "$(INTDIR)\printmenu.obj"
	-@erase "$(INTDIR)\Profiles.obj"
	-@erase "$(INTDIR)\pschemaapi.obj"
	-@erase "$(INTDIR)\quit.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\readprs.obj"
	-@erase "$(INTDIR)\readstr.obj"
	-@erase "$(INTDIR)\readtra.obj"
	-@erase "$(INTDIR)\referenceapi.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\res.obj"
	-@erase "$(INTDIR)\resgen.obj"
	-@erase "$(INTDIR)\resmatch.obj"
	-@erase "$(INTDIR)\savedoc.obj"
	-@erase "$(INTDIR)\schemas.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\searchmenu.obj"
	-@erase "$(INTDIR)\searchmenustr.obj"
	-@erase "$(INTDIR)\searchref.obj"
	-@erase "$(INTDIR)\selectionapi.obj"
	-@erase "$(INTDIR)\spellchecker.obj"
	-@erase "$(INTDIR)\structchange.obj"
	-@erase "$(INTDIR)\structcommands.obj"
	-@erase "$(INTDIR)\structcreation.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structlocate.obj"
	-@erase "$(INTDIR)\structmodif.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\structselect.obj"
	-@erase "$(INTDIR)\style.obj"
	-@erase "$(INTDIR)\textcommands.obj"
	-@erase "$(INTDIR)\thotmsg.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\uaccess.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\Uio.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undoapi.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\unstructchange.obj"
	-@erase "$(INTDIR)\unstructlocate.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\viewapi.obj"
	-@erase "$(INTDIR)\viewcommands.obj"
	-@erase "$(INTDIR)\views.obj"
	-@erase "$(INTDIR)\visibility.obj"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(INTDIR)\writedoc.obj"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(INTDIR)\zoom.obj"
	-@erase "$(OUTDIR)\libThotEditor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /D "_DEBUG" /D "_AMAYA_RELEASE_" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "WIN32" /D "_WINDOWS" /D "__STDC__" /D "STDC_HEADERS" /D "SOCKS" /D "MATHML" /D "THOT_TOOLTIPS" /Fp"$(INTDIR)\libThotEditor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libThotEditor.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libThotEditor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\alloca.obj" \
	"$(INTDIR)\appdialogue.obj" \
	"$(INTDIR)\appli.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributeapi.obj" \
	"$(INTDIR)\attributes.obj" \
	"$(INTDIR)\attrmenu.obj" \
	"$(INTDIR)\attrpresent.obj" \
	"$(INTDIR)\boxlocate.obj" \
	"$(INTDIR)\boxmoves.obj" \
	"$(INTDIR)\boxparams.obj" \
	"$(INTDIR)\boxpositions.obj" \
	"$(INTDIR)\boxrelations.obj" \
	"$(INTDIR)\boxselection.obj" \
	"$(INTDIR)\browser.obj" \
	"$(INTDIR)\buildboxes.obj" \
	"$(INTDIR)\buildlines.obj" \
	"$(INTDIR)\callback.obj" \
	"$(INTDIR)\callbackinit.obj" \
	"$(INTDIR)\changeabsbox.obj" \
	"$(INTDIR)\changepresent.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\checkermenu.obj" \
	"$(INTDIR)\closedoc.obj" \
	"$(INTDIR)\colors.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\content.obj" \
	"$(INTDIR)\contentapi.obj" \
	"$(INTDIR)\context.obj" \
	"$(INTDIR)\createabsbox.obj" \
	"$(INTDIR)\createdoc.obj" \
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\creationmenu.obj" \
	"$(INTDIR)\dialogapi.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayselect.obj" \
	"$(INTDIR)\displayview.obj" \
	"$(INTDIR)\docs.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\draw.obj" \
	"$(INTDIR)\editcommands.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\extprintmenu.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\geom.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\keyboards.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\lookup.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\modiftype.obj" \
	"$(INTDIR)\opendoc.obj" \
	"$(INTDIR)\openview.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\picture.obj" \
	"$(INTDIR)\pictureapi.obj" \
	"$(INTDIR)\picturebase.obj" \
	"$(INTDIR)\picturemenu.obj" \
	"$(INTDIR)\pivot.obj" \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\pnghandler.obj" \
	"$(INTDIR)\presentationapi.obj" \
	"$(INTDIR)\presentmenu.obj" \
	"$(INTDIR)\presrules.obj" \
	"$(INTDIR)\presvariables.obj" \
	"$(INTDIR)\printmenu.obj" \
	"$(INTDIR)\Profiles.obj" \
	"$(INTDIR)\pschemaapi.obj" \
	"$(INTDIR)\quit.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\readprs.obj" \
	"$(INTDIR)\readstr.obj" \
	"$(INTDIR)\readtra.obj" \
	"$(INTDIR)\referenceapi.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\res.obj" \
	"$(INTDIR)\resgen.obj" \
	"$(INTDIR)\resmatch.obj" \
	"$(INTDIR)\savedoc.obj" \
	"$(INTDIR)\schemas.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\searchmenu.obj" \
	"$(INTDIR)\searchmenustr.obj" \
	"$(INTDIR)\searchref.obj" \
	"$(INTDIR)\selectionapi.obj" \
	"$(INTDIR)\spellchecker.obj" \
	"$(INTDIR)\structchange.obj" \
	"$(INTDIR)\structcommands.obj" \
	"$(INTDIR)\structcreation.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structlocate.obj" \
	"$(INTDIR)\structmodif.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\structselect.obj" \
	"$(INTDIR)\style.obj" \
	"$(INTDIR)\textcommands.obj" \
	"$(INTDIR)\thotmsg.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uaccess.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\Uio.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\undoapi.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\unstructchange.obj" \
	"$(INTDIR)\unstructlocate.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\viewapi.obj" \
	"$(INTDIR)\viewcommands.obj" \
	"$(INTDIR)\views.obj" \
	"$(INTDIR)\visibility.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\writedoc.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xpmhandler.obj" \
	"$(INTDIR)\zoom.obj" \
	"$(OUTDIR)\libjpeg.lib" \
	"$(OUTDIR)\libpng.lib"

"$(OUTDIR)\libThotEditor.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libThotEditor.dep")
!INCLUDE "libThotEditor.dep"
!ELSE 
!MESSAGE Warning: cannot find "libThotEditor.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libThotEditor - Win32 Release" || "$(CFG)" == "libThotEditor - Win32 Debug"

!IF  "$(CFG)" == "libThotEditor - Win32 Release"

"libjpeg - Win32 Release" : 
   cd "\users\guetari\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Release" 
   cd "..\LibThotEditor"

"libjpeg - Win32 ReleaseCLEAN" : 
   cd "\users\guetari\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Release" RECURSE=1 CLEAN 
   cd "..\LibThotEditor"

!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"

"libjpeg - Win32 Debug" : 
   cd "\users\guetari\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Debug" 
   cd "..\LibThotEditor"

"libjpeg - Win32 DebugCLEAN" : 
   cd "\users\guetari\Amaya\Windows\libjpeg"
   $(MAKE) /$(MAKEFLAGS) /F .\libjpeg.mak CFG="libjpeg - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\LibThotEditor"

!ENDIF 

!IF  "$(CFG)" == "libThotEditor - Win32 Release"

"libpng - Win32 Release" : 
   cd "\users\guetari\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Release" 
   cd "..\LibThotEditor"

"libpng - Win32 ReleaseCLEAN" : 
   cd "\users\guetari\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Release" RECURSE=1 CLEAN 
   cd "..\LibThotEditor"

!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"

"libpng - Win32 Debug" : 
   cd "\users\guetari\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Debug" 
   cd "..\LibThotEditor"

"libpng - Win32 DebugCLEAN" : 
   cd "\users\guetari\Amaya\Windows\libpng"
   $(MAKE) /$(MAKEFLAGS) /F .\libpng.mak CFG="libpng - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\LibThotEditor"

!ENDIF 

SOURCE=..\..\thotlib\view\absboxes.c

"$(INTDIR)\absboxes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\abspictures.c

"$(INTDIR)\abspictures.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\actions.c

"$(INTDIR)\actions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\alloca.c

"$(INTDIR)\alloca.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\appdialogue.c

"$(INTDIR)\appdialogue.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\appli.c

"$(INTDIR)\appli.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\applicationapi.c

"$(INTDIR)\applicationapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\attributeapi.c

"$(INTDIR)\attributeapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\attributes.c

"$(INTDIR)\attributes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\attrmenu.c

"$(INTDIR)\attrmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\attrpresent.c

"$(INTDIR)\attrpresent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxlocate.c

"$(INTDIR)\boxlocate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxmoves.c

"$(INTDIR)\boxmoves.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxparams.c

"$(INTDIR)\boxparams.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxpositions.c

"$(INTDIR)\boxpositions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxrelations.c

"$(INTDIR)\boxrelations.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxselection.c

"$(INTDIR)\boxselection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\browser.c

"$(INTDIR)\browser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\buildboxes.c

"$(INTDIR)\buildboxes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\buildlines.c

"$(INTDIR)\buildlines.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\callback.c

"$(INTDIR)\callback.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\callbackinit.c

"$(INTDIR)\callbackinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\changeabsbox.c

"$(INTDIR)\changeabsbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\changepresent.c

"$(INTDIR)\changepresent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\checkaccess.c

"$(INTDIR)\checkaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\checkermenu.c

"$(INTDIR)\checkermenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\closedoc.c

"$(INTDIR)\closedoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\colors.c

"$(INTDIR)\colors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\config.c

"$(INTDIR)\config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\content.c

"$(INTDIR)\content.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\contentapi.c

"$(INTDIR)\contentapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\context.c

"$(INTDIR)\context.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\createabsbox.c

"$(INTDIR)\createabsbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\createdoc.c

"$(INTDIR)\createdoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\createpages.c

"$(INTDIR)\createpages.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\creationmenu.c

"$(INTDIR)\creationmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\dialogapi.c

"$(INTDIR)\dialogapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\dictionary.c

"$(INTDIR)\dictionary.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\displaybox.c

"$(INTDIR)\displaybox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\displayselect.c

"$(INTDIR)\displayselect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\View\displayview.c

"$(INTDIR)\displayview.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\docs.c

"$(INTDIR)\docs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\documentapi.c

"$(INTDIR)\documentapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\draw.c

"$(INTDIR)\draw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\editcommands.c

"$(INTDIR)\editcommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\epshandler.c

"$(INTDIR)\epshandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\exceptions.c

"$(INTDIR)\exceptions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\externalref.c

"$(INTDIR)\externalref.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\extprintmenu.c

"$(INTDIR)\extprintmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\fileaccess.c

"$(INTDIR)\fileaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\font.c

"$(INTDIR)\font.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\frame.c

"$(INTDIR)\frame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\geom.c

"$(INTDIR)\geom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\gifhandler.c

"$(INTDIR)\gifhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\hyphen.c

"$(INTDIR)\hyphen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\inites.c

"$(INTDIR)\inites.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\input.c

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\interface.c

"$(INTDIR)\interface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\jpeghandler.c

"$(INTDIR)\jpeghandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\keyboards.c

"$(INTDIR)\keyboards.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\labelalloc.c

"$(INTDIR)\labelalloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\language.c

"$(INTDIR)\language.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\lookup.c

"$(INTDIR)\lookup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\memory.c

"$(INTDIR)\memory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\message.c

"$(INTDIR)\message.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\modiftype.c

"$(INTDIR)\modiftype.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\opendoc.c

"$(INTDIR)\opendoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\openview.c

"$(INTDIR)\openview.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\pagecommands.c

"$(INTDIR)\pagecommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\paginate.c

"$(INTDIR)\paginate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\picture.c

"$(INTDIR)\picture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\pictureapi.c

"$(INTDIR)\pictureapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\picturebase.c

"$(INTDIR)\picturebase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\picturemenu.c

"$(INTDIR)\picturemenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\pivot.c

"$(INTDIR)\pivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\platform.c

"$(INTDIR)\platform.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\pnghandler.c

"$(INTDIR)\pnghandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\presentationapi.c

"$(INTDIR)\presentationapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\presentmenu.c

"$(INTDIR)\presentmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\presrules.c

"$(INTDIR)\presrules.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\presvariables.c

"$(INTDIR)\presvariables.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\printmenu.c

"$(INTDIR)\printmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\Dialogue\Profiles.c

"$(INTDIR)\Profiles.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\pschemaapi.c

"$(INTDIR)\pschemaapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\quit.c

"$(INTDIR)\quit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readpivot.c

"$(INTDIR)\readpivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readprs.c

"$(INTDIR)\readprs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readstr.c

"$(INTDIR)\readstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readtra.c

"$(INTDIR)\readtra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\referenceapi.c

"$(INTDIR)\referenceapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\references.c

"$(INTDIR)\references.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\registry.c

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\res.c

"$(INTDIR)\res.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\resgen.c

"$(INTDIR)\resgen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\resmatch.c

"$(INTDIR)\resmatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\savedoc.c

"$(INTDIR)\savedoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\schemas.c

"$(INTDIR)\schemas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\schtrad.c

"$(INTDIR)\schtrad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\scroll.c

"$(INTDIR)\scroll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\search.c

"$(INTDIR)\search.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\searchmenu.c

"$(INTDIR)\searchmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\searchmenustr.c

"$(INTDIR)\searchmenustr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\searchref.c

"$(INTDIR)\searchref.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\selectionapi.c

"$(INTDIR)\selectionapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\spellchecker.c

"$(INTDIR)\spellchecker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\structchange.c

"$(INTDIR)\structchange.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\structcommands.c

"$(INTDIR)\structcommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\structcreation.c

"$(INTDIR)\structcreation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\structlist.c

"$(INTDIR)\structlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\structlocate.c

"$(INTDIR)\structlocate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\structmodif.c

"$(INTDIR)\structmodif.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\structschema.c

"$(INTDIR)\structschema.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\structselect.c

"$(INTDIR)\structselect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\style.c

"$(INTDIR)\style.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\textcommands.c

"$(INTDIR)\textcommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\thotmsg.c

"$(INTDIR)\thotmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\translation.c

"$(INTDIR)\translation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\tree.c

"$(INTDIR)\tree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\treeapi.c

"$(INTDIR)\treeapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\uaccess.c

"$(INTDIR)\uaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\uconvert.c

"$(INTDIR)\uconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\Unicode\Uio.c

"$(INTDIR)\Uio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\undo.c

"$(INTDIR)\undo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\undoapi.c

"$(INTDIR)\undoapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\units.c

"$(INTDIR)\units.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\unstructchange.c

"$(INTDIR)\unstructchange.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\unstructlocate.c

"$(INTDIR)\unstructlocate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\ustring.c

"$(INTDIR)\ustring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\viewapi.c

"$(INTDIR)\viewapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\editing\viewcommands.c

"$(INTDIR)\viewcommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\views.c

"$(INTDIR)\views.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\visibility.c

"$(INTDIR)\visibility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\windowdisplay.c

"$(INTDIR)\windowdisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\word.c

"$(INTDIR)\word.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\writedoc.c

"$(INTDIR)\writedoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\writepivot.c

"$(INTDIR)\writepivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\xbmhandler.c

"$(INTDIR)\xbmhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\xpmhandler.c

"$(INTDIR)\xpmhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\zoom.c

"$(INTDIR)\zoom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

