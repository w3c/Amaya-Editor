
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef _DIALOG_H_
#define _DIALOG_H_

#ifdef WWW_XWINDOWS
#include <X11/Intrinsic.h>
#endif
#include "typebase.h"
#include "tree.h"

#define INTEGER_DATA 1
#define STRING_DATA 2

enum DButtons
  {
     D_CANCEL, D_DONE
  };

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void         TtaNewPulldown (int ref, ThotWidget parent, char *title, int number, char *text, char *equiv);
extern void         TtaSetPulldownOff (int ref, ThotWidget parent);
extern void         TtaSetPulldownOn (int ref, ThotWidget parent);
extern void         TtaNewPopup (int ref, ThotWidget parent, char *title, int number, char *text, char *equiv, char button);
extern void         TtaNewSubmenu (int ref, int ref_parent, int entry, char *title, int number, char *text, char *equiv, boolean react);
extern void         TtaNewIconMenu (int ref, int ref_parent, int entry, char *title, int number, Pixmap * icons, boolean horizontal);
extern void         TtaSetMenuForm (int ref, int val);
extern void         TtaNewToggleMenu (int ref, int ref_parent, char *title, int number, char *text, char *equiv, boolean react);
extern void         TtaSetToggleMenu (int ref, int val, boolean on);
extern void         TtaChangeMenuEntry (int ref, int entry, char *texte);
extern void         TtaRedrawMenuEntry (int ref, int entry, char *fontname, Pixel color, int activate);
extern void         TtaDestroyDialogue (int ref);
extern void         TtaNewForm (int, ThotWidget, int, int, char *, boolean, int, char, int);
extern void         TtaChangeFormTitle (int ref, char *title);
extern void         TtaNewSheet (int, ThotWidget, int, int, char *, int, char *, boolean, int, char, int);
extern void         TtaNewDialogSheet (int, ThotWidget, int, int, char *, int, char *, boolean, int, char, int);
extern void         TtaAttachForm (int ref);
extern void         TtaDetachForm (int ref);
extern void         TtaNewSelector (int ref, int ref_parent, char *title, int number, char *text, int height, char *label, boolean withText, boolean react);
extern void         TtaSetSelector (int ref, int entry, char *text);
extern void         TtaNewLabel (int ref, int ref_parent, char *text);
extern void         TtaNewTextForm (int ref, int ref_parent, char *title, int width, int height, boolean react);
extern void         TtaSetTextForm (int ref, char *text);
extern void         TtaNewNumberForm (int ref, int ref_parent, char *title, int min, int max, boolean react);
extern void         TtaSetNumberForm (int ref, int val);
extern void         TtaSetDialoguePosition (void);
extern void         TtaShowDialogue (int ref, boolean remanent);
extern void         TtaWaitShowDialogue (void);
extern boolean      TtaTestWaitShowDialogue (void);
extern void         TtaAbortShowDialogue (void);
extern void         TtaUnmapDialogue (int ref);

#else  /* __STDC__ */

extern void         TtaNewPulldown ( /* int ref, ThotWidget parent, char *title, int number, char *text, char *equiv */ );
extern void         TtaSetPulldownOff ( /* int ref, ThotWidget parent */ );
extern void         TtaSetPulldownOn ( /* int ref, ThotWidget parent */ );
extern void         TtaNewPopup ( /* int ref, ThotWidget parent, char *title, int number, char *text, char *equiv, char button */ );
extern void         TtaNewSubmenu ( /* int ref, int ref_parent, int entry, char *title, int number, char *text, char *equiv, boolean react */ );
extern void         TtaNewIconMenu ( /*int ref, int ref_parent, int entry, char *title, int number, Pixmap *icons,  boolean horizontal */ );
extern void         TtaSetMenuForm ( /* int ref, int val */ );
extern void         TtaNewToggleMenu ( /* int ref, int ref_parent, char *title, int number, char *text, char *equiv, boolean react */ );
extern void         TtaSetToggleMenu ( /* int ref, int val, boolean on */ );
extern void         TtaChangeMenuEntry ( /* int ref, int entry, char *texte */ );
extern void         TtaRedrawMenuEntry ( /* int ref, int entry, char *fontname, Pixel color, int activate */ );
extern void         TtaDestroyDialogue ( /* int ref */ );
extern void         TtaNewForm ( /* int ref, ThotWidget parent, int ref_parent, int entry, char *title, boolean horizontal, int package, char button */ );
extern void         TtaChangeFormTitle ( /* int ref, char *title */ );
extern void         TtaNewSheet ( /* int ref, ThotWidget parent, int ref_parent, int entry, char *title, int number, char *text, boolean horizontal, int package, char button */ );
extern void         TtaNewDialogSheet ( /* int ref, ThotWidget parent, int ref_parent, int entry, char *title, int number, char *text, boolean horizontal, int package, char button */ );
extern void         TtaAttachForm ( /* int ref */ );
extern void         TtaDetachForm ( /* int ref */ );
extern void         TtaNewSelector ( /* int ref, int ref_parent, char *title, int number, char *text, int height, char *label, boolean withText, boolean react */ );
extern void         TtaSetSelector ( /* int ref, int entry, char *text */ );
extern void         TtaNewLabel ( /* int ref, int ref_parent, char *text */ );
extern void         TtaNewTextForm ( /* int ref, int ref_parent, char *title, int width, int height, boolean react */ );
extern void         TtaSetTextForm ( /* int ref, char *text */ );
extern void         TtaNewNumberForm ( /* int ref, int ref_parent, char *title, int min, int max, boolean react */ );
extern void         TtaSetNumberForm ( /* int ref, int val */ );
extern void         TtaSetDialoguePosition ( /* void */ );
extern void         TtaShowDialogue ( /* int ref, boolean remanent */ );
extern void         TtaWaitShowDialogue ( /* void */ );
extern boolean      TtaTestWaitShowDialogue ( /* void */ );
extern void         TtaAbortShowDialogue ( /* void */ );
extern void         TtaUnmapDialogue ( /* int ref */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
