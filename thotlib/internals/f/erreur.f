
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void erreur ( int entier, int code );
extern void erreur_fatale ( char *s );

#else /* __STDC__ */

extern void erreur (/* int entier, int code */);
extern void erreur_fatale (/* char *s */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
