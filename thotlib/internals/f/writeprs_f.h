#ifndef WRITE_PRS_F_H
#define WRITE_PRS_F_H

#ifdef __STDC__
extern void                wrTypeRegle (PRuleType T) ;
extern void                wrTypeCondition (PresCondition T) ;
extern void                wrVarType (VariableType T) ;
extern void                wrTypeContenu (ContentType T) ;
extern void                WriteRules (PtrPRule pR) ;
extern boolean             WrSchPres (Name fname, PtrPSchema pSchPres, PtrSSchema pSchStr) ;
#else  /* __STDC__ */
extern void                wrTypeRegle () ;
extern void                wrTypeCondition () ;
extern void                wrVarType () ;
extern void                wrTypeContenu () ;
extern void                WriteRules () ;
extern boolean             WrSchPres () ;
#endif /* __STDC__ */

#endif /* WRITE_PRS_F_H */
