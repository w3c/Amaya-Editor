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

!IF  "$(CFG)" == "libThotEditor - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

ALL : "$(OUTDIR)\libThotEditor.lib"


CLEAN :
	-@erase "$(INTDIR)\absboxes.obj"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\animbox.obj"
	-@erase "$(INTDIR)\appdialogue.obj"
	-@erase "$(INTDIR)\appdialogue_wx.obj"
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
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\creationmenu.obj"
	-@erase "$(INTDIR)\dialogapi.obj"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displayselect.obj"
	-@erase "$(INTDIR)\displayview.obj"
	-@erase "$(INTDIR)\docs.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\editcommands.obj"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\fontconfig.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\geom.obj"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\hyphen.obj"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\keyboards.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\modiftype.obj"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\picture.obj"
	-@erase "$(INTDIR)\pictureapi.obj"
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
	-@erase "$(INTDIR)\schemas.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\searchmenu.obj"
	-@erase "$(INTDIR)\searchref.obj"
	-@erase "$(INTDIR)\selectionapi.obj"
	-@erase "$(INTDIR)\spellchecker.obj"
	-@erase "$(INTDIR)\spline.obj"
	-@erase "$(INTDIR)\stix.obj"
	-@erase "$(INTDIR)\structcommands.obj"
	-@erase "$(INTDIR)\structcreation.obj"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structmodif.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\structselect.obj"
	-@erase "$(INTDIR)\style.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\textcommands.obj"
	-@erase "$(INTDIR)\thotmsg.obj"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undoapi.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\unstructchange.obj"
	-@erase "$(INTDIR)\unstructlocate.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\viewapi.obj"
	-@erase "$(INTDIR)\views.obj"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(INTDIR)\writedoc.obj"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(OUTDIR)\libThotEditor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /I "..\..\amaya" /D "NDEBUG" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "__STDC__" /D "STDC_HEADERS" /D "SOCKSTHOT_TOOLTIPS" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /Fp"$(INTDIR)\libThotEditor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libThotEditor.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libThotEditor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\animbox.obj" \
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
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\creationmenu.obj" \
	"$(INTDIR)\dialogapi.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayselect.obj" \
	"$(INTDIR)\displayview.obj" \
	"$(INTDIR)\docs.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\editcommands.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\fontconfig.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\geom.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\keyboards.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\modiftype.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\picture.obj" \
	"$(INTDIR)\pictureapi.obj" \
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
	"$(INTDIR)\schemas.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\searchmenu.obj" \
	"$(INTDIR)\searchref.obj" \
	"$(INTDIR)\selectionapi.obj" \
	"$(INTDIR)\spellchecker.obj" \
	"$(INTDIR)\spline.obj" \
	"$(INTDIR)\stix.obj" \
	"$(INTDIR)\structcommands.obj" \
	"$(INTDIR)\structcreation.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structmodif.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\structselect.obj" \
	"$(INTDIR)\style.obj" \
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\textcommands.obj" \
	"$(INTDIR)\thotmsg.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\undoapi.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\unstructchange.obj" \
	"$(INTDIR)\unstructlocate.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\viewapi.obj" \
	"$(INTDIR)\views.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\writedoc.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xpmhandler.obj" \
	"$(INTDIR)\appdialogue_wx.obj"

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

ALL : "$(OUTDIR)\libThotEditor.lib" "$(OUTDIR)\libThotEditor.bsc"


CLEAN :
	-@erase "$(INTDIR)\absboxes.obj"
	-@erase "$(INTDIR)\absboxes.sbr"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\abspictures.sbr"
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\actions.sbr"
	-@erase "$(INTDIR)\animbox.obj"
	-@erase "$(INTDIR)\animbox.sbr"
	-@erase "$(INTDIR)\appdialogue.obj"
	-@erase "$(INTDIR)\appdialogue.sbr"
	-@erase "$(INTDIR)\appdialogue_wx.obj"
	-@erase "$(INTDIR)\appdialogue_wx.sbr"
	-@erase "$(INTDIR)\appli.obj"
	-@erase "$(INTDIR)\appli.sbr"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\applicationapi.sbr"
	-@erase "$(INTDIR)\attributeapi.obj"
	-@erase "$(INTDIR)\attributeapi.sbr"
	-@erase "$(INTDIR)\attributes.obj"
	-@erase "$(INTDIR)\attributes.sbr"
	-@erase "$(INTDIR)\attrmenu.obj"
	-@erase "$(INTDIR)\attrmenu.sbr"
	-@erase "$(INTDIR)\attrpresent.obj"
	-@erase "$(INTDIR)\attrpresent.sbr"
	-@erase "$(INTDIR)\boxlocate.obj"
	-@erase "$(INTDIR)\boxlocate.sbr"
	-@erase "$(INTDIR)\boxmoves.obj"
	-@erase "$(INTDIR)\boxmoves.sbr"
	-@erase "$(INTDIR)\boxparams.obj"
	-@erase "$(INTDIR)\boxparams.sbr"
	-@erase "$(INTDIR)\boxpositions.obj"
	-@erase "$(INTDIR)\boxpositions.sbr"
	-@erase "$(INTDIR)\boxrelations.obj"
	-@erase "$(INTDIR)\boxrelations.sbr"
	-@erase "$(INTDIR)\boxselection.obj"
	-@erase "$(INTDIR)\boxselection.sbr"
	-@erase "$(INTDIR)\browser.obj"
	-@erase "$(INTDIR)\browser.sbr"
	-@erase "$(INTDIR)\buildboxes.obj"
	-@erase "$(INTDIR)\buildboxes.sbr"
	-@erase "$(INTDIR)\buildlines.obj"
	-@erase "$(INTDIR)\buildlines.sbr"
	-@erase "$(INTDIR)\callback.obj"
	-@erase "$(INTDIR)\callback.sbr"
	-@erase "$(INTDIR)\callbackinit.obj"
	-@erase "$(INTDIR)\callbackinit.sbr"
	-@erase "$(INTDIR)\changeabsbox.obj"
	-@erase "$(INTDIR)\changeabsbox.sbr"
	-@erase "$(INTDIR)\changepresent.obj"
	-@erase "$(INTDIR)\changepresent.sbr"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\checkaccess.sbr"
	-@erase "$(INTDIR)\checkermenu.obj"
	-@erase "$(INTDIR)\checkermenu.sbr"
	-@erase "$(INTDIR)\closedoc.obj"
	-@erase "$(INTDIR)\closedoc.sbr"
	-@erase "$(INTDIR)\colors.obj"
	-@erase "$(INTDIR)\colors.sbr"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\config.sbr"
	-@erase "$(INTDIR)\content.obj"
	-@erase "$(INTDIR)\content.sbr"
	-@erase "$(INTDIR)\contentapi.obj"
	-@erase "$(INTDIR)\contentapi.sbr"
	-@erase "$(INTDIR)\context.obj"
	-@erase "$(INTDIR)\context.sbr"
	-@erase "$(INTDIR)\createabsbox.obj"
	-@erase "$(INTDIR)\createabsbox.sbr"
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\createpages.sbr"
	-@erase "$(INTDIR)\creationmenu.obj"
	-@erase "$(INTDIR)\creationmenu.sbr"
	-@erase "$(INTDIR)\dialogapi.obj"
	-@erase "$(INTDIR)\dialogapi.sbr"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\dictionary.sbr"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displaybox.sbr"
	-@erase "$(INTDIR)\displayselect.obj"
	-@erase "$(INTDIR)\displayselect.sbr"
	-@erase "$(INTDIR)\displayview.obj"
	-@erase "$(INTDIR)\displayview.sbr"
	-@erase "$(INTDIR)\docs.obj"
	-@erase "$(INTDIR)\docs.sbr"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\documentapi.sbr"
	-@erase "$(INTDIR)\editcommands.obj"
	-@erase "$(INTDIR)\editcommands.sbr"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\epshandler.sbr"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\exceptions.sbr"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\externalref.sbr"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\fileaccess.sbr"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\font.sbr"
	-@erase "$(INTDIR)\fontconfig.obj"
	-@erase "$(INTDIR)\fontconfig.sbr"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\frame.sbr"
	-@erase "$(INTDIR)\geom.obj"
	-@erase "$(INTDIR)\geom.sbr"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\gifhandler.sbr"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\global.sbr"
	-@erase "$(INTDIR)\hyphen.obj"
	-@erase "$(INTDIR)\hyphen.sbr"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\inites.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\jpeghandler.sbr"
	-@erase "$(INTDIR)\keyboards.obj"
	-@erase "$(INTDIR)\keyboards.sbr"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\labelalloc.sbr"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\language.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\modiftype.obj"
	-@erase "$(INTDIR)\modiftype.sbr"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\pagecommands.sbr"
	-@erase "$(INTDIR)\paginate.obj"
	-@erase "$(INTDIR)\paginate.sbr"
	-@erase "$(INTDIR)\picture.obj"
	-@erase "$(INTDIR)\picture.sbr"
	-@erase "$(INTDIR)\pictureapi.obj"
	-@erase "$(INTDIR)\pictureapi.sbr"
	-@erase "$(INTDIR)\pivot.obj"
	-@erase "$(INTDIR)\pivot.sbr"
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\platform.sbr"
	-@erase "$(INTDIR)\pnghandler.obj"
	-@erase "$(INTDIR)\pnghandler.sbr"
	-@erase "$(INTDIR)\presentationapi.obj"
	-@erase "$(INTDIR)\presentationapi.sbr"
	-@erase "$(INTDIR)\presentmenu.obj"
	-@erase "$(INTDIR)\presentmenu.sbr"
	-@erase "$(INTDIR)\presrules.obj"
	-@erase "$(INTDIR)\presrules.sbr"
	-@erase "$(INTDIR)\presvariables.obj"
	-@erase "$(INTDIR)\presvariables.sbr"
	-@erase "$(INTDIR)\printmenu.obj"
	-@erase "$(INTDIR)\printmenu.sbr"
	-@erase "$(INTDIR)\Profiles.obj"
	-@erase "$(INTDIR)\Profiles.sbr"
	-@erase "$(INTDIR)\pschemaapi.obj"
	-@erase "$(INTDIR)\pschemaapi.sbr"
	-@erase "$(INTDIR)\quit.obj"
	-@erase "$(INTDIR)\quit.sbr"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\readpivot.sbr"
	-@erase "$(INTDIR)\readprs.obj"
	-@erase "$(INTDIR)\readprs.sbr"
	-@erase "$(INTDIR)\readstr.obj"
	-@erase "$(INTDIR)\readstr.sbr"
	-@erase "$(INTDIR)\readtra.obj"
	-@erase "$(INTDIR)\readtra.sbr"
	-@erase "$(INTDIR)\referenceapi.obj"
	-@erase "$(INTDIR)\referenceapi.sbr"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\references.sbr"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.sbr"
	-@erase "$(INTDIR)\res.obj"
	-@erase "$(INTDIR)\res.sbr"
	-@erase "$(INTDIR)\resgen.obj"
	-@erase "$(INTDIR)\resgen.sbr"
	-@erase "$(INTDIR)\resmatch.obj"
	-@erase "$(INTDIR)\resmatch.sbr"
	-@erase "$(INTDIR)\schemas.obj"
	-@erase "$(INTDIR)\schemas.sbr"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\schtrad.sbr"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\scroll.sbr"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\search.sbr"
	-@erase "$(INTDIR)\searchmenu.obj"
	-@erase "$(INTDIR)\searchmenu.sbr"
	-@erase "$(INTDIR)\searchref.obj"
	-@erase "$(INTDIR)\searchref.sbr"
	-@erase "$(INTDIR)\selectionapi.obj"
	-@erase "$(INTDIR)\selectionapi.sbr"
	-@erase "$(INTDIR)\spellchecker.obj"
	-@erase "$(INTDIR)\spellchecker.sbr"
	-@erase "$(INTDIR)\spline.obj"
	-@erase "$(INTDIR)\spline.sbr"
	-@erase "$(INTDIR)\stix.obj"
	-@erase "$(INTDIR)\stix.sbr"
	-@erase "$(INTDIR)\structcommands.obj"
	-@erase "$(INTDIR)\structcommands.sbr"
	-@erase "$(INTDIR)\structcreation.obj"
	-@erase "$(INTDIR)\structcreation.sbr"
	-@erase "$(INTDIR)\structlist.obj"
	-@erase "$(INTDIR)\structlist.sbr"
	-@erase "$(INTDIR)\structmodif.obj"
	-@erase "$(INTDIR)\structmodif.sbr"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\structschema.sbr"
	-@erase "$(INTDIR)\structselect.obj"
	-@erase "$(INTDIR)\structselect.sbr"
	-@erase "$(INTDIR)\style.obj"
	-@erase "$(INTDIR)\style.sbr"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\tableH.sbr"
	-@erase "$(INTDIR)\textcommands.obj"
	-@erase "$(INTDIR)\textcommands.sbr"
	-@erase "$(INTDIR)\thotmsg.obj"
	-@erase "$(INTDIR)\thotmsg.sbr"
	-@erase "$(INTDIR)\translation.obj"
	-@erase "$(INTDIR)\translation.sbr"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\tree.sbr"
	-@erase "$(INTDIR)\treeapi.obj"
	-@erase "$(INTDIR)\treeapi.sbr"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\uconvert.sbr"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undo.sbr"
	-@erase "$(INTDIR)\undoapi.obj"
	-@erase "$(INTDIR)\undoapi.sbr"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\units.sbr"
	-@erase "$(INTDIR)\unstructchange.obj"
	-@erase "$(INTDIR)\unstructchange.sbr"
	-@erase "$(INTDIR)\unstructlocate.obj"
	-@erase "$(INTDIR)\unstructlocate.sbr"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\ustring.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\viewapi.obj"
	-@erase "$(INTDIR)\viewapi.sbr"
	-@erase "$(INTDIR)\views.obj"
	-@erase "$(INTDIR)\views.sbr"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\windowdisplay.sbr"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(INTDIR)\word.sbr"
	-@erase "$(INTDIR)\writedoc.obj"
	-@erase "$(INTDIR)\writedoc.sbr"
	-@erase "$(INTDIR)\writepivot.obj"
	-@erase "$(INTDIR)\writepivot.sbr"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xbmhandler.sbr"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(INTDIR)\xpmhandler.sbr"
	-@erase "$(OUTDIR)\libThotEditor.bsc"
	-@erase "$(OUTDIR)\libThotEditor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /I "..\..\amaya" /D "_FONTCONFIG" /D "_STIX" /D "_DEBUG" /D "_WINDOWS_DLL" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /D "__STDC__" /D "STDC_HEADERS" /D "SOCKSTHOT_TOOLTIPS" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libThotEditor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libThotEditor.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\absboxes.sbr" \
	"$(INTDIR)\abspictures.sbr" \
	"$(INTDIR)\actions.sbr" \
	"$(INTDIR)\animbox.sbr" \
	"$(INTDIR)\appdialogue.sbr" \
	"$(INTDIR)\appli.sbr" \
	"$(INTDIR)\applicationapi.sbr" \
	"$(INTDIR)\attributeapi.sbr" \
	"$(INTDIR)\attributes.sbr" \
	"$(INTDIR)\attrmenu.sbr" \
	"$(INTDIR)\attrpresent.sbr" \
	"$(INTDIR)\boxlocate.sbr" \
	"$(INTDIR)\boxmoves.sbr" \
	"$(INTDIR)\boxparams.sbr" \
	"$(INTDIR)\boxpositions.sbr" \
	"$(INTDIR)\boxrelations.sbr" \
	"$(INTDIR)\boxselection.sbr" \
	"$(INTDIR)\browser.sbr" \
	"$(INTDIR)\buildboxes.sbr" \
	"$(INTDIR)\buildlines.sbr" \
	"$(INTDIR)\callback.sbr" \
	"$(INTDIR)\callbackinit.sbr" \
	"$(INTDIR)\changeabsbox.sbr" \
	"$(INTDIR)\changepresent.sbr" \
	"$(INTDIR)\checkaccess.sbr" \
	"$(INTDIR)\checkermenu.sbr" \
	"$(INTDIR)\closedoc.sbr" \
	"$(INTDIR)\colors.sbr" \
	"$(INTDIR)\config.sbr" \
	"$(INTDIR)\content.sbr" \
	"$(INTDIR)\contentapi.sbr" \
	"$(INTDIR)\context.sbr" \
	"$(INTDIR)\createabsbox.sbr" \
	"$(INTDIR)\createpages.sbr" \
	"$(INTDIR)\creationmenu.sbr" \
	"$(INTDIR)\dialogapi.sbr" \
	"$(INTDIR)\dictionary.sbr" \
	"$(INTDIR)\displaybox.sbr" \
	"$(INTDIR)\displayselect.sbr" \
	"$(INTDIR)\displayview.sbr" \
	"$(INTDIR)\docs.sbr" \
	"$(INTDIR)\documentapi.sbr" \
	"$(INTDIR)\editcommands.sbr" \
	"$(INTDIR)\epshandler.sbr" \
	"$(INTDIR)\exceptions.sbr" \
	"$(INTDIR)\externalref.sbr" \
	"$(INTDIR)\fileaccess.sbr" \
	"$(INTDIR)\font.sbr" \
	"$(INTDIR)\fontconfig.sbr" \
	"$(INTDIR)\frame.sbr" \
	"$(INTDIR)\geom.sbr" \
	"$(INTDIR)\gifhandler.sbr" \
	"$(INTDIR)\global.sbr" \
	"$(INTDIR)\hyphen.sbr" \
	"$(INTDIR)\inites.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\jpeghandler.sbr" \
	"$(INTDIR)\keyboards.sbr" \
	"$(INTDIR)\labelalloc.sbr" \
	"$(INTDIR)\language.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\modiftype.sbr" \
	"$(INTDIR)\pagecommands.sbr" \
	"$(INTDIR)\paginate.sbr" \
	"$(INTDIR)\picture.sbr" \
	"$(INTDIR)\pictureapi.sbr" \
	"$(INTDIR)\pivot.sbr" \
	"$(INTDIR)\platform.sbr" \
	"$(INTDIR)\pnghandler.sbr" \
	"$(INTDIR)\presentationapi.sbr" \
	"$(INTDIR)\presentmenu.sbr" \
	"$(INTDIR)\presrules.sbr" \
	"$(INTDIR)\presvariables.sbr" \
	"$(INTDIR)\printmenu.sbr" \
	"$(INTDIR)\Profiles.sbr" \
	"$(INTDIR)\pschemaapi.sbr" \
	"$(INTDIR)\quit.sbr" \
	"$(INTDIR)\readpivot.sbr" \
	"$(INTDIR)\readprs.sbr" \
	"$(INTDIR)\readstr.sbr" \
	"$(INTDIR)\readtra.sbr" \
	"$(INTDIR)\referenceapi.sbr" \
	"$(INTDIR)\references.sbr" \
	"$(INTDIR)\registry.sbr" \
	"$(INTDIR)\res.sbr" \
	"$(INTDIR)\resgen.sbr" \
	"$(INTDIR)\resmatch.sbr" \
	"$(INTDIR)\schemas.sbr" \
	"$(INTDIR)\schtrad.sbr" \
	"$(INTDIR)\scroll.sbr" \
	"$(INTDIR)\search.sbr" \
	"$(INTDIR)\searchmenu.sbr" \
	"$(INTDIR)\searchref.sbr" \
	"$(INTDIR)\selectionapi.sbr" \
	"$(INTDIR)\spellchecker.sbr" \
	"$(INTDIR)\spline.sbr" \
	"$(INTDIR)\stix.sbr" \
	"$(INTDIR)\structcommands.sbr" \
	"$(INTDIR)\structcreation.sbr" \
	"$(INTDIR)\structlist.sbr" \
	"$(INTDIR)\structmodif.sbr" \
	"$(INTDIR)\structschema.sbr" \
	"$(INTDIR)\structselect.sbr" \
	"$(INTDIR)\style.sbr" \
	"$(INTDIR)\tableH.sbr" \
	"$(INTDIR)\textcommands.sbr" \
	"$(INTDIR)\thotmsg.sbr" \
	"$(INTDIR)\translation.sbr" \
	"$(INTDIR)\tree.sbr" \
	"$(INTDIR)\treeapi.sbr" \
	"$(INTDIR)\uconvert.sbr" \
	"$(INTDIR)\undo.sbr" \
	"$(INTDIR)\undoapi.sbr" \
	"$(INTDIR)\units.sbr" \
	"$(INTDIR)\unstructchange.sbr" \
	"$(INTDIR)\unstructlocate.sbr" \
	"$(INTDIR)\ustring.sbr" \
	"$(INTDIR)\viewapi.sbr" \
	"$(INTDIR)\views.sbr" \
	"$(INTDIR)\windowdisplay.sbr" \
	"$(INTDIR)\word.sbr" \
	"$(INTDIR)\writedoc.sbr" \
	"$(INTDIR)\writepivot.sbr" \
	"$(INTDIR)\xbmhandler.sbr" \
	"$(INTDIR)\xpmhandler.sbr" \
	"$(INTDIR)\appdialogue_wx.sbr"

"$(OUTDIR)\libThotEditor.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libThotEditor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\animbox.obj" \
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
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\creationmenu.obj" \
	"$(INTDIR)\dialogapi.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayselect.obj" \
	"$(INTDIR)\displayview.obj" \
	"$(INTDIR)\docs.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\editcommands.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\fontconfig.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\geom.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\keyboards.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\modiftype.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\paginate.obj" \
	"$(INTDIR)\picture.obj" \
	"$(INTDIR)\pictureapi.obj" \
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
	"$(INTDIR)\schemas.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\searchmenu.obj" \
	"$(INTDIR)\searchref.obj" \
	"$(INTDIR)\selectionapi.obj" \
	"$(INTDIR)\spellchecker.obj" \
	"$(INTDIR)\spline.obj" \
	"$(INTDIR)\stix.obj" \
	"$(INTDIR)\structcommands.obj" \
	"$(INTDIR)\structcreation.obj" \
	"$(INTDIR)\structlist.obj" \
	"$(INTDIR)\structmodif.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\structselect.obj" \
	"$(INTDIR)\style.obj" \
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\textcommands.obj" \
	"$(INTDIR)\thotmsg.obj" \
	"$(INTDIR)\translation.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\treeapi.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\undoapi.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\unstructchange.obj" \
	"$(INTDIR)\unstructlocate.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\viewapi.obj" \
	"$(INTDIR)\views.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\writedoc.obj" \
	"$(INTDIR)\writepivot.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xpmhandler.obj" \
	"$(INTDIR)\appdialogue_wx.obj"

"$(OUTDIR)\libThotEditor.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libThotEditor.dep")
!INCLUDE "libThotEditor.dep"
!ELSE 
!MESSAGE Warning: cannot find "libThotEditor.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libThotEditor - Win32 Release" || "$(CFG)" == "libThotEditor - Win32 Debug"
SOURCE=..\..\thotlib\view\absboxes.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\absboxes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\absboxes.obj"	"$(INTDIR)\absboxes.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\abspictures.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\abspictures.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\abspictures.obj"	"$(INTDIR)\abspictures.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\actions.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\actions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\actions.obj"	"$(INTDIR)\actions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\animbox.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\animbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\animbox.obj"	"$(INTDIR)\animbox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\appdialogue.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\appdialogue.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\appdialogue.obj"	"$(INTDIR)\appdialogue.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\appdialogue_wx.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\appdialogue_wx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\appdialogue_wx.obj"	"$(INTDIR)\appdialogue_wx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\appli.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\appli.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\appli.obj"	"$(INTDIR)\appli.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\applicationapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\applicationapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\applicationapi.obj"	"$(INTDIR)\applicationapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\attributeapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\attributeapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\attributeapi.obj"	"$(INTDIR)\attributeapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\attributes.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\attributes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\attributes.obj"	"$(INTDIR)\attributes.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\attrmenu.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\attrmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\attrmenu.obj"	"$(INTDIR)\attrmenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\attrpresent.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\attrpresent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\attrpresent.obj"	"$(INTDIR)\attrpresent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\boxlocate.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\boxlocate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\boxlocate.obj"	"$(INTDIR)\boxlocate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\boxmoves.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\boxmoves.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\boxmoves.obj"	"$(INTDIR)\boxmoves.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\boxparams.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\boxparams.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\boxparams.obj"	"$(INTDIR)\boxparams.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\boxpositions.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\boxpositions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\boxpositions.obj"	"$(INTDIR)\boxpositions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\boxrelations.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\boxrelations.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\boxrelations.obj"	"$(INTDIR)\boxrelations.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\boxselection.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\boxselection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\boxselection.obj"	"$(INTDIR)\boxselection.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\browser.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\browser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\browser.obj"	"$(INTDIR)\browser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\buildboxes.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\buildboxes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\buildboxes.obj"	"$(INTDIR)\buildboxes.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\buildlines.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\buildlines.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\buildlines.obj"	"$(INTDIR)\buildlines.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\callback.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\callback.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\callback.obj"	"$(INTDIR)\callback.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\callbackinit.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\callbackinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\callbackinit.obj"	"$(INTDIR)\callbackinit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\changeabsbox.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\changeabsbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\changeabsbox.obj"	"$(INTDIR)\changeabsbox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\changepresent.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\changepresent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\changepresent.obj"	"$(INTDIR)\changepresent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\checkaccess.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\checkaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\checkaccess.obj"	"$(INTDIR)\checkaccess.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\checkermenu.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\checkermenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\checkermenu.obj"	"$(INTDIR)\checkermenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\closedoc.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\closedoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\closedoc.obj"	"$(INTDIR)\closedoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\colors.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\colors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\colors.obj"	"$(INTDIR)\colors.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\config.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\config.obj"	"$(INTDIR)\config.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\content.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\content.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\content.obj"	"$(INTDIR)\content.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\contentapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\contentapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\contentapi.obj"	"$(INTDIR)\contentapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\context.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\context.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\context.obj"	"$(INTDIR)\context.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\createabsbox.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\createabsbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\createabsbox.obj"	"$(INTDIR)\createabsbox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\createpages.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\createpages.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\createpages.obj"	"$(INTDIR)\createpages.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\creationmenu.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\creationmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\creationmenu.obj"	"$(INTDIR)\creationmenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\dialogapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\dialogapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\dialogapi.obj"	"$(INTDIR)\dialogapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\dictionary.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\dictionary.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\dictionary.obj"	"$(INTDIR)\dictionary.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\displaybox.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\displaybox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\displaybox.obj"	"$(INTDIR)\displaybox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\displayselect.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\displayselect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\displayselect.obj"	"$(INTDIR)\displayselect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Thotlib\View\displayview.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\displayview.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\displayview.obj"	"$(INTDIR)\displayview.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\docs.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\docs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\docs.obj"	"$(INTDIR)\docs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\documentapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\documentapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\documentapi.obj"	"$(INTDIR)\documentapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\editcommands.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\editcommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\editcommands.obj"	"$(INTDIR)\editcommands.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\epshandler.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\epshandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\epshandler.obj"	"$(INTDIR)\epshandler.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\exceptions.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\exceptions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\exceptions.obj"	"$(INTDIR)\exceptions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\externalref.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\externalref.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\externalref.obj"	"$(INTDIR)\externalref.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\fileaccess.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\fileaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\fileaccess.obj"	"$(INTDIR)\fileaccess.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\font.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\font.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\font.obj"	"$(INTDIR)\font.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\fontconfig.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\fontconfig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\fontconfig.obj"	"$(INTDIR)\fontconfig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\frame.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\frame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\frame.obj"	"$(INTDIR)\frame.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\geom.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\geom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\geom.obj"	"$(INTDIR)\geom.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\gifhandler.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\gifhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\gifhandler.obj"	"$(INTDIR)\gifhandler.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\global.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\global.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\global.obj"	"$(INTDIR)\global.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\hyphen.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\hyphen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\hyphen.obj"	"$(INTDIR)\hyphen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\inites.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\inites.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\inites.obj"	"$(INTDIR)\inites.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\input.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\input.obj"	"$(INTDIR)\input.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\interface.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\interface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\interface.obj"	"$(INTDIR)\interface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\jpeghandler.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\jpeghandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\jpeghandler.obj"	"$(INTDIR)\jpeghandler.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\keyboards.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\keyboards.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\keyboards.obj"	"$(INTDIR)\keyboards.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\labelalloc.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\labelalloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\labelalloc.obj"	"$(INTDIR)\labelalloc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\language.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\language.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\language.obj"	"$(INTDIR)\language.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\memory.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\memory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\memory.obj"	"$(INTDIR)\memory.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\message.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\message.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\message.obj"	"$(INTDIR)\message.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\modiftype.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\modiftype.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\modiftype.obj"	"$(INTDIR)\modiftype.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\pagecommands.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\pagecommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\pagecommands.obj"	"$(INTDIR)\pagecommands.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\paginate.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\paginate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\paginate.obj"	"$(INTDIR)\paginate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\picture.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\picture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\picture.obj"	"$(INTDIR)\picture.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\pictureapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\pictureapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\pictureapi.obj"	"$(INTDIR)\pictureapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\pivot.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\pivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\pivot.obj"	"$(INTDIR)\pivot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\platform.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\platform.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\platform.obj"	"$(INTDIR)\platform.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\pnghandler.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\pnghandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\pnghandler.obj"	"$(INTDIR)\pnghandler.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\presentationapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\presentationapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\presentationapi.obj"	"$(INTDIR)\presentationapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\presentmenu.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\presentmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\presentmenu.obj"	"$(INTDIR)\presentmenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\presrules.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\presrules.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\presrules.obj"	"$(INTDIR)\presrules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\presvariables.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\presvariables.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\presvariables.obj"	"$(INTDIR)\presvariables.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\printmenu.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\printmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\printmenu.obj"	"$(INTDIR)\printmenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Thotlib\Dialogue\Profiles.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\Profiles.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\Profiles.obj"	"$(INTDIR)\Profiles.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\pschemaapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\pschemaapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\pschemaapi.obj"	"$(INTDIR)\pschemaapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\quit.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\quit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\quit.obj"	"$(INTDIR)\quit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readpivot.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\readpivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\readpivot.obj"	"$(INTDIR)\readpivot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readprs.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\readprs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\readprs.obj"	"$(INTDIR)\readprs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readstr.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\readstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\readstr.obj"	"$(INTDIR)\readstr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\readtra.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\readtra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\readtra.obj"	"$(INTDIR)\readtra.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\referenceapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\referenceapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\referenceapi.obj"	"$(INTDIR)\referenceapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\references.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\references.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\references.obj"	"$(INTDIR)\references.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\registry.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\registry.obj"	"$(INTDIR)\registry.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\res.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\res.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\res.obj"	"$(INTDIR)\res.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\resgen.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\resgen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\resgen.obj"	"$(INTDIR)\resgen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\resmatch.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\resmatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\resmatch.obj"	"$(INTDIR)\resmatch.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\schemas.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\schemas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\schemas.obj"	"$(INTDIR)\schemas.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\schtrad.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\schtrad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\schtrad.obj"	"$(INTDIR)\schtrad.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\scroll.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\scroll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\scroll.obj"	"$(INTDIR)\scroll.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\search.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\search.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\search.obj"	"$(INTDIR)\search.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\searchmenu.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\searchmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\searchmenu.obj"	"$(INTDIR)\searchmenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\searchref.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\searchref.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\searchref.obj"	"$(INTDIR)\searchref.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\selectionapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\selectionapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\selectionapi.obj"	"$(INTDIR)\selectionapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\spellchecker.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\spellchecker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\spellchecker.obj"	"$(INTDIR)\spellchecker.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\spline.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\spline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\spline.obj"	"$(INTDIR)\spline.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\dialogue\stix.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\stix.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\stix.obj"	"$(INTDIR)\stix.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\structcommands.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\structcommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\structcommands.obj"	"$(INTDIR)\structcommands.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\structcreation.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\structcreation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\structcreation.obj"	"$(INTDIR)\structcreation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\structlist.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\structlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\structlist.obj"	"$(INTDIR)\structlist.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\structmodif.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\structmodif.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\structmodif.obj"	"$(INTDIR)\structmodif.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\structschema.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\structschema.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\structschema.obj"	"$(INTDIR)\structschema.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\structselect.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\structselect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\structselect.obj"	"$(INTDIR)\structselect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\presentation\style.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\style.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\style.obj"	"$(INTDIR)\style.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\tableH.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\tableH.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\tableH.obj"	"$(INTDIR)\tableH.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\textcommands.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\textcommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\textcommands.obj"	"$(INTDIR)\textcommands.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\thotmsg.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\thotmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\thotmsg.obj"	"$(INTDIR)\thotmsg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\translation.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\translation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\translation.obj"	"$(INTDIR)\translation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\tree.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\tree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\tree.obj"	"$(INTDIR)\tree.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\tree\treeapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\treeapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\treeapi.obj"	"$(INTDIR)\treeapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\unicode\uconvert.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\uconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\uconvert.obj"	"$(INTDIR)\uconvert.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\undo.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\undo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\undo.obj"	"$(INTDIR)\undo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\undoapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\undoapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\undoapi.obj"	"$(INTDIR)\undoapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\base\units.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\units.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\units.obj"	"$(INTDIR)\units.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\editing\unstructchange.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\unstructchange.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\unstructchange.obj"	"$(INTDIR)\unstructchange.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\unstructlocate.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\unstructlocate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\unstructlocate.obj"	"$(INTDIR)\unstructlocate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\unicode\ustring.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\ustring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\ustring.obj"	"$(INTDIR)\ustring.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\viewapi.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\viewapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\viewapi.obj"	"$(INTDIR)\viewapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\views.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\views.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\views.obj"	"$(INTDIR)\views.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\view\windowdisplay.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\windowdisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\windowdisplay.obj"	"$(INTDIR)\windowdisplay.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\content\word.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\word.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\word.obj"	"$(INTDIR)\word.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\writedoc.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\writedoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\writedoc.obj"	"$(INTDIR)\writedoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\document\writepivot.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\writepivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\writepivot.obj"	"$(INTDIR)\writepivot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\xbmhandler.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\xbmhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\xbmhandler.obj"	"$(INTDIR)\xbmhandler.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\thotlib\image\xpmhandler.c

!IF  "$(CFG)" == "libThotEditor - Win32 Release"


"$(INTDIR)\xpmhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libThotEditor - Win32 Debug"


"$(INTDIR)\xpmhandler.obj"	"$(INTDIR)\xpmhandler.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

