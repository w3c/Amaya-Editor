
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern boolean TueAvantPage ( PtrAbstractBox pPage, int fenetre, PtrDocument pDoc, int VueNb );
extern void Pages ( PtrDocument pDoc, int Vue, boolean Assoc );

#ifdef __COLPAGE__
/**CP*/ /* moins de procedures externes */

#else /* __COLPAGE__ */
extern void AjoutePageEnFin ( PtrElement, int, PtrDocument, boolean);
#endif /* __COLPAGE__ */

#else /* __STDC__ */
extern boolean TueAvantPage (/* PtrAbstractBox pPage, int fenetre, PtrDocument pDoc, int VueNb */);
extern void Pages (/* PtrDocument pDoc, int Vue, boolean Assoc */);

#ifdef __COLPAGE__
/**CP*/ /* moins de procedures externes */

#else /* __COLPAGE__ */
extern void AjoutePageEnFin (/* PtrElement pElRacine, int VueSch, PtrDocument pDoc, boolean withAPP */);
#endif /* __COLPAGE__ */

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
