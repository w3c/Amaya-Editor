
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void GDRRefCible ( Element CibleOrigin, Element CibleTrans, Document DocOrigin, Document DocTrans );
extern void GDRRefElem ( Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans );
extern void GDRRefAttr ( Attribute AttrRefOrigin, Attribute AttrRefTrans, Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans );
extern boolean GDRFindListeRef ( Element ElemRef, Attribute AttrRef, Element *ElemRefTrans, Attribute *AttrRefTrans );
extern boolean GDRFindListeCible ( Element ElemRef, Attribute AttrRef, Element *ElemCible, Document *DocCible );
extern boolean GDRFindTransfType ( Element ElSource, Element ElRoot, pTreeTyp pTypeRoot, ElementType *TypeElemSource );
extern boolean GDRStructUnitEquiv ( Element ElSource, SSchema SchDest, int NumRegleDest );
extern Element GDRNewUnit ( Document DocDest, Element ElSource, Element PereDest );
extern boolean GDRCreeChild ( pTreeTyp Typ, Element *NewElem, Element Parent, Document Docu );
extern boolean GDRCopieAttr ( Document DocDest, Element SourElem, Element DestElem );
extern boolean GDRTransfertFeuilles ( Document DocDest, Element SourElem, Element DestElem );
extern boolean GDRCopieContenu ( Document DocDest, int DestTypeNum, Element SourElem, Element DestElem );
extern boolean GDRCreeDest ( pTreeTyp Typ, Document DocDest, int DestTypeNum, SSchema DestSch, Element DestPere, Element SourElem, Element *NewElem );
extern Element GDRCopyTree ( Element Source, Document DocSource, Document DocDest, Element PereDest );
extern boolean GDRRechNewType ( StrListeTyp **LRacineTree, Element Elem, pTreeTyp TypePereSource, pTreeTyp *ElempTreeTyp, int *DestRegle, SSchema *DestSSchema, boolean Recur );
extern boolean GDRCreerAncetres ( pTreeTyp p, Element DestPere, pTreeTyp Older, Element *PereCourant );
extern pTreeTyp GDRFindTypeExtern ( pTreeTyp TypePereDest, char *NameExtern );
extern boolean GDRCollerMassif ( Element SourElem, pTreeTyp TypePereSource, Element DestPere, pTreeTyp TypePereDest, TypeCouplage *SC );
extern boolean GDRColler ( TyRelation Relation );

#else /* __STDC__ */

extern void GDRRefCible (/* Element CibleOrigin, Element CibleTrans, Document DocOrigin, Document DocTrans */);
extern void GDRRefElem (/* Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans */);
extern void GDRRefAttr (/* Attribute AttrRefOrigin, Attribute AttrRefTrans, Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans */);
extern boolean GDRFindListeRef (/* Element ElemRef, Attribute AttrRef, Element *ElemRefTrans, Attribute *AttrRefTrans */);
extern boolean GDRFindListeCible (/* Element ElemRef, Attribute AttrRef, Element *ElemCible, Document *DocCible */);
extern boolean GDRFindTransfType (/* Element ElSource, Element ElRoot, pTreeTyp pTypeRoot, ElementType *TypeElemSource */);
extern boolean GDRStructUnitEquiv (/* Element ElSource, SSchema SchDest, int NumRegleDest */);
extern Element GDRNewUnit (/* Document DocDest, Element ElSource, Element PereDest */);
extern boolean GDRCreeChild (/* pTreeTyp Typ, Element *NewElem, Element Parent, Document Docu */);
extern boolean GDRCopieAttr (/* Document DocDest, Element SourElem, Element DestElem */);
extern boolean GDRTransfertFeuilles (/* Document DocDest, Element SourElem, Element DestElem */);
extern boolean GDRCopieContenu (/* Document DocDest, int DestTypeNum, Element SourElem, Element DestElem */);
extern boolean GDRCreeDest (/* pTreeTyp Typ, Document DocDest, int DestTypeNum, SSchema DestSch, Element DestPere, Element SourElem, Element *NewElem */);
extern Element GDRCopyTree (/* Element Source, Document DocSource, Document DocDest, Element PereDest */);
extern boolean GDRRechNewType (/* StrListeTyp **LRacineTree, Element Elem, pTreeTyp TypePereSource, pTreeTyp *ElempTreeTyp, int *DestRegle, SSchema *DestSSchema, boolean Recur */);
extern boolean GDRCreerAncetres (/* pTreeTyp p, Element DestPere, pTreeTyp Older, Element *PereCourant */);
extern pTreeTyp GDRFindTypeExtern (/* pTreeTyp TypePereDest, char *NameExtern */);
extern boolean GDRCollerMassif (/* Element SourElem, pTreeTyp TypePereSource, Element DestPere, pTreeTyp TypePereDest, TypeCouplage *SC */);
extern boolean GDRColler (/* TyRelation Relation */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
