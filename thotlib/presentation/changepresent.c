
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
static void TraiteSArbre(PtrElement pE, PRuleType *TypeR, PtrDocument *pDoc, PtrPRule *pRule, int *vue);
#else /* __STDC__ */
static void TraiteSArbre();
#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |	ApplHerit on vient d'appliquer la regle de presentation de type	| */
/* |		TypeR au pave pAb. Verifie si les paves environnants	| */
/* |		heritent de cette regle et si oui leur applique		| */
/* |		l'heritage.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ApplHerit(PRuleType TypeR, PtrAbstractBox pAb, PtrDocument pDoc)

#else /* __STDC__ */
static void ApplHerit(TypeR, pAb, pDoc)
	PRuleType TypeR;
	PtrAbstractBox pAb;
	PtrDocument pDoc;
#endif /* __STDC__ */

{
  PtrElement      pEl;
  int             vue;
  PtrAbstractBox         pP;
  PtrPRule    pRule;
  boolean         stop;
  PtrPSchema      pSchP;
  PtrAttribute     pAttrib;
  PtrPRule    pRe1;
#ifdef __COLPAGE__
 boolean bool;
#endif /* __COLPAGE__ */
  
  pEl = pAb->AbElement;
  vue = pAb->AbDocView;
  if (pEl->ElNext != NULL)
    {
      /* l'element a un suivant. Celui-ci herite-t-il de son precedent ? */
      pP = NULL;
      while (pEl->ElNext != NULL && pP == NULL)
	{
	  pEl = pEl->ElNext;
	  pP = pEl->ElAbstractBox[vue - 1];	/* saute les paves de presentation */
	  stop = FALSE;
	  do
	    if (pP == NULL)
	      stop = TRUE;
	    else if (!pP->AbPresentationBox)
	      stop = TRUE;
	    else
	      pP = pP->AbNext;
	  while (!(stop));
	  if (pP != NULL)
	    if (pP->AbDead)
	      pP = NULL;
	}
      if (pP != NULL)
	{
	  /* il y a un element suivant dont le pave pP pourrait heriter de pAb */
	  pRule = SearchRulepAb(pDoc, pP, &pSchP, TypeR, TRUE, &pAttrib);
	  if (pRule != NULL)
	    {
	      pRe1 = pRule;
	      if (pRe1->PrPresMode == PresInherit &&
		  pRe1->PrInheritMode == InheritPrevious)
		/* la regle de cet element herite du precedent, on applique */
		/* la regle */
#ifdef __COLPAGE__
		if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
		if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib))
#endif /* __COLPAGE__ */
		  {
		    if (TypeR == PtSize)
		      pP->AbSizeChange = TRUE;
		    else if (TypeR == PtDepth || TypeR == PtLineStyle ||
			     TypeR == PtLineWeight || TypeR == PtFillPattern ||
			     TypeR == PtBackground || TypeR == PtForeground)
		      pP->AbAspectChange = TRUE;
		    else
		      pP->AbChange = TRUE;
		    ApplHerit(TypeR, pP, pDoc);
		    RedispAbsBox(pP, pDoc);
		  }
	    }
	}
      pEl = pAb->AbElement;
    }
  if (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
    /* l'element a des descendants. Ceux-ci heritent-t-il de leur */
    /* ascendant ? */
    TraiteSArbre(pEl, &TypeR, &pDoc, &pRule, &vue);
  if (pEl->ElParent != NULL)
    {
      /* l'element a un ascendant. Celui-ci herite-t-il de son premier */
      /* descendant ? */
      pP = NULL;
      while (pEl->ElParent != NULL && pP == NULL)
	{
	  pEl = pEl->ElParent;
	  pP = pEl->ElAbstractBox[vue - 1];	/* saute les paves de presentation */
	  stop = FALSE;
	  do
	    if (pP == NULL)
	      stop = TRUE;
	    else if (!pP->AbPresentationBox)
	      stop = TRUE;
	    else
	      pP = pP->AbNext;
	  while (!(stop));
	  if (pP != NULL)
	    if (pP->AbDead)
	      pP = NULL;
	}
      if (pP != NULL)
	{
	  /* il y a un element ascendant dont le pave pP pourrait heriter */
	  /* de pAb */
	  pRule = SearchRulepAb(pDoc, pP, &pSchP, TypeR, TRUE, &pAttrib);
	  pRe1 = pRule;
	  if (pRule != NULL)
	    if (pRe1->PrPresMode == PresInherit &&
		pRe1->PrInheritMode == InheritChild)
	      /* la regle de cet element herite du descendant, on */
	      /* applique la regle */
#ifdef __COLPAGE__
	      if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	      if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib))
#endif /* __COLPAGE__ */
		{
		  if (TypeR == PtSize)
		    pP->AbSizeChange = TRUE;
		  else if (TypeR == PtDepth || TypeR == PtLineStyle ||
			   TypeR == PtLineWeight || TypeR == PtFillPattern ||
			   TypeR == PtBackground || TypeR == PtForeground)
		    pP->AbAspectChange = TRUE;
		  else
		    pP->AbChange = TRUE;
		  ApplHerit(TypeR, pP, pDoc);
		  RedispAbsBox(pP, pDoc);
		}
	}
    }

  if (!pAb->AbPresentationBox)
    /* ce n'est pas un pave de presentation. On regarde si les paves de */
    /* presentation crees par l'element heritent de leur createur */
    {
      /* on regarde d'abord les paves crees devant (par CreateBefore) */
      pEl = pAb->AbElement;
      pP = pAb->AbPrevious;
      while (pP != NULL)
	if (!pP->AbPresentationBox || pP->AbElement != pEl)
	    /* ce n'est pas un pave de presentation de l'element, on arrete */
	    pP = NULL;
	else
	  {
	    pRule = SearchRulepAb(pDoc, pP, &pSchP, TypeR, TRUE, &pAttrib);
	    if (pRule != NULL)
	      if (pRule->PrPresMode == PresInherit && pRule->PrInheritMode == InheritCreator)
		/* la regle de ce pave herite de son createur, on l'applique */
#ifdef __COLPAGE__
	        if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	        if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib))
#endif /* __COLPAGE__ */
		{
		  if (TypeR == PtSize)
		    pP->AbSizeChange = TRUE;
		  else if (TypeR == PtDepth || TypeR == PtLineStyle ||
			   TypeR == PtLineWeight || TypeR == PtFillPattern ||
			   TypeR == PtBackground || TypeR == PtForeground)
		    pP->AbAspectChange = TRUE;
		  else
		    pP->AbChange = TRUE;
		  ApplHerit(TypeR, pP, pDoc);
		  RedispAbsBox(pP, pDoc);
		}
	    /* examine le pave precedent */
	    pP = pP->AbPrevious;
          }
      /* on regarde les paves crees derriere (par CreateAfter) */
      pP = pAb->AbNext;
      while (pP != NULL)
	if (!pP->AbPresentationBox || pP->AbElement != pEl)
	    /* ce n'est pas un pave de presentation de l'element, on arrete */
	    pP = NULL;
	else
	  {
	    pRule = SearchRulepAb(pDoc, pP, &pSchP, TypeR, TRUE, &pAttrib);
	    if (pRule != NULL)
	      if (pRule->PrPresMode == PresInherit && pRule->PrInheritMode == InheritCreator)
		/* la regle de ce pave herite de son createur, on l'applique */
#ifdef __COLPAGE__
	        if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	        if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib))
#endif /* __COLPAGE__ */
		{
		  if (TypeR == PtSize)
		    pP->AbSizeChange = TRUE;
		  else if (TypeR == PtDepth || TypeR == PtLineStyle ||
			   TypeR == PtLineWeight || TypeR == PtFillPattern ||
			   TypeR == PtBackground || TypeR == PtForeground)
		    pP->AbAspectChange = TRUE;
		  else
		    pP->AbChange = TRUE;
		  ApplHerit(TypeR, pP, pDoc);
		  RedispAbsBox(pP, pDoc);
		}
	    /* examine le pave suivant */
	    pP = pP->AbNext;
          }
      /* on regarde les paves crees au niveau inferieur (par Create et CreateLast) */
      pP = pAb->AbFirstEnclosed;
      while (pP != NULL)
	{
	  if (pP->AbPresentationBox && pP->AbElement == pEl)
	    /* c'est un pave de presentation de l'element, on le traite */
	    {
	    pRule = SearchRulepAb(pDoc, pP, &pSchP, TypeR, TRUE, &pAttrib);
	    if (pRule != NULL)
	      if (pRule->PrPresMode == PresInherit && pRule->PrInheritMode == InheritCreator)
		/* la regle de ce pave herite de son createur, on l'applique */
#ifdef __COLPAGE__
	        if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib, &bool))
#else /* __COLPAGE__ */
	        if (ApplyRule(pRule, pSchP, pP, pDoc, pAttrib))
#endif /* __COLPAGE__ */
		{
		  if (TypeR == PtSize)
		    pP->AbSizeChange = TRUE;
		  else if (TypeR == PtDepth || TypeR == PtLineStyle ||
			   TypeR == PtLineWeight || TypeR == PtFillPattern ||
			   TypeR == PtBackground || TypeR == PtForeground)
		    pP->AbAspectChange = TRUE;
		  else
		    pP->AbChange = TRUE;
		  ApplHerit(TypeR, pP, pDoc);
		  RedispAbsBox(pP, pDoc);
		}
            }
	  /* examine le pave suivant */
	  pP = pP->AbNext;
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
/* |	PRuleMessagePre	On veut ajouter ou modifier (selon nouveau) la	| */
/* |	regle de presentation specifique pRule a l'element pEl du	| */
/* |	document pDoc. On envoie le message APP correspondant a		| */
/* |	l'application et on retourne la reponse de l'application.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean PRuleMessagePre(PtrElement pEl, PtrPRule pRule, PtrDocument pDoc, boolean nouveau)
#else /* __STDC__ */
static boolean PRuleMessagePre(pEl, pRule, pDoc, nouveau)
	PtrElement pEl;
	PtrPRule pRule;
	PtrDocument pDoc;
	boolean nouveau;
#endif /* __STDC__ */
{
	NotifyPresentation notifyPres;
	PtrPRule	   pR, pRPrec;
	boolean		   dontdoit;

	if (nouveau)
	  {
	  notifyPres.event = TtePRuleCreate;
	  notifyPres.pRule = NULL;
	  }
	else
	  {
	  notifyPres.event = TtePRuleModify;
	  notifyPres.pRule = (PRule)pRule;
	  }
	notifyPres.document = (Document)IdentDocument(pDoc);
	notifyPres.element = (Element)pEl;
	notifyPres.pRuleType = NumTypePRuleAPI(pRule->PrType);
	dontdoit = CallEventType((NotifyEvent *)&notifyPres, TRUE);
	if (dontdoit)
	  /* l'application demande a l'editeur de ne rien faire */
	  if (nouveau)
	     /* supprime la regle de presentation specifique ajoutee */
	     /* par SearchPresRule */
	     {
	     pR = pEl->ElFirstPRule;
	     pRPrec = NULL;
	     /* parcourt les regles de presentation specifiques de l'element */
	     while (pR != NULL)
	       if (pR != pRule)
	         /* ce n'est pas la regle cherchee, passe a la suivante */
	         {
	         pRPrec = pR;
	         pR = pR->PrNextPRule;
	         } 
	       else
		 /* c'est la regle a supprimer */
	         {
		 if (pRPrec == NULL)
		    pEl->ElFirstPRule = pRule->PrNextPRule;
		 else
		    pRPrec->PrNextPRule = pRule->PrNextPRule;
		 /* libere la regle */
		 FreeReglePres(pRule);
		 pR = NULL;
		 }
	     }
	return dontdoit;
}

/* ---------------------------------------------------------------------- */
/* |	PRuleMessagePost      On vient d'ajouter ou de modifier (selon	| */
/* |	nouveau) la regle de presentation specifique pRule pour	| */
/* |	l'element pEl du document pDoc. On envoie le message APP	| */
/* |	correspondant a l'application.					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void PRuleMessagePost(PtrElement pEl, PtrPRule pRule, PtrDocument pDoc, boolean nouveau)

#else /* __STDC__ */
static void PRuleMessagePost(pEl, pRule, pDoc, nouveau)
	PtrElement pEl;
	PtrPRule pRule;
	PtrDocument pDoc;
	boolean nouveau;
#endif /* __STDC__ */

{
	NotifyPresentation notifyPres;

	if (nouveau)
	  notifyPres.event = TtePRuleCreate;
	else
	  notifyPres.event = TtePRuleModify;
	notifyPres.pRule = (PRule)pRule;
	notifyPres.document = (Document)IdentDocument(pDoc);
	notifyPres.element = (Element)pEl;
	notifyPres.pRuleType = NumTypePRuleAPI(pRule->PrType);
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
/* |		qui appartient a la vue Vue				| */
/* |		et qui n'est pas un pave de presentation.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrAbstractBox PaveDeElem(PtrElement pEl, int Vue)
#else /* __STDC__ */
PtrAbstractBox PaveDeElem(pEl, Vue)
	PtrElement pEl;
	int Vue;
#endif /* __STDC__ */
{
  PtrAbstractBox         pAb;
  boolean         stop;
  
  pAb = NULL;
  if (Vue > 0)
    if (pEl->ElAbstractBox[Vue - 1] != NULL)
      /* on prend le pave de la vue choisie par l'utilisateur */
      pAb = pEl->ElAbstractBox[Vue - 1];
  /* saute les paves de presentation */
  stop = FALSE;
  do
    if (pAb == NULL)
      stop = TRUE;
    else if (!pAb->AbPresentationBox)
      stop = TRUE;
    else
      pAb = pAb->AbNext;
  while (!(stop));
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
  boolean         nouveau, reaff, lignes;
  PtrPRule    pRule, pR, pRStd;
  PtrPSchema      pSPR;
  PtrSSchema	  pSSR;
  PtrAttribute     pAttr;
  PtrDocument     pDoc;
  PtrElement      pEl;
  PtrAbstractBox         pP;
  int		  updateframe[MAX_VIEW_DOC];
  int             viewSch;
  boolean         attr;
  int		  vue;
  boolean	  stop;
  PosRule       *pRe1;
  boolean	  doit;
  NotifyAttribute notifyAttr;
  int             x, y;
#ifdef __COLPAGE__
  boolean bool;
#endif /* __COLPAGE__ */

  /* nettoie la table des frames a reafficher */
  for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
    updateframe[vue - 1] = 0;
  reaff = FALSE;  		/* rien a reafficher */
  pEl = pAb->AbElement;	/* l'element auquel correspond le pave */
  pDoc = DocumentOfElement(pEl);	/* le document auquel il appartient */ 
  /* numero de cette vue */ 
  viewSch = AppliedView(pEl, NULL, pDoc, pAb->AbDocView);
  /* le pave est-il dans une mise en lignes ? */
  lignes = FALSE;	/* a priori non */
  doit = FALSE;
  pP = pAb->AbEnclosing;
  /* on examine les paves englobants */
  while (!lignes && pP != NULL)
    {
      if (pP->AbLeafType == LtCompound && pP->AbInLine)
	lignes = TRUE;	/* on est dans un pave mis en lignes */
      else if (!pP->AbAcceptLineBreak)
	pP = NULL;	/* on est dans un pave insecable, inutile */
      /* d'examiner les paves englobants */
      else	/* passe au pave englobant */
	pP = pP->AbEnclosing;
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
		DimFenetre(frame, &x, &y);
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
	    pRule = pRStd;
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
	      pRule = SearchPresRule(pEl, PtVertPos, &nouveau, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */

	      doit = !PRuleMessagePre(pEl, pRule, pDoc, nouveau);
	      if (doit)
	        {
	        if (nouveau)
		  /* l'element n'avait pas de regle de position verticale */
		  /* specifique */
		  {
		  pR = pRule->PrNextPRule;	/* conserve le chainage */
		  *pRule = *pRStd;	/* recopie la regle standard */
		  pRule->PrCond = NULL;
		  pRule->PrNextPRule = pR;	/* restaure le chainage */
		  pRule->PrViewNum = viewSch;
		  /* si la regle copiee est associee a un attribut, garde le */
		  /* lien avec cet attribut */
		  if (pAttr != NULL)
		    {
		      pRule->PrSpecifAttr = pAttr->AeAttrNum;
		      pRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		    }
		  }
	        pRe1 = &pRule->PrPosRule;
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
	    for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	      if (pEl->ElAbstractBox[vue -1] != NULL)
	        /* l'element traite' a un pave dans cette vue */
	        if (pDoc->DocView[vue - 1].DvSSchema == 
		    pDoc->DocView[pAb->AbDocView -1].DvSSchema
		    && pDoc->DocView[vue - 1].DvPSchemaView ==
		    pDoc->DocView[pAb->AbDocView -1].DvPSchemaView)
		  /* c'est une vue de meme type que la vue traitee, on */
		  /* traite le pave de l'element dans cette vue */
		  {
		  pP = pEl->ElAbstractBox[vue -1];
		  /* saute les paves de presentation */
		  stop = FALSE;
		  do
		    if (pP == NULL)
		      stop = TRUE;
		    else
		      if (!pP->AbPresentationBox)
			stop = TRUE;
		      else
			pP = pP->AbNext;
		  while (!stop);
		  if (pP != NULL)
		    /* applique la nouvelle regle de position verticale */
#ifdef __COLPAGE__
	            ApplyRule(pRule, pSPR, pP, pDoc, pAttr, &bool);
#else /* __COLPAGE__ */
	            ApplyRule(pRule, pSPR, pP, pDoc, pAttr);
#endif /* __COLPAGE__ */
		      {
			pP->AbVertPosChange = TRUE;
			/* la position vert.du pave a change' */
			RedispAbsBox(pP, pDoc);
			reaff = TRUE;	
			/* il faut reafficher le pave */
			if (!AssocView(pEl))
			  updateframe[vue -1] = pDoc->DocViewFrame[vue -1];
			else
			  updateframe[vue -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
		      }
		  }
	    if (attr)
	       CallEventAttribute(&notifyAttr, FALSE);
	    else
	       PRuleMessagePost(pEl, pRule, pDoc, nouveau);
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
		DimFenetre(frame, &x, &y);
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
	      pRule = pRStd;
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
	      pRule = SearchPresRule(pEl, PtHorizPos, &nouveau, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */
	      doit = !PRuleMessagePre(pEl, pRule, pDoc, nouveau);
	      if (doit)
		{
	        if (nouveau)
		  /* on a cree' une regle de position horizontale pour l'element */
		  {
		  pR = pRule->PrNextPRule;
		  /* recopie la regle standard */
		  *pRule = *pRStd;
		  pRule->PrCond = NULL;
		  pRule->PrNextPRule = pR;
		  pRule->PrViewNum = viewSch;
		  /* si la regle copiee est associee a un attribut, garde le lien */
		  /* avec cet attribut */
		  if (pAttr != NULL)
		    {
		      pRule->PrSpecifAttr = pAttr->AeAttrNum;
		      pRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		    }
		  }
	        pRe1 = &pRule->PrPosRule;
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
	    for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	      if (pEl->ElAbstractBox[vue -1] != NULL)
	        /* l'element traite' a un pave dans cette vue */
	        if ((pDoc->DocView[vue - 1].DvSSchema ==
		     pDoc->DocView[pAb->AbDocView -1].DvSSchema)
		    && (pDoc->DocView[vue - 1].DvPSchemaView ==
		        pDoc->DocView[pAb->AbDocView -1].DvPSchemaView))
		  /* c'est une vue de meme type que la vue traitee, on */
		  /* traite le pave de l'element dans cette vue */
		  {
		  pP = pEl->ElAbstractBox[vue -1];
		  /* saute les paves de presentation */
		  stop = FALSE;
		  do
		    if (pP == NULL)
		      stop = TRUE;
		    else
		      if (!pP->AbPresentationBox)
			stop = TRUE;
		      else
			pP = pP->AbNext;
		  while (!stop);
		  if (pP != NULL)	
		    /* applique la nouvelle regle de position verticale */
#ifdef __COLPAGE__
	            ApplyRule(pRule, pSPR, pP, pDoc, pAttr, &bool);
#else /* __COLPAGE__ */
	            ApplyRule(pRule, pSPR, pP, pDoc, pAttr);
#endif /* __COLPAGE__ */
		      {
			pP->AbHorizPosChange = TRUE;
			RedispAbsBox(pP, pDoc); /* indique le pave a reafficher */
			reaff = TRUE;	/* il faut reafficher le pave */
			if (!AssocView(pEl))
			  updateframe[vue -1] = pDoc->DocViewFrame[vue -1];
			else
			  updateframe[vue -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
		      }
		  }
	    if (attr)
	       CallEventAttribute(&notifyAttr, FALSE);
	    else
	       PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	    }
	}
    }
  if (reaff)
    {
      if (Disp)
	{
	for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	  if (updateframe[vue -1] > 0)
	    /* eteint la selection dans la vue traitee */
	    SwitchSelection(updateframe[vue -1], FALSE);
        AbstractImageUpdated(pDoc);	/* met a jour l'image abstraite */
        RedisplayDocViews(pDoc);	/* fait reafficher ce qui doit l'etre */
        for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	  if (updateframe[vue -1] > 0)
	    /* rallume la selection dans la vue traitee */
	    SwitchSelection(updateframe[vue -1], TRUE);
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
  boolean         nouveau, reaff, ok, imagetrick;
  PtrPRule    pRule, pR, pRStd;
  PtrPSchema      pSPR;
  PtrSSchema	  pSSR;
  PtrAttribute     pAttr;
  PtrDocument     pDoc;
  PtrElement      pEl;
  int             hauteur, largeur, hauteurRef, largeurRef;
  int             updateframe[MAX_VIEW_DOC];
  int             viewSch;
  boolean         attr;
  PtrAbstractBox         pP;
  int             vue;
  boolean         stop;
  boolean	  doit;
  NotifyAttribute notifyAttr;
  int             x, y;
#ifdef __COLPAGE__
 boolean          bool;
#endif /* __COLPAGE__ */

  /* nettoie la table des frames a reafficher */
  for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
    updateframe[vue - 1] = 0;
  reaff = FALSE;		/* rien a reafficher */
  pEl = pAb->AbElement;	/* l'element auquel correspond le pave */
  pDoc = DocumentOfElement(pEl);	/* le document auquel appartient le pave */ 
  /* numero de cette vue dans le schema de presentation qui la definit */
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
		DimFenetre(frame, &largeurRef, &hauteurRef);
	      else
		/* la largeur de la boite est un pourcentage de la largeur */
		/* de la boite englobante */
		largeurRef = pAb->AbEnclosing->AbBox->BxWidth;
	      /* calcule le nouveau rapport (pourcentage) de la boite */
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
	      pRule = pRStd;
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
	      pRule = SearchPresRule(pEl, PtWidth, &nouveau, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */
	      doit = !PRuleMessagePre(pEl, pRule, pDoc, nouveau);
	      if (doit)
                {
		  if (nouveau)
		    /* on a cree' une regle de largeur pour l'element */
		    {
		      pR = pRule->PrNextPRule;
		      /* on recopie la regle standard */
		      *pRule = *pRStd;
		      pRule->PrCond = NULL;
		      pRule->PrNextPRule = pR;
		      pRule->PrViewNum = viewSch;
		      /* si la regle copiee est associee a un attribut, garde le lien */
		      /* avec cet attribut */
		      if (pAttr != NULL)
			{
			  pRule->PrSpecifAttr = pAttr->AeAttrNum;
			  pRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
			}
		    }
		  pRule->PrDimRule.DrAttr = FALSE;
		  /* change la regle specifique */
		  pRule->PrDimRule.DrValue = x;
                }
	    } /* regle specifique */

	  if (doit)
	    {
	      pDoc->DocModified = TRUE;    /* le document est modifie' */
	      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
		if (pEl->ElAbstractBox[vue -1] != NULL)
		  /* l'element traite' a un pave dans cette vue */
		  if (pDoc->DocView[vue - 1].DvSSchema == pDoc->DocView[pAb->AbDocView -1].DvSSchema
		      && pDoc->DocView[vue - 1].DvPSchemaView == pDoc->DocView[pAb->AbDocView -1].DvPSchemaView)
		    /* c'est une vue de meme type que la vue traitee, on */
		    /* traite le pave de l'element dans cette vue */
		    {
		      pP = pEl->ElAbstractBox[vue -1];
		      /* saute les paves de presentation */
		      stop = FALSE;
		      do
			if (pP == NULL)
			  stop = TRUE;
			else
			  if (!pP->AbPresentationBox)
			    stop = TRUE;
			  else
			    pP = pP->AbNext;
		      while (!stop);
		      if (pP != NULL)
			/* applique la nouvelle regle specifique */
#ifdef __COLPAGE__
			if (ApplyRule(pRule, pSPR, pP, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
			if (ApplyRule(pRule, pSPR, pP, pDoc, pAttr))
#endif /* __COLPAGE__ */
			  {
			    pP->AbWidthChange = TRUE;
			    /* la position vert.du pave a change' */
			    RedispAbsBox(pP, pDoc); /* indique le pave a reafficher */
			    reaff = TRUE;   /* il faut reafficher le pave */
			    if (!AssocView(pEl))
			      updateframe[vue -1] = pDoc->DocViewFrame[vue -1];
			    else
			      updateframe[vue -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
			  }
		    }
	      if (attr)
		CallEventAttribute(&notifyAttr, FALSE);
	      else
		PRuleMessagePost(pEl, pRule, pDoc, nouveau);
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
		DimFenetre(frame, &largeurRef, &hauteurRef);
	      else
		/* la largeur de la boite est un pourcentage de la largeur */
		/* de la boite englobante */
		hauteurRef = pAb->AbEnclosing->AbBox->BxHeight;
	      /* calcule le nouveau rapport (pourcentage) de la boite */
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
	      pRule = pRStd;
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
	      pRule = SearchPresRule(pEl, PtHeight, &nouveau, pDoc, pAb->AbDocView);
	      /* envoie un message APP a l'application */
	      doit = !PRuleMessagePre(pEl, pRule, pDoc, nouveau);
	      if (doit)
                {
		  if (nouveau)
		    /* on a cree' une regle de largeur pour l'element */
		    {
		      pR = pRule->PrNextPRule;
		      /* on recopie la regle standard */
		      *pRule = *pRStd;
		      pRule->PrCond = NULL;
		      pRule->PrNextPRule = pR;
		      pRule->PrViewNum = viewSch;
		      /* si la regle copiee est associee a un attribut, garde le lien */
		      /* avec cet attribut */
		      if (pAttr != NULL)
			{
			  pRule->PrSpecifAttr = pAttr->AeAttrNum;
			  pRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
			}
		    }
		  pRule->PrDimRule.DrAttr = FALSE;
		  /* change la regle specifique */
		  pRule->PrDimRule.DrValue = y;
                }
	    } /* regle specifique */

	  if (doit)
	    {
	      pDoc->DocModified = TRUE;    /* le document est modifie' */
	      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
		if (pEl->ElAbstractBox[vue -1] != NULL)
		  /* l'element traite' a un pave dans cette vue */
		  if (pDoc->DocView[vue - 1].DvSSchema == pDoc->DocView[pAb->AbDocView -1].DvSSchema
		      && pDoc->DocView[vue - 1].DvPSchemaView == pDoc->DocView[pAb->AbDocView -1].DvPSchemaView)
		    /* c'est une vue de meme type que la vue traitee, on */
		    /* traite le pave de l'element dans cette vue */
		    {
		      pP = pEl->ElAbstractBox[vue -1];
		      /* saute les paves de presentation */
		      stop = FALSE;
		      do
			if (pP == NULL)
			  stop = TRUE;
			else
			  if (!pP->AbPresentationBox)
			    stop = TRUE;
			  else
			    pP = pP->AbNext;
		      while (!stop);
		      if (pP != NULL)
			/* applique la nouvelle regle specifique */
#ifdef __COLPAGE__
			if (ApplyRule(pRule, pSPR, pP, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
			if (ApplyRule(pRule, pSPR, pP, pDoc, pAttr))
#endif /* __COLPAGE__ */
			  {
			    pP->AbHeightChange = TRUE;
			    RedispAbsBox(pP, pDoc); /* indique le pave a reafficher */
			    reaff = TRUE;   /* il faut reafficher */
			    if (!AssocView(pEl))
			      updateframe[vue -1] = pDoc->DocViewFrame[vue -1];
			    else
			      updateframe[vue -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
			  }
		    }
	      if (attr)
		CallEventAttribute(&notifyAttr, FALSE);
	      else
		PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	    }
	}
    }
  if (reaff)
    {
      if (Disp)
	{
	  for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	    if (updateframe[vue -1] > 0)
	      /* eteint la selection dans la vue traitee */
	      SwitchSelection(updateframe[vue -1], FALSE);
	  AbstractImageUpdated(pDoc);     /* mise a jour de l'image abstraite */
	  RedisplayDocViews(pDoc);  /* reafficher ce qu'il faut */
	  for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	    if (updateframe[vue -1] > 0)
	      /* rallume la selection dans la vue traitee */
	      SwitchSelection(updateframe[vue -1], TRUE);
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
  PtrAbstractBox         pPavLignes, pP;
  boolean         ok, found, stop;
  PtrAbstractBox         pPa1;
  PtrAbstractBox         PavEnLigne;

  pPavLignes = *pAbb1;
  pPa1 = pPavLignes;
  if (pPa1->AbLeafType == LtCompound && pPa1->AbInLine)
    found = TRUE;
  else
    found = FALSE;
  if (!found && (*pAbb1)->AbEnclosing == (*pAbb2)->AbEnclosing)
    {
      /* on considere l'element englobant s'il n'englobe que les elements */
      /* selectionne's et d'eventuels paves de presentation */
      pPavLignes = (*pAbb1)->AbEnclosing;
      if (pPavLignes != NULL)
	{
	  ok = TRUE;
	  pP = *pAbb1;
	  while (ok && pP->AbPrevious != NULL)
	    {
	      pP = pP->AbPrevious;
	      ok = pP->AbPresentationBox;
	    }
	  pP = *pAbb2;
	  while (ok && pP->AbNext != NULL)
	    {
	      pP = pP->AbNext;
	      ok = pP->AbPresentationBox;
	    }
	  if (ok)
	    {
	      pPa1 = pPavLignes;
	      if (pPa1->AbLeafType == LtCompound &&
		  pPa1->AbInLine)
		{
		  found = TRUE;
		  *pAbb1 = pPavLignes;
		  *pAbb2 = pPavLignes;
		} 
	      else
		found = FALSE;
	    }
	}
    }
  if (!found)
    /* on n'a pas found' d'element mis en lignes */
    {
      pPavLignes = *pAbb1;
      /* on considere les elements englobe's du premier pave */
      while (!found && pPavLignes->AbFirstEnclosed != NULL)
	{
	  pPavLignes = pPavLignes->AbFirstEnclosed;
	  /* saute les paves de presentation */
	  stop = FALSE;
	  do
	    if (pPavLignes == NULL)
	      stop = TRUE;
	    else if (!pPavLignes->AbPresentationBox)
	      stop = TRUE;
	    else
	      pPavLignes = pPavLignes->AbNext;
	  while (!(stop));
	  if (pPavLignes != NULL)
	    {
	      pPa1 = pPavLignes;
	      if (pPa1->AbLeafType == LtCompound && pPa1->AbInLine)
		found = TRUE;
	      else
		found = FALSE;
	    }
	}
    }
  if (found)
    PavEnLigne = pPavLignes;
  else
    PavEnLigne = NULL;
  return PavEnLigne;
}


/* ---------------------------------------------------------------------- */
/* |	TraiteSArbre							| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void TraiteSArbre(PtrElement pE, PRuleType *TypeR, PtrDocument *pDoc, PtrPRule *pRule, int *vue)
#else /* __STDC__ */
static void TraiteSArbre(pE, TypeR, pDoc, pRule, vue)
	PtrElement pE;
	PRuleType *TypeR;
	PtrDocument *pDoc;
	PtrPRule *pRule;
	int *vue;
#endif /* __STDC__ */
{
  boolean         stop;
  PtrAbstractBox         pP, pPF;
  PtrPSchema      pSPR;
  PtrAttribute     pAttr;
  PtrPRule    pRe1;
#ifdef __COLPAGE__
 boolean bool;
#endif /* __COLPAGE__ */
  
  if (pE->ElTerminal)
    pE = NULL;
  else
    pE = pE->ElFirstChild;	/* on passe au premier fils */
  while (pE != NULL)
    {
      pP = pE->ElAbstractBox[*vue - 1]; /* 1er pave de l'element dans la vue */
      if (pP != NULL)
	if (pP->AbDead)
	  pP = NULL;	/* on ne traite pas les paves morts */
      if (pP == NULL)
	/* cet element n'a pas de pave, mais ses descendants en */
	/* ont peut etre... */
	TraiteSArbre(pE, TypeR, pDoc, pRule, vue);
      else
	{
	  /* il y a un element descendant dont les paves peuvent heriter de pAb. 
	     On parcourt ses paves dans la vue */
	  stop = FALSE;
	  while (!stop)
	    {
	      *pRule = SearchRulepAb(*pDoc, pP, &pSPR, *TypeR, TRUE, &pAttr);
	      if (*pRule != NULL)
		{
		  pRe1 = *pRule;
		  if (pRe1->PrPresMode == PresInherit && (pRe1->PrInheritMode == InheritParent ||
				pRe1->PrInheritMode == InheritGrandFather))
		    /* la regle de ce pave herite de l'ascendant, */
		    /* on applique la regle */
#ifdef __COLPAGE__
	            if (ApplyRule(*pRule, pSPR, pP, *pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
	            if (ApplyRule(*pRule, pSPR, pP, *pDoc, pAttr))
#endif /* __COLPAGE__ */
		      {
			if (*TypeR == PtSize)
			  pP->AbSizeChange = TRUE;
			else if (*TypeR == PtDepth || *TypeR == PtLineStyle ||
				 *TypeR == PtLineWeight || *TypeR == PtFillPattern ||
				 *TypeR == PtBackground || *TypeR == PtForeground)
			  pP->AbAspectChange = TRUE;
			else
			  pP->AbChange = TRUE;
			RedispAbsBox(pP, *pDoc);
			if (!pP->AbPresentationBox)
			  ApplHerit(*TypeR, pP, *pDoc);
		      }
		}
	      if (!pP->AbPresentationBox)
		/* c'est le pave principal de l'element, on traite */
		/* les paves crees par l'element au niveau inferieur */
		{
		  pPF = pP->AbFirstEnclosed;
		  while (pPF != NULL)
		    {
		      if (pPF->AbElement == pE)
			{
			  *pRule = SearchRulepAb(*pDoc, pPF, &pSPR, *TypeR, TRUE, &pAttr);
			  if (*pRule != NULL)
			    {
			      pRe1 = *pRule;
			      if (pRe1->PrPresMode == PresInherit 
				  && pRe1->PrInheritMode == InheritParent)
#ifdef __COLPAGE__
	            		if (ApplyRule(*pRule, pSPR, pPF, *pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
	            		if (ApplyRule(*pRule, pSPR, pPF, *pDoc, pAttr))
#endif /* __COLPAGE__ */
				  {
				    if (*TypeR == PtSize)
				      pPF->AbSizeChange = TRUE;
				    else if (*TypeR == PtDepth ||
					     *TypeR == PtLineStyle ||
					     *TypeR == PtLineWeight ||
					     *TypeR == PtFillPattern ||
					     *TypeR == PtBackground ||
					     *TypeR == PtForeground)
				      pP->AbAspectChange = TRUE;
				    else
				      pPF->AbChange = TRUE;
				    RedispAbsBox(pPF, *pDoc);
				  }
			    }
			}
		      pPF = pPF->AbNext;
		    }
		}
	      pP = pP->AbNext;	/* passe au pave suivant */
	      if (pP == NULL)
		stop = TRUE;	/* pas de pave suivant, on arrete */
	      else	
		/* on arrete si le pave suivant n'appartient pas a */
		/* l'element */
		stop = pP->AbElement != pE;
	    }
	}
      pE = pE->ElNext;	/* on traite l'element suivant */
    }
}



/* ---------------------------------------------------------------------- */
/* |	ApplNouvRegle applique au pave courant la regle de		| */
/* |		presentation specifique qui vient d'etre creee.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ApplNouvRegle(PtrDocument pDoc, PtrPRule pRule, PtrElement pEl)
#else /* __STDC__ */
void ApplNouvRegle(pDoc, pRule, pEl)
	PtrDocument pDoc;
	PtrPRule pRule;
	PtrElement pEl;
#endif /* __STDC__ */
{
  PtrAbstractBox        pAb;
  int            vue;
  boolean        stop;
#ifdef __COLPAGE__
 boolean bool;
#endif /* __COLPAGE__ */
  
    /* parcourt toutes les vues du document */
    for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
      if (pEl->ElAbstractBox[vue - 1] != NULL)
	/* l'element traite' a un pave dans cette vue */
	if (pDoc->DocView[vue - 1].DvSSchema == pDoc->DocSSchema &&
	    pDoc->DocView[vue - 1].DvPSchemaView == pRule->PrViewNum)
	  {
	    /* c'est une vue de meme type que la vue traitee, on */
	    /* traite le pave de l'element dans cette vue */
	    pAb = pEl->ElAbstractBox[vue - 1];
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
	      if (ApplyRule(pRule, NULL, pAb, pDoc, NULL, &bool))
#else /* __COLPAGE__ */
	      if (ApplyRule(pRule, NULL, pAb, pDoc, NULL))
#endif /* __COLPAGE__ */
		{
		  switch (pRule->PrType)
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
		  ApplHerit(pRule->PrType, pAb, pDoc);
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
void ModifGraphiques(PtrElement pEl, PtrDocument pDoc, int VueTraitee, boolean ChngStyleTrait, char StyleTrait, boolean ChngEpaisTrait, int EpaisTrait, TypeUnit EpaisTraitUnit, boolean ChngTrame, int Trame, boolean ChngCoulFond, int CoulFond, boolean ChngCoulTrace, int CoulTrace)

#else /* __STDC__ */
void ModifGraphiques(pEl, pDoc, VueTraitee, ChngStyleTrait, StyleTrait, ChngEpaisTrait, EpaisTrait, EpaisTraitUnit, ChngTrame, Trame, ChngCoulFond, CoulFond, ChngCoulTrace, CoulTrace)
	PtrElement pEl;
	PtrDocument pDoc;
	int VueTraitee;
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
	boolean		nouveau;
	PtrPRule	pRule;
	int		viewSch;
  
  viewSch = AppliedView(pEl, NULL, pDoc, VueTraitee); /* numero de cette vue */
  /* style des traits dans le graphique */
  if (ChngStyleTrait)
    {
      /*cherche la regle de presentation specifique 'StyleTrait' de l'element*/
      /* ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtLineStyle, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtLineStyle;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrChrValue = StyleTrait;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* epaisseur des traits dans le graphique */
  if (ChngEpaisTrait)
    {
      /* cherche la regle de presentation specifique 'Epaisseur Trait' de */
      /* l'element ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtLineWeight, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtLineWeight;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrMinUnit = EpaisTraitUnit;
        pRule->PrMinAttr = FALSE;
        pRule->PrMinValue = EpaisTrait;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* trame de remplissage */
  if (ChngTrame)
    {
      /* cherche la regle de presentation specifique 'Trame' de l'element */
      /* ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtFillPattern, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtFillPattern;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrIntValue = Trame;
        pRule->PrAttrValue = FALSE;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* couleur de fond */
  if (ChngCoulFond)
    {
      /* cherche la regle de presentation specifique 'CouleurFond' de */
      /* l'element ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtBackground, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtBackground;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrIntValue = CoulFond;
        pRule->PrAttrValue = FALSE;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* couleur du trace' */
  if (ChngCoulTrace)
    {
      /* cherche la regle de presentation specifique 'CouleurTrace' de */
      /* l'element ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtForeground, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtForeground;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrIntValue = CoulTrace;
        pRule->PrAttrValue = FALSE;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	SupprPres supprime toutes les regles de presentation specifiques| */
/* |		associees a l'element pEl.				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void SupprPres(PtrElement pEl, PtrDocument pDoc, RuleSet ERegles, int VueTraitee)
#else /* __STDC__ */
static void SupprPres(pEl, pDoc, ERegles, VueTraitee)
	PtrElement pEl;
	PtrDocument pDoc;
	RuleSet ERegles;
	int VueTraitee;
#endif /* __STDC__ */
{
  PtrPRule    pRule, pR, pRS;
  PRuleType       TypeRegleP;
  int             viewSch;
  NotifyPresentation notifyPres;

  viewSch = AppliedView(pEl, NULL, pDoc, VueTraitee); /* type de cette vue */
  pRule = pEl->ElFirstPRule;
  pR = NULL;		
  /* parcourt les regles de presentation specifiques de l'element */
  while (pRule != NULL)
    if (pRule->PrViewNum != viewSch || !RuleSetIn(pRule->PrType, ERegles))
      /* cette regle n'est pas concernee */
      {
	pR = pRule;
	pRule = pR->PrNextPRule;
      } 
    else
      {
	/* la regle concerne la vue traitee */
	/* retire la regle de la chaine des regles de presentation */
	/* specifique de l'element */
	pRS = pRule->PrNextPRule;	/* regle a traiter apres */
	notifyPres.event = TtePRuleDelete;
	notifyPres.document = (Document)IdentDocument(pDoc);
	notifyPres.element = (Element)pEl;
	notifyPres.pRule = (PRule)pRule;
	notifyPres.pRuleType = NumTypePRuleAPI(pRule->PrType);
	if (!CallEventType((NotifyEvent *)&notifyPres, TRUE))
	  {
	  if (pR == NULL)
	    pEl->ElFirstPRule = pRS;
	  else
	    pR->PrNextPRule = pRS;
	  TypeRegleP = pRule->PrType;	
	  /* libere la regle */
	  FreeReglePres(pRule);
	  pDoc->DocModified = TRUE;	/* le document est modifie' */
	  /* applique la regle standard de meme type que la regle courante */
	  /* aux paves de l'element qui existent dans les vues de meme type */
	  /* que la vue active. */
	  AppliqueRegleStandard(pEl, pDoc, TypeRegleP, viewSch);
	  notifyPres.event = TtePRuleDelete;
	  notifyPres.document = (Document)IdentDocument(pDoc);
	  notifyPres.element = (Element)pEl;
	  notifyPres.pRule = NULL;
	  notifyPres.pRuleType = NumTypePRuleAPI(TypeRegleP);
	  CallEventType((NotifyEvent *)&notifyPres, FALSE);
	  }
	/* passe a la regle suivante */
	pRule = pRS;
      }
}

/* ---------------------------------------------------------------------- */
/* |	ChangeCouleur change la presentation specifique de la couleur	| */
/* |	de fond ou de trace' (selon Fond) pour tous les elements de la	| */
/* |	selection courante.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void ChangeCouleur(int numCouleur, boolean Fond)

#else /* __STDC__ */
void ChangeCouleur(numCouleur, Fond)
	int numCouleur;
	boolean Fond;
#endif /* __STDC__ */

{
  PtrDocument	SelDoc;
  PtrElement	PremSel, DerSel, pEl;
  PtrAbstractBox	pAb;
  int		premcar, dercar;
  boolean	selok, ChangeTrame;
  int		numTrame;
  RuleSet       LesRegles;

  /* demande quelle est la selection courante */
  selok = GetCurrentSelection(&SelDoc, &PremSel, &DerSel, &premcar, &dercar);
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
      if (premcar > 1 || dercar > 0)
        CutSelection(SelDoc, &PremSel, &DerSel, &premcar, &dercar);
      /* parcourt les elements selectionnes */
      pEl = PremSel;
      while (pEl != NULL)
        {
	  /* on saute les elements qui sont des copies */
	  if (!pEl->ElIsCopy)
	    /* on saute les elements non modifiables */
	    if (!ElementIsReadOnly(pEl))
	      /* on saute les marques de page */
	      if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
		{
		  ChangeTrame = FALSE;
		  numTrame = 0;
		  if (Fond)
		    /* on change la couleur de fond avec la souris */
		    {
		      pAb = PaveDeElem(pEl, SelectedView);
		      if (pAb != NULL)
			if (pAb->AbFillPattern < 2)
			  /* on force la trame backgroundcolor si la trame du pave */
			  /* est nopattern ou foregroundcolor */
			  {
			    ChangeTrame = TRUE;
			    numTrame = 2;
			  }
		    }
		  if (numCouleur == -1)
		    {
		      /* Couleur standard */
		      RuleSetClr(LesRegles);
		      if (Fond)
			{
			  RuleSetPut(LesRegles, PtFillPattern);
			  RuleSetPut(LesRegles, PtBackground);
			}
		      else
			RuleSetPut(LesRegles, PtForeground);
		      SupprPres(pEl, SelDoc, LesRegles, SelectedView);
		    }
		  else
		    ModifGraphiques(pEl, SelDoc, SelectedView, FALSE, ' ', FALSE, 0, FALSE,
				  ChangeTrame, numTrame, Fond, numCouleur, !Fond, numCouleur);
		  /* si on est dans un element copie' par inclusion,   */
		  /* on met a jour les copies de cet element. */
		  RedisplayCopies(pEl, SelDoc, TRUE);
		}
	  /* cherche l'element a traiter ensuite */
	  pEl = NextInSelection(pEl, DerSel);
        }
      /* tente de fusionner les elements voisins et reaffiche les paves */
      /* modifie's et la selection */
      MergeAndSelect(SelDoc, PremSel, DerSel, premcar, dercar);
    }
}


/* ---------------------------------------------------------------------- */
/* |	ModifCaracteres	applique a l'element pEl les modifications sur	| */
/* |		les caracteres demandes par l'utilisateur.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ModifCaracteres(PtrElement pEl, PtrDocument pDoc, int VueTraitee, boolean ChngFamille, char Famille, boolean ChngStyle, int Style, boolean ChngCorps, int Corps, boolean ChngSouligne, int SoulStyle, boolean ChngEpais, int SoulEpais)

#else /* __STDC__ */
void ModifCaracteres(pEl, pDoc, VueTraitee, ChngFamille, Famille, ChngStyle, Style, ChngCorps, Corps, ChngSouligne, SoulStyle, ChngEpais, SoulEpais)
	PtrElement pEl;
	PtrDocument pDoc;
	int VueTraitee;
	boolean ChngFamille;
	char Famille;
	boolean ChngStyle;
	int Style;
	boolean ChngCorps;
	int Corps;
	boolean ChngSouligne;
	int SoulStyle;
	boolean ChngEpais;
	int SoulEpais;
#endif /* __STDC__ */

{
	boolean		nouveau;
	PtrPRule	pRule;
	int		viewSch;
  
  viewSch = AppliedView(pEl, NULL, pDoc, VueTraitee); /* numero de cette vue*/
  /* applique les choix de l'utilisateur */
  /* Famille de polices de caracteres */
  if (ChngFamille)
    {
      /* cherche la regle de presentation specifique 'Fonte' de l'element */
      /* ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtFont, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtFont;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrChrValue = Famille;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* Style de caracteres */
  if (ChngStyle)
    {
      /* cherche la regle de presentation specifique 'Style' de l'element */
      /* ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtStyle, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtStyle;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        switch (Style)
	  {	
	case 0:
	  pRule->PrChrValue = 'R';
	  break;
	case 1:
	  pRule->PrChrValue = 'B';
	  break;
	case 2:
	  pRule->PrChrValue = 'I';
	  break;
	case 3:
          pRule->PrChrValue = 'O'; /* oblique */
          break;
	case 4:
          pRule->PrChrValue = 'G'; /* gras italique */
          break;
	case 5:
	  pRule->PrChrValue = 'Q'; /* gras Oblique */
          break;
	default:
	  pRule->PrChrValue = 'R';
	  break;
	  }
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* Corps des caracteres */
  if (ChngCorps)
    {
      /* cherche la regle de presentation specifique 'Corps' de l'element */
      /* ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtSize, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtSize;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrMinUnit = UnPoint;
        pRule->PrMinAttr = FALSE;
        pRule->PrMinValue = Corps;
        pDoc->DocModified = TRUE; /* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
        PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }

  /* Souligne' */
  if (ChngSouligne)
    {
      /* cherche la regle de presentation specifique 'Souligne' de l'element */
      /* ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtUnderline, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtUnderline;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        switch (SoulStyle)
	  {
	case 0:
	  pRule->PrChrValue = 'N'; /* sans souligne */
	  break;
	case 1:
	  pRule->PrChrValue = 'U'; /* souligne continu */
	  break;
	case 2:
	  pRule->PrChrValue = 'O'; /* surligne */
	  break;
	case 3:
	  pRule->PrChrValue = 'C'; /* biffer */
	  break;
	default:
	  pRule->PrChrValue = 'N';
	  break;
	  }
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* Epaisseur du souligne */
  if (ChngEpais)
    {
      /* cherche la regle de presentation specifique SoulEpais de l'element */
      /* ou en cree une nouvelle */
      pRule = SearchPresRule(pEl, PtThickness, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        /* met les choix de l'utilisateur dans cette regle */
        pRule->PrType = PtThickness;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        switch (SoulEpais)
	  {
	case 0:
	  pRule->PrChrValue = 'N'; /* souligne mince */
	  break;
	case 1:
	  pRule->PrChrValue = 'T'; /* souligne epais */
	  break;
	default:
	  pRule->PrChrValue = 'N';
	  break;
	  }
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        /* si le pave existe, applique la nouvelle regle au pave */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	ModifLignes applique a l'element pEl les modifications		| */
/* |		sur la mise en ligne demandes par l'utilisateur.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ModifLignes(PtrElement pEl, PtrDocument pDoc, int VueTraitee, boolean ChngCadr, int Cadr, boolean ChngJustif, boolean Justif, boolean ChngIndent, int ValIndent, boolean ChngInterL, int InterLigne, boolean ChngHyphen, boolean Hyphenate)
#else /* __STDC__ */
void ModifLignes(pEl, pDoc, VueTraitee, ChngCadr, Cadr, ChngJustif, Justif, ChngIndent, ValIndent, ChngInterL, InterLigne, ChngHyphen, Hyphenate)
	PtrElement pEl;
	PtrDocument pDoc;
	int VueTraitee;
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
  boolean         nouveau;
  PtrPRule    pRule;
  int             viewSch;
  
  viewSch = AppliedView(pEl, NULL, pDoc, VueTraitee); /* Le type de cette vue */
  /* applique les choix de l'utilisateur */
  if (ChngCadr && Cadr > 0)
    {
      pRule = SearchPresRule(pEl, PtAdjust, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        pRule->PrType = PtAdjust;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        switch (Cadr)
	  {
	case 1:
	  pRule->PrAdjust = AlignLeft;
	  break;
	case 2:
	  pRule->PrAdjust = AlignRight;
	  break;
	case 3:
	  pRule->PrAdjust = AlignCenter;
	  break;
	case 4:
	  pRule->PrAdjust = AlignLeftDots;
	  break;
	default:
	  pRule->PrAdjust = AlignLeft;
	  break;
	  }
        pDoc->DocModified = TRUE;	 /* le document est modifie' */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* Justification */
  if (ChngJustif)
    {
      pRule = SearchPresRule(pEl, PtJustify, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        pRule->PrType = PtJustify;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrJustify = Justif;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* Coupure des mots */
  if (ChngHyphen)
    {
      pRule = SearchPresRule(pEl, PtHyphenate, &nouveau, pDoc,VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        pRule->PrType = PtHyphenate;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrJustify = Hyphenate;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        ApplNouvRegle(pDoc, pRule, pEl);
        PRuleMessagePost(pEl, pRule, pDoc, nouveau);
        }
    }
  /* Renfoncement de la 1ere ligne */
  if (ChngIndent)
    {
      pRule = SearchPresRule(pEl, PtIndent, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        pRule->PrType = PtIndent;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrMinUnit = UnPoint;
        pRule->PrMinAttr = FALSE;
        pRule->PrMinValue = ValIndent;
        pDoc->DocModified = TRUE;	
        /* le document est modifie' */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
  /* Interligne */
  if (ChngInterL)
    {
      pRule = SearchPresRule(pEl, PtLineSpacing, &nouveau, pDoc, VueTraitee);
      if (!PRuleMessagePre(pEl, pRule, pDoc, nouveau))
        {
        pRule->PrType = PtLineSpacing;
        pRule->PrViewNum = viewSch;
        pRule->PrPresMode = PresImmediate;
        pRule->PrMinUnit = UnPoint;
        pRule->PrMinAttr = FALSE;
        pRule->PrMinValue = InterLigne;
        pDoc->DocModified = TRUE;	/* le document est modifie' */
        ApplNouvRegle(pDoc, pRule, pEl);
	PRuleMessagePost(pEl, pRule, pDoc, nouveau);
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	AppliqueRegleStandard	applique a l'element pEl du document	| */
/* |	pDoc la regle de presentation standard de type TypeRegleP	| */
/* |	pour la vue viewSch.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void AppliqueRegleStandard(PtrElement pEl, PtrDocument pDoc, PRuleType TypeRegleP, int viewSch)
#else /* __STDC__ */
void AppliqueRegleStandard(pEl, pDoc, TypeRegleP, viewSch)
	PtrElement pEl;
	PtrDocument pDoc;
	PRuleType TypeRegleP;
	int viewSch;
#endif /* __STDC__ */
{
	PtrPRule    pRP;
	PtrAbstractBox         pAb;
	PtrPSchema      pSPR;
	PtrAttribute     pAttr;
	boolean         stop;
	PtrAbstractBox         pPa1;
	int             vue;
#ifdef __COLPAGE__
 boolean bool;
#endif /* __COLPAGE__ */

	/* applique la regle standard de meme type que la regle courante */
	/* aux paves de l'element qui existent dans les vues de meme type */
	/* que la vue active. */
	pRP = NULL; 
	/* on n'a pas encore cherche' la regle standard */
	if (viewSch > 0)        
	  /* il y a une vue active */
	  /* parcourt toutes les vues du document */
	  for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	    if (pEl->ElAbstractBox[vue -1] != NULL)
	      /* l'element traite' a un pave dans cette vue */
	      if (pDoc->DocView[vue - 1].DvSSchema == pDoc->DocSSchema &&
		  pDoc->DocView[vue - 1].DvPSchemaView == viewSch)
		/* c'est une vue de meme type que la vue traitee, on */
		/* traite le pave de l'element dans cette vue */
		{
		  pAb = pEl->ElAbstractBox[vue -1];
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
			pRP = SearchRulepAb(pDoc, pAb, &pSPR, TypeRegleP, TRUE, &pAttr);
#ifdef __COLPAGE__
	              ApplyRule(pRP, pSPR, pAb, pDoc, pAttr, &bool);
#else /* __COLPAGE__ */
	              ApplyRule(pRP, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
			/* marque que le pave a change' et doit etre reaffiche' */
			  pPa1 = pAb;
			  switch (TypeRegleP)
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
			  ApplHerit(TypeRegleP, pAb, pDoc);
		    }
		}
}

/* ---------------------------------------------------------------------- */
/* |	SupprPresSpec supprime les regles de presentation specifiques	| */
/* |		contenues dans 'Regles' attachees aux elements du	| */
/* |		sous-arbre de racine pSA				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void SupprPresSpec(PtrElement pSA, PtrDocument pDoc, RuleSet Regles, int VueTraitee)
#else /* __STDC__ */
void SupprPresSpec(pSA, pDoc, Regles, VueTraitee)
	PtrElement pSA;
	PtrDocument pDoc;
	RuleSet Regles;
	int VueTraitee;
#endif /* __STDC__ */
{
  PtrElement      pEl;	/* traite la racine du sous-arbre */
  
  SupprPres(pSA, pDoc, Regles, VueTraitee);
  if (!pSA->ElTerminal)
    {
      /* passe au premier fils */
      pEl = pSA->ElFirstChild;
      while (pEl != NULL)
	{
	  /* traite le sous-arbre de ce fils */
	  SupprPresSpec(pEl, pDoc, Regles, VueTraitee);
	  /* passe au fils suivant */
	  pEl = pEl->ElNext;
	}
    }
}
/* End Of Module modpres */
