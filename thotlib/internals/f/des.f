
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrAbstractBox Pave_Suivant ( PtrAbstractBox adpave );
extern PtrBox DesBoiteTerm ( int fen, int x, int y );
extern void DesBPosition ( int fen, int xm, int ym );
extern void DesBDimension ( int fen, int xm, int ym );
extern void ModeCreation ( PtrBox ibox, int fen );
extern void DesCaractere ( PtrBox ibox, PtrTextBuffer *adbuff, int *x, int *icar, int *nbcar, int *nbbl );
extern boolean APPgraphicModify(PtrElement pEl, int val, int fen, boolean pre);

#else /* __STDC__ */

extern PtrAbstractBox Pave_Suivant (/* PtrAbstractBox adpave */);
extern PtrBox DesBoiteTerm (/* int fen, int x, int y */);
extern void DesBPosition (/* int fen, int xm, int ym */);
extern void DesBDimension (/* int fen, int xm, int ym */);
extern void ModeCreation (/* PtrBox ibox, int fen */);
extern void DesCaractere (/* PtrBox ibox, PtrTextBuffer *adbuff, int *x, int *icar, int *nbcar, int *nbbl */);
extern boolean APPgraphicModify(/*PtrElement pEl, int val, int fen, boolean pre*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
