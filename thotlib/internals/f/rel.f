
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void Positionner ( AbPosition regle, PtrBox ibox, int fen, boolean EnX );
extern boolean Dimensionner ( PtrAbstractBox adpave, int fen, boolean EnX );
extern void PlacerAxe ( AbPosition regle, PtrBox ibox, int fen, boolean EnX );
extern PtrBox BoiteHInclus ( PtrBox ibox, PtrBox prec );
extern PtrBox BoiteVInclus ( PtrBox ibox, PtrBox prec );
extern void RazHorsEnglobe ( PtrBox abox );
extern void RazPosition ( PtrBox debox, boolean EnX );
extern void RazAxe ( PtrBox debox, boolean EnX );
extern void RazLiens ( PtrBox abox );
extern void RazDim ( PtrBox debox, boolean EnX, int fen );

#else /* __STDC__ */

extern void Positionner (/* AbPosition regle, PtrBox ibox, int fen, boolean EnX */);
extern boolean Dimensionner (/* PtrAbstractBox adpave, int fen, boolean EnX */);
extern void PlacerAxe (/* AbPosition regle, PtrBox ibox, int fen, boolean EnX */);
extern PtrBox BoiteHInclus (/* PtrBox ibox, PtrBox prec */);
extern PtrBox BoiteVInclus (/* PtrBox ibox, PtrBox prec */);
extern void RazHorsEnglobe (/* PtrBox abox */);
extern void RazPosition (/* PtrBox debox, boolean EnX */);
extern void RazAxe (/* PtrBox debox, boolean EnX */);
extern void RazLiens (/* PtrBox abox */);
extern void RazDim (/* PtrBox debox, boolean EnX, int fen */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
