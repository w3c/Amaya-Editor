
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean Parent ( PtrBox ibox, PtrBox cebox );
extern boolean XEnAbsolu ( PtrBox ibox );
extern boolean YEnAbsolu ( PtrBox ibox );
extern void XYEnAbsolu ( PtrBox ibox, boolean *EnHorizontal, boolean *EnVertical );
extern void AjusteTrace ( PtrAbstractBox adpave, boolean InversionHoriz, boolean InversionVert, boolean Saisie);
extern void HorizInverse(PtrBox ibox, OpRelation op);
extern void VertInverse(PtrBox ibox, OpRelation op);
extern void ChangeLgContenu ( PtrBox ibox, PtrBox org, int large, int dbl, int fen );
extern void ChangeHtContenu ( PtrBox ibox, PtrBox org, int haut, int fen );
extern void ChangeLargeur ( PtrBox ibox, PtrBox org, PtrBox prec, int delta, int dbl, int fen );
extern void ChangeHauteur ( PtrBox ibox, PtrBox org, PtrBox prec, int delta, int fen );
extern void ChngBElast ( PtrBox ibox, PtrBox org, OpRelation op, int delta, int fen, boolean EnX );
extern void DepXContenu ( PtrBox ibox, int delta, int fen );
extern void DepYContenu ( PtrBox ibox, int delta, int fen );
extern void DepAxe ( PtrBox ibox, PtrBox prec, int delta, int fen );
extern void DepBase ( PtrBox ibox, PtrBox prec, int delta, int fen );
extern void ModLarg ( PtrBox ibox, PtrBox org, PtrBox prec, int delta, int dbl, int fen );
extern void ModHaut ( PtrBox ibox, PtrBox org, PtrBox prec, int delta, int fen );
extern void DepOrgX ( PtrBox ibox, PtrBox prec, int delta, int fen );
extern void DepOrgY ( PtrBox ibox, PtrBox prec, int delta, int fen );
extern void Englobx ( PtrAbstractBox adpave, PtrBox org, int fen );
extern void Engloby ( PtrAbstractBox adpave, PtrBox org, int fen );

#else /* __STDC__ */

extern boolean Parent (/* PtrBox ibox, PtrBox cebox */);
extern boolean XEnAbsolu (/* PtrBox ibox */);
extern boolean YEnAbsolu (/* PtrBox ibox */);
extern void XYEnAbsolu (/* PtrBox ibox, boolean *EnHorizontal, boolean *EnVertical */);
extern void HorizInverse(/*PtrBox ibox, OpRelation op*/);
extern void VertInverse(/*PtrBox ibox, OpRelation op*/);
extern void AjusteTrace (/* PtrAbstractBox adpave, boolean InversionHoriz, boolean InversionVert, boolean Saisie */);
extern void ChangeLgContenu (/* PtrBox ibox, PtrBox org, int large, int dbl, int fen */);
extern void ChangeHtContenu (/* PtrBox ibox, PtrBox org, int haut, int fen */);
extern void ChangeLargeur (/* PtrBox ibox, PtrBox org, PtrBox prec, int delta, int dbl, int fen */);
extern void ChangeHauteur (/* PtrBox ibox, PtrBox org, PtrBox prec, int delta, int fen */);
extern void ChngBElast (/* PtrBox ibox, PtrBox org, OpRelation op, int delta, int fen, boolean EnX */);
extern void DepXContenu (/* PtrBox ibox, int delta, int fen */);
extern void DepYContenu (/* PtrBox ibox, int delta, int fen */);
extern void DepAxe (/* PtrBox ibox, PtrBox prec, int delta, int fen */);
extern void DepBase (/* PtrBox ibox, PtrBox prec, int delta, int fen */);
extern void ModLarg (/* PtrBox ibox, PtrBox org, PtrBox prec, int delta, int dbl, int fen */);
extern void ModHaut (/* PtrBox ibox, PtrBox org, PtrBox prec, int delta, int fen */);
extern void DepOrgX (/* PtrBox ibox, PtrBox prec, int delta, int fen */);
extern void DepOrgY (/* PtrBox ibox, PtrBox prec, int delta, int fen */);
extern void Englobx (/* PtrAbstractBox adpave, PtrBox org, int fen */);
extern void Engloby (/* PtrAbstractBox adpave, PtrBox org, int fen */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
