
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrDocument DocuDeElem ( PtrElement pEl );
extern void Protege ( PtrElement pE );
extern PtrElement AutreMarque ( PtrElement pEl );
extern boolean ElemReadOnly ( PtrElement pEl );
extern boolean ElemHidden ( PtrElement pEl );
extern PtrElement AvChNomType ( PtrElement pEl, char *nomType );
extern PtrElement ArChNomType ( PtrElement pEl, char *nomType );
extern PtrElement AvChercheVideOuRefer ( PtrElement pEl, int Cible );
extern PtrElement ArChercheVideOuRefer ( PtrElement pEl, boolean Cible );
extern void InsDernier ( PtrElement ancien, PtrElement nouveau );
extern boolean DansSArbre ( PtrElement pEl, PtrElement pRac );
extern void  MetAttributsImposes(PtrElement pEl, SRule *pRe1, PtrSSchema pSS, boolean AvecAttributs, PtrDocument pDoc);
extern boolean TypeOK ( PtrElement pE, int Typ, PtrSSchema pStr );
extern boolean Avant ( PtrElement pEl1, PtrElement pEl2 );
extern boolean Englobe ( PtrElement p1, PtrElement p2 );
extern PtrElement AncetreCommun ( PtrElement p1, PtrElement p2 );
extern PtrElement PremFeuille ( PtrElement pEl );
extern PtrElement DerFeuille ( PtrElement pRacine );
extern PtrElement Ascendant ( PtrElement pElDep, int TypeEl, PtrSSchema pS );
extern PtrElement Av2Cherche ( PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 );
extern PtrElement AvCherche ( PtrElement pEl, int Typ, PtrSSchema pStr );
extern PtrElement Ar2Cherche ( PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 );
extern PtrElement ArCherche ( PtrElement pEl, int Typ, PtrSSchema pStr );
extern PtrElement ArChElVisible ( PtrElement RlRoot, PtrElement pEl, int Vue );
extern PtrElement AvAttrCherche ( PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr );
extern PtrElement ArAttrCherche ( PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr );
extern void SauteMarquePage ( PtrElement *p );
extern void SautePageDebut ( PtrElement *p );
extern void SauteArMarquePage ( PtrElement *p );
extern PtrElement Successeur ( PtrElement pEl );
extern PtrElement FeuillePrecedente ( PtrElement pEl );
extern PtrElement FeuilleSuivante ( PtrElement pEl );
extern void InsAvant ( PtrElement ancien, PtrElement nouveau );
extern void InsApres ( PtrElement ancien, PtrElement nouveau );
extern void AjPremFils ( PtrElement ancien, PtrElement nouveau );
extern void InsChoix ( PtrElement pEl, PtrElement *p, boolean enplace );
extern int NewLabel ( PtrDocument pDoc );
extern int GetCurrentLabel ( PtrDocument pDoc );
extern void SetCurrentLabel ( PtrDocument pDoc, int label );
extern PtrElement CreeSArbre ( int TypeElem, PtrSSchema Str, PtrDocument pDoc, int NAssoc, boolean Desc, boolean RlRoot, boolean AvecAttributs, boolean AvecLabel );
extern void RetireExclus ( PtrElement *pEl );
extern void Retire ( PtrElement El );
extern void AttrRetire ( PtrElement pEl, PtrAttribute pAttr );
extern void AttrSupprime ( PtrElement pEl, PtrAttribute pAttr );
extern void Supprime ( PtrElement *El );
extern PtrElement CopieArbre ( PtrElement Source, PtrDocument DocSource, int NAssoc, PtrSSchema pSchS, PtrDocument DocCopie, PtrElement pPere, boolean VerifAttr, boolean PartageRef );
extern void CopieInclus ( PtrElement pEl, PtrDocument pDoc );
extern PtrElement DupElem ( PtrElement El, PtrDocument pDoc );
extern PtrAttribute AttrWithNum ( PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch );
extern PtrAttribute GetAttrInEnclosing ( PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch, PtrElement *pElAttr );
extern void VerifieLangueRacine ( PtrDocument pDoc, PtrElement pEl );

#else /* __STDC__ */

extern PtrDocument DocuDeElem (/* PtrElement pEl */);
extern void Protege (/* PtrElement pE */);
extern PtrElement AutreMarque (/* PtrElement pEl */);
extern boolean ElemReadOnly (/* PtrElement pEl */);
extern boolean ElemHidden (/* PtrElement pEl */);
extern PtrElement AvChNomType (/* PtrElement pEl, char *nomType */);
extern PtrElement ArChNomType (/* PtrElement pEl, char *nomType */);
extern PtrElement AvChercheVideOuRefer (/* PtrElement pEl, int Cible */);
extern PtrElement ArChercheVideOuRefer (/* PtrElement pEl, boolean Cible */);
extern void InsDernier (/* PtrElement ancien, PtrElement nouveau */);
extern boolean DansSArbre (/* PtrElement pEl, PtrElement pRac */);
extern void  MetAttributsImposes(/*PtrElement pEl, SRule *pRe1, PtrSSchema pSS, boolean AvecAttributs, PtrDocument pDoc*/);
extern boolean TypeOK (/* PtrElement pE, int Typ, PtrSSchema pStr */);
extern boolean Avant (/* PtrElement pEl1, PtrElement pEl2 */);
extern boolean Englobe (/* PtrElement p1, PtrElement p2 */);
extern PtrElement AncetreCommun (/* PtrElement p1, PtrElement p2 */);
extern PtrElement PremFeuille (/* PtrElement pEl */);
extern PtrElement DerFeuille (/* PtrElement pRacine */);
extern PtrElement Ascendant (/* PtrElement pElDep, int TypeEl, PtrSSchema pS */);
extern PtrElement Av2Cherche (/* PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 */);
extern PtrElement AvCherche (/* PtrElement pEl, int Typ, PtrSSchema pStr */);
extern PtrElement Ar2Cherche (/* PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 */);
extern PtrElement ArCherche (/* PtrElement pEl, int Typ, PtrSSchema pStr */);
extern PtrElement ArChElVisible (/* PtrElement RlRoot, PtrElement pEl, int Vue */);
extern PtrElement AvAttrCherche (/* PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr */);
extern PtrElement ArAttrCherche (/* PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr */);
extern void SauteMarquePage (/* PtrElement *p */);
extern void SautePageDebut (/* PtrElement *p */);
extern void SauteArMarquePage (/* PtrElement *p */);
extern PtrElement Successeur (/* PtrElement pEl */);
extern PtrElement FeuillePrecedente (/* PtrElement pEl */);
extern PtrElement FeuilleSuivante (/* PtrElement pEl */);
extern void InsAvant (/* PtrElement ancien, PtrElement nouveau */);
extern void InsApres (/* PtrElement ancien, PtrElement nouveau */);
extern void AjPremFils (/* PtrElement ancien, PtrElement nouveau */);
extern void InsChoix (/* PtrElement pEl, PtrElement *p, boolean enplace */);
extern int NewLabel (/* PtrDocument pDoc */);
extern int GetCurrentLabel (/* PtrDocument pDoc */);
extern void SetCurrentLabel (/* PtrDocument pDoc, int label */);
extern PtrElement CreeSArbre (/* int TypeElem, PtrSSchema Str, PtrDocument pDoc, int NAssoc, boolean Desc, boolean RlRoot, boolean AvecAttributs, boolean AvecLabel */);
extern void RetireExclus (/* PtrElement *pEl */);
extern void Retire (/* PtrElement El */);
extern void AttrRetire (/* PtrElement pEl, PtrAttribute pAttr */);
extern void AttrSupprime (/* PtrElement pEl, PtrAttribute pAttr */);
extern void Supprime (/* PtrElement *El */);
extern PtrElement CopieArbre (/* PtrElement Source, PtrDocument DocSource, int NAssoc, PtrSSchema pSchS, PtrDocument DocCopie, PtrElement pPere, boolean VerifAttr, boolean PartageRef */);
extern void CopieInclus (/* PtrElement pEl, PtrDocument pDoc */);
extern PtrElement DupElem (/* PtrElement El, PtrDocument pDoc */);
extern PtrAttribute AttrWithNum (/* PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch */);
extern PtrAttribute GetAttrInEnclosing (/* PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch, PtrElement *pElAttr */);
extern void VerifieLangueRacine (/* PtrDocument pDoc, PtrElement pEl */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
