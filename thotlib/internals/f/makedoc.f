
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void FunctionDeclaration ( char *line );
extern void ParamDeclaration ( char *line );
extern void ProcessLine ( char *line );
extern int InitDocument ( void );
extern int main ( int argc, char **argv );

#else /* __STDC__ */

extern void FunctionDeclaration (/* char *line */);
extern void ParamDeclaration (/* char *line */);
extern void ProcessLine (/* char *line */);
extern int InitDocument (/* void */);
extern int main (/* int argc, char **argv */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
