/*
 * lookup.c -- Binding of TtaXLookupString in place of the X11
 *             XLookupString function.
 */


#include "thot_gui.h"
#include "thot_sys.h"
#include "interface.h"

#ifndef _WINDOWS
/*
 * Use the Thot XLookupString function instead of
 * the default X11 one. May generate problems when
 * linking with the X11 libraries, but enable support
 * for ISO-Latin-1 charset in Motif dialogs even if
 * there is no correct Locale support.
 */

#ifdef __STDC__
int                 XLookupString (XKeyEvent * event, char *buffer, int nbytes,
				   KeySym * keysym, XComposeStatus * status)
#else  /* __STDC__ */
int                 XLookupString (event, buffer, nbytes, keysym, status)
XKeyEvent          *event;
char               *buffer;
int                 nbytes;
KeySym             *keysym;
XComposeStatus     *status;

#endif /* __STDC__ */
{
   return (TtaXLookupString (event, buffer, nbytes, keysym, status));
}
#endif /* ! _WINDOWS */

/* ---------------------------------------------------------------------- */
/* | LookupLoadResources : link in the XLookupString replacement        | */
/* ---------------------------------------------------------------------- */
void                LookupLoadResources ()
{
   /*
    * Use the Thot XLookupString
    */
   TtaUseOwnXLookupString = 1;
}
