
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void EcrireAjModifAttr ( Element elem1, char *Buffer, int LgBuffer, BinFile Fichier );
extern void EcrireAjModifPres ( Element elem1, char *Buffer, int LgBuffer, BinFile Fichier );
extern void EcrireRetraitAttr ( Element elem2, char *Buffer, int LgBuffer, BinFile Fichier );
extern void EcrireRetraitPres ( Element elem2, char *Buffer, int LgBuffer, BinFile Fichier );
extern void TraiterModifAjAttrPres ( int codeAttrPres, int NomAttribut, BOOL *adrBAjModif, Element elem2, Element elem1, BinFile Fichier );
extern void TraiterRetraitAttrPres ( int codeAttrPres, int NomAttribut, BOOL *adrBRetrait, Element elem2, BinFile Fichier );
extern BOOL DiffLabAttrPres ( Element elem2, Element elem1, BinFile Fichier );
extern void ChercherCorrespFils ( Element elem2, Element elem1, int *CorrespFils1, int NbFils1 );
extern void GetFilsNum ( Element Pere, int Place, Element *adrFils );
extern void TraiterAjoutChaine ( Element elem2, int CodeAttribut, BinFile Fichier, BOOL *adrBRetraitChaine );
extern void ExtrairePhrase ( Element elem, int Place, int Lg, char *PhraseExt );
extern void TraiterRetraitChaine ( Element elem2, Element elem1, int CodeAttribut, BinFile Fichier, BOOL *adrBAjChaine );
extern BOOL TraiterFeuilleTexte ( Element elem2, Element elem1, BinFile Fichier );
extern void indic ( void );
extern void DeltaMain ( char *NameDoc1, Document DocAnnote, Document Doc1 );

#else /* __STDC__ */

extern void EcrireAjModifAttr (/* Element elem1, char *Buffer, int LgBuffer, BinFile Fichier */);
extern void EcrireAjModifPres (/* Element elem1, char *Buffer, int LgBuffer, BinFile Fichier */);
extern void EcrireRetraitAttr (/* Element elem2, char *Buffer, int LgBuffer, BinFile Fichier */);
extern void EcrireRetraitPres (/* Element elem2, char *Buffer, int LgBuffer, BinFile Fichier */);
extern void TraiterModifAjAttrPres (/* int codeAttrPres, int NomAttribut, BOOL *adrBAjModif, Element elem2, Element elem1, BinFile Fichier */);
extern void TraiterRetraitAttrPres (/* int codeAttrPres, int NomAttribut, BOOL *adrBRetrait, Element elem2, BinFile Fichier */);
extern BOOL DiffLabAttrPres (/* Element elem2, Element elem1, BinFile Fichier */);
extern void ChercherCorrespFils (/* Element elem2, Element elem1, int *CorrespFils1, int NbFils1 */);
extern void GetFilsNum (/* Element Pere, int Place, Element *adrFils */);
extern void TraiterAjoutChaine (/* Element elem2, int CodeAttribut, BinFile Fichier, BOOL *adrBRetraitChaine */);
extern void ExtrairePhrase (/* Element elem, int Place, int Lg, char *PhraseExt */);
extern void TraiterRetraitChaine (/* Element elem2, Element elem1, int CodeAttribut, BinFile Fichier, BOOL *adrBAjChaine */);
extern BOOL TraiterFeuilleTexte (/* Element elem2, Element elem1, BinFile Fichier */);
extern void indic (/* void */);
extern void DeltaMain (/* char *NameDoc1, Document DocAnnote, Document Doc1 */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
