
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CommandArgs ( int argc, char **argv );
extern int CPtGet ( int origine, int numero, char *buff, int maxlen );
extern int CArgGetint ( int *res );

#else /* __STDC__ */

extern void CommandArgs (/* int argc, char **argv */);
extern int CPtGet (/* int origine, int numero, char *buff, int maxlen */);
extern int CArgGetint (/* int *res */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
