/*
 *
 *  (c) COPYRIGHT INRIA,  Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
 /*
 * Warning:
 * This module is part of Thot, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

 /*
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) 
 */

/*
   table2.c : Traitements specifiques a la structure Table.

 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "select_tv.h"
#include "modif_tv.h"
#include "appdialogue_tv.h"

#include "attributes_f.h"
#include "attrpresent_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "createabsbox_f.h"
#include "changeabsbox_f.h"
#include "tree_f.h"
#include "references_f.h"
#include "presrules_f.h"

/* exceptions definies dans Tableau */
#include "exc_Table.h"


/*----------------------------------------------------------------------
   CanApplAttrRules retourne vrai dans ApplAttrsi on peut 
   appliquer les regles de l'attribut a` l'element.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CanApplAttrRules (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, ThotBool * ApplAttr)

#else  /* __STDC__ */
void                CanApplAttrRules (pEl, pAttr, pDoc, ApplAttr)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;
ThotBool            *ApplAttr;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute        pAttrTab;
   PtrElement          pElAttr;

   *ApplAttr = TRUE;		/* a priori, on peut appliquer la regle */
   if (TypeHasException (EXC_TR_Table_ATTRIBUT, pEl->ElTypeNumber, pEl->ElStructSchema))
     {
	/* on est dans un tableau sur un element necessitant
	   un traitement particulier pour ses attributs */
	attr = GetAttrWithException (EXC_ID_Type_Table, pEl->ElStructSchema);
	if ((pAttrTab = GetTypedAttrAncestor (pEl, attr, pEl->ElStructSchema,
					      &pElAttr)))
	  {
	     /* le tableau porte bien un attribut Type_Table */

	     if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Title_Table, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de largeur de l'attribut Title_Width 
		     quand l'attribut Type_Table vaut Colonnes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Title_Width, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 3 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Compound_Column, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_bas 
		     quand l'attribut Type_Table vaut Lignes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Foot_Hairline_CompCol, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 2 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Compound_Row, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_Droit 
		     quand l'attribut Type_Table vaut Colonnes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Right_Hairline_CompRow, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 3 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Headings, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_Inferieur 
		     quand l'attribut Type_Table vaut Lignes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Foot_Hairline_Headings, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 2 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_The_Columns, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_Gauche 
		     quand l'attribut Type_Table vaut Colonnes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Left_Hairline_Cols, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 3 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   CheckHeightRuleHairline traite la regle de hauteur des filets verticaux.
   Cette procedure est appelee lors de l'application de    
   toute regle de hauteur ou de largeur elastique.         
   - pAb est le pave auquel on applique la regle.         
   - pRule est la regle a` traiter.                       
   - position definit la position qui determine la hauteur    
   elastique                                               
   Le traitement particulier n'a lieu que s'il s'agit de   
   la regle de hauteur appliquee a` un pave de             
   presentation cree' par un element Headings,       
   The_Columns, Simple_Column ou                   
   Compound_Column et si la hauteur est definie par  
   un element FootTable.                                 
   Dans ce cas, la procedure retourne un pointeur sur le   
   pave du premier element FootTable qui suit         
   l'element de pAb. Dans les autres cas, ou si le pave   
   cherche' n'existe pas, elle retourne NULL.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CheckHeightRuleHairline (PtrAbstractBox pAb, PosRule * position, PtrPRule pRule, PtrAbstractBox * pAbFootTable)

#else  /* __STDC__ */
void                CheckHeightRuleHairline (pAb, position, pRule, *pAbFootTable)
PtrAbstractBox      pAb;
PosRule            *position;
PtrPRule            pRule;
PtrAbstractBox     *pAbFootTable;

#endif /* __STDC__ */

{
   PtrElement          pEl;

   *pAbFootTable = NULL;
   if (pRule->PrType == PtHeight)
      /* c'est bien une regle de hauteur */
      if (pAb->AbPresentationBox)
	 /* cette regle s'applique a un pave de presentation */
	{
	   pEl = pAb->AbElement;
	   if (TypeHasException (EXC_ID_FootTable, position->PoRefIdent,
				 pEl->ElStructSchema))
	      /* la regle exprime une position par rapport a un element */
	      /* Bas_Table */
	      if (TypeHasException (EXC_ID_Headings, pEl->ElTypeNumber,
				    pEl->ElStructSchema) ||
		  TypeHasException (EXC_ID_The_Columns, pEl->ElTypeNumber,
				    pEl->ElStructSchema) ||
		  TypeHasException (EXC_ID_Simple_Column, pEl->ElTypeNumber,
				    pEl->ElStructSchema) ||
	       TypeHasException (EXC_ID_Compound_Column, pEl->ElTypeNumber,
				 pEl->ElStructSchema))
		 /* c'est un pave de presentation d'un element En\240T\352tes */
		 /* Les\240Colonnes, Colonne\240Simple ou Colonne\240Compos\351e */
		{
		   /* supprime l'indicateur AbstractBox Actif pour eviter que le */
		   /* filet soit affiche' en couleur */
		   pAb->AbSensitive = FALSE;
		   /* cherche le premier element Bas_Table qui suit */
		   pEl = FwdSearchTypedElem (pEl, GetElemWithException (EXC_ID_FootTable,
				 pEl->ElStructSchema), pEl->ElStructSchema);
		   if (pEl != NULL)
		      /* retourne le pave de l'element trouve' */
		      *pAbFootTable = pEl->ElAbstractBox[pAb->AbDocView - 1];
		}
	}
}


/*----------------------------------------------------------------------
   CheckNextIsFootTable modifie le booleen PcLast si l'element pEl
   est une ligne de tableau. PcLast est mis a` vrai si    
   l'element est suivi d'un element FootTable et a` Faux 
   sinon. Si l'element pEl n'est pas une ligne de tableau, 
   le booleen PcLast n'est pas modifie'.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CheckNextIsFootTable (PtrElement pEl, ThotBool * PcLast)
#else  /* __STDC__ */
void                CheckNextIsFootTable (pEl, PcLast)
PtrElement          pEl;
ThotBool            *PcLast;
#endif /* __STDC__ */

{
   if (TypeHasException (EXC_ID_Simple_Row, pEl->ElTypeNumber, pEl->ElStructSchema)
       || TypeHasException (EXC_ID_Compound_Row, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* c'est bien une ligne simple ou composee de tableau */
      if (pEl->ElParent != NULL)
	 if (TypeHasException (EXC_ID_The_Rows, pEl->ElParent->ElTypeNumber,
			       pEl->ElParent->ElStructSchema))
	    /* c'est une ligne du 1er niveau */
	   {
	      if (pEl->ElNext == NULL)
		 *PcLast = TRUE;
	      else
		{
		   *PcLast = FALSE;
		   if (pEl->ElNext != NULL)
		      if (TypeHasException (EXC_ID_FootTable, pEl->ElNext->ElTypeNumber,
					    pEl->ElNext->ElStructSchema))
			 /* le suivant est un element FootTable */
			 *PcLast = TRUE;
		}
	   }
}


/*----------------------------------------------------------------------
   SetVertOverflow                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetVertOverflow (PtrPRule pRule, PtrAbstractBox pAb)
#else  /* __STDC__ */
void                SetVertOverflow (pRule, pAb)
PtrPRule            pRule;
PtrAbstractBox      pAb;
#endif /* __STDC__ */

{
   PtrElement          pEl;
   ThotBool             found;
   PtrAttribute        pAttr;
   int                 attr;

   if (pRule->PrType == PtHeight || pRule->PrType == PtVertPos)
      /* c'est une regle de hauteur ou de position verticale */
     {
	pEl = pAb->AbElement;
	if (TypeHasException (EXC_ID_Cell, pEl->ElTypeNumber, pEl->ElStructSchema))
	   /* c'est un pave d'une cellule de tableau */
	  {
	     /* cherche si la cellule possede l'attribut Debordement_vert */
	     /* cherche d'abord le numero de l'attribut */
	     attr = GetAttrWithException (EXC_ID_Extens_Vertic, pEl->ElStructSchema);
	     /* cherche si cet attribut est porte' par l'element */
	     pAttr = pEl->ElFirstAttr;
	     found = FALSE;
	     while (!found && pAttr != NULL)
		if (pAttr->AeAttrNum == attr &&
		    pAttr->AeAttrSSchema == pEl->ElStructSchema)
		   found = TRUE;
		else
		   pAttr = pAttr->AeNext;
	     if (found)
		/* l'element porte cet attribut, le pave deborde */
		/* verticalement de son englobant */
	       {
		  /* Dans la vue "Source_View" du schema de presentation HTMLP.P */
		  /* les cellules etendues verticalement doivent quand meme   */
		  /* respecter l'englobement */
		  if (pRule->PrViewNum != 2 || ustrcmp (pAb->AbElement->ElStructSchema->SsName, TEXT("HTML")) != 0)
		     pAb->AbVertEnclosing = FALSE;
	       }
	     else
		/* l'element ne porte pas (ou plus) cet attribut, le pave */
		/* ne deborde pas (plus) verticalement de son englobant */
		pAb->AbVertEnclosing = TRUE;
	  }
     }
}


/*----------------------------------------------------------------------
  ApplHeightHairline reapplique la regle de hauteur de toutes
  les boites de presentation qui representent un filet vertical
  dans le sous-arbre de pAb verticaux
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplHeightHairline (PtrAbstractBox pAb, int view, PtrDocument pDoc)
#else  /* __STDC__ */
static void         ApplHeightHairline (pAb, view, pDoc)
PtrAbstractBox      pAb;
int                 view;
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   PtrAbstractBox      pFils;
   PtrElement          pEl;
   PtrPRule            pRule;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;

   if (pAb->AbFirstEnclosed != NULL)
      /* ce pave a des descendants, on traite tous ses fils */
     {
	pFils = pAb->AbFirstEnclosed;
	do
	  {
	     ApplHeightHairline (pFils, view, pDoc);
	     pFils = pFils->AbNext;
	  }
	while (pFils != NULL);
     }
   else
      /* ce pave est une feuille */
   if (pAb->AbPresentationBox)
      /* c'est un pave de presentantion */
     {
	pEl = pAb->AbElement;
	if (TypeHasException (EXC_ID_Headings, pEl->ElTypeNumber,
			      pEl->ElStructSchema) ||
	    TypeHasException (EXC_ID_The_Columns, pEl->ElTypeNumber,
			      pEl->ElStructSchema) ||
	    TypeHasException (EXC_ID_Simple_Column, pEl->ElTypeNumber,
			      pEl->ElStructSchema) ||
	    TypeHasException (EXC_ID_Compound_Column, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
	   /* c'est un pave de presentation d'un element En_Tetes */
	   /* The_Columns, Simple_Column ou Compound_Column */
	  {
	     /* on cherche sa regle de hauteur */
	     pRule = SearchRulepAb (pDoc, pAb, &pSPR, PtHeight, FnAny, TRUE, &pAttr);
	     if (pRule->PrDimRule.DrPosition)
		/* c'est une hauteur elastique */
		if (pRule->PrDimRule.DrPosRule.PoRefKind == RkElType)
		   if (TypeHasException (EXC_ID_FootTable,
					 pRule->PrDimRule.DrPosRule.PoRefIdent, pEl->ElStructSchema))
		      /* la regle exprime une position par rapport a un */
		      /* element FootTable */
		      /*c'est bien un filet vertical, on reapplique la regle */
		      if (ApplyRule (pRule, pSPR, pAb, pDoc, pAttr))
			{
			   /* indique que la hauteur du pave' a change' */
			   pAb->AbHeightChange = TRUE;
			   /* conserve le pointeur sur le pave a reafficher */
			   if (AssocView (pEl))
			      pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
				 Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->
							   ElAssocNum - 1]);
			   else
			      pDoc->DocViewModifiedAb[view - 1] =
				 Enclosing (pAb, pDoc->DocViewModifiedAb[view - 1]);
			}
	  }
     }
}


/*----------------------------------------------------------------------
  ApplHeightRuleToHairlines reapplique la regle de hauteur de tous les filets
  verticaux crees par les elements de l'entete qui precede l'element
  de type FootTable pointe' par pFootTable
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplHeightRuleToHairlines (PtrElement pFootTable, PtrDocument pDoc)
#else  /* __STDC__ */
void                ApplHeightRuleToHairlines (pFootTable, pDoc)
PtrElement          pFootTable;
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrAbstractBox      pAb;
   int                 view;
   int                 NType;
   ThotBool             stop;

   /* cherche l'element precedent de type En_Tetes */
   pEl = NULL;
   NType = GetElemWithException (EXC_ID_Headings, pFootTable->ElStructSchema);
   if (NType != 0)
      pEl = BackSearchTypedElem (pFootTable, NType, pFootTable->ElStructSchema);
   if (pEl != NULL)
      /* pEl est l'element En_Tetes dont on doit recalculer les filets */
      /* on examine tous les paves de l'element En_tetes dans toutes */
      /* vues du document */
      for (view = 1; view <= MAX_VIEW_DOC; view++)
	{
	   /* premier pave de l'element En_Tetes dans la vue courante */
	   pAb = pEl->ElAbstractBox[view - 1];
	   stop = FALSE;
	   do
	      if (pAb == NULL)
		 stop = TRUE;
	      else if (pAb->AbElement != pEl)
		 /* ce pave n'appartient pas a l'element En_Tetes */
		 stop = TRUE;
	      else
		{
		   /* traite les hauteurs des filets de ce sous-arbre */
		   ApplHeightHairline (pAb, view, pDoc);
		   /* passe au pave suivant */
		   pAb = pAb->AbNext;
		}
	   while (!stop);
	}
}


/*----------------------------------------------------------------------
  DeleteRowHairline    detruit les boites de presentation FootHairline
  creees par la ligne de tableau qui precede le saut de page pointe'
  par pElPage.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DeleteRowHairline (PtrElement pElPage, PtrDocument pDoc)

#else  /* __STDC__ */
static void         DeleteRowHairline (pElPage, pDoc)
PtrElement          pElPage;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   int                 view;
   PtrAbstractBox      pAb;
   ThotBool             found;

   /* supprime le filet horizontal du bas de la ligne precedente */
   pEl = pElPage->ElPrevious;	/* element precedent le saut de page */
   if (pEl != NULL)
      if (TypeHasException (EXC_ID_Simple_Row, pEl->ElTypeNumber, pEl->ElStructSchema) ||
	  TypeHasException (EXC_ID_Compound_Row, pEl->ElTypeNumber, pEl->ElStructSchema))
	 /* c'est bien une ligne de tableau */
	 /* on examine toutes les vues du document */
	 for (view = 1; view <= MAX_VIEW_DOC; view++)
	   {
	      pAb = pEl->ElAbstractBox[view - 1];	/* 1er pave de la ligne */
	      found = FALSE;
	      while (pAb != NULL && !found)
		 if (pAb->AbElement != pEl)
		    /* ce pave n'appartient pas a la ligne, fin de la */
		    /* recherche */
		    pAb = NULL;
		 else
		   {
		      if (pAb->AbPresentationBox)
			 if (pAb->AbLeafType == LtGraphics)
			    /* c'est un pave de presentation graphique */
			    found = TRUE;
		      if (!found)
			 /* passe au pave suivant */
			 pAb = pAb->AbNext;
		   }
	      if (found)
		{
		   /* detruit le pave found' */
		   SetDeadAbsBox (pAb);
		   /* conserve le pointeur sur le pave a reafficher */
		   if (AssocView (pEl))
		      pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
			 Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->
							   ElAssocNum - 1]);
		   else
		      pDoc->DocViewModifiedAb[view - 1] =
			 Enclosing (pAb, pDoc->DocViewModifiedAb[view - 1]);
		}
	   }
}


/*----------------------------------------------------------------------
  SearchTypeExcept cherche dans le sous arbre de l'element pElToCut un element :
             - portant une exception TypeExcept
             - de meme schemas de structure
             - qui n'est pas une copie
  La recherche n'explore pas les elements :
               - portant une exception PageBreak
               - qui sont des natures
               - qui sont des copies
  La recherche s'arrete sur l'element StopElem si Restrict = TRUE
  Renvoi un pointeur sur l'element trouve ou NULL si rien n'a ete trouve
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   SearchTypeExcept (PtrElement pElToCut, PtrElement StopElem, int TypeExcept, ThotBool Restrict)
#else  /* __STDC__ */
static PtrElement   SearchTypeExcept (pElToCut, StopElem, TypeExcept, Restrict)
PtrElement          pElToCut;
PtrElement          StopElem;
int                 TypeExcept;
ThotBool             Restrict;
#endif /* __STDC__ */

{
   PtrElement          pE, Repetition;

   Repetition = NULL;

   if (pElToCut != NULL && !pElToCut->ElTerminal)
     {
	for (pE = pElToCut->ElFirstChild; pE != NULL; pE = pE->ElNext)
	  {
	     if (Restrict && pE == StopElem)	/* on arrete la recherche */
		break;
	     else
	       {
		  if (pE->ElStructSchema == pElToCut->ElStructSchema)	/* meme schema de structure */
		    {
		       if (pE->ElSource == NULL)	/* ce n'est pas une copie */
			 {
			    if (TypeHasException (TypeExcept, pE->ElTypeNumber, pE->ElStructSchema))	/* exception ok */
			      {
				 Repetition = pE;	/* c'est donc le bon */
				 break;
			      }
			    else
			       /* recursion si l'element ne porte pas l'exception ExcPageBreak */ 
			       if (!TypeHasException (ExcPageBreak, pE->ElTypeNumber, pE->ElStructSchema))
			         SearchTypeExcept (pE, StopElem, TypeExcept, Restrict);
			 }
		    }
	       }
	  }
     }

   return Repetition;
}


/*----------------------------------------------------------------------
  InsertPageInTable       Si pElPage est une marque de page dans une
  structure demandant une coupure spe'ciale, cree les elements a repeter
  devant et derriere le saut de page puis cree les paves de ces elements
  ainsi que ceux du saut de page
  et retourne Vrai. Si on n'est pas dans une structure a coupure speciale,
  ne fait rien et retourne Faux.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InsertPageInTable (PtrElement pElPage, PtrDocument pDoc, int viewNb, ThotBool * cutDone)

#else  /* __STDC__ */
void                InsertPageInTable (pElPage, pDoc, viewNb, cutDone)
PtrElement          pElPage;
PtrDocument         pDoc;
int                 viewNb;
ThotBool            *cutDone;

#endif /* __STDC__ */

{
   PtrElement          pEl, pElToCopy, pSpecial;
   ThotBool             finish;

   *cutDone = FALSE;
   /* cherche les ascendants qui demandent un traitement special des */
   /* coupures par saut de page */
   pSpecial = pElPage->ElParent;
   finish = FALSE;
   while (!finish)
     {
	if (pSpecial == NULL)
	   finish = TRUE;
	else
	  {
	     if (TypeHasException (ExcPageBreak, pSpecial->ElTypeNumber,
				   pSpecial->ElStructSchema))
		/* cet element demande une coupure speciale */
	       {
		  *cutDone = TRUE;
		  /* cas particulier des tableaux */
		  if (TypeHasException (EXC_ID_The_Rows, pElPage->ElParent->ElTypeNumber,
					pElPage->ElParent->ElStructSchema))
		     /* supprime le filet horizontal du bas de la ligne precedente */
		     if (viewNb == 0)	/* inutile lors de la pagination */
			DeleteRowHairline (pElPage, pDoc);

		  /*cherche l'element a repeter */
		  pElToCopy = SearchTypeExcept (pSpecial, pElPage, ExcPageBreakRepBefore, FALSE);
		  if (pElToCopy != NULL)
		     /* il y a bien un element a repeter avant le saut de page */
		    {
		       pEl = NewSubtree (pElToCopy->ElTypeNumber, pElToCopy->ElStructSchema,
				    pDoc, pElToCopy->ElAssocNum, FALSE, TRUE,
					 TRUE, TRUE);
		       GetReference (&pEl->ElSource);
		       if (pEl->ElSource == NULL)
			  DeleteElement (&pEl, pDoc);
		       else
			 {
			    pEl->ElSource->RdElement = pEl;
			    pEl->ElSource->RdTypeRef = RefInclusion;
			    /* insere l'element cree' dans l'arbre abstrait */
			    InsertElementBefore (pElPage, pEl);
			    /* lie l'inclusion a l'element trouve' */
			    if (SetReference (pEl, NULL, pElToCopy, pDoc, pDoc, FALSE, FALSE))
			       /* l'element a inclure est connu, on le copie */
			       CopyIncludedElem (pEl, pDoc);
			    /* cree les paves du nouvel element */
			    CreateAllAbsBoxesOfEl (pEl, pDoc);
			 }
		       if (viewNb == 0)
			  /* cas particulier des tableaux */
			  if (TypeHasException (EXC_ID_The_Rows, pElPage->ElParent->ElTypeNumber,
					 pElPage->ElParent->ElStructSchema))
			     /* reapplique les regles de hauteur des filets verticaux */
			     ApplHeightRuleToHairlines (pEl, pDoc);
		    }
		  if (viewNb > 0)
		     /* cree les paves du saut de page */
		     CreateNewAbsBoxes (pElPage, pDoc, viewNb);

		  /*cherche l'element a repeter */
		  pElToCopy = SearchTypeExcept (pSpecial, pElPage, ExcPageBreakRepetition, FALSE);
		  if (pElToCopy != NULL)
		     /* il y a bien un element a repeter apres le saut de page */
		    {
		       pEl = NewSubtree (pElToCopy->ElTypeNumber, pElToCopy->ElStructSchema,
				    pDoc, pElToCopy->ElAssocNum, FALSE, TRUE,
					 TRUE, TRUE);
		       /* associe un bloc reference a l'element cree' */
		       GetReference (&pEl->ElSource);
		       if (pEl->ElSource == NULL)
			  DeleteElement (&pEl, pDoc);
		       else
			 {
			    pEl->ElSource->RdElement = pEl;
			    pEl->ElSource->RdTypeRef = RefInclusion;
			    /* insere l'element cree dans l'arbre abstrait */
			    InsertElementAfter (pElPage, pEl);
			    /* lie l'inclusion a l'element trouve' */
			    if (SetReference (pEl, NULL, pElToCopy, pDoc, pDoc, FALSE, FALSE))
			       /* l'element a inclure est connu, on le copie */
			       CopyIncludedElem (pEl, pDoc);
			    /* cree les paves du nouvel element */
			    CreateAllAbsBoxesOfEl (pEl, pDoc);
			 }
		    }
	       }
	     pSpecial = pSpecial->ElParent;	/* passe a l'ascendant */
	  }
     }
}				/* End Of InsertPageInTable */


/*----------------------------------------------------------------------
  DeletePageInTable      Si l'element saut de page pointe' par
  pElPage est dans une structure a coupure speciale, supprime les
  elements repetes qui precedent et qui suivent.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeletePageInTable (PtrElement pElPage, PtrDocument pDoc)
#else  /* __STDC__ */
void                DeletePageInTable (pElPage, pDoc)
PtrElement          pElPage;
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   PtrElement          pElPrevious, pElPrevious1, pElNext, pElNext1;
   ThotBool             stop;

   /* supprime les elements repetes precedents */
   pElPrevious = pElPage->ElPrevious;
   stop = FALSE;
   while (!stop)
     {
	if (pElPrevious == NULL)
	   stop = TRUE;		/* pas d'autre element precedent */
	else if (!TypeHasException (ExcPageBreakRepBefore, pElPrevious->ElTypeNumber,
				    pElPrevious->ElStructSchema))
	   /* l'element precedent n'est pas une repetition */
	   stop = TRUE;
	else if (pElPrevious->ElSource == NULL)
	   /* l'element precedent n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* il faut supprimer cet element precedent */
	  {
	     pElPrevious1 = pElPrevious->ElPrevious;
	     DeleteElement (&pElPrevious, pDoc);
	     pElPrevious = pElPrevious1;
	  }
     }
   /* supprime les elements repetes suivants */
   pElNext = pElPage->ElNext;
   stop = FALSE;
   while (!stop)
     {
	if (pElNext == NULL)
	   stop = TRUE;		/* pas d'autre element suivant */
	else if (!TypeHasException (ExcPageBreakRepetition, pElNext->ElTypeNumber,
				    pElNext->ElStructSchema))
	   /* l'element suivant n'est pas une repetition */
	   stop = TRUE;
	else if (pElNext->ElSource == NULL)
	   /* l'element suivant n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* il faut supprimer cet element suivant */
	  {
	     pElNext1 = pElNext->ElNext;
	     DeleteElement (&pElNext, pDoc);
	     pElNext = pElNext1;
	  }
     }
}


/*----------------------------------------------------------------------
  DeletePageAbsBoxes  Si l'element saut de page pointe'
  par pElPage est dans une structure demandant une coupure speciale,
  detruit les paves des elements repetes qui precedent et ceux des
  elements repetes qui suivent.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeletePageAbsBoxes (PtrElement pElPage, PtrDocument pDoc, int viewNb)
#else  /* __STDC__ */
void                DeletePageAbsBoxes (pElPage, pDoc, viewNb)
PtrElement          pElPage;
PtrDocument         pDoc;
int                 viewNb;

#endif /* __STDC__ */

{
   PtrElement          pElPrevious, pElNext;
   ThotBool             stop;

   /* detruit les paves des elements repetes qui precedent */
   pElPrevious = pElPage->ElPrevious;
   stop = FALSE;
   while (!stop)
     {
	if (pElPrevious == NULL)
	   stop = TRUE;		/* pas d'autre element precedent */
	else if (!TypeHasException (ExcPageBreakRepBefore, pElPrevious->ElTypeNumber,
				    pElPrevious->ElStructSchema))
	   /* l'element precedent n'est pas une repetition */
	   stop = TRUE;
	else if (pElPrevious->ElSource == NULL)
	   /* l'element precedent n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* c'est bien un element repete', on detruit ses paves */
	  {
	     DestroyAbsBoxesView (pElPrevious, pDoc, FALSE, viewNb);
	     pElPrevious = pElPrevious->ElPrevious;
	  }
     }
   /* detruit les paves des elements repetes qui suivent */
   pElNext = pElPage->ElNext;
   stop = FALSE;
   while (!stop)
     {
	if (pElNext == NULL)
	   stop = TRUE;		/* pas d'autre element suivant */
	else if (!TypeHasException (ExcPageBreakRepetition, pElNext->ElTypeNumber,
				    pElNext->ElStructSchema))
	   /* l'element suivant n'est pas une repetition */
	   stop = TRUE;
	else if (pElNext->ElSource == NULL)
	   /* l'element suivant n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* c'est bien un element repete', on detruit ses paves */
	  {
	     DestroyAbsBoxesView (pElNext, pDoc, FALSE, viewNb);
	     pElNext = pElNext->ElNext;
	  }
     }
}				/* End Of DeletePageAbsBoxes */


/*----------------------------------------------------------------------
   Table2LoadResources : connecte les ressources de traitement des  
   tableaux hors de l'editeur (pour le print)          	                                        
  ----------------------------------------------------------------------*/
void                Table2LoadResources ()
{

   if (ThotLocalActions[T_vertspan] == NULL)
     {
	/* initialisations */

	/* connexion des ressources */
	TteConnectAction (T_ruleattr, (Proc) CanApplAttrRules);
	TteConnectAction (T_abref, (Proc) CheckHeightRuleHairline);
	TteConnectAction (T_condlast, (Proc) CheckNextIsFootTable);
	TteConnectAction (T_vertspan, (Proc) SetVertOverflow);
	TteConnectAction (T_insertpage, (Proc) InsertPageInTable);
	TteConnectAction (T_deletepage, (Proc) DeletePageInTable);
	TteConnectAction (T_deletepageab, (Proc) DeletePageAbsBoxes);
     }
}

/* End Of Module Table2 */
