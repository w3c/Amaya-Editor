
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern void DeleteSelection(int fen);
extern void DeleteChar(int fen);
extern void SelectBefore(int fen);
extern void SelectAfter(int fen);
extern void SelectParent(int fen);
extern void SelectChild(int fen);
extern void PreviousChar(int fen);
extern void NextChar(int fen);
extern void PreviousLine(int fen);
extern void NextLine(int fen);
extern void BeginningOfLine(int fen);
extern void EndOfLine(int fen);

extern char *EquivAction ( char *name );
#ifdef WWW_XWINDOWS
extern void XCharTranslation ( XEvent *event );
extern XtTranslations InitTranslations (char *appliname);
#endif /* WWW_XWINDOWS */
extern void MaTranslation(int fen, unsigned char *chaine, unsigned int nb,
                          int mask, int key);

#else /* __STDC__ */
extern void DeleteSelection(/*int fen*/);
extern void DeleteChar(/*int fen*/);
extern void SelectBefore(/*int fen*/);
extern void SelectAfter(/*int fen*/);
extern void SelectParent(/*int fen*/);
extern void SelectChild(/*int fen*/);
extern void PreviousChar(/*int fen*/);
extern void NextChar(/*int fen*/);
extern void PreviousLine(/*int fen*/);
extern void NextLine(/*int fen*/);
extern void BeginningOfLine(/*int fen*/);
extern void EndOfLine(/*int fen*/);

extern char *EquivAction (/* char *name */);
#ifdef WWW_XWINDOWS
extern void XCharTranslation ( /* XEvent *event */ );
extern XtTranslations InitTranslations (/* char *appliname */);
#endif /* WWW_XWINDOWS */
extern void MaTranslation(/* int fen, unsigned char *chaine, unsigned int nb,
                          int mask, int key */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
