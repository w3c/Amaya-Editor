
/* -- Copyright (c) 1990 - 1996 Inria/CNRS  All rights reserved. -- */

extern void InitTableSchTrad ();
extern void ClearTableSchTrad();

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrTSchema SchemaTrad ( PtrSSchema SS );
extern PtrTSchema LdSchTrad ( Name fname, PtrSSchema SS );

#else /* __STDC__ */

extern PtrTSchema SchemaTrad (/* PtrSSchema SS */);
extern PtrTSchema LdSchTrad (/* Name fname, PtrSSchema SS */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
