
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitMenuLangues ();
extern void RetMenuAttributs ( int refmenu, int att );
extern void RetMenuAttrRequis ( int ref, int val, char *txt );
extern void ConstruitMenuAttrRequis ( PtrAttribute pAttr, PtrDocument pDoc );
extern int ComposeMenuAttributs ( char BufMenu[1024], PtrDocument pDoc );
extern void RetMenuValAttr ( int ref, int valmenu, char *valtexte );
extern void RetMenuLangue ( int ref, int val, char * txt );

#else /* __STDC__ */

extern void InitMenuLangues ();
extern void RetMenuAttributs (/* int refmenu, int att */);
extern void RetMenuAttrRequis (/* int ref, int val, char *txt */);
extern void ConstruitMenuAttrRequis (/* PtrAttribute pAttr, PtrDocument pDoc*/);
extern int ComposeMenuAttributs (/* char BufMenu[1024], PtrDocument pDoc */);
extern void RetMenuValAttr (/* int ref, int valmenu, char *valtexte */);
extern void RetMenuLangue (/* int ref, int val, char * txt */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
