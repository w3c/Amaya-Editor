/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   tableau2.c : Traitements specifiques a la structure Tableau.

 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "select_tv.h"
#include "modif_tv.h"

#include "attributes_f.h"
#include "attrpresent_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "createabsbox_f.h"
#include "changeabsbox_f.h"
#include "tree_f.h"
#include "references_f.h"
#include "presrules_f.h"
#include "exc_tableau.h"


/*----------------------------------------------------------------------
   Tableau_ApplRegleAttribut retourne vrai dans ApplAttrsi on peut 
   appliquer les regles de l'attribut a` l'element.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Tableau_ApplRegleAttribut (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, boolean * ApplAttr)

#else  /* __STDC__ */
void                Tableau_ApplRegleAttribut (pEl, pAttr, pDoc, ApplAttr)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;
boolean            *ApplAttr;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute        pAttrTab;
   PtrElement          pElAttr;

   *ApplAttr = TRUE;		/* a priori, on peut appliquer la regle */
   if (TypeHasException (EXC_TR_Tableau_ATTRIBUT, pEl->ElTypeNumber, pEl->ElStructSchema))
     {
	/* on est dans un tableau sur un element necessitant
	   un traitement particulier pour ses attributs */
	attr = GetAttrWithException (EXC_ID_Type_Tableau, pEl->ElStructSchema);
	if ((pAttrTab = GetTypedAttrAncestor (pEl, attr, pEl->ElStructSchema,
					      &pElAttr)))
	  {
	     /* le tableau porte bien un attribut Type_Tableau */

	     if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Titre_Tableau, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de largeur de l'attribut Largeur_Titre 
		     quand l'attribut Type_Tableau vaut Colonnes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Largeur_Titre, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 3 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Colonne_Composee, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_bas 
		     quand l'attribut Type_Tableau vaut Lignes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Filet_Bas_ColComp, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 2 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Ligne_Composee, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_Droit 
		     quand l'attribut Type_Tableau vaut Colonnes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Filet_Droit_LigComp, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 3 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_En_Tetes, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_Inferieur 
		     quand l'attribut Type_Tableau vaut Lignes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Filet_Inf_EnTetes, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 2 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }

	     else if (pEl->ElTypeNumber == GetElemWithException (EXC_ID_Les_Colonnes, pEl->ElStructSchema))
	       {
		  /* on inhibe la regle de creation de l'attribut Filet_Gauche 
		     quand l'attribut Type_Tableau vaut Colonnes ou Tabulations */
		  if (pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Filet_Gauche_LesCol, pEl->ElStructSchema)
		      && (pAttrTab->AeAttrValue == 3 || pAttrTab->AeAttrValue == 4))
		     *ApplAttr = FALSE;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   Tableau_PaveRef traite la regle de hauteur des filets verticaux.
   Cette procedure est appelee lors de l'application de    
   toute regle de hauteur ou de largeur elastique.         
   - pAb est le pave auquel on applique la regle.         
   - pRegle est la regle a` traiter.                       
   - Posit definit la position qui determine la hauteur    
   elastique                                               
   Le traitement particulier n'a lieu que s'il s'agit de   
   la regle de hauteur appliquee a` un pave de             
   presentation cree' par un element En\240T\352tes,       
   Les\240Colonnes, Colonne\240Simple ou                   
   Colonne\240Compos\351 et si la hauteur est definie par  
   un element Bas_tableau.                                 
   Dans ce cas, la procedure retourne un pointeur sur le   
   pave du premier element BasPageTableau qui suit         
   l'element de pAb. Dans les autres cas, ou si le pave   
   cherche' n'existe pas, elle retourne NULL.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                Tableau_PaveRef (PtrAbstractBox pAb, PosRule * Posit, PtrPRule pRegle, PtrAbstractBox * pPavBas)

#else  /* __STDC__ */
void                Tableau_PaveRef (pAb, Posit, pRegle, *pPavBas)
PtrAbstractBox      pAb;
PosRule            *Posit;
PtrPRule            pRegle;
PtrAbstractBox     *pPavBas;

#endif /* __STDC__ */

{
   PtrElement          pEl;

   *pPavBas = NULL;
   if (pRegle->PrType == PtHeight)
      /* c'est bien une regle de hauteur */
      if (pAb->AbPresentationBox)
	 /* cette regle s'applique a un pave de presentation */
	{
	   pEl = pAb->AbElement;
	   if (TypeHasException (EXC_ID_BasTableau, Posit->PoTypeRefElem,
				 pEl->ElStructSchema))
	      /* la regle exprime une position par rapport a un element */
	      /* Bas_tableau */
	      if (TypeHasException (EXC_ID_En_Tetes, pEl->ElTypeNumber,
				    pEl->ElStructSchema) ||
		  TypeHasException (EXC_ID_Les_Colonnes, pEl->ElTypeNumber,
				    pEl->ElStructSchema) ||
		  TypeHasException (EXC_ID_Colonne_Simple, pEl->ElTypeNumber,
				    pEl->ElStructSchema) ||
	       TypeHasException (EXC_ID_Colonne_Composee, pEl->ElTypeNumber,
				 pEl->ElStructSchema))
		 /* c'est un pave de presentation d'un element En\240T\352tes */
		 /* Les\240Colonnes, Colonne\240Simple ou Colonne\240Compos\351e */
		{
		   /* supprime l'indicateur AbstractBox Actif pour eviter que le */
		   /* filet soit affiche' en couleur */
		   pAb->AbSensitive = FALSE;
		   /* cherche le premier element Bas_tableau qui suit */
		   pEl = FwdSearchTypedElem (pEl, GetElemWithException (EXC_ID_BasTableau,
				 pEl->ElStructSchema), pEl->ElStructSchema);
		   if (pEl != NULL)
		      /* retourne le pave de l'element trouve' */
		      *pPavBas = pEl->ElAbstractBox[pAb->AbDocView - 1];
		}
	}
}


/*----------------------------------------------------------------------
   Tableau_Cond_Dernier modifie le booleen PcLast si l'element pEl
   est une ligne de tableau. PcLast est mis a` vrai si    
   l'element est suivi d'un element Bas_Tableau et a` Faux 
   sinon. Si l'element pEl n'est pas une ligne de tableau, 
   le booleen PcLast n'est pas modifie'.                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                Tableau_Cond_Dernier (PtrElement pEl, boolean * PcLast)

#else  /* __STDC__ */
void                Tableau_Cond_Dernier (pEl, PcLast)
PtrElement          pEl;
boolean            *PcLast;

#endif /* __STDC__ */

{
   if (TypeHasException (EXC_ID_Ligne_Simple, pEl->ElTypeNumber, pEl->ElStructSchema)
       || TypeHasException (EXC_ID_Ligne_Composee, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* c'est bien une ligne simple ou composee de tableau */
      if (pEl->ElParent != NULL)
	 if (TypeHasException (EXC_ID_Les_Lignes, pEl->ElParent->ElTypeNumber,
			       pEl->ElParent->ElStructSchema))
	    /* c'est une ligne du 1er niveau */
	   {
	      if (pEl->ElNext == NULL)
		 *PcLast = TRUE;
	      else
		{
		   *PcLast = FALSE;
		   if (pEl->ElNext != NULL)
		      if (TypeHasException (EXC_ID_BasTableau, pEl->ElNext->ElTypeNumber,
					    pEl->ElNext->ElStructSchema))
			 /* le suivant est un element BasTableau */
			 *PcLast = TRUE;
		}
	   }
}


/*----------------------------------------------------------------------
   TableauDebordeVertical                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TableauDebordeVertical (PtrPRule pRegle, PtrAbstractBox pAb)

#else  /* __STDC__ */
void                TableauDebordeVertical (pRegle, pAb)
PtrPRule            pRegle;
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   boolean             trouve;
   PtrAttribute        pAttr;
   int                 attr;

   if (pRegle->PrType == PtHeight || pRegle->PrType == PtVertPos)
      /* c'est une regle de hauteur ou de position verticale */
     {
	pEl = pAb->AbElement;
	if (TypeHasException (EXC_ID_Cellule, pEl->ElTypeNumber, pEl->ElStructSchema))
	   /* c'est un pave d'une cellule de tableau */
	  {
	     /* cherche si la cellule possede l'attribut Debordement_vert */
	     /* cherche d'abord le numero de l'attribut */
	     attr = GetAttrWithException (EXC_ID_Extens_Vertic, pEl->ElStructSchema);
	     /* cherche si cet attribut est porte' par l'element */
	     pAttr = pEl->ElFirstAttr;
	     trouve = FALSE;
	     while (!trouve && pAttr != NULL)
		if (pAttr->AeAttrNum == attr &&
		    pAttr->AeAttrSSchema == pEl->ElStructSchema)
		   trouve = TRUE;
		else
		   pAttr = pAttr->AeNext;
	     if (trouve)
		/* l'element porte cet attribut, le pave deborde */
		/* verticalement de son englobant */
	       {
		  /* Dans la vue "Source_View" du schema de presentation HTMLP.P */
		  /* les cellules etendues verticalement doivent quand meme   */
		  /* respecter l'englobement */
		  if (pRegle->PrViewNum != 2 || strcmp (pAb->AbElement->ElStructSchema->SsName, "HTML") != 0)
		     pAb->AbVertEnclosing = FALSE;
	       }
	     else
		/* l'element ne porte pas (ou plus) cet attribut, le pave */
		/* ne deborde pas (plus) verticalement de son englobant */
		pAb->AbVertEnclosing = TRUE;
	  }
     }
}


/* HauteurPavesFilets   reapplique la regle de hauteur de toutes */
/* les boites de presentation qui representent un filet vertical */
/* dans le sous-arbre de pAb. */
/* verticaux */

#ifdef __STDC__
static void         HauteurPavesFilets (PtrAbstractBox pAb, int Vue, PtrDocument pDoc)

#else  /* __STDC__ */
static void         HauteurPavesFilets (pAb, Vue, pDoc)
PtrAbstractBox      pAb;
int                 Vue;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox      pFils;
   PtrElement          pEl;
   PtrPRule            pRegle;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;

   if (pAb->AbFirstEnclosed != NULL)
      /* ce pave a des descendants, on traite tous ses fils */
     {
	pFils = pAb->AbFirstEnclosed;
	do
	  {
	     HauteurPavesFilets (pFils, Vue, pDoc);
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
	if (TypeHasException (EXC_ID_En_Tetes, pEl->ElTypeNumber,
			      pEl->ElStructSchema) ||
	    TypeHasException (EXC_ID_Les_Colonnes, pEl->ElTypeNumber,
			      pEl->ElStructSchema) ||
	    TypeHasException (EXC_ID_Colonne_Simple, pEl->ElTypeNumber,
			      pEl->ElStructSchema) ||
	    TypeHasException (EXC_ID_Colonne_Composee, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
	   /* c'est un pave de presentation d'un element En_Tetes */
	   /* Les_Colonnes, Colonne_Simple ou Colonne_Composee */
	  {
	     /* on cherche sa regle de hauteur */
	     pRegle = SearchRulepAb (pDoc, pAb, &pSPR, PtHeight, TRUE, &pAttr);
	     if (pRegle->PrDimRule.DrPosition)
		/* c'est une hauteur elastique */
		if (pRegle->PrDimRule.DrPosRule.PoRefElem)
		   if (TypeHasException (EXC_ID_BasTableau,
					 pRegle->PrDimRule.DrPosRule.PoTypeRefElem, pEl->ElStructSchema))
		      /* la regle exprime une position par rapport a un */
		      /* element Bas_tableau */
		      /*c'est bien un filet vertical, on reapplique la regle */
		      if (ApplyRule (pRegle, pSPR, pAb, pDoc, pAttr))
			{
			   /* indique que la hauteur du pave' a change' */
			   pAb->AbHeightChange = TRUE;
			   /* conserve le pointeur sur le pave a reafficher */
			   if (AssocView (pEl))
			      pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
				 Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->
							   ElAssocNum - 1]);
			   else
			      pDoc->DocViewModifiedAb[Vue - 1] =
				 Enclosing (pAb, pDoc->DocViewModifiedAb[Vue - 1]);
			}
	  }
     }
}


/* HauteurFilets        reapplique la regle de hauteur de tous les filets */
/* verticaux crees par les elements de l'entete qui precede l'element */
/* de type Bas_tableau pointe' par pBasTableau */

#ifdef __STDC__
void                HauteurFilets (PtrElement pBasTableau, PtrDocument pDoc)

#else  /* __STDC__ */
void                HauteurFilets (pBasTableau, pDoc)
PtrElement          pBasTableau;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrAbstractBox      pAb;
   int                 vue;
   int                 NType;
   boolean             stop;

   /* cherche l'element precedent de type En_Tetes */
   pEl = NULL;
   NType = GetElemWithException (EXC_ID_En_Tetes, pBasTableau->ElStructSchema);
   if (NType != 0)
      pEl = BackSearchTypedElem (pBasTableau, NType, pBasTableau->ElStructSchema);
   if (pEl != NULL)
      /* pEl est l'element En_Tetes dont on doit recalculer les filets */
      /* on examine tous les paves de l'element En_tetes dans toutes */
      /* vues du document */
      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	{
	   /* premier pave de l'element En_Tetes dans la vue courante */
	   pAb = pEl->ElAbstractBox[vue - 1];
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
		   HauteurPavesFilets (pAb, vue, pDoc);
		   /* passe au pave suivant */
		   pAb = pAb->AbNext;
		}
	   while (!stop);
	}
}


/* DetruitFiletLigne    detruit les boites de presentation Filet_Bas */
/* creees par la ligne de tableau qui precede le saut de page pointe' */
/* par pElPage. */

#ifdef __STDC__
static void         DetruitFiletLigne (PtrElement pElPage, PtrDocument pDoc)

#else  /* __STDC__ */
static void         DetruitFiletLigne (pElPage, pDoc)
PtrElement          pElPage;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   int                 vue;
   PtrAbstractBox      pAb;
   boolean             trouve;

   /* supprime le filet horizontal du bas de la ligne precedente */
   pEl = pElPage->ElPrevious;	/* element precedent le saut de page */
   if (pEl != NULL)
      if (TypeHasException (EXC_ID_Ligne_Simple, pEl->ElTypeNumber, pEl->ElStructSchema) ||
	  TypeHasException (EXC_ID_Ligne_Composee, pEl->ElTypeNumber, pEl->ElStructSchema))
	 /* c'est bien une ligne de tableau */
	 /* on examine toutes les vues du document */
	 for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	   {
	      pAb = pEl->ElAbstractBox[vue - 1];	/* 1er pave de la ligne */
	      trouve = FALSE;
	      while (pAb != NULL && !trouve)
		 if (pAb->AbElement != pEl)
		    /* ce pave n'appartient pas a la ligne, fin de la */
		    /* recherche */
		    pAb = NULL;
		 else
		   {
		      if (pAb->AbPresentationBox)
			 if (pAb->AbLeafType == LtGraphics)
			    /* c'est un pave de presentation graphique */
			    trouve = TRUE;
		      if (!trouve)
			 /* passe au pave suivant */
			 pAb = pAb->AbNext;
		   }
	      if (trouve)
		{
		   /* detruit le pave trouve' */
		   SetDeadAbsBox (pAb);
		   /* conserve le pointeur sur le pave a reafficher */
		   if (AssocView (pEl))
		      pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
			 Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->
							   ElAssocNum - 1]);
		   else
		      pDoc->DocViewModifiedAb[vue - 1] =
			 Enclosing (pAb, pDoc->DocViewModifiedAb[vue - 1]);
		}
	   }
}


/* ChercheRepetition cherche dans le sous arbre de l'element ACouper un element :
 *            - portant une exception TypeExcept
 *              - de meme schemas de structure
 *              - qui n'est pas une copie
 * La recherche n'explore pas les elements :
 *              - portant une exception PageBreak
 *              - qui sont des natures
 *              - qui sont des copies
 * La recherche s'arrete sur l'element StopElem si Restrict = TRUE
 * Renvoi un pointeur sur l'element trouve ou NULL si rien n'a ete trouve
 */

#ifdef __STDC__
static PtrElement   ChercheRepetition (PtrElement ACouper, PtrElement StopElem, int TypeExcept, boolean Restrict)

#else  /* __STDC__ */
static PtrElement   ChercheRepetition (ACouper, StopElem, TypeExcept, Restrict)
PtrElement          ACouper;
PtrElement          StopElem;
int                 TypeExcept;
boolean             Restrict;

#endif /* __STDC__ */

{
   PtrElement          pE, Repetition;

   Repetition = NULL;

   if (ACouper != NULL && !ACouper->ElTerminal)
     {
	for (pE = ACouper->ElFirstChild; pE != NULL; pE = pE->ElNext)
	  {
	     if (Restrict && pE == StopElem)	/* on arrete la recherche */
		break;
	     else
	       {
		  if (pE->ElStructSchema == ACouper->ElStructSchema)	/* meme schema de structure */
		    {
		       if (pE->ElSource == NULL)	/* ce n'est pas une copie */
			 {
			    if (TypeHasException (TypeExcept, pE->ElTypeNumber, pE->ElStructSchema))	/* exception ok */
			      {
				 Repetition = pE;	/* c'est donc le bon */
				 break;
			      }
			    else
			       /* recursion si l'element ne porte pas l'exception ExcPageBreak */ if (!TypeHasException (ExcPageBreak, pE->ElTypeNumber, pE->ElStructSchema))
			       ChercheRepetition (pE, StopElem, TypeExcept, Restrict);
			 }
		    }
	       }
	  }
     }

   return Repetition;
}


/* Exc_Page_Break_Inserer       Si pElPage est une marque de page dans une */
/* structure demandant une coupure spe'ciale, cree les elements a repeter devant et derriere le saut de page */
/* puis cree les paves de ces elements ainsi que ceux du saut de page */
/* et retourne Vrai. */
/* Si on n'est pas dans une structure a coupure speciale, ne fait rien et retourne Faux. */

#ifdef __STDC__
void                Exc_Page_Break_Inserer (PtrElement pElPage, PtrDocument pDoc, int VueNb, boolean * coupe)

#else  /* __STDC__ */
void                Exc_Page_Break_Inserer (pElPage, pDoc, VueNb, coupe)
PtrElement          pElPage;
PtrDocument         pDoc;
int                 VueNb;
boolean            *coupe;

#endif /* __STDC__ */

{
   PtrElement          pEl, pACopier, pSpecial;
   boolean             fin;

   *coupe = FALSE;
   /* cherche les ascendants qui demandent un traitement special des */
   /* coupures par saut de page */
   pSpecial = pElPage->ElParent;
   fin = FALSE;
   while (!fin)
     {
	if (pSpecial == NULL)
	   fin = TRUE;
	else
	  {
	     if (TypeHasException (ExcPageBreak, pSpecial->ElTypeNumber,
				   pSpecial->ElStructSchema))
		/* cet element demande une coupure speciale */
	       {
		  *coupe = TRUE;
		  /* cas particulier des tableaux */
		  if (TypeHasException (EXC_ID_Les_Lignes, pElPage->ElParent->ElTypeNumber,
					pElPage->ElParent->ElStructSchema))
		     /* supprime le filet horizontal du bas de la ligne precedente */
		     if (VueNb == 0)	/* inutile lors de la pagination */
			DetruitFiletLigne (pElPage, pDoc);

		  /*cherche l'element a repeter */
		  pACopier = ChercheRepetition (pSpecial, pElPage, ExcPageBreakRepBefore, FALSE);
		  if (pACopier != NULL)
		     /* il y a bien un element a repeter avant le saut de page */
		    {
		       pEl = NewSubtree (pACopier->ElTypeNumber, pACopier->ElStructSchema,
				    pDoc, pACopier->ElAssocNum, FALSE, TRUE,
					 TRUE, TRUE);
		       GetReference (&pEl->ElSource);
		       if (pEl->ElSource == NULL)
			  DeleteElement (&pEl);
		       else
			 {
			    pEl->ElSource->RdElement = pEl;
			    pEl->ElSource->RdTypeRef = RefInclusion;
			    /* insere l'element cree' dans l'arbre abstrait */
			    InsertElementBefore (pElPage, pEl);
			    /* lie l'inclusion a l'element trouve' */
			    if (SetReference (pEl, NULL, pACopier, pDoc, pDoc, FALSE, FALSE))
			       /* l'element a inclure est connu, on le copie */
			       CopyIncludedElem (pEl, pDoc);
			    /* cree les paves du nouvel element */
			    CreateAllAbsBoxesOfEl (pEl, pDoc);
			 }
		       if (VueNb == 0)
			  /* cas particulier des tableaux */
			  if (TypeHasException (EXC_ID_Les_Lignes, pElPage->ElParent->ElTypeNumber,
					 pElPage->ElParent->ElStructSchema))
			     /* reapplique les regles de hauteur des filets verticaux */
			     HauteurFilets (pEl, pDoc);
		    }
		  if (VueNb > 0)
		     /* cree les paves du saut de page */
		     CreateNewAbsBoxes (pElPage, pDoc, VueNb);

		  /*cherche l'element a repeter */
		  pACopier = ChercheRepetition (pSpecial, pElPage, ExcPageBreakRepetition, FALSE);
		  if (pACopier != NULL)
		     /* il y a bien un element a repeter apres le saut de page */
		    {
		       pEl = NewSubtree (pACopier->ElTypeNumber, pACopier->ElStructSchema,
				    pDoc, pACopier->ElAssocNum, FALSE, TRUE,
					 TRUE, TRUE);
		       /* associe un bloc reference a l'element cree' */
		       GetReference (&pEl->ElSource);
		       if (pEl->ElSource == NULL)
			  DeleteElement (&pEl);
		       else
			 {
			    pEl->ElSource->RdElement = pEl;
			    pEl->ElSource->RdTypeRef = RefInclusion;
			    /* insere l'element cree dans l'arbre abstrait */
			    InsertElementAfter (pElPage, pEl);
			    /* lie l'inclusion a l'element trouve' */
			    if (SetReference (pEl, NULL, pACopier, pDoc, pDoc, FALSE, FALSE))
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
}				/* End Of Exc_Page_Break_Inserer */


/* Exc_Page_Break_Supprime      Si l'element saut de page pointe' par */
/* pElPage est dans une structure a coupure speciale, supprime les */
/* elements repetes qui precedent et qui suivent. */

#ifdef __STDC__
void                Exc_Page_Break_Supprime (PtrElement pElPage, PtrDocument pDoc)

#else  /* __STDC__ */
void                Exc_Page_Break_Supprime (pElPage, pDoc)
PtrElement          pElPage;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          Prec, Prec1, Suiv, Suiv1;
   boolean             stop;

   /* supprime les elements repetes precedents */
   Prec = pElPage->ElPrevious;
   stop = FALSE;
   while (!stop)
     {
	if (Prec == NULL)
	   stop = TRUE;		/* pas d'autre element precedent */
	else if (!TypeHasException (ExcPageBreakRepBefore, Prec->ElTypeNumber,
				    Prec->ElStructSchema))
	   /* l'element precedent n'est pas une repetition */
	   stop = TRUE;
	else if (Prec->ElSource == NULL)
	   /* l'element precedent n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* il faut supprimer cet element precedent */
	  {
	     Prec1 = Prec->ElPrevious;
	     DeleteElement (&Prec);
	     Prec = Prec1;
	  }
     }
   /* supprime les elements repetes suivants */
   Suiv = pElPage->ElNext;
   stop = FALSE;
   while (!stop)
     {
	if (Suiv == NULL)
	   stop = TRUE;		/* pas d'autre element suivant */
	else if (!TypeHasException (ExcPageBreakRepetition, Suiv->ElTypeNumber,
				    Suiv->ElStructSchema))
	   /* l'element suivant n'est pas une repetition */
	   stop = TRUE;
	else if (Suiv->ElSource == NULL)
	   /* l'element suivant n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* il faut supprimer cet element suivant */
	  {
	     Suiv1 = Suiv->ElNext;
	     DeleteElement (&Suiv);
	     Suiv = Suiv1;
	  }
     }
}				/* End of Exc_Page_Break_Supprime */


/* Exc_Page_Break_Detruit_Pave  Si l'element saut de page pointe' */
/* par pElPage est dans une structure demandant une coupure speciale, */
/* detruit les paves des elements repetes qui precedent et ceux des */
/* elements repetes qui suivent. */

#ifdef __STDC__
void                Exc_Page_Break_Detruit_Pave (PtrElement pElPage, PtrDocument pDoc, int VueNb)

#else  /* __STDC__ */
void                Exc_Page_Break_Detruit_Pave (pElPage, pDoc, VueNb)
PtrElement          pElPage;
PtrDocument         pDoc;
int                 VueNb;

#endif /* __STDC__ */

{
   PtrElement          Prec, Suiv;
   boolean             stop;

   /* detruit les paves des elements repetes qui precedent */
   Prec = pElPage->ElPrevious;
   stop = FALSE;
   while (!stop)
     {
	if (Prec == NULL)
	   stop = TRUE;		/* pas d'autre element precedent */
	else if (!TypeHasException (ExcPageBreakRepBefore, Prec->ElTypeNumber,
				    Prec->ElStructSchema))
	   /* l'element precedent n'est pas une repetition */
	   stop = TRUE;
	else if (Prec->ElSource == NULL)
	   /* l'element precedent n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* c'est bien un element repete', on detruit ses paves */
	  {
	     DestroyAbsBoxesView (Prec, pDoc, FALSE, VueNb);
	     Prec = Prec->ElPrevious;
	  }
     }
   /* detruit les paves des elements repetes qui suivent */
   Suiv = pElPage->ElNext;
   stop = FALSE;
   while (!stop)
     {
	if (Suiv == NULL)
	   stop = TRUE;		/* pas d'autre element suivant */
	else if (!TypeHasException (ExcPageBreakRepetition, Suiv->ElTypeNumber,
				    Suiv->ElStructSchema))
	   /* l'element suivant n'est pas une repetition */
	   stop = TRUE;
	else if (Suiv->ElSource == NULL)
	   /* l'element suivant n'est pas une inclusion */
	   stop = TRUE;
	else
	   /* c'est bien un element repete', on detruit ses paves */
	  {
	     DestroyAbsBoxesView (Suiv, pDoc, FALSE, VueNb);
	     Suiv = Suiv->ElNext;
	  }
     }
}				/* End Of Exc_Page_Break_Detruit_Pave */

/* End Of Module tableau2 */
