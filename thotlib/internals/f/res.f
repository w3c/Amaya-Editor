
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void * GDRGetMem ( unsigned MemSize, char SourceFunc[] );
extern void GDRLibere ( void * Adresse );
extern void GDRLibereTreeSch ( pTreeSch Noeud );
extern void GDRLibereListeUnite ( pStrListeUnit p );
extern void GDRLibereTreeType ( pTreeTyp Noeud );
extern void GDRLibereDyck ( FlatTree *Noeud );
extern boolean GDRRetireMarqueDePage ( Element Elem, Document Docu );
extern void GDRFindSchType ( pTreeSch pRacine, pTreeSch *pType, int ElemRegle, int PereRegle, boolean recur );
extern void GDRFindTreeType ( pTreeTyp pRacine, pTreeTyp *pType, ElementType ElemType, int PereRegle, boolean recur );
extern void GDRCreeTreeType ( TreeTyp **pNewType, pTreeSch pSchemaType, TreeTyp *pPere, PtrSchemaResdyn pResdyn, boolean newTypeCanon );
extern void GDRTrieDyck ( pFlatTree Debut );
extern void GDRCreeDyck ( FlatTree **RlPrevious, pTreeTyp pType, FlatTree **DebutArbrePlat, Tyff Typeff, boolean source );
extern boolean GDRCreeRecTypes ( pTreeTyp PremiereRecursion, pTreeTyp TreeTypeSource, pTreeTyp *pNewType, pTreeTyp TreeTypePereDest, PtrSchemaResdyn pResdyn );
extern boolean GDRAddListeUnit ( pTreeTyp ElemTreeTyp, int TypeNum );
extern boolean GDRTransTypeUnites ( void );
extern boolean GDRDevRecursif ( Element Elem, pTreeTyp PereTreeTyp, PtrSchemaResdyn pResdyn );
extern void GDRCompatibilite ( TypeCouplage *TC, Tyff Typeff );
extern void GDRffectifPur ( TypeCouplage *TC );
extern void GDRMarqueffective ( pTreeTyp Typ, PtrSchemaResdyn pResdyn );
extern boolean GDRRechArbre ( ElementType SourElemType, ElementType DestElemType, TypeCouplage **SC, TypeCouplage **DC );
extern boolean GDRDevRecTypeDest ( pTreeTyp T, int MaxProf, int prof, PtrSchemaResdyn pResdyn );
extern int GDRCalcProfondeur ( pTreeTyp T, PtrSchemaResdyn pResdyn );
extern boolean GDRDevRecDest ( TypeCouplage *SC, TypeCouplage *DC );
extern void GDRRAZCouplage ( TypeCouplage *Debut );
extern TyRelation GDRCalcCouplage ( ElementType SourElemType, ElementType DestElemType, TypeCouplage **SC, TypeCouplage **DC );
extern TyRelation GDRCalcRelation ( TypeCouplage **SC, TypeCouplage **DC, Tyff Typeff, boolean Tri );
extern boolean GDRCollerRestructurer ( void );
extern boolean GDRInitContexte ( Document DocDest, Element ElDest, TyOperation TypeOp );
extern boolean GDRInitContexteElem ( Document DocDest, Element ElSource, Element ElDest, TyOperation TypeOp );
extern boolean GDRRegenerer ( StrListeEl *p );
extern void GDRFinRestruct ( boolean Resultat );
extern boolean GDRRepererSource ( Element ElemAColler, StrListeEl *El );
extern Element GDRCreeElem ( Element OldElem, boolean Del, boolean Avant, ElementType *ElTypeNumber );
extern boolean GDRCreerDescendant ( Element *ElParent, Element *ElFrere, pTreeSch TypeACreer, pTreeSch Limite );
extern boolean GDRPrepareDest ( Element ElemAColler, Element FrereDest, ElementType *DestType, TyOperation TypeOperation );
extern Element GDRRestructurer ( Element FrereDest, ElementType *DestType, TyOperation TypeOperation );
extern Element GDRRestructurerElem ( Element FrereDest, ElementType *DestType, TyOperation TypeOperation );
extern Element RestructureEtColleDedans ( Document ParDocDest, Element ElemDest );
extern Element GDRRestructureEtConvertir ( Document ParDocDest, Element ElemDest );
extern Element RestructureEtColleFrere ( Document ParDocDest, Element FrereDest, boolean Dev, int nR, SSchema pS );
extern Element RestructureElem ( Document doc, Element SourceElem, SSchema pS, int nR );

#else /* __STDC__ */

extern void * GDRGetMem (/* unsigned MemSize, char SourceFunc[] */);
extern void GDRLibere (/* void * Adresse */);
extern void GDRLibereTreeSch (/* pTreeSch Noeud */);
extern void GDRLibereListeUnite (/* pStrListeUnit p */);
extern void GDRLibereTreeType (/* pTreeTyp Noeud */);
extern void GDRLibereDyck (/* FlatTree *Noeud */);
extern boolean GDRRetireMarqueDePage (/* Element Elem, Document Docu */);
extern void GDRFindSchType (/* pTreeSch pRacine, pTreeSch *pType, int ElemRegle, int PereRegle, boolean recur */);
extern void GDRFindTreeType (/* pTreeTyp pRacine, pTreeTyp *pType, ElementType ElemType, int PereRegle, boolean recur */);
extern void GDRCreeTreeType (/* TreeTyp **pNewType, pTreeSch pSchemaType, TreeTyp *pPere, PtrSchemaResdyn pResdyn, boolean newTypeCanon */);
extern void GDRTrieDyck (/* pFlatTree Debut */);
extern void GDRCreeDyck (/* FlatTree **RlPrevious, pTreeTyp pType, FlatTree **DebutArbrePlat, Tyff Typeff, boolean source */);
extern boolean GDRCreeRecTypes (/* pTreeTyp PremiereRecursion, pTreeTyp TreeTypeSource, pTreeTyp *pNewType, pTreeTyp TreeTypePereDest, PtrSchemaResdyn pResdyn */);
extern boolean GDRAddListeUnit (/* pTreeTyp ElemTreeTyp, int TypeNum */);
extern boolean GDRTransTypeUnites (/* void */);
extern boolean GDRDevRecursif (/* Element Elem, pTreeTyp PereTreeTyp, PtrSchemaResdyn pResdyn */);
extern void GDRCompatibilite (/* TypeCouplage *TC, Tyff Typeff */);
extern void GDRffectifPur (/* TypeCouplage *TC */);
extern void GDRMarqueffective (/* pTreeTyp Typ, PtrSchemaResdyn pResdyn */);
extern boolean GDRRechArbre (/* ElementType SourElemType, ElementType DestElemType, TypeCouplage **SC, TypeCouplage **DC */);
extern boolean GDRDevRecTypeDest (/* pTreeTyp T, int MaxProf, int prof, PtrSchemaResdyn pResdyn */);
extern int GDRCalcProfondeur (/* pTreeTyp T, PtrSchemaResdyn pResdyn */);
extern boolean GDRDevRecDest (/* TypeCouplage *SC, TypeCouplage *DC */);
extern void GDRRAZCouplage (/* TypeCouplage *Debut */);
extern TyRelation GDRCalcCouplage (/* ElementType SourElemType, ElementType DestElemType, TypeCouplage **SC, TypeCouplage **DC */);
extern TyRelation GDRCalcRelation (/* TypeCouplage **SC, TypeCouplage **DC, Tyff Typeff, boolean Tri */);
extern boolean GDRCollerRestructurer (/* void */);
extern boolean GDRInitContexte (/* Document DocDest, Element ElDest, TyOperation TypeOp */);
extern boolean GDRInitContexteElem (/* Document DocDest, Element ElSource, Element ElDest, TyOperation TypeOp */);
extern boolean GDRRegenerer (/* StrListeEl *p */);
extern void GDRFinRestruct (/* boolean Resultat */);
extern boolean GDRRepererSource (/* Element ElemAColler, StrListeEl *El */);
extern Element GDRCreeElem (/* Element OldElem, boolean Del, boolean Avant, ElementType *ElTypeNumber */);
extern boolean GDRCreerDescendant (/* Element *ElParent, Element *ElFrere, pTreeSch TypeACreer, pTreeSch Limite */);
extern boolean GDRPrepareDest (/* Element ElemAColler, Element FrereDest, ElementType *DestType, TyOperation TypeOperation */);
extern Element GDRRestructurer (/* Element FrereDest, ElementType *DestType, TyOperation TypeOperation */);
extern Element GDRRestructurerElem (/* Element FrereDest, ElementType *DestType, TyOperation TypeOperation */);
extern Element RestructureEtColleDedans (/* Document ParDocDest, Element ElemDest */);
extern Element GDRRestructureEtConvertir (/* Document ParDocDest, Element ElemDest */);
extern Element RestructureEtColleFrere (/* Document ParDocDest, Element FrereDest, boolean Dev, int nR, SSchema pS */);
extern Element RestructureElem (/* Document doc, Element SourceElem, SSchema pS, int nR */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
