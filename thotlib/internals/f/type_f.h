#ifndef TYP_F_H
#define TYP_F_H

#ifdef __STDC__
extern void                initgener (void) ;
extern void                NewFunction (TypeFunct TypeF) ;
extern PtrTypoFunction     ChercheFonction (Name NomFonct) ;
extern void                NewModele (Name NomModele) ;
extern PtrModeleCompo      ChercheModele (Name NomModele) ;
extern void                nouvregle (void) ;
extern void                initAttr (PtrRTypoAttribut pRAtt, int att) ;
extern void                NewAttr (int att) ;
extern void                copierregle (PtrRegleTypo pRegle) ;
extern PtrRegleTypo        ChercheRegle (TypeFunct TypeF, PtrRegleTypo PremRegle) ;
extern void                add_condition (PtrRegleTypo pRegle) ;
extern void                copy_condition (void) ;
extern void                NewRule () ;
extern void                InsererRegle (PtrRegleTypo pReg) ;
extern void                InsererModele (PtrModeleCompo pMod) ;
extern void                copy_generate (Name n, indLine * wi, indLine * wl) ;
extern void                ProcessToken (indLine wi, indLine wl, SyntacticCode c, SyntacticCode r, int nb, SyntRuleNum pr) ;
#else
extern void                initgener () ;
extern void                NewFunction () ;
extern PtrTypoFunction     ChercheFonction () ;
extern void                NewModele () ;
extern PtrModeleCompo      ChercheModele () ;
extern void                nouvregle () ;
extern void                initAttr () ;
extern void                NewAttr () ;
extern void                copierregle () ;
extern PtrRegleTypo        ChercheRegle () ;
extern void                add_condition () ;
extern void                copy_condition () ;
extern void                NewRule () ;
extern void                InsererRegle () ;
extern void                InsererModele () ;
extern void                copy_generate () ;
extern void                ProcessToken () ;
#endif /* __STDC__ */

#endif /* TYP_F_H */
