
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern char *carsuivant ( char *carcour );
extern BOOL EstDelim ( char caractere );
extern BOOL EstBlanc ( char c );
extern phrase *DecouperPhrases ( int NumDoc, char *pBuffer, int long_Buffer, int *adrNbPhrases );
extern char *MyAlloc ( unsigned int Taille );
extern void ListeTexte ( Document Doc, Element element, char *Fich, char *Titre );

#else /* __STDC__ */

extern char *carsuivant (/* char *carcour */);
extern BOOL EstDelim (/* char caractere */);
extern BOOL EstBlanc (/* char c */);
extern phrase *DecouperPhrases (/* int NumDoc, char *pBuffer, int long_Buffer, int *adrNbPhrases */);
extern char *MyAlloc (/* unsigned int Taille */);
extern void ListeTexte (/* Document Doc, Element element, char *Fich, char *Titre */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
