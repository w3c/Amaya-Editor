
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void AnnoterRacine ( Element RlRoot );
extern void Ajouter ( char *chaine, int place, int longueur );
extern void AddAttr ( char *chaine, int place );
extern void AnnoterElemAssoc ( Element Root2, int CodeAttr, char *TypeNom );
extern void AnnoterFeuilles ( Element Noeud1, Element Noeud2, int *TabCorresp, int lgTab );
extern void AnnoterNewMetaTexte ( Element Noeud );
extern void AnnoterPhrasesDetruites ( strTexte *Meta, char TabDet[], int NbEntrees );
extern void AttacherAttributNum ( typeCarArbre *CAd, typeCarArbre *CAf, int TtAttribute, int Valeur );

#else /* __STDC__ */

extern void AnnoterRacine (/* Element RlRoot */);
extern void Ajouter (/* char *chaine, int place, int longueur */);
extern void AddAttr (/* char *chaine, int place */);
extern void AnnoterElemAssoc (/* Element Root2, int CodeAttr, char *TypeNom */);
extern void AnnoterFeuilles (/* Element Noeud1, Element Noeud2, int *TabCorresp, int lgTab */);
extern void AnnoterNewMetaTexte (/* Element Noeud */);
extern void AnnoterPhrasesDetruites (/* strTexte *Meta, char TabDet[], int NbEntrees */);
extern void AttacherAttributNum (/* typeCarArbre *CAd, typeCarArbre *CAf, int TtAttribute, int Valeur */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
