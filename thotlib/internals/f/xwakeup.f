
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int main ( int argc, char **argv );
extern int Usage ( void );
extern int Error ( char *s );

#else /* __STDC__ */

extern int main (/* int argc, char **argv */);
extern int Usage (/* void */);
extern int Error (/* char *s */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
