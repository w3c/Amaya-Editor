#ifndef WRITE_TYP_F_H
#define WRITE_TYP_F_H

#ifdef __STDC__
extern void                wrshort (int n) ;
extern void                wrsignshort (int n) ;
extern void                wrnom (Name N) ;
extern void                wrnomproc (Name N) ;
extern void                wrptrregle (PtrRegleTypo b) ;
extern void                wrptrfonction (PtrTypoFunction b) ;
extern void                wrTypeRegleTypo (TypeFunct T) ;
extern void                wrlistfunction (PtrTypoFunction pFunc) ;
extern void                wrbool (boolean b) ;
extern void                wrptrbloctypo (PtrRegleTypo b) ;
extern void                wrptrattrtypo (PtrRTypoAttribut b) ;
extern void                wrTypeCondTypo (TypeCondTypo T) ;
extern void                WriteReglesTypo (PtrRegleTypo pR) ;
extern void                WriteReglesAttr (int att, PtrRTypoAttribut pA, PtrSSchema pSchStr) ;
extern void                WrSchTyp (Name fname, PtrSchTypo pSchTypo, PtrSSchema pSchStr) ;
#else  /* __STDC__ */
extern void                wrshort () ;
extern void                wrsignshort () ;
extern void                wrnom () ;
extern void                wrnomproc () ;
extern void                wrptrregle () ;
extern void                wrptrfonction () ;
extern void                wrTypeRegleTypo () ;
extern void                wrlistfunction () ;
extern void                wrbool () ;
extern void                wrptrbloctypo () ;
extern void                wrptrattrtypo () ;
extern void                wrTypeCondTypo () ;
extern void                WriteReglesTypo () ;
extern void                WriteReglesAttr () ;
extern void                WrSchTyp () ;
#endif /* __STDC__ */

#endif /* WRITE_TYP_F_H */
