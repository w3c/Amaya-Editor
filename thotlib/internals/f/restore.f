
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CopieModif ( Document Doc1, Element elem2, Element *elem1, BinFile Fichier, char *Marque, Element Pere );
extern void AjouteChaine ( char *BufferDep, int LgBufferDep, char **AdrBufferFin, BinFile Fichier, char *MarqueSuiv, int *LgResu );
extern void main ( int argc, char **argv );

#else /* __STDC__ */

extern void CopieModif (/* Document Doc1, Element elem2, Element *elem1, BinFile Fichier, char *Marque, Element Pere */);
extern void AjouteChaine (/* char *BufferDep, int LgBufferDep, char **AdrBufferFin, BinFile Fichier, char *MarqueSuiv, int *LgResu */);
extern void main (/* int argc, char **argv */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
