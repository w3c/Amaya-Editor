
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void testfunction2 ( void_ptr arg1, void_func arg2 );
extern int testfunction3 ( const int arg1 );
extern int new_change ( void );
extern int test_char_sign ( int arg );
extern int main ( int argc, char ** argv );

#else /* __STDC__ */

extern void testfunction2 (/* void_ptr arg1, void_func arg2 */);
extern int testfunction3 (/* const int arg1 */);
extern int new_change (/* void */);
extern int test_char_sign (/* int arg */);
extern int main (/* int argc, char ** argv */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
