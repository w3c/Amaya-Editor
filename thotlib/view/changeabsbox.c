/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * -- Modifications des images abstraites.
 * Ce module effectue les modifications des images abstraites des documents
 * et demande au Mediateur le reaffichage incremental. 
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "page_tv.h"
#include "select_tv.h"

#include "absboxes_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "exceptions_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "structlist_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "writepivot_f.h"

static PtrAbstractBox pAbbBegin[MAX_VIEW_DOC];

/*----------------------------------------------------------------------
   IsEnclosing retourne vrai si le pave pAbb1 englobe le pave pAbb2   
   ou si les deux pointeurs pointent le meme pave.         
  ----------------------------------------------------------------------*/
static ThotBool     IsEnclosing (PtrAbstractBox pAbb1, PtrAbstractBox pAbb2)
{
   ThotBool            ret;
   PtrAbstractBox      pAbb;

   if (pAbb2 == NULL || pAbb1 == pAbb2)
      ret = TRUE;
   else
     {
	ret = FALSE;
	pAbb = pAbb2;
	do
	   if (pAbb->AbEnclosing == pAbb1)
	      ret = TRUE;
	   else
	      pAbb = pAbb->AbEnclosing;
	while (!ret && pAbb != NULL);
     }
   return ret;
}

/*----------------------------------------------------------------------
   Enclosing retourne un pointeur sur le pave de plus bas niveau   
   qui englobe a la fois les deux paves pAbb1 et pAbb2.    
  ----------------------------------------------------------------------*/
PtrAbstractBox      Enclosing (PtrAbstractBox pAbb1, PtrAbstractBox pAbb2)
{
   PtrAbstractBox      pAbb;
   ThotBool            found;

   if (pAbb1 == NULL)
      pAbb = pAbb2;
   else if (pAbb2 == NULL)
      pAbb = pAbb1;
   else if (pAbb1 == pAbb2)
      pAbb = pAbb1;
   else
     {
	pAbb = pAbb1;
	do
	  {
	     found = IsEnclosing (pAbb, pAbb2);
	     if (!found)
		pAbb = pAbb->AbEnclosing;
	  }
	while (!found && pAbb != NULL);
     }
   return pAbb;
}

/*----------------------------------------------------------------------
   UpdateAbsBoxVolume
   update the volume of the abstract box of element pEl in view view.
  ----------------------------------------------------------------------*/
void         UpdateAbsBoxVolume (PtrElement pEl, int view, PtrDocument pDoc)
{
  int	          dVol;
  PtrAbstractBox  pAb;

  pAb = pEl->ElAbstractBox[view];
  if (pAb != NULL)
    {
      pAb->AbChange = TRUE;
      if (pDoc->DocView[view].DvPSchemaView > 0)
	pDoc->DocViewModifiedAb[view] = Enclosing (pAb,
					       pDoc->DocViewModifiedAb[view]);
      dVol = pAb->AbVolume - pEl->ElTextLength;
      do
	{
	  pAb->AbVolume -= dVol;
	  pAb = pAb->AbEnclosing;
	}
      while (pAb != NULL);
    }
}

/*----------------------------------------------------------------------
   SimpleSearchRulepEl cherche dans la chaine pRule la regle du       
   type typeRule a appliquer pour la vue view.                
  ----------------------------------------------------------------------*/
static void SimpleSearchRulepEl (PtrPRule * pRuleView1, PtrElement pEl,
				 int view, PRuleType typeRule,
				 FunctionType typeFunc, PtrPRule *pRule,
				 PtrDocument pDoc)
{
  PtrPRule   pR;

  pR = *pRule;
  *pRule = NULL;
  *pRuleView1 = NULL;
  while (pR != NULL)
    {
      if (pR->PrType == typeRule &&
	  (typeRule != PtFunction || typeFunc == FnAny || pR->PrPresFunction == typeFunc))
	{
	  /* regle du type cherche' */
	  if (pR->PrViewNum == view &&	    /* pour la vue voulue */
	      (pR->PrCond == NULL ||
	       CondPresentation (pR->PrCond, pEl, NULL, NULL, view,
				 pEl->ElStructSchema, pDoc)))
	    /* les conditions d'application de la regle sont satisfaites */
	    /* cette regle convient, a moins qu'on en trouve une autre */
	    /* plus specifique dans les regles qui suivent */
	    *pRule = pR;
	  else
	    {
	      if (pR->PrViewNum == 1 && *pRuleView1 == NULL &&
		  /* regle du type cherche' pour la vue 1 */
		  (pR->PrCond == NULL ||
		   CondPresentation (pR->PrCond, pEl, NULL, NULL, view,
				     pEl->ElStructSchema, pDoc)))
		/* les conditions d'application de la regle sont satisfaites*/
		/* on la garde pour le cas ou on ne trouve pas mieux */
		*pRuleView1 = pR;
	    }
	  /* regle suivante */
	  pR = pR->PrNextPRule;
	}
      else if (pR->PrType > typeRule)
	/* ce n'est pas le type de regle cherche' */
	/* il n'y a plus de regle de ce type, stop */
	pR = NULL;
      else
	/* regle suivante */
	pR = pR->PrNextPRule;
    }
}

/*----------------------------------------------------------------------
   GlobalSearchRulepEl returns the presentation rule of type (typeRule or
  typeFunc) that applies to the element pEl in the view.
  If the parameter presNum is NULL, the applies to the element itself,
  else the rule applie to that presentation box.
  In the second case, pSchP gives the presentation schema where the
  presentation box is defined (NULL = main presntation schema).
  When pEl refers a page, if isElPage is TRUE, the rule applies to the
  page break element, else the rule applies to the the enclosing block
  (page bottom + page break + page top).
  The parameter attr says if attributes are taken or not into account.
  Return:
  The rule and pSPR points to the presentation schema that gives the rule.
  If the returned rule is associated to an attribute, pAttr points this
  attribute.
  ----------------------------------------------------------------------*/
PtrPRule GlobalSearchRulepEl (PtrElement pEl, PtrDocument pDoc, PtrPSchema *pSPR,
			      PtrSSchema *pSSR, int presNum, PtrPSchema pSchP,
			      int view, PRuleType typeRule,
			      FunctionType typeFunc, ThotBool isElPage,
			      ThotBool attr, PtrAttribute * pAttr)
{
  int                 index;
  PtrPRule            pRule, pRuleSpecView1, pRuleView1;
  PtrSSchema          pSchS;
  ThotBool            stop;
  PtrPRule            pR;
  PtrAttribute        pA;
  PtrElement          pElAttr;
  InheritAttrTable   *inheritTable;
  PtrPSchema          pSP;
  PtrHandlePSchema    pHd;
  int                 l;

  pRule = NULL;
  *pAttr = NULL;
  *pSPR = NULL;
  if (pEl != NULL && PresentationSchema (pEl->ElStructSchema, pDoc) != NULL)
    {
      /* cherche d'abord parmi les regles de presentation specifique */
      /* associees a l'element, sauf s'il s'agit d'un pave de presentation */
      if (presNum == 0)
	{
	  pRule = pEl->ElFirstPRule;
	  stop = FALSE;
	  while (pRule != NULL && !stop)
	    if (pRule->PrType == typeRule &&
		(typeRule != PtFunction || typeFunc == FnAny ||
		 pRule->PrPresFunction == typeFunc) &&
		pRule->PrViewNum == view)
	      stop = TRUE;
	    else
	      /* regle suivante */
	      pRule = pRule->PrNextPRule;
	  if (pRule != NULL)
	    {
	      /* si la regle est associee a un attribut, on retourne un */
	      /* pointeur sur cet attribut */
	      if (pRule->PrSpecifAttr > 0)
		{
		  *pAttr = pEl->ElFirstAttr;
		  stop = FALSE;
		  while (*pAttr != NULL && !stop)
		    if ((*pAttr)->AeAttrNum == pRule->PrSpecifAttr &&
			(*pAttr)->AeAttrSSchema == pRule->PrSpecifAttrSSchema)
		      stop = TRUE;
		    else
		      *pAttr = (*pAttr)->AeNext;
		}
	    }
	}
      else
	/* les boites de presentation ne sont pas concernees par les */
	/* attributs */
	attr = FALSE;
      
      /* cherche parmi les regles de presentation des attributs de l'element,
	 si on n'a pas encore trouve' */
      if (pRule == NULL && attr)
	{
	  pA = pEl->ElFirstAttr;	/* premier attribut de l'element */
	  /* parcourt tous les attributs de l'element, de facon a garder la */
	  /* regle qui correspond au dernier attribut qui possede ce type de */
	  /* regle */
	  while (pA != NULL)
	    {
	      pR = NULL;
	      /* Si on travaille pour la vue principale, on traite d'abord les */
	      /* schemas de presentation additionnels les plus prioritaires, */
	      /* sinon on ignore les schemas additionnels. */
	      if (view == 1)
		pHd = FirstPSchemaExtension (pA->AeAttrSSchema, pDoc);
	      else
		pHd = NULL;
	      if (pHd == NULL)
		/* pas de schema additionnel. On prend le schema principal */
		pSP = PresentationSchema (pA->AeAttrSSchema, pDoc);
	      else
		/* on prend le schema additionnel le plus prioritaire */
		{
		  while (pHd->HdNextPSchema != NULL)
		    pHd = pHd->HdNextPSchema;
		  pSP = pHd->HdPSchema;
		}
	      while (pR == NULL && pSP != NULL)
		{
		  /* premiere regle de presentation de cet attribut */
		  pR = AttrPresRule (pA, pEl, FALSE, NULL, pSP);
		  /* parcourt les regles de presentation de l'attribut */
		  while (pR != NULL)
		    {
		      if (pR->PrType == typeRule &&
			  (typeRule != PtFunction || typeFunc == FnAny ||
			   pR->PrPresFunction == typeFunc) &&
			  pR->PrViewNum == view)
			/* regle du type cherche' pour la vue voulue */
			if (pR->PrCond == NULL ||
			    CondPresentation (pR->PrCond, pEl, pA, pEl, view,
					      pA->AeAttrSSchema, pDoc))
			  /* les conditions d'application de la regle sont */
			  /* satisfaites. On garde cette regle et on continue */
			  /* pour voir s'il y en a une plus specifique */
			  {
			    pRule = pR;
			    *pAttr = pA;
			    *pSPR = pSP;
			    *pSSR = pA->AeAttrSSchema;
			  }
		      if (pR->PrType <= typeRule)
			/* regle suivante */
			pR = pR->PrNextPRule;
		      else
			pR = NULL;
		    }
		  if (pR == NULL)
		    /* on n'a pas encore trouve'. On continue de chercher dans */
		    /* les schemas de presentation de moindre priorite' */
		    {
		      if (pHd == NULL)
			/* on cherchait dans le schema de presentation principal
			   c'est fini */
			pSP = NULL;
		      else
			{
			  /* on prend le schema de presentation additionnel de */
			  /* moindre priorite' */
			  pHd = pHd->HdPrevPSchema;
			  if (pHd == NULL)
			    /* plus de schemas additionnels, on prend le schema
			       principal */
			    pSP = PresentationSchema (pA->AeAttrSSchema, pDoc);
			  else
			    pSP = pHd->HdPSchema;
			}
		    }
		}
	      /* passe a l'attribut suivant de l'element */
	      pA = pA->AeNext;
	    }
	}

      /* cherche parmi les regles de presentation des attributs */
      /* herites par l'element, si on n'a pas encore trouve' */
      if (pRule == NULL)
	/* on n'applique pas aux boites de presentation les regles de */
	/* presentation des attributs herites */
	if (presNum == 0)
	  {
	    pSP = PresentationSchema (pEl->ElStructSchema, pDoc);
	    if (pSP->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	      {
		/* il y a heritage possible */
		if ((inheritTable = pSP->PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		  {
		    /* cette table n'existe pas on la genere */
		    CreateInheritedAttrTable (pEl, pDoc);
		    inheritTable = pSP->PsInheritedAttr[pEl->ElTypeNumber - 1];
		  }
		for (l = 1; l <= pEl->ElStructSchema->SsNAttributes &&
		       *pSPR == NULL; l++)
		  if ((*inheritTable)[l - 1] &&
		      (*pAttr = GetTypedAttrAncestor (pEl, l,
						      pEl->ElStructSchema,
						      &pElAttr)) != NULL)
		    /* pEl inherit from the attribute */
		    /* check if the attribute exists */
		    {
		      /* on traite d'abord les schemas de presentation */
		      /* additionnels les plus prioritaires si on travaille*/
		      /* pour la vue principale. Sinon, on ignore les */
		      /* schemas additionnels */
		      if (view == 1)
			pHd = FirstPSchemaExtension ((*pAttr)->AeAttrSSchema,
						     pDoc);
		      else
			pHd = NULL;
		      if (pHd == NULL)
			/* pas de schema additionnel. Prend le schema
			   principal */
			pSP = PresentationSchema ((*pAttr)->AeAttrSSchema,
						  pDoc);
		      else
			/* on cherche le schema additionnel le plus
			   proritaire */
			{
			  while (pHd->HdNextPSchema != NULL)
			    pHd = pHd->HdNextPSchema;
			  pSP = pHd->HdPSchema;
			}
		      while (pRule == NULL && pSP != NULL)
			{
			  /* premiere regle de presentation de cet
			     attribut dans ce schema de presentation */
			  pR = AttrPresRule (*pAttr, pEl, TRUE, NULL, pSP);
			  /* parcourt les regles de presentation de l'attr. */
			  while (pR != NULL)
			    {
			      if (pR->PrType == typeRule &&
				  (typeRule != PtFunction ||
				   typeFunc == FnAny ||
				   pR->PrPresFunction == typeFunc) &&
				  pR->PrViewNum == view)
				/*regle du type cherche' pour la vue voulue*/
				if (pR->PrCond == NULL ||
				    CondPresentation (pR->PrCond, pEl,
						      *pAttr, pElAttr, view,
						      (*pAttr)->AeAttrSSchema,
						      pDoc))
				  /*les conditions d'application de la regle
				    sont satisfaites. On garde cette regle et
				    on continue pour trouver les regles plus
				    specifiques */
				  {
				    pRule = pR;
				    *pSPR = pSP;
				    *pSSR = (*pAttr)->AeAttrSSchema;
				  }
			      if (pR->PrType <= typeRule)
				/* regle suivante */
				pR = pR->PrNextPRule;
			      else
				pR = NULL;
			    }
			  if (pRule == NULL)
			    /* on n'a pas encore trouve'. On continue de
			       chercher dans les schemas de presentation de
			       moindre priorite */
			    {
			      if (pHd == NULL)
				/* on cherchait dans le schema de
				   presentation principal. Fini */
				pSP = NULL;
			      else
				{
				  /* on prend le schema de presentation 
				     additionnel de moindre priorite' */
				  pHd = pHd->HdPrevPSchema;
				  if (pHd == NULL)
				    /* plus de schemas additionnels, on
				       prend le schema principal */
				    pSP = PresentationSchema ((*pAttr)->AeAttrSSchema, pDoc);
				  else
				    pSP = pHd->HdPSchema;
				}
			    }
			}
		    }
	      }
	  }

      if (pRule == NULL)
	{
	  /* on n'a pas encore trouve' */
	  *pAttr = NULL;
	  pSchS = pEl->ElStructSchema;
	  if (presNum == 0 && pEl->ElTerminal &&
	      pEl->ElLeafType == LtPageColBreak && isElPage)
	    {
	      /* on cherche le type de la boite page */
	      presNum = GetPageBoxType (pEl, pDoc, view, &pSchP);
	      pSchS = pEl->ElStructSchema;
	      index = pEl->ElTypeNumber;
	    }
	  else if (presNum == 0 || pSchP == NULL)
	    /* cherche le schema de presentation de l'element */
	    {
	      SearchPresSchema (pEl, &pSchP, &index, &pSchS, pDoc);
	      /***** s'il s'agit de l'element racine d'un objet d'une nature
		     differente de son pere, pSchP est le schema de presentation
		     de la nature englobante et view (numero de vue dans le
		     schema devrait etre mis a jour... */
	    }
	  /* on traite d'abord les schemas de presentation additionnels les */
	  /* plus prioritaires */
	  pHd = FirstPSchemaExtension (pSchS, pDoc);
	  if (pHd == NULL)
	    pSP = pSchP;
	  else
	    {
	      while (pHd->HdNextPSchema != NULL)
		pHd = pHd->HdNextPSchema;
	      pSP = pHd->HdPSchema;
	    }
	  
	  while (pRule == NULL && pSP != NULL)
	    {
	      /* premiere regle de presentation de ce type d'element */
	      if (pSP != NULL)
		{
		  if (presNum > 0)
		    pRule = pSP->PsPresentBox[presNum - 1].PbFirstPRule;
		  else
		    pRule = pSP->PsElemPRule[index - 1];
		}
	      /* cherche une regle du type voulu, pour la vue voulue, parmi */
	      /* les regles du type d'element */
	      pRuleView1 = NULL;
	      SimpleSearchRulepEl (&pRuleView1, pEl, view, typeRule, typeFunc,
				   &pRule, pDoc);
	      if (pRule == NULL)
		/* on n'a pas encore trouve'. On continue de chercher dans les */
		/* schemas de presentation de moindre priorite' */
		{
		  if (pHd == NULL)
		    /* on cherchait dans le schema de presentation principal. */
		    /* c'est fini */
		    pSP = NULL;
		  else
		    {
		      /* on prend le schema de presentation additionnel de moindre */
		      /* priorite' */
		      pHd = pHd->HdPrevPSchema;
		      if (pHd == NULL)
			/* plus de schemas additionnels, on prend le schema princ. */
			pSP = pSchP;
		      else
			pSP = pHd->HdPSchema;
		    }
		}
	    }
	  
	  if (pRule == NULL && pSchP != NULL)
	    {
	      /* on n'a pas trouve, cherche la regle par defaut de ce type */
	      pRuleSpecView1 = pRuleView1;
	      /* premiere regle de presentation par defaut */
	      pRule = pSchP->PsFirstDefaultPRule;
	      SimpleSearchRulepEl (&pRuleView1, pEl, view, typeRule, typeFunc,
				   &pRule, pDoc);
	      if (pRule == NULL)
		{
		  if (pRuleSpecView1 != NULL)
		    pRule = pRuleSpecView1;
		  else
		    pRule = pRuleView1;
		}
	    }
	  else if (pSP != NULL)
	    pSchP = pSP;
	  /* the rule is relative to an additional schema */
	  *pSPR = pSchP;
	  *pSSR = pSchS;
	}
    }
  return pRule;
}

/*----------------------------------------------------------------------
  SearchRulepAb returns the presentation rule of type (typeRule or
  typeFunc) that applies to the abstract box pAb.
  The parameter attr says if attributes are taken or not into account.
  Return:
  The rule and pSPR points to the presentation schema that gives the rule. 
  If the returned rule is associated to an attribute, pAttr points this
  attribute.
  ----------------------------------------------------------------------*/
PtrPRule SearchRulepAb (PtrDocument pDoc, PtrAbstractBox pAb,
			PtrPSchema * pSPR, PRuleType typeRule,
			FunctionType typeFunc, ThotBool attr,
			PtrAttribute * pAttr)
{
  PtrSSchema          pSSR;
  PtrPRule            pRuleFound;
  int                 presNum;
  
  if (pAb == NULL)
    pRuleFound = NULL;
  else
    {
      if (pAb->AbPresentationBox)
	presNum = pAb->AbTypeNum;
      else
	presNum = 0;
      pRuleFound = GlobalSearchRulepEl (pAb->AbElement, pDoc, pSPR, &pSSR, presNum,
					pAb->AbPSchema, pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView,
					typeRule, typeFunc, FALSE, attr, pAttr);
    }
  return pRuleFound;
}

/*----------------------------------------------------------------------
   IsDiffPosition retourne vrai si la position abPos est differente de 
   la position Posit (variable de la fonction NouvRef).    
  ----------------------------------------------------------------------*/
static ThotBool IsDiffPosition (AbPosition abPos, AbPosition * abPosit,
				ThotBool isInverted)
{
  AbPosition         *pAbbox1;
  ThotBool            different;

  pAbbox1 = &abPos;
  different = TRUE;
  if (pAbbox1->PosRefEdge == abPosit->PosRefEdge &&
      pAbbox1->PosRefEdge == abPosit->PosRefEdge &&
      pAbbox1->PosDistance == abPosit->PosDistance &&
      pAbbox1->PosUnit == abPosit->PosUnit &&
      pAbbox1->PosUserSpecified == abPosit->PosUserSpecified &&
      pAbbox1->PosAbRef == abPosit->PosAbRef)
    {
      if (!isInverted)
	{
	   if (pAbbox1->PosEdge == abPosit->PosEdge)
	      different = FALSE;
	}
      else
	{
	   if (pAbbox1->PosEdge == Right && abPosit->PosEdge == Left)
	      different = FALSE;
	   else if (pAbbox1->PosEdge == Left && abPosit->PosEdge == Right)
	      different = FALSE;
	   else if (pAbbox1->PosEdge == Top && abPosit->PosEdge == Bottom)
	      different = FALSE;
	   else if (pAbbox1->PosEdge == Bottom && abPosit->PosEdge == Top)
	      different = FALSE;
	}
    }
  return different;
}

/*----------------------------------------------------------------------
   IsDiffDimension retourne vrai si la dimension abDim est differente  
   de la dimension abDimens (variable de la fonction         
   NouvRef).                                               
  ----------------------------------------------------------------------*/
static ThotBool     IsDiffDimension (AbDimension abDim, AbDimension * abDimens)
{
   AbDimension        *pAbbox1;
   ThotBool            different;

   pAbbox1 = &abDim;
   if (pAbbox1->DimIsPosition)
      different = TRUE;
   else if (pAbbox1->DimAbRef == abDimens->DimAbRef &&
	    pAbbox1->DimValue == abDimens->DimValue &&
	    pAbbox1->DimUnit == abDimens->DimUnit &&
	    pAbbox1->DimUserSpecified == abDimens->DimUserSpecified &&
	    pAbbox1->DimSameDimension == abDimens->DimSameDimension)
      different = FALSE;
   else
      different = TRUE;
   return different;
}

/*----------------------------------------------------------------------
   IsNewPosOrDim cherche si la regle pointee par pR (qui appartient au   
   schema de presentation pointe' par pSPR) en s'appliquant
   au pave pAb change de valeur.                          
   Si oui, la fonction retourne vrai et applique la regle  
   a pAb, sinon elle retourne faux.                       
   levelPos indique la position relative de pRef par rapport    
   pAb.                                                   
   pAttr pointe le bloc attribut auquel correspond la regle
   pR (NULL si la regle n'est pas une regle d'attribut).  
  ----------------------------------------------------------------------*/
static ThotBool IsNewPosOrDim (PtrAbstractBox pAb, PtrPRule pR,
			       PtrPSchema pSPR, Level levelPos,
			       PtrDocument pDoc, PtrAttribute pAttr)
{
  ThotBool            ret;
  AbPosition          abPosit;
  AbDimension         Dimens;
  PosRule            *pRe1;
  DimensionRule      *pRelD1;
  PtrPRule            pRegleDim;
  PtrPSchema          pSPRDim;
  PtrAttribute        pAttrDim;
  AbPosition          abDimElast;
  ThotBool            nextToPage;

  ret = FALSE;
  if (pR != NULL)
    {
      /* s'il s'agit d'un positionnement vertical et que le pave precedent */
      /* est une marque de page, il faut reevaluer la regle */
      nextToPage = FALSE;
      if (pR->PrType == PtVertPos &&
	  pAb->AbPrevious != NULL &&
	  pAb->AbPrevious->AbElement->ElTypeNumber == PageBreak + 1)
	nextToPage = TRUE;
      switch (pR->PrType)
	{
	case PtVertRef:
	case PtHorizRef:
	case PtVertPos:
	case PtHorizPos:
	  pRe1 = &pR->PrPosRule;
	  if (pRe1->PoRelation == levelPos ||
	      pRe1->PoRelation == RlReferred ||
	      (pRe1->PoRelation == RlSameLevel &&
	       (levelPos == RlPrevious || levelPos == RlNext)) ||
	      nextToPage ||
	      (pR->PrType == PtVertPos && pAb->AbHeight.DimIsPosition) ||
	      (pR->PrType == PtHorizPos && pAb->AbWidth.DimIsPosition))
	    switch (pR->PrType)
	      {
	      case PtVertRef:
		abPosit = pAb->AbVertRef;
		(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		ret = IsDiffPosition (pAb->AbVertRef, &abPosit, FALSE);
		break;
	      case PtHorizRef:
		abPosit = pAb->AbHorizRef;
		(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		ret = IsDiffPosition (pAb->AbHorizRef, &abPosit, FALSE);
		break;
	      case PtVertPos:
		if (!pAb->AbHeight.DimIsPosition)
		  {
		    abPosit = pAb->AbVertPos;
		    (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		    ret = IsDiffPosition (pAb->AbVertPos, &abPosit, FALSE);
		  }
		else
		  /* on reevalue la dimension d'un pave elastique en */
		  /* meme temps que sa position */
		  {
		    /* on conserve la position et la dimension d'origine
		       du pave' */
		    abPosit = pAb->AbVertPos;
		    abDimElast = pAb->AbHeight.DimPosition;
		    /* applique la regle de position */
		    (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		    /* cherche et applique la regle de dimension */
		    pRegleDim = SearchRulepAb (pDoc, pAb, &pSPRDim,
					       PtHeight, FnAny, TRUE, &pAttrDim);
		    (void) ApplyRule (pRegleDim, pSPRDim, pAb, pDoc,
				      pAttrDim);
		    /* compare la position et la dimension d'origine avec
		       celles qui viennent d'etre calculees */
		    if (pAb->AbBox != NULL)
		      {
		      if (pAb->AbBox->BxVertInverted)
			/* le mediateur avait inverse' position et
			   dimension */
			{
			  /* compare avec inversion */
			  if (IsDiffPosition (pAb->AbVertPos, &abPosit, TRUE) ||
			      IsDiffPosition (pAb->AbHeight.DimPosition,
					      &abDimElast, TRUE))
			    /* il y a eu au moins un changement */
			    {
			      ret = TRUE;
			      pAb->AbHeightChange = TRUE;
			      pAb->AbVertPosChange = TRUE;
			      if (pAb->AbElement->ElTerminal &&
				  pAb->AbElement->ElLeafType == LtGraphics)
				/* restore the graphics */
				pAb->AbShape = pAb->AbElement->ElGraph;
			    }
			  else
			    /* pas de changement, on retablit position
			       et dimension d'origine */
			    {
			      ret = FALSE;
			      pAb->AbVertPos = abPosit;
			      pAb->AbHeight.DimPosition = abDimElast;
			    }
			}
		      else if (IsDiffPosition (pAb->AbVertPos, &abPosit, FALSE) ||
			    IsDiffPosition (pAb->AbHeight.DimPosition,
					    &abDimElast, FALSE))
			/* pas d'inversion position/dimension */
			  /* il y a eu au moins un changement */
			  {
			    ret = TRUE;
			    pAb->AbHeightChange = TRUE;
			    pAb->AbVertPosChange = TRUE;
			  }
			else
			  /* pas de changement */
			  ret = FALSE;
		      }
		  }
		break;
	      case PtHorizPos:
		if (!pAb->AbWidth.DimIsPosition)
		  {
		    abPosit = pAb->AbHorizPos;
		    (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		    ret = IsDiffPosition (pAb->AbHorizPos, &abPosit, FALSE);
		  }
		else
		  /* on reevalue la dimension d'un pave elastique en
		     meme temps que sa position */
		  {
		    /* on conserve la position et la dimension d'origine
		       du pave' */
		    abPosit = pAb->AbHorizPos;
		    abDimElast = pAb->AbWidth.DimPosition;
		    /* applique la regle de position */
		    (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		    /* cherche et applique la regle de dimension */
		    pRegleDim = SearchRulepAb (pDoc, pAb, &pSPRDim,
					       PtWidth, FnAny, TRUE, &pAttrDim);
		    (void) ApplyRule (pRegleDim, pSPRDim, pAb, pDoc,
				      pAttrDim);
		    /* compare la position et la dimension d'origine avec
		       celles qui viennent d'etre calculees */
		    if (pAb->AbBox != NULL)
		      {
			if (pAb->AbBox->BxHorizInverted)
			  /* le mediateur avait inverse' position et
			     dimension */
			  {
			    /* compare avec inversion */
			    if (IsDiffPosition (pAb->AbHorizPos, &abPosit, TRUE) ||
				IsDiffPosition (pAb->AbWidth.DimPosition,
						&abDimElast, TRUE))
			      /* il y a eu au moins un changement */
			      {
				ret = TRUE;
				pAb->AbWidthChange = TRUE;
				pAb->AbHorizPosChange = TRUE;
				if (pAb->AbElement->ElTerminal &&
				    pAb->AbElement->ElLeafType == LtGraphics)
				  /* restore the graphics */
				  pAb->AbShape = pAb->AbElement->ElGraph;
			      }
			    else
			      /* pas de changement, on retablit position et
				 dimension d'origine */
			      {
				ret = FALSE;
				pAb->AbHorizPos = abPosit;
				pAb->AbWidth.DimPosition = abDimElast;
			      }
			  }
			else if (IsDiffPosition (pAb->AbHorizPos, &abPosit, FALSE) ||
				 IsDiffPosition (pAb->AbWidth.DimPosition,
						 &abDimElast, FALSE))
			  /* il y a eu au moins un changement */
			  {
			    /* pas d'inversion position/dimension */
			    ret = TRUE;
			    pAb->AbWidthChange = TRUE;
			    pAb->AbHorizPosChange = TRUE;
			  }
			else
			  /* pas de changement */
			  ret = FALSE;
		      }
		  }
		break;
	      default:
		break;
	      }
	  break;
	  
	case PtHeight:
	case PtWidth:
	  /* sauve d'abord la dimension du pave' */
	  if (pR->PrType == PtHeight)
	    Dimens = pAb->AbHeight;
	  else
	    Dimens = pAb->AbWidth;
	  
	  /* traitement selon la regle de dimension */
	  pRelD1 = &pR->PrDimRule;
	  if (pRelD1->DrPosition)
	    {
	      /* regle de dimension elastique */
	      if (!Dimens.DimIsPosition)
		/* pave non elastique */
		/* on applique la regle */
		{
		  (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		  ret = TRUE;
		}
	    }
	  else if (!Dimens.DimIsPosition &&
		   !pRelD1->DrAbsolute &&
		   /* ce n'est pas une dimension absolue */
		   (pRelD1->DrRelation == levelPos ||
		    (pRelD1->DrRelation == RlSameLevel &&
		     (levelPos == RlPrevious || levelPos == RlNext))))
	    {
	      /* regle de dimension non elastique */
	      if (pR->PrType == PtHeight)
		/* hauteur */
		{
		  (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		  ret = IsDiffDimension (pAb->AbHeight, &Dimens);
		}
	      else
		/* largeur */
		{
		  (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
		  ret = IsDiffDimension (pAb->AbWidth, &Dimens);
		}
	    }
	  break;
	default:
	  break;
	}
    }
  return (ret);
}

/*----------------------------------------------------------------------
   ReapplRef reapplique toutes les regles appliquees au pave pAb  
   et qui font reference au pave pRef.                     
   Retourne vrai si au moins une regle a ete reappliquee.  
  ----------------------------------------------------------------------*/
static ThotBool ReapplRef (PtrAbstractBox pRef, PtrAbstractBox pAb,
			   PtrDocument pDoc)
{
   ThotBool            ret;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;
   PtrPRule            pR;
   PtrAbstractBox      pAbbox1;
   AbPosition         *pPosAb;
   AbDimension        *pDimAb;

   ret = FALSE;
   pAbbox1 = pAb;
   if (pAbbox1->AbVertRef.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbVertRef;
	/* prend d'abord la regle standard */
	pPosAb->PosEdge = VertRef;
	pPosAb->PosRefEdge = VertRef;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtVertRef, FnAny, TRUE, &pAttr);
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	pAbbox1->AbVertRefChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbHorizRef.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbHorizRef;
	/* prend d'abord la regle standard */
	pPosAb->PosEdge = HorizRef;
	pPosAb->PosRefEdge = HorizRef;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtHorizRef, FnAny, TRUE, &pAttr);
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	pAbbox1->AbHorizRefChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbHeight.DimIsPosition)
     {
	pPosAb = &pAbbox1->AbHeight.DimPosition;
	if (pPosAb->PosAbRef == pRef)
	  {
	     pPosAb->PosEdge = Top;
	     pPosAb->PosRefEdge = Top;
	     pPosAb->PosDistance = 0;
	     pPosAb->PosAbRef = NULL;
	     pPosAb->PosUserSpecified = FALSE;
	     pR = SearchRulepAb (pDoc, pAb, &pSPR, PtHeight, FnAny, TRUE, &pAttr);
	     (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	     pAbbox1->AbHeightChange = TRUE;
	     /* force la reevaluation de la position verticale : lorsqu'on */
	     /* change la dimension d'une boite elastique, il faut aussi */
	     /* faire reevaluer sa position par le mediateur. */
	     pAbbox1->AbVertPos.PosAbRef = pRef;
	     ret = TRUE;
	  }
     }
   else if (pAbbox1->AbHeight.DimAbRef == pRef)
     {
	pDimAb = &pAbbox1->AbHeight;
	pDimAb->DimValue = -1;
	pDimAb->DimAbRef = NULL;
	pDimAb->DimUnit = UnRelative;
	pDimAb->DimSameDimension = TRUE;
	pDimAb->DimUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtHeight, FnAny, TRUE, &pAttr);
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	pAbbox1->AbHeightChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbWidth.DimIsPosition)
     {
	pPosAb = &pAbbox1->AbWidth.DimPosition;
	if (pPosAb->PosAbRef == pRef)
	  {
	     pPosAb->PosEdge = Left;
	     pPosAb->PosRefEdge = Left;
	     pPosAb->PosDistance = 0;
	     pPosAb->PosAbRef = NULL;
	     pR = SearchRulepAb (pDoc, pAb, &pSPR, PtWidth, FnAny, TRUE, &pAttr);
	     pPosAb->PosUserSpecified = FALSE;
	     (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	     pAbbox1->AbWidthChange = TRUE;
	     /* force la reevaluation de la position horizontale : lorsqu'on */
	     /* change la dimension d'une boite elastique, il faut aussi */
	     /* faire reevaluer sa position par le mediateur. */
	     pAbbox1->AbHorizPos.PosAbRef = pRef;
	     ret = TRUE;
	  }
     }
   else if (pAbbox1->AbWidth.DimAbRef == pRef)
     {
	pDimAb = &pAbbox1->AbWidth;
	pDimAb->DimValue = -1;
	pDimAb->DimAbRef = NULL;
	pDimAb->DimUnit = UnRelative;
	pDimAb->DimSameDimension = TRUE;
	pDimAb->DimUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtWidth, FnAny, TRUE, &pAttr);
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	pAbbox1->AbWidthChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbVertPos.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbVertPos;
	pPosAb->PosEdge = Top;
	pPosAb->PosRefEdge = Top;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtVertPos, FnAny, TRUE, &pAttr);
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	pAbbox1->AbVertPosChange = TRUE;
	if (pAbbox1->AbHeight.DimIsPosition)
	   /* force la reevaluation de la hauteur : lorsqu'on */
	   /* change la position d'une boite elastique, il faut aussi */
	   /* faire reevaluer sa dimension par le mediateur. */
	   pAbbox1->AbHeightChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbHorizPos.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbHorizPos;
	pPosAb->PosEdge = Left;
	pPosAb->PosRefEdge = Left;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtHorizPos, FnAny, TRUE, &pAttr);
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
	pAbbox1->AbHorizPosChange = TRUE;
	if (pAbbox1->AbWidth.DimIsPosition)
	   /* force la reevaluation de la largeur : lorsqu'on */
	   /* change la position d'une boite elastique, il faut aussi */
	   /* faire reevaluer sa dimension par le mediateur. */
	   pAbbox1->AbWidthChange = TRUE;
	ret = TRUE;
     }
   return ret;
}

/*----------------------------------------------------------------------
   FunctionRule retourne le pointeur sur la premiere regle        
   de fonction de presentation associee a l'element pEl.   
   Rend dans pSchP un pointeur sur le schema de            
   presentation auquel appartient la regle.                
   Retourne NULL s'il n'y a pas de regle de creation pour  
   cet element                                             
  ----------------------------------------------------------------------*/
PtrPRule FunctionRule (PtrElement pEl, PtrPSchema * pSchP, PtrDocument pDoc)
{
   PtrPRule            pRule;
   int                 index;
   PtrSSchema          pSchS;

   pRule = NULL;
   SearchPresSchema (pEl, pSchP, &index, &pSchS, pDoc);
   if (*pSchP != NULL)
     {
	/* pRule : premiere regle de presentation specifique a ce type */
	/* d'element */
	pRule = (*pSchP)->PsElemPRule[index - 1];
	if (pRule != NULL)
	  {
	     while (pRule->PrType < PtFunction && pRule->PrNextPRule != NULL)
		pRule = pRule->PrNextPRule;
	     if (pRule->PrType != PtFunction)
		pRule = NULL;
	  }
     }
   return pRule;
}

/*----------------------------------------------------------------------
   SetDeadAbsBox marque Mort le pave pointe par pAb et met a jour       
   le volume des paves englobants. Le pave mort sera       
   detruit par la procedure AbstractImageUpdated, apres traitement     
   par le Mediateur.                                       
  ----------------------------------------------------------------------*/
void                SetDeadAbsBox (PtrAbstractBox pAb)
{
   int                 vol;
   PtrAbstractBox      pAbb;
   PtrAbstractBox      pAbbox1;

   pAb->AbDead = TRUE;
   /* met a jour le volume des paves englobants dans la vue */
   vol = pAb->AbVolume;
   if (vol > 0)
     {
	pAb->AbVolume = 0;
	pAbb = pAb->AbEnclosing;
	while (pAbb != NULL)
	  {
	     pAbbox1 = pAbb;
	     pAbbox1->AbVolume -= vol;
	     pAbb = pAbbox1->AbEnclosing;
	  }
     }
}

/*----------------------------------------------------------------------
   ApplyRefAbsBoxSupp cherche tous les paves qui font reference a un pave a
   detruire pointe par pAb, et pour ces paves reapplique  
   les regles qui font reference au pave a detruire.       
   Retourne dans pAbbReDisp un pointeur sur le pave a        
   reafficher.                                             
  ----------------------------------------------------------------------*/
void ApplyRefAbsBoxSupp (PtrAbstractBox pAb, PtrAbstractBox * pAbbReDisp,
			 PtrDocument pDoc)
{
   PtrAbstractBox      pAbb;

   *pAbbReDisp = pAb;
   /* cherche d'abord dans le pave englobant */
   pAbb = pAb->AbEnclosing;
   if (pAbb != NULL)
      if (ReapplRef (pAb, pAbb, pDoc))
	 *pAbbReDisp = pAbb;
   /* cherche dans les paves precedents */
   pAbb = pAb->AbPrevious;
   while (pAbb != NULL)
     {
	if (!pAbb->AbDead)
	   if (ReapplRef (pAb, pAbb, pDoc))
	      *pAbbReDisp = pAbb->AbEnclosing;
	pAbb = pAbb->AbPrevious;
	/* cherche dans les paves suivants */
     }
   pAbb = pAb->AbNext;
   while (pAbb != NULL)
     {
	if (!pAbb->AbDead)
	   if (ReapplRef (pAb, pAbb, pDoc))
	      *pAbbReDisp = pAbb->AbEnclosing;
	pAbb = pAbb->AbNext;
     }
}

/*----------------------------------------------------------------------
   ApplyRefAbsBoxNew La suite de paves comprise entre pAbbFirst et pAbbLast   
   vient d'etre creee, modifie les paves environnants qui  
   peuvent se referer aux nouveaux paves.                  
   Retourne dans pAbbReDisp un pointeur sur le pave qui      
   englobe tous les paves modifies, y compris les nouveaux 
   paves.                                                  
  ----------------------------------------------------------------------*/
void ApplyRefAbsBoxNew (PtrAbstractBox pAbbFirst, PtrAbstractBox pAbbLast,
			PtrAbstractBox * pAbbReDisp, PtrDocument pDoc)
{
   PtrAbstractBox      pAbb;
   PtrPRule            pRule;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;

   /* cherche la pave minimum a reafficher a priori */
   if (pAbbLast == pAbbFirst)
      *pAbbReDisp = pAbbFirst;
   /* on reaffichera le pave unique */
   else
      *pAbbReDisp = pAbbFirst->AbEnclosing;
   /* il y a plusieurs paves pour cet */
   /* element, on reaffichera le pave englobant */
   /* cherche les paves environnants pour trouver ceux qui se referent aux */
   /* nouveaux paves */
   /* traite d'abord le pave englobant */
   pAbb = pAbbFirst->AbEnclosing;
   if (pAbb != NULL)
     {
	/* il y a un pave englobant */
	pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtVertRef, FnAny, TRUE, &pAttr);
	if (IsNewPosOrDim (pAbb, pRule, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *pAbbReDisp = pAbb;
	     pAbb->AbVertRefChange = TRUE;
	  }
	pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtHorizRef, FnAny, TRUE, &pAttr);
	if (IsNewPosOrDim (pAbb, pRule, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *pAbbReDisp = pAbb;
	     pAbb->AbHorizRefChange = TRUE;
	  }
	pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtHeight, FnAny, TRUE, &pAttr);
	if (IsNewPosOrDim (pAbb, pRule, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *pAbbReDisp = pAbb;
	     pAbb->AbHeightChange = TRUE;
	  }
	pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtWidth, FnAny, TRUE, &pAttr);
	if (IsNewPosOrDim (pAbb, pRule, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *pAbbReDisp = pAbb;
	     pAbb->AbWidthChange = TRUE;
	  }
     }
   /* cherche dans les paves precedents */
   if (pAbbFirst->AbPrevious != NULL)
     {
	pAbb = pAbbFirst->AbPrevious;
	do
	  {
	     if (!pAbb->AbDead)
		/* on ne traite pas les paves morts */
	       {
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtVertPos, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlNext, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbVertPosChange = TRUE;
		    }
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtHorizPos, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlNext, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbHorizPosChange = TRUE;
		    }
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtHeight, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlNext, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbHeightChange = TRUE;
		    }
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtWidth, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlNext, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbWidthChange = TRUE;
		    }
	       }
	     pAbb = pAbb->AbPrevious;
	  }
	while (pAbb != NULL);
     }
   /* cherche dans les paves suivants */
   if (pAbbLast->AbNext != NULL)
     {
	pAbb = pAbbLast->AbNext;
	do
	  {
	     if (!pAbb->AbDead)
	       {
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtVertPos, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbVertPosChange = TRUE;
		    }
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtHorizPos, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbHorizPosChange = TRUE;
		    }
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtHeight, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbHeightChange = TRUE;
		    }
		  pRule = SearchRulepAb (pDoc, pAbb, &pSPR, PtWidth, FnAny, TRUE, &pAttr);
		  if (IsNewPosOrDim (pAbb, pRule, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *pAbbReDisp = pAbb->AbEnclosing;
		       pAbb->AbWidthChange = TRUE;
		    }
	       }
	     pAbb = pAbb->AbNext;
	  }
	while (pAbb != NULL);
     }
}

/*----------------------------------------------------------------------
   AbsBoxPresType     cherche le pave de presentation de type         
   BoxType (defini dans le schema de presentation pSchP) 
   qui a ete cree dans la meme vue que le pave pAbb,         
   pour l'element de pAbb.                                   
   On considere le premier pave (pAbb) et le dernier pave    
   de la chaine des paves dupliques de l'element           
   Retourne un pointeur sur ce pave ou NULL si pas trouve'.
  ----------------------------------------------------------------------*/
static PtrAbstractBox AbsBoxPresType (PtrAbstractBox pAbb, PtrPRule pRPres,
				      PtrPSchema pSchP)
{
   PtrAbstractBox      pAb, pAbbMain;
   ThotBool            found, stop;	/* 1er pave de l'element dans la vue */
   int                 boxType;
   PtrAbstractBox      pAbbPres;


   boxType = pRPres->PrPresBox[0];	/* numero de type de la boite cherchee */
   /* Cas particulier des paves de presentation de la racine */
   /* TO DO ? */
   pAb = pAbb;
   found = FALSE;
   /* cherche d'abord le pave principal de l'element */
   pAbbMain = NULL;
   stop = FALSE;
   do
      if (pAb == NULL)
	 stop = TRUE;		/* pas trouve' */
      else if (pAb->AbElement != pAbb->AbElement)
	 stop = TRUE;		/* ce n'est pas un pave de l'element */
      else if (pAb->AbPresentationBox)
	 /* c'est un pave' de presentation, on le saute */
	 pAb = pAb->AbNext;
      else
	{
	   /* premier pave' de l'element qui ne soit pas un pave' de */
	   /* presentation, c'est le pave' principal */
	   pAbbMain = pAb;
	   stop = TRUE;
	}
   while (!stop);
   if (pAbbMain != NULL)
      /* on a trouve' le pave principal */
     {
	/* positionne pAb sur le 1er pave a tester selon la regle de creation */
	if (pRPres->PrPresFunction == FnCreateFirst || pRPres->PrPresFunction == FnCreateLast)
	  {
	     pAb = pAbbMain->AbFirstEnclosed;	/* 1er descendant */
	     if (pRPres->PrPresFunction == FnCreateLast)
		/* cherche le dernier descendant du pave principal */
		if (pAb != NULL)
		   while (pAb->AbNext != NULL)
		      pAb = pAb->AbNext;
	  }
	else if (pRPres->PrPresFunction == FnCreateBefore)
	   pAb = pAbbMain->AbPrevious;
	else if (pRPres->PrPresFunction == FnCreateAfter || pRPres->PrPresFunction == FnCreateWith)
	   pAb = pAbbMain->AbNext;
	else
	   pAb = NULL;
	/* lance la recherche */
	while (pAb != NULL && !found)
	   if (pAb->AbElement != pAbb->AbElement)
	      /* ce pave n'appartient pas au meme element, echec */
	      pAb = NULL;
	   else
	      /* ce pave' appartient a l'element */
	      if (pAb->AbPresentationBox && pAb->AbTypeNum == boxType
		  && pAb->AbPSchema == pSchP)
	      found = TRUE;	/* pave' du type cherche': on a trouve' */
	   else
	      /* passe au suivant dans la bonne direction */
	      if (pRPres->PrPresFunction == FnCreateFirst ||
		  pRPres->PrPresFunction == FnCreateAfter)
	      pAb = pAb->AbNext;
	   else
	      pAb = pAb->AbPrevious;
     }
   if (found)
      pAbbPres = pAb;
   else
      pAbbPres = NULL;
   return pAbbPres;
}

/*----------------------------------------------------------------------
   ApplFunctionPresRules                                               
  ----------------------------------------------------------------------*/
static void ApplFunctionPresRules (PtrPRule pRule, PtrPSchema pSchP,
				   PtrAttribute pAttr, PtrDocument pDoc,
				   PtrElement pEl, ThotBool change,
				   ThotBool first)
{
   int                 view;
   PtrAbstractBox      pAb, pAbbReDisp;
   ThotBool            stop;
   PtrPRule            pRe1;
   PtrCondition        pCond;

   if (pRule != NULL)
      /* la chaine des regles de presentation n'est pas vide */
      if (pRule->PrType == PtFunction)
	 /* la premiere regle de la chaine est bien une fonction */
	 /* cherche parmi les fonctions les regles de creation conditionelle */
	 do
	   {
	      pRe1 = pRule;
	      if (pRe1->PrPresFunction == FnCreateBefore
		  || pRe1->PrPresFunction == FnCreateWith
		  || pRe1->PrPresFunction == FnCreateFirst
		  || pRe1->PrPresFunction == FnCreateLast
		  || pRe1->PrPresFunction == FnCreateAfter)
		 /* c'est une regle de creation */
		{
		   pCond = pRe1->PrCond;
		   while (pCond != NULL)
		     {
			/* verifie toutes les conditions qui s'appliquent a cette regle */

			if ((pCond->CoCondition == PcFirst && first) ||
			    (pCond->CoCondition == PcLast && !first))
			  {
			     if ((!pCond->CoNotNegative && change) ||
				 (pCond->CoNotNegative && !change))
				/* il faut creer la boite de presentation */
				/* dans toutes les vues, modifie les paves existants en */
				/* fonction des nouveaux paves crees. */
				for (view = 1; view <= MAX_VIEW_DOC; view++)
				   /* cree le pave de presentation dans la vue */
				  {
				     pAb = CrAbsBoxesPres (pEl, pDoc, pRule,
							   pEl->ElStructSchema,
					             pAttr, view, pSchP, TRUE);
				     if (pAb != NULL)
					/* un pave de presentation a ete cree */
					/* change les regles des autres paves qui peuvent */
					/* se referer au pave cree' */
				       {
					  ApplyRefAbsBoxNew (pAb, pAb, &pAbbReDisp, pDoc);
					  pAbbReDisp = Enclosing (pAb, pAbbReDisp);
					  /* conserve le pointeur sur le pave a reafficher */
					  pDoc->DocViewModifiedAb[view - 1] =
						Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[view - 1]);
				       }
				  }
			     else
				/* il faut detruire la boite de presentation */
				/* cherche dans toutes les vues le pave qui a ete cree */
				/* par cette regle */
				for (view = 1; view <= MAX_VIEW_DOC; view++)
				  {
				     pAb = AbsBoxPresType (pEl->ElAbstractBox[view - 1], pRe1, pSchP);
				     if (pAb != NULL)
					/* detruit le pave trouve' */
				       {
					  SetDeadAbsBox (pAb);
					  /* change les regles des autres paves qui se */
					  /* referent au pave detruit */
					  ApplyRefAbsBoxSupp (pAb, &pAbbReDisp, pDoc);
					  pAbbReDisp = Enclosing (pAb, pAbbReDisp);
					  /* conserve le pointeur sur le pave a reafficher */
					  pDoc->DocViewModifiedAb[view - 1] =
						Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[view - 1]);
				       }
				  }
			  }
			/* passe a la condition suivante pour la meme regle */
			pCond = pCond->CoNextCondition;
		     }
		}
	      pRule = pRule->PrNextPRule;
	      /* regle de presentation suivante */
	      if (pRule == NULL)
		 stop = TRUE;
	      else if (pRule->PrType > PtFunction)
		 stop = TRUE;
	      else
		 stop = FALSE;
	   }
	 while (!stop);
}

/*----------------------------------------------------------------------
   ChangeFirstLast cree ou detruit les paves de presentation de        
   l'element pEl qui sont conditionnes au fait qu'il soit  
   le premier (si first est vrai) ou le dernier (si first est
   faux) descendant de son pere.                           
   change indique si l'element cesse d'etre premier ou       
   dernier (vrai) ou au contraire s'il le devient (faux).  
   pDoc pointe sur le descripteur du document ou on        
   travaille.                                              
  ----------------------------------------------------------------------*/
void ChangeFirstLast (PtrElement pEl, PtrDocument pDoc, ThotBool first,
		      ThotBool change)
{
   PtrPRule            pRPres;
   PtrPSchema          pSchP, pSP;
   PtrAttribute        pAttr;
   PtrElement          pElAttr;
   int                 l;
   InheritAttrTable   *inheritTable;

   if (pEl != NULL)
     {
	/* cherche la 1ere fonction de presentation associee au type de */
	/* l'element */
	pRPres = FunctionRule (pEl, &pSchP, pDoc);
	if (pSchP != NULL)
	  {
	     /* traite les regles de creation associees au type de l'element */
	     pAttr = NULL;
	     ApplFunctionPresRules (pRPres, pSchP, pAttr, pDoc, pEl, change, first);
	     /* l'element herite-t-il d'attributs qui ont des fonctions de */
	     /* presentation */
	     pSP = PresentationSchema (pEl->ElStructSchema, pDoc);
	     if (pSP->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	       {
		  /* il y a heritage possible */
		  if ((inheritTable =
		       pSP->PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		    {
		       /* cette table n'existe pas on la genere */
		       CreateInheritedAttrTable (pEl, pDoc);
		       inheritTable = pSP->PsInheritedAttr[pEl->ElTypeNumber - 1];
		    }
		  for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
		     if ((*inheritTable)[l - 1])	/* pEl herite de l'attribut l */
			/* cherche si l existe au dessus */
			if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElStructSchema,
							 &pElAttr)) != NULL)
			  {
			     pSchP = PresentationSchema (pAttr->AeAttrSSchema,
							 pDoc);
			     pRPres = AttrPresRule (pAttr, pEl, TRUE, NULL, pSchP);
			     /* traite les regles de creation associees a l'attribut */
			     ApplFunctionPresRules (pRPres, pSchP, pAttr, pDoc, pEl, change, first);
			  }
	       }

	     /* l'element a-t-il des attributs qui ont des fonctions de */
	     /* presentation ? */
	     pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
	     /* boucle sur les attributs de l'element */
	     while (pAttr != NULL)
	       {
		  pSchP = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
		  /* cherche le debut des regles de presentation associees a */
		  /* l'attribut */
		  pRPres = AttrPresRule (pAttr, pEl, FALSE, NULL, pSchP);
		  /* traite les regles de creation associees a l'attribut */
		  ApplFunctionPresRules (pRPres, pSchP, pAttr, pDoc, pEl, change, first);
		  /* passe a l'attribut suivant de l'element */
		  pAttr = pAttr->AeNext;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   DestroyNewAbsBox dechaine et libere la suite des paves comprise entre
   pAbbFirst et pAbbLast, ainsi que tous les paves englobes.
   Ces paves n'ont pas encore ete vus par le Mediateur, inutile de lui signaler
   leur disparition si cette suite correspond a un element sur plusieurs    
   pages, on parcourt la chaine des dupliques.
  ----------------------------------------------------------------------*/
static void DestroyNewAbsBox (PtrAbstractBox * pAbbFirst,
			      PtrAbstractBox * pAbbLast, int frame,
			      PtrDocument pDoc)
{
   int                 vol;
   PtrAbstractBox      pAb, pAbb;
   PtrAbstractBox      pAbbox1;

   pAb = *pAbbFirst;
   while (pAb != NULL)
      /* met a jour le volume des paves englobants dans la vue */
     {
	vol = pAb->AbVolume;
	if (vol > 0)
	  {
	     pAbb = pAb->AbEnclosing;
	     while (pAbb != NULL)
	       {
		  pAbbox1 = pAbb;
		  pAbbox1->AbVolume -= vol;
		  pAbb = pAbbox1->AbEnclosing;
	       }
	  }
	/* dechaine et libere le pave et ses paves englobes */
	FreeAbView (pAb, frame);

	/* passe au pave suivant a supprimer */
	if (pAb == *pAbbLast)
	   pAb = NULL;
	/* on a traite' le dernier, on arrete */
	else
	   /* passe au pave' suivant */
	   pAb = pAb->AbNext;
     }
   *pAbbFirst = NULL;
   *pAbbLast = NULL;
}

/*----------------------------------------------------------------------
   CreateAllAbsBoxesOfEl     Cree pour toutes les vues existantes tous les 
   paves de l'element pointe par pE appartenant au document pointe 
   par pDoc.                                                       
  ----------------------------------------------------------------------*/
void  CreateAllAbsBoxesOfEl (PtrElement pE, PtrDocument pDoc)
{
  int                 view;

  if (pE == NULL || pE->ElStructSchema == NULL)
    return;
  /* indique qu'il faut creer tous les paves sans limite de volume */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    {
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	/* la vue est ouverte */
	pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
    }
  /* cree effectivement les paves dans toutes les vues existantes */
  CreateNewAbsBoxes (pE, pDoc, 0);
  /* applique les regles retardees concernant les paves cree's */
  ApplDelayedRule (pE, pDoc);
}

/*----------------------------------------------------------------------
   CreateNewAbsBoxes   cree les paves du sous-arbre dont la racine est    
   pointe par pEl, dans le document dont le contexte du    
   document pDoc.                                          
   Les paves sont crees dans toutes les vues si viewNb est  
   nul, ou seulement dans la vue de numero viewNb           
   si viewNb>0.                                             
   Les paves existants affectes par la creation des        
   nouveaux paves sont modifies. Les pointeurs sur les     
   paves a reafficher du document sont mis a jour.         
  ----------------------------------------------------------------------*/
void CreateNewAbsBoxes (PtrElement pEl, PtrDocument pDoc, int viewNb)
{
   PtrAbstractBox      pAb, pAbbReDisp, pAbbR, pAbbFirst, pAbbLast,
                       pAbbSibling, pAbbox1;
   int                 view, firstView, lastView, frame;
   ThotBool            stop, complete;

   if (pEl != NULL)
     {
       if (viewNb == 0)
	 {
	   firstView = 1;
	   /* nombre de vues definies dans le schema de pres. du document */
	   lastView = MAX_VIEW_DOC;
	 }
       else
	 {
	   firstView = viewNb;
	   lastView = viewNb;
	 }

       /* pour toutes les vues demandees, cree les paves du sous-arbre de */
       /* l'element et reapplique les regles affectees par la creation des */
       /* nouveaux paves */
       for (view = firstView; view <= lastView; view++)
	 {
	   frame = pDoc->DocViewFrame[view - 1];
	   if (pDoc->DocView[view - 1].DvPSchemaView > 0 && frame != 0)
	     /* la vue existe */
	     {
	       pAbbReDisp = NULL;
	       /* il n'y a encore rien a reafficher */
	       pAbbFirst = NULL;
	       pAbbLast = NULL;
	       /* cree et chaine les paves correspondant a l'element */
	       if (pAbbReDisp == NULL)
		 pAb = AbsBoxesCreate (pEl, pDoc, view, TRUE, TRUE, &complete);
	       else
		 pAb = NULL;

	       if (pAb != NULL || pAbbFirst != NULL)
		 /* l'element a au moins un pave dans la vue */
		 {
		   if (pAbbFirst == NULL)
		     /* cherche le premier pave cree pour le nouvel element */
		     {
		       pAbbFirst = pAb;
		       stop = FALSE;
		       do
			 if (pAbbFirst->AbPrevious == NULL)
			   stop = TRUE;
			 else if (pAbbFirst->AbPrevious->AbElement == pEl)
			   /* ce pave appartient a l'element */
			   pAbbFirst = pAbbFirst->AbPrevious;
			 else if (ElemIsAnAncestor (pEl, pAbbFirst->AbPrevious->AbElement))
			   /* ce pave appartient a un descendant de */
			   /* l'element (l'element lui-meme a une */
			   /* visibilite' nulle dans la vue et n'a donc */
			   /* pas de pave dans cette vue) */
			   pAbbFirst = pAbbFirst->AbPrevious;
			 else
			   /* ce pave n'appartient pas a l'element */
			   stop = TRUE;
		       while (!stop);

		       /* cherche le dernier pave cree pour le nouvel element */
		       pAbbLast = pAb;
		       stop = FALSE;
		       do
			 if (pAbbLast->AbNext == NULL)
			   stop = TRUE;
			 else if (pAbbLast->AbNext->AbElement == pEl)
			   /* ce pave appartient a l'element */
			   pAbbLast = pAbbLast->AbNext;
			 else if (ElemIsAnAncestor (pEl, pAbbLast->AbNext->AbElement))
			   /* ce pave appartient a un descendant de */
			   /* l'element (l'element lui-meme a une */
			   /* visibilite' nulle dans la vue et n'a donc */
			   /* pas de pave dans cette vue) */
			   pAbbLast = pAbbLast->AbNext;
			 else
			   /* ce pave n'appartient pas a l'element */
			   stop = TRUE;
		       while (!stop);
		     }

		   pAbbReDisp = Enclosing (pAbbFirst, pAbbLast);
		   /* verifie que les paves precedents et suivants sont complets */
		   /* et les detruit s'ils sont incomplets */
		   if (pAbbFirst->AbPrevious != NULL)
		     {
		       pAbbox1 = pAbbFirst->AbPrevious;
		       if (pAbbox1->AbLeafType == LtCompound
			   && !pAbbox1->AbInLine
			   && pAbbox1->AbTruncatedTail)
			 {
			 /* le pave precedent est incomplet a la fin */
			 if (pDoc->DocView[view - 1].DvSync)
			   /* La vue est synchronisee, on supprime tous les paves */
			   /* precedents */
			   {
			     pAb = pAbbFirst;
			     do
			       {
				 pAbbSibling = pAb->AbPrevious;
				 while (pAbbSibling != NULL)
				   {
				     SetDeadAbsBox (pAbbSibling);
				     ApplyRefAbsBoxSupp (pAbbSibling, &pAbbR, pDoc);
				     pAbbReDisp = Enclosing (pAbbR, pAbbReDisp);
				     pAbbSibling = pAbbSibling->AbPrevious;
				   }
				 pAb = pAb->AbEnclosing;
				 if (pAb != NULL)
				   {
				   if (pAb->AbTruncatedHead)
				     /* le pave englobant est deja coupe' en tete. */
				     pAb = NULL;
				   else
				     /* le pave englobant est coupe' en tete, il faut */
				     /* encore detruire les paves qui le precedent. */
				     {
				     TruncateOrCompleteAbsBox (pAb, TRUE, TRUE, pDoc);
				     pAb->AbTruncatedHead = TRUE;
				     }
				   }
			       }
			     while (pAb != NULL);
			   }
			 else
			   /* ce n'est pas une vue synchronisee, on detruit les */
			   /* paves que l'on vient de creer */
			   DestroyNewAbsBox (&pAbbFirst, &pAbbLast, frame, pDoc);
			 }
		     }
		   if (pAbbLast != NULL &&
		       pAbbLast->AbNext != NULL)
		     {
		       pAbbox1 = pAbbLast->AbNext;
		       if (pAbbox1->AbLeafType == LtCompound
			   && !pAbbox1->AbInLine
			   && pAbbox1->AbTruncatedHead)
			 {
			   /* le pave suivant est incomplet au debut */
			   if (pDoc->DocView[view - 1].DvSync)
			     /* La vue est synchronisee, on supprime 
				tous les paves suivants */
			     {
			       pAb = pAbbLast;
			       do
				 {
				   pAbbSibling = pAb->AbNext;
				   while (pAbbSibling != NULL)
				     {
				       SetDeadAbsBox (pAbbSibling);
				       ApplyRefAbsBoxSupp (pAbbSibling, &pAbbR, pDoc);
				       pAbbReDisp = Enclosing (pAbbR, pAbbReDisp);
				       pAbbSibling = pAbbSibling->AbNext;
				     }
				   pAb = pAb->AbEnclosing;
				   if (pAb != NULL)
				     {
				       if (pAb->AbTruncatedTail)
					 pAb = NULL;
				       else
					 /* le pave englobant est coupe' en queue */
					 {
					   TruncateOrCompleteAbsBox (pAb, TRUE, FALSE, pDoc);
					   pAb->AbTruncatedTail = TRUE;
					 }
				     }
				 }
			       while (pAb != NULL);
			     }
			   else
			     /* ce n'est pas une vue synchronisee, on detruit les */
			     /* paves que l'on vient de creer */
			     DestroyNewAbsBox (&pAbbFirst, &pAbbLast, frame, pDoc);
			 }
		     }
		   /* si on n'a pas pu creer tous les paves du contenu de */
		   /* l'element (ils depasseraient la capacite de la fenetre), */
		   /* il faut supprimer tous les paves suivant l'element */
		   if (pAbbLast != NULL)
		     if (pAbbLast->AbLeafType == LtCompound)
		       if (!pAbbLast->AbInLine)
			 if (pAbbLast->AbTruncatedTail)
			   /* on n'a pas pu creer tous les paves */
			   {
			     pAb = pAbbLast;
			     /* traite le pave de l'element et les paves englobants */
			     do		/* marque le pave coupe' */
			       {
				 TruncateOrCompleteAbsBox (pAb, TRUE, FALSE, pDoc);
				 pAb->AbTruncatedTail = TRUE;
				 /* supprime tous ses freres suivants */
				 pAbbSibling = pAb->AbNext;
				 while (pAbbSibling != NULL)
				   {
				     if (!pAbbSibling->AbDead)
				       /* detruit le pave' */
				       {
					 SetDeadAbsBox (pAbbSibling);
					 /* change les regles des autres paves qui 
					    se referent au pave detruit */
					 ApplyRefAbsBoxSupp (pAbbSibling, &pAbbR, pDoc);
					 pAbbReDisp = Enclosing (pAbbR, pAbbReDisp);
				       }
				     pAbbSibling = pAbbSibling->AbNext;
				   }
				 pAb = pAb->AbEnclosing;
				 if (pAb != NULL)
				   if (pAb->AbTruncatedTail)
				     pAb = NULL;
				 /* pave deja coupe', on s'arrete */
			       }
			     while (pAb != NULL);
			   }
		 }
	       if (pAbbFirst != NULL)
		 /* modifie les paves qui peuvent se referer aux nouveaux paves */
		 {
		   ApplyRefAbsBoxNew (pAbbFirst, pAbbLast, &pAbbR, pDoc);
		   ApplDelayedRule (pEl, pDoc);
		   pAbbReDisp = Enclosing (pAbbR, pAbbReDisp);
		   /* conserve le pointeur sur le pave a reafficher */
		   pDoc->DocViewModifiedAb[view - 1] = Enclosing (pAbbReDisp,
					   pDoc->DocViewModifiedAb[view - 1]);
		 }
	     }
	 }
     }
}

/*----------------------------------------------------------------------
   DestroyAbsBoxesView detruit pour la vue de numero view les paves du        
   sous-arbre dont la racine est pointee par pEl, dans le  
   document pDoc.                                          
   Les paves existants affectes par la destruction sont    
   modifies. Les pointeurs de paves modifies du document   
   sont mis a jour                                         
   Si verify est vrai, on verifie si les paves englobants   
   deviennent complets. En effet la suppression d'un pave  
   incomplet a une extremite' peut rendre le pave englobant
   complete a cette extremite.                              
  ----------------------------------------------------------------------*/
void DestroyAbsBoxesView (PtrElement pEl, PtrDocument pDoc, ThotBool verify,
			  int view)
{
  PtrAbstractBox      pAb, pAbbReDisp, pAbbR, pAbb, pElAscent, PcFirst;
  PtrAbstractBox      pAbbox1, PcLast;
  PtrElement          pElChild;

  pAb = pEl->ElAbstractBox[view - 1];
  if (pAb == NULL)
    {
      /* pas de pave pour cet element dans cette vue, alors que */
      /* la vue existe */
      if (!pEl->ElTerminal)
	/* cherche les paves des descendants de l'element */
	{
	  pElChild = pEl->ElFirstChild;
	  while (pElChild != NULL)
	    {
	      DestroyAbsBoxesView (pElChild, pDoc, verify, view);
	      pElChild = pElChild->ElNext;
	    }
	}
    }
  else
    /* l'element a au moins un pave dans la vue */
    {
      /* si l'element cree un pave englobant par la regle FnCreateEnclosing, */
      /* c'est ce pave englobant qu'il faut detruire */
      if (pAb->AbEnclosing != NULL &&
	  pAb->AbEnclosing->AbPresentationBox &&
	  pAb->AbEnclosing->AbElement == pAb->AbElement)
	pAb = pAb->AbEnclosing;

      PcFirst = NULL;
      PcLast = NULL;
      pAbbReDisp = pAb;	/* on reaffichera ce pave */
      /* traite tous les paves correspondant a cet element */
      /* les marque d'abord tous 'morts' en verifiant si leur */
      /* suppression complete le pave englobant */
      do
	{
	  if (verify &&
	      pAb->AbLeafType == LtCompound && !pAb->AbInLine)
	    {
	      if (pAb->AbTruncatedHead && pEl->ElPrevious == NULL)
		/* c'est le pave du premier element */
		{
		  pElAscent = pAb->AbEnclosing;
		  while (pElAscent != NULL)
		    {
		      pAbb = TruncateOrCompleteAbsBox (pElAscent, FALSE, TRUE, pDoc);
		      if (pAbb != NULL)
			pAbbReDisp = Enclosing (pAbbReDisp, pAbb);
		      if (pElAscent->AbElement->ElPrevious != NULL)
			pElAscent = NULL;
		      else
			pElAscent = pElAscent->AbEnclosing;
		    }
		}
	      if (pAb->AbTruncatedTail && pEl->ElNext == NULL)
		/* c'est le pave du dernier element */
		{
		  pElAscent = pAb->AbEnclosing;
		  while (pElAscent != NULL)
		    {
		      pAbb = TruncateOrCompleteAbsBox (pElAscent, FALSE, FALSE, pDoc);
		      if (pAbb != NULL)
			pAbbReDisp = Enclosing (pAbbReDisp, pAbb);
		      pElAscent = pElAscent->AbEnclosing;
		      if (pElAscent != NULL &&
			  pElAscent->AbElement->ElNext != NULL)
			pElAscent = NULL;
		    }
		}
	    }
	  if (pAb->AbNew)
	    {
	      pAbbox1 = pAb;
	      FreeAbView (pAb, pDoc->DocViewFrame[view - 1]);
	      pAb = pAbbox1;
	    }
	  else
	    {
	      SetDeadAbsBox (pAb);
	      if (pAb == AbsBoxSelectedAttr)
		CancelSelection ();
	      if (PcFirst == NULL)
		PcFirst = pAb;
	      PcLast = pAb;
	      /* passe au pave suivant du meme element */
	      pAb = pAb->AbNext;
	    }
	  if (pAb != NULL)
	    {
	      pAbbox1 = pAb;
	      if (pAbbox1->AbElement != pEl)
		/* il s'agit d'un autre element, on arrete */
		pAb = NULL;
	      else if (pAbbox1->AbElement->ElTerminal
		       && pAbbox1->AbElement->ElLeafType == LtPageColBreak)
		/* c'est un pave de haut ou bas de page, on n'en */
		/* traite qu'un */
		pAb = NULL;
	    }
	}
      while (pAb != NULL);

      if (PcFirst == NULL)
	return;
      else if (PcFirst != PcLast)
	/* il y a plusieurs paves pour cet element, on reaffichera */
	/* le pave englobant */
	pAbbReDisp = Enclosing (pAbbReDisp, PcFirst->AbEnclosing);

      pAb = PcFirst;
      do
	{
	  /* cherche tous les paves qui font reference au pave a */
	  /* detruire et pour ces paves, reapplique les regles qui font */
	  /* reference au pave a detruire */
	  ApplyRefAbsBoxSupp (pAb, &pAbbR, pDoc);
	  pAbbReDisp = Enclosing (pAbbReDisp, pAbbR);
	  /* passe au pave mort suivant */
	  if (pAb == PcLast)
	    pAb = NULL;
	  else
	    pAb = pAb->AbNext;
	}
      while (pAb != NULL);
      /* conserve le pointeur sur le pave a reafficher */
      pDoc->DocViewModifiedAb[view - 1] = Enclosing (pAbbReDisp,
                                            pDoc->DocViewModifiedAb[view - 1]);
    }
}

/*----------------------------------------------------------------------
   DestroyAbsBoxes detruit les paves du sous-arbre dont la racine est    
   pointee par pEl, dans le document pDoc.                 
   Les paves existants affectes par la destruction sont    
   modifies. Les pointeurs de paves modifies du document   
   sont mis a jour                                         
   Si verify est vrai, on verifie si les paves englobants   
   deviennent complets. En effet la suppression d'un pave  
   incomplet a une extremite' peut rendre le pave englobant
   complete a cette extremite.                              
  ----------------------------------------------------------------------*/
void DestroyAbsBoxes (PtrElement pEl, PtrDocument pDoc, ThotBool verify)
{
  int                 view;

  if (pEl != NULL)
    /* traite les paves de toutes les vues */
    for (view = 1; view <= MAX_VIEW_DOC; view++)
      {
	if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	  /* la vue  existe */
	  /* detruit les paves de cette vue */
	  DestroyAbsBoxesView (pEl, pDoc, verify, view);
      }
}

/*----------------------------------------------------------------------
   RedispRef Reaffiche les paves de la reference pointee par   
   pRef appartenant au document pointe' par pDocRef.       
   Si pAb est NULL, tous les paves (sauf les paves de     
   presentation) de la reference, dans toutes les vues,    
   sont recalcules et reaffiches ; sinon, seuls les paves  
   qui copient le pave pointe' par pAb sont recalcules et 
   reaffiches s'ils changent.                              
   Note: cette nouvelle procedure est extraite du code de  
   l'ancienne procedure RedispAllReferences.                           
  ----------------------------------------------------------------------*/
void RedispRef (PtrReference pRef, PtrAbstractBox pAb, PtrDocument pDocRef)
{
   PtrElement          pElRef;
   PtrAbstractBox      pAbb, pPavRef;
   PtrPRule            pRule;
   ThotBool            found, redisp, stop, same;
   PtrTextBuffer       bufCopy, BufOriginal;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;
   PtrPRule            pRe1;
   PtrAbstractBox      pAbbox1;
   int                 v, frame, j, h;
   ThotBool            complete;

   pElRef = pRef->RdElement;
   /* element qui reference */
   if (pRef->RdAttribute == NULL)
      /* on ne traite pas les */
      /* references par attribut */
      for (v = 1; v <= MAX_VIEW_DOC; v++)
	 if (pElRef->ElAbstractBox[v - 1] != NULL)
	    /* la reference a au moins un pave dans la vue */
	    /* saute les paves de presentation */
	   {
	      pPavRef = pElRef->ElAbstractBox[v - 1];
	      stop = FALSE;
	      do
		 if (pPavRef == NULL)
		    stop = TRUE;
		 else if (!pPavRef->AbPresentationBox)
		    stop = TRUE;
		 else
		    pPavRef = pPavRef->AbNext;
	      while (!stop);
	      redisp = FALSE;
	      if (pRef->RdTypeRef == RefInclusion)
		 /* c'est une inclusion, on cree ses paves si ce n'est pas */
		 /* deja fait */
		 /* indique le volume que pourront prendre les paves cree's */
		{
		  if (pDocRef->DocView[v - 1].DvPSchemaView > 0)
		    pDocRef->DocViewFreeVolume[v - 1] = pDocRef->DocViewVolume[v - 1];
		   pAbb = AbsBoxesCreate (pElRef, pDocRef, v, TRUE, TRUE, &complete);
		   redisp = pAbb != NULL;
		}
	      else
		 /* C'est un element reference. L'element reference copie-t-il */
		 /* ce type de pave ? */
		 /* cherche dans les regles de presentation de la */
		 /* reference une regle FnCopy qui copie ce type de pave */
		{
		   found = FALSE;
		   pRule = SearchRulepAb (pDocRef, pPavRef, &pSPR, PtFunction, FnAny, TRUE, &pAttr);
		   stop = FALSE;
		   do
		      if (pRule == NULL)
			 stop = TRUE;
		      else if (pRule->PrType != PtFunction || found)
			 stop = TRUE;
		      else
			{
			   pRe1 = pRule;
			   if (pRe1->PrViewNum == pDocRef->DocView[v - 1].DvPSchemaView
			       && pRe1->PrPresFunction == FnCopy)
			     {
			      if (pAb == NULL)
				 found = TRUE;
			      else
				{
				   pAbbox1 = pAb;
				   if (pRe1->PrNPresBoxes == 0)
				      found = (ustrcmp (pRe1->PrPresBoxName, pAbbox1->AbPSchema->PsPresentBox[pAbbox1->AbTypeNum - 1].PbName) == 0);
				   else
				      found = pRe1->PrPresBox[0] == pAbbox1->AbTypeNum;
				}
			     }
			   if (!found)
			      pRule = pRule->PrNextPRule;
			}
		   while (!stop);
		   if (found)
		      /* oui, la reference copie ce type de pave */
		     {
			if (pAb == NULL)
			   same = FALSE;
			else
			   /* la copie est-elle deja same a l'original ? */
			  {
			     bufCopy = pPavRef->AbText;
			     BufOriginal = pAb->AbText;
			     j = 1;
			     while (BufOriginal->BuContent[j - 1] == bufCopy->BuContent[j - 1]
				    && BufOriginal->BuContent[j - 1] != EOS
				    && bufCopy->BuContent[j - 1] != EOS)
				j++;
			     same = BufOriginal->BuContent[j - 1] == EOS
				&& bufCopy->BuContent[j - 1] == EOS;
			  }
			if (!same)
			   /* contenus differents, applique la regle de copie */
			   if (ApplyRule (pRule, pSPR, pPavRef, pDocRef, pAttr))
			      redisp = TRUE;
		     }
		}
	      if (redisp)
		 /* on ne reaffiche pas les paves qui n'ont pas encore */
		 /* ete affiches */
		 if (!pPavRef->AbNew)
		   {
		      pPavRef->AbChange = TRUE;
		      frame = pDocRef->DocViewFrame[v - 1];
		      h = PageHeight;
		      (void) ChangeConcreteImage (frame, &h, pPavRef);
		      /* on ne reaffiche pas si on est en train de calculer */
		      /* les pages */
		      if (PageHeight == 0)
			 DisplayFrame (frame);
		   }
	   }
}

/*----------------------------------------------------------------------
   RedispAllReferences Le pave de presentation pointe' par pAb a change'      
   de contenu. Cherche toutes les references a un element  
   englobant de l'element creant ce pave et qui copient    
   ce pave. Demande leur reaffichage                       
  ----------------------------------------------------------------------*/
void RedispAllReferences (PtrAbstractBox pAb, PtrDocument pDoc)
{
   PtrElement          pEl;
   PtrReference        pRef;
   int                 level;
   PtrExternalDoc      pDocExt;
   PtrDocument         pDocRef;

   pEl = pAb->AbElement;
   /* traite l'element qui a cree' ce pave et les elements englobants */
   /* sur 3 niveaux */
   level = 0;
   pDocExt = NULL;
   pRef = NULL;
   while (pEl != NULL && level < 3)
     {
	pRef = NextReferenceToEl (pEl, pDoc, FALSE, pRef, &pDocRef, &pDocExt, TRUE);
	if (pRef != NULL)
	   /* cet element est reference' par au moins un autre element */
	   /* parcourt la chaine des elements qui le referencent */
	   while (pRef != NULL)
	      /* reaffiche les paves de la reference qui copient le pave */
	     {
		RedispRef (pRef, pAb, pDocRef);
		/* passe a la reference suivante */
		pRef = NextReferenceToEl (pEl, pDoc, FALSE, pRef, &pDocRef, &pDocExt, TRUE);
		/* passe au niveau superieur */
	     }
	pEl = pEl->ElParent;
	level++;
     }
}

/*----------------------------------------------------------------------
   SearchAbsBoxBackward                                                           
  ----------------------------------------------------------------------*/
static PtrAbstractBox SearchAbsBoxBackward (PtrAbstractBox pAbb1,
					    ThotBool Test, PtrSSchema pSchStr,
					    PtrPSchema pSchP, int Typ,
					    ThotBool Pres)
{
   PtrAbstractBox      p, s;
   PtrAbstractBox      pAbbox1;

   p = NULL;
   if (Test)
     {
	pAbbox1 = pAbb1;
	if (Pres)
	  {
	     if (pAbbox1->AbPresentationBox && pAbbox1->AbTypeNum == Typ)
	       {
		  if (pAbbox1->AbPSchema == pSchP)
		     p = pAbb1;
		  /* found ! C'est le pave lui-meme */
	       }
	  }
	else if (!pAbbox1->AbPresentationBox
		 && pAbbox1->AbElement->ElTypeNumber == Typ
		 && (pSchStr == NULL
		     || pAbbox1->AbElement->ElStructSchema == pSchStr))
	   p = pAbb1;
     }
   if (p == NULL)
      /* on cherche parmi les fils du pave */
     {
	s = pAbb1->AbFirstEnclosed;
	while (s != NULL && p == NULL)
	  {
	     p = SearchAbsBoxBackward (s, TRUE, pSchStr, pSchP, Typ, Pres);
	     s = s->AbNext;
	  }
     }
   return p;
}

/*----------------------------------------------------------------------
   AbsBoxFromElOrPres cherche un pave en avant dans un arbre de paves, a   
   partir du pave pointe' par pAb. Si pres est vrai, on   
   cherche un pave de presentation du type typeElOrPres defini dans 
   le schema de presentation pSchP. Si pres est faux, on   
   cherche un pave d'un element structure de type typeElOrPres      
   defini dans le schema de structure pointe' par pSchStr. 
   Retourne un pointeur sur le pave trouve' ou             
   NULL si echec.                                          
  ----------------------------------------------------------------------*/
PtrAbstractBox AbsBoxFromElOrPres (PtrAbstractBox pAb, ThotBool pres,
				   int typeElOrPres, PtrPSchema pSchP,
				   PtrSSchema pSchStr)
{
   PtrAbstractBox      pAbbResult, pAbbForward, pAbbAscent;
   ThotBool            stop;
   PtrAbstractBox      pAbbox1;

   pAbbResult = NULL;
   if (pAb != NULL)
      /* cherche dans le sous-arbre du pave */
     {
	pAbbResult = SearchAbsBoxBackward (pAb, FALSE, pSchStr, pSchP, typeElOrPres, pres);
	if (pAbbResult == NULL)
	   /* si echec, cherche dans les sous-arbres des freres suivants */
	  {
	     pAbbForward = pAb->AbNext;
	     while (pAbbForward != NULL && pAbbResult == NULL)
	       {
		  pAbbResult = SearchAbsBoxBackward (pAbbForward, TRUE, pSchStr, pSchP, typeElOrPres, pres);
		  pAbbForward = pAbbForward->AbNext;
		  /* si echec, cherche le premier ascendant avec un frere suivant */
	       }
	     if (pAbbResult == NULL)
	       {
		  stop = FALSE;
		  pAbbAscent = pAb;
		  do
		    {
		       pAbbAscent = pAbbAscent->AbEnclosing;
		       if (pAbbAscent == NULL)
			  stop = TRUE;
		       else if (pAbbAscent->AbNext != NULL)
			  stop = TRUE;
		    }
		  while (!stop);
		  if (pAbbAscent != NULL)
		     /* cherche si ce pave est celui cherche */
		    {
		       pAbbAscent = pAbbAscent->AbNext;
		       if (pAbbAscent != NULL)
			 {
			    pAbbox1 = pAbbAscent;
			    if (pres)
			      {
				 if (pAbbox1->AbPresentationBox && pAbbox1->AbTypeNum == typeElOrPres)
				   {
				      if (pAbbox1->AbPSchema == pSchP)
					 pAbbResult = pAbbAscent;
				      /* trouve */
				   }
			      }
			    else if (!pAbbox1->AbPresentationBox
				     && pAbbox1->AbElement->ElTypeNumber == typeElOrPres
				     && (pSchStr == NULL
					 || pAbbox1->AbElement->ElStructSchema == pSchStr))
			       pAbbResult = pAbbAscent;
			    /* trouve */
			    if (pAbbResult == NULL)
			       pAbbResult = AbsBoxFromElOrPres (pAbbAscent, pres, typeElOrPres, pSchP, pSchStr);
			 }
		    }
	       }
	  }
     }
   return pAbbResult;
}

/*----------------------------------------------------------------------
   FindFirstAbsBox  Si pAbbBegin n'a pas de valeur pour la vue nv, met dans  
   pAbbBegin[nv] un pointeur sur le premier pave            
   correspondant a l'element pElBegin dans la vue nv.      
  ----------------------------------------------------------------------*/
static void         FindFirstAbsBox (PtrElement pElBegin, int nv)
{
   PtrElement          pEl;
   ThotBool            stop;

   if (pAbbBegin[nv - 1] == NULL)
     {
	pEl = pElBegin;
	stop = FALSE;
	do
	   if (pEl == NULL)
	      stop = TRUE;
	   else if (pEl->ElAbstractBox[nv - 1] != NULL)
	      stop = TRUE;
	   else
	      /* l'element n'a pas de pave dans la vue, */
	      /* ?cherche un element suivant qui ait un pave */
	      pEl = FwdSearchTypedElem (pEl, 0, NULL);
	while (!stop);
	if (pEl != NULL)
	   pAbbBegin[nv - 1] = pEl->ElAbstractBox[nv - 1];
     }
}

/*----------------------------------------------------------------------
   ComputePageNum      renumerote toutes les pages qui concernent      
   la vue view a partir de l'element pointe' par pEl,       
   lui-meme compris.                                       
  ----------------------------------------------------------------------*/
void ComputePageNum (PtrElement pEl, PtrDocument pDoc, int view)
{
   PtrElement          pPage;
   PtrPSchema          pSchP;
   ThotBool            stop;
   int                 numpageprec, cpt;

   stop = FALSE;
   pPage = pEl;
   numpageprec = 0;
   /* renumerote toutes les pages qui suivent pour la meme vue */
   do
     {
	pPage = FwdSearchTypedElem (pPage, PageBreak + 1, NULL);
	if (pPage == NULL)
	   /* c'etait la derniere page, on s'arrete */
	   stop = TRUE;
	else if (pPage->ElViewPSchema == view)
	   /* cette page concerne la vue, on la traite */
	  {
	     numpageprec = pPage->ElPageNumber;
	     /* cherche le compteur de page a appliquer a cette page */
	     cpt = GetPageCounter (pPage, pDoc, view, &pSchP);
	     if (cpt == 0)
		/* page non numerotee, on s'arrete */
		stop = TRUE;
	     else
		/* calcule le nouveau numero de page */
	       {
		  pPage->ElPageNumber = CounterVal (cpt, pPage->ElStructSchema, pSchP, pPage, view);
		  if (pPage->ElPageNumber == numpageprec)
		     /* le numero de page n'a pas change', on s'arrete */
		     stop = TRUE;
	       }
	  }
     }
   while (!stop);
}

/*----------------------------------------------------------------------
   ComputeContent   recalcule le contenu de toutes les boites de    
   presentation du type boxType (dans le schema de       
   presentation pSchP) qui sont apres pElBegin, dans la    
   vue nv.                                                 
  ----------------------------------------------------------------------*/
static void ComputeContent (int boxType, int nv, PtrDocument pDoc,
			    PtrSSchema pSS, PtrPSchema pSchP,
			    PtrElement pElBegin, ThotBool redisp)
{
  PtrAbstractBox      pAb;
  int                 frame, h;
  PtrAbstractBox      pAbbox1;

  FindFirstAbsBox (pElBegin, nv);
  pAb = pAbbBegin[nv - 1];
  while (pAb != NULL)
    {
      pAbbox1 = pAb;
      if (pAbbox1->AbPresentationBox &&
	  pAbbox1->AbTypeNum == boxType &&
	  pAbbox1->AbPSchema == pSchP)
	/* fait reafficher le pave de presentation si le contenu a */
	/* change' */
	if (NewVariable (pSchP->PsPresentBox[boxType - 1].PbContVariable,
			 pSS, pSchP, pAb, NULL, pDoc))
	  /* et si le pave a deja ete traite' par le mediateur */
	  if (!pAb->AbNew)
	    {
	      pAbbox1->AbChange = TRUE;
	      frame = pDoc->DocViewFrame[nv - 1];
	      h = PageHeight;
	      ChangeConcreteImage (frame, &h, pAb);
	      /* on ne reaffiche pas si on est en train de calculer les */
	      /* pages */
	      if (PageHeight == 0 && redisp)
		DisplayFrame (frame);
	      /* cherche le pave de presentation suivant de ce type */
	    }
      pAb = AbsBoxFromElOrPres (pAb, TRUE, boxType, pSchP, NULL);
    }
}

/*----------------------------------------------------------------------
   ComputeCrPresBoxes reevalue les conditions de creation de toutes  
   les boites de presentation du type boxType (dans le   
   schema de presentation pSchP) qui sont apres pElBegin,  
   dans la vue nv.                                         
  ----------------------------------------------------------------------*/
static void ComputeCrPresBoxes (int boxType, int nv, PtrPSchema pSchP,
				PtrDocument pDoc, PtrElement pElBegin,
				ThotBool redisp)
{
   PtrAbstractBox      pAb, pAbbFollow;
   int                 frame, h;
   ThotBool            found, stop;
   PtrPRule            pRCre;
   PtrPSchema          pSPR;
   PtrSSchema          pSSR;
   PtrAttribute        pAttr;
   int                 viewSch;
   PtrAbstractBox      pAbbox1;
   PtrPRule            pRe1;
   int                 presNum;

   FindFirstAbsBox (pElBegin, nv);
   pAb = pAbbBegin[nv - 1];
   pAbbFollow = NULL;
   viewSch = pDoc->DocView[nv - 1].DvPSchemaView;
   while (pAb != NULL)
     {
	pAbbox1 = pAb;
	if (pAbbox1->AbPresentationBox && !pAb->AbDead
	    && pAbbox1->AbTypeNum == boxType
	    && pAbbox1->AbPSchema == pSchP)
	   /* cherche la regle de l'element createur, regle qui cree */
	   /* cette boite */
	  {
	     found = FALSE;
	     presNum = 0;	/* a priori, le createur n'est pas un pave de pres */
	     /* cas ou le createur est lui-meme un pave de presentation */
	     /* identifie par le fait que le pere est un pave de pres */
	     /* en effet les paves de pres ne peuvent creer que des fils */
	     if (pAbbox1->AbEnclosing->AbPresentationBox)
		presNum = pAbbox1->AbEnclosing->AbTypeNum;
	     pRCre = GlobalSearchRulepEl (pAbbox1->AbElement, pDoc, &pSPR, &pSSR, presNum, NULL,
				  viewSch, PtFunction, FnAny, TRUE, FALSE, &pAttr);
	     stop = FALSE;
	     do
		if (pRCre == NULL)
		   stop = TRUE;
		else if (pRCre->PrType != PtFunction)
		   stop = TRUE;
		else
		  {
		     pRe1 = pRCre;
		     if (pRe1->PrViewNum == viewSch
			 && pRe1->PrPresBox[0] == boxType
			 && pSchP == pSPR
			 && (pRe1->PrPresFunction == FnCreateBefore
			     || pRe1->PrPresFunction == FnCreateAfter
			     || pRe1->PrPresFunction == FnCreateFirst
			     || pRe1->PrPresFunction == FnCreateLast))
			found = TRUE;
		     else
			pRCre = pRe1->PrNextPRule;
		  }
	     while (!stop && !found);
	     if (found)
		/* reevalue les conditions d'application de la regle */
		if (!CondPresentation (pRCre->PrCond, pAb->AbElement, NULL,
				       NULL, viewSch,
				       pAb->AbElement->ElStructSchema, pDoc))
		   /* On va detruire le pave, on cherche d'abord le pave de */
		   /* presentation suivant de meme type */
		  {
		     pAbbFollow = AbsBoxFromElOrPres (pAb, TRUE, boxType, pSchP, NULL);
		     /* tue le pave */
		     SetDeadAbsBox (pAb);
		     /* signale le pave mort au mediateur */
		     frame = pDoc->DocViewFrame[nv - 1];
		     h = PageHeight;
		     ChangeConcreteImage (frame, &h, pAb);
		     /* on ne reaffiche pas si on est en train de calculer les */
		     /* pages */
		     if (PageHeight == 0 && redisp)
			DisplayFrame (frame);
		     /* libere le pave tue' */
		     FreeDeadAbstractBoxes (pAb, frame);
		     pAb = NULL;
		     /* cherche le pave de presentation suivant de ce type */
		  }
	  }
	if (pAbbFollow == NULL)
	  {
	     if (pAb != NULL)
		pAb = AbsBoxFromElOrPres (pAb, TRUE, boxType, pSchP, NULL);
	  }
	else
	  {
	     pAb = pAbbFollow;
	     pAbbFollow = NULL;
	  }
     }
}

/*----------------------------------------------------------------------
   ComputeCreation  pour toutes les boites du type boxType (dans  
   le schema de presentation pSchP) qui sont apres         
   pElBegin, dans la vue nv, reevalue les conditions de    
   creation de boites de presentation.                     
   presBox indique si boxType est un type de boite de       
   presentation ou de boite d'element structure'.          
   Pour toutes les conditions de creation qui dependent du 
   compteur counter et qui sont satisfaites, la boite est      
   creee si elle n'existe pas deja.                        
  ----------------------------------------------------------------------*/
static void ComputeCreation (int boxType, ThotBool presBox, int counter,
			     int nv, PtrSSchema pSS, PtrPSchema pSchP,
			     PtrDocument pDoc, PtrElement pElBegin,
			     ThotBool redisp)
{
   PtrAbstractBox      pAb, pAbb, pAbbNext;
   PtrElement          pEl;
   int                 frame, h;
   ThotBool            stop, isCreated, depend, boxok, page;
   PtrPRule            pRCre;
   PtrPSchema          pSPR, pSchPOrig;
   PtrSSchema          pSSR;
   PtrAttribute        pAttr;
   int                 viewSch;
   PtrAbstractBox      pAbbox1;
   PtrPRule            pRe1;
   PtrCondition        pCond;
   int                 presNum;

   FindFirstAbsBox (pElBegin, nv);
   pAb = pAbbBegin[nv - 1];
   viewSch = pDoc->DocView[nv - 1].DvPSchemaView;
   page = FALSE;
   if (presBox)
      if (pSchP->PsPresentBox[boxType - 1].PbPageBox)
	 /* c'est une boite page */
	 page = TRUE;
   while (pAb != NULL)
     {
	     boxok = FALSE;
	     pAbbox1 = pAb;
	     if (pAbbox1->AbPresentationBox == presBox
		 && pAbbox1->AbTypeNum == boxType
		 && pAbbox1->AbPSchema == pSchP)
		boxok = TRUE;
	     else if (presBox)
		if (pSchP->PsPresentBox[boxType - 1].PbPageBox)
		   if (pAbbox1->AbElement->ElTerminal
		       && pAbbox1->AbElement->ElLeafType == LtPageColBreak
		       && pAbbox1->AbLeafType == LtCompound)
		      /* c'est un element Marque de page */
		      /* mais pas un pave de colonne */
		     {
			/*attention GetPageBoxType est susceptible de modifier pSchP 
			   il faut donc prendre des precautions */
			pSchPOrig = pSchP;
			if (boxType == GetPageBoxType (pAbbox1->AbElement, pDoc, viewSch, &pSchPOrig)
			    && pSchPOrig == pSchP)
			   /* c'est bien ce type de boite page */
			   boxok = TRUE;
		     }

	     if (boxok)
		/* cherche toutes les regles de creation de cette boite */
	       {
		  presNum = 0;	/* a priori pAb n'est pas une boite de presentation */
		  if (pAb->AbPresentationBox)
		     presNum = boxType;
		  pRCre = GlobalSearchRulepEl (pAb->AbElement, pDoc, &pSPR, &pSSR, presNum, NULL,
				  viewSch, PtFunction, FnAny, TRUE, FALSE, &pAttr);
		  stop = FALSE;
		  do
		     if (pRCre == NULL)
			/* il n' y a plus de regles */
			stop = TRUE;
		     else if (pRCre->PrType != PtFunction)
			/* il n'y a plus de regle fonction de presentation */
			stop = TRUE;
		     else
			/* la regle est une fonction de presentation */
		       {
			  pRe1 = pRCre;
			  if (pRe1->PrViewNum == viewSch
			      && (pRe1->PrPresFunction == FnCreateBefore
				  || pRe1->PrPresFunction == FnCreateWith
				  || pRe1->PrPresFunction == FnCreateAfter
				  || pRe1->PrPresFunction == FnCreateFirst
				  || pRe1->PrPresFunction == FnCreateLast))
			     /* c'est une regle de creation pour cette vue */
			     /* la creation depend-elle du compteur counter ? */
			    {
			       depend = FALSE;
			       pCond = pRe1->PrCond;
			       while (pCond != NULL && !depend)
				 {
				    if (pCond->CoCondition == PcInterval ||
					pCond->CoCondition == PcEven ||
					pCond->CoCondition == PcOdd ||
					pCond->CoCondition == PcOne)
				       if (pCond->CoCounter == counter)
					  depend = TRUE;
				    pCond = pCond->CoNextCondition;
				 }
			       if (depend)
				  /* reevalue les conditions d'application de la regle */
				  if (CondPresentation (pRCre->PrCond, pAb->AbElement,
							NULL, NULL, viewSch, pAb->AbElement->ElStructSchema, pDoc))
				     /* cherche si le pave est deja cree' */
				    {
				       isCreated = FALSE;
				       if (page || pRe1->PrPresFunction == FnCreateFirst
					   || pRe1->PrPresFunction == FnCreateLast)
					 {
					    pAbb = pAb->AbFirstEnclosed;
					    while (!isCreated && pAbb != NULL)
					       if (pAbb->AbPresentationBox
						   && pAbb->AbTypeNum == pRe1->PrPresBox[0]
						 && pAbb->AbPSchema == pSPR)
						  isCreated = TRUE;
					       else
						  pAbb = pAbb->AbNext;
					 }
				       else
					 {
					    pAbb = pAb;
					    while (!isCreated && pAbb != NULL
						   && pAbb->AbElement == pAb->AbElement)
					       if (pAbb->AbPresentationBox
						   && pAbb->AbTypeNum == pRe1->PrPresBox[0]
						 && pAbb->AbPSchema == pSPR)
						  isCreated = TRUE;
					       else
						  pAbb = pAbb->AbNext;
					 }
				       if (!isCreated)
					  /* on cree le pave de presentation */
					 {
					    pEl = pAb->AbElement;
					    /* code supprime car les regles de creation des hauts et bas de page */
					    /* sont definies a partir du corps de page qui est au meme niveau */
					    /* que pAb */
					    if (pEl->ElTerminal
						&& pEl->ElLeafType == LtPageColBreak)
					      {
						 pAbbNext = pEl->ElAbstractBox[nv - 1];
						 pAbb = pAbbNext->AbFirstEnclosed;
						 if (pAbb != NULL)
						   {
						      while (pAbb->AbPresentationBox)
							 pAbb = pAbb->AbNext;
						      pEl->ElAbstractBox[nv - 1] = pAbb;
						   }
					      }
					    else
					       pAbbNext = NULL;
					    pAbb = CrAbsBoxesPres (pEl, pDoc, pRCre, pSS, pAttr,
						    nv, pSchP, TRUE);
					    if (pAbbNext != NULL)
					       pEl->ElAbstractBox[nv - 1] = pAbbNext;
					    if (pAbb != NULL)
					      {
						frame = pDoc->DocViewFrame[nv - 1];
						h = PageHeight;
						ChangeConcreteImage (frame, &h, pAbb);
						/* on ne reaffiche pas si on est en
						   train de calculer les pages */
						if (PageHeight == 0 && redisp)
						  DisplayFrame (frame);
						/* on passe a la regle suivante */
					      }
					 }
				    }
			    }
			  pRCre = pRe1->PrNextPRule;
		       }
		  while (!stop);
	       }
	/* cherche le pave suivant de ce type */
	if (page)
	   /* on cherche une boite page */
	   pAb = AbsBoxFromElOrPres (pAb, FALSE, PageBreak + 1, NULL, NULL);
	else
	   pAb = AbsBoxFromElOrPres (pAb, presBox, boxType, pSchP, NULL);
     }
}

/*----------------------------------------------------------------------
   AttachCounterValue Transmet a l'attribut de nom NmAttr              
   la valeur du compteur counter defini dans pSchP associe' a  
   pSchS pour pEl. pEl peut est une reference a un document
   inclus pElIncluded peut etre soit le document inclus, soit
   le document inclus dans sa forme demi expansee: il est  
   alors dans le  document l'incluant.                     
  ----------------------------------------------------------------------*/
static void AttachCounterValue (PtrElement pEl, PtrElement pElIncluded,
				PtrDocument pDocIncluded, Name NmAttr,
				int counter, PtrPSchema pSchP,
				PtrSSchema pSchS)
{
   int                 att;
   PtrAttribute        pAttr;
   TtAttribute        *pTtAttr;
   ThotBool            found;

   if (pElIncluded != NULL)
     {
	/* le document inclus est charge', cherche l'attribut dans */
	/* son schema de structure */
	att = 0;
	found = FALSE;
	while (att < pElIncluded->ElStructSchema->SsNAttributes && !found)
	  {
	     att++;
	     pTtAttr = &(pElIncluded->ElStructSchema->SsAttribute[att - 1]);
	     if (pTtAttr->AttrType == AtNumAttr)
		/* c'est un attribut numerique */
		if (ustrncmp (pTtAttr->AttrOrigName, NmAttr, sizeof (Name)) == 0)
		   /* il a le nom cherche' */
		   found = TRUE;
	  }
	if (found)
	  {
	     /* l'attribut est bien defini dans le schema de structure du */
	     /* document inclus, on le met sur la racine */
	     GetAttribute (&pAttr);
	     pAttr->AeAttrSSchema = pElIncluded->ElStructSchema;
	     pAttr->AeAttrNum = att;
	     pAttr->AeAttrType = AtNumAttr;
	     pAttr->AeAttrValue = CounterVal (counter, pSchS, pSchP, pEl, 1);
	     AttachAttrWithValue (pElIncluded, pDocIncluded, pAttr);
	     DeleteAttribute (NULL, pAttr);
	  }
     }
}

/*----------------------------------------------------------------------
   TransmitCounterVal pEl (appartenant au document pDoc) est une     
   inclusion de document externe. Transmet a l'attribut de 
   nom nameAttr du document inclus la valeur du compteur     
   counter defini dans le schema de presentation pSchP associe'
   au schema de structure pSchS.                           
  ----------------------------------------------------------------------*/
void TransmitCounterVal (PtrElement pEl, PtrDocument pDoc, Name nameAttr,
			 int counter, PtrPSchema pSchP, PtrSSchema pSchS)
{
   PtrElement          pElIncluded;
   PtrReference        pRef;
   PtrDocument         pDocIncluded;
   DocumentIdentifier  IdentDocIncluded;

   /* verifie d'abord qu'il s'agit bien d'une inclusion de document */
   {
      if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsReference)
	 pRef = pEl->ElReference;
      else
	 pRef = pEl->ElSource;
      /* accede au document inclus (a sa racine) */
      pElIncluded = ReferredElement (pRef, &IdentDocIncluded, &pDocIncluded);

      /* Transmet au document inclus, en externe */
      AttachCounterValue (pEl, pElIncluded, pDocIncluded, nameAttr, counter, pSchP, pSchS);
      /* Transmet au document inclus, semi expanse */
      AttachCounterValue (pEl, pEl, pDoc, nameAttr, counter, pSchP, pSchS);
   }
}

/*----------------------------------------------------------------------
   ChangeBoxesCounter dans le document dont le contexte pDoc, change
   le contenu de toutes les boites de presentation qui sont
   affectees par le compteur counter du schema de presentation 
   pSchP, apartir de l'element pElBegin.                   
  ----------------------------------------------------------------------*/
static void ChangeBoxesCounter (PtrElement pElBegin, PtrDocument pDoc,
				int counter, PtrPSchema pSchP, PtrSSchema pSS,
				ThotBool redisp)
{
  int                 util, view;
  Counter            *pCo1;
  PtrElement          pEl;
  int                 regle;
  Counter            *pCounter;
  PtrElement          pElRoot;

  /* On traite toutes les boites qui utilisent ce compteur comme contenu */
  pCo1 = &pSchP->PsCounter[counter - 1];
  for (util = 1; util <= pCo1->CnNPresBoxes; util++)
    for (view = 1; view <= MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	{
	  /* Il faut determiner si on doit reevaluer du debut de l'image
	     abstraite ou de pElBegin; on ne reevalue que si CnMinMaxPresBox
	     est TRUE, i.e. une boite est creee par une condition de min ou de
	     max du compteur */
	  if (!pCo1->CnMinMaxPresBox[util - 1])
	    ComputeContent (pCo1->CnPresBox[util - 1], view, pDoc, pSS, pSchP,
			    pElBegin, redisp);
	  else
	    {
	      /* On determine le debut de l'image abstraite */
	      if (pDoc->DocViewRootAb[view - 1] != NULL)
		pElRoot = pDoc->DocViewRootAb[view - 1]->AbElement;
	      else
		pElRoot = NULL;
	      if (pElRoot != NULL)
		{
		  pAbbBegin[view - 1] = NULL;
		  ComputeContent (pCo1->CnPresBox[util - 1], view, pDoc, pSS,
				  pSchP, pElRoot, redisp);
		}
	      else
		ComputeContent (pCo1->CnPresBox[util - 1], view, pDoc, pSS,
				pSchP, pElBegin, redisp);
	    }
	}
  /* On traite toutes les boites dont la creation est conditionnee par */
  /* la valeur de ce compteur */
  for (util = 1; util <= pCo1->CnNCreatedBoxes; util++)
    for (view = 1; view <= MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	{
	  /* Il faut determiner si on doit reevaluer depuis le debut de
	     l'image abstraite ou depuis pElBegin; on ne reevalue que si
	     CnMinMaxCreatedBox est TRUE, i.e. si une boite est creee par
	     une condition de min ou de max du compteur */
	  if (!pCo1->CnMinMaxCreatedBox[util - 1])
	    ComputeCrPresBoxes (pCo1->CnCreatedBox[util - 1], view,
				pSchP, pDoc, pElBegin, redisp);
	  else
	    {
	      /* On determine le debut de l'image abstraite */
	      if (pDoc->DocViewRootAb[view - 1] != NULL)
		pElRoot = pDoc->DocViewRootAb[view - 1]->AbElement;
	      else
		pElRoot = NULL;
	      if (pElRoot != NULL)
		{
		  pAbbBegin[view - 1] = NULL;
		  ComputeCrPresBoxes (pCo1->CnCreatedBox[util - 1], view,
				      pSchP, pDoc, pElRoot, redisp);
		}
	      else
		ComputeCrPresBoxes (pCo1->CnCreatedBox[util - 1], view,
				    pSchP, pDoc, pElBegin, redisp);
	    }
	}
  /* On traite toutes les boites qui creent d'autres boites selon la */
  /* valeur de ce compteur */
  for (util = 1; util <= pCo1->CnNCreators; util++)
    for (view = 1; view <= MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	{
	  /* Il faut determiner si on doit reevaluer du debut de l'image
	     abstraite ou de pElBegin; on ne reevalue que si CnMinMaxCreator
	     est TRUE, i.e. si une boite est creee par une condition de min
	     ou de max du compteur */
	  if (!pCo1->CnMinMaxCreator[util - 1])
	    ComputeCreation (pCo1->CnCreator[util - 1],
			     pCo1->CnPresBoxCreator[util - 1], counter, view,
			     pSS, pSchP, pDoc, pElBegin, redisp);
	  else
	    {
	      /* On determine le debut de l'image abstraite */
	      if (pDoc->DocViewRootAb[view - 1] != NULL)
		pElRoot = pDoc->DocViewRootAb[view - 1]->AbElement;
	      else
		pElRoot = NULL;
	      if (pElRoot != NULL)
		{
		  pAbbBegin[view - 1] = NULL;
		  ComputeCreation (pCo1->CnCreator[util - 1],
				   pCo1->CnPresBoxCreator[util - 1], counter,
				   view, pSS, pSchP, pDoc, pElRoot, redisp);
		}
	      else
		ComputeCreation (pCo1->CnCreator[util - 1],
				 pCo1->CnPresBoxCreator[util - 1], counter,
				 view, pSS, pSchP, pDoc, pElBegin, redisp);
	    }
	}
  /* On traite les regles de transmission des valeurs du compteur */
  pCounter = &pSchP->PsCounter[counter - 1];
  /* examine toutes les regles Transmit du compteur */
  for (regle = 1; regle <= pCounter->CnNTransmAttrs; regle++)
    {
      pEl = pElBegin;
      while (pEl != NULL)
	{
	  if (pEl->ElTypeNumber == pCounter->CnTransmSSchemaAttr[regle - 1])
	    /* c'est le type de document auquel le compteur est */
	    /* transmis, on applique la regle de transmission */
	    TransmitCounterVal (pEl, pDoc, pCounter->CnTransmAttr[regle - 1],
				counter, pSchP, pSS);
	  /* cherche le document inclus suivant */
	  pEl = FwdSearchTypedElem (pEl, pCounter->CnTransmSSchemaAttr[regle - 1], pSS);
	}
    }
}

/*----------------------------------------------------------------------
   UpdateNum1Elem met a jour et fait reafficher les numeros qui       
   apparaissent a partir du sous-arbre pointe par pElBegin 
   (lui-meme compris) et qui sont affectes par l'element   
   pElModif.                                               
  ----------------------------------------------------------------------*/
static void UpdateNum1Elem (PtrElement pElBegin, PtrElement pElModif,
			    PtrDocument pDoc, ThotBool redisp)
{
   int                 counter, oper, i;
   PtrPSchema          pSchP;
   PtrSSchema          pSS;
   int                 index;
   ThotBool            trigger;
   Counter            *pCo1;
   CntrItem           *pCp1;
   SRule              *pRe1;

   /* si l'element pElModif est une marque de page, renumerote les */
   /* les sauts de page qui suivent, a partir de pElBegin. */
   if (pElModif->ElTerminal && pElModif->ElLeafType == LtPageColBreak)
      ComputePageNum (pElBegin, pDoc, pElModif->ElViewPSchema);
   /* cherche le schema de presentation de l'element : pSchP */
   SearchPresSchema (pElModif, &pSchP, &index, &pSS, pDoc);
   if (pSchP != NULL)
      /* cherche les compteurs affectes par pElModif */
      for (counter = 1; counter <= pSchP->PsNCounters; counter++)
	 /* pour tous les compteurs du schema */
	{
	   pCo1 = &pSchP->PsCounter[counter - 1];
	   /* examine toutes les operations du compteur */
	   for (oper = 1; oper <= pCo1->CnNItems; oper++)
	     {
		pCp1 = &pCo1->CnItem[oper - 1];
		/* teste si le type de l'element pElModif trigger */
		/* l'operation sur le compteur */
		trigger = FALSE;
		if (pCp1->CiElemType == index)
		   /* l'element a le type qui declanche l'operation */
		   trigger = TRUE;
		else
		   /* cherche si le type de l'element est equivalent a celui */
		   /* /qui declanche l'operation */
		  {
		     pRe1 = &pSS->SsRule[pCp1->CiElemType - 1];
		     if (pRe1->SrConstruct == CsChoice && pRe1->SrNChoices > 0)
		       {
			  i = 0;
			  do
			    {
			       i++;
			       if (pRe1->SrChoice[i - 1] == index)
				  trigger = TRUE;
			    }
			  while (!trigger && i < pRe1->SrNChoices);
		       }
		  }
		if (trigger)
		   /* l'operation du compteur counter est declanchee par les */
		   /* elements du type de pElModif. */
		   ChangeBoxesCounter (pElBegin, pDoc, counter, pSchP, pSS, redisp);
	     }
	}
}

/*----------------------------------------------------------------------
   UpdateNum                                                          
  ----------------------------------------------------------------------*/
static void UpdateNum (PtrElement pElD, PtrElement pElM, PtrDocument pDoc,
		       ThotBool redisp)
{
   PtrElement          pEl;

   if (pElD != NULL && pElM != NULL)
      /* traite recursivement le sous-arbre pointe par pElM */
     {
	UpdateNum1Elem (pElD, pElM, pDoc, redisp);
	if (!pElM->ElTerminal)
	  {
	     pEl = pElM->ElFirstChild;
	     while (pEl != NULL)
	       {
		  UpdateNum (pElD, pEl, pDoc, redisp);
		  pEl = pEl->ElNext;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   UpdateNumbers pour le document dont le contexte pDoc, met a jour  
   et fait reafficher les numeros qui apparaissent a partir
   du sous-arbre pointe par pElBegin (lui-meme compris) et 
   qui sont affectes par les elements du sous-arbre        
   (racine comprise) pointe par pElModif.                  
  ----------------------------------------------------------------------*/
void UpdateNumbers (PtrElement pElBegin, PtrElement pElModif,
		    PtrDocument pDoc, ThotBool redisp)
{
   int                 i;

   for (i = 1; i <= MAX_VIEW_DOC; i++)
      pAbbBegin[i - 1] = NULL;
   UpdateNum (pElBegin, pElModif, pDoc, redisp);
}

/*----------------------------------------------------------------------
   UpdateBoxesCounter reaffiche toutes les boites de presentation de pDoc,
   qui se trouvent a l'interieur de et apres l'element     
   pointe' par pElBegin et dont le contenu depend du       
   compteur counter defini dans le schema de presentation      
   pSchP.                                                  
  ----------------------------------------------------------------------*/
void UpdateBoxesCounter (PtrElement pElBegin, PtrDocument pDoc, int counter,
			 PtrPSchema pSchP, PtrSSchema pSS)
{
   int                 i;

   for (i = 1; i <= MAX_VIEW_DOC; i++)
      pAbbBegin[i - 1] = NULL;
   ChangeBoxesCounter (pElBegin, pDoc, counter, pSchP, pSS, TRUE);
}

/*----------------------------------------------------------------------
   SetChange marque dans le pave pAbb que la regle de type typeRule a 
   change'.                                                
  ----------------------------------------------------------------------*/
void SetChange (PtrAbstractBox pAb, PRuleType typeRule)
{
  switch (typeRule)
    {
    case PtWidth:
      pAb->AbWidthChange = TRUE;
      break;
    case PtHeight:
      pAb->AbHeightChange = TRUE;
      break;
    case PtMarginTop:
    case PtMarginBottom:
    case PtMarginRight:
    case PtMarginLeft:
    case PtBorderTopColor:
    case PtBorderRightColor:
    case PtBorderBottomColor:
    case PtBorderLeftColor:
    case PtBorderTopStyle:
    case PtBorderRightStyle:
    case PtBorderBottomStyle:
    case PtBorderLeftStyle:
    case PtBorderTopWidth:
    case PtBorderBottomWidth:
    case PtBorderRightWidth:
    case PtBorderLeftWidth:
    case PtPaddingTop:
    case PtPaddingBottom:
    case PtPaddingRight:
    case PtPaddingLeft:
      pAb->AbMBPChange = TRUE;
      break;
    case PtHorizPos:
      pAb->AbHorizPosChange = TRUE;
      break;
    case PtVertPos:
      pAb->AbVertPosChange = TRUE;
      break;
    case PtHorizRef:
      pAb->AbHorizRefChange = TRUE;
      break;
    case PtVertRef:
      pAb->AbVertRefChange = TRUE;
      break;
    case PtSize:
      pAb->AbSizeChange = TRUE;
      break;
    case PtDepth:
    case PtLineStyle:
    case PtLineWeight:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
    case PtXRadius:
    case PtYRadius:
      pAb->AbAspectChange = TRUE;
      break;
    default:
      pAb->AbChange = TRUE;
      break;
    }
}

/*----------------------------------------------------------------------
   ApplyInheritPresRule si la regle de presentation de type typeRule        
   qui doit s'appliquer au pave pAb est une regle         
   d'heritage, on applique cette regle au pave' pointe'    
   par pAb, et on fait de meme sur son sous-arbre.        
  ----------------------------------------------------------------------*/
static void ApplyInheritPresRule (PtrAbstractBox pAb, PRuleType typeRule,
				  PtrDocument pDoc)
{
   PtrPRule            pRPres;
   PtrAbstractBox      pAbbChild;
   PtrPSchema          pSPR;
   PtrAttribute        pA;

   /* on ne traite que si le type de la regle permet l'heritage */
   if (typeRule == PtVisibility
       || typeRule == PtMarginTop
       || typeRule == PtMarginRight
       || typeRule == PtMarginBottom
       || typeRule == PtMarginLeft
       || typeRule == PtPaddingTop
       || typeRule == PtPaddingRight
       || typeRule == PtPaddingBottom
       || typeRule == PtPaddingLeft
       || typeRule == PtBorderTopWidth
       || typeRule == PtBorderRightWidth
       || typeRule == PtBorderBottomWidth
       || typeRule == PtBorderLeftWidth
       || typeRule == PtBorderTopColor
       || typeRule == PtBorderRightColor
       || typeRule == PtBorderBottomColor
       || typeRule == PtBorderLeftColor
       || typeRule == PtBorderTopStyle
       || typeRule == PtBorderRightStyle
       || typeRule == PtBorderBottomStyle
       || typeRule == PtBorderLeftStyle
       || typeRule == PtDepth
       || typeRule == PtSize
       || typeRule == PtStyle
       || typeRule == PtWeight
       || typeRule == PtFont
       || typeRule == PtUnderline
       || typeRule == PtThickness
       || typeRule == PtIndent
       || typeRule == PtLineSpacing
       || typeRule == PtAdjust
       || typeRule == PtHyphenate
       || typeRule == PtLineStyle
       || typeRule == PtLineWeight
       || typeRule == PtFillPattern
       || typeRule == PtBackground
       || typeRule == PtForeground)
      /* cherche la regle de ce type qui s'applique au pave' */
     {
	pRPres = SearchRulepAb (pDoc, pAb, &pSPR, typeRule, FnAny, TRUE, &pA);
	if (pRPres->PrPresMode == PresInherit)
	   /* c'est une regle d'heritage, on l'applique au pave' */
	   if (ApplyRule (pRPres, pSPR, pAb, pDoc, pA))
	      /* le pave est modifie' */
	     {
	       SetChange (pAb, typeRule);
	       /* traite les paves fils */
	       pAbbChild = pAb->AbFirstEnclosed;
	       while (pAbbChild != NULL)
		 {
		   ApplyInheritPresRule (pAbbChild, typeRule, pDoc);
		   pAbbChild = pAbbChild->AbNext;
		 }
	     }
     }
}

/*----------------------------------------------------------------------
   ApplyPresRuleAb applique au pave pAb appartenant au document     
   pDoc la regle de presentation pRule appartenant au     
   schema de presentation pSchP.                           
   S'il s'agit d'une regle de position ou de dimension,    
   que le pave pAb est elastique et que sa boite a ete    
   inversee, on reapplique la regle complementaire         
   (regle de dimension pour une regle de position et       
   inversement).                                           
   Retourne Vrai si la regle a ete appliquee.              
  ----------------------------------------------------------------------*/
static ThotBool ApplyPresRuleAb (PtrPRule pRule, PtrPSchema pSchP,
				 PtrAbstractBox pAb, PtrDocument pDoc,
				 PtrAttribute pAttr)
{
  PtrPRule            pRegle2;
  PtrPSchema          pSchP2;
  PtrAttribute        pAttr2;
  ThotBool            ret;

  /* applique la regle a` appliquer */
  ret = ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
  /* verifie si c'est une regle de dimension ou de position */
  /* qui s'applique a un pave' elastique dont la boite a ete */
  /* inversee. Dans ce cas reapplique la regle de position ou */
  /* de dimension */
  if (ret && pRule != NULL)
    {
      if (pRule->PrType == PtVertPos || pRule->PrType == PtHeight)
	/* c'est une regle de dimension ou de position verticale */
	{
	  if (pAb->AbHeight.DimIsPosition &&
	      /* le pave est elastique verticalement */
	      pAb->AbBox != NULL &&
	      /* le pave possede une boite dans l'image concrete */
	      pAb->AbBox->BxVertInverted)
	    /* la boite a ete inverse'e verticalement, on */
	    {
	      /* reapplique la regle complementaire */
	      if (pRule->PrType == PtVertPos)
		{
		  pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtHeight,
					   FnAny, TRUE, &pAttr2);
		  if (pRegle2 != NULL)
		    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
		      pAb->AbHeightChange = TRUE;
		}
	      else
		{
		  pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtVertPos,
					   FnAny, TRUE, &pAttr2);
		  if (pRegle2 != NULL)
		    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
		      pAb->AbVertPosChange = TRUE;
		}
	    }
	}
      else if (pRule->PrType == PtHorizPos || pRule->PrType == PtWidth)
	{
	  /* c'est une regle de dimension ou de position horiz. */
	  if (pAb->AbWidth.DimIsPosition &&
	      /* le pave est elastique horizontalement */
	      pAb->AbBox != NULL &&
	      /* le pave possede une boite dans l'image concrete */
	      pAb->AbBox->BxHorizInverted)
	    {
	      /* la boite a ete inverse'e horizontalement, on */
	      /* reapplique la regle complementaire */
	      if (pRule->PrType == PtHorizPos)
		{
		  pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtWidth,
					   FnAny, TRUE, &pAttr2);
		  if (pRegle2 != NULL)
		    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
		      pAb->AbWidthChange = TRUE;
		}
	      else
		{
		  pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtHorizPos,
					   FnAny, TRUE, &pAttr2);
		  if (pRegle2 != NULL)
		    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
		      pAb->AbHorizPosChange = TRUE;
		}
	    }
	}
    }
  return ret;
}

/*----------------------------------------------------------------------
   ElemWithinImage  checks if abstract boxes of the element pEl 
   Corresponding to view may be displayed within the abstract image
   part already builded.
  ----------------------------------------------------------------------*/
ThotBool ElemWithinImage (PtrElement pEl, int view, PtrAbstractBox pAbbRoot,
			  PtrDocument pDoc)
{
   ThotBool            result, finished, found;
   PtrElement          pAsc;
   PtrAbstractBox      pAb;

   result = TRUE;
   finished = FALSE;
   /* Is there an abstract box in this view for the element next to this one */
   if (pEl->ElPrevious != NULL)
     {
	pAb = pEl->ElPrevious->ElAbstractBox[view - 1];
	if (pAb != NULL)
	   /* The previous element has an abstract box in the view */
	   {
	   finished = TRUE;
	   /* If the abstract box of the previous element is complete at the
	      end, the element will have its abstract box in the existing
              image */
	   if (pAb->AbInLine || pAb->AbLeafType != LtCompound)
	      result = TRUE;
	   else
	      result = !pAb->AbTruncatedTail;
	   }
     }
   else if (pEl->ElNext != NULL)
      /* There is a next element */
     {
	pAb = pEl->ElNext->ElAbstractBox[view - 1];
	if (pAb != NULL)
	   /* The next element has an abstract box in the view */
	  {
	     finished = TRUE;
	     /* si le pave de l'element suivant est complet en tete, */
	     /* l'element aura son pave dans l'image existante */
	     if (pAb->AbInLine || pAb->AbLeafType != LtCompound)
		/* les paves mis en lignes sont toujours entiers */
		result = TRUE;
	     else
		result = !pAb->AbTruncatedHead;
	  }
     }
   else
      /* l'element n'a aucun voisin. On cherche le premier ascendant qui */
      /* ait un pave dans la vue */
     {
	pAsc = pEl->ElParent;
	found = FALSE;
	while (pAsc != NULL && !found)
	   if (pAsc->ElAbstractBox[view - 1] == NULL)
	      pAsc = pAsc->ElParent;
	   else
	      found = TRUE;
	if (found)
	   if (pAsc->ElAbstractBox[view - 1]->AbInLine ||
	       ((!pAsc->ElAbstractBox[view - 1]->AbTruncatedHead) &&
		(!pAsc->ElAbstractBox[view - 1]->AbTruncatedTail)))
	      /* le premier pave englobant est complet */
	     {
		result = TRUE;
		finished = TRUE;
	     }
     }
   if (!finished && pEl->ElParent == NULL)
      /* c'est un element racine. Il s'agit donc d'une vue qu'on cree */
      /* entierement */
     {
	result = TRUE;
	finished = TRUE;
     }
   if (!finished && pAbbRoot != NULL)
      /* on regarde si notre element est entre l'element qui possede le */
      /* premier pave feuille de la vue et celui qui possede le dernier */
     {
	/* cherche le premier pave feuille */
	pAb = pAbbRoot;
	while (pAb->AbFirstEnclosed != NULL)
	   pAb = pAb->AbFirstEnclosed;
	if (ElemIsBefore (pEl, pAb->AbElement))
	   /* notre element se trouve avant l'element qui a la premiere */
	   /* feuille */
	   if (!pAbbRoot->AbTruncatedHead)
	      /* l'image est complete en tete,on creera l'image de l'element */
	      result = TRUE;
	   else
	      /* il est hors de l'image */
	      result = FALSE;
	else
	   /* notre element se trouve apres l'element qui a la premiere */
	   /* feuille, comment se situe-t-il par rapport a l'element qui */
	   /* a la derniere feuille de l'image ? */
	  {
	     /* cherche d'abord le dernier pave feuille */
	     pAb = pAbbRoot;
	     while (pAb->AbFirstEnclosed != NULL)
	       {
		  pAb = pAb->AbFirstEnclosed;
		  while (pAb->AbNext != NULL)
		     pAb = pAb->AbNext;
	       }
	     if (ElemIsBefore (pEl, pAb->AbElement))
		/* notre element se trouve avant l'element qui a la derniere */
		/* feuille, il est dans l'image */
		result = TRUE;
	     else
		/* notre element se trouve apres l'element qui a la derniere */
		/* feuille */
	     if (!pAbbRoot->AbTruncatedTail)
		/* l'image est complete en queue, on creera l'image de */
		/* l'element */
		result = TRUE;
	     else
		/* il est hors de l'image */
		result = FALSE;
	  }
     }
   return (result);
}

/*----------------------------------------------------------------------
  UpdatePresAttr applies or remove presentation rules attached to the
  attribute pAttr to the current element pEl.
  This change is also performed on enclosed elements if prsentations
  parameters are inherited.
  ----------------------------------------------------------------------*/
void                UpdatePresAttr (PtrElement pEl, PtrAttribute pAttr,
				    PtrElement pElAttr, PtrDocument pDoc,
				    ThotBool remove, ThotBool inherit,
				    PtrAttribute pAttrComp)
{
  PtrPRule            pR, pRuleView1, pRNA, firstOfType;
  PRuleType           typeRule;
  FunctionType        func;
  PtrAbstractBox      pAb, pReaff, pPR;
  PtrAbstractBox      pAbNext, pAbChild, pAbSibling;
  PtrPSchema          pSchP, pSPR;
  PtrAttribute        pAttrib;
  PtrHandlePSchema    pHd;
  TypeUnit            unit;
  int                 view, viewSch, val;
  ThotBool            appl, stop, sameType, found;
  ThotBool            existingView;
  ThotBool            createBox, complete;

  viewSch = 0;
  pAbNext = NULL;
  typeRule = (PRuleType) 0;
  func = (FunctionType) 0;
  /* on applique successivement tous les schemas de presentation en commencant
     par le moins prioritaire : le schema de presentation principal */
  pHd = NULL;
  pSchP = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
  while (pSchP != NULL)
    {
      /* pR: premiere regle correspondant a l'attribut */
      pR = AttrPresRule (pAttr, pEl, inherit, pAttrComp, pSchP);
      firstOfType = pR;
      
      /* traite toutes les regles associees a cette valeur d'attribut dans */
      /* ce schema de presentation */
      while (firstOfType != NULL)
	/* Manage each rule type in all views where the element is presented */
	{
	  typeRule = pR->PrType;
	  /* type des regles courantes */
	  if (typeRule == PtFunction)
	    func = pR->PrPresFunction;
	  pRuleView1 = NULL;
	  if (pR->PrViewNum == 1)
	    /* regle pour la vue 1 */
	    /* on la garde pour le cas ou on ne trouve pas mieux */
	    pRuleView1 = pR;

	  for (view = 1; view <= MAX_VIEW_DOC; view++)
	    {
	      pR = firstOfType;
	      /* check if the view exists */
	      existingView = pDoc->DocView[view - 1].DvPSchemaView > 0;
	      if (existingView)
		{
		  viewSch = AppliedView (pEl, pAttr, pDoc, view);
		  /* look at additional presentation schemas for only the view 1 */
		  existingView = (pHd == NULL || viewSch == 1);
		}
	      if (existingView)
		{
		  /* if it's a creation rule it applies to all views */
		  if (typeRule != PtFunction ||
		      (func != FnCreateBefore && func != FnCreateFirst &&
		       func != FnCreateWith && func == FnCreateLast && func != FnCreateAfter))
		    /* it's not a creation rule, look for a rule of the same
		       type and specific to that view */
		    {
		      pR = firstOfType;
		      stop = FALSE;
		      do
			{
			  sameType = FALSE;
			  if (pR->PrType == typeRule)
			    {
			      if (typeRule != PtFunction)
				sameType = TRUE;
			      else if (pR->PrPresFunction == func)
				sameType = TRUE;
			    }
			  if (!sameType)
			    pR = NULL;
			  else if (pR->PrViewNum == viewSch)
			    stop = TRUE;
			  else
			    pR = pR->PrNextPRule;
			}
		      while (!stop && pR != NULL);
		      
		      if (pR == NULL)
			pR = pRuleView1;
		    }
		  if (pR && pR->PrCond &&
		      !CondPresentation (pR->PrCond, pEl, pAttr, pElAttr,
					 viewSch, pAttr->AeAttrSSchema, pDoc))
		    /* due to conditions the rule doesn't apply */
		    pR = NULL;
		}
	      else
		pR = NULL;

	      if (pR != NULL)
		{
		  /* by default nothing to redisplay */
		  pReaff = NULL;
		  pAb = NULL;
		  createBox = ElemWithinImage (pEl, view, pDoc->DocViewRootAb[view - 1], pDoc);
		  if (createBox && pEl->ElAbstractBox[view - 1] == NULL &&
		      typeRule == PtVisibility)
		    {
		      /* the abstract box doesn't exist and it's a rule Visibility */
		      val = IntegerRule (pR, pEl, view, &appl, &unit, pAttr, NULL);
		      if ((!remove && val > 0) || (remove && val <= 0))
			/* cette regle rend le pave visible et ce n'est pas une suppression
			   ou c'est une suppression et le pave etait rendu invisible par la regle
			   on cree le pave et ses paves de presentation eventuels */
			{
			  pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
			  pAb = AbsBoxesCreate (pEl, pDoc, view, TRUE, TRUE, &complete);
			  if (pAb != NULL)
			    /* au moins un pave a ete cree. pAb est le
			       premier. Cherche le dernier cree */
			    {
			      pAbNext = pAb;
			      stop = FALSE;
			      do
				if (pAbNext->AbNext == NULL)
				  stop = TRUE;
				else if (pAbNext->AbNext->AbElement != pEl)
				  stop = TRUE;
				else
				  pAbNext = pAbNext->AbNext;
			      while (!stop);
			    }
			}
		    }

		  if (createBox &&
		      pEl->ElAbstractBox[view - 1] != NULL && typeRule == PtFunction &&
		      (func == FnCreateBefore || func == FnCreateAfter ||
		       func == FnCreateWith || func == FnCreateFirst || func == FnCreateLast) &&
		      !remove)
		    /* il faut creer un pave de presentation */
		    {
		      pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
		      pAb = CrAbsBoxesPres (pEl, pDoc, pR,
					    pAttr->AeAttrSSchema, pAttr, view,
					    pSchP, TRUE);
		      pAbNext = pAb;
		    }

		  /* traite les paves crees par la regle de visibilite ou de */
		  /* creation */
		  /* code inutile et incorrect (fait dans AbsBoxesCreate) si saut de page */
		  /* il reste correct si on a cree seulement des paves de presentation */
		  if (pAb != NULL)
		    /* les nouveaux paves doivent etre pris en compte par */
		    /* leurs voisins */
		    ApplyRefAbsBoxNew (pAb, pAbNext, &pReaff, pDoc);
		  
		  /* traite les paves qui existaient deja */
		  /* il faut faire une boucle de parcours des paves dupliques
		     de l'element */
		  if (pEl->ElAbstractBox[view - 1] != NULL && pAb == NULL)
		    {
		      pAb = pEl->ElAbstractBox[view - 1];
		      /* saute les paves de presentation de l'element */
		      while (pAb->AbNext != NULL && pAb->AbPresentationBox &&
			     pAb->AbElement == pEl)
			pAb = pAb->AbNext;

		      appl = FALSE;
		      /* on n'a pas applique' la regle */
		      /* applique la regle au pave de l'element s'il n'est
			 pas mort */
		      if (!pAb->AbDead)
			{
			  pRNA = SearchRulepAb (pDoc, pAb, &pSPR, typeRule,
						func, TRUE, &pAttrib);
			  if (pR == pRNA || remove)
			    appl = ApplyPresRuleAb (pRNA, pSPR, pAb, pDoc,
						    pAttrib);
			}
		      if (!appl && remove &&
			  pR->PrType == PtFunction &&
			  pR->PrPresFunction == FnNotInLine)
			/* remove the NotInLine */
			{
			  pAb->AbNotInLine = FALSE;
			  appl = TRUE;
			}
		      
		      if (appl)
			/* on a change' la presentation du pave */
			{
			  if (pAb->AbVisibility <= 0)
			    /* l'attribut rend le pave invisible */
			    {
			      pAb = pEl->ElAbstractBox[view - 1];
			      /* 1er pave a tuer */
			      pReaff = pAb;
			      /* on reaffichera au moins ce pave */
			      do
				/* on tue tous les paves de l'element */
				{
				  SetDeadAbsBox (pAb);
				  /* tue un pave */
				  /* change les regles des autres paves qui se */
				  /* referent au pave detruit */
				  ApplyRefAbsBoxSupp (pAb, &pPR, pDoc);
				  pReaff = Enclosing (pReaff, pPR);
				  pAb = pAb->AbNext;
				  if (pAb == NULL)
				    stop = TRUE;
				  else if (pAb->AbElement != pEl)
				    stop = TRUE;
				  else
				    stop = FALSE;
				}
			      while (!stop);
			    }
			  else
			    /* le pave est toujours visible, mais a change' */
			    {
			      pReaff = pAb;
			      SetChange (pAb, typeRule);
			      /* le parametre de presentation qui vient
				 d'etre change' peut se transmettre par
				 heritage. */
			      /* On traite le sous-arbre. */
			      pAbChild = pAb->AbFirstEnclosed;
			      while (pAbChild != NULL)
				{
				  ApplyInheritPresRule (pAbChild, typeRule, pDoc);
				  pAbChild = pAbChild->AbNext;
				}
			      /* on traite les pavés freres */
			      if (pAb->AbEnclosing)
				{
				  pAbSibling = pAb->AbEnclosing->AbFirstEnclosed;
				  while (pAbSibling != NULL)
				    {
				      ApplyInheritPresRule (pAbSibling, typeRule, pDoc);
				      pAbSibling = pAbSibling->AbNext;
				    }
				}
			    }
			}
		    }

		  if (pEl->ElAbstractBox[view - 1] != NULL
		      && typeRule == PtFunction
		      && (func == FnCreateBefore || func == FnCreateAfter
			  || func == FnCreateWith
			  || func == FnCreateFirst || func == FnCreateLast)
		      && remove)
		    /* on supprime un attribut qui portait une regle de */
		    /* creation ; il faut detruire le pave de presentation que */
		    /* cree la regle pR */
		    /* cherche d'abord le pave de l'element */
		    {
		      pAb = pEl->ElAbstractBox[view - 1];
		      /* saute les paves de presentation de l'element */
		      while (pAb->AbNext != NULL && pAb->AbPresentationBox && pAb->AbElement == pEl)
			pAb = pAb->AbNext;
		      /* cherche le pave de presentation a detruire */
		      found = FALSE;
		      switch (func)
			{
			case FnCreateBefore:
			  pAb = pAb->AbPrevious;
			  break;
			case FnCreateAfter:
			case FnCreateWith:
			  pAb = pAb->AbNext;
			  break;
			case FnCreateFirst:
			  pAb = pAb->AbFirstEnclosed;
			  break;
			case FnCreateLast:
			  /* cherche le dernier pave englobe' */
			  pAb = pAb->AbFirstEnclosed;
			  if (pAb != NULL)
			    while (pAb->AbNext != NULL)
			      pAb = pAb->AbNext;
			  break;
			default:
			  break;
			}
		      if (pAb != NULL)
			do
			  if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
			    {
			    /* c'est un pave marque de page, on passe au pave
			       voisin */
			    if (func == FnCreateBefore || func == FnCreateLast)
			      pAb = pAb->AbPrevious;
			    else
			      pAb = pAb->AbNext;
			    }
			  else if (!pAb->AbPresentationBox ||
				   pAb->AbElement != pEl)
			    /* ce n'est pas un pave' marque de saut de page */
			    /* ce n'est pas un pave de presentation ou ce */
			    /* pave n'appartient pas a l'element */
			    pAb = NULL;
			  else if (pAb->AbTypeNum == pR->PrPresBox[0] &&
				   pAb->AbPSchema == pSchP &&
				   pAb->AbCreatorAttr == pAttr)
			    /* c'est un pave de presentation de l'element */
			    /* ce pave a la type cherche', et il est cre'e' */
			    /* par l'attribut. On a trouve' */
			    found = TRUE;
			  else if (func == FnCreateBefore ||
				   func == FnCreateLast)
			    /* passe au pave voisin */
			    pAb = pAb->AbPrevious;
			  else
			    pAb = pAb->AbNext;
			while (pAb != NULL && !found);

		      if (found)
			/* le pave a detruire est pointe' par pAb */
			if (!pAb->AbDead)
			  {
			    pReaff = pAb;
			    /* on reaffichera au moins ce pave */
			    SetDeadAbsBox (pAb);
			    /* tue le pave */
			    /* change les regles des autres paves qui se */
			    /* referent au pave detruit */
			    ApplyRefAbsBoxSupp (pAb, &pPR, pDoc);
			    pReaff = Enclosing (pReaff, pPR);
			  }
		    }
		  /* conserve le pointeur sur le pave a reafficher */
		  if (pReaff != NULL)
		    pDoc->DocViewModifiedAb[view - 1] =
			 Enclosing (pReaff, pDoc->DocViewModifiedAb[view - 1]);
		}
	    } /* fin de la boucle sur les vues */
	  
	  /* passe a la premiere regle de presentation suivante d'un
	     type different sauf s'il s'agit d'une regle conditionnelle */
	  pR = firstOfType;
	  while (pR != NULL &&
		 pR->PrType == firstOfType->PrType &&
		 pR->PrCond == firstOfType->PrCond &&
		 (pR->PrType != PtFunction ||
		  (pR->PrPresFunction == func &&
		   (pR->PrPresFunction != FnCreateBefore ||
		    pR->PrPresBox[0] == firstOfType->PrPresBox[0]))))
	    pR = pR->PrNextPRule;
	  firstOfType = pR;
	}
      /* on traite les schemas de presentation de plus forte priorite' */
      if (pHd)
	/* on prend le schema de presentation additionnel de priorite' */
	/* superieure */
	pHd = pHd->HdNextPSchema;
      else
	/* on cherchait dans le schema de presentation principal */
	/* on prend le premier schema de presentation additionnel */
	{
	  pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc);
	  /* mais si c'est ID ou CLASS, on prend les extensions du schema
	     de presentation associe' au schema de structure du document */
	  if (AttrHasException (ExcCssClass, pAttr->AeAttrNum,
				pAttr->AeAttrSSchema) ||
	      AttrHasException (ExcCssId, pAttr->AeAttrNum,
				pAttr->AeAttrSSchema))
            pHd = FirstPSchemaExtension (pDoc->DocSSchema, pDoc);
	}
      if (pHd)
	pSchP = pHd->HdPSchema;
      else
	/* plus de schemas additionnels, on arrete */
	pSchP = NULL;
    }
}

/*----------------------------------------------------------------------
   IsIdenticalTextType       retourne 'vrai' si l'element pointe par pEl     
   (appartenant au document pointe par pDoc) a un frere suivant et 
   si les deux elements sont des feuilles de texte dans le meme    
   alphabet, avec les memes attributs et les memes regles de       
   presentation specifiques.                                       
   Dans ce cas, les deux elements sont fusionnes et l'image        
   abstraite est mise a jour. pLib contient au retour un pointeur  
   sur l'element libere par la fusion (cet element n'est pas rendu 
   a la memoire libre).                                            
  ----------------------------------------------------------------------*/
ThotBool IsIdenticalTextType (PtrElement pEl, PtrDocument pDoc,
			      PtrElement * pLib)
{
  PtrElement          pEl2, pEVoisin;
  PtrAbstractBox      pAb;
  int                 view, dvol;
  ThotBool            equal, stop;

  equal = FALSE;
  if (pEl != NULL)
    {
      pEl2 = pEl->ElNext;
      if (pEl2 != NULL && pEl->ElLeafType == LtText && pEl->ElTerminal &&
	  pEl2->ElTerminal && pEl2->ElLeafType == LtText &&
	  pEl2->ElLanguage == pEl->ElLanguage &&
	  !pEl2->ElHolophrast && !pEl->ElHolophrast &&
	  SameAttributes (pEl, pEl2) &&
	  pEl->ElSource == NULL && pEl2->ElSource == NULL &&
	  BothHaveNoSpecRules (pEl, pEl2) &&
	  pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct != CsConstant &&
	  pEl2->ElStructSchema->SsRule[pEl2->ElTypeNumber - 1].SrConstruct != CsConstant)
	{
	  equal = TRUE;
	  if (! MergeTextElements (pEl, pLib, pDoc,
				   TRUE,  /* Notifier l'application => evenements */
				   TRUE)) /* Detruire les paves du 2eme element */
	    return FALSE;
	  if (pEl->ElStructSchema == NULL)
	    /* pEl a ete libere' par l'application */
	    return equal;
	  /* teste si pEl est le dernier fils de son pere, */
	  /* abstraction faite des marques de page */
	  /* fusionne les deux elements de texte */
	  pEVoisin = pEl->ElNext;
	  stop = FALSE;
	  do
	    if (pEVoisin == NULL)
	      /* pEl devient le dernier fils de son pere */
	      {
		ChangeFirstLast (pEl, pDoc, FALSE, FALSE);
		stop = TRUE;
	      }
	    else if (!pEVoisin->ElTerminal
		     || pEVoisin->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	    else
	      pEVoisin = pEVoisin->ElNext;
	  while (!stop);
	  /* met a jour le volume des paves correspondants */
	  for (view = 1; view <= MAX_VIEW_DOC; view++)
	    {
	      pAb = pEl->ElAbstractBox[view - 1];
	      if (pAb != NULL)
		{
		  pAb->AbChange = TRUE;
		  pDoc->DocViewModifiedAb[view - 1] =
		           Enclosing (pAb, pDoc->DocViewModifiedAb[view - 1]);
		  dvol = pEl->ElTextLength - pAb->AbVolume;
		  
		  if (pAb->AbDead && pAb->AbNext != NULL)
		    pAb->AbNext->AbVolume += dvol;
		  
		  do
		    {
		      pAb->AbVolume += dvol;
		      pAb = pAb->AbEnclosing;
		    }
		  while (!(pAb == NULL));
		}
	    }
	}
    }
  return equal;
}
