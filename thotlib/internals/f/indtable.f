
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern PtrElement Ind_recupmarque(PtrElement pDesc);
extern PtrElement Ind_marquesuivante(PtrElement pDesc, PtrElement pMarque);
extern PtrElement Ind_table(int num);
extern int Ind_creertable(int numtable, boolean avec_raz);
extern void Ind_remplirtable(boolean avec_fusion);
extern void Ind_remplirCR();
extern char *Ind_textecle(int numcle, PtrElement pEl);
extern char *Ind_semantique(PtrElement pEl);
extern int Ind_inittable(int num);
extern int Ind_tousdesc();
extern int Ind_touscroise();

#else /* __STDC__ */
extern PtrElement Ind_recupmarque(/*PtrElement pDesc*/);
extern PtrElement Ind_marquesuivante(/*PtrElement pDesc, PtrElement pMarque*/);
extern PtrElement Ind_table(/*int num*/);
extern int Ind_creertable(/*int numtable, boolean avec_raz*/);
extern void Ind_remplirtable(/*boolean avec_fusion*/);
extern void Ind_remplirCR();
extern char *Ind_textecle(/*int numcle, PtrElement pEl*/);
extern char *Ind_semantique(/*PtrElement pEl*/);
extern int Ind_inittable(/*int num*/);
extern int Ind_tousdesc();
extern int Ind_touscroise();
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
