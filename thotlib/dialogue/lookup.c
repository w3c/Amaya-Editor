/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * lookup.c -- Binding of TtaXLookupString in place of the X11
 *             XLookupString function.
 *
 * Author: D. Veillard (INRIA/W3C)
 *
 */


#include "thot_gui.h"
#include "thot_sys.h"
#include "interface.h"

#ifndef _GTK
#ifndef _WINDOWS
/*----------------------------------------------------------------------
  XLookupString
  Thot XLookupString function replacement for the default X11 one.
  May generate problems when linking with the X11 libraries, but
  enables support for ISO-Latin-1 charset in Motif dialogs even if
  there is no correct Locale support.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 XLookupString (ThotKeyEvent * event, char *buffer, int nbytes,
				   KeySym * keysym, ThotComposeStatus * status)
#else  /* __STDC__ */
int                 XLookupString (event, buffer, nbytes, keysym, status)
ThotKeyEvent          *event;
char               *buffer;
int                 nbytes;
KeySym             *keysym;
ThotComposeStatus     *status;

#endif /* __STDC__ */
{
   return (TtaXLookupString (event, buffer, nbytes, keysym, status));
}
#endif /* ! _WINDOWS */
#endif /* _GTK */


/*----------------------------------------------------------------------
   LookupLoadResources 
  link in the XLookupString replacement        
  ----------------------------------------------------------------------*/
void                LookupLoadResources ()
{
#ifndef _WINDOWS
   /*
    * Use the Thot XLookupString
    */
   TtaUseOwnXLookupString = 1;
#endif /* ! _WINDOWS */
}

