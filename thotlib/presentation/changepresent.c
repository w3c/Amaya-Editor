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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "abspictures_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "createabsbox_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "boxpositions_f.h"
#include "presrules_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "applicationapi_f.h"
#include "unstructchange_f.h"
#include "appli_f.h"
#include "tree_f.h"

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
   boolean             stop;
   PtrAbstractBox      pAbb, pAbbF;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

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
#ifdef __COLPAGE__
			  if (ApplyRule (*pPRule, pSPR, pAbb, pDoc, pAttr, &bool))
#else  /* __COLPAGE__ */
			  if (ApplyRule (*pPRule, pSPR, pAbb, pDoc, pAttr))
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
					 if (ApplyRule (*pPRule, pSPR, pAbbF, pDoc, pAttr, &bool))
#else  /* __COLPAGE__ */
					 if (ApplyRule (*pPRule, pSPR, pAbbF, pDoc, pAttr))
#endif /* __COLPAGE__ */
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
   boolean             stop;
   PtrPSchema          pSchP;
   PtrAttribute        pAttrib;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

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
#ifdef __COLPAGE__
		     if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else  /* __COLPAGE__ */
		     if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
		   if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else  /* __COLPAGE__ */
		   if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
		      if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else  /* __COLPAGE__ */
		      if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
		      if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else  /* __COLPAGE__ */
		      if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
			if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else  /* __COLPAGE__ */
			if (ApplyRule (pPRule, pSchP, pAbbCur, pDoc, pAttrib))
#endif /* __COLPAGE__ */
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
static int          NumTypePRuleAPI (PtrPRule pRule)
#else  /* __STDC__ */
static int          NumTypePRuleAPI (pRule)
PRuleType           pRule;

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


/*----------------------------------------------------------------------
   	PRuleMessagePre	On veut ajouter ou modifier (selon isNew) la	
   	regle de presentation specifique pPRule a l'element pEl du	
   	document pDoc. On envoie le message APP correspondant a		
   	l'application et on retourne la reponse de l'application.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      PRuleMessagePre (PtrElement pEl, PtrPRule pPRule, PtrDocument pDoc, boolean isNew)
#else  /* __STDC__ */
static boolean      PRuleMessagePre (pEl, pPRule, pDoc, isNew)
PtrElement          pEl;
PtrPRule            pPRule;
PtrDocument         pDoc;
boolean             isNew;

#endif /* __STDC__ */
{
   NotifyPresentation  notifyPres;
   PtrPRule            pR, pRPrec;
   boolean             noApply;

   if (isNew)
     notifyPres.event = TtePRuleCreate;
   else
     notifyPres.event = TtePRuleModify;
   notifyPres.pRule = (PRule) pPRule;
   notifyPres.document = (Document) IdentDocument (pDoc);
   notifyPres.element = (Element) pEl;
   notifyPres.pRuleType = NumTypePRuleAPI (pPRule);
   noApply = CallEventType ((NotifyEvent *) & notifyPres, TRUE);
   if (noApply)
      /* l'application demande a l'editeur de ne rien faire */
      if (isNew)
	 /* supprime la regle de presentation specifique ajoutee */
	 /* par SearchPresRule */
	{
	   pR = pEl->ElFirstPRule;
	   pRPrec = NULL;
	   /* parcourt les regles de presentation specifiques de l'element */
	   while (pR != NULL)
	      if (pR != pPRule)
		 /* ce n'est pas la regle cherchee, passe a la suivante */
		{
		   pRPrec = pR;
		   pR = pR->PrNextPRule;
		}
	      else
		 /* c'est la regle a supprimer */
		{
		   if (pRPrec == NULL)
		      pEl->ElFirstPRule = pPRule->PrNextPRule;
		   else
		      pRPrec->PrNextPRule = pPRule->PrNextPRule;
		   /* libere la regle */
		   FreePresentRule (pPRule);
		   pR = NULL;
		}
	}
   return noApply;
}

/*----------------------------------------------------------------------
  PRuleMessagePost      On vient d'ajouter ou de modifier (selon isNew)
  la regle de presentation specifique pPRule pour l'element pEl du document
  pDoc. On envoie le message APP correspondant a l'application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PRuleMessagePost (PtrElement pEl, PtrPRule pPRule, PtrDocument pDoc, boolean isNew)

#else  /* __STDC__ */
static void         PRuleMessagePost (pEl, pPRule, pDoc, isNew)
PtrElement          pEl;
PtrPRule            pPRule;
PtrDocument         pDoc;
boolean             isNew;

#endif /* __STDC__ */

{
   NotifyPresentation  notifyPres;

   if (isNew)
      notifyPres.event = TtePRuleCreate;
   else
      notifyPres.event = TtePRuleModify;
   notifyPres.pRule = (PRule) pPRule;
   notifyPres.document = (Document) IdentDocument (pDoc);
   notifyPres.element = (Element) pEl;
   notifyPres.pRuleType = NumTypePRuleAPI (pPRule);
   CallEventType ((NotifyEvent *) & notifyPres, FALSE);
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
   unsigned char      *s1;
   int                 num;

   s1 = RuleS1;
   for (num = SETSIZE; --num >= 0;)
      *s1++ = 0;
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   	BothHaveNoSpecRules retourne Vrai si aucun des deux elements pointes par	
   		pEl1 et pEl2 possedent des regles de presentation	
   		specifique.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
boolean             BothHaveNoSpecRules (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   boolean             equal;

   equal = FALSE;
   /*on peut faire mieux... */
   if (pEl1->ElFirstPRule == NULL && pEl2->ElFirstPRule == NULL)
      equal = TRUE;
   return equal;
}
#endif /* WIN_PRINT */

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
   boolean             stop;

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

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   	NewPosition est appele' par le Mediateur, lorsque		
   		l'utilisateur deplace une boite a l'ecran.		
   		pAb est le pave deplace' et deltaX et deltaY		
   		representent l'amplitude du deplacement en pixels	
   		frame indique la fenetre.				
   		display indique s'il faut reafficher ou simplement		
   		recalculer l'image.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewPosition (PtrAbstractBox pAb, int deltaX, int deltaY, int frame, boolean display)
#else  /* __STDC__ */
void                NewPosition (pAb, deltaX, deltaY, frame, display)
PtrAbstractBox      pAb;
int                 deltaX;
int                 deltaY;
int                 frame;
boolean             display;

#endif /* __STDC__ */
{
   PtrPRule            pPRule, pR, pRStd;
   PtrPSchema          pSPR;
   PtrSSchema          pSSR;
   PtrAttribute        pAttr;
   PtrDocument         pDoc;
   PtrElement          pEl;
   PtrAbstractBox      pAbbCur;
   PosRule            *pRe1;
   NotifyAttribute     notifyAttr;
   int                 x, y;
   int                 updateframe[MAX_VIEW_DOC];
   int                 viewSch;
   int                 view;
   int                 value;
   boolean             bValue;
   boolean             attr, stop, doit;
   boolean             isNew, reDisp, isLined;
#ifdef __COLPAGE__
   boolean             bool;
#endif /* __COLPAGE__ */

   /* nettoie la table des frames a reafficher */
   for (view = 1; view <= MAX_VIEW_DOC; view++)
      updateframe[view - 1] = 0;
   /* rien a reafficher */
   reDisp = FALSE;
   /* l'element auquel correspond le pave */
   pEl = pAb->AbElement;
   /* le document auquel il appartient */
   pDoc = DocumentOfElement (pEl);
   /* numero de cette view */
   viewSch = AppliedView (pEl, NULL, pDoc, pAb->AbDocView);
   /* le pave est-il dans une mise en lignes ? a priori non */
   isLined = FALSE;
   doit = FALSE;
   pAbbCur = pAb->AbEnclosing;
   /* on examine les paves englobants */
   while (!isLined && pAbbCur != NULL)
     {
       if (pAbbCur->AbLeafType == LtCompound && pAbbCur->AbInLine)
	 /* on est dans un pave mis en lignes */
	 isLined = TRUE;
       else if (!pAbbCur->AbAcceptLineBreak)
	 /* dans un pave insecable, inutile d'examiner les paves englobants */
	 pAbbCur = NULL;
       else
	 /* passe au pave englobant */
	 pAbbCur = pAbbCur->AbEnclosing;
     }
   
   /* traite la position verticale */
   if (deltaY != 0)
     {
       /* cherche d'abord la regle de position qui s'applique a l'element */
       pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtVertPos, FnAny, FALSE, TRUE, &pAttr);
       /* on ne decale pas les paves qui ont une position flottante ou qui */
       /* sont dans une mise en ligne */
       if (pRStd->PrPosRule.PoPosDef != NoEdge
	   && pAb->AbVertPos.PosAbRef != NULL
	   && !isLined)
	 {
	   if (pRStd->PrPosRule.PoDistUnit == UnPercent)
	     {
	       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		 GetSizesFrame (frame, &x, &y);
	       else
		 y = pAb->AbEnclosing->AbBox->BxHeight;
	       deltaY = LogicalValue (deltaY, UnPercent, (PtrAbstractBox) y);
	     }
	   else if (pRStd->PrPosRule.PoDistUnit != UnPixel)
	     deltaY = LogicalValue (deltaY, pRStd->PrPosRule.PoDistUnit, pAb);
	   /* cherche si la position verticale de l'element est determinee */
	   /* par un attribut auquel est associee l'exception NewVPos */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewVPos, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle position sera rangee dans l'attribut */
	       attr = TRUE;
	   doit = TRUE;
	   if (attr)
	     {
	       pPRule = pRStd;
	       if (pAttr->AeAttrType == AtNumAttr)
		 /* modifier la valeur de l'attribut */
		 {
		   notifyAttr.event = TteAttrModify;
		   notifyAttr.document = (Document) IdentDocument (pDoc);
		   notifyAttr.element = (Element) pEl;
		   notifyAttr.attribute = (Attribute) pAttr;
		   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
		   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
		   if (CallEventAttribute (&notifyAttr, TRUE))
		     doit = FALSE;
		   else
		     {
		       pAttr->AeAttrValue += deltaY;
		       /* fait reafficher les variables de presentation utilisant */
		       /* l'attribut */
		       RedisplayAttribute (pAttr, pEl, pDoc);
		       if (display)
			 /* la nouvelle valeur de l'attribut doit etre prise en */
			 /* compte dans les copies-inclusions de l'element */
			 RedisplayCopies (pEl, pDoc, TRUE);
		     }
		 }
	     }
	   else
	     /* la nouvelle hauteur doit etre rangee dans une regle */
	     /* de presentation specifique */
	     {
	       /* cherche si l'element possede deja une regle de position */
	       /* verticale specifique */
	       pPRule = SearchPresRule (pEl, PtVertPos, 0, &isNew, pDoc, pAb->AbDocView);
	       
	       if (isNew)
		 /* l'element n'avait pas de regle de position */
		 /* verticale specifique */
		 {
		   /* conserve le chainage */
		   pR = pPRule->PrNextPRule;
		   /* recopie la regle standard */
		   *pPRule = *pRStd;
		   pPRule->PrCond = NULL;
		   /* restaure le chainage */
		   pPRule->PrNextPRule = pR;
		   pPRule->PrViewNum = viewSch;
		   /* si la regle copiee est associee a un attribut, */
		   /* garde le lien avec cet attribut */
		   if (pAttr != NULL)
		     {
		       pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		       pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		     }
		 }
	       pRe1 = &pPRule->PrPosRule;
	       bValue = pRe1->PoDistAttr;
	       value = pRe1->PoDistance;
	       if (pRe1->PoDistAttr)
		 /* la distance est la valeur d'un attribut */
		 pRe1->PoDistance = AttrValue (pAttr);
	       pRe1->PoDistAttr = FALSE;
	       /* modifie la distance dans la regle specifique */
	       pRe1->PoDistance += deltaY;
	       
	       /* envoie un message APP a l'application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 {
		   /* reset previous values */
		   pRe1->PoDistAttr = bValue;
		   pRe1->PoDistance = value;
		 }
	     }
	   
	   if (doit)
	     {
	       pDoc->DocModified = TRUE;
	       /* le document est modifie' */
	       for (view = 1; view <= MAX_VIEW_DOC; view++)
		 if (pEl->ElAbstractBox[view - 1] != NULL)
		   /* l'element traite' a un pave dans cette view */
		   if (pDoc->DocView[view - 1].DvSSchema ==
		       pDoc->DocView[pAb->AbDocView - 1].DvSSchema
		       && pDoc->DocView[view - 1].DvPSchemaView ==
		       pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView)
		     /* c'est une view de meme type que la view traitee, on */
		     /* traite le pave de l'element dans cette view */
		     {
		       pAbbCur = pEl->ElAbstractBox[view - 1];
		       /* saute les paves de presentation */
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
			 /* applique la nouvelle regle de position verticale */
#ifdef __COLPAGE__
			 ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
		         ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr);
#endif /* __COLPAGE__ */

			 pAbbCur->AbVertPosChange = TRUE;
			 /* la position vert.du pave a change' */
			 RedispAbsBox (pAbbCur, pDoc);
			 reDisp = TRUE;
			 /* il faut reafficher le pave */
			 if (!AssocView (pEl))
			   updateframe[view - 1] = pDoc->DocViewFrame[view - 1];
			 else
			   updateframe[view - 1] = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
		     }
	       if (attr)
		 CallEventAttribute (&notifyAttr, FALSE);
	       else
		 PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	     }
	 }
     }

   /* traite la position horizontale */
   if (deltaX != 0)
     /* cherche d'abord la regle de position qui s'applique a l'element */
     {
       pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtHorizPos, FnAny, FALSE, TRUE, &pAttr);
       /* on ne decale pas les paves qui ont une position flottante ou qui */
       /* sont mis en lignes */
       if (pRStd->PrPosRule.PoPosDef != NoEdge
	   && pAb->AbHorizPos.PosAbRef != NULL
	   && !isLined)
	 {
	   if (pRStd->PrPosRule.PoDistUnit == UnPercent)
	     {
	       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		 GetSizesFrame (frame, &x, &y);
	       else
		 x = pAb->AbEnclosing->AbBox->BxWidth;
	       deltaX = LogicalValue (deltaX, UnPercent, (PtrAbstractBox) x);
	     }
	   else if (pRStd->PrPosRule.PoDistUnit != UnPixel)
	     deltaX = LogicalValue (deltaX, pRStd->PrPosRule.PoDistUnit, pAb);
	   /* cherche si la position horizontale de l'element est determinee */
	   /* par un attribut auquel est associee l'exception NewHPos */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewHPos, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle position sera rangee dans l'attribut */
	       attr = TRUE;
	   doit = TRUE;
	   if (attr)
	     {
	       pPRule = pRStd;
	       if (pAttr->AeAttrType == AtNumAttr)
		 /* modifier la valeur de l'attribut */
		 {
		   notifyAttr.event = TteAttrModify;
		   notifyAttr.document = (Document) IdentDocument (pDoc);
		   notifyAttr.element = (Element) pEl;
		   notifyAttr.attribute = (Attribute) pAttr;
		   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
		   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
		   if (CallEventAttribute (&notifyAttr, TRUE))
		     doit = FALSE;
		   else
		     {
		       pAttr->AeAttrValue += deltaX;
		       /* fait reafficher les variables de presentation utilisant */
		       /* l'attribut */
		       RedisplayAttribute (pAttr, pEl, pDoc);
		       if (display)
			 /* la nouvelle valeur de l'attribut doit etre prise en */
			 /* compte dans les copies-inclusions de l'element */
			 RedisplayCopies (pEl, pDoc, TRUE);
		     }
		 }
	     }
	   else
	     /* la nouvelle hauteur doit etre rangee dans une regle */
	     /* de presentation specifique */
	     {
	       /* cherche si l'element possede deja une regle de position */
	       /* horizontale specifique */
	       pPRule = SearchPresRule (pEl, PtHorizPos, 0, &isNew, pDoc, pAb->AbDocView);
	       if (isNew)
		 /* on a cree' une regle de position horizontale pour l'element */
		 {
		   pR = pPRule->PrNextPRule;
		   /* recopie la regle standard */
		   *pPRule = *pRStd;
		   pPRule->PrCond = NULL;
		   pPRule->PrNextPRule = pR;
		   pPRule->PrViewNum = viewSch;
		   /* si la regle copiee est associee a un attribut, */
		   /* garde le lien avec cet attribut */
		   if (pAttr != NULL)
		     {
		       pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		       pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		     }
		 }
	       pRe1 = &pPRule->PrPosRule;
	       bValue = pRe1->PoDistAttr;
	       value = pRe1->PoDistance;
	       if (pRe1->PoDistAttr)
		 /* la distance est la valeur d'un attribut */
		 pRe1->PoDistance = AttrValue (pAttr);
	       pRe1->PoDistAttr = FALSE;
	       /* change la distance dans la regle specifique */
	       pRe1->PoDistance += deltaX;
	       
	       /* envoie un message APP a l'application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 {
		   /* reset previous values */
		   pRe1->PoDistAttr = bValue;
		   pRe1->PoDistance = value;
		 }
	     }
	   if (doit)
	     {
	       pDoc->DocModified = TRUE;	/* le document est modifie' */
	       for (view = 1; view <= MAX_VIEW_DOC; view++)
		 if (pEl->ElAbstractBox[view - 1] != NULL)
		   /* l'element traite' a un pave dans cette view */
		   if ((pDoc->DocView[view - 1].DvSSchema ==
			pDoc->DocView[pAb->AbDocView - 1].DvSSchema)
		       && (pDoc->DocView[view - 1].DvPSchemaView ==
			   pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView))
		     /* c'est une view de meme type que la view traitee, on */
		     /* traite le pave de l'element dans cette view */
		     {
		       pAbbCur = pEl->ElAbstractBox[view - 1];
		       /* saute les paves de presentation */
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
			 /* applique la nouvelle regle de position verticale */
#ifdef __COLPAGE__
			 ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
		         ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr);
#endif /* __COLPAGE__ */
			 pAbbCur->AbHorizPosChange = TRUE;
			 /* indique le pave a reafficher */
			 RedispAbsBox (pAbbCur, pDoc);
			 /* il faut reafficher le pave */
			 reDisp = TRUE;
			 if (!AssocView (pEl))
			   updateframe[view - 1] = pDoc->DocViewFrame[view - 1];
			 else
			   updateframe[view - 1] = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
		     }
	       if (attr)
		 CallEventAttribute (&notifyAttr, FALSE);
	       else
		 PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	     }
	 }
     }

   if (reDisp)
     {
       if (display)
	 {
	   for (view = 1; view <= MAX_VIEW_DOC; view++)
	     if (updateframe[view - 1] > 0)
	       /* eteint la selection dans la view traitee */
	       SwitchSelection (updateframe[view - 1], FALSE);
	   /* met a jour l'image abstraite */
	   AbstractImageUpdated (pDoc);
	   /* fait reafficher ce qui doit l'etre */
	   RedisplayDocViews (pDoc);
	   for (view = 1; view <= MAX_VIEW_DOC; view++)
	     if (updateframe[view - 1] > 0)
	       /* rallume la selection dans la view traitee */
	       SwitchSelection (updateframe[view - 1], TRUE);
	 }
     }
}
#endif /* WIN_PRINT */

/*----------------------------------------------------------------------
  NewDimension est appele par le Mediateur, lorsque l'utilisateur
  deforme une boite a l'ecran. pAb est le pave deforme'	
  width et height donnent les nouvelles dimensions en pixels.
  frame indique la fenetre.				
  display indique s'il faut reafficher ou simplement recalculer l'image.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewDimension (PtrAbstractBox pAb, int width, int height, int frame, boolean display)
#else  /* __STDC__ */
void                NewDimension (pAb, width, height, frame, display)
PtrAbstractBox      pAb;
int                 width;
int                 height;
int                 frame;
boolean             display;
#endif /* __STDC__ */
{
   PtrPRule            pPRule, pR, pRStd;
   PtrPSchema          pSPR;
   PtrSSchema          pSSR;
   PtrAttribute        pAttr;
   PtrDocument         pDoc;
   PtrElement          pEl;
   PtrAbstractBox      pAbbCur;
   NotifyAttribute     notifyAttr;
   Document            doc;
   DisplayMode         oldDisplayMode;
   int                 x, y;
   int                 heightRef, widthRef;
   int                 updateframe[MAX_VIEW_DOC];
   int                 viewSch;
   int                 view;
   int                 value;
   boolean             bValue, bAbs;
   boolean             attr, stop, doit;
   boolean             isNew, reDisp, ok;
#ifdef __COLPAGE__
   boolean             bool;
#endif /* __COLPAGE__ */

   /* nettoie la table des frames a reafficher */
   for (view = 0; view < MAX_VIEW_DOC; view++)
     updateframe[view] = 0;
   /* rien a reafficher */
   reDisp = FALSE;
   /* l'element auquel correspond le pave */
   pEl = pAb->AbElement;

   /* le document auquel appartient le pave */
   pDoc = DocumentOfElement (pEl);
   doc = (Document) IdentDocument (pDoc);
   oldDisplayMode = documentDisplayMode[doc - 1];
   if (oldDisplayMode == DisplayImmediately)
     {
       TtaSetDisplayMode (doc, DeferredDisplay);
       reDisp = TRUE;
     }

   /* numero de cette view dans le schema de presentation qui la definit */
   viewSch = AppliedView (pEl, NULL, pDoc, pAb->AbDocView);
   doit = FALSE;

   /* traite le changement de largeur */
   if (width != 0 && pAb->AbBox != NULL && width != pAb->AbBox->BxWidth)
     {
       /* cherche d'abord la regle de dimension qui s'applique a l'element */
       pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtWidth, FnAny, FALSE, TRUE, &pAttr);
       /* on ne change pas la largeur si c'est celle du contenu ou si */
       /* c'est une boite elastique.  */
       ok = TRUE;
       if (!pRStd->PrDimRule.DrPosition)
	 {
	   if (pRStd->PrDimRule.DrRelation == RlEnclosed)
	     /* largeur du contenu */
	     if (pAb->AbLeafType != LtPicture)
	       /* sauf si image */
	       ok = FALSE;
	 }
       else
	 ok = FALSE;
       
       if (ok)
	 {
	   if (pRStd->PrDimRule.DrUnit == UnPercent)
	     {
	       if (!pRStd->PrDimRule.DrAbsolute)
		 /* la largeur de la boite est un pourcentage de la largeur */
		 /* d'une autre boite */
		 widthRef = pAb->AbWidth.DimAbRef->AbBox->BxWidth;
	       else if (pAb->AbEnclosing == NULL)
		 /* la largeur de la boite est un pourcentage de la largeur */
		 /* de la boite englobante */
		 GetSizesFrame (frame, &widthRef, &heightRef);
	       else
		 /* la largeur de la boite est un pourcentage de la largeur */
		 /* de la boite englobante */
		 widthRef = pAb->AbEnclosing->AbBox->BxWidth;
	       /* calcule le isNew rapport (pourcentage) de la boite */
	       x = LogicalValue (width, UnPercent, (PtrAbstractBox) widthRef);
	     }
	   else
	     /* calcule la nouvelle largeur en unite logique */
	     x = LogicalValue (width, pRStd->PrDimRule.DrUnit, pAb);
	   
	   /* cherche si la largeur de l'element est determinee par un */
	   /* attribut auquel est associee l'exception NewWidth */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewWidth, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle largeur sera rangee dans l'attribut */
	       attr = TRUE;
	   
	   doit = TRUE;
	   if (attr)
	     {
	       pPRule = pRStd;
	       if (pAttr->AeAttrType == AtNumAttr)
		 /* modifie la valeur de l'attribut */
		 {
		   notifyAttr.event = TteAttrModify;
		   notifyAttr.document = doc;
		   notifyAttr.element = (Element) pEl;
		   notifyAttr.attribute = (Attribute) pAttr;
		   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
		   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
		   if (CallEventAttribute (&notifyAttr, TRUE))
		     doit = FALSE;
		   else
		     {
		       pAttr->AeAttrValue = x;
		       /* fait reafficher les variables de presentation */
		       /* utilisant l'attribut */
		       RedisplayAttribute (pAttr, pEl, pDoc);
		       if (display)
			 /* la nouvelle valeur de l'attribut doit etre prise en */
			 /* compte dans les copies-inclusions de l'element */
			 RedisplayCopies (pEl, pDoc, TRUE);
		     }
		 }
	     }
	   else
	     /* la nouvelle largeur doit etre rangee dans une regle */
	     /* de presentation specifique */
	     {
	       /* cherche si l'element a deja une regle de largeur specifique */
	       pPRule = SearchPresRule (pEl, PtWidth, 0, &isNew, pDoc, pAb->AbDocView);
	       if (isNew)
		 /* on a cree' une regle de largeur pour l'element */
		 {
		   pR = pPRule->PrNextPRule;
		   /* on recopie la regle standard */
		   *pPRule = *pRStd;
		   pPRule->PrCond = NULL;
		   pPRule->PrNextPRule = pR;
		   pPRule->PrViewNum = viewSch;
		   /* si la regle copiee est associee a un attribut, */
		   /* garde le lien avec cet attribut */
		   if (pAttr != NULL)
		     {
		       pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		       pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		     }
		 }

	       bValue = pPRule->PrDimRule.DrAttr;
	       bAbs = pPRule->PrDimRule.DrAbsolute;
	       value = pPRule->PrDimRule.DrValue;
	       /* change la regle specifique - dimension absolue !! */
	       pPRule->PrDimRule.DrAttr = FALSE;
	       pPRule->PrDimRule.DrAbsolute = TRUE;
	       pPRule->PrDimRule.DrValue = x;

	       /* envoie un message APP a l'application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 {
		   /* reset previous values */
		   pPRule->PrDimRule.DrAttr = bValue;
		   pPRule->PrDimRule.DrAbsolute = bAbs;
		   pPRule->PrDimRule.DrValue = value;
		 }
	     }

	   if (doit)
	     {
	       pDoc->DocModified = TRUE;	/* le document est modifie' */
	       for (view = 1; view <= MAX_VIEW_DOC; view++)
		 if (pEl->ElAbstractBox[view - 1] != NULL)
		   /* l'element traite' a un pave dans cette view */
		   if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocView[pAb->AbDocView - 1].DvSSchema
		       && pDoc->DocView[view - 1].DvPSchemaView == pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView)
		     /* c'est une view de meme type que la view traitee, on */
		     /* traite le pave de l'element dans cette view */
		     {
		       pAbbCur = pEl->ElAbstractBox[view - 1];
		       /* saute les paves de presentation */
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
			 /* applique la nouvelle regle specifique */
#ifdef __COLPAGE__
			 if (ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool))
#else  /* __COLPAGE__ */
			 if (ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr))
#endif /* __COLPAGE__ */
			   {
			     pAbbCur->AbWidthChange = TRUE;
			     /* la position vert.du pave a change' */
			     /* indique le pave a reafficher */
			     RedispAbsBox (pAbbCur, pDoc);
			     /* il faut reafficher le pave */
			     reDisp = TRUE;
			     if (!AssocView (pEl))
			       updateframe[view - 1] = pDoc->DocViewFrame[view - 1];
			     else
			       updateframe[view - 1] = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
			   }
		     }
	       if (attr)
		 CallEventAttribute (&notifyAttr, FALSE);
	       else
		 PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	     }
	 }
     }

   /* traite le changement de hauteur de la boite */
   if (height != 0 && pAb->AbBox != NULL && height != pAb->AbBox->BxHeight)
     /* cherche d'abord la regle de dimension qui s'applique a l'element */
     {
       pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtHeight, FnAny, FALSE, TRUE, &pAttr);
       /* on ne change pas la hauteur si c'est celle du contenu ou si c'est */
       /* une boite elastique. */
       ok = TRUE;
       if (!pRStd->PrDimRule.DrPosition)
	 {
	   if (pRStd->PrDimRule.DrRelation == RlEnclosed)
	     /* hauteur du contenu */
	     if (pAb->AbLeafType != LtPicture)
	       ok = FALSE;
	 }
       else
	 ok = FALSE;
       
       if (ok)
	 {
	   if (pRStd->PrDimRule.DrUnit == UnPercent)
	     {
	       if (!pRStd->PrDimRule.DrAbsolute)
		 /* la hauteur de la boite est un pourcentage de la hauteur */
		 /* d'une autre boite */
		 heightRef = pAb->AbWidth.DimAbRef->AbBox->BxHeight;
	       else if (pAb->AbEnclosing == NULL)
		 /* la hauteur de la boite est un pourcentage de la hauteur */
		 /* de la boite englobante */
		 GetSizesFrame (frame, &widthRef, &heightRef);
	       else
		 /* la largeur de la boite est un pourcentage de la largeur */
		 /* de la boite englobante */
		 heightRef = pAb->AbEnclosing->AbBox->BxHeight;
	       /* calcule le isNew rapport (pourcentage) de la boite */
	       y = LogicalValue (height, UnPercent, (PtrAbstractBox) heightRef);
	     }
	   else
	     /* calcule la nouvelle largeur en unite logique */
	     y = LogicalValue (height, pRStd->PrDimRule.DrUnit, pAb);
	   
	   /* cherche si la hauteur de l'element est determinee par un */
	   /* attribut auquel est associee l'exception NewHeight */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewHeight, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle hauteur sera rangee dans l'attribut */
	       attr = TRUE;
	   doit = TRUE;
	   if (attr)
	     {
	       pPRule = pRStd;
	       if (pAttr->AeAttrType == AtNumAttr)
		 /* modifier la valeur de l'attribut */
		 {
		   notifyAttr.event = TteAttrModify;
		   notifyAttr.document = doc;
		   notifyAttr.element = (Element) pEl;
		   notifyAttr.attribute = (Attribute) pAttr;
		   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
		   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
		   if (CallEventAttribute (&notifyAttr, TRUE))
		     doit = FALSE;
		   else
		     {
		       pAttr->AeAttrValue = y;
		       /* fait reafficher les variables de presentation */
		       /* utilisant l'attribut */
		       RedisplayAttribute (pAttr, pEl, pDoc);
		       if (display)
			 /* la nouvelle valeur de l'attribut doit etre prise en */
			 /* compte dans les copies-inclusions de l'element */
			 RedisplayCopies (pEl, pDoc, TRUE);
		     }
		 }
	     }
	   else
	     /* la nouvelle hauteur doit etre rangee dans une regle */
	     /* de presentation specifique */
	     {
	       /* cherche si l'element a deja une regle de hauteur specifique */
	       pPRule = SearchPresRule (pEl, PtHeight, 0, &isNew, pDoc, pAb->AbDocView);
	       if (isNew)
		 /* on a cree' une regle de hauteur pour l'element */
		 {
		   pR = pPRule->PrNextPRule;
		   /* on recopie la regle standard */
		   *pPRule = *pRStd;
		   pPRule->PrCond = NULL;
		   pPRule->PrNextPRule = pR;
		   pPRule->PrViewNum = viewSch;
		   /* si la regle copiee est associee a un attribut, */
		   /* garde le lien avec cet attribut */
		   if (pAttr != NULL)
		     {
		       pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		       pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		     }
		 }

	       bValue = pPRule->PrDimRule.DrAttr;
	       bAbs = pPRule->PrDimRule.DrAbsolute;
	       value = pPRule->PrDimRule.DrValue;
	       /* change la regle specifique - dimension absolue !! */
	       pPRule->PrDimRule.DrAttr = FALSE;
	       pPRule->PrDimRule.DrAbsolute = TRUE;
	       pPRule->PrDimRule.DrValue = y;

	       /* envoie un message APP a l'application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 {
		   /* reset previous values */
		   pPRule->PrDimRule.DrAttr = bValue;
		   pPRule->PrDimRule.DrAbsolute = bAbs;
		   pPRule->PrDimRule.DrValue = value;
		 }
	     }
	   
	   if (doit)
	     {
	       pDoc->DocModified = TRUE;	/* le document est modifie' */
	       for (view = 1; view <= MAX_VIEW_DOC; view++)
		 if (pEl->ElAbstractBox[view - 1] != NULL)
		   /* l'element traite' a un pave dans cette view */
		   if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocView[pAb->AbDocView - 1].DvSSchema
		       && pDoc->DocView[view - 1].DvPSchemaView == pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView)
		     /* c'est une view de meme type que la view traitee, on */
		     /* traite le pave de l'element dans cette view */
		     {
		       pAbbCur = pEl->ElAbstractBox[view - 1];
		       /* saute les paves de presentation */
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
			 /* applique la nouvelle regle specifique */
#ifdef __COLPAGE__
			 if (ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool))
#else  /* __COLPAGE__ */
			 if (ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr))
#endif /* __COLPAGE__ */
			   {
			     pAbbCur->AbHeightChange = TRUE;
			     RedispAbsBox (pAbbCur, pDoc);	/* indique le pave a reafficher */
			     reDisp = TRUE;	/* il faut reafficher */
			     if (!AssocView (pEl))
			       updateframe[view - 1] = pDoc->DocViewFrame[view - 1];
			     else
			       updateframe[view - 1] = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
			   }
		     }
	       if (attr)
		 CallEventAttribute (&notifyAttr, FALSE);
	       else
		 PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	     }
	 }
     }

   if (reDisp || oldDisplayMode == DisplayImmediately)
     {
       TtaSetDisplayMode (doc, DisplayImmediately);
       if (display)
	 {
	   for (view = 1; view <= MAX_VIEW_DOC; view++)
	     if (updateframe[view - 1] > 0)
	       /* eteint la selection dans la view traitee */
	       SwitchSelection (updateframe[view - 1], FALSE);

	   AbstractImageUpdated (pDoc);	/* mise a jour de l'image abstraite */
	   RedisplayDocViews (pDoc);	/* reafficher ce qu'il faut */

	   for (view = 1; view <= MAX_VIEW_DOC; view++)
	     if (updateframe[view - 1] > 0)
	       /* rallume la selection dans la view traitee */
	       SwitchSelection (updateframe[view - 1], TRUE);
	 }
     }
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
  boolean             stop;
#ifdef __COLPAGE__
  boolean             bool;
#endif /* __COLPAGE__ */

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
#ifdef __COLPAGE__
	  /* boucle sur les paves de l'element */
	  while (pAb != NULL)
	    {
#else  /* __COLPAGE__ */
	      if (pAb != NULL)
#endif /* __COLPAGE__ */
		/* applique la regle de presentation specifique a ce pave' */
#ifdef __COLPAGE__
		if (ApplyRule (pPRule, NULL, pAb, pDoc, NULL, &bool))
#else  /* __COLPAGE__ */
		  if (ApplyRule (pPRule, NULL, pAb, pDoc, NULL))
#endif /* __COLPAGE__ */
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
#ifdef __COLPAGE__
	      pAb = pAb->AbNextRepeated;
	    }
#endif /* __COLPAGE__ */
	}
}


/*----------------------------------------------------------------------
  ModifyGraphics applique a l'element pEl les modifications sur	
  les graphiques demandes par l'utilisateur.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ModifyGraphics (PtrElement pEl, PtrDocument pDoc, int viewToApply, boolean modifLineStyle, char LineStyle, boolean modifLineWeight, int LineWeight, TypeUnit LineWeightUnit, boolean modifFillPattern, int FillPattern, boolean modifColorBackground, int ColorBackground, boolean modifLineColor, int LineColor)

#else  /* __STDC__ */
void                ModifyGraphics (pEl, pDoc, viewToApply, modifLineStyle, LineStyle, modifLineWeight, LineWeight, LineWeightUnit, modifFillPattern, FillPattern, modifColorBackground, ColorBackground, modifLineColor, LineColor)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewToApply;
boolean             modifLineStyle;
char                LineStyle;
boolean             modifLineWeight;
int                 LineWeight;
TypeUnit            LineWeightUnit;
boolean             modifFillPattern;
int                 FillPattern;
boolean             modifColorBackground;
int                 ColorBackground;
boolean             modifLineColor;
int                 LineColor;

#endif /* __STDC__ */

{
  TypeUnit            unit;
  PtrPRule            pPRule, pFunctRule;
  int                 viewSch, value;
  boolean             isNew;

  /* numero de cette view */
  viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
  /* style des traits dans le graphique */
  if (modifLineStyle)
    {
      /*cherche la regle de presentation specifique 'LineStyle' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtLineStyle, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtLineStyle;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = (int) pPRule->PrChrValue;
      pPRule->PrChrValue = LineStyle;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  pDoc->DocModified = TRUE;	/* le document est modifie' */
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrChrValue = (char) value;
    }

  /* epaisseur des traits dans le graphique */
  if (modifLineWeight)
    {
      /* cherche la regle de presentation specifique 'Epaisseur Trait' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtLineWeight, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtLineWeight;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      unit = pPRule->PrMinUnit;
      pPRule->PrMinUnit = LineWeightUnit;
      pPRule->PrMinAttr = FALSE;
      value = pPRule->PrMinValue;
      pPRule->PrMinValue = LineWeight;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  pDoc->DocModified = TRUE;	/* le document est modifie' */
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	}
      else if (!isNew)
	{
	  /* reset the previous value */
	  pPRule->PrMinUnit = unit;
	  pPRule->PrMinValue = value;
	}
    }

  /* trame de remplissage */
  if (modifFillPattern)
    {
      /* cherche la regle de presentation specifique 'FillPattern' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtFillPattern, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtFillPattern;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = pPRule->PrIntValue;
      pPRule->PrIntValue = FillPattern;
      pPRule->PrAttrValue = FALSE;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  pDoc->DocModified = TRUE;	/* le document est modifie' */
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  
	  /* It the element is not a leaf in the abstract tree, create a
	     ShowBox rule for the element if there is none */
	  if (!pEl->ElTerminal)
	    {
	      pFunctRule = SearchPresRule (pEl, PtFunction, FnShowBox, &isNew,
					   pDoc, viewToApply);
	      pFunctRule->PrType = PtFunction;
	      pFunctRule->PrViewNum = viewSch;
	      pFunctRule->PrPresMode = PresFunction;
	      pFunctRule->PrPresFunction = FnShowBox;
	      pFunctRule->PrPresBoxRepeat = FALSE;
	      pFunctRule->PrNPresBoxes = 0;
	      if (isNew)
		if (!PRuleMessagePre (pEl, pFunctRule, pDoc, isNew))
		  {
		    ApplyNewRule (pDoc, pFunctRule, pEl);
		    PRuleMessagePost (pEl, pFunctRule, pDoc, isNew);
		  }
	    }
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrIntValue = value;
    }

  /* couleur de fond */
  if (modifColorBackground)
    {
      /* cherche la regle de presentation specifique 'Background' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtBackground, 0, &isNew, pDoc, viewToApply);
      pPRule->PrType = PtBackground;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = pPRule->PrIntValue;
      pPRule->PrIntValue = ColorBackground;
      pPRule->PrAttrValue = FALSE;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  /* met les choix de l'utilisateur dans cette regle */
	  pDoc->DocModified = TRUE;	/* le document est modifie' */
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  
	  /* It the element is not a leaf in the abstract tree, create a
	     ShowBox rule for the element if there is none */
	  if (!pEl->ElTerminal)
	    {
	      pFunctRule = SearchPresRule (pEl, PtFunction, FnShowBox, &isNew,
					   pDoc, viewToApply);
	      pFunctRule->PrType = PtFunction;
	      pFunctRule->PrViewNum = viewSch;
	      pFunctRule->PrPresMode = PresFunction;
	      pFunctRule->PrPresFunction = FnShowBox;
	      pFunctRule->PrPresBoxRepeat = FALSE;
	      pFunctRule->PrNPresBoxes = 0;
	      if (isNew)
		if (!PRuleMessagePre (pEl, pFunctRule, pDoc, isNew))
		  {
		    ApplyNewRule (pDoc, pFunctRule, pEl);
		    PRuleMessagePost (pEl, pFunctRule, pDoc, isNew);
		  }
	    }
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrIntValue = value;
    }

  /* couleur du trace' */
  if (modifLineColor)
    {
      /* cherche la regle de presentation specifique 'CouleurTrace' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtForeground, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtForeground;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = pPRule->PrIntValue;
      pPRule->PrIntValue = LineColor;
      pPRule->PrAttrValue = FALSE;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  pDoc->DocModified = TRUE;	/* le document est modifie' */
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrIntValue = value;
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
   int                 viewSch;
   boolean             found;

   /* type de cette view */
   viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
   pPRule = pEl->ElFirstPRule;
   pR = NULL;
   found = FALSE;
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
	   notifyPres.document = (Document) IdentDocument (pDoc);
	   notifyPres.element = (Element) pEl;
	   notifyPres.pRule = (PRule) pPRule;
	   notifyPres.pRuleType = NumTypePRuleAPI (pPRule);
	   if (!CallEventType ((NotifyEvent *) & notifyPres, TRUE))
	     {
		if (pR == NULL)
		   pEl->ElFirstPRule = pRS;
		else
		   pR->PrNextPRule = pRS;
		pDoc->DocModified = TRUE;	/* le document est modifie' */
		/* applique la regle standard de meme type que la regle courante */
		/* aux paves de l'element qui existent dans les vues de meme type */
		/* que la view active. */
		ApplyStandardRule (pEl, pDoc, pPRule->PrType, pPRule->PrPresFunction, viewSch);
		notifyPres.event = TtePRuleDelete;
		notifyPres.document = (Document) IdentDocument (pDoc);
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
  ModifyColor change la presentation specifique de la couleur	
  de fond ou de trace' (selon Background) pour tous les elements de la	
  selection courante.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ModifyColor (int colorNum, boolean Background)
#else  /* __STDC__ */
void                ModifyColor (colorNum, Background)
int                 colorNum;
boolean             Background;
#endif /* __STDC__ */
{
   PtrDocument         SelDoc;
   PtrElement          pElFirstSel, pElLastSel, pEl;
   PtrAbstractBox      pAb;
   int                 firstChar, lastChar;
   boolean             selok, modifFillPattern;
   int                 fillPatternNum;
   RuleSet             rulesS;

   /* demande quelle est la selection courante */
   selok = GetCurrentSelection (&SelDoc, &pElFirstSel, &pElLastSel, &firstChar, &lastChar);
   if (!selok)
      /* rien n'est selectionne' */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
   else if (SelDoc->DocReadOnly)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
   else if (SelDoc != NULL && SelDoc->DocSSchema != NULL)
     /* le document selectionne' n'a pas ete ferme' */
     {
	/* eteint la selection courante */
	TtaClearViewSelections ();

	/* si une chaine de caracteres complete est selectionnee, */
	/* selectionne l'element TEXTE */
	if (pElFirstSel->ElTerminal && pElFirstSel->ElLeafType == LtText &&
	    firstChar <= 1)
	  if (pElLastSel != pElFirstSel ||
	      (pElFirstSel == pElLastSel &&
	       lastChar > pElFirstSel->ElTextLength))
	    firstChar = 0;
	if (pElLastSel->ElTerminal && pElLastSel->ElLeafType == LtText &&
	    lastChar > pElLastSel->ElTextLength)
	  if (pElLastSel != pElFirstSel ||
	      (pElFirstSel == pElLastSel && firstChar == 0))
	    lastChar = 0;

	/* Coupe les elements du debut et de la fin de la selection */
	/* s'ils sont partiellement selectionnes */
	if (firstChar > 1 || lastChar > 0)
	   IsolateSelection (SelDoc, &pElFirstSel, &pElLastSel, &firstChar,
			     &lastChar, TRUE);

	if (colorNum < 0)
	   /* standard color */
	   {
	   /* set selection to the highest level elements having the same
	      content */
	     if (ThotLocalActions[T_selectsiblings] != NULL)
	       (*ThotLocalActions[T_selectsiblings]) (&pElFirstSel, &pElLastSel, &firstChar, &lastChar);
	     if (firstChar == 0 && lastChar == 0)
	       if (pElFirstSel->ElPrevious == NULL && pElLastSel->ElNext == NULL)
		 if (pElFirstSel->ElParent != NULL &&
		     pElFirstSel->ElParent == pElLastSel->ElParent)
	           {
		     pElFirstSel = pElFirstSel->ElParent;
		     while (pElFirstSel->ElPrevious == NULL &&
			    pElFirstSel->ElNext == NULL &&
			    pElFirstSel->ElParent != NULL)
		       pElFirstSel = pElFirstSel->ElParent;
		     pElLastSel = pElFirstSel;
	           }
	   }

	/* parcourt les elements selectionnes */
	pEl = pElFirstSel;
	while (pEl != NULL)
	  {
	     /* on saute les elements qui sont des copies */
	     if (!pEl->ElIsCopy)
		/* on saute les elements non modifiables */
		if (!ElementIsReadOnly (pEl))
		   /* on saute les marques de page */
		   if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
		     {
			modifFillPattern = FALSE;
			fillPatternNum = 0;
			if (Background)
			   /* on change la couleur de fond avec la souris */
			  {
			     pAb = AbsBoxOfEl (pEl, SelectedView);
			     if (pAb != NULL)
				if (pAb->AbFillPattern < 2)
				   /* on force la trame backgroundcolor si la
				      trame du pave */
				   /* est nopattern ou foregroundcolor */
				  {
				     modifFillPattern = TRUE;
				     fillPatternNum = 2;
				  }
			  }
			if (colorNum == -1)
			  {
			     /* Couleur standard */
			     RuleSetClr (rulesS);
			     if (Background)
			       {
				  RuleSetPut (rulesS, PtFillPattern);
				  RuleSetPut (rulesS, PtBackground);
				  RuleSetPut (rulesS, PtFunction);
			       }
			     else
				RuleSetPut (rulesS, PtForeground);
			     RemoveSpecPresTree (pEl, SelDoc, rulesS, SelectedView);
			  }
			else
			   ModifyGraphics (pEl, SelDoc, SelectedView, FALSE,
					   ' ', FALSE, 0, FALSE,
					   modifFillPattern, fillPatternNum,
					   Background, colorNum, !Background,
					   colorNum);
			/* si on est dans un element copie' par inclusion,   */
			/* on met a jour les copies de cet element. */
			RedisplayCopies (pEl, SelDoc, TRUE);
		     }
	     /* cherche l'element a traiter ensuite */
	     pEl = NextInSelection (pEl, pElLastSel);
	  }
	/* tente de fusionner les elements voisins et reaffiche les paves */
	/* modifie's et la selection */
	MergeAndSelect (SelDoc, pElFirstSel, pElLastSel, firstChar, lastChar);
     }
}


/*----------------------------------------------------------------------
  ModifyChar applique a l'element pEl les modifications sur	
  les caracteres demandes par l'utilisateur.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ModifyChar (PtrElement pEl, PtrDocument pDoc, int viewToApply, boolean modifFamily, char family, boolean modifStyle, int charStyle, boolean modifsize, int size, boolean modifUnderline, int underline, boolean modifWeight, int weightUnderline)

#else  /* __STDC__ */
void                ModifyChar (pEl, pDoc, viewToApply, modifFamily, family, modifStyle, charStyle, modifsize, size, modifUnderline, underline, modifWeight, weightUnderline)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewToApply;
boolean             modifFamily;
char                family;
boolean             modifStyle;
int                 charStyle;
boolean             modifsize;
int                 size;
boolean             modifUnderline;
int                 underline;
boolean             modifWeight;
int                 weightUnderline;

#endif /* __STDC__ */

{
   boolean             isNew;
   PtrPRule            pPRule;
   int                 viewSch;
   int                 intValue;
   char                value;

   /* numero de cette view */
   viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
   /* applique les choix de l'utilisateur */
   /* family de polices de caracteres */
   if (modifFamily)
     {
	/* cherche la regle de presentation specifique 'Fonte' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtFont, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtFont;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	pPRule->PrChrValue = family;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
   /* charStyle de caracteres */
   if (modifStyle)
     {
	/* cherche la regle de presentation specifique 'charStyle' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtStyle, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtStyle;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	switch (charStyle)
	  {
	  case 0:
	    pPRule->PrChrValue = 'R';
	    break;
	  case 1:
	    pPRule->PrChrValue = 'B';
	    break;
	  case 2:
	    pPRule->PrChrValue = 'I';
	    break;
	  case 3:
	    pPRule->PrChrValue = 'O';	/* oblique */
	    break;
	  case 4:
	    pPRule->PrChrValue = 'G';	/* gras italique */
	    break;
	  case 5:
	    pPRule->PrChrValue = 'Q';	/* gras Oblique */
	    break;
	  default:
	    pPRule->PrChrValue = 'R';
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
   /* Corps des caracteres */
   if (modifsize)
     {
	/* cherche la regle de presentation specifique 'Corps' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtSize, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtSize;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	pPRule->PrMinUnit = UnPoint;
	pPRule->PrMinAttr = FALSE;
	intValue = pPRule->PrMinValue;
	pPRule->PrMinValue = size;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrMinValue = intValue;
     }

   /* Souligne' */
   if (modifUnderline)
     {
	/* cherche la regle de presentation specifique 'Souligne' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtUnderline, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtUnderline;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	switch (underline)
	  {
	  case 0:
	    pPRule->PrChrValue = 'N';	/* sans souligne */
	    break;
	  case 1:
	    pPRule->PrChrValue = 'U';	/* souligne continu */
	    break;
	  case 2:
	    pPRule->PrChrValue = 'O';	/* surligne */
	    break;
	  case 3:
	    pPRule->PrChrValue = 'C';	/* biffer */
	    break;
	  default:
	    pPRule->PrChrValue = 'N';
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
   /* Epaisseur du souligne */
   if (modifWeight)
     {
	/* cherche la regle de presentation specifique weightUnderline de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtThickness, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtThickness;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	switch (weightUnderline)
	  {
	  case 0:
	    pPRule->PrChrValue = 'N';	/* souligne mince */
	    break;
	  case 1:
	    pPRule->PrChrValue = 'T';	/* souligne epais */
	    break;
	  default:
	    pPRule->PrChrValue = 'N';
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
}


/*----------------------------------------------------------------------
   	ModifyLining applique a l'element pEl les modifications		
   		sur la mise en ligne demandes par l'utilisateur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ModifyLining (PtrElement pEl, PtrDocument pDoc, int viewToApply, boolean modifAdjust, int Adjust, boolean modifJustif, boolean Justif, boolean modifIndent, int ValIndent, boolean modifLineSpacing, int LineSpacing, boolean modifHyphen, boolean Hyphenate)
#else  /* __STDC__ */
void                ModifyLining (pEl, pDoc, viewToApply, modifAdjust, Adjust, modifJustif, Justif, modifIndent, ValIndent, modifLineSpacing, LineSpacing, modifHyphen, Hyphenate)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewToApply;
boolean             modifAdjust;
int                 Adjust;
boolean             modifJustif;
boolean             Justif;
boolean             modifIndent;
int                 ValIndent;
boolean             modifLineSpacing;
int                 LineSpacing;
boolean             modifHyphen;
boolean             Hyphenate;

#endif /* __STDC__ */
{
   boolean             isNew;
   PtrPRule            pPRule;
   BAlignment          value;
   int                 viewSch;
   int                 intValue;
   boolean             bValue;
   viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);	/* Le type de cette view */
   /* applique les choix de l'utilisateur */
   if (modifAdjust && Adjust > 0)
     {
	pPRule = SearchPresRule (pEl, PtAdjust, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtAdjust;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrAdjust;
	switch (Adjust)
	  {
	  case 1:
	    pPRule->PrAdjust = AlignLeft;
	    break;
	  case 2:
	    pPRule->PrAdjust = AlignRight;
	    break;
	  case 3:
	    pPRule->PrAdjust = AlignCenter;
	    break;
	  case 4:
	    pPRule->PrAdjust = AlignLeftDots;
	    break;
	  default:
	    pPRule->PrAdjust = AlignLeft;
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrAdjust = value;
     }
   /* Justification */
   if (modifJustif)
     {
	pPRule = SearchPresRule (pEl, PtJustify, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtJustify;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	bValue = pPRule->PrJustify;
	pPRule->PrJustify = Justif;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrJustify = bValue;
     }
   /* Coupure des mots */
   if (modifHyphen)
     {
	pPRule = SearchPresRule (pEl, PtHyphenate, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtHyphenate;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	bValue = pPRule->PrJustify;
	pPRule->PrJustify = Hyphenate;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrJustify = bValue;
     }
   /* Renfoncement de la 1ere ligne */
   if (modifIndent)
     {
	pPRule = SearchPresRule (pEl, PtIndent, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtIndent;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	pPRule->PrMinUnit = UnPoint;
	pPRule->PrMinAttr = FALSE;
	intValue = pPRule->PrMinValue;
	pPRule->PrMinValue = ValIndent;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;
	     /* le document est modifie' */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrMinValue = intValue;
     }
   /* Interligne */
   if (modifLineSpacing)
     {
	pPRule = SearchPresRule (pEl, PtLineSpacing, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtLineSpacing;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	pPRule->PrMinUnit = UnPoint;
	pPRule->PrMinAttr = FALSE;
	intValue = pPRule->PrMinValue;
	pPRule->PrMinValue = LineSpacing;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     pDoc->DocModified = TRUE;	/* le document est modifie' */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrMinValue = intValue;
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
#ifdef __COLPAGE__
  boolean           bool;
#endif /* __COLPAGE__ */

  if (pRP == NULL)
     return;

#ifdef __COLPAGE__
  ApplyRule (pRP, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
  ApplyRule (pRP, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
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
  boolean             stop;
  int                 view;

#ifdef __COLPAGE__
  boolean             bool;

#endif /* __COLPAGE__ */
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
  ApplyPRules applies a set of PRules to all abstract boxes concerned by
  the given element type or the given attribute type or the given
  presentation box.
  For each displayed abstract box check and each new presention rule
  check if it is concerned by this new pRule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               ApplyPRules (Document doc, PtrSSchema pSS, int elType, int attrType, int presBox, PtrPRule pRule, boolean remove)
#else  /* __STDC__ */
void               ApplyPRules (doc, pSS, elType, attrType, presBox, pRule, remove)
Document           doc;
PtrSSchema         pSS;
int                elType;
int                attrType;
int                presBox;
PtrPRule           pRule;
boolean            remove;
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
  boolean         selectionOK;
  boolean         found;

  pDoc = LoadedDocument[doc - 1];
  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  /* eteint la selection courante */
  TtaClearViewSelections ();
  /* do nothing if the document no longer exists */
  if (pDoc != NULL)
    /* examine all abstract boxes of elements */
    for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      /* the abstract box of the root element */
      pAb = pDoc->DocViewRootAb[view];
      /* the schema view associatde with the current view */
      viewSch = pDoc->DocView[view].DvPSchemaView;
      while (pAb != NULL)
	{
	  found = FALSE;
	  if (elType > 0)
	    /* new presentation rules are attached to an element type */
	    found = (pAb->AbElement->ElTypeNumber == elType &&
	      pAb->AbElement->ElStructSchema->SsCode == pSS->SsCode);
	  else if (attrType > 0)
	    {
	      /* new presentation rules are attached to an attribute type */
	      pAttr = pAb->AbElement->ElFirstAttr;
	      while (!found && pAttr != NULL)
		{
		  found = (pAttr->AeAttrNum == attrType && pAttr->AeAttrSSchema->SsCode == pSS->SsCode);
		  pAttr = pAttr->AeNext;
		}
	    }
	  else if (presBox > 0)
	    /* new presentation rules are attached to a presentation box */
	    found = (pAb->AbPresentationBox && pAb->AbTypeNum == presBox && pAb->AbPSchema == pSS->SsPSchema);
	  if (found)
	    {
	      /* examine each presentation rule */
	      pCurrentRule = pRule;
	      while (pCurrentRule != NULL)
		{
		  /* is the view concerned by the presentation rule ? */
		  ruleType = pCurrentRule->PrType;
		  /* note if a rule has been applied */
		  found = FALSE;
		  if (pCurrentRule->PrViewNum == 1 || pCurrentRule->PrViewNum == viewSch)
		    {
		      /* checks if the abstract box is concerned by the new rule */
		      pRP = SearchRulepAb (pDoc, pAb, &pSPR, ruleType, pCurrentRule->PrPresFunction, TRUE, &pAttr);
		      if (pRP == pCurrentRule || remove)
			{
			  /* apply a new rule */
			  found = TRUE;
			  if (remove && ruleType == PtFunction &&
			           pAb->AbLeafType == LtCompound)
			    {
			      /* remove a PtFunction rule */
			      if (pCurrentRule->PrPresFunction == FnBackgroundPicture
				  && pAb->AbPictBackground != NULL)
				{
				  TtaFreeMemory ((((PictInfo *) (pAb->AbPictBackground))->PicFileName));
				  FreePictInfo ((int *) (pAb->AbPictBackground));
				  pAb->AbPictBackground = NULL;
				}
			      else if (pCurrentRule->PrPresFunction == FnPictureMode
				       && pAb->AbPictBackground != NULL)
				((PictInfo *) (pAb->AbPictBackground))->PicPresent = FillFrame;
			      else if (pCurrentRule->PrPresFunction == FnShowBox)
				pAb->AbFillBox = FALSE;
			      pAb->AbAspectChange = TRUE;
			      pDoc->DocViewModifiedAb[view] = pAb;
			    }
			  else if (pRP != NULL)
			    ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
			}
		    }
		  pCurrentRule = pCurrentRule->PrNextPRule;
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
	    if (presBox == 0 && pAb->AbPresentationBox)
	      pAb = NextAbstractBox (pAb);
	    else if (presBox > 0 && !pAb->AbPresentationBox)
	      pAb = NextAbstractBox (pAb);
	}
    }
  /* tente de fusionner les elements voisins et reaffiche les paves */
  /* modifie's et la selection */
  if (pSelDoc != NULL && pSelDoc == pDoc)
    MergeAndSelect (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
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
