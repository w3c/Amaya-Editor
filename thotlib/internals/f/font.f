
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int VolumCar ( int volpixel );
extern int NumberOfFonts ( void );
extern int PointToPixel ( int valeur);
extern int PixelToPoint ( int valeur);
extern int CarWidth ( unsigned char c, ptrfont font );
extern int CarHeight ( unsigned char c, ptrfont font );
extern int CarAscent ( unsigned char c, ptrfont font );
extern int FontAscent ( ptrfont font );
extern int FontHeight ( ptrfont font );
extern int PixelValue(int val, TypeUnit unit, PtrAbstractBox adpave);
extern int LogicalValue(int val, TypeUnit unit, PtrAbstractBox adpave);
extern int FontBase ( ptrfont font );
extern int PseudoTaille ( int taille );
extern int TailleEnPoints ( int taille );
extern ptrfont LoadFont(char name[100]);
extern void NomFonte (char alphabet, char police, int evidence, int taille, TypeUnit unit, char r_nom[10], char r_nomX[100]);
extern ptrfont LireFonte (char alphabet, char police, int evidence, int taille, TypeUnit unit);
extern ptrfont ChargeFonte (char alphabet, char police, int evidence, int taille, TypeUnit unit, int fen);
extern void InitFont ( char *name);
extern void LibFont (int fen);

#else /* __STDC__ */

extern int VolumCar (/* int volpixel */);
extern int NumberOfFonts (/* void */);
extern int PointToPixel (/* int valeur */);
extern int PixelToPoint (/* int valeur */);
extern int CarWidth (/* unsigned char c, ptrfont font */);
extern int CarHeight (/* unsigned char c, ptrfont font */);
extern int CarAscent (/* unsigned char c, ptrfont font */);
extern int FontAscent (/* ptrfont font */);
extern int FontHeight (/* ptrfont font */);
extern int PixelValue(/*int val, TypeUnit unit, PtrAbstractBox adpave*/);
extern int LogicalValue(/*int val, TypeUnit unit, PtrAbstractBox adpave*/);
extern int FontBase (/* ptrfont font */);
extern int PseudoTaille (/* int taille */);
extern int TailleEnPoints (/* int taille */);
extern ptrfont LoadFont(/*char name[100]*/);
extern void NomFonte (/*char alphabet, char police, int evidence, int taille, TypeUnit unit, char r_nom[10], char r_nomX[100]*/);
extern ptrfont LireFonte (/*char alphabet, char police, int evidence, int taille, TypeUnit unit*/);
extern ptrfont ChargeFonte (/* char alphabet, char police, int evidence, int taille, TypeUnit unit, int fen*/);
extern void InitFont (/*char *name*/);
extern void LibFont (/*int fen*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
