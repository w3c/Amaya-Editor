
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void SearchLoadRessources();
extern void EnleveFormRecherche ( PtrDocument pDoc, boolean FermeDoc );
extern void RetMenuChercheElemVide ( int ref, int val );
extern void ChercherElementVide ( PtrDocument pDoc );
extern void RetMenuChercheReferVide ( int ref, int val );
extern void ChercherReferenceVide ( PtrDocument pDoc );
extern void RetMenuChoixRefer ( int val );
extern void CreeEtActiveMenuReferences ( char *bufMenu, int nbEntrees, int * entreeChoisie );
extern void RetMenuChercheRefA ( int Ref, int Data );
extern void ChercherLesReferences ( PtrDocument pDoc );
extern void ChercherRemplacerTexte ( PtrDocument pDoc );
extern void RetMenuRemplacerTexte ( int ref, int val, char *txt );
extern void MenuAllerPage ( PtrDocument pDoc, int VueDoc, int VueSch, boolean Assoc );
extern void RetMenuAllerPage ( int ref, int val );

#else /* __STDC__ */

extern void SearchLoadRessources();
extern void EnleveFormRecherche (/* PtrDocument pDoc, boolean FermeDoc */);
extern void RetMenuChercheElemVide (/* int ref, int val */);
extern void ChercherElementVide (/* PtrDocument pDoc */);
extern void RetMenuChercheReferVide (/* int ref, int val */);
extern void ChercherReferenceVide (/* PtrDocument pDoc */);
extern void RetMenuChoixRefer (/* int val */);
extern void CreeEtActiveMenuReferences (/* char *bufMenu, int nbEntrees, int * entreeChoisie */);
extern void RetMenuChercheRefA (/* int Ref, int Data */);
extern void ChercherLesReferences (/* PtrDocument pDoc */);
extern void ChercherRemplacerTexte (/* PtrDocument pDoc */);
extern void RetMenuRemplacerTexte (/* int ref, int val, char *txt */);
extern void MenuAllerPage (/* PtrDocument pDoc, int VueDoc, int VueSch, boolean Assoc */);
extern void RetMenuAllerPage (/* int ref, int val */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
