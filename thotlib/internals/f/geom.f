
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int PolyLineCreation ( int fen, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer );
extern void PolyLineModification ( int fen, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close );
extern int PolyLineExtension ( int fen, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close );
extern int GeomCreation ( int fen, int *x, int *y, int xr, int yr, int *large, int *haut, int xmin, int xmax, int ymin, int ymax, int PosX, int PosY, int DimX, int DimY );
extern void ChPosition ( int fen, int *x, int *y, int large, int haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym );
extern void ChDimension ( int fen, int x, int y, int *large, int *haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym );

#else /* __STDC__ */

extern int PolyLineCreation (/* int fen, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer */);
extern void PolyLineModification (/* int fen, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close */);
extern int PolyLineExtension (/* int fen, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close */);
extern int GeomCreation (/* int fen, int *x, int *y, int xr, int yr, int *large, int *haut, int xmin, int xmax, int ymin, int ymax, int PosX, int PosY, int DimX, int DimY */);
extern void ChPosition (/* int fen, int *x, int *y, int large, int haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym */);
extern void ChDimension (/* int fen, int x, int y, int *large, int *haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
