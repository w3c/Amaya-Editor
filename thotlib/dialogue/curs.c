
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
   WindowCurs = XCreateFontCursor (TtDisplay, XC_hand2);
   VCurs = XCreateFontCursor (TtDisplay, XC_sb_v_double_arrow);
   HCurs = XCreateFontCursor (TtDisplay, XC_sb_h_double_arrow);
   HVCurs = XCreateFontCursor (TtDisplay, XC_fleur);
   WaitCurs = XCreateFontCursor (TtDisplay, XC_watch);
#endif /* NEW_WILLOWS */
}
