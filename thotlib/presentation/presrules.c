
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
#include "environ.var"
#include "appdialogue.var"

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
/* |	SuivNonPres Si pP pointe un pave de presentation, retourne	| */
/* |		dans pP le premier pave qui n'est pas un pave de	| */
/* |		presentation et qui suit le pave pP a l'appel.		| */
/* |		Retourne NULL si le pave n'est suivi que de paves de	| */
/* |		presentation. Si, a l'appel, pP est  un pave qui n'est	| */
/* |		pas un pave de presentation, alors pP reste inchange'.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void SuivNonPres(PtrAbstractBox *pP)
#else /* __STDC__ */
static void SuivNonPres(pP)
	PtrAbstractBox *pP;
#endif /* __STDC__ */
{
  boolean	stop;
  
  
  stop = FALSE;
  do
    if (*pP == NULL)
      stop = TRUE;
    else if (!(*pP)->AbPresentationBox)
      stop = TRUE;
    else
      *pP = (*pP)->AbNext;
  while (!(stop));
}


/* ---------------------------------------------------------------------- */
/* |	HeritAsc	  rend le premier element pH ascendant de pE	| */
/* |			  et qui possede un pave dans la vue nv		| */
/* |			  retourne ce pave dans pP ou NULL sinon 	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void HeritAsc(PtrElement pE, DocViewNumber nv, PtrAbstractBox *pP, PtrElement *pH)
#else /* __STDC__ */
static void HeritAsc(pE, nv, pP, pH)
	PtrElement pE;
	DocViewNumber nv;
	PtrAbstractBox *pP;
	PtrElement *pH;
#endif /* __STDC__ */
{
  
  *pH = pE;
  if ((*pH)->ElParent == NULL)
    *pP = NULL;
  else
    while ((*pH)->ElParent != NULL && *pP == NULL)
      {
	*pH = (*pH)->ElParent;
	*pP = (*pH)->ElAbstractBox[nv - 1];
	if (*pP != NULL)
	  if ((*pP)->AbDead)
	    *pP = NULL;
      }
}



/* ---------------------------------------------------------------------- */
/* |	ptrHerit  rend le pointeur sur le pave correpondant a l'element	| */
/* |	qui sert de reference quand on applique la regle d'heritage	| */
/* |	pointe par pRegle a l'element pointe par pEl, dans la vue nv.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox ptrHerit(PtrPRule pRegle, PtrElement pEl, DocViewNumber nv)

#else /* __STDC__ */
static PtrAbstractBox ptrHerit(pRegle, pEl, nv)
	PtrPRule pRegle;
	PtrElement pEl;
	DocViewNumber nv;
#endif /* __STDC__ */

{
  PtrElement      pH;
  PtrAbstractBox         pP;
  
  pP = NULL;
  pH = pEl;
  if (pEl != NULL)
    switch (pRegle->PrInheritMode)
      {
      case InheritParent:
	HeritAsc(pEl, nv, &pP, &pH);
	break;
      case InheritGrandFather:
	HeritAsc(pEl, nv, &pP, &pH);
	if (pP != NULL)
	  {
	    pH = pP->AbElement;
	    pP = NULL;
	    HeritAsc(pH, nv, &pP, &pH);
	  }
	break;
      case InheritPrevious:
	while (pH->ElPrevious != NULL && pP == NULL)
	  {
	    pH = pH->ElPrevious;
	    pP = pH->ElAbstractBox[nv - 1];
	    SuivNonPres(&pP);	/* saute les paves de presentation */
	    if (pP != NULL)
	      if (pP->AbDead)
		pP = NULL;
	  }
	if (pP == NULL)
	  HeritAsc(pEl, nv, &pP, &pH);
	break;
      case InheritChild:
	while (!pH->ElTerminal && pH->ElFirstChild != NULL && pP == NULL)
	  {
	    pH = pH->ElFirstChild;
	    pP = pH->ElAbstractBox[nv - 1];
	    SuivNonPres(&pP);	/* saute les paves de presentation */
	    if (pP != NULL)
	      if (pP->AbDead)
		pP = NULL;
	  }
	break;
      case InheritCreator:
	pP = pEl->ElAbstractBox[nv - 1];
	if (pP != NULL)
	  {
	    SuivNonPres(&pP);	/* saute les paves de presentation */
	    if (pP != NULL)
	      if (pP->AbDead)
	        pP = NULL;
	  }
	break;
      }
  
  SuivNonPres(&pP);	/* saute les paves de presentation */
  return pP;
}


/* ---------------------------------------------------------------------- */
/* |	ptrHeritImm  rend le pointeur sur le pave correspondant a	| */
/* |		l'element qui sert de reference quand on applique la	| */
/* |		regle d'heritage pointe par pRegle a l'element pointe	| */
/* |		par pEl, dans la vue nv. On ne considere que l'element	| */
/* |		immediatement voisin (pere, frere, fils).		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox ptrHeritImm(PtrPRule pRegle, PtrElement pEl, DocViewNumber nv)

#else /* __STDC__ */
static PtrAbstractBox ptrHeritImm(pRegle, pEl, nv)
	PtrPRule pRegle;
	PtrElement pEl;
	DocViewNumber nv;
#endif /* __STDC__ */

{
  PtrAbstractBox         pP;
  
  pP = NULL;
  if (pEl != NULL)
    switch (pRegle->PrInheritMode)
      {
      case InheritParent:
  	if (pEl->ElParent != NULL)
    	  pP = pEl->ElParent->ElAbstractBox[nv - 1];
	break;
      case InheritGrandFather:
	if (pEl->ElParent != NULL)
	  if (pEl->ElParent->ElParent != NULL)
	    pP = pEl->ElParent->ElParent->ElAbstractBox[nv - 1];
	break;
      case InheritPrevious:
	if (pEl->ElPrevious != NULL)
	  pP = pEl->ElPrevious->ElAbstractBox[nv - 1];
	if (pP == NULL)
  	  if (pEl->ElParent != NULL)
    	    pP = pEl->ElParent->ElAbstractBox[nv - 1];
	break;
      case InheritChild:
	if (!pEl->ElTerminal)
	  if (pEl->ElFirstChild != NULL)
	    pP = pEl->ElFirstChild->ElAbstractBox[nv - 1];
	break;
      case InheritCreator:
	pP = pEl->ElAbstractBox[nv - 1];
	if (pP != NULL)
	  {
	    SuivNonPres(&pP);	/* saute les paves de presentation */
	    if (pP->AbDead)
	      pP = NULL;
	  }
	break;
      }
  
  SuivNonPres(&pP);
  if (pP != NULL)
    if (pP->AbDead)
      pP = NULL;
  return pP;
}



/* ---------------------------------------------------------------------- */
/* |	valcarregle evalue une regle de presentation de type caractere	| */
/* |		pour la vue nv. La regle a evaluer est pointee par	| */
/* |		pRegle, et l'element auquel elle s'applique est pointe	| */
/* |		par pEl. Au retour, ok indique si l'evaluation a pu	| */
/* |		etre faite. 						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static char valcarregle(PtrPRule pRegle, PtrElement pEl, DocViewNumber nv, boolean *ok)

#else /* __STDC__ */
static char valcarregle(pRegle, pEl, nv, ok)
	PtrPRule pRegle;
	PtrElement pEl;
	DocViewNumber nv;
	boolean *ok;
#endif /* __STDC__ */

{
  PtrAbstractBox         pP;
  char            val;
  PtrPRule    pRe1;
  PtrAbstractBox         pPa1;
  
  val = ' ';
  *ok = TRUE;
  if (pRegle != NULL && pEl != NULL)
    {
      pRe1 = pRegle;
      switch (pRe1->PrPresMode)
	{
  case PresInherit:
	  pP = ptrHerit(pRegle, pEl, nv);
	  if (pP == NULL)
	    *ok = FALSE;
	  else
	    {
	      pPa1 = pP;
	      switch (pRe1->PrType)
		{
	  case PtFont:
		  val = pPa1->AbFont;
		  break;
	  case PtStyle:
		    if (pPa1->AbHighlight == 1)
		      val = 'B';
		    else if (pPa1->AbHighlight == 2)
		      val = 'I';
		    else if (pPa1->AbHighlight == 3)  /* O : oblique */
                      val = 'O';
		    else if (pPa1->AbHighlight == 4)  /* G : gras italique */
                      val = 'G';
		    else if (pPa1->AbHighlight == 5)  /* Q : gras oblique */
                      val = 'Q';
		    else
		      val = 'R';
		  break;
	  case PtUnderline:
		    if (pPa1->AbUnderline == 1)
		      val = 'U';
		    else if (pPa1->AbUnderline == 2)
		      val = 'O';
		    else if (pPa1->AbUnderline == 3)
		      val = 'C';		
		    else
		      val = 'N';
		  break;
		  
	  case PtThickness:
		    if (pPa1->AbThickness == 1)
		      val = 'T';
		    else
		      val = 'N';
		  break;
	  case PtLineStyle:
		     val = pPa1->AbLineStyle;
		  break;
	  default:
		  break;
		}
	      
	    }
	  break;
  case PresFunction:
	  
	  break;
  case PresImmediate:
	  val = pRe1->PrChrValue;
	  break;
	}
    }
  return val;
}


/* ---------------------------------------------------------------------- */
/* |	valcadregle evalue une regle d'ajustement pour la vue nv.	| */
/* |		La regle a evaluer est pointee par pRegle, et l'element	| */
/* |		auquel elle s'applique est pointe par pEl.		| */
/* |		Au retour, ok indique si l'evaluation a pu etre faite.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static BAlignment valcadregle(PtrPRule pRegle, PtrElement pEl, DocViewNumber nv, boolean *ok)

#else /* __STDC__ */
static BAlignment valcadregle(pRegle, pEl, nv, ok)
	PtrPRule pRegle;
	PtrElement pEl;
	DocViewNumber nv;
	boolean *ok;
#endif /* __STDC__ */

{
  PtrAbstractBox         pP;
  BAlignment         val;
  PtrPRule    pRe1;
  
  val = AlignLeft;
  *ok = TRUE;
  if (pRegle != NULL && pEl != NULL)
    {
      pRe1 = pRegle;
      switch (pRe1->PrPresMode)
	{
  case PresInherit:
	  pP = ptrHerit(pRegle, pEl, nv);
	  if (pP == NULL)
	    *ok = FALSE;
	  else
	    if (pRe1->PrType == PtAdjust)
	      val = pP->AbAdjust;
	  break;
  case PresFunction:
	  break;
  case PresImmediate:
	  if (pRe1->PrType == PtAdjust)
	    val = pRe1->PrAdjust;
	  break;
	}
      
    }
  return val;
}



/* ---------------------------------------------------------------------- */
/* |	 valboolregle evalue une regle de presentation de type booleen	| */
/* |		pour la vue nv. La regle a evaluer est pointee par	| */
/* |		pRegle, et l'element auquel elle s'applique est pointe	| */
/* |		par pEl.						| */
/* |		Au retour, ok indique si l'evaluation a pu etre faite.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean valboolregle(PtrPRule pRegle, PtrElement pEl, DocViewNumber nv, boolean *ok)

#else /* __STDC__ */
static boolean valboolregle(pRegle, pEl, nv, ok)
	PtrPRule pRegle;
	PtrElement pEl;
	DocViewNumber nv;
	boolean *ok;
#endif /* __STDC__ */

{
  PtrAbstractBox         pP;
  boolean         val;
  PtrPRule    pRe1;
  
  val = FALSE;
  *ok = TRUE;
  if (pRegle != NULL && pEl != NULL)
    {
      pRe1 = pRegle;
      switch (pRe1->PrPresMode)
	{
  case PresInherit:
	  pP = ptrHerit(pRegle, pEl, nv);
	  if (pP == NULL)
	    *ok = FALSE;
	  else if (pRe1->PrType == PtJustify)
	    val = pP->AbJustify;
	  else if (pRe1->PrType == PtHyphenate)
	    val = pP->AbHyphenate;
	  break;
  case PresFunction:
	  break;
  case PresImmediate:
	  if (pRe1->PrType == PtJustify || pRe1->PrType == PtHyphenate)
	    val = pRe1->PrJustify;
	  break;
	}
      
    }
  return val;
}


/* ---------------------------------------------------------------------- */
/* |	valintregle evalue une regle de presentation de type entier pour| */
/* |		la vue nv. La regle a evaluer est pointee par pRegle,	| */
/* |		et l'element auquel elle s'applique est pointe par pEl.	| */
/* |		Au retour, ok indique si l'evaluation a pu etre faite et| */
/* |		unit indique, dans le cas de regle PtIndent, PtBreak1,	| */
/* |		PtBreak2 ou PtSize, si la valeur est exprimee en	| */
/* |		points typo, en relatif (numero de corps si PtSize),	| */
/* |		etc. Si la regle est une regle de presentation		| */
/* |		d'attribut, pAttr pointe sur le bloc d'attribut auquel	| */
/* |		la regle correspond.					| */
/* |		Fonction utilisee dans crimabs				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int valintregle(PtrPRule pRegle, PtrElement pEl, DocViewNumber nv, boolean *ok, TypeUnit *unit, PtrAttribute pAttr)
#else /* __STDC__ */
int valintregle(pRegle, pEl, nv, ok, unit, pAttr)
	PtrPRule pRegle;
	PtrElement pEl;
	DocViewNumber nv;
	boolean *ok;
	TypeUnit *unit;
	PtrAttribute pAttr;
#endif /* __STDC__ */
{
  PtrAbstractBox         pP;
  int             val, i;
  PtrPRule    pRe1;
  PtrElement      pH;

  val = 0;
  *ok = TRUE;
  *unit = UnRelative;
  if (pRegle != NULL && pEl != NULL)
    {
      pRe1 = pRegle;
      switch (pRe1->PrPresMode)
	{
	case PresInherit:
	  if (pRe1->PrType == PtVisibility)
	    pP = ptrHeritImm(pRegle, pEl, nv);
	  else
	    pP = ptrHerit(pRegle, pEl, nv);
	  if (pP == NULL)
	    *ok = FALSE;
	  else
	    {
	      if (pRe1->PrInhAttr)
		{	
		  /* c'est la valeur d'un attribut */ 
		  if (pRe1->PrInhDelta < 0)
		    /* il faut retrancher cette valeur */
		    i = -AttrValue(pAttr);
		  else	
		    /* il faut ajouter cette valeur */
		    i = AttrValue(pAttr);
		  if (pRe1->PrInhUnit == UnRelative || pRe1->PrInhUnit == UnXHeight)
		    if (pRe1->PrType == PtIndent
			|| pRe1->PrType == PtLineSpacing
			|| pRe1->PrType == PtLineWeight)
		      /* convertit en 1/10 de caractere */
		      i = 10 * i;
		}
	      else	
		/* c'est la valeur elle meme qui est dans la regle */
		i = pRe1->PrInhDelta;

	      switch (pRe1->PrType)
		{
		case PtVisibility:
		  val = pP->AbVisibility + i;
		  break;
		case PtSize:
		  val = pP->AbSize + i;
		  *unit = pP->AbSizeUnit;
		  if (*unit == UnRelative)
		    if (val > MAX_LOG_SIZE)
		      val = MAX_LOG_SIZE;
		    else if (val < 0)
		      val = 0;
		  break;
		case PtIndent:
		  val = pP->AbIndent + i;
		  *unit = pP->AbIndentUnit;
		  break;
		case PtLineSpacing:
		  val = pP->AbLineSpacing + i;
		  *unit = pP->AbLineSpacingUnit;
		  break;
		case PtDepth:
		  val = pP->AbDepth + i;
		  break;
		case PtFillPattern:
		  val = pP->AbFillPattern;
		  break;
		case PtBackground:
		  val = pP->AbBackground;
		  break;
		case PtForeground:
		  val = pP->AbForeground;
		  break;
		case PtLineWeight:
		  val = pP->AbLineWeight + i;
		  if (val < 0)
		    val = 0;
		  *unit = pP->AbLineWeightUnit;
		  break;
		default:
		  break;
		}
	      
	      if (pRe1->PrInhMinOrMax != 0)
		/* il y a un minimum ou un maximum a respecter */
		{
		  if (pRe1->PrMinMaxAttr)	
		    /* c'est la valeur d'un attribut */ 
		    {
		      if (pRe1->PrInhMinOrMax < 0)
			/* inverser cette valeur */
			i = -AttrValue(pAttr);
		      else
			i = AttrValue(pAttr);
		      if (pRe1->PrInhUnit == UnRelative ||
			  pRe1->PrInhUnit == UnXHeight)
			if (pRe1->PrType == PtIndent || pRe1->PrType == PtLineSpacing ||
			    pRe1->PrType == PtLineWeight)
			  /* convertit en 1/10 de caractere */
			  i = 10 * i;
		    }
		  else	
		    /* c'est la valeur elle meme qui est dans la regle */
		    i = pRe1->PrInhMinOrMax;
		  if (pRe1->PrInhDelta >= 0)	
		    /* c'est un maximum */
		    /* dans les paves, les tailles relatives sont */
		    /* exprimees dans une echelle de valeurs entre 0 et */
		    /* n-1, alors que dans les regles de presentation */
		    /* l'echelle est entre 1 et n. */
		    if (pRe1->PrType == PtSize && pP->AbSizeUnit == UnRelative)
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
		    if (pRe1->PrType == PtSize && pP->AbSizeUnit == UnRelative)
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
	  if (pRe1->PrType == PtVisibility
	      || pRe1->PrType == PtDepth
	      || pRe1->PrType == PtFillPattern
	      || pRe1->PrType == PtBackground
	      || pRe1->PrType == PtForeground)
	    if (pRe1->PrAttrValue)
	      /* c'est la valeur d'un attribut */ 
	      if (pRe1->PrIntValue < 0)
		/* il faut inverser cette valeur */
		val = -AttrValue(pAttr);
	      else
		val = AttrValue(pAttr);
	    else	
	      /* c'est la valeur elle meme qui est dans la regle */
	      val = pRe1->PrIntValue;
	  
	  else if (pRe1->PrType == PtBreak1
		   || pRe1->PrType == PtBreak2
		   || pRe1->PrType == PtIndent
		   || pRe1->PrType == PtSize
		   || pRe1->PrType == PtLineSpacing
		   || pRe1->PrType == PtLineWeight)
	    {
	      if (pRe1->PrMinAttr)	
		/* c'est la valeur d'un attribut */
		{
		  if (pRe1->PrMinValue < 0)
		    /* il faut inverser cette valeur */
		    val = -AttrValue(pAttr);
		  else
		    val = AttrValue(pAttr);
		  if (pRe1->PrMinUnit == UnRelative
		      || pRe1->PrMinUnit == UnXHeight)
		    if (pRe1->PrType != PtSize)
		      /* convertit en 1/10 de caractere */
		      val = val * 10;
		}
	      else
		/* c'est la valeur elle-meme qui est dans la regle */
		val = pRe1->PrMinValue;
	      if (pRe1->PrMinUnit == UnPercent && pRe1->PrType != PtIndent)
		{
		  if( pRe1->PrType == PtSize)
		    {
		      HeritAsc(pEl, nv, &pP, &pH);
		      if (pP == NULL)
			*ok = FALSE;
		      else
			{
			  val = pP->AbSize + i;
			  *unit = pP->AbSizeUnit;
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
		  *unit = pRe1->PrMinUnit;
		  if (pRe1->PrType == PtSize && pRe1->PrMinUnit == UnRelative)
		    val--;
		}
	    }

	  if (pRe1->PrType == PtSize && *unit == UnRelative)
	    {
	      if (val > MAX_LOG_SIZE)
		val = MAX_LOG_SIZE;
	      else if (val < 0)
		val = 0;
	    }
	  else if (pRe1->PrType == PtLineWeight && val < 0)
	    val = 0;
	  break;
	default:
	  break;
	}
    }
  return val;
}

/* ---------------------------------------------------------------------- */
/* |	testpave : Teste si le pave pAb a les caracteristiques		| */
/* |		NumType (type du pave) et PoRefElem (pave de presentation	| */
/* |		ou d'element) si NotType est faux ou s'il n'a pas les	| */
/* |		caracteristiques NumType et PoRefElem si NotType est vrai.| */
/* |		 Cela permet de determiner le pave pAb par rapport	| */
/* |		 auquel le pave en cours de traitement va se positionner| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void testpave(boolean *found, PtrPSchema pSP, boolean PoRefElem, int NumType, boolean NotType, PtrAbstractBox pAb)

#else /* __STDC__ */
static void testpave(found, pSP, PoRefElem, NumType, NotType, pAb)
	boolean *found;
	PtrPSchema pSP;
	boolean PoRefElem;
	int NumType;
	boolean NotType;
	PtrAbstractBox pAb;
#endif /* __STDC__ */

{
  PtrAbstractBox         pPa1;
  
  pPa1 = pAb;
  if (!pPa1->AbDead)
    if (NotType) /* on accepte le pave s'il est de type different de NumType */
      {
	if (PoRefElem)  /* un pave d'un element de structure */
	  {
	    if ((NumType == MAX_PRES_VARIABLE + 1) && (pPa1->AbPresentationBox))
	      /* Cas d'une regle Not AnyElem, on accepte la premiere boite */
	      /* de presentation trouvee */
	      *found = TRUE;
	  }
	else  /* un pave d'une boite de pres. */
	  if ((NumType == MAX_PRES_BOX + 1) && (!pPa1->AbPresentationBox))
	    /* Cas d'une regle Not AnyBox, on accepte le premier element */
	    /* trouve' */
	    *found = TRUE;
	/* on est dans une regle differente de Any... */ 
	if (pPa1->AbTypeNum != NumType 
	    || pPa1->AbPresentationBox == PoRefElem 
	    || pPa1->AbPSchema != pSP)
	  *found = TRUE;
      }
    else 
      {
	if (PoRefElem) /* un pave d'un element de structure */
	  {
	    if ((NumType == MAX_PRES_VARIABLE + 1) && (!pPa1->AbPresentationBox))
	    /* Cas d'une regle AnyElem, on accepte le premier element trouve */
	      *found = TRUE;
	  }
	else  /* un pave d'une boite de pres. */
	  if ((NumType == MAX_PRES_BOX + 1) && (pPa1->AbPresentationBox))
	    /* Cas d'une regle AnyBox, on accepte la premiere boite de */
	    /* presentation trouvee */
	    *found = TRUE;
	/* on est dans une regle differente de Any... */ 
	if (pPa1->AbTypeNum == NumType 
	    && pPa1->AbPresentationBox != PoRefElem 
	    && (pPa1->AbPSchema == pSP 
		|| pSP == NULL))
	  *found = TRUE;
      }
}

/* ---------------------------------------------------------------------- */
/* |	AttributCreeBoite	retourne Vrai si l'une des regles de	| */
/* |		presentation de l'attribut pointe' par pAttr cree le	| */
/* |		pave de presentation pointe' par pAb.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean AttributCreeBoite(PtrAttribute pAttr, PtrAbstractBox pAb)

#else /* __STDC__ */
static boolean AttributCreeBoite(pAttr, pAb)
	PtrAttribute pAttr;
	PtrAbstractBox pAb;
#endif /* __STDC__ */

{
    boolean	     ret, stop;
    PtrPRule     pRegle;
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
	  pRegle = ReglePresAttr(pAttr, pAb->AbElement, FALSE, NULL, pSchP);
	  /* saute les regles precedant les  fonctions */
	  stop = FALSE;
	  do
	    if (pRegle == NULL)
	      stop = TRUE;
	    else if (pRegle->PrType > PtFunction)
	      {
	      /* pas de fonction de presentation */
	      stop = TRUE;
	      pRegle = NULL;
	      }
	    else if (pRegle->PrType == PtFunction)
	      stop = TRUE;
	    else
	      pRegle = pRegle->PrNextPRule;
	  while (!stop);
	  /* cherche toutes les fonctions de creation */
	  stop = FALSE;
	  do
	    if (pRegle == NULL)
	      /* fin de la chaine */
	      stop = TRUE;
	    else if (pRegle->PrType != PtFunction)
	      /* fin des fonctions */
	      stop = TRUE;
	    else
	      /* c'est une regle fonction */
	      {
	      if (pRegle->PrPresFunction == FnCreateBefore ||
		  pRegle->PrPresFunction == FnCreateWith ||
		  pRegle->PrPresFunction == FnCreateEnclosing ||
	          pRegle->PrPresFunction == FnCreateFirst ||
	          pRegle->PrPresFunction == FnCreateAfter ||
	          pRegle->PrPresFunction == FnCreateLast)
	        /* c'est une regle de creation */
	        {
	        if (!pRegle->PrElement &&
		    pRegle->PrPresBox[0] == pAb->AbTypeNum &&
		    pRegle->PrNPresBoxes == 1 &&
		    pSchP == pAb->AbPSchema)
		  /* cette regle cree notre pave, on a trouve' */
		  {
		  stop = TRUE;
		  ret = TRUE;
		  }
	        }
	      if (!stop)
	        /* passe a la regle suivante de la chaine */
	        pRegle = pRegle->PrNextPRule;
	      }
	  while (!stop);
	  if (!ret)
	     /* on n'a pas encore trouve'. On cherche dans les schemas de */
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
/* |	testDescend : Teste si le pave pAb ou un de ses		| */
/* |		descendants a les caracteristiques			| */
/* |		NumType (type du pave) et PoRefElem (pave de presentation	| */
/* |		ou d'element) si NotType est faux ou s'il n'a pas les	| */
/* |		caracteristiques NumType et PoRefElem si NotType est vrai.| */
/* |		Cela permet de determiner le pave pAb par rapport	| */
/* |		auquel le pave en cours de traitement va se positionner	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void testDescend(boolean *found, PtrPSchema pSP, boolean PoRefElem, int NumType, boolean NotType, PtrAbstractBox pAb)

#else /* __STDC__ */
static void testDescend(found, pSP, PoRefElem, NumType, NotType, pAb)
	boolean *found;
	PtrPSchema pSP;
	boolean PoRefElem;
	int NumType;
	boolean NotType;
	PtrAbstractBox pAb;
#endif /* __STDC__ */

{
  testpave(found, pSP, PoRefElem, NumType, NotType, pAb);
  if (!(*found))
    if (pAb->AbFirstEnclosed != NULL)
      {
      pAb = pAb->AbFirstEnclosed;
      do
	{
	testDescend(found, pSP, PoRefElem, NumType, NotType, pAb);
	if (!(*found))
	  pAb = pAb->AbNext;
	}      
      while (!(*found) && pAb != NULL);
      }
}


/* ---------------------------------------------------------------------- */
/* |	PaveRef Si NotType est faux, rend un pointeur sur le pave de	| */
/* |		type NumType et de niveau Niv (relativement au pave	| */
/* |		pP). Si NotType est vrai, rend un pointeur sur le	| */
/* |		premier pave de niveau Niv (relativement a pP) qui n'est| */
/* |		pas de type NumType.					| */
/* |		Si PoRefElem est vrai, le pave represente par NumType est	| */
/* |		celui d'un element de la representation interne, sinon	| */
/* |		c'est une boite de presentation definie dans le schema	| */
/* |		de presentation pointe' par pSP.			| */
/* |		Si Niv vaut RlReferred, on cherche parmi les paves de	| */
/* |		l'element designe' par l'attribut reference pointe'	| */
/* |		par pAttr.						| */
/* |		Au retour, la fonction rend NULL si aucun pave ne	| */
/* |		correspond.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox PaveRef(boolean NotType, int NumType, PtrPSchema pSP, Level Niv, boolean PoRefElem, PtrAbstractBox pP, PtrAttribute pAttr)

#else /* __STDC__ */
static PtrAbstractBox PaveRef(NotType, NumType, pSP, Niv, PoRefElem, pP, pAttr)
	boolean NotType;
	int NumType;
	PtrPSchema pSP;
	Level Niv;
	boolean PoRefElem;
	PtrAbstractBox pP;
	PtrAttribute pAttr;
#endif /* __STDC__ */

{
  boolean         found;
  PtrAbstractBox         pAb;
  int             vue;
  PtrAbstractBox	  pPavPrincipal;
  
  pAb = pP;
  if (pAb != NULL)
    {
      found = FALSE;
      switch (Niv)
	{
	case RlEnclosing:
	  do
	    {
	      pAb = pAb->AbEnclosing;
	      if (NumType == 0)
		found = TRUE;
	      else if (pAb != NULL)
		testpave(&found, pSP, PoRefElem, NumType, NotType, pAb);
	    }
	  while (!(pAb == NULL || found));
	  break;

	case RlSameLevel:
	  /* accede au premier pave de ce niveau */
	  if (pAb->AbEnclosing != NULL)
	     pAb = pAb->AbEnclosing->AbFirstEnclosed;
	  /* cherche en avant le pave demande' */
	  do
	    if (NumType == 0)
	      if (pAb->AbDead)
		pAb = pAb->AbNext;
	      else
		found = TRUE;
	    else
	      {
		testpave(&found, pSP, PoRefElem, NumType, NotType, pAb);
		if (!found)
		  pAb = pAb->AbNext;
	      }
	  while (!(pAb == NULL || found));
	  break;

	case RlEnclosed:
	  pAb = pAb->AbFirstEnclosed;
	  if (pAb != NULL)
	    do
	      if (NumType == 0)
		if (pAb->AbDead)
		  pAb = pAb->AbNext;
		else
		  found = TRUE;
	      else
		{
		  testpave(&found, pSP, PoRefElem, NumType, NotType, pAb);
		  if (!found)
		    pAb = pAb->AbNext;
		}
	    while (!(pAb == NULL || found));
	  break;

	case RlPrevious:
	  pAb = pAb->AbPrevious;
	  if (pAb != NULL)
	    do
	      if (NumType == 0)
		if (pAb->AbDead)
		  pAb = pAb->AbPrevious;
		else
		  found = TRUE;
	      else
		{
		  testpave(&found, pSP, PoRefElem, NumType, NotType, pAb);
		  if (!found)
		    pAb = pAb->AbPrevious;
		}
	  while (!(pAb == NULL || found));
	  break;

	case RlNext:
	  pAb = pAb->AbNext;
	  if (pAb != NULL)
	    do
	      if (NumType == 0)
		if (pAb->AbDead)
		  pAb = pAb->AbNext;
		else
		  found = TRUE;
	      else
		{
		  testpave(&found, pSP, PoRefElem, NumType, NotType, pAb);
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
	        if (pAb->AbElement == pP->AbElement)
	  	  found = TRUE;
	        else
	  	  pAb = NULL;
	    while ((!found) && pAb != NULL);
	    if (!found)
	      /* pas trouve', on cherche parmi les freres precedents */
	      {
	      pAb = pP;
	      do
	        if (pAb->AbPresentationBox)
	          pAb = pAb->AbPrevious;
	        else
	          if (pAb->AbElement == pP->AbElement)
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
		if (AttributCreeBoite(pAttr, pP))
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
	  pPavPrincipal = NULL;
	  if (pAb != NULL)
	    /* cherche en avant le pave demande */ 
	    do
	      if (NumType == 0)
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
		      /* on n'a pas trouve' de pave pour cet element */
		      pAb = NULL;
		else
		  found = TRUE;
	      else
		{
		  testpave(&found, pSP, PoRefElem, NumType, NotType, pAb);
		  if (!found)
		    {
		    if (!pAb->AbPresentationBox)
		      pPavPrincipal = pAb;
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
	  if (pAb == NULL && pPavPrincipal != NULL && NumType != 0)
	     /* on cherche parmi les paves descendants du pave principal */
	     {
	     pAb = pPavPrincipal->AbFirstEnclosed;
	     while (pAb != NULL && !found)
		{
		testDescend(&found, pSP, PoRefElem, NumType, NotType, pAb);
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

  
  if (!VueAssoc(pAb->AbElement)) 
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
static void Contenu(PtrElement pEl, PtrAbstractBox pAb, PtrDocument pDoc)

#else /* __STDC__ */
static void Contenu(pEl, pAb, pDoc)
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
    CopyStringToText(pEl->ElSructSchema->SsRule[pEl->ElTypeNumber-1].SrName,
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
	  if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrFirstOfPair)
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
/* |	RegleCreation	cherche dans la chaine de regles de presentation| */
/* |	qui commence par pRe et qui appartient au schema de		| */
/* |	presentation pointe' par pSPR, la regle de creation qui		| */
/* |	engendre le pave pCree.						| */
/* |	Si cette regle est trouvee, retourne TRUE et TypeCreation	| */
/* |	contient le type de cette regle,				| */
/* |	Sinon, retourne FALSE.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean RegleCreation(PtrPRule pRe, PtrPSchema pSPR, PtrAbstractBox pCree, FunctionType *TypeCreation)
#else /* __STDC__ */
static boolean RegleCreation(pRe, pSPR, pCree, TypeCreation)
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
  if (!RegleCreation(pRCre, pSPR, pCree, &Ret))
    /* on n'a pas trouve la regle qui cree la bonne boite */
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
	pRCre = ReglePresAttr(pA, pCreateur->AbElement, FALSE, NULL, pSchP);
	ok = RegleCreation(pRCre, pSchP, pCree, &Ret);
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

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |	Chaine	chaine et remplit le pave pointe par pAb, correspondant| */
/* |		a l'element pointe par pEl dans la vue nv du document	| */
/* |		dont le contexte est pointe par pDoc.			| */
/* |    EnAvant dit si la creation se fait en avant ou non		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void Chaine(PtrAbstractBox pAb, PtrElement pEl, DocViewNumber nv, int VueSch, PtrDocument pDoc, boolean EnAvant)
#else /* __STDC__ */
void Chaine(pAb, pEl, nv, VueSch, pDoc, EnAvant)
	PtrAbstractBox pAb;
	PtrElement pEl;
	DocViewNumber nv;
	int VueSch;
	PtrDocument pDoc;
	boolean EnAvant;
#endif /* __STDC__ */
{
  PtrElement      pE;
  PtrAbstractBox         pP;
  boolean         stop;
  PtrAbstractBox         pPa1;
  PtrElement      pEl1;
  PtrAbstractBox  pPage, pPEnglobant;
  boolean trouve, peretrouve;

  pPage = NULL;
  pPa1 = pAb;
  pE = pEl;
  pP = NULL;
  peretrouve = FALSE;
  if (EnAvant)
    /* cas simples de chainage si on insere un pave au milieu de l'i.a.  */
    {
    /* si l'element a un frere precedent (non MP) qui a un pave */
    /* et qui ne contient pas d'element MP de cette vue, */
    /* alors le nouveau pave a pour pere le pere du dernier dup precedent */
      if (pEl->ElPrevious != NULL
          && !((pEl->ElPrevious->ElTypeNumber == PageBreak + 1)
                && pEl->ElPrevious->ElViewPSchema == VueSch))
        {
          pE = pEl->ElPrevious;
          /* on cherche si pE contient une MP de la vue */
          /* pour cela on recherche le pave marque page precedent et */
          /* on regarde si l'element correspondant a pour ascendant pE */
          pP = RechPavPage(pE, nv, VueSch, FALSE);
          if (pP != NULL) /* document pagine */
            {
              pEl1 = pP->AbElement;
              trouve = FALSE;
              while (!trouve && pEl1 != NULL)
                if (pEl1->ElParent == pE)
                  trouve = TRUE;
                else
                  pEl1 = pEl1->ElParent;
              if (!trouve) /* si la derniere MP n'est pas fils de pE */
                           /* on peut chainer pAb au pave pere de pP */
                {
                  pP = pE->ElAbstractBox[nv - 1];
                  if (pP != NULL)
                    {
                      while (pP->AbPresentationBox
                             && pP->AbElement == pE && pP->AbNext !=NULL)
                        pP = pP->AbNext;
                      /* on va a la fin de la chaine des dupliques */
                      /* de l'element precedent */
                      while (pP->AbNextRepeated != NULL)
                        pP = pP->AbNextRepeated;
                      /* on prend le pere de ce pave : ce sera le pere de */
                      /* de pAb */
                      pP = pP->AbEnclosing;
                      peretrouve = TRUE;
                    }
                  
                  /* dans tous les autres cas, on revient au cas general */
                }
            }
        }
      else
    /* sinon, si l'element a un frere suivant (non MP) qui a un pave, */
    /* le pere du pave sera le premier pave du pere du frere */
    /* de l'element suivant */
        if (pEl->ElNext != NULL
            && !((pEl->ElNext->ElTypeNumber == PageBreak + 1)
                 && pEl->ElNext->ElViewPSchema == VueSch))
          {
            pE = pEl->ElNext;
            pP = pE->ElAbstractBox[nv - 1];
            if (pP != NULL)
              {
              /* on prend le pere de ce pave : ce sera le pere de pAb */
                pP = pP->AbEnclosing;
                peretrouve = TRUE;
              }
          }
        
       /* dans les autres cas (!peretrouve), c'est la panique si */
       /* l'insersion se fait au milieu de l'i.a. ! */
       /* il faut recreer l'image abstraite des pages concernees */
       /* pour l'instant on applique le code general */
    }
  if (!peretrouve)
    {
    /* cherche dans cette vue le premier pave d'un element ascendant */
    pP = NULL;
    pE = pEl;
    while (pE->ElParent != NULL && pP == NULL)
      {
      pE = pE->ElParent;
      pP = pE->ElAbstractBox[nv - 1];
      }
    }
  if (pP == NULL)
    /* il n'y a pas de pave englobant pAb */ 
    {
      if (pEl->ElAbstractBox[nv - 1] == NULL)
        pEl->ElAbstractBox[nv - 1] = pAb;
    }
  else			
    /* pAb n'est pas le pave le plus englobant pour cette vue */
    /*  pP: 1er pave de l'element ascendant pour cette vue */
    {
     if (!peretrouve)
     {
       /* le premier pave qui n'est pas de presentation est l'englobant */
       /* sauf si le pere est la racine : dans ce cas, l'englobant est */
       /* le pave de la marque de page precedente */
       if ((!VueAssoc(pEl) && (pE == pDoc->DocRootElement))
            || (VueAssoc(pEl)
            && (pE == pDoc->DocAssocRoot[pEl->ElAssocNum - 1])))
         /* cas ou le pere est la racine */
         /* recherche du pave page ou le pave pAb doit etre inclus */
         pPage = RechPavPage(pEl, nv, VueSch, TRUE);
       if (pPage == NULL) /* le pere n'est pas la racine */
         /* ou le document est sans pagination */
         {
           /* saute les paves de presentation que l'ascendant a crees par la */
           /* regle CreateBefore */
           while (pP->AbPresentationBox && pP->AbElement == pE &&
                  pP->AbNext != NULL)
             pP = pP->AbNext;	
           if (EnAvant)
             while (pP->AbNextRepeated!=NULL) /* on se positionne sur le */
               /* dernier pave duplique */
               /* cas en avant */
               pP = pP->AbNextRepeated;
           /* cas ou l'on fonctionne en arriere : on est sur le bon pave */
         }
       else
        {
          /* il faut descendre la hierarchie des pages et colonnes */
          /* pPage est le pave corps de page de plus haut niveau */
          trouve = FALSE;
          while (!trouve)
            {
              if (pPage->AbFirstEnclosed != NULL
                  && pPage->AbFirstEnclosed->AbElement->ElTypeNumber == 
                              PageBreak+1)
                pPage = pPage->AbFirstEnclosed; 
              else
                trouve = TRUE;
              if (!trouve)
                while (pPage->AbPresentationBox)
                  pPage = pPage->AbNext;
	      /* si EnAvant on se place sur la derniere colonne */
	      if (!trouve && EnAvant) /* TODO et cas !EnAvant ??? */
                {
                  while (pPage->AbNext != NULL)
                    pPage = pPage->AbNext;
                  while (pPage->AbPresentationBox)
                    pPage = pPage->AbPrevious;
  	        } 
            }
           pP = pPage;
        }
      } /* fin du cas ou !peretrouve */
      pPa1->AbEnclosing = pP;
      if (pPa1->AbEnclosing->AbFirstEnclosed == NULL)
	/* c'est le premier pave englobe */
	{
	  pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	  if (pEl->ElAbstractBox[nv - 1] == NULL)
	    pEl->ElAbstractBox[nv - 1] = pAb;
	  /* 1er pave de l'element */
	}
      else
	/* il y a deja des paves englobes */
	/* cas retire : les paves de corps, haut, bas et filet */
	/* ne sont pas chaines avec la procedure Chaine */
	/* test sur marque de page supprime */
	  {
	    if (pEl->ElAbstractBox[nv - 1] != NULL)
	      /* cet element a deja des paves dans cette vue */
	      /* saute les paves de l'element */
	      /* en tenant compte des eventuels paves dupliques de l'element */
	      {
		pP = pEl->ElAbstractBox[nv - 1];
		stop = FALSE;
		do
	          if (pP->AbNext == NULL && pP->AbNextRepeated == NULL)
                    stop = TRUE;
	          else if (pP->AbNext != NULL)
	            if (pP->AbNext->AbElement != pEl)
                      stop = TRUE;
                    else
                      pP = pP->AbNext;
	          else /* pP->AbNext == NULL et pP->AbNextRepeated != NULL*/
	             pP = pP->AbNextRepeated;
		while (!(stop));
		/* insere le nouveau pave apres */
		pPa1->AbPrevious = pP;
		pPa1->AbNext = pP->AbNext;
	      } 
	    else	
	      /* cet element n'a pas encore de paves dans cette vue */
	      {
		pEl->ElAbstractBox[nv - 1] = pAb;	
		/* 1er pave de l'element */
		/* cherche l'element precedent ayant un pave dans la vue */
	        /* et qui soit dans le sous-arbre defini par l'element */
	        /* du pave pere */
	        /* on saute des les elements marque de page */
	        /* dans le cas ou le pave pere est un pave corps de page, */
	        /* on ne trouve jamais d'element precedent (ok) */
	        pE = pEl;
	        do
	          pE = BackSearchVisibleElem(pPa1->AbEnclosing->AbElement, pE, nv);
	        while ((pE != NULL) && (pE->ElTypeNumber == PageBreak + 1));
                /* verifie si le pave trouve pour un element precedent */
                /* est bien inclus dans le meme pave englobant. */
	        /* il faut parcourir la suite des paves de l'element */
	        /* precedent (il peut avoir plusieurs paves dup) */
                /* Par exemple deux notes de bas de page successives */
                /* peuvent avoir leurs paves dans des boites de bas de */
                /* page differentes */ 
	        /* ce dernier cas n'est plus un cas particulier car les */
	        /* paves relatifs a l'element Notes sont chaines avec dup */
 	        if (pE != NULL)
                  {
                    /* pave de l'element precedent */
                    pP = pE->ElAbstractBox[nv - 1];
	            /* on saute les paves de presentation */
	            while (pP->AbPresentationBox)
	               pP = pP->AbNext;
	            trouve = FALSE;
	            while (!trouve && pP != NULL)
	              {
	              /* boucle de recherche dans les ascendants */
	              /* pourquoi ne pas rester au niveau du pere ? */
	              pPEnglobant = pP->AbEnclosing;
	              while (pPEnglobant != pPa1->AbEnclosing &&
	                     pPEnglobant != NULL)
	                pPEnglobant = pPEnglobant->AbEnclosing;
	              /*  si pPEnglobant=NULL, pas trouve, on passe au pave */
	              /* duplique suivant de l'element precedent */
	              if (pPEnglobant == NULL)
	                pP = pP->AbNextRepeated;
 	              else
	                trouve = TRUE;
	              }
	            if (!trouve)
		      /* ils n'ont pas le meme pave englobant, on ne */
		      /* chainera pas le pave au pave de l'element precedent */
		      pE = NULL;
		  }
	        /* cas particulier ou pE = PageBreak Debut supprime */
		if (pE == NULL)
	          /* pas de pave d'element (hors marque de page) precedent */
                  {
                  pP = pPa1->AbEnclosing->AbFirstEnclosed;
                  if (pP != NULL)
	            /* on saute les paves de presentation de l'englobant (ou */
	            /* de la racine si l'englobant est une marque de page ) */
	            if ((pP->AbElement == pPa1->AbEnclosing->AbElement) ||
                        ((pPa1->AbEnclosing->AbElement->ElTypeNumber
                          == PageBreak + 1)
                         && (pP->AbElement
                          == pPa1->AbEnclosing->AbEnclosing->AbElement)))
		      if (RegleCree(pDoc, pPa1->AbEnclosing, pP) == FnCreateLast)
			/* le pave existant doit etre le dernier, on insere */
			/* le nouveau pave devant lui */
			{
			pPa1->AbNext = pP;
			pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			} 
		      else	
			/* on saute les paves crees par une regle Create de */
                        /* l'englobant (ou de la racine si marque de page) */
			{
			stop = FALSE;
			do
			  if (pP->AbNext == NULL)
			    stop = TRUE;
                          else
			    if (pPa1->AbEnclosing->AbElement->ElTypeNumber ==
				PageBreak + 1)
                              if (pP->AbNext->AbElement !=
                                  pPa1->AbEnclosing->AbEnclosing->AbElement)
                                stop = TRUE;
                              else
                                pP = pP->AbNext;
                            else
			      if (pP->AbNext->AbElement !=
                                  pPa1->AbEnclosing->AbElement)
			        stop = TRUE;
			      else
				if (RegleCree(pDoc, pPa1->AbEnclosing, pP->AbNext) == 
				    FnCreateLast)
				  /* le pave suivant doit etre le dernier */
				  stop = TRUE;
			        else
				  pP = pP->AbNext;
			while (!(stop));
			/* on insere le nouveau pave apres */
			pPa1->AbPrevious = pP;
			pPa1->AbNext = pP->AbNext;
			}
		      else	
			/* insere le nouveau pave en tete */
			{
			pPa1->AbNext = pP;
			pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			}
		  } 
		else	
		  /* il y a un pave d'un element precedent */
	          /* qui a le meme englobant */
		  /* on cherche le dernier pave de l'element precedent */
		  {
	          /* pP est le premier pave du precedent qui a le meme pere */
	          /* on insere le nouveau pave apres le dernier pave de pP */
	          /* tout en restant dans le meme sous arbre (meme pere) */
		  /*	    pP = pE->ElAbstractBox[nv - 1]; */
		    stop = FALSE;
		    do
		      if (pP->AbNext == NULL)
			stop = TRUE;
		      else if (pP->AbNext->AbElement != pE)
			stop = TRUE;
		      else
			pP = pP->AbNext;
		    while (!(stop));
		    /* insere le nouveau pave apres pP */
		    pPa1->AbPrevious = pP;
		    pPa1->AbNext = pP->AbNext;
		  }
	      }
	    if (pPa1->AbPrevious != NULL)
	      pPa1->AbPrevious->AbNext = pAb;
	    if (pPa1->AbNext != NULL)
	      pPa1->AbNext->AbPrevious = pAb;
	  }
    }
  if (pEl->ElHolophrast || (pEl->ElTerminal && pEl->ElLeafType != LtPageColBreak))
    /* met le contenu de l'element dans le pave, sauf si c'est un */
    /* element de haut ou de bas de page */
    {
      Contenu(pEl, pAb, pDoc);
      /* ajoute le volume du pave a celui de tous ses englobants */
      if (pPa1->AbVolume > 0)
	{
	  pP = pPa1->AbEnclosing;
	  while (pP != NULL)
	    {
	      pP->AbVolume += pPa1->AbVolume;
	      pP = pP->AbEnclosing;
	    }
	}
    }
  else
    {
      pPa1->AbLeafType = LtCompound;
      pPa1->AbVolume = 0;
      pPa1->AbInLine = FALSE;
      pPa1->AbTruncatedHead = TRUE;
      pPa1->AbTruncatedTail = TRUE;
    }
}

#else /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |	Chaine	chaine et remplit le pave pointe par pAb, correspondant| */
/* |		a l'element pointe par pEl dans la vue nv du document	| */
/* |		dont le contexte est pointe par pDoc.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void Chaine(PtrAbstractBox pAb, PtrElement pEl, DocViewNumber nv, PtrDocument pDoc)

#else /* __STDC__ */
void Chaine(pAb, pEl, nv, pDoc)
	PtrAbstractBox pAb;
	PtrElement pEl;
	DocViewNumber nv;
	PtrDocument pDoc;
#endif /* __STDC__ */

{
  PtrElement      pE;
  PtrAbstractBox         pP;
  boolean         stop;
  PtrAbstractBox         pPa1;
  PtrElement      pEl1;
  
  pPa1 = pAb;
  /* cherche dans cette vue le premier element ascendant qui ait un pave' */
  pP = NULL;
  pE = pEl;
  while (pE->ElParent != NULL && pP == NULL)
    {
      pE = pE->ElParent;
      pP = pE->ElAbstractBox[nv - 1];
    }
  if (pP == NULL)
    /* il n'y a pas de pave englobant pAb */ 
    {
      if (pEl->ElAbstractBox[nv - 1] == NULL)
        pEl->ElAbstractBox[nv - 1] = pAb;
    }
  else			
    /* pAb n'est pas le pave le plus englobant pour cette vue */
    /*  pP: 1er pave de l'element ascendant pour cette vue */
    /* saute les paves de presentation que l'ascendant a crees par la */
    /* regle CreateBefore */
    {
      while (pP->AbPresentationBox && pP->AbElement == pE && pP->AbNext != NULL)
	pP = pP->AbNext;	
      /* le premier pave qui n'est pas de presentation est l'englobant */
      pPa1->AbEnclosing = pP;
      if (pPa1->AbEnclosing->AbFirstEnclosed == NULL)
	/* c'est le premier pave englobe */
	{
	  pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	  if (pEl->ElAbstractBox[nv - 1] == NULL)
	    pEl->ElAbstractBox[nv - 1] = pAb;
	  /* 1er pave de l'element */
	}
      else
	/* il y a deja des paves englobes */
	if (pEl->ElTypeNumber == PageBreak + 1 
	    && pEl->ElPageType == PgBegin)
	  /* c'est une marque de page de debut d'element, on la chaine */
	  /* en tete */
	  {
	    pPa1->AbNext = pPa1->AbEnclosing->AbFirstEnclosed;
	    if (pPa1->AbNext != NULL)
	      pPa1->AbNext->AbPrevious = pAb;
	    pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	  }
	else
	  {
	    if (pEl->ElAbstractBox[nv - 1] != NULL)
	      /* cet element a deja des paves dans cette vue */
	      /* saute les paves de l'element */
	      {
		pP = pEl->ElAbstractBox[nv - 1];
		stop = FALSE;
		do
		  if (pP->AbNext == NULL)
		    stop = TRUE;
		  else if (pP->AbNext->AbElement != pEl)
		    stop = TRUE;
		  else
		    pP = pP->AbNext;
		while (!(stop));
		/* insere le nouveau pave apres */
		pPa1->AbPrevious = pP;
		pPa1->AbNext = pP->AbNext;
	      } 
	    else	
	      /* cet element n'a pas encore de paves dans cette vue */
	      {
		pEl->ElAbstractBox[nv - 1] = pAb;	
		/* 1er pave de l'element */
		/* cherche l'element precedent ayant un pave dans la vue */
		pE = BackSearchVisibleElem(pPa1->AbEnclosing->AbElement, pEl, nv);
		if (pE != NULL)
		  /* verifie si le pave trouve pour un element precedent */
		  /* est bien inclus dans le meme pave englobant. */
		  /* Par exemple deux notes de bas de page successives */
		  /* peuvent avoir leurs paves dans des boites de bas de */
		  /* page differentes */
		  {
		    pP = pE->ElAbstractBox[nv - 1];	
		    /* pave de l'element precedent */
		    do
		      pP = pP->AbEnclosing;
		    while (!(pP == pPa1->AbEnclosing || pP == NULL));
		    if (pP == NULL)
		      /* ils n'ont pas le meme pave englobant, on ne */
		      /* chainera pas le pave au pave de l'element precedent */
		      pE = NULL;
		  }
		if (pE != NULL)
		  if (pE->ElTypeNumber == PageBreak + 1 
		      && pE->ElPageType == PgBegin)
		    /* le precedent est une marque de page de debut */
		    /* d'element, on verifie si elle est suivie par des */
		    /* paves de presentation de l'englobant */
		    {
		      pP = pE->ElAbstractBox[nv - 1];	
		      /* pave de l'element precedent */
		      if (pP->AbNext != NULL)
			if (pP->AbNext->AbElement == 
			    pPa1->AbEnclosing->AbElement)
			  /* la marque de page est suivie par un pave cree */
			  /* par l'englobant */
			  pE = NULL;
		    }
		if (pE == NULL)
		  /* pas de pave d'element precedent */
		  {
		    pP = pPa1->AbEnclosing->AbFirstEnclosed;
		    /* saute les eventuelles marques de page de debut */
		    /* d'element */
		    stop = FALSE;
		    do
		      if (pP == NULL)
			stop = TRUE;
		      else
			{
			  pEl1 = pP->AbElement;
			  if (pEl1->ElTypeNumber == PageBreak + 1 &&
			      pEl1->ElPageType == PgBegin)
			    pP = pP->AbNext;
			  else
			    stop = TRUE;
			}
		    while (!(stop));
		    if (pP != NULL)
		      if (pP->AbElement == pPa1->AbEnclosing->AbElement)
			if (RegleCree(pDoc, pPa1->AbEnclosing, pP) == FnCreateLast)
			  /* le pave existant doit etre le dernier, on insere */
			  /* le nouveau pave devant lui */
			  {
			    pPa1->AbNext = pP;
			    pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			  } 
			else	
			  /* on saute les paves crees par une regle */
			  /* CreateFirst de l'englobant */
			  {
			    stop = FALSE;
			    do
			      if (pP->AbNext == NULL)
				stop = TRUE;
			      else if (pP->AbNext->AbElement != 
				       pPa1->AbEnclosing->AbElement)
				stop = TRUE;
			      else if (RegleCree(pDoc, pPa1->AbEnclosing, pP->AbNext) == 
				       FnCreateLast)
				/* le pave suivant doit etre le dernier */
				stop = TRUE;
			      else
				pP = pP->AbNext;
			    while (!(stop));
			    /* on insere le nouveau pave apres */
			    pPa1->AbPrevious = pP;
			    pPa1->AbNext = pP->AbNext;
			  }
		      else	
			/* insere le nouveau pave en tete */
			{
			  pPa1->AbNext = pP;
			  pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			}
		  } 
		else	
		  /* il y a un pave d'un element precedent */
		  {
		    pP = pE->ElAbstractBox[nv - 1];
		    if (pP->AbEnclosing != NULL &&
			pP->AbEnclosing->AbPresentationBox &&
			pP->AbEnclosing->AbElement == pE)
		       /* cet element a cree' un pave englobant par la regle */
		       /* FnCreateEnclosing, on se place au niveau de ce pave' */
		       /* englobant */
		       pP = pP->AbEnclosing;
		    else
		       {
		       /* on cherche le dernier pave de l'element precedent */
		       stop = FALSE;
		       do
		         if (pP->AbNext == NULL)
			   stop = TRUE;
		         else if (pP->AbNext->AbElement != pE)
			   stop = TRUE;
		         else
			   pP = pP->AbNext;
		       while (!(stop));
		       }
		    /* insere le nouveau pave apres pP */
		    pPa1->AbPrevious = pP;
		    pPa1->AbNext = pP->AbNext;
		  }
	      }
	    if (pPa1->AbPrevious != NULL)
	      pPa1->AbPrevious->AbNext = pAb;
	    if (pPa1->AbNext != NULL)
	      pPa1->AbNext->AbPrevious = pAb;
	  }
    }
  if (pEl->ElHolophrast || (pEl->ElTerminal && pEl->ElLeafType != LtPageColBreak))
    /* met le contenu de l'element dans le pave, sauf si c'est un */
    /* element de haut ou de bas de page */
    {
      Contenu(pEl, pAb, pDoc);
      /* ajoute le volume du pave a celui de tous ses englobants */
      if (pPa1->AbVolume > 0)
	{
	  pP = pPa1->AbEnclosing;
	  while (pP != NULL)
	    {
	      pP->AbVolume += pPa1->AbVolume;
	      pP = pP->AbEnclosing;
	    }
	}
    }
  else
    {
      pPa1->AbLeafType = LtCompound;
      pPa1->AbVolume = 0;
      pPa1->AbInLine = FALSE;
      pPa1->AbTruncatedHead = TRUE;
      pPa1->AbTruncatedTail = TRUE;
    }
}
#endif /* __COLPAGE__ */



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
  boolean         trouve;
  int             i;
  PtrSSchema    pSc1;
  SRule          *pRe1;

  if (pEl == NULL)
    {
    *pSchP = NULL;
    *pSchS = NULL;
    *NumEntree = 0;
    }
  else
    {
    *pSchP = pEl->ElSructSchema->SsPSchema;
    *pSchS = pEl->ElSructSchema;
    /* premiere regle de presentation specifique a ce type d'element */
    *NumEntree = pEl->ElTypeNumber;
    /* s'il s'agit de l'element racine d'une nature, on prend les regles */
    /* de presentation (s'il y en a) de la regle nature dans la structure */
    /* englobante. */
    /* on ne traite pas les marques de page */
    if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
      if (pEl->ElParent != NULL) 
        /* il y a un englobant */ 
        if (pEl->ElParent->ElSructSchema != pEl->ElSructSchema)
	  /* cherche la regle introduisant la nature dans le schema de */
	  /* structure de l'englobant. */
	  {
	  pSc1 = pEl->ElParent->ElSructSchema;
	  trouve = FALSE;
	  i = 0;
	  do
	    {
	      i++;
	      pRe1 = &pSc1->SsRule[i - 1];
	      if (pRe1->SrConstruct == CsNatureSchema)
		if (pRe1->SrSSchemaNat == pEl->ElSructSchema)
		  trouve = TRUE;
	    }
	  while (!(trouve || i >= pSc1->SsNRules));
	  if (trouve)
	    if (pSc1->SsPSchema->PsElemPRule[i - 1] != NULL)
	      /* il y a des regles de presentation specifiques */
	      {
		*pSchP = pSc1->SsPSchema;
		*NumEntree = i;
		*pSchS = pEl->ElParent->ElSructSchema;
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

  if (VueAssoc(pAb->AbElement))
    frame = pDoc->DocAssocFrame[pAb->AbElement->ElAssocNum - 1];
  else
    frame = pDoc->DocViewFrame[pAb->AbDocView - 1];
  ret = PaveAffiche(pAb, frame);
  return ret;
}


/* ---------------------------------------------------------------------- */
/* |	applPosRelat 	applique la regle de position PR-pRegle 	| */
/* |		au pave pPa1. 						| */
/* |		rend vrai dans appl si la regle a ete appliquee.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void applPosRelat(AbPosition *PPos, PosRule RP, PtrPRule pRegle, PtrAttribute pAttr, PtrPSchema pSchP, PtrAbstractBox pPa1, PtrDocument pDoc, boolean *appl)
#else /* __STDC__ */
static void applPosRelat(PPos, RP, pRegle, pAttr, pSchP, pPa1, pDoc, appl)
	AbPosition *PPos;
	PosRule RP;
	PtrPRule pRegle;
	PtrAttribute pAttr;
	PtrPSchema pSchP;
	PtrAbstractBox pPa1;
	PtrDocument pDoc;
	boolean *appl;
#endif /* __STDC__ */
{
  PtrAbstractBox         pPavPos;
  boolean         SautPage;
  PtrAbstractBox         pavpere;
  PosRule       *pRe1;
  PtrPRule	  pRSpec;
  PtrPSchema 	  pSchPPage;
  int		  b, HauteurHautPage;
  
  *appl = FALSE;		
  /* on n'a pas (encore) applique' la regle */
  SautPage = FALSE;
  /* le pave est-il une marque de saut de page (le trait horizontal */
  /* qui separe les pages dans les images) ? Dans ce cas, il aura */
  /* une position flottante */
  if (pPa1->AbElement->ElTerminal && pPa1->AbElement->ElLeafType == LtPageColBreak)
    if (pPa1->AbLeafType != LtCompound && !pPa1->AbPresentationBox)
      SautPage = TRUE;
  pRe1 = &RP;
  if (pRe1->PoPosDef == NoEdge || SautPage)
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
      /* cherche le pave (pPavPos) par rapport auquel le pave */
      /* traite' se positionne  */
      if (pRe1->PoRefElem)
	{
	  /* appelle l'exception des tableaux, au cas ou ce serait la regle */
	  /* de hauteur d'un filet vertical d'un tableau */
	  pPavPos = NULL;
	  if (ThotLocalActions[T_Tableau_PaveRef]!= NULL)
	    (*ThotLocalActions[T_Tableau_PaveRef])(pPa1, pRe1, pRegle, &pPavPos);
	  /* si l'exception n'a pas ete traitee, effectue un traitement normal*/
	  if (pPavPos == NULL)
	    pPavPos = PaveRef(pRe1->PoNotRel, pRe1->PoTypeRefElem, pSchP,
			      pRe1->PoRelation, pRe1->PoRefElem, pPa1, pAttr);
	}
      else
	pPavPos = PaveRef(pRe1->PoNotRel, pRe1->PoRefPresBox, pSchP, pRe1->PoRelation, pRe1
			  ->PoRefElem, pPa1, pAttr);
      if (pPavPos != NULL)
	{
	  /* on a trouve le pave de reference */
	  PPos->PosEdge = pRe1->PoPosDef;
	  PPos->PosRefEdge = pRe1->PoPosRef;
	  if (pRe1->PoDistAttr)	
	    /* c'est la valeur d'un attribut */ 
	    {
	    if (pRe1->PoDistance < 0)
	      /* il faut inverser cette valeur */
	      PPos->PosDistance = -AttrValue(pAttr);
	    else
	      PPos->PosDistance = AttrValue(pAttr);
	    if (pRe1->PoDistUnit == UnRelative ||
		pRe1->PoDistUnit == UnXHeight)
	      /* convertit en 1/10 de caractere */
	      PPos->PosDistance = 10 * PPos->PosDistance;
	    }
	  else	
	    /* c'est la valeur elle meme qui est dans la regle */
	    PPos->PosDistance = pRe1->PoDistance;
	  PPos->PosUnit = pRe1->PoDistUnit;
	  PPos->PosAbRef = pPavPos;
	  if (FirstCreation)
	    PPos->PosUserSpecified = pRe1->PoUserSpecified;
	  else
	    PPos->PosUserSpecified = FALSE;
	  if (PPos->PosUserSpecified)
	     PPos->PosUserSpecified = CheckPPosUser(pPa1, pDoc);
	  *appl = TRUE;	
	  /* a priori, l'englobement est respecte' */
	  if (pRegle->PrType == PtHorizPos)
            pPa1->AbHorizEnclosing = TRUE;
          else if (pRegle->PrType == PtVertPos)
            pPa1->AbVertEnclosing = TRUE;
	  /* verifie si le pave deroge a la regle d'englobement */
	  if (pRe1->PoRelation == RlRoot)
	    if (PPos->PosAbRef != pPa1->AbEnclosing)
	      /* ce pave deroge a la regle d'englobement */
	      
	      if (pRegle->PrType == PtHorizPos)
		pPa1->AbHorizEnclosing = FALSE;
	      else if (pRegle->PrType == PtVertPos)
		pPa1->AbVertEnclosing = FALSE;
	  if (PPos->PosAbRef == pPa1->AbEnclosing)
	    /* le pave se positionne par rapport a l'englobant */
	    
	    if (pRegle->PrType == PtHorizPos) 
	      /* position horizontale */ 
	      if (PPos->PosDistance < 0)
		{
		  if (PPos->PosEdge == Left && PPos->PosRefEdge ==
		    Left)
		  /* le cote gauche du pave est a gauche du */
		  /* cote gauche de l'englobant: debordement */
		    pPa1->AbHorizEnclosing = FALSE;
		}
	      else 
		{
		  if (PPos->PosDistance > 0)
		  {
		    if (PPos->PosEdge == Right && PPos->PosRefEdge == Right)
		    /* le cote droit du pave est a droite du */
		    /* cote droit de l'englobant: debordement */
		      pPa1->AbHorizEnclosing = FALSE;
		  }
	        }
	    else if (pRegle->PrType == PtVertPos)
	      {
#ifndef __COLPAGE__
	      /* regarde si le premier fils de l'englobant est un saut de page */
	      pavpere = pPa1->AbEnclosing;
	      if (pavpere->AbFirstEnclosed != pPa1
		  && pavpere->AbFirstEnclosed->AbElement->ElTerminal
		  && pavpere->AbFirstEnclosed->AbElement->ElLeafType == LtPageColBreak)
		{
		  /* positionne par rapport au saut de page */
		  PPos->PosRefEdge = Bottom;
		  PPos->PosAbRef = pavpere->AbFirstEnclosed;
		  /* mise a jour de la distance */
		  /* si PPos->PosDistance est en unite relative on ne fait rien */
		  /* sinon, PPos->PosDistance est decremente de la hauteur */
		  /* du haut de la boite haut de page */
		  if (PPos->PosUnit == UnPoint)
		    {
		      /* on cherche la boite page correspondant a la regle page */
		      /* portee par un des ascendants  */
		      b = TypeBPage (pavpere->AbFirstEnclosed->AbElement,
			      pavpere->AbFirstEnclosed->AbElement->ElViewPSchema,
			      &pSchPPage);
		      HauteurHautPage = pSchPPage->PsPresentBox[b - 1].PbHeaderHeight;
		       /* PbHeaderHeight toujours en points typo */
		       if (PPos->PosDistance - HauteurHautPage >= 0)
		         PPos->PosDistance = PPos->PosDistance - HauteurHautPage;
		    }
		  PPos->PosUserSpecified = FALSE;
		}
#endif /* __COLPAGE__ */
	      if (PPos->PosDistance < 0)
		{
		  if (PPos->PosEdge == Top && PPos->PosRefEdge == Top)
		    /* le haut du pave est au-dessus du */
		    /* haut de l'englobant: debordement */
		    pPa1->AbVertEnclosing = FALSE;
		 }
	      else if (PPos->PosDistance > 0)
		if (PPos->PosEdge == Bottom && PPos->PosRefEdge == Bottom)
		  /* le bas du pave est au-dessous du */
		  /* bas de l'englobant: debordement */
		  pPa1->AbVertEnclosing = FALSE;
	      }
	} 
      else
	/* on n'a pas trouve le pave de reference */ 
	if (pPa1->AbLeafType != LtCompound
	    && !(pRe1->PoRelation == RlNext 
		 || pRe1->PoRelation == RlPrevious 
		 || pRe1->PoRelation == RlSameLevel 
		 || pRe1->PoRelation == RlCreator
		 || pRe1->PoRelation == RlReferred))
	  {
            /* inutile de reessayer d'appliquer la regle */
            /* quand les paves environnants seront crees. */
	    PPos->PosEdge = pRe1->PoPosDef;
	    PPos->PosRefEdge = pRe1->PoPosRef;
	    PPos->PosDistance = pRe1->PoDistance;
	    PPos->PosUnit = pRe1->PoDistUnit;
	    PPos->PosAbRef = NULL;
	    PPos->PosUserSpecified = pRe1->PoUserSpecified;
	    if (PPos->PosUserSpecified)
	       PPos->PosUserSpecified = CheckPPosUser(pPa1, pDoc);
	    *appl = TRUE;
	    
	  }
	else if (pPa1->AbEnclosing == NULL && pRe1->PoRelation == RlEnclosing)
	  /* positionnement par rapport a la fenetre */
	  {
	    PPos->PosEdge = pRe1->PoPosDef;
	    PPos->PosRefEdge = pRe1->PoPosRef;
	    if (pRe1->PoDistAttr)
	      /* valeur d'un attribut */
	      {
	      if (pRe1->PoDistance < 0)
		/* inverser cette valeur */
		PPos->PosDistance = -AttrValue(pAttr);
	      else
		PPos->PosDistance = AttrValue(pAttr);
	      if (pRe1->PoDistUnit == UnRelative ||
		  pRe1->PoDistUnit == UnXHeight)
	        /* convertit en 1/10 de caractere */
		PPos->PosDistance = 10 * PPos->PosDistance;
	      }
	    else	
	      /* c'est la valeur elle meme */
	      PPos->PosDistance = pRe1->PoDistance;
	    if (FirstCreation)
	      PPos->PosUserSpecified = pRe1->PoUserSpecified;
	    else
	      PPos->PosUserSpecified = FALSE;
	    if (PPos->PosUserSpecified)
	      PPos->PosUserSpecified = CheckPPosUser(pPa1, pDoc);
	    PPos->PosUnit = pRe1->PoDistUnit;
	    PPos->PosAbRef = NULL;
	    *appl = TRUE;
	  }
	else
	  /* on n'a pas trouve' le pave' de reference */
	  /* si c'est un positionnement par rapport au precedent ou au */
	  /* suivant, on positionne le pave par rapport a l'englobant */
	  if (pPa1->AbEnclosing != NULL &&
	      (
	      (pRegle->PrType == PtVertPos && pRe1->PoRelation == RlPrevious &&
	        pRe1->PoPosDef == Top && pRe1->PoPosRef == Bottom) ||
	      (pRegle->PrType == PtVertPos && pRe1->PoRelation == RlNext &&
	        pRe1->PoPosDef == Bottom && pRe1->PoPosRef == Top) ||
	      (pRegle->PrType == PtHorizPos && pRe1->PoRelation == RlPrevious &&
	        pRe1->PoPosDef == Left && pRe1->PoPosRef == Right) ||
	      (pRegle->PrType == PtHorizPos && pRe1->PoRelation == RlNext &&
	        pRe1->PoPosDef == Right && pRe1->PoPosRef == Left)
	      )
	     )
	    /* c'est une regle de positionnement vertical en dessous du
	    precedent et on n'a pas trouve' le precedent. On remplace par
	    un positionnement en haut de l'englobant */
	    {
	    PPos->PosAbRef = pPa1->AbEnclosing;
	    PPos->PosEdge = pRe1->PoPosDef;
	    PPos->PosRefEdge = pRe1->PoPosDef;
	    /* s'agit-il d'une regle de presentation specifique ? */
	    pRSpec = pPa1->AbElement->ElFirstPRule;
	    while (pRSpec != NULL && pRSpec != pRegle)
	       pRSpec = pRSpec->PrNextPRule;
	    if (pRSpec == pRegle)
	       /* c'est une regle de presentation specifique */
	       /* on prend le decalage en compte */
	       PPos->PosDistance = pRe1->PoDistance;
	    else
	       /* c'est une regle generique */
	       /* on se positionne tout contre l'englobant */
	       PPos->PosDistance = 0;
	    /* on pourra reessayer d'appliquer la regle plus tard : */
	    /* le precedent existera peut etre, alors */
	    *appl = FALSE;
	    PPos->PosUnit = pRe1->PoDistUnit;
	    if (FirstCreation)
	      PPos->PosUserSpecified = pRe1->PoUserSpecified;
	    else
	      PPos->PosUserSpecified = FALSE;
	    if (PPos->PosUserSpecified)
	       PPos->PosUserSpecified = CheckPPosUser(pPa1, pDoc);
            pPa1->AbVertEnclosing = TRUE;
	    }
    }
}



/* ---------------------------------------------------------------------- */
/* |	appldimension	 applique au pave pointe' par pAb la regle	| */
/* |		de dimension pointee par pRegle.			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void appldimension(AbDimension *PDim, PtrAbstractBox pAb, PtrPSchema pSchP, PtrAttribute pAttr, boolean *appl, PtrPRule pRegle, PtrDocument pDoc)
#else /* __STDC__ */
static void appldimension(PDim, pAb, pSchP, pAttr, appl, pRegle, pDoc)
	AbDimension *PDim;
	PtrAbstractBox pAb;
	PtrPSchema pSchP;
	PtrAttribute pAttr;
	boolean *appl;
	PtrPRule pRegle;
	PtrDocument pDoc;
#endif /* __STDC__ */
{
  PtrAbstractBox         pP;
  PtrAttribute     pA;
  boolean         stop;
  DimensionRule   *pRe1;
  int		  AttrRegle;
  
  /* on met a priori les valeurs correspondant a une dimension */
  /* egale a celle du contenu. */
  PDim->DimIsPosition = FALSE;
  PDim->DimValue = 0;
  PDim->DimAbRef = NULL;
  PDim->DimUnit = pRegle->PrDimRule.DrUnit;
  PDim->DimSameDimension = TRUE;
  PDim->DimMinimum = pRegle->PrDimRule.DrMin;
  if (FirstCreation)
    PDim->DimUserSpecified = pRegle->PrDimRule.DrUserSpecified;
  else
    PDim->DimUserSpecified = FALSE;
  if (PDim->DimUserSpecified)
    PDim->DimUserSpecified = CheckPPosUser(pAb, pDoc);

  *appl = FALSE;
  pRe1 = &pRegle->PrDimRule;
  if (pRe1->DrPosition)
    {
      /* Box elastique, la dimension est definie comme une position */
      /* applique la regle */
      applPosRelat(&PDim->DimPosition, pRe1->DrPosRule, pRegle, pAttr, pSchP, pAb, pDoc, appl);
      /* si la regle a pu etre appliquee, le boite est reellement elastique */
      if (*appl)
	PDim->DimIsPosition = TRUE;
    }
  else if (pRe1->DrAbsolute)
    {
      /* valeur absolue */
      if (pRe1->DrAttr)	
	/* c'est la valeur d'un attribut */
	{
	if (pRe1->DrValue < 0)
	  /* il faut inverser cette valeur */
	  PDim->DimValue = -AttrValue(pAttr);
	else
	  PDim->DimValue = AttrValue(pAttr);
	if (pRe1->DrUnit == UnRelative ||
	    pRe1->DrUnit == UnXHeight)
	  /* convertit en 1/10 de caractere */
	  PDim->DimValue = 10 * PDim->DimValue;
	}
      else		
	/* c'est la valeur elle meme qui est dans la regle */
	PDim->DimValue = pRe1->DrValue;
      *appl = TRUE;
    }
  else if (pRe1->DrRelation == RlEnclosed)
    {
      /* dimension du contenu */
      *appl = TRUE;	
      /* les valeurs mises a priori conviennent */
      PDim->DimValue = pRe1->DrValue;	
    } 
  else
    {
      /* dimensions relatives a l'englobant ou un frere */
      PDim->DimSameDimension = pRe1->DrSameDimens;
      /* essaie d'appliquer la regle de dimensionnement relatif */
      if (pRe1->DrRefElement)
	pP = PaveRef(pRe1->DrNotRelat, pRe1->DrTypeRefElem, pSchP,
		     pRe1->DrRelation, pRe1->DrRefElement, pAb, pAttr);
      else
	pP = PaveRef(pRe1->DrNotRelat, pRe1->DrRefPresBox, pSchP,
		     pRe1->DrRelation, pRe1->DrRefElement, pAb, pAttr);
      PDim->DimAbRef = pP;
      if (pP == NULL && pAb->AbElement != NULL)
	if (pAb->AbEnclosing == NULL && pRe1->DrRelation == RlEnclosing)
	  /* heritage des dimensions de la fenetre */
	  {
	    if (pRe1->DrValue == 0)
	      {
		PDim->DimValue = 100;
		PDim->DimUnit = UnPercent;
	      }
	    else
	      {
		PDim->DimUnit = pRe1->DrUnit;
		if (pRe1->DrAttr)	
		  /* c'est la valeur d'un attribut */ 
		  {
		  if (pRe1->DrValue < 0)
		    /* inverser cette valeur */
		    PDim->DimValue = -AttrValue(pAttr);
		  else
		    PDim->DimValue = AttrValue(pAttr);
		  if (pRe1->DrUnit == UnRelative ||
		      pRe1->DrUnit == UnXHeight)
		    /* convertit en 1/10 de caractere */
		    PDim->DimValue = 10 * PDim->DimValue;
		  }
		else	
		  /* c'est la valeur elle meme */
		  PDim->DimValue = pRe1->DrValue;
	      }
	    *appl = TRUE;
	  }
      if (pP != NULL)
	{
	  if (pRe1->DrAttr)
	    /* c'est la valeur d'un attribut */
	    {
	      pA = pAttr;
	      AttrRegle = pRe1->DrValue;
	      if (pRe1->DrValue < 0)
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
	      if (pRe1->DrValue < 0)
		/* inverser cette valeur */
		PDim->DimValue = -PDim->DimValue;
	      if (pRe1->DrUnit == UnRelative ||
		  pRe1->DrUnit == UnXHeight)
	        /* convertit en 1/10 de caractere */
		PDim->DimValue = 10 * PDim->DimValue;
	    } 
	  else	
	    /* c'est la valeur elle-meme */
	    PDim->DimValue = pRe1->DrValue;
	  PDim->DimUnit = pRe1->DrUnit;
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
/* |	le boolean de retour indique si le pave pAb a ete detruit.	| */
/* |	On ajoute une marque colonne apres l'element pour permettre	| */
/* |	de reprendre une presentation conforme a la regle qui		| */
/* |	s'appliquait avant l'element pAb->AbElement			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean applCol(PtrDocument pDoc, PtrAbstractBox pAb, int VueSch, PtrPRule pRegle)	
#else /* __STDC__ */
static boolean applCol(pDoc, pAb, VueSch, pRegle)
	PtrDocument pDoc;
	PtrAbstractBox pAb;
	int VueSch;
	PtrPRule pRegle;
#endif /* __STDC__ */
{ 
    PtrElement      pElCol, pEl1, pEl, pPrec;
    PtrAbstractBox         pP;
    boolean         colexiste, stop, AvantpEl;
    boolean	    complet;
    PtrAbstractBox         PavR;
    int             frame, h, Vue;
    boolean         bool, pavedetruit, trouve;
    PtrElement   pElGrCols, pElGr1, pSuiv;
    pavedetruit = FALSE; /* a priori pas de destruction de paves */
    if (pRegle->PrViewNum == VueSch)
	/* la regle Column concerne la vue du pave traite' */
    {
	Vue = pAb->AbDocView;
	pEl = pAb->AbElement;
        pElCol = NULL;
	colexiste = FALSE;
	AvantpEl = FALSE; /* il faut recreer l'image si les marques sont creees*/
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
			&& (pEl1->ElViewPSchema != VueSch
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
	              pElGrCols = NewSubtree(PageBreak + 1, pEl->ElSructSchema,
			pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE,TRUE);
	              InsertElementBefore (pElCol, pElGrCols);
		      pElGrCols->ElPageType = ColGroup;
		      pElGrCols->ElViewPSchema = VueSch;
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
			if (pPrec->ElViewPSchema != VueSch)
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
			if (pEl1->ElViewPSchema == VueSch 
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
	                        pElGrCols = NewSubtree(PageBreak + 1, pEl->ElSructSchema,
			           pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	                        InsertElementBefore (pElCol, pElGrCols);
		                pElGrCols->ElPageType = ColGroup;
		                pElGrCols->ElViewPSchema = VueSch;
		                pElGrCols->ElPageNumber = 0;
		                /* numero attribue pour les groupes */
				AvantpEl = TRUE; /* pour recreer l'image */
				/* on detruit le pave de pElCol pour */
				/* le recreer apres pElGrCols */
				pP = pElCol->ElAbstractBox[Vue-1];
	                        while (pP != NULL && pP->AbElement == pElCol)
	                          {
	                            TuePave(pP);
	                            SuppRfPave(pP, &PavR, pDoc);
	                            pP = pP->AbNext;
	                          }
			      }
			    /* on ajoute une marque colonne gauche si elle */
			    /* n'existe pas deja,  pour revenir */
		            /* a l'ancienne regle apres l'element */ 
			    if (pEl->ElNext == NULL
			      || pEl->ElNext->ElTypeNumber != PageBreak+1)
			      {
			      pEl1 = NewSubtree(PageBreak + 1, pEl->ElSructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		              InsertElementAfter(pEl, pEl1);
		              pEl1->ElPageType = ColComputed;
		              pEl1->ElViewPSchema = VueSch;
		              pEl1->ElPageNumber = 1;
	                       /* numero attribue pour les colonnes gauche */

		               /* on cree une marque groupe de colonnes */
		              pElGr1 = NewSubtree(PageBreak + 1, pEl->ElSructSchema, pDoc, 
			       pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		               InsertElementAfter(pEl, pElGr1);
		               pElGr1->ElPageType = ColGroup;
		               pElGr1->ElViewPSchema = VueSch;
		               pElGr1->ElPageNumber = 0;
	                              /* numero attribue pour les groupes */

		              }

			}
	      }
	  }
	
	if (!colexiste &&
	    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct != CsChoice)
	  {
	    /* on cree une marque de colonne */
	    /* et une marque groupe de colonnes Cols */
	    pElGrCols = NewSubtree(PageBreak + 1, pEl->ElSructSchema,
			    pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	    pElCol = NewSubtree(PageBreak + 1, pEl->ElSructSchema, pDoc, 
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
		          while (pEl1 != NULL && pEl1->ElViewPSchema != VueSch 
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
		      AvantpEl = TRUE;
		      InsertElementBefore(pEl, pElCol);
	              InsertElementBefore (pElCol, pElGrCols);
		      /* on cree une marque colonne et une marque groupe */
		      /* de colonnes apres pour revenir */
		      /* a l'ancienne regle apres l'element */
		      /* si l'element a des suivants */
		      pSuiv = pEl;
		      trouve = FALSE;
		      while (!trouve && pSuiv != NULL)
			{
			  trouve = (pSuiv->ElNext != NULL);
			  if (!trouve)
			    pSuiv = pSuiv->ElParent;
			}
		      if (trouve)
		      {
		      /* on cree une marque colonne et une marque groupe */
		      /* de colonnes apres pour revenir */
		      /* a l'ancienne regle apres l'element */
		      pEl1 = NewSubtree(PageBreak + 1, pEl->ElSructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		      pElGr1 = NewSubtree(PageBreak + 1, pEl->ElSructSchema, pDoc, 
				 pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		      InsertElementAfter(pEl, pElGr1);
		      InsertElementAfter(pElGr1, pEl1);
		      pEl1->ElPageType = ColComputed;
		      pEl1->ElViewPSchema = VueSch;
		      pEl1->ElPageNumber = 1; /* premiere colonne de la serie */
		      pElGr1->ElPageType = ColGroup;
		      pElGr1->ElViewPSchema = VueSch;
		      pElGr1->ElPageNumber = 0; /* numero attribue pour les groupes */
		      }
		    }
	        /* on initialise pElCol et pElGrCols */
		pElCol->ElPageType = ColBegin;
		pElCol->ElViewPSchema = VueSch;
		pElCol->ElPageNumber = 1; /* premiere colonne de la page */
		pElGrCols->ElPageType = ColGroup;
		pElGrCols->ElViewPSchema = VueSch;
		pElGrCols->ElPageNumber = 0; /* numero attribue pour les groupes*/
	      } /* fin pElCol != NULL */
          }  /* fin de !colexiste */		    
      if (AvantpEl)
	{ 
	   /* creation des paves : il faut detruire ceux de pEl */
	   /* cf. code de InsereMarque de page.c */
	   pP = pEl->ElAbstractBox[Vue - 1];
	   /* destruction des paves de l'element pEl */
           if (pP != NULL)
	     pavedetruit = TRUE; /* code retour */
	   while (pP != NULL && pP->AbElement == pEl)
	     {
	       TuePave(pP);
	       SuppRfPave(pP, &PavR, pDoc);
	       pP = pP->AbNext;
	     }
           if (VueAssoc(pEl))
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
	     ModifVue(frame, &h, pP);
	     }
	    /* on nettoie l'image abstraite des paves morts */
	    LibPavMort(pP); /* liberation a partir de la racine car */
	                    /* un pave colonne a pu etre detruit */
	      
            /* cree les paves de la marque de colonne et groupe de colonnes */
            /* correspondant a la regle Column */
	    pP = CreePaves(pElGrCols, pDoc, Vue, TRUE, TRUE, &complet);
	    if (pElGrCols->ElAbstractBox[Vue-1] != NULL)
	      pP = CreePaves(pElCol, pDoc, Vue, TRUE, TRUE, &complet);
	  } /* fin AvantpEl */

        } /* fin pEl marque page non debut */
    }
  return pavedetruit;
}

#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |	applPage 	applique une regle Page				| */
/* |	on insere l'element marque page AVANT l'element (sauf racine)	| */
/* |	le boolean de retour indique si le pave pAb a ete detruit	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean applPage(PtrDocument pDoc, PtrAbstractBox pAb, int VueSch, PtrPRule pRegle, FunctionType TypeMiseEnPage)

#else /* __STDC__ */
static boolean applPage(pDoc, pAb, VueSch, pRegle, TypeMiseEnPage)
	PtrDocument pDoc;
	PtrAbstractBox pAb;
	int VueSch;
	PtrPRule pRegle;
	FunctionType TypeMiseEnPage;
#endif /* __STDC__ */

#else /* __COLPAGE__ */
/* ---------------------------------------------------------------------- */
/* |	applPage 	applique une regle Page				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void applPage(PtrDocument pDoc, PtrAbstractBox pAb, int VueSch, PtrPRule pRegle, FunctionType TypeMiseEnPage)

#else /* __STDC__ */
static void applPage(pDoc, pAb, VueSch, pRegle, TypeMiseEnPage)
	PtrDocument pDoc;
	PtrAbstractBox pAb;
	int VueSch;
	PtrPRule pRegle;
	FunctionType TypeMiseEnPage;
#endif /* __STDC__ */
#endif /* __COLPAGE__ */

{
    PtrElement      pElPage, pEl, pElFils, pPrec;
    PtrAbstractBox         pP;
    boolean         pageexiste, stop;
    int             cpt;
    PtrPSchema      pSchP;
    PtrElement      pEl1;
    boolean	    complet;
#ifdef __COLPAGE__
	    PtrAbstractBox         PavR;
     int             frame, h, Vue;
     boolean         bool, pavedetruit;
#else /* __COLPAGE__ */
    boolean         cree;
#endif /* __COLPAGE__ */
    
#ifdef __COLPAGE__
    pavedetruit = FALSE; /* a priori pas de destruction de paves */
#endif /* __COLPAGE__ */
    if (pRegle->PrViewNum == VueSch)
	/* la regle Page concerne la vue du pave traite' */
    {
#ifdef __COLPAGE__
	Vue = pAb->AbDocView;
#endif /* __COLPAGE__ */
	pElPage = NULL;
	pageexiste = FALSE;
	pEl = pAb->AbElement;
	/* l'element contient-il deja une marque de page de debut */
	/* d'element pour cette vue ? */
	if (!pEl->ElTerminal)
	{
	    pElFils = pEl->ElFirstChild;
	    stop = FALSE;
	    do
		if (pElFils == NULL)
		    stop = TRUE;
		else
		{
		    pEl1 = pElFils;
		    if (pEl1->ElTypeNumber != PageBreak + 1)
			stop = TRUE;
		    else if (pEl1->ElViewPSchema == VueSch && pEl1->ElPageType == PgBegin)
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
		          pageexiste = TRUE;
		          stop = TRUE;
	                }
#else /* __COLPAGE__ */
		    {
			pElPage = pEl1;
			pageexiste = TRUE;
			stop = TRUE;
		    }
#endif /* __COLPAGE__ */
		    else
                    /* on saute les eventuelles marque page d'autres vues ? */
			pElFils = pEl1->ElNext;
		}
	    while (!(stop));
	}
	if (!pageexiste)
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
			if (pPrec->ElViewPSchema != VueSch
			    || pPrec->ElPageType == ColBegin
			    || pPrec->ElPageType == ColComputed
			    || pPrec->ElPageType == ColGroup
			    || pPrec->ElPageType == ColUser)
#else /* __COLPAGE__ */
			if (pPrec->ElViewPSchema != VueSch)
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
			if (pEl1->ElViewPSchema == VueSch && pEl1->ElPageType == PgBegin)
			{
			    pageexiste = TRUE;
			    pElPage = pEl1;
			}
	    }
	}
	
	if (!pageexiste &&
	    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct != CsChoice)
	{
	    /* on cree une marque de page */
	    pElPage = NewSubtree(PageBreak + 1, pEl->ElSructSchema, pDoc, 
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
		    pEl1->ElViewPSchema = VueSch;
		    /* cherche le compteur de pages a appliquer */
		    cpt = CptPage(pElPage, pEl1->ElViewPSchema, &pSchP);
		    if (cpt > 0)
		        /* calcule la valeur du compteur de pages */
		        pEl1->ElPageNumber = ComptVal(cpt, pElPage->ElSructSchema, pSchP, pElPage,
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
	          pEl1->ElViewPSchema = VueSch;
	          /* cherche le compteur de pages a appliquer */
	          cpt = CptPage(pElPage, pEl1->ElViewPSchema, &pSchP);
	          if (cpt > 0)
	            /* calcule la valeur du compteur de pages */
	            pEl1->ElPageNumber = ComptVal(cpt, pElPage->ElSructSchema, pSchP, pElPage,
	                                        pEl1->ElViewPSchema);
	          else       
	            /* page non numerotee */
	            pEl1->ElPageNumber = 1;
	          /* creation des paves : il faut detruire ceux de pEl */
	          /* cf. code de InsereMarque de page.c */
	          pP = pEl->ElAbstractBox[Vue - 1];
	          /* destruction des paves de l'element pEl */
	          pavedetruit = TRUE; /* code retour */
	          while (pP != NULL && pP->AbElement == pEl)
	            {
	              TuePave(pP);
	              SuppRfPave(pP, &PavR, pDoc);
	              pP = pP->AbNext;
	            }
	          if (!pAb->AbNew)
	            /* on previent le mediateur */
	            {
	              if (VueAssoc(pEl))
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
	              bool = ModifVue(frame, &h, pP);
	            }
	          /* on nettoie l'image abstraite des paves morts */
	          LibPavMort(pAb->AbEnclosing);
	        }
	
	/* TODO quand on traitera les regles recto verso */
	/*  on verifie que la cardinalite de la page est en accord avec la
	   regle appliquee */
	PagePleine(Vue, pDoc, pElPage, TRUE);
	if (pElPage != NULL
	    && !VuePleine (Vue, pDoc, pElPage))
            /* cree les paves de la marque de page */
            /* correspondant a la regle page */
	    /* si la vue n'est pas pleine */
	  pP = CreePaves(pElPage, pDoc, Vue, TRUE, TRUE, &complet);
	/* on met a jour les numeros des pages suivantes */
        MajNumeros(NextElement(pElPage), pElPage, pDoc, TRUE);
       }  /* fin de !pageexiste */
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
		pEl1->ElViewPSchema = VueSch;
		/* cherche le compteur de pages a appliquer */
		cpt = CptPage(pElPage, pEl1->ElViewPSchema, &pSchP);
		if (cpt > 0)
		    /* calcule la valeur du compteur de pages */
		    pEl1->ElPageNumber = ComptVal(cpt, pElPage->ElSructSchema, pSchP, pElPage,
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
			/* de l'element suivant est complet en tete. */
			cree = !pP->AbTruncatedHead;
		}
		if (cree)
		    /* cree les paves de la marque de page */
		    pP = CreePaves(pElPage, pDoc, pAb->AbDocView, TRUE, TRUE, &complet);
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
/* |		nom PNom et on ignore pSchP.				| */
/* |		Retourne Vrai si un tel pave existe, et dans ce cas P	| */
/* |		pointe sur le pave trouve'				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean trouvepave(int Ntype, PtrPSchema pSchP, Name PNom, PtrAbstractBox *P)

#else /* __STDC__ */
static boolean trouvepave(Ntype, pSchP, PNom, P)
	int Ntype;
	PtrPSchema pSchP;
	Name PNom;
	PtrAbstractBox *P;
#endif /* __STDC__ */

{
  boolean         ret;
  PtrAbstractBox         PtPav;
  PtrAbstractBox         pPa1;
  
  ret = FALSE;
  pPa1 = *P;
  if (pPa1->AbPresentationBox)
   if (pPa1->AbLeafType == LtText)
    if (Ntype != 0)
      ret = strcmp(pPa1->AbPSchema->PsPresentBox[pPa1->AbTypeNum - 1].PbName,
		   pSchP->PsPresentBox[Ntype-1].PbName) == 0;
    else
      ret = strcmp(pPa1->AbPSchema->PsPresentBox[pPa1->AbTypeNum - 1].PbName,
		   PNom) == 0;
  if (!ret)
    if ((*P)->AbFirstEnclosed == NULL)
      ret = FALSE;
    else
      {
	PtPav = (*P)->AbFirstEnclosed;
	ret = trouvepave(Ntype, pSchP, PNom, &PtPav);
	while (!ret && PtPav->AbNext != NULL)
	  {
	    PtPav = PtPav->AbNext;
	    ret = trouvepave(Ntype, pSchP, PNom, &PtPav);
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
/* |		PNom et au retour, pSchP contient un pointeur sur le	| */
/* |		schema de presentation ou est definie la boite et NType	| */
/* |		contient le numero de type de cette boite.		| */
/* |		Retourne Vrai si un tel element existe et dans ce cas,	| */
/* |		pEl pointe sur l'element trouve'.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean ACopier(int *NType, PtrPSchema *pSchP, PtrSSchema *pSchS, Name PNom, PtrElement *pEl)

#else /* __STDC__ */
static boolean ACopier(NType, pSchP, pSchS, PNom, pEl)
	int *NType;
	PtrPSchema *pSchP;
	PtrSSchema *pSchS;
	Name PNom;
	PtrElement *pEl;
#endif /* __STDC__ */

{
  boolean         ret, stop;
  PtrElement      pE1;
  PtrPSchema      pSP;
  PtrSSchema	  pSS;
  PtrPRule    pRCre;
  PtrAttribute     pA;
  PtrPRule    pRe1;
  
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
	pRe1 = pRCre;
	if (pRe1->PrPresFunction == FnCreateBefore 
	    || pRe1->PrPresFunction == FnCreateWith
	    || pRe1->PrPresFunction == FnCreateAfter
	    || pRe1->PrPresFunction == FnCreateFirst
	    || pRe1->PrPresFunction == FnCreateLast)	
	  /* c'est une regle de creation */
	  if (*NType != 0)
	    {
	      ret = pRe1->PrPresBox[0] == *NType ;
	      if (ret)
		ret = strcmp(pSS->SsName, (*pSchS)->SsName)== 0;
	/* on supprime le test sur l'egalite des schemas P et on teste uniquement */
	/* les schemas de structure : cela permet a des chapitres de se referencer */
	/* mutuellement meme si leur schema de presentation different legerement */
	/* il faut que les schemas P aient les memes boites de presentation utilisees */
	/* en copie */
	    }
	  else
	      ret = strcmp(pSP->PsPresentBox[pRe1->PrPresBox[0]-1].PbName, PNom)
		    == 0;
	  if (ret && (pSP != *pSchP)) 
		/* retourne le schema de presentation et le */
		/* numero de type de la boite creee */
		{
		  *pSchP = pSP;
		  *pSchS = pSS;
		  *NType = pRe1->PrPresBox[0];
		}
	   
	if (!ret)
	  pRCre = pRe1->PrNextPRule;
      }
  while (!(stop || ret));
  if (!ret)		
    /* ce n'est pas l'element cherche', on cherche dans ses */
    /* descendants */
    if (!(*pEl)->ElTerminal && (*pEl)->ElFirstChild != NULL)
      {
	pE1 = (*pEl)->ElFirstChild;
	ret = ACopier(NType, pSchP, pSchS, PNom, &pE1);
	while (!ret && pE1->ElNext != NULL)
	  {
	    pE1 = pE1->ElNext;
	    ret = ACopier(NType, pSchP, pSchS, PNom, &pE1);
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
  PtrAbstractBox         pPa1;
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
	    pPa1 = *pAb;
	    GetBufTexte(&pBuffP);  
	    /* acquiert un buffer pour la copie */
	    /* chaine le buffer de la copie */
	    if (*pBuffPrec == NULL)
	      {
		pPa1->AbText = pBuffP;
		/* c'est le 1er buffer */
		pPa1->AbLanguage = pEC->ElLanguage;
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
	    pPa1->AbVolume += pBuffP->BuLength;
	    *pBuffPrec = pBuffP;
	    pBuffE = pBuffE->BuNext;
	  }
      }
}



/* ---------------------------------------------------------------------- */
/* |	ChercheDansSArbre cherche dans le sous-arbre dont la racine est	| */
/* |		pointe'e par pRac un element de numero de type TypeEl	| */
/* |		defini dans le schema de structure pSS, si NomType est	| */
/* |		nul, de nom de type NomType sinon.			| */
/* |		Retourne un pointeur sur l'element trouve' ou NULL si pas| */
/* |		trouve'							| */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static PtrElement ChercheDansSArbre(PtrElement pRac, int TypeEl, PtrSSchema pSS, Name NomType)

#else /* __STDC__ */
static PtrElement ChercheDansSArbre(pRac, TypeEl, pSS, NomType)
	PtrElement pRac;
	int TypeEl;
	PtrSSchema pSS;
	Name NomType;
#endif /* __STDC__ */

{
	PtrElement      pEC, pFils;
  
	pEC = NULL;	/* a priori on n'a pas trouve' */
	if (NomType[0] != '\0')
	  /* on compare les noms de type */
	  {
	    if (strcmp(NomType, pRac->ElSructSchema->SsRule[pRac->ElTypeNumber - 1].SrName) == 0)
	       pEC = pRac;
	  }
	else
	  /* on compare les numero de type et code de schema de structure */
	  if (pRac->ElTypeNumber == TypeEl 
	      && pRac->ElSructSchema->SsCode == pSS->SsCode)
	      /* c'est l'element cherche' */
	      pEC = pRac;
	if (pEC == NULL)	
	  if (!pRac->ElTerminal)
	    {
	      /* on cherche parmi les fils de l'element */
	      pFils = pRac->ElFirstChild;
	      while (pFils != NULL && pEC == NULL)
		{
		  pEC = ChercheDansSArbre(pFils, TypeEl, pSS, NomType);
		  pFils = pFils->ElNext;
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
void applCopie(PtrDocument pDoc, PtrPRule pRegle, PtrAbstractBox pAb, boolean AvecDescCopie)
#else /* __STDC__ */
void applCopie(pDoc, pRegle, pAb, AvecDescCopie)
	PtrDocument pDoc;
	PtrPRule pRegle;
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
  PtrAbstractBox         pPa1;
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
  if (pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct == CsReference)
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
	if (pRegle->PrElement)
	  /* il faut copier le contenu d'un element structure' contenu */
	  /* dans l'element reference'. On cherche cet element */
	  pE = ChercheDansSArbre(pE, pRegle->PrPresBox[0], pEl1->ElSructSchema,
				 pRegle->PrPresBoxName);
	else	
	  /* il faut copier une boite de presentation */
	  /* prend le schema de presentation qui s'applique a la reference */
	  {
	    ChSchemaPres(pAb->AbElement, &pSchP, &i, &pSchS);
	    if (pRegle->PrNPresBoxes == 0)
	      /* la boite de presentation a copier est definie par son nom */
	      {
		TBoite = 0;
		strncpy(NBoite, pRegle->PrPresBoxName, MAX_NAME_LENGTH);
		/* nom de la boite a cherche */
	      } 
	    else	
	      /* la boite de presentation est definie par son numero de type */
	      TBoite = pRegle->PrPresBox[0];	
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
	      /* on a trouve' le pave a copier, on le copie */
	      {
		pPa1 = pAb;
		pPa1->AbLeafType = LtText;
		pPa1->AbVolume = pP->AbVolume;
		if (pP->AbText != NULL)
		   *pPa1->AbText = *pP->AbText;
		pPa1->AbLanguage = pP->AbLanguage;
		pPa1->AbCanBeModified = FALSE;
	      }
	    else
	      /* on n'a pas trouve le pave a copier */
	      if (!DansTampon(pE))
		/* on ne fait rien si l'element reference' est dans le buffer*/
		/* de Couper-Coller */
		/* on cherche dans le sous-arbre abstrait de l'element */
		/* reference' un element qui cree la boite a copier */
		{
		  /*pElSrce = pE;*/
		  found = ACopier(&TBoite, &pSchP, &pSchS, NBoite, &pE);
		  if (found)
		    /* on a trouve' l'element pE qui cree la boite a copier */
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
			  /* on a trouve' la regle FnCopy. On l'applique en */
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
    if (pRegle->PrElement)
      {
      /*cherche d'abord l'element a copier a l'interieur de l'element copieur*/
      pE = ChercheDansSArbre(pAb->AbElement, pRegle->PrPresBox[0],
			     pEl1->ElSructSchema, pRegle->PrPresBoxName);

      if (pE == NULL)
	/* on n'a pas trouve' l'element a copier */
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
	      pE = ChercheDansSArbre(pEl, pRegle->PrPresBox[0],
				    pEl->ElSructSchema, pRegle->PrPresBoxName);
	    if (pE == NULL)
	      /* si on n'a pas trouve pE, c'est que c'etait une marque */
	      /* page qui avait ete genere par la racine : elle a ete */
	      /* placee comme premier fils : on applique lors le code */
	      /* de la V3 (recherche sur le pere)  */
	      pE = ChercheDansSArbre(pEl1->ElParent, pRegle->PrPresBox[0],
	       		    pEl1->ElSructSchema, pRegle->PrPresBoxName);
	   }
#else /* __COLPAGE__ */
	     /* on travaille pour une marque de page qui est engendree par */
	     /* le debut d'un element. On cherche dans cet element */
	     pE = ChercheDansSArbre(pEl1->ElParent, pRegle->PrPresBox[0],
				    pEl1->ElSructSchema, pRegle->PrPresBoxName);
#endif /* __COLPAGE__ */
      /* si on n'a pas trouve', on cherche en arriere l'element a copier */
      if (pE == NULL)
        if (pRegle->PrNPresBoxes > 0)
	  /* la boite a copier est definie par son numero de type */
          pE = BackSearchTypedElem(pAb->AbElement, pRegle->PrPresBox[0], pEl1->ElSructSchema);
/*        else */
	  /* la boite a copier est definie par son nom */
	  /* non implemente' */
      }
  if (pRegle->PrElement && pE != NULL)
    /* il faut copier l'element structure' pointe' par pE */
    {
      pPa1 = pAb;
      /* initialise le pave */
      pPa1->AbLeafType = LtText;
      pPa1->AbCanBeModified = FALSE;
      pPa1->AbVolume = 0;
      pBuffPrec = NULL;
      /* pas de buffer precedent */
      if (TypeHasException(1207, pPa1->AbElement->ElTypeNumber, pPa1->AbElement->ElSructSchema))
	{
	  if (ThotLocalActions[T_indexcopy] != NULL)
	    (*ThotLocalActions[T_indexcopy])(pE, &pAb, &pBuffPrec);
	}
      else
	{
	/* si l'element a copier est lui-meme une reference qui copie un */
	/* autre element, c'est cet autre element qu'on copie */
	pRP = NULL;
	if (pE->ElSructSchema->SsRule[pE->ElTypeNumber - 1].SrConstruct == CsReference)
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
	pDC->CdCopyRule = pRegle;
	pDC->CdNext = pE->ElCopyDescr;
	pDC->CdPrevious = NULL;
	if (pDC->CdNext != NULL)
	   pDC->CdNext->CdPrevious = pDC;
	pE->ElCopyDescr = pDC;
	pPa1->AbCopyDescr = pDC;
	}
    }
}


/* ---------------------------------------------------------------------- */
/* |	Applique   applique au pave pointe par pAb la regle pointee par| */
/* |		pRegle dans le schema de presentation pointe par pSchP.	| */
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
boolean Applique(PtrPRule pRegle, PtrPSchema pSchP, PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr, boolean *pavedetruit)
#else /* __STDC__ */
boolean Applique(pRegle, pSchP, pAb, pDoc, pAttr, pavedetruit)
	PtrPRule pRegle;
	PtrPSchema pSchP;
	PtrAbstractBox pAb;
	PtrDocument pDoc;
	PtrAttribute pAttr;
	boolean *pavedetruit;
#endif /* __STDC__ */
#else /* __COLPAGE__ */
#ifdef __STDC__
boolean Applique(PtrPRule pRegle, PtrPSchema pSchP, PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr)
#else /* __STDC__ */
boolean Applique(pRegle, pSchP, pAb, pDoc, pAttr)
	PtrPRule pRegle;
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
  int             VueSch;
  PtrAbstractBox         pPa1;
  PictInfo *myImageDescriptor;
#ifdef __COLPAGE__
  *pavedetruit = FALSE;
#else /* __COLPAGE__ */
  boolean         danspage, apresSautPage;
  AbPosition     *pPavP1;
#endif /* __COLPAGE__ */
  
  appl = TRUE;
  if (pRegle != NULL && pAb != NULL)
    if (pAb->AbElement != NULL)
    {
      pPa1 = pAb;
      VueSch = VueAAppliquer(pPa1->AbElement, pAttr, pDoc, pPa1->AbDocView);
      switch (pRegle->PrType)
	{
	case PtWidth:
	  appldimension(&pPa1->AbWidth, pAb, pSchP, pAttr, &appl,pRegle,pDoc);
	  break;
	case PtHeight:
	  appldimension(&pPa1->AbHeight, pAb, pSchP, pAttr, &appl,pRegle,pDoc);
	  /* traitement special pour le debordement vertical des cellules*/
	  /* de tableau etendues verticalement */
	  if (ThotLocalActions[T_TableauDebordeVertical]!= NULL)
	    (*ThotLocalActions[T_TableauDebordeVertical])(pRegle, pAb);
	  break;
	case PtVisibility:
	  pPa1->AbVisibility = valintregle(pRegle, pPa1->AbElement, 
					    pPa1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbVisibility = 10;
	      appl = TRUE;
	    }
	  break;
	case PtDepth:
	  pPa1->AbDepth = valintregle(pRegle, pPa1->AbElement, pPa1->AbDocView, 
				      &appl, &unit, pAttr);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbDepth = 0;
	      appl = TRUE;
	    }
	  break;
	case PtFillPattern:
	  pPa1->AbFillPattern = valintregle(pRegle, pPa1->AbElement, 
					 pPa1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbFillPattern = 0;
	      appl = TRUE;
	    }
	  break;
	case PtBackground:
	  pPa1->AbBackground = valintregle(pRegle, pPa1->AbElement, 
					    pPa1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbBackground = 0;
	      appl = TRUE;
	    }
	  break;
	case PtForeground:
	  pPa1->AbForeground = valintregle(pRegle, pPa1->AbElement, 
					    pPa1->AbDocView, &appl, &unit, pAttr);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbForeground = 1;
	      appl = TRUE;
	    }
	  break;
	case PtLineStyle:
	  pPa1->AbLineStyle = valcarregle(pRegle, pPa1->AbElement,
					    pPa1->AbDocView, &appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    {
	      pPa1->AbLineStyle = 'S';
	      appl = TRUE;
	    }
	  break;
	case PtFont:
	  pPa1->AbFont = valcarregle(pRegle, pPa1->AbElement, pPa1->AbDocView,
					&appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    {
	      pPa1->AbFont = 'T';
	      appl = TRUE;
	    }
	  if (pPa1->AbFont >= 'a' && pPa1->AbFont <= 'z')
	    /* on n'utilise que des majuscules pour les noms de police */
	    pPa1->AbFont = (char)((int)(pPa1->AbFont) - 32);
	  break;
	case PtAdjust:
	  pPa1->AbAdjust = valcadregle(pRegle, pPa1->AbElement, 
					 pPa1->AbDocView, &appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbAdjust = AlignLeft;
	      appl = TRUE;
	    }
	  break;
	case PtJustify:
	  pPa1->AbJustify = valboolregle(pRegle, pPa1->AbElement, 
					 pPa1->AbDocView, &appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbJustify = FALSE;
	      appl = TRUE;
	    }
	  break;
	case PtHyphenate:
	  pPa1->AbHyphenate = valboolregle(pRegle, pPa1->AbElement, 
					    pPa1->AbDocView, &appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbHyphenate = FALSE;
	      appl = TRUE;
	    }
	  break;
	case PtStyle:
	  c = valcarregle(pRegle, pPa1->AbElement, pPa1->AbDocView, &appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbHighlight = 0;
	      appl = TRUE;
	    }
	  else
	    switch (c)
	      {
	    case 'I':
		pPa1->AbHighlight = 2;
		break;
	    case 'B':
		pPa1->AbHighlight = 1;
		break;
		/*iso*/	      case 'O':
		pPa1->AbHighlight = 3;
		break;
	    case 'G':
		pPa1->AbHighlight = 4;
		break;
	    case 'Q':
		pPa1->AbHighlight = 5;
		break;
	    default:
		pPa1->AbHighlight = 0;
		break;
	      }
	  break;
	case PtUnderline:
	  c = valcarregle(pRegle, pPa1->AbElement, pPa1->AbDocView, &appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbUnderline = 0;
	      appl = TRUE;
	    } 
	  else
	    switch (c)
	      {
	    case 'C':
		pPa1->AbUnderline = 3;
		break;
	    case 'O':
		pPa1->AbUnderline = 2;
		break;
	    case 'U':
		pPa1->AbUnderline = 1;
		break;
	    default:
		pPa1->AbUnderline = 0;
		break;
	      }
	  break;
	case PtThickness:
	  c = valcarregle(pRegle, pPa1->AbElement, pPa1->AbDocView, &appl);
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbThickness = 0;
	      appl = TRUE;
	    } 
	  else
	    switch (c)
	      {
	    case 'T':
		pPa1->AbThickness = 1;
		break;
	    default:
		pPa1->AbThickness = 0;
		break;
	      }
	  break;
	case PtSize:
	  /* on applique la regle de taille */
	  pPa1->AbSize = valintregle(pRegle, pPa1->AbElement, 
					pPa1->AbDocView, &appl, &unit, pAttr);
	  if (appl)
	    pPa1->AbSizeUnit = unit;
	  else
	    if (pPa1->AbElement->ElParent == NULL)
	      /* c'est la racine, on met a priori la valeur par defaut */
	      {
	        pPa1->AbSize = 3;
	        pPa1->AbSizeUnit = UnRelative;
		appl = TRUE;
	      }
	  break;
	case PtIndent:
	  pPa1->AbIndent = valintregle(pRegle, pPa1->AbElement, 
					pPa1->AbDocView, &appl, &unit, pAttr);
	  pPa1->AbIndentUnit = unit;
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbIndent = 0;
	      appl = TRUE;
	    }
	  break;
	case PtLineSpacing:
	  pPa1->AbLineSpacing = valintregle(pRegle, pPa1->AbElement, 
					    pPa1->AbDocView, &appl, &unit, pAttr);
	  pPa1->AbLineSpacingUnit = unit;
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbLineSpacing = 10;
	      pPa1->AbLineSpacingUnit = UnRelative;
	      appl = TRUE;
	    }
	  break;
	case PtLineWeight:
	  pPa1->AbLineWeight = valintregle(pRegle, pPa1->AbElement, 
					    pPa1->AbDocView, &appl, &unit, pAttr);
	  pPa1->AbLineWeightUnit = unit;
	  if (!appl && pPa1->AbElement->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pPa1->AbLineWeight = 1;
	      pPa1->AbLineWeightUnit = UnPoint;
	      appl = TRUE;
	    }
	  break;
	case PtVertRef:
	  Posit = pPa1->AbVertRef;
	  applPosRelat(&Posit, pRegle->PrPosRule, pRegle, pAttr, pSchP, pAb,
		       pDoc, &appl);
	  pPa1->AbVertRef = Posit;
	  break;
	case PtHorizRef:
	  Posit = pPa1->AbHorizRef;
	  applPosRelat(&Posit, pRegle->PrPosRule, pRegle, pAttr, pSchP, pAb,
		       pDoc, &appl);
	  pPa1->AbHorizRef = Posit;
	  break;
	case PtVertPos:
	 /* erreur : ce n'est pas a l'editeur d'interpreter */
	 /* la mise en ligne. On supprime ce code */	  
#ifdef __COLPAGE__
	  /* changement complet du code */
	  /* plus de cas particulier pour les pages et colonnes */
	  /* ce n'est pas un cas particulier : on applique */
	  /* ses regles */
	  /* applique la regle de positionnement de l'element */
	  Posit = pPa1->AbVertPos;
	  applPosRelat(&Posit, pRegle->PrPosRule, pRegle, pAttr, pSchP, pAb,
		       pDoc, &appl);
	  pPa1->AbVertPos = Posit;
	  /* traitement special pour le debordement vertical des cellules*/
	  /* de tableau etendues verticalement */
	  if (ThotLocalActions[T_TableauDebordeVertical]!= NULL)
	    (*ThotLocalActions[T_TableauDebordeVertical])(pRegle, pAb);
	  break;
#else /* __COLPAGE__ */
	      /* Si le precedent est un separateur de page, le pave est */
	      /* positionne' en dessous de ce saut de page, sauf si le pave */
	      /* positionne' fait partie d'un separateur de page (c'est une */
	      /* boite de haut ou de bas de page qui doit etre placee par */
	      /* rapport au filet separateur) ou s'il se positionne par */
	      /* rapport a un autre element. */
	      apresSautPage = FALSE;
	      if (pPa1->AbPrevious != NULL)
		/* il y a un pave precedent */ 
		{
		  if (!pPa1->AbPrevious->AbDead 
		      && pPa1->AbPrevious->AbElement->
		      ElTypeNumber == PageBreak + 1 
		      && pPa1->AbElement->ElTypeNumber != PageBreak + 1)
		    if (pRegle->PrPosRule.PoRelation == RlSameLevel 
		        || pRegle->PrPosRule.PoRelation == RlPrevious)
		     apresSautPage = TRUE;
		    else 
		      {
			if (pRegle->PrPosRule.PoRelation == RlEnclosing)
		          {
		  	    if (pPa1->AbPrevious->AbElement->
			        ElPageType != PgBegin)
		              apresSautPage = TRUE;
		          }  
		      }
		}
	      else
		/* il n'y a pas de pave precedent */ 
		if (pPa1->AbElement->ElPrevious != NULL)	
		  /* il y a un element precedent */
		  if (pPa1->AbElement->ElPrevious->ElTypeNumber == PageBreak + 1 
		      && pPa1->AbElement->ElPrevious->ElViewPSchema == VueSch 
		      && pPa1->AbElement->ElTypeNumber != PageBreak + 1)
		    /* l'element precedent est une marque de page pour la vue */
		    if (pRegle->PrPosRule.PoRelation == RlSameLevel 
			|| pRegle->PrPosRule.PoRelation == RlPrevious )
		      apresSautPage = TRUE;
	      if (apresSautPage)	
		/* position: en dessous du pave precedent */ 
		if (pPa1->AbPrevious == NULL)
		  /* le pave de la marque de page n'est pas encore cree', on */
		  /* ne peut pas appliquer la regle de positionnement */
		  appl = FALSE;
		else
		  {
		    pPavP1 = &pPa1->AbVertPos;
		    pPavP1->PosEdge = Top;
		    pPavP1->PosRefEdge = Bottom;
		    pPavP1->PosDistance = 0;
		    pPavP1->PosUnit = UnPoint;
		    pPavP1->PosAbRef = pPa1->AbPrevious;
		    pPavP1->PosUserSpecified = FALSE;
		  }
	      else	
		/* s'il s'agit d'une boite page (celle qui englobe le filet */
		/* et les hauts et pieds de page), et si elle n'est precedee */
		/* d'aucun element, elle se positionne en haut de l'englobant. */
		{
		  danspage = FALSE;
		  if (pPa1->AbEnclosing != NULL)
		    if (pPa1->AbElement->ElTypeNumber == PageBreak + 1 
			&& pPa1->AbEnclosing->AbElement->
			ElTypeNumber != PageBreak + 1 
			&& pPa1->AbElement->ElPrevious == NULL)
		      if (pPa1->AbPrevious == NULL)
			danspage = TRUE;
		  
		  if (danspage)
		    {
		      pPavP1 = &pPa1->AbVertPos;
		      pPavP1->PosAbRef = pPa1->AbEnclosing;
		      pPavP1->PosEdge = Top;
		      pPavP1->PosRefEdge = Top;
		      pPavP1->PosDistance = 0;
		      pPavP1->PosUnit = UnPoint;
		      pPavP1->PosUserSpecified = FALSE;
		    } 
		  else	
		    /* applique la regle de positionnement de l'element */
		    {
		      Posit = pPa1->AbVertPos;
		      applPosRelat(&Posit, pRegle->PrPosRule, pRegle, pAttr,
				   pSchP, pAb, pDoc, &appl);
		      pPa1->AbVertPos = Posit;
		    }
		}
	      /* traitement special pour le debordement vertical des cellules*/
	      /* de tableau etendues verticalement */
	      if (ThotLocalActions[T_TableauDebordeVertical]!= NULL)
		(*ThotLocalActions[T_TableauDebordeVertical])(pRegle, pAb);
	  break;
#endif /* __COLPAGE__ */
	case PtHorizPos:
	      Posit = pPa1->AbHorizPos;
	      applPosRelat(&Posit, pRegle->PrPosRule, pRegle, pAttr, pSchP,
			   pAb, pDoc, &appl);
	      pPa1->AbHorizPos = Posit;
	  break;
	case PtFunction:
	  switch (pRegle->PrPresFunction)
	    {
          case FnLine:
	      if (pPa1->AbLeafType == LtCompound)
		/* si la regle de mise en lignes est definie pour la */
		/* vue principale, elle s'applique a toutes les vues, */
		/* sinon, elle ne s'applique qu'a la vue pour laquelle */
		/* elle est definie */
		if (pRegle->PrViewNum == 1 || pRegle->PrViewNum == VueSch)
		  pPa1->AbInLine = TRUE;
	      break;
	  case FnNoLine:
	      if (pPa1->AbLeafType == LtCompound)
		if (pRegle->PrViewNum == VueSch)
		   pPa1->AbInLine = FALSE;
	      break;
          case FnPage:
#ifdef __COLPAGE__
	      if (applPage(pDoc, pAb, VueSch, pRegle, pRegle->PrPresFunction))
	        *pavedetruit = TRUE;
#else /* __COLPAGE__ */
#endif /* __COLPAGE__ */
	      applPage(pDoc, pAb, VueSch, pRegle, pRegle->PrPresFunction);
	      break;
          case FnColumn:
#ifdef __COLPAGE__
	      if (applCol(pDoc, pAb, VueSch, pRegle))
	        *pavedetruit = TRUE;
#endif /* __COLPAGE__ */
	      break;
	  case FnSubColumn:
	      
	      break;
          case FnCopy:
	      if (!pAb->AbElement->ElHolophrast)
	        /* on n'applique pas la regle copie a un element holophraste'*/
	        applCopie(pDoc, pRegle, pAb, TRUE);
	      break;
	  case FnContentRef:
	      CopieConstante(pRegle->PrPresBox[0], pSchP, pAb);
	      break;
          default:
	      break;
	    }
	  
	  break;
	case PtPictInfo:
	  UpdateImageDescriptor(pPa1->AbPictInfo, (int *)&(pRegle->PrPictInfo));
	  myImageDescriptor = (PictInfo *)pPa1->AbPictInfo;
	  myImageDescriptor->PicFileName = pPa1->AbElement->ElText->BuContent;
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
/* |		Au retour, Nouveau indique s'il s'agit d'une regle	| */
/* |		nouvellement creee ou non.				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrPRule ChReglePres(PtrElement pEl, PRuleType TypeR, boolean *Nouveau, PtrDocument pDoc, int Vue)
#else /* __STDC__ */
PtrPRule ChReglePres(pEl, TypeR, Nouveau, pDoc, Vue)
	PtrElement pEl;
	PRuleType TypeR;
	boolean *Nouveau;
	PtrDocument pDoc;
	int Vue;
#endif /* __STDC__ */
{
  PtrPRule    pRegle, pR;
  PtrElement      pEl1;
  PtrPRule    pRe1;
  
  *Nouveau = FALSE;
  pRegle = NULL;
  if (pEl != NULL)
    {
      pEl1 = pEl; 
      /* l'element du pave */ 
      if (pEl1->ElFirstPRule == NULL)
	{
	  /* cet element n'a aucune regle de presentation specifique, on en */
	  /* cree une et on la chaine a l'element */
	  GetReglePres(&pRegle);
	  *Nouveau = TRUE;
	  pEl1->ElFirstPRule = pRegle;
	  pRegle->PrType = TypeR;
	} 
      else
	{
	  /* cherche parmi les regles de presentation specifiques de
	     l'element si ce type de regle existe pour la vue
	     a laquelle appartient le pave. */
	  pR = pEl1->ElFirstPRule; /* premiere regle specifique de l'element */
	  while (pRegle == NULL)
	    {
	      pRe1 = pR;
	      if (pRe1->PrType == TypeR &&
		  pRe1->PrViewNum == pDoc->DocView[Vue - 1].DvPSchemaView)
		/* la regle existe deja */
		pRegle = pR;
	      else if (pRe1->PrNextPRule != NULL)
		/* passe a la regle specifique suivante de l'element */
		pR = pRe1->PrNextPRule;
	      else
		{
		  /* On a examine' toutes les regles specifiques de */
		  /* l'element, ajoute une nouvelle regle en fin de chaine */
		  GetReglePres(&pRegle);
		  *Nouveau = TRUE;
		  pRe1->PrNextPRule = pRegle;
		  pRegle->PrType = TypeR;
		}
	    }
	}
    }
  return pRegle;
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
  PtrDocument     pDo1;
  PtrAbstractBox         pPa1;
  
  
  pDo1 = pDoc;
  pPa1 = pAb;
  if (!VueAssoc(pPa1->AbElement))
    pDo1->DocViewModifiedAb[pPa1->AbDocView - 1] =
      Englobant(pAb, pDo1->DocViewModifiedAb[pPa1->AbDocView-1]);
  else
    pDo1->DocAssocModifiedAb[pPa1->AbElement->ElAssocNum - 1] =
      Englobant(pAb,pDo1->DocAssocModifiedAb[pPa1->
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
  PtrPRule    pRegleDimH, pRegleDimV, pR, pRStd;
  PtrPSchema      pSPR;
  PtrSSchema    pSSR;
  PtrDocument     pDoc;
  PtrElement      pEl;
  PtrAttribute	    pAttr;
  int             hauteur, largeur;
  int             frame[MAX_VIEW_DOC];
  int             VueSch;
  PtrAbstractBox         pP;
  int             VueDoc;
  boolean         stop;
  int		  Vue;
#ifdef __COLPAGE__
  boolean   bool; 
#endif /* __COLPAGE__ */
  
  pRegleDimH = NULL;
  pRegleDimV = NULL;
  /* nettoie la table des frames a reafficher */
  for (VueDoc = 1; VueDoc <= MAX_VIEW_DOC; VueDoc++)
    frame[VueDoc - 1] = 0;
  pEl = pAb->AbElement;	/* l'element auquel correspond le pave */
  pDoc = DocumentOfElement(pEl);	/* le document auquel appartient le pave */ 
  Vue = pAb->AbDocView;		/* la vue concernee */
  VueSch = VueAAppliquer(pEl, NULL, pDoc, Vue);/* type de cette vue dans le schema P */
  
  /* les deltas de dimension que l'on va appliquer sont ceux 
     de la boite par defaut avec laquelle on a cree l'image */
  DimPavePt(pAb, &hauteur, &largeur);
  
  /* traite le changement de largeur */
  
  /* cherche d'abord la regle de dimension qui s'applique a l'element */
  pRStd = ReglePEl(pEl, &pSPR, &pSSR, 0, NULL, VueSch, PtWidth, FALSE, TRUE, &pAttr);
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
      pRegleDimH = ChReglePres(pEl, PtWidth, &nouveau, pDoc, Vue);
      if (nouveau)
	/* on a cree' une regle de largeur pour l'element */
	{
	  pR = pRegleDimH->PrNextPRule;	/* on recopie la regle standard */
	  *pRegleDimH = *pRStd;
	  pRegleDimH->PrNextPRule = pR;
	  pRegleDimH->PrCond = NULL;
	  pRegleDimH->PrViewNum = VueSch;
	}
      pRegleDimH->PrDimRule.DrAbsolute = TRUE;
      pRegleDimH->PrDimRule.DrSameDimens = FALSE;
      pRegleDimH->PrDimRule.DrMin = FALSE;
      pRegleDimH->PrDimRule.DrUnit = UnPoint;
      pRegleDimH->PrDimRule.DrAttr = FALSE;
      
      /* change la longueur dans la regle specifique */
      pRegleDimH->PrDimRule.DrValue = largeur;
    }
  
  /* traite le changement de hauteur de la boite */
  
  /* cherche d'abord la regle de dimension qui s'applique a l'element */
  pRStd = ReglePEl(pEl, &pSPR, &pSSR, 0, NULL, VueSch, PtHeight, FALSE, TRUE, &pAttr);
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
      pRegleDimV = ChReglePres(pEl, PtHeight, &nouveau, pDoc, Vue);
      if (nouveau)
	/* on a cree' une regle de hauteur pour l'element */
	{
	  pR = pRegleDimV->PrNextPRule;	/* on recopie la regle standard */
	  *pRegleDimV = *pRStd;
	  pRegleDimV->PrNextPRule = pR;
	  pRegleDimV->PrCond = NULL;
	  pRegleDimV->PrViewNum = VueSch;
	}
      pRegleDimV->PrDimRule.DrAbsolute = TRUE;
      pRegleDimV->PrDimRule.DrSameDimens = FALSE;
      pRegleDimV->PrDimRule.DrMin = FALSE;
      pRegleDimV->PrDimRule.DrUnit = UnPoint;
      pRegleDimV->PrDimRule.DrAttr = FALSE;
      
      /* change le parametre de la regle */
      pRegleDimV->PrDimRule.DrValue = hauteur;
    }
  
  /* applique les nouvelles regles de presentation */
  
  if (pRegleDimV != NULL || pRegleDimH != NULL)
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
	      pP = pEl->ElAbstractBox[VueDoc -1];
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
		{
		  /* applique la nouvelle regle specifique Horizontale */
		  if (pRegleDimH != NULL)
#ifdef __COLPAGE__
		    if (Applique(pRegleDimH, pSPR, pP, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
		    if (Applique(pRegleDimH, pSPR, pP, pDoc, pAttr))
#endif /* __COLPAGE__ */
		      pP->AbWidthChange = TRUE;
		  /* applique la nouvelle regle specifique Verticale */
		  if (pRegleDimV != NULL)
#ifdef __COLPAGE__
		    if (Applique(pRegleDimV, pSPR, pP, pDoc, pAttr, &bool))
#else /* __COLPAGE__ */
		    if (Applique(pRegleDimV, pSPR, pP, pDoc, pAttr))
#endif /* __COLPAGE__ */
		      pP->AbHeightChange = TRUE;
		  
		  PavReaff(pP, pDoc); /* indique le pave a reafficherv */
		  if (!VueAssoc(pEl))
		    frame[VueDoc -1] = pDoc->DocViewFrame[VueDoc -1];
		  else
		    frame[VueDoc -1] = pDoc->DocAssocFrame[pEl->ElAssocNum -1];
		}
	    }
    }
}
/* End Of Module pres */
