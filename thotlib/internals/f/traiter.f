
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void init_code ( FILE *falpha );
extern boolean alphabet ( void );
extern void pretraitement ( PtrDico dico );
extern int charger ( FILE *fdico, PtrDico dict );
extern void vider ( PtrDico dict, char *nomdict );
extern void creer ( PtrDico dict, char *nomdict );
extern int memdico ( void );
extern int main ( int argc, char *argv[] );

#else /* __STDC__ */

extern void init_code (/* FILE *falpha */);
extern boolean alphabet (/* void */);
extern void pretraitement (/* PtrDico dico */);
extern int charger (/* FILE *fdico, PtrDico dict */);
extern void vider (/* PtrDico dict, char *nomdict */);
extern void creer (/* PtrDico dict, char *nomdict */);
extern int memdico (/* void */);
extern int main (/* int argc, char *argv[] */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
