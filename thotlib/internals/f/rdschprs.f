
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern AttrComparType rdComparAttr ( BinFile file );
extern PtrPSchema RdSchPres ( Name fname, PtrSSchema SS );

#else /* __STDC__ */

extern AttrComparType rdComparAttr (/* BinFile file */);
extern PtrPSchema RdSchPres (/* Name fname, PtrSSchema SS */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
