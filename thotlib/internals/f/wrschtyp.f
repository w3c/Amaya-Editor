
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void wrshort ( int n );
extern void wrsignshort ( int n );
extern void wrnom ( Name N );
extern void wrnomproc ( Name N );
extern void wrptrregle ( PtrRegleTypo b );
extern void wrTypeRegleTypo ( TypeFunct T );
extern void wrlistfunction ( PtrTypoFunction pFunc );
extern void wrbool ( boolean b );
extern void wrptrbloctypo ( PtrRegleTypo b );
extern void wrptrattrtypo ( PtrRTypoAttribut b );
extern void wrTypeCondTypo ( TypeCondTypo T );
extern void WriteReglesTypo ( PtrRegleTypo pR );
extern void WriteReglesAttr ( int att, PtrRTypoAttribut pA, PtrSSchema pSchStr );
extern void WrSchTyp ( Name fname, PtrSchTypo pSchTypo, PtrSSchema pSchStr );

#else /* __STDC__ */

extern void wrshort (/* int n */);
extern void wrsignshort (/* int n */);
extern void wrnom (/* Name N */);
extern void wrnomproc (/* Name N */);
extern void wrptrregle (/* PtrRegleTypo b */);
extern void wrTypeRegleTypo (/* TypeFunct T */);
extern void wrlistfunction (/* PtrTypoFunction pFunc */);
extern void wrbool (/* boolean b */);
extern void wrptrbloctypo (/* PtrRegleTypo b */);
extern void wrptrattrtypo (/* PtrRTypoAttribut b */);
extern void wrTypeCondTypo (/* TypeCondTypo T */);
extern void WriteReglesTypo (/* PtrRegleTypo pR */);
extern void WriteReglesAttr (/* int att, PtrRTypoAttribut pA, PtrSSchema pSchStr */);
extern void WrSchTyp (/* Name fname, PtrSchTypo pSchTypo, PtrSSchema pSchStr */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
