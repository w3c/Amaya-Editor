/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module is part of the Thot library.
 * 
 */
 
/*
 * gestion des regles de presentation de l'image abstraite.
 * Ce module applique les regles de presentation aux paves.     
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "picture.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#include "tree_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "appdialogue_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"
#include "absboxes_f.h"
#include "abspictures_f.h"
#include "buildboxes_f.h"
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
#include "font_f.h"
#include "units_f.h"

/*----------------------------------------------------------------------
   	AttrValue retourne la valeur que prend l'attribut numerique	
   		pointe' par pAttr.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 AttrValue (PtrAttribute pAttr)
#else  /* __STDC__ */
int                 AttrValue (pAttr)
PtrAttribute        pAttr;

#endif /* __STDC__ */
{
   int                 ret;


   ret = 0;
   if (pAttr != NULL)
     {
	if (pAttr->AeAttrType == AtNumAttr)
	   ret = pAttr->AeAttrValue;
     }
   return ret;
}


/*----------------------------------------------------------------------
   	FollowNotPres Si pAbb pointe un pave de presentation, retourne	
   		dans pAbb le premier pave qui n'est pas un pave de	
   		presentation et qui suit le pave pAbb a l'appel.		
   		Retourne NULL si le pave n'est suivi que de paves de	
   		presentation. Si, a l'appel, pAbb est  un pave qui n'est	
   		pas un pave de presentation, alors pAbb reste inchange'.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FollowNotPres (PtrAbstractBox * pAbb)
#else  /* __STDC__ */
static void         FollowNotPres (pAbb)
PtrAbstractBox     *pAbb;

#endif /* __STDC__ */
{
   ThotBool            stop;


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


/*----------------------------------------------------------------------
   	AncestorAbsBox	  rend le premier element pElAsc ascendant de pE
   			  et qui possede un pave (non de presentation)
			  dans la vue view
   			  retourne ce pave dans pAbb ou NULL sinon
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AncestorAbsBox (PtrElement pE, DocViewNumber view, PtrAbstractBox * pAbb, PtrElement * pElAsc)
#else  /* __STDC__ */
static void         AncestorAbsBox (pE, view, pAbb, pElAsc)
PtrElement          pE;
DocViewNumber       view;
PtrAbstractBox     *pAbb;
PtrElement         *pElAsc;

#endif /* __STDC__ */
{

   *pElAsc = pE;
   *pAbb = NULL;
   if (pE->ElParent != NULL)
      while ((*pElAsc)->ElParent != NULL && *pAbb == NULL)
	{
	   *pElAsc = (*pElAsc)->ElParent;
	   *pAbb = (*pElAsc)->ElAbstractBox[view - 1];
	   while (*pAbb && (*pAbb)->AbPresentationBox &&
		           (*pAbb)->AbElement == *pElAsc)
	      *pAbb = (*pAbb)->AbNext;
	   if (*pAbb != NULL)
	      if ((*pAbb)->AbDead || (*pAbb)->AbElement != *pElAsc)
		 *pAbb = NULL;
	}
}



/*----------------------------------------------------------------------
   	AbsBoxInherit  rend le pointeur sur le pave correpondant a l'element	
   	qui sert de reference quand on applique la regle d'heritage	
   	pointe par pPRule a l'element pointe par pEl, dans la vue view.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrAbstractBox AbsBoxInherit (PtrPRule pPRule, PtrElement pEl, DocViewNumber view)

#else  /* __STDC__ */
static PtrAbstractBox AbsBoxInherit (pPRule, pEl, view)
PtrPRule            pPRule;
PtrElement          pEl;
DocViewNumber       view;

#endif /* __STDC__ */

{
   PtrElement          pElInherit;
   PtrAbstractBox      pAbb;

   pAbb = NULL;
   pElInherit = pEl;
   if (pEl != NULL)
      switch (pPRule->PrInheritMode)
	    {
	       case InheritParent:
		  AncestorAbsBox (pEl, view, &pAbb, &pElInherit);
		  break;
	       case InheritGrandFather:
		  AncestorAbsBox (pEl, view, &pAbb, &pElInherit);
		  if (pAbb != NULL)
		    {
		       pElInherit = pAbb->AbElement;
		       pAbb = NULL;
		       AncestorAbsBox (pElInherit, view, &pAbb, &pElInherit);
		    }
		  break;
	       case InheritPrevious:
		  while (pElInherit->ElPrevious != NULL && pAbb == NULL)
		    {
		       pElInherit = pElInherit->ElPrevious;
		       pAbb = pElInherit->ElAbstractBox[view - 1];
		       FollowNotPres (&pAbb);	/* saute les paves de presentation */
		       if (pAbb != NULL)
			  if (pAbb->AbDead)
			     pAbb = NULL;
		    }
		  if (pAbb == NULL)
		     AncestorAbsBox (pEl, view, &pAbb, &pElInherit);
		  break;
	       case InheritChild:
		  while (!pElInherit->ElTerminal && pElInherit->ElFirstChild != NULL && pAbb == NULL)
		    {
		       pElInherit = pElInherit->ElFirstChild;
		       pAbb = pElInherit->ElAbstractBox[view - 1];
		       FollowNotPres (&pAbb);	/* saute les paves de presentation */
		       if (pAbb != NULL)
			  if (pAbb->AbDead)
			     pAbb = NULL;
		    }
		  break;
	       case InheritCreator:
		  pAbb = pEl->ElAbstractBox[view - 1];
		  if (pAbb != NULL)
		    {
		       FollowNotPres (&pAbb);	/* saute les paves de presentation */
		       if (pAbb != NULL)
			  if (pAbb->AbDead)
			     pAbb = NULL;
		    }
		  break;
	    }

   FollowNotPres (&pAbb);	/* saute les paves de presentation */
   return pAbb;
}


/*----------------------------------------------------------------------
   	AbsBoxInheritImm  rend le pointeur sur le pave correspondant a	
   		l'element qui sert de reference quand on applique la	
   		regle d'heritage pointe par pPRule a l'element pointe	
   		par pEl, dans la vue view. On ne considere que l'element	
   		immediatement voisin (pere, frere, fils).		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrAbstractBox AbsBoxInheritImm (PtrPRule pPRule, PtrElement pEl, DocViewNumber view)

#else  /* __STDC__ */
static PtrAbstractBox AbsBoxInheritImm (pPRule, pEl, view)
PtrPRule            pPRule;
PtrElement          pEl;
DocViewNumber       view;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbb;

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
		       FollowNotPres (&pAbb);	/* saute les paves de presentation */
		       if (pAbb->AbDead)
			  pAbb = NULL;
		    }
		  break;
	    }

   FollowNotPres (&pAbb);
   if (pAbb != NULL)
      if (pAbb->AbDead)
	 pAbb = NULL;
   return pAbb;
}


/*----------------------------------------------------------------------
  BorderStyleCharValue
  return the character value of the border style whose numeric value
  is borderStyle
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char  BorderStyleCharValue (int borderStyle)
#else  /* __STDC__ */
static char  BorderStyleCharValue (borderStyle)
int borderStyle;
#endif /* __STDC__ */
{
   char ret;

   switch (borderStyle)
     {
     case 1:
       ret = '0';  /* none */
       break;
     case 2:
       ret = 'H';  /* hidden */
       break;
     case 3:
       ret = '.';  /* dotted */
       break;
     case 4:
       ret = '-';  /* dashed */
       break;
     case 5:
       ret = 'S';  /* solid */
       break;
     case 6:
       ret = 'D';  /* double */
       break;
     case 7:
       ret = 'G';  /* groove */
       break;
     case 8:
       ret = 'R';  /* ridge */
       break;
     case 9:
       ret = 'I';  /* inset */
       break;
     case 10:
       ret = 'O';  /* outset */
       break;
     default:
       ret = '0';
       break;
     }
   return ret;
}

/*----------------------------------------------------------------------
  BorderStyleIntValue
  return the numeric value of the border style whose character value
  is borderStyle
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int BorderStyleIntValue (CHAR_T borderStyle)
#else  /* __STDC__ */
static int  BorderStyleIntValue (borderStyle)
CHAR_T borderStyle;
#endif /* __STDC__ */
{
   int ret;

   switch (borderStyle)
     {
     case '0':
       ret = 1;  /* none */
       break;
     case 'H':
       ret = 2;  /* hidden */
       break;
     case '.':
       ret = 3;  /* dotted */
       break;
     case '-':
       ret = 4;  /* dashed */
       break;
     case 'S':
       ret = 5;  /* solid */
       break;
     case 'D':
       ret = 6;  /* double */
       break;
     case 'G':
       ret = 7;  /* groove */
       break;
     case 'R':
       ret = 8;  /* ridge */
       break;
     case 'I':
       ret = 9;  /* inset */
       break;
     case 'O':
       ret = 10;  /* outset */
       break;
     default:
       ret = 1;
       break;
     }
   return ret;
}

/*----------------------------------------------------------------------
   	CharRule evalue une regle de presentation de type caractere	
   		pour la vue view. La regle a evaluer est pointee par	
   		pPRule, et l'element auquel elle s'applique est pointe	
   		par pEl. Au retour, ok indique si l'evaluation a pu	
   		etre faite. 						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char         CharRule (PtrPRule pPRule, PtrElement pEl, DocViewNumber view, ThotBool * ok)

#else  /* __STDC__ */
static char         CharRule (pPRule, pEl, view, ok)
PtrPRule            pPRule;
PtrElement          pEl;
DocViewNumber       view;
ThotBool           *ok;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbb;
   char                val;

   val = SPACE;
   *ok = TRUE;
   if (pPRule != NULL && pEl != NULL)
     {
     switch (pPRule->PrPresMode)
	{
	case PresFunction:
	   break;
	case PresImmediate:
	   val = pPRule->PrChrValue;
	   break;
	case PresInherit:
	   pAbb = AbsBoxInherit (pPRule, pEl, view);
	   if (pAbb == NULL)
	      *ok = FALSE;
	   else
	      switch (pPRule->PrType)
		 {
		 case PtFont:
		    val = pAbb->AbFont;
		    break;
		 case PtStyle:
		    if (pAbb->AbFontStyle == 1) /* I: Italic */
		       val = 'I';
		    else if (pAbb->AbFontStyle == 2)	/* O: oblique */
		       val = 'O';
		    else	/* default: R: Roman */
		       val = 'R';
		    break;
		 case PtWeight:
		    if (pAbb->AbFontWeight == 1)	/* B: Bold */
		       val = 'B';
		    else	/* default: N: Normal */
		       val = 'N';
		    break;
		 case PtUnderline:
		    if (pAbb->AbUnderline == 1)
		       val = 'U';
		    else if (pAbb->AbUnderline == 2)
		       val = 'O';
		    else if (pAbb->AbUnderline == 3)
		       val = 'C';
		    else
		       val = 'N';
		    break;
		 case PtThickness:
		    if (pAbb->AbThickness == 1)
		       val = 'T';
		    else
		       val = 'N';
		    break;
		 case PtLineStyle:
		    val = pAbb->AbLineStyle;
		    break;
		 case PtBorderTopStyle:
		    val = BorderStyleCharValue (pAbb->AbTopStyle);
		    break;
		 case PtBorderRightStyle:
		    val = BorderStyleCharValue (pAbb->AbRightStyle);
		    break;
		 case PtBorderBottomStyle:
		    val = BorderStyleCharValue (pAbb->AbBottomStyle);
		    break;
		 case PtBorderLeftStyle:
		    val = BorderStyleCharValue (pAbb->AbLeftStyle);
		    break;
		 default:
		    break;
	         }
	   break;
	}
     }
   return val;
}


/*----------------------------------------------------------------------
   	AlignRule evalue une regle d'ajustement pour la vue view.	
   		La regle a evaluer est pointee par pPRule, et l'element	
   		auquel elle s'applique est pointe par pEl.		
   		Au result, ok indique si l'evaluation a pu etre faite.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static BAlignment   AlignRule (PtrPRule pPRule, PtrElement pEl, DocViewNumber view, ThotBool * ok)

#else  /* __STDC__ */
static BAlignment   AlignRule (pPRule, pEl, view, ok)
PtrPRule            pPRule;
PtrElement          pEl;
DocViewNumber       view;
ThotBool           *ok;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbb;
   BAlignment          val;

   val = AlignLeft;
   *ok = TRUE;
   if (pPRule != NULL && pEl != NULL)
     {
	switch (pPRule->PrPresMode)
	      {
		 case PresInherit:
		    pAbb = AbsBoxInherit (pPRule, pEl, view);
		    if (pAbb == NULL)
		       *ok = FALSE;
		    else if (pPRule->PrType == PtAdjust)
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



/*----------------------------------------------------------------------
   	 BoolRule evalue une regle de presentation de type booleen	
   		pour la vue view. La regle a evaluer est pointee par	
   		pPRule, et l'element auquel elle s'applique est pointe	
   		par pEl.						
   		Au result, ok indique si l'evaluation a pu etre faite.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     BoolRule (PtrPRule pPRule, PtrElement pEl, DocViewNumber view, ThotBool * ok)

#else  /* __STDC__ */
static ThotBool     BoolRule (pPRule, pEl, view, ok)
PtrPRule            pPRule;
PtrElement          pEl;
DocViewNumber       view;
ThotBool           *ok;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbb;
   ThotBool            val;

   val = FALSE;
   *ok = TRUE;
   if (pPRule != NULL && pEl != NULL)
     {
	switch (pPRule->PrPresMode)
	      {
		 case PresInherit:
		    pAbb = AbsBoxInherit (pPRule, pEl, view);
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
		    if (pPRule->PrType == PtJustify ||
			pPRule->PrType == PtHyphenate ||
			pPRule->PrType == PtVertOverflow ||
			pPRule->PrType == PtHorizOverflow)
		       val = pPRule->PrJustify;
		    break;
	      }

     }
   return val;
}

/*----------------------------------------------------------------------
  GetEnclosingAttr
  If pAInit is an attribute of type attrNumber, return that attribute,
  otherwise return the attribute of type attrNumber attached to element
  pEl or one of its ancestors.
  Return NULL is neither pEl nor any of its ancestor have such an attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAttribute GetEnclosingAttr (PtrElement pEl, int attrNumber, PtrAttribute pAInit)
#else  /* __STDC__ */
static PtrAttribute GetEnclosingAttr (pEl, attrNumber, pAInit)
PtrElement	pEl;
int		attrNumber;
PtrAttribute    pAInit;
#endif /* __STDC__ */
{
   PtrAttribute        pAttr, pA;
   PtrElement	       pAsc;

   pAttr = NULL;
   if (attrNumber < 0)
      attrNumber = -attrNumber;
   if (pAInit != NULL &&
       pAInit->AeAttrNum == attrNumber &&
       pAInit->AeAttrSSchema == pEl->ElStructSchema)
      pAttr = pAInit;
   else
      {
      pAsc = pEl;
      while (pAttr == NULL && pAsc != NULL)
         {
         pA = pAsc->ElFirstAttr;
         while (pAttr == NULL && pA != NULL)
	    if (pA->AeAttrNum == attrNumber &&
	        pA->AeAttrSSchema == pEl->ElStructSchema)
	       pAttr = pA;
	    else
	       pA = pA->AeNext;
         pAsc = pAsc->ElParent;
         }
      }
   return pAttr;
}

/*----------------------------------------------------------------------
   	IntegerRule evalue une regle de presentation de type entier pour
   		la vue view. La regle a evaluer est pointee par pPRule,	
   		et l'element auquel elle s'applique est pointe par pEl.	
   		Au retour, ok indique si l'evaluation a pu etre faite et
   		unit indique, dans le cas de regle PtIndent, PtBreak1,	
   		PtBreak2 ou PtSize, si la valeur est exprimee en	
   		points typo, en relatif (numero de corps si PtSize),	
   		etc. Si la regle est une regle de presentation		
   		d'attribut, pAttr pointe sur le bloc d'attribut auquel	
   		la regle correspond.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 IntegerRule (PtrPRule pPRule, PtrElement pEl, DocViewNumber view, ThotBool * ok, TypeUnit * unit, PtrAttribute pAttr)
#else  /* __STDC__ */
int                 IntegerRule (pPRule, pEl, view, ok, unit, pAttr)
PtrPRule            pPRule;
PtrElement          pEl;
DocViewNumber       view;
ThotBool           *ok;
TypeUnit           *unit;
PtrAttribute        pAttr;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAbb;
   PtrElement          pElInherit;
   int                 val, i;

   val = 0;
   *ok = TRUE;
   *unit = UnRelative;
   if (pPRule != NULL && pEl != NULL)
     {
     switch (pPRule->PrPresMode)
        {
        case PresInherit:
           if (pPRule->PrType == PtVisibility)
              pAbb = AbsBoxInheritImm (pPRule, pEl, view);
           else
              pAbb = AbsBoxInherit (pPRule, pEl, view);
           if (pAbb == NULL)
              *ok = FALSE;
           else
             {
                if (pPRule->PrInhAttr)
                  {
		     /* c'est la valeur d'un attribut */
                     pAttr = GetEnclosingAttr (pEl, pPRule->PrInhDelta, pAttr);
                     if (pPRule->PrInhDelta < 0 && !pPRule->PrInhPercent)
                        /* il faut retrancher cette valeur */
                        i = -AttrValue (pAttr);
                     else
                        /* il faut ajouter cette valeur */
                        i = AttrValue (pAttr);
                     if (!pPRule->PrInhPercent)
                       if (pPRule->PrInhUnit == UnRelative ||
                           pPRule->PrInhUnit == UnXHeight)
                         if (pPRule->PrType == PtIndent ||
                             pPRule->PrType == PtLineSpacing ||
                             pPRule->PrType == PtLineWeight ||
			     pPRule->PrType == PtMarginTop ||
			     pPRule->PrType == PtMarginRight ||
			     pPRule->PrType == PtMarginBottom ||
			     pPRule->PrType == PtMarginLeft ||
			     pPRule->PrType == PtPaddingTop ||
			     pPRule->PrType == PtPaddingRight ||
			     pPRule->PrType == PtPaddingBottom ||
			     pPRule->PrType == PtPaddingLeft ||
			     pPRule->PrType == PtBorderTopWidth ||
			     pPRule->PrType == PtBorderRightWidth ||
			     pPRule->PrType == PtBorderBottomWidth ||
			     pPRule->PrType == PtBorderLeftWidth )
                           /* convertit en 1/10 de caractere */
                           i = 10 * i;
                  }
                else
                   /* c'est la valeur elle meme qui est dans la regle */
                   i = pPRule->PrInhDelta;

                switch (pPRule->PrType)
                   {
                   case PtVisibility:
                      if (pPRule->PrInhPercent)
                         val = (pAbb->AbVisibility * i) / 100;
                      else
                         val = pAbb->AbVisibility + i;
                      break;
                   case PtSize:
                      if (pPRule->PrInhPercent)
                         val = (pAbb->AbSize * i) / 100;
                      else
                         val = pAbb->AbSize + i;
                      *unit = pAbb->AbSizeUnit;
                      if (*unit == UnRelative)
                         if (val > MAX_LOG_SIZE)
                            val = MAX_LOG_SIZE;
                         else if (val < 0)
                            val = 0;
                      break;
                   case PtIndent:
                      if (pPRule->PrInhPercent)
                         val = (pAbb->AbIndent * i) / 100;
                      else
                         val = pAbb->AbIndent + i;
                      *unit = pAbb->AbIndentUnit;
                      break;
                   case PtLineSpacing:
                      if (pPRule->PrInhPercent)
                         val = (pAbb->AbLineSpacing * i) / 100;
                      else
                         val = pAbb->AbLineSpacing + i;
                      *unit = pAbb->AbLineSpacingUnit;
                      break;
                   case PtDepth:
                      if (pPRule->PrInhPercent)
                         val = (pAbb->AbDepth * i) / 100;
                      else
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
                      if (pPRule->PrInhPercent)
                         val = (pAbb->AbLineWeight * i) / 100;
                      else
                         val = pAbb->AbLineWeight + i;
                      if (val < 0)
                         val = 0;
                      *unit = pAbb->AbLineWeightUnit;
                      break;
                   case PtBorderTopColor:
                      val = pAbb->AbTopBColor;
                      break;
                   case PtBorderRightColor:
                      val = pAbb->AbRightBColor;
                      break;
                   case PtBorderBottomColor:
                      val = pAbb->AbBottomBColor;
                      break;
                   case PtBorderLeftColor:
                      val = pAbb->AbLeftBColor;
                      break;
                   case PtMarginTop:
                      val = pAbb->AbTopMargin;
                      *unit = pAbb->AbTopMarginUnit;
                      break;
                   case PtMarginRight:
                      val = pAbb->AbRightMargin;
                      *unit = pAbb->AbRightMarginUnit;
                      break;
                   case PtMarginBottom:
                      val = pAbb->AbBottomMargin;
                      *unit = pAbb->AbBottomMarginUnit;
                      break;
                   case PtMarginLeft:
                      val = pAbb->AbLeftMargin;
                      *unit = pAbb->AbLeftMarginUnit;
                      break;
                   case PtPaddingTop:
                      val = pAbb->AbTopPadding;
                      *unit = pAbb->AbTopPaddingUnit;
                      break;
                   case PtPaddingRight:
                      val = pAbb->AbRightPadding;
                      *unit = pAbb->AbRightPaddingUnit;
                      break;
                   case PtPaddingBottom:
                      val = pAbb->AbBottomPadding;
                      *unit = pAbb->AbBottomPaddingUnit;
                      break;
                   case PtPaddingLeft:
                      val = pAbb->AbLeftPadding;
                      *unit = pAbb->AbLeftPaddingUnit;
                      break;
                   case PtBorderTopWidth:
                      val = pAbb->AbTopBorder;
                      *unit = pAbb->AbTopBorderUnit;
                      break;
                   case PtBorderRightWidth:
                      val = pAbb->AbRightBorder;
                      *unit = pAbb->AbRightBorderUnit;
                      break;
                   case PtBorderBottomWidth:
                      val = pAbb->AbBottomBorder;
                      *unit = pAbb->AbBottomBorderUnit;
                      break;
                   case PtBorderLeftWidth:
                      val = pAbb->AbLeftBorder;
                      *unit = pAbb->AbLeftBorderUnit;
                      break;
                   default:
                      break;
                   }

                if (pPRule->PrInhMinOrMax != 0 && !pPRule->PrInhPercent)
                   /* il y a un minimum ou un maximum a respecter */
                  {
                     if (pPRule->PrMinMaxAttr)
                        /* c'est la valeur d'un attribut */
                       {
                          pAttr = GetEnclosingAttr (pEl, pPRule->PrInhMinOrMax,
						    pAttr);
                          if (pPRule->PrInhMinOrMax < 0)
                             /* inverser cette valeur */
                             i = -AttrValue (pAttr);
                          else
                             i = AttrValue (pAttr);
                          if (pPRule->PrInhUnit == UnRelative ||
                              pPRule->PrInhUnit == UnXHeight)
                             if (pPRule->PrType == PtIndent ||
                                 pPRule->PrType == PtLineSpacing ||
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
           if (pPRule->PrType == PtVisibility ||
               pPRule->PrType == PtDepth ||
               pPRule->PrType == PtFillPattern ||
               pPRule->PrType == PtBackground ||
               pPRule->PrType == PtForeground ||
	       pPRule->PrType == PtBorderTopColor ||
	       pPRule->PrType == PtBorderRightColor ||
	       pPRule->PrType == PtBorderBottomColor ||
	       pPRule->PrType == PtBorderLeftColor)
              if (pPRule->PrAttrValue)
                 /* c'est la valeur d'un attribut */
                 {
                 pAttr = GetEnclosingAttr (pEl, pPRule->PrIntValue, pAttr);
                 if (pPRule->PrIntValue < 0)
                    /* il faut inverser cette valeur */
                    val = -AttrValue (pAttr);
                 else
                    val = AttrValue (pAttr);
                 }
              else
                 /* c'est la valeur elle meme qui est dans la regle */
                 val = pPRule->PrIntValue;

           else if (pPRule->PrType == PtBreak1 ||
                    pPRule->PrType == PtBreak2 ||
                    pPRule->PrType == PtIndent ||
                    pPRule->PrType == PtSize ||
                    pPRule->PrType == PtLineSpacing ||
                    pPRule->PrType == PtLineWeight ||
		    pPRule->PrType == PtMarginTop ||
		    pPRule->PrType == PtMarginRight ||
		    pPRule->PrType == PtMarginBottom ||
		    pPRule->PrType == PtMarginLeft ||
		    pPRule->PrType == PtPaddingTop ||
		    pPRule->PrType == PtPaddingRight ||
		    pPRule->PrType == PtPaddingBottom ||
		    pPRule->PrType == PtPaddingLeft ||
		    pPRule->PrType == PtBorderTopWidth ||
		    pPRule->PrType == PtBorderRightWidth ||
		    pPRule->PrType == PtBorderBottomWidth ||
		    pPRule->PrType == PtBorderLeftWidth)
             {
                if (pPRule->PrMinAttr)
                   /* c'est la valeur d'un attribut */
                  {
                     pAttr = GetEnclosingAttr (pEl, pPRule->PrMinValue, pAttr);
                     if (pPRule->PrMinValue < 0)
                        /* il faut inverser cette valeur */
                        val = -AttrValue (pAttr);
                     else
                        val = AttrValue (pAttr);
                     if (pPRule->PrMinUnit == UnRelative
                         || pPRule->PrMinUnit == UnXHeight)
                        if (pPRule->PrType != PtSize)
                           /* convertit en 1/10 de caractere */
                           val = val * 10;
                  }
                else
                   /* c'est la valeur elle-meme qui est dans la regle */
                   val = pPRule->PrMinValue;

                if (pPRule->PrMinUnit == UnPercent &&
		    (pPRule->PrType == PtBreak1 ||
		     pPRule->PrType == PtBreak2 ||
		     pPRule->PrType == PtSize ||
		     pPRule->PrType == PtLineSpacing ||
		     pPRule->PrType == PtLineWeight))
                  {
                     if (pPRule->PrType == PtSize)
		       /* font-size is relative to the parent's font-size */
                       {
			 /* get the parent abstract box */
                          AncestorAbsBox (pEl, view, &pAbb, &pElInherit);
                          if (pAbb == NULL)
			     /* no parent ??? */
                             *ok = FALSE;
                          else
			     /* compute the font-size */
                             {
                             val = (pAbb->AbSize * val) / 100;
                             *unit = pAbb->AbSizeUnit;
                             }
                       }
                     else
		       /* the value is relative to the font-size of the
			  abstract box itself (relative size) */
                       {
                          /* Relative a la police courante */
                          *unit = UnRelative;
                          val = val / 10;
                       }
                  }
                else
                  {
                     *unit = pPRule->PrMinUnit;
                     if (pPRule->PrType == PtSize &&
			 pPRule->PrMinUnit == UnRelative)
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

/*----------------------------------------------------------------------
   VerifyAbsBox : Teste si le pave pAb a les caracteristiques
   	numAbType (type du pave) et refKind (pave de presentation
   	ou d'element) si notType est faux ou s'il n'a pas les
	caracteristiques numAbType et refKind si notType est vrai.
   	Cela permet de determiner le pave pAb par rapport	
   	auquel le pave en cours de traitement va se positionner
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         VerifyAbsBox (ThotBool * found, PtrPSchema pSP, RefKind refKind, int numAbType, ThotBool notType, PtrAbstractBox pAb)

#else  /* __STDC__ */
static void         VerifyAbsBox (found, pSP, refKind, numAbType, notType, pAb)
ThotBool           *found;
PtrPSchema          pSP;
RefKind             refKind;
int                 numAbType;
ThotBool            notType;
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   PtrAttribute	    pAttr;
   ThotBool	    attrFound;

   if (!pAb->AbDead)
      {
      pAttr = NULL;
      if (refKind == RkAttr && pAb->AbElement != NULL)
	if (!pAb->AbPresentationBox)
	  {
	  pAttr = pAb->AbElement->ElFirstAttr;
	  attrFound = FALSE;
	  while (pAttr != NULL && !attrFound)
             if (pAttr->AeAttrSSchema->SsCode == pSP->PsStructCode &&
		 pAttr->AeAttrNum == numAbType)
                attrFound = TRUE;
             else
                pAttr = pAttr->AeNext;
	  }
      if (notType)
	/* on accepte le pave s'il est de type different de numAbType */
	{
	   if (refKind == RkElType)
	     /* un pave d'un element de structure */
	     {
	     if (numAbType == MAX_RULES_SSCHEMA + 1)
		{
		/* C'est une regle Not AnyElem, on accepte la premiere */
		/* boite de presentation trouvee */
		if (pAb->AbPresentationBox)
		   *found = TRUE;
		}
	     else
		/* c'est une regle Not Type */
		if (pAb->AbTypeNum != numAbType ||
		    pAb->AbPresentationBox ||
		    pAb->AbPSchema != pSP)
		   *found = TRUE;
	     }
	   else if (refKind == RkPresBox)
	     /* un pave d'une boite de pres. */
	     {
	     if (numAbType == MAX_PRES_BOX + 1)
		{
	        /* Cas d'une regle Not AnyBox, on accepte le premier */
	        /* element trouve' */
		if (!pAb->AbPresentationBox)
	           *found = TRUE;
		}
	     else
		/* c'est une regle Not Box */
		if (pAb->AbTypeNum != numAbType ||
		    !pAb->AbPresentationBox ||
		    pAb->AbPSchema != pSP)
		   *found = TRUE;
	     }
	   else if (refKind == RkAttr)
	     /* le pave d'un element portant un attribut */
	     {
	     if (pAttr == NULL)
		*found = TRUE;
	     }
	}
      else
	{
	   if (refKind == RkElType)
	     /* un pave d'un element de structure */
	     {
	     if (numAbType == MAX_RULES_SSCHEMA + 1)
		{
		/* C'est une regle AnyElem, on accepte le premier element
		   trouve' */
		if (!pAb->AbPresentationBox)
		   *found = TRUE;
		}
	     else
		/* C'est une regle Type */
		if (pAb->AbTypeNum == numAbType &&
		    !pAb->AbPresentationBox &&
		    (pAb->AbPSchema == pSP || pSP == NULL))
		   *found = TRUE;
	     }
	   else if (refKind == RkPresBox)
	     /* un pave d'une boite de presentation */
	     {
	     if (numAbType == MAX_PRES_BOX + 1)
		{
	        /* C'est une regle AnyBox, on accepte la premiere boite de */
	        /* presentation trouvee */
		if (pAb->AbPresentationBox)
	           *found = TRUE;
		}
	     else
		/* C'est une regle Box */
		if (pAb->AbTypeNum == numAbType &&
		    pAb->AbPresentationBox &&
		    (pAb->AbPSchema == pSP || pSP == NULL))
		   *found = TRUE;
	     }
	   else if (refKind == RkAttr)
	     {
	     if (pAttr != NULL)
		*found = TRUE;
	     }
	}
   }
}

/*----------------------------------------------------------------------
   	AttrCreatePresBox	retourne Vrai si l'une des regles de	
   		presentation de l'attribut pointe' par pAttr cree le	
   		pave de presentation pointe' par pAb.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     AttrCreatePresBox (PtrAttribute pAttr, PtrAbstractBox pAb)

#else  /* __STDC__ */
static ThotBool     AttrCreatePresBox (pAttr, pAb)
PtrAttribute        pAttr;
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   ThotBool            ret, stop;
   PtrPRule            pPRule;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;

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
	     pPRule = AttrPresRule (pAttr, pAb->AbElement, FALSE, NULL, pSchP);
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
			     /* cette regle cree notre pave, on a trouv' */
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

/*----------------------------------------------------------------------
   	VerifyAbsBoxDescent : Teste si le pave pAb ou un de ses		
   		descendants a les caracteristiques			
   		numAbType (type du pave) et refKind (pave de presentation
   		ou d'element) si notType est faux ou s'il n'a pas les	
   		caracteristiques numAbType et refKind si notType est vrai.
   		Cela permet de determiner le pave pAb par rapport	
   		auquel le pave en cours de traitement va se positionner	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         VerifyAbsBoxDescent (ThotBool * found, PtrPSchema pSP, RefKind refKind, int numType, ThotBool notType, PtrAbstractBox pAb)

#else  /* __STDC__ */
static void         VerifyAbsBoxDescent (found, pSP, refKind, numType, notType, pAb)
ThotBool           *found;
PtrPSchema          pSP;
RefKind		    refKind;
int                 numType;
ThotBool            notType;
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   VerifyAbsBox (found, pSP, refKind, numType, notType, pAb);
   if (!(*found))
      if (pAb->AbFirstEnclosed != NULL)
	{
	   pAb = pAb->AbFirstEnclosed;
	   do
	     {
		VerifyAbsBoxDescent (found, pSP, refKind, numType, notType, pAb);
		if (!(*found))
		   pAb = pAb->AbNext;
	     }
	   while (!(*found) && pAb != NULL);
	}
}


/*----------------------------------------------------------------------
   	SearchAbsBoxRef Si notType est faux, rend un pointeur sur le pave de	
   		type numAbType et de niveau level (relativement au pave	
   		pAbb). Si notType est vrai, rend un pointeur sur le	
   		premier pave de niveau level (relativement a pAbb) qui n'est
   		pas de type numAbType.					
   		Si refKind == RkElType, le pave represente par numAbType est
   		celui d'un element de la representation interne,
		si refKind == RkPresBox, c'est une boite de presentation
		definie dans le schema de presentation pointe' par pSP.				si refKind == RkAttr, c'est un numero d'attribut.
   		Si level vaut RlReferred, on cherche parmi les paves de	
   		l'element designe' par l'attribut reference pointe'	
   		par pAttr.						
   		Au result, la fonction rend NULL si aucun pave ne	
   		correspond.						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrAbstractBox SearchAbsBoxRef (ThotBool notType, int numAbType, PtrPSchema pSP, Level level, RefKind refKind, PtrAbstractBox pAbb, PtrAttribute pAttr)

#else  /* __STDC__ */
static PtrAbstractBox SearchAbsBoxRef (notType, numAbType, pSP, level, refKind, pAbb, pAttr)
ThotBool            notType;
int                 numAbType;
PtrPSchema          pSP;
Level               level;
RefKind		    refKind;
PtrAbstractBox      pAbb;
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   ThotBool            found;
   PtrAbstractBox      pAb;
   int                 view;
   PtrAbstractBox      pAbbMain;

   pAb = pAbb;
   if (pAb != NULL)
     {
	found = FALSE;
	switch (level)
	      {
		 case RlEnclosing:
		    do
		      {
			 pAb = pAb->AbEnclosing;
			 if (numAbType == 0)
			    found = TRUE;
			 else if (pAb != NULL)
			    VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
		      }
		    while (pAb != NULL && !found);
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
			    VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
			    if (!found)
			       pAb = pAb->AbNext;
			 }
		    while (pAb != NULL && !found);
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
			       VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
			       if (!found)
				  pAb = pAb->AbNext;
			    }
		       while (pAb != NULL && !found);
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
			       VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
			       if (!found)
				  pAb = pAb->AbPrevious;
			    }
		       while (pAb != NULL && !found);
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
			       VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
			       if (!found)
				  pAb = pAb->AbNext;
			    }
		       while (pAb != NULL && !found);
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
			    else if (pAb->AbElement == pAbb->AbElement)
			       found = TRUE;
			    else
			       pAb = NULL;
			 while (!found && pAb != NULL);
			 if (!found)
			    /* pas trouve', on cherche parmi les freres precedents */
			   {
			      pAb = pAbb;
			      do
				 if (pAb->AbPresentationBox)
				    pAb = pAb->AbPrevious;
				 else if (pAb->AbElement == pAbb->AbElement)
				    found = TRUE;
				 else
				    pAb = NULL;
			      while (!found && pAb != NULL);
			   }
		      }
		    if (pAb != NULL)
		       if (pAb->AbDead)
			  pAb = NULL;
		    break;

		 case RlReferred:
		    view = pAb->AbDocView;
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
				   if (AttrCreatePresBox (pAttr, pAbb))
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
					 ElAbstractBox[view - 1];
		    pAbbMain = NULL;
		    if (pAb != NULL)
		       /* cherche en avant le pave demande */
		       do
			  if (numAbType == 0)
			     if (pAb->AbDead)
				/* ce pave est en cours de destruction */
				if (pAb->AbNext == NULL)
				   pAb = NULL;
				else if (pAb->AbNext->AbElement == pAb->AbElement)
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
			       VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
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
		       while (pAb != NULL && !found);
		    if (pAb == NULL && pAbbMain != NULL && numAbType != 0)
		       /* on cherche parmi les paves descendants du pave principal */
		      {
			 pAb = pAbbMain->AbFirstEnclosed;
			 while (pAb != NULL && !found)
			   {
			      VerifyAbsBoxDescent (&found, pSP, refKind, numAbType, notType, pAb);
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



/*----------------------------------------------------------------------
   	GetConstantBuffer   acquiert un buffer de texte pour la constante de	
   		presentation correspondant au pave pointe par pAb.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                GetConstantBuffer (PtrAbstractBox pAb)

#else  /* __STDC__ */
void                GetConstantBuffer (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBT;


   GetTextBuffer (&pBT);
   pAb->AbText = pBT;
   if (pAb->AbLeafType == LtText)
     pAb->AbLanguage = TtaGetDefaultLanguage ();
   pAb->AbVolume = 0;
}



/*----------------------------------------------------------------------
   	UpdateFreeVol	met a jour le volume libre restant dans la vue	
   		du pave pAb, en prenant en compte le volume de ce	
   		nouveau pave feuille.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UpdateFreeVol (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                UpdateFreeVol (pAb, pDoc)
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{


   if (!AssocView (pAb->AbElement))
      /* vue de l'arbre principal */
      pDoc->DocViewFreeVolume[pAb->AbDocView - 1] -= pAb->AbVolume;
   else
      /* vue d'elements associes */
      pDoc->DocAssocFreeVolume[pAb->AbElement->ElAssocNum - 1] -= pAb->AbVolume;
}



/*----------------------------------------------------------------------
   	FillContent met dans le pave pointe par pAb le contenu de l'element
   		feuille pointe par pEl.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FillContent (PtrElement pEl, PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                FillContent (pEl, pAb, pDoc)
PtrElement          pEl;
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 lg, i;
   PtrTextBuffer       pBu1;
   PtrReference        pPR1;
   PtrReferredDescr    pDe1;

   if (pEl->ElHolophrast)
     {
	pAb->AbLeafType = LtText;
	GetConstantBuffer (pAb);
	pBu1 = pAb->AbText;
	CopyStringToText (TEXT("<"), pBu1, &lg);
	CopyStringToText (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
			  pBu1, &i);
	lg += i;
	CopyStringToText (TEXT(">"), pBu1, &i);
	lg += i;
	pAb->AbVolume = lg;
	pAb->AbCanBeModified = FALSE;
	pAb->AbSensitive = TRUE;
	/* met a jour le volume libre restant dans la vue */
	UpdateFreeVol (pAb, pDoc);
     }
   else if (pEl->ElTerminal)
     {
	pAb->AbLeafType = pEl->ElLeafType;
	switch (pEl->ElLeafType)
	      {
		 case LtPicture:
		    /* saute les paves crees par FnCreateBefore */
		    /* while (pAb->AbText != NULL && pAb->AbNext != NULL)
		       pAb = pAb->AbNext; */
		    NewPictInfo (pAb, pEl->ElText->BuContent, UNKNOWN_FORMAT);
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
		 case LtPolyLine:
		    /* prend le contenu de l'element correspondant */
		    pAb->AbPolyLineBuffer = pEl->ElPolyLineBuffer;
		    pAb->AbVolume = pEl->ElNPoints;
		    pAb->AbPolyLineShape = pEl->ElPolyLineType;
		    break;
		 case LtSymbol:
		 case LtGraphics:
		    pAb->AbShape = pEl->ElGraph;
		    pAb->AbGraphAlphabet = 'G';
		    if (pAb->AbShape == EOS)
		       pAb->AbVolume = 0;
		    else
		       pAb->AbVolume = 1;
		    break;
		 case LtReference:
		    pAb->AbLeafType = LtText;
		    GetConstantBuffer (pAb);
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
				    if (!IsASavedElement (pPR1->RdReferred->ReReferredElem))
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
				      while (pDe1->ReExtDocument[i - 1] != EOS)
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
		    pBu1->BuContent[lg] = EOS;
		    /* fin de la chaine de car. */
		    pBu1->BuLength = lg;
		    pAb->AbVolume = lg;
		    pAb->AbCanBeModified = FALSE;
		    break;
		 case LtPairedElem:
		    pAb->AbLeafType = LtText;
		    GetConstantBuffer (pAb);
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
		    pBu1->BuContent[2] = EOS;
		    /* fin de la chaine de car. */
		    pBu1->BuLength = 2;
		    pAb->AbVolume = 2;
		    pAb->AbCanBeModified = FALSE;
		    break;
		 default:
		    break;
	      }
	/* met a jour le volume libre restant dans la vue */
	UpdateFreeVol (pAb, pDoc);
     }
}


/*----------------------------------------------------------------------
   	PageCreateRule	cherche dans la chaine de regles de presentation
   	qui commence par pPRule et qui appartient au schema de		
   	presentation pointe' par pSPR, la regle de creation qui		
   	engendre le pave pCree.						
   	Si cette regle est trouvee, retourne TRUE et TypeCreation	
   	contient le type de cette regle,				
   	Sinon, retourne FALSE.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     PageCreateRule (PtrPRule pPRule, PtrPSchema pSPR, PtrAbstractBox pCree, FunctionType * TypeCreation)
#else  /* __STDC__ */
static ThotBool     PageCreateRule (pPRule, pSPR, pCree, TypeCreation)
PtrPRule            pPRule;
PtrPSchema          pSPR;
PtrAbstractBox      pCree;
FunctionType       *TypeCreation;

#endif /* __STDC__ */
{
   ThotBool            stop;
   ThotBool            result;

   result = FALSE;
   stop = FALSE;
   if (pCree != NULL)
      do
	 if (pPRule == NULL)
	    /* il n' y a plus de regles dans la chaine de regles */
	    stop = TRUE;
	 else if (pPRule->PrType > PtFunction)
	    /* il n'y a plus de regle fonction de presentation dans la chaine */
	    stop = TRUE;
	 else if (pPRule->PrType < PtFunction)
	    /* on n'a pas encore vu les Fonctions */
	    pPRule = pPRule->PrNextPRule;
	 else
	   {
	      /* la regle pPRule est une fonction de presentation */
	      if ((pPRule->PrPresFunction == FnCreateBefore
		   || pPRule->PrPresFunction == FnCreateWith
		   || pPRule->PrPresFunction == FnCreateAfter
		   || pPRule->PrPresFunction == FnCreateEnclosing
		   || pPRule->PrPresFunction == FnCreateFirst
		   || pPRule->PrPresFunction == FnCreateLast)
		  && pCree->AbTypeNum == pPRule->PrPresBox[0]
		  && pCree->AbPSchema == pSPR)
		 /* c'est une regle de creation et elle cree ce type de */
		 /* boite */
		{
		   stop = TRUE;
		   result = TRUE;
		   *TypeCreation = pPRule->PrPresFunction;
		}
	      else
		 /* passe a la regle suivante de la chaine */
		 pPRule = pPRule->PrNextPRule;
	   }
      while (!stop);
   return result;
}


/*----------------------------------------------------------------------
  TypeCreatedRule retourne le type de la regle de presentation appelee	
  par le pave pAbbCreator et qui a cree le pave pAbbCreated.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
FunctionType        TypeCreatedRule (PtrDocument pDoc, PtrAbstractBox pAbbCreator, PtrAbstractBox pAbbCreated)
#else  /* __STDC__ */
FunctionType        TypeCreatedRule (pDoc, pAbbCreator, pAbbCreated)
PtrDocument         pDoc;
PtrAbstractBox      pAbbCreator;
PtrAbstractBox      pAbbCreated;
#endif /* __STDC__ */

{
   PtrPRule            pPRuleCre;
   FunctionType        result;
   PtrPSchema          pSPR;
   PtrAttribute        pAttr;
   PtrAttribute        pA;
   ThotBool            ok;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;

   result = FnLine;
   /* cherche les regles de creation en ignorant les attributs */
   pPRuleCre = SearchRulepAb (pDoc, pAbbCreator, &pSPR, PtFunction, FnAny, FALSE, &pAttr);
   if (!PageCreateRule (pPRuleCre, pSPR, pAbbCreated, &result))
      /* on n'a pas found la regle qui cree la bonne boite */
      /* on cherche les regles de creation associees aux attributs */
      /* de l'element createur */
     {
	ok = FALSE;
	pA = pAbbCreator->AbElement->ElFirstAttr;
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
		  pPRuleCre = AttrPresRule (pA, pAbbCreator->AbElement, FALSE, NULL, pSchP);
		  ok = PageCreateRule (pPRuleCre, pSchP, pAbbCreated, &result);
		  if (pHd == NULL)
		     /* on n'a pas encore traite' les schemas de presentation additionnels
		        On prend le premier schema additionnel. */
		     pHd = pA->AeAttrSSchema->SsFirstPSchemaExtens;
		  else
		     /* passe au schema additionnel suivant */
		     pHd = pHd->HdNextPSchema;
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
   return result;
}



/*----------------------------------------------------------------------
   	SearchPresSchema	cherche le schema de presentation a appliquer a	
   		l'element pointe par pEl. Retourne dans pSchP un	
   		pointeur sur ce schema, dans indexElType le numero de	
   		l'entree correspondant a l'element dans ce schema et	
   		dans pSchS un pointeur sur le schema de structure auquel
   		correspond le schema de presentation retourne'.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SearchPresSchema (PtrElement pEl, PtrPSchema * pSchP, int *indexElType, PtrSSchema * pSchS)
#else  /* __STDC__ */
void                SearchPresSchema (pEl, pSchP, indexElType, pSchS)
PtrElement          pEl;
PtrPSchema         *pSchP;
int                *indexElType;
PtrSSchema         *pSchS;

#endif /* __STDC__ */
{
   ThotBool            found;
   int                 i;
   PtrSSchema          pSc1;
   SRule              *pSRule;

   if (pEl == NULL || pEl->ElStructSchema == NULL)
     {
	*pSchP = NULL;
	*pSchS = NULL;
	*indexElType = 0;
     }
   else
     {
	*pSchP = pEl->ElStructSchema->SsPSchema;
	*pSchS = pEl->ElStructSchema;
	/* premiere regle de presentation specifique a ce type d'element */
	*indexElType = pEl->ElTypeNumber;
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
		if (pSc1 != NULL)
		  {
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
		   while (!found && i < pSc1->SsNRules);
		   if (found)
		      if (pSc1->SsPSchema->PsElemPRule[i - 1] != NULL)
			 /* il y a des regles de presentation specifiques */
			{
			   *pSchP = pSc1->SsPSchema;
			   *indexElType = i;
			   *pSchS = pEl->ElParent->ElStructSchema;
			}
		  }
		}
     }
}

/*----------------------------------------------------------------------
   	CheckPPosUser							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CheckPPosUser (PtrAbstractBox pAb, PtrDocument pDoc)
#else  /* __STDC__ */
static ThotBool     CheckPPosUser (pAb, pDoc)
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 frame;
   ThotBool            result;

   if (AssocView (pAb->AbElement))
      frame = pDoc->DocAssocFrame[pAb->AbElement->ElAssocNum - 1];
   else
      frame = pDoc->DocViewFrame[pAb->AbDocView - 1];
   result = IsAbstractBoxDisplayed (pAb, frame);
   return result;
}


/*----------------------------------------------------------------------
   	ApplyPos 	applique la regle de position PR-pPRule 	
   		au pave pAbb1. 						
   		rend vrai dans appl si la regle a ete appliquee.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyPos (AbPosition * PPos, PosRule *positionRule, PtrPRule pPRule, PtrAttribute pAttr, PtrPSchema pSchP, PtrAbstractBox pAbb1, PtrDocument pDoc, ThotBool * appl)
#else  /* __STDC__ */
static void         ApplyPos (PPos, positionRule, pPRule, pAttr, pSchP, pAbb1, pDoc, appl)
AbPosition         *PPos;
PosRule            *positionRule;
PtrPRule            pPRule;
PtrAttribute        pAttr;
PtrPSchema          pSchP;
PtrAbstractBox      pAbb1;
PtrDocument         pDoc;
ThotBool           *appl;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAbbPos;
   ThotBool            pageBreak;
   PtrAbstractBox      pAbbParent;
   PosRule            *pPosRule;
   PtrPRule            pRSpec;
   PtrPSchema          pSchPPage;
   int                 b, PageHeaderHeight;

   *appl = FALSE;
   /* on n'a pas (encore) applique' la regle */
   pageBreak = FALSE;
   /* le pave est-il une marque de saut de page (le trait horizontal */
   /* qui separe les pages dans les images) ? Dans ce cas, il aura */
   /* une position flottante */
   if (pAbb1->AbElement->ElTerminal && pAbb1->AbElement->ElLeafType == LtPageColBreak)
      if (pAbb1->AbLeafType != LtCompound && !pAbb1->AbPresentationBox)
	 pageBreak = TRUE;
   pPosRule = positionRule;
   if (pPosRule->PoPosDef == NoEdge || pageBreak)
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
	pAbbPos = NULL;
	if (pPosRule->PoRefKind == RkElType)
	   /* appelle l'exception des tableaux, au cas ou ce serait la */
	   /* regle de hauteur d'un filet vertical d'un tableau */
	   if (ThotLocalActions[T_abref] != NULL)
	      (*ThotLocalActions[T_abref]) (pAbb1, pPosRule, pPRule, &pAbbPos);
	      /* si l'exception n'a pas ete traitee, effectue un traitement
		 normal */
	if (pAbbPos == NULL)
	   pAbbPos = SearchAbsBoxRef (pPosRule->PoNotRel,
			  pPosRule->PoRefIdent, pSchP, pPosRule->PoRelation,
			  pPosRule->PoRefKind, pAbb1, pAttr);
	if (pAbbPos != NULL)
	  {
	     /* on a trouve' le pave de reference */
	     PPos->PosEdge = pPosRule->PoPosDef;
	     PPos->PosRefEdge = pPosRule->PoPosRef;
	     if (pPosRule->PoDistAttr)
		/* c'est la valeur d'un attribut */
	       {
		  if (pPosRule->PoDistance < 0)
		     /* il faut inverser cette valeur */
		     PPos->PosDistance = -AttrValue (pAttr);
		  else
		     PPos->PosDistance = AttrValue (pAttr);
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
		PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
	     *appl = TRUE;
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
		     /* regarde si le premier fils de l'englobant est un saut de page */
		     pAbbParent = pAbb1->AbEnclosing;
		     if (pAbbParent->AbFirstEnclosed != pAbb1
			 && !pAbbParent->AbFirstEnclosed->AbDead
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
			       b = GetPageBoxType (pAbbParent->AbFirstEnclosed->AbElement,
					      pAbbParent->AbFirstEnclosed->AbElement->ElViewPSchema,
					      &pSchPPage);
			       PageHeaderHeight = pSchPPage->PsPresentBox[b - 1].PbHeaderHeight;
			       /* PbHeaderHeight toujours en points typo */
			       if (PPos->PosDistance - PageHeaderHeight >= 0)
				  PPos->PosDistance = PPos->PosDistance - PageHeaderHeight;
			    }
			  PPos->PosUserSpecified = FALSE;
		       }
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
	   /* on n'a pas trouve' le pave' de reference */
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
		PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
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
		     PPos->PosDistance = -AttrValue (pAttr);
		  else
		     PPos->PosDistance = AttrValue (pAttr);
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
		PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
	     PPos->PosUnit = pPosRule->PoDistUnit;
	     PPos->PosAbRef = NULL;
	     *appl = TRUE;
	  }
	else
	   /* on n'a pas trouv' le pave' de reference */
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
	      precedent et on n'a pas trouve' le precedent. On remplace par
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
		PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
	     pAbb1->AbVertEnclosing = TRUE;
	     }
           else
             /* position flottante, equivalente a une regle VertPos=NULL ou
                HorizPos=NULL */
             {
	     PPos->PosEdge = NoEdge;
             PPos->PosRefEdge = NoEdge;
             PPos->PosDistance = 0;
             PPos->PosUnit = UnRelative;
             PPos->PosAbRef = NULL;
             PPos->PosUserSpecified = FALSE;
	     *appl = FALSE;
             }
     }
}



/*----------------------------------------------------------------------
   	ApplyDim	 applique au pave pointe' par pAb la regle	
   		de dimension pointee par pPRule.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyDim (AbDimension * pdimAb, PtrAbstractBox pAb, PtrPSchema pSchP, PtrAttribute pAttr, ThotBool * appl, PtrPRule pPRule, PtrDocument pDoc)
#else  /* __STDC__ */
static void         ApplyDim (pdimAb, pAb, pSchP, pAttr, appl, pPRule, pDoc)
AbDimension        *pdimAb;
PtrAbstractBox      pAb;
PtrPSchema          pSchP;
PtrAttribute        pAttr;
ThotBool           *appl;
PtrPRule            pPRule;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAbbRef;
   PtrAttribute        pA;
   ThotBool            stop;
   DimensionRule      *pDRule;
   int                 attrRule;

   /* on met a priori les valeurs correspondant a une dimension */
   /* egale a celle du contenu. */
   pdimAb->DimIsPosition = FALSE;
   pdimAb->DimValue = 0;
   pdimAb->DimAbRef = NULL;
   pdimAb->DimUnit = pPRule->PrDimRule.DrUnit;
   pdimAb->DimSameDimension = TRUE;
   pdimAb->DimMinimum = pPRule->PrDimRule.DrMin;
   if (FirstCreation)
      pdimAb->DimUserSpecified = pPRule->PrDimRule.DrUserSpecified;
   else
      pdimAb->DimUserSpecified = FALSE;
   if (pdimAb->DimUserSpecified)
      pdimAb->DimUserSpecified = CheckPPosUser (pAb, pDoc);

   *appl = FALSE;
   pDRule = &pPRule->PrDimRule;
   if (pDRule->DrPosition)
     {
	/* Box elastique, la dimension est definie comme une position */
	/* applique la regle */
	ApplyPos (&(pdimAb->DimPosition), &(pDRule->DrPosRule), pPRule, pAttr, pSchP, pAb, pDoc, appl);
	/* si la regle a pu etre appliquee, le boite est reellement elastique */
	if (*appl)
	   pdimAb->DimIsPosition = TRUE;
     }
   else if (pDRule->DrAbsolute)
     {
	/* valeur absolue */
	if (pDRule->DrAttr)
	   /* c'est la valeur d'un attribut */
	  {
	     if (pDRule->DrValue < 0)
		/* il faut inverser cette valeur */
		pdimAb->DimValue = -AttrValue (pAttr);
	     else
		pdimAb->DimValue = AttrValue (pAttr);
	     if (pDRule->DrUnit == UnRelative ||
		 pDRule->DrUnit == UnXHeight)
		/* convertit en 1/10 de caractere */
		pdimAb->DimValue = 10 * pdimAb->DimValue;
	  }
	else
	   /* c'est la valeur elle meme qui est dans la regle */
	   pdimAb->DimValue = pDRule->DrValue;
	*appl = TRUE;
     }
   else if (pDRule->DrRelation == RlEnclosed)
     {
	/* dimension du contenu */
	*appl = TRUE;
	/* les valeurs mises a priori conviennent */
	pdimAb->DimValue = pDRule->DrValue;
     }
   else
     {
	/* dimensions relatives a l'englobant ou un frere */
	pdimAb->DimSameDimension = pDRule->DrSameDimens;
	/* essaie d'appliquer la regle de dimensionnement relatif */
	pAbbRef = SearchAbsBoxRef (pDRule->DrNotRelat, pDRule->DrRefIdent, pSchP,
		      pDRule->DrRelation, pDRule->DrRefKind, pAb, pAttr);
	pdimAb->DimAbRef = pAbbRef;
	if (pAbbRef == NULL && pAb->AbElement != NULL)
	   if (pAb->AbEnclosing == NULL && pDRule->DrRelation == RlEnclosing)
	      /* heritage des dimensions de la fenetre */
	     {
		if (pDRule->DrValue == 0)
		  {
		     pdimAb->DimValue = 100;
		     pdimAb->DimUnit = UnPercent;
		  }
		else
		  {
		     pdimAb->DimUnit = pDRule->DrUnit;
		     if (pDRule->DrAttr)
			/* c'est la valeur d'un attribut */
		       {
			  if (pDRule->DrValue < 0)
			     /* inverser cette valeur */
			     pdimAb->DimValue = -AttrValue (pAttr);
			  else
			     pdimAb->DimValue = AttrValue (pAttr);
			  if (pDRule->DrUnit == UnRelative ||
			      pDRule->DrUnit == UnXHeight)
			     /* convertit en 1/10 de caractere */
			     pdimAb->DimValue = 10 * pdimAb->DimValue;
		       }
		     else
			/* c'est la valeur elle meme */
			pdimAb->DimValue = pDRule->DrValue;
		  }
		*appl = TRUE;
	     }
	if (pAbbRef != NULL)
	  {
	     if (pDRule->DrAttr)
		/* c'est la valeur d'un attribut */
	       {
		  pA = pAttr;
		  attrRule = pDRule->DrValue;
		  if (pDRule->DrValue < 0)
		     attrRule = -attrRule;

		  /* l'attribut est-il celui de la regle ? */
		  if (pAttr->AeAttrNum != attrRule
		      || pAttr->AeAttrSSchema->SsPSchema != pSchP)
		     /* ce n'est pas l'attribut indique' dans la regle, */
		     /* cherche si l'elem. possede l'attribut de la regle */
		    {
		       pA = pAb->AbElement->ElFirstAttr;
		       stop = FALSE;
		       do
			  if (pA == NULL)
			     stop = TRUE;	/* dernier attribut de l'element */
			  else if (pA->AeAttrNum == attrRule
				   && pA->AeAttrSSchema->SsPSchema == pSchP)
			     stop = TRUE;	/* c'est l'attribut cherche' */
			  else
			     pA = pA->AeNext;
		       while (!stop);
		    }
		  pdimAb->DimValue = AttrValue (pA);
		  if (pDRule->DrValue < 0)
		     /* inverser cette valeur */
		     pdimAb->DimValue = -pdimAb->DimValue;
		  if (pDRule->DrUnit == UnRelative ||
		      pDRule->DrUnit == UnXHeight)
		     /* convertit en 1/10 de caractere */
		     pdimAb->DimValue = 10 * pdimAb->DimValue;
	       }
	     else
		/* c'est la valeur elle-meme */
		pdimAb->DimValue = pDRule->DrValue;
	     pdimAb->DimUnit = pDRule->DrUnit;
	     *appl = TRUE;
	  }
     }
}


/*----------------------------------------------------------------------
   	ApplyPage 	applique une regle Page				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplyPage (PtrDocument pDoc, PtrAbstractBox pAb, int viewSch, PtrPRule pPRule, FunctionType pageType)

#else  /* __STDC__ */
static void         ApplyPage (pDoc, pAb, viewSch, pPRule, pageType)
PtrDocument         pDoc;
PtrAbstractBox      pAb;
int                 viewSch;
PtrPRule            pPRule;
FunctionType        pageType;
#endif /* __STDC__ */
{
   PtrElement          pElPage, pEl, pElChild, pPrec;
   PtrAbstractBox      pP;
   ThotBool            exitingPage, stop;
   int                 counter;
   PtrPSchema          pSchP;
   PtrElement          pEl1;
   ThotBool            complete;
   ThotBool            cree;

   if (pPRule->PrViewNum == viewSch)
      /* la regle Page concerne la vue du pave traite' */
     {
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
		       {
			  pElPage = pEl1;
			  exitingPage = TRUE;
			  stop = TRUE;
		       }
		     else
			/* on saute les eventuelles marque page d'autres vues ? */
			pElChild = pEl1->ElNext;
		  }
	     while (!stop);
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
	     pElPage = NewSubtree (PageBreak + 1, pEl->ElStructSchema, pDoc,
				   pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	     if (pElPage != NULL)
		/* on a cree une marque de page */

		/* le reste de la procedure est different entre les deux versions */
		/* on la chaine comme premier fils de l'element */
	       {
		  InsertFirstChild (pEl, pElPage);
		  /* on l'initialise */
		  pEl1 = pElPage;
		  pEl1->ElPageType = PgBegin;
		  pEl1->ElViewPSchema = viewSch;
		  /* cherche le compteur de pages a appliquer */
		  counter = GetPageCounter (pElPage, pEl1->ElViewPSchema, &pSchP);
		  if (counter > 0)
		     /* calcule la valeur du compteur de pages */
		     pEl1->ElPageNumber = CounterVal (counter, pElPage->ElStructSchema, pSchP, pElPage,
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
		     pP = AbsBoxesCreate (pElPage, pDoc, pAb->AbDocView, TRUE, TRUE, &complete);
		  /* on met a jour les numeros des pages suivantes */
		  UpdateNumbers (NextElement (pElPage), pElPage, pDoc, TRUE);
	       }
	  }

	/* TODO quand on traitera les regles recto verso */
	/* on verifie que la cardinalite de la page est en accord avec la regle
	   appliquee */
     }
}


/*----------------------------------------------------------------------
   	FindAbsBox cherche dans le sous-arbre (racine comprise)		
   		du pave pointe' par pAb un pave de presentation du type	
   		NType defini dans le schema de presentation pointe' par	
   		pSchP. Si NType=0 on cherche un pave de presentation de	
   		nom presBoxName et on ignore pSchP.				
   		Retourne Vrai si un tel pave existe, et dans ce cas pAb	
   		pointe sur le pave trouve'				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     FindAbsBox (int Ntype, PtrPSchema pSchP, Name presBoxName, PtrAbstractBox * pAb)

#else  /* __STDC__ */
static ThotBool     FindAbsBox (Ntype, pSchP, presBoxName, pAb)
int                 Ntype;
PtrPSchema          pSchP;
Name                presBoxName;
PtrAbstractBox     *pAb;

#endif /* __STDC__ */

{
   ThotBool            result;
   PtrAbstractBox      pAbbCur;

   result = FALSE;
   if ((*pAb)->AbPresentationBox)
      if ((*pAb)->AbLeafType == LtText)
	 if (Ntype != 0)
	    result = ustrcmp ((*pAb)->AbPSchema->PsPresentBox[(*pAb)->AbTypeNum - 1].PbName, pSchP->PsPresentBox[Ntype - 1].PbName) == 0;
	 else
	    result = ustrcmp ((*pAb)->AbPSchema->PsPresentBox[(*pAb)->AbTypeNum - 1].PbName, presBoxName) == 0;
   if (!result)
      if ((*pAb)->AbFirstEnclosed == NULL)
	 result = FALSE;
      else
	{
	   pAbbCur = (*pAb)->AbFirstEnclosed;
	   result = FindAbsBox (Ntype, pSchP, presBoxName, &pAbbCur);
	   while (!result && pAbbCur->AbNext != NULL)
	     {
		pAbbCur = pAbbCur->AbNext;
		result = FindAbsBox (Ntype, pSchP, presBoxName, &pAbbCur);
	     }
	   if (result)
	      *pAb = pAbbCur;
	}
   return result;
}



/*----------------------------------------------------------------------
   	SearchElCrPresBoxCopy	cherche dans le sous arbre de l'element pointe' par pEl	
   		(racine comprise) un element auquel est associee une	
   		regle de presentation creant une boite du type presBoxType	
   		definie dans le schema de presentation pointe par pSchP.
   		Si presBoxType=0, on cherche une regle creant un pave de nom	
   		presBoxName et au retour, pSchP contient un pointeur sur le	
   		schema de presentation ou est definie la boite et presBoxType	
   		contient le numero de type de cette boite.		
   		Retourne Vrai si un tel element existe et dans ce cas,	
   		pEl pointe sur l'element trouve'.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     SearchElCrPresBoxCopy (int *presBoxType, PtrPSchema * pSchP, PtrSSchema * pSchS, Name presBoxName, PtrElement * pEl)
#else  /* __STDC__ */
static ThotBool     SearchElCrPresBoxCopy (presBoxType, pSchP, pSchS, presBoxName, pEl)
int                *presBoxType;
PtrPSchema         *pSchP;
PtrSSchema         *pSchS;
Name                presBoxName;
PtrElement         *pEl;
#endif /* __STDC__ */

{
   ThotBool            result, stop;
   PtrElement          pE1;
   PtrPSchema          pSP;
   PtrSSchema          pSS;
   PtrPRule            pPRuleCre;
   PtrAttribute        pA;
   PtrPRule            pPRule;

   result = FALSE;
   /* cherche toutes les regles de  creation de cet element */
   pPRuleCre = GlobalSearchRulepEl (*pEl, &pSP, &pSS, 0, NULL, 1, PtFunction, FnAny, FALSE, FALSE, &pA);
   stop = FALSE;
   do
      if (pPRuleCre == NULL)
	 /* il n' y a plus de regles */
	 stop = TRUE;
      else if (pPRuleCre->PrType != PtFunction)
	 /* il n'y a plus de regle fonction de presentation */
	 stop = TRUE;
      else
	{
	   /* la regle est une fonction de presentation */
	   pPRule = pPRuleCre;
	   if (pPRule->PrPresFunction == FnCreateBefore
	       || pPRule->PrPresFunction == FnCreateWith
	       || pPRule->PrPresFunction == FnCreateAfter
	       || pPRule->PrPresFunction == FnCreateFirst
	       || pPRule->PrPresFunction == FnCreateLast)
	      /* c'est une regle de creation */
	      if (*presBoxType != 0)
		{
		   result = pPRule->PrPresBox[0] == *presBoxType;
		   if (result)
		      result = ustrcmp (pSS->SsName, (*pSchS)->SsName) == 0;
		   /* on supprime le test sur l'egalite des schemas P et on teste uniquement */
		   /* les schemas de structure : cela permet a des chapitres de se referencer */
		   /* mutuellement meme si leur schema de presentation different legerement */
		   /* il faut que les schemas P aient les memes boites de presentation utilisees */
		   /* en copie */
		}
	      else
		 result = ustrcmp (pSP->PsPresentBox[pPRule->PrPresBox[0] - 1].PbName, presBoxName)
		    == 0;
	   if (result && (pSP != *pSchP))
	      /* retourne le schema de presentation et le */
	      /* numero de type de la boite creee */
	     {
		*pSchP = pSP;
		*pSchS = pSS;
		*presBoxType = pPRule->PrPresBox[0];
	     }

	   if (!result)
	      pPRuleCre = pPRule->PrNextPRule;
	}
   while (!stop && !result);
   if (!result)
      /* ce n'est pas l'element cherche', on cherche dans ses */
      /* descendants */
      if (!(*pEl)->ElTerminal && (*pEl)->ElFirstChild != NULL)
	{
	   pE1 = (*pEl)->ElFirstChild;
	   result = SearchElCrPresBoxCopy (presBoxType, pSchP, pSchS, presBoxName, &pE1);
	   while (!result && pE1->ElNext != NULL)
	     {
		pE1 = pE1->ElNext;
		result = SearchElCrPresBoxCopy (presBoxType, pSchP, pSchS, presBoxName, &pE1);
	     }
	   if (result)
	      *pEl = pE1;
	}
   return result;
}



/*----------------------------------------------------------------------
   	CopyLeaves  copie dans le pave pAb le contenu de toutes les	
   		feuilles de texte du sous-arbre de l'element pointe'	
   		par pEC							
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         CopyLeaves (PtrElement pEC, PtrAbstractBox * pAb, PtrTextBuffer * pBuffPrec)

#else  /* __STDC__ */
static void         CopyLeaves (pEC, pAb, pBuffPrec)
PtrElement          pEC;
PtrAbstractBox     *pAb;
PtrTextBuffer      *pBuffPrec;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuffE;
   PtrAbstractBox      pAbb1;
   PtrTextBuffer       pBuffP;

   if (!pEC->ElTerminal)
      /* ce n'est pas une feuille, on traite tous les fils */
     {
	pEC = pEC->ElFirstChild;
	while (pEC != NULL)
	  {
	     CopyLeaves (pEC, pAb, pBuffPrec);
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
	     GetTextBuffer (&pBuffP);
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
	     ustrncpy (pBuffP->BuContent, pBuffE->BuContent, THOT_MAX_CHAR);
	     /* copie le contenu */
	     pBuffP->BuLength = pBuffE->BuLength;
	     /* copie la longueur */
	     pAbb1->AbVolume += pBuffP->BuLength;
	     *pBuffPrec = pBuffP;
	     pBuffE = pBuffE->BuNext;
	  }
     }
}



/*----------------------------------------------------------------------
   	SearchElInSubTree cherche dans le sous-arbre dont la racine est	
   		pointe'e par pElRoot un element de numero de type elType	
   		defini dans le schema de structure pSS, si typeName est	
   		nul, de nom de type typeName sinon.			
   		Retourne un pointeur sur l'element trouve' ou NULL si pas
   		trouve'							
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static PtrElement   SearchElInSubTree (PtrElement pElRoot, int elType, PtrSSchema pSS, Name typeName)

#else  /* __STDC__ */
static PtrElement   SearchElInSubTree (pElRoot, elType, pSS, typeName)
PtrElement          pElRoot;
int                 elType;
PtrSSchema          pSS;
Name                typeName;

#endif /* __STDC__ */

{
   PtrElement          pEC, pElChild;

   pEC = NULL;			/* a priori on n'a pas trouve' */
   if (typeName[0] != WC_EOS)
      /* on compare les noms de type */
     {
	if (ustrcmp (typeName, pElRoot->ElStructSchema->SsRule[pElRoot->ElTypeNumber - 1].SrName) == 0)
	   pEC = pElRoot;
     }
   else
      /* on compare les numero de type et code de schema de structure */
      if (pElRoot->ElTypeNumber == elType
	  && pElRoot->ElStructSchema->SsCode == pSS->SsCode)
      /* c'est l'element cherche' */
      pEC = pElRoot;
   if (pEC == NULL)
      if (!pElRoot->ElTerminal)
	{
	   /* on cherche parmi les fils de l'element */
	   pElChild = pElRoot->ElFirstChild;
	   while (pElChild != NULL && pEC == NULL)
	     {
		pEC = SearchElInSubTree (pElChild, elType, pSS, typeName);
		pElChild = pElChild->ElNext;
	     }
	}
   return pEC;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     GetRuleCopy (PtrPRule pPRule)
#else  /* __STDC__ */
static PtrPRule     GetRuleCopy (pPRule)
PtrPRule            pPRule;

#endif /* __STDC__ */
{
   ThotBool            found;

   found = FALSE;
   while (pPRule != NULL && !found)
     {
	if (pPRule->PrType == PtFunction)
	   if (pPRule->PrPresFunction == FnCopy)
	      found = TRUE;
	if (!found)
	  {
	     pPRule = pPRule->PrNextPRule;
	     if (pPRule != NULL)
		if (pPRule->PrType > PtFunction)
		   pPRule = NULL;
	  }
     }
   if (!found)
      pPRule = NULL;
   return pPRule;
}


/*----------------------------------------------------------------------
   	ApplyCopy applique une regle de copie.				
   		  Procedure appelee aussi dans modif.c			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyCopy (PtrDocument pDoc, PtrPRule pPRule, PtrAbstractBox pAb, ThotBool withDescCopy)
#else  /* __STDC__ */
void                ApplyCopy (pDoc, pPRule, pAb, withDescCopy)
PtrDocument         pDoc;
PtrPRule            pPRule;
PtrAbstractBox      pAb;
ThotBool            withDescCopy;

#endif /* __STDC__ */
{
   PtrElement          pE, pElSv;
   PtrAbstractBox      pAbbCur;
   int                 view, i;
   ThotBool            found;
   PtrPSchema          pSchP;
   PtrSSchema          pSchS;
   PtrTextBuffer       pBuffPrec;
   int                 boxType;
   Name                boxName;
   DocumentIdentifier  IDoc;
   PtrDocument         pDocRef;
   PtrElement          pEl1;
   PtrAbstractBox      pAbb1;
   PresentationBox    *pBo1;
   PtrCopyDescr        pDC;
   PtrPRule            pPRule1;
   PtrAttribute        pAttr;
   ThotBool            Ref;

   pEl1 = pAb->AbElement;
   pE = NULL;
   Ref = FALSE;
   if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct == CsReference)
     {
	/* la regle Copy s'applique a un pave' d'un element reference */
	Ref = TRUE;
	/* cherche l'element qui est reference' */
	pE = ReferredElement (pEl1->ElReference, &IDoc, &pDocRef);
     }
   else if (pAb->AbPresentationBox)
      if (pAb->AbCreatorAttr != NULL)
	 if (pAb->AbCreatorAttr->AeAttrType == AtReferenceAttr)
	    /* la regle Copy s'applique a un pave' de presentation */
	    /* cree' par un attribut reference */
	   {
	      Ref = TRUE;
	      /* cherche l'element qui est reference' par l'attribut */
	      pE = ReferredElement (pAb->AbCreatorAttr->AeAttrReference, &IDoc, &pDocRef);
	   }
   if (Ref)
     {
	/* c'est une copie par reference */
	if (pE != NULL)
	   /* l'element qui est reference' existe, il est pointe' par pE */
	   if (pPRule->PrElement)
	      /* il faut copier le contenu d'un element structure' contenu */
	      /* dans l'element reference'. On cherche cet element */
	      pE = SearchElInSubTree (pE, pPRule->PrPresBox[0], pEl1->ElStructSchema, pPRule->PrPresBoxName);
	   else
	      /* il faut copier une boite de presentation */
	      /* prend le schema de presentation qui s'applique a la reference */
	     {
		SearchPresSchema (pAb->AbElement, &pSchP, &i, &pSchS);
		if (pPRule->PrNPresBoxes == 0)
		   /* la boite de presentation a copier est definie par son nom */
		  {
		     boxType = 0;
		     ustrncpy (boxName, pPRule->PrPresBoxName, MAX_NAME_LENGTH);
		     /* nom de la boite a cherche */
		  }
		else
		   /* la boite de presentation est definie par son numero de type */
		   boxType = pPRule->PrPresBox[0];
		/* numero de type de la boite */
		/* cherche dans toutes les vues une boite du type de celle a */
		/* copier parmi les paves de cet element et de ses descendants */
		view = 0;
		found = FALSE;
		do
		  {
		     view++;
		     /* premier pave de l'element dans cette vue */
		     pAbbCur = pE->ElAbstractBox[view - 1];
		     if (pAbbCur != NULL)
			do
			  {
			     found = FindAbsBox (boxType, pSchP, boxName, &pAbbCur);
			     if (!found)
			       {
				  pAbbCur = pAbbCur->AbNext;
				  if (pAbbCur != NULL)
				     if (pAbbCur->AbElement != pE)
					pAbbCur = NULL;
			       }
			  }
			while (!found && pAbbCur != NULL);
		  }
		while (!found && view < MAX_VIEW_DOC);
		if (found)
		   /* on a trouve' le pave a copier, on le copie */
		  {
		     pAbb1 = pAb;
		     pAbb1->AbLeafType = LtText;
		     pAbb1->AbVolume = pAbbCur->AbVolume;
		     if (pAbbCur->AbText != NULL)
			*pAbb1->AbText = *pAbbCur->AbText;
		     pAbb1->AbLanguage = pAbbCur->AbLanguage;
		     pAbb1->AbCanBeModified = FALSE;
		  }
		else
		   /* on n'a pas trouve le pave a copier */
		if (!IsASavedElement (pE))
		   /* on ne fait rien si l'element reference' est dans le buffer */
		   /* de Couper-Coller */
		   /* on cherche dans le sous-arbre abstrait de l'element */
		   /* reference' un element qui cree la boite a copier */
		  {
		     /*pElSrce = pE; */
		     found = SearchElCrPresBoxCopy (&boxType, &pSchP, &pSchS, boxName, &pE);
		     if (found)
			/* on a trouve' l'element pE qui cree la boite a copier */
		       {
			  pBo1 = &pSchP->PsPresentBox[boxType - 1];
			  if (pBo1->PbContent == ContVariable)
			     /* on fait comme si le pave appartenait a l'element */
			     /* a copier */
			    {
			       pElSv = pAb->AbElement;
			       pAb->AbElement = pE;
			       found = NewVariable (pBo1->PbContVariable, pSchS, pSchP,
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
			       pPRule1 = GetRuleCopy (pBo1->PbFirstPRule);
			       if (pPRule1 != NULL)
				  /* on a trouve' la regle FnCopy. On l'applique en */
				  /* faisant comme si le pave appartenait a l'element */
				  /* qui cree la boite de presentation a copier */
				 {
				    pElSv = pAb->AbElement;
				    pAb->AbElement = pE;
				    ApplyCopy (pDoc, pPRule1, pAb, TRUE);
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
	/*cherche d'abord l'element a copier a l'interieur de l'element copieur */
	pE = SearchElInSubTree (pAb->AbElement, pPRule->PrPresBox[0], pEl1->ElStructSchema, pPRule->PrPresBoxName);

	if (pE == NULL)
	   /* on n'a pas trouve' l'element a copier */
	   if (pEl1->ElTypeNumber == PageBreak + 1)
	      if (pEl1->ElPageType == PgBegin)
		 /* on travaille pour une marque de page qui est engendree par */
		 /* le debut d'un element. On cherche dans cet element */
		 pE = SearchElInSubTree (pEl1->ElParent, pPRule->PrPresBox[0], pEl1->ElStructSchema, pPRule->PrPresBoxName);
	/* si on n'a pas trouve', on cherche en arriere l'element a copier */
	if (pE == NULL)
	   if (pPRule->PrNPresBoxes > 0)
	      /* la boite a copier est definie par son numero de type */
	      pE = BackSearchTypedElem (pAb->AbElement, pPRule->PrPresBox[0], pEl1->ElStructSchema);
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
	if (TypeHasException (1207, pAbb1->AbElement->ElTypeNumber, pAbb1->AbElement->ElStructSchema))
	  {
	     if (ThotLocalActions[T_indexcopy] != NULL)
		(*ThotLocalActions[T_indexcopy]) (pE, &pAb, &pBuffPrec);
	  }
	else
	  {
	     /* si l'element a copier est lui-meme une reference qui copie un */
	     /* autre element, c'est cet autre element qu'on copie */
	     pPRule1 = NULL;
	     if (pE->ElStructSchema->SsRule[pE->ElTypeNumber - 1].SrConstruct == CsReference)
	       {
		  pPRule1 = GlobalSearchRulepEl (pE, &pSchP, &pSchS, 0, NULL, 1, PtFunction, FnAny, FALSE, FALSE, &pAttr);
		  pPRule1 = GetRuleCopy (pPRule1);
	       }
	     if (pPRule1 == NULL)
		/* copie simplement toutes les feuilles de texte de pE */
		CopyLeaves (pE, &pAb, &pBuffPrec);
	     else
		/* applique la regle de copie transitive */
	       {
		  pElSv = pAb->AbElement;
		  pAb->AbElement = pE;
		  ApplyCopy (pDoc, pPRule1, pAb, FALSE);
		  pAb->AbElement = pElSv;
	       }
	  }
	if (withDescCopy)
	  {
	     /* ajoute a l'element copie' un descripteur d'element copie' */
	     GetDescCopy (&pDC);
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


/*----------------------------------------------------------------------
   	ApplyRule   applique au pave pointe par pAb la regle pointee par
   		pPRule dans le schema de presentation pointe par pSchP.	
   		Si pAttr n'est pas NULL, c'est un pointeur sur le bloc	
   		attribut auquel correspond la regle a appliquer.	
   		Retourne true si la regle a ete appliquee ou ne pourra	
   		jamais etre appliquee, false si elle n'a pas pu etre	
   		appliquee mais qu'elle pourra etre appliquee quand	
   		d'autres paves seront construits.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ApplyRule (PtrPRule pPRule, PtrPSchema pSchP, PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr)
#else  /* __STDC__ */
ThotBool            ApplyRule (pPRule, pSchP, pAb, pDoc, pAttr)
PtrPRule            pPRule;
PtrPSchema          pSchP;
PtrAbstractBox      pAb;
PtrDocument         pDoc;
PtrAttribute        pAttr;

#endif /* __STDC__ */
{
  TypeUnit            unit;
  AbPosition          Posit;
  PresConstant	     *pConst;
  PathBuffer	      directoryName;
  CHAR_T	      fname[MAX_PATH];
  CHAR_T              c;
  int                 viewSch, i;
  ThotBool            appl;
  ThotBool            insidePage, afterPageBreak;
  AbPosition         *pPavP1;

  appl = FALSE;
  if (pPRule != NULL && pAb != NULL)
    if (pAb->AbElement != NULL)
      {
	viewSch = AppliedView (pAb->AbElement, pAttr, pDoc, pAb->AbDocView);
	switch (pPRule->PrType)
	  {

	  case PtFunction:
	    switch (pPRule->PrPresFunction)
	      {

	      case FnLine:
		if (pAb->AbLeafType == LtCompound)
		  /* si la regle de mise en lignes est definie pour la */
		  /* vue principale, elle s'applique a toutes les vues, */
		  /* sinon, elle ne s'applique qu'a la vue pour laquelle */
		  /* elle est definie */
		  if (pPRule->PrViewNum == 1 || pPRule->PrViewNum == viewSch)
		    {
		      pAb->AbInLine = TRUE;		      
		      appl = TRUE;
		    }
		break;

	      case FnNoLine:
		if (pAb->AbLeafType == LtCompound &&
		    pPRule->PrViewNum == viewSch)
		  {
		    pAb->AbInLine = FALSE;
		    appl = TRUE;
		  }
		break;

	      case FnPage:
		ApplyPage (pDoc, pAb, viewSch, pPRule, pPRule->PrPresFunction);
		appl = TRUE;
		break;

	      case FnColumn:
		break;

	      case FnSubColumn:
		break;

	      case FnCopy:
		/* on n'applique pas la regle copie a un element holophraste'*/
		if (!pAb->AbElement->ElHolophrast)
		  {
		    ApplyCopy (pDoc, pPRule, pAb, TRUE);
		    appl = TRUE;
		  }
		break;

	      case FnContentRef:
		ConstantCopy (pPRule->PrPresBox[0], pSchP, pAb);
		appl = TRUE;
		break;

	      case FnShowBox:
		if (pAb->AbLeafType == LtCompound &&
		    pPRule->PrViewNum == viewSch)
		  {
		    pAb->AbFillBox = TRUE;
		    appl = TRUE;
		  }
		break;

	      case FnBackgroundPicture:
		if (pAb->AbLeafType == LtCompound &&
		    pPRule->PrViewNum == viewSch)
		  {
		    if (pSchP == NULL)
		       pSchP = pDoc->DocSSchema->SsPSchema;
		    pConst = &pSchP->PsConstant[pPRule->PrPresBox[0] - 1];
		    if (pConst->PdString[0] != EOS)
		      {
# ifndef _WINDOWS
			if (pConst->PdString[0] == DIR_SEP)
# else  /* _WINDOWS */
			if (pConst->PdString[0] == DIR_SEP ||
			    (pConst->PdString[1] == ':' &&
			     pConst->PdString[2] == DIR_SEP))
# endif /* _WINDOWS */
			  /* absolute file name */
			  ustrncpy (fname, pConst->PdString, MAX_PATH - 1);
			else
			  /* relative file name */
			  {
			    ustrncpy (directoryName, SchemaPath, MAX_PATH - 1);
			    MakeCompleteName (pConst->PdString, TEXT(""), directoryName, fname, &i);
			  }
			NewPictInfo (pAb, fname, UNKNOWN_FORMAT);
			appl = TRUE;
		      }
		  }
		break;

	      case FnPictureMode:
		if (pPRule->PrViewNum == viewSch)
		  if (pAb->AbElement->ElTerminal &&
		      pAb->AbElement->ElLeafType == LtPicture)
		    {
		      if (pAb->AbElement->ElPictInfo == NULL)
			 NewPictInfo (pAb, TEXT(""), UNKNOWN_FORMAT);
		      ((PictInfo *) (pAb->AbElement->ElPictInfo))->PicPresent =
			                  (PictureScaling)pPRule->PrPresBox[0];
		      appl = TRUE;
		    }
		  else if (pAb->AbPresentationBox)
		    {
		      if (pAb->AbPictInfo == NULL)
			 NewPictInfo (pAb, TEXT(""), UNKNOWN_FORMAT);
		      ((PictInfo *) (pAb->AbPictInfo))->PicPresent =
			                  (PictureScaling)pPRule->PrPresBox[0];
		      appl = TRUE;
		    }
		  else if (pAb->AbLeafType == LtCompound)
		    {
		      if (pAb->AbPictBackground == NULL)
			NewPictInfo (pAb, NULL, UNKNOWN_FORMAT);
		      ((PictInfo *) (pAb->AbPictBackground))->PicPresent =
			                  (PictureScaling)pPRule->PrPresBox[0];
		      appl = TRUE;
		    }
		break;

	      case FnNotInLine:
		if (pPRule->PrViewNum == viewSch)
		  {
		    pAb->AbNotInLine = TRUE;
		    appl = TRUE;
		  }
		break;

	      default:
		break;
	      }	    
	    break;

	  case PtVisibility:
	    pAb->AbVisibility = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbVisibility = 10;
		appl = TRUE;
	      }
	    break;

	  case PtDepth:
	    pAb->AbDepth = IntegerRule (pPRule, pAb->AbElement, pAb->AbDocView,
					&appl, &unit, pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbDepth = 0;
		appl = TRUE;
	      }
	    break;

	  case PtFillPattern:
	    pAb->AbFillPattern = IntegerRule (pPRule, pAb->AbElement,
					      pAb->AbDocView, &appl, &unit,
					      pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbFillPattern = 0;
		appl = TRUE;
	      }
	    break;

	  case PtBackground:
	    pAb->AbBackground = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbBackground = DefaultBColor;
		appl = TRUE;
	      }
	    break;

	  case PtForeground:
	    pAb->AbForeground = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbForeground = DefaultFColor;
		appl = TRUE;
	      }
	    break;

          case PtBorderTopColor:
	    pAb->AbTopBColor = IntegerRule (pPRule, pAb->AbElement,
					    pAb->AbDocView, &appl, &unit,
					    pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* no BorderTopColor for the root element. Set initial value */
	      {
		pAb->AbTopBColor = -1;
		appl = TRUE;
	      }
	    break;

          case PtBorderRightColor:
	    pAb->AbRightBColor = IntegerRule (pPRule, pAb->AbElement,
					      pAb->AbDocView, &appl, &unit,
					      pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* no BorderRightColor for the root element. Set initial value */
	      {
		pAb->AbRightBColor = -1;
		appl = TRUE;
	      }
	    break;

          case PtBorderBottomColor:
	    pAb->AbBottomBColor = IntegerRule (pPRule, pAb->AbElement,
					       pAb->AbDocView, &appl, &unit,
					       pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* no BorderBottomColor for the root element. Set initial value*/
	      {
		pAb->AbBottomBColor = -1;
		appl = TRUE;
	      }
	    break;

          case PtBorderLeftColor:
	    pAb->AbLeftBColor = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* no BorderLeftColor for the root element. Set initial value */
	      {
		pAb->AbLeftBColor = -1;
		appl = TRUE;
	      }
	    break;

	  case PtFont:
	    pAb->AbFont = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      {
		pAb->AbFont = 'T';
		appl = TRUE;
	      }
	    if (pAb->AbFont >= 'a' && pAb->AbFont <= 'z')
	      /* on n'utilise que des majuscules pour les noms de police */
	      pAb->AbFont = (char) ((int) (pAb->AbFont) - 32);
	    break;

	  case PtStyle:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbFontStyle = 0;
		appl = TRUE;
	      }
	    else
	      switch (c)
		{
		case 'I':	/* Italic */
		  pAb->AbFontStyle = 1;
		  break;
		case 'O':	/* Oblique */
		  pAb->AbFontStyle = 2;
		  break;
		default:	/* Roman */
		  pAb->AbFontStyle = 0;
		  break;
		}
	    break;

	  case PtWeight:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbFontWeight = 0;
		appl = TRUE;
	      }
	    else
	      switch (c)
		{
		case 'B':	/* Bold */
		  pAb->AbFontWeight = 1;
		  break;
		default:	/* Normal */
		  pAb->AbFontWeight = 0;
		  break;
		}
	    break;

	  case PtUnderline:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbUnderline = 0;
		appl = TRUE;
	      }
	    else
	      switch (c)
		{
		case 'C':
		  pAb->AbUnderline = 3;
		  break;
		case 'O':
		  pAb->AbUnderline = 2;
		  break;
		case 'U':
		  pAb->AbUnderline = 1;
		  break;
		default:
		  pAb->AbUnderline = 0;
		  break;
		}
	    break;

	  case PtThickness:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbThickness = 0;
		appl = TRUE;
	      }
	    else
	      switch (c)
		{
		case 'T':
		  pAb->AbThickness = 1;
		  break;
		default:
		  pAb->AbThickness = 0;
		  break;
		}
	    break;

	  case PtLineStyle:
	    pAb->AbLineStyle = CharRule (pPRule, pAb->AbElement,
					 pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      {
		pAb->AbLineStyle = 'S';
		appl = TRUE;
	      }
	    break;

          case PtBorderTopStyle:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      {
	        c = '0';
		appl = TRUE;
	      }
	    if (appl)
                pAb->AbTopStyle = BorderStyleIntValue (c);
	    break;

          case PtBorderRightStyle:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      {
	        c = '0';
		appl = TRUE;
	      }
	    if (appl)
                pAb->AbRightStyle = BorderStyleIntValue (c);
	    break;

          case PtBorderBottomStyle:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      {
	        c = '0';
		appl = TRUE;
	      }
	    if (appl)
                pAb->AbBottomStyle = BorderStyleIntValue (c);
	    break;

          case PtBorderLeftStyle:
	    c = CharRule (pPRule, pAb->AbElement, pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      {
	        c = '0';
		appl = TRUE;
	      }
	    if (appl)
                pAb->AbLeftStyle = BorderStyleIntValue (c);
	    break;

	  case PtIndent:
	    pAb->AbIndent = IntegerRule (pPRule, pAb->AbElement,
					 pAb->AbDocView, &appl, &unit, pAttr);
	    pAb->AbIndentUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbIndent = 0;
		appl = TRUE;
	      }
	    break;

	  case PtSize:
	    /* on applique la regle de taille */
	    pAb->AbSize = IntegerRule (pPRule, pAb->AbElement,
				       pAb->AbDocView, &appl, &unit, pAttr);
	    if (appl)
	      pAb->AbSizeUnit = unit;
	    else if (pAb->AbElement->ElParent == NULL)
	      /* c'est la racine, on met a priori la valeur par defaut */
	      {
		pAb->AbSize = 3;
		pAb->AbSizeUnit = UnRelative;
		appl = TRUE;
	      }
	    break;

	  case PtLineSpacing:
	    pAb->AbLineSpacing = IntegerRule (pPRule, pAb->AbElement,
					      pAb->AbDocView, &appl, &unit,
					      pAttr);
	    pAb->AbLineSpacingUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbLineSpacing = 10;
		pAb->AbLineSpacingUnit = UnRelative;
		appl = TRUE;
	      }
	    break;

	  case PtLineWeight:
	    pAb->AbLineWeight = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    pAb->AbLineWeightUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbLineWeight = 1;
		pAb->AbLineWeightUnit = UnPoint;
		appl = TRUE;
	      }
	    break;

          case PtMarginTop:
	    pAb->AbTopMargin = IntegerRule (pPRule, pAb->AbElement,
					    pAb->AbDocView, &appl, &unit,
					    pAttr);
	    pAb->AbTopMarginUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbTopMargin = 0;
		appl = TRUE;
	      }
            break;

          case PtMarginRight:
	    pAb->AbRightMargin = IntegerRule (pPRule, pAb->AbElement,
					      pAb->AbDocView, &appl, &unit,
					      pAttr);
	    pAb->AbRightMarginUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbRightMargin = 0;
		appl = TRUE;
	      }
            break;

          case PtMarginBottom:
	    pAb->AbBottomMargin = IntegerRule (pPRule, pAb->AbElement,
					       pAb->AbDocView, &appl, &unit,
					       pAttr);
	    pAb->AbBottomMarginUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbBottomMargin = 0;
		appl = TRUE;
	      }
            break;

          case PtMarginLeft:
	    pAb->AbLeftMargin = IntegerRule (pPRule, pAb->AbElement,
					    pAb->AbDocView, &appl, &unit,
					    pAttr);
	    pAb->AbLeftMarginUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbLeftMargin = 0;
		appl = TRUE;
	      }
            break;

          case PtPaddingTop:
	    pAb->AbTopPadding = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    pAb->AbTopPaddingUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbTopPadding = 0;
		appl = TRUE;
	      }
            break;

          case PtPaddingRight:
	    pAb->AbRightPadding = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    pAb->AbRightPaddingUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbRightPadding = 0;
		appl = TRUE;
	      }
            break;

          case PtPaddingBottom:
	    pAb->AbBottomPadding = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    pAb->AbBottomPaddingUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbBottomPadding = 0;
		appl = TRUE;
	      }
            break;

          case PtPaddingLeft:
 	    pAb->AbLeftPadding = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    pAb->AbLeftPaddingUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbLeftPadding = 0;
		appl = TRUE;
	      }
           break;

          case PtBorderTopWidth:
	    pAb->AbTopBorder = IntegerRule (pPRule, pAb->AbElement,
					    pAb->AbDocView, &appl, &unit,
					    pAttr);
	    pAb->AbTopBorderUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbTopBorder = 0;
		appl = TRUE;
	      }
            break;

          case PtBorderRightWidth:
	    pAb->AbRightBorder = IntegerRule (pPRule, pAb->AbElement,
					      pAb->AbDocView, &appl, &unit,
					      pAttr);
	    pAb->AbRightBorderUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbRightBorder = 0;
		appl = TRUE;
	      }
            break;

          case PtBorderBottomWidth:
	    pAb->AbBottomBorder = IntegerRule (pPRule, pAb->AbElement,
					       pAb->AbDocView, &appl, &unit,
					       pAttr);
	    pAb->AbBottomBorderUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbBottomBorder = 0;
		appl = TRUE;
	      }
            break;

          case PtBorderLeftWidth:
	    pAb->AbLeftBorder = IntegerRule (pPRule, pAb->AbElement,
					     pAb->AbDocView, &appl, &unit,
					     pAttr);
	    pAb->AbLeftBorderUnit = unit;
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbLeftBorder = 0;
		appl = TRUE;
	      }
            break;

          case PtVertRef:
	    Posit = pAb->AbVertRef;
	    ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr, pSchP, pAb,
		      pDoc, &appl);
	    pAb->AbVertRef = Posit;
	    break;

	  case PtHorizRef:
	    Posit = pAb->AbHorizRef;
	    ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr, pSchP, pAb,
		      pDoc, &appl);
	    pAb->AbHorizRef = Posit;
	    break;

	  case PtVertPos:
	    /* erreur : ce n'est pas a l'editeur d'interpreter */
	    /* la mise en ligne. On supprime ce code */
	    /* Si le precedent est un separateur de page, le pave est */
	    /* positionne' en dessous de ce saut de page, sauf si le pave */
	    /* positionne' fait partie d'un separateur de page (c'est une */
	    /* boite de haut ou de bas de page qui doit etre placee par */
	    /* rapport au filet separateur) ou s'il se positionne par */
	    /* rapport a un autre element. */
	    afterPageBreak = FALSE;
	    if (pAb->AbPrevious != NULL)
	      /* il y a un pave precedent */
	      {
		if (!pAb->AbPrevious->AbDead
		    && pAb->AbPrevious->AbElement->
		    ElTypeNumber == PageBreak + 1
		    && pAb->AbElement->ElTypeNumber != PageBreak + 1)
		  if (pPRule->PrPosRule.PoRelation == RlSameLevel
		      || pPRule->PrPosRule.PoRelation == RlPrevious)
		    afterPageBreak = TRUE;
		  else
		    {
		      if (pPRule->PrPosRule.PoRelation == RlEnclosing)
			{
			  if (pAb->AbPrevious->AbElement->
			      ElPageType != PgBegin)
			    afterPageBreak = TRUE;
			}
		    }
	      }
	    else
	      /* il n'y a pas de pave precedent */
	      if (pAb->AbElement->ElPrevious != NULL)
		/* il y a un element precedent */
		if (pAb->AbElement->ElPrevious->ElTypeNumber == PageBreak + 1
		    && pAb->AbElement->ElPrevious->ElViewPSchema == viewSch
		    && pAb->AbElement->ElTypeNumber != PageBreak + 1)
		  /* l'element precedent est une marque de page pour la vue */
		  if (pPRule->PrPosRule.PoRelation == RlSameLevel
		      || pPRule->PrPosRule.PoRelation == RlPrevious)
		    afterPageBreak = TRUE;
	    if (afterPageBreak)
	      /* position: en dessous du pave precedent */
	      if (pAb->AbPrevious == NULL)
		/* le pave de la marque de page n'est pas encore cree', on */
		/* ne peut pas appliquer la regle de positionnement */
		appl = FALSE;
	      else
		{
		  pPavP1 = &pAb->AbVertPos;
		  pPavP1->PosEdge = Top;
		  pPavP1->PosRefEdge = Bottom;
		  pPavP1->PosDistance = 0;
		  pPavP1->PosUnit = UnPoint;
		  pPavP1->PosAbRef = pAb->AbPrevious;
		  pPavP1->PosUserSpecified = FALSE;
		}
	    else
	      /* s'il s'agit d'une boite page (celle qui englobe le filet */
	      /* et les hauts et pieds de page), et si elle n'est precedee */
	      /* d'aucun element, elle se positionne en haut de l'englobant. */
	      {
		insidePage = FALSE;
		if (pAb->AbEnclosing != NULL)
		  if (pAb->AbElement->ElTypeNumber == PageBreak + 1
		      && pAb->AbEnclosing->AbElement->
		      ElTypeNumber != PageBreak + 1
		      && pAb->AbElement->ElPrevious == NULL)
		    if (pAb->AbPrevious == NULL)
		      insidePage = TRUE;
		
		if (insidePage)
		  {
		    pPavP1 = &pAb->AbVertPos;
		    pPavP1->PosAbRef = pAb->AbEnclosing;
		    pPavP1->PosEdge = Top;
		    pPavP1->PosRefEdge = Top;
		    pPavP1->PosDistance = 0;
		    pPavP1->PosUnit = UnPoint;
		    pPavP1->PosUserSpecified = FALSE;
		  }
		else
		  /* applique la regle de positionnement de l'element */
		  {
		    Posit = pAb->AbVertPos;
		    ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr,
			      pSchP, pAb, pDoc, &appl);
		    pAb->AbVertPos = Posit;
		  }
	      }
	    /* traitement special pour le debordement vertical des cellules */
	    /* de tableau etendues verticalement */
	    if (ThotLocalActions[T_vertspan] != NULL)
	      (*ThotLocalActions[T_vertspan]) (pPRule, pAb);
	    break;

	  case PtHorizPos:
	    Posit = pAb->AbHorizPos;
	    ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr, pSchP,
		      pAb, pDoc, &appl);
	    pAb->AbHorizPos = Posit;
	    break;

	  case PtHeight:
	    ApplyDim (&pAb->AbHeight, pAb, pSchP, pAttr, &appl, pPRule, pDoc);
	    /* traitement special pour le debordement vertical des cellules */
	    /* de tableau etendues verticalement */
	    if (ThotLocalActions[T_vertspan] != NULL)
	       (*ThotLocalActions[T_vertspan]) (pPRule, pAb);
	    break;

	  case PtWidth:
	    ApplyDim (&pAb->AbWidth, pAb, pSchP, pAttr, &appl, pPRule, pDoc);
	    break;

	  case PtJustify:
	    pAb->AbJustify = BoolRule (pPRule, pAb->AbElement,
				       pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbJustify = FALSE;
		appl = TRUE;
	      }
	    break;

	  case PtHyphenate:
	    pAb->AbHyphenate = BoolRule (pPRule, pAb->AbElement,
					 pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbHyphenate = FALSE;
		appl = TRUE;
	      }
	    break;

	  case PtVertOverflow:
	    pAb->AbVertEnclosing = !BoolRule (pPRule,
					      pAb->AbElement,
					      pAb->AbDocView, &appl);
	    break;

	  case PtHorizOverflow:
	    pAb->AbHorizEnclosing = !BoolRule (pPRule,
					       pAb->AbElement,
					       pAb->AbDocView, &appl);
	    break;

	  case PtAdjust:
	    pAb->AbAdjust = AlignRule (pPRule, pAb->AbElement,
					 pAb->AbDocView, &appl);
	    if (!appl && pAb->AbElement->ElParent == NULL)
	      /* Pas de regle pour la racine, on met la valeur par defaut */
	      {
		pAb->AbAdjust = AlignLeft;
		appl = TRUE;
	      }
	    break;

	  default:
	    break;
	  }	
      }
  return appl;
}


/*----------------------------------------------------------------------
   	SearchPresRule	Cherche si la regle de presentation specifique
   		de type ruleType concernant la vue de numero view existe
   		pour l'element pEl.
   		Retourne un pointeur sur cette regle si elle existe,
   		sinon cree une nouvelle regle de ce type, l'ajoute a la
   		chaine des regles de presentation specifiques de
   		l'element et retourne un pointeur sur la nouvelle regle.
   		Au retour, isNew indique s'il s'agit d'une regle
   		nouvellement creee ou non.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPRule            SearchPresRule (PtrElement pEl, PRuleType ruleType, FunctionType funcType, ThotBool * isNew, PtrDocument pDoc, int view)
#else  /* __STDC__ */
PtrPRule            SearchPresRule (pEl, ruleType, funcType, isNew, pDoc, view)
PtrElement          pEl;
PRuleType           ruleType;
FunctionType	    funcType;
ThotBool           *isNew;
PtrDocument         pDoc;
int                 view;

#endif /* __STDC__ */
{
   PtrPRule            pResultRule;
   PtrPRule            pPRule;
   ThotBool	       found;

   *isNew = FALSE;
   pResultRule = NULL;
   if (pEl != NULL)
     {
	/* l'element du pave */
	if (pEl->ElFirstPRule == NULL)
	  {
	     /* cet element n'a aucune regle de presentation specifique, on */
	     /* en cree une et on la chaine a l'element */
	     GetPresentRule (&pResultRule);
	     *isNew = TRUE;
	     pEl->ElFirstPRule = pResultRule;
	  }
	else
	  {
	     /* cherche parmi les regles de presentation specifiques de
	        l'element si ce type de regle existe pour la vue
	        a laquelle appartient le pave. */
	     pPRule = pEl->ElFirstPRule;	/* premiere regle specifique de l'element */
	     while (pResultRule == NULL)
	       {
		  found = FALSE;
		  if (pPRule->PrViewNum == pDoc->DocView[view-1].DvPSchemaView)
		     if (pPRule->PrType == ruleType)
			if (ruleType == PtFunction)
			   found = (pPRule->PrPresFunction == funcType);
			else
			   found = TRUE;
		  if (found)
		     /* la regle existe deja */
		     pResultRule = pPRule;
		  else
		    if (pPRule->PrNextPRule != NULL)
		       /* passe a la regle specifique suivante de l'element */
		       pPRule = pPRule->PrNextPRule;
		    else
		      {
		       /* On a examine' toutes les regles specifiques de */
		       /* l'element, ajoute une nouvelle regle en fin de chaine */
		       GetPresentRule (&pResultRule);
		       *isNew = TRUE;
		       pPRule->PrNextPRule = pResultRule;
		      }
	       }
	  }
	if (*isNew)
	  {
	     pResultRule->PrType = ruleType;
	     if (ruleType == PtFunction)
	        {
		pResultRule->PrPresMode = PresFunction;
		pResultRule->PrPresFunction = funcType;
		pResultRule->PrPresBoxRepeat = FALSE;
		pResultRule->PrNPresBoxes = 0;
		}
	  }
     }
   return pResultRule;
}


/*----------------------------------------------------------------------
   	RedispAbsBox indique dans le contexte du document que le pave pAb	
   		est a reafficher					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedispAbsBox (PtrAbstractBox pAb, PtrDocument pDoc)
#else  /* __STDC__ */
void                RedispAbsBox (pAb, pDoc)
PtrAbstractBox      pAb;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
  if (!AssocView (pAb->AbElement))
    pDoc->DocViewModifiedAb[pAb->AbDocView - 1] = Enclosing (pAb, pDoc->DocViewModifiedAb[pAb->AbDocView - 1]);
  else
    pDoc->DocAssocModifiedAb[pAb->AbElement->ElAssocNum - 1] = Enclosing (pAb, pDoc->DocAssocModifiedAb[pAb->AbElement->ElAssocNum - 1]);
}



/*----------------------------------------------------------------------
   	NewDimPicture fixe les dimensions d'un pave-image  lorsque	
   		le driver d'image ne sait pas donner une dimension a	
   		cette image. C'est le cas lorsqu'on ne tient pas compte	
   		de la cropping frame  (pour le CGM, par exemple.)	
   		Cette fonction fait le meme travail que NewDimension	
   		sans reafficher le document a la fin, laissant ce	
   		travail au driver d'images.				
   		On traite le cas ou une IMAGE est dimensionnee par son	
   		contenu comme si c'etait une dimension fixe.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewDimPicture (PtrAbstractBox pAb)
#else  /* __STDC__ */
void                NewDimPicture (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
   ThotBool            IsNew, ok;
   PtrPRule            pPRuleDimH, pPRuleDimV, pR, pRStd;
   PtrPSchema          pSPR;
   PtrSSchema          pSSR;
   PtrDocument         pDoc;
   PtrElement          pEl;
   PtrAttribute        pAttr;
   int                 height, width;
   int                 frame[MAX_VIEW_DOC];
   int                 viewSch;
   PtrAbstractBox      pAbb;
   int                 viewDoc;
   ThotBool            stop;
   int                 view;

   pPRuleDimH = NULL;
   pPRuleDimV = NULL;
   /* nettoie la table des frames a reafficher */
   for (viewDoc = 1; viewDoc <= MAX_VIEW_DOC; viewDoc++)
      frame[viewDoc - 1] = 0;
   pEl = pAb->AbElement;	/* l'element auquel correspond le pave */
   pDoc = DocumentOfElement (pEl);	/* le document auquel appartient le pave */
   view = pAb->AbDocView;	/* la vue concernee */
   viewSch = AppliedView (pEl, NULL, pDoc, view);	/* type de cette vue dans le schema P */

   /* les deltas de dimension que l'on va appliquer sont ceux 
      de la boite par defaut avec laquelle on a cree l'image */
   height = PixelToPoint (pAb->AbBox->BxH);
   width = PixelToPoint (pAb->AbBox->BxW);

   /* traite le changement de largeur */

   /* cherche d'abord la regle de dimension qui s'applique a l'element */
   pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtWidth, FnAny, FALSE, TRUE, &pAttr);
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
	pPRuleDimH = SearchPresRule (pEl, PtWidth, 0, &IsNew, pDoc, view);
	if (IsNew)
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
	pPRuleDimH->PrDimRule.DrValue = width;
     }

   /* traite le changement de hauteur de la boite */

   /* cherche d'abord la regle de dimension qui s'applique a l'element */
   pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtHeight, FnAny, FALSE, TRUE, &pAttr);
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
	pPRuleDimV = SearchPresRule (pEl, PtHeight, 0, &IsNew, pDoc, view);
	if (IsNew)
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
	pPRuleDimV->PrDimRule.DrValue = height;
     }

   /* applique les nouvelles regles de presentation */

   if (pPRuleDimV != NULL || pPRuleDimH != NULL)
     {
	for (viewDoc = 1; viewDoc <= MAX_VIEW_DOC; viewDoc++)
	   if (pEl->ElAbstractBox[viewDoc - 1] != NULL)
	      /* l'element traite' a un pave dans cette vue */
	      if (pDoc->DocView[viewDoc - 1].DvSSchema == pDoc->DocView[view - 1].DvSSchema &&
		  pDoc->DocView[viewDoc - 1].DvPSchemaView == pDoc->DocView[view - 1].DvPSchemaView)
		 /* c'est une vue de meme type que la vue traitee, on */
		 /* traite le pave de l'element dans cette vue */
		{
		   pAbb = pEl->ElAbstractBox[viewDoc - 1];
		   /* saute les paves de presentation */
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox)
			 stop = TRUE;
		      else
			 pAbb = pAbb->AbNext;
		   while (!stop);

		   if (pAbb != NULL)
		     {
			/* applique la nouvelle regle specifique Horizontale */
			if (pPRuleDimH != NULL)
			   if (ApplyRule (pPRuleDimH, pSPR, pAbb, pDoc, pAttr))
			      pAbb->AbWidthChange = TRUE;
			/* applique la nouvelle regle specifique Verticale */
			if (pPRuleDimV != NULL)
			   if (ApplyRule (pPRuleDimV, pSPR, pAbb, pDoc, pAttr))
			      pAbb->AbHeightChange = TRUE;

			RedispAbsBox (pAbb, pDoc);	/* indique le pave a reafficherv */
			if (!AssocView (pEl))
			   frame[viewDoc - 1] = pDoc->DocViewFrame[viewDoc - 1];
			else
			   frame[viewDoc - 1] = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
		     }
		}
     }
}
