
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern BOOL EstDans ( int j, int *DejaVu2, int LgDejaVu2 );
extern void Union ( int *DejaVu2, int *adrLgDejaVu2, int ind2 );
extern int ChercheTauxMax ( RensComp *StockComp, int LgStockComp );
extern void Liberation ( RensComp *StockComp, int LgStockComp );
extern float ComparerFeuille ( Element Noeud1, Element Noeud2 );
extern int ChercherNoeud ( int place1, Element Noeud1, Element Pere2, int *DejaVu2, int *adrLgDejaVu2 );
extern int GetNbElemAssoc ( Document document );
extern int ParcoursArbre ( Document document1, Document document2 );

#else /* __STDC__ */

extern BOOL EstDans (/* int j, int *DejaVu2, int LgDejaVu2 */);
extern void Union (/* int *DejaVu2, int *adrLgDejaVu2, int ind2 */);
extern int ChercheTauxMax (/* RensComp *StockComp, int LgStockComp */);
extern void Liberation (/* RensComp *StockComp, int LgStockComp */);
extern float ComparerFeuille (/* Element Noeud1, Element Noeud2 */);
extern int ChercherNoeud (/* int place1, Element Noeud1, Element Pere2, int *DejaVu2, int *adrLgDejaVu2 */);
extern int GetNbElemAssoc (/* Document document */);
extern int ParcoursArbre (/* Document document1, Document document2 */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
