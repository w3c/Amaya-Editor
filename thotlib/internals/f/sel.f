
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void AnnuleMrq ( int fen );
extern void SetSelect ( int fen, boolean allume );
extern void ResetSelect ( int fen );
extern void RazSelect ( void );
extern PtrTextBuffer DestBuff(PtrTextBuffer adbuff, int fen);
extern void MajMrq ( int fen, int dx, int dblanc, int dcar );
extern void ReevalMrq ( ViewSelection *marque );
extern void PoseSelect ( int fen, PtrAbstractBox Pav, int c1, int cN, boolean Debut, boolean Fin, boolean Unique );
extern void PoseMrq ( int fen, int x, int y, int bouton );
extern int PavPosFen ( PtrAbstractBox pav, int fen, int axe );
extern boolean PaveAffiche ( PtrAbstractBox pav, int fen );

#else /* __STDC__ */

extern void AnnuleMrq (/* int fen */);
extern void SetSelect (/* int fen, boolean allume */);
extern void ResetSelect (/* int fen */);
extern void RazSelect (/* void */);
extern PtrTextBuffer DestBuff(/*PtrTextBuffer adbuff, int fen*/);
extern void MajMrq (/* int fen, int dx, int dblanc, int dcar */);
extern void ReevalMrq (/* ViewSelection *marque */);
extern void PoseSelect (/* int fen, PtrAbstractBox Pav, int c1, int cN, boolean Debut, boolean Fin, boolean Unique */);
extern void PoseMrq (/* int fen, int x, int y, int bouton */);
extern int PavPosFen (/* PtrAbstractBox pav, int fen, int axe */);
extern boolean PaveAffiche (/* PtrAbstractBox pav, int fen */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
