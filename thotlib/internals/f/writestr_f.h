#ifndef WRITE_STR_F_H
#define WRITE_STR_F_H

#ifdef __STDC__
extern int                 uniqueident (void) ;
extern void                wrshort (int n) ;
extern void                wrsignshort (int n) ;
extern void                wrNom (Name n) ;
extern void                wrBool (boolean b) ;
extern void                wrTypeAttr (AttribType T) ;
extern void                wrConstructeur (RConstruct C) ;
extern void                wrTypeBase (BasicType T) ;
extern boolean             WrSchStruct (Name fname, PtrSSchema pSchStr, int Code) ;
#else  /* __STDC__ */
extern int                 uniqueident () ;
extern void                wrshort () ;
extern void                wrsignshort () ;
extern void                wrNom () ;
extern void                wrBool () ;
extern void                wrTypeAttr () ;
extern void                wrConstructeur () ;
extern void                wrTypeBase () ;
extern boolean             WrSchStruct () ;
#endif				/* __STDC__ */

#endif /* WRITE_STR_F_H */

