
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean TtaCheckDirectory ( char *aDirectory );
extern boolean TtaCheckPath ( PathBuffer path );
extern int SearchFile ( char *nomfichier, int recherche, char *nomcomplet );

#else /* __STDC__ */

extern boolean TtaCheckDirectory (/* char *aDirectory */);
extern boolean TtaCheckPath (/* PathBuffer path */);
extern int SearchFile (/* char *nomfichier, int recherche, char *nomcomplet */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
