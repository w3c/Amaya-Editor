
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int uniqueident ( void );
extern void wrshort ( int n );
extern void wrsignshort ( int n );
extern void wrNom ( Name n );
extern void wrBool ( boolean b );
extern void wrTypeAttr ( AttribType T );
extern void wrConstructeur ( RConstruct C );
extern void wrTypeBase ( BasicType T );
extern boolean WrSchStruct ( Name fname, PtrSSchema pSchStr, int Code );

#else /* __STDC__ */

extern int uniqueident (/* void */);
extern void wrshort (/* int n */);
extern void wrsignshort (/* int n */);
extern void wrNom (/* Name n */);
extern void wrBool (/* boolean b */);
extern void wrTypeAttr (/* AttribType T */);
extern void wrConstructeur (/* RConstruct C */);
extern void wrTypeBase (/* BasicType T */);
extern boolean WrSchStruct (/* Name fname, PtrSSchema pSchStr, int Code */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
