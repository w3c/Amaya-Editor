
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void Placer ( PtrAbstractBox adpave, int SeuilVisu, int fen, boolean EnX, boolean EnY );
extern void HautCoupure ( PtrAbstractBox Pv, boolean EnPt, int *ht, int *pos, int *nbcar );
extern boolean MarqueCoupure(PtrAbstractBox Pv, int *page);
extern void PosPavePt ( PtrAbstractBox Pv, int *xCoord, int *yCoord );
extern void DimPavePt ( PtrAbstractBox Pv, int *hauteur, int *largeur );
extern void MarqueAPlacer ( PtrAbstractBox adpave, boolean EnX, boolean EnY );

#else /* __STDC__ */

extern void Placer (/* PtrAbstractBox adpave, int SeuilVisu, int fen, boolean EnX, boolean EnY */);
extern void HautCoupure (/* PtrAbstractBox Pv, boolean EnPt, int *ht, int *pos, int *nbcar */);
extern boolean MarqueCoupure(/* PtrAbstractBox Pv, int *page */);
extern void PosPavePt (/* PtrAbstractBox Pv, int *xCoord, int *yCoord */);
extern void DimPavePt (/* PtrAbstractBox Pv, int *hauteur, int *largeur */);
extern void MarqueAPlacer (/* PtrAbstractBox adpave, boolean EnX, boolean EnY */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
