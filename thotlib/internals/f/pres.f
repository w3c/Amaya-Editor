
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int AttrValue ( PtrAttribute pAttr );
extern int valintregle ( PtrPRule pRegle, PtrElement pEl, DocViewNumber nv, boolean *ok, TypeUnit *unit, PtrAttribute pAttr );
extern void GetBufConst ( PtrAbstractBox pPav );
extern void MajVolLibre ( PtrAbstractBox pPav, PtrDocument pDoc );
extern FunctionType RegleCree ( PtrDocument pDoc, PtrAbstractBox pCreateur, PtrAbstractBox pCree );
extern void ChSchemaPres ( PtrElement pEl, PtrPSchema *pSchP, int *NumEntree, PtrSSchema *pSchS );
extern void applCopie ( PtrDocument pDoc, PtrPRule pRegle, PtrAbstractBox pPav, boolean AvecDescCopie );
#ifdef __COLPAGE__
extern void Chaine ( PtrAbstractBox pPav, PtrElement pEl, DocViewNumber nv, int VueSch, PtrDocument pDoc, boolean EnAvant );
extern boolean Applique ( PtrPRule pRegle, PtrPSchema pSchP, PtrAbstractBox pPav, PtrDocument pDoc, PtrAttribute pAttr, boolean *pavedetruit );
#else /* __COLPAGE__ */
extern void Chaine ( PtrAbstractBox pPav, PtrElement pEl, DocViewNumber nv, PtrDocument pDoc );
extern boolean Applique ( PtrPRule pRegle, PtrPSchema pSchP, PtrAbstractBox pPav, PtrDocument pDoc, PtrAttribute pAttr);
#endif /* __COLPAGE__ */
extern PtrPRule ChReglePres ( PtrElement pEl, PRuleType TypeR, boolean *Nouveau, PtrDocument pDoc, int Vue );
extern void PavReaff ( PtrAbstractBox pPav, PtrDocument pDoc );
extern void NouvDimImage(PtrAbstractBox pPav);

#else /* __STDC__ */

extern int AttrValue (/* PtrAttribute pAttr */);
extern int valintregle (/* PtrPRule pRegle, PtrElement pEl, DocViewNumber nv, boolean *ok, TypeUnit *unit, PtrAttribute pAttr */);
extern void GetBufConst (/* PtrAbstractBox pPav */);
extern void MajVolLibre (/* PtrAbstractBox pPav, PtrDocument pDoc */);
extern FunctionType RegleCree (/* PtrDocument pDoc, PtrAbstractBox pCreateur, PtrAbstractBox pCree */);
extern void ChSchemaPres (/* PtrElement pEl, PtrPSchema *pSchP, int *NumEntree, PtrSSchema *pSchS */);
extern void applCopie (/* PtrDocument pDoc, PtrPRule pRegle, PtrAbstractBox pPav, boolean AvecDescCopie */);
#ifdef __COLPAGE__
extern void Chaine (/* PtrAbstractBox pPav, PtrElement pEl, DocViewNumber nv, int VueSch, PtrDocument pDoc, boolean EnAvant */);
extern boolean Applique (/* PtrPRule pRegle, PtrPSchema pSchP, PtrAbstractBox pPav, PtrDocument pDoc, PtrAttribute pAttr, boolean *pavedetruit */);
#else /* __COLPAGE__ */
extern void Chaine (/* PtrAbstractBox pPav, PtrElement pEl, DocViewNumber nv, PtrDocument pDoc */);
extern boolean Applique (/* PtrPRule pRegle, PtrPSchema pSchP, PtrAbstractBox pPav, PtrDocument pDoc, PtrAttribute pAttr */);
#endif /* __COLPAGE__ */
extern PtrPRule ChReglePres (/* PtrElement pEl, PRuleType TypeR, boolean *Nouveau, PtrDocument pDoc, int Vue */);
extern void PavReaff (/* PtrAbstractBox pPav, PtrDocument pDoc */);
extern void NouvDimImage(/* PtrAbstractBox pPav */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
