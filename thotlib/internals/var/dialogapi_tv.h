/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef THOT_DIALOGAPI_VAR
#define THOT_DIALOGAPI_VAR

THOT_EXPORT Proc CallbackDialogueProc
  #ifdef THOT_INITGLOBALVAR
    = (Proc)CallbackError
  #endif 
;
//THOT_EXPORT void (*CallbackDialogue) (); /* initialized into dialogapi.c */

THOT_EXPORT ThotBool WithMessages
  #ifdef THOT_INITGLOBALVAR
    = TRUE
  #endif 
;
/* First free reference */
THOT_EXPORT int FirstFreeRef;
/* List of catalogues */
THOT_EXPORT struct Cat_List*    PtrCatalogue;
THOT_EXPORT struct E_List*      PtrFreeE_List;
THOT_EXPORT int                 NbOccE_List;
THOT_EXPORT int                 NbLibE_List;
THOT_EXPORT int                 CurrentWait;
THOT_EXPORT int                 ShowReturn;
THOT_EXPORT int                 ShowX, ShowY;
THOT_EXPORT struct Cat_Context* ShowCat
  #ifdef THOT_INITGLOBALVAR
    = NULL
  #endif 
;
THOT_EXPORT ThotWidget          MainShell, PopShell;

#ifdef _WINGUI
  THOT_EXPORT HFONT          formFONT;
  THOT_EXPORT char          *iconID;
  THOT_EXPORT OPENFILENAME  OpenFileName;
  THOT_EXPORT int           cyValue
  #ifdef THOT_INITGLOBALVAR
      = 10
    #endif 
  ;
#endif  /* _WINGUI */
  
#ifdef _MOTIF
  THOT_EXPORT ThotAppContext Def_AppCont;
  THOT_EXPORT Display       *GDp;
#endif /* _GTK */


#endif /* #ifndef THOT_DIALOGAPI_VAR */
