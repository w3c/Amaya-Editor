
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* 
   curs.c : lecture des curseurs X
   Module de gestion des curseurs et bitmaps X
   I. Vatton - Juillet 87
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#undef EXPORT
#define EXPORT extern
#include "frame.var"

/* ---------------------------------------------------------------------- */
/* |    InitCurs charge les curseurs utilises par l'interface.          | */
/* ---------------------------------------------------------------------- */
void                InitCurs ()
{
#ifndef NEW_WILLOWS
   CursCmd (0) = XCreateFontCursor (GDp (0), XC_crosshair);
   CursSel (0) = XCreateFontCursor (GDp (0), XC_top_left_arrow);
   CursFen (0) = XCreateFontCursor (GDp (0), XC_hand2);
   CursDepV (0) = XCreateFontCursor (GDp (0), XC_sb_v_double_arrow);
   CursDepH (0) = XCreateFontCursor (GDp (0), XC_sb_h_double_arrow);
   CursDepHV (0) = XCreateFontCursor (GDp (0), XC_fleur);
   CursWait (0) = XCreateFontCursor (GDp (0), XC_watch);
#endif /* NEW_WILLOWS */
}
