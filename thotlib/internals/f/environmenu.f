
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern void InitPathDoc( void );
extern void InitSelectNatures( void );
extern void InitMenusThot ( void );
extern void RetMenuConfirmDoc ( int ref, int val );
extern void RetMenuDirSchemas ( char *Text );
extern void RetMenuDirDocuments ( char *Text );
extern void RetMenuOuvrirDocument ( int ref, int val, char *txt );
extern void RetMenuCreeDocument ( int Ref, char *txt );
extern void RetMenuClasseImport ( int ref, char *txt );
extern void RetMenuFormatImport ( int ref, int val );
extern void RetEspaceModes(int ref, int val);

#else /* __STDC__ */
extern void InitPathDoc(/* void */);
extern void InitSelectNatures(/* void */);
extern void InitMenusThot (/* void */);
extern void RetMenuConfirmDoc (/* int ref, int val */);
extern void RetMenuDirSchemas (/* char *Text */);
extern void RetMenuDirDocuments (/* char *Text */);
extern void RetMenuOuvrirDocument (/* int ref, int val, char *txt */);
extern void RetMenuCreeDocument (/* int Ref, char *txt */);
extern void RetMenuClasseImport (/* int ref, char *txt */);
extern void RetMenuFormatImport (/* int ref, int val */);
extern void RetEspaceModes(/* int ref, int val */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
