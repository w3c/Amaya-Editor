
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void GetXYOrg ( int fen, int *XOrg, int *YOrg );
extern void DefClip ( int fen, int xd, int yd, int xf, int yf );
extern void DefRegion ( int fen, int xd, int yd, int xf, int yf );
extern void TtaRefresh();
extern boolean AfDebFenetre ( int fen, int delta );
extern void AjouteACreer ( PtrBox *acreer, PtrBox ibox, int fen );
extern boolean AfFinFenetre ( int fen, int delta );
extern void AfficherVue ( int fen );

#else /* __STDC__ */

extern void GetXYOrg (/* int fen, int *XOrg, int *YOrg */);
extern void DefClip (/* int fen, int xd, int yd, int xf, int yf */);
extern void DefRegion (/* int fen, int xd, int yd, int xf, int yf */);
extern void TtaRefresh();
extern boolean AfDebFenetre (/* int fen, int delta */);
extern void AjouteACreer (/* PtrBox *acreer, PtrBox ibox, int fen */);
extern boolean AfFinFenetre (/* int fen, int delta */);
extern void AfficherVue (/* int fen */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
