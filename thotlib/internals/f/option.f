
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitVisu ( int fen, int Visibilite, int Zoom );
extern void GetVisu ( int fen, int *Visibilite, int *Zoom );
extern void ModVisu ( int fen, int Visibilite, int Zoom );

#else /* __STDC__ */

extern void InitVisu (/* int fen, int Visibilite, int Zoom */);
extern void GetVisu (/* int fen, int *Visibilite, int *Zoom */);
extern void ModVisu (/* int fen, int Visibilite, int Zoom */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
