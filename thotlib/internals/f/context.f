
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

#ifdef WWW_XWINDOWS
extern void X_Erreur ( Display *dpy, XErrorEvent *err );
extern void X_ErreurFatale ( Display *dpy );
extern void SelectionEvents ( XSelectionEvent *event );
#endif /* WWW_XWINDOWS */
extern boolean ShowReference ( void );
extern void InitGC ( void );
extern void InitEcrans ( char *name, int dx, int dy );
extern void InitApplications ( void );
extern void InitDocContexts ( void );
extern boolean ShowReadOnly();

#else /* __STDC__ */

#ifdef WWW_XWINDOWS
extern void X_Erreur (/* Display *dpy, XErrorEvent *err */);
extern void X_ErreurFatale (/* Display *dpy */);
extern void SelectionEvents (/* XSelectionEvent *event */);
#endif /* WWW_XWINDOWS */
extern boolean ShowReference (/* void */);
extern void InitGC (/* void */);
extern void InitEcrans (/* char *name, int dx, int dy */);
extern void InitApplications (/* void */);
extern void InitDocContexts (/* void */);
extern boolean ShowReadOnly();

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
