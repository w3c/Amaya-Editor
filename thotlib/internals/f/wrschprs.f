
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void WriteRules ( PtrPRule pR );
extern boolean WrSchPres ( Name fname, PtrPSchema pSchPres, PtrSSchema pSchStr );

#else /* __STDC__ */

extern void WriteRules (/* PtrPRule pR */);
extern boolean WrSchPres (/* Name fname, PtrPSchema pSchPres, PtrSSchema pSchStr */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
