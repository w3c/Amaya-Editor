
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern char *TypePave ( PtrAbstractBox pPav );
extern void LibPavVue ( PtrAbstractBox pPav );
extern void LibPavElem ( PtrElement pEl );
extern void LibPavMort ( PtrAbstractBox pPav );
extern void AjoutePaves ( PtrAbstractBox PavRacine, PtrDocument pDoc, boolean Tete );
extern boolean NonSecable ( PtrAbstractBox pPav );
extern void AjusteVolume ( PtrElement pEl, PtrDocument pDoc );
extern void VolAugmente ( boolean EnTete, int dVol, int fenetre );
extern void VolReduit ( boolean EnTete, int dVol, int fenetre );
extern void VerifPave ( PtrElement pEl, int Vue, PtrDocument pDoc, boolean debut, boolean affiche );
extern void VolumeArbAbs ( PtrAbstractBox PavRacine, PtrAbstractBox PremPav, PtrAbstractBox DerPav, int *VolAvant, int *VolApres, int *VolArbre );
extern void SauterDansVue ( int fenetre, int distance );
#ifdef __COLPAGE__
extern void RecursEvalCP(PtrAbstractBox pPav, PtrDocument pDoc);
extern void TuePresDroite(PtrAbstractBox pPav, PtrDocument pDoc);
extern void TuePresVoisin(PtrAbstractBox pVoisin, boolean Avant, PtrDocument pDoc, PtrAbstractBox *PavR, PtrAbstractBox *PavReaff, int *volsupp, PtrAbstractBox pPav, boolean SaufCreeAvec);
extern void DetrPaveSuivants(PtrAbstractBox pPav, PtrDocument pDoc);
extern void AffPaveDebug(PtrAbstractBox pPav);
extern boolean PaveCorrect(PtrAbstractBox pPav);
#endif /* __COLPAGE__ */

#else /* __STDC__ */

extern char *TypePave (/* PtrAbstractBox pPav */);
extern void LibPavVue (/* PtrAbstractBox pPav */);
extern void LibPavElem (/* PtrElement pEl */);
extern void LibPavMort (/* PtrAbstractBox pPav */);
extern void AjoutePaves (/* PtrAbstractBox PavRacine, PtrDocument pDoc, boolean Tete */);
extern boolean NonSecable (/* PtrAbstractBox pPav */);
extern void AjusteVolume (/* PtrElement pEl, PtrDocument pDoc */);
extern void VolAugmente (/* boolean EnTete, int dVol, int fenetre */);
extern void VolReduit (/* boolean EnTete, int dVol, int fenetre */);
extern void VerifPave (/* PtrElement pEl, int Vue, PtrDocument pDoc, boolean debut, boolean affiche */);
extern void VolumeArbAbs (/* PtrAbstractBox PavRacine, PtrAbstractBox PremPav, PtrAbstractBox DerPav, int *VolAvant, int *VolApres, int *VolArbre */);
extern void SauterDansVue (/* int fenetre, int distance */);
#ifdef __COLPAGE__
extern void RecursEvalCP(/* PtrAbstractBox pPav, PtrDocument pDoc */);
extern void TuePresDroite(/* PtrAbstractBox pPav, PtrDocument pDoc */);
extern void TuePresVoisin(/* PtrAbstractBox pVoisin, boolean Avant, PtrDocument pDoc, PtrAbstractBox *PavR, PtrAbstractBox *PavReaff, int *volsupp, PtrAbstractBox pPav, boolean SaufCreeAvec */);
extern void DetrPaveSuivants(/* PtrAbstractBox pPav, PtrDocument pDoc */);
extern void AffPaveDebug(/* PtrAbstractBox pPav */);
extern boolean PaveCorrect(/* PtrAbstractBox pPav */);
#endif /* __COLPAGE__ */

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
