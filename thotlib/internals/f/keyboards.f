
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void AddKeyboards ();
extern void KBMap(int kb);
extern void SetupKeyboard ( int kb );
#ifdef WWW_XWINDOWS
extern void ColorsEvent ( XEvent *event );
#endif /* WWW_XWINDOWS */

#else /* __STDC__ */

extern void AddKeyboards ();
extern void KBMap(/*int kb*/);
extern void SetupKeyboard (/* int kb */);
#ifdef WWW_XWINDOWS
extern void ColorsEvent (/* XEvent *event */);
#endif /* WWW_XWINDOWS */

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
