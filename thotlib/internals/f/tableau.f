
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int Tableau_OrphanCell ( PtrElement pCell, PtrElement pSauve );
extern boolean Tableau_EscapeSpecial ( PtrDocument SelDoc );
extern void Tableau_DistribAlignHorizVertic ( PtrElement pEl, PtrAttribute pAttrCell, PtrDocument SelDoc );
extern void TableauAttributSpecial ( PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc );
extern boolean Tableau_TypeExclus ( PtrElement pEl, int eltype, PtrSSchema pSS );
extern boolean Tableau_AttributRef ( PtrAttribute pAttr );
extern void Tableau_MetAttrCommuns ( PtrElement pCell, PtrDocument pDoc );
extern void Tableau_MetAttrTitreLigne ( PtrElement pTitre, PtrDocument pDoc );
extern void Tableau_MetAttrLigne ( PtrElement pLigne, PtrDocument pDoc );
extern void Tableau_MetAttrTitreColonne ( PtrElement pTitreCol, PtrDocument pDoc );
extern void Tableau_MetAttrColonneSimple ( PtrElement pCol, PtrDocument pDoc );
extern void Tableau_MetAttrColonneComp ( PtrElement pCol, PtrDocument pDoc );
extern void Tableau_MetAttrTitreTableau ( PtrElement pTitreTab, PtrDocument pDoc );
extern void Tableau_MetAttrCellule ( PtrElement pCell, PtrElement pCol, PtrDocument pDoc );
extern void Tableau_MetAttrLesColonnes ( PtrElement pLesCol, PtrDocument pDoc );
extern void Tableau_MetAttrEnTetes ( PtrElement pEnTetes, PtrDocument pDoc );
extern void Tableau_MetAttrTable ( PtrElement pTable, PtrDocument pDoc );
extern void Tableau_CreeColSimple ( PtrElement pEl, PtrDocument pDoc );
extern void Tableau_CreeLigneSimple ( PtrElement pEl, PtrDocument pDoc );
extern void Cree_Tableau ( PtrElement pEl, PtrDocument pDoc );
extern void HauteurPavesFilets ( PtrAbstractBox pPav, int Vue, PtrDocument pDoc );
extern void HauteurFilets ( PtrElement pBasTableau, PtrDocument pDoc );
extern void Tableau_CreeFiletLigne ( PtrElement pLigne, PtrElement pBasPage, PtrDocument pDoc );
extern void Tableau_Creation_DoIt ( PtrElement pEl, PtrDocument pDoc );
extern void Tableau_Creation ( PtrElement pEl, PtrDocument pDoc );
extern void Tableau_SelectColSimple ( PtrElement pEl );
extern boolean Tableau_Selection ( PtrElement pEl, PtrDocument pDoc, boolean Ext );
extern boolean Tableau_DernierSauve ( PtrElement pElSv );
extern void Tableau_ColleCellules ( PtrElement pCol, int NbPreced, int NbCell, PtrElement *pSvCell, PtrDocument pDoc );
extern void Tableau_VerifColCell ( PtrElement pCol, PtrElement *pCell, PtrElement *pCellPrec, PtrDocument pDoc );
extern void Tableau_VerifieLigne ( PtrElement pLigne, PtrElement pLesCol, PtrDocument pDoc );
extern void Tableau_ColleVoisin ( PtrElement pColle, PtrElement *pElSv, PtrDocument pDoc );
extern void Exc_Page_Break_Couper ( PtrElement *PremSel, PtrElement *DerSel, PtrDocument pDoc, boolean *Sauve, boolean *DetruirePage );
extern void Tableau_Couper ( PtrElement pBasTableau, PtrDocument pDoc );
extern void TableauVerifExtension ( PtrAttribute pAttr, PtrElement PremEl, PtrElement DerEl, boolean SupprimeAttr );
extern boolean Tableau_CanHolophrast ( PtrElement pEl );

#else /* __STDC__ */

extern int Tableau_OrphanCell (/* PtrElement pCell, PtrElement pSauve */);
extern boolean Tableau_EscapeSpecial (/* PtrDocument SelDoc */);
extern void Tableau_DistribAlignHorizVertic (/* PtrElement pEl, PtrAttribute pAttrCell, PtrDocument SelDoc */);
extern void TableauAttributSpecial (/* PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc */);
extern boolean Tableau_TypeExclus (/* PtrElement pEl, int eltype, PtrSSchema pSS */);
extern boolean Tableau_AttributRef (/* PtrAttribute pAttr */);
extern void Tableau_MetAttrCommuns (/* PtrElement pCell, PtrDocument pDoc */);
extern void Tableau_MetAttrTitreLigne (/* PtrElement pTitre, PtrDocument pDoc */);
extern void Tableau_MetAttrLigne (/* PtrElement pLigne, PtrDocument pDoc */);
extern void Tableau_MetAttrTitreColonne (/* PtrElement pTitreCol, PtrDocument pDoc */);
extern void Tableau_MetAttrColonneSimple (/* PtrElement pCol, PtrDocument pDoc */);
extern void Tableau_MetAttrColonneComp (/* PtrElement pCol, PtrDocument pDoc */);
extern void Tableau_MetAttrTitreTableau (/* PtrElement pTitreTab, PtrDocument pDoc */);
extern void Tableau_MetAttrCellule (/* PtrElement pCell, PtrElement pCol, PtrDocument pDoc */);
extern void Tableau_MetAttrLesColonnes (/* PtrElement pLesCol, PtrDocument pDoc */);
extern void Tableau_MetAttrEnTetes (/* PtrElement pEnTetes, PtrDocument pDoc */);
extern void Tableau_MetAttrTable (/* PtrElement pTable, PtrDocument pDoc */);
extern void Tableau_CreeColSimple (/* PtrElement pEl, PtrDocument pDoc */);
extern void Tableau_CreeLigneSimple (/* PtrElement pEl, PtrDocument pDoc */);
extern void Cree_Tableau (/* PtrElement pEl, PtrDocument pDoc */);
extern void HauteurPavesFilets (/* PtrAbstractBox pPav, int Vue, PtrDocument pDoc */);
extern void HauteurFilets (/* PtrElement pBasTableau, PtrDocument pDoc */);
extern void Tableau_CreeFiletLigne (/* PtrElement pLigne, PtrElement pBasPage, PtrDocument pDoc */);
extern void Tableau_Creation_DoIt (/* PtrElement pEl, PtrDocument pDoc */);
extern void Tableau_Creation (/* PtrElement pEl, PtrDocument pDoc */);
extern void Tableau_SelectColSimple (/* PtrElement pEl */);
extern boolean Tableau_Selection (/* PtrElement pEl, PtrDocument pDoc, boolean Ext */);
extern boolean Tableau_DernierSauve (/* PtrElement pElSv */);
extern void Tableau_ColleCellules (/* PtrElement pCol, int NbPreced, int NbCell, PtrElement *pSvCell, PtrDocument pDoc */);
extern void Tableau_VerifColCell (/* PtrElement pCol, PtrElement *pCell, PtrElement *pCellPrec, PtrDocument pDoc */);
extern void Tableau_VerifieLigne (/* PtrElement pLigne, PtrElement pLesCol, PtrDocument pDoc */);
extern void Tableau_ColleVoisin (/* PtrElement pColle, PtrElement *pElSv, PtrDocument pDoc */);
extern void Exc_Page_Break_Couper (/* PtrElement *PremSel, PtrElement *DerSel, PtrDocument pDoc, boolean *Sauve, boolean *DetruirePage */);
extern void Tableau_Couper (/* PtrElement pBasTableau, PtrDocument pDoc */);
extern void TableauVerifExtension (/* PtrAttribute pAttr, PtrElement PremEl, PtrElement DerEl, boolean SupprimeAttr */);
extern boolean Tableau_CanHolophrast (/* PtrElement pEl */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
