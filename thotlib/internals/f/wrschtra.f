
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void wrshort ( int n );
extern void wrsignshort ( int n );
extern void wrnom ( Name n );
extern void wrbool ( boolean b );
extern void wrTypeCondTrad ( TransCondition T );
extern void wrTypeRegleTrad ( TRuleType T );
extern void wrTrPosition ( TOrder P );
extern void wrTypeCree ( CreatedObject T );
extern void wrRelatPos ( TRelatPosition P );
extern void wrTrCptTypeOp ( TCounterOp T );
extern void wrVarTradType ( TranslVarType T );
extern void wrptrregle ( PtrTRule p );
extern void WriteRegles ( PtrTRule pR );
extern void wrptrbloc ( PtrTRuleBlock b );
extern void WriteBlocs ( PtrTRuleBlock pB, PtrSSchema pSchStr );
extern void WriteReglesAttr ( int att, PtrSSchema pSchStr, PtrTSchema pSchTra );
extern void WriteReglesPres ( int pres, PtrSSchema pSchStr, PtrTSchema pSchTra );
extern boolean WrSchTrad ( Name fname, PtrTSchema pSchTra, PtrSSchema pSchStr );

#else /* __STDC__ */

extern void wrshort (/* int n */);
extern void wrsignshort (/* int n */);
extern void wrnom (/* Name n */);
extern void wrbool (/* boolean b */);
extern void wrTypeCondTrad (/* TransCondition T */);
extern void wrTypeRegleTrad (/* TRuleType T */);
extern void wrTrPosition (/* TOrder P */);
extern void wrTypeCree (/* CreatedObject T */);
extern void wrRelatPos (/* TRelatPosition P */);
extern void wrTrCptTypeOp (/* TCounterOp T */);
extern void wrVarTradType (/* TranslVarType T */);
extern void wrptrregle (/* PtrTRule p */);
extern void WriteRegles (/* PtrTRule pR */);
extern void wrptrbloc (/* PtrTRuleBlock b */);
extern void WriteBlocs (/* PtrTRuleBlock pB, PtrSSchema pSchStr */);
extern void WriteReglesAttr (/* int att, PtrSSchema pSchStr, PtrTSchema pSchTra */);
extern void WriteReglesPres (/* int pres, PtrSSchema pSchStr, PtrTSchema pSchTra */);
extern boolean WrSchTrad (/* Name fname, PtrTSchema pSchTra, PtrSSchema pSchStr */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
