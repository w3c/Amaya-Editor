
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void DebInsert ( void );
extern void InsertChar (int frame, unsigned char car, int clavier );
extern void FinInsertText ( void );
extern void FinInsertParagraph ( PtrAbstractBox adpave, int fen );
extern void ClearRegistre ( PtrTextBuffer registre );
extern void EditionEtSpecials ( int numero, boolean IsCar );
extern void CollerXBuffer ( unsigned char *Xbuffer, int nbytes );
#else /* __STDC__ */

extern void DebInsert (/* void */);
extern void InsertChar (/*int frame, unsigned char car, int clavier */);
extern void FinInsertText (/* void */);
extern void FinInsertParagraph (/* PtrAbstractBox adpave, int fen */);
extern void ClearRegistre (/* PtrTextBuffer registre */);
extern void EditionEtSpecials (/* int numero, boolean IsCar */);
extern void CollerXBuffer (/* unsigned char *Xbuffer, int nbytes */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
