
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int main ( int argc, char *argv[] );
extern int writeblanks ( char *start, char *end );
extern int test1 ( char *buf );
extern int convert1 ( char *buf, FILE *out );

#else /* __STDC__ */

extern int main (/* int argc, char *argv[] */);
extern int writeblanks (/* char *start, char *end */);
extern int test1 (/* char *buf */);
extern int convert1 (/* char *buf, FILE *out */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
