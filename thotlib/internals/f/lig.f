
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int round(float e);
extern PtrBox Suivante ( PtrAbstractBox adpave );
extern void Aligner ( PtrBox boxmere, PtrLine adligne, int delta, int fen, boolean absoluEnX, boolean absoluEnY );
extern boolean testcsl ( PtrBox ibox, int *large, int *lgcarspec, int *nbcarsl, int *nbblanc, int *newicar, PtrTextBuffer *newbuff );
extern void AjoutBoite ( PtrBox ibox, int *sousbase, int *surbase, PtrLine adligne );
#ifdef __COLPAGE__
extern void RemplirLigne(PtrLine adligne, PtrAbstractBox Pv, boolean uneSuite, boolean *plein, boolean *ajust);
#else /* __COLPAGE__ */
extern void RemplirLigne ( PtrLine adligne, PtrAbstractBox Pv, boolean *plein, boolean *ajust );
#endif /* __COLPAGE__ */
extern void BlocDeLigne ( PtrBox ibox, int fen, int *haut );
extern void DispBloc ( PtrBox ibox, int fen, PtrLine premligne, boolean *chgDS, boolean *chgFS );
extern void ReevalBloc ( PtrAbstractBox adpave, PtrLine lig, PtrBox org, int fen );
extern void MajBloc ( PtrAbstractBox pave, PtrLine adligne, PtrBox ibox, int dx, int dbl, int fen );
extern void EnglLigne ( PtrBox ibox, int fen, PtrAbstractBox adpave );

#else /* __STDC__ */

extern int round(/*float e*/);
extern PtrBox Suivante (/* PtrAbstractBox adpave */);
extern void Aligner (/* PtrBox boxmere, PtrLine adligne, int delta, int fen, boolean absoluEnX, boolean absoluEnY */);
extern boolean testcsl (/* PtrBox ibox, int *large, int *lgcarspec, int *nbcarsl, int *nbblanc, int *newicar, PtrTextBuffer *newbuff */);
extern void AjoutBoite (/* PtrBox ibox, int *sousbase, int *surbase, PtrLine adligne */);
#ifdef __COLPAGE__
extern void RemplirLigne(/* PtrLine adligne, PtrAbstractBox Pv, boolean *plein, boolean *ajust */);
#else /* __COLPAGE__ */
extern void RemplirLigne (/* PtrLine adligne, PtrAbstractBox Pv, boolean *plein, boolean *ajust */);
#endif /* __COLPAGE__ */
extern void BlocDeLigne (/* PtrBox ibox, int fen, int *haut */);
extern void DispBloc (/* PtrBox ibox, int fen, PtrLine premligne, boolean *chgDS, boolean *chgFS */);
extern void ReevalBloc (/* PtrAbstractBox adpave, PtrLine lig, PtrBox org, int fen */);
extern void MajBloc (/* PtrAbstractBox pave, PtrLine adligne, PtrBox ibox, int dx, int dbl, int fen */);
extern void EnglLigne (/* PtrBox ibox, int fen, PtrAbstractBox adpave */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
