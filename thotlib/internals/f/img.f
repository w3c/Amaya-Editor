
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern C_points *PointsControle(PtrTextBuffer buffer, int nb);
extern void DimTexte ( PtrTextBuffer adbuff, int nbcar, ptrfont font, int *large, int *carbl );
extern void EvalSymb ( PtrAbstractBox adpave, int *large, int *haut );
extern void EvalGraph ( PtrAbstractBox adpave, int *large, int *haut );
extern void EvalPolyLine ( PtrAbstractBox adpave, int *large, int *haut );
extern void EvalText ( PtrAbstractBox adpave, int *large, int *haut, int *carbl );
extern void EvalComp ( PtrAbstractBox adpave, int fen, int *large, int *haut );
extern PtrLine DesLigne ( PtrBox ibox );
extern void MajBox ( PtrBox ibox, PtrLine adligne, int dcar, int dbl, int dx, int just, int dy, int fen, boolean BCOUP );
extern void DispBoite ( PtrAbstractBox adpave, boolean Recree, int fen );
extern void DiffereEnglobement ( PtrBox ibox, boolean EnX );
extern void TraiteEnglobement ( int fen );
extern void ModFenetre ( int fen );
extern void RazVue ( int fen );
extern boolean ModifVue ( int fen, int *page, PtrAbstractBox Pv );

#else /* __STDC__ */

extern C_points *PointsControle(/* PtrTextBuffer buffer, int nb */);
extern void DimTexte (/* PtrTextBuffer adbuff, int nbcar, ptrfont font, int *large, int *carbl */);
extern void EvalSymb (/* PtrAbstractBox adpave, int *large, int *haut */);
extern void EvalGraph (/* PtrAbstractBox adpave, int *large, int *haut */);
extern void EvalPolyLine (/* PtrAbstractBox adpave, int *large, int *haut */);
extern void EvalText (/* PtrAbstractBox adpave, int *large, int *haut, int *carbl */);
extern void EvalComp (/* PtrAbstractBox adpave, int fen, int *large, int *haut */);
extern PtrLine DesLigne (/* PtrBox ibox */);
extern void MajBox (/* PtrBox ibox, PtrLine adligne, int dcar, int dbl, int dx, int just, int dy, int fen, boolean BCOUP */);
extern void DispBoite (/* PtrAbstractBox adpave, boolean Recree, int fen */);
extern void DiffereEnglobement (/* PtrBox ibox, boolean EnX */);
extern void TraiteEnglobement (/* int fen */);
extern void ModFenetre (/* int fen */);
extern void RazVue (/* int fen */);
extern boolean ModifVue (/* int fen, int *page, PtrAbstractBox Pv */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
