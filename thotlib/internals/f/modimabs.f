
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrAbstractBox Englobant ( PtrAbstractBox pPav1, PtrAbstractBox pPav2 );
extern void ChercheReglePEl ( PtrPRule *pRVue1, PtrElement pEl, int Vue, PRuleType TRegle, PtrPRule *pRegle );
extern PtrPRule ReglePEl ( PtrElement pEl, PtrPSchema *pSPR, PtrSSchema *pSSR, int NumPres, PtrPSchema pSchP, int Vue, PRuleType TRegle, boolean Pag, boolean attr, PtrAttribute *pAttr );
extern PtrPRule LaRegle ( PtrDocument pDoc, PtrAbstractBox pPav, PtrPSchema *pSPR, PRuleType TRegle, boolean attr, PtrAttribute *pAttr );
extern PtrPRule RegleFonction ( PtrElement pEl, PtrPSchema *pSchP );
extern void TuePave ( PtrAbstractBox pPav );
extern void SuppRfPave ( PtrAbstractBox pPav, PtrAbstractBox *PavReaff, PtrDocument pDoc );
extern void NouvRfPave ( PtrAbstractBox PremPav, PtrAbstractBox DerPav, PtrAbstractBox *PavReaff, PtrDocument pDoc );
extern void PlusPremDer ( PtrElement pEl, PtrDocument pDoc, boolean Prem, boolean Plus );
extern PtrElement PageElAssoc ( PtrElement pEl, int VueNb, int *typeBoite );
extern void CrPaveNouv ( PtrElement pEl, PtrDocument pDoc, int VueNb );
extern void DetPavVue ( PtrElement pEl, PtrDocument pDoc, boolean Verif, int vue );
extern void DetrPaves ( PtrElement pEl, PtrDocument pDoc, boolean Verif );
extern void ReafReference ( PtrReference pRef, PtrAbstractBox pPav, PtrDocument pDocRef );
extern void ChngRef ( PtrAbstractBox pPav, PtrDocument pDoc );
extern PtrAbstractBox PavCherche ( PtrAbstractBox pPav, boolean Pres, int Typ, PtrPSchema pSchP, PtrSSchema pSchStr );
extern void ReNumPages ( PtrElement pEl, int vue );
extern void TransmetValCompt ( PtrElement pEl, PtrDocument pDoc, Name NmAttr, int cpt, PtrPSchema pSchP, PtrSSchema pSchS );
extern void MajNumeros ( PtrElement pElDebut, PtrElement pElModif, PtrDocument pDocu, boolean reaff );
extern void ChngBtCompt ( PtrElement pElDebut, PtrDocument pDoc, int cpt, PtrPSchema pSchP, PtrSSchema pSS );
extern void ChngPresAttr ( PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, boolean Suppr, boolean herit, PtrAttribute pAttrComp );
extern boolean MemeTexte ( PtrElement pEl, PtrDocument pDoc, PtrElement *pLib );
extern void CreeTousPaves ( PtrElement pE, PtrDocument pDoc );

#else /* __STDC__ */

extern PtrAbstractBox Englobant (/* PtrAbstractBox pPav1, PtrAbstractBox pPav2 */);
extern void ChercheReglePEl (/* PtrPRule *pRVue1, PtrElement pEl, int Vue, PRuleType TRegle, PtrPRule *pRegle */);
extern PtrPRule ReglePEl (/* PtrElement pEl, PtrPSchema *pSPR, PtrSSchema *pSSR, int NumPres, PtrPSchema pSchP, int Vue, PRuleType TRegle, boolean Pag, boolean attr, PtrAttribute *pAttr */);
extern PtrPRule LaRegle (/* PtrDocument pDoc, PtrAbstractBox pPav, PtrPSchema *pSPR, PRuleType TRegle, boolean attr, PtrAttribute *pAttr */);
extern PtrPRule RegleFonction (/* PtrElement pEl, PtrPSchema *pSchP */);
extern void TuePave (/* PtrAbstractBox pPav */);
extern void SuppRfPave (/* PtrAbstractBox pPav, PtrAbstractBox *PavReaff, PtrDocument pDoc */);
extern void NouvRfPave (/* PtrAbstractBox PremPav, PtrAbstractBox DerPav, PtrAbstractBox *PavReaff, PtrDocument pDoc */);
extern void PlusPremDer (/* PtrElement pEl, PtrDocument pDoc, boolean Prem, boolean Plus */);
extern PtrElement PageElAssoc (/* PtrElement pEl, int VueNb, int *typeBoite */);
extern void CrPaveNouv (/* PtrElement pEl, PtrDocument pDoc, int VueNb */);
extern void DetPavVue (/* PtrElement pEl, PtrDocument pDoc, boolean Verif, int vue */);
extern void DetrPaves (/* PtrElement pEl, PtrDocument pDoc, boolean Verif */);
extern void ReafReference (/* PtrReference pRef, PtrAbstractBox pPav, PtrDocument pDocRef */);
extern void ChngRef (/* PtrAbstractBox pPav, PtrDocument pDoc */);
extern PtrAbstractBox PavCherche (/* PtrAbstractBox pPav, boolean Pres, int Typ, PtrPSchema pSchP, PtrSSchema pSchStr */);
extern void ReNumPages (/* PtrElement pEl, int vue */);
extern void TransmetValCompt (/* PtrElement pEl, PtrDocument pDoc, Name NmAttr, int cpt, PtrPSchema pSchP, PtrSSchema pSchS */);
extern void MajNumeros (/* PtrElement pElDebut, PtrElement pElModif, PtrDocument pDocu, boolean reaff */);
extern void ChngBtCompt (/* PtrElement pElDebut, PtrDocument pDoc, int cpt, PtrPSchema pSchP, PtrSSchema pSS */);
extern void ChngPresAttr (/* PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, boolean Suppr, boolean herit, PtrAttribute pAttrComp */);
extern boolean MemeTexte (/* PtrElement pEl, PtrDocument pDoc, PtrElement *pLib */);
extern void CreeTousPaves (/* PtrElement pE, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
