
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void erreurTraduc ( int err );
extern PtrTSchema RdSchTrad ( Name fname, PtrSSchema SS );
extern void LibereSchTrad ( PtrTSchema pSchT, PtrSSchema pSS );

#else /* __STDC__ */

extern void erreurTraduc (/* int err */);
extern PtrTSchema RdSchTrad (/* Name fname, PtrSSchema SS */);
extern void LibereSchTrad (/* PtrTSchema pSchT, PtrSSchema pSS */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
