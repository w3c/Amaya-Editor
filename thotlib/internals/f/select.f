
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void TtcPreviousElement(Document document, View view);
extern void TtcNextElement(Document document, View view);
extern void TtcParentElement(Document document, View view);
extern void TtcChildElement(Document document, View view);
extern void InitSelect ( void );
extern void MarqueInsertion ( void );
extern boolean SelEditeur ( PtrDocument *SDoc, PtrElement *PrEl, PtrElement *DerEl, int *PrCar, int *DerCar );
extern void SelectVue ( PtrDocument *pD, int *nv, boolean *Assoc );
extern void AnnuleSelect ( void );
extern void DeSelDoc ( PtrDocument pDoc );
extern PtrElement SelSuivant ( PtrElement pEl, PtrElement PcLast );
extern void ChngSelVuesInact ( void );
extern boolean HiddenType ( PtrElement pEl );
extern void AllumeSelection ( boolean DebVisible, boolean drag );
extern void DesactVue ( PtrDocument pDoc, int vue, boolean assoc );
extern void VisuSelect ( PtrAbstractBox PavRac, boolean Visible );
extern void AllumeEnglobantVisible ( PtrElement pEl );
extern void SelectPosition ( PtrDocument pDoc, PtrElement pEl, int Prem );
extern void SelectString ( PtrDocument pDoc, PtrElement pEl, int Prem, int Der );
extern void SelectEl ( PtrDocument pDoc, PtrElement pEl, boolean Debut, boolean Controle );
extern void SelEtend ( PtrElement pEl, int icar, boolean ptfixe, boolean debut, boolean drag );
extern void SelAjoute ( PtrElement pEl, boolean dernier );
extern void DeSelect ( PtrElement pEl, PtrDocument pDoc );
extern void SelectWithAPP ( PtrDocument pDoc, PtrElement pEl, boolean Debut, boolean Controle );
extern void SelectPositionWithAPP ( PtrDocument pDoc, PtrElement pEl, int Prem );
extern void SelectStringWithAPP ( PtrDocument pDoc, PtrElement pEl, int Prem, int Der );
extern void SelectCour ( int nfen, PtrAbstractBox pPav, int icar, boolean extension, boolean avecmaj, boolean DoubleClic, boolean drag );
extern char *TypePageCol( PtrElement pPage ) ;
extern void PrepareMenuSelection();
extern void ComposeMessageSelect();
extern int ComposeMenuSelection ( char BufMenu[1024] );
extern boolean SelectIntervallePaire ( void );
extern void SelectionAutour ( int val );
extern void TtaSetCurrentKeyboard(int keyboard);
#else /* __STDC__ */

extern void TtcPreviousElement(/*Document document, View view*/);
extern void TtcNextElement(/*Document document, View view*/);
extern void TtcParentElement(/*Document document, View view*/);
extern void TtcChildElement(/*Document document, View view*/);
extern void InitSelect (/* void */);
extern void MarqueInsertion (/* void */);
extern boolean SelEditeur (/* PtrDocument *SDoc, PtrElement *PrEl, PtrElement *DerEl, int *PrCar, int *DerCar */);
extern void SelectVue (/* PtrDocument *pD, int *nv, boolean *Assoc */);
extern void AnnuleSelect (/* void */);
extern void DeSelDoc (/* PtrDocument pDoc */);
extern PtrElement SelSuivant (/* PtrElement pEl, PtrElement PcLast */);
extern void ChngSelVuesInact (/* void */);
extern boolean HiddenType (/* PtrElement pEl */);
extern void AllumeSelection (/* boolean DebVisible, boolean drag */);
extern void DesactVue (/* PtrDocument pDoc, int vue, boolean assoc */);
extern void VisuSelect (/* PtrAbstractBox PavRac, boolean Visible */);
extern void AllumeEnglobantVisible (/* PtrElement pEl */);
extern void SelectPosition (/* PtrDocument pDoc, PtrElement pEl, int Prem */);
extern void SelectString (/* PtrDocument pDoc, PtrElement pEl, int Prem, int Der */);
extern void SelectEl (/* PtrDocument pDoc, PtrElement pEl, boolean Debut, boolean Controle */);
extern void SelEtend (/* PtrElement pEl, int icar, boolean ptfixe, boolean debut, boolean drag */);
extern void SelAjoute (/* PtrElement pEl, boolean dernier */);
extern void DeSelect (/* PtrElement pEl, PtrDocument pDoc */);
extern void SelectWithAPP (/* PtrDocument pDoc, PtrElement pEl, boolean Debut, boolean Controle */);
extern void SelectPositionWithAPP (/* PtrDocument pDoc, PtrElement pEl, int Prem */);
extern void SelectStringWithAPP (/* PtrDocument pDoc, PtrElement pEl, int Prem, int Der */);
extern void SelectCour (/* int nfen, PtrAbstractBox pPav, int icar, boolean extension, boolean avecmaj, boolean DoubleClic, boolean drag */);
extern  char *TypePageCol( /* PtrElement pPage */ );
extern void PrepareMenuSelection();
extern void ComposeMessageSelect();
extern int ComposeMenuSelection (/* char BufMenu[1024] */);
extern boolean SelectIntervallePaire (/* void */);
extern void SelectionAutour (/* int val */);
extern void TtaSetCurrentKeyboard(/*int keyboard*/);
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
