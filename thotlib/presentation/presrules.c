
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
  pres.c -- gestion des regles de presentation de l'image abstraite.
  Ce module applique les regles de presentation aux paves.	
  V. Quint	 Octobre 1984	
  France Logiciel no de depot 88-39-001-00

  Major changes:
  IV : Mai 92   adaptation Tool Kit
 */

#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appdialogue.h"

#define EXPORT extern
#include "platform_tv.h"
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "appdialogue_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "absboxes_f.h"
#include "abspictures_f.h"
#include "buildboxes_f.h"
#include "indpres_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "boxpositions_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "boxselection_f.h"
#include "content_f.h"
#include "presvariables_f.h"


/* ---------------------------------------------------------------------- */
/* |	AttrValue retourne la valeur que prend l'attribut numerique	| */
/* |		pointe' par pAttr.					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int AttrValue(PtrAttribute pAttr)
#else /* __STDC__ */
int AttrValue(pAttr)
	PtrAttribute pAttr;
#endif /* __STDC__ */
{
  int ret;
  
  
  ret = 0;
  if (pAttr != NULL)
    {
      if (pAttr->AeAttrType == AtNumAttr)
	ret = pAttr->AeAttrValue;
    }
  return ret;
}


/* ---------------------------------------------------------------------- */
/* |	FollowNotPres Si pAbb pointe un pave de presentation, retourne	| */
/* |		dans pAbb le premier pave qui n'est pas un pave de	| */
/* |		presentation et qui suit le pave pAbb a l'appel.		| */
/* |		Retourne NULL si le pave n'est suivi que de paves de	| */
/* |		presentation. Si, a l'appel, pAbb est  un pave qui n'est	| */
/* |		pas un pave de presentation, alors pAbb reste inchange'.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void FollowNotPres(PtrAbstractBox *pAbb)
#else /* __STDC__ */
static void FollowNotPres(pAbb)
	PtrAbstractBox *pAbb;
#endif /* __STDC__ */
{
  boolean	stop;
  
  
  stop = FALSE;
  do
    if (*pAbb == NULL)
      stop = TRUE;
    else if (!(*pAbb)->AbPresentationBox)
      stop = TRUE;
    else
      *pAbb = (*pAbb)->AbNext;
  while (!stop);
}


/* ---------------------------------------------------------------------- */
/* |	AscentAbsBox	  rend le premier element pElAsc ascendant de pE	| */
/* |			  et qui possede un pave dans la vue view		| */
/* |			  retourne ce pave dans pAbb ou NULL sinon 	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void AscentAbsBox(PtrElement pE, DocViewNumber view, PtrAbstractBox *pAbb, PtrElement *pElAsc)
#else /* __STDC__ */
static void AscentAbsBox(pE, view, pAbb, pElAsc)
	PtrElement pE;
	DocViewNumber view;
	PtrAbstractBox *pAbb;
	PtrElement *pElAsc;
#endif /* __STDC__ */
{
  
  *pElAsc = pE;
  if ((*pElAsc)->ElParent == NULL)
    *pAbb = NULL;
  else
    while ((*pElAsc)->ElParent != NULL && *pAbb == NULL)
      {
	*pElAsc = (*pElAsc)->ElParent;
	*pAbb = (*pElAsc)->ElAbstractBox[view - 1];
	if (*pAbb != NULL)
	  if ((*pAbb)->AbDead)
	    *pAbb = NULL;
      }
}



/* ---------------------------------------------------------------------- */
/* |	AbsBoxInherit  rend le pointeur sur le pave correpondant a l'element	| */
/* |	qui sert de reference quand on applique la regle d'heritage	| */
/* |	pointe par pPRule a l'element pointe par pEl, dans la vue view.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox AbsBoxInherit(PtrPRule pPRule, PtrElement pEl, DocViewNumber view)

#else /* __STDC__ */
static PtrAbstractBox AbsBoxInherit(pPRule, pEl, view)
	PtrPRule pPRule;
	PtrElement pEl;
	DocViewNumber view;
#endif /* __STDC__ */

{
  PtrElement      pElInherit;
  PtrAbstractBox         pAbb;
  
  pAbb = NULL;
  pElInherit = pEl;
  if (pEl != NULL)
    switch (pPRule->PrInheritMode)
      {
      case InheritParent:
	AscentAbsBox(pEl, view, &pAbb, &pElInherit);
	break;
      case InheritGrandFather:
	AscentAbsBox(pEl, view, &pAbb, &pElInherit);
	if (pAbb != NULL)
	  {
	    pElInherit = pAbb->AbElement;
	    pAbb = NULL;
	    AscentAbsBox(pElInherit, view, &pAbb, &pElInherit);
	  }
	break;
      case InheritPrevious:
	while (pElInherit->ElPrevious != NULL && pAbb == NULL)
	  {
	    pElInherit = pElInherit->ElPrevious;
	    pAbb = pElInherit->ElAbstractBox[view - 1];
	    FollowNotPres(&pAbb);	/* saute les paves de presentation */
	    if (pAbb != NULL)
	      if (pAbb->AbDead)
		pAbb = NULL;
	  }
	if (pAbb == NULL)
	  AscentAbsBox(pEl, view, &pAbb, &pElInherit);
	break;
      case InheritChild:
	while (!pElInherit->ElTerminal && pElInherit->ElFirstChild != NULL && pAbb == NULL)
	  {
	    pElInherit = pElInherit->ElFirstChild;
	    pAbb = pElInherit->ElAbstractBox[view - 1];
	    FollowNotPres(&pAbb);	/* saute les paves de presentation */
	    if (pAbb != NULL)
	      if (pAbb->AbDead)
		pAbb = NULL;
	  }
	break;
      case InheritCreator:
	pAbb = pEl->ElAbstractBox[view - 1];
	if (pAbb != NULL)
	  {
	    FollowNotPres(&pAbb);	/* saute les paves de presentation */
	    if (pAbb != NULL)
	      if (pAbb->AbDead)
	        pAbb = NULL;
	  }
	break;
      }
  
  FollowNotPres(&pAbb);	/* saute les paves de presentation */
  return pAbb;
}


/* ---------------------------------------------------------------------- */
/* |	AbsBoxInheritImm  rend le pointeur sur le pave correspondant a	| */
/* |		l'element qui sert de reference quand on applique la	| */
/* |		regle d'heritage pointe par pPRule a l'element pointe	| */
/* |		par pEl, dans la vue view. On ne considere que l'element	| */
/* |		immediatement voisin (pere, frere, fils).		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox AbsBoxInheritImm(PtrPRule pPRule, PtrElement pEl, DocViewNumber view)

#else /* __STDC__ */
static PtrAbstractBox AbsBoxInheritImm(pPRule, pEl, view)
	PtrPRule pPRule;
	PtrElement pEl;
	DocViewNumber view;
#endif /* __STDC__ */

{
  PtrAbstractBox         pAbb;
  
  pAbb = NULL;
  if (pEl != NULL)
    switch (pPRule->PrInheritMode)
      {
      case InheritParent:
  	if (pEl->ElParent != NULL)
    	  pAbb = pEl->ElParent->ElAbstractBox[view - 1];
	break;
      case InheritGrandFather:
	if (pEl->ElParent != NULL)
	  if (pEl->ElParent->ElParent != NULL)
	    pAbb = pEl->ElParent->ElParent->ElAbstractBox[view - 1];
	break;
      case InheritPrevious:
	if (pEl->ElPrevious != NULL)
	  pAbb = pEl->ElPrevious->ElAbstractBox[view - 1];
	if (pAbb == NULL)
  	  if (pEl->ElParent != NULL)
    	    pAbb = pEl->ElParent->ElAbstractBox[view - 1];
	break;
      case InheritChild:
	if (!pEl->ElTerminal)
	  if (pEl->ElFirstChild != NULL)
	    pAbb = pEl->ElFirstChild->ElAbstractBox[view - 1];
	break;
      case InheritCreator:
	pAbb = pEl->ElAbstractBox[view - 1];
	if (pAbb != NULL)
	  {
	    FollowNotPres(&pAbb);	/* saute les paves de presentation */
	    if (pAbb->AbDead)
	      pAbb = NULL;
	  }
	break;
      }
  
  FollowNotPres(&pAbb);
  if (pAbb != NULL)
    if (pAbb->AbDead)
      pAbb = NULL;
  return pAbb;
}



/* ---------------------------------------------------------------------- */
/* |	CharRule evalue une regle de presentation de type caractere	| */
/* |		pour la vue view. La regle a evaluer est pointee par	| */
/* |		pPRule, et l'element auquel elle s'applique est pointe	| */
/* |		par pEl. Au result, ok indique si l'evaluation a pu	| */
/* |		etre faite. 						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static char CharRule(PtrPRule pPRule, PtrElement pEl, DocViewNumber view, boolean *ok)

#else /* __STDC__ */
static char CharRule(pPRule, pEl, view, ok)
	PtrPRule pPRule;
	PtrElement pEl;
	DocViewNumber view;
	boolean *ok;
#endif /* __STDC__ */

{
  PtrAbstractBox         pAbb;
  char            val;
  PtrAbstractBox         pAbba1;
  
  val = ' ';
  *ok = TRUE;
  if (pPRule != NULL && pEl != NULL)
    {
      switch (pPRule->PrPresMode)
	{
	case PresInherit:
	  pAbb = AbsBoxInherit(pPRule, pEl, view);
	  if (pAbb == NULL)
	    *ok = FALSE;
	  else
	    {
	      pAbba1 = pAbb;
	      switch (pPRule->PrType)
		{
	  case PtFont:
		  val = pAbba1->AbFont;
		  break;
	  case PtStyle:
		    if (pAbba1->AbHighlight == 1)
		      val = 'B';
		    else if (pAbba1->AbHighlight == 2)
		      val = 'I';
		    else if (pAbba1->AbHighlight == 3)  /* O : oblique */
                      val = 'O';
		    else if (pAbba1->AbHighlight == 4)  /* G : gras italique */
                      val = 'G';
		    else if (pAbba1->AbHighlight == 5)  /* Q : gras oblique */
                      val = 'Q';
		    else
		      val = 'R';
		  break;
	  case PtUnderline:
		    if (pAbba1->AbUnderline == 1)
		      val = 'U';
		    else if (pAbba1->AbUnderline == 2)
		      val = 'O';
		    else if (pAbba1->AbUnderline == 3)
		      val = 'C';		
		    else
		      val = 'N';
		  break;
		  
	  case PtThickness:
		    if (pAbba1->AbThickness == 1)
		      val = 'T';
		    else
		      val = 'N';
		  break;
	  case PtLineStyle:
		     val = pAbba1->AbLineStyle;
		  break;
	  default:
		  break;
		}
	      
	    }
	  break;
  case PresFunction:
	  
	  break;
  case PresImmediate:
	  val = pPRule->PrChrValue;
	  break;
	}
    }
  return val;
}


/* ---------------------------------------------------------------------- */
/* |	AlignRule evalue une regle d'ajustement pour la vue view.	| */
/* |		La regle a evaluer est pointee par pPRule, et l'element	| */
/* |		auquel elle s'applique est pointe par pEl.		| */
/* |		Au result, ok indique si l'evaluation a pu etre faite.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static BAlignment AlignRule(PtrPRule pPRule, PtrElement pEl, DocViewNumber view, boolean *ok)

#else /* __STDC__ */
static BAlignment AlignRule(pPRule, pEl, view, ok)
	PtrPRule pPRule;
	PtrElement pEl;
	DocViewNumber view;
	boolean *ok;
#endif /* __STDC__ */

{
  PtrAbstractBox         pAbb;
  BAlignment         val;
  
  val = AlignLeft;
  *ok = TRUE;
  if (pPRule != NULL && pEl != NULL)
    {
      switch (pPRule->PrPresMode)
	{
  case PresInherit:
	  pAbb = AbsBoxInherit(pPRule, pEl, view);
	  if (pAbb == NULL)
	    *ok = FALSE;
	  else
	    if (pPRule->PrType == PtAdjust)
	      val = pAbb->AbAdjust;
	  break;
  case PresFunction:
	  break;
  case PresImmediate:
	  if (pPRule->PrType == PtAdjust)
	    val = pPRule->PrAdjust;
	  break;
	}
      
    }
  return val;
}



/* ---------------------------------------------------------------------- */
/* |	 BoolRule evalue une regle de presentation de type booleen	| */
/* |		pour la vue view. La regle a evaluer est pointee par	| */
/* |		pPRule, et l'element auquel elle s'applique est pointe	| */
/* |		par pEl.						| */
/* |		Au result, ok indique si l'evaluation a pu etre faite.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean BoolRule(PtrPRule pPRule, PtrElement pEl, DocViewNumber view, boolean *ok)

#else /* __STDC__ */
static boolean BoolRule(pPRule, pEl, view, ok)
	PtrPRule pPRule;
	PtrElement pEl;
	DocViewNumber view;
	boolean *ok;
#endif /* __STDC__ */

{
  PtrAbstractBox         pAbb;
  boolean         val;
  
  val = FALSE;
  *ok = TRUE;
  if (pPRule != NULL && pEl != NULL)
    {
      switch (pPRule->PrPresMode)
	{
  case PresInherit:
	  pAbb = AbsBoxInherit(pPRule, pEl, view);
	  if (pAbb == NULL)
	    *ok = FALSE;
	  else if (pPRule->PrType == PtJustify)
	    val = pAbb->AbJustify;
	  else if (pPRule->PrType == PtHyphenate)
	    val = pAbb->AbHyphenate;
	  break;
  case PresFunction:
	  break;
  case PresImmediate:
	  if (pPRule->PrType == PtJustify || pPRule->PrType == PtHyphenate)
	    val = pPRule->PrJustify;
	  break;
	}
      
    }
  return val;
}


/* ---------------------------------------------------------------------- */
/* |	valintregle evalue une regle de presentation de type entier pour| */
/* |		la vue view. La regle a evaluer est pointee par pPRule,	| */
/* |		et l'element auquel elle s'applique est pointe par pEl.	| */
/* |		Au result, ok indique si l'evaluation a pu etre faite et| */
/* |		unit indique, dans le cas de regle PtIndent, PtBreak1,	| */
/* |		PtBreak2 ou PtSize, si la valeur est exprimee en	| */
/* |		points typo, en relatif (numero de corps si PtSize),	| */
/* |		etc. Si la regle est une regle de presentation		| */
/* |		d'attribut, pAttr pointe sur le bloc d'attribut auquel	| */
/* |		la regle correspond.					| */
/* |		Fonction utilisee dans crimabs				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int valintregle(PtrPRule pPRule, PtrElement pEl, DocViewNumber view, boolean *ok, TypeUnit *unit, PtrAttribute pAttr)
#else /* __STDC__ */
int valintregle(pPRule, pEl, view, ok, unit, pAttr)
	PtrPRule pPRule;
	PtrElement pEl;
	DocViewNumber view;
	boolean *ok;
	TypeUnit *unit;
	PtrAttribute pAttr;
#endif /* __STDC__ */
{
  PtrAbstractBox         pAbb;
  int             val, i;
  PtrElement      pElInherit;

  val = 0;
  *ok = TRUE;
  *unit = UnRelative;
  if (pPRule != NULL && pEl != NULL)
    {
      switch (pPRule->PrPresMode)
	{
	case PresInherit:
	  if (pPRule->PrType == PtVisibility)
	    pAbb = AbsBoxInheritImm(pPRule, pEl, view);
	  else
	    pAbb = AbsBoxInherit(pPRule, pEl, view);
	  if (pAbb == NULL)
	    *ok = FALSE;
	  else
	    {
	      if (pPRule->PrInhAttr)
		{	
		  /* c'est la valeur d'un attribut */ 
		  if (pPRule->PrInhDelta < 0)
		    /* il faut retrancher cette valeur */
		    i = -AttrValue(pAttr);
		  else	
		    /* il faut ajouter cette valeur */
		    i = AttrValue(pAttr);
		  if (pPRule->PrInhUnit == UnRelative || pPRule->PrInhUnit == UnXHeight)
		    if (pPRule->PrType == PtIndent
			|| pPRule->PrType == PtLineSpacing
			|| pPRule->PrType == PtLineWeight)
		      /* coviewertit en 1/10 de caractere */
		      i = 10 * i;
		}
	      else	
		/* c'est la valeur elle meme qui est dans la regle */
		i = pPRule->PrInhDelta;

	      switch (pPRule->PrType)
		{
		case PtVisibility:
		  val = pAbb->AbVisibility + i;
		  break;
		case PtSize:
		  val = pAbb->AbSize + i;
		  *unit = pAbb->AbSizeUnit;
		  if (*unit == UnRelative)
		    if (val > MAX_LOG_SIZE)
		      val = MAX_LOG_SIZE;
		    else if (val < 0)
		      val = 0;
		  break;
		case PtIndent:
		  val = pAbb->AbIndent + i;
		  *unit = pAbb->AbIndentUnit;
		  break;
		case PtLineSpacing:
		  val = pAbb->AbLineSpacing + i;
		  *unit = pAbb->AbLineSpacingUnit;
		  break;
		case PtDepth:
		  val = pAbb->AbDepth + i;
		  break;
		case PtFillPattern:
		  val = pAbb->AbFillPattern;
		  break;
		case PtBackground:
		  val = pAbb->AbBackground;
		  break;
		case PtForeground:
		  val = pAbb->AbForeground;
		  break;
		case PtLineWeight:
		  val = pAbb->AbLineWeight + i;
		  if (val < 0)
		    val = 0;
		  *unit = pAbb->AbLineWeightUnit;
		  break;
		default:
		  break;
		}
	      
	      if (pPRule->PrInhMinOrMax != 0)
		/* il y a un minimum ou un maximum a respecter */
		{
		  if (pPRule->PrMinMaxAttr)	
		    /* c'est la valeur d'un attribut */ 
		    {
		      if (pPRule->PrInhMinOrMax < 0)
			/* inverser cette valeur */
			i = -AttrValue(pAttr);
		      else
			i = AttrValue(pAttr);
		      if (pPRule->PrInhUnit == UnRelative ||
			  pPRule->PrInhUnit == UnXHeight)
			if (pPRule->PrType == PtIndent || pPRule->PrType == PtLineSpacing ||
			    pPRule->PrType == PtLineWeight)
			  /* convertit en 1/10 de caractere */
			  i = 10 * i;
		    }
		  else	
		    /* c'est la valeur elle meme qui est dans la regle */
		    i = pPRule->PrInhMinOrMax;
		  if (pPRule->PrInhDelta >= 0)	
		    /* c'est un maximum */
		    /* dans les paves, les tailles relatives sont */
		    /* exprimees dans une echelle de valeurs entre 0 et */
		    /* n-1, alors que dans les regles de presentation */
		    /* l'echelle est entre 1 et n. */
		    if (pPRule->PrType == PtSize && pAbb->AbSizeUnit == UnRelative)
		      {
			if (val > i - 1)
			  val = i - 1;
		      }
		    else 
		      {
			if (val > i)
			  val = i;
		      }
		  else	
		    /* c'est un minimum */
		    /* dans les paves, les tailles relatives sont */
		    /* exprimees dans une echelle de valeurs entre 0 et */
		    /* n-1, alors que dans les regles de presentation */
		    /* l'echelle est entre 1 et n. */
		    if (pPRule->PrType == PtSize && pAbb->AbSizeUnit == UnRelative)
		      {
			if (val < i - 1)
			  val = i - 1;
		      }
		    else if (val < i)
		      val = i;
		}
	    }
	  break;
	case PresFunction:
	  break;
	case PresImmediate:
	  if (pPRule->PrType == PtVisibility
	      || pPRule->PrType == PtDepth
	      || pPRule->PrType == PtFillPattern
	      || pPRule->PrType == PtBackground
	      || pPRule->PrType == PtForeground)
	    if (pPRule->PrAttrValue)
	      /* c'est la valeur d'un attribut */ 
	      if (pPRule->PrIntValue < 0)
		/* il faut inverser cette valeur */
		val = -AttrValue(pAttr);
	      else
		val = AttrValue(pAttr);
	    else	
	      /* c'est la valeur elle meme qui est dans la regle */
	      val = pPRule->PrIntValue;
	  
	  else if (pPRule->PrType == PtBreak1
		   || pPRule->PrType == PtBreak2
		   || pPRule->PrType == PtIndent
		   || pPRule->PrType == PtSize
		   || pPRule->PrType == PtLineSpacing
		   || pPRule->PrType == PtLineWeight)
	    {
	      if (pPRule->PrMinAttr)	
		/* c'est la valeur d'un attribut */
		{
		  if (pPRule->PrMinValue < 0)
		    /* il faut inverser cette valeur */
		    val = -AttrValue(pAttr);
		  else
		    val = AttrValue(pAttr);
		  if (pPRule->PrMinUnit == UnRelative
		      || pPRule->PrMinUnit == UnXHeight)
		    if (pPRule->PrType != PtSize)
		      /* convertit en 1/10 de caractere */
		      val = val * 10;
		}
	      else
		/* c'est la valeur elle-meme qui est dans la regle */
		val = pPRule->PrMinValue;
	      if (pPRule->PrMinUnit == UnPercent && pPRule->PrType != PtIndent)
		{
		  if( pPRule->PrType == PtSize)
		    {
		      AscentAbsBox(pEl, view, &pAbb, &pElInherit);
		      if (pAbb == NULL)
			*ok = FALSE;
		      else
			{
			  val = pAbb->AbSize + i;
			  *unit = pAbb->AbSizeUnit;
			  if (*unit == UnRelative)
			    if (val > MAX_LOG_SIZE)
			      val = MAX_LOG_SIZE;
			    else if (val < 0)
			      val = 0;
			}
		    }
		  else
		    {
		      /* Relative a la police courante */
		      *unit = UnRelative;
		      val = val / 10;
		    }
		}
	      else
		{
		  *unit = pPRule->PrMinUnit;
		  if (pPRule->PrType == PtSize && pPRule->PrMinUnit == UnRelative)
		    val--;
		}
	    }

	  if (pPRule->PrType == PtSize && *unit == UnRelative)
	    {
	      if (val > MAX_LOG_SIZE)
		val = MAX_LOG_SIZE;
	      else if (val < 0)
		val = 0;
	    }
	  else if (pPRule->PrType == PtLineWeight && val < 0)
	    val = 0;
	  break;
	default:
	  break;
	}
    }
  return val;
}

/* ---------------------------------------------------------------------- */
/* |	VerifyAbsBox : Teste si le pave pAb a les caracteristiques		| */
/* |		numAbType (type du pave) et PoRefElem (pave de presentation	| */
/* |		ou d'element) si notType est faux ou s'il n'a pas les	| */
/* |		caracteristiques numAbType et PoRefElem si notType est vrai.| */
/* |		 Cela permet de determiner le pave pAb par rapport	| */
/* |		 auquel le pave en cours de traitement va se positionner| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void VerifyAbsBox(boolean *found, PtrPSchema pSP, boolean PoRefElem, int numAbType, boolean notType, PtrAbstractBox pAb)

#else /* __STDC__ */
static void VerifyAbsBox(found, pSP, PoRefElem, numAbType, notType, pAb)
	boolean *found;
	PtrPSchema pSP;
	boolean PoRefElem;
	int numAbType;
	boolean notType;
	PtrAbstractBox pAb;
#endif /* __STDC__ */

{
  PtrAbstractBox         pAbba1;
  
  pAbba1 = pAb;
  if (!pAbba1->AbDead)
    if (notType) /* on accepte le pave s'il est de type different de numAbType */
      {
	if (PoRefElem)  /* un pave d'un element de structure */
	  {
	    if ((numAbType == MAX_PRES_VARIABLE + 1) && (pAbba1->AbPresentationBox))
	      /* Cas d'une regle Not AnyElem, on accepte la premiere boite */
	      /* de presentation trouvee */
	      *found = TRUE;
	  }
	else  /* un pave d'une boite de pres. */
	  if ((numAbType == MAX_PRES_BOX + 1) && (!pAbba1->AbPresentationBox))
	    /* Cas d'une regle Not AnyBox, on accepte le premier element */
	    /* found' */
	    *found = TRUE;
	/* on est dans une regle differente de Any... */ 
	if (pAbba1->AbTypeNum != numAbType 
	    || pAbba1->AbPresentationBox == PoRefElem 
	    || pAbba1->AbPSchema != pSP)
	  *found = TRUE;
      }
    else 
      {
	if (PoRefElem) /* un pave d'un element de structure */
	  {
	    if ((numAbType == MAX_PRES_VARIABLE + 1) && (!pAbba1->AbPresentationBox))
	    /* Cas d'une regle AnyElem, on accepte le premier element found */
	      *found = TRUE;
	  }
	else  /* un pave d'une boite de pres. */
	  if ((numAbType == MAX_PRES_BOX + 1) && (pAbba1->AbPresentationBox))
	    /* Cas d'une regle AnyBox, on accepte la premiere boite de */
	    /* presentation trouvee */
	    *found = TRUE;
	/* on est dans une regle differente de Any... */ 
	if (pAbba1->AbTypeNum == numAbType 
	    && pAbba1->AbPresentationBox != PoRefElem 
	    && (pAbba1->AbPSchema == pSP 
		|| pSP == NULL))
	  *found = TRUE;
      }
}

/* ---------------------------------------------------------------------- */
/* |	AttrCreatePresBox	retourne Vrai si l'une des regles de	| */
/* |		presentation de l'attribut pointe' par pAttr cree le	| */
/* |		pave de presentation pointe' par pAb.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean AttrCreatePresBox(PtrAttribute pAttr, PtrAbstractBox pAb)

#else /* __STDC__ */
static boolean AttrCreatePresBox(pAttr, pAb)
	PtrAttribute pAttr;
	PtrAbstractBox pAb;
#endif /* __STDC__ */

{
    boolean	     ret, stop;
    PtrPRule     pPRule;
    PtrPSchema	     pSchP;
    PtrHandlePSchema pHd;

    ret = FALSE;
    if (pAb->AbPresentationBox)
      {
      /* on cherchera d'abord dans le schema de presentation principal de */
      /* l'attribut */
      pSchP = pAttr->AeAttrSSchema->SsPSchema;
      pHd = NULL;
      /* on examine le schema de presentation principal, puis les schemas */
      /* additionnels */
      while (pSchP != NULL && !ret)
	  {
	  /* cherche dans ce schema de presentation le debut de la chaine */
	  /* des regles de presentation de l'attribut */
	  pPRule = AttrPresRule(pAttr, pAb->AbElement, FALSE, NULL, pSchP);
	  /* saute les regles precedant les  fonctions */
	  stop = FALSE;
	  do
	    if (pPRule == NULL)
	      stop = TRUE;
	    else if (pPRule->PrType > PtFunction)
	      {
	      /* pas de fonction de presentation */
	      stop = TRUE;
	      pPRule = NULL;
	      }
	    else if (pPRule->PrType == PtFunction)
	      stop = TRUE;
	    else
	      pPRule = pPRule->PrNextPRule;
	  while (!stop);
	  /* cherche toutes les fonctions de creation */
	  stop = FALSE;
	  do
	    if (pPRule == NULL)
	      /* fin de la chaine */
	      stop = TRUE;
	    else if (pPRule->PrType != PtFunction)
	      /* fin des fonctions */
	      stop = TRUE;
	    else
	      /* c'est une regle fonction */
	      {
	      if (pPRule->PrPresFunction == FnCreateBefore ||
		  pPRule->PrPresFunction == FnCreateWith ||
		  pPRule->PrPresFunction == FnCreateEnclosing ||
	          pPRule->PrPresFunction == FnCreateFirst ||
	          pPRule->PrPresFunction == FnCreateAfter ||
	          pPRule->PrPresFunction == FnCreateLast)
	        /* c'est une regle de creation */
	        {
	        if (!pPRule->PrElement &&
		    pPRule->PrPresBox[0] == pAb->AbTypeNum &&
		    pPRule->PrNPresBoxes == 1 &&
		    pSchP == pAb->AbPSchema)
		  /* cette regle cree notre pave, on a found' */
		  {
		  stop = TRUE;
		  ret = TRUE;
		  }
	        }
	      if (!stop)
	        /* passe a la regle suivante de la chaine */
	        pPRule = pPRule->PrNextPRule;
	      }
	  while (!stop);
	  if (!ret)
	     /* on n'a pas encore found'. On cherche dans les schemas de */
	     /* presentation additionnels */
	     {	  
	     if (pHd == NULL)
	       /* on n'a pas encore traite' les schemas de presentation
	       additionnels. On prend le premier schema additionnel. */
	       pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
	     else
	       /* passe au schema additionnel suivant */
	       pHd = pHd->HdNextPSchema;
	     if (pHd == NULL)
	       /* il n'y a pas (ou plus) de schemas additionnels */
	       pSchP = NULL;
	     else
	       pSchP = pHd->HdPSchema;
	     }
 	  }
      }
    return ret;
}

/* ---------------------------------------------------------------------- */
/* |	VerifyAbsBoxDescent : Teste si le pave pAb ou un de ses		| */
/* |		descendants a les caracteristiques			| */
/* |		numAbType (type du pave) et PoRefElem (pave de presentation	| */
/* |		ou d'element) si notType est faux ou s'il n'a pas les	| */
/* |		caracteristiques numAbType et PoRefElem si notType est vrai.| */
/* |		Cela permet de determiner le pave pAb par rapport	| */
/* |		auquel le pave en cours de traitement va se positionner	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void VerifyAbsBoxDescent(boolean *found, PtrPSchema pSP, boolean PoRefElem, int numType, boolean notType, PtrAbstractBox pAb)

#else /* __STDC__ */
static void VerifyAbsBoxDescent(found, pSP, PoRefElem, numType, notType, pAb)
	boolean *found;
	PtrPSchema pSP;
	boolean PoRefElem;
	int numType;
	boolean notType;
	PtrAbstractBox pAb;
#endif /* __STDC__ */

{
  VerifyAbsBox(found, pSP, PoRefElem, numType, notType, pAb);
  if (!(*found))
    if (pAb->AbFirstEnclosed != NULL)
      {
      pAb = pAb->AbFirstEnclosed;
      do
	{
	VerifyAbsBoxDescent(found, pSP, PoRefElem, numType, notType, pAb);
	if (!(*found))
	  pAb = pAb->AbNext;
	}      
      while (!(*found) && pAb != NULL);
      }
}


/* ---------------------------------------------------------------------- */
/* |	PaveRef Si notType est faux, rend un pointeur sur le pave de	| */
/* |		type numAbType et de niveau Niv (relativement au pave	| */
/* |		pAbb). Si notType est vrai, rend un pointeur sur le	| */
/* |		premier pave de niveau Niv (relativement a pAbb) qui n'est| */
/* |		pas de type numAbType.					| */
/* |		Si PoRefElem est vrai, le pave represente par numAbType est	| */
/* |		celui d'un element de la representation interne, sinon	| */
/* |		c'est une boite de presentation definie dans le schema	| */
/* |		de presentation pointe' par pSP.			| */
/* |		Si Niv vaut RlReferred, on cherche parmi les paves de	| */
/* |		l'element designe' par l'attribut reference pointe'	| */
/* |		par pAttr.						| */
/* |		Au result, la fonction rend NULL si aucun pave ne	| */
/* |		correspond.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox PaveRef(boolean notType, int numAbType, PtrPSchema pSP, Level Niv, boolean PoRefElem, PtrAbstractBox pAbb, PtrAttribute pAttr)

#else /* __STDC__ */
static PtrAbstractBox PaveRef(notType, numAbType, pSP, Niv, PoRefElem, pAbb, pAttr)
	boolean notType;
	int numAbType;
	PtrPSchema pSP;
	Level Niv;
	boolean PoRefElem;
	PtrAbstractBox pAbb;
	PtrAttribute pAttr;
#endif /* __STDC__ */

{
  boolean         found;
  PtrAbstractBox         pAb;
  int             vue;
  PtrAbstractBox	  pAbbMain;
  
  pAb = pAbb;
  if (pAb != NULL)
    {
      found = FALSE;
      switch (Niv)
	{
	case RlEnclosing:
	  do
	    {
	      pAb = pAb->AbEnclosing;
	      if (numAbType == 0)
		found = TRUE;
	      else if (pAb != NULL)
		VerifyAbsBox(&found, pSP, PoRefElem, numAbType, notType, pAb);
	    }
	  while (!(pAb == NULL || found));
	  break;

	case RlSameLevel:
	  /* accede au premier pave de ce niveau */
	  if (pAb->AbEnclosing != NULL)
	     pAb = pAb->AbEnclosing->AbFirstEnclosed;
	  /* cherche en avant le pave demande' */
	  do
	    if (numAbType == 0)
	      if (pAb->AbDead)
		pAb = pAb->AbNext;
	      else
		found = TRUE;
	    else
	      {
		VerifyAbsBox(&found, pSP, PoRefElem, numAbType, notType, pAb);
		if (!found)
		  pAb = pAb->AbNext;
	      }
	  while (!(pAb == NULL || found));
	  break;

	case RlEnclosed:
	  pAb = pAb->AbFirstEnclosed;
	  if (pAb != NULL)
	    do
	      if (numAbType == 0)
		if (pAb->AbDead)
		  pAb = pAb->AbNext;
		else
		  found = TRUE;
	      else
		{
		  VerifyAbsBox(&found, pSP, PoRefElem, numAbType, notType, pAb);
		  if (!found)
		    pAb = pAb->AbNext;
		}
	    while (!(pAb == NULL || found));
	  break;

	case RlPrevious:
	  pAb = pAb->AbPrevious;
	  if (pAb != NULL)
	    do
	      if (numAbType == 0)
		if (pAb->AbDead)
		  pAb = pAb->AbPrevious;
		else
		  found = TRUE;
	      else
		{
		  VerifyAbsBox(&found, pSP, PoRefElem, numAbType, notType, pAb);
		  if (!found)
		    pAb = pAb->AbPrevious;
		}
	  while (!(pAb == NULL || found));
	  break;

	case RlNext:
	  pAb = pAb->AbNext;
	  if (pAb != NULL)
	    do
	      if (numAbType == 0)
		if (pAb->AbDead)
		  pAb = pAb->AbNext;
		else
		  found = TRUE;
	      else
		{
		  VerifyAbsBox(&found, pSP, PoRefElem, numAbType, notType, pAb);
		  if (!found)
		    pAb = pAb->AbNext;
		}
	  while (!(pAb == NULL || found));
	  break;

	case RlSelf:
	case RlContainsRef:
	  break;

	case RlRoot:
	  /* on cherche le pave racine de l'image abstraite */
	  while (pAb->AbEnclosing != NULL)
	    pAb = pAb->AbEnclosing;
	  break;

	case RlCreator:
	  /* on cherche le pave' qui a cree' ce pave de presentation */
	  /* est-ce le pere ? */
	  if (pAb->AbEnclosing != NULL)
	    if (pAb->AbEnclosing->AbElement == pAb->AbElement)
	      if (!pAb->AbEnclosing->AbPresentationBox)
		{
		found = TRUE;	/* c'est bien le pere */
		pAb = pAb->AbEnclosing;
		}
	  if (!found)
	    /* on cherche parmi les freres */
	    {
	    /* parmi les freres suivants d'abord */
	    do
	      if (pAb->AbPresentationBox)
	        pAb = pAb->AbNext;
	      else
	        if (pAb->AbElement == pAbb->AbElement)
	  	  found = TRUE;
	        else
	  	  pAb = NULL;
	    while ((!found) && pAb != NULL);
	    if (!found)
	      /* pas found', on cherche parmi les freres precedents */
	      {
	      pAb = pAbb;
	      do
	        if (pAb->AbPresentationBox)
	          pAb = pAb->AbPrevious;
	        else
	          if (pAb->AbElement == pAbb->AbElement)
	  	    found = TRUE;
	          else
	  	    pAb = NULL;
	      while ((!found) && pAb != NULL);
	      }
	    }
	  if (pAb != NULL)
	    if (pAb->AbDead)
	      pAb = NULL;
	  break;

	case RlReferred:
	  vue = pAb->AbDocView;
	  if (pAttr == NULL)
	    {
	    /* cherche les attributs references de l'element */
	    pAttr = pAb->AbElement->ElFirstAttr;
	    found = FALSE;
	    while (pAttr != NULL && !found)
	      {
	      if (pAttr->AeAttrType == AtReferenceAttr)
		/* c'est un attribut reference */
		{
		/* cet attribut a-t-il une regle de presentation qui */
		/* cree le pave pour lequel on travaille ? */
		if (AttrCreatePresBox(pAttr, pAbb))
		  /* oui, c'est l'attribut cherche' */
	          found = TRUE;
		}
	      if (!found)
		/* passe a l'attribut suivant de l'element */
	        pAttr = pAttr->AeNext;
	      }
	    }
	  pAb = NULL;
	  /* cherche le premier pave de l'element designe' par */
	  /* l'attribut */
	  if (pAttr != NULL)
	    if (pAttr->AeAttrType == AtReferenceAttr)
	      if (pAttr->AeAttrReference != NULL)
		if (pAttr->AeAttrReference->RdReferred != NULL)
		  /* les references externes ne sont pas utilisees */
		  /* dans les positionnements et les dimensionnements */
		  if (!pAttr->AeAttrReference->RdReferred->ReExternalRef)
		    if (pAttr->AeAttrReference->RdReferred->ReReferredElem != NULL)
		      pAb = pAttr->AeAttrReference->RdReferred->ReReferredElem->
							ElAbstractBox[vue - 1];
	  pAbbMain = NULL;
	  if (pAb != NULL)
	    /* cherche en avant le pave demande */ 
	    do
	      if (numAbType == 0)
		if (pAb->AbDead)
		  /* ce pave est en cours de destruction */
		  if (pAb->AbNext == NULL)
		    pAb = NULL;
		  else
		    if (pAb->AbNext->AbElement == pAb->AbElement)
		      /* le pave suivant appartient au meme element,
			 on le prend en compte */
		      pAb = pAb->AbNext;
		    else
		      /* on n'a pas found' de pave pour cet element */
		      pAb = NULL;
		else
		  found = TRUE;
	      else
		{
		  VerifyAbsBox(&found, pSP, PoRefElem, numAbType, notType, pAb);
		  if (!found)
		    {
		    if (!pAb->AbPresentationBox)
		      pAbbMain = pAb;
		    if (pAb->AbNext != NULL)
		      if (pAb->AbNext->AbElement == pAb->AbElement)
			pAb = pAb->AbNext;
		      else
			pAb = NULL;
		    else
		      pAb = NULL;
		    }
		}
	  while (!(pAb == NULL || found));
	  if (pAb == NULL && pAbbMain != NULL && numAbType != 0)
	     /* on cherche parmi les paves descendants du pave principal */
	     {
	     pAb = pAbbMain->AbFirstEnclosed;
	     while (pAb != NULL && !found)
		{
		VerifyAbsBoxDescent(&found, pSP, PoRefElem, numAbType, notType, pAb);
		if (!found && pAb != NULL)
		   pAb = pAb->AbNext;
		}
	     }
	  break;

	default:
	  break;
	}
    }
  return pAb;
}



/* ---------------------------------------------------------------------- */
/* |	GetBufConst   acquiert un buffer de texte pour la constante de	| */
/* |		presentation correspondant au pave pointe par pAb.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void GetBufConst(PtrAbstractBox pAb)

#else /* __STDC__ */
void GetBufConst(pAb)
	PtrAbstractBox pAb;
#endif /* __STDC__ */

{
  PtrTextBuffer  pBT;
  
  
  GetBufTexte(&pBT);
  pAb->AbText = pBT;
  pAb->AbLanguage = TtaGetDefaultLanguage();
  pAb->AbVolume = 0;
}



/* ---------------------------------------------------------------------- */
/* |	MajVolLibre	met a jour le volume libre restant dans la vue	| */
/* |		du pave pAb, en prenant en compte le volume de ce	| */
/* |		nouveau pave feuille.					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void MajVolLibre(PtrAbstractBox pAb, PtrDocument pDoc)

#else /* __STDC__ */
void MajVolLibre(pAb, pDoc)
	PtrAbstractBox pAb;
	PtrDocument pDoc;
#endif /* __STDC__ */

{

  
  if (!AssocView(pAb->AbElement)) 
    /* vue de l'arbre principal */ 
    pDoc->DocViewFreeVolume[pAb->AbDocView - 1] -= pAb->AbVolume;
  else			
    /* vue d'elements associes */
    pDoc->DocAssocFreeVolume[pAb->AbElement->ElAssocNum-1] -= pAb->AbVolume;
}



/* ---------------------------------------------------------------------- */
/* |	Contenu met dans le pave pointe par pAb le contenu de l'element| */
/* |		feuille pointe par pEl.					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void Contenu(PtrElement pEl, PtrAbstractBox pAb, PtrDocument pDoc)

#else /* __STDC__ */
void Contenu(pEl, pAb, pDoc)
	PtrElement pEl;
	PtrAbstractBox pAb;
	PtrDocument pDoc;
#endif /* __STDC__ */

{
  int             lg, i;
  PtrTextBuffer  pBu1;
  PtrReference    pPR1;
  PtrReferredDescr pDe1;

  if (pEl->ElHolophrast)
    {
    pAb->AbLeafType = LtText;
    GetBufConst(pAb);
    pAb->AbLanguage = TtaGetDefaultLanguage();
    pBu1 = pAb->AbText;
    CopyStringToText("<", pBu1, &lg);
    CopyStringToText(pEl->ElStructSchema->SsRule[pEl->ElTypeNumber-1].SrName,
			 pBu1, &i);
    lg += i;
    CopyStringToText(">", pBu1, &i);
    lg += i;
    pAb->AbVolume = lg;
    pAb->AbCanBeModified = FALSE;
    pAb->AbSensitive = TRUE;
    /* met a jour le volume libre restant dans la vue */
    MajVolLibre(pAb, pDoc);
    }
  else if (pEl->ElTerminal)
    {
      pAb->AbLeafType = pEl->ElLeafType;
      switch (pEl->ElLeafType)
	{
  case LtPicture:
	  /* saute les paves crees par FnCreateBefore */
	  /* while (pAb->AbText != NULL && pAb->AbNext != NULL)
	     pAb = pAb->AbNext;	*/
	  NewImageDescriptor(pAb, pEl->ElText->BuContent, UNKNOWN_FORMAT);
	  pAb->AbVolume = pEl->ElTextLength;
	  break;
  case LtText:
	  /* saute les paves crees par FnCreateBefore */
	  while (pAb->AbText != NULL && pAb->AbNext != NULL)
	    pAb = pAb->AbNext;	
	  /* prend le contenu de l'element correspondant */
	  pAb->AbText = pEl->ElText;
	  pAb->AbLanguage = pEl->ElLanguage;
	  pAb->AbVolume = pEl->ElTextLength;
	  break;
  case LtPlyLine:
	  /* prend le contenu de l'element correspondant */
	  pAb->AbPolyLineBuffer = pEl->ElPolyLineBuffer;
	  pAb->AbVolume = pEl->ElNPoints;
	  pAb->AbPolyLineShape = pEl->ElPolyLineType;
	  break;
  case LtSymbol:
  case LtGraphics:
	  pAb->AbShape = pEl->ElGraph;
	  pAb->AbGraphAlphabet = 'G';
	  if (pAb->AbShape == '\0')
	    pAb->AbVolume = 0;
	  else
	    pAb->AbVolume = 1;
	  break;
  case LtReference:
	  pAb->AbLeafType = LtText;
	  GetBufConst(pAb);
	  pAb->AbLanguage = TtaGetDefaultLanguage();
	  pBu1 = pAb->AbText;
	  pBu1->BuContent[0] = '[';
	  lg = 2;
	  pBu1->BuContent[lg - 1] = '?';	
	  /* la reference pointe sur rien */
	  if (pEl->ElReference != NULL)
	    {
	      pPR1 = pEl->ElReference;
	      if (pPR1->RdInternalRef)
		{
		  if (pPR1->RdReferred != NULL)
		    if (!pPR1->RdReferred->ReExternalRef)
		      if (!DansTampon(pPR1->RdReferred->ReReferredElem))
			/* l'element reference' n'est pas dans le */
			/* buffer des elements coupe's */
			pBu1->BuContent[lg - 1] = '*';
		  lg++;
		  pBu1->BuContent[lg - 1] = ']';
		} 
	      else
		{
		  if (pPR1->RdReferred != NULL)
		    if (pPR1->RdReferred->ReExternalRef)
		      /* copie le nom du document reference' */
		      {
			i = 1;
			pDe1 = pPR1->RdReferred;
			while (pDe1->ReExtDocument[i - 1] != '\0')
			  {
			    pBu1->BuContent[lg - 1] = pDe1->ReExtDocument[i - 1];
			    lg++;
			    i++;
			  }
			lg--;
		      }
		  lg++;
		  if (!pPR1->RdInternalRef)
		    {
		      pBu1->BuContent[0] = '<';
		      pBu1->BuContent[lg - 1] = '>';
		    }
		  else
		    pBu1->BuContent[lg - 1] = ']';
		}
	    }
	  pBu1->BuContent[lg] = '\0';	
	  /* fin de la chaine de car. */ 
	  pBu1->BuLength = lg;
	  pAb->AbVolume = lg;
	  pAb->AbCanBeModified = FALSE;
	  break;
  case LtPairedElem:
	  pAb->AbLeafType = LtText;
	  GetBufConst(pAb);
	  pAb->AbLanguage = TtaGetDefaultLanguage();
	  pBu1 = pAb->AbText;
	  if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrFirstOfPair)
	    {
	      pBu1->BuContent[0] = '<';
	      pBu1->BuContent[1] = '<';
	    }
	  else
	    {
	      pBu1->BuContent[0] = '>';
	      pBu1->BuContent[1] = '>';
	    }
	  pBu1->BuContent[2] = '\0';	
	  /* fin de la chaine de car. */ 
	  pBu1->BuLength = 2;
	  pAb->AbVolume = 2;
	  pAb->AbCanBeModified = FALSE;
	  break;
	default:
	  break;	
	}
      /* met a jour le volume libre restant dans la vue */
      MajVolLibre(pAb, pDoc);
    }
}


/* ---------------------------------------------------------------------- */
/* |	PageCreateRule	cherche dans la chaine de regles de presentation| */
/* |	qui commence par pRe et qui appartient au schema de		| */
/* |	presentation pointe' par pSPR, la regle de creation qui		| */
/* |	engendre le pave pCree.						| */
/* |	Si cette regle est trouvee, retourne TRUE et TypeCreation	| */
/* |	contient le type de cette regle,				| */
/* |	Sinon, retourne FALSE.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean PageCreateRule(PtrPRule pRe, PtrPSchema pSPR, PtrAbstractBox pCree, FunctionType *TypeCreation)
#else /* __STDC__ */
static boolean PageCreateRule(pRe, pSPR, pCree, TypeCreation)
	PtrPRule pRe;
	PtrPSchema pSPR;
	PtrAbstractBox pCree;
	FunctionType *TypeCreation;
#endif /* __STDC__ */
{
  boolean	stop;
  boolean	Ret;
  
  Ret = FALSE;
  stop = FALSE;
  if (pCree != NULL)
  do
    if (pRe == NULL)
      /* il n' y a plus de regles dans la chaine de regles */
      stop = TRUE;
    else if (pRe->PrType > PtFunction)
      /* il n'y a plus de regle fonction de presentation dans la chaine */
      stop = TRUE;
    else if (pRe->PrType < PtFunction)
      /* on n'a pas encore vu les Fonctions */
      pRe = pRe->PrNextPRule;
    else
      {
	/* la regle pRe est une fonction de presentation */
	if ((pRe->PrPresFunction == FnCreateBefore 
	     || pRe->PrPresFunction == FnCreateWith
	     || pRe->PrPresFunction == FnCreateAfter
	     || pRe->PrPresFunction == FnCreateEnclosing
	     || pRe->PrPresFunction == FnCreateFirst 
	     || pRe->PrPresFunction == FnCreateLast) 
	    && pCree->AbTypeNum == pRe->PrPresBox[0]
	    && pCree->AbPSchema == pSPR)
	  /* c'est une regle de creation et elle cree ce type de */
	  /* boite */
	  {
	    stop = TRUE;
	    Ret = TRUE;
	    *TypeCreation = pRe->PrPresFunction;
	  } 
	else
	  /* passe a la regle suivante de la chaine */
	  pRe = pRe->PrNextPRule;
      }
  while (!stop);
  return Ret;
}


/* ---------------------------------------------------------------------- */
/* |	RegleCree retourne le type de la regle de presentation appelee	| */
/* |		par le pave pCreateur et qui a cree le pave pCree.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
FunctionType RegleCree(PtrDocument pDoc, PtrAbstractBox pCreateur, PtrAbstractBox pCree)

#else /* __STDC__ */
FunctionType RegleCree(pDoc, pCreateur, pCree)
	PtrDocument pDoc;
	PtrAbstractBox pCreateur;
	PtrAbstractBox pCree;
#endif /* __STDC__ */

{
  PtrPRule     pRCre;
  FunctionType        Ret;
  PtrPSchema       pSPR;
  PtrAttribute      pAttr;
  PtrAttribute	   pA;
  boolean	   ok;
  PtrPSchema	   pSchP;
  PtrHandlePSchema pHd;

  Ret = FnLine;	
  /* cherche les regles de creation en ignorant les attributs */
  pRCre = LaRegle(pDoc, pCreateur, &pSPR, PtFunction, FALSE, &pAttr);
  if (!PageCreateRule(pRCre, pSPR, pCree, &Ret))
    /* on n'a pas found la regle qui cree la bonne boite */
    /* on cherche les regles de creation associees aux attributs */
    /* de l'element createur */
    {
    ok = FALSE;
    pA = pCreateur->AbElement->ElFirstAttr;
    while (pA != NULL && !ok)
      {
      pSchP = pA->AeAttrSSchema->SsPSchema;
      pHd = NULL;
      /* on examine le schema de presentation principal, puis les schemas */
      /* additionnels */
      while (pSchP != NULL && !ok)
        {
	/* cherche la premiere regle de presentation pour cet attribut */
	/* dans ce schema de presentation */
	pRCre = AttrPresRule(pA, pCreateur->AbElement, FALSE, NULL, pSchP);
	ok = PageCreateRule(pRCre, pSchP, pCree, &Ret);
	if (pHd == NULL)
	  /* on n'a pas encore traite' les schemas de presentation additionnels
	  On prend le premier schema additionnel. */
	  pHd = pA->AeAttrSSchema->SsFirstPSchemaExtens;
	else
	  /* passe au schema additionnel suivant */
	  pHd = pHd-> HdNextPSchema;
	if (pHd == NULL)
	  /* il n'y a pas (ou plus) de schemas additionnels */
	  pSchP = NULL;
	else
	  pSchP = pHd->HdPSchema;
	}
      if (!ok)
	/* passe a l'attribut suivant de l'element */
	pA = pA->AeNext;
      }
    }
  return Ret;
}



/* ---------------------------------------------------------------------- */
/* |	ChSchemaPres	cherche le schema de presentation a appliquer a	| */
/* |		l'element pointe par pEl. Retourne dans pSchP un	| */
/* |		pointeur sur ce schema, dans NumEntree le numero de	| */
/* |		l'entree correspondant a l'element dans ce schema et	| */
/* |		dans pSchS un pointeur sur le schema de structure auquel| */
/* |		correspond le schema de presentation retourne'.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void ChSchemaPres(PtrElement pEl, PtrPSchema *pSchP, int *NumEntree, PtrSSchema *pSchS)
#else /* __STDC__ */
void ChSchemaPres(pEl, pSchP, NumEntree, pSchS)
	PtrElement pEl;
	PtrPSchema *pSchP;
	int *NumEntree;
	PtrSSchema *pSchS;
#endif /* __STDC__ */
{
  boolean         found;
  int             i;
  PtrSSchema    pSc1;
  SRule          *pSRule;

  if (pEl == NULL)
    {
    *pSchP = NULL;
    *pSchS = NULL;
    *NumEntree = 0;
    }
  else
    {
    *pSchP = pEl->ElStructSchema->SsPSchema;
    *pSchS = pEl->ElStructSchema;
    /* premiere regle de presentation specifique a ce type d'element */
    *NumEntree = pEl->ElTypeNumber;
    /* s'il s'agit de l'element racine d'une nature, on prend les regles */
    /* de presentation (s'il y en a) de la regle nature dans la structure */
    /* englobante. */
    /* on ne traite pas les marques de page */
    if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
      if (pEl->ElParent != NULL) 
        /* il y a un englobant */ 
        if (pEl->ElParent->ElStructSchema != pEl->ElStructSchema)
	  /* cherche la regle introduisant la nature dans le schema de */
	  /* structure de l'englobant. */
	  {
	  pSc1 = pEl->ElParent->ElStructSchema;
	  found = FALSE;
	  i = 0;
	  do
	    {
	      i++;
	      pSRule = &pSc1->SsRule[i - 1];
	      if (pSRule->SrConstruct == CsNatureSchema)
		if (pSRule->SrSSchemaNat == pEl->ElStructSchema)
		  found = TRUE;
	    }
	  while (!(found || i >= pSc1->SsNRules));
	  if (found)
	    if (pSc1->SsPSchema->PsElemPRule[i - 1] != NULL)
	      /* il y a des regles de presentation specifiques */
	      {
		*pSchP = pSc1->SsPSchema;
		*NumEntree = i;
		*pSchS = pEl->ElParent->ElStructSchema;
	      }
	  }
    }
}

/* ---------------------------------------------------------------------- */
/* |	CheckPPosUser							| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean CheckPPosUser(PtrAbstractBox pAb, PtrDocument pDoc)
#else /* __STDC__ */
static boolean CheckPPosUser(pAb, pDoc)
	PtrAbstractBox pAb;
	PtrDocument pDoc;
#endif /* __STDC__ */
{
  int		frame;
  boolean	ret;

  if (AssocView(pAb->AbElement))
    frame = pDoc->DocAssocFrame[pAb->AbElement->ElAssocNum - 1];
  else
    frame = pDoc->DocViewFrame[pAb->AbDocView - 1];
  ret = IsAbstractBoxDisplayed(pAb, frame);
  return ret;
}


/* ---------------------------------------------------------------------- */
/* |	applPosRelat 	applique la regle de position PR-pPRule 	| */
/* |		au pave pAbb1. 						| */
/* |		rend vrai dans appl si la regle a ete appliquee.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void applPosRelat(AbPosition *PPos, PosRule RP, PtrPRule pPRule, PtrAttribute pAttr, PtrPSchema pSchP, PtrAbstractBox pAbb1, PtrDocument pDoc, boolean *appl)
#else /* __STDC__ */
static void applPosRelat(PPos, RP, pPRule, pAttr, pSchP, pAbb1, pDoc, appl)
	AbPosition *PPos;
	PosRule RP;
	PtrPRule pPRule;
	PtrAttribute pAttr;
	PtrPSchema pSchP;
	PtrAbstractBox pAbb1;
	PtrDocument pDoc;
	boolean *appl;
#endif /* __STDC__ */
{
  PtrAbstractBox         pAbbPos;
  boolean         SautPage;
  PtrAbstractBox         pAbbParent;
  PosRule       *pPosRule;
  PtrPRule	  pRSpec;
  PtrPSchema 	  pSchPPage;
  int		  b, PageHeaderHeight;
  
  *appl = FALSE;		
  /* on n'a pas (encore) applique' la regle */
  SautPage = FALSE;
  /* le pave est-il une marque de saut de page (le trait horizontal */
  /* qui separe les pages dans les images) ? Dans ce cas, il aura */
  /* une position flottante */
  if (pAbb1->AbElement->ElTerminal && pAbb1->AbElement->ElLeafType == LtPageColBreak)
    if (pAbb1->AbLeafType != LtCompound && !pAbb1->AbPresentationBox)
      SautPage = TRUE;
  pPosRule = &RP;
  if (pPosRule->PoPosDef == NoEdge || SautPage)
    /* position flottante: regle VertPos=NULL ou HorizPos=NULL */
    {
      PPos->PosEdge = NoEdge;
      PPos->PosRefEdge = NoEdge;
      PPos->PosDistance = 0;
      PPos->PosUnit = UnRelative;
      PPos->PosAbRef = NULL;
      PPos->PosUserSpecified = FALSE;
      *appl = TRUE;
    }
  else
    {
      /* cherche le pave (pAbbPos) par rapport auquel le pave */
      /* traite' se positionne  */
      if (pPosRule->PoRefElem)
	{
	  /* appelle l'exception des tableaux, au cas ou ce serait la regle */
	  /* de hauteur d'un filet vertical d'un tableau */
	  pAbbPos = NULL;
	  if (ThotLocalActions[T_abref]!= NULL)
	    (*ThotLocalActions[T_abref])(pAbb1, pPosRule, pPRule, &pAbbPos);
	  /* si l'exception n'a pas ete traitee, effectue un traitement normal*/
	  if (pAbbPos == NULL)
	    pAbbPos = PaveRef(pPosRule->PoNotRel, pPosRule->PoTypeRefElem, pSchP,
			      pPosRule->PoRelation, pPosRule->PoRefElem, pAbb1, pAttr);
	}
      else
	pAbbPos = PaveRef(pPosRule->PoNotRel, pPosRule->PoRefPresBox, pSchP, pPosRule->PoRelation, pPosRule
			  ->PoRefElem, pAbb1, pAttr);
      if (pAbbPos != NULL)
	{
	  /* on a found le pave de reference */
	  PPos->PosEdge = pPosRule->PoPosDef;
	  PPos->PosRefEdge = pPosRule->PoPosRef;
	  if (pPosRule->PoDistAttr)	
	    /* c'est la valeur d'un attribut */ 
	    {
	    if (pPosRule->PoDistance < 0)
	      /* il faut inverser cette valeur */
	      PPos->PosDistance = -AttrValue(pAttr);
	    else
	      PPos->PosDistance = AttrValue(pAttr);
	    if (pPosRule->PoDistUnit == UnRelative ||
		pPosRule->PoDistUnit == UnXHeight)
	      /* convertit en 1/10 de caractere */
	      PPos->PosDistance = 10 * PPos->PosDistance;
	    }
	  else	
	    /* c'est la valeur elle meme qui est dans la regle */
	    PPos->PosDistance = pPosRule->PoDistance;
	  PPos->PosUnit = pPosRule->PoDistUnit;
	  PPos->PosAbRef = pAbbPos;
	  if (FirstCreation)
	    PPos->PosUserSpecified = pPosRule->PoUserSpecified;
	  else
	    PPos->PosUserSpecified = FALSE;
	  if (PPos->PosUserSpecified)
	     PPos->PosUserSpecified = CheckPPosUser(pAbb1, pDoc);
	  *appl = TRUE;	
	  /* a priori, l'englobement est respecte' */
	  if (pPRule->PrType == PtHorizPos)
            pAbb1->AbHorizEnclosing = TRUE;
          else if (pPRule->PrType == PtVertPos)
            pAbb1->AbVertEnclosing = TRUE;
	  /* verifie si le pave deroge a la regle d'englobement */
	  if (pPosRule->PoRelation == RlRoot)
	    if (PPos->PosAbRef != pAbb1->AbEnclosing)
	      /* ce pave deroge a la regle d'englobement */
	      
	      if (pPRule->PrType == PtHorizPos)
		pAbb1->AbHorizEnclosing = FALSE;
	      else if (pPRule->PrType == PtVertPos)
		pAbb1->AbVertEnclosing = FALSE;
	  if (PPos->PosAbRef == pAbb1->AbEnclosing)
	    /* le pave se positionne par rapport a l'englobant */
	    
	    if (pPRule->PrType == PtHorizPos) 
	      /* position horizontale */ 
	      if (PPos->PosDistance < 0)
		{
		  if (PPos->PosEdge == Left && PPos->PosRefEdge ==
		    Left)
		  /* le cote gauche du pave est a gauche du */
		  /* cote gauche de l'englobant: debordement */
		    pAbb1->AbHorizEnclosing = FALSE;
		}
	      else 
		{
		  if (PPos->PosDistance > 0)
		  {
		    if (PPos->PosEdge == Right && PPos->PosRefEdge == Right)
		    /* le cote droit du pave est a droite du */
		    /* cote droit de l'englobant: debordement */
		      pAbb1->AbHorizEnclosing = FALSE;
		  }
	        }
	    else if (pPRule->PrType == PtVertPos)
	      {
#ifndef __COLPAGE__
	      /* regarde si le premier fils de l'englobant est un saut de page */
	      pAbbParent = pAbb1->AbEnclosing;
	      if (pAbbParent->AbFirstEnclosed != pAbb1
		  && pAbbParent->AbFirstEnclosed->AbElement->ElTerminal
		  && pAbbParent->AbFirstEnclosed->AbElement->ElLeafType == LtPageColBreak)
		{
		  /* positionne par rapport au saut de page */
		  PPos->PosRefEdge = Bottom;
		  PPos->PosAbRef = pAbbParent->AbFirstEnclosed;
		  /* mise a jour de la distance */
		  /* si PPos->PosDistance est en unite relative on ne fait rien */
		  /* sinon, PPos->PosDistance est decremente de la hauteur */
		  /* du haut de la boite haut de page */
		  if (PPos->PosUnit == UnPoint)
		    {
		      /* on cherche la boite page correspondant a la regle page */
		      /* portee par un des ascendants  */
		      b = TypeBPage (pAbbParent->AbFirstEnclosed->AbElement,
			      pAbbParent->AbFirstEnclosed->AbElement->ElViewPSchema,
			      &pSchPPage);
		      PageHeaderHeight = pSchPPage->PsPresentBox[b - 1].PbHeaderHeight;
		       /* PbHeaderHeight toujours en points typo */
		       if (PPos->PosDistance - PageHeaderHeight >= 0)
		         PPos->PosDistance = PPos->PosDistance - PageHeaderHeight;
		    }
		  PPos->PosUserSpecified = FALSE;
		}
#endif /* __COLPAGE__ */
	      if (PPos->PosDistance < 0)
		{
		  if (PPos->PosEdge == Top && PPos->PosRefEdge == Top)
		    /* le haut du pave est au-dessus du */
		    /* haut de l'englobant: debordement */
		    pAbb1->AbVertEnclosing = FALSE;
		 }
	      else if (PPos->PosDistance > 0)
		if (PPos->PosEdge == Bottom && PPos->PosRefEdge == Bottom)
		  /* le bas du pave est au-dessous du */
		  /* bas de l'englobant: debordement */
		  pAbb1->AbVertEnclosing = FALSE;
	      }
	} 
      else
	/* on n'a pas found le pave de reference */ 
	if (pAbb1->AbLeafType != LtCompound
	    && !(pPosRule->PoRelation == RlNext 
		 || pPosRule->PoRelation == RlPrevious 
		 || pPosRule->PoRelation == RlSameLevel 
		 || pPosRule->PoRelation == RlCreator
		 || pPosRule->PoRelation == RlReferred))
	  {
            /* inutile de reessayer d'appliquer la regle */
            /* quand les paves environnants seront crees. */
	    PPos->PosEdge = pPosRule->PoPosDef;
	    PPos->PosRefEdge = pPosRule->PoPosRef;
	    PPos->PosDistance = pPosRule->PoDistance;
	    PPos->PosUnit = pPosRule->PoDistUnit;
	    PPos->PosAbRef = NULL;
	    PPos->PosUserSpecified = pPosRule->PoUserSpecified;
	    if (PPos->PosUserSpecified)
	       PPos->PosUserSpecified = CheckPPosUser(pAbb1, pDoc);
	    *appl = TRUE;
	    
	  }
	else if (pAbb1->AbEnclosing == NULL && pPosRule->PoRelation == RlEnclosing)
	  /* positionnement par rapport a la fenetre */
	  {
	    PPos->PosEdge = pPosRule->PoPosDef;
	    PPos->PosRefEdge = pPosRule->PoPosRef;
	    if (pPosRule->PoDistAttr)
	      /* valeur d'un attribut */
	      {
	      if (pPosRule->PoDistance < 0)
		/* inverser cette valeur */
		PPos->PosDistance = -AttrValue(pAttr);
	      else
		PPos->PosDistance = AttrValue(pAttr);
	      if (pPosRule->PoDistUnit == UnRelative ||
		  pPosRule->PoDistUnit == UnXHeight)
	        /* convertit en 1/10 de caractere */
		PPos->PosDistance = 10 * PPos->PosDistance;
	      }
	    else	
	      /* c'est la valeur elle meme */
	      PPos->PosDistance = pPosRule->PoDistance;
	    if (FirstCreation)
	      PPos->PosUserSpecified = pPosRule->PoUserSpecified;
	    else
	      PPos->PosUserSpecified = FALSE;
	    if (PPos->PosUserSpecified)
	      PPos->PosUserSpecified = CheckPPosUser(pAbb1, pDoc);
	    PPos->PosUnit = pPosRule->PoDistUnit;
	    PPos->PosAbRef = NULL;
	    *appl = TRUE;
	  }
	else
	  /* on n'a pas found' le pave' de reference */
	  /* si c'est un positionnement par rapport au precedent ou au */
	  /* suivant, on positionne le pave par rapport a l'englobant */
	  if (pAbb1->AbEnclosing != NULL &&
	      (
	      (pPRule->PrType == PtVertPos && pPosRule->PoRelation == RlPrevious &&
	        pPosRule->PoPosDef == Top && pPosRule->PoPosRef == Bottom) ||
	      (pPRule->PrType == PtVertPos && pPosRule->PoRelation == RlNext &&
	        pPosRule->PoPosDef == Bottom && pPosRule->PoPosRef == Top) ||
	      (pPRule->PrType == PtHorizPos && pPosRule->PoRelation == RlPrevious &&
	        pPosRule->PoPosDef == Left && pPosRule->PoPosRef == Right) ||
	      (pPRule->PrType == PtHorizPos && pPosRule->PoRelation == RlNext &&
	        pPosRule->PoPosDef == Right && pPosRule->PoPosRef == Left)
	      )
	     )
	    /* c'est une regle de positionnement vertical en dessous du
	    precedent et on n'a pas found' le precedent. On remplace par
	    un positionnement en haut de l'englobant */
	    {
	    PPos->PosAbRef = pAbb1->AbEnclosing;
	    PPos->PosEdge = pPosRule->PoPosDef;
	    PPos->PosRefEdge = pPosRule->PoPosDef;
	    /* s'agit-il d'une regle de presentation specifique ? */
	    pRSpec = pAbb1->AbElement->ElFirstPRule;
	    while (pRSpec != NULL && pRSpec != pPRule)
	       pRSpec = pRSpec->PrNextPRule;
	    if (pRSpec == pPRule)
	       /* c'est une regle de presentation specifique */
	       /* on prend le decalage en compte */
	       PPos->PosDistance = pPosRule->PoDistance;
	    else
	       /* c'est une regle generique */
	       /* on se positionne tout contre l'englobant */
	       PPos->PosDistance = 0;
	    /* on pourra reessayer d'appliquer la regle plus tard : */
	    /* le precedent existera peut etre, alors */
	    *appl = FALSE;
	    PPos->PosUnit = pPosRule->PoDistUnit;
	    if (FirstCreation)
	      PPos->PosUserSpecified = pPosRule->PoUserSpecified;
	    else
	      PPos->PosUserSpecified = FALSE;
	    if (PPos->PosUserSpecified)
	       PPos->PosUserSpecified = CheckPPosUser(pAbb1, pDoc);
            pAbb1->AbVertEnclosing = TRUE;
	    }
    }
}



/* ---------------------------------------------------------------------- */
/* |	appldimension	 applique au pave pointe' par pAb la regle	| */
/* |		de dimension pointee par pPRule.			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void appldimension(AbDimension *PDim, PtrAbstractBox pAb, PtrPSchema pSchP, PtrAttribute pAttr, boolean *appl, PtrPRule pPRule, PtrDocument pDoc)
#else /* __STDC__ */
static void appldimension(PDim, pAb, pSchP, pAttr, appl, pPRule, pDoc)
	AbDimension *PDim;
	PtrAbstractBox pAb;
	PtrPSchema pSchP;
	PtrAttribute pAttr;
	boolean *appql;
	PtrPRule pPRule;
	PtrDocument pDoc;
#endif /* __STDC__ */
{
  PtrAbstractBox         pP;
  PtrAttribute     pA;
  boolean         stop;
  DimensionRule   *pDRule;
  int		  AttrRegle;
  
  /* on met a priori les valeurs correspondant a une dimension */
  /* egale a celle du contenu. */
  PDim->DimIsPosition = FALSE;
  PDim->DimValue = 0;
  PDim->DimAbRef = NULL;
  PDim->DimUnit = pPRule->PrDimRule.DrUnit;
  PDim->DimSameDimension = TRUE;
  PDim->DimMinimum = pPRule->PrDimRule.DrMin;
  if (FirstCreation)
    PDim->DimUserSpecified = pPRule->PrDimRule.DrUserSpecified;
  else
    PDim->DimUserSpecified = FALSE;
  if (PDim->DimUserSpecified)
    PDim->DimUserSpecified = CheckPPosUser(pAb, pDoc);

  *appl = FALSE;
  pDRule = &pPRule->PrDimRule;
  if (pDRule->DrPosition)
    {
      /* Box elastique, la dimension est definie comme une position */
      /* applique la regle */
      applPosRelat(&PDim->DimPosition, pDRule->DrPosRule, pPRule, pAttr, pSchP, pAb, pDoc, appl);
      /* si la regle a pu etre appliquee, le boite est reellement elastique */
      if (*appl)
	PDim->DimIsPosition = TRUE;
    }
  else if (pDRule->DrAbsolute)
    {
      /* valeur absolue */
      if (pDRule->DrAttr)	
	/* c'est la valeur d'un attribut */
	{
	if (pDRule->DrValue < 0)
	  /* il faut inverser cette valeur */
	  PDim->DimValue = -AttrValue(pAttr);
	else
	  PDim->DimValue = AttrValue(pAttr);
	if (pDRule->DrUnit == UnRelative ||
	    pDRule->DrUnit == UnXHeight)
	  /* convertit en 1/10 de caractere */
	  PDim->DimValue = 10 * PDim->DimValue;
	}
      else		
	/* c'est la valeur elle meme qui est dans la regle */
	PDim->DimValue = pDRule->DrValue;
      *appl = TRUE;
    }
  else if (pDRule->DrRelation == RlEnclosed)
    {
      /* dimension du contenu */
      *appl = TRUE;	
      /* les valeurs mises a priori conviennent */
      PDim->DimValue = pDRule->DrValue;	
    } 
  else
    {
      /* dimensions relatives a l'englobant ou un frere */
      PDim->DimSameDimension = pDRule->DrSameDimens;
      /* essaie d'appliquer la regle de dimensionnement relatif */
      if (pDRule->DrRefElement)
	pP = PaveRef(pDRule->DrNotRelat, pDRule->DrTypeRefElem, pSchP,
		     pDRule->DrRelation, pDRule->DrRefElement, pAb, pAttr);
      else
	pP = PaveRef(pDRule->DrNotRelat, pDRule->DrRefPresBox, pSchP,
		     pDRule->DrRelation, pDRule->DrRefElement, pAb, pAttr);
      PDim->DimAbRef = pP;
      if (pP == NULL && pAb->AbElement != NULL)
	if (pAb->AbEnclosing == NULL && pDRule->DrRelation == RlEnclosing)
	  /* heritage des dimensions de la fenetre */
	  {
	    if (pDRule->DrValue == 0)
	      {
		PDim->DimValue = 100;
		PDim->DimUnit = UnPercent;
	      }
	    else
	      {
		PDim->DimUnit = pDRule->DrUnit;
		if (pDRule->DrAttr)	
		  /* c'est la valeur d'un attribut */ 
		  {
		  if (pDRule->DrValue < 0)
		    /* inverser cette valeur */
		    PDim->DimValue = -AttrValue(pAttr);
		  else
		    PDim->DimValue = AttrValue(pAttr);
		  if (pDRule->DrUnit == UnRelative ||
		      pDRule->DrUnit == UnXHeight)
		    /* convertit en 1/10 de caractere */
		    PDim->DimValue = 10 * PDim->DimValue;
		  }
		else	
		  /* c'est la valeur elle meme */
		  PDim->DimValue = pDRule->DrValue;
	      }
	    *appl = TRUE;
	  }
      if (pP != NULL)
	{
	  if (pDRule->DrAttr)
	    /* c'est la valeur d'un attribut */
	    {
	      pA = pAttr;
	      AttrRegle = pDRule->DrValue;
	      if (pDRule->DrValue < 0)
		  AttrRegle = - AttrRegle;
	      
	      /* l'attribut est-il celui de la regle ? */
	      if (pAttr->AeAttrNum != AttrRegle
		  || pAttr->AeAttrSSchema->SsPSchema != pSchP)
		/* ce n'est pas l'attribut indique' dans la regle, */
		/* cherche si l'elem. possede l'attribut de la regle */
		{
		  pA = pAb->AbElement->ElFirstAttr;
		  stop = FALSE;
		  do
		    if (pA == NULL)
		      stop = TRUE;	/* dernier attribut de l'element */
		    else if (pA->AeAttrNum == AttrRegle
			     && pA->AeAttrSSchema->SsPSchema == pSchP)
		      stop = TRUE;	/* c'est l'attribut cherche' */
		    else
		      pA = pA->AeNext;
		  while (!(stop));
		}
	      PDim->DimValue = AttrValue(pA);
	      if (pDRule->DrValue < 0)
		/* inverser cette valeur */
		PDim->DimValue = -PDim->DimValue;
	      if (pDRule->DrUnit == UnRelative ||
		  pDRule->DrUnit == UnXHeight)
	        /* convertit en 1/10 de caractere */
		PDim->DimValue = 10 * PDim->DimValue;
	    } 
	  else	
	    /* c'est la valeur elle-meme */
	    PDim->DimValue = pDRule->DrValue;
	  PDim->DimUnit = pDRule->DrUnit;
	  *appl = TRUE;
	}
    }
}


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |	applCol		applique une regle Column			| */
/* |	on insere l'element marque Column AVANT l'element		| */
/* |	si element racine, on insere la marque col comme premier fils	| */
/* |	si element marque page, on insere la marque col APRES.		| */
/* |	le boolean de result indique si le pave pAb a ete detruit.	| */
/* |	On ajoute une marque colonne apres l'element pour permettre	| */
/* |	de reprendre une presentation conforme a la regle qui		| */
/* |	s'appliquait avant l'element pAb->AbElement			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean applCol(PtrDocument pDoc, PtrAbstractBox pAb, int viewSch, PtrPRule pPRule)	
#else /* __STDC__ */
static boolean applCol(pDoc, pAb, viewSch, pPRule)
	PtrDocument pDoc;
	PtrAbstractBox pAb;
	int viewSch;
	PtrPRule pPRule;
#endif /* __STDC__ */
{ 
    PtrElement      pElCol, pEl1, pEl, pPrec;
    PtrAbstractBox         pP;
    boolean         colexiste, stop, beforepEl;
    boolean	    complete;
    PtrAbstractBox         pAbbR;
    int             frame, h, Vue;
    boolean         bool, pavedetruit, found;
    PtrElement   pElGrCols, pElGr1, pSuiv;
    pavedetruit = FALSE; /* a priori pas de destruction de paves */
    if (pPRule->PrViewNum == viewSch)
	/* la regle Column concerne la vue du pave traite' */
    {
	Vue = pAb->AbDocView;
	pEl = pAb->AbElement;
        pElCol = NULL;
	colexiste = FALSE;
	beforepEl = FALSE; /* il faut recreer l'image si les marques sont creees*/
	/* a priori, on n'a pas a creer les paves si la */
	/* colonne existe deja (sauf si manque colgroupee */
        /* si pEl est une marque page qui n'est pas une page debut */
	/* on ignore la regle. */
      if (!(pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	    && (pEl->ElPageType == PgUser
		|| pEl->ElPageType == PgComputed)))
	{
	/* creation d'un element Marque colonne debut comme frere avant pEl */
	/* sauf si pEl est la racine (creation comme premier fils) */
	/* ou un element Marque Page (regle Column dans une boite Page) */

 	/* tout d'abord, on verifie que la marque colonne n'existe pas deja */
        pEl1 = NULL;
	if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
 	  /* on verifie que la marque colonne n'existe pas deja apres */
	  pEl1 = pEl->ElNext;
	else
	  if (pEl->ElParent == NULL)
	    /* on verifie que la marque colonne n'existe pas deja comme fils*/
            pEl1 = pEl->ElFirstChild;
	if (pEl1 != NULL)
	  {   
	      while (pEl1->ElTerminal && pEl1->ElTypeNumber == PageBreak + 1
			&& (pEl1->ElViewPSchema != viewSch
			    || pEl1->ElPageType != ColBegin))
		    /* ignore les sauts de page pour les autres vues */
		    /* et les marques autres que ColBegin */
			pEl1 = pEl1->ElNext;

	      if (pEl1 != NULL &&
		  pEl1->ElTerminal && pEl1->ElTypeNumber == PageBreak + 1)
		/* on est sur que l'on est sur une marque ColBegin */
		/* donc on ne cree pas une nouvelle marque */
		{
		  pElCol = pEl1;
		  colexiste = TRUE;
	          /* code temporaire : si pas de ColGroup qui precede */
		  /* ce ColBegin, on en ajoute un pour etre conforme au */
		  /* nouveau code */
		  if (!(pElCol->ElPrevious != NULL
		      && pElCol->ElPrevious->ElTypeNumber == PageBreak + 1
		      && pElCol->ElPrevious->ElPageType == ColGroup))
		    {  
		      /* si l'element ColGroup existe, on ne cree rien */
		      /* sinon, on cree l'element ColGroup */
	              pElGrCols = NewSubtree(PageBreak + 1, pEl->ElStructSchema,
			pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE,TRUE);
	              InsertElementBefore (pElCol, pElGrCols);
		      pElGrCols->ElPageType = ColGroup;
		      pElGrCols->ElViewPSchema = viewSch;
		      pElGrCols->ElPageNumber = 0; /* numero attribue pour les groupes */
		      /* pas besoin de recreer l'image car l'element */
		      /* colgroupees est place apres l'element marque page */
		    }
		}
	  } /* fin pEl1 != NULL pour pEl = marque page ou racine */
		
        if (!( (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
               || (pEl->ElParent == NULL)))
            /* regle colonne sur un element quelconque */
	    /* l'element est-il precede' par un saut de colonne identique a */
	    /* celui qu'on veut creer ? */
	  {
	    pPrec = pEl;
	    while (pPrec->ElPrevious == NULL && pPrec->ElParent != NULL)
		pPrec = pPrec->ElParent;
	    pPrec = pPrec->ElPrevious;
	    stop = pPrec == NULL;
	    while (!stop)
		if (pPrec->ElTerminal)
		{
		    stop = TRUE;
		    /* ignore les sauts de page pour les autres vues */
		    if (pPrec->ElTypeNumber == PageBreak + 1)
			if (pPrec->ElViewPSchema != viewSch)
			{
			    pPrec = pPrec->ElPrevious;
			    stop = pPrec == NULL;
			}
		}
		else
		{
		    pPrec = pPrec->ElFirstChild;
		    if (pPrec == NULL)
			stop = TRUE;
		    else
			while (pPrec->ElNext != NULL)
			    pPrec = pPrec->ElNext;
		}
	    if (pPrec != NULL)
	      {
		pEl1 = pPrec;
		if (pEl1->ElTerminal)
		    if (pEl1->ElTypeNumber == PageBreak + 1)
			if (pEl1->ElViewPSchema == viewSch 
	                           && pEl1->ElPageType == ColBegin)
			  {
			    colexiste = TRUE;
			    pElCol = pEl1;
	                    /* code temporaire : si pas de ColGroup qui precede */
		            /* ce ColBegin, on en ajoute un pour etre conforme au */
		            /* nouveau code */
		            if (!(pEl1->ElPrevious != NULL
		                 && pEl1->ElPrevious->ElTypeNumber == PageBreak + 1
		                 && pEl1->ElPrevious->ElPageType == ColGroup))
		               /* l'element ColGroup existe, on ne cree rien */
		              {
		                /* on cree l'element ColGroup */
	                        pElGrCols = NewSubtree(PageBreak + 1, pEl->ElStructSchema,
			           pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	                        InsertElementBefore (pElCol, pElGrCols);
		                pElGrCols->ElPageType = ColGroup;
		                pElGrCols->ElViewPSchema = viewSch;
		                pElGrCols->ElPageNumber = 0;
		                /* numero attribue pour les groupes */
				beforepEl = TRUE; /* pour recreer l'image */
				/* on detruit le pave de pElCol pour */
				/* le recreer apres pElGrCols */
				pP = pElCol->ElAbstractBox[Vue-1];
	                        while (pP != NULL && pP->AbElement == pElCol)
	                          {
	                            TuePave(pP);
	                            SuppRfPave(pP, &pAbbR, pDoc);
	                            pP = pP->AbNext;
	                          }
			      }
			    /* on ajoute une marque colonne gauche si elle */
			    /* n'existe pas deja,  pour revenir */
		            /* a l'ancienne regle apres l'element */ 
			    if (pEl->ElNext == NULL
			      || pEl->ElNext->ElTypeNumber != PageBreak+1)
			      {
			      pEl1 = NewSubtree(PageBreak + 1, pEl->ElStructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		              InsertElementAfter(pEl, pEl1);
		              pEl1->ElPageType = ColComputed;
		              pEl1->ElViewPSchema = viewSch;
		              pEl1->ElPageNumber = 1;
	                       /* numero attribue pour les colonnes gauche */

		               /* on cree une marque groupe de colonnes */
		              pElGr1 = NewSubtree(PageBreak + 1, pEl->ElStructSchema, pDoc, 
			       pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		               InsertElementAfter(pEl, pElGr1);
		               pElGr1->ElPageType = ColGroup;
		               pElGr1->ElViewPSchema = viewSch;
		               pElGr1->ElPageNumber = 0;
	                              /* numero attribue pour les groupes */

		              }

			}
	      }
	  }
	
	if (!colexiste &&
	    pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct != CsChoice)
	  {
	    /* on cree une marque de colonne */
	    /* et une marque groupe de colonnes Cols */
	    pElGrCols = NewSubtree(PageBreak + 1, pEl->ElStructSchema,
			    pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	    pElCol = NewSubtree(PageBreak + 1, pEl->ElStructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	    if (pElCol != NULL)
		/* on a cree une marque de colonne */
	      {
	        if (pEl->ElTypeNumber == PageBreak + 1)
		  /* si pEl est une PageBreak on chainepElGrCols et pElCol apres pEl */
		  {
		    InsertElementAfter (pEl, pElGrCols);
	            InsertElementAfter (pElGrCols, pElCol);
		  }
	        else
	          if (pEl->ElParent == NULL)
		  /* si pEl est la racine, on la chaine comme fils de pEl, */
		  /* apres l'element marque page de PgBegin */
	            {
		      pEl1 = pEl->ElFirstChild;
		      if (!pEl1->ElTerminal && !(pEl1->ElLeafType == LtPageColBreak))
		        /* erreur */
		        ;
		      else
			{
		          while (pEl1 != NULL && pEl1->ElViewPSchema != viewSch 
		     	         && pEl1->ElPageType != PgBegin)
		            pEl1 = pEl1->ElNext;
		          if (pEl1 != NULL)
			    {
			      InsertElementAfter (pEl1, pElGrCols);
	                      InsertElementAfter (pElGrCols, pElCol);
			    }
			}
		    }
	          else 
		    {
		      /* insersion avant pEl (comme pour une regle Page) */
		      beforepEl = TRUE;
		      InsertElementBefore(pEl, pElCol);
	              InsertElementBefore (pElCol, pElGrCols);
		      /* on cree une marque colonne et une marque groupe */
		      /* de colonnes apres pour revenir */
		      /* a l'ancienne regle apres l'element */
		      /* si l'element a des suivants */
		      pSuiv = pEl;
		      found = FALSE;
		      while (!found && pSuiv != NULL)
			{
			  found = (pSuiv->ElNext != NULL);
			  if (!found)
			    pSuiv = pSuiv->ElParent;
			}
		      if (found)
		      {
		      /* on cree une marque colonne et une marque groupe */
		      /* de colonnes apres pour revenir */
		      /* a l'ancienne regle apres l'element */
		      pEl1 = NewSubtree(PageBreak + 1, pEl->ElStructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		      pElGr1 = NewSubtree(PageBreak + 1, pEl->ElStructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		      InsertElementAfter(pEl, pElGr1);
		      InsertElementAfter(pElGr1, pEl1);
		      pEl1->ElPageType = ColComputed;
		      pEl1->ElViewPSchema = viewSch;
		      pEl1->ElPageNumber = 1; /* premiere colonne de la serie */
		      pElGr1->ElPageType = ColGroup;
		      pElGr1->ElViewPSchema = viewSch;
		      pElGr1->ElPageNumber = 0; /* numero attribue pour les groupes */
		      }
		    }
	        /* on initialise pElCol et pElGrCols */
		pElCol->ElPageType = ColBegin;
		pElCol->ElViewPSchema = viewSch;
		pElCol->ElPageNumber = 1; /* premiere colonne de la page */
		pElGrCols->ElPageType = ColGroup;
		pElGrCols->ElViewPSchema = viewSch;
		pElGrCols->ElPageNumber = 0; /* numero attribue pour les groupes*/
	      } /* fin pElCol != NULL */
          }  /* fin de !colexiste */		    
      if (beforepEl)
	{ 
	   /* creation des paves : il faut detruire ceux de pEl */
	   /* cf. code de InsereMarque de page.c */
	   pP = pEl->ElAbstractBox[Vue - 1];
	   /* destruction des paves de l'element pEl */
           if (pP != NULL)
	     pavedetruit = TRUE; /* code result */
	   while (pP != NULL && pP->AbElement == pEl)
	     {
	       TuePave(pP);
	       SuppRfPave(pP, &pAbbR, pDoc);
	       pP = pP->AbNext;
	     }
           if (AssocView(pEl))
	     {
	     pP = pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0];
	     frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	     }
	   else
	     {
	     pP = pDoc->DocRootElement->ElAbstractBox[Vue - 1];
	     frame = pDoc->DocViewFrame[Vue - 1];
	     }
	   if (!(pEl->ElAbstractBox[Vue - 1])->AbNew)
	     /* on previent le mediateur */
	     {
	     h = 0;
	     ChangeConcreteImage(frame, &h, pP);
	     }
	    /* on nettoie l'image abstraite des paves morts */
	    FreeDeadAbstractBoxes(pP); /* liberation a partir de la racine car */
	                    /* un pave colonne a pu etre detruit */
	      
            /* cree les paves de la marque de colonne et groupe de colonnes */
            /* correspondant a la regle Column */
	    pP = AbsBoxesCreate(pElGrCols, pDoc, Vue, TRUE, TRUE, &complete);
	    if (pElGrCols->ElAbstractBox[Vue-1] != NULL)
	      pP = AbsBoxesCreate(pElCol, pDoc, Vue, TRUE, TRUE, &complete);
	  } /* fin beforepEl */

        } /* fin pEl marque page non debut */
    }
  return pavedetruit;
}

#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |	applPage 	applique une regle Page				| */
/* |	on insere l'element marque page AVANT l'element (sauf racine)	| */
/* |	le boolean de result indique si le pave pAb a ete detruit	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean applPage(PtrDocument pDoc, PtrAbstractBox pAb, int viewSch, PtrPRule pPRule, FunctionType pageType)

#else /* __STDC__ */
static boolean applPage(pDoc, pAb, viewSch, pPRule, pageType)
	PtrDocument pDoc;
	PtrAbstractBox pAb;
	int viewSch;
	PtrPRule pPRule;
	FunctionType pageType;
#endif /* __STDC__ */

#else /* __COLPAGE__ */
/* ---------------------------------------------------------------------- */
/* |	applPage 	applique une regle Page				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void applPage(PtrDocument pDoc, PtrAbstractBox pAb, int viewSch, PtrPRule pPRule, FunctionType pageType)

#else /* __STDC__ */
static void applPage(pDoc, pAb, viewSch, pPRule, pageType)
	PtrDocument pDoc;
	PtrAbstractBox pAb;
	int viewSch;
	PtrPRule pPRule;
	FunctionType pageType;
#endif /* __STDC__ */
#endif /* __COLPAGE__ */

{
    PtrElement      pElPage, pEl, pElChild, pPrec;
    PtrAbstractBox         pP;
    boolean         exitingPage, stop;
    int             counter;
    PtrPSchema      pSchP;
    PtrElement      pEl1;
    boolean	    complete;
#ifdef __COLPAGE__
	    PtrAbstractBox         pAbbR;
     int             frame, h, Vue;
     boolean         bool, pavedetruit;
#else /* __COLPAGE__ */
    boolean         cree;
#endif /* __COLPAGE__ */
    
#ifdef __COLPAGE__
    pavedetruit = FALSE; /* a priori pas de destruction de paves */
#endif /* __COLPAGE__ */
    if (pPRule->PrViewNum == viewSch)
	/* la regle Page concerne la vue du pave traite' */
    {
#ifdef __COLPAGE__
	Vue = pAb->AbDocView;
#endif /* __COLPAGE__ */
	pElPage = NULL;
	exitingPage = FALSE;
	pEl = pAb->AbElement;
	/* l'element contient-il deja une marque de page de debut */
	/* d'element pour cette vue ? */
	if (!pEl->ElTerminal)
	{
	    pElChild = pEl->ElFirstChild;
	    stop = FALSE;
	    do
		if (pElChild == NULL)
		    stop = TRUE;
		else
		{
		    pEl1 = pElChild;
		    if (pEl1->ElTypeNumber != PageBreak + 1)
			stop = TRUE;
		    else if (pEl1->ElViewPSchema == viewSch && pEl1->ElPageType == PgBegin)
#ifdef __COLPAGE__
		      /* on deplace l'element marque page devant l'element */
	              if (pEl->ElParent != NULL
                          && (pEl1->ElAbstractBox[Vue-1] == NULL))
		        /* sauf si pEl est racine ! */
	                /* et sauf si l'element  MP a deja un pave dans la vue */
	                /* en effet c'est dans le cas ou la construction de */
	                /* l'image se fait en arriere : la destruction des paves */
	                /* ne peut se faire si simplement. On prefere laisser */
	                /* cet element 1er fils, il sera deplace lors d'une autre */
	                /* construction de l'image */
	                {
	                  DeleteElement(&pEl1);
	                  stop = TRUE;
	                }
	              else
	                /* pEl est la racine ou la MP a deja des paves */
	                {
		          pElPage = pEl1;
		          exitingPage = TRUE;
		          stop = TRUE;
	                }
#else /* __COLPAGE__ */
		    {
			pElPage = pEl1;
			exitingPage = TRUE;
			stop = TRUE;
		    }
#endif /* __COLPAGE__ */
		    else
                    /* on saute les eventuelles marque page d'autres vues ? */
			pElChild = pEl1->ElNext;
		}
	    while (!(stop));
	}
	if (!exitingPage)
	    /* l'element ne contient pas de marque de page en tete */
	    /* l'element est-il precede' par un saut de page identique a */
	    /* celui qu'on veut creer ? */
	{
	    pPrec = pEl;
	    while (pPrec->ElPrevious == NULL && pPrec->ElParent != NULL)
		pPrec = pPrec->ElParent;
	    pPrec = pPrec->ElPrevious;
	    stop = pPrec == NULL;
	    while (!stop)
		if (pPrec->ElTerminal)
		{
		    stop = TRUE;
		    /* ignore les saut de pages pour les autres vues */
		    if (pPrec->ElTypeNumber == PageBreak + 1)
#ifdef __COLPAGE__
		        /* ainsi que les sauts de colonne */
			if (pPrec->ElViewPSchema != viewSch
			    || pPrec->ElPageType == ColBegin
			    || pPrec->ElPageType == ColComputed
			    || pPrec->ElPageType == ColGroup
			    || pPrec->ElPageType == ColUser)
#else /* __COLPAGE__ */
			if (pPrec->ElViewPSchema != viewSch)
#endif /* __COLPAGE__ */
			{
			    pPrec = pPrec->ElPrevious;
			    stop = pPrec == NULL;
			}
		}
		else
		{
		    pPrec = pPrec->ElFirstChild;
		    if (pPrec == NULL)
			stop = TRUE;
		    else
			while (pPrec->ElNext != NULL)
			    pPrec = pPrec->ElNext;
		}
	    if (pPrec != NULL)
	    {
		pEl1 = pPrec;
		if (pEl1->ElTerminal)
		    if (pEl1->ElTypeNumber == PageBreak + 1)
			if (pEl1->ElViewPSchema == viewSch && pEl1->ElPageType == PgBegin)
			{
			    exitingPage = TRUE;
			    pElPage = pEl1;
			}
	    }
	}
	
	if (!exitingPage &&
	    pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct != CsChoice)
	{
	    /* on cree une marque de page */
	    pElPage = NewSubtree(PageBreak + 1, pEl->ElStructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	    if (pElPage != NULL)
		/* on a cree une marque de page */

	  /* le reste de la procedure est different entre les deux versions */
#ifdef __COLPAGE__
		/* on la chaine comme premier fils de l'element si pEl est la racine */
	      if (pEl->ElParent == NULL)
	        {
	    	    InsertFirstChild(pEl, pElPage);	
		    /* on l'initialise */
		    pEl1 = pElPage;
		    pEl1->ElPageType = PgBegin;
		    pEl1->ElViewPSchema = viewSch;
		    /* cherche le compteur de pages a appliquer */
		    counter = CptPage(pElPage, pEl1->ElViewPSchema, &pSchP);
		    if (counter > 0)
		        /* calcule la valeur du compteur de pages */
		        pEl1->ElPageNumber = ComptVal(counter, pElPage->ElStructSchema, pSchP, pElPage,
					       pEl1->ElViewPSchema);
		    else	
		        /* page non numerotee */
		        pEl1->ElPageNumber = 1;
	        }
	      else
	        /* insersion avant pEl */
	        {
	          InsertElementBefore(pEl, pElPage);
	          /* on l'initialise */
	          pEl1 = pElPage;
	          pEl1->ElPageType = PgBegin;
	          pEl1->ElViewPSchema = viewSch;
	          /* cherche le compteur de pages a appliquer */
	          counter = CptPage(pElPage, pEl1->ElViewPSchema, &pSchP);
	          if (counter > 0)
	            /* calcule la valeur du compteur de pages */
	            pEl1->ElPageNumber = ComptVal(counter, pElPage->ElStructSchema, pSchP, pElPage,
	                                        pEl1->ElViewPSchema);
	          else       
	            /* page non numerotee */
	            pEl1->ElPageNumber = 1;
	          /* creation des paves : il faut detruire ceux de pEl */
	          /* cf. code de InsereMarque de page.c */
	          pP = pEl->ElAbstractBox[Vue - 1];
	          /* destruction des paves de l'element pEl */
	          pavedetruit = TRUE; /* code result */
	          while (pP != NULL && pP->AbElement == pEl)
	            {
	              TuePave(pP);
	              SuppRfPave(pP, &pAbbR, pDoc);
	              pP = pP->AbNext;
	            }
	          if (!pAb->AbNew)
	            /* on previent le mediateur */
	            {
	              if (AssocView(pEl))
	                {
	                  pP = (pDoc->DocAssocRoot[pEl->ElAssocNum - 1])->ElAbstractBox[0];
	                  frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	                }
	              else
	                {
	                  pP = pDoc->DocRootElement->ElAbstractBox[Vue - 1];
	                  frame = pDoc->DocViewFrame[Vue - 1];
	                }
	              h = 0;
	              bool = ChangeConcreteImage(frame, &h, pP);
	            }
	          /* on nettoie l'image abstraite des paves morts */
	          FreeDeadAbstractBoxes(pAb->AbEnclosing);
	        }
	
	/* TODO quand on traitera les regles recto verso */
	/*  on verifie que la cardinalite de la page est en accord avec la
	   regle appliquee */
	PagePleine(Vue, pDoc, pElPage, TRUE);
	if (pElPage != NULL
	    && !IsViewFull (Vue, pDoc, pElPage))
            /* cree les paves de la marque de page */
            /* correspondant a la regle page */
	    /* si la vue n'est pas pleine */
	  pP = AbsBoxesCreate(pElPage, pDoc, Vue, TRUE, TRUE, &complete);
	/* on met a jour les numeros des pages suivantes */
        MajNumeros(NextElement(pElPage), pElPage, pDoc, TRUE);
       }  /* fin de !exitingPage */
    }
  return pavedetruit;
} /* fin de applPage */

#else /* __COLPAGE__ */
		/* on la chaine comme premier fils de l'element */
	    {
		InsertFirstChild(pEl, pElPage);	
		/* on l'initialise */
		pEl1 = pElPage;
		pEl1->ElPageType = PgBegin;
		pEl1->ElViewPSchema = viewSch;
		/* cherche le compteur de pages a appliquer */
		counter = CptPage(pElPage, pEl1->ElViewPSchema, &pSchP);
		if (counter > 0)
		    /* calcule la valeur du compteur de pages */
		    pEl1->ElPageNumber = ComptVal(counter, pElPage->ElStructSchema, pSchP, pElPage,
					       pEl1->ElViewPSchema);
		else	
		    /* page non numerotee */
		    pEl1->ElPageNumber = 1;
		/* faut-il creer les paves de la marque de page ? */
		cree = TRUE;	/* a priori, on les cree */
		if (pElPage->ElNext != NULL)
		    /* la marque de page a un element suivant */
		{
		    pP = pElPage->ElNext->ElAbstractBox[pAb->AbDocView - 1];
		    if (pP == NULL)
			/* l'element suivant la marque de page n'a pas de pave */
			/* dans la vue,on ne cree pas les paves de la marque page */
			cree = FALSE;
		    else	
			/* on ne cree les paves de la marque de page que le pave */
			/* de l'element suivant est complete en tete. */
			cree = !pP->AbTruncatedHead;
		}
		if (cree)
		    /* cree les paves de la marque de page */
		    pP = AbsBoxesCreate(pElPage, pDoc, pAb->AbDocView, TRUE, TRUE, &complete);
		/* on met a jour les numeros des pages suivantes */
		MajNumeros(NextElement(pElPage), pElPage, pDoc, TRUE);
	    }
	}
	
	/* TODO quand on traitera les regles recto verso */	
	/* on verifie que la cardinalite de la page est en accord avec la regle
	   appliquee */
    }
}
#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |	trouvepave cherche dans le sous-arbre (racine comprise)		| */
/* |		du pave pointe' par P un pave de presentation du type	| */
/* |		NType defini dans le schema de presentation pointe' par	| */
/* |		pSchP. Si NType=0 on cherche un pave de presentation de	| */
/* |		nom presBoxName et on ignore pSchP.				| */
/* |		Retourne Vrai si un tel pave existe, et dans ce cas P	| */
/* |		pointe sur le pave found'				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean trouvepave(int Ntype, PtrPSchema pSchP, Name presBoxName, PtrAbstractBox *P)

#else /* __STDC__ */
static boolean trouvepave(Ntype, pSchP, presBoxName, P)
	int Ntype;
	PtrPSchema pSchP;
	Name presBoxName;
	PtrAbstractBox *P;
#endif /* __STDC__ */

{
  boolean         ret;
  PtrAbstractBox         PtPav;
  PtrAbstractBox         pAbb1;
  
  ret = FALSE;
  pAbb1 = *P;
  if (pAbb1->AbPresentationBox)
   if (pAbb1->AbLeafType == LtText)
    if (Ntype != 0)
      ret = strcmp(pAbb1->AbPSchema->PsPresentBox[pAbb1->AbTypeNum - 1].PbName,
		   pSchP->PsPresentBox[Ntype-1].PbName) == 0;
    else
      ret = strcmp(pAbb1->AbPSchema->PsPresentBox[pAbb1->AbTypeNum - 1].PbName,
		   presBoxName) == 0;
  if (!ret)
    if ((*P)->AbFirstEnclosed == NULL)
      ret = FALSE;
    else
      {
	PtPav = (*P)->AbFirstEnclosed;
	ret = trouvepave(Ntype, pSchP, presBoxName, &PtPav);
	while (!ret && PtPav->AbNext != NULL)
	  {
	    PtPav = PtPav->AbNext;
	    ret = trouvepave(Ntype, pSchP, presBoxName, &PtPav);
	  }
	if (ret)
	  *P = PtPav;
      }
  return ret;
}



/* ---------------------------------------------------------------------- */
/* |	ACopier	cherche dans le sous arbre de l'element pointe' par pEl	| */
/* |		(racine comprise) un element auquel est associee une	| */
/* |		regle de presentation creant une boite du type NType	| */
/* |		definie dans le schema de presentation pointe par pSchP.| */
/* |		Si NType=0, on cherche une regle creant une pave de nom	| */
/* |		presBoxName et au result, pSchP contient un pointeur sur le	| */
/* |		schema de presentation ou est definie la boite et NType	| */
/* |		contient le numero de type de cette boite.		| */
/* |		Retourne Vrai si un tel element existe et dans ce cas,	| */
/* |		pEl pointe sur l'element found'.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean ACopier(int *NType, PtrPSchema *pSchP, PtrSSchema *pSchS, Name presBoxName, PtrElement *pEl)

#else /* __STDC__ */
static boolean ACopier(NType, pSchP, pSchS, presBoxName, pEl)
	int *NType;
	PtrPSchema *pSchP;
	PtrSSchema *pSchS;
	Name presBoxName;
	PtrElement *pEl;
#endif /* __STDC__ */

{
  boolean         ret, stop;
  PtrElement      pE1;
  PtrPSchema      pSP;
  PtrSSchema	  pSS;
  PtrPRule    pRCre;
  PtrAttribute     pA;
  PtrPRule    pPRule;
  
  ret = FALSE;		
  /* cherche toutes les regles de  creation de cet element */
  pRCre = ReglePEl(*pEl, &pSP, &pSS, 0, NULL, 1, PtFunction, FALSE, FALSE, &pA);
  stop = FALSE;
  do
    if (pRCre == NULL)
      /* il n' y a plus de regles */
      stop = TRUE;
    else if (pRCre->PrType != PtFunction)
      /* il n'y a plus de regle fonction de presentation */
      stop = TRUE;
    else
      {
	/* la regle est une fonction de presentation */
	pPRule = pRCre;
	if (pPRule->PrPresFunction == FnCreateBefore 
	    || pPRule->PrPresFunction == FnCreateWith
	    || pPRule->PrPresFunction == FnCreateAfter
	    || pPRule->PrPresFunction == FnCreateFirst
	    || pPRule->PrPresFunction == FnCreateLast)	
	  /* c'est une regle de creation */
	  if (*NType != 0)
	    {
	      ret = pPRule->PrPresBox[0] == *NType ;
	      if (ret)
		ret = strcmp(pSS->SsName, (*pSchS)->SsName)== 0;
	/* on supprime le test sur l'egalite des schemas P et on teste uniquement */
	/* les schemas de structure : cela permet a des chapitres de se referencer */
	/* mutuellement meme si leur schema de presentation different legerement */
	/* il faut que les schemas P aient les memes boites de presentation utilisees */
	/* en copie */
	    }
	  else
	      ret = strcmp(pSP->PsPresentBox[pPRule->PrPresBox[0]-1].PbName, presBoxName)
		    == 0;
	  if (ret && (pSP != *pSchP)) 
		/* retourne le schema de presentation et le */
		/* numero de type de la boite creee */
		{
		  *pSchP = pSP;
		  *pSchS = pSS;
		  *NType = pPRule->PrPresBox[0];
		}
	   
	if (!ret)
	  pRCre = pPRule->PrNextPRule;
      }
  while (!(stop || ret));
  if (!ret)		
    /* ce n'est pas l'element cherche', on cherche dans ses */
    /* descendants */
    if (!(*pEl)->ElTerminal && (*pEl)->ElFirstChild != NULL)
      {
	pE1 = (*pEl)->ElFirstChild;
	ret = ACopier(NType, pSchP, pSchS, presBoxName, &pE1);
	while (!ret && pE1->ElNext != NULL)
	  {
	    pE1 = pE1->ElNext;
	    ret = ACopier(NType, pSchP, pSchS, presBoxName, &pE1);
	  }
	if (ret)
	  *pEl = pE1;
      }
  return ret;
}



/* ---------------------------------------------------------------------- */
/* |	CopieFeuilles  copie dans le pave pAb le contenu de toutes les	| */
/* |		feuilles de texte du sous-arbre de l'element pointe'	| */
/* |		par pEC							| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void CopieFeuilles(PtrElement pEC, PtrAbstractBox *pAb, PtrTextBuffer *pBuffPrec)

#else /* __STDC__ */
static void CopieFeuilles(pEC, pAb, pBuffPrec)
	PtrElement pEC;
	PtrAbstractBox *pAb;
	PtrTextBuffer *pBuffPrec;
#endif /* __STDC__ */

{
  PtrTextBuffer  pBuffE;
  PtrAbstractBox         pAbb1;
  PtrTextBuffer  pBuffP;
  
  if (!pEC->ElTerminal)
    /* ce n'est pas une feuille, on traite tous les fils */
    {
      pEC = pEC->ElFirstChild;
      while (pEC != NULL)
	{
	  CopieFeuilles(pEC, pAb, pBuffPrec);
	  pEC = pEC->ElNext;
	}
    }
  else
    /* c'est une feuille */ 
    if (pEC->ElLeafType == LtText)
      /* c'est une feuille de texte, on la copie */
      {
	pBuffE = pEC->ElText;
	/* pointeur sur le buffer de */
	/* l'element a copier */
	while (pBuffE != NULL)	
	  /* copie les buffers de l'element */ 
	  {
	    pAbb1 = *pAb;
	    GetBufTexte(&pBuffP);  
	    /* acquiert un buffer pour la copie */
	    /* chaine le buffer de la copie */
	    if (*pBuffPrec == NULL)
	      {
		pAbb1->AbText = pBuffP;
		/* c'est le 1er buffer */
		pAbb1->AbLanguage = pEC->ElLanguage;
	      }
	    else
	      {
		pBuffP->BuPrevious = *pBuffPrec;
		(*pBuffPrec)->BuNext = pBuffP;
	      }
	    strncpy(pBuffP->BuContent, pBuffE->BuContent, MAX_CHAR);
	    /* copie le contenu */
	    pBuffP->BuLength = pBuffE->BuLength; 
	    /* copie la longueur */
	    pAbb1->AbVolume += pBuffP->BuLength;
	    *pBuffPrec = pBuffP;
	    pBuffE = pBuffE->BuNext;
	  }
      }
}



/* ---------------------------------------------------------------------- */
/* |	ChercheDansSArbre cherche dans le sous-arbre dont la racine est	| */
/* |		pointe'e par pRac un element de numero de type TypeEl	| */
/* |		defini dans le schema de structure pSS, si typeName est	| */
/* |		nul, de nom de type typeName sinon.			| */
/* |		Retourne un pointeur sur l'element found' ou NULL si pas| */
/* |		found'							| */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static PtrElement ChercheDansSArbre(PtrElement pRac, int TypeEl, PtrSSchema pSS, Name typeName)

#else /* __STDC__ */
static PtrElement ChercheDansSArbre(pRac, TypeEl, pSS, typeName)
	PtrElement pRac;
	int TypeEl;
	PtrSSchema pSS;
	Name typeName;
#endif /* __STDC__ */

{
	PtrElement      pEC, pElChild;
  
	pEC = NULL;	/* a priori on n'a pas found' */
	if (typeName[0] != '\0')
	  /* on compare les noms de type */
	  {
	    if (strcmp(typeName, pRac->ElStructSchema->SsRule[pRac->ElTypeNumber - 1].SrName) == 0)
	       pEC = pRac;
	  }
	else
	  /* on compare les numero de type et code de schema de structure */
	  if (pRac->ElTypeNumber == TypeEl 
	      && pRac->ElStructSchema->SsCode == pSS->SsCode)
	      /* c'est l'element cherche' */
	      pEC = pRac;
	if (pEC == NULL)	
	  if (!pRac->ElTerminal)
	    {
	      /* on cherche parmi les fils de l'element */
	      pElChild = pRac->ElFirstChild;
	      while (pElChild != NULL && pEC == NULL)
		{
		  pEC = ChercheDansSArbre(pElChild, TypeEl, pSS, typeName);
		  pElChild = pElChild->ElNext;
		}
	    }
	return pEC;
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrPRule GetRegleCopy(PtrPRule pRP)
#else /* __STDC__ */
static PtrPRule GetRegleCopy(pRP)
	PtrPRule pRP;
#endif /* __STDC__ */
{
  boolean	found;

  found = FALSE;
  while (pRP != NULL && !found)
    {
      if (pRP->PrType == PtFunction)
	if (pRP->PrPresFunction == FnCopy)
	  found = TRUE;
      if (!found)
	{
	  pRP = pRP->PrNextPRule;
	  if (pRP != NULL)
	    if (pRP->PrType > PtFunction)
	      pRP = NULL;
	}
    }
  if (!found)
    pRP = NULL;
  return pRP;
}


/* ---------------------------------------------------------------------- */
/* |	applCopie applique une regle de copie.				| */
/* |		  Procedure appelee aussi dans modif.c			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void applCopie(PtrDocument pDoc, PtrPRule pPRule, PtrAbstractBox pAb, boolean AvecDescCopie)
#else /* __STDC__ */
void applCopie(pDoc, pPRule, pAb, AvecDescCopie)
	PtrDocument pDoc;
	PtrPRule pPRule;
	PtrAbstractBox pAb;
	boolean AvecDescCopie;
#endif /* __STDC__ */
{
  PtrElement      pE, /*pElSrce,*/ pElSv;
  PtrAbstractBox         pP;
  int             vue, i;
  boolean         found;
  PtrPSchema      pSchP;
  PtrSSchema    pSchS;
  PtrTextBuffer  pBuffPrec;
  int             TBoite;
  Name             NBoite;
  DocumentIdentifier IDoc;
  PtrDocument     pDocRef;
  PtrElement      pEl1;
  PtrAbstractBox         pAbb1;
  PresentationBox         *pBo1;
  PtrCopyDescr	  pDC;
  PtrPRule	  pRP;
  PtrAttribute     pAttr;	
  boolean	  Ref;
#ifdef __COLPAGE__
  PtrElement      pEl;
#endif /* __COLPAGE__ */

  pEl1 = pAb->AbElement;
  pE = NULL;
  Ref = FALSE;
  if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct == CsReference)
    {
      /* la regle Copy s'applique a un pave' d'un element reference */
      Ref = TRUE;
      /* cherche l'element qui est reference' */
      pE = ReferredElement(pEl1->ElReference, &IDoc, &pDocRef);
    }
  else
    if (pAb->AbPresentationBox)
      if (pAb->AbCreatorAttr != NULL)
        if (pAb->AbCreatorAttr->AeAttrType == AtReferenceAttr)
          /* la regle Copy s'applique a un pave' de presentation */
          /* cree' par un attribut reference */
          {
          Ref = TRUE;
          /* cherche l'element qui est reference' par l'attribut */
          pE = ReferredElement(pAb->AbCreatorAttr->AeAttrReference, &IDoc, &pDocRef);
          }
  if (Ref)
    {
      /* c'est une copie par reference */
      if (pE != NULL)
	/* l'element qui est reference' existe, il est pointe' par pE */
	if (pPRule->PrElement)
	  /* il faut copier le contenu d'un element structure' contenu */
	  /* dans l'element reference'. On cherche cet element */
	  pE = ChercheDansSArbre(pE, pPRule->PrPresBox[0], pEl1->ElStructSchema,
				 pPRule->PrPresBoxName);
	else	
	  /* il faut copier une boite de presentation */
	  /* prend le schema de presentation qui s'applique a la reference */
	  {
	    ChSchemaPres(pAb->AbElement, &pSchP, &i, &pSchS);
	    if (pPRule->PrNPresBoxes == 0)
	      /* la boite de presentation a copier est definie par son nom */
	      {
		TBoite = 0;
		strncpy(NBoite, pPRule->PrPresBoxName, MAX_NAME_LENGTH);
		/* nom de la boite a cherche */
	      } 
	    else	
	      /* la boite de presentation est definie par son numero de type */
	      TBoite = pPRule->PrPresBox[0];	
	    /* numero de type de la boite */
	    /* cherche dans toutes les vues une boite du type de celle a */
	    /* copier parmi les paves de cet element et de ses descendants */
	    vue = 0;
	    found = FALSE;
	    do
	      {
		vue++;	
		/* premier pave de l'element dans cette vue */
		pP = pE->ElAbstractBox[vue - 1];
		if (pP != NULL)
		  do
		    {
		      found = trouvepave(TBoite, pSchP, NBoite,&pP);
		      if (!found)
			{
			  pP = pP->AbNext;
			  if (pP != NULL)
			    if (pP->AbElement != pE)
			      pP = NULL;
			}
		    }
		  while (!(found || pP == NULL));
	      }
	    while (!(found || vue >= MAX_VIEW_DOC));
	    if (found)	
	      /* on a found' le pave a copier, on le copie */
	      {
		pAbb1 = pAb;
		pAbb1->AbLeafType = LtText;
		pAbb1->AbVolume = pP->AbVolume;
		if (pP->AbText != NULL)
		   *pAbb1->AbText = *pP->AbText;
		pAbb1->AbLanguage = pP->AbLanguage;
		pAbb1->AbCanBeModified = FALSE;
	      }
	    else
	      /* on n'a pas found le pave a copier */
	      if (!DansTampon(pE))
		/* on ne fait rien si l'element reference' est dans le buffer*/
		/* de Couper-Coller */
		/* on cherche dans le sous-arbre abstrait de l'element */
		/* reference' un element qui cree la boite a copier */
		{
		  /*pElSrce = pE;*/
		  found = ACopier(&TBoite, &pSchP, &pSchS, NBoite, &pE);
		  if (found)
		    /* on a found' l'element pE qui cree la boite a copier */
		    {
		      pBo1 = &pSchP->PsPresentBox[TBoite - 1];
		      if (pBo1->PbContent == ContVariable)
			/* on fait comme si le pave appartenait a l'element */
			/* a copier */
			{
			  pElSv = pAb->AbElement;
			  pAb->AbElement = pE;
			  found = NouvVariable(pBo1->PbContVariable, pSchS, pSchP,
					       pAb, pDoc);
			  /* on retablit le pointeur correct */
			  pAb->AbElement = pElSv;
			}
		      if (pBo1->PbContent == FreeContent)
			/* le contenu de la boite de presentation a copier */
			/* est lui-meme defini par une regle FnCopy */
			{
			/* on cherche cette regle FnCopy parmi les regles de */
			/* presentation de la boite de presentation a copier */
			pRP = GetRegleCopy(pBo1->PbFirstPRule);
			if (pRP != NULL)
			  /* on a found' la regle FnCopy. On l'applique en */
			  /* faisant comme si le pave appartenait a l'element*/
			  /* qui cree la boite de presentation a copier */
			  {
			  pElSv = pAb->AbElement;
			  pAb->AbElement = pE;
			  applCopie(pDoc, pRP, pAb, TRUE);
			  pAb->AbElement = pElSv;
			  pE = NULL;
			  }
			}
		    }
		}
	  }
    } 
  else
    /* ce n'est pas une copie par reference */ 
    if (pPRule->PrElement)
      {
      /*cherche d'abord l'element a copier a l'interieur de l'element copieur*/
      pE = ChercheDansSArbre(pAb->AbElement, pPRule->PrPresBox[0],
			     pEl1->ElStructSchema, pPRule->PrPresBoxName);

      if (pE == NULL)
	/* on n'a pas found' l'element a copier */
	if (pEl1->ElTypeNumber == PageBreak + 1)
	  if (pEl1->ElPageType == PgBegin)
#ifdef __COLPAGE__
	    /* la marque de page est placee devant l'element qui porte */
	    /* la regle de creation de marque page */
	    /* sauf dans le cas ou la marque de page a ete cree par la racine*/
	  {
	    pEl = pEl1;
	    /* on saute les eventuelles autres marques de pages */
	    /*  (des autres vues) et marques de colonnes */
	    while (pEl != NULL 
	           && pEl->ElTypeNumber == PageBreak + 1)
	      pEl = pEl->ElNext;
	    /* pEl est l'element qui a cree la marque de page */
	    /*  On cherche dans cet element */
	    if (pEl != NULL)
	      pE = ChercheDansSArbre(pEl, pPRule->PrPresBox[0],
				    pEl->ElStructSchema, pPRule->PrPresBoxName);
	    if (pE == NULL)
	      /* si on n'a pas found pE, c'est que c'etait une marque */
	      /* page qui avait ete genere par la racine : elle a ete */
	      /* placee comme premier fils : on applique lors le code */
	      /* de la V3 (recherche sur le pere)  */
	      pE = ChercheDansSArbre(pEl1->ElParent, pPRule->PrPresBox[0],
	       		    pEl1->ElStructSchema, pPRule->PrPresBoxName);
	   }
#else /* __COLPAGE__ */
	     /* on travaille pour une marque de page qui est engendree par */
	     /* le debut d'un element. On cherche dans cet element */
	     pE = ChercheDansSArbre(pEl1->ElParent, pPRule->PrPresBox[0],
				    pEl1->ElStructSchema, pPRule->PrPresBoxName);
#endif /* __COLPAGE__ */
      /* si on n'a pas found', on cherche en arriere l'element a copier */
      if (pE == NULL)
        if (pPRule->PrNPresBoxes > 0)
	  /* la boite a copier est definie par son numero de type */
          pE = BackSearchTypedElem(pAb->AbElement, pPRule->PrPresBox[0], pEl1->ElStructSchema);
/*        else */
	  /* la boite a copier est definie par son nom */
	  /* non implemente' */
      }
  if (pPRule->PrElement && pE != NULL)
    /* il faut copier l'element structure' pointe' par pE */
    {
      pAbb1 = pAb;
      /* initialise le pave */
      pAbb1->AbLeafType = LtText;
      pAbb1->AbCanBeModified = FALSE;
      pAbb1->AbVolume = 0;
      pBuffPrec = NULL;
      /* pas de buffer precedent */
      if (TypeHasException(1207, pAbb1->AbElement->ElTypeNumber, pAbb1->AbElement->ElStructSchema))
	{
	  if (ThotLocalActions[T_indexcopy] != NULL)
	    (*ThotLocalActions[T_indexcopy])(pE, &pAb, &pBuffPrec);
	}
      else
	{
	/* si l'element a copier est lui-meme une reference qui copie un */
	/* autre element, c'est cet autre element qu'on copie */
	pRP = NULL;
	if (pE->ElStructSchema->SsRule[pE->ElTypeNumber - 1].SrConstruct == CsReference)
	   {
	   pRP = ReglePEl(pE, &pSchP, &pSchS, 0, NULL, 1, PtFunction, FALSE, FALSE, &pAttr);
	   pRP = GetRegleCopy(pRP);
	   }
	if (pRP == NULL)
	  /* copie simplement toutes les feuilles de texte de pE */
          CopieFeuilles(pE, &pAb, &pBuffPrec);
	else
	  /* applique la regle de copie transitive */
	  {
	  pElSv = pAb->AbElement;
	  pAb->AbElement = pE;
	  applCopie(pDoc, pRP, pAb, FALSE);
	  pAb->AbElement = pElSv;
	  }
	}
	      if (AvecDescCopie)
	{
	/* ajoute a l'element copie' un descripteur d'element copie' */
	GetDescCopie(&pDC);
	pDC->CdCopiedAb = pAb;
	pDC->CdCopiedElem = pE;
	pDC->CdCopyRule = pPRule;
	pDC->CdNext = pE->ElCopyDescr;
	pDC->CdPrevious = NULL;
	if (pDC->CdNext != NULL)
	   pDC->CdNext->CdPrevious = pDC;
	pE->ElCopyDescr = pDC;
	pAbb1->AbCopyDescr = pDC;
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	Applique   applique au pave pointe par pAb la regle pointee par| */
/* |		pPRule dans le schema de presentation pointe par pSchP.	| */
/* |		Si pAttr n'est pas NULL, c'est un pointeur sur le bloc	| */
/* |		attribut auquel correspond la regle a appliquer.	| */
/* |		Retourne true si la regle a ete appliquee ou ne pourra	| */
/* |		jamais etre appliquee, false si elle n'a pas pu etre	| */
/* |		appliquee mais qu'elle pourra etre appliquee quand	| */
/* |		d'autres paves seront construits.			| */
	/* le boolean de pavedetruit indique si le pave pAb a ete detruit| */
/* ---------------------------------------------------------------------- */
#ifdef __COLPAGE__
#ifdef __STDC__
boolean Applique(PtrPRule pPRule, PtrPSchema pSchP, PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr, boolean *pavedetruit)
#else /* __STDC__ */
boolean Applique(pPRule, pSchP, pAb, pDoc, pAttr, pavedetruit)
	PtrPRule pPRule;
	PtrPSchema pSchP;
	PtrAbstractBox pAb;
	PtrDocument pDoc;
	PtrAttribute pAttr;
	boolean *pavedetruit;
#endif /* __STDC__ */
#else /* __COLPAGE__ */
#ifdef __STDC__
boolean Applique(PtrPRule pPRule, PtrPSchema pSchP, PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr)
#else /* __STDC__ */
boolean Applique(pPRule, pSchP, pAb, pDoc, pAttr)
	PtrPRule pPRule;
	PtrPSchema pSchP;
	PtrAbstractBox pAb;
	PtrDocument pDoc;
	PtrAttribute pAttr;
#endif /* __STDC__ */
#endif /* __COLPAGE__ */
{
  boolean         appl;
  TypeUnit        unit;
  AbPosition     Posit;
  char            c;
  int             viewSch;
  PtrAbstractBox         pAbb1;
  PictInfo *myImageDescriptor;
#ifdef __COLPAGE__
  *pavedetruit = FALSE;
#else /* __COLPAGE__ */
  boolean         danspage, apresSautPage;
  AbPosition     *pPavP1;
#endif /* __COLPAGE__ */
  
  appl = TRUE;
  if (pPRule != NULL && pAb != NULL)
    if (pAb->AbElement != NULL)
    {
      pAbb1 = pAb;
      viewSch = AppliedView(pAbb1->AbElement, pAttr, pDoc, pAbb1->AbDocView);
      switch (pPRule->PrType)
	{
	case PtWidth:
	  appldimension(&pAbb1->AbWidth, pAb, pSchP, pAttr, &appl,pPRule,pDoc);
	  break;
	case PtHeight:
	  appldimension(&pAbb1->AbHeight, pAb, pSchP, pAttr, &appl,pPRule,pDoc);
	  /* traitement special pour le debordement vertical des cellules*/
	  /* de tableau etendues verticalement */
	  if (ThotLocalActions[T_vertspan]!= NULL)
	    (*ThotLocalActions[T_vertspan])(pPRule, pAb);
	  break;
	case PtVisibility:
	  pAbb1->AbVisibility = valintregle(pPRule, pAbb1->AbElement, 
					    pAbb1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbVisibility = 10;
	      appl = TRUE;
	    }
	  break;
	case PtDepth:
	  pAbb1->AbDepth = valintregle(pPRule, pAbb1->AbElement, pAbb1->AbDocView, 
				      &appl, &unit, pAttr);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbDepth = 0;
	      appl = TRUE;
	    }
	  break;
	case PtFillPattern:
	  pAbb1->AbFillPattern = valintregle(pPRule, pAbb1->AbElement, 
					 pAbb1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbFillPattern = 0;
	      appl = TRUE;
	    }
	  break;
	case PtBackground:
	  pAbb1->AbBackground = valintregle(pPRule, pAbb1->AbElement, 
					    pAbb1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbBackground = 0;
	      appl = TRUE;
	    }
	  break;
	case PtForeground:
	  pAbb1->AbForeground = valintregle(pPRule, pAbb1->AbElement, 
					    pAbb1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbForeground = 1;
	      appl = TRUE;
	    }
	  break;
	case PtLineStyle:
	  pAbb1->AbLineStyle = CharRule(pPRule, pAbb1->AbElement,
					    pAbb1->AbDocView, &appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    {
	      pAbb1->AbLineStyle = 'S';
	      appl = TRUE;
	    }
	  break;
	case PtFont:
	  pAbb1->AbFont = CharRule(pPRule, pAbb1->AbElement, pAbb1->AbDocView,
					&appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    {
	      pAbb1->AbFont = 'T';
	      appl = TRUE;
	    }
	  if (pAbb1->AbFont >= 'a' && pAbb1->AbFont <= 'z')
	    /* on n'utilise que des majuscules pour les noms de police */
	    pAbb1->AbFont = (char)((int)(pAbb1->AbFont) - 32);
	  break;
	case PtAdjust:
	  pAbb1->AbAdjust = AlignRule(pPRule, pAbb1->AbElement, 
					 pAbb1->AbDocView, &appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbAdjust = AlignLeft;
	      appl = TRUE;
	    }
	  break;
	case PtJustify:
	  pAbb1->AbJustify = BoolRule(pPRule, pAbb1->AbElement, 
					 pAbb1->AbDocView, &appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbJustify = FALSE;
	      appl = TRUE;
	    }
	  break;
	case PtHyphenate:
	  pAbb1->AbHyphenate = BoolRule(pPRule, pAbb1->AbElement, 
					    pAbb1->AbDocView, &appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbHyphenate = FALSE;
	      appl = TRUE;
	    }
	  break;
	case PtStyle:
	  c = CharRule(pPRule, pAbb1->AbElement, pAbb1->AbDocView, &appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbHighlight = 0;
	      appl = TRUE;
	    }
	  else
	    switch (c)
	      {
	    case 'I':
		pAbb1->AbHighlight = 2;
		break;
	    case 'B':
		pAbb1->AbHighlight = 1;
		break;
		/*iso*/	      case 'O':
		pAbb1->AbHighlight = 3;
		break;
	    case 'G':
		pAbb1->AbHighlight = 4;
		break;
	    case 'Q':
		pAbb1->AbHighlight = 5;
		break;
	    default:
		pAbb1->AbHighlight = 0;
		break;
	      }
	  break;
	case PtUnderline:
	  c = CharRule(pPRule, pAbb1->AbElement, pAbb1->AbDocView, &appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbUnderline = 0;
	      appl = TRUE;
	    } 
	  else
	    switch (c)
	      {
	    case 'C':
		pAbb1->AbUnderline = 3;
		break;
	    case 'O':
		pAbb1->AbUnderline = 2;
		break;
	    case 'U':
		pAbb1->AbUnderline = 1;
		break;
	    default:
		pAbb1->AbUnderline = 0;
		break;
	      }
	  break;
	case PtThickness:
	  c = CharRule(pPRule, pAbb1->AbElement, pAbb1->AbDocView, &appl);
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbThickness = 0;
	      appl = TRUE;
	    } 
	  else
	    switch (c)
	      {
	    case 'T':
		pAbb1->AbThickness = 1;
		break;
	    default:
		pAbb1->AbThickness = 0;
		break;
	      }
	  break;
	case PtSize:
	  /* on applique la regle de taille */
	  pAbb1->AbSize = valintregle(pPRule, pAbb1->AbElement, 
					pAbb1->AbDocView, &appl, &unit, pAttr);
	  if (appl)
	    pAbb1->AbSizeUnit = unit;
	  else
	    if (pAbb1->AbElement->ElParent == NULL)
	      /* c'est la racine, on met a priori la valeur par defaut */
	      {
	        pAbb1->AbSize = 3;
	        pAbb1->AbSizeUnit = UnRelative;
		appl = TRUE;
	      }
	  break;
	case PtIndent:
	  pAbb1->AbIndent = valintregle(pPRule, pAbb1->AbElement, 
					pAbb1->AbDocView, &appl, &unit, pAttr);
	  pAbb1->AbIndentUnit = unit;
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbIndent = 0;
	      appl = TRUE;
	    }
	  break;
	case PtLineSpacing:
	  pAbb1->AbLineSpacing = valintregle(pPRule, pAbb1->AbElement, 
					    pAbb1->AbDocView, &appl, &unit, pAttr);
	  pAbb1->AbLineSpacingUnit = unit;
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbLineSpacing = 10;
	      pAbb1->AbLineSpacingUnit = UnRelative;
	      appl = TRUE;
	    }
	  break;
	case PtLineWeight:
	  pAbb1->AbLineWeight = valintregle(pPRule, pAbb1->AbElement, 
					    pAbb1->AbDocView, &appl, &unit, pAttr);
	  pAbb1->AbLineWeightUnit = unit;
	  if (!appl && pAbb1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAbb1->AbLineWeight = 1;
	      pAbb1->AbLineWeightUnit = UnPoint;
	      appl = TRUE;
	    }
	  break;
	case PtVertRef:
	  Posit = pAbb1->AbVertRef;
	  applPosRelat(&Posit, pPRule->PrPosRule, pPRule, pAttr, pSchP, pAb,
		       pDoc, &appl);
	  pAbb1->AbVertRef = Posit;
	  break;
	case PtHorizRef:
	  Posit = pAbb1->AbHorizRef;
	  applPosRelat(&Posit, pPRule->PrPosRule, pPRule, pAttr, pSchP, pAb,
		       pDoc, &appl);
	  pAbb1->AbHorizRef = Posit;
	  break;
	case PtVertPos:
	 /* erreur : ce n'est pas a l'editeur d'interpreter */
	 /* la mise en ligne. On supprime ce code */	  
#ifdef __COLPAGE__
	  /* changement complete du code */
	  /* plus de cas particulier pour les pages et colonnes */
	  /* ce n'est pas un cas particulier : on applique */
	  /* ses regles */
	  /* applique la regle de positionnement de l'element */
	  Posit = pAbb1->AbVertPos;
	  applPosRelat(&Posit, pPRule->PrPosRule, pPRule, pAttr, pSchP, pAb,
		       pDoc, &appl);
	  pAbb1->AbVertPos = Posit;
	  /* traitement special pour le debordement vertical des cellules*/
	  /* de tableau etendues verticalement */
	  if (ThotLocalActions[T_vertspan]!= NULL)
	    (*ThotLocalActions[T_vertspan])(pPRule, pAb);
	  break;
#else /* __COLPAGE__ */
	      /* Si le precedent est un separateur de page, le pave est */
	      /* positionne' en dessous de ce saut de page, sauf si le pave */
	      /* positionne' fait partie d'un separateur de page (c'est une */
	      /* boite de haut ou de bas de page qui doit etre placee par */
	      /* rapport au filet separateur) ou s'il se positionne par */
	      /* rapport a un autre element. */
	      apresSautPage = FALSE;
	      if (pAbb1->AbPrevious != NULL)
		/* il y a un pave precedent */ 
		{
		  if (!pAbb1->AbPrevious->AbDead 
		      && pAbb1->AbPrevious->AbElement->
		      ElTypeNumber == PageBreak + 1 
		      && pAbb1->AbElement->ElTypeNumber != PageBreak + 1)
		    if (pPRule->PrPosRule.PoRelation == RlSameLevel 
		        || pPRule->PrPosRule.PoRelation == RlPrevious)
		     apresSautPage = TRUE;
		    else 
		      {
			if (pPRule->PrPosRule.PoRelation == RlEnclosing)
		          {
		  	    if (pAbb1->AbPrevious->AbElement->
			        ElPageType != PgBegin)
		              apresSautPage = TRUE;
		          }  
		      }
		}
	      else
		/* il n'y a pas de pave precedent */ 
		if (pAbb1->AbElement->ElPrevious != NULL)	
		  /* il y a un element precedent */
		  if (pAbb1->AbElement->ElPrevious->ElTypeNumber == PageBreak + 1 
		      && pAbb1->AbElement->ElPrevious->ElViewPSchema == viewSch 
		      && pAbb1->AbElement->ElTypeNumber != PageBreak + 1)
		    /* l'element precedent est une marque de page pour la vue */
		    if (pPRule->PrPosRule.PoRelation == RlSameLevel 
			|| pPRule->PrPosRule.PoRelation == RlPrevious )
		      apresSautPage = TRUE;
	      if (apresSautPage)	
		/* position: en dessous du pave precedent */ 
		if (pAbb1->AbPrevious == NULL)
		  /* le pave de la marque de page n'est pas encore cree', on */
		  /* ne peut pas appliquer la regle de positionnement */
		  appl = FALSE;
		else
		  {
		    pPavP1 = &pAbb1->AbVertPos;
		    pPavP1->PosEdge = Top;
		    pPavP1->PosRefEdge = Bottom;
		    pPavP1->PosDistance = 0;
		    pPavP1->PosUnit = UnPoint;
		    pPavP1->PosAbRef = pAbb1->AbPrevious;
		    pPavP1->PosUserSpecified = FALSE;
		  }
	      else	
		/* s'il s'agit d'une boite page (celle qui englobe le filet */
		/* et les hauts et pieds de page), et si elle n'est precedee */
		/* d'aucun element, elle se positionne en haut de l'englobant. */
		{
		  danspage = FALSE;
		  if (pAbb1->AbEnclosing != NULL)
		    if (pAbb1->AbElement->ElTypeNumber == PageBreak + 1 
			&& pAbb1->AbEnclosing->AbElement->
			ElTypeNumber != PageBreak + 1 
			&& pAbb1->AbElement->ElPrevious == NULL)
		      if (pAbb1->AbPrevious == NULL)
			danspage = TRUE;
		  
		  if (danspage)
		    {
		      pPavP1 = &pAbb1->AbVertPos;
		      pPavP1->PosAbRef = pAbb1->AbEnclosing;
		      pPavP1->PosEdge = Top;
		      pPavP1->PosRefEdge = Top;
		      pPavP1->PosDistance = 0;
		      pPavP1->PosUnit = UnPoint;
		      pPavP1->PosUserSpecified = FALSE;
		    } 
		  else	
		    /* applique la regle de positionnement de l'element */
		    {
		      Posit = pAbb1->AbVertPos;
		      applPosRelat(&Posit, pPRule->PrPosRule, pPRule, pAttr,
				   pSchP, pAb, pDoc, &appl);
		      pAbb1->AbVertPos = Posit;
		    }
		}
	      /* traitement special pour le debordement vertical des cellules*/
	      /* de tableau etendues verticalement */
	      if (ThotLocalActions[T_vertspan]!= NULL)
		(*ThotLocalActions[T_vertspan])(pPRule, pAb);
	  break;
#endif /* __COLPAGE__ */
	case PtHorizPos:
	      Posit = pAbb1->AbHorizPos;
	      applPosRelat(&Posit, pPRule->PrPosRule, pPRule, pAttr, pSchP,
			   pAb, pDoc, &appl);
	      pAbb1->AbHorizPos = Posit;
	  break;
	case PtFunction:
	  switch (pPRule->PrPresFunction)
	    {
          case FnLine:
	      if (pAbb1->AbLeafType == LtCompound)
		/* si la regle de mise en lignes est definie pour la */
		/* vue principale, elle s'applique a toutes les vues, */
		/* sinon, elle ne s'applique qu'a la vue pour laquelle */
		/* elle est definie */
		if (pPRule->PrViewNum == 1 || pPRule->PrViewNum == viewSch)
		  pAbb1->AbInLine = TRUE;
	      break;
	  case FnNoLine:
	      if (pAbb1->AbLeafType == LtCompound)
		if (pPRule->PrViewNum == viewSch)
		   pAbb1->AbInLine = FALSE;
	      break;
          case FnPage:
#ifdef __COLPAGE__
	      if (applPage(pDoc, pAb, viewSch, pPRule, pPRule->PrPresFunction))
	        *pavedetruit = TRUE;
#else /* __COLPAGE__ */
#endif /* __COLPAGE__ */
	      applPage(pDoc, pAb, viewSch, pPRule, pPRule->PrPresFunction);
	      break;
          case FnColumn:
#ifdef __COLPAGE__
	      if (applCol(pDoc, pAb, viewSch, pPRule))
	        *pavedetruit = TRUE;
#endif /* __COLPAGE__ */
	      break;
	  case FnSubColumn:
	      
	      break;
          case FnCopy:
	      if (!pAb->AbElement->ElHolophrast)
	        /* on n'applique pas la regle copie a un element holophraste'*/
	        applCopie(pDoc, pPRule, pAb, TRUE);
	      break;
	  case FnContentRef:
	      ConstantCopy(pPRule->PrPresBox[0], pSchP, pAb);
	      break;
          default:
	      break;
	    }
	  
	  break;
	case PtPictInfo:
	  UpdateImageDescriptor(pAbb1->AbPictInfo, (int *)&(pPRule->PrPictInfo));
	  myImageDescriptor = (PictInfo *)pAbb1->AbPictInfo;
	  myImageDescriptor->PicFileName = pAbb1->AbElement->ElText->BuContent;
	  break;
	default:
	  break;
	}
      
    }
  return appl;
}


/* ---------------------------------------------------------------------- */
/* |	ChReglePres	Cherche si la regle de presentation specifique	| */
/* |		de type TypeR concernant la vue de numero Vue existe	| */
/* |		pour l'element pEl.					| */
/* |		Retourne un pointeur sur cette regle si elle existe,	| */
/* |		sinon cree une nouvelle regle de ce type, l'ajoute a la	| */
/* |		chaine des regles de presentation specifiques de	| */
/* |		l'element et retourne un pointeur sur la nouvelle regle.| */
/* |		Au result, isNew indique s'il s'agit d'une regle	| */
/* |		nouvellement creee ou non.				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrPRule ChReglePres(PtrElement pEl, PRuleType TypeR, boolean *isNew, PtrDocument pDoc, int Vue)
#else /* __STDC__ */
PtrPRule ChReglePres(pEl, TypeR, isNew, pDoc, Vue)
	PtrElement pEl;
	PRuleType TypeR;
	boolean *isNew;
	PtrDocument pDoc;
	int Vue;
#endif /* __STDC__ */
{
  PtrPRule    pResultRule;
  PtrPRule    pPRule;
  
  *isNew = FALSE;
  pResultRule = NULL;
  if (pEl != NULL)
    {
      /* l'element du pave */ 
      if (pEl->ElFirstPRule == NULL)
	{
	  /* cet element n'a aucune regle de presentation specifique, on en */
	  /* cree une et on la chaine a l'element */
	  GetReglePres(&pResultRule);
	  *isNew = TRUE;
	  pEl->ElFirstPRule = pResultRule;
	  pResultRule->PrType = TypeR;
	} 
      else
	{
	  /* cherche parmi les regles de presentation specifiques de
	     l'element si ce type de regle existe pour la vue
	     a laquelle appartient le pave. */
	  pPRule = pEl->ElFirstPRule; /* premiere regle specifique de l'element */
	  while (pResultRule == NULL)
	    {
	      if (pPRule->PrType == TypeR &&
		  pPRule->PrViewNum == pDoc->DocView[Vue - 1].DvPSchemaView)
		/* la regle existe deja */
		pResultRule = pPRule;
	      else if (pPRule->PrNextPRule != NULL)
		/* passe a la regle specifique suivante de l'element */
		pPRule = pPRule->PrNextPRule;
	      else
		{
		  /* On a examine' toutes les regles specifiques de */
		  /* l'element, ajoute une nouvelle regle en fin de chaine */
		  GetReglePres(&pResultRule);
		  *isNew = TRUE;
		  pPRule->PrNextPRule = pResultRule;
		  pResultRule->PrType = TypeR;
		}
	    }
	}
    }
  return pResultRule;
}



/* ---------------------------------------------------------------------- */
/* |	PavReaff indique dans le contexte du document que le pave pAb	| */
/* |		est a reafficher					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void PavReaff(PtrAbstractBox pAb, PtrDocument pDoc)

#else /* __STDC__ */
void PavReaff(pAb, pDoc)
	PtrAbstractBox pAb;
	PtrDocument pDoc;
#endif /* __STDC__ */

{
  
  
  if (!AssocView(pAb->AbElement))
    pDoc->DocViewModifiedAb[pAb->AbDocView - 1] =
      Englobant(pAb, pDoc->DocViewModifiedAb[pAb->AbDocView-1]);
  else
    pDoc->DocAssocModifiedAb[pAb->AbElement->ElAssocNum - 1] =
      Englobant(pAb,pDoc->DocAssocModifiedAb[pAb->
					  AbElement->ElAssocNum-1]);
}



/* ---------------------------------------------------------------------- */
/* |	NouvDimImage fixe les dimensions d'un pave-image  lorsque	| */
/* |		le driver d'image ne sait pas donner une dimension a	| */
/* |		cette image. C'est le cas lorsqu'on ne tient pas compte	| */
/* |		de la cropping frame  (pour le CGM, par exemple.)	| */
/* |		Cette fonction fait le meme travail que NouvDimension	| */
/* |		sans reafficher le document a la fin, laissant ce	| */
/* |		travail au driver d'images.				| */
/* |		On traite le cas ou une IMAGE est dimensionnee par son	| */
/* |		contenu comme si c'etait une dimension fixe.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void NouvDimImage(PtrAbstractBox pAb)
#else /* __STDC__ */
void NouvDimImage(pAb)
	PtrAbstractBox pAb;
#endif /* __STDC__ */
{
  boolean         nouveau, ok;
  PtrPRule    pPRuleDimH, pPRuleDimV, pR, pRStd;
  PtrPSchema      pSPR;
  PtrSSchema    pSSR;
  PtrDocument     pDoc;
  PtrElement      pEl;
  PtrAttribute	    pAttr;
  int             hauteur, largeur;
  int             frame[MAX_VIEW_DOC];
  int             viewSch;
  PtrAbstractBox         pAbb;
  int             VueDoc;
  boolean         stop;
  int		  Vue;
#ifdef __COLPAGE__
  boolean   bool; 
#endif /* __COLPAGE__ */
  
  pPRuleDimH = NULL;
  pPRuleDimV = NULL;
  /* nettoie la table des frames a reafficher */
  for (VueDoc = 1; VueDoc <= MAX_VIEW_DOC; VueDoc++)
    frame[VueDoc - 1] = 0;
  pEl = pAb->AbElement;	/* l'element auquel correspond le pave */
  pDoc = DocumentOfElement(pEl);	/* le document auquel appartient le pave */ 
  Vue = pAb->AbDocView;		/* la vue concernee */
  viewSch = AppliedView(pEl, NULL, pDoc, Vue);/* type de cette vue dans le schema P */
  
  /* les deltas de dimension que l'on va appliquer sont ceux 
     de la boite par defaut avec laquelle on a cree l'image */
  hauteur = PixelToPoint (pAb->AbBox->BxHeight);
  largeur = PixelToPoint (pAb->AbBox->BxWidth);
  
  /* traite le changement de largeur */
  
  /* cherche d'abord la regle de dimension qui s'applique a l'element */
  pRStd = ReglePEl(pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtWidth, FALSE, TRUE, &pAttr);
  /* on ne s'occupe que du cas ou l'image est dimensionnee par le contenu */
  ok = FALSE;
  if (!pRStd->PrDimRule.DrPosition)
    if (pRStd->PrDimRule.DrRelation == RlEnclosed)
      /* largeur du contenu */
      if (pAb->AbLeafType == LtPicture)
	ok = TRUE;
  
  if (ok)
    {
      /* cherche si l'element a deja une regle de largeur specifique */
      pPRuleDimH = ChReglePres(pEl, PtWidth, &nouveau, pDoc, Vue);
      if (nouveau)
	/* on a cree' une regle de largeur pour l'element */
	{
	  pR = pPRuleDimH->PrNextPRule;	/* on recopie la regle standard */
	  *pPRuleDimH = *pRStd;
	  pPRuleDimH->PrNextPRule = pR;
	  pPRuleDimH->PrCond = NULL;
	  pPRuleDimH->PrViewNum = viewSch;
	}
      pPRuleDimH->PrDimRule.DrAbsolute = TRUE;
      pPRuleDimH->PrDimRule.DrSameDimens = FALSE;
      pPRuleDimH->PrDimRule.DrMin = FALSE;
      pPRuleDimH->PrDimRule.DrUnit = UnPoint;
      pPRuleDimH->PrDimRule.DrAttr = FALSE;
      
      /* change la longueur dans la regle specifique */
      pPRuleDimH->PrDimRule.DrValue = largeur;
    }
  
  /* traite le changement de hauteur de la boite */
  
  /* cherche d'abord la regle de dimension qui s'applique a l'element */
  pRStd = ReglePEl(pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtHeight, FALSE, TRUE, &pAttr);
  /* on ne s'occupe que du cas ou l'image est dimensionnee par le contenu */
  ok = FALSE;
  if (!pRStd->PrDimRule.DrPosition)
    if (pRStd->PrDimRule.DrRelation == RlEnclosed)
      /* largeur du contenu */
      if (pAb->AbLeafType == LtPicture)
	ok = TRUE;
  
  if (ok)
    {
      /* cherche si l'element a deja une regle de hauteur specifique */
      pPRuleDimV = ChReglePres(pEl, PtHeight, &nouveau, pDoc, Vue);
      if (nouveau)
	/* on a cree' une regle de hauteur pour l'element */
	{
	  pR = pPRuleDimV->PrNextPRule;	/* on recopie la regle standard */
	  *pPRuleDimV = *pRStd;
	  pPRuleDimV->PrNextPRule = pR;
	  pPRuleDimV->PrCond = NULL;
	  pPRuleDimV->PrViewNum = viewSch;
	}
      pPRuleDimV->PrDimRule.DrAbsolute = TRUE;
      pPRuleDimV->PrDimRule.DrSameDimens = FALSE;
      pPRuleDimV->PrDimRule.DrMin = FALSE;
      pPRuleDimV->PrDimRule.DrUnit = UnPoint;
      pPRuleDimV->PrDimRule.DrAttr = FALSE;
      
      /* change le parametre de la regle */
      pPRuleDimV->PrDimRule.DrValue = hauteur;
    }
  
  /* applique les nouvelles regles de presentation */
  
  if (pPRuleDimV != NULL || pPRuleDimH != NULL)
    {
      pDoc->DocModified = TRUE;    /* le document est modifie' */
      for (VueDoc = 1; VueDoc <= MAX_VIEW_DOC; VueDoc++)
	if (pEl->ElAbstractBox[VueDoc -1] != NULL)
	  /* l'element traite' a un pave dans cette vue */
	  if (pDoc->DocView[VueDoc - 1].DvSSchema == pDoc->DocView[Vue -1].DvSSchema &&
	      pDoc->DocView[VueDoc - 1].DvPSchemaView == pDoc->DocView[Vue -1].DvPSchemaView)
	    /* c'est une vue de meme type que la vue traitee, on */
	    /* traite le pave de l'element dans cette vue */
	    {
	      pAbb = pEl->ElAbstractBox[VueDoc -1];
	      /* saute les paves de presentation */
	      stop = FALSE;
	      do
		if (pAbb == NULL)
		  stop = TRUE;
		else
		  if (!pAbb->AbPresentationBox)
		    stop = TRUE;
		  else
		    pAbb = pAbb->AbNext;
	      while (!stop);
	      
	      if (pAbb != NULL)
		{
		  /* applique la nouvelle regle specifique Horizontale */
		  if (pPRuleDimH != NULL)
#ifdef __COLPAGE__
		    if (Applique(pPRuleDimH, pSPR, pAbb, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
		    if (Applique(pPRuleDimH, pSPR, pAbb, pDoc, pAttr))
#endif /* __COLPAGE__ */
		      pAbb->AbWidthChange = TRUE;
		  /* applique la nouvelle regle specifique Verticale */
		  if (pPRuleDimV != NULL)
#ifdef __COLPAGE__
		    if (Applique(pPRuleDimV, pSPR, pAbb, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
		    if (Applique(pPRuleDimV, pSPR, pAbb, pDoc, pAttr))
#endif /* __COLPAGE__ */
		      pAbb->AbHeightChange = TRUE;
		  
		  PavReaff(pAbb, pDoc); /* indique le pave a reafficherv */
		  if (!AssocView(pEl))
		    frame[VueDoc -1] = pDoc->DocViewFrame[VueDoc -1];
		  else
		    frame[VueDoc -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
		}
	    }
    }
}
/* End Of Module pres */
