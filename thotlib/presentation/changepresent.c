
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
  modpres.c -- Modifications de la presentation specifique.
  Modifications de la presentation specifique.
  V. Quint	Juin 1987
  Major changes:
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "appaction.h"
#include "presentation.h"
#include "message.h"

#undef EXPORT
#define EXPORT extern
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"


#include "tree_f.h"
#include "attributes_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "boxpositions_f.h"
#include "presrules_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"

#ifdef __STDC__
static void ApplyInherit(PRuleType ruleType, PtrAbstractBox pAb, PtrDocument pDoc);
#else /* __STDC__ */
static void ApplyInherit();
#endif /* __STDC__ */
			    

/* ---------------------------------------------------------------------- */
/* |	ApplyRuleSubTree							| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ApplyRuleSubTree(PtrElement pE, PRuleType ruleType, PtrDocument pDoc, PtrPRule *pPRule, int view)
#else /* __STDC__ */
static void ApplyRuleSubTree(pE, ruleType, pDoc, pPRule, view)
	PtrElement pE;
	PRuleType ruleType;
	PtrDocument pDoc;
	PtrPRule *pPRule;
	int view;
#endif /* __STDC__ */
{
  boolean         stop;
  PtrAbstractBox         pAbb, pAbbF;
  PtrPSchema      pSPR;
  PtrAttribute     pAttr;
#ifdef __COLPAGE__
 boolean bool;
#endif /* __COLPAGE__ */
  
  if (pE->ElTerminal)
    pE = NULL;
  else
    pE = pE->ElFirstChild;	/* on passe au premier fils */
  while (pE != NULL)
    {
      pAbb = pE->ElAbstractBox[view - 1]; /* 1er pave de l'element dans la view */
      if (pAbb != NULL)
	if (pAbb->AbDead)
	  pAbb = NULL;	/* on ne traite pas les paves morts */
      if (pAbb == NULL)
	/* cet element n'a pas de pave, mais ses descendants en */
	/* ont peut etre... */
	ApplyRuleSubTree(pE, ruleType, pDoc, pPRule, view);
      else
	{
	  /* il y a un element descendant dont les paves peuvent heriter de pAb. 
	     On parcourt ses paves dans la view */
	  stop = FALSE;
	  while (!stop)
	    {
	      *pPRule = SearchRulepAb(pDoc, pAbb, &pSPR, ruleType, TRUE, &pAttr);
	      if (*pPRule != NULL)
		{
		  if ((*pPRule)->PrPresMode == PresInherit && ((*pPRule)->PrInheritMode == InheritParent ||
				(*pPRule)->PrInheritMode == InheritGrandFather))
		    /* la regle de ce pave herite de l'ascendant, */
		    /* on applique la regle */
#ifdef __COLPAGE__
	            if (ApplyRule(*pPRule, pSPR, pAbb, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
	            if (ApplyRule(*pPRule, pSPR, pAbb, pDoc, pAttr))
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
			RedispAbsBox(pAbb, pDoc);
			if (!pAbb->AbPresentationBox)
			  ApplyInherit(ruleType, pAbb, pDoc);
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
			  *pPRule = SearchRulepAb(pDoc, pAbbF, &pSPR, ruleType, TRUE, &pAttr);
			  if (*pPRule != NULL)
			    {
			      if ((*pPRule)->PrPresMode == PresInherit 
				  && (*pPRule)->PrInheritMode == InheritParent)
#ifdef __COLPAGE__
	            		if (ApplyRule(*pPRule, pSPR, pAbbF, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
	            		if (ApplyRule(*pPRule, pSPR, pAbbF, pDoc, pAttr))
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
				    RedispAbsBox(pAbbF, pDoc);
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


/* ---------------------------------------------------------------------- */
/* |	ApplyInherit on vient d'appliquer la regle de presentation de type	| */
/* |		ruleType au pave pAb. Verifie si les paves environnants	| */
/* |		heritent de cette regle et si oui leur applique		| */
/* |		l'heritage.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ApplyInherit(PRuleType ruleType, PtrAbstractBox pAb, PtrDocument pDoc)

#else /* __STDC__ */
static void ApplyInherit(ruleType, pAb, pDoc)
	PRuleType ruleType;
	PtrAbstractBox pAb;
	PtrDocument pDoc;
#endif /* __STDC__ */

{
  PtrElement      pEl;
  int             view;
  PtrAbstractBox         pAbbCur;
  PtrPRule    pPRule;
  boolean         stop;
  PtrPSchema      pSchP;
  PtrAttribute     pAttrib;
#ifdef __COLPAGE__
 boolean bool;
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
	  pPRule = SearchRulepAb(pDoc, pAbbCur, &pSchP, ruleType, TRUE, &pAttrib);
	  if (pPRule != NULL)
	    {
	      if (pPRule->PrPresMode == PresInherit &&
		  pPRule->PrInheritMode == InheritPrevious)
		/* la regle de cet element herite du precedent, on applique */
		/* la regle */
#ifdef __COLPAGE__
		if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
		if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib))
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
		    ApplyInherit(ruleType, pAbbCur, pDoc);
		    RedispAbsBox(pAbbCur, pDoc);
		  }
	    }
	}
      pEl = pAb->AbElement;
    }
  if (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
    /* l'element a des descendants. Ceux-ci heritent-t-il de leur */
    /* ascendant ? */
    ApplyRuleSubTree(pEl, ruleType, pDoc, &pPRule, view);
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
	  pPRule = SearchRulepAb(pDoc, pAbbCur, &pSchP, ruleType, TRUE, &pAttrib);
	  if (pPRule != NULL)
	    if (pPRule->PrPresMode == PresInherit &&
		pPRule->PrInheritMode == InheritChild)
	      /* la regle de cet element herite du descendant, on */
	      /* applique la regle */
#ifdef __COLPAGE__
	      if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	      if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib))
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
		  ApplyInherit(ruleType, pAbbCur, pDoc);
		  RedispAbsBox(pAbbCur, pDoc);
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
	    pPRule = SearchRulepAb(pDoc, pAbbCur, &pSchP, ruleType, TRUE, &pAttrib);
	    if (pPRule != NULL)
	      if (pPRule->PrPresMode == PresInherit && pPRule->PrInheritMode == InheritCreator)
		/* la regle de ce pave herite de son createur, on l'applique */
#ifdef __COLPAGE__
	        if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	        if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib))
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
		  ApplyInherit(ruleType, pAbbCur, pDoc);
		  RedispAbsBox(pAbbCur, pDoc);
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
	    pPRule = SearchRulepAb(pDoc, pAbbCur, &pSchP, ruleType, TRUE, &pAttrib);
	    if (pPRule != NULL)
	      if (pPRule->PrPresMode == PresInherit && pPRule->PrInheritMode == InheritCreator)
		/* la regle de ce pave herite de son createur, on l'applique */
#ifdef __COLPAGE__
	        if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	        if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib))
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
		  ApplyInherit(ruleType, pAbbCur, pDoc);
		  RedispAbsBox(pAbbCur, pDoc);
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
	    pPRule = SearchRulepAb(pDoc, pAbbCur, &pSchP, ruleType, TRUE, &pAttrib);
	    if (pPRule != NULL)
	      if (pPRule->PrPresMode == PresInherit && pPRule->PrInheritMode == InheritCreator)
		/* la regle de ce pave herite de son createur, on l'applique */
#ifdef __COLPAGE__
	        if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	        if (ApplyRule(pPRule, pSchP, pAbbCur, pDoc, pAttrib))
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
		  ApplyInherit(ruleType, pAbbCur, pDoc);
		  RedispAbsBox(pAbbCur, pDoc);
		}
            }
	  /* examine le pave suivant */
	  pAbbCur = pAbbCur->AbNext;
	}
    }
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int NumTypePRuleAPI(PRuleType tr)
#else /* __STDC__ */
static int NumTypePRuleAPI(tr)
	PRuleType tr;
#endif /* __STDC__ */
{
	switch (tr)
	{
	case PtVisibility:
	  return PRVisibility;
	  break;
	case PtFunction:
	  return PRFunction;
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
	default:
	  return 0;
	  break;
	}
}


/* ---------------------------------------------------------------------- */
/* |	PRuleMessagePre	On veut ajouter ou modifier (selon isNew) la	| */
/* |	regle de presentation specifique pPRule a l'element pEl du	| */
/* |	document pDoc. On envoie le message APP correspondant a		| */
/* |	l'application et on retourne la reponse de l'application.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean PRuleMessagePre(PtrElement pEl, PtrPRule pPRule, PtrDocument pDoc, boolean isNew)
#else /* __STDC__ */
static boolean PRuleMessagePre(pEl, pPRule, pDoc, isNew)
	PtrElement pEl;
	PtrPRule pPRule;
	PtrDocument pDoc;
	boolean isNew;
#endif /* __STDC__ */
{
	NotifyPresentation notifyPres;
	PtrPRule	   pR, pRPrec;
	boolean		   dontdoit;

	if (isNew)
	  {
	  notifyPres.event = TtePRuleCreate;
	  notifyPres.pRule = NULL;
	  }
	else
	  {
	  notifyPres.event = TtePRuleModify;
	  notifyPres.pRule = (PRule)pPRule;
	  }
	notifyPres.document = (Document)IdentDocument(pDoc);
	notifyPres.element = (Element)pEl;
	notifyPres.pRuleType = NumTypePRuleAPI(pPRule->PrType);
	dontdoit = CallEventType((NotifyEvent *)&notifyPres, TRUE);
	if (dontdoit)
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
		 FreePresentRule(pPRule);
		 pR = NULL;
		 }
	     }
	return dontdoit;
}

/* ---------------------------------------------------------------------- */
/* |	PRuleMessagePost      On vient d'ajouter ou de modifier (selon	| */
/* |	isNew) la regle de presentation specifique pPRule pour	| */
/* |	l'element pEl du document pDoc. On envoie le message APP	| */
/* |	correspondant a l'application.					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void PRuleMessagePost(PtrElement pEl, PtrPRule pPRule, PtrDocument pDoc, boolean isNew)

#else /* __STDC__ */
static void PRuleMessagePost(pEl, pPRule, pDoc, isNew)
	PtrElement pEl;
	PtrPRule pPRule;
	PtrDocument pDoc;
	boolean isNew;
#endif /* __STDC__ */

{
	NotifyPresentation notifyPres;

	if (isNew)
	  notifyPres.event = TtePRuleCreate;
	else
	  notifyPres.event = TtePRuleModify;
	notifyPres.pRule = (PRule)pPRule;
	notifyPres.document = (Document)IdentDocument(pDoc);
	notifyPres.element = (Element)pEl;
	notifyPres.pRuleType = NumTypePRuleAPI(pPRule->PrType);
	CallEventType((NotifyEvent *)&notifyPres, FALSE);
}

#define SETSIZE 32
/* ---------------------------------------------------------------------- */
/* |	RuleSetIn teste l'appartenance d'un element a` un ensemble.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int RuleSetIn(register int N, RuleSet S1)
#else /* __STDC__ */
int RuleSetIn(N, S1)
	register int N;
	RuleSet S1;
#endif /* __STDC__ */
{
  if (N >= 0 && N < SETSIZE*8)
    return S1[N>>3] & (1 << (N & 7));
  else 
    return 0;
}


/* ---------------------------------------------------------------------- */
/* |	RuleSetPut ajoute un element a` un ensemble.			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void RuleSetPut(RuleSet S1, int N)
#else /* __STDC__ */
void RuleSetPut(S1, N)
	RuleSet S1;
	int N;
#endif /* __STDC__ */
{
  if (N >= 0 && N < SETSIZE*8)
    S1[N>>3] |= (1 << (N & 7));
}


/* ---------------------------------------------------------------------- */
/* |	RuleSetClr met a` 0 l'ensemble S1.				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void RuleSetClr(RuleSet S1)
#else /* __STDC__ */
void RuleSetClr(S1)
	RuleSet S1;
#endif /* __STDC__ */
{
  unsigned char	*s1;
  int	N;
  
  s1 = S1;
  for (N = SETSIZE; --N >= 0; )
    *s1++ = 0;
}

/* ---------------------------------------------------------------------- */
/* |	MemesRegleSpecif retourne Vrai si les deux elements pointes par	| */
/* |		pEl1 et pEl2 possedent les memes regles de presentation	| */
/* |		specifique.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean MemesRegleSpecif(PtrElement pEl1, PtrElement pEl2)
#else /* __STDC__ */
boolean MemesRegleSpecif(pEl1, pEl2)
	PtrElement pEl1;
	PtrElement pEl2;
#endif /* __STDC__ */
{
  boolean         egal;
  
  egal = FALSE;
  /*on peut faire mieux... */
  if (pEl1->ElFirstPRule == NULL && pEl2->ElFirstPRule == NULL)
    egal = TRUE;
  return egal;
}



/* ---------------------------------------------------------------------- */
/* |	PaveDeElem retourne un pointeur sur le pave de l'element pEl	| */
/* |		qui appartient a la view view				| */
/* |		et qui n'est pas un pave de presentation.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrAbstractBox PaveDeElem(PtrElement pEl, int view)
#else /* __STDC__ */
PtrAbstractBox PaveDeElem(pEl, view)
	PtrElement pEl;
	int view;
#endif /* __STDC__ */
{
  PtrAbstractBox         pAb;
  boolean         stop;
  
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


/* ---------------------------------------------------------------------- */
/* |	PavResizable  est appele' par le Mediateur, pour savoir si la	| */
/* |		boite du pave' pAb peut etre retaillee horizontalement	| */
/* |		ou verticalement (selon Horiz).				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean  PavResizable(PtrAbstractBox pAb, boolean Horiz)
#else /* __STDC__ */
boolean  PavResizable(pAb, Horiz)
	PtrAbstractBox pAb;
	boolean Horiz;
#endif /* __STDC__ */
{
  boolean  result;
  PtrElement pEl;
  PtrDocument pDoc;
  
  pEl = pAb->AbElement;
  pDoc = DocumentOfElement(pEl); /* le document auquel appartient le pave */
  if (pDoc->DocReadOnly)
    {
      TtaDisplaySimpleMessage (INFO, LIB,RO_DOC_FORBIDDEN);
      result = FALSE;
    }
  else
    if (pEl->ElIsCopy)
      result = FALSE;
    else
      if (ElementIsReadOnly(pEl))
	result = FALSE;
      else
        if (TypeHasException(ExcNoResize, pEl->ElTypeNumber, pEl->ElStructSchema))
	  result = FALSE;
        else
	  if (Horiz)
	    result = 
	      !TypeHasException(ExcNoHResize, pEl->ElTypeNumber, pEl->ElStructSchema);
	  else
	    result = 
	      !TypeHasException(ExcNoVResize, pEl->ElTypeNumber, pEl->ElStructSchema);
  return result;
}



/* ---------------------------------------------------------------------- */
/* |	PavMovable  est appele' par le Mediateur, pour savoir si la	| */
/* |		boite du pave' pAb peut etre deplacee horizontalement	| */
/* |		ou  verticalement (selon Horiz).			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean PavMovable(PtrAbstractBox pAb, boolean Horiz)
#else /* __STDC__ */
boolean PavMovable(pAb, Horiz)
	PtrAbstractBox pAb;
	boolean Horiz;
#endif /* __STDC__ */
{
  boolean  result;
  PtrElement pEl;
  PtrDocument pDoc;
  
  pEl = pAb->AbElement;
  pDoc = DocumentOfElement(pEl); 
  if (pDoc == NULL)
     return FALSE;
  if (pDoc->DocReadOnly)
    {
      TtaDisplaySimpleMessage (INFO, LIB, RO_DOC_FORBIDDEN);
      result = FALSE;
    }
  else
    if (pEl->ElIsCopy)
      result = FALSE;
    else
      if (ElementIsReadOnly(pEl))
	result = FALSE;
      else
        if (TypeHasException(ExcNoMove, pEl->ElTypeNumber, pEl->ElStructSchema))
	  result = FALSE;
        else
	  if (Horiz)
	    result = 
	      !TypeHasException(ExcNoHMove, pEl->ElTypeNumber, pEl->ElStructSchema);
	  else
	    result = 
	      !TypeHasException(ExcNoVMove, pEl->ElTypeNumber, pEl->ElStructSchema);
  return result;
}


/* ---------------------------------------------------------------------- */
/* |	NouvPosition est appele' par le Mediateur, lorsque		| */
/* |		l'utilisateur deplace une boite a l'ecran.		| */
/* |		pAb est le pave deplace' et deltaX et deltaY		| */
/* |		representent l'amplitude du deplacement en pixels	| */
/* |		frame indique la fenetre.				| */
/* |		Disp indique s'il faut reafficher ou simplement		| */
/* |		recalculer l'image.					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void NouvPosition(PtrAbstractBox pAb, int deltaX, int deltaY, int frame, boolean Disp)
#else /* __STDC__ */
void NouvPosition(pAb, deltaX, deltaY, frame, Disp)
	PtrAbstractBox pAb;
	int deltaX;
	int deltaY;
	int frame;
	boolean Disp;
#endif /* __STDC__ */
{
  boolean         isNew, reaff, lignes;
  PtrPRule    pPRule, pR, pRStd;
  PtrPSchema      pSPR;
  PtrSSchema	  pSSR;
  PtrAttribute     pAttr;
  PtrDocument     pDoc;
  PtrElement      pEl;
  PtrAbstractBox         pAbbCur;
  int		  updateframe[MAX_VIEW_DOC];
  int             viewSch;
  boolean         attr;
  int		  view;
  boolean	  stop;
  PosRule       *pRe1;
  boolean	  doit;
  NotifyAttribute notifyAttr;
  int             x, y;
#ifdef __COLPAGE__
  boolean bool;
#endif /* __COLPAGE__ */

  /* nettoie la table des frames a reafficher */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    updateframe[view - 1] = 0;
  reaff = FALSE;  		/* rien a reafficher */
  pEl = pAb->AbElement;	/* l'element auquel correspond le pave */
  pDoc = DocumentOfElement(pEl);	/* le document auquel il appartient */ 
  /* numero de cette view */ 
  viewSch = AppliedView(pEl, NULL, pDoc, pAb->AbDocView);
  /* le pave est-il dans une mise en lignes ? */
  lignes = FALSE;	/* a priori non */
  doit = FALSE;
  pAbbCur = pAb->AbEnclosing;
  /* on examine les paves englobants */
  while (!lignes && pAbbCur != NULL)
    {
      if (pAbbCur->AbLeafType == LtCompound && pAbbCur->AbInLine)
	lignes = TRUE;	/* on est dans un pave mis en lignes */
      else if (!pAbbCur->AbAcceptLineBreak)
	pAbbCur = NULL;	/* on est dans un pave insecable, inutile */
      /* d'examiner les paves englobants */
      else	/* passe au pave englobant */
	pAbbCur = pAbbCur->AbEnclosing;
    }

  /* traite la position verticale */
  if (deltaY != 0)
    {
      /* cherche d'abord la regle de position qui s'applique a l'element */
      pRStd = GlobalSearchRulepEl(pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtVertPos, FALSE, TRUE,
		       &pAttr);
      /* on ne decale pas les paves qui ont une position flottante ou qui */
      /* sont dans une mise en ligne */
      if (pRStd->PrPosRule.PoPosDef != NoEdge 
	  && pAb->AbVertPos.PosAbRef != NULL 
	  && !lignes)
	{
	  if (pRStd->PrPosRule.PoDistUnit == UnPercent)
	    {
	      if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		GetSizesFrame(frame, &x, &y);
	      else
		y = pAb->AbEnclosing->AbBox->BxHeight;
	      deltaY = LogicalValue(deltaY, UnPercent, (PtrAbstractBox)y);
	    }
	  else if (pRStd->PrPosRule.PoDistUnit != UnPixel)
	    deltaY = LogicalValue(deltaY, pRStd->PrPosRule.PoDistUnit, pAb);
	  /* cherche si la position verticale de l'element est determinee */
	  /* par un attribut auquel est associee l'exception NewVPos */
	  attr = FALSE;
	  if (pAttr != NULL)
	    if (AttrHasException(ExcNewVPos,pAttr->AeAttrNum, pAttr->AeAttrSSchema))
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
	      notifyAttr.document = (Document)IdentDocument(pDoc);
	      notifyAttr.element = (Element)pEl;
	      notifyAttr.attribute = (Attribute)pAttr;
	      notifyAttr.attributeType.AttrSSchema = (SSchema)(pAttr->AeAttrSSchema);
	      notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
	      if (CallEventAttribute(&notifyAttr, TRUE))
		doit = FALSE;
	      else
		{
		pAttr->AeAttrValue += deltaY;
		/* fait reafficher les variables de presentation utilisant */
		/* l'attribut */
		RedisplayAttribute(pAttr, pEl, pDoc);
		if (Disp)
		   /* la nouvelle valeur de l'attribut doit etre prise en */
		   /* compte dans les copies-inclusions de l'element */
		   RedisplayCopies(pEl, pDoc, TRUE);
		}
	      }
	    } 
	  else	
	    /* la nouvelle hauteur doit etre rangee dans une regle */
	    /* de presentation specifique */
	    {
	      /* cherche si l'element possede deja une regle de position */
	      /* verticale specifique */
	      pPRule = SearchPresRule(pEl, PtVertPos, &isNew, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */

	      doit = !PRuleMessagePre(pEl, pPRule, pDoc, isNew);
	      if (doit)
	        {
	        if (isNew)
		  /* l'element n'avait pas de regle de position verticale */
		  /* specifique */
		  {
		  pR = pPRule->PrNextPRule;	/* conserve le chainage */
		  *pPRule = *pRStd;	/* recopie la regle standard */
		  pPRule->PrCond = NULL;
		  pPRule->PrNextPRule = pR;	/* restaure le chainage */
		  pPRule->PrViewNum = viewSch;
		  /* si la regle copiee est associee a un attribut, garde le */
		  /* lien avec cet attribut */
		  if (pAttr != NULL)
		    {
		      pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		      pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		    }
		  }
	        pRe1 = &pPRule->PrPosRule;
	        if (pRe1->PoDistAttr)
		  /* la distance est la valeur d'un attribut */
		  pRe1->PoDistance = AttrValue(pAttr);
	        pRe1->PoDistAttr = FALSE;
	        /* modifie la distance dans la regle specifique */
	        pRe1->PoDistance += deltaY;
		}
	    }
	  if (doit)
	    {
	    pDoc->DocModified = TRUE;
	    /* le document est modifie' */
	    for (view = 1; view <= MAX_VIEW_DOC; view++)
	      if (pEl->ElAbstractBox[view -1] != NULL)
	        /* l'element traite' a un pave dans cette view */
	        if (pDoc->DocView[view - 1].DvSSchema == 
		    pDoc->DocView[pAb->AbDocView -1].DvSSchema
		    && pDoc->DocView[view - 1].DvPSchemaView ==
		    pDoc->DocView[pAb->AbDocView -1].DvPSchemaView)
		  /* c'est une view de meme type que la view traitee, on */
		  /* traite le pave de l'element dans cette view */
		  {
		  pAbbCur = pEl->ElAbstractBox[view -1];
		  /* saute les paves de presentation */
		  stop = FALSE;
		  do
		    if (pAbbCur == NULL)
		      stop = TRUE;
		    else
		      if (!pAbbCur->AbPresentationBox)
			stop = TRUE;
		      else
			pAbbCur = pAbbCur->AbNext;
		  while (!stop);
		  if (pAbbCur != NULL)
		    /* applique la nouvelle regle de position verticale */
#ifdef __COLPAGE__
	            ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool);
#else /* __COLPAGE__ */
	            ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr);
#endif /* __COLPAGE__ */
		      {
			pAbbCur->AbVertPosChange = TRUE;
			/* la position vert.du pave a change' */
			RedispAbsBox(pAbbCur, pDoc);
			reaff = TRUE;	
			/* il faut reafficher le pave */
			if (!AssocView(pEl))
			  updateframe[view -1] = pDoc->DocViewFrame[view -1];
			else
			  updateframe[view -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
		      }
		  }
	    if (attr)
	       CallEventAttribute(&notifyAttr, FALSE);
	    else
	       PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	    }
	}
    }
  /* traite la position horizontale */
  if (deltaX != 0)
    /* cherche d'abord la regle de position qui s'applique a l'element */
    {
      pRStd = GlobalSearchRulepEl(pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtHorizPos, FALSE, TRUE, &pAttr);
      /* on ne decale pas les paves qui ont une position flottante ou qui */
      /* sont mis en lignes */
      if (pRStd->PrPosRule.PoPosDef != NoEdge 
	  && pAb->AbHorizPos.PosAbRef != NULL 
	  && !lignes)
	{
	  if (pRStd->PrPosRule.PoDistUnit == UnPercent)
	    {
	      if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		GetSizesFrame(frame, &x, &y);
	      else
		x = pAb->AbEnclosing->AbBox->BxWidth;
	      deltaX = LogicalValue(deltaX, UnPercent, (PtrAbstractBox)x);
	    }
	  else if (pRStd->PrPosRule.PoDistUnit != UnPixel)
	    deltaX = LogicalValue(deltaX, pRStd->PrPosRule.PoDistUnit, pAb); 
	  /* cherche si la position horizontale de l'element est determinee */
	  /* par un attribut auquel est associee l'exception NewHPos */
	  attr = FALSE;
	  if (pAttr != NULL)
	    if (AttrHasException(ExcNewHPos, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
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
		notifyAttr.document = (Document)IdentDocument(pDoc);
		notifyAttr.element = (Element)pEl;
		notifyAttr.attribute = (Attribute)pAttr;
		notifyAttr.attributeType.AttrSSchema = (SSchema)(pAttr->AeAttrSSchema);
		notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
		if (CallEventAttribute(&notifyAttr, TRUE))
		  doit = FALSE;
		else
		  {
		  pAttr->AeAttrValue += deltaX;
		  /* fait reafficher les variables de presentation utilisant */
		  /* l'attribut */
		  RedisplayAttribute(pAttr,pEl,pDoc);
		  if (Disp)
		     /* la nouvelle valeur de l'attribut doit etre prise en */
		     /* compte dans les copies-inclusions de l'element */
		     RedisplayCopies(pEl, pDoc, TRUE);
		  }
		}
	    } 
	  else	/* la nouvelle hauteur doit etre rangee dans une regle */
	    /* de presentation specifique */
	    {
	      /* cherche si l'element possede deja une regle de position */
	      /* horizontale specifique */
	      pPRule = SearchPresRule(pEl, PtHorizPos, &isNew, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */
	      doit = !PRuleMessagePre(pEl, pPRule, pDoc, isNew);
	      if (doit)
		{
	        if (isNew)
		  /* on a cree' une regle de position horizontale pour l'element */
		  {
		  pR = pPRule->PrNextPRule;
		  /* recopie la regle standard */
		  *pPRule = *pRStd;
		  pPRule->PrCond = NULL;
		  pPRule->PrNextPRule = pR;
		  pPRule->PrViewNum = viewSch;
		  /* si la regle copiee est associee a un attribut, garde le lien */
		  /* avec cet attribut */
		  if (pAttr != NULL)
		    {
		      pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		      pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		    }
		  }
	        pRe1 = &pPRule->PrPosRule;
	        if (pRe1->PoDistAttr)
		  /* la distance est la valeur d'un attribut */
		  pRe1->PoDistance = AttrValue(pAttr);
	        pRe1->PoDistAttr = FALSE;
	        /* change la distance dans la regle specifique */
	        pRe1->PoDistance += deltaX;
		}
	    }
	  if (doit)
	    {
	    pDoc->DocModified = TRUE;	/* le document est modifie' */
	    for (view = 1; view <= MAX_VIEW_DOC; view++)
	      if (pEl->ElAbstractBox[view -1] != NULL)
	        /* l'element traite' a un pave dans cette view */
	        if ((pDoc->DocView[view - 1].DvSSchema ==
		     pDoc->DocView[pAb->AbDocView -1].DvSSchema)
		    && (pDoc->DocView[view - 1].DvPSchemaView ==
		        pDoc->DocView[pAb->AbDocView -1].DvPSchemaView))
		  /* c'est une view de meme type que la view traitee, on */
		  /* traite le pave de l'element dans cette view */
		  {
		  pAbbCur = pEl->ElAbstractBox[view -1];
		  /* saute les paves de presentation */
		  stop = FALSE;
		  do
		    if (pAbbCur == NULL)
		      stop = TRUE;
		    else
		      if (!pAbbCur->AbPresentationBox)
			stop = TRUE;
		      else
			pAbbCur = pAbbCur->AbNext;
		  while (!stop);
		  if (pAbbCur != NULL)	
		    /* applique la nouvelle regle de position verticale */
#ifdef __COLPAGE__
	            ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool);
#else /* __COLPAGE__ */
	            ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr);
#endif /* __COLPAGE__ */
		      {
			pAbbCur->AbHorizPosChange = TRUE;
			RedispAbsBox(pAbbCur, pDoc); /* indique le pave a reafficher */
			reaff = TRUE;	/* il faut reafficher le pave */
			if (!AssocView(pEl))
			  updateframe[view -1] = pDoc->DocViewFrame[view -1];
			else
			  updateframe[view -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
		      }
		  }
	    if (attr)
	       CallEventAttribute(&notifyAttr, FALSE);
	    else
	       PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	    }
	}
    }
  if (reaff)
    {
      if (Disp)
	{
	for (view = 1; view <= MAX_VIEW_DOC; view++)
	  if (updateframe[view -1] > 0)
	    /* eteint la selection dans la view traitee */
	    SwitchSelection(updateframe[view -1], FALSE);
        AbstractImageUpdated(pDoc);	/* met a jour l'image abstraite */
        RedisplayDocViews(pDoc);	/* fait reafficher ce qui doit l'etre */
        for (view = 1; view <= MAX_VIEW_DOC; view++)
	  if (updateframe[view -1] > 0)
	    /* rallume la selection dans la view traitee */
	    SwitchSelection(updateframe[view -1], TRUE);
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	NouvDimension est appele par le Mediateur, lorsque l'utilisateur| */
/* |		deforme une boite a l'ecran. pAb est le pave deforme'	| */
/* |		et deltaX,deltaY representent l'amplitude de la		| */
/* |		deformation en pixels.					| */
/* |		frame indique la fenetre.				| */
/* |		Disp indique s'il faut reafficher ou simplement		| */
/* |		recalculer l'image.					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void NouvDimension(PtrAbstractBox pAb, int deltaX, int deltaY, int frame, boolean Disp)
#else /* __STDC__ */
void NouvDimension(pAb, deltaX, deltaY, frame, Disp)
	PtrAbstractBox pAb;
	int deltaX;
	int deltaY;
	int frame;
	boolean Disp;
#endif /* __STDC__ */
{
  boolean         isNew, reaff, ok, imagetrick;
  PtrPRule    pPRule, pR, pRStd;
  PtrPSchema      pSPR;
  PtrSSchema	  pSSR;
  PtrAttribute     pAttr;
  PtrDocument     pDoc;
  PtrElement      pEl;
  int             hauteur, largeur, hauteurRef, largeurRef;
  int             updateframe[MAX_VIEW_DOC];
  int             viewSch;
  boolean         attr;
  PtrAbstractBox         pAbbCur;
  int             view;
  boolean         stop;
  boolean	  doit;
  NotifyAttribute notifyAttr;
  int             x, y;
#ifdef __COLPAGE__
 boolean          bool;
#endif /* __COLPAGE__ */

  /* nettoie la table des frames a reafficher */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    updateframe[view - 1] = 0;
  reaff = FALSE;		/* rien a reafficher */
  pEl = pAb->AbElement;	/* l'element auquel correspond le pave */
  pDoc = DocumentOfElement(pEl);	/* le document auquel appartient le pave */ 
  /* numero de cette view dans le schema de presentation qui la definit */
  viewSch = AppliedView(pEl, NULL, pDoc, pAb->AbDocView);
  doit = FALSE;

  /* traite le changement de largeur */
  if (deltaX != 0)
    {
      /* cherche d'abord la regle de dimension qui s'applique a l'element */
      pRStd = GlobalSearchRulepEl(pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtWidth, FALSE, TRUE, &pAttr);
      /* on ne change pas la largeur si c'est celle du contenu ou si */
      /* c'est une boite elastique.  */
      ok = TRUE;
      imagetrick = FALSE;
      if (!pRStd->PrDimRule.DrPosition)
	{
	if (pRStd->PrDimRule.DrRelation == RlEnclosed)
	  /* largeur du contenu */
	  if (pAb->AbLeafType != LtPicture)
	    /* sauf si image */
	    ok = FALSE;
	  else
	    imagetrick = TRUE;
	}
      else
	ok = FALSE;

      if (ok)
	{
	  /* largeur courante de la boite */
	  largeur =  pAb->AbBox->BxWidth;
	  if (pRStd->PrDimRule.DrUnit == UnPercent)
	    {
	      if (!pRStd->PrDimRule.DrAbsolute)
		/* la largeur de la boite est un pourcentage de la largeur */
		/* d'une autre boite */
		largeurRef = pAb->AbWidth.DimAbRef->AbBox->BxWidth;
	      else if (pAb->AbEnclosing == NULL)
		/* la largeur de la boite est un pourcentage de la largeur */
		/* de la boite englobante */
		GetSizesFrame(frame, &largeurRef, &hauteurRef);
	      else
		/* la largeur de la boite est un pourcentage de la largeur */
		/* de la boite englobante */
		largeurRef = pAb->AbEnclosing->AbBox->BxWidth;
	      /* calcule le isNew rapport (pourcentage) de la boite */
	      x = LogicalValue(deltaX+largeur, UnPercent, (PtrAbstractBox)largeurRef);
	    }
	  else
	    /* calcule la nouvelle largeur en unite logique */
	    x = LogicalValue(deltaX+largeur, pRStd->PrDimRule.DrUnit, pAb);

	  /* cherche si la largeur de l'element est determinee par un */
	  /* attribut auquel est associee l'exception NewWidth */
	  attr = FALSE;
	  if (pAttr != NULL)
	      if (AttrHasException(ExcNewWidth, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
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
                notifyAttr.document = (Document)IdentDocument(pDoc);
                notifyAttr.element = (Element)pEl;
                notifyAttr.attribute = (Attribute)pAttr;
                notifyAttr.attributeType.AttrSSchema = (SSchema)(pAttr->AeAttrSSchema);
                notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
                if (CallEventAttribute(&notifyAttr, TRUE))
                  doit = FALSE;
                else
                  {
		    pAttr->AeAttrValue = x;
		    /* fait reafficher les variables de presentation */
		    /* utilisant l'attribut */
		    RedisplayAttribute(pAttr, pEl, pDoc);
		    if (Disp)
		      /* la nouvelle valeur de l'attribut doit etre prise en */
		      /* compte dans les copies-inclusions de l'element */
		      RedisplayCopies(pEl, pDoc, TRUE);
		  }
		}
	    } 
	  else    /* la nouvelle largeur doit etre rangee dans une regle */
	    /* de presentation specifique */
	    {
	      /* cherche si l'element a deja une regle de largeur specifique */
	      pPRule = SearchPresRule(pEl, PtWidth, &isNew, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */
	      doit = !PRuleMessagePre(pEl, pPRule, pDoc, isNew);
	      if (doit)
                {
		  if (isNew)
		    /* on a cree' une regle de largeur pour l'element */
		    {
		      pR = pPRule->PrNextPRule;
		      /* on recopie la regle standard */
		      *pPRule = *pRStd;
		      pPRule->PrCond = NULL;
		      pPRule->PrNextPRule = pR;
		      pPRule->PrViewNum = viewSch;
		      /* si la regle copiee est associee a un attribut, garde le lien */
		      /* avec cet attribut */
		      if (pAttr != NULL)
			{
			  pPRule->PrSpecifAttr = pAttr->AeAttrNum;
			  pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
			}
		    }
		  pPRule->PrDimRule.DrAttr = FALSE;
		  /* change la regle specifique */
		  pPRule->PrDimRule.DrValue = x;
                }
	    } /* regle specifique */

	  if (doit)
	    {
	      pDoc->DocModified = TRUE;    /* le document est modifie' */
	      for (view = 1; view <= MAX_VIEW_DOC; view++)
		if (pEl->ElAbstractBox[view -1] != NULL)
		  /* l'element traite' a un pave dans cette view */
		  if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocView[pAb->AbDocView -1].DvSSchema
		      && pDoc->DocView[view - 1].DvPSchemaView == pDoc->DocView[pAb->AbDocView -1].DvPSchemaView)
		    /* c'est une view de meme type que la view traitee, on */
		    /* traite le pave de l'element dans cette view */
		    {
		      pAbbCur = pEl->ElAbstractBox[view -1];
		      /* saute les paves de presentation */
		      stop = FALSE;
		      do
			if (pAbbCur == NULL)
			  stop = TRUE;
			else
			  if (!pAbbCur->AbPresentationBox)
			    stop = TRUE;
			  else
			    pAbbCur = pAbbCur->AbNext;
		      while (!stop);
		      if (pAbbCur != NULL)
			/* applique la nouvelle regle specifique */
#ifdef __COLPAGE__
			if (ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
			if (ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr))
#endif /* __COLPAGE__ */
			  {
			    pAbbCur->AbWidthChange = TRUE;
			    /* la position vert.du pave a change' */
			    RedispAbsBox(pAbbCur, pDoc); /* indique le pave a reafficher */
			    reaff = TRUE;   /* il faut reafficher le pave */
			    if (!AssocView(pEl))
			      updateframe[view -1] = pDoc->DocViewFrame[view -1];
			    else
			      updateframe[view -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
			  }
		    }
	      if (attr)
		CallEventAttribute(&notifyAttr, FALSE);
	      else
		PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	    }
	}
    }
  
  /* traite le changement de hauteur de la boite */
  if (deltaY != 0)
    /* cherche d'abord la regle de dimension qui s'applique a l'element */
    {
      pRStd = GlobalSearchRulepEl(pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtHeight, FALSE, TRUE, &pAttr);
      /* on ne change pas la hauteur si c'est celle du contenu ou si c'est */
      /* une boite elastique. */
      ok = TRUE;
      imagetrick = FALSE;
      if (!pRStd->PrDimRule.DrPosition)
	{
	if (pRStd->PrDimRule.DrRelation == RlEnclosed)
	  /* hauteur du contenu */ 
	  if (pAb->AbLeafType != LtPicture)
	    ok = FALSE;
	  else
	    imagetrick = TRUE;
	}
      else
	ok = FALSE;

      if (ok)
	{
	  /* hauteur courante de la boite */
	  hauteur =  pAb->AbBox->BxHeight;
	  if (pRStd->PrDimRule.DrUnit == UnPercent)
	    {
	      if (!pRStd->PrDimRule.DrAbsolute)
		/* la hauteur de la boite est un pourcentage de la hauteur */
		/* d'une autre boite */
		hauteurRef = pAb->AbWidth.DimAbRef->AbBox->BxHeight;
	      else if (pAb->AbEnclosing == NULL)
		/* la hauteur de la boite est un pourcentage de la hauteur */
		/* de la boite englobante */
		GetSizesFrame(frame, &largeurRef, &hauteurRef);
	      else
		/* la largeur de la boite est un pourcentage de la largeur */
		/* de la boite englobante */
		hauteurRef = pAb->AbEnclosing->AbBox->BxHeight;
	      /* calcule le isNew rapport (pourcentage) de la boite */
	      y = LogicalValue(deltaY+hauteur, UnPercent, (PtrAbstractBox)hauteurRef);
	    }
	  else
	    /* calcule la nouvelle largeur en unite logique */
	    y = LogicalValue(deltaY+hauteur, pRStd->PrDimRule.DrUnit, pAb);

	  /* cherche si la hauteur de l'element est determinee par un */
	  /* attribut auquel est associee l'exception NewHeight */
	  attr = FALSE;
	  if (pAttr != NULL)
	    if (AttrHasException(ExcNewHeight, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
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
                notifyAttr.document = (Document)IdentDocument(pDoc);
                notifyAttr.element = (Element)pEl;
                notifyAttr.attribute = (Attribute)pAttr;
                notifyAttr.attributeType.AttrSSchema = (SSchema)(pAttr->AeAttrSSchema);
                notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
                if (CallEventAttribute(&notifyAttr, TRUE))
                  doit = FALSE;
                else
                  {
		    pAttr->AeAttrValue = y;
		    /* fait reafficher les variables de presentation */
		    /* utilisant l'attribut */
		    RedisplayAttribute(pAttr,pEl,pDoc);
		    if (Disp)
		      /* la nouvelle valeur de l'attribut doit etre prise en */
		      /* compte dans les copies-inclusions de l'element */
		      RedisplayCopies(pEl, pDoc, TRUE);
		  }
		}
	    } 
	  else
	    /* la nouvelle hauteur doit etre rangee dans une regle */
	    /* de presentation specifique */
	    {
	      /* cherche si l'element a deja une regle de hauteur specifique */
	      pPRule = SearchPresRule(pEl, PtHeight, &isNew, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */
	      doit = !PRuleMessagePre(pEl, pPRule, pDoc, isNew);
	      if (doit)
                {
		  if (isNew)
		    /* on a cree' une regle de largeur pour l'element */
		    {
		      pR = pPRule->PrNextPRule;
		      /* on recopie la regle standard */
		      *pPRule = *pRStd;
		      pPRule->PrCond = NULL;
		      pPRule->PrNextPRule = pR;
		      pPRule->PrViewNum = viewSch;
		      /* si la regle copiee est associee a un attribut, garde le lien */
		      /* avec cet attribut */
		      if (pAttr != NULL)
			{
			  pPRule->PrSpecifAttr = pAttr->AeAttrNum;
			  pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
			}
		    }
		  pPRule->PrDimRule.DrAttr = FALSE;
		  /* change la regle specifique */
		  pPRule->PrDimRule.DrValue = y;
                }
	    } /* regle specifique */

	  if (doit)
	    {
	      pDoc->DocModified = TRUE;    /* le document est modifie' */
	      for (view = 1; view <= MAX_VIEW_DOC; view++)
		if (pEl->ElAbstractBox[view -1] != NULL)
		  /* l'element traite' a un pave dans cette view */
		  if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocView[pAb->AbDocView -1].DvSSchema
		      && pDoc->DocView[view - 1].DvPSchemaView == pDoc->DocView[pAb->AbDocView -1].DvPSchemaView)
		    /* c'est une view de meme type que la view traitee, on */
		    /* traite le pave de l'element dans cette view */
		    {
		      pAbbCur = pEl->ElAbstractBox[view -1];
		      /* saute les paves de presentation */
		      stop = FALSE;
		      do
			if (pAbbCur == NULL)
			  stop = TRUE;
			else
			  if (!pAbbCur->AbPresentationBox)
			    stop = TRUE;
			  else
			    pAbbCur = pAbbCur->AbNext;
		      while (!stop);
		      if (pAbbCur != NULL)
			/* applique la nouvelle regle specifique */
#ifdef __COLPAGE__
			if (ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
			if (ApplyRule(pPRule, pSPR, pAbbCur, pDoc, pAttr))
#endif /* __COLPAGE__ */
			  {
			    pAbbCur->AbHeightChange = TRUE;
			    RedispAbsBox(pAbbCur, pDoc); /* indique le pave a reafficher */
			    reaff = TRUE;   /* il faut reafficher */
			    if (!AssocView(pEl))
			      updateframe[view -1] = pDoc->DocViewFrame[view -1];
			    else
			      updateframe[view -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
			  }
		    }
	      if (attr)
		CallEventAttribute(&notifyAttr, FALSE);
	      else
		PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	    }
	}
    }
  if (reaff)
    {
      if (Disp)
	{
	  for (view = 1; view <= MAX_VIEW_DOC; view++)
	    if (updateframe[view -1] > 0)
	      /* eteint la selection dans la view traitee */
	      SwitchSelection(updateframe[view -1], FALSE);
	  AbstractImageUpdated(pDoc);     /* mise a jour de l'image abstraite */
	  RedisplayDocViews(pDoc);  /* reafficher ce qu'il faut */
	  for (view = 1; view <= MAX_VIEW_DOC; view++)
	    if (updateframe[view -1] > 0)
	      /* rallume la selection dans la view traitee */
	      SwitchSelection(updateframe[view -1], TRUE);
	}
    }
}



/* ---------------------------------------------------------------------- */
/* |	PaveMisEnLigne	cherche si le pave englobant de pAbb1 et pAbb2	| */
/* |		ou l'un des descendants de pAbb1 est mis en lignes.	| */
/* |		Si oui, retourne un pointeur sur le pave mis en lignes.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAbstractBox PaveMisEnLigne(PtrAbstractBox *pAbb1, PtrAbstractBox *pAbb2)

#else /* __STDC__ */
PtrAbstractBox PaveMisEnLigne(pAbb1, pAbb2)
	PtrAbstractBox *pAbb1;
	PtrAbstractBox *pAbb2;
#endif /* __STDC__ */

{
  PtrAbstractBox         pAbbGroupLines, pAbbCur;
  boolean         ok, found, stop;
  PtrAbstractBox         pAbbCura1;
  PtrAbstractBox         pAbbLine;

  pAbbGroupLines = *pAbb1;
  pAbbCura1 = pAbbGroupLines;
  if (pAbbCura1->AbLeafType == LtCompound && pAbbCura1->AbInLine)
    found = TRUE;
  else
    found = FALSE;
  if (!found && (*pAbb1)->AbEnclosing == (*pAbb2)->AbEnclosing)
    {
      /* on considere l'element englobant s'il n'englobe que les elements */
      /* selectionne's et d'eventuels paves de presentation */
      pAbbGroupLines = (*pAbb1)->AbEnclosing;
      if (pAbbGroupLines != NULL)
	{
	  ok = TRUE;
	  pAbbCur = *pAbb1;
	  while (ok && pAbbCur->AbPrevious != NULL)
	    {
	      pAbbCur = pAbbCur->AbPrevious;
	      ok = pAbbCur->AbPresentationBox;
	    }
	  pAbbCur = *pAbb2;
	  while (ok && pAbbCur->AbNext != NULL)
	    {
	      pAbbCur = pAbbCur->AbNext;
	      ok = pAbbCur->AbPresentationBox;
	    }
	  if (ok)
	    {
	      pAbbCura1 = pAbbGroupLines;
	      if (pAbbCura1->AbLeafType == LtCompound &&
		  pAbbCura1->AbInLine)
		{
		  found = TRUE;
		  *pAbb1 = pAbbGroupLines;
		  *pAbb2 = pAbbGroupLines;
		} 
	      else
		found = FALSE;
	    }
	}
    }
  if (!found)
    /* on n'a pas found' d'element mis en lignes */
    {
      pAbbGroupLines = *pAbb1;
      /* on considere les elements englobe's du premier pave */
      while (!found && pAbbGroupLines->AbFirstEnclosed != NULL)
	{
	  pAbbGroupLines = pAbbGroupLines->AbFirstEnclosed;
	  /* saute les paves de presentation */
	  stop = FALSE;
	  do
	    if (pAbbGroupLines == NULL)
	      stop = TRUE;
	    else if (!pAbbGroupLines->AbPresentationBox)
	      stop = TRUE;
	    else
	      pAbbGroupLines = pAbbGroupLines->AbNext;
	  while (!stop);
	  if (pAbbGroupLines != NULL)
	    {
	      pAbbCura1 = pAbbGroupLines;
	      if (pAbbCura1->AbLeafType == LtCompound && pAbbCura1->AbInLine)
		found = TRUE;
	      else
		found = FALSE;
	    }
	}
    }
  if (found)
    pAbbLine = pAbbGroupLines;
  else
    pAbbLine = NULL;
  return pAbbLine;
}



/* ---------------------------------------------------------------------- */
/* |	ApplNouvRegle applique au pave courant la regle de		| */
/* |		presentation specifique qui vient d'etre creee.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ApplNouvRegle(PtrDocument pDoc, PtrPRule pPRule, PtrElement pEl)
#else /* __STDC__ */
void ApplNouvRegle(pDoc, pPRule, pEl)
	PtrDocument pDoc;
	PtrPRule pPRule;
	PtrElement pEl;
#endif /* __STDC__ */
{
  PtrAbstractBox        pAb;
  int            view;
  boolean        stop;
#ifdef __COLPAGE__
 boolean bool;
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
	      else
		if (!pAb->AbPresentationBox)
		  stop = TRUE;
		else
		  pAb = pAb->AbNext;
	    while (!stop);
#ifdef __COLPAGE__
     /* boucle sur les paves de l'element */
     while (pAb != NULL)
       {
#else /* __COLPAGE__ */
	    if (pAb != NULL)
#endif /* __COLPAGE__ */
	      /* applique la regle de presentation specifique a ce pave' */
#ifdef __COLPAGE__
	      if (ApplyRule(pPRule, NULL, pAb, pDoc, NULL, &bool))
#else /* __COLPAGE__ */
	      if (ApplyRule(pPRule, NULL, pAb, pDoc, NULL))
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
		    default:
			 pAb->AbChange = TRUE;
			 break;
		    }
		  ApplyInherit(pPRule->PrType, pAb, pDoc);
		  /* indique le pave a faire reafficher */
		  RedispAbsBox(pAb, pDoc);
		}
#ifdef __COLPAGE__
         pAb = pAb->AbNextRepeated;
       }
#endif /* __COLPAGE__ */
	  }
}


/* ---------------------------------------------------------------------- */
/* |	ModifGraphiques	applique a l'element pEl les modifications sur	| */
/* |		les graphiques demandes par l'utilisateur.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ModifGraphiques(PtrElement pEl, PtrDocument pDoc, int viewToApply, boolean ChngStyleTrait, char StyleTrait, boolean ChngEpaisTrait, int EpaisTrait, TypeUnit EpaisTraitUnit, boolean ChngTrame, int Trame, boolean ChngCoulFond, int CoulFond, boolean ChngCoulTrace, int CoulTrace)

#else /* __STDC__ */
void ModifGraphiques(pEl, pDoc, viewToApply, ChngStyleTrait, StyleTrait, ChngEpaisTrait, EpaisTrait, EpaisTraitUnit, ChngTrame, Trame, ChngCoulFond, CoulFond, ChngCoulTrace, CoulTrace)
	PtrElement pEl;
	PtrDocument pDoc;
	int viewToApply;
	boolean ChngStyleTrait;
	char StyleTrait;
	boolean ChngEpaisTrait;
	int EpaisTrait;
	TypeUnit EpaisTraitUnit;
	boolean ChngTrame;
	int Trame;
	boolean ChngCoulFond;
	int CoulFond;
	boolean ChngCoulTrace;
	int CoulTrace;
#endif /* __STDC__ */

{
	boolean		isNew;
	PtrPRule	pPRule;
	int		viewSch;
  
  viewSch = AppliedView(pEl, NULL, pDoc, viewToApply); /* numero de cette view */
  /* style des traits dans le graphique */
  if (ChngStyleTrait)
    {
      /*cherche la regle de presentation specifique 'StyleTrait' de l'element*/
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtLineStyle, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtLineStyle;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrChrValue = StyleTrait;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* epaisseur des traits dans le graphique */
  if (ChngEpaisTrait)
    {
      /* cherche la regle de presentation specifique 'Epaisseur Trait' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtLineWeight, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtLineWeight;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrMinUnit = EpaisTraitUnit;
        pPRule->PrMinAttr = FALSE;
        pPRule->PrMinValue = EpaisTrait;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* trame de remplissage */
  if (ChngTrame)
    {
      /* cherche la regle de presentation specifique 'Trame' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtFillPattern, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtFillPattern;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrIntValue = Trame;
        pPRule->PrAttrValue = FALSE;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* couleur de fond */
  if (ChngCoulFond)
    {
      /* cherche la regle de presentation specifique 'CouleurFond' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtBackground, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtBackground;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrIntValue = CoulFond;
        pPRule->PrAttrValue = FALSE;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* couleur du trace' */
  if (ChngCoulTrace)
    {
      /* cherche la regle de presentation specifique 'CouleurTrace' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtForeground, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtForeground;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrIntValue = CoulTrace;
        pPRule->PrAttrValue = FALSE;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	SupprPres supprime toutes les regles de presentation specifiques| */
/* |		associees a l'element pEl.				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void SupprPres(PtrElement pEl, PtrDocument pDoc, RuleSet rules, int viewToApply)
#else /* __STDC__ */
static void SupprPres(pEl, pDoc, rules, viewToApply)
	PtrElement pEl;
	PtrDocument pDoc;
	RuleSet rules;
	int viewToApply;
#endif /* __STDC__ */
{
  PtrPRule    pPRule, pR, pRS;
  PRuleType       ruleType;
  int             viewSch;
  NotifyPresentation notifyPres;

  viewSch = AppliedView(pEl, NULL, pDoc, viewToApply); /* type de cette view */
  pPRule = pEl->ElFirstPRule;
  pR = NULL;		
  /* parcourt les regles de presentation specifiques de l'element */
  while (pPRule != NULL)
    if (pPRule->PrViewNum != viewSch || !RuleSetIn(pPRule->PrType, rules))
      /* cette regle n'est pas concernee */
      {
	pR = pPRule;
	pPRule = pR->PrNextPRule;
      } 
    else
      {
	/* la regle concerne la view traitee */
	/* retire la regle de la chaine des regles de presentation */
	/* specifique de l'element */
	pRS = pPRule->PrNextPRule;	/* regle a traiter apres */
	notifyPres.event = TtePRuleDelete;
	notifyPres.document = (Document)IdentDocument(pDoc);
	notifyPres.element = (Element)pEl;
	notifyPres.pRule = (PRule)pPRule;
	notifyPres.pRuleType = NumTypePRuleAPI(pPRule->PrType);
	if (!CallEventType((NotifyEvent *)&notifyPres, TRUE))
	  {
	  if (pR == NULL)
	    pEl->ElFirstPRule = pRS;
	  else
	    pR->PrNextPRule = pRS;
	  ruleType = pPRule->PrType;	
	  /* libere la regle */
	  FreePresentRule(pPRule);
	  pDoc->DocModified = TRUE;	/* le document est modifie' */
	  /* applique la regle standard de meme type que la regle courante */
	  /* aux paves de l'element qui existent dans les vues de meme type */
	  /* que la view active. */
	  AppliqueRegleStandard(pEl, pDoc, ruleType, viewSch);
	  notifyPres.event = TtePRuleDelete;
	  notifyPres.document = (Document)IdentDocument(pDoc);
	  notifyPres.element = (Element)pEl;
	  notifyPres.pRule = NULL;
	  notifyPres.pRuleType = NumTypePRuleAPI(ruleType);
	  CallEventType((NotifyEvent *)&notifyPres, FALSE);
	  }
	/* passe a la regle suivante */
	pPRule = pRS;
      }
}

/* ---------------------------------------------------------------------- */
/* |	ChangeCouleur change la presentation specifique de la couleur	| */
/* |	de fond ou de trace' (selon Fond) pour tous les elements de la	| */
/* |	selection courante.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void ChangeCouleur(int colorNum, boolean Fond)

#else /* __STDC__ */
void ChangeCouleur(colorNum, Fond)
	int colorNum;
	boolean Fond;
#endif /* __STDC__ */

{
  PtrDocument	SelDoc;
  PtrElement	pElFirstSel, pElLastSel, pEl;
  PtrAbstractBox	pAb;
  int		firstChar, lastChar;
  boolean	selok, modifFrame;
  int		frameNum;
  RuleSet       rulesS;

  /* demande quelle est la selection courante */
  selok = GetCurrentSelection(&SelDoc, &pElFirstSel, &pElLastSel, &firstChar, &lastChar);
  if (!selok)
    /* rien n'est selectionne' */
    TtaDisplaySimpleMessage (INFO, LIB,SEL_EL);
  else if (SelDoc->DocReadOnly)
    TtaDisplaySimpleMessage (INFO, LIB,RO_DOC_FORBIDDEN);
  else
    {
      ClearAllViewSelection();
      /* Coupe les elements du debut et de la fin de la selection*/
      /* s'ils sont partiellement selectionnes */
      if (firstChar > 1 || lastChar > 0)
        CutSelection(SelDoc, &pElFirstSel, &pElLastSel, &firstChar, &lastChar);
      /* parcourt les elements selectionnes */
      pEl = pElFirstSel;
      while (pEl != NULL)
        {
	  /* on saute les elements qui sont des copies */
	  if (!pEl->ElIsCopy)
	    /* on saute les elements non modifiables */
	    if (!ElementIsReadOnly(pEl))
	      /* on saute les marques de page */
	      if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
		{
		  modifFrame = FALSE;
		  frameNum = 0;
		  if (Fond)
		    /* on change la couleur de fond avec la souris */
		    {
		      pAb = PaveDeElem(pEl, SelectedView);
		      if (pAb != NULL)
			if (pAb->AbFillPattern < 2)
			  /* on force la trame backgroundcolor si la trame du pave */
			  /* est nopattern ou foregroundcolor */
			  {
			    modifFrame = TRUE;
			    frameNum = 2;
			  }
		    }
		  if (colorNum == -1)
		    {
		      /* Couleur standard */
		      RuleSetClr(rulesS);
		      if (Fond)
			{
			  RuleSetPut(rulesS, PtFillPattern);
			  RuleSetPut(rulesS, PtBackground);
			}
		      else
			RuleSetPut(rulesS, PtForeground);
		      SupprPres(pEl, SelDoc, rulesS, SelectedView);
		    }
		  else
		    ModifGraphiques(pEl, SelDoc, SelectedView, FALSE, ' ', FALSE, 0, FALSE,
				  modifFrame, frameNum, Fond, colorNum, !Fond, colorNum);
		  /* si on est dans un element copie' par inclusion,   */
		  /* on met a jour les copies de cet element. */
		  RedisplayCopies(pEl, SelDoc, TRUE);
		}
	  /* cherche l'element a traiter ensuite */
	  pEl = NextInSelection(pEl, pElLastSel);
        }
      /* tente de fusionner les elements voisins et reaffiche les paves */
      /* modifie's et la selection */
      MergeAndSelect(SelDoc, pElFirstSel, pElLastSel, firstChar, lastChar);
    }
}


/* ---------------------------------------------------------------------- */
/* |	ModifCaracteres	applique a l'element pEl les modifications sur	| */
/* |		les caracteres demandes par l'utilisateur.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ModifCaracteres(PtrElement pEl, PtrDocument pDoc, int viewToApply, boolean modifFamily, char family, boolean modifStyle, int charStyle, boolean ChngCorps, int Corps, boolean modifUnderline, int underline, boolean modifThick, int thickUnderline)

#else /* __STDC__ */
void ModifCaracteres(pEl, pDoc, viewToApply, modifFamily, family, modifStyle, charStyle, ChngCorps, Corps, modifUnderline, underline, modifThick, thickUnderline)
	PtrElement pEl;
	PtrDocument pDoc;
	int viewToApply;
	boolean modifFamily;
	char family;
	boolean modifStyle;
	int charStyle;
	boolean ChngCorps;
	int Corps;
	boolean modifUnderline;
	int underline;
	boolean modifThick;
	int thickUnderline;
#endif /* __STDC__ */

{
	boolean		isNew;
	PtrPRule	pPRule;
	int		viewSch;
  
  viewSch = AppliedView(pEl, NULL, pDoc, viewToApply); /* numero de cette view*/
  /* applique les choix de l'utilisateur */
  /* family de polices de caracteres */
  if (modifFamily)
    {
      /* cherche la regle de presentation specifique 'Fonte' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtFont, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtFont;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrChrValue = family;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* charStyle de caracteres */
  if (modifStyle)
    {
      /* cherche la regle de presentation specifique 'charStyle' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtStyle, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtStyle;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
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
          pPRule->PrChrValue = 'O'; /* oblique */
          break;
	case 4:
          pPRule->PrChrValue = 'G'; /* gras italique */
          break;
	case 5:
	  pPRule->PrChrValue = 'Q'; /* gras Oblique */
          break;
	default:
	  pPRule->PrChrValue = 'R';
	  break;
	  }
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* Corps des caracteres */
  if (ChngCorps)
    {
      /* cherche la regle de presentation specifique 'Corps' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtSize, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtSize;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrMinUnit = UnPoint;
        pPRule->PrMinAttr = FALSE;
        pPRule->PrMinValue = Corps;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
        PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }

  /* Souligne' */
  if (modifUnderline)
    {
      /* cherche la regle de presentation specifique 'Souligne' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtUnderline, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtUnderline;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        switch (underline)
	  {
	case 0:
	  pPRule->PrChrValue = 'N'; /* sans souligne */
	  break;
	case 1:
	  pPRule->PrChrValue = 'U'; /* souligne continu */
	  break;
	case 2:
	  pPRule->PrChrValue = 'O'; /* surligne */
	  break;
	case 3:
	  pPRule->PrChrValue = 'C'; /* biffer */
	  break;
	default:
	  pPRule->PrChrValue = 'N';
	  break;
	  }
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* Epaisseur du souligne */
  if (modifThick)
    {
      /* cherche la regle de presentation specifique thickUnderline de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule(pEl, PtThickness, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pPRule->PrType = PtThickness;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        switch (thickUnderline)
	  {
	case 0:
	  pPRule->PrChrValue = 'N'; /* souligne mince */
	  break;
	case 1:
	  pPRule->PrChrValue = 'T'; /* souligne epais */
	  break;
	default:
	  pPRule->PrChrValue = 'N';
	  break;
	  }
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	ModifLignes applique a l'element pEl les modifications		| */
/* |		sur la mise en ligne demandes par l'utilisateur.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ModifLignes(PtrElement pEl, PtrDocument pDoc, int viewToApply, boolean ChngCadr, int Cadr, boolean ChngJustif, boolean Justif, boolean ChngIndent, int ValIndent, boolean ChngInterL, int InterLigne, boolean ChngHyphen, boolean Hyphenate)
#else /* __STDC__ */
void ModifLignes(pEl, pDoc, viewToApply, ChngCadr, Cadr, ChngJustif, Justif, ChngIndent, ValIndent, ChngInterL, InterLigne, ChngHyphen, Hyphenate)
	PtrElement pEl;
	PtrDocument pDoc;
	int viewToApply;
	boolean ChngCadr;
	int Cadr;
	boolean ChngJustif;
	boolean Justif;
	boolean ChngIndent;
	int ValIndent;
	boolean ChngInterL;
	int InterLigne;
	boolean ChngHyphen;
	boolean Hyphenate;
#endif /* __STDC__ */
{
  boolean         isNew;
  PtrPRule    pPRule;
  int             viewSch;
  
  viewSch = AppliedView(pEl, NULL, pDoc, viewToApply); /* Le type de cette view */
  /* applique les choix de l'utilisateur */
  if (ChngCadr && Cadr > 0)
    {
      pPRule = SearchPresRule(pEl, PtAdjust, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        pPRule->PrType = PtAdjust;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        switch (Cadr)
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
        pDoc->DocModified = TRUE;	 /* le document est modifie' */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* Justification */
  if (ChngJustif)
    {
      pPRule = SearchPresRule(pEl, PtJustify, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        pPRule->PrType = PtJustify;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrJustify = Justif;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* Coupure des mots */
  if (ChngHyphen)
    {
      pPRule = SearchPresRule(pEl, PtHyphenate, &isNew, pDoc,viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        pPRule->PrType = PtHyphenate;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrJustify = Hyphenate;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        ApplNouvRegle(pDoc, pPRule, pEl);
        PRuleMessagePost(pEl, pPRule, pDoc, isNew);
        }
    }
  /* Renfoncement de la 1ere ligne */
  if (ChngIndent)
    {
      pPRule = SearchPresRule(pEl, PtIndent, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        pPRule->PrType = PtIndent;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrMinUnit = UnPoint;
        pPRule->PrMinAttr = FALSE;
        pPRule->PrMinValue = ValIndent;
        pDoc->DocModified = TRUE;	
        /* le document est modifie' */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
  /* Interligne */
  if (ChngInterL)
    {
      pPRule = SearchPresRule(pEl, PtLineSpacing, &isNew, pDoc, viewToApply);
      if (!PRuleMessagePre(pEl, pPRule, pDoc, isNew))
        {
        pPRule->PrType = PtLineSpacing;
        pPRule->PrViewNum = viewSch;
        pPRule->PrPresMode = PresImmediate;
        pPRule->PrMinUnit = UnPoint;
        pPRule->PrMinAttr = FALSE;
        pPRule->PrMinValue = InterLigne;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        ApplNouvRegle(pDoc, pPRule, pEl);
	PRuleMessagePost(pEl, pPRule, pDoc, isNew);
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	AppliqueRegleStandard	applique a l'element pEl du document	| */
/* |	pDoc la regle de presentation standard de type ruleType	| */
/* |	pour la view viewSch.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void AppliqueRegleStandard(PtrElement pEl, PtrDocument pDoc, PRuleType ruleType, int viewSch)
#else /* __STDC__ */
void AppliqueRegleStandard(pEl, pDoc, ruleType, viewSch)
	PtrElement pEl;
	PtrDocument pDoc;
	PRuleType ruleType;
	int viewSch;
#endif /* __STDC__ */
{
	PtrPRule    pRP;
	PtrAbstractBox         pAb;
	PtrPSchema      pSPR;
	PtrAttribute     pAttr;
	boolean         stop;
	PtrAbstractBox         pPa1;
	int             view;
#ifdef __COLPAGE__
 boolean bool;
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
	    if (pEl->ElAbstractBox[view -1] != NULL)
	      /* l'element traite' a un pave dans cette view */
	      if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocSSchema &&
		  pDoc->DocView[view - 1].DvPSchemaView == viewSch)
		/* c'est une view de meme type que la view traitee, on */
		/* traite le pave de l'element dans cette view */
		{
		  pAb = pEl->ElAbstractBox[view -1];
		  /* saute les paves de presentation */
		  stop = FALSE;
		  do
		    if (pAb == NULL)
		      stop = TRUE;
		    else
		      if (!pAb->AbPresentationBox)
			stop = TRUE;
		      else
			pAb = pAb->AbNext;
		  while (!stop);
		  if (pAb != NULL)
		    {
		      /* cherche la regle standard si on ne l'a pas encore */
		      if (pRP == NULL)
			pRP = SearchRulepAb(pDoc, pAb, &pSPR, ruleType, TRUE, &pAttr);
#ifdef __COLPAGE__
	              ApplyRule(pRP, pSPR, pAb, pDoc, pAttr, &bool);
#else /* __COLPAGE__ */
	              ApplyRule(pRP, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
			/* marque que le pave a change' et doit etre reaffiche' */
			  pPa1 = pAb;
			  switch (ruleType)
			    {
		      case PtVertRef:
			      pPa1->AbVertRefChange = TRUE;
			      break;
		      case PtHorizRef:
			      pPa1->AbHorizRefChange = TRUE;
			      break;
		      case PtHeight:
			      pPa1->AbHeightChange = TRUE;
			      break;
		      case PtWidth:
			      pPa1->AbWidthChange = TRUE;
			      break;
		      case PtVertPos:
			      pPa1->AbVertPosChange = TRUE;
			      break;
		      case PtHorizPos:
			      pPa1->AbHorizPosChange = TRUE;
			      break;
		      case PtSize:
			      pPa1->AbSizeChange = TRUE;
			      break;
		      case PtDepth:
		      case PtLineStyle:
		      case PtLineWeight:
		      case PtFillPattern:
		      case PtBackground:
		      case PtForeground:
			      pPa1->AbAspectChange = TRUE;
			      break;
		      default:
			      pPa1->AbChange = TRUE;
			      break;
			    }
			  
			  RedispAbsBox(pAb, pDoc);
			  /* applique la regle de meme type aux paves environnants */
			  /* s'ils heritent de ce parametre de presentation */
			  ApplyInherit(ruleType, pAb, pDoc);
		    }
		}
}

/* ---------------------------------------------------------------------- */
/* |	SupprPresSpec supprime les regles de presentation specifiques	| */
/* |		contenues dans 'Regles' attachees aux elements du	| */
/* |		sous-arbre de racine pSA				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void SupprPresSpec(PtrElement pSA, PtrDocument pDoc, RuleSet Regles, int viewToApply)
#else /* __STDC__ */
void SupprPresSpec(pSA, pDoc, Regles, viewToApply)
	PtrElement pSA;
	PtrDocument pDoc;
	RuleSet Regles;
	int viewToApply;
#endif /* __STDC__ */
{
  PtrElement      pEl;	/* traite la racine du sous-arbre */
  
  SupprPres(pSA, pDoc, Regles, viewToApply);
  if (!pSA->ElTerminal)
    {
      /* passe au premier fils */
      pEl = pSA->ElFirstChild;
      while (pEl != NULL)
	{
	  /* traite le sous-arbre de ce fils */
	  SupprPresSpec(pEl, pDoc, Regles, viewToApply);
	  /* passe au fils suivant */
	  pEl = pEl->ElNext;
	}
    }
}
/* End Of Module modpres */
