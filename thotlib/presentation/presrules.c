/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
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
#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "picture.h"
#include "appdialogue.h"
#include "svgedit.h"

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"

#include "applicationapi_f.h"
#include "tree_f.h"
#include "structcreation_f.h"
#include "content.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "appdialogue_f.h"
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
#include "nodialog.h"
#include "nodialog_f.h"

/*----------------------------------------------------------------------
  AttrValue retourne la valeur que prend l'attribut numerique	
  pointe' par pAttr.					
  ----------------------------------------------------------------------*/
int AttrValue (PtrAttribute pAttr)
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
static void FollowNotPres (PtrAbstractBox * pAbb)
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
static void AncestorAbsBox (PtrElement pE, DocViewNumber view,
                            PtrAbstractBox *pAbb, PtrElement *pElAsc)
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
static PtrAbstractBox AbsBoxInherit (PtrPRule pPRule, PtrElement pEl,
                                     DocViewNumber view)
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
        while (!pElInherit->ElTerminal && pElInherit->ElFirstChild &&
               pAbb == NULL)
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
static PtrAbstractBox AbsBoxInheritImm (PtrPRule pPRule, PtrElement pEl,
                                        DocViewNumber view)
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
static char  BorderStyleCharValue (int borderStyle)
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
static int BorderStyleIntValue (char borderStyle)
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
char CharRule (PtrPRule pPRule, PtrElement pEl, DocViewNumber view,
               ThotBool *ok)
{
  PtrAbstractBox      pAbb;
  char                val;

  val = SPACE;
  *ok = TRUE;
  if (pPRule && pEl)
    {
      switch (pPRule->PrPresMode)
        {
        case PresFunction:
          break;
        case PresImmediate:
          if (pPRule->PrType == PtFont)
            {
              if (pPRule->PrChrValue == 't' ||
                  pPRule->PrChrValue == 'T')
                val = (char)1;
              else if (pPRule->PrChrValue == 'h' ||
                       pPRule->PrChrValue == 'H')
                val = (char)2;
              else if (pPRule->PrChrValue == 'c' ||
                       pPRule->PrChrValue == 'C')
                val = (char)3;
              else
                val = (char)1;
            }
          else
            val = pPRule->PrChrValue;
          break;
        case PresCurrentColor:
        case PresInherit:
          pAbb = AbsBoxInherit (pPRule, pEl, view);
          if (pAbb == NULL)
            *ok = FALSE;
          else
            switch (pPRule->PrType)
              {
              case PtFont:
                val = (char)(pAbb->AbFont);
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
              case PtVariant:
                if (pAbb->AbFontVariant == 1)	/* Normal */
                  val = 'N';
                else if (pAbb->AbFontVariant == 2)	/* SmallCaps */
                  val = 'C';
                else if (pAbb->AbFontVariant == 3)	/* DoubleStruck */
                  val = 'D';
                else if (pAbb->AbFontVariant == 4)	/* Fraktur */
                  val = 'F';
                else if (pAbb->AbFontVariant == 5)	/* Script */
                  val = 'S';
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
              case PtDirection:
                val = pAbb->AbDirection;
                break;
              case PtUnicodeBidi:
                val = pAbb->AbUnicodeBidi;
                break;
              case PtLineStyle:
                val = pAbb->AbLineStyle;
                break;
              case PtDisplay:
                val = pAbb->AbDisplay;
                break;
              case PtListStyleType:
                val = pAbb->AbListStyleType;
                break;
              case PtListStylePosition:
                val = pAbb->AbListStylePosition;
                break;
              case PtListStyleImage:
                val = pAbb->AbListStyleImage;
                break;
              case PtFloat:
                val = pAbb->AbFloat;
                break;
              case PtClear:
                val = pAbb->AbClear;
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
              case PtFillRule:
                val = pAbb->AbFillRule;
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
  Au retour, ok indique si l'evaluation a pu etre faite.	
  ----------------------------------------------------------------------*/
static BAlignment AlignRule (PtrPRule pPRule, PtrElement pEl,
                             DocViewNumber view, ThotBool *ok)
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
        case PresCurrentColor:
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
ThotBool BoolRule (PtrPRule pPRule, PtrElement pEl, DocViewNumber view,
                   ThotBool *ok)
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
        case PresCurrentColor:
          pAbb = AbsBoxInherit (pPRule, pEl, view);
          if (pAbb == NULL)
            *ok = FALSE;
          else if (pPRule->PrType == PtHyphenate)
            val = pAbb->AbHyphenate;
          break;
        case PresFunction:
          break;
        case PresImmediate:
          if (pPRule->PrType == PtHyphenate ||
              pPRule->PrType == PtVertOverflow ||
              pPRule->PrType == PtHorizOverflow ||
              pPRule->PrType == PtGather ||
              pPRule->PrType == PtPageBreak ||
              pPRule->PrType == PtLineBreak)
            val = pPRule->PrBoolValue;
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
static PtrAttribute GetEnclosingAttr (PtrElement pEl, int attrNumber,
                                      PtrAttribute pAInit)
{
  PtrAttribute        pAttr, pA;
  PtrElement	       pAsc;

  pAttr = NULL;
  if (attrNumber < 0)
    attrNumber = -attrNumber;
  if (pAInit != NULL &&
      pAInit->AeAttrNum == attrNumber &&
      !strcmp (pAInit->AeAttrSSchema->SsName, pEl->ElStructSchema->SsName))
    pAttr = pAInit;
  else
    {
      pAsc = pEl;
      while (pAttr == NULL && pAsc != NULL)
        {
          pA = pAsc->ElFirstAttr;
          while (pAttr == NULL && pA != NULL)
            if (pA->AeAttrNum == attrNumber &&
                !strcmp (pA->AeAttrSSchema->SsName, pEl->ElStructSchema->SsName))
              pAttr = pA;
            else
              pA = pA->AeNext;
          pAsc = pAsc->ElParent;
        }
    }
  return pAttr;
}

/*----------------------------------------------------------------------
  GetElementByUrl
  ----------------------------------------------------------------------*/
static PtrElement GetElementByUrl (PtrElement pEl, PtrDocument pDoc, char* Url)
{
  PtrElement     referred;
  PtrElement     pElAttr;
  PtrAttribute   pAttr;
  int            attrType;

  referred = NULL;
  if (Url[0] == '#') /* handles only internal links */
    {
      /* start from the document root and search forward an element with an id
         attribute */
      pElAttr = pDoc->DocDocElement;
      attrType = GetAttrWithException (ExcCssId, pEl->ElStructSchema);
      do
	{
	  pElAttr = FwdSearch2Attributes (pElAttr, 0, NULL, attrType, 0,
					  pEl->ElStructSchema, NULL);
	  if (pElAttr)
	    /* this element has an id attribute */
	    {
	      /* get this attribute */
	      pAttr = pElAttr->ElFirstAttr;
	      if (pAttr)
		do
		  if (pAttr->AeAttrNum == attrType &&
		      !strcmp (pAttr->AeAttrSSchema->SsName,
			       pEl->ElStructSchema->SsName) &&
		      StringAndTextEqual (&Url[1], pAttr->AeAttrText))
		    /* the expected attribute */
		    referred = pElAttr;
		  else
		    pAttr = pAttr->AeNext;
		while (pAttr && !referred);
	    }
	}
      while (!referred && pElAttr);
    }
  return referred;
}

/*----------------------------------------------------------------------
  ApplyFillUrl
  Apply a CSS rule "fill: url(...)" to element pEl in document pDoc.
  Url contains the url of the paint server to be applied.
  Return the referred paint server element if it has been found.
  ----------------------------------------------------------------------*/
static PtrElement ApplyFillUrl (PtrElement pEl, PtrDocument pDoc, char* Url)
{
  PtrElement     paintServer = NULL;
#ifdef _GL

  /* look in document pDoc for an element with an id attribute with the same
     value as Url */
  if (!pEl || !Url || !pDoc)
    return NULL;
  paintServer = GetElementByUrl (pEl, pDoc, Url);
  if (paintServer)
    /* the referred paint server has been found */
    {
      if (paintServer->ElGradient && paintServer->ElGradientDef)
	/* it's a gradient paint server */
	{
	  /* make a reference from element pEl to the paint server */
	  pEl->ElGradient = paintServer->ElGradient;
	  pEl->ElGradientDef = FALSE;
	}
      else
	/* only gradients are handled in this version @@@@ */
	paintServer = NULL;
    }
#endif /* _GL */
  return paintServer;
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
  If the abstract box for which the rule is evaluated exists,
  it's pAb. Otherwise, pAb is NULL.
  ----------------------------------------------------------------------*/
int IntegerRule (PtrPRule pPRule, PtrElement pEl, DocViewNumber view,
                 ThotBool *ok, TypeUnit *unit, PtrAttribute pAttr,
		 PtrAbstractBox pAb, PtrPSchema pSchP, PtrDocument pDoc)
{
  PtrAbstractBox      pAbb;
  PtrElement          pElInherit;
  int                 val, i, sign;

  val = 0;
  *ok = TRUE;
  *unit = UnRelative;
  if (pPRule && pEl)
    {
      switch (pPRule->PrPresMode)
        {
        case PresInherit:
        case PresCurrentColor:
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
                  if (!pPRule->PrInhPercent &&
                      (pPRule->PrInhUnit == UnRelative ||
                       pPRule->PrInhUnit == UnXHeight))
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
                        pPRule->PrType == PtBorderLeftWidth ||
                        pPRule->PrType == PtXRadius ||
                        pPRule->PrType == PtYRadius ||
                        pPRule->PrType == PtTop ||
                        pPRule->PrType == PtRight ||
                        pPRule->PrType == PtBottom ||
                        pPRule->PrType == PtLeft ||
                        pPRule->PrType == PtBackgroundHorizPos ||
                        pPRule->PrType == PtBackgroundVertPos ||
                        pPRule->PrType == PtOpacity ||
                        pPRule->PrType == PtFillOpacity ||
                        pPRule->PrType == PtStrokeOpacity ||
                        pPRule->PrType == PtStopOpacity)
                      /* convertit en 1/10 de caractere */
                      /* ou en milliemes pour l'opacite' */
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
                  *unit = pAbb->AbSizeUnit;
                  if (pPRule->PrInhPercent)
                    /* a percentage is applied to the inherited value */
                    {
                      if (*unit == UnRelative)
                        /* convert the relative inherited value into points*/
                        {
                          val = (ThotFontPointSize (pAbb->AbSize) * i) / 100;
                          *unit = UnPoint;
                        }
                      else
                        val = (pAbb->AbSize * i) / 100;
                    }
                  else if (pPRule->PrInhUnit == *unit || i == 0)
                    /* same units, just add values */
                    val = pAbb->AbSize + i;
                  else if (*unit == UnRelative)
                    /* the inherited value is relative, but the delta is
                       absolute. Convert the inherited value */
                    {
                      val = ThotFontPointSize (pAbb->AbSize);
                      if (pPRule->PrInhUnit == UnPoint)
                        val = val + i;
                      else
                        val = val + PixelToPoint (i);
                      *unit = UnPoint;
                    }
                  else if (*unit == UnPoint)
                    /* the inherited value is in points */
                    {
                      if (pPRule->PrInhUnit == UnRelative)
                        /* a increment of 1 relative unit equals 20% */
                        val = pAbb->AbSize + ((pAbb->AbSize * i * 20) / 100);
                      else if (pPRule->PrInhUnit == UnPixel)
                        /* delta is in pixels. Convert it to points */
                        val = pAbb->AbSize + PixelToPoint (i);
                    }
                  else if (*unit == UnPixel)
                    /* the inherited value is in pixels */
                    {
                      if (pPRule->PrInhUnit == UnRelative)
                        /* a increment of 1 relative unit equals 20% */
                        val = pAbb->AbSize + ((pAbb->AbSize * i * 20) / 100);
                      else if (pPRule->PrInhUnit == UnPoint)
                        /* delta is in pixels. Convert it to pixels */
                        val = pAbb->AbSize + PointToPixel (i);
                    }
                  else if (*unit == UnPercent)
                    /* the inherited value is a percentage */
                    {
                      if (i < 0)
                        {
                          sign = -1;
                          i = -i;
                        }
                      else
                        sign = 1;
                      if (pPRule->PrInhUnit == UnPixel)
                        i = FontRelSize (PixelToPoint (i));
                      else if (pPRule->PrInhUnit == UnPoint)
                        i = FontRelSize (i);
                      val = pAbb->AbSize + (sign * i * 20);
                    }

                  if (*unit == UnRelative)
                    {
                      if (val > MAX_LOG_SIZE)
                        val = MAX_LOG_SIZE;
                      else if (val < 0)
                        val = 0;
                    }
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
                    val = pAbb->AbDepth;
                  else
                    val = pAbb->AbDepth + i;
                  break;
                case PtFillPattern:
                  val = pAbb->AbFillPattern;
                  break;
                case PtOpacity:
                  val = pAbb->AbOpacity + i;
                  if (val > 1000)
                    val = 1000;
                  else if (val < 0)
                    val = 0;
                  break;
                case PtFillOpacity:
                  val = pAbb->AbFillOpacity + i;
                  if (val > 1000)
                    val = 1000;
                  else if (val < 0)
                    val = 0;
                  break;
                case PtStrokeOpacity:
                  val = pAbb->AbStrokeOpacity + i;
                  if (val > 1000)
                    val = 1000;
                  else if (val < 0)
                    val = 0;
                  break;
                case PtStopOpacity:
                  val = pAbb->AbStopOpacity + i;
                  if (val > 1000)
                    val = 1000;
                  else if (val < 0)
                    val = 0;
                  break;
                case PtBackground:
		  if (pPRule->PrPresMode == PresCurrentColor)
		    val = pAbb->AbColor;
		  else
		    {
		      val = pAbb->AbBackground;
		      if (pAbb->AbGradientBackground)
			*unit = UnGradient;
		    }
                  break;
                case PtForeground:
		  if (pPRule->PrPresMode == PresCurrentColor)
		    val = pAbb->AbColor;
		  else
                    val = pAbb->AbForeground;
                  break;
                case PtColor:
                  val = pAbb->AbColor;
                  break;
                case PtStopColor:
		  if (pPRule->PrPresMode == PresCurrentColor)
		    val = pAbb->AbColor;
		  else
		    val = pAbb->AbStopColor;
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
                case PtXRadius:
                  val = pAbb->AbRx;
                  *unit = pAbb->AbRxUnit;
                  break;
                case PtYRadius:
                  val = pAbb->AbRy;
                  *unit = pAbb->AbRyUnit;
                  break;
                case PtTop:
                  if (pAb->AbLeafType != LtCompound ||
                      pAbb->AbPositioning == NULL)
                    {
                      val = 0;
                      *unit = UnUndefined;
                    }
                  else
                    {
                      val = pAbb->AbPositioning->PnTopDistance;
                      *unit = pAbb->AbPositioning->PnTopUnit;
                    }
                  break;
                case PtRight:
                  if (pAb->AbLeafType != LtCompound ||
                      pAbb->AbPositioning == NULL)
                    {
                      val = 0;
                      *unit = UnUndefined;
                    }
                  else
                    {
                      val = pAbb->AbPositioning->PnRightDistance;
                      *unit = pAbb->AbPositioning->PnRightUnit;
                    }
                  break;
                case PtBottom:
                  if (pAb->AbLeafType != LtCompound ||
                      pAbb->AbPositioning == NULL)
                    {
                      val = 0;
                      *unit = UnUndefined;
                    }
                  else
                    {
                      val = pAbb->AbPositioning->PnBottomDistance;
                      *unit = pAbb->AbPositioning->PnBottomUnit;
                    }
                  break;
                case PtLeft:
                  if (pAb->AbLeafType != LtCompound ||
                      pAbb->AbPositioning == NULL)
                    {
                      val = 0;
                      *unit = UnUndefined;
                    }
                  else
                    {
                      val = pAbb->AbPositioning->PnLeftDistance;
                      *unit = pAbb->AbPositioning->PnLeftUnit;
                    }
                  break;
                case PtBackgroundHorizPos:
                case PtBackgroundVertPos:
                  /* to be written @@@@ */
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
                            pPRule->PrType == PtLineWeight ||
                            pPRule->PrType == PtOpacity ||
                            pPRule->PrType == PtFillOpacity ||
                            pPRule->PrType == PtStrokeOpacity ||
                            pPRule->PrType == PtStopOpacity)
                          /* convertit en 1/10 de caractere ou en milliemes
                             pour l'opacite' */
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
                    if (pPRule->PrType == PtSize &&
                        pAbb->AbSizeUnit == UnRelative)
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
                    if (pPRule->PrType == PtSize &&
			pAbb->AbSizeUnit == UnRelative)
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
              pPRule->PrType == PtColor ||
              pPRule->PrType == PtStopColor ||
              pPRule->PrType == PtBorderTopColor ||
              pPRule->PrType == PtBorderRightColor ||
              pPRule->PrType == PtBorderBottomColor ||
              pPRule->PrType == PtBorderLeftColor)
            {
              if (pPRule->PrValueType == PrAttrValue)
                /* c'est la valeur d'un attribut */
                {
                  pAttr = GetEnclosingAttr (pEl, pPRule->PrIntValue, pAttr);
                  if (pPRule->PrIntValue < 0)
                    /* il faut inverser cette valeur */
                    val = -AttrValue (pAttr);
                  else
                    val = AttrValue (pAttr);
                }
              else if (pPRule->PrValueType == PrNumValue)
                /* c'est la valeur elle meme qui est dans la regle */
                val = pPRule->PrIntValue;
	      else if (pPRule->PrValueType == PrConstStringValue)
		{
		  if (pPRule->PrType == PtBackground)
		    /* the rule contains the number of the constant (in the
		       presentation schema) that contains the url of the
		       gradient to be painted in the background */
		    {
		      val = pPRule->PrIntValue;
		      *unit = UnGradient;
		    }
		  else
		    *ok = FALSE;
		}
	      else
		*ok = FALSE;
            }
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
                   pPRule->PrType == PtBorderLeftWidth ||
                   pPRule->PrType == PtXRadius ||
                   pPRule->PrType == PtYRadius ||
                   pPRule->PrType == PtTop ||
                   pPRule->PrType == PtRight ||
                   pPRule->PrType == PtBottom ||
                   pPRule->PrType == PtLeft ||
                   pPRule->PrType == PtBackgroundHorizPos ||
                   pPRule->PrType == PtBackgroundVertPos)
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
          if (pPRule->PrType == PtOpacity || 
              pPRule->PrType == PtFillOpacity ||
              pPRule->PrType == PtStrokeOpacity ||
              pPRule->PrType == PtStopOpacity)
            {
              if (pPRule->PrValueType == PrAttrValue)
                /* C'est la valeur d'un attribut */
                {
                  pAttr = GetEnclosingAttr (pEl, pPRule->PrIntValue, pAttr);
                  /* the attribute value is supposed to be a percentage, but
                     the value in the abstract box is in thousandth */
                  val = AttrValue (pAttr) * 10;
                }
              else if (pPRule->PrValueType == PrNumValue)
                /* c'est la valeur elle-meme qui est dans la regle */
                val = pPRule->PrIntValue;
	      else
		val = 1000;
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
  MarkerRule
  Applies a marker* presentation rule.
  Returns the marker element to be associated with element pEl when applying
  rule pPRule from presentation schema pSchP.
  ----------------------------------------------------------------------*/
static PtrElement MarkerRule (PtrPRule pPRule, PtrElement pEl,
			      DocViewNumber view, ThotBool *ok,
			      PtrPSchema pSchP, PtrDocument pDoc)
{
  PtrElement        marker;
  PtrAbstractBox    pAbb;
  PresConstant	    *pConst;

  marker = NULL;
  *ok = TRUE;
  if (pPRule && pEl)
    /* do not associate a marker with a copy of a marker */
    if (!TypeHasException (ExcIsMarker, pEl->ElTypeNumber, pEl->ElStructSchema))
      {
	if (pPRule->PrPresMode == PresInherit)
	  {
	    pAbb = AbsBoxInherit (pPRule, pEl, view);
	    if (pAbb == NULL)
	      *ok = FALSE;
	    else
	      switch (pPRule->PrType)
		{
                case PtMarker:
                  marker = pAbb->AbMarker;
		  break;
                case PtMarkerStart:
                  marker = pAbb->AbMarkerStart;
		  break;
                case PtMarkerMid:
                  marker = pAbb->AbMarkerMid;
		  break;
                case PtMarkerEnd:
                  marker = pAbb->AbMarkerEnd;
		  break;
	        default:
		  marker = NULL;
		  break;
		}
	  }
	else if (pPRule->PrPresMode == PresImmediate)
	  {
	    if (pPRule->PrValueType == PrNumValue && pPRule->PrIntValue == 0)
	      marker = NULL;
	    else if (pPRule->PrValueType == PrConstStringValue)
	      /* the rule contains the number of the constant (in the
		 presentation schema) that contains the url of the
		 marker element to be used */
	      {
		pConst = &pSchP->PsConstant[pPRule->PrIntValue - 1];
		if (pConst->PdString && pConst->PdString[0] != EOS)
		  marker = GetElementByUrl (pEl, pDoc, pConst->PdString);
	      }
	    else
	      *ok = FALSE;
	  }
	else
	  *ok = FALSE;
      }
  return marker;
}

/*----------------------------------------------------------------------
  VerifyAbsBox : Teste si le pave pAb a les caracteristiques
  numAbType (type du pave) et refKind (pave de presentation
  ou d'element) si notType est faux ou s'il n'a pas les
	caracteristiques numAbType et refKind si notType est vrai.
  Cela permet de determiner le pave pAb par rapport	
  auquel le pave en cours de traitement va se positionner
  ----------------------------------------------------------------------*/
static void VerifyAbsBox (ThotBool *found, PtrPSchema pSP, RefKind refKind,
                          int numAbType, ThotBool notType, PtrAbstractBox pAb)
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
              if (pSP && pAttr->AeAttrNum == numAbType &&
                  !strcmp (pAttr->AeAttrSSchema->SsName, pSP->PsStructName))
                attrFound = TRUE;
              else
                pAttr = pAttr->AeNext;
          }
      if (notType)
        /* on accepte le pave s'il est de type different de numAbType */
        {
          if (refKind == RkAnyElem)
            {
              /* C'est une regle Not AnyElem, on accepte la premiere */
              /* boite de presentation trouvee */
              if (pAb->AbPresentationBox)
                *found = TRUE;
            }
          else if (refKind == RkElType)
            /* c'est une regle Not Type */
            {
              if (pAb->AbTypeNum != numAbType ||
                  pAb->AbPresentationBox ||
                  pAb->AbPSchema != pSP)
                *found = TRUE;
            }
          else if (refKind == RkAnyBox)
            {
              /* Cas d'une regle Not AnyBox, on accepte le premier */
              /* element trouve' */
              if (!pAb->AbPresentationBox)
                *found = TRUE;
            }
          else if (refKind == RkPresBox)
            {
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
          if (refKind == RkAnyElem)
            {
              /* C'est une regle AnyElem, on accepte le premier element
                 trouve' */
              if (!pAb->AbPresentationBox)
                *found = TRUE;
            }
          else if (refKind == RkElType)
            /* C'est une regle Type */
            {
              if (pAb->AbTypeNum == numAbType &&
                  !pAb->AbPresentationBox &&
                  (pAb->AbPSchema == pSP || pSP == NULL))
                *found = TRUE;
            }
          else if (refKind == RkAnyBox)
            {
              /* C'est une regle AnyBox, on accepte la premiere boite de */
              /* presentation trouvee */
              if (pAb->AbPresentationBox)
                *found = TRUE;
            }
          else if (refKind == RkPresBox)
            {
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
  AttrCreatePresBox
  retourne Vrai si l'une des regles de presentation de l'attribut pointe'
  par pAttr cree le pave de presentation pointe' par pAb.
  ----------------------------------------------------------------------*/
static ThotBool AttrCreatePresBox (PtrAttribute pAttr, PtrAbstractBox pAb,
                                   PtrDocument pDoc)
{
  ThotBool            ret, stop;
  int                 valNum, match;
  PtrPRule            pPRule;
  PtrPSchema          pSchP;
  PtrHandlePSchema    pHd;
  PtrAttributePres    attrBlock;

  ret = FALSE;
  if (pAb->AbPresentationBox)
    {
      /* on cherchera d'abord dans le schema de presentation principal de */
      /* l'attribut */
      pSchP = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
      pHd = NULL;
      /* on examine le schema de presentation principal, puis les schemas */
      /* additionnels */
      while (pSchP != NULL && !ret)
        {
          /* process all values of the attribute, in case of a text attribute
             with multiple values */
          valNum = 1; match = 1;
          do
            {
              pPRule = AttrPresRule (pAttr, pAb->AbElement, FALSE, NULL, pSchP,
                                     &valNum, &match, &attrBlock);
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
                          /* cette regle cree notre pave, on a trouve' */
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
            }
          while (valNum > 0 && !ret);

          if (!ret)
            /* on n'a pas encore trouve'. On cherche dans les schemas de */
            /* presentation additionnels */
            {
              if (pHd)
                /* get next extension schema */
                pHd = pHd->HdNextPSchema;
              else if (CanApplyCSSToElement (pAb->AbElement))
                /* get first extension schema */
                pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc,
                                             pAb->AbElement);

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
static void VerifyAbsBoxDescent (ThotBool *found, PtrPSchema pSP,
                                 RefKind refKind, int numType,
                                 ThotBool notType, PtrAbstractBox pAb)
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
  definie dans le schema de presentation pointe' par pSP.
  si refKind == RkAttr, c'est un numero d'attribut.
  Si level vaut RlReferred, on cherche parmi les paves de	
  l'element designe' par l'attribut reference pointe'	
  par pAttr.						
  Au result, la fonction rend NULL si aucun pave ne	
  correspond.						
  ----------------------------------------------------------------------*/
static PtrAbstractBox SearchAbsBoxRef (ThotBool notType, int numAbType,
                                       PtrPSchema pSP, Level level,
                                       RefKind refKind, PtrAbstractBox pAbb,
                                       PtrAttribute pAttr, PtrDocument pDoc)
{
  ThotBool            found;
  PtrAbstractBox      pAb;
  PtrAbstractBox      pAbbMain;
  int                 view;

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
                {
                  if (pAb->AbDead)
                    pAb = pAb->AbNext;
                  else
                    found = TRUE;
                }
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
                {
                  if (pAb->AbDead)
                    pAb = pAb->AbPrevious;
                  else
                    found = TRUE;
                }
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
                {
                  if (pAb->AbDead)
                    pAb = pAb->AbNext;
                  else
                    found = TRUE;
                }
              else
                {
                  VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
                  if (!found)
                    pAb = pAb->AbNext;
                }
            while (pAb != NULL && !found);
          break;
	  
        case RlLastSibling:
          pAb = pAb->AbNext;
          if (pAb != NULL)
            {
              do
                if (pAb->AbNext)
                  pAb = pAb->AbNext;
                else
                  found = TRUE;
              while (pAb && !found);
              found = FALSE;
              if (pAb)
                do
                  if (pAb == pAbb)
                    pAb = NULL;
                  else if (pAb->AbDead)
                    pAb = pAb->AbPrevious;
                  else
                    found = TRUE;
                while (pAb != NULL && !found);
            }
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
          if (pAb->AbEnclosing && pAb->AbEnclosing->AbElement == pAb->AbElement &&
              !pAb->AbEnclosing->AbPresentationBox)
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
          if (pAb && pAb->AbDead)
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
                      if (AttrCreatePresBox (pAttr, pAbb, pDoc))
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
          if (pAttr && pAttr->AeAttrType == AtReferenceAttr &&
              pAttr->AeAttrReference &&
              pAttr->AeAttrReference->RdReferred &&
              pAttr->AeAttrReference->RdReferred->ReReferredElem != NULL)
            pAb = pAttr->AeAttrReference->RdReferred->ReReferredElem->ElAbstractBox[view - 1];
          pAbbMain = NULL;
          if (pAb != NULL)
            /* cherche en avant le pave demande */
            do
              if (numAbType == 0)
                {
                  if (pAb->AbDead)
                    {
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
                    }
                  else
                    found = TRUE;
                }
              else
                {
                  VerifyAbsBox (&found, pSP, refKind, numAbType, notType, pAb);
                  if (!found)
                    {
                      if (!pAb->AbPresentationBox)
                        pAbbMain = pAb;
                      if (pAb->AbNext != NULL)
                        {
                          if (pAb->AbNext->AbElement == pAb->AbElement)
                            pAb = pAb->AbNext;
                          else
                            pAb = NULL;
                        }
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
void GetConstantBuffer (PtrAbstractBox pAb)
{
  PtrTextBuffer       pBT;

  GetTextBuffer (&pBT);
  pAb->AbText = pBT;
  if (pAb->AbLeafType == LtText)
    pAb->AbLang = TtaGetDefaultLanguage ();
  pAb->AbVolume = 0;
}

/*----------------------------------------------------------------------
  UpdateFreeVol	met a jour le volume libre restant dans la vue	
  du pave pAb, en prenant en compte le volume de ce	
  nouveau pave feuille.					
  ----------------------------------------------------------------------*/
void UpdateFreeVol (PtrAbstractBox pAb, PtrDocument pDoc)
{
  pDoc->DocViewFreeVolume[pAb->AbDocView - 1] -= pAb->AbVolume;
}

/*----------------------------------------------------------------------
  FillContent met dans le pave pointe par pAb le contenu de l'element
  feuille pointe par pEl.					
  ----------------------------------------------------------------------*/
void FillContent (PtrElement pEl, PtrAbstractBox pAb, PtrDocument pDoc)
{
  char               *text;
  int                 lg, i;
  PtrTextBuffer       pBu1;
  PtrReference        pPR1;
  PtrPRule            pRSpec;
  ThotBool            rxRule, ryRule;

  if (pEl->ElHolophrast)
    {
      pAb->AbLeafType = LtText;
      GetConstantBuffer (pAb);
      pBu1 = pAb->AbText;
      CopyStringToBuffer ((unsigned char *)"<", pBu1, &lg);
      CopyStringToBuffer ((unsigned char *)pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber-1]->SrName,
                          pBu1, &i);
      lg += i;
      CopyStringToBuffer ((unsigned char *)">", pBu1, &i);
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
          if (pAb->AbElement->ElPictInfo)
            pAb->AbPictInfo = pAb->AbElement->ElPictInfo;
          else
            {
              /* initialize the new image context */
              lg = ustrlen (pEl->ElText->BuContent) * 2 + 1;
              text = (char *)TtaGetMemory (lg);
              CopyBuffer2MBs (pEl->ElText, 0, (unsigned char *)text, lg);
              NewPictInfo (pAb, text, UNKNOWN_FORMAT, False);
              TtaFreeMemory(text);
            }
          pAb->AbVolume = pEl->ElTextLength;
          break;
        case LtText:
          /* saute les paves crees par FnCreateBefore */
          while (pAb->AbText != NULL && pAb->AbNext != NULL)
            pAb = pAb->AbNext;
          /* prend le contenu de l'element correspondant */
          pAb->AbText = pEl->ElText;
          pAb->AbLang = pEl->ElLanguage;
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
          if (pEl->ElLeafType == LtGraphics &&
	      (pEl->ElGraph == 1 || pEl->ElGraph == 'C'))
            /* rectangle with rounded corners */
            {
	      /* check specific presentation rules related to rounded corners */
              rxRule = FALSE;  ryRule = FALSE;
	      pRSpec = pEl->ElFirstPRule;
	      while (pRSpec)
		{
		  if (pRSpec->PrType == PtXRadius)
		    rxRule = TRUE;
		  if (pRSpec->PrType == PtYRadius)
		    ryRule = TRUE;
		  pRSpec = pRSpec->PrNextPRule;
		}
	      if (rxRule && !ryRule)
		{
		  pAb-> AbRx = 0;
		  pAb-> AbRy = -1;
		}
	      else if (ryRule && !rxRule)
		{
		  pAb-> AbRx = -1;
		  pAb-> AbRy = 0;
		}
	      else
		{
		  pAb-> AbRx = 0;
		  pAb-> AbRy = 0;
		}
            }
          pAb->AbGraphScript = 'G';
          if (pAb->AbShape == EOS)
            pAb->AbVolume = 0;
          else
            pAb->AbVolume = 1;
          break;
        case LtPath:
          /* prend le contenu de l'element correspondant */
          pAb->AbFirstPathSeg = pEl->ElFirstPathSeg;
          pAb->AbVolume = pEl->ElVolume;
          break;
        case LtReference:
          pAb->AbLeafType = LtText;
          GetConstantBuffer (pAb);
          pBu1 = pAb->AbText;
          pBu1->BuContent[0] = TEXT('[');
          lg = 2;
          pBu1->BuContent[lg - 1] = TEXT('?');
          /* la reference pointe sur rien */
          if (pEl->ElReference != NULL)
            {
              pPR1 = pEl->ElReference;
              if (pPR1->RdReferred &&
                  !IsASavedElement (pPR1->RdReferred->ReReferredElem))
                /* l'element reference' n'est pas dans le */
                /* buffer des elements coupe's */
                pBu1->BuContent[lg - 1] = TEXT('*');
              lg++;
              pBu1->BuContent[lg - 1] = TEXT(']');
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
          if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrFirstOfPair)
            {
              pBu1->BuContent[0] = TEXT('<');
              pBu1->BuContent[1] = TEXT('<');
            }
          else
            {
              pBu1->BuContent[0] = TEXT('>');
              pBu1->BuContent[1] = TEXT('>');
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
static ThotBool PageCreateRule (PtrPRule pPRule, PtrPSchema pSPR,
                                PtrAbstractBox pCree,
                                FunctionType *TypeCreation)
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
FunctionType TypeCreatedRule (PtrDocument pDoc, PtrAbstractBox pAbbCreator,
                              PtrAbstractBox pAbbCreated)
{
  PtrPRule            pPRuleCre;
  FunctionType        result;
  PtrPSchema          pSPR;
  PtrAttribute        pAttr;
  PtrAttribute        pA;
  int                 valNum, match;
  ThotBool            ok;
  PtrPSchema          pSchP;
  PtrHandlePSchema    pHd;
  PtrAttributePres    attrBlock;

  result = FnLine;
  /* cherche les regles de creation en ignorant les attributs */
  pPRuleCre = SearchRulepAb (pDoc, pAbbCreator, &pSPR, PtFunction, FnAny,
                             FALSE, &pAttr);
  if (!PageCreateRule (pPRuleCre, pSPR, pAbbCreated, &result))
    /* on n'a pas found la regle qui cree la bonne boite */
    /* on cherche les regles de creation associees aux attributs */
    /* de l'element createur */
    {
      ok = FALSE;
      pA = pAbbCreator->AbElement->ElFirstAttr;
      while (pA != NULL && !ok)
        {
          pSchP = PresentationSchema (pA->AeAttrSSchema, pDoc);
          pHd = NULL;
          /* on examine le schema de presentation principal, puis les schemas*/
          /* additionnels */
          while (pSchP != NULL && !ok)
            {
              /* cherche la premiere regle de presentation pour cet */
              /* attribut dans ce schema de presentation */
              /* process all values of the attribute, in case of a text
                 attribute with multiple values */
              valNum = 1; match = 1;
              do
                {
                  pPRuleCre = AttrPresRule (pA, pAbbCreator->AbElement, FALSE,
                                     NULL, pSchP, &valNum, &match, &attrBlock);
                  ok = PageCreateRule (pPRuleCre, pSchP, pAbbCreated, &result);
                }
              while (valNum > 0);

              if (pHd)
                /* get next extension schema */
                pHd = pHd->HdNextPSchema;
              else if (CanApplyCSSToElement (pAbbCreator->AbElement))
                /* get first extension schema */
                pHd = FirstPSchemaExtension (pA->AeAttrSSchema, pDoc,
                                             pAbbCreator->AbElement);

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
void SearchPresSchema (PtrElement pEl, PtrPSchema *pSchP, int *indexElType,
                       PtrSSchema *pSchS, PtrDocument pDoc)
{
  ThotBool            found;
  int                 i;
  PtrSSchema          pSc1;
  PtrPSchema          pSP;
  PtrSRule            pSRule;

  if (pEl == NULL || pEl->ElStructSchema == NULL)
    {
      *pSchP = NULL;
      *pSchS = NULL;
      *indexElType = 0;
    }
  else
    {
      *pSchS = pEl->ElStructSchema;
      *pSchP = PresentationSchema (*pSchS, pDoc);
      /* premiere regle de presentation specifique a ce type d'element */
      *indexElType = pEl->ElTypeNumber;
      /* s'il s'agit de l'element racine d'une nature, on prend les regles */
      /* de presentation (s'il y en a) de la regle nature dans la structure */
      /* englobante. */
      /* on ne traite pas les marques de page */
      if ((!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak) &&
           *indexElType == (*pSchS)->SsRootElem)
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
                      pSRule = pSc1->SsRule->SrElem[i - 1];
                      if (pSRule->SrConstruct == CsNatureSchema)
                        if (pSRule->SrSSchemaNat == pEl->ElStructSchema)
                          found = TRUE;
                    }
                  while (!found && i < pSc1->SsNRules);
                  if (found)
                    {
                      pSP = PresentationSchema (pSc1, pDoc);
                      if (pSP && i <= pSP->PsNElemPRule &&
                          pSP->PsElemPRule->ElemPres[i - 1])
                        /* il y a des regles de presentation pour ce type */
                        {
                          *pSchP = pSP;
                          *indexElType = i;
                          *pSchS = pEl->ElParent->ElStructSchema;
                        }
                    }
                }
            }
    }
}

/*----------------------------------------------------------------------
  CheckPPosUser							
  ----------------------------------------------------------------------*/
static ThotBool CheckPPosUser (PtrAbstractBox pAb, PtrDocument pDoc)
{
  return (IsAbstractBoxDisplayed (pAb, pDoc->DocViewFrame[pAb->AbDocView - 1]));
}

/*----------------------------------------------------------------------
  ApplyPos applies the pos rule pRule to the abstract box pAb.
  The parameter pRefAb points to the abstract box that submits the apply.
  Returns TRUE in appl if the rule is applied.
  ----------------------------------------------------------------------*/
static void ApplyPos (AbPosition *PPos, PosRule *positionRule, PtrPRule pPRule,
                      PtrAttribute pAttr, PtrPSchema pSchP, PtrAbstractBox pAbb1,
                      PtrAbstractBox pRefAb, PtrDocument pDoc, ThotBool *appl)
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
  if (pAbb1->AbElement->ElTerminal &&
      pAbb1->AbElement->ElLeafType == LtPageColBreak)
    if (pAbb1->AbLeafType != LtCompound && !pAbb1->AbPresentationBox)
      pageBreak = TRUE;
  pPosRule = positionRule;
  if (pPosRule->PoPosDef == NoEdge || pageBreak)
    /* position flottante: regle VertPos=NULL ou HorizPos=NULL */
    {
      PPos->PosEdge = NoEdge;
      PPos->PosRefEdge = NoEdge;
      PPos->PosDistance = 0;
      PPos->PosDistDelta = 0;
      PPos->PosUnit = UnRelative;
      PPos->PosDeltaUnit = UnRelative;
      PPos->PosAbRef = NULL;
      PPos->PosUserSpecified = FALSE;
      *appl = TRUE;
    }
  else
    {
      /* cherche le pave (pAbbPos) par rapport auquel le pave */
      /* traite' se positionne  */
      pAbbPos = NULL;
      /* si l'exception n'a pas ete traitee, effectue un traitement
         normal */
      if (pAbbPos == NULL)
        pAbbPos = SearchAbsBoxRef (pPosRule->PoNotRel, pPosRule->PoRefIdent,
                                   pSchP, pPosRule->PoRelation,
                                   pPosRule->PoRefKind, pAbb1, pAttr, pDoc);
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
          PPos->PosDistDelta = pPosRule->PoDistDelta;
          PPos->PosUnit = pPosRule->PoDistUnit;
          PPos->PosDeltaUnit = pPosRule->PoDeltaUnit;
          PPos->PosAbRef = pAbbPos;
          PPos->PosUserSpecified = FALSE;
          if (PPos->PosUserSpecified)
            PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
          *appl = TRUE;
          /* verifie si le pave deroge a la regle d'englobement */
          if (pPosRule->PoRelation == RlRoot &&
              PPos->PosAbRef != pAbb1->AbEnclosing)
            {
              /* ce pave deroge a la regle d'englobement */
              if (pPRule->PrType == PtHorizPos)
                pAbb1->AbHorizEnclosing = FALSE;
              else if (pPRule->PrType == PtVertPos)
                pAbb1->AbVertEnclosing = FALSE;
            }
          if (PPos->PosAbRef == pAbb1->AbEnclosing)
            /* le pave se positionne par rapport a l'englobant */
            {
              if (pPRule->PrType == PtHorizPos)
                {
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
                      if (PPos->PosDistance > 0 &&
                          PPos->PosEdge == Right && PPos->PosRefEdge == Right)
                        /* le cote droit du pave est a droite du */
                        /* cote droit de l'englobant: debordement */
                        pAbb1->AbHorizEnclosing = FALSE;
                    }
                }
              else if (pPRule->PrType == PtVertPos)
                {
                  /* regarde si le premier fils de l'englobant est un saut
                     de page */
                  pAbbParent = pAbb1->AbEnclosing;
                  if (pAbbParent->AbFirstEnclosed != pAbb1 &&
                      !pAbbParent->AbFirstEnclosed->AbDead &&
                      pAbbParent->AbFirstEnclosed->AbElement->ElTerminal &&
                      pAbbParent->AbFirstEnclosed->AbElement->ElLeafType == LtPageColBreak)
                    {
                      /* positionne par rapport au saut de page */
                      PPos->PosRefEdge = Bottom;
                      PPos->PosAbRef = pAbbParent->AbFirstEnclosed;
                      /* mise a jour de la distance */
                      /* si PPos->PosDistance est en unite relative on ne fait
                         rien sinon, PPos->PosDistance est decremente de la
                         hauteur du haut de la boite haut de page */
                      if (PPos->PosUnit == UnPoint)
                        {
                          /* on cherche la boite page correspondant a la
                             regle page portee par un des ascendants  */
                          b = GetPageBoxType (pAbbParent->AbFirstEnclosed->AbElement, pDoc,
                                              pAbbParent->AbFirstEnclosed->AbElement->ElViewPSchema,
                                              &pSchPPage);
                          PageHeaderHeight = pSchPPage->PsPresentBox->PresBox[b - 1]->PbHeaderHeight;
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
        }
      else
        /* on n'a pas trouve' le pave' de reference */
        if (pAbb1->AbLeafType != LtCompound &&
            pPosRule->PoRelation != RlNext &&
            pPosRule->PoRelation != RlLastSibling &&
            pPosRule->PoRelation != RlPrevious &&
            pPosRule->PoRelation != RlSameLevel &&
            pPosRule->PoRelation != RlCreator &&
            pPosRule->PoRelation != RlReferred)
          {
            /* inutile de reessayer d'appliquer la regle */
            /* quand les paves environnants seront crees. */
            PPos->PosEdge = pPosRule->PoPosDef;
            PPos->PosRefEdge = pPosRule->PoPosRef;
            PPos->PosDistance = pPosRule->PoDistance;
            PPos->PosDistDelta = pPosRule->PoDistDelta;
            PPos->PosUnit = pPosRule->PoDistUnit;
            PPos->PosDeltaUnit = pPosRule->PoDeltaUnit;
            PPos->PosAbRef = NULL;
            PPos->PosUserSpecified = pPosRule->PoUserSpecified;
            if (PPos->PosUserSpecified)
              PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
            *appl = TRUE;
          }
        else if (pAbb1->AbEnclosing == NULL &&
                 pPosRule->PoRelation == RlEnclosing)
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
            PPos->PosDistDelta = pPosRule->PoDistDelta;
            PPos->PosUserSpecified = FALSE;
            if (PPos->PosUserSpecified)
              PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
            PPos->PosUnit = pPosRule->PoDistUnit;
            PPos->PosDeltaUnit = pPosRule->PoDeltaUnit;
            PPos->PosAbRef = NULL;
            *appl = TRUE;
          }
        else if (pAbb1->AbEnclosing &&
                 ((pPRule->PrType == PtVertPos &&
                   pPosRule->PoRelation == RlPrevious &&
                   pPosRule->PoPosDef == Top && pPosRule->PoPosRef == Bottom) ||
                  (pPRule->PrType == PtVertPos &&
                   (pPosRule->PoRelation == RlNext ||
                    pPosRule->PoRelation == RlLastSibling) &&
                   pPosRule->PoPosDef == Bottom && pPosRule->PoPosRef == Top) ||
                  (pPRule->PrType == PtHorizPos &&
                   pPosRule->PoRelation == RlPrevious &&
                   pPosRule->PoPosDef == Left && pPosRule->PoPosRef == Right) ||
                  (pPRule->PrType == PtHorizPos &&
                   (pPosRule->PoRelation == RlNext ||
                    pPosRule->PoRelation == RlLastSibling) &&
                   pPosRule->PoPosDef == Right && pPosRule->PoPosRef == Left)))
          /* the referred next or previous abstract box is not found
             change the rule to refer the enclosing abstract box */
          {
            PPos->PosAbRef = pAbb1->AbEnclosing;
            if (pPRule->PrType == PtHorizPos &&
                !TypeHasException (ExcIsColHead, pAbb1->AbElement->ElTypeNumber,
                                  pAbb1->AbElement->ElStructSchema) &&
                pPosRule->PoPosDef == Left && pPosRule->PoPosRef == Right &&
                pAbb1->AbAdjust != AlignLeft)
              {
                /* check the text_align rule */
                if (pAbb1->AbAdjust == AlignCenter)
                  {
                    PPos->PosEdge = VertMiddle;
                    PPos->PosRefEdge = VertMiddle;
                  }
                else
                  {
                    PPos->PosEdge = Right;
                    PPos->PosRefEdge = Right;
                  }
              }
            else
              {
                PPos->PosEdge = pPosRule->PoPosDef;
                PPos->PosRefEdge = pPosRule->PoPosDef;
              }
            /* s'agit-il d'une regle de presentation specifique ? */
            pRSpec = pAbb1->AbElement->ElFirstPRule;
            while (pRSpec != NULL && pRSpec != pPRule)
              pRSpec = pRSpec->PrNextPRule;
            if (pRSpec == pPRule)
              /* c'est une regle de presentation specifique */
              /* on prend le decalage en compte */
              {
                PPos->PosDistance = pPosRule->PoDistance;
                PPos->PosDistDelta = pPosRule->PoDistDelta;
              }
            else
              /* c'est une regle generique */
              /* on se positionne tout contre l'englobant */
              {
                PPos->PosDistance = 0;
                PPos->PosDistDelta = 0;
              }

            if (pRefAb == pAbb1->AbEnclosing)
              // it's a delayed rule that cannot apply
              *appl = TRUE;
            else
              // it will be retried later 
              *appl = FALSE;
            PPos->PosUnit = pPosRule->PoDistUnit;
            PPos->PosDeltaUnit = pPosRule->PoDeltaUnit;
            PPos->PosUserSpecified = FALSE;
            if (PPos->PosUserSpecified)
              PPos->PosUserSpecified = CheckPPosUser (pAbb1, pDoc);
            pAbb1->AbVertEnclosing = TRUE;
          }
        else
          /* generate either the VertPos=NULL or the HorizPos=NULL rule */
          {
            PPos->PosEdge = NoEdge;
            PPos->PosRefEdge = NoEdge;
            PPos->PosDistance = 0;
            PPos->PosDistDelta = pPosRule->PoDistDelta;
            PPos->PosUnit = UnRelative;
            PPos->PosDeltaUnit = pPosRule->PoDeltaUnit;
            PPos->PosAbRef = NULL;
            PPos->PosUserSpecified = FALSE;
            if (pRefAb == pAbb1->AbEnclosing)
              // it's a delayed rule that cannot apply
              *appl = TRUE;
            else
              // it will be retried later 
              *appl = FALSE;
          }
    }
}


/*----------------------------------------------------------------------
  ApplyDim applies the dim rule pRule  to the abstract box pAb.		
  Returns TRUE in appl if the rule is applied.
  ----------------------------------------------------------------------*/
static void ApplyDim (AbDimension *pdimAb, PtrAbstractBox pAb,
                      PtrAbstractBox pRefAb, PtrPSchema pSchP,
                      PtrAttribute pAttr, ThotBool *appl,
                      PtrPRule pPRule, PtrDocument pDoc)
{
  PtrAbstractBox      pAbbRef;
  PtrAttribute        pA;
  ThotBool            stop;
  DimensionRule      *pDRule;
  int                 attrRule;

  *appl = FALSE;
  pDRule = &pPRule->PrDimRule;
  /* by default inherit from the contents */
  pdimAb->DimIsPosition = FALSE;
  pdimAb->DimValue = -1;
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

  if (pDRule->DrPosition)
    {
      /* Stretched box, the dimension is defined by a pos rule */
      ApplyPos (&(pdimAb->DimPosition), &(pDRule->DrPosRule), pPRule, pAttr,
                pSchP, pAb, pRefAb, pDoc, appl);
      if (*appl)
        /* the stretch rule is now applied */
        pdimAb->DimIsPosition = TRUE;
    }
  else if (pDRule->DrAbsolute)
    {
      /* absolute value */
      if (pDRule->DrAttr)
        /* it's an attribute value */
        {
          if (pDRule->DrValue < 0)
            /* reverse the value */
            pdimAb->DimValue = -AttrValue (pAttr);
          else
            pdimAb->DimValue = AttrValue (pAttr);
          if (pDRule->DrUnit == UnRelative ||
              pDRule->DrUnit == UnXHeight)
            /* convert into 1/10 of characters */
            pdimAb->DimValue = 10 * pdimAb->DimValue;
        }
      else
        /* take the value in the rule */
        {
          pdimAb->DimValue = pDRule->DrValue;
          /* in HTML and MathML, interpret a fixed value as a minimum if it's
           the height of a table, a table row or a table cell */
          if (pPRule->PrType == PtHeight)
            if (TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber,
                                  pAb->AbElement->ElStructSchema) ||
                TypeHasException (ExcIsRow, pAb->AbElement->ElTypeNumber,
                                  pAb->AbElement->ElStructSchema) ||
                TypeHasException (ExcIsTable, pAb->AbElement->ElTypeNumber,
                                  pAb->AbElement->ElStructSchema))
              pdimAb->DimMinimum = TRUE;
        }
      *appl = TRUE;
    }
  else if (pDRule->DrRelation == RlEnclosed)
    {
      /* contents value */
      if (!pAb->AbEnclosing)
        /* value for the root element */
        pdimAb->DimValue = 0;
      *appl = TRUE;
    }
  else
    {
      /* dimensions relatives a l'englobant ou un frere */
      pdimAb->DimSameDimension = pDRule->DrSameDimens;
      /* essaie d'appliquer la regle de dimensionnement relatif */
      pAbbRef = SearchAbsBoxRef (pDRule->DrNotRelat, pDRule->DrRefIdent,
                                 pSchP, pDRule->DrRelation,
                                 pDRule->DrRefKind, pAb, pAttr, pDoc);
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
              if (pAttr->AeAttrNum != attrRule ||
                  PresentationSchema (pAttr->AeAttrSSchema, pDoc) != pSchP)
                /* ce n'est pas l'attribut indique' dans la regle, */
                /* cherche si l'elem. possede l'attribut de la regle */
                {
                  pA = pAb->AbElement->ElFirstAttr;
                  stop = FALSE;
                  do
                    if (pA == NULL)
                      stop = TRUE;	/* dernier attribut de l'element */
                    else if (pA->AeAttrNum == attrRule &&
                             PresentationSchema (pA->AeAttrSSchema, pDoc) == pSchP)
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
  SearchPRule  cherche pour la vue view une regle de type ruleType
  dans la chaine de regles dont l'ancre est firstRule. Retourne   
  un pointeur sur cette regle si elle existe ou insere une regle  
  de ce type dans la chaine et retourne un pointeur sur la        
  regle creee.                                                    
  ----------------------------------------------------------------------*/
static PtrPRule SearchPRule (PtrPRule *firstRule, PRuleType ruleType, int view)
{
  PtrPRule            pR, pPRule;

  pR = *firstRule;
  pPRule = NULL;
  while (pR && pR->PrType < ruleType)
    {
      pPRule = pR;
      pR = pR->PrNextPRule;
    }
  while (pR && pR->PrType == ruleType && pR->PrViewNum < view)
    {
      pPRule = pR;
      pR = pR->PrNextPRule;
    }
  if (pR == NULL || pR->PrType != ruleType || pR->PrViewNum != view)
    /* il n'y a pas de regle de ce type pour cette vue, on en cree une */
    {
      GetPresentRule (&pR);
      if (pR)
        {
          pR->PrType = ruleType;
          /* on insere la regle cree */
          if (pPRule == NULL)
            {
              pR->PrNextPRule = *firstRule;
              *firstRule = pR;
            }
          else
            {
              pR->PrNextPRule = pPRule->PrNextPRule;
              pPRule->PrNextPRule = pR;
            }
          pR->PrCond = NULL;
          pR->PrViewNum = view;
          pR->PrSpecifAttr = 0;
          pR->PrSpecifAttrSSchema = NULL;
        }
    }
  return pR;
}

/*----------------------------------------------------------------------
  ApplyPage applies the Page presentation rule.
  ----------------------------------------------------------------------*/
static void ApplyPage (PtrDocument pDoc, PtrAbstractBox pAb, int viewSch,
                       PtrPRule pPRule, FunctionType pageType,
                       PtrPSchema pSchP)
{
  PtrElement          pElPage, pEl, pElChild, pPrec;
  PtrAbstractBox      pP;
  PtrPRule            pRWidth, pPageWidthRule;
  int                 counter;
  ThotBool            existingPage, stop;
  ThotBool            complete;
  ThotBool            create;

  if (pPRule->PrViewNum == viewSch)
    /* la regle Page concerne la vue du pave traite' */
    {
      pElPage = NULL;
      existingPage = FALSE;
      pEl = pAb->AbElement;
      /* l'element contient-il deja une marque de page de debut */
      /* d'element pour cette vue ? */
      if (!pEl->ElTerminal)
        {
          pElChild = pEl->ElFirstChild;
          while (pElChild && !existingPage)
            {
              if (pElChild->ElTypeNumber != PageBreak + 1)
                pElChild = NULL;
              else if (pElChild->ElViewPSchema == viewSch &&
                       pElChild->ElPageType == PgBegin)
                {
                  pElPage = pElChild;
                  existingPage = TRUE;
                }
              else
                /* on saute les eventuelles marque page d'autres vues */
                pElChild = pElChild->ElNext;
            }
        }
      if (!existingPage)
        /* l'element ne contient pas de marque de page en tete pour cette vue*/
        /* l'element est-il precede' par un saut de page identique a */
        /* celui qu'on veut creer ? */
        {
          pPrec = pEl;
          while (pPrec->ElPrevious == NULL && pPrec->ElParent != NULL)
            pPrec = pPrec->ElParent;
          pPrec = pPrec->ElPrevious;
          stop = (pPrec == NULL);
          while (!stop)
            if (pPrec->ElTerminal)
              {
                stop = TRUE;
                /* ignore les saut de pages pour les autres vues */
                if (pPrec->ElTypeNumber == PageBreak + 1 &&
                    pPrec->ElViewPSchema != viewSch)
                  {
                    pPrec = pPrec->ElPrevious;
                    stop = (pPrec == NULL);
                  }
              }
            else
              {
                pPrec = pPrec->ElFirstChild;
                if (!pPrec)
                  stop = TRUE;
                else
                  while (pPrec->ElNext)
                    pPrec = pPrec->ElNext;
              }
          if (pPrec && pPrec->ElTerminal &&
              pPrec->ElTypeNumber == PageBreak + 1 &&
              pPrec->ElViewPSchema == viewSch &&
              pPrec->ElPageType == PgBegin)
            {
              existingPage = TRUE;
              pElPage = pPrec;
            }
        }

      if (!existingPage &&
          pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct != CsChoice)
        {
          /* c'est la premiere fois qu'on applique la regle page a cet
             element. On cree pour cet element une regle de largeur qui
             lui donne la largeur de la boite page */
          if (pPRule->PrPresBox[0] > 0)
            /* the Page rule refers to some Page box */
            {
              /* cherche d'abord la regle de largeur de l'element qui porte la
                 regle Page */
              pRWidth = SearchPRule (&pSchP->PsElemPRule->ElemPres[pEl->ElTypeNumber-1],
                                     PtWidth, pPRule->PrViewNum);
              if (pRWidth)
                {
                  /* cherche la regle de largeur de la boite page referencee
                     par la regle Page */
                  pPageWidthRule = SearchPRule (&pSchP->PsPresentBox->PresBox[pPRule->PrPresBox[0]-1]->PbFirstPRule, PtWidth, pPRule->PrViewNum);
                  /* modifie la regle de largeur */
                  pRWidth->PrPresMode = PresImmediate;
                  pRWidth->PrDimRule = pPageWidthRule->PrDimRule;
                }
            }

          /* on cree une marque de page */
          pElPage = NewSubtree (PageBreak + 1, pEl->ElStructSchema, pDoc,
                                TRUE, TRUE, TRUE, TRUE);
          if (pElPage)
            /* on a cree une marque de page */
            /* on l'insere comme premier fils de l'element */
            {
              InsertFirstChild (pEl, pElPage);
              /* on l'initialise */
              pElPage->ElPageType = PgBegin;
              pElPage->ElViewPSchema = viewSch;
              /* cherche le compteur de pages a appliquer */
              counter = GetPageCounter (pElPage, pDoc, pElPage->ElViewPSchema,
                                        &pSchP);
              if (counter > 0)
                /* calcule la valeur du compteur de pages */
                pElPage->ElPageNumber = CounterVal (counter,
                                                    pElPage->ElStructSchema,
                                                    pSchP, pElPage,
                                                    pElPage->ElViewPSchema,
						    pDoc);
              else
                /* page non numerotee */
                pElPage->ElPageNumber = 1;
              /* faut-il creer les paves de la marque de page ? */
              create = TRUE;	/* a priori, on les cree */
              if (pElPage->ElNext)
                /* la marque de page a un element suivant */
                {
                  pP = pElPage->ElNext->ElAbstractBox[pAb->AbDocView - 1];
                  if (pP == NULL)
                    /* l'element suivant la marque de page n'a pas de pave */
                    /* dans la vue,on ne cree pas les paves de la marque page*/
                    create = FALSE;
                  else
                    /* on ne cree les paves de la marque de page que si le */
                    /* pave de l'element suivant est complet en tete */
                    create = !pP->AbTruncatedHead;
                }
              if (create)
                /* on cree les paves de la marque de page */
                pP = AbsBoxesCreate (pElPage, pDoc, pAb->AbDocView, TRUE,
                                     TRUE, &complete);
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
static ThotBool FindAbsBox (int Ntype, PtrPSchema pSchP, Name presBoxName,
                            PtrAbstractBox *pAb)
{
  ThotBool            result;
  PtrAbstractBox      pAbbCur;

  result = FALSE;
  if ((*pAb)->AbPresentationBox && (*pAb)->AbLeafType == LtText &&
      (*pAb)->AbTypeNum > 0)
    {
      if (Ntype != 0)
        result = !strcmp ((*pAb)->AbPSchema->PsPresentBox->PresBox[(*pAb)->AbTypeNum - 1]->PbName, pSchP->PsPresentBox->PresBox[Ntype - 1]->PbName);
      else
        result = !strcmp ((*pAb)->AbPSchema->PsPresentBox->PresBox[(*pAb)->AbTypeNum - 1]->PbName, presBoxName);
    }
  if (!result)
    {
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
static ThotBool SearchElCrPresBoxCopy (int *presBoxType, PtrPSchema *pSchP,
                                       PtrSSchema *pSchS, Name presBoxName,
                                       PtrElement *pEl)
{
  ThotBool            result, stop;
  PtrElement          pE1;
  PtrPSchema          pSP;
  PtrSSchema          pSS;
  PtrPRule            pPRuleCre;
  PtrAttribute        pA;
  PtrPRule            pPRule;
  PtrDocument         pDoc;

  result = FALSE;
  pDoc = DocumentOfElement (*pEl);
  /* cherche toutes les regles de  creation de cet element */
  pPRuleCre = GlobalSearchRulepEl (*pEl, pDoc, &pSP, &pSS, FALSE, 0, NULL, 1,
                                   PtFunction, FnAny, FALSE, FALSE, &pA);
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
          {
            /* c'est une regle de creation */
            if (*presBoxType != 0)
              {
                result = pPRule->PrPresBox[0] == *presBoxType;
                if (result)
                  result = strcmp (pSS->SsName, (*pSchS)->SsName) == 0;
                /* on supprime le test sur l'egalite des schemas P et on teste uniquement */
                /* les schemas de structure : cela permet a des chapitres de se referencer */
                /* mutuellement meme si leur schema de presentation different legerement */
                /* il faut que les schemas P aient les memes boites de presentation utilisees */
                /* en copie */
              }
            else
              result = !strcmp (pSP->PsPresentBox->PresBox[pPRule->PrPresBox[0] - 1]->PbName, presBoxName);
          }
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
static void CopyLeaves (PtrElement pEC, PtrAbstractBox *pAb,
                        PtrTextBuffer *pBuffPrec)
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
                pAbb1->AbLang = pEC->ElLanguage;
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
static PtrElement SearchElInSubTree (PtrElement pElRoot, int elType,
                                     PtrSSchema pSS, Name typeName)
{
  PtrElement          pEC, pElChild;

  pEC = NULL;			/* a priori on n'a pas trouve' */
  if (typeName[0] != EOS)
    /* on compare les noms de type */
    {
      if (strncmp (typeName,
                   pElRoot->ElStructSchema->SsRule->SrElem[pElRoot->ElTypeNumber - 1]->SrName,
                   MAX_NAME_LENGTH) == 0)
        pEC = pElRoot;
    }
  else
    /* on compare les numero de type et code de schema de structure */
    if (pElRoot->ElTypeNumber == elType &&
        !strcmp (pElRoot->ElStructSchema->SsName, pSS->SsName))
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
static PtrPRule GetRuleCopy (PtrPRule pPRule)
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
void ApplyCopy (PtrDocument pDoc, PtrPRule pPRule, PtrAbstractBox pAb,
                ThotBool withDescCopy)
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
  PtrElement          pEl1;
  PtrAbstractBox      pAbb1;
  PtrPresentationBox  pPBox;
  PtrCopyDescr        pDC;
  PtrPRule            pPRule1;
  PtrAttribute        pAttr;
  ThotBool            Ref;

  pEl1 = pAb->AbElement;
  pE = NULL;
  Ref = FALSE;
  if (pEl1->ElStructSchema->SsRule->SrElem[pEl1->ElTypeNumber - 1]->SrConstruct == CsReference)
    {
      /* la regle Copy s'applique a un pave' d'un element reference */
      Ref = TRUE;
      /* cherche l'element qui est reference' */
      pE = ReferredElement (pEl1->ElReference);
    }
  else if (pAb->AbPresentationBox)
    if (pAb->AbCreatorAttr != NULL)
      if (pAb->AbCreatorAttr->AeAttrType == AtReferenceAttr)
        /* la regle Copy s'applique a un pave' de presentation */
        /* cree' par un attribut reference */
        {
          Ref = TRUE;
          /* cherche l'element qui est reference' par l'attribut */
          pE = ReferredElement (pAb->AbCreatorAttr->AeAttrReference);
        }
  if (Ref)
    {
      /* c'est une copie par reference */
      if (pE != NULL)
        {
          /* l'element qui est reference' existe, il est pointe' par pE */
          if (pPRule->PrElement)
            /* il faut copier le contenu d'un element structure' contenu */
            /* dans l'element reference'. On cherche cet element */
            pE = SearchElInSubTree (pE, pPRule->PrPresBox[0], pEl1->ElStructSchema, pPRule->PrPresBoxName);
          else
            /* il faut copier une boite de presentation */
            /* prend le schema de presentation qui s'applique a la reference */
            {
              SearchPresSchema (pAb->AbElement, &pSchP, &i, &pSchS, pDoc);
              if (pPRule->PrNPresBoxes == 0)
                /* la boite de presentation a copier est definie par son nom */
                {
                  boxType = 0;
                  strncpy (boxName, pPRule->PrPresBoxName, MAX_NAME_LENGTH);
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
                  pAbb1->AbLang = pAbbCur->AbLang;
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
                        pPBox = pSchP->PsPresentBox->PresBox[boxType - 1];
                        if (pPBox->PbContent == ContVariable)
                          /* on fait comme si le pave appartenait a l'element */
                          /* a copier */
                          {
                            pElSv = pAb->AbElement;
                            pAb->AbElement = pE;
                            found = NewVariable (pPBox->PbContVariable, pSchS,
                                                 pSchP, pAb, NULL, pDoc);
                            /* on retablit le pointeur correct */
                            pAb->AbElement = pElSv;
                          }
                        if (pPBox->PbContent == FreeContent)
                          /* le contenu de la boite de presentation a copier */
                          /* est lui-meme defini par une regle FnCopy */
                          {
                            /* on cherche cette regle FnCopy parmi les regles de */
                            /* presentation de la boite de presentation a copier */
                            pPRule1 = GetRuleCopy (pPBox->PbFirstPRule);
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
            pE = BackSearchTypedElem (pAb->AbElement, pPRule->PrPresBox[0],
                                      pEl1->ElStructSchema, NULL);
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
      /* si l'element a copier est lui-meme une reference qui copie un */
      /* autre element, c'est cet autre element qu'on copie */
      pPRule1 = NULL;
      if (pE->ElStructSchema->SsRule->SrElem[pE->ElTypeNumber - 1]->SrConstruct == CsReference)
        {
          pPRule1 = GlobalSearchRulepEl (pE, pDoc, &pSchP, &pSchS, FALSE, 0,
                                         NULL, 1, PtFunction, FnAny, FALSE,
                                         FALSE, &pAttr);
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
  NewAbPositioning
  create and intializes a Positioning block for abstract box pAb.
  ----------------------------------------------------------------------*/
static void NewAbPositioning (PtrAbstractBox pAb)
{
  pAb->AbPositioning = (Positioning *) TtaGetMemory (sizeof (Positioning));
  pAb->AbPositioning->PnAlgorithm = PnStatic;
  pAb->AbPositioning->PnTopDistance = 0;
  pAb->AbPositioning->PnTopUnit = UnUndefined;
  pAb->AbPositioning->PnRightDistance = 0;
  pAb->AbPositioning->PnRightUnit = UnUndefined;
  pAb->AbPositioning->PnBottomDistance = 0;
  pAb->AbPositioning->PnBottomUnit = UnUndefined;
  pAb->AbPositioning->PnLeftDistance = 0;
  pAb->AbPositioning->PnLeftUnit = UnUndefined;
}

/*----------------------------------------------------------------------
  GetGradientStop
  ----------------------------------------------------------------------*/
static GradientStop* GetGradientStop (PtrElement stop, PtrElement gradient)
{
  GradientStop *gstop;
  Gradient     *grad;

  if (stop == NULL || gradient == NULL || gradient->ElGradient == NULL)
    /* error */
    return NULL;
  grad = gradient->ElGradient;
  gstop = grad->firstStop;
  while (gstop)
    {
      if (gstop->el == stop)
        return gstop;
      else
	gstop = gstop->next;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ApplyRule applies the pRule rule to the pAb abstract box.
  The parameter pSchP points to the presentation schema.
  If pAttr is not null, it points to the attribute which generates the rule.
  The parameter pRefAb points to the abstract box that submits the apply.
  Returns TRUE if the rule is applied or will be never applied, FALSE it
  it's not applied and can be applied later when other abstract boxes will
  be created.
  ----------------------------------------------------------------------*/
ThotBool ApplyRule (PtrPRule pPRule, PtrPSchema pSchP, PtrAbstractBox pAb,
                    PtrDocument pDoc, PtrAttribute pAttr, PtrAbstractBox pRefAb)
{
  TypeUnit            unit;
  AbPosition          Posit;
  PresConstant	     *pConst;
  PathBuffer	      directoryName;
  AbPosition         *pPavP1;
  ThotPictInfo       *image;
  PtrAbstractBox      pAbb, pParent;
  PtrElement          pEl, paintServer;
  char                fname[MAX_PATH];
  unsigned char       c;
  int                 viewSch, i;
  GradientStop       *gstop;
  Document            doc;
  unsigned short      red, green, blue;
  ThotBool            appl;
  ThotBool            insidePage, afterPageBreak;
  ThotBool            ignorefix = FALSE;

  appl = FALSE;
  if (pPRule && pAb && pAb->AbElement)
    {
      pEl = pAb->AbElement;
      viewSch = AppliedView (pEl, pAttr, pDoc, pAb->AbDocView);
      switch (pPRule->PrType)
        {
        case PtVisibility:
          pAb->AbVisibility = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbVisibility = 10;
              appl = TRUE;
            }
          if (appl && pAb->AbVisibility == 0)
            pAb->AbDead = TRUE;
           break;
        case PtFunction:
          switch (pPRule->PrPresFunction)
            {
            case FnLine:
              if (pAb->AbLeafType == LtCompound && pAb->AbDisplay != 'N')
                /* si la regle de mise en lignes est definie pour la */
                /* vue principale, elle s'applique a toutes les vues, */
                /* sinon, elle ne s'applique qu'a la vue pour laquelle */
                /* elle est definie */
                if (pPRule->PrViewNum == 1 || pPRule->PrViewNum == viewSch)
                  pAb->AbInLine = TRUE;		      
              appl = TRUE;
              break;
            case FnNoLine:
              if (pAb->AbLeafType == LtCompound &&
                  pPRule->PrViewNum == viewSch)
                pAb->AbInLine = FALSE;
              appl = TRUE;
              break;
            case FnPage:
              /* ignore the Page rule when working for the editor (as opposed
                 to the printer) and if it applies to the root of a generic
                 XML document */
              if (Printing || pEl->ElParent ||
                  !pDoc->DocSSchema->SsIsXml)
                ApplyPage (pDoc, pAb, viewSch, pPRule, pPRule->PrPresFunction,
                           pSchP);
              appl = TRUE;
              break;
            case FnColumn:
            case FnSubColumn:
              appl = TRUE;
              break;
            case FnCopy:
              /* on n'applique pas la regle copie a un element holophraste'*/
              if (!pEl->ElHolophrast)
                ApplyCopy (pDoc, pPRule, pAb, TRUE);
              appl = TRUE;
              break;
            case FnContentRef:
              ConstantCopy (pPRule->PrPresBox[0], pSchP, pAb);
              appl = TRUE;
              break;
            case FnShowBox:
              if (pAb->AbLeafType == LtCompound &&
                  pPRule->PrViewNum == viewSch)
                pAb->AbFillBox = TRUE;
              appl = TRUE;
              break;
            case FnBackgroundPicture:
              appl = TRUE;
              if (pAb->AbLeafType == LtCompound &&
                  pPRule->PrViewNum == viewSch)
                {
                  if (pPRule->PrPresBox[0] <= 0)
                    /* it's a CSS rule "background-picture: none" */
                    {
                      if (pAb->AbPictBackground)
                        /* remove the background image */
                        {
                          image = (ThotPictInfo *)pAb->AbPictBackground;
                          TtaFreeMemory (image->PicFileName);
                          image->PicFileName = NULL;
                          /* don't free the context as it gets
                             the presentation mode which could be
                             used by a future FnBackgroundPicture rule */
#ifdef IV
                          CleanPictInfo (image);
                          TtaFreeMemory (image->PicFileName);
                          TtaFreeMemory (pAb->AbPictBackground);
                          pAb->AbPictBackground = NULL;
#endif
                        } 
                    }
                  else
                    {
                      if (pSchP == NULL)
                        pSchP = PresentationSchema (pDoc->DocSSchema, pDoc);
                      pConst = &pSchP->PsConstant[pPRule->PrPresBox[0] - 1];
                      if (pConst->PdString && pConst->PdString[0] != EOS)
                        {
#ifndef _WINDOWS
                          if (pConst->PdString[0] == DIR_SEP)
#else  /* _WINDOWS */
                            if (pConst->PdString[0] == DIR_SEP ||
                                (pConst->PdString[1] == ':' &&
                                 pConst->PdString[2] == DIR_SEP))
#endif /* _WINDOWS */
                              /* absolute file name */
                              strncpy (fname, pConst->PdString, MAX_PATH - 1);
                            else
                              /* relative file name */
                              {
                                strncpy (directoryName, SchemaPath, MAX_PATH-1);
                                MakeCompleteName (pConst->PdString, "",
                                                  directoryName, fname, &i);
                              }
                          NewPictInfo (pAb, fname, UNKNOWN_FORMAT, False);
                        }
                    }
                }
              break;
            case FnBackgroundRepeat:
              if (pPRule->PrViewNum == viewSch)
                {
                  if (pEl->ElTerminal &&
                      pEl->ElLeafType == LtPicture)
                    {
                      if (pEl->ElPictInfo == NULL)
                        NewPictInfo (pAb, "", UNKNOWN_FORMAT, False);
                      ((ThotPictInfo *) (pEl->ElPictInfo))->PicPresent =
                        (PictureScaling)pPRule->PrPresBox[0];
                    }
                  else if (pAb->AbPresentationBox)
                    {
                      if (pAb->AbPictInfo == NULL)
                        NewPictInfo (pAb, "", UNKNOWN_FORMAT, False);
                      ((ThotPictInfo *) (pAb->AbPictInfo))->PicPresent =
                        (PictureScaling)pPRule->PrPresBox[0];
                    }
                  else if (pAb->AbLeafType == LtCompound)
                    {
                      if (pAb->AbPictBackground == NULL)
                        NewPictInfo (pAb, "", UNKNOWN_FORMAT, False);
                      ((ThotPictInfo *) (pAb->AbPictBackground))->PicPresent =
                        (PictureScaling)pPRule->PrPresBox[0];
                    }
                }
              appl = TRUE;
              break;
            case FnNotInLine:
              if (pPRule->PrViewNum == viewSch)
                pAb->AbNotInLine = TRUE;
              appl = TRUE;
              break;
            default:
              appl = TRUE;
              break;
            }	    
          break;
        case PtVertOverflow:
          pAb->AbVertEnclosing = !BoolRule (pPRule, pEl,
                                            pAb->AbDocView, &appl);
          break;
        case PtHorizOverflow:
          pAb->AbHorizEnclosing = !BoolRule (pPRule, pEl,
                                             pAb->AbDocView, &appl);
          break;
        case PtVertRef:
          Posit = pAb->AbVertRef;
          ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr, pSchP, pAb,
                    pRefAb, pDoc, &appl);
          pAb->AbVertRef = Posit;
          break;
        case PtHorizRef:
          Posit = pAb->AbHorizRef;
          ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr, pSchP, pAb,
                    pRefAb, pDoc, &appl);
          pAb->AbHorizRef = Posit;
          break;
        case PtHeight:
          ApplyDim (&pAb->AbHeight, pAb, pRefAb, pSchP, pAttr, &appl, pPRule, pDoc);
          break;
        case PtWidth:
          ApplyDim (&pAb->AbWidth, pAb, pRefAb, pSchP, pAttr, &appl, pPRule, pDoc);
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
              if (!pAb->AbPrevious->AbDead &&
                  pAb->AbPrevious->AbElement-> ElTypeNumber == PageBreak + 1 &&
                  pEl->ElTypeNumber != PageBreak + 1)
                {
                  if (pPRule->PrPosRule.PoRelation == RlSameLevel
                      || pPRule->PrPosRule.PoRelation == RlPrevious)
                    afterPageBreak = TRUE;
                  else if (pPRule->PrPosRule.PoRelation == RlEnclosing &&
                           pAb->AbPrevious->AbElement-> ElPageType != PgBegin)
                    afterPageBreak = TRUE;
                }
            }
          else
            /* il n'y a pas de pave precedent */
            if (pEl->ElPrevious != NULL)
              /* il y a un element precedent */
              if (pEl->ElPrevious->ElTypeNumber == PageBreak + 1
                  && pEl->ElPrevious->ElViewPSchema == viewSch
                  && pEl->ElTypeNumber != PageBreak + 1)
                /* l'element precedent est une marque de page pour la vue */
                if (pPRule->PrPosRule.PoRelation == RlSameLevel
                    || pPRule->PrPosRule.PoRelation == RlPrevious)
                  afterPageBreak = TRUE;
          if (afterPageBreak)
            {
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
                  pPavP1->PosDistDelta = 0;
                  pPavP1->PosUnit = UnPoint;
                  pPavP1->PosDeltaUnit = UnPoint;
                  pPavP1->PosAbRef = pAb->AbPrevious;
                  pPavP1->PosUserSpecified = FALSE;
                }
            }
          else
            /* s'il s'agit d'une boite page (celle qui englobe le filet */
            /* et les hauts et pieds de page), et si elle n'est precedee */
            /* d'aucun element, elle se positionne en haut de l'englobant. */
            {
              insidePage = FALSE;
              if (pAb->AbEnclosing != NULL)
                if (pEl->ElTypeNumber == PageBreak + 1
                    && pAb->AbEnclosing->AbElement->
                    ElTypeNumber != PageBreak + 1
                    && pEl->ElPrevious == NULL)
                  if (pAb->AbPrevious == NULL)
                    insidePage = TRUE;
	      
              if (insidePage)
                {
                  pPavP1 = &pAb->AbVertPos;
                  pPavP1->PosAbRef = pAb->AbEnclosing;
                  pPavP1->PosEdge = Top;
                  pPavP1->PosRefEdge = Top;
                  pPavP1->PosDistance = 0;
                  pPavP1->PosDistDelta = 0;
                  pPavP1->PosUnit = UnPoint;
                  pPavP1->PosDeltaUnit = UnPoint;
                  pPavP1->PosUserSpecified = FALSE;
                }
              else
                /* applique la regle de positionnement de l'element */
                {
                  Posit = pAb->AbVertPos;
                  ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr,
                            pSchP, pAb, pRefAb, pDoc, &appl);
                  pAb->AbVertPos = Posit;
                }
            }
          break;
        case PtHorizPos:
          Posit = pAb->AbHorizPos;
          ApplyPos (&Posit, &(pPRule->PrPosRule), pPRule, pAttr, pSchP,
                    pAb, pRefAb, pDoc, &appl);
          pAb->AbHorizPos = Posit;
          break;
        case PtMarginTop:
          pAb->AbTopMargin = IntegerRule (pPRule, pEl,
                                          pAb->AbDocView, &appl, &unit,
                                          pAttr, pAb, pSchP, pDoc);
          pAb->AbTopMarginUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbTopMargin = 0;
              appl = TRUE;
            }
          break;
        case PtMarginRight:
          pAb->AbRightMargin = IntegerRule (pPRule, pEl,
                                            pAb->AbDocView, &appl, &unit,
                                            pAttr, pAb, pSchP, pDoc);
          pAb->AbRightMarginUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbRightMargin = 0;
              appl = TRUE;
            }
          break;
        case PtMarginBottom:
          pAb->AbBottomMargin = IntegerRule (pPRule, pEl,
                                             pAb->AbDocView, &appl, &unit,
                                             pAttr, pAb, pSchP, pDoc);
          pAb->AbBottomMarginUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbBottomMargin = 0;
              appl = TRUE;
            }
          break;
        case PtMarginLeft:
          pAb->AbLeftMargin = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
          pAb->AbLeftMarginUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbLeftMargin = 0;
              appl = TRUE;
            }
          break;
        case PtPaddingTop:
          pAb->AbTopPadding = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
          pAb->AbTopPaddingUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbTopPadding = 0;
              appl = TRUE;
            }
          break;
        case PtPaddingRight:
          pAb->AbRightPadding = IntegerRule (pPRule, pEl,
                                             pAb->AbDocView, &appl, &unit,
                                             pAttr, pAb, pSchP, pDoc);
          pAb->AbRightPaddingUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbRightPadding = 0;
              appl = TRUE;
            }
          break;
        case PtPaddingBottom:
          pAb->AbBottomPadding = IntegerRule (pPRule, pEl,
                                              pAb->AbDocView, &appl, &unit,
                                              pAttr, pAb, pSchP, pDoc);
          pAb->AbBottomPaddingUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbBottomPadding = 0;
              appl = TRUE;
            }
          break;
        case PtPaddingLeft:
          pAb->AbLeftPadding = IntegerRule (pPRule, pEl,
                                            pAb->AbDocView, &appl, &unit,
                                            pAttr, pAb, pSchP, pDoc);
          pAb->AbLeftPaddingUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbLeftPadding = 0;
              appl = TRUE;
            }
          break;
        case PtBorderTopWidth:
          pAb->AbTopBorder = IntegerRule (pPRule, pEl,
                                          pAb->AbDocView, &appl, &unit,
                                          pAttr, pAb, pSchP, pDoc);
          pAb->AbTopBorderUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbTopBorder = 0;
              appl = TRUE;
            }
          break;
        case PtBorderRightWidth:
          pAb->AbRightBorder = IntegerRule (pPRule, pEl,
                                            pAb->AbDocView, &appl, &unit,
                                            pAttr, pAb, pSchP, pDoc);
          pAb->AbRightBorderUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbRightBorder = 0;
              appl = TRUE;
            }
          break;
        case PtBorderBottomWidth:
          pAb->AbBottomBorder = IntegerRule (pPRule, pEl,
                                             pAb->AbDocView, &appl, &unit,
                                             pAttr, pAb, pSchP, pDoc);
          pAb->AbBottomBorderUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbBottomBorder = 0;
              appl = TRUE;
            }
          break;
        case PtBorderLeftWidth:
          pAb->AbLeftBorder = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
          pAb->AbLeftBorderUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbLeftBorder = 0;
              appl = TRUE;
            }
          break;
        case PtBorderTopColor:
          pAb->AbTopBColor = IntegerRule (pPRule, pEl,
                                          pAb->AbDocView, &appl, &unit,
                                          pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* no BorderTopColor for the root element. Set initial value */
            {
              pAb->AbTopBColor = -1;
              appl = TRUE;
            }
          break;
        case PtBorderRightColor:
          pAb->AbRightBColor = IntegerRule (pPRule, pEl,
                                            pAb->AbDocView, &appl, &unit,
                                            pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* no BorderRightColor for the root element. Set initial value */
            {
              pAb->AbRightBColor = -1;
              appl = TRUE;
            }
          break;
        case PtBorderBottomColor:
          pAb->AbBottomBColor = IntegerRule (pPRule, pEl,
                                             pAb->AbDocView, &appl, &unit,
                                             pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* no BorderBottomColor for the root element. Set initial value*/
            {
              pAb->AbBottomBColor = -1;
              appl = TRUE;
            }
          break;
        case PtBorderLeftColor:
          pAb->AbLeftBColor = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* no BorderLeftColor for the root element. Set initial value */
            {
              pAb->AbLeftBColor = -1;
              appl = TRUE;
            }
          break;
        case PtBorderTopStyle:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              c = '0';
              appl = TRUE;
            }
          if (appl)
            pAb->AbTopStyle = BorderStyleIntValue (c);
          break;
        case PtBorderRightStyle:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              c = '0';
              appl = TRUE;
            }
          if (appl)
            pAb->AbRightStyle = BorderStyleIntValue (c);
          break;
        case PtBorderBottomStyle:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              c = '0';
              appl = TRUE;
            }
          if (appl)
            pAb->AbBottomStyle = BorderStyleIntValue (c);
          break;
        case PtBorderLeftStyle:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              c = '0';
              appl = TRUE;
            }
          if (appl)
            pAb->AbLeftStyle = BorderStyleIntValue (c);
          break;
        case PtSize:
          /* on applique la regle de taille */
          pAb->AbSize = IntegerRule (pPRule, pEl,
                                     pAb->AbDocView, &appl, &unit,
                                     pAttr, pAb, pSchP, pDoc);
          if (appl)
            pAb->AbSizeUnit = unit;
          else if (pEl->ElParent == NULL)
            /* c'est la racine, on met a priori la valeur par defaut */
            {
              pAb->AbSize = 3;
              pAb->AbSizeUnit = UnRelative;
              appl = TRUE;
            }
          break;
        case PtStyle:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
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
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
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
        case PtVariant:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbFontVariant = 1;
              appl = TRUE;
            }
          else
            switch (c)
              {
              case 'N':	/* Normal */
                pAb->AbFontVariant = 1;
                break;
              case 'C':	/* SmallCaps */
                pAb->AbFontVariant = 2;
                break;
              case 'D':	/* DoubleStruck */
                pAb->AbFontVariant = 3;
                break;
              case 'F':	/* Fraktur */
                pAb->AbFontVariant = 4;
                break;
              case 'S':	/* Script */
                pAb->AbFontVariant = 5;
                break;
              default:	/* Normal */
                pAb->AbFontVariant = 1;
                break;
              }
          break;
        case PtFont:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              pAb->AbFont = 1;
              appl = TRUE;
            }
          else
            pAb->AbFont = (int) c & 0xFF;
          break;
        case PtUnderline:
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
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
          c = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
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
        case PtIndent:
          pAb->AbIndent = IntegerRule (pPRule, pEl,
                                       pAb->AbDocView, &appl, &unit,
                                       pAttr, pAb, pSchP, pDoc);
          pAb->AbIndentUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbIndent = 0;
              appl = TRUE;
            }
          break;
        case PtLineSpacing:
          pAb->AbLineSpacing = IntegerRule (pPRule, pEl,
                                            pAb->AbDocView, &appl, &unit,
                                            pAttr, pAb, pSchP, pDoc);
          pAb->AbLineSpacingUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbLineSpacing = 10;
              pAb->AbLineSpacingUnit = UnRelative;
              appl = TRUE;
            }
          break;
        case PtDepth:
          pAb->AbDepth = IntegerRule (pPRule, pEl, pAb->AbDocView,
                                      &appl, &unit, pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbDepth = 0;
              appl = TRUE;
            }
          break;
        case PtAdjust:
          pAb->AbAdjust = AlignRule (pPRule, pEl, pAb->AbDocView,
                                     &appl);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbAdjust = AlignLeft;
              appl = TRUE;
            }
          break;
        case PtDirection:
          pAb->AbDirection = CharRule (pPRule, pEl,
                                       pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              /* initial value: LeftToRight */
              pAb->AbDirection = 'L';
              appl = TRUE;
            }
          break;
        case PtUnicodeBidi:
          pAb->AbUnicodeBidi = CharRule (pPRule, pEl,
                                         pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              /* initial value: Normal */
              pAb->AbUnicodeBidi = 'N';
              appl = TRUE;
            }
          break;
        case PtLineStyle:
          pAb->AbLineStyle = CharRule (pPRule, pEl,
                                       pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              pAb->AbLineStyle = 'S';
              appl = TRUE;
            }
          break;
        case PtLineWeight:
          pAb->AbLineWeight = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
          pAb->AbLineWeightUnit = unit;
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbLineWeight = 1;
              pAb->AbLineWeightUnit = UnPoint;
              appl = TRUE;
            }
          break;
        case PtFillPattern:
          pAb->AbFillPattern = IntegerRule (pPRule, pEl,
                                            pAb->AbDocView, &appl, &unit,
                                            pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbFillPattern = 0;
              appl = TRUE;
            }
          break;
        case PtOpacity:	      
          pAb->AbOpacity = IntegerRule (pPRule, pEl,
                                        pAb->AbDocView, &appl, &unit,
                                        pAttr, pAb, pSchP, pDoc);
	  if (appl && strcmp (pEl->ElStructSchema->SsName, "SVG"))
	    /* don't change for SVG */
	    {
	      pAb->AbFillOpacity = pAb->AbOpacity;
	      pAb->AbStrokeOpacity = pAb->AbOpacity;
	    }
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbOpacity = 1000;
              appl = TRUE;	      
            }
          break;
        case PtFillOpacity:
	  if (strcmp (pEl->ElStructSchema->SsName, "SVG"))
	      /* not an SVG element */
	    appl = TRUE;
	  else
	    pAb->AbFillOpacity = IntegerRule (pPRule, pEl,
					      pAb->AbDocView, &appl, &unit,
					      pAttr, pAb, pSchP, pDoc);
	  if (!appl && pEl->ElParent == NULL)
	    /* Pas de regle pour la racine, on met la valeur par defaut */
	    {
	      pAb->AbFillOpacity = 1000;
	      appl = TRUE;	      
	    }
          break;
        case PtStrokeOpacity:
	  if (strcmp (pEl->ElStructSchema->SsName, "SVG"))
	      /* not an SVG element */
	    appl = TRUE;
	  else
	    pAb->AbStrokeOpacity = IntegerRule (pPRule, pEl,
						pAb->AbDocView, &appl, &unit,
						pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbStrokeOpacity = 1000;
              appl = TRUE;
            }
          break;
        case PtStopOpacity:	      
          pAb->AbStopOpacity = IntegerRule (pPRule, pEl,
					    pAb->AbDocView, &appl, &unit,
					    pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbStopOpacity = 1000;
              appl = TRUE;	      
            }
	  if (appl && pEl->ElParent && pEl->ElParent->ElGradient &&
	      pEl->ElParent->ElGradientDef)
	    {
	      gstop = GetGradientStop (pEl, pEl->ElParent);
	      if (gstop)
		gstop->a = (unsigned short) (pAb->AbStopOpacity *255 / 1000);
	    }
          break;
        case PtFillRule:
          pAb->AbFillRule = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            {
              pAb->AbFillRule = 'n';
              appl = TRUE;
            }
          break;
        case PtMarker:
          pAb->AbMarker = MarkerRule (pPRule, pEl, pAb->AbDocView, &appl,
				      pSchP, pDoc);
	  if (appl && pAb->AbMarker)
	    {
	      if (TypeHasException (ExcUseMarkers, pEl->ElTypeNumber,
				    pEl->ElStructSchema))
		/* This element uses markers */
		{
		  doc = (Document) IdentDocument (pDoc);
		  GenerateMarkers ((Element)pEl, doc,
				   (Element)pAb->AbMarker, 0);
		}
	    }
          if (!appl && pEl->ElParent == NULL)
            /* No rule for the root, use the default value */
            {
              pAb->AbMarker = NULL;
              appl = TRUE;
            }
          break;
        case PtMarkerStart:
          pAb->AbMarkerStart = MarkerRule (pPRule, pEl, pAb->AbDocView, &appl,
					   pSchP, pDoc);
	  if (appl && pAb->AbMarkerStart)
	    {
	      if (TypeHasException (ExcUseMarkers, pEl->ElTypeNumber,
				    pEl->ElStructSchema))
		/* This element uses markers */
		{
		  doc = (Document) IdentDocument (pDoc);
		  GenerateMarkers ((Element)pEl, doc,
				   (Element)pAb->AbMarkerStart, 1);
		}
	    }
          if (!appl && pEl->ElParent == NULL)
            /* No rule for the root, use the default value */
            {
              pAb->AbMarkerStart = NULL;
              appl = TRUE;
            }
          break;
        case PtMarkerMid:
          pAb->AbMarkerMid = MarkerRule (pPRule, pEl, pAb->AbDocView, &appl,
					 pSchP, pDoc);
	  if (appl && pAb->AbMarkerMid)
	    {
	      if (TypeHasException (ExcUseMarkers, pEl->ElTypeNumber,
				    pEl->ElStructSchema))
		/* This element uses markers */
		{
		  doc = (Document) IdentDocument (pDoc);
		  GenerateMarkers ((Element)pEl, doc,
				   (Element)pAb->AbMarkerMid, 2);
		}
	    }
          if (!appl && pEl->ElParent == NULL)
            /* No rule for the root, use the default value */
            {
              pAb->AbMarkerMid = NULL;
              appl = TRUE;
            }
          break;
        case PtMarkerEnd:
          pAb->AbMarkerEnd = MarkerRule (pPRule, pEl, pAb->AbDocView, &appl,
					 pSchP, pDoc);
	  if (appl && pAb->AbMarkerEnd)
	    {
	      if (TypeHasException (ExcUseMarkers, pEl->ElTypeNumber,
				    pEl->ElStructSchema))
		/* This element uses markers */
		{
		  doc = (Document) IdentDocument (pDoc);
		  GenerateMarkers ((Element)pEl, doc,
				   (Element)pAb->AbMarkerEnd, 3);
		}
	    }
          if (!appl && pEl->ElParent == NULL)
            /* No rule for the root, use the default value */
            {
              pAb->AbMarkerEnd = NULL;
              appl = TRUE;
            }
          break;
        case PtBackground:
          pAb->AbBackground = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
	  if (appl)
	    {
	    if (unit != UnGradient)
	      pAb->AbGradientBackground = FALSE;
	    else
	      {
		pAb->AbGradientBackground = TRUE;
		if (TypeHasException (ExcUsePaintServer, pEl->ElTypeNumber,
				      pEl->ElStructSchema))
		  /* This element uses paint servers */
		  {
		    paintServer = NULL;
		    if (pSchP == NULL)
		      pSchP = PresentationSchema (pDoc->DocSSchema, pDoc);
		    pConst = &pSchP->PsConstant[pAb->AbBackground - 1];
		    if (pConst->PdString && pConst->PdString[0] != EOS)
		      paintServer = ApplyFillUrl (pEl, pDoc, pConst->PdString);
		    if (!paintServer)
		      /* paint server not found. Use default color */
		      {
			pAb->AbBackground = DefaultFColor;
			pAb->AbGradientBackground = FALSE;
		      }
		  }
	      }
	    }
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine,
               on met la valeur par defaut */
            {
              pAb->AbBackground = DefaultBColor;
	      pAb->AbGradientBackground = FALSE;
              appl = TRUE;
            }
          break;
        case PtForeground:
          pAb->AbForeground = IntegerRule (pPRule, pEl,
                                           pAb->AbDocView, &appl, &unit,
                                           pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbForeground = DefaultFColor;
              appl = TRUE;
            }
          break;
        case PtColor:
          pAb->AbColor = IntegerRule (pPRule, pEl,
				      pAb->AbDocView, &appl, &unit,
				      pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur indefini */
            {
              pAb->AbColor = DefaultFColor;
              appl = TRUE;
            }
          break;
        case PtStopColor:
          pAb->AbStopColor = IntegerRule (pPRule, pEl,
					  pAb->AbDocView, &appl, &unit,
					  pAttr, pAb, pSchP, pDoc);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur black */
            {
              pAb->AbStopColor = DefaultFColor;
              appl = TRUE;
            }
	  if (appl && pEl->ElParent && pEl->ElParent->ElGradient &&
	      pEl->ElParent->ElGradientDef)
	    {
	      gstop = GetGradientStop (pEl, pEl->ElParent);
	      if (gstop)
		{
		  TtaGiveThotRGB (pAb->AbStopColor, &red, &green, &blue);
		  gstop->r = red;
		  gstop->g = green;
		  gstop->b = blue;
		}
	    }
          break;
        case PtHyphenate:
          pAb->AbHyphenate = BoolRule (pPRule, pEl,
                                       pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbHyphenate = FALSE;
              appl = TRUE;
            }
          break;
        case PtPageBreak:
          pAb->AbAcceptPageBreak = BoolRule (pPRule, pEl,
                                             pAb->AbDocView, &appl);
          break;
        case PtLineBreak:
          pAb->AbAcceptLineBreak = BoolRule (pPRule, pEl,
                                             pAb->AbDocView, &appl);
          break;
        case PtGather:
          pAb->AbBuildAll = BoolRule (pPRule, pEl,
                                      pAb->AbDocView, &appl);
          break;
        case PtXRadius:
          pAb->AbRx = IntegerRule (pPRule, pEl, pAb->AbDocView,
                                   &appl, &unit, pAttr, pAb, pSchP, pDoc);
          pAb->AbRxUnit = unit;
          break;
        case PtYRadius:
          pAb->AbRy = IntegerRule (pPRule, pEl, pAb->AbDocView,
                                   &appl, &unit, pAttr, pAb, pSchP, pDoc);
          pAb->AbRyUnit = unit;
          break;
        case PtTop:
          if (pAb->AbLeafType == LtCompound)
            {
              if (pAb->AbPositioning == NULL)
                NewAbPositioning (pAb);
              pAb->AbPositioning->PnTopDistance = IntegerRule (pPRule,
                   pEl, pAb->AbDocView, &appl, &unit, pAttr, pAb, pSchP, pDoc);
              pAb->AbPositioning->PnTopUnit = unit;
            }
          break;
        case PtRight:
          if (pAb->AbLeafType == LtCompound)
            {
              if (pAb->AbPositioning == NULL)
                NewAbPositioning (pAb);
              pAb->AbPositioning->PnRightDistance = IntegerRule (pPRule,
                   pEl, pAb->AbDocView, &appl, &unit, pAttr, pAb, pSchP, pDoc);
              pAb->AbPositioning->PnRightUnit = unit;
            }
          break;
        case PtBottom:
          if (pAb->AbLeafType == LtCompound)
            {
              if (pAb->AbPositioning == NULL)
                NewAbPositioning (pAb);
              pAb->AbPositioning->PnBottomDistance = IntegerRule (pPRule,
                    pEl, pAb->AbDocView, &appl, &unit, pAttr, pAb, pSchP, pDoc);
              pAb->AbPositioning->PnBottomUnit = unit;
            }
          break;
        case PtLeft:
          if (pAb->AbLeafType == LtCompound)
            {
              if (pAb->AbPositioning == NULL)
                NewAbPositioning (pAb);
              pAb->AbPositioning->PnLeftDistance = IntegerRule (pPRule,
                    pEl, pAb->AbDocView, &appl, &unit, pAttr, pAb, pSchP, pDoc);
              pAb->AbPositioning->PnLeftUnit = unit;
            }
          break;
        case PtBackgroundHorizPos:
          if (pAb->AbLeafType == LtCompound &&
              pPRule->PrViewNum == viewSch)
            {
              if (pAb->AbPictBackground == NULL)
                NewPictInfo (pAb, "", UNKNOWN_FORMAT, False);
              ((ThotPictInfo *) (pAb->AbPictBackground))->PicPosX =
                 IntegerRule (pPRule, pEl, pAb->AbDocView, &appl,
                              &unit, pAttr, pAb, pSchP, pDoc);
              ((ThotPictInfo *) (pAb->AbPictBackground))->PicXUnit = unit;
            }
          break;
        case PtBackgroundVertPos:
          if (pAb->AbLeafType == LtCompound &&
              pPRule->PrViewNum == viewSch)
            {
              if (pAb->AbPictBackground == NULL)
                NewPictInfo (pAb, "", UNKNOWN_FORMAT, False);
              ((ThotPictInfo *) (pAb->AbPictBackground))->PicPosY =
                 IntegerRule (pPRule, pEl, pAb->AbDocView, &appl,
                              &unit, pAttr, pAb, pSchP, pDoc);
              ((ThotPictInfo *) (pAb->AbPictBackground))->PicYUnit = unit;
            }
          break;
        case PtVis:
          // CSS visibility
          pAb->AbVis = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (pAb->AbVis == 'I')
            {
              if (pAb->AbEnclosing)
                pAb->AbVis = pAb->AbEnclosing->AbVis;
              else
                pAb->AbVis = 'V';
            }
          if (pAb->AbVis == 'C' &&
              !TypeHasException (ExcIsColHead,pEl->ElTypeNumber,
                                 pEl->ElStructSchema) &&
              !TypeHasException (ExcIsRow, pEl->ElTypeNumber,
                                 pEl->ElStructSchema))
            pAb->AbVis = 'V';
           break;
        case PtDisplay:
          pAb->AbDisplay = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (appl)
            {
              if (pAb->AbDisplay == 'N')
                /* display: none */
                {
                  if (pEl->ElPrevious && pEl->ElNext &&
                      TypeHasException (ExcHidden, pEl->ElPrevious->ElTypeNumber,
                                        pEl->ElPrevious->ElStructSchema) &&
                      TypeHasException (ExcHidden, pEl->ElNext->ElTypeNumber,
                                        pEl->ElNext->ElStructSchema))
                    {
                      pParent = pAb->AbEnclosing;
                      while (pParent && pParent->AbElement &&
                             pParent->AbElement->ElStructSchema &&
                             pParent->AbElement->ElStructSchema->SsName &&
                             !strcmp (pParent->AbElement->ElStructSchema->SsName, "Template"))
                        // Skip template elements
                        pParent = pParent->AbEnclosing;
                      if (pParent)
                        {
                          pParent->AbInLine = TRUE;
                          pParent->AbBuildAll = TRUE;
                        }
                    }
                  pAb->AbVisibility = 0;
                  pAb->AbDead = TRUE;
                  pAb->AbInLine = FALSE;
                }
              else if (pAb->AbDisplay == 'B' &&
                       pAb->AbElement->ElStructSchema->SsIsXml)
                /* display: inline */
                {
                  pAb->AbInLine = TRUE;
                  pAb->AbBuildAll = TRUE;
                }
              else if (pAb->AbDisplay == 'I')
                /* display: inline */
                {
                  pParent = pAb->AbEnclosing;
                  while (pParent && pParent->AbElement && pParent->AbEnclosing &&
                         pParent->AbElement->ElStructSchema &&
                         pParent->AbElement->ElStructSchema->SsName &&
                         // look for an enclosing block element
                         (pParent->AbDisplay == 'I' ||
                          // U = I for generic XML documents
                         (pParent->AbDisplay == 'U' &&
                          pParent->AbElement->ElStructSchema->SsIsXml) ||
                          !strcmp (pParent->AbElement->ElStructSchema->SsName, "Template")))
                    pParent = pParent->AbEnclosing;
                  if (pParent && !pParent->AbInLine)
                    {
                      pParent->AbInLine = TRUE;
                      pParent->AbBuildAll = TRUE;
                    }
                  pAb->AbAcceptLineBreak = TRUE;
                  pAb->AbBuildAll = TRUE;
                  pAb->AbNotInLine = FALSE;
                }
              else if (pAb->AbDisplay == 'L')
                {
                  /* display: list-item */
                  appl = CreateListItemMarker (pAb, pDoc, pAttr);
                }
              else if (pAb->AbDisplay != 'U')
                {
                  /* display: block */
                  if (pAb->AbFloat != 'N')
                    pAb->AbNotInLine = FALSE;
                  pAb->AbAcceptLineBreak = FALSE;
                }
              else if (!strcmp (pAb->AbElement->ElStructSchema->SsName, "Template") &&
                       pAb->AbEnclosing && pAb->AbEnclosing->AbInLine)
                {
                  // when a use or a repeat is winthin a block of lines
                  pAb->AbDisplay = 'I';
                  pAb->AbAcceptLineBreak = TRUE;
                }
            }
          else if (pEl->ElParent == NULL)
            {
              pAb->AbDisplay = 'U';
              appl = TRUE;
            }
          break;
        case PtListStyleType:
          pAb->AbListStyleType = CharRule (pPRule, pEl,
                                           pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbListStyleType = 'D';  /* disc */
              appl = TRUE;
            }
          break;
        case PtListStylePosition:
          pAb->AbListStylePosition = CharRule (pPRule, pEl,
                                               pAb->AbDocView, &appl);
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbListStylePosition = 'O';  /* outside */
              appl = TRUE;
            }
          break;
        case PtListStyleImage:
          if (pPRule->PrPresMode == PresInherit)
            pAb->AbListStyleImage = CharRule (pPRule, pEl,
                                              pAb->AbDocView, &appl);
          else if (pPRule->PrPresMode == PresImmediate &&
                   pPRule->PrViewNum == viewSch)
            {
              if (pPRule->PrIntValue <= 0)
                /* it's a rule "ListStylePicture = None" */
                {
                  pAb->AbListStyleImage = 'N';
                  if (pAb->AbPictListStyle &&
                      pAb->AbLeafType == LtCompound)
                    /* remove the list-style image */
                    {
                      image = (ThotPictInfo *)pAb->AbPictListStyle;
                      CleanPictInfo (image);
                      TtaFreeMemory (image->PicFileName);
                      TtaFreeMemory (pAb->AbPictListStyle);
                      pAb->AbPictListStyle = NULL;
                    } 
                  appl = TRUE;
                }
              else
                /* there is an URL for the list-style image */
                {
                  appl = TRUE;
                  if (pAb->AbLeafType == LtCompound)
                    {
                      pAb->AbListStyleImage = 'Y';
                      if (pSchP == NULL)
                        pSchP = PresentationSchema (pDoc->DocSSchema, pDoc);
                      pConst = &pSchP->PsConstant[pPRule->PrIntValue - 1];
                      if (pConst->PdString && pConst->PdString[0] != EOS)
                        {
#ifndef _WINDOWS
                          if (pConst->PdString[0] == DIR_SEP)
#else  /* _WINDOWS */
                            if (pConst->PdString[0] == DIR_SEP ||
                                (pConst->PdString[1] == ':' &&
                                 pConst->PdString[2] == DIR_SEP))
#endif /* _WINDOWS */
                              /* absolute file name */
                              strncpy (fname, pConst->PdString, MAX_PATH - 1);
                            else
                              /* relative file name */
                              {
                                strncpy (directoryName, SchemaPath, MAX_PATH-1);
                                MakeCompleteName (pConst->PdString, "",
                                                  directoryName, fname, &i);
                              }
                          NewPictInfo (pAb, fname, UNKNOWN_FORMAT, True);
                        }
                    }
                }
            }
          if (!appl && pEl->ElParent == NULL)
            /* Pas de regle pour la racine, on met la valeur par defaut */
            {
              pAb->AbListStyleImage = 'N';  /* none */
              appl = TRUE;
            }
          break;
        case PtFloat:
          pAb->AbFloat = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          if (pAb->AbFloat != 'N')
            {
              if (pAb->AbEnclosing &&
                  pAb->AbEnclosing->AbElement &&
                  pAb->AbEnclosing->AbElement->ElStructSchema &&
                  (!strcmp (pAb->AbEnclosing->AbElement->ElStructSchema->SsName, "SVG") ||
                   !strcmp (pAb->AbEnclosing->AbElement->ElStructSchema->SsName, "MathML")))
                /* don't accept float within SVG and MathML */
                pAb->AbFloat = 'N';
              else
                {
                  if (pAb->AbEnclosing)
                    /* rule gather on the parent box */
                    pAb->AbEnclosing->AbBuildAll = TRUE;
                  pAb->AbAcceptLineBreak = FALSE;
                  pAb->AbBuildAll = TRUE;
                  pAb->AbNotInLine = FALSE;
                }
            }
          break;
        case PtClear:
          pAb->AbClear = CharRule (pPRule, pEl, pAb->AbDocView, &appl);
          break;
        case PtPosition:
          if (pAb->AbLeafType == LtCompound &&
              pPRule->PrPresMode == PresInherit)
            {
              pAbb = AbsBoxInherit (pPRule, pEl, pAb->AbDocView);
              if (pAbb == NULL)
                appl = FALSE;
              else
                {
                  if (pAbb->AbPositioning)
                    {
                      if (!pAb->AbPositioning)
                        NewAbPositioning (pAb);
                      pAb->AbPositioning->PnAlgorithm =
                        pAbb->AbPositioning->PnAlgorithm;
                    }
                  else
                    {
                      if (pAb->AbPositioning)
                        {
                          TtaFreeMemory (pAb->AbPositioning);
                          pAb->AbPositioning = NULL;
                        }
                    }
                  appl = TRUE;
                }
            } 
          else
            {
              if (pPRule->PrChrValue == 'S')  /* position: static */
                {
                  if (pAb->AbLeafType == LtCompound && pAb->AbPositioning)
                    {
                      TtaFreeMemory (pAb->AbPositioning);
                      pAb->AbPositioning = NULL;
                    }
                  appl = TRUE;
                }
              else if (pAb->AbLeafType == LtCompound)
                {
                  if (!pAb->AbPositioning)
                    NewAbPositioning (pAb);
                  switch (pPRule->PrChrValue)
                    {
                    case 'R':
                      pAb->AbPositioning->PnAlgorithm = PnRelative;
                      if (pAb->AbEnclosing)
                        /* rule gather on the parent box */
                        pAb->AbEnclosing->AbBuildAll = TRUE;
                      appl = TRUE;
                      break;
                    case 'A':
                      if (pAb->AbEnclosing)
                        /* rule gather on the parent box */
                        pAb->AbEnclosing->AbBuildAll = TRUE;
                      pAb->AbPositioning->PnAlgorithm = PnAbsolute;
                      appl = TRUE;
                      break;
                    case 'F':
                      //TtaGetEnvBoolean ("IGNORE_FIXED_POS", &ignorefix);
                      if (!ignorefix)
                        {
                          if (pDoc->DocViewRootAb[pAb->AbDocView - 1])
                            /* rule gather on the root box */
                            pDoc->DocViewRootAb[pAb->AbDocView - 1]->AbBuildAll = TRUE;
                        }
                      pAb->AbPositioning->PnAlgorithm = PnFixed;
                      appl = TRUE;
                      break;
                    default:
                      break;
                    }
                }
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
PtrPRule SearchPresRule (PtrElement pEl, PRuleType ruleType,
                         FunctionType funcType, ThotBool *isNew,
                         PtrDocument pDoc, int view)
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
              if (pPRule->PrViewNum == pDoc->DocView[view-1].DvPSchemaView &&
                  pPRule->PrType == ruleType)
                {
                  if (ruleType == PtFunction)
                    found = (pPRule->PrPresFunction == funcType);
                  else
                    found = TRUE;
                }
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
void RedispAbsBox (PtrAbstractBox pAb, PtrDocument pDoc)
{
  int         view;

  view = pAb->AbDocView - 1;
  pDoc->DocViewModifiedAb[view] = Enclosing (pAb, pDoc->DocViewModifiedAb[view]);
}
