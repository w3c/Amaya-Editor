
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void AfPage (FILE * fout);
extern void WChaine(ThotWindow w, char *chaine, int x, int y, ptrfont font, ThotGC GClocal);
extern void FontOrig ( ptrfont font, char firstchar, int *pX, int *pY );
extern void AfCar ( char car, int fen, int x, int y, ptrfont font, int RO, int active, int fg );
extern int AfChaine(char *buff, int i, int lg, int fen, int x, int y, ptrfont font, int lgboite, int bl, int hyphen, int debutbloc, int RO, int active, int fg);
extern void AfSoul ( int fen, int x, int y, ptrfont font, int type, int epais, int lg, int RO, int active, int fg );
extern void AfPoints ( int fen, int x, int y, int lgboite, int RO, int active, int fg );
extern void AfRadical ( int fen, int epais, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg );
extern void AfIntegrale ( int fen, int epais, int x, int y, int l, int h, int type, ptrfont font, int RO, int active, int fg );
extern void AfSigma ( int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg );
extern void AfPi ( int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg );
extern void AfIntersection ( int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg );
extern void AfUnion ( int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg );
extern void AfFleche ( int fen, int epais, int style, int x, int y, int l, int h, int orientation, int RO, int active, int fg );
extern void AfCrochet ( int fen, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg );
extern void AfParenthese ( int fen, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg );
extern void AfAccolade ( int fen, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg );
extern void AfRectangle ( int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif );
extern void AfLosange ( int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif );
extern void AfBrisees ( int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg , int fleche);
extern void AfPolygone ( int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int motif );
extern void AfCourbe(int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int fleche, C_points *controls);
extern void AfSpline(int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int motif, C_points *controls);
extern void AfOvale ( int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif );
extern void AfEllipse ( int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif );
extern void AfLigne ( int fen, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int active, int fg );
extern void AfTrait ( int fen, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int active, int fg );
extern void AfOblique ( int fen, int epais, int style, int x, int y, int l, int h, int sens, int RO, int active, int fg );
extern void AfCoin ( int fen, int epais, int style, int x, int y, int l, int h, int coin, int RO, int active, int fg );
extern void AfCadreRect ( int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif );
extern void AfCadreEllipse ( int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif );
extern void PSPageInfo( int pagenum, int width, int height );
extern void psBoundingBox(int fen, int width, int height);
extern void Clear ( int fen, int larg, int haut, int x, int y );
extern void Invideo ( int fen, int larg, int haut, int x, int y );
extern void Scroll ( int fen, int larg, int haut, int xd, int yd, int xf, int yf );
extern int FinDeChaine ( char *chaine, char *suffix );
extern void XFlushOutput ( int fen );
extern void Trame ( int fen, int x, int y, int large, int haut, ThotWindow w, int RO, int active, int fg, int bg, int motif );

#else /* __STDC__ */

extern void AfPage (/*FILE * fout */);
extern void WChaine(/*ThotWindow w, char *chaine, int x, int y, ptrfont font, ThotGC GClocal*/);
extern void FontOrig (/* ptrfont font, char firstchar, int *pX, int *pY */);
extern void AfCar (/* char car, int fen, int x, int y, ptrfont font, int RO, int active, int fg */);
extern int AfChaine(/* char *buff, int i, int lg, int fen, int x, int y, ptrfont font, int lgboite, int bl, int hyphen, int debutbloc, int RO, int active, int fg */);
extern void AfSoul (/* int fen, int x, int y, ptrfont font, int type, int epais, int lg, int RO, int active, int fg */);
extern void AfPoints (/* int fen, int x, int y, int lgboite, int RO, int active, int fg */);
extern void AfRadical (/* int fen, int epais, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg */);
extern void AfIntegrale (/* int fen, int epais, int x, int y, int l, int h, int type, ptrfont font, int RO, int active, int fg */);
extern void AfSigma (/* int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg */);
extern void AfPi (/* int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg */);
extern void AfIntersection (/* int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg */);
extern void AfUnion (/* int fen, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg */);
extern void AfFleche (/* int fen, int epais, int style, int x, int y, int l, int h, int orientation, int RO, int active, int fg */);
extern void AfCrochet (/* int fen, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg */);
extern void AfParenthese (/* int fen, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg */);
extern void AfAccolade (/* int fen, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg */);
extern void AfRectangle (/* int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif */);
extern void AfLosange (/* int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif */);
extern void AfBrisees (/* int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg , int fleche*/);
extern void AfPolygone (/* int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int motif */);
extern void AfCourbe(/*int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int fleche, C_points *controls*/);
extern void AfSpline(/*int fen, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int motif, C_points *controls*/);
extern void AfOvale (/* int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif */);
extern void AfEllipse (/* int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif */);
extern void AfLigne (/* int fen, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int active, int fg */);
extern void AfTrait (/* int fen, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int active, int fg */);
extern void AfOblique (/* int fen, int epais, int style, int x, int y, int l, int h, int sens, int RO, int active, int fg */);
extern void AfCoin (/* int fen, int epais, int style, int x, int y, int l, int h, int coin, int RO, int active, int fg */);
extern void AfCadreRect (/* int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif */);
extern void AfCadreEllipse (/* int fen, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif */);
extern void PSPageInfo(/* int pagenum, int width, int height*/ );
extern void psBoundingBox(/* int fen, int width, int height */);
extern void Clear (/* int fen, int larg, int haut, int x, int y */);
extern void Invideo (/* int fen, int larg, int haut, int x, int y */);
extern void Scroll (/* int fen, int larg, int haut, int xd, int yd, int xf, int yf */);
extern int FinDeChaine (/* char *chaine, char *suffix */);
extern void XFlushOutput (/* int fen */);
extern void Trame (/* int fen, int x, int y, int large, int haut, ThotWindow w, int RO, int active, int fg, int bg, int motif */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
