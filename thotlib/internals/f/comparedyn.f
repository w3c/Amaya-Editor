
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern TyRelation GDRQuelleRelation(TyComp *Tyc, int i, int j);
extern void GDRRemonterCouplage(TyComp *Tyc);
extern void GDRCoupler(TyComp *Tyc, int ISource, int IDest);
extern boolean GDRDeCouplerSource(TyComp *Tyc, int ISource);
extern void GDRSupprimerCouplage(TyComp *Tyc);
extern int * GDRCalculDecalages ( char * Modele );
extern TyRelation GDRRechercheFacteur (TyComp *Tyc, char * Chaine, char * Modele  );
extern TyRelation GDRRechercheEquivalence (TyComp *Tyc );
extern TyRelation GDRRechercheMassif(TyComp *Tyc, char *Dest, char *Source);
extern TyRelation GDRCompare ( TypeCouplage *SC, TypeCouplage *DC, Tyff Typeff);

#else /* __STDC__ */

extern TyRelation GDRQuelleRelation(/* TyComp *Tyc, int i, int j */);
extern void GDRRemonterCouplage(/* TyComp *Tyc */);
extern void GDRCoupler(/* TyComp *Tyc, int ISource, int IDest */);
extern boolean GDRDeCouplerSource(/* TyComp *Tyc, int ISource */);
extern void GDRSupprimerCouplage(/* TyComp *Tyc */);
extern int * GDRCalculDecalages (/* char * Modele */);
extern TyRelation GDRRechercheFacteur (/* TyComp *Tyc, char * Chaine, char * Modele */);
extern TyRelation GDRRechercheEquivalence (/* TyComp *Tyc */);
extern TyRelation GDRRechercheMassif(/* TyComp *Tyc, char *Dest, char *Source */);
extern TyRelation GDRCompare (/* TypeCouplage *SC, TypeCouplage *DC, Tyff Typeff */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
