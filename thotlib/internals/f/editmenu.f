
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void SaisitNomSchPres ( PtrSSchema pSchStr, Name name );
extern boolean ConfirmeFerme ( PtrDocument pDoc, boolean *Sauver );
extern boolean FormeComplete ( void );
extern void MotifOuvrirUneVue ( int frame );
extern void MotifSynchronisationVues ( int frame );
extern void MotifPaginerVue ( int frame );
extern void MotifFermerLaVue ( int frame );
extern void MotifLister ( int frame );
extern void MotifSauverDocument ( int frame );
extern void MotifSauverComme ( int frame );
extern void MotifAutoSauvegarde ( int frame );
extern void MotifPaginerDoc ( int frame );
extern void MotifImpression ( int frame );
extern void MotifPresenter ( int frame );
extern void MotifFermerLeDocument ( int frame );
extern void MotifAnnulerCommande ( int frame );
extern void MotifChercherTexte ( int frame );
extern void MotifChercherElementVide ( int frame );
extern void MotifChercherReferenceVide ( int frame );
extern void MotifChercherRefA ( int frame );
extern void MotifAllerPageNumero ( int frame );
extern void MotifPresenterCaracteres ( int frame );
extern void MotifPresenterGraphiques ( int frame );
extern void MotifPresenterCouleurs ( int frame );
extern void MotifPresenterFormat ( int frame );
extern void MotifPresentationStandard ( int frame );
extern void MotifCreerSautPage ( int frame );
extern void ExecCmd ( int numero, boolean shift );
extern void EnleveFormulaires ( PtrDocument pDoc, boolean FermetureDoc );
extern void CallbackEditor ( int Ref, int TypeData, int Data );
extern boolean MenuImage(int frame, char *nom, int *typim, int *pres, PtrBox ibox);

#else /* __STDC__ */

extern void SaisitNomSchPres (/* PtrSSchema pSchStr, Name name */);
extern boolean ConfirmeFerme (/* PtrDocument pDoc, boolean *Sauver */);
extern boolean FormeComplete (/* void */);
extern void MotifOuvrirUneVue (/* int frame */);
extern void MotifSynchronisationVues (/* int frame */);
extern void MotifPaginerVue (/* int frame */);
extern void MotifFermerLaVue (/* int frame */);
extern void MotifLister (/* int frame */);
extern void MotifSauverDocument (/* int frame */);
extern void MotifSauverComme (/* int frame */);
extern void MotifAutoSauvegarde (/* int frame */);
extern void MotifPaginerDoc (/* int frame */);
extern void MotifImpression (/* int frame */);
extern void MotifPresenter (/* int frame */);
extern void MotifFermerLeDocument (/* int frame */);
extern void MotifAnnulerCommande (/* int frame */);
extern void MotifChercherTexte (/* int frame */);
extern void MotifChercherElementVide (/* int frame */);
extern void MotifChercherReferenceVide (/* int frame */);
extern void MotifChercherRefA (/* int frame */);
extern void MotifAllerPageNumero (/* int frame */);
extern void MotifPresenterCaracteres (/* int frame */);
extern void MotifPresenterGraphiques (/* int frame */);
extern void MotifPresenterCouleurs (/* int frame */);
extern void MotifPresenterFormat (/* int frame */);
extern void MotifPresentationStandard (/* int frame */);
extern void MotifCreerSautPage (/* int frame */);
extern void ExecCmd (/* int numero, boolean shift */);
extern void EnleveFormulaires (/* PtrDocument pDoc, boolean FermetureDoc */);
extern void CallbackEditor (/* int Ref, int TypeData, int Data */);
extern boolean MenuImage(/*int frame, char *nom, int *typim, int *pres, PtrBox ibox */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
