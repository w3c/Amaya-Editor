#ifndef WRITE_TRA_F_H
#define WRITE_TRA_F_H

#ifdef __STDC__
extern void                wrshort (int n) ; 
extern void                wrsignshort (int n) ;
extern void                wrnom (Name n) ;
extern void                wrbool (boolean b) ;
extern void                wrTypeCondTrad (TransCondition T) ;
extern void                wrRelatNivAsc (RelatNAscend R) ;
extern void                wrTypeRegleTrad (TRuleType T) ;
extern void                wrTrPosition (TOrder P) ;
extern void                wrTypeCree (CreatedObject T) ;
extern void                wrRelatPos (TRelatPosition P) ;
extern void                wrTrCptTypeOp (TCounterOp T) ;
extern void                wrVarTradType (TranslVarType T) ;
extern void                wrComptStyle (CounterStyle S) ;
extern void                wrptrregle (PtrTRule p) ;
extern void                WriteRegles (PtrTRule pR) ;
extern void                wrptrbloc (PtrTRuleBlock b) ;
extern void                WriteBlocs (PtrTRuleBlock pB, PtrSSchema pSchStr) ;
extern void                WriteReglesAttr (int att, PtrSSchema pSchStr, PtrTSchema pSchTra) ;
extern void                WriteReglesPres (int pres, PtrSSchema pSchStr, PtrTSchema pSchTra) ;
extern boolean             WrSchTrad (Name fname, PtrTSchema pSchTra, PtrSSchema pSchStr) ;
#else  /* __STDC__ */
extern void                wrshort () ; 
extern void                wrsignshort () ;
extern void                wrnom () ;
extern void                wrbool () ;
extern void                wrTypeCondTrad () ;
extern void                wrRelatNivAsc () ;
extern void                wrTypeRegleTrad () ;
extern void                wrTrPosition () ;
extern void                wrTypeCree () ;
extern void                wrRelatPos () ;
extern void                wrTrCptTypeOp () ;
extern void                wrVarTradType () ;
extern void                wrComptStyle () ;
extern void                wrptrregle () ;
extern void                WriteRegles () ;
extern void                wrptrbloc () ;
extern void                WriteBlocs () ;
extern void                WriteReglesAttr () ;
extern void                WriteReglesPres () ;
extern boolean             WrSchTrad () ;
#endif /* __STDC__ */

#endif /* WRITE_TRA_F_H */
