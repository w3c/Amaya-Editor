
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean SeparateurMot ( char car );
extern void Minuscule ( char *word );
extern int CutLastWord ( ptrfont font, Language langue, PtrTextBuffer *buffer, int *rank, int *width, boolean *hyphen );
extern boolean Hyphenable ( PtrBox ibox );

#else /* __STDC__ */

extern boolean SeparateurMot (/* char car */);
extern void Minuscule (/* char *word */);
extern int CutLastWord (/* ptrfont font, Language langue, PtrTextBuffer *buffer, int *rank, int *width, boolean *hyphen */);
extern boolean Hyphenable (/* PtrBox ibox */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
