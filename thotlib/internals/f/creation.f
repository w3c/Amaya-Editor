
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void NotifySubTree ( APPevent AppEvent, PtrDocument pDoc, PtrElement pEl, int docOrig );
extern void NomTypeUtilisateur(PtrSSchema pSS, int NumType, Name Retour);
extern void ReaffDoc ( PtrDocument pDoc );
extern void MajImAbs ( PtrDocument pDoc );
extern PtrElement FirstAssocElem ( PtrDocument doc, int typeref, PtrSSchema pSSref );
extern PtrElement CreeAssocElem ( PtrDocument doc, int RefType, PtrSSchema pSSref );
extern boolean RemplRefer ( PtrElement pElRef, PtrAttribute pAttrRef, PtrDocument pDoc, PtrElement *pSelEl );
extern void ChaineChoix ( PtrElement pEl, PtrElement *p, PtrDocument pDoc );
extern void RetMenuChoixElem ( int Entree, char *pNom );
extern int MenuChoixElem(PtrSSchema pS, int regle, PtrElement pEl, char *BufMenu, Name TitreMenu, PtrDocument pDoc);
extern void Traite_exceptions_creation ( PtrElement pEl, PtrDocument pDoc );
extern PtrElement CreeVoisin ( PtrDocument pDoc, PtrElement pEl, boolean Dev, boolean CreePaves, int TypeEl, PtrSSchema pSS, boolean Inclusion );
extern PtrElement CreeDedans ( PtrDocument pDoc, PtrElement pEl, boolean CreePaves, boolean Inclusion );
extern PtrElement CreeColleDansTexte ( boolean cree, boolean colle, boolean Page, PtrElement *pLib );
extern void TesterAction_CreeColle ( int *prevmenuind, int *menuind, int *nbentree );
extern void RetMenuCreeColle ( boolean cree, boolean colle, int Val );
extern void CreeMenuPage();
extern void CreeColle ( boolean cree, boolean colle, char bouton , boolean *ok);
extern PtrAbstractBox CreeFeuille ( PtrAbstractBox DansPav, int *fenetre, LeafType Nat, boolean Before );
extern void ReaffNouvContenu ( PtrElement pEl1, PtrDocument pDoc, int dvol, int VueAJour, PtrAbstractBox pPav );
extern void NouvContenu ( PtrAbstractBox pPav );
extern void CoupeAvantSelection( PtrElement *premsel, int *premcar, PtrElement *dersel, int *dercar, PtrDocument pDoc );
extern void PavesTexteCoupe(PtrElement pEl, PtrElement pSuite, PtrElement pSuivant, PtrDocument pDoc);
extern void CoupeApresSelection(PtrElement dersel, int dercar, PtrDocument pDoc);

#else /* __STDC__ */

extern void NotifySubTree (/* APPevent AppEvent, PtrDocument pDoc, PtrElement pEl, int docOrig */);
extern void NomTypeUtilisateur(/*PtrSSchema pSS, int NumType, Name Retour*/);
extern void ReaffDoc (/* PtrDocument pDoc */);
extern void MajImAbs (/* PtrDocument pDoc */);
extern PtrElement FirstAssocElem (/* PtrDocument doc, int typeref, PtrSSchema pSSref */);
extern PtrElement CreeAssocElem (/* PtrDocument doc, int RefType, PtrSSchema pSSref */);
extern boolean RemplRefer (/* PtrElement pElRef, PtrAttribute pAttrRef, PtrDocument pDoc, PtrElement *pSelEl */);
extern void ChaineChoix (/* PtrElement pEl, PtrElement *p, PtrDocument pDoc */);
extern void RetMenuChoixElem (/* int Entree, char *pNom */);
extern int MenuChoixElem(/*PtrSSchema pS, int regle, PtrElement pEl, char *BufMenu, Name TitreMenu, PtrDocument pDoc*/);
extern void Traite_exceptions_creation (/* PtrElement pEl, PtrDocument pDoc */);
extern PtrElement CreeVoisin (/* PtrDocument pDoc, PtrElement pEl, boolean Dev, boolean CreePaves, int TypeEl, PtrSSchema pSS, boolean Inclusion */);
extern PtrElement CreeDedans (/* PtrDocument pDoc, PtrElement pEl, boolean CreePaves, boolean Inclusion */);
extern PtrElement CreeColleDansTexte (/* boolean cree, boolean colle, boolean Page, PtrElement *pLib */);
extern void TesterAction_CreeColle (/* int *prevmenuind, int *menuind, int *nbentree */);
extern void RetMenuCreeColle (/* boolean cree, boolean colle, int Val */);
extern void CreeMenuPage();
extern void CreeColle (/* boolean cree, boolean colle, char bouton, boolean *ok */);
extern PtrAbstractBox CreeFeuille (/* PtrAbstractBox DansPav, int *fenetre, LeafType Nat, boolean Before */);
extern void ReaffNouvContenu (/* PtrElement pEl1, PtrDocument pDoc, int dvol, int VueAJour, PtrAbstractBox pPav */);
extern void NouvContenu (/* PtrAbstractBox pPav */);
extern void CoupeAvantSelection(/* PtrElement *premsel, int *premcar, PtrElement *dersel, int *dercar, PtrDocument pDoc */);
extern void PavesTexteCoupe(/* PtrElement pEl, PtrElement pSuite, PtrElement pSuivant, PtrDocument pDoc */);
extern void CoupeApresSelection(/*PtrElement dersel, int dercar, PtrDocument pDoc*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
