
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern PtrTabTri Ind_InitTabTri();
extern PtrTabTri GetTabTri(int num);
extern PtrAttribute Ind_TrouveRef(PtrElement pEl);
extern PtrTabInd GetTabInd(int num);
extern int FreeTabTri ( PtrTabTri pTab );
extern int FreeTabInd ( PtrTabInd pTab );
extern int Act_TestLoadExtension ( PtrDocument pDoc, Name VName, Name VNameP );
extern boolean Ind_SuppressionDesc ( PtrElement pEl, PtrDocument pDoc );
extern void Ind_CreerMarque ( PtrElement pEl, PtrDocument pDoc, char * valcle );
extern void Ind_NumMarque(PtrElement pEl, PtrDocument pDoc);
extern PtrElement Ind_LibMarque ( PtrElement pEl, PtrDocument pDoc , boolean verif, int *nb);
extern int Ind_SupDesc( PtrDocument pDoc );
extern int Ind_SupMarque( PtrDocument pDoc );
extern int Ind_dansdoc ( PtrElement pEl, PtrDocument pDoc );
extern void Ind_VerifMarque ( PtrElement *pEl, PtrDocument pDoc );
extern PtrElement Ind_LibDesc ( PtrElement pEl, PtrDocument pDoc );
extern void Ind_OrdDesc ( PtrElement pEl, PtrElement pSel, PtrDocument pDoc );
extern char * Ind_TitreDansMenu ( PtrElement pEl );
extern PtrElement Ind_TrouveDesc ( PtrElement pEl );
extern void Ind_Creation ( PtrElement pEl, PtrDocument pDoc );
extern int Ind_creerindex ( int numtable, boolean avec_raz, boolean avec_fusion );
extern int Ind_NbTable ();
extern int Ind_creertoutindex ( boolean avec_fusion );
extern void ActionIndexer ( int fenetre );

#else /* __STDC__ */
extern PtrTabTri Ind_InitTabTri();
extern PtrTabTri GetTabTri(/*int num*/);
extern PtrAttribute Ind_TrouveRef(/*PtrElement pEl*/);
extern PtrTabInd GetTabInd(/*int num*/);
extern int FreeTabTri (/* PtrTabTri pTab */);
extern int FreeTabInd (/* PtrTabInd pTab */);
extern int Act_TestLoadExtension (/* PtrDocument pDoc, Name Name, Name NameP */);
extern boolean Ind_SuppressionDesc (/* PtrElement pEl, PtrDocument pDoc */);
extern void Ind_CreerMarque (/* PtrElement pEl, PtrDocument pDoc, char * valcle */);
extern void Ind_NumMarque(/*PtrElement pEl, PtrDocument pDoc*/);
extern PtrElement Ind_LibMarque (/* PtrElement pEl, PtrDocument pDoc, boolean verif, int *nb */);
extern int Ind_SupDesc(/* PtrDocument pDoc */);
extern int Ind_SupMarque(/* PtrDocument pDoc */);
extern int Ind_dansdoc (/* PtrElement pEl, PtrDocument pDoc */);
extern void Ind_VerifMarque (/* PtrElement *pEl, PtrDocument pDoc */);
extern PtrElement Ind_LibDesc (/* PtrElement pEl, PtrDocument pDoc */);
extern void Ind_OrdDesc (/* PtrElement pEl, PtrElement pSel, PtrDocument pDoc */);
extern char * Ind_TitreDansMenu (/* PtrElement pEl */);
extern PtrElement Ind_TrouveDesc (/* PtrElement pEl */);
extern void Ind_Creation (/* PtrElement pEl, PtrDocument pDoc */);
extern int Ind_creerindex (/* int numtable, boolean avec_raz, boolean avec_fusion */);
extern int Ind_NbTable ();
extern int Ind_creertoutindex (/* boolean avec_fusion */);
extern void ActionIndexer (/* int fenetre */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
