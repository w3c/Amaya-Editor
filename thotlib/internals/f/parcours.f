
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern typeCarArbre *PremCarArbre ( Element Noeud );
extern void LibereCarArbre ( typeCarArbre *CA );
extern void CarArbreSuiv ( typeCarArbre *CA );
extern void CarArbrePrec ( typeCarArbre *CA );
extern void FeuilleArbreSuiv ( typeCarArbre *CA );
extern char CarArbre ( typeCarArbre *CA );
extern typeCarArbre *DuplCarArbre ( typeCarArbre *CA );
extern void CopieCarArbre ( typeCarArbre *CAsrc, typeCarArbre *CAdest );
extern BOOL EgCarArbre ( typeCarArbre *CA1, typeCarArbre *CA2 );
extern void MajCarArbre ( typeCarArbre *CA, Element Noeud );
extern void PrintCarArbre ( typeCarArbre *CA, char *ch );

#else /* __STDC__ */

extern typeCarArbre *PremCarArbre (/* Element Noeud */);
extern void LibereCarArbre (/* typeCarArbre *CA */);
extern void CarArbreSuiv (/* typeCarArbre *CA */);
extern void CarArbrePrec (/* typeCarArbre *CA */);
extern void FeuilleArbreSuiv (/* typeCarArbre *CA */);
extern char CarArbre (/* typeCarArbre *CA */);
extern typeCarArbre *DuplCarArbre (/* typeCarArbre *CA */);
extern void CopieCarArbre (/* typeCarArbre *CAsrc, typeCarArbre *CAdest */);
extern BOOL EgCarArbre (/* typeCarArbre *CA1, typeCarArbre *CA2 */);
extern void MajCarArbre (/* typeCarArbre *CA, Element Noeud */);
extern void PrintCarArbre (/* typeCarArbre *CA, char *ch */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
