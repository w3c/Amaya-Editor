/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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

#ifndef _WIN_PRINT
THOT_EXPORT WIN_Form formulary;
THOT_EXPORT FrCatalogue FrameCatList [MAX_FRAME + 1];
#endif /* _WIN_PRINT */

THOT_EXPORT int          nAmayaShow;
/* following variables are declared as extern in frame_tv.h */
THOT_EXPORT HINSTANCE     hInstance
  #ifdef THOT_INITGLOBALVAR
      = 0
  #endif 
  ;

THOT_EXPORT HBITMAP       WIN_LastBitmap
  #ifdef THOT_INITGLOBALVAR
      = 0
    #endif 
  ;

THOT_EXPORT int      bIndex
  #ifdef THOT_INITGLOBALVAR
      = 0
    #endif 
  ;

THOT_EXPORT int      bAbsBase
  #ifdef THOT_INITGLOBALVAR
      = 60
    #endif 
  ;

THOT_EXPORT BYTE     fVirt;
THOT_EXPORT char     key;

THOT_EXPORT UINT subMenuID [MAX_FRAME];

#endif  /* _WINGUI */
  
#ifdef _MOTIF
  THOT_EXPORT ThotAppContext Def_AppCont;
  THOT_EXPORT Display       *GDp;
#endif /* _GTK */


#endif /* #ifndef THOT_DIALOGAPI_VAR */
