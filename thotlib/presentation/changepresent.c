/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
 * Change specific presentation
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "appaction.h"
#include "appdialogue.h"
#include "presentation.h"
#include "message.h"
#include "picture.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "abspictures_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "boxpositions_f.h"
#include "boxselection_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "docs_f.h"
#include "documentapi_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "unstructchange_f.h"

#ifdef __STDC__
static void         ApplyInherit (PRuleType ruleType, PtrAbstractBox pAb, PtrDocument pDoc);

#else  /* __STDC__ */
static void         ApplyInherit ();

#endif /* __STDC__ */


/*----------------------------------------------------------------------
  ApplyRuleSubTree
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyRuleSubTree (PtrElement pE, PRuleType ruleType, PtrDocument pDoc, PtrPRule * pPRule, int view)
#else  /* __STDC__ */
static void         ApplyRuleSubTree (pE, ruleType, pDoc, pPRule, view)
PtrElement          pE;
PRuleType           ruleType;
PtrDocument         pDoc;
PtrPRule           *pPRule;
int                 view;

#endif /* __STDC__ */
{
   ThotBool            stop;
   PtrAbstractBox      pAbb, pAbbF;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;

   if (pE->ElTerminal)
      pE = NULL;
   else
      pE = pE->ElFirstChild;	/* on passe au premier fils */
   while (pE != NULL)
     {
	pAbb = pE->ElAbstractBox[view - 1];	/* 1er pave de l'element dans la view */
	if (pAbb != NULL)
	   if (pAbb->AbDead)
	      pAbb = NULL;	/* on ne traite pas les paves morts */
	if (pAbb == NULL)
	   /* cet element n'a pas de pave, mais ses descendants en */
	   /* ont peut etre... */
	   ApplyRuleSubTree (pE, ruleType, pDoc, pPRule, view);
	else
	  {
	     /* il y a un element descendant dont les paves peuvent heriter de pAb. 
	        On parcourt ses paves dans la view */
	     stop = FALSE;
	     while (!stop)
	       {
		  *pPRule = SearchRulepAb (pDoc, pAbb, &pSPR, ruleType, FnAny, TRUE, &pAttr);
		  if (*pPRule != NULL)
		    {
		       if ((*pPRule)->PrPresMode == PresInherit && ((*pPRule)->PrInheritMode == InheritParent ||
			    (*pPRule)->PrInheritMode == InheritGrandFather))
			  /* la regle de ce pave herite de l'ascendant, */
			  /* on applique la regle */
			  if (ApplyRule (*pPRule, pSPR, pAbb, pDoc, pAttr))
			    {
			       if (ruleType == PtSize)
				  pAbb->AbSizeChange = TRUE;
			       else if (ruleType == PtDepth || ruleType == PtLineStyle ||
					ruleType == PtLineWeight || ruleType == PtFillPattern ||
					ruleType == PtBackground || ruleType == PtForeground)
				  pAbb->AbAspectChange = TRUE;
			       else
				  pAbb->AbChange = TRUE;
			       RedispAbsBox (pAbb, pDoc);
			       if (!pAbb->AbPresentationBox)
				  ApplyInherit (ruleType, pAbb, pDoc);
			    }
		    }
		  if (!pAbb->AbPresentationBox)
		     /* c'est le pave principal de l'element, on traite */
		     /* les paves crees par l'element au niveau inferieur */
		    {
		       pAbbF = pAbb->AbFirstEnclosed;
		       while (pAbbF != NULL)
			 {
			    if (pAbbF->AbElement == pE)
			      {
				 *pPRule = SearchRulepAb (pDoc, pAbbF, &pSPR, ruleType, FnAny, TRUE, &pAttr);
				 if (*pPRule != NULL)
				   {
				      if ((*pPRule)->PrPresMode == PresInherit
					  && (*pPRule)->PrInheritMode == InheritParent)
					 if (ApplyRule (*pPRule, pSPR, pAbbF, pDoc, pAttr))
					   {
					      if (ruleType == PtSize)
						 pAbbF->AbSizeChange = TRUE;
					      else if (ruleType == PtDepth ||
						  ruleType == PtLineStyle ||
						 ruleType == PtLineWeight ||
						 ruleType == PtFillPattern ||
						 ruleType == PtBackground ||
						   ruleType == PtForeground)
						 pAbb->AbAspectChange = TRUE;
					      else
						 pAbbF->AbChange = TRUE;
					      RedispAbsBox (pAbbF, pDoc);
					   }
				   }
			      }
			    pAbbF = pAbbF->AbNext;
			 }
		    }
		  pAbb = pAbb->AbNext;	/* passe au pave suivant */
		  if (pAbb == NULL)
		     stop = TRUE;	/* pas de pave suivant, on arrete */
		  else
		     /* on arrete si le pave suivant n'appartient pas a */
		     /* l'element */
		     stop = pAbb->AbElement != pE;
	       }
	  }
	pE = pE->ElNext;	/* on traite l'element suivant */
     }
}


/*----------------------------------------------------------------------
   	ApplyInherit on vient d'appliquer la regle de presentation de type	
   		ruleType au pave pAb. Verifie si les paves environnants	
   		heritent de cette regle et si oui leur applique		
   		l'heritage.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyInherit (PRuleType ruleType, PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
static void         ApplyInherit (ruleType, pAb, pDoc)
PRuleType           ruleType;
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   int                 view;
   PtrAbstractBox      pAbbCur;
   PtrPRule            pPRule;
   ThotBool            stop;
   PtrPSchema          pSchP;
   PtrAttribute        pAttrib;

   pEl = pAb->AbElement;
   view = pAb->AbDocView;
   if (pEl->ElNext != NULL)
     {
	/* l'element a un suivant. Celui-ci herite-t-il de son precedent ? */
	pAbbCur = NULL;
	while (pEl->ElNext != NULL && pAbbCur == NULL)
	  {
	     pEl = pEl->ElNext;
	     pAbbCur = pEl->ElAbstractBox[view - 1];	/* saute les paves de presentation */
	     stop = FALSE;
	     do
		if (pAbbCur == NULL)
		   stop = TRUE;
		else if (!pAbbCur->AbPresentationBox)
		   stop = TRUE;
		else
		   pAbbCur = pAbbCur->AbNext;
	     while (!stop);
	     if (pAbbCur != NULL)
		if (pAbbCur->AbDead)
		   pAbbCur = NULL;
	  }
	if (pAbbCur != NULL)
	  {
	     /* il y a un element suivant dont le pave pAbbCur pourrait heriter de pAb */
	     pPRule = SearchRulepAb (pDoc, pAbbCur, &pSchP, ruleType, FnAny, TRUE, &pAttrib);
	     if (pPRule != NULL)
	       {
		  if (pPRule->PrPresMode == PresInherit &&
		      pPRule->PrInheritMode == InheritPrevious)
		     /* la regle de cet element herite du precedent, on applique */
		     /* la regle */
		     if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
		       {
			  if (ruleType == PtSize)
			     pAbbCur->AbSizeChange = TRUE;
			  else if (ruleType == PtDepth || ruleType == PtLineStyle ||
				   ruleType == PtLineWeight || ruleType == PtFillPattern ||
				   ruleType == PtBackground || ruleType == PtForeground)
			     pAbbCur->AbAspectChange = TRUE;
			  else
			     pAbbCur->AbChange = TRUE;
			  ApplyInherit (ruleType, pAbbCur, pDoc);
			  RedispAbsBox (pAbbCur, pDoc);
		       }
	       }
	  }
	pEl = pAb->AbElement;
     }
   if (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
      /* l'element a des descendants. Ceux-ci heritent-t-il de leur */
      /* ascendant ? */
      ApplyRuleSubTree (pEl, ruleType, pDoc, &pPRule, view);
   if (pEl->ElParent != NULL)
     {
	/* l'element a un ascendant. Celui-ci herite-t-il de son premier */
	/* descendant ? */
	pAbbCur = NULL;
	while (pEl->ElParent != NULL && pAbbCur == NULL)
	  {
	     pEl = pEl->ElParent;
	     pAbbCur = pEl->ElAbstractBox[view - 1];	/* saute les paves de presentation */
	     stop = FALSE;
	     do
		if (pAbbCur == NULL)
		   stop = TRUE;
		else if (!pAbbCur->AbPresentationBox)
		   stop = TRUE;
		else
		   pAbbCur = pAbbCur->AbNext;
	     while (!(stop));
	     if (pAbbCur != NULL)
		if (pAbbCur->AbDead)
		   pAbbCur = NULL;
	  }
	if (pAbbCur != NULL)
	  {
	     /* il y a un element ascendant dont le pave pAbbCur pourrait heriter */
	     /* de pAb */
	     pPRule = SearchRulepAb (pDoc, pAbbCur, &pSchP, ruleType, FnAny, TRUE, &pAttrib);
	     if (pPRule != NULL)
		if (pPRule->PrPresMode == PresInherit &&
		    pPRule->PrInheritMode == InheritChild)
		   /* la regle de cet element herite du descendant, on */
		   /* applique la regle */
		   if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
		     {
			if (ruleType == PtSize)
			   pAbbCur->AbSizeChange = TRUE;
			else if (ruleType == PtDepth || ruleType == PtLineStyle ||
				 ruleType == PtLineWeight || ruleType == PtFillPattern ||
			ruleType == PtBackground || ruleType == PtForeground)
			   pAbbCur->AbAspectChange = TRUE;
			else
			   pAbbCur->AbChange = TRUE;
			ApplyInherit (ruleType, pAbbCur, pDoc);
			RedispAbsBox (pAbbCur, pDoc);
		     }
	  }
     }

   if (!pAb->AbPresentationBox)
      /* ce n'est pas un pave de presentation. On regarde si les paves de */
      /* presentation crees par l'element heritent de leur createur */
     {
	/* on regarde d'abord les paves crees devant (par CreateBefore) */
	pEl = pAb->AbElement;
	pAbbCur = pAb->AbPrevious;
	while (pAbbCur != NULL)
	   if (!pAbbCur->AbPresentationBox || pAbbCur->AbElement != pEl)
	      /* ce n'est pas un pave de presentation de l'element, on arrete */
	      pAbbCur = NULL;
	   else
	     {
		pPRule = SearchRulepAb (pDoc, pAbbCur, &pSchP, ruleType, FnAny, TRUE, &pAttrib);
		if (pPRule != NULL)
		   if (pPRule->PrPresMode == PresInherit && pPRule->PrInheritMode == InheritCreator)
		      /* la regle de ce pave herite de son createur, on l'applique */
		      if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
			{
			   if (ruleType == PtSize)
			      pAbbCur->AbSizeChange = TRUE;
			   else if (ruleType == PtDepth || ruleType == PtLineStyle ||
				    ruleType == PtLineWeight || ruleType == PtFillPattern ||
				    ruleType == PtBackground || ruleType == PtForeground)
			      pAbbCur->AbAspectChange = TRUE;
			   else
			      pAbbCur->AbChange = TRUE;
			   ApplyInherit (ruleType, pAbbCur, pDoc);
			   RedispAbsBox (pAbbCur, pDoc);
			}
		/* examine le pave precedent */
		pAbbCur = pAbbCur->AbPrevious;
	     }
	/* on regarde les paves crees derriere (par CreateAfter) */
	pAbbCur = pAb->AbNext;
	while (pAbbCur != NULL)
	   if (!pAbbCur->AbPresentationBox || pAbbCur->AbElement != pEl)
	      /* ce n'est pas un pave de presentation de l'element, on arrete */
	      pAbbCur = NULL;
	   else
	     {
		pPRule = SearchRulepAb (pDoc, pAbbCur, &pSchP, ruleType, FnAny, TRUE, &pAttrib);
		if (pPRule != NULL)
		   if (pPRule->PrPresMode == PresInherit && pPRule->PrInheritMode == InheritCreator)
		      /* la regle de ce pave herite de son createur, on l'applique */
		      if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
			{
			   if (ruleType == PtSize)
			      pAbbCur->AbSizeChange = TRUE;
			   else if (ruleType == PtDepth || ruleType == PtLineStyle ||
				    ruleType == PtLineWeight || ruleType == PtFillPattern ||
				    ruleType == PtBackground || ruleType == PtForeground)
			      pAbbCur->AbAspectChange = TRUE;
			   else
			      pAbbCur->AbChange = TRUE;
			   ApplyInherit (ruleType, pAbbCur, pDoc);
			   RedispAbsBox (pAbbCur, pDoc);
			}
		/* examine le pave suivant */
		pAbbCur = pAbbCur->AbNext;
	     }
	/* on regarde les paves crees au niveau inferieur (par Create et CreateLast) */
	pAbbCur = pAb->AbFirstEnclosed;
	while (pAbbCur != NULL)
	  {
	     if (pAbbCur->AbPresentationBox && pAbbCur->AbElement == pEl)
		/* c'est un pave de presentation de l'element, on le traite */
	       {
		  pPRule = SearchRulepAb (pDoc, pAbbCur, &pSchP, ruleType, FnAny, TRUE, &pAttrib);
		  if (pPRule != NULL)
		     if (pPRule->PrPresMode == PresInherit && pPRule->PrInheritMode == InheritCreator)
			/* la regle de ce pave herite de son createur, on l'applique */
			if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
			  {
			     if (ruleType == PtSize)
				pAbbCur->AbSizeChange = TRUE;
			     else if (ruleType == PtDepth || ruleType == PtLineStyle ||
				      ruleType == PtLineWeight || ruleType == PtFillPattern ||
				      ruleType == PtBackground || ruleType == PtForeground)
				pAbbCur->AbAspectChange = TRUE;
			     else
				pAbbCur->AbChange = TRUE;
			     ApplyInherit (ruleType, pAbbCur, pDoc);
			     RedispAbsBox (pAbbCur, pDoc);
			  }
	       }
	     /* examine le pave suivant */
	     pAbbCur = pAbbCur->AbNext;
	  }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int          NumTypePRuleAPI (PtrPRule pRule)
#else  /* __STDC__ */
int          NumTypePRuleAPI (pRule)
PRuleType    pRule;

#endif /* __STDC__ */
{
   switch (pRule->PrType)
	 {
	    case PtVisibility:
	       return PRVisibility;
	       break;
	    case PtVertRef:
	       return PRVertRef;
	       break;
	    case PtHorizRef:
	       return PRHorizRef;
	       break;
	    case PtHeight:
	       return PRHeight;
	       break;
	    case PtWidth:
	       return PRWidth;
	       break;
	    case PtVertPos:
	       return PRVertPos;
	       break;
	    case PtHorizPos:
	       return PRHorizPos;
	       break;
	    case PtSize:
	       return PRSize;
	       break;
	    case PtStyle:
	       return PRStyle;
	       break;
	    case PtWeight:
	       return PRWeight;
	       break;
	    case PtFont:
	       return PRFont;
	       break;
	    case PtUnderline:
	       return PRUnderline;
	       break;
	    case PtThickness:
	       return PRThickness;
	       break;
	    case PtIndent:
	       return PRIndent;
	       break;
	    case PtLineSpacing:
	       return PRLineSpacing;
	       break;
	    case PtDepth:
	       return PRDepth;
	       break;
	    case PtAdjust:
	       return PRAdjust;
	       break;
	    case PtJustify:
	       return PRJustify;
	       break;
	    case PtLineStyle:
	       return PRLineStyle;
	       break;
	    case PtLineWeight:
	       return PRLineWeight;
	       break;
	    case PtFillPattern:
	       return PRFillPattern;
	       break;
	    case PtBackground:
	       return PRBackground;
	       break;
	    case PtForeground:
	       return PRForeground;
	       break;
	    case PtHyphenate:
	       return PRHyphenate;
	       break;
	    case PtBreak1:
	       return PRNoBreak1;
	       break;
	    case PtBreak2:
	       return PRNoBreak2;
	       break;
	    case PtFunction:
	       if (pRule->PrPresFunction == FnShowBox)
		  return PRShowBox;
	       else if (pRule->PrPresFunction == FnNotInLine)
		  return PRNotInLine;
	       else
	          return PRFunction;
	       break;
	    default:
	       return 0;
	       break;
	 }
}

#define SETSIZE 32
/*----------------------------------------------------------------------
   	RuleSetIn teste l'appartenance d'un element a` un ensemble.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 RuleSetIn (register int num, RuleSet RuleS1)
#else  /* __STDC__ */
int                 RuleSetIn (num, RuleS1)
register int        num;
RuleSet             RuleS1;

#endif /* __STDC__ */
{
   if (num >= 0 && num < SETSIZE * 8)
      return RuleS1[num >> 3] & (1 << (num & 7));
   else
      return 0;
}


/*----------------------------------------------------------------------
   	RuleSetPut ajoute un element a` un ensemble.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RuleSetPut (RuleSet RuleS1, int num)
#else  /* __STDC__ */
void                RuleSetPut (RuleS1, num)
RuleSet             RuleS1;
int                 num;

#endif /* __STDC__ */
{
   if (num >= 0 && num < SETSIZE * 8)
      RuleS1[num >> 3] |= (1 << (num & 7));
}


/*----------------------------------------------------------------------
   	RuleSetClr met a` 0 l'ensemble RuleS1.				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RuleSetClr (RuleSet RuleS1)
#else  /* __STDC__ */
void                RuleSetClr (RuleS1)
RuleSet             RuleS1;

#endif /* __STDC__ */
{
   USTRING s1;
   int     num;

   s1 = RuleS1;
   for (num = SETSIZE; --num >= 0;)
      *s1++ = 0;
}

/*----------------------------------------------------------------------
   	BothHaveNoSpecRules retourne Vrai si aucun des deux elements pointes par	
   		pEl1 et pEl2 possedent des regles de presentation	
   		specifique.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
ThotBool            BothHaveNoSpecRules (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   ThotBool            equal;

   equal = FALSE;
   /*on peut faire mieux... */
   if (pEl1->ElFirstPRule == NULL && pEl2->ElFirstPRule == NULL)
      equal = TRUE;
   return equal;
}

/*----------------------------------------------------------------------
   	AbsBoxOfEl retourne un pointeur sur le pave de l'element pEl	
   		qui appartient a la view view				
   		et qui n'est pas un pave de presentation.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox      AbsBoxOfEl (PtrElement pEl, int view)
#else  /* __STDC__ */
PtrAbstractBox      AbsBoxOfEl (pEl, view)
PtrElement          pEl;
int                 view;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   ThotBool            stop;

   pAb = NULL;
   if (view > 0)
      if (pEl->ElAbstractBox[view - 1] != NULL)
	 /* on prend le pave de la view choisie par l'utilisateur */
	 pAb = pEl->ElAbstractBox[view - 1];
   /* saute les paves de presentation */
   stop = FALSE;
   do
      if (pAb == NULL)
	 stop = TRUE;
      else if (!pAb->AbPresentationBox)
	 stop = TRUE;
      else
	 pAb = pAb->AbNext;
   while (!stop);
   return pAb;
}

/*----------------------------------------------------------------------
  ApplyNewRule applique au pave courant la regle de		
  presentation specifique qui vient d'etre creee.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyNewRule (PtrDocument pDoc, PtrPRule pPRule, PtrElement pEl)
#else  /* __STDC__ */
void                ApplyNewRule (pDoc, pPRule, pEl)
PtrDocument         pDoc;
PtrPRule            pPRule;
PtrElement          pEl;

#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  int                 view;
  ThotBool            stop;

  /* parcourt toutes les vues du document */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    if (pEl->ElAbstractBox[view - 1] != NULL)
      /* l'element traite' a un pave dans cette view */
      if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocSSchema &&
	  pDoc->DocView[view - 1].DvPSchemaView == pPRule->PrViewNum)
	{
	  /* c'est une view de meme type que la view traitee, on */
	  /* traite le pave de l'element dans cette view */
	  pAb = pEl->ElAbstractBox[view - 1];
	  /* saute les paves de presentation */
	  stop = FALSE;
	  do
	    if (pAb == NULL)
	      stop = TRUE;
	    else if (!pAb->AbPresentationBox)
	      stop = TRUE;
	    else
	      pAb = pAb->AbNext;
	  while (!stop);
	      if (pAb != NULL)
		/* applique la regle de presentation specifique a ce pave' */
		  if (ApplyRule (pPRule, NULL, pAb, pDoc, NULL))
		    {
		      switch (pPRule->PrType)
			{
			case PtHeight:
			  pAb->AbHeightChange = TRUE;
			  break;
			case PtWidth:
			  pAb->AbWidthChange = TRUE;
			  break;
			case PtVertPos:
			  pAb->AbVertPosChange = TRUE;
			  break;
			case PtHorizPos:
			  pAb->AbHorizPosChange = TRUE;
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
			  pAb->AbAspectChange = TRUE;
			  break;
			case PtFunction:
			  if (pPRule->PrPresFunction == FnPictureMode
			      || pPRule->PrPresFunction == FnBackgroundPicture
			      || pPRule->PrPresFunction == FnShowBox)
			    pAb->AbAspectChange = TRUE;
			  else
			    pAb->AbChange = TRUE;
			  break;
			default:
			  pAb->AbChange = TRUE;
			  break;
			}
		      ApplyInherit (pPRule->PrType, pAb, pDoc);
		      /* indique le pave a faire reafficher */
		      RedispAbsBox (pAb, pDoc);
		    }
	}
}


/*----------------------------------------------------------------------
  RemoveSpecifPres supprime toutes les regles de presentation specifiques
  associees a l'element pEl.				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveSpecifPres (PtrElement pEl, PtrDocument pDoc, RuleSet rules, int viewToApply)
#else  /* __STDC__ */
static void         RemoveSpecifPres (pEl, pDoc, rules, viewToApply)
PtrElement          pEl;
PtrDocument         pDoc;
RuleSet             rules;
int                 viewToApply;

#endif /* __STDC__ */
{
   PtrPRule            pPRule, pR, pRS;
   NotifyPresentation  notifyPres;
   RuleSet             rulesS;
   Document            doc;
   int                 viewSch;
   ThotBool            found;

   /* type de cette view */
   viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
   pPRule = pEl->ElFirstPRule;
   pR = NULL;
   found = FALSE;
   doc = IdentDocument (pDoc);
   /* parcourt les regles de presentation specifiques de l'element */
   while (pPRule != NULL)
      if (pPRule->PrViewNum != viewSch || !RuleSetIn (pPRule->PrType, rules))
	 /* cette regle n'est pas concernee */
	{
	   pR = pPRule;
	   pPRule = pR->PrNextPRule;
	}
      else
	{
	   found = TRUE;
	   /* la regle concerne la view traitee */
	   /* retire la regle de la chaine des regles de presentation */
	   /* specifique de l'element */
	   pRS = pPRule->PrNextPRule;	/* regle a traiter apres */
	   notifyPres.event = TtePRuleDelete;
	   notifyPres.document = doc;
	   notifyPres.element = (Element) pEl;
	   notifyPres.pRule = (PRule) pPRule;
	   notifyPres.pRuleType = NumTypePRuleAPI (pPRule);
	   if (!CallEventType ((NotifyEvent *) & notifyPres, TRUE))
	     {
		if (pR == NULL)
		   pEl->ElFirstPRule = pRS;
		else
		   pR->PrNextPRule = pRS;
		SetDocumentModified (pDoc, TRUE, 0);
		/* applique la regle standard de meme type que la regle courante */
		/* aux paves de l'element qui existent dans les vues de meme type */
		/* que la view active. */
		ApplyStandardRule (pEl, pDoc, pPRule->PrType, pPRule->PrPresFunction, viewSch);
		notifyPres.event = TtePRuleDelete;
		notifyPres.document = doc;
		notifyPres.element = (Element) pEl;
		notifyPres.pRule = NULL;
		notifyPres.pRuleType = NumTypePRuleAPI (pPRule);
		/* libere la regle */
		FreePresentRule (pPRule);
		CallEventType ((NotifyEvent *) & notifyPres, FALSE);
	     }
	   /* passe a la regle suivante */
	   pPRule = pRS;
	}

   if (!found && pEl->ElParent)
     {
       if (RuleSetIn (PtBackground, rules))
	 {
	   RuleSetClr (rulesS);
	   RuleSetPut (rulesS, PtBackground);
	   RemoveSpecifPres (pEl->ElParent, pDoc, rulesS, viewToApply);
	 }
       else if (RuleSetIn (PtForeground, rules))
	 {
	   RuleSetClr (rulesS);
	   RuleSetPut (rulesS, PtForeground);
	   RemoveSpecifPres (pEl->ElParent, pDoc, rulesS, viewToApply);
	 }
     }
}

/*----------------------------------------------------------------------
  ApplyPRuleAndRedisplay: apply a presentation rule, update the view
  and apply the inheritance of the rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyPRuleAndRedisplay (PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr, PtrPRule pRP, PtrPSchema pSPR)
#else  /* __STDC__ */
static void         ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR)
PtrAbstractBox      pAb;
PtrDocument         pDoc;
PtrAttribute        pAttr;
PtrPRule            pRP;
PtrPSchema          pSPR;

#endif /* __STDC__ */
{
  if (pRP == NULL)
     return;

  ApplyRule (pRP, pSPR, pAb, pDoc, pAttr);
  /* marque que le pave a change' et doit etre reaffiche' */
  switch (pRP->PrType)
    {
    case PtVertRef:
      pAb->AbVertRefChange = TRUE;
      break;
    case PtHorizRef:
      pAb->AbHorizRefChange = TRUE;
      break;
    case PtHeight:
      pAb->AbHeightChange = TRUE;
      break;
    case PtWidth:
      pAb->AbWidthChange = TRUE;
      break;
    case PtVertPos:
      pAb->AbVertPosChange = TRUE;
      break;
    case PtHorizPos:
      pAb->AbHorizPosChange = TRUE;
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
      pAb->AbAspectChange = TRUE;
      break;
    case PtFunction:
      if (pRP->PrPresFunction == FnPictureMode
	  || pRP->PrPresFunction == FnBackgroundPicture
	  || pRP->PrPresFunction == FnShowBox)
	pAb->AbAspectChange = TRUE;
      else
	pAb->AbChange = TRUE;
      break;
    default:
      pAb->AbChange = TRUE;
      break;
    }
  
  RedispAbsBox (pAb, pDoc);
  /* applique la regle de meme type aux paves environnants */
  /* s'ils heritent de ce parametre de presentation */
  ApplyInherit (pRP->PrType, pAb, pDoc);
}

/*----------------------------------------------------------------------
  ApplyStandardRule	applique a l'element pEl du document	
  pDoc la regle de presentation standard de type ruleType	
  pour la view viewSch.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyStandardRule (PtrElement pEl, PtrDocument pDoc, PRuleType ruleType, FunctionType funcType, int viewSch)
#else  /* __STDC__ */
void                ApplyStandardRule (pEl, pDoc, ruleType, funcType, viewSch)
PtrElement          pEl;
PtrDocument         pDoc;
PRuleType           ruleType;
FunctionType        funcType;
int                 viewSch;

#endif /* __STDC__ */
{
  PtrPRule            pRP;
  PtrAbstractBox      pAb;
  PtrPSchema          pSPR;
  PtrAttribute        pAttr;
  ThotBool            stop;
  int                 view;

  /* applique la regle standard de meme type que la regle courante */
  /* aux paves de l'element qui existent dans les vues de meme type */
  /* que la view active. */
  pRP = NULL;
  /* on n'a pas encore cherche' la regle standard */
  if (viewSch > 0)
    /* il y a une view active */
    /* parcourt toutes les vues du document */
    for (view = 1; view <= MAX_VIEW_DOC; view++)
      if (pEl->ElAbstractBox[view - 1] != NULL)
	/* l'element traite' a un pave dans cette view */
	if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocSSchema &&
	    pDoc->DocView[view - 1].DvPSchemaView == viewSch)
	  /* c'est une view de meme type que la view traitee, on */
	  /* traite le pave de l'element dans cette view */
	  {
	    pAb = pEl->ElAbstractBox[view - 1];
	    /* saute les paves de presentation */
	    stop = FALSE;
	    do
	      if (pAb == NULL)
		stop = TRUE;
	      else if (!pAb->AbPresentationBox)
		stop = TRUE;
	      else
		pAb = pAb->AbNext;
	    while (!stop);
	    if (pAb != NULL)
	      {
		/* cherche la regle standard si on ne l'a pas encore */
		if (pRP == NULL)
		  pRP = SearchRulepAb (pDoc, pAb, &pSPR, ruleType, funcType, TRUE, &pAttr);
		if (pRP != NULL)
		  ApplyPRuleAndRedisplay(pAb, pDoc, pAttr, pRP, pSPR);
	      }
	  }
}

/*----------------------------------------------------------------------
  NextAbstractBox: search the next abstract box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAbstractBox     NextAbstractBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrAbstractBox     NextAbstractBox (pAb)
PtrAbstractBox     pAb;
#endif /* __STDC__ */

{
  PtrAbstractBox      pNextAb;

  /* Il y a un premier fils different du pave reference ? */
  if (pAb->AbFirstEnclosed != NULL)
    pNextAb = pAb->AbFirstEnclosed;
  /* Il y a un frere different du pave reference ? */
  else if (pAb->AbNext != NULL)
    pNextAb = pAb->AbNext;
  /* Sinon on remonte dans la hierarchie */
  else
    {
      while (pAb != NULL && pAb->AbEnclosing != NULL && pAb->AbNext == NULL)
	pAb = pAb->AbEnclosing;

      if (pAb != NULL)
	pNextAb = pAb->AbNext;
      else
	pNextAb = NULL;
    }
  return pNextAb;
}

/*----------------------------------------------------------------------
  RemoveFunctionPRule
  The pPres function presentation rule has been removed for the element
  having abstract box pAb.  Undo that presentation rule for that abstract box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void       RemoveFunctionPRule (PtrPRule pPres, PtrAbstractBox pAb, PtrDocument pDoc)
#else  /* __STDC__ */
static void       RemoveFunctionPRule (pPres, pAb, pDoc)
PtrPRule pPres;
PtrAbstractBox pAb;
PtrDocument pDoc;
#endif /* __STDC__ */

{
   if (pPres->PrPresFunction == FnBackgroundPicture &&
       pAb->AbPictBackground != NULL)
     {
       TtaFreeMemory ((((PictInfo *) (pAb->AbPictBackground))->PicFileName));
       FreePictInfo ((PictInfo *) (pAb->AbPictBackground));
       TtaFreeMemory (pAb->AbPictBackground);
       pAb->AbPictBackground = NULL;
     }
   else if (pPres->PrPresFunction == FnPictureMode &&
	    pAb->AbPictBackground != NULL)
       ((PictInfo *) (pAb->AbPictBackground))->PicPresent = FillFrame;
   else if (pPres->PrPresFunction == FnShowBox)
       pAb->AbFillBox = FALSE;

   pAb->AbAspectChange = TRUE;
   RedispAbsBox (pAb, pDoc);
   /* pDoc->DocViewModifiedAb[view] = pAb; */
}

/*----------------------------------------------------------------------
  ApplyASpecificStyleRule
  Redisplay boxes of element pEl that are concerned by removing the
  presentation function pRule
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            ApplyASpecificStyleRule (PtrPRule pRule, PtrElement pEl, PtrDocument pDoc, ThotBool remove)
#else  /* __STDC__ */
void            ApplyASpecificStyleRule (pRule, pEl, pDoc, remove)
PtrPRule	pRule;
PtrElement	pEl;
PtrDocument	pDoc;
ThotBool        remove;

#endif /* __STDC__ */

{
  PtrAbstractBox  pAb, pParent;
  PtrPRule	  pCurrentRule, pRP;
  PtrPSchema	  pSPR;
  PtrAttribute	  pAttr;
  PRuleType	  ruleType;
  int             viewSch;
  int             view;
  ThotBool	  done, enclosed;

  TtaClearViewSelections ();
  enclosed = FALSE;
  /* do nothing if the document no longer exists */
  if (pDoc != NULL)
    /* examine all abstract boxes of element */
    for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      /* the abstract box of the root element */
      pAb = pEl->ElAbstractBox[view];
      /* the schema view associatde with the current view */
      viewSch = pDoc->DocView[view].DvPSchemaView;
      while (pAb != NULL)
	{
	  /* process each presentation rule */
	  pCurrentRule = pRule;
	  done = FALSE;
	  if (pCurrentRule != NULL)
	    {
	    ruleType = pCurrentRule->PrType;
	    /* is the view concerned by the presentation rule ? */
	    if (pCurrentRule->PrViewNum == 1 ||
		pCurrentRule->PrViewNum == viewSch)
	       {
	       /* checks if the abstract box is concerned by the rule */
	       pRP = SearchRulepAb (pDoc, pAb, &pSPR, ruleType,
				   pCurrentRule->PrPresFunction, TRUE, &pAttr);
	       if (pRP == pCurrentRule || remove)
		  {
		  done = TRUE;
	          if (remove && ruleType == PtFunction &&
		     pAb->AbLeafType == LtCompound)
	             /* remove a PtFunction rule */
		     RemoveFunctionPRule (pCurrentRule, pAb, pDoc);
		  else if (pRP != NULL)
		     ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
		  }
	       }
	    /* pCurrentRule = pCurrentRule->PrNextPRule; */
	    }

	  if (done)
	    {
	      /* update abstract image and redisplay */
	      AbstractImageUpdated (pDoc);
	      RedisplayDocViews (pDoc);
	    }

	  /* get the next abstract box of that element */
	  if (enclosed)
	     {
	     pParent = pAb->AbEnclosing;
	     pAb = pAb->AbNext;
	     while (pAb && pAb->AbElement != pEl)
		pAb = pAb->AbNext;
	     if (pAb == NULL && pParent)
		{
		pAb = pParent->AbNext;
		if (pAb && pAb->AbElement != pEl)
		   pAb = NULL;
		enclosed = FALSE;
		}
	     }
          else
	     if (pAb->AbPresentationBox || !pAb->AbFirstEnclosed)
		{
	        pAb = pAb->AbNext;
		if (pAb && pAb->AbElement != pEl)
		   pAb = NULL;
		}
	     else
		{
		pAb = pAb->AbFirstEnclosed;
	        pParent = pAb->AbEnclosing;
	        while (pAb && pAb->AbElement != pEl)
		   pAb = pAb->AbNext;
		if (pAb)
		   enclosed = TRUE;
		else if (pParent)
		   {
		   pAb = pParent->AbNext;
		   if (pAb && pAb->AbElement != pEl)
		      pAb = NULL;
		   }
		}
	}
    }
}

/*----------------------------------------------------------------------
  ApplyAGenericStyleRule applies a Presentation Rule to all abstract
  boxes concerned by  the given element type or the given attribute type
  or the given presentation box.
  For each displayed abstract box and each new presention rule
  check if it is concerned by this new pRule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               ApplyAGenericStyleRule (Document doc, PtrSSchema pSS, int elType, int attrType, int presBox, PtrPRule pRule, ThotBool remove)
#else  /* __STDC__ */
void               ApplyAGenericStyleRule (doc, pSS, elType, attrType, presBox, pRule, remove)
Document           doc;
PtrSSchema         pSS;
int                elType;
int                attrType;
int                presBox;
PtrPRule           pRule;
ThotBool           remove;
#endif /* __STDC__ */

{
  PtrPRule        pCurrentRule, pRP;
  PtrPSchema      pSPR;
  PtrAttribute    pAttr;
  PtrDocument     pDoc, pSelDoc;
  PtrElement      pFirstSel, pLastSel;
  PtrAbstractBox  pAb;
  PRuleType       ruleType;
  int             firstChar, lastChar;
  int             viewSch;
  int             view;
  ThotBool        selectionOK;
  ThotBool        found;

  pDoc = LoadedDocument[doc - 1];
  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel,
				     &firstChar, &lastChar);
  /* eteint la selection courante */
  TtaClearViewSelections ();
  /* do nothing if the document no longer exists */
  if (pDoc != NULL)
    /* examine all abstract boxes of elements */
    for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      /* the abstract box of the root element */
      pAb = pDoc->DocViewRootAb[view];
      /* the schema view associated with the current view */
      viewSch = pDoc->DocView[view].DvPSchemaView;
      while (pAb != NULL)
	{
	  found = FALSE;
	  if (elType > 0)
	    /* presentation rules are associated with an element type */
	    found = (pAb->AbElement->ElTypeNumber == elType &&
	             pAb->AbElement->ElStructSchema->SsCode == pSS->SsCode);
	  else if (attrType > 0)
	    {
	      /* presentation rules are associated with an attribute type */
	      pAttr = pAb->AbElement->ElFirstAttr;
	      while (!found && pAttr != NULL)
		{
		  found = (pAttr->AeAttrNum == attrType &&
			   pAttr->AeAttrSSchema->SsCode == pSS->SsCode);
		  if (!found)
		     pAttr = pAttr->AeNext;
		}
	    }
	  else if (presBox > 0)
	    /* presentation rules are associated with a presentation box */
	    found = (pAb->AbPresentationBox && pAb->AbTypeNum == presBox &&
		     pAb->AbPSchema == pSS->SsPSchema);
	  if (found)
	    {
	      /* process each presentation rule */
	      pCurrentRule = pRule;
	      if (pCurrentRule != NULL)
		{
		  ruleType = pCurrentRule->PrType;
		  found = FALSE;    /* indicate if a rule has been applied */
		  /* is the view concerned by the presentation rule ? */
		  if (pCurrentRule->PrViewNum == 1 ||
		      pCurrentRule->PrViewNum == viewSch)
		    {
		      /* checks if the abstract box is concerned by the rule */
		      pRP = SearchRulepAb (pDoc, pAb, &pSPR, ruleType,
				   pCurrentRule->PrPresFunction, TRUE, &pAttr);
		      if (pRP == pCurrentRule || remove)
			{
			  /* apply a new rule */
			  found = TRUE;
			  if (remove && ruleType == PtFunction &&
			      pAb->AbLeafType == LtCompound)
			    {
			      /* remove a PtFunction rule */
			      RemoveFunctionPRule (pCurrentRule, pAb, pDoc);
			      if (pRP != NULL && pRP != pCurrentRule)
				ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
			    }
			  else if (pRP != NULL)
			    ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
			}
		    }
		  /*pCurrentRule = pCurrentRule->PrNextPRule;*/
		}
	      /* redisplay the element if needed */
	      if (found)
		{
		  /* update abstract image and redisplay */
		  AbstractImageUpdated (pDoc);
		  RedisplayDocViews (pDoc);
		}
	    }
	  /* get the next abstract box */
	  pAb = NextAbstractBox (pAb);
	  if (pAb != NULL)
	    if ((presBox == 0 && pAb->AbPresentationBox) ||
		(presBox > 0  && !pAb->AbPresentationBox))
	      pAb = NextAbstractBox (pAb);
	}
    }
  /* tente de fusionner les elements voisins et reaffiche les paves */
  /* modifie's et la selection */
  if (pSelDoc != NULL && pSelDoc == pDoc)
    SelectRange (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
}

/*----------------------------------------------------------------------
  RemoveSpecPresTree supprime les regles de presentation specifiques	
  contenues dans 'RulesS' attachees aux elements du	
  sous-arbre de racine pElRoot				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveSpecPresTree (PtrElement pElRoot, PtrDocument pDoc, RuleSet RulesS, int viewToApply)
#else  /* __STDC__ */
void                RemoveSpecPresTree (pElRoot, pDoc, RulesS, viewToApply)
PtrElement          pElRoot;
PtrDocument         pDoc;
RuleSet             RulesS;
int                 viewToApply;

#endif /* __STDC__ */
{
   PtrElement          pEl, pNext;

   if (!pElRoot->ElTerminal)
     {
	/* passe au premier fils */
	pEl = pElRoot->ElFirstChild;
	while (pEl != NULL)
	  {
	     pNext = pEl->ElNext;
	     /* traite le sous-arbre de ce fils */
	     RemoveSpecPresTree (pEl, pDoc, RulesS, viewToApply);
	     /* passe au fils suivant */
	     pEl = pNext;
	  }
     }
   /* traite la racine du sous-arbre */
   RemoveSpecifPres (pElRoot, pDoc, RulesS, viewToApply);
}

/*----------------------------------------------------------------------
   TtaRemovePRule

   Removes a presentation rule from an element and release that rule.

   Parameters:
   element: the element with which the presentation rule is associated.
   pRule: the presentation rule to be removed.
   document: the document to which the element belongs.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRemovePRule (Element element, PRule pRule, Document document)
#else  /* __STDC__ */
void                TtaRemovePRule (element, pRule, document)
Element             element;
PRule               pRule;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres, pPreviousPres;

   UserErrorCode = 0;
   if (element == NULL || pRule == NULL)
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
       pPres = ((PtrElement) element)->ElFirstPRule;
       pPreviousPres = NULL;
       while (pPres != NULL && pPres != (PtrPRule) pRule)
	 {
	   pPreviousPres = pPres;
	   pPres = pPres->PrNextPRule;
	 }
       if (pPres != (PtrPRule) pRule)
	 /* This element does not own a presentation rule */
	 TtaError (ERR_invalid_parameter);
       else
	 {
	   if (pPreviousPres == NULL)
	     ((PtrElement) element)->ElFirstPRule = pPres->PrNextPRule;
	   else
	     pPreviousPres->PrNextPRule = pPres->PrNextPRule;
	   pPres->PrNextPRule = NULL;
#ifndef NODISPLAY
	   if (pPres->PrType == PtFunction)
	      ApplyASpecificStyleRule (pPres, (PtrElement)element,
				  LoadedDocument[document - 1], TRUE);
	   else
	      RedisplayDefaultPresentation (document, (PtrElement) element,
			pPres->PrType, pPres->PrPresFunction,pPres->PrViewNum);
#endif
	   FreePresentRule (pPres);
	 }
     }
}

/*----------------------------------------------------------------------
   TtaNextPRule

   Returns the first presentation rule associated with a given
   element (if pRule is NULL) or the presentation rule that
   follows a given rule of a given element.

   Parameters:
   element: the element of interest.
   pRule: a presentation rule of that element, or NULL
   if the first rule is asked.

   Return parameter:
   pRule: the next presentation rule, or NULL if
   pRule is the last rule of the element.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaNextPRule (Element element, PRule * pRule)

#else  /* __STDC__ */
void                TtaNextPRule (element, pRule)
Element             element;
PRule              *pRule;

#endif /* __STDC__ */

{
   PtrPRule            nextPRule;

   UserErrorCode = 0;
   nextPRule = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	if (*pRule == NULL)
	   nextPRule = ((PtrElement) element)->ElFirstPRule;
	else
	   nextPRule = ((PtrPRule) (*pRule))->PrNextPRule;
     }
   *pRule = (PRule) nextPRule;
}
