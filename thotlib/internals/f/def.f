
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void DefFenV ( int fen, int delta, int selection );
extern void DefFenH ( int fen, int delta, int selection );
extern void CadrerVueEnX ( int fen, int x, int large );
extern void CadrerVueEnY ( int fen, int y, int haut );
extern int ZoneImageAbs ( int fen, int *debut, int *fin, int *total );
extern void VolumeAffiche ( int fen, int *Xpos, int *Ypos, int *largeur, int *hauteur );
extern void MontrerBoite ( int fen, PtrBox ibox, int position, int pourcent );
extern boolean SansDef ( int fen, int selection );
extern void MontrerSelection ( int fen, boolean actif );
extern boolean PaveHorsFenetre(int fen, PtrAbstractBox pave);

#else /* __STDC__ */

extern void DefFenV (/* int fen, int delta, int selection */);
extern void DefFenH (/* int fen, int delta, int selection */);
extern void CadrerVueEnX (/* int fen, int x, int large */);
extern void CadrerVueEnY (/* int fen, int y, int haut */);
extern int ZoneImageAbs (/* int fen, int *debut, int *fin, int *total */);
extern void VolumeAffiche (/* int fen, int *Xpos, int *Ypos, int *largeur, int *hauteur */);
extern void MontrerBoite (/* int fen, PtrBox ibox, int position, int pourcent */);
extern boolean SansDef (/* int fen, int selection */);
extern void MontrerSelection (/* int fen, boolean actif */);
extern boolean PaveHorsFenetre(/*int fen, PtrAbstractBox pave*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
