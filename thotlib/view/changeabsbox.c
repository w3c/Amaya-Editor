/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
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

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "page_tv.h"

#include "absboxes_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "structlist_f.h"
#include "tree_f.h"
#include "writepivot_f.h"

static PtrAbstractBox pAbbBegin[MAX_VIEW_DOC];


/*----------------------------------------------------------------------
   IsEnclosing retourne vrai si le pave pAbb1 englobe le pave pAbb2   
   ou si les deux pointeurs pointent le meme pave.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      IsEnclosing (PtrAbstractBox pAbb1, PtrAbstractBox pAbb2)

#else  /* __STDC__ */
static boolean      IsEnclosing (pAbb1, pAbb2)
PtrAbstractBox      pAbb1;
PtrAbstractBox      pAbb2;

#endif /* __STDC__ */
{
   boolean             ret;
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
#ifdef __STDC__
PtrAbstractBox      Enclosing (PtrAbstractBox pAbb1, PtrAbstractBox pAbb2)
#else  /* __STDC__ */
PtrAbstractBox      Enclosing (pAbb1, pAbb2)
PtrAbstractBox      pAbb1;
PtrAbstractBox      pAbb2;
#endif /* __STDC__ */

{
   PtrAbstractBox      pAbb;
   boolean             found;

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
#ifdef __STDC__
void                UpdateAbsBoxVolume (PtrElement pEl, int view, PtrDocument pDoc)
#else
void                UpdateAbsBoxVolume (pEl, view, pDoc)
   PtrElement pEl;
   int view;
   PtrDocument pDoc;

#endif /* __STDC__ */
{
   int		       dVol;
   PtrAbstractBox      pAb;

   pAb = pEl->ElAbstractBox[view];
   if (pAb != NULL)
      {
	pAb->AbChange = TRUE;
	if (!AssocView (pEl))
	   if (pDoc->DocView[view].DvPSchemaView > 0)
	      pDoc->DocViewModifiedAb[view] =
		Enclosing (pAb, pDoc->DocViewModifiedAb[view]);
	   else
	      pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
		Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
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
#ifdef __STDC__
void                SimpleSearchRulepEl (PtrPRule * pRuleView1, PtrElement pEl, int view, PRuleType typeRule, FunctionType typeFunc, PtrPRule * pRule)
#else  /* __STDC__ */
void                SimpleSearchRulepEl (pRuleView1, pEl, view, typeRule, typeFunc, pRule)
PtrPRule           *pRuleView1;
PtrElement          pEl;
int                 view;
PRuleType           typeRule;
FunctionType        typeFunc;
PtrPRule           *pRule;
#endif /* __STDC__ */

{
  boolean             found;
  PtrPRule            pR;

  pR = *pRule;
  *pRuleView1 = NULL;
  found = FALSE;
  while (!found && pR != NULL)
    {
      if (pR->PrType == typeRule &&
	  (typeRule != PtFunction || typeFunc == FnAny ||
	   pR->PrPresFunction == typeFunc))
	{
	  /* regle du type cherche' */
	  if (pR->PrViewNum == view)
	    /* pour la vue voulue */
	    if (pR->PrCond == NULL ||
		CondPresentation (pR->PrCond, pEl, NULL, view, pEl->ElStructSchema))
	      /* les conditions d'application de la regle sont satisfaites */
	      found = TRUE;
	  
	  if (!found)
	    {
	      if (pR->PrViewNum == 1 && *pRuleView1 == NULL)
		/* regle du type cherche' pour la vue 1 */
		if (pR->PrCond == NULL ||
		    CondPresentation (pR->PrCond, pEl, NULL, view, pEl->ElStructSchema))
		  /* les conditions d'application de la regle sont satisfaites */
		  /* on la garde pour le cas ou on ne trouve pas mieux */
		  *pRuleView1 = pR;
	      /* regle suivante */
	      pR = pR->PrNextPRule;
	    }
	}
      else
	/* ce n'est pas le type de regle cherche' */
	if (pR->PrType > typeRule)
	  /* il n'y a plus de regle de ce type, stop */
	  pR = NULL;
	else
	  /* regle suivante */
	  pR = pR->PrNextPRule;
    }
  if (found)
    *pRule = pR;
  else
    *pRule = NULL;
}


/*----------------------------------------------------------------------
   GlobalSearchRulepEl retourne un pointeur sur la regle de type typeRule       
   a appliquer a l'element pointe' par pEl dans la vue de  
   numero view.                                             
   Retourne dans pSPR un pointeur sur le schema de         
   presentation auquel appartient la regle retournee.      
   Si presNum est nul, on cherche la regle a appliquer     
   a l'element lui-meme, sinon, on cherche de la regle a   
   appliquer a son pave de presentation de numero presNum. 
   Dans ce dernier cas, si pSchP est NULL, le pave de      
   presentation est defini dans le schema de presentation  
   a appliquer a l'element, sinon ce pave est defini dans  
   le schema de presentation pointe' par pSchP.            
   Dans le cas ou pEl pointe une marque de page, si isElPage    
   est vrai, il s'agit de la regle a appliquer au filet de 
   separation sinon il s'agit de la regle a appliquer au   
   bloc de separation de page.                             
   dans V4, le booleen isElPage n'a plus aucun sens             
   Si attr est vrai, on cherche parmi les regles de        
   presentation des attributs de l'element, sinon, on      
   ignore les regles de presentation des attributs.        
   Si on trouve la regle parmi celles qui sont associees   
   a un attribut, au retour pAttr pointe sur le bloc       
   attribut correspondant, sinon pAttr est NULL.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPRule            GlobalSearchRulepEl (PtrElement pEl, PtrPSchema * pSPR, PtrSSchema * pSSR, int presNum, PtrPSchema pSchP, int view, PRuleType typeRule, FunctionType typeFunc, boolean isElPage, boolean attr, PtrAttribute * pAttr)
#else  /* __STDC__ */
PtrPRule            GlobalSearchRulepEl (pEl, pSPR, pSSR, presNum, pSchP, view, typeRule, typeFunc, isElPage, attr, pAttr)
PtrElement          pEl;
PtrPSchema         *pSPR;
PtrSSchema         *pSSR;
int                 presNum;
PtrPSchema          pSchP;
int                 view;
PRuleType           typeRule;
FunctionType        typeFunc;
boolean             isElPage;
boolean             attr;
PtrAttribute       *pAttr;
#endif /* __STDC__ */

{
   int                 index;
   PtrPRule            pRule, pRuleSpecView1, pRuleView1;
   PtrSSchema          pSchS;
   boolean             stop;
   PtrPRule            pR;
   PtrAttribute        pA;
   PtrElement          pElAttr;
   InheritAttrTable   *inheritTable;
   int                 l;
   PtrPSchema          pSP;
   PtrHandlePSchema    pHd;

#ifdef __COLPAGE__
   int                 nb;

#endif /* __COLPAGE__ */

   pRule = NULL;
   *pAttr = NULL;
   *pSPR = NULL;
   if (pEl != NULL)
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
			  if ((*pAttr)->AeAttrNum == pRule->PrSpecifAttr
			      && (*pAttr)->AeAttrSSchema == pRule->PrSpecifAttrSSchema)
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

	/* cherche parmi les regles de presentation des */
	/* attributs de l'element, si on n'a pas encore trouve' */
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
		     pHd = pA->AeAttrSSchema->SsFirstPSchemaExtens;
		  else
		     pHd = NULL;
		  if (pHd == NULL)
		     /* pas de schema additionnel. On prend le schema principal */
		     pSP = pA->AeAttrSSchema->SsPSchema;
		  else
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
		       stop = FALSE;
		       while (pR != NULL && !stop)
			 {
			    if (pR->PrType == typeRule &&
				(typeRule != PtFunction	|| typeFunc == FnAny ||
				 pR->PrPresFunction == typeFunc) &&
				pR->PrViewNum == view)
			       /* regle du type cherche' pour la vue voulue */
			       if (pR->PrCond == NULL ||
				   CondPresentation (pR->PrCond, pEl, pA, view, pA->AeAttrSSchema))
				  /* les conditions d'application de la regle sont */
				  /* satisfaites */
				 {
				    pRule = pR;
				    *pAttr = pA;
				    *pSPR = pSP;
				    *pSSR = pA->AeAttrSSchema;
				    stop = TRUE;
				 }
			    if (!stop)
			       pR = pR->PrNextPRule;	/* regle suivante */
			 }
		       if (pR == NULL)
			  /* on n'a pas encore trouve'. On continue de chercher dans */
			  /* les schemas de presentation de moindre priorite' */
			 {
			    if (pHd == NULL)
			       /* on cherchait dans le schema de presentation principal. */
			       /* c'est fini */
			       pSP = NULL;
			    else
			      {
				 /* on prend le schema de presentation additionnel de */
				 /* moindre priorite' */
				 pHd = pHd->HdPrevPSchema;
				 if (pHd == NULL)
				    /* plus de schemas additionnels, on prend le schema princ */
				    pSP = pA->AeAttrSSchema->SsPSchema;
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
		if (pEl->ElStructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
		  {
		     /* il y a heritage possible */
		     if ((inheritTable = pEl->ElStructSchema->SsPSchema->
			  PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		       {
			  /* cette table n'existe pas on la genere */
			  CreateInheritedAttrTable (pEl);
			  inheritTable = pEl->ElStructSchema->SsPSchema->
			     PsInheritedAttr[pEl->ElTypeNumber - 1];
		       }
		     for (l = 1; l <= pEl->ElStructSchema->SsNAttributes && *pSPR == NULL;
			  l++)
			if ((*inheritTable)[l - 1])
			   /* pEl herite de l'attribut l */
			   /* cherche si l'attribut l existe au dessus */
			   if ((*pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElStructSchema,
							 &pElAttr)) != NULL)
			     {
				/* on traite d'abord les schemas de presentation */
				/* additionnels les plus prioritaires si on travaille pour */
				/* la vue principale. Sinon, on ignore les schemas */
				/* additionnels */
				if (view == 1)
				   pHd = (*pAttr)->AeAttrSSchema->SsFirstPSchemaExtens;
				else
				   pHd = NULL;
				if (pHd == NULL)
				   /* pas de schema additionnel. Prend le schema principal */
				   pSP = (*pAttr)->AeAttrSSchema->SsPSchema;
				else
				   /* on cherche le schema additionnel le plus proritaire */
				  {
				     while (pHd->HdNextPSchema != NULL)
					pHd = pHd->HdNextPSchema;
				     pSP = pHd->HdPSchema;
				  }
				stop = FALSE;
				while (!stop && pSP != NULL)
				  {
				     /* premiere regle de presentation de cet attribut dans */
				     /* ce schema de presentation */
				     pRule = AttrPresRule (*pAttr, pEl, TRUE, NULL, pSP);
				     /* parcourt les regles de presentation de l'attribut */
				     while (pRule != NULL && !stop)
				       {
					  if (pRule->PrType == typeRule &&
					      (typeRule != PtFunction ||
						typeFunc == FnAny ||
						pRule->PrPresFunction == typeFunc) &&
					      pRule->PrViewNum == view)
					     /* regle du type cherche' pour la vue voulue */
					     if (pRule->PrCond == NULL ||
						 CondPresentation (pRule->PrCond, pEl, *pAttr,
					     view, (*pAttr)->AeAttrSSchema))
						/* les conditions d'application de la regle */
						/* sont satisfaites */
					       {
						  *pSPR = pSP;
						  *pSSR = (*pAttr)->AeAttrSSchema;
						  stop = TRUE;
					       }
					  if (!stop)
					     /* regle suivante */
					     pRule = pRule->PrNextPRule;
				       }
				     if (!stop)
					/* on n'a pas encore trouve'. On continue de chercher */
					/* dans les schemas de presentation de moindre priorite */
				       {
					  if (pHd == NULL)
					     /* on cherchait dans le schema de presentation */
					     /* principal. C'est fini */
					     pSP = NULL;
					  else
					    {
					       /* on prend le schema de presentation additionnel de */
					       /* moindre priorite' */
					       pHd = pHd->HdPrevPSchema;
					       if (pHd == NULL)
						  /* plus de schemas additionnels, on prend le schema */
						  /* principal */
						  pSP = (*pAttr)->AeAttrSSchema->SsPSchema;
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
#ifdef __COLPAGE__
	     if (presNum == 0 && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
	       {
		  /* on cherche le type de la boite page ou colonne */
		  if (pEl->ElPageType == PgBegin
		      || pEl->ElPageType == PgComputed
		      || pEl->ElPageType == PgUser)
		     presNum = GetPageBoxType (pEl, view, &pSchP);
		  else		/* cas de colonne */
		     presNum = TypeBCol (pEl, view, &pSchP, &nb);
#else  /* __COLPAGE__ */
	     if (presNum == 0 && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak && isElPage)
	       {
		  /* on cherche le type de la boite page */
		  presNum = GetPageBoxType (pEl, view, &pSchP);
#endif /* __COLPAGE__ */
		  pSchS = pEl->ElStructSchema;
		  index = pEl->ElTypeNumber;
	       }
	     else if (presNum == 0 || pSchP == NULL)
		/* cherche le schema de presentation de l'element */
		SearchPresSchema (pEl, &pSchP, &index, &pSchS);
	     /* on traite d'abord les schemas de presentation additionnels les */
	     /* plus prioritaires */
	     pHd = pSchS->SsFirstPSchemaExtens;
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
		     if (presNum > 0)
			pRule = pSP->PsPresentBox[presNum - 1].PbFirstPRule;
		     else
			pRule = pSP->PsElemPRule[index - 1];
		  /* cherche une regle du type voulu, pour la vue voulue, parmi */
		  /* les regles du type d'element */
		  pRuleView1 = NULL;
		  SimpleSearchRulepEl (&pRuleView1, pEl, view, typeRule, typeFunc, &pRule);
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
		  SimpleSearchRulepEl (&pRuleView1, pEl, view, typeRule, typeFunc, &pRule);
		  if (pRule == NULL)
		     if (pRuleSpecView1 != NULL)
			pRule = pRuleSpecView1;
		     else
			pRule = pRuleView1;
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
   SearchRulepAb retourne un pointeur sur la regle de type typeRule        
   a appliquer au pave pAb.                               
   Selon que attr est vrai ou faux, on tient compte ou non 
   des attributs porte's par l'element auquel correspond   
   pAb.                                                   
   Au retour, pSPR contient un pointeur sur le schema de   
   structure auquel appartient la regle.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPRule            SearchRulepAb (PtrDocument pDoc, PtrAbstractBox pAb, PtrPSchema * pSPR, PRuleType typeRule, FunctionType typeFunc, boolean attr, PtrAttribute * pAttr)
#else  /* __STDC__ */
PtrPRule            SearchRulepAb (pDoc, pAb, pSPR, typeRule, typeFunc, attr, pAttr)
PtrDocument         pDoc;
PtrAbstractBox      pAb;
PtrPSchema         *pSPR;
PRuleType           typeRule;
FunctionType        typeFunc;
boolean             attr;
PtrAttribute       *pAttr;

#endif /* __STDC__ */

{
  int                 presNum;
  PtrSSchema          pSSR;
  PtrPRule            pRuleFound;
  
  if (pAb == NULL)
    pRuleFound = NULL;
  else
    {
      if (pAb->AbPresentationBox)
	presNum = pAb->AbTypeNum;
      else
	presNum = 0;
      pRuleFound = GlobalSearchRulepEl (pAb->AbElement, pSPR, &pSSR, presNum,
					pAb->AbPSchema, pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView,
					typeRule, typeFunc, FALSE, attr, pAttr);
    }
  return pRuleFound;
}



/*----------------------------------------------------------------------
   IsDiffPosition retourne vrai si la position abPos est differente de 
   la position Posit (variable de la fonction NouvRef).    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      IsDiffPosition (AbPosition abPos, AbPosition * abPosit, boolean isInverted)
#else  /* __STDC__ */
static boolean      IsDiffPosition (abPos, abPosit, isInverted)
AbPosition          abPos;
AbPosition         *abPosit;
boolean             isInverted;

#endif /* __STDC__ */
{
   AbPosition         *pAbbox1;
   boolean             different;

   pAbbox1 = &abPos;
   different = TRUE;
   if (pAbbox1->PosRefEdge == abPosit->PosRefEdge &&
       pAbbox1->PosRefEdge == abPosit->PosRefEdge &&
       pAbbox1->PosDistance == abPosit->PosDistance &&
       pAbbox1->PosUnit == abPosit->PosUnit &&
       pAbbox1->PosUserSpecified == abPosit->PosUserSpecified &&
       pAbbox1->PosAbRef == abPosit->PosAbRef)
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
   return different;
}



/*----------------------------------------------------------------------
   IsDiffDimension retourne vrai si la dimension abDim est differente  
   de la dimension abDimens (variable de la fonction         
   NouvRef).                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      IsDiffDimension (AbDimension abDim, AbDimension * abDimens)

#else  /* __STDC__ */
static boolean      IsDiffDimension (abDim, abDimens)
AbDimension         abDim;
AbDimension        *abDimens;

#endif /* __STDC__ */

{
   AbDimension        *pAbbox1;
   boolean             different;

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

#ifdef __STDC__
static boolean      IsNewPosOrDim (PtrAbstractBox pAb, PtrPRule pR, PtrPSchema pSPR, Level levelPos, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
static boolean      IsNewPosOrDim (pAb, pR, pSPR, levelPos, pDoc, pAttr)
PtrAbstractBox      pAb;
PtrPRule            pR;
PtrPSchema          pSPR;
Level               levelPos;
PtrDocument         pDoc;
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   boolean             ret;
   AbPosition          abPosit;
   AbDimension         Dimens;
   PosRule            *pRe1;
   DimensionRule      *pRelD1;
   PtrPRule            pRegleDim;
   PtrPSchema          pSPRDim;
   PtrAttribute        pAttrDim;
   AbPosition          abDimElast;

#ifdef __COLPAGE__
   boolean             bool;

#else  /* __COLPAGE__ */
   boolean             nextToPage;

#endif /* __COLPAGE__ */

   ret = FALSE;
   if (pR != NULL)
     {
#ifndef __COLPAGE__
	/* s'il s'agit d'un positionnement vertical et que le pave precedent */
	/* est une marque de page, il faut reevaluer la regle */
	nextToPage = FALSE;
	if (pR->PrType == PtVertPos)
	   if (pAb->AbPrevious != NULL)
	      if (pAb->AbPrevious->AbElement->ElTypeNumber == PageBreak + 1)
		 nextToPage = TRUE;
#endif /* __COLPAGE__ */
	switch (pR->PrType)
	      {
		 case PtVertRef:
		 case PtHorizRef:
		 case PtVertPos:
		 case PtHorizPos:
		    pRe1 = &pR->PrPosRule;
		    if (pRe1->PoRelation == levelPos || pRe1->PoRelation == RlReferred
			|| (pRe1->PoRelation == RlSameLevel
			  && (levelPos == RlPrevious || levelPos == RlNext))
#ifndef __COLPAGE__
			|| nextToPage
#endif /* __COLPAGE__ */
			|| (pR->PrType == PtVertPos && pAb->AbHeight.DimIsPosition)
			|| (pR->PrType == PtHorizPos && pAb->AbWidth.DimIsPosition))
		       switch (pR->PrType)
			     {
				case PtVertRef:
				   abPosit = pAb->AbVertRef;
#ifdef __COLPAGE__
				   (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				   (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				   ret = IsDiffPosition (pAb->AbVertRef, &abPosit, FALSE);
				   break;
				case PtHorizRef:
				   abPosit = pAb->AbHorizRef;
#ifdef __COLPAGE__
				   (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				   (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				   ret = IsDiffPosition (pAb->AbHorizRef, &abPosit, FALSE);
				   break;
				case PtVertPos:
				   if (pAb->AbHeight.DimIsPosition)
				      /* on reevalue la dimension d'un pave elastique en meme temps */
				      /* que sa position */
				     {
					/* on conserve la position et la dimension d'origine du pave' */
					abPosit = pAb->AbVertPos;
					abDimElast = pAb->AbHeight.DimPosition;
					/* applique la regle de position */
#ifdef __COLPAGE__
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					/* cherche et applique la regle de dimension */
					pRegleDim = SearchRulepAb (pDoc, pAb, &pSPRDim, PtHeight, FnAny, TRUE, &pAttrDim);
#ifdef __COLPAGE__
					(void) ApplyRule (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim, &bool);
#else  /* __COLPAGE__ */
					(void) ApplyRule (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim);
#endif /* __COLPAGE__ */
					/* compare la position et la dimension d'origine avec celles qui */
					/* qui viennent d'etre calculees */
					if (pAb->AbBox != NULL)
					   if (pAb->AbBox->BxVertInverted)
					      /* le mediateur avait inverse' position et dimension */
					     {
						/* compare avec inversion */
						if (IsDiffPosition (pAb->AbVertPos, &abPosit, TRUE) ||
						    IsDiffPosition (pAb->AbHeight.DimPosition, &abDimElast, TRUE))
						   /* il y a eu au moins un changement */
						  {
						     ret = TRUE;
						     pAb->AbHeightChange = TRUE;
						     pAb->AbVertPosChange = TRUE;
						     if (pAb->AbLeafType == LtGraphics)
							/* retablit le caractere graphique qui a ete inverse' */
							if (pAb->AbElement->ElTerminal)
							   if (pAb->AbElement->ElLeafType == LtGraphics)
							      pAb->AbShape = pAb->AbElement->ElGraph;
						  }
						else
						   /* pas de changement, on retablit position et dimension */
						   /* d'origine */
						  {
						     ret = FALSE;
						     pAb->AbVertPos = abPosit;
						     pAb->AbHeight.DimPosition = abDimElast;
						  }
					     }
					   else
					      /* pas d'inversion position/dimension */
					     {
						if (IsDiffPosition (pAb->AbVertPos, &abPosit, FALSE) ||
						    IsDiffPosition (pAb->AbHeight.DimPosition, &abDimElast, FALSE))
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
				   else
				     {
					abPosit = pAb->AbVertPos;
#ifdef __COLPAGE__
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					ret = IsDiffPosition (pAb->AbVertPos, &abPosit, FALSE);
				     }
				   break;
				case PtHorizPos:
				   if (pAb->AbWidth.DimIsPosition)
				      /* on reevalue la dimension d'un pave elastique en meme temps */
				      /* que sa position */
				     {
					/* on conserve la position et la dimension d'origine du pave' */
					abPosit = pAb->AbHorizPos;
					abDimElast = pAb->AbWidth.DimPosition;
					/* applique la regle de position */
#ifdef __COLPAGE__
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					/* cherche et applique la regle de dimension */
					pRegleDim = SearchRulepAb (pDoc, pAb, &pSPRDim, PtWidth, FnAny, TRUE, &pAttrDim);
#ifdef __COLPAGE__
					(void) ApplyRule (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim, &bool);
#else  /* __COLPAGE__ */
					(void) ApplyRule (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim);
#endif /* __COLPAGE__ */
					/* compare la position et la dimension d'origine avec celles qui */
					/* qui viennent d'etre calculees */
					if (pAb->AbBox != NULL)
					   if (pAb->AbBox->BxHorizInverted)
					      /* le mediateur avait inverse' position et dimension */
					     {
						/* compare avec inversion */
						if (IsDiffPosition (pAb->AbHorizPos, &abPosit, TRUE) ||
						    IsDiffPosition (pAb->AbWidth.DimPosition, &abDimElast, TRUE))
						   /* il y a eu au moins un changement */
						  {
						     ret = TRUE;
						     pAb->AbWidthChange = TRUE;
						     pAb->AbHorizPosChange = TRUE;
						     if (pAb->AbLeafType == LtGraphics)
							/* retablit le caractere graphique qui a ete inverse' */
							if (pAb->AbElement->ElTerminal)
							   if (pAb->AbElement->ElLeafType == LtGraphics)
							      pAb->AbShape = pAb->AbElement->ElGraph;
						  }
						else
						   /* pas de changement, on retablit position et dimension */
						   /* d'origine */
						  {
						     ret = FALSE;
						     pAb->AbHorizPos = abPosit;
						     pAb->AbWidth.DimPosition = abDimElast;
						  }
					     }
					   else
					      /* pas d'inversion position/dimension */
					     {
						if (IsDiffPosition (pAb->AbHorizPos, &abPosit, FALSE) ||
						    IsDiffPosition (pAb->AbWidth.DimPosition, &abDimElast, FALSE))
						   /* il y a eu au moins un changement */
						  {
						     ret = TRUE;
						     pAb->AbWidthChange = TRUE;
						     pAb->AbHorizPosChange = TRUE;
						  }
						else
						   /* pas de changement */
						   ret = FALSE;
					     }
				     }
				   else
				     {
					abPosit = pAb->AbHorizPos;
#ifdef __COLPAGE__
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					ret = IsDiffPosition (pAb->AbHorizPos, &abPosit, FALSE);
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
#ifdef __COLPAGE__
			      (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
			      (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
			      ret = TRUE;
			   }
		      }
		    else
		       /* regle de dimension non elastique */
		    if (!Dimens.DimIsPosition)
		       if (!pRelD1->DrAbsolute)
			  /* ce n'est pas une dimension absolue */
			  if (pRelD1->DrRelation == levelPos
			      || (pRelD1->DrRelation == RlSameLevel
			  && (levelPos == RlPrevious || levelPos == RlNext)))
			     if (pR->PrType == PtHeight)
				/* hauteur */
			       {
#ifdef __COLPAGE__
				  (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				  (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				  ret = IsDiffDimension (pAb->AbHeight, &Dimens);
			       }
			     else
				/* largeur */
			       {
#ifdef __COLPAGE__
				  (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				  (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				  ret = IsDiffDimension (pAb->AbWidth, &Dimens);
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

#ifdef __STDC__
static boolean      ReapplRef (PtrAbstractBox pRef, PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
static boolean      ReapplRef (pRef, pAb, pDoc)
PtrAbstractBox      pRef;
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   boolean             ret;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;
   PtrPRule            pR;
   PtrAbstractBox      pAbbox1;
   AbPosition         *pPosAb;
   AbDimension        *pDimAb;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

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
#ifdef __COLPAGE__
	appl = ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
	     (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	     (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
	pDimAb->DimValue = 0;
	pDimAb->DimAbRef = NULL;
	pDimAb->DimUnit = UnRelative;
	pDimAb->DimSameDimension = TRUE;
	pDimAb->DimUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtHeight, FnAny, TRUE, &pAttr);
#ifdef __COLPAGE__
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
	     (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	     (void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
	pDimAb->DimValue = 0;
	pDimAb->DimAbRef = NULL;
	pDimAb->DimUnit = UnRelative;
	pDimAb->DimSameDimension = TRUE;
	pDimAb->DimUserSpecified = FALSE;
	pR = SearchRulepAb (pDoc, pAb, &pSPR, PtWidth, FnAny, TRUE, &pAttr);
#ifdef __COLPAGE__
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) ApplyRule (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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

#ifdef __STDC__
PtrPRule            FunctionRule (PtrElement pEl, PtrPSchema * pSchP)

#else  /* __STDC__ */
PtrPRule            FunctionRule (pEl, pSchP)
PtrElement          pEl;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

{
   PtrPRule            pRule;
   int                 index;
   PtrSSchema          pSchS;

   pRule = NULL;
   SearchPresSchema (pEl, pSchP, &index, &pSchS);
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

#ifdef __STDC__
void                SetDeadAbsBox (PtrAbstractBox pAb)

#else  /* __STDC__ */
void                SetDeadAbsBox (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */

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

#ifdef __STDC__
void                ApplyRefAbsBoxSupp (PtrAbstractBox pAb, PtrAbstractBox * pAbbReDisp, PtrDocument pDoc)

#else  /* __STDC__ */
void                ApplyRefAbsBoxSupp (pAb, pAbbReDisp, pDoc)
PtrAbstractBox      pAb;
PtrAbstractBox     *pAbbReDisp;
PtrDocument         pDoc;

#endif /* __STDC__ */

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

#ifdef __STDC__
void                ApplyRefAbsBoxNew (PtrAbstractBox pAbbFirst, PtrAbstractBox pAbbLast, PtrAbstractBox * pAbbReDisp, PtrDocument pDoc)

#else  /* __STDC__ */
void                ApplyRefAbsBoxNew (pAbbFirst, pAbbLast, pAbbReDisp, pDoc)
PtrAbstractBox      pAbbFirst;
PtrAbstractBox      pAbbLast;
PtrAbstractBox     *pAbbReDisp;
PtrDocument         pDoc;

#endif /* __STDC__ */

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

#ifdef __STDC__
static PtrAbstractBox AbsBoxPresType (PtrAbstractBox pAbb, PtrPRule pRPres, PtrPSchema pSchP)

#else  /* __STDC__ */
static PtrAbstractBox AbsBoxPresType (pAbb, pRPres, pSchP)
PtrAbstractBox      pAbb;
PtrPRule            pRPres;
PtrPSchema          pSchP;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb, pAbbMain;
   boolean             found, stop;	/* 1er pave de l'element dans la vue */
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
#ifdef __COLPAGE__
	if (pRPres->PrPresFunction == FnCreateFirst)
	   pAb = pAbbMain->AbFirstEnclosed;	/* 1er descendant */
	else if (pRPres->PrPresFunction == FnCreateLast)
	  {
	     while (pAbbMain->AbNextRepeated != NULL)
		pAbbMain = pAbbMain->AbNextRepeated;
	     pAb = pAbbMain->AbFirstEnclosed;	/* 1er descendant */
	     /* cherche le dernier descendant du pave principal */
	     if (pAb != NULL)
		while (pAb->AbNext != NULL)
		   pAb = pAb->AbNext;
	  }
	else if (pRPres->PrPresFunction == FnCreateBefore)
	   pAb = pAbbMain->AbPrevious;
	else if (pRPres->PrPresFunction == FnCreateAfter)
	  {
	     while (pAbbMain->AbNextRepeated != NULL)
		pAbbMain = pAbbMain->AbNextRepeated;
	     pAb = pAbbMain->AbNext;
	  }
#else  /* __COLPAGE__ */
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
#endif /* __COLPAGE__ */
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

#ifdef __STDC__
static void         ApplFunctionPresRules (PtrPRule pRule, PtrPSchema pSchP, PtrAttribute pAttr, PtrDocument pDoc, PtrElement pEl, boolean change, boolean first)

#else  /* __STDC__ */
static void         ApplFunctionPresRules (pRule, pSchP, pAttr, pDoc, pEl, change, first)
PtrPRule            pRule;
PtrPSchema          pSchP;
PtrAttribute        pAttr;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             change;
boolean             first;

#endif /* __STDC__ */

{
   int                 view;
   PtrAbstractBox      pAb, pAbbReDisp;
   boolean             stop;
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
				     pAb = CrAbsBoxesPres (pEl, pDoc, pRule, pEl->ElStructSchema,
					   pAttr, view, pSchP, FALSE, TRUE);
				     if (pAb != NULL)
					/* un pave de presentation a ete cree */
					/* change les regles des autres paves qui peuvent */
					/* se referer au pave cree' */
				       {
					  ApplyRefAbsBoxNew (pAb, pAb, &pAbbReDisp, pDoc);
					  pAbbReDisp = Enclosing (pAb, pAbbReDisp);
					  /* conserve le pointeur sur le pave a reafficher */
					  if (AssocView (pEl))
					     pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
						Enclosing (pAbbReDisp,
					      pDoc->DocAssocModifiedAb[pEl->
							   ElAssocNum - 1]);
					  else
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
					  if (AssocView (pEl))
					     pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
						Enclosing (pAbbReDisp, pDoc->
							   DocAssocModifiedAb[pEl->ElAssocNum - 1]);
					  else
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

#ifdef __STDC__
void                ChangeFirstLast (PtrElement pEl, PtrDocument pDoc, boolean first, boolean change)

#else  /* __STDC__ */
void                ChangeFirstLast (pEl, pDoc, first, change)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             first;
boolean             change;

#endif /* __STDC__ */

{
   PtrPRule            pRPres;
   PtrPSchema          pSchP;
   PtrAttribute        pAttr;
   PtrElement          pElAttr;
   int                 l;
   InheritAttrTable   *inheritTable;

   if (pEl != NULL)
     {
	/* cherche la 1ere fonction de presentation associee au type de */
	/* l'element */
	pRPres = FunctionRule (pEl, &pSchP);
	if (pSchP != NULL)
	  {
	     /* traite les regles de creation associees au type de l'element */
	     pAttr = NULL;
	     ApplFunctionPresRules (pRPres, pSchP, pAttr, pDoc, pEl, change, first);
	     /* l'element herite-t-il d'attributs qui ont des fonctions de */
	     /* presentation */
	     if (pEl->ElStructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	       {
		  /* il y a heritage possible */
		  if ((inheritTable =
		       pEl->ElStructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		    {
		       /* cette table n'existe pas on la genere */
		       CreateInheritedAttrTable (pEl);
		       inheritTable = pEl->ElStructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1];
		    }
		  for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
		     if ((*inheritTable)[l - 1])	/* pEl herite de l'attribut l */
			/* cherche si l existe au dessus */
			if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElStructSchema,
							 &pElAttr)) != NULL)
			  {
			     pSchP = pAttr->AeAttrSSchema->SsPSchema;
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
		  pSchP = pAttr->AeAttrSSchema->SsPSchema;
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
   GetPageBreakForAssoc  cherche si l'element associe pEl doit etre affiche 
   dans une boite de haut ou de bas de page, pour la vue   
   viewNb.                                                  
   Si oui, retourne un pointeur sur l'element marque de    
   page ou l'element associe doit s'afficher et retourne   
   dans boxType le numero du type de boite de haut ou de 
   bas de page qui doit contenir l'element associe.        
   Sinon, retourne Nil.                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          GetPageBreakForAssoc (PtrElement pEl, int viewNb, int *boxType)

#else  /* __STDC__ */
PtrElement          GetPageBreakForAssoc (pEl, viewNb, boxType)
PtrElement          pEl;
int                 viewNb;
int                *boxType;

#endif /* __STDC__ */

{
   PtrPSchema          pSchP;
   int                 index, b;
   PtrElement          pElPage, pAsc, pElRef;
   PtrPRule            pR, pRP;
   PtrSSchema          pSchS;
   PtrReference        pRef;
   boolean             stop, stop1, ref;
   PtrPSchema          pSc1;
   PresentationBox    *pBo1;

   pElPage = NULL;
   *boxType = 0;
   /* cherche le schema de presentation a appliquer a l'element */
   SearchPresSchema (pEl, &pSchP, &index, &pSchS);
   if (pSchP != NULL)
      if (!AssocView (pEl))
	 /* il s'agit d'un element associe' a afficher dans une boite de */
	 /* haut ou de bas de page */
	{
	   pElRef = NULL;
	   /* l'element referencant n'est pas encore trouve' */
	   if (pEl->ElReferredDescr != NULL)
	      /* il y a des references a cet element */
	      if (pEl->ElReferredDescr->ReFirstReference != NULL)
		 /* on exclut les references par attribut et on evite les */
		 /* references appartenant a des elements associes */
		{
		   ref = FALSE;
		   pRef = pEl->ElReferredDescr->ReFirstReference;
		   /* 1ere reference */
		   do
		     {
			if (pRef->RdAttribute == NULL)

			   /* ce n'est pas une reference par attribut */
			  {
			     pElRef = pRef->RdElement;
			     /* l'element referencant */
			     if (pElRef->ElAssocNum == 0)
				/* l'element referencant n'est pas dans un element */
				/* associe', on a trouve' */
				ref = TRUE;
			  }
			if (!ref)
			   /* reference suivante a l'element */
			   pRef = pRef->RdNext;
		     }
		   while (!ref && pRef != NULL);
		}
	   if (pElRef == NULL)
	      /* cet element n'est pas reference' par un autre element */
	     {
		pSc1 = pSchP;
		if (pSc1->PsInPageHeaderOrFooter[pEl->ElTypeNumber - 1])
		   /* l'element devrait s'afficher dans un haut ou bas de page */
		   /* cherche dans le schema de presentation le type de boite de */
		   /* presentation qui contient ce type d'element */
		  {
		     b = 1;
		     while (b <= pSc1->PsNPresentBoxes && *boxType == 0)
		       {
			  pBo1 = &pSc1->PsPresentBox[b - 1];
			  if (pBo1->PbContent == ContElement
			      && pBo1->PbContElem == pEl->ElTypeNumber)
			     *boxType = b;
			  else
			     b++;
		       }
		  }
	     }
	   else
	      /* cet element est reference' */
	      /* n premiere reference sur cet element */
	     {
		pElPage = pElRef;
		/* cherche le type de la boite de haut ou bas de page ou doit */
		/* s'afficher l'element associe. Pour cela cherche le premier */
		/* element qui englobe la 1ere reference et qui possede une regle */
		/* Page. */
		pAsc = pElPage;
		do
		   if (pAsc != NULL)
		      /* cherche le schema de presentation de l'element */
		     {
			SearchPresSchema (pAsc, &pSchP, &index, &pSchS);
			if (pSchP == NULL)
			   pR = NULL;
			else
			   /* 1ere regle de presentation de l'element */
			   pR = pSchP->PsElemPRule[index - 1];
			/* cherche les regles Page de cet element */
			stop = FALSE;
			do
			   if (pR == NULL)
			      stop = TRUE;
			   else if (pR->PrType > PtFunction
				    || *boxType != 0)
			      stop = TRUE;
			   else
			     {
				if (pR->PrType == PtFunction
				    && pR->PrPresFunction == FnPage
				    && pR->PrViewNum == viewNb)
				   /* c'est une regle Page concernant la vue traitee */
				   /* cherche les regles de creation de la boite page */
				   /* pour trouver les boites de haut et bas de page  */
				  {
				     pRP = pSchP->PsPresentBox[pR->PrPresBox[0] - 1].
					PbFirstPRule;
				     stop1 = FALSE;
				     do
					if (pRP == NULL)
					   stop1 = TRUE;
					else if (pRP->PrType > PtFunction
						 || *boxType != 0)
					   stop1 = TRUE;
					else
					  {
					     if (pRP->PrType == PtFunction
						 && (pRP->PrPresFunction == FnCreateBefore
						     || pRP->PrPresFunction == FnCreateAfter)
						 && pRP->PrViewNum == viewNb)
						/* c'est une regle de creation de boite de haut */
						/* ou de bas de page */
						/* la boite creee contient-elle ce type */
						/* d'elements associes ? */
					       {
						  pBo1 = &pSchP->PsPresentBox[pRP->
							  PrPresBox[0] - 1];
						  if (pBo1->PbContent == ContElement
						      && pBo1->PbContElem == pEl->ElTypeNumber)
						     *boxType = pRP->PrPresBox[0];
					       }
					     pRP = pRP->PrNextPRule;
					     /* regle suivante de la page */
					  }
				     while (!stop1);
				  }
				pR = pR->PrNextPRule;
				/* regle suivante de l'element */
			     }
			while (!stop);
			if (*boxType == 0)
			   pAsc = pAsc->ElParent;
			/* passe a l'element englobant */
		     }
		while (pAsc != NULL && *boxType == 0) ;
		if (*boxType == 0)
		   if (pElPage->ElAssocNum != 0
		       && pElPage->ElAssocNum == pEl->ElAssocNum)
		      /* la reference est elle-meme dans un element associe' de */
		      /* meme type, on cherche l'element associe' en question */
		     {
			pAsc = pElPage;
			stop = FALSE;
			do
			   if (pAsc->ElStructSchema->
			       SsRule[pAsc->ElTypeNumber - 1].SrAssocElem)
			      stop = TRUE;
			   else
			      pAsc = pAsc->ElParent;
			while (!stop && pAsc != NULL);
			if (pAsc == NULL)
			   pElPage = NULL;
			/* echec */
			else
			   /* cherche la marque de page ou s'affiche cet element */
			   /* *associe' qui contient la reference a pEl. */
			   pElPage = GetPageBreakForAssoc (pAsc, viewNb, boxType);
		     }
		   else
		      pElPage = NULL;
		else
		   /* cherche, a partir de la 1ere reference, la premiere */
		   /* marque de page concernant la vue traitee */
		  {
		     pBo1 = &pSchP->PsPresentBox[*boxType - 1];
		     do
		       {
#ifdef __COLPAGE__
			  /* on cherche avant l'element reference que ce soit */
			  /* pour un affichage en haut ou en bas de page */
			  /* test haut ou bas de page supprime */
			  pElPage = BackSearchTypedElem (pElPage, PageBreak + 1, NULL);
			  if (pElPage == NULL)
			     stop = TRUE;
			  else if (pElPage->ElViewPSchema == viewNb
				   && (pElPage->ElPageType == PgBegin
				       || pElPage->ElPageType == PgComputed
				       || pElPage->ElPageType == PgUser))
			     stop = TRUE;
			  else
			     stop = FALSE;
#else  /* __COLPAGE__ */
			  if (pBo1->PbPageHeader)
			     pElPage = BackSearchTypedElem (pElPage, PageBreak + 1, NULL);
			  if (pBo1->PbPageFooter)
			     pElPage = FwdSearchTypedElem (pElPage, PageBreak + 1, NULL);
			  if (pElPage == NULL)
			     stop = TRUE;
			  else if (pElPage->ElViewPSchema == viewNb)
			     stop = TRUE;
			  else
			     stop = FALSE;
#endif /* __COLPAGE__ */
		       }
		     while (!stop);
		  }
	     }
	}
   return pElPage;
}


/*  CreateHeaderFooterForAssocEl N'EST PLUS UTILISEE avec les colonnes       */

#ifndef __COLPAGE__

/*----------------------------------------------------------------------
   CreateHeaderFooterForAssocEl
   verifie si l'element associe' pointe' par pEl doit
   etre affiche' dans une boite de haut ou de bas de page,
   pour la vue viewNb.
   Si oui, cree la boite de haut ou bas de page si elle
   n'existe pas et en cas de creation retourne l'adresse
   de son pave dans pAbbReDisp.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         CreateHeaderFooterForAssocEl (PtrElement pEl, int viewNb, PtrDocument pDoc, PtrAbstractBox * pAbbReDisp)

#else  /* __STDC__ */
static void         CreateHeaderFooterForAssocEl (pEl, viewNb, pDoc, pAbbReDisp)
PtrElement          pEl;
int                 viewNb;
PtrDocument         pDoc;
PtrAbstractBox     *pAbbReDisp;

#endif /* __STDC__ */

{
   PtrPSchema          pSchP;
   int                 index, boxType, TypeP;
   PtrElement          pElPage;
   PtrAbstractBox      pAbbPage, pAbbPageThread;
   PtrPRule            pRule;
   PtrSSchema          pSchS;
   boolean             found, stop;
   int                 viewSch;

   *pAbbReDisp = NULL;
   pAbbPageThread = NULL;
   viewSch = pDoc->DocView[viewNb - 1].DvPSchemaView;
   pElPage = GetPageBreakForAssoc (pEl, viewSch, &boxType);
   if (pElPage != NULL)
     {
	/* L'element doit etre affiche' dans une boite de haut ou de bas de */
	/* page et pElPage pointe sur la marque de page dans l'arbre abstrait */
	SearchPresSchema (pEl, &pSchP, &index, &pSchS);
	if (pElPage->ElAbstractBox[viewNb - 1] == NULL)
	   /* cette marque de page n'a pas de pave, il faut les creer */
	   CheckAbsBox (pElPage, viewNb, pDoc, TRUE, TRUE);
	if (pElPage->ElAbstractBox[viewNb - 1] != NULL)
	   /* cherche dans les boites de la marque de page celle qui */
	   /* doit contenir notre element associe' */
	  {
	     pAbbPage = pElPage->ElAbstractBox[viewNb - 1]->AbFirstEnclosed;
	     stop = FALSE;
	     do
		if (pAbbPage == NULL)
		   stop = TRUE;
		else if (pAbbPage->AbTypeNum == boxType)
		   stop = TRUE;
		else
		  {
		     if (!pAbbPage->AbPresentationBox)
			pAbbPageThread = pAbbPage;
		     pAbbPage = pAbbPage->AbNext;
		  }
	     while (!stop);
	     if (pAbbPage == NULL)
		/* cette boite de haut ou bas de page n'existe pas, on la cree */
		/* cherche le type de boite page */
	       {
		  TypeP = GetPageBoxType (pElPage, viewSch, &pSchP);
		  if (TypeP > 0)
		     /* cherche parmi les regles de la boite page celle qui */
		     /* engendre ce type de boite */
		    {
		       pRule = pSchP->PsPresentBox[TypeP - 1].PbFirstPRule;
		       found = FALSE;
		       if (pRule != NULL)
			  do
			    {
			       if (pRule->PrType > PtFunction)
				  pRule = NULL;
			       else if (pRule->PrType == PtFunction)
				  if (pRule->PrPresFunction == FnCreateBefore
				  || pRule->PrPresFunction == FnCreateAfter)
				     if (pRule->PrPresBox[0] == boxType)
					if (pRule->PrViewNum == viewSch)
					   found = TRUE;
			       if (pRule != NULL && !found)
				  pRule = pRule->PrNextPRule;
			    }
			  while (!found && pRule != NULL);
		       if (pRule != NULL)
			 {
			    pElPage->ElAbstractBox[viewNb - 1] = pAbbPageThread;
			    pAbbPage = CrAbsBoxesPres (pElPage, pDoc, pRule,
					      pElPage->ElStructSchema, NULL,
						viewNb, pSchP, FALSE, TRUE);
			    *pAbbReDisp = pAbbPage;
			    /* il faut reafficher ce pave */
			    pElPage->ElAbstractBox[viewNb - 1] = pAbbPageThread->
			       AbEnclosing;
			 }
		    }
	       }
	     if (pAbbPage != NULL)
		/* la boite de haut ou bas de page existe, elle englobera */
		/* la boite de notre element */
		pEl->ElParent->ElAbstractBox[viewNb - 1] = pAbbPage;
	  }
     }
}
#endif /* __COLPAGE__ */



/*----------------------------------------------------------------------
   DestroyNewAbsBox dechaine et libere la suite des paves comprise      
   entre pAbbFirst et pAbbLast, ainsi que tous les paves       
   englobes. Ces paves n'ont pas encore ete vus par le     
   Mediateur, inutile de lui signaler leur disparition     
   si cette suite correspond a un element sur plusieurs    
   pages, on parcours la chaine des dupliques              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         DestroyNewAbsBox (PtrAbstractBox * pAbbFirst, PtrAbstractBox * pAbbLast)

#else  /* __STDC__ */
static void         DestroyNewAbsBox (pAbbFirst, pAbbLast)
PtrAbstractBox     *pAbbFirst;
PtrAbstractBox     *pAbbLast;

#endif /* __STDC__ */

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
	LibAbbView (pAb);

	/* passe au pave suivant a supprimer */
	if (pAb == *pAbbLast)
	   pAb = NULL;
	/* on a traite' le dernier, on arrete */
	else
	   /* passe au pave' suivant */
#ifdef __COLPAGE__
	if (pAb->AbNext != NULL)
	   pAb = pAb->AbNext;
	else
	   /* cas ou l'element dont on detruit les paves s'etend sur */
	   /* plusieurs pages : pAb->AbNextRepeated != NULL. */
	if (pAb->AbNextRepeated != NULL)	/*normalement toujours vrai */
	   pAb = pAb->AbNextRepeated;
	/* a voir : parcours plus systematique entre pAbbFirst et pAbbLast ? */
	/* a voir : cas ou le pere devient vide ! */
#else  /* __COLPAGE__ */
	   pAb = pAb->AbNext;
#endif /* __COLPAGE__ */
     }
   *pAbbFirst = NULL;
   *pAbbLast = NULL;
}


#ifdef __COLPAGE__

/*----------------------------------------------------------------------
   ContainEl recherche dans le sous-arbre de racine pEl      
   si il y a un element de type typeEl.               
   Si typeEl est une Marque Page, on verifie si       
   l'element trouve appartient a la vue viewSch.    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ContainEl (PtrElement pEl, int typeEl, PtrSSchema pStr, int viewSch, PtrElement * pElCont)

#else  /* __STDC__ */
boolean             ContainEl (pEl, typeEl, pStr, viewSch, pElCont)
PtrElement          pEl;
int                 typeEl;
PtrSSchema          pStr;
int                 viewSch;
PtrElement         *pElCont;

#endif /* __STDC__ */

{
   PtrElement          s;
   boolean             found;

   found = FALSE;
   (*pElCont) = NULL;
   if (EquivalentType (pEl, typeEl, pStr))
      /* trouve ! C'est l'element lui-meme */
      if ((typeEl != PageBreak + 1)
	  || (typeEl == PageBreak + 1
	      && pEl->ElViewPSchema == viewSch))
	{
	   found = TRUE;
	   (*pElCont) = pEl;
	}
   if (!found && !pEl->ElTerminal)
      /* on cherche parmi les fils de l'element */
     {
	s = pEl->ElFirstChild;
	while (!found && s != NULL)
	  {
	     found = ContainEl (s, typeEl, pStr, viewSch, pElCont);
	     s = s->ElNext;
	  }
     }
   return found;
}

#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/*----------------------------------------------------------------------
   Nouvelle procedure  pour les colonnes               
   ContainPageColRule recherche dans le sous-arbre de       
   racine pEl si il y a une regle Page ou Column   
   portee par un element pour la vue viewSch        
   cette procedure s'inspire de ReglePage_HautPage 
   de page.c                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             ContainPageColRule (PtrElement pEl, int viewSch)

#else  /* __STDC__ */
boolean             ContainPageColRule (pEl, viewSch)
PtrElement          pEl;
int                 viewSch;

#endif /* __STDC__ */

{
   PtrPRule            pRule;
   PtrSSchema          pSchS;
   PtrPSchema          pSchP;
   PtrElement          pE;
   int                 index;
   boolean             found, stop;


   SearchPresSchema (pEl, &pSchP, &index, &pSchS);
   if (pSchP == NULL)
      pRule = NULL;
   else
      pRule = pSchP->PsElemPRule[index - 1];
   /* 1ere regle de pres. du type */
   found = FALSE;
   stop = FALSE;
   do
      if (pRule == NULL)
	 stop = TRUE;
      else if (pRule->PrType > PtFunction)
	 stop = TRUE;
      else if (pRule->PrType == PtFunction &&
	       pRule->PrViewNum == viewSch &&
	       (pRule->PrPresFunction == FnPage
		|| pRule->PrPresFunction == FnColumn))
	{			/* c'est une regle Page ou colonne */
	   found = TRUE;
	}
      else
	 pRule = pRule->PrNextPRule;
   while (!stop && !found);
   if (!found && !pEl->ElTerminal)
     {
	pE = pEl->ElFirstChild;
	while (!found && pE != NULL)
	  {
	     found = ContainPageColRule (pE, viewSch);
	     pE = pE->ElNext;
	  }
     }
   return found;
}

#endif /* __COLPAGE__ */

/*----------------------------------------------------------------------
   CreateAllAbsBoxesOfEl     Cree pour toutes les vues existantes tous les 
   paves de l'element pointe par pE appartenant au document pointe 
   par pDoc.                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAllAbsBoxesOfEl (PtrElement pE, PtrDocument pDoc)
#else  /* __STDC__ */
void                CreateAllAbsBoxesOfEl (pE, pDoc)
PtrElement          pE;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 view;

   if (pE == NULL || pE->ElStructSchema == NULL)
     return;
   /* indique qu'il faut creer tous les paves sans limite de volume */
   if (!AssocView (pE))
      /* nombre de vues du document */
      for (view = 1; view <= MAX_VIEW_DOC; view++)
	{
	   if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	      /* la vue est ouverte */
	      pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
	}
   else
      /* vue d'elements associes */
   if (pDoc->DocAssocFrame[pE->ElAssocNum - 1] != 0)
      pDoc->DocAssocFreeVolume[pE->ElAssocNum - 1] = THOT_MAXINT;
   /* cree effectivement les paves dans toutes les vues existantes */
   CreateNewAbsBoxes (pE, pDoc, 0);
   /* applique les regles retardees concernant les paves cree's */
   ApplDelayedRule (pE, pDoc);
}


#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   procedure completement remaniee dans la V4                        
   CreateNewAbsBoxes   cree les paves du sous-arbre dont la racine est    
   pointe par pEl, dans le document dont le contexte du    
   document pDoc.                                          
   Les paves sont crees dans toutes les vues si viewNb est  
   nul, ou seulement dans la vue de numero viewNb           
   si viewNb>0.                                             
   Les paves existants affectes par la creation des        
   nouveaux paves sont modifies. Les pointeurs sur les     
   paves a reafficher du document sont mis a jour.         
   si pEl est une PageBreak, l'image abstraite qui suit   
   est detruite et ensuite recreee                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CreateNewAbsBoxes (PtrElement pEl, PtrDocument pDoc, int viewNb)

#else  /* __STDC__ */
void                CreateNewAbsBoxes (pEl, pDoc, viewNb)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewNb;

#endif /* __STDC__ */

{
   int                 view, firstView, lastView;
   PtrAbstractBox      pAb, pAbbReDisp, pAbbR, pAbbFirst, pAbbLast, pAbbSibling;
   boolean             existingView, stop, assoc;
   boolean             complete;
   PtrAbstractBox      pAbbox1;
   int                 frame, h;
   boolean             paginatedView, bool, found;
   PtrAbstractBox      pAbbRoot, pAbb1, pAbb;
   PtrElement          pEl1, pElCont;

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
	     assoc = AssocView (pEl);
	     if (!assoc)
		/* l'element ne s'affiche pas dans une vue */
		/* d'elements associes */
	       {
		  existingView = pDoc->DocView[view - 1].DvPSchemaView > 0;
		  if (existingView)
		    {
		       pAbbRoot = pDoc->DocViewRootAb[view - 1];
		       paginatedView = (pAbbRoot->AbFirstEnclosed != NULL
				   && pAbbRoot->AbFirstEnclosed->AbElement->
					ElTypeNumber == PageBreak + 1);
		       frame = pDoc->DocViewFrame[view - 1];
		    }
	       }
	     else
		/* c'est une vue d'elements associes */
	       {
		  existingView = pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0 && view == 1;
		  if (existingView)
		    {
		       pAbbRoot = pDoc->DocAssocRoot[pEl->ElAssocNum - 1]
			  ->ElAbstractBox[0];
		       paginatedView = (pAbbRoot->AbFirstEnclosed != NULL
				   && pAbbRoot->AbFirstEnclosed->AbElement->
					ElTypeNumber == PageBreak + 1);
		       frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
		    }
	       }
	     if (existingView)
	       {
		  pAbbReDisp = NULL;
		  /* il n'y a encore rien a reafficher */
		  pAbbFirst = NULL;
		  pAbbLast = NULL;
		  if (!(!assoc && pEl->ElAssocNum != 0
			&& pEl->ElParent != NULL
			&& pEl->ElParent->ElParent == NULL))
		     /* traitement particulier des elements associes */
		     /* ils seront crees au moment de la creation de la reference */
		     /* on ne fait rien ici si c'est un element reference (fils de */
		     /* la racine). Par contre, les fils de cet element sont traites */
		     /* comme des paves normaux */
		    {
		       /* mise a jour du nombre de pages si vue paginee */
		       if (paginatedView)
			  if (assoc)
			    {
			       if (pDoc->DocAssocNPages[pEl->ElAssocNum - 1] != -1)
				  pDoc->DocAssocNPages[pEl->ElAssocNum - 1]
				     = NbPages (pAbbRoot);
			    }
			  else if (pDoc->DocViewNPages[view - 1] != -1)
			     pDoc->DocViewNPages[view - 1] = NbPages (pAbbRoot);
		       /* si pEl est une Marque Page, */
		       /* ou si pEl a un descendant Marque Page, */
		       /* ou si pEl (ou ses descendants) porte une regle page */
		       /* ou colonne */
		       /* on detruit toute l'image */
		       /* abstraite qui suit pour construire correctement la page */
		       if (ContainEl (pEl, PageBreak + 1, pDoc->DocSSchema,
			    pDoc->DocView[view - 1].DvPSchemaView, &pElCont)
			   || ContainPageColRule (pEl, pDoc->DocView[view - 1].DvPSchemaView))
			 {	/* destruction des paves des elements qui suivent pEl */
			    /* on recherche le pave de l'element suivant */
			    pAbb1 = NULL;
			    pEl1 = pEl;
			    found = FALSE;
			    while (!found && pEl1 != NULL)
			       if (pEl1->ElNext != NULL)
				 {
				    pEl1 = pEl1->ElNext;
				    if (pEl1->ElAbstractBox[view - 1] != NULL)
				      {
					 found = TRUE;
					 pAbb1 = pEl1->ElAbstractBox[view - 1];
				      }
				 }
			       else
				 {
				    pEl1 = pEl1->ElParent;
				    if (pEl1 != NULL)
				       if (pEl1->ElAbstractBox[view - 1] != NULL)
					 {
					    /* toujours vrai ? */
					    pAbb = pEl1->ElAbstractBox[view - 1];
					    while (pAbb->AbPresentationBox)
					       pAbb = pAbb->AbNext;
					    /* on marque le pave coupe pour etre sur */
					    /* que le pave du nouvel element soit cree */
					    pAbb->AbTruncatedTail = TRUE;
					 }
				 }
			    /* je ne comprends plus ce code !!! */
			    /* il est incompatible avec ContainPageColRule */
			    /* je le supprime donc. TODO : a verifier */
			    /* cas particulier ou l'element a partir duquel detruire */
			    /* contient un element marque page qui a un pave : il */
			    /* faut detruire a partir de cette marque de page */
			    /*      if (pEl1 != NULL) */
			    /*      if (ContainEl(pEl1, PageBreak+1, pDoc->DocSSchema, */
			    /*                  pDoc->DocView[view - 1].DvPSchemaView, &pElCont)) */
			    /*          if (pElCont->ElAbstractBox[view - 1] != NULL) */
			    /*            pAbb1 = pElCont->ElAbstractBox[view - 1]; */
			    if (pAbb1 != NULL && pAbb1 != pAbbRoot)
			      {
				 DestrAbbNext (pAbb1, pDoc);
				 h = 0;
				 bool = ChangeConcreteImage (frame, &h, pAbbRoot);
				 FreeDeadAbstractBoxes (pAbbRoot);
			      }
			    if (assoc)
			      {
				 if (pDoc->DocAssocNPages[pEl->ElAssocNum - 1] != -1)
				    pDoc->DocAssocNPages[pEl->ElAssocNum - 1]
				       = NbPages (pAbbRoot);
			      }
			    else if (pDoc->DocViewNPages[view - 1] != -1)
			       pDoc->DocViewNPages[view - 1] = NbPages (pAbbRoot);

			    /* TODO a discuter avec Vincent */
			    /* on repositionne la variable globale FirstCreation */
			    /* a faux car on est sur que pEl est soit une */
			    /* marque page, soit un element qui, comme il */
			    /* contient une marque page, ne peut pas */
			    /* avoir une position ou dimension donnee par */
			    /* l'utilisateur (role de FirstCreation) */
			    /* si on laisse ce booleen a vrai, tous les elements */
			    /* dont on cree les paves dans l'appel ci-dessous */
			    /* sont consideres comme nouvellement crees, donc */
			    /* les graphiques pealablement existants aussi */
			    FirstCreation = FALSE;
			    /* on cree les paves de l'element ainsi que des */
			    /* elements contenus dans la page */
			    pAb = AbsBoxesCreate (pAbbRoot->AbElement, pDoc, view, TRUE,
						  TRUE, &complete);
			 }	/* fin cas ContainEl != NULL */
		       /* ou regle page ou colonne pour pEl */
		       else
			  pAb = AbsBoxesCreate (pEl, pDoc, view, TRUE, TRUE, &complete);
		       if (pEl->ElAbstractBox[view - 1] != NULL
			   && pEl->ElTypeNumber != PageBreak + 1)
			  /* l'element a au moins un pave dans la vue */
			  /* tout ce code est inutile pour les Marques PaginateView ??? */
			 {
			    /* cherche le premier pave cree pour le nouvel element */
			    pAbbFirst = pEl->ElAbstractBox[view - 1];
			    /* cas modif 622 non traitee : pb. comment detecter */
			    /* que des paves des descendants ont ete crees alors */
			    /* que pEl a une visibilite nulle : la valeur pAb */
			    /* retournee par AbsBoxesCreate n'est pas significative ! */
			    /* cherche le dernier pave cree pour le nouvel element */
			    /* il peut etre sur une autre page si l'element */
			    /* a une marque page comme fils */
			    pAbbLast = pEl->ElAbstractBox[view - 1];
			    if (pAbbLast != NULL)
			      {
				 stop = FALSE;
				 do
				    if (pAbbLast->AbPresentationBox
					&& pAbbLast->AbNext != NULL
				      && pAbbLast->AbNext->AbElement == pEl)
				       pAbbLast = pAbbLast->AbNext;
				    else if (pAbbLast->AbNextRepeated != NULL)
				       pAbbLast = pAbbLast->AbNextRepeated;
				    else
				       stop = TRUE;
				 while (!stop);
			      }
			    /* verifie que les paves precedents et suivants sont complets */
			    /* et les detruit s'ils sont incomplets */
			    if (pAbbFirst->AbPrevious != NULL)
			      {
				 pAbbox1 = pAbbFirst->AbPrevious;
				 /* on saute les eventuels paves de presentation */
				 while (pAbbox1 != NULL && pAbbox1->AbPresentationBox)
				    pAbbox1 = pAbbox1->AbPrevious;
				 if (pAbbox1 != NULL && pAbbox1->AbLeafType == LtCompound
				 /*                       && !pAbbox1->AbInLine  */
				     && pAbbox1->AbTruncatedTail)
				    /* le pave precedent est incomplet a la fin */
				    if (assoc || pDoc->DocView[view - 1].DvSync)
				       /* La vue est synchronisee, on supprime tous les paves */
				       /* precedents */
				       /* code change car AbTruncatedHead ne suffit pas */
				       /* pour savoir ou s'arreter de detruire */
				       /* cas des elements sur plusieurs pages */
				       /* ainsi que des paves de presentation repetes */
				       /* on remonte la hierarchie jusqu'a la racine */
				      {
					 pAb = pAbbFirst;
					 while (pAb != NULL && pAb != pAbbRoot
					  && pAb->AbElement->ElTypeNumber !=
						PageBreak + 1)
					   {
					      pAbbSibling = pAb->AbPrevious;
					      while (pAbbSibling != NULL)
						{
						   SetDeadAbsBox (pAbbSibling);
						   ApplyRefAbsBoxSupp (pAbbSibling, &pAbbR, pDoc);
						   pAbbSibling = pAbbSibling->AbPrevious;
						}
					      pAb = pAb->AbEnclosing;
					      pAb->AbTruncatedHead = TRUE;
					   }
					 /* si la vue n'est pas paginee, on marque la racine */
					 /* coupee en tete */
					 if (pAb->AbEnclosing == NULL)
					    pAb->AbTruncatedHead = TRUE;
					 /* si la vue est paginee, on detruit les pages */
					 /* precedentes */
					 if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
					   {
					      /* on saute les paves de */
					      /* presentation de la page courante: haut */
					      pAbbSibling = pAb;
					      while (pAbbSibling->AbPrevious != NULL
						     && pAbbSibling->AbPrevious->AbElement == pAb->AbElement)
						 pAbbSibling = pAbbSibling->AbPrevious;
					      pAbbSibling = pAbbSibling->AbPrevious;
					      /* dernier pave de la page precedente */
					      /* si il existe une page precedente */
					      /* on la detruit (et les precedentes) */
					      while (pAbbSibling != NULL)
						{
						   SetDeadAbsBox (pAbbSibling);
						   ApplyRefAbsBoxSupp (pAbbSibling, &pAbbR, pDoc);
						   pAbbSibling = pAbbSibling->AbPrevious;
						}
					      /* on marque la racine coupee en queue */
					      pAb->AbEnclosing->AbTruncatedTail = TRUE;
					   }
				      }		/* fin de la destruction des paves precedents */
				    else
				       /* ce n'est pas une vue synchronisee, on detruit les */
				       /* paves que l'on vient de creer */
				       DestroyNewAbsBox (&pAbbFirst, &pAbbLast);
			      }	/* fin pAbbFirst->AbPrevious != NULL */
			    if (pAbbLast != NULL)
			      {
				 pAbbox1 = pAbbLast->AbNext;
				 /* on saute les eventuels paves de presentation du suivant */
				 while (pAbbox1 != NULL && pAbbox1->AbPresentationBox)
				    pAbbox1 = pAbbox1->AbNext;
				 if ((pAbbox1 != NULL
				      && pAbbox1->AbLeafType == LtCompound
				      && pAbbox1->AbTruncatedHead
				      && (assoc || pDoc->DocView[view - 1].DvSync))
				     || (pAbbLast->AbLeafType == LtCompound
					 && pAbbLast->AbTruncatedTail))
				   {
				      /* La vue est synchronisee, on supprime 
				         tous les paves suivants */
				      /* ou si on n'a pas pu creer tous les paves du contenu de */
				      /* l'element (ils depasseraient la capacite de la fenetre) */
				      /* il faut supprimer tous les paves suivant l'element */
				      /* code change car AbTruncatedTail ne suffit pas */
				      /* pour savoir ou s'arreter de detruire */
				      /* on remonte la hierarchie jusqu'a la racine */
				      pAb = pAbbLast;
				      while (pAb != NULL && pAb != pAbbRoot
					  && pAb->AbElement->ElTypeNumber !=
					     PageBreak + 1)
					{
					   /* supprime tous ses freres suivants */
					   /* TODO cas des paves de presentation repetes !! */
					   pAbbSibling = pAb->AbNext;
					   while (pAbbSibling != NULL)
					     {
						SetDeadAbsBox (pAbbSibling);
						/* change les regles des autres paves qui
						   se referent au pave detruit */
						ApplyRefAbsBoxSupp (pAbbSibling, &pAbbR, pDoc);
						pAbbSibling = pAbbSibling->AbNext;
					     }
					   pAb = pAb->AbEnclosing;
					   pAb->AbTruncatedTail = TRUE;
					}
				      /* si la vue n'est pas paginee, on marque la racine */
				      /* coupee en queue */
				      if (pAb->AbEnclosing == NULL)
					 pAb->AbTruncatedTail = TRUE;
				      /* si la vue est paginee, on detruit les pages */
				      /* suivantes */
				      if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
					{
					   /* on saute les paves de */
					   /* presentation de la page courante: bas et filet */
					   pAbbSibling = pAb;
					   while (pAbbSibling->AbNext != NULL
						  && pAbbSibling->AbNext->AbElement == pAb->AbElement)
					      pAbbSibling = pAbbSibling->AbNext;
					   pAbbSibling = pAbbSibling->AbNext;	/* premier pave de la page suivante */
					   /* si il existe une page suivante */
					   /* on la detruit (et les suivantes) */
					   while (pAbbSibling != NULL)
					     {
						SetDeadAbsBox (pAbbSibling);
						ApplyRefAbsBoxSupp (pAbbSibling, &pAbbR, pDoc);
						pAbbSibling = pAbbSibling->AbNext;
					     }
					   /* on marque la racine coupee en queue */
					   pAb->AbEnclosing->AbTruncatedTail = TRUE;
					}
				   }	/* fin de la destruction des paves suivants */
				 if (pAbbox1 != NULL && pAbbox1->AbLeafType == LtCompound
				     && pAbbox1->AbTruncatedHead
				     && !(assoc || pDoc->DocView[view - 1].DvSync))
				    /* ce n'est pas une vue synchronisee, on detruit les */
				    /* paves que l'on vient de creer */
				    DestroyNewAbsBox (&pAbbFirst, &pAbbLast);
			      }	/* fin pAbbLast != NULL */
			 }	/* fin pEl != marquepage */
		       /* modifie les paves qui peuvent se referer aux nouveaux paves */
		       /* fait dans AbsBoxesCreate car les paves pAbbFirst et pAbbLast ne sont */
		       /* pas toujours freres */
		       /* ApplyRefAbsBoxNew(pAbbFirst, pAbbLast, &pAbbR, pDoc); */
		       /* conserve le pointeur sur le pave a reafficher */
		       /* c'est la racine */
		       if (AssocView (pEl))
			  pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] = pAbbRoot;
		       else
			  pDoc->DocViewModifiedAb[view - 1] = pAbbRoot;
		       ApplDelayedRule (pEl, pDoc);
		    }		/* fin pEl != assoc en haut ou bas de page */
	       }		/* fin existingView */
	  }			/* boucle for sur les vues */
     }				/* fin pEl != NULL */
}

#else  /* __COLPAGE__ */

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

#ifdef __STDC__
void                CreateNewAbsBoxes (PtrElement pEl, PtrDocument pDoc, int viewNb)

#else  /* __STDC__ */
void                CreateNewAbsBoxes (pEl, pDoc, viewNb)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewNb;

#endif /* __STDC__ */

{
   int                 view, firstView, lastView;
   PtrAbstractBox      pAb, pAbbReDisp, pAbbR, pAbbFirst, pAbbLast, pAbbSibling;
   boolean             existingView, stop, assocView;
   boolean             complete;
   PtrAbstractBox      pAbbox1;

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
	     assocView = AssocView (pEl);
	     if (!assocView)
		/* l'element ne s'affiche pas dans une vue */
		/* d'elements associes */
		existingView = pDoc->DocView[view - 1].DvPSchemaView > 0;
	     else
		/* c'est une vue d'elements associes */
		existingView = pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0 && view == 1;
	     if (existingView)
	       {
		  pAbbReDisp = NULL;
		  /* il n'y a encore rien a reafficher */
		  pAbbFirst = NULL;
		  pAbbLast = NULL;
		  if (!assocView)
		     /* traitement particulier des elements associes */
		    {
		       CreateHeaderFooterForAssocEl (pEl, view, pDoc, &pAbbReDisp);
		       pAbbFirst = pAbbReDisp;
		       pAbbLast = pAbbReDisp;
		    }
		  /* cree et chaine les paves correspondant a l'element, si ca n'a */
		  /* pas deja ete fait par CreateHeaderFooterForAssocEl */
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
			       /* le pave precedent est incomplet a la fin */
			       if (assocView || pDoc->DocView[view - 1].DvSync)
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
					    if (pAb->AbTruncatedHead)
					       /* le pave englobant est deja coupe' en tete. */
					       pAb = NULL;
					    else
					       /* le pave englobant est coupe' en tete, il faut */
					       /* encore detruire les paves qui le precedent. */
					       pAb->AbTruncatedHead = TRUE;
				      }
				    while (pAb != NULL);
				 }
			       else
				  /* ce n'est pas une vue synchronisee, on detruit les */
				  /* paves que l'on vient de creer */
				  DestroyNewAbsBox (&pAbbFirst, &pAbbLast);
			 }
		       if (pAbbLast != NULL)
			  if (pAbbLast->AbNext != NULL)
			    {
			       pAbbox1 = pAbbLast->AbNext;
			       if (pAbbox1->AbLeafType == LtCompound
				   && !pAbbox1->AbInLine
				   && pAbbox1->AbTruncatedHead)
				  /* le pave suivant est incomplet au debut */
				  if (assocView || pDoc->DocView[view - 1].DvSync)
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
					       if (pAb->AbTruncatedTail)
						  pAb = NULL;
					       else
						  /* le pave englobant est coupe' en queue */
						  pAb->AbTruncatedTail =
						     TRUE;
					 }
				       while (pAb != NULL);
				    }
				  else
				     /* ce n'est pas une vue synchronisee, on detruit les */
				     /* paves que l'on vient de creer */
				     DestroyNewAbsBox (&pAbbFirst, &pAbbLast);
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
		       if (AssocView (pEl))
			  pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
			     Enclosing (pAbbReDisp,
			     pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
		       else
			  pDoc->DocViewModifiedAb[view - 1] =
			     Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[view - 1]);
		    }
	       }
	  }
     }
}
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   PosBoolAbsBoxCh met a vrai les booleens de position verticale      
   et horizontale                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PosBoolAbsBoxCh (PtrAbstractBox pAb)

#else  /* __STDC__ */
static void         PosBoolAbsBoxCh (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   if (pAb != NULL)
     {
	pAb->AbVertPosChange = TRUE;
	pAb->AbVertRefChange = TRUE;
	pAb->AbHorizPosChange = TRUE;
	pAb->AbHorizRefChange = TRUE;
	pAb = pAb->AbFirstEnclosed;
	while (pAb != NULL)
	  {
	     PosBoolAbsBoxCh (pAb);
	     pAb = pAb->AbNext;
	  }
     }
}

#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   procedure completement remaniee dans la V4                        
   DestroyAbsBoxesView detruit pour la vue de numero view les paves du        
   sous-arbre dont la racine est pointee par pEl, dans le  
   document pDoc.                                          
   Destruction des paves suite a la destruction de pEl     
   Les paves existants affectes par la destruction sont    
   modifies. Les pointeurs de paves modifies du document   
   sont mis a jour                                         
   Si Verif est vrai, on verifie si les paves englobants   
   deviennent complets. En effet la suppression d'un pave  
   incomplet a une extremite' peut rendre le pave englobant
   complete a cette extremite.                              
   Attention : on peut detruire des paves de plusieurs     
   sous-arbres car pEl peut avoir des paves dupliques      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DestroyAbsBoxesView (PtrElement pEl, PtrDocument pDoc, boolean verify, int view)

#else  /* __STDC__ */
void                DestroyAbsBoxesView (pEl, pDoc, verify, view)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             verify;
int                 view;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb, pAbbReDisp, pAbbR, pAbb, pElAscent, PcFirst,
                       PcLast;
   PtrElement          pElChild;
   boolean             stop;
   PtrElement          pEl1;
   PtrAbstractBox      pPavFollow;

   pAbbReDisp = NULL;
   pAb = pEl->ElAbstractBox[view - 1];
   if (pAb == NULL)
      /* pas de pave pour cet element dans cette vue, alors que */
      /* la vue existe */
     {
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
	pEl1 = pEl;
	PcFirst = NULL;
	PcLast = NULL;
	pPavFollow = NULL;
	pAbbReDisp = pAb;	/* on reaffichera ce pave */
	/* traite tous les paves correspondant a cet element */
	/* les marque d'abord tous 'morts' en verifiant si leur */
	/* suppression complete le pave englobant */
	do
	  {
	     if (verify)
		if (pAb->AbLeafType == LtCompound && !pAb->AbPresentationBox)
		   /* les booleens AbTruncatedHead et AbTruncatedTail ne sont pas */
		   /* significatifs pour les paves de presentation */
		   /* && !pAb->AbInLine) */
		  {
		     if (pAb->AbTruncatedHead && pAb->AbPreviousRepeated == NULL)
			/* si le pave a un dup precedent, il ne faut pas rendre les */
			/* peres complets car ils sont eux aussi dupliques */
			/* remarque: si un pave est coupe et non duplique, */
			/* alors ses peres ne peuvent pas etre dupliques */
			if (pEl->ElPrevious == NULL)
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
		     if (pAb->AbTruncatedTail && pAb->AbNextRepeated == NULL)
			/* si le pave a un dup suivant, il ne faut pas rendre les */
			/* peres complets car ils sont eux aussi dupliques */
			if (pEl->ElNext == NULL)
			   /* c'est le pave du dernier element */
			  {
			     pElAscent = pAb->AbEnclosing;
			     while (pElAscent != NULL)
			       {
				  pAbb = TruncateOrCompleteAbsBox (pElAscent, FALSE, FALSE, pDoc);
				  if (pAbb != NULL)
				     pAbbReDisp = Enclosing (pAbbReDisp, pAbb);
				  pElAscent = pElAscent->AbEnclosing;
				  if (pElAscent != NULL)
				     if (pElAscent->AbElement->ElNext != NULL)
					pElAscent = NULL;
			       }
			  }
		  }		/* fin cas verif */
	     SetDeadAbsBox (pAb);
	     if (PcFirst == NULL)
		PcFirst = pAb;
	     PcLast = pAb;
	     /* passe au pave suivant du meme element */
	     /* cas particulier de la destruction d'un element marque de page */
	     /* destruction des paves de l'element */
	     /* et du reste de l'image abstraite : pour refaire une */
	     /* duplication correcte */
	     if (pEl->ElTypeNumber == PageBreak + 1)
		pAb = pAb->AbNext;
	     else
	       {
		  /* on memorise les paves qui ne sont pas de presentation */
		  /* pour le passage au pave duplique suivant */
		  if (!pAb->AbPresentationBox)
		     pPavFollow = pAb;
		  if (pAb->AbNext != NULL)
		     if (pAb->AbNext->AbElement == pEl)
			/* il s'agit d'un autre pave de l'element */
			pAb = pAb->AbNext;
		     else
			pAb = NULL;	/* pave de l'element suivant */
		  else if (pPavFollow != NULL)
		     pAb = pPavFollow->AbNextRepeated;
		  /* on prend le pave duplique suivant */
		  else
		     pAb = NULL;	/* cas jamais possible ?? */
	       }
	  }
	while (pAb != NULL);
	/* si on detruit une marque de page, on a detruit toutes les pages */
	/* suivantes : il faut mettre la racine coupee en queue */
	if (pEl->ElTypeNumber == PageBreak + 1)
	  {
	     pAbb = pEl->ElAbstractBox[view - 1]->AbEnclosing;
	     while (pAbb != NULL)
	       {
		  pAbb->AbTruncatedTail = TRUE;
		  pAbb = pAbb->AbEnclosing;	/* en prevision des colonnes */
	       }
	  }
	if (PcFirst != PcLast)
	   /* il y a plusieurs paves pour cet element, on reaffichera */
	   /* le pave englobant */
	   pAbbReDisp = Enclosing (pAbbReDisp, PcFirst->AbEnclosing);
	if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrAssocElem)
	   if (PcFirst->AbPSchema->PsInPageHeaderOrFooter[pEl->ElTypeNumber - 1])
	      /* c'est un element qui s'affiche en haut ou bas de page */
	      /* cherche si le pave englobant est vide (ne contient */
	      /* que des paves morts ou de presentation) */
	     {
		pAbb = PcFirst->AbEnclosing->AbFirstEnclosed;
		stop = FALSE;
		do
		   if (pAbb == NULL)
		      stop = TRUE;
		   else if (!pAbb->AbDead && !pAbb->AbPresentationBox)
		      stop = TRUE;
		   else
		      pAbb = pAbb->AbNext;
		while (!stop);
		if (pAbb == NULL)
		   /* il n'y a que des paves morts, on tue le pave */
		   /* englobant */
		  {
		     PcFirst = PcFirst->AbEnclosing;
		     PcLast = PcFirst;
		     if (pEl->ElParent->ElAbstractBox[view - 1] == PcFirst)
			pEl->ElParent->ElAbstractBox[view - 1] = NULL;
		     SetDeadAbsBox (PcFirst);
		     pAbbReDisp = Enclosing (pAbbReDisp, PcFirst);
		  }
	     }
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
	     else if (pEl->ElTypeNumber == PageBreak + 1)
		pAb = pAb->AbNext;
	     else
	       {
		  /* on memorise les paves qui ne sont pas de presentation */
		  /* pour le passage au pave duplique suivant */
		  if (!pAb->AbPresentationBox)
		     pPavFollow = pAb;
		  if (pAb->AbNext != NULL)
		     if (pAb->AbNext->AbElement == pEl)
			/* il s'agit d'un autre pave de l'element */
			pAb = pAb->AbNext;
		     else
			pAb = NULL;	/* pave de l'element suivant */
		  else if (pPavFollow != NULL)
		     pAb = pPavFollow->AbNextRepeated;
		  /* on prend le pave duplique suivant */
		  else
		     pAb = NULL;	/* cas jamais possible ?? */
	       }
	  }
	while (pAb != NULL);
	/* conserve le pointeur sur le pave a reafficher */
	if (AssocView (pEl))
	   pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
	      Enclosing (pAbbReDisp, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	else
	   pDoc->DocViewModifiedAb[view - 1] = Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[view - 1]);
     }				/* fin du else : l'element a un pave dans la vue */
}


#else  /* __COLPAGE__ */
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

#ifdef __STDC__
void                DestroyAbsBoxesView (PtrElement pEl, PtrDocument pDoc, boolean verify, int view)

#else  /* __STDC__ */
void                DestroyAbsBoxesView (pEl, pDoc, verify, view)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             verify;
int                 view;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb, pAbbReDisp, pAbbR, pAbb, pElAscent, PcFirst,
                       PcLast;
   PtrElement          pElChild;
   boolean             stop;
   PtrElement          pEl1;
   PtrAbstractBox      pAbbox1;

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
	if (pAb->AbEnclosing != NULL)
	   if (pAb->AbEnclosing->AbPresentationBox)
	      if (pAb->AbEnclosing->AbElement == pAb->AbElement)
		 pAb = pAb->AbEnclosing;
	pEl1 = pEl;
	if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrAssocElem)
	   /* traitement particulier des elements associes : le pere */
	   /* de l'element a pour pave le pave englobant de l'element. */
	   /* Dans le cas ou ces elements associes sont affiches dans */
	   /* une boite de haut ou de bas de page, le pave de l'element */
	   /* englobant pourrait etre dans une autre page... */
	   pEl->ElParent->ElAbstractBox[view - 1] = pAb->AbEnclosing;
	PcFirst = NULL;
	PcLast = NULL;
	pAbbReDisp = pAb;	/* on reaffichera ce pave */
	/* traite tous les paves correspondant a cet element */
	/* les marque d'abord tous 'morts' en verifiant si leur */
	/* suppression complete le pave englobant */
	do
	  {
	     if (verify)
		if (pAb->AbLeafType == LtCompound && !pAb->AbInLine)
		  {
		     if (pAb->AbTruncatedHead)
			if (pEl->ElPrevious == NULL)
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
		     if (pAb->AbTruncatedTail)
			if (pEl->ElNext == NULL)
			   /* c'est le pave du dernier element */
			  {
			     pElAscent = pAb->AbEnclosing;
			     while (pElAscent != NULL)
			       {
				  pAbb = TruncateOrCompleteAbsBox (pElAscent, FALSE, FALSE, pDoc);
				  if (pAbb != NULL)
				     pAbbReDisp = Enclosing (pAbbReDisp, pAbb);
				  pElAscent = pElAscent->AbEnclosing;
				  if (pElAscent != NULL)
				     if (pElAscent->AbElement->ElNext != NULL)
					pElAscent = NULL;
			       }
			  }
		  }
	     SetDeadAbsBox (pAb);
	     if (PcFirst == NULL)
		PcFirst = pAb;
	     PcLast = pAb;
	     /* passe au pave suivant du meme element */
	     pAb = pAb->AbNext;
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
	if (PcFirst != PcLast)
	   /* il y a plusieurs paves pour cet element, on reaffichera */
	   /* le pave englobant */
	   pAbbReDisp = Enclosing (pAbbReDisp, PcFirst->AbEnclosing);
	/* Est-ce un pave d'un element associe qui s'affiche en haut */
	/* ou en bas de page ? */
	if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrAssocElem)
	   if (PcFirst->AbEnclosing->AbElement->ElTypeNumber == PageBreak + 1)
	      /* le pave englobant est une boite de page */
	      /* cherche si le pave englobant est vide (ne contient */
	      /* que des paves morts ou de presentation) */
	     {
		pAbb = PcFirst->AbEnclosing->AbFirstEnclosed;
		stop = FALSE;
		do
		   if (pAbb == NULL)
		      stop = TRUE;
		   else if (!pAbb->AbDead && !pAbb->AbPresentationBox)
		      stop = TRUE;
		   else
		      pAbb = pAbb->AbNext;
		while (!stop);
		if (pAbb == NULL)
		   /* il n'y a que des paves morts, on tue le pave */
		   /* englobant */
		  {
		     PcFirst = PcFirst->AbEnclosing;
		     PcLast = PcFirst;
		     if (pEl->ElParent->ElAbstractBox[view - 1] == PcFirst)
			pEl->ElParent->ElAbstractBox[view - 1] = NULL;
		     SetDeadAbsBox (PcFirst);
		     pAbbReDisp = Enclosing (pAbbReDisp, PcFirst);
		     /* traite ensuite les paves qui se referent aux paves morts */
		  }
	     }
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
	if (AssocView (pEl))
	   pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
	      Enclosing (pAbbReDisp, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	else
	   pDoc->DocViewModifiedAb[view - 1] = Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[view - 1]);
     }
}
#endif /* __COLPAGE__ */


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

#ifdef __STDC__
void                DestroyAbsBoxes (PtrElement pEl, PtrDocument pDoc, boolean verify)

#else  /* __STDC__ */
void                DestroyAbsBoxes (pEl, pDoc, verify)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             verify;

#endif /* __STDC__ */

{
   int                 view;
   boolean             existingView;

   if (pEl != NULL)
      /* traite les paves de toutes les vues */
      for (view = 1; view <= MAX_VIEW_DOC; view++)
	 /* teste si la vue  existe */
	{
	   if (!AssocView (pEl))
	      /* ce n'est pas un element associe */
	      existingView = pDoc->DocView[view - 1].DvPSchemaView > 0;
	   else
	      /* c'est un element associe */
	   if (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] == 0)
	      existingView = FALSE;
	   else
	      existingView = view == 1;
	   if (existingView)
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

#ifdef __STDC__
void                RedispRef (PtrReference pRef, PtrAbstractBox pAb, PtrDocument pDocRef)

#else  /* __STDC__ */
void                RedispRef (pRef, pAb, pDocRef)
PtrReference        pRef;
PtrAbstractBox      pAb;
PtrDocument         pDocRef;

#endif /* __STDC__ */

{
   PtrElement          pElRef;
   PtrAbstractBox      pAbb, pPavRef;
   int                 v, frame, j, h;
   PtrPRule            pRule;
   boolean             found, redisp, stop, same;
   PtrTextBuffer       bufCopy, BufOriginal;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;
   PtrPRule            pRe1;
   PtrAbstractBox      pAbbox1;
   boolean             complete;

#ifdef __COLPAGE__
   PtrElement          pE;
   int                 nb;

#endif /* __COLPAGE__ */

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
		   if (!AssocView (pElRef))
		     {
			if (pDocRef->DocView[v - 1].DvPSchemaView > 0)
#ifdef __COLPAGE__
			  {
			     /* le document est-il pagine dans cette vue ? */
			     /* si oui, on compte le nombre de pages actuel */
			     pAbb = pDocRef->DocViewRootAb[v - 1];
			     if (pAbb->AbFirstEnclosed != NULL
				 && pAbb->AbFirstEnclosed->AbElement->ElTypeNumber ==
				 PageBreak + 1)
			       {
				  nb = NbPages (pAbb);
				  pDocRef->DocViewNPages[v - 1] = nb;
				  pDocRef->DocViewFreeVolume[v - 1] = THOT_MAXINT;
			       }
			     else
				pDocRef->DocViewFreeVolume[v - 1] = pDocRef->DocViewVolume[v - 1];
			  }
#else  /* __COLPAGE__ */
			   pDocRef->DocViewFreeVolume[v - 1] = pDocRef->DocViewVolume[v - 1];
#endif /* __COLPAGE__ */
		     }
		   else
		      /* element associe */
		     {
			if (pDocRef->DocAssocFrame[pElRef->ElAssocNum - 1] > 0)
#ifdef __COLPAGE__
			  {
			     pE = pDocRef->DocAssocRoot[pElRef->ElAssocNum - 1];
			     pAbb = pE->ElAbstractBox[v - 1];
			     if (pAbb != NULL && pAbb->AbFirstEnclosed != NULL
				 && pAbb->AbFirstEnclosed->AbElement->ElTypeNumber ==
				 PageBreak + 1)
			       {
				  nb = NbPages (pAbb);
				  pDocRef->DocAssocNPages[pElRef->ElAssocNum - 1] = nb;
				  pDocRef->DocAssocFreeVolume[pElRef->ElAssocNum - 1] = THOT_MAXINT;
			       }
			     else
				pDocRef->DocAssocFreeVolume[pElRef->ElAssocNum - 1] =
				   pDocRef->DocAssocVolume[pElRef->ElAssocNum - 1];
			  }
#else  /* __COLPAGE__ */
			   pDocRef->DocAssocFreeVolume[pElRef->ElAssocNum - 1] =
			      pDocRef->DocAssocVolume[pElRef->ElAssocNum - 1];
#endif /* __COLPAGE__ */
		     }
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
			      if (pAb == NULL)
				 found = TRUE;
			      else
				{
				   pAbbox1 = pAb;
				   if (pRe1->PrNPresBoxes == 0)
				      found = (strcmp (pRe1->PrPresBoxName,
						       pAbbox1->AbPSchema->PsPresentBox[pAbbox1->AbTypeNum - 1].PbName) == 0);
				   else
				      found = pRe1->PrPresBox[0] == pAbbox1->AbTypeNum;
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
#ifdef __COLPAGE__
			   if (ApplyRule (pRule, pSPR, pPavRef, pDocRef, pAttr, &bool))
#else  /* __COLPAGE__ */
			   if (ApplyRule (pRule, pSPR, pPavRef, pDocRef, pAttr))
#endif /* __COLPAGE__ */
			      redisp = TRUE;
		     }
		}
	      if (redisp)
		 /* on ne reaffiche pas les paves qui n'ont pas encore */
		 /* ete affiches */
		 if (!pPavRef->AbNew)
		   {
		      pPavRef->AbChange = TRUE;
		      if (AssocView (pElRef))
			 frame = pDocRef->DocAssocFrame[pElRef->ElAssocNum - 1];
		      else
			 frame = pDocRef->DocViewFrame[v - 1];
#ifdef __COLPAGE__
		      h = BreakPageHeight;
#else  /* __COLPAGE__ */
		      h = PageHeight;
#endif /* __COLPAGE__ */
		      (void) ChangeConcreteImage (frame, &h, pPavRef);
		      /* on ne reaffiche pas si on est en train de calculer */
		      /* les pages */
#ifdef __COLPAGE__
		      if (BreakPageHeight == 0)
#else  /* __COLPAGE__ */
		      if (PageHeight == 0)
#endif /* __COLPAGE__ */
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

#ifdef __STDC__
void                RedispAllReferences (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                RedispAllReferences (pAb, pDoc)
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

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

#ifdef __STDC__
static PtrAbstractBox SearchAbsBoxBackward (PtrAbstractBox pAbb1, boolean Test, PtrSSchema pSchStr, PtrPSchema pSchP, int Typ, boolean Pres)

#else  /* __STDC__ */
static PtrAbstractBox SearchAbsBoxBackward (pAbb1, Test, pSchStr, pSchP, Typ, Pres)
PtrAbstractBox      pAbb1;
boolean             Test;
PtrSSchema          pSchStr;
PtrPSchema          pSchP;
int                 Typ;
boolean             Pres;

#endif /* __STDC__ */

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
#ifdef __STDC__
PtrAbstractBox      AbsBoxFromElOrPres (PtrAbstractBox pAb, boolean pres, int typeElOrPres, PtrPSchema pSchP, PtrSSchema pSchStr)
#else  /* __STDC__ */
PtrAbstractBox      AbsBoxFromElOrPres (pAb, pres, typeElOrPres, pSchP, pSchStr)
PtrAbstractBox      pAb;
boolean             pres;
int                 typeElOrPres;
PtrPSchema          pSchP;
PtrSSchema          pSchStr;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAbbResult, pAbbForward, pAbbAscent;
   boolean             stop;
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
#ifdef __STDC__
static void         FindFirstAbsBox (PtrElement pElBegin, int nv)
#else  /* __STDC__ */
static void         FindFirstAbsBox (pElBegin, nv)
PtrElement          pElBegin;
int                 nv;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   boolean             stop;

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
#ifdef __STDC__
void                ComputePageNum (PtrElement pEl, int view)
#else  /* __STDC__ */
void                ComputePageNum (pEl, view)
PtrElement          pEl;
int                 view;

#endif /* __STDC__ */
{
   PtrElement          pPage;
   PtrPSchema          pSchP;
   boolean             stop;
   int                 numpageprec, cpt;
   PtrElement          pEl1;

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
#ifdef __COLPAGE__
	else if (pPage->ElViewPSchema == view
		 && (pPage->ElPageType == PgBegin
		     || pPage->ElPageType == PgComputed
		     || pPage->ElPageType == PgUser))
	   /* on ne considere pas les colonnes TODO a changer ?? */
#else  /* __COLPAGE__ */
	else if (pPage->ElViewPSchema == view)
#endif /* __COLPAGE__ */
	   /* cette page concerne la vue, on la traite */
	  {
	     pEl1 = pPage;
	     numpageprec = pEl1->ElPageNumber;
	     /* cherche le compteur de page a appliquer a cette page */
	     cpt = GetPageCounter (pPage, view, &pSchP);
	     if (cpt == 0)
		/* page non numerotee, on s'arrete */
		stop = TRUE;
	     else
		/* calcule le nouveau numero de page */
	       {
		  pEl1->ElPageNumber = CounterVal (cpt, pPage->ElStructSchema, pSchP, pPage, view);
		  if (pEl1->ElPageNumber == numpageprec)
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
#ifdef __STDC__
static void         ComputeContent (int boxType, int nv, PtrDocument pDoc, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElBegin, boolean redisp)
#else  /* __STDC__ */
static void         ComputeContent (boxType, nv, pDoc, pSS, pSchP, pElBegin, redisp)
int                 boxType;
int                 nv;
PtrDocument         pDoc;
PtrSSchema          pSS;
PtrPSchema          pSchP;
PtrElement          pElBegin;
boolean             redisp;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 frame, h;
   PtrAbstractBox      pAbbox1;

   FindFirstAbsBox (pElBegin, nv);
   pAb = pAbbBegin[nv - 1];
   while (pAb != NULL)
     {
	pAbbox1 = pAb;
	if (pAbbox1->AbPresentationBox
	    && pAbbox1->AbTypeNum == boxType
	    && pAbbox1->AbPSchema == pSchP)
	   /* fait reafficher le pave de presentation si le contenu a */
	   /* change' */
	   if (NewVariable (pSchP->PsPresentBox[boxType - 1].PbContVariable, pSS, pSchP, pAb, pDoc))
	      /* et si le pave a deja ete traite' par le mediateur */
	      if (!pAb->AbNew)
		{
		   pAbbox1->AbChange = TRUE;
		   if (AssocView (pElBegin))
		      frame = pDoc->DocAssocFrame[pElBegin->ElAssocNum - 1];
		   else
		      frame = pDoc->DocViewFrame[nv - 1];
#ifdef __COLPAGE__
		   h = BreakPageHeight;
#else  /* __COLPAGE__ */
		   h = PageHeight;
#endif /* __COLPAGE__ */
		   ChangeConcreteImage (frame, &h, pAb);
		   /* on ne reaffiche pas si on est en train de calculer les */
		   /* pages */
#ifdef __COLPAGE__
		   if (BreakPageHeight == 0 && redisp)
#else  /* __COLPAGE__ */
		   if (PageHeight == 0 && redisp)
#endif /* __COLPAGE__ */
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
#ifdef __STDC__
static void         ComputeCrPresBoxes (int boxType, int nv, PtrPSchema pSchP, PtrDocument pDoc, PtrElement pElBegin, boolean redisp)
#else  /* __STDC__ */
static void         ComputeCrPresBoxes (boxType, nv, pSchP, pDoc, pElBegin, redisp)
int                 boxType;
int                 nv;
PtrPSchema          pSchP;
PtrDocument         pDoc;
PtrElement          pElBegin;
boolean             redisp;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb, pAbbFollow;
   int                 frame, h;
   boolean             found, stop;
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
	     pRCre = GlobalSearchRulepEl (pAbbox1->AbElement, &pSPR, &pSSR, presNum, NULL,
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
#ifndef __COLPAGE__
			     || pRe1->PrPresFunction == FnCreateWith
#endif /* __COLPAGE__ */
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
		if (!CondPresentation (pRCre->PrCond, pAb->AbElement, NULL, viewSch,
				       pAb->AbElement->ElStructSchema))
		   /* On va detruire le pave, on cherche d'abord le pave de */
		   /* presentation suivant de meme type */
		  {
		     pAbbFollow = AbsBoxFromElOrPres (pAb, TRUE, boxType, pSchP, NULL);
		     /* tue le pave */
		     SetDeadAbsBox (pAb);
		     /* signale le pave mort au mediateur */
		     if (AssocView (pElBegin))
			frame = pDoc->DocAssocFrame[pElBegin->ElAssocNum - 1];
		     else
			frame = pDoc->DocViewFrame[nv - 1];
#ifdef __COLPAGE__
		     h = BreakPageHeight;
#else  /* __COLPAGE__ */
		     h = PageHeight;
#endif /* __COLPAGE__ */
		     ChangeConcreteImage (frame, &h, pAb);
		     /* on ne reaffiche pas si on est en train de calculer les */
		     /* pages */
#ifdef __COLPAGE__
		     if (BreakPageHeight == 0 && redisp)
#else  /* __COLPAGE__ */
		     if (PageHeight == 0 && redisp)
#endif /* __COLPAGE__ */
			DisplayFrame (frame);
		     /* libere le pave tue' */
		     FreeDeadAbstractBoxes (pAb);
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
#ifdef __STDC__
static void         ComputeCreation (int boxType, boolean presBox, int counter, int nv, PtrSSchema pSS, PtrPSchema pSchP, PtrDocument pDoc, PtrElement pElBegin, boolean redisp)
#else  /* __STDC__ */
static void         ComputeCreation (boxType, presBox, counter, nv, pSS, pSchP, pDoc, pElBegin, redisp)
int                 boxType;
boolean             presBox;
int                 counter;
int                 nv;
PtrSSchema          pSS;
PtrPSchema          pSchP;
PtrDocument         pDoc;
PtrElement          pElBegin;
boolean             redisp;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb, pAbb, pAbbNext;
   PtrElement          pEl;
   int                 frame, h;
   boolean             stop, isCreated, depend, boxok, page;
   PtrPRule            pRCre;
   PtrPSchema          pSPR, pSchPOrig;
   PtrSSchema          pSSR;
   PtrAttribute        pAttr;
   int                 viewSch;
   PtrAbstractBox      pAbbox1;
   PtrPRule            pRe1;
   PtrCondition        pCond;
   int                 presNum;

#ifdef __COLPAGE__
   PtrAbstractBox      pPrevious, pRac;
   PtrElement          pElRac;

#endif /* __COLPAGE__ */

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
#ifdef __COLPAGE__
	if (!pAb->AbDead)
	  {
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
		       && (pAbbox1->AbElement->ElPageType == PgBegin
			   || pAbbox1->AbElement->ElPageType == PgComputed
			   || pAbbox1->AbElement->ElPageType == PgUser)
#endif /* __COLPAGE__ */
		       && pAbbox1->AbLeafType == LtCompound)
		      /* c'est un element Marque de page */
		      /* mais pas un pave de colonne */
		     {
			/*attention GetPageBoxType est susceptible de modifier pSchP 
			   il faut donc prendre des precautions */
			pSchPOrig = pSchP;
			if (boxType == GetPageBoxType (pAbbox1->AbElement, viewSch, &pSchPOrig)
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
		  pRCre = GlobalSearchRulepEl (pAb->AbElement, &pSPR, &pSSR, presNum, NULL,
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
#ifndef __COLPAGE__
				  || pRe1->PrPresFunction == FnCreateWith
#endif /* __COLPAGE__ */
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
							NULL, viewSch, pAb->AbElement->ElStructSchema))
				     /* cherche si le pave est deja cree' */
				    {
				       isCreated = FALSE;
#ifdef __COLPAGE__
				       /* cas page supprime, ce n'est plus un cas particulier */
				       if (pRe1->PrPresFunction == FnCreateFirst
					   || pRe1->PrPresFunction == FnCreateLast)
					 {
					    /* cas particulier des paves de presentation de */
					    /* la racine : ils sont sous la premiere et la */
					    /* derniere page si le document est pagine */
					    if (AssocView (pAb->AbElement))
					      {
						 pElRac = pDoc->
						    DocAssocRoot[pAb->AbElement->ElAssocNum - 1];
						 pRac = pElRac->ElAbstractBox[0];
					      }
					    else
					      {
						 pElRac = pDoc->DocRootElement;
						 pRac = pElRac->ElAbstractBox[nv - 1];
					      }
					    if (pAb == pRac
						&& (!pAb->AbTruncatedHead || !pAb->AbTruncatedTail))
					      {
						 /* le document est-il pagine ? */
						 /* cad les paves fils sont des paves de page */
						 pAbb = pAb->AbFirstEnclosed;
						 if (pAbb != NULL
						     && pAbb->AbElement->ElTerminal
						     && pAbb->AbElement->ElLeafType == LtPageColBreak)
						   {
						      pEl = pAbb->AbElement;
						      /* cherche le premier corps de page: on saute */
						      /* le (les ?) premier haut de page eventuel */
						      while (pAbb != NULL && pAbb->AbElement == pEl
							     && pAbb->AbPresentationBox)
							 pAbb = pAbb->AbNext;
						      /* pAbb est le premier corps de page */
						      /* les premiers fils de la racine sont ses */
						      /* premiers fils */
						      if (!pAb->AbTruncatedHead)
							{
							   if (pAbb != NULL)
							      pAbb = pAbb->AbFirstEnclosed;
							   while (!isCreated && pAbb != NULL
								  && pAbb->AbElement == pElRac)
							      if (pAbb->AbPresentationBox
								  && pAbb->AbTypeNum == pRe1->PrPresBox[0]
								  && pAbb->AbPSchema == pSPR)
								 isCreated = TRUE;
							      else
								 pAbb = pAbb->AbNext;
							}
						      /* cas de la derniere page (derniers fils) */
						      if (!pAb->AbTruncatedTail && !isCreated)
							{
							   /* on cherche le dernier pave corps */
							   /* de page */
							   pPrevious = NULL;
							   pAbb = pAb->AbFirstEnclosed;
							   while (pAbb != NULL)
							     {
								if (!pAbb->AbPresentationBox)
								   pPrevious = pAbb;
								pAbb = pAbb->AbNext;
							     }
							   /* pPrevious est le dernier pave corps de page */
							   if (pPrevious != NULL)
							     {
								pAbb = pPrevious->AbFirstEnclosed;
								if (pAbb != NULL)
								   while (pAbb->AbNext != NULL)
								      pAbb = pAbb->AbNext;
								/*pAbb est le dernier pave fils du corps de page */
								while (!isCreated && pAbb != NULL
								       && pAbb->AbElement == pElRac)
								   if (pAbb->AbPresentationBox
								       && pAbb->AbTypeNum == pRe1->PrPresBox[0]
								       && pAbb->AbPSchema == pSPR)
								      isCreated = TRUE;
								   else
								      pAbb = pAbb->AbPrevious;
							     }
							}
						   }
					      }
					    /* fin du cas particulier de la racine */
					    else
					      {
						 pAbb = pAb->AbFirstEnclosed;
						 while (!isCreated && pAbb != NULL)
						    if (pAbb->AbPresentationBox
							&& pAbb->AbTypeNum == pRe1->PrPresBox[0]
							&& pAbb->AbPSchema == pSPR)
						       isCreated = TRUE;
						    else if (pAbb->AbNext != NULL)
						       pAbb = pAbb->AbNext;
						    else
						       pAbb = pAbb->AbNextRepeated;
					      }
					 }
#else  /* __COLPAGE__ */
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
#endif /* __COLPAGE__ */
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
#ifndef __COLPAGE__
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
#endif /* __COLPAGE__ */
					    pAbb = CrAbsBoxesPres (pEl, pDoc, pRCre, pSS, pAttr,
						    nv, pSchP, FALSE, TRUE);
					    if (pAbbNext != NULL)
					       pEl->ElAbstractBox[nv - 1] = pAbbNext;
					    if (pAbb != NULL)
					      {
						 if (AssocView (pElBegin))
						    frame =
						       pDoc->DocAssocFrame[pElBegin->ElAssocNum - 1];
						 else
						    frame = pDoc->DocViewFrame[nv - 1];
#ifdef __COLPAGE__
						 h = BreakPageHeight;
#else  /* __COLPAGE__ */
						 h = PageHeight;
#endif /* __COLPAGE__ */
						 ChangeConcreteImage (frame, &h, pAbb);
						 /* on ne reaffiche pas si on est en
						    train de calculer les pages */
#ifdef __COLPAGE__
						 if (BreakPageHeight == 0 && redisp)
#else  /* __COLPAGE__ */
						 if (PageHeight == 0 && redisp)
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
	  }			/* fin si pAb pas mort */
#endif /* __COLPAGE__ */
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

#ifdef __STDC__
static void         AttachCounterValue (PtrElement pEl, PtrElement pElIncluded, PtrDocument pDocIncluded, Name NmAttr, int counter, PtrPSchema pSchP, PtrSSchema pSchS)

#else  /* __STDC__ */
static void         AttachCounterValue (pEl, pElIncluded, pDocIncluded, NmAttr, counter, pSchP, pSchS)
PtrElement          pEl;
PtrElement          pElIncluded;
PtrDocument         pDocIncluded;
Name                NmAttr;
int                 counter;
PtrPSchema          pSchP;
PtrSSchema          pSchS;

#endif /* __STDC__ */

{
   int                 att;
   PtrAttribute        pAttr;
   TtAttribute        *pTtAttr;
   boolean             found;

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
		if (strncmp (pTtAttr->AttrOrigName, NmAttr, sizeof (Name)) == 0)
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

#ifdef __STDC__
void                TransmitCounterVal (PtrElement pEl, PtrDocument pDoc, Name nameAttr, int counter, PtrPSchema pSchP, PtrSSchema pSchS)

#else  /* __STDC__ */
void                TransmitCounterVal (pEl, pDoc, nameAttr, counter, pSchP, pSchS)
PtrElement          pEl;
PtrDocument         pDoc;
Name                nameAttr;
int                 counter;
PtrPSchema          pSchP;
PtrSSchema          pSchS;

#endif /* __STDC__ */

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

#ifdef __STDC__
static void         ChangeBoxesCounter (PtrElement pElBegin, PtrDocument pDoc, int counter, PtrPSchema pSchP, PtrSSchema pSS, boolean redisp)

#else  /* __STDC__ */
static void         ChangeBoxesCounter (pElBegin, pDoc, counter, pSchP, pSS, redisp)
PtrElement          pElBegin;
PtrDocument         pDoc;
int                 counter;
PtrPSchema          pSchP;
PtrSSchema          pSS;
boolean             redisp;

#endif /* __STDC__ */

{
   int                 util, view;
   Counter            *pCo1;
   PtrElement          pEl;
   int                 regle;
   Counter            *pCounter;
   PtrElement          pElRoot;

   /* ce code etait precedemment plus loin */
   /* On traite toutes les boites qui utilisent ce compteur comme contenu */
   pCo1 = &pSchP->PsCounter[counter - 1];
   for (util = 1; util <= pCo1->CnNPresBoxes; util++)
      if (AssocView (pElBegin))
	 ComputeContent (pCo1->CnPresBox[util - 1], 1, pDoc, pSS, pSchP, pElBegin,
			 redisp);
      else
	 for (view = 1; view <= MAX_VIEW_DOC; view++)
	    if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	      {
		 /* Il faut determiner si on doit reevaluer du debut de l'image
		    abstraite ou de pElBegin; on ne reevalue que si CnMinMaxPresBox est
		    TRUE, i.e. une boite est creee par une condition de min ou de max
		    du compteur */
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
			   ComputeContent (pCo1->CnPresBox[util - 1], view, pDoc, pSS, pSchP,
					   pElRoot, redisp);
			}
		      else
			 ComputeContent (pCo1->CnPresBox[util - 1], view, pDoc, pSS, pSchP,
					 pElBegin, redisp);
		   }
	      }
   /* On traite toutes les boites dont la creation est conditionnee par */
   /* la valeur de ce compteur */
   for (util = 1; util <= pCo1->CnNCreatedBoxes; util++)
      if (AssocView (pElBegin))
	 ComputeCrPresBoxes (pCo1->CnCreatedBox[util - 1], 1, pSchP, pDoc, pElBegin,
			     redisp);
      else
	 for (view = 1; view <= MAX_VIEW_DOC; view++)
	    if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	      {
		 /* Il faut determiner si on doit reevaluer depuis le debut de l'image
		    abstraite ou depuis pElBegin; on ne reevalue que si CnMinMaxCreatedBox
		    est TRUE, i.e. si une boite est creee par une condition de min ou
		    de max du compteur */
		 if (!pCo1->CnMinMaxCreatedBox[util - 1])
		    ComputeCrPresBoxes (pCo1->CnCreatedBox[util - 1], view, pSchP, pDoc,
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
			   ComputeCrPresBoxes (pCo1->CnCreatedBox[util - 1], view, pSchP, pDoc,
					       pElRoot, redisp);
			}
		      else
			 ComputeCrPresBoxes (pCo1->CnCreatedBox[util - 1], view, pSchP, pDoc,
					     pElBegin, redisp);
		   }
	      }
   /* On traite toutes les boites qui creent d'autres boites selon la */
   /* valeur de ce compteur */
   for (util = 1; util <= pCo1->CnNCreators; util++)
      if (AssocView (pElBegin))
	 ComputeCreation (pCo1->CnCreator[util - 1],
		   pCo1->CnPresBoxCreator[util - 1], counter, 1, pSS, pSchP,
			  pDoc, pElBegin, redisp);
      else
	 for (view = 1; view <= MAX_VIEW_DOC; view++)
	    if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	      {
		 /* Il faut determiner si on doit reevaluer du debut de l'image
		    abstraite ou de pElBegin; on ne reevalue que si CnMinMaxCreator est
		    TRUE, i.e. si une boite est creee par une condition de min ou de max
		    du compteur */
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
			    pCo1->CnPresBoxCreator[util - 1], counter, view,
					 pSS, pSchP, pDoc, pElRoot, redisp);
			}
		      else
			 ComputeCreation (pCo1->CnCreator[util - 1],
					  pCo1->CnPresBoxCreator[util - 1], counter, view, pSS, pSchP,
					  pDoc, pElBegin, redisp);
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

#ifdef __STDC__
static void         UpdateNum1Elem (PtrElement pElBegin, PtrElement pElModif, PtrDocument pDoc, boolean redisp)

#else  /* __STDC__ */
static void         UpdateNum1Elem (pElBegin, pElModif, pDoc, redisp)
PtrElement          pElBegin;
PtrElement          pElModif;
PtrDocument         pDoc;
boolean             redisp;

#endif /* __STDC__ */

{
   int                 counter, oper, i;
   PtrPSchema          pSchP;
   PtrSSchema          pSS;
   int                 index;
   boolean             trigger;
   Counter            *pCo1;
   CntrItem           *pCp1;
   SRule              *pRe1;

   /* si l'element pElModif est une marque de page, renumerote les */
   /* les sauts de page qui suivent, a partir de pElBegin. */
   if (pElModif->ElTerminal && pElModif->ElLeafType == LtPageColBreak)
      ComputePageNum (pElBegin, pElModif->ElViewPSchema);
   /* cherche le schema de presentation de l'element : pSchP */
   SearchPresSchema (pElModif, &pSchP, &index, &pSS);
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

#ifdef __STDC__
static void         UpdateNum (PtrElement pElD, PtrElement pElM, PtrDocument pDoc, boolean redisp)

#else  /* __STDC__ */
static void         UpdateNum (pElD, pElM, pDoc, redisp)
PtrElement          pElD;
PtrElement          pElM;
PtrDocument         pDoc;
boolean             redisp;

#endif /* __STDC__ */

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

#ifdef __STDC__
void                UpdateNumbers (PtrElement pElBegin, PtrElement pElModif, PtrDocument pDoc, boolean redisp)

#else  /* __STDC__ */
void                UpdateNumbers (pElBegin, pElModif, pDoc, redisp)
PtrElement          pElBegin;
PtrElement          pElModif;
PtrDocument         pDoc;
boolean             redisp;

#endif /* __STDC__ */

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

#ifdef __STDC__
void                UpdateBoxesCounter (PtrElement pElBegin, PtrDocument pDoc, int counter, PtrPSchema pSchP, PtrSSchema pSS)

#else  /* __STDC__ */
void                UpdateBoxesCounter (pElBegin, pDoc, counter, pSchP, pSS)
PtrElement          pElBegin;
PtrDocument         pDoc;
int                 counter;
PtrPSchema          pSchP;
PtrSSchema          pSS;

#endif /* __STDC__ */

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

#ifdef __STDC__
static void         SetChange (PtrAbstractBox pAbb, PRuleType typeRule)

#else  /* __STDC__ */
static void         SetChange (pAbb, typeRule)
PtrAbstractBox      pAbb;
PRuleType           typeRule;

#endif /* __STDC__ */

{

   switch (typeRule)
	 {
	    case PtWidth:
	       pAbb->AbWidthChange = TRUE;
	       break;
	    case PtHeight:
	       pAbb->AbHeightChange = TRUE;
	       break;
	    case PtHorizPos:
	       pAbb->AbHorizPosChange = TRUE;
	       break;
	    case PtVertPos:
	       pAbb->AbVertPosChange = TRUE;
	       break;
	    case PtHorizRef:
	       pAbb->AbHorizRefChange = TRUE;
	       break;
	    case PtVertRef:
	       pAbb->AbVertRefChange = TRUE;
	       break;
	    case PtSize:
	       pAbb->AbSizeChange = TRUE;
	       break;
	    case PtDepth:
	    case PtLineStyle:
	    case PtLineWeight:
	    case PtFillPattern:
	    case PtBackground:
	    case PtForeground:
	       pAbb->AbAspectChange = TRUE;
	       break;
	    default:
	       pAbb->AbChange = TRUE;
	       break;
	 }

}


/*----------------------------------------------------------------------
   ApplyInheritPresRule si la regle de presentation de type typeRule        
   qui doit s'appliquer au pave pAb est une regle         
   d'heritage, on applique cette regle au pave' pointe'    
   par pAb, et on fait de meme sur son sous-arbre.        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplyInheritPresRule (PtrAbstractBox pAb, PRuleType typeRule, PtrDocument pDoc)

#else  /* __STDC__ */
static void         ApplyInheritPresRule (pAb, typeRule, pDoc)
PtrAbstractBox      pAb;
PRuleType           typeRule;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule            pRPres;
   PtrAbstractBox      pAbbChild;
   PtrPSchema          pSPR;
   PtrAttribute        pA;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   /* on ne traite que si le type de la regle permet l'heritage */
   if (typeRule == PtVisibility
       || typeRule == PtDepth
       || typeRule == PtSize
       || typeRule == PtStyle
       || typeRule == PtFont
       || typeRule == PtUnderline
       || typeRule == PtThickness
       || typeRule == PtIndent
       || typeRule == PtLineSpacing
       || typeRule == PtAdjust
       || typeRule == PtJustify
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
#ifdef __COLPAGE__
	   if (ApplyRule (pRPres, pSPR, pAb, pDoc, pA, &bool))
#else  /* __COLPAGE__ */
	   if (ApplyRule (pRPres, pSPR, pAb, pDoc, pA))
#endif /* __COLPAGE__ */
	      /* le pave est modifie' */
	     {
		if (typeRule == PtSize)
		   pAb->AbSizeChange = TRUE;
		else if (typeRule == PtDepth || typeRule == PtLineStyle ||
			 typeRule == PtLineWeight ||
			 typeRule == PtFillPattern ||
		       typeRule == PtBackground || typeRule == PtForeground)
		   pAb->AbAspectChange = TRUE;
		else
		   pAb->AbChange = TRUE;
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

#ifdef __STDC__
static boolean      ApplyPresRuleAb (PtrPRule pRule, PtrPSchema pSchP, PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
static boolean      ApplyPresRuleAb (pRule, pSchP, pAb, pDoc, pAttr)
PtrPRule            pRule;
PtrPSchema          pSchP;
PtrAbstractBox      pAb;
PtrDocument         pDoc;
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   PtrPRule            pRegle2;
   PtrPSchema          pSchP2;
   PtrAttribute        pAttr2;
   boolean             ret;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   /* applique la regle a` appliquer */
#ifdef __COLPAGE__
   ret = ApplyRule (pRule, pSchP, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
   ret = ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
   /* verifie si c'est une regle de dimension ou de position */
   /* qui s'applique a un pave' elastique dont la boite a ete */
   /* inversee. Dans ce cas reapplique la regle de position ou */
   /* de dimension */
   if (ret && pRule != NULL)
      if (pRule->PrType == PtVertPos || pRule->PrType == PtHeight)
	 /* c'est une regle de dimension ou de position verticale */
	{
	   if (pAb->AbHeight.DimIsPosition)
	      /* le pave est elastique verticalement */
	      if (pAb->AbBox != NULL)
		 /* le pave possede une boite dans l'image concrete */
		 if (pAb->AbBox->BxVertInverted)
		    /* la boite a ete inverse'e verticalement, on */
		    /* reapplique la regle complementaire */
		    if (pRule->PrType == PtVertPos)
		      {
			 pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtHeight, FnAny,
						  TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbHeightChange = TRUE;
		      }
		    else
		      {
			 pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtVertPos, FnAny,
						  TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbVertPosChange = TRUE;
		      }
	}
      else if (pRule->PrType == PtHorizPos || pRule->PrType == PtWidth)

	 /* c'est une regle de dimension ou de position horiz. */
	{
	   if (pAb->AbWidth.DimIsPosition)
	      /* le pave est elastique horizontalement */
	      if (pAb->AbBox != NULL)
		 /* le pave possede une boite dans l'image concrete */
		 if (pAb->AbBox->BxHorizInverted)
		    /* la boite a ete inverse'e horizontalement, on */
		    /* reapplique la regle complementaire */
		    if (pRule->PrType == PtHorizPos)
		      {
			 pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtWidth, FnAny,
						  TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbWidthChange = TRUE;
		      }
		    else
		      {
			 pRegle2 = SearchRulepAb (pDoc, pAb, &pSchP2, PtHorizPos, FnAny,
						  TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (ApplyRule (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbHorizPosChange = TRUE;
		      }
	}
   return ret;
}



/*----------------------------------------------------------------------
   ElemWithinImage  checks if abstract boxes of the element pEl 
   Corresponding to view may be displayed within the abstract image
   part already builded.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ElemWithinImage (PtrElement pEl, int view, PtrAbstractBox pAbbRoot, PtrDocument pDoc)
#else  /* __STDC__ */
boolean             ElemWithinImage (pEl, view, pAbbRoot, pDoc)
PtrElement          pEl;
int                 view;
PtrAbstractBox      pAbbRoot;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   boolean             result, finished, found;
   PtrElement          pAsc;
   PtrAbstractBox      pAb;

   result = TRUE;
   finished = FALSE;
   /* Has the element close to this one an abstract box in the view */
   if (pEl->ElPrevious != NULL)
     {
	pAb = pEl->ElPrevious->ElAbstractBox[view - 1];
	if (pAb != NULL)
	   /* The previous element has an abstract box in the view */
	  {
	     finished = TRUE;
	     /* If the abstract box of the previous element is complete in queue,
	        the element will have its abstract box in the existing image */
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
   UpdatePresAttr
  Pour l'element pEl dans le document pDoc, supprime 
   ou applique (selon remove) la presentation attachee a    
   l'attribut pointe par pAttr.                            
   Ce changement de la presentation a lieu egalement sur   
   le sous-arbre de l'element, si les parametres de        
   presentation associes a l'attribut sont herites.        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UpdatePresAttr (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, boolean remove, boolean inherit, PtrAttribute pAttrComp)

#else  /* __STDC__ */
void                UpdatePresAttr (pEl, pAttr, pDoc, remove, inherit, pAttrComp)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;
boolean             remove;
boolean             inherit;
PtrAttribute        pAttrComp;

#endif /* __STDC__ */

{
   PtrPRule            pR, pRuleView1, pRuleNext, pRNA, pRSpecif, pRS,
                       pRP;
   PRuleType           typeRule;
   FunctionType        TFonct;
   PtrAbstractBox      pAbb, pReaff, pPR, pAbbNext, pAbbChild;
   PtrPSchema          pSchP, pSPR;
   PtrAttribute        pAttrib;
   PtrHandlePSchema    pHd;
   TypeUnit            unit;
   int                 view, viewSch, val;
   boolean             appl, stop, sameType, found;
   boolean             existingView;
   boolean             createBox, complete;
#ifdef __COLPAGE__
   boolean             TRep;
   PtrAbstractBox      pPE;
#endif /* __COLPAGE__ */

   viewSch = 0;
   typeRule = (PRuleType) 0;
   TFonct = (FunctionType) 0;
   /* on cherche d'abord dans les schemas de presentation additionnels les */
   /* plus prioritaires */
   pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
   if (pHd == NULL)
      /* pas de schema additionnel. Prend le schema de presentation principal*/
      pSchP = pAttr->AeAttrSSchema->SsPSchema;
   else
     {
	while (pHd->HdNextPSchema != NULL)
	   pHd = pHd->HdNextPSchema;
	pSchP = pHd->HdPSchema;
     }
   while (pSchP != NULL)
     {
	/* pR: premiere regle correspondant a l'attribut */
	pR = AttrPresRule (pAttr, pEl, inherit, pAttrComp, pSchP);
	if (pR != NULL)
	  {
	     typeRule = pR->PrType;
	     /* type des regles courantes */
	     if (typeRule == PtFunction)
#ifdef __COLPAGE__
	       {
		  TRep = pR->PrPresBoxRepeat;
		  TFonct = pR->PrPresFunction;
	       }
#else  /* __COLPAGE__ */
		TFonct = pR->PrPresFunction;
#endif /* __COLPAGE__ */
	  }
	pRuleView1 = NULL;
	/* regle de ce type pour la vue 1 */
	/* traite toutes les regles associees a cette valeur d'attribut dans ce */
	/* schema de presentation */
	while (pR != NULL)
	   /* chaque regle est traitee dans toutes les vues ou l'element */
	   /* a un pave */
	  {
	     if (pR->PrViewNum == 1)
		/* regle pour la vue 1 */
		pRuleView1 = pR;	/* on la garde pour le cas ou on ne trouve pas mieux */
	     pRuleNext = pR->PrNextPRule;	/* regle suivante */
	     for (view = 1; view <= MAX_VIEW_DOC; view++)
	       {
		  /* verifie d'abord si la vue existe */
		  if (AssocView (pEl))
		     /* element associe */
		     existingView = pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0 && view == 1;
		  else
		     existingView = pDoc->DocView[view - 1].DvPSchemaView > 0;
		  if (existingView)
		    {
		       viewSch = AppliedView (pEl, pAttr, pDoc, view);
		       /* on ne considere les schemas de presentation additionnels que */
		       /* pour la vue principale */
		       existingView = (pHd == NULL || viewSch == 1);
		    }
		  if (existingView)
		     if (pR->PrCond != NULL)
			if (!CondPresentation (pR->PrCond, pEl, pAttr, viewSch,
					       pAttr->AeAttrSSchema))
			   /* les conditions d'application de la regle ne sont pas */
			   /* satisfaites, on n'applique pas la regle */
			   existingView = FALSE;
		  if (existingView)
		    {
		       /* si c'est une regle de creation on l'applique a toutes les */
		       /* vues */
		       if (pR->PrType != PtFunction
			   || !(pR->PrPresFunction == FnCreateBefore || pR->PrPresFunction == FnCreateFirst
#ifndef __COLPAGE__
				|| pR->PrPresFunction == FnCreateWith
#endif /* __COLPAGE__ */
				|| pR->PrPresFunction == FnCreateLast || pR->PrPresFunction == FnCreateAfter))
			  /* ce n'est pas une regle de creation ; on cherche la regle */
			  /* de meme type qui concerne cette vue */
			  if (pRuleNext != NULL)
			    {
			       sameType = FALSE;
			       if (pRuleNext->PrType == typeRule)
				  if (typeRule == PtFunction)
				    {
				       if (pR->PrPresFunction == pRuleNext->PrPresFunction)
					  sameType = TRUE;
				    }
				  else
				     sameType = TRUE;
			       if (sameType && pRuleNext->PrViewNum == viewSch)
				 {
				    pR = pRuleNext;
				    pRuleNext = pR->PrNextPRule;
				 }
			    }
		       pReaff = NULL;
		       /* il n'y a rien (encore) a reafficher */
		       pAbb = NULL;
		       /* on n'a pas cree de pave */
		       createBox = ElemWithinImage (pEl, view, pDoc->DocViewRootAb[view - 1], pDoc);
		       if (createBox && pEl->ElAbstractBox[view - 1] == NULL && typeRule == PtVisibility)
			  /* le pave n'existe pas et c'est une regle de visibilite */
			 {
			    val = IntegerRule (pR, pEl, view, &appl, &unit, pAttr);
			    if ((!remove && val > 0) || (remove && val <= 0))
			       /* cette regle rend le pave visible et ce n'est pas une suppression
			          ou c'est une suppression et le pave etait rendu invisible par la regle
			          on cree le pave et ses paves de presentation eventuels */
			      {
				if (AssocView (pEl))
				  pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = THOT_MAXINT;
				else
				  pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
				pAbb = AbsBoxesCreate (pEl, pDoc, view, TRUE, TRUE, &complete);
				if (pAbb != NULL)
				  /* au moins un pave a ete cree. pAbb est le
				     premier. Cherche le dernier cree */
				  {
				  pAbbNext = pAbb;
				  stop = FALSE;
				  do
				    if (pAbbNext->AbNext == NULL)
					stop = TRUE;
				    else if (pAbbNext->AbNext->AbElement != pEl)
					stop = TRUE;
				    else
					pAbbNext = pAbbNext->AbNext;
				  while (!stop);
				  }
			      }
			 }
		       if (createBox
			   && pEl->ElAbstractBox[view - 1] != NULL && typeRule == PtFunction
			   && (TFonct == FnCreateBefore || TFonct == FnCreateAfter
#ifndef __COLPAGE__
			       || TFonct == FnCreateWith
#endif /* __COLPAGE__ */
		               || TFonct == FnCreateFirst || TFonct == FnCreateLast)
			   && !remove)
			  /* il faut creer un pave de presentation */
			 {
			   if (AssocView (pEl))
			     pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = THOT_MAXINT;
			   else
			     pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
			   pAbb = CrAbsBoxesPres (pEl, pDoc, pR, pAttr->AeAttrSSchema, pAttr, view,
						  pSchP, FALSE, TRUE);
			   pAbbNext = pAbb;
			 }
		       /* traite les paves crees par la regle de visibilite ou de */
		       /* creation */
		       /* code inutile et incorrect (fait dans AbsBoxesCreate) si saut de page */
		       /* il reste correct si on a cree seulement des paves de presentation */
		       if (pAbb != NULL)
			    /* les nouveaux paves doivent etre pris en compte par */
			    /* leurs voisins */
			    ApplyRefAbsBoxNew (pAbb, pAbbNext, &pReaff, pDoc);

		       /* traite les paves qui existaient deja */
		       /* il faut faire une boucle de parcours des paves dupliques de l'element */
		       if (pEl->ElAbstractBox[view - 1] != NULL && pAbb == NULL)
			 {
			    pAbb = pEl->ElAbstractBox[view - 1];
			    /* saute les paves de presentation de l'element */
			    while (pAbb->AbNext != NULL && pAbb->AbPresentationBox && pAbb->AbElement == pEl)
			       pAbb = pAbb->AbNext;
#ifdef __COLPAGE__
			    /* debut de la boucle */
			    do
			      {
#endif /* __COLPAGE__ */
				 appl = FALSE;
				 /* on n'a pas applique' la regle */
				 /* applique la regle au pave de l'element s'il n'est pas mort */
				 if (!pAbb->AbDead)
				    if (pR->PrType == typeRule && pR->PrViewNum == viewSch)
				       /* regle specifique de la vue traitee */
				       if (remove)
					 {
					    pRNA = SearchRulepAb (pDoc, pAbb, &pSPR, typeRule, TFonct, TRUE, &pAttrib);
					    appl = ApplyPresRuleAb (pRNA, pSPR, pAbb, pDoc, pAttrib);
					 }
				       else
					  appl = ApplyPresRuleAb (pR, pSchP, pAbb, pDoc, pAttr);
				    else
				       /* applique la regle de la vue 1 si elle existe */
				    if (pRuleView1 != NULL)
				       if (remove)
					 {
					    pRNA = SearchRulepAb (pDoc, pAbb, &pSPR, typeRule, TFonct, TRUE, &pAttrib);
					    appl = ApplyPresRuleAb (pRNA, pSPR, pAbb, pDoc, pAttrib);
					 }
				       else
					  appl = ApplyPresRuleAb (pRuleView1, pSchP, pAbb, pDoc, pAttr);
				    else
				       appl = FALSE;
				 if (!appl)
				    if (remove)
				       if (pR->PrType == PtFunction && pR->PrPresFunction == FnNotInLine)
					  /* on desapplique une regle NotInLine */
                                          {
                                          pAbb->AbNotInLine = FALSE;
                                          appl = TRUE;
                                          }
				 if (appl)
				    /* on a change' la presentation du pave */
				   {
				      if (!remove)
					 /* ce n'est pas une suppression d'attribut */
					 /* cherche si l'element possede pour cette vue une */
					 /* regle de presentation specifique de meme type que */
					 /* celle qu'on vient d'appliquer */
					{
					   pRSpecif = pEl->ElFirstPRule;
					   pRP = NULL;
					   stop = FALSE;
					   do
					      {
					      if (pRSpecif == NULL)
						 stop = TRUE;
					      else if (pRSpecif->PrViewNum == viewSch &&
					       pRSpecif->PrType == typeRule)
						 if (typeRule == PtFunction)
						   {
						   if (pR->PrPresFunction == pRSpecif->PrPresFunction)
						      stop = TRUE; /* trouve'*/
						   }
						 else
						   stop = TRUE;	/* trouve' */
					      if (!stop)
						{
						   pRP = pRSpecif;
						   pRSpecif = pRSpecif->PrNextPRule;
						}
					      }
					   while (!stop);
					   if (pRSpecif != NULL)
					      /* l'element possede cette regle specifique, on */
					      /* la supprime : l'attribut a priorite' sur la */
					      /* presentation specifique */
					     {
						pRS = pRSpecif->PrNextPRule;
						if (pRP == NULL)
						   /* c'etait la 1ere */
						   pEl->ElFirstPRule = pRS;
						else
						   pRP->PrNextPRule = pRS;
						FreePresentRule (pRSpecif);
						/* libere la regle */
					     }
					}

				      if (pAbb->AbVisibility <= 0)
					 /* l'attribut rend le pave invisible */
					{
#ifdef __COLPAGE__
					   /* on se decale au premier pave de cette suite de pave */
					   /* sans passer par l'element (la suite est discontinue a */
					   /* cause des dupliques) */
					   while (pAbb->AbPrevious != NULL
						  && pAbb->AbPrevious->AbElement == pAbb->AbElement)
					      pAbb = pAbb->AbPrevious;
#else  /* __COLPAGE__ */
					   pAbb = pEl->ElAbstractBox[view - 1];
#endif /* __COLPAGE__ */
					   /* 1er pave a tuer */
					   pReaff = pAbb;
					   /* on reaffichera au moins ce pave */
					   do
					      /* on tue tous les paves de l'element */
					     {
						SetDeadAbsBox (pAbb);
						/* tue un pave */
						/* change les regles des autres paves qui se */
						/* referent au pave detruit */
						ApplyRefAbsBoxSupp (pAbb, &pPR, pDoc);
						pReaff = Enclosing (pReaff, pPR);
						pAbb = pAbb->AbNext;
						if (pAbb == NULL)
						   stop = TRUE;
						else if (pAbb->AbElement != pEl)
						   stop = TRUE;
						else
						   stop = FALSE;
					     }
					   while (!stop);
					}

				      else
					 /* le pave est toujours visible, mais a change' */
					{
					   pReaff = pAbb;
					   SetChange (pAbb, typeRule);
					   /* le parametre de presentation qui vient d'etre */
					   /* change' peut se transmettre au sous-arbre par */
					   /* heritage. On traite le sous-arbre. */
					   pAbbChild = pAbb->AbFirstEnclosed;
					   while (pAbbChild != NULL)
					     {
						ApplyInheritPresRule (pAbbChild, typeRule, pDoc);
						pAbbChild = pAbbChild->AbNext;
					     }
					}
				   }
#ifdef __COLPAGE__
				 /* on passe au duplique suivant */
				 pAbb = pAbb->AbNextRepeated;
			      }
			    while (pAbb != NULL);	/* fin boucle des dupliques */
#endif /* __COLPAGE__ */
			 }

		       if (pEl->ElAbstractBox[view - 1] != NULL
			   && typeRule == PtFunction
			   && (TFonct == FnCreateBefore || TFonct == FnCreateAfter
#ifndef __COLPAGE__
			       || TFonct == FnCreateWith
#endif /* __COLPAGE__ */
		       || TFonct == FnCreateFirst || TFonct == FnCreateLast)
			   && remove)
			  /* on supprime un attribut qui portait une regle de */
			  /* creation ; il faut detruire le pave de presentation que */
			  /* cree la regle pR */
			  /* cherche d'abord le pave de l'element */
			 {
			    pAbb = pEl->ElAbstractBox[view - 1];
			    /* saute les paves de presentation de l'element */
			    while (pAbb->AbNext != NULL && pAbb->AbPresentationBox && pAbb->AbElement == pEl)
			       pAbb = pAbb->AbNext;
#ifdef __COLPAGE__
			    if (TFonct == FnCreateAfter || TFonct == FnCreateLast)
			       while (pAbb->AbNextRepeated != NULL)
				  pAbb = pAbb->AbNextRepeated;
			    pPE = pAbb;		/* pour paves repetes */
			    /* on boucle sur les paves de presentation repetes */
			    do
			      {
				 pAbb = pPE;
#endif /* __COLPAGE__ */
				 /* cherche le pave de presentation a detruire */
				 found = FALSE;
				 switch (TFonct)
				       {
					  case FnCreateBefore:
					     pAbb = pAbb->AbPrevious;
					     break;
					  case FnCreateAfter:
#ifndef __COLPAGE__
					  case FnCreateWith:
#endif /* __COLPAGE__ */
					     pAbb = pAbb->AbNext;
					     break;
					  case FnCreateFirst:
					     pAbb = pAbb->AbFirstEnclosed;
					     break;
					  case FnCreateLast:
					     /* cherche le dernier pave englobe' */
					     pAbb = pAbb->AbFirstEnclosed;
					     if (pAbb != NULL)
						while (pAbb->AbNext != NULL)
						   pAbb = pAbb->AbNext;
					     break;
					  default:
					     break;
				       }
				 if (pAbb != NULL)
				    do
				       /* pas de cas special pour les marques de page dans V4 */
#ifndef __COLPAGE__
				       if (pAbb->AbElement->ElTypeNumber == PageBreak + 1)
					  /* c'est un pave marque de page,passe au pave voisin */
					  if (TFonct == FnCreateBefore || TFonct == FnCreateLast)
					     pAbb = pAbb->AbPrevious;
					  else
					     pAbb = pAbb->AbNext;
				       else
#endif /* __COLPAGE__ */
					  /* ce n'est pas un pave' marque de saut de page */
				       if (!pAbb->AbPresentationBox || pAbb->AbElement != pEl)
					  /* ce n'est pas un pave de presentation ou ce pave */
					  /* n'appartient pas a l'element */
					  pAbb = NULL;
				       else
					  /* c'est un pave de presentation de l'element */
				       if (pAbb->AbTypeNum == pR->PrPresBox[0] && pAbb->AbPSchema == pSchP)
					  /* ce pave a la type cherche', on a trouve' */
					  found = TRUE;
				       else
					  /* passe au pave voisin */
				       if (TFonct == FnCreateBefore || TFonct == FnCreateLast)
					  pAbb = pAbb->AbPrevious;
				       else
					  pAbb = pAbb->AbNext;
				    while (pAbb != NULL && !found);
				 if (found)
				    /* le pave a detruire est pointe' par pAbb */
				    if (!pAbb->AbDead)
				      {
					 pReaff = pAbb;
					 /* on reaffichera au moins ce pave */
					 SetDeadAbsBox (pAbb);
					 /* tue le pave */
					 /* change les regles des autres paves qui se referent */
					 /* au pave detruit */
					 ApplyRefAbsBoxSupp (pAbb, &pPR, pDoc);
					 pReaff = Enclosing (pReaff, pPR);
				      }
#ifdef __COLPAGE__
				 /* on passe au pave duplique suivant pour trouver le pave */
				 /* de presentation repete */
				 if (!TRep)
				    pPE = NULL;
				 else if (TFonct == FnCreateBefore || TFonct == FnCreateFirst)
				    pPE = pPE->AbNextRepeated;
				 else
				    pPE = pPE->AbPreviousRepeated;
			      }
			    while (pPE != NULL);
#endif /* __COLPAGE__ */
			 }
		       /* conserve le pointeur sur le pave a reafficher */
		       if (pReaff != NULL)
			 {
			    if (AssocView (pEl))
			       pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
				  Enclosing (pReaff, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
			    else
			       pDoc->DocViewModifiedAb[view - 1] =
				  Enclosing (pReaff, pDoc->DocViewModifiedAb[view - 1]);
			    /* fin de la boucle sur les vues */
			 }
		    }
	       }

	     /* passe a la regle de presentation suivante pour l'attribut */
	     pR = pRuleNext;
	     if (pR != NULL)
		if (pR->PrType != typeRule ||
		    (typeRule == PtFunction && pR->PrPresFunction != TFonct))
		   /* on change de type de regle */
		  {
		     typeRule = pR->PrType;
		     if (typeRule == PtFunction)
			TFonct = pR->PrPresFunction;
		     pRuleView1 = NULL;
		  }
	  }
	/* on traite les schemas de presentation de moindre priorite' */
	if (pHd == NULL)
	   /* on cherchait dans le schema de presentation principal. */
	   /* c'est fini */
	   pSchP = NULL;
	else
	  {
	     /* on prend le schema de presentation additionnel de priorite' */
	     /* inferieure */
	     pHd = pHd->HdPrevPSchema;
	     if (pHd == NULL)
		/* plus de schemas additionnels, on prend le schema princ */
		pSchP = pAttr->AeAttrSSchema->SsPSchema;
	     else
		pSchP = pHd->HdPSchema;
	  }
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
#ifdef __STDC__
boolean             IsIdenticalTextType (PtrElement pEl, PtrDocument pDoc, PtrElement * pLib)

#else  /* __STDC__ */
boolean             IsIdenticalTextType (pEl, pDoc, pLib)
PtrElement          pEl;
PtrDocument         pDoc;
PtrElement         *pLib;

#endif /* __STDC__ */

{
   PtrElement          pEl2, pEVoisin;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pAbbox1;
   int                 view, dvol;
   boolean             equal, stop;

   equal = FALSE;
   if (pEl != NULL)
     {
	pEl2 = pEl->ElNext;
	if (pEl2 != NULL && pEl->ElLeafType == LtText && pEl->ElTerminal)
	   if (pEl2->ElTerminal && pEl2->ElLeafType == LtText)
	      if (pEl2->ElLanguage == pEl->ElLanguage)
		 if (!pEl2->ElHolophrast && !pEl->ElHolophrast)
		    if (SameAttributes (pEl, pEl2))
		       if (pEl->ElSource == NULL && pEl2->ElSource == NULL)
			  if (BothHaveNoSpecRules (pEl, pEl2))
			     if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct !=
				 CsConstant)
				if (pEl2->ElStructSchema->SsRule[pEl2->ElTypeNumber - 1].SrConstruct
				    != CsConstant)
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
					       if (!AssocView (pEl))
						  pDoc->DocViewModifiedAb[view - 1] =
						     Enclosing (pAb, pDoc->DocViewModifiedAb[view - 1]);
					       else
						 {
						    pEl2 = pEl;
						    pDoc->DocAssocModifiedAb[pEl2->ElAssocNum - 1] =
						       Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl2->ElAssocNum - 1]);
						 }
					       dvol = pEl->ElTextLength - pAb->AbVolume;

					       if (pAb->AbDead && pAb->AbNext != NULL)
						  pAb->AbNext->AbVolume += dvol;

					       do
						 {
						    pAbbox1 = pAb;
						    pAbbox1->AbVolume += dvol;
						    pAb = pAbbox1->AbEnclosing;
						 }
					       while (!(pAb == NULL));
					    }
				       }
				  }
     }
   return equal;
}
