/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Change specific presentation
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
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
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "unstructchange_f.h"

/*----------------------------------------------------------------------
  ApplyRuleSubTree
  ----------------------------------------------------------------------*/
static void   ApplyRuleSubTree (PtrElement pE, PRuleType ruleType,
                                PtrDocument pDoc, PtrPRule * pPRule,
                                int view, ThotBool display)
{
  PtrAbstractBox      pAbb, pAbbF;
  PtrPSchema          pSPR;
  PtrAttribute        pAttr;
  ThotBool            complete;

  if (pE->ElTerminal)
    pE = NULL;
  else
    /* on passe au premier fils */
    pE = pE->ElFirstChild;
  /* traite tous les fils */
  while (pE)
    {
      /* 1er pave de l'element dans la view */
      pAbb = pE->ElAbstractBox[view - 1];
      if (pAbb == NULL)
        /* no abstract box */
        if (ruleType == PtVisibility  || ruleType == PtVis|| ruleType == PtDisplay)
          /* it's a visibility rule. Try to create the abstract box */
          pAbb = AbsBoxesCreate (pE, pDoc, view, True, True, &complete);
      if (pAbb)
        {
          /* on ne traite pas les paves morts */
          while (pAbb && pAbb->AbPresentationBox && pAbb->AbDead)
            pAbb = pAbb->AbNext;
          if (pAbb && pAbb->AbDead && !pAbb->AbPresentationBox)
            pAbb = NULL;
        }
      if (pAbb == NULL)
        /* cet element n'a pas de pave, mais ses descendants en */
        /* ont peut etre... */
        ApplyRuleSubTree (pE, ruleType, pDoc, pPRule, view, display);
      else
        {
          /* il y a un element descendant dont les paves peuvent heriter
             de pAb. On parcourt ses paves dans la vue */
          do
            {
              *pPRule = SearchRulepAb (pDoc, pAbb, &pSPR, ruleType, FnAny,
                                       TRUE, &pAttr);
              if (*pPRule)
                {
                  if ((*pPRule)->PrPresMode == PresInherit &&
                      ((*pPRule)->PrInheritMode == InheritParent ||
                       (*pPRule)->PrInheritMode == InheritGrandFather))
                    /* la regle de ce pave herite de l'ascendant, */
                    /* on applique la regle */
                    if (ApplyRule (*pPRule, pSPR, pAbb, pDoc, pAttr, pAbb))
                      {
                        SetChange (pAbb, pDoc, ruleType, (FunctionType)0);
                        if (display)
                          RedispAbsBox (pAbb, pDoc);
                        if (!pAbb->AbPresentationBox)
                          ApplyInherit (ruleType, pAbb, pDoc, display);
                      }
                }
              if (!pAbb->AbPresentationBox)
                /* c'est le pave principal de l'element, on traite */
                /* les paves crees par l'element au niveau inferieur */
                {
                  pAbbF = pAbb->AbFirstEnclosed;
                  while (pAbbF)
                    {
                      if (pAbbF->AbElement == pE)
                        {
                          *pPRule = SearchRulepAb (pDoc, pAbbF, &pSPR,ruleType,
                                                   FnAny, TRUE, &pAttr);
                          if (*pPRule)
                            {
                              if ((*pPRule)->PrPresMode == PresInherit
                                  && (*pPRule)->PrInheritMode == InheritParent)
                                if (ApplyRule (*pPRule, pSPR, pAbbF,
                                               pDoc, pAttr, pAbb))
                                  {
                                    SetChange (pAbbF, pDoc, ruleType,
                                               (FunctionType)0);
                                    if (display)
                                      RedispAbsBox (pAbbF, pDoc);
                                  }
                            }
                        }
                      pAbbF = pAbbF->AbNext;
                    }
                }
              pAbb = pAbb->AbNext;	/* passe au pave suivant */
            }
          /* on arrete s'il n'y a pas de pave suivant ou si le pave
             suivant n'appartient pas a l'element */
          while (pAbb && pAbb->AbElement == pE);
        }
      pE = pE->ElNext;	/* on traite l'element suivant */
    }
}

/*----------------------------------------------------------------------
  ApplyInherit 
  The ruleType presentation rule has been applied, check if siblings
  or children inherit from this rule.
  The parameter display is TRUE when boxes should be redisplayed after
  any change.
  ----------------------------------------------------------------------*/
void ApplyInherit (PRuleType ruleType, PtrAbstractBox pAb,
                   PtrDocument pDoc, ThotBool display)
{
  PtrElement          pEl;
  int                 view;
  PtrAbstractBox      pAbCur;
  PtrPRule            pRule = NULL;
  PtrPSchema          pSchP;
  PtrAttribute        pAttr;

  pEl = pAb->AbElement;
  view = pAb->AbDocView;
  if (pEl->ElNext)
    {
      /* l'element a un frere suivant. Celui-ci herite-t-il de son
         precedent ? */
      pAbCur = NULL;
      while (pEl->ElNext && pAbCur == NULL)
        {
          pEl = pEl->ElNext;
          pAbCur = pEl->ElAbstractBox[view - 1];
          /* saute les paves de presentation */
          while (pAbCur && pAbCur->AbPresentationBox)
            pAbCur = pAbCur->AbNext;
          if (pAbCur)
            if (pAbCur->AbDead)
              pAbCur = NULL;
        }
      if (pAbCur)
        {
          /* il y a un element suivant dont le pave pAbCur pourrait
             heriter de pAb. Cherche sa regle de presentation */
          pRule = SearchRulepAb (pDoc, pAbCur, &pSchP, ruleType, FnAny,
                                 TRUE, &pAttr);
          if (pRule)
            {
              if (pRule->PrPresMode == PresInherit &&
                  pRule->PrInheritMode == InheritPrevious)
                /* la regle de cet element herite du precedent,
                   on applique la regle */
                if (ApplyRule (pRule, pSchP, pAbCur, pDoc, pAttr, pAb))
                  {
                    SetChange (pAbCur, pDoc, ruleType, (FunctionType)0);
                    ApplyInherit (ruleType, pAbCur, pDoc, display);
                    if (display)
                      RedispAbsBox (pAbCur, pDoc);
                  }
            }
        }
      pEl = pAb->AbElement;
    }
  if (!pEl->ElTerminal && pEl->ElFirstChild)
    /* check if children inherit some rules? */
    ApplyRuleSubTree (pEl, ruleType, pDoc, &pRule, view, display);
  if (pEl->ElParent)
    {
      /* l'element a un ascendant. Celui-ci herite-t-il de son premier */
      /* descendant ? */
      pAbCur = NULL;
      while (pEl->ElParent && !pAbCur)
        {
          pEl = pEl->ElParent;
          /* skip presentation boxes */
          pAbCur = pEl->ElAbstractBox[view - 1];
          while (pAbCur && pAbCur->AbPresentationBox)
            pAbCur = pAbCur->AbNext;
          if (pAbCur && pAbCur->AbDead)
            pAbCur = NULL;
        }
      if (pAbCur)
        {
          /* apply delayed rules */
          ApplyDelayedRules (ruleType, pAbCur, pDoc);

          /* il y a un element ascendant dont le pave pAbCur pourrait
             heriter de pAb */
          pRule = SearchRulepAb (pDoc, pAbCur, &pSchP, ruleType, FnAny,
                                 TRUE, &pAttr);
          if (pRule && pRule->PrPresMode == PresInherit &&
              pRule->PrInheritMode == InheritChild)
            /* la regle de cet element herite du descendant, on */
            /* applique la regle */
            if (ApplyRule (pRule, pSchP, pAbCur, pDoc, pAttr, pAbCur))
              {
                SetChange (pAbCur, pDoc, ruleType, (FunctionType)0);
                ApplyInherit (ruleType, pAbCur, pDoc, display);
                if (display)
                  RedispAbsBox (pAbCur, pDoc);
              }
        }
    }

  if (!pAb->AbPresentationBox)
    /* ce n'est pas un pave de presentation. On regarde si les paves de */
    /* presentation crees par l'element heritent de leur createur */
    {
      /* on regarde d'abord les paves crees devant (par CreateBefore) */
      pEl = pAb->AbElement;
      pAbCur = pAb->AbPrevious;
      while (pAbCur != NULL)
        if (!pAbCur->AbPresentationBox || pAbCur->AbElement != pEl)
          /* ce n'est pas un pave de presentation de l'element, on arrete*/
          pAbCur = NULL;
        else
          {
            pRule = SearchRulepAb (pDoc, pAbCur, &pSchP, ruleType, FnAny,
                                   TRUE, &pAttr);
            if (pRule != NULL)
              if (pRule->PrPresMode == PresInherit &&
                  pRule->PrInheritMode == InheritCreator &&
                  /* skip list-item markers */
                  pAbCur->AbPresentationBox && pAbCur->AbTypeNum != 0)
                /* la regle de ce pave herite de son createur,
                   on l'applique */
                if (ApplyRule (pRule, pSchP, pAbCur, pDoc, pAttr, pAbCur))
                  {
                    SetChange (pAbCur, pDoc, ruleType, (FunctionType)0);
                    ApplyInherit (ruleType, pAbCur, pDoc, display);
                    if (display)
                      RedispAbsBox (pAbCur, pDoc);
                  }
            /* examine le pave precedent */
            pAbCur = pAbCur->AbPrevious;
          }
      /* on regarde les paves crees derriere (par CreateAfter) */
      pAbCur = pAb->AbNext;
      while (pAbCur != NULL)
        if (!pAbCur->AbPresentationBox || pAbCur->AbElement != pEl)
          /* ce n'est pas un pave de presentation de l'element, on arrete*/
          pAbCur = NULL;
        else
          {
            pRule = SearchRulepAb (pDoc, pAbCur, &pSchP, ruleType, FnAny,
                                   TRUE, &pAttr);
            if (pRule != NULL)
              if (pRule->PrPresMode == PresInherit &&
                  pRule->PrInheritMode == InheritCreator)
                /* la regle de ce pave herite de son createur,
                   on l'applique */
                if (ApplyRule (pRule, pSchP, pAbCur, pDoc, pAttr, pAbCur))
                  {
                    SetChange (pAbCur, pDoc, ruleType, (FunctionType)0);
                    ApplyInherit (ruleType, pAbCur, pDoc, display);
                    if (display)
                      RedispAbsBox (pAbCur, pDoc);
                  }
            /* examine le pave suivant */
            pAbCur = pAbCur->AbNext;
          }
      /* on regarde les paves crees au niveau inferieur (par CreateFirst et
         CreateLast) */
      pAbCur = pAb->AbFirstEnclosed;
      while (pAbCur)
        {
          if (pAbCur->AbPresentationBox && pAbCur->AbElement == pEl)
            /* c'est un pave de presentation de l'element, on le traite */
            {
              pRule = SearchRulepAb (pDoc, pAbCur, &pSchP, ruleType, FnAny,
                                     TRUE, &pAttr);
              if (pRule)
                if (pRule->PrPresMode == PresInherit &&
                    pRule->PrInheritMode == InheritCreator)
                  /* la regle de ce pave herite de son createur,
                     on l'applique */
                  if (ApplyRule (pRule, pSchP, pAbCur, pDoc, pAttr, pAb))
                    {
                      SetChange (pAbCur, pDoc, ruleType, (FunctionType)0);
                      ApplyInherit (ruleType, pAbCur, pDoc, display);
                      if (display)
                        RedispAbsBox (pAbCur, pDoc);
                    }
            }
          /* examine le pave suivant */
          pAbCur = pAbCur->AbNext;
        }
      /* check the list item markers created as grand children of the
         element */
      pEl = pAb->AbElement;
      if (!pEl->ElTerminal && pEl->ElFirstChild &&
          pEl->ElFirstChild->ElAbstractBox[view - 1])
        {
          pAbCur = pEl->ElFirstChild->ElAbstractBox[view-1]->AbFirstEnclosed;
          while (pAbCur)
            {
              if (!pAbCur->AbPresentationBox)
                pAbCur = NULL;
              else
                {
                  if (pAbCur->AbElement == pEl)
                    {
                      pRule = SearchRulepAb (pDoc, pAbCur, &pSchP, ruleType,
                                             FnAny, TRUE, &pAttr);
                      if (pRule)
                        if (pRule->PrPresMode == PresInherit &&
                            pRule->PrInheritMode == InheritCreator)
                          /* la regle de ce pave herite de son createur,
                             on l'applique */
                          if (ApplyRule (pRule, pSchP, pAbCur, pDoc, pAttr, pAbCur))
                            {
                              SetChange (pAbCur, pDoc, ruleType, (FunctionType)0);
                              ApplyInherit (ruleType, pAbCur, pDoc, display);
                              if (display)
                                RedispAbsBox (pAbCur, pDoc);
                            }
                    }
                  pAbCur = pAbCur->AbNext;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int NumTypePRuleAPI (PtrPRule pRule)
{
  switch (pRule->PrType)
    {
    case PtVisibility:
      return PRVisibility;
      break;
    case PtVis:
      return PRVis;
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
    case PtMarginTop:
      return PRMarginTop;
      break;
    case PtMarginRight:
      return PRMarginRight;
      break;
    case PtMarginBottom:
      return PRMarginBottom;
      break;
    case PtMarginLeft:
      return PRMarginLeft;
      break;
    case PtPaddingTop:
      return PRPaddingTop;
      break;
    case PtPaddingRight:
      return PRPaddingRight;
      break;
    case PtPaddingBottom:
      return PRPaddingBottom;
      break;
    case PtPaddingLeft:
      return PRPaddingLeft;
      break;
    case PtBorderTopWidth:
      return PRBorderTopWidth;
      break;
    case PtBorderRightWidth:
      return PRBorderRightWidth;
      break;
    case PtBorderBottomWidth:
      return PRBorderBottomWidth;
      break;
    case PtBorderLeftWidth:
      return PRBorderLeftWidth;
      break;
    case PtXRadius:
      return PRXRadius;
      break;
    case PtYRadius:
      return PRYRadius;
      break;
    case PtTop:
      return PRTop;
      break;
    case PtRight:
      return PRRight;
      break;
    case PtBottom:
      return PRBottom;
      break;
    case PtLeft:
      return PRLeft;
      break;
    case PtBackgroundHorizPos:
      return PRBackgroundHorizPos;
      break;
    case PtBackgroundVertPos:
      return PRBackgroundVertPos;
      break;
    case PtBorderTopColor:
      return PRBorderTopColor;
      break;
    case PtBorderRightColor:
      return PRBorderRightColor;
      break;
    case PtBorderBottomColor:
      return PRBorderBottomColor;
      break;
    case PtBorderLeftColor:
      return PRBorderLeftColor;
      break;
    case PtBorderTopStyle:
      return PRBorderTopStyle;
      break;
    case PtBorderRightStyle:
      return PRBorderRightStyle;
      break;
    case PtBorderBottomStyle:
      return PRBorderBottomStyle;
      break;
    case PtBorderLeftStyle:
      return PRBorderLeftStyle;
      break;
    case PtDisplay:
      return PRDisplay;
      break;
    case PtListStyleType:
      return PRListStyleType;
      break;
    case PtListStyleImage:
      return PRListStyleImage;
      break;
    case PtListStylePosition:
      return PRListStylePosition;
      break;
    case PtFloat:
      return PRFloat;
      break;
    case PtClear:
      return PRClear;
      break;
    case PtPosition:
      return PRPosition;
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
    case PtVariant:
      return PRVariant;
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
    case PtDirection:
      return PRDirection;
      break;
    case PtUnicodeBidi:
      return PRUnicodeBidi;
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
    case PtOpacity:
      return PROpacity;
      break;
    case PtFillOpacity:
      return PRFillOpacity;
      break;
    case PtStrokeOpacity:
      return PRStrokeOpacity;
      break;
    case PtStopOpacity:
      return PRStopOpacity;
      break;
    case PtMarker:
      return PRMarker;
      break;
    case PtMarkerStart:
      return PRMarkerStart;
      break;
    case PtMarkerMid:
      return PRMarkerMid;
      break;
    case PtMarkerEnd:
      return PRMarkerEnd;
      break;
    case PtFillRule:
      return PRFillRule;
      break;
    case PtBackground:
      return PRBackground;
      break;
    case PtForeground:
      return PRForeground;
      break;
    case PtColor:
      return PRColor;
      break;
    case PtStopColor:
      return PRStopColor;
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
int RuleSetIn (register int num, RuleSet RuleS1)
{
  if (num >= 0 && num < SETSIZE * 8)
    return RuleS1[num >> 3] & (1 << (num & 7));
  else
    return 0;
}


/*----------------------------------------------------------------------
  RuleSetPut ajoute un element a` un ensemble.			
  ----------------------------------------------------------------------*/
void RuleSetPut (RuleSet RuleS1, int num)
{
  if (num >= 0 && num < SETSIZE * 8)
    RuleS1[num >> 3] |= (1 << (num & 7));
}


/*----------------------------------------------------------------------
  RuleSetClr met a` 0 l'ensemble RuleS1.				
  ----------------------------------------------------------------------*/
void RuleSetClr (RuleSet RuleS1)
{
  unsigned char *s1;
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
ThotBool BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
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
PtrAbstractBox AbsBoxOfEl (PtrElement pEl, int view)
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
void    ApplyNewRule (PtrDocument pDoc, PtrPRule pRule, PtrElement pEl)
{
  PtrAbstractBox      pAb;
  int                 view;
  ThotBool            stop;

  /* parcourt toutes les vues du document */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    if (pEl->ElAbstractBox[view - 1] != NULL)
      /* l'element traite' a un pave dans cette view */
      if (pDoc->DocView[view - 1].DvSSchema == pDoc->DocSSchema &&
          pDoc->DocView[view - 1].DvPSchemaView == pRule->PrViewNum)
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
          if (pAb)
            /* applique la regle de presentation specifique a ce pave' */
            if (ApplyRule (pRule, NULL, pAb, pDoc, NULL, pAb))
              {
                if (pRule->PrType == PtFunction)
                  SetChange (pAb, pDoc, pRule->PrType,
                             (FunctionType)pRule->PrPresFunction);
                else
                  SetChange (pAb, pDoc, pRule->PrType, (FunctionType)0);
                ApplyInherit (pRule->PrType, pAb, pDoc, TRUE);
                /* indique le pave a faire reafficher */
                RedispAbsBox (pAb, pDoc);
              }
        }
}


/*----------------------------------------------------------------------
  RemoveSpecifPres supprime toutes les regles de presentation specifiques
  associees a l'element pEl.				
  ----------------------------------------------------------------------*/
static void RemoveSpecifPres (PtrElement pEl, PtrDocument pDoc,
                              RuleSet rules, int viewToApply)
{
  PtrPRule            pRule, pR, pRS;
  NotifyPresentation  notifyPres;
  RuleSet             rulesS;
  Document            doc;
  int                 viewSch;
  ThotBool            found;

  /* type de cette view */
  viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
  pRule = pEl->ElFirstPRule;
  if (pRule == NULL)
    return;
  pR = NULL;
  found = FALSE;
  doc = IdentDocument (pDoc);
  /* manage all specific presentation rules of the element if it still exists */
  while (pRule && pEl->ElStructSchema)
    if (pRule->PrViewNum != viewSch || !RuleSetIn (pRule->PrType, rules))
      /* skip this rule */
      {
        pR = pRule;
        pRule = pR->PrNextPRule;
      }
    else
      {
        found = TRUE;
        /* la regle concerne la view traitee */
        /* retire la regle de la chaine des regles de presentation */
        /* specifique de l'element */
        pRS = pRule->PrNextPRule;	/* regle a traiter apres */
        notifyPres.event = TtePRuleDelete;
        notifyPres.document = doc;
        notifyPres.element = (Element) pEl;
        notifyPres.pRule = (PRule) pRule;
        notifyPres.pRuleType = NumTypePRuleAPI (pRule);
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
            ApplyStandardRule (pEl, pDoc, pRule->PrType, pRule->PrPresFunction, viewSch);
            notifyPres.event = TtePRuleDelete;
            notifyPres.document = doc;
            notifyPres.element = (Element) pEl;
            notifyPres.pRule = NULL;
            notifyPres.pRuleType = NumTypePRuleAPI (pRule);
            /* libere la regle */
            FreePresentRule (pRule, pEl->ElStructSchema);
            CallEventType ((NotifyEvent *) & notifyPres, FALSE);
          }
        /* passe a la regle suivante */
        pRule = pRS;
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
  KillAllListItemMarkers
  Kill all list item markers in the pAbsBox substree.
  If self, kills also the list item markers generated by pAbsBox as previous
  siblings
  Returns the root of the subtree to be redisplayed
  ----------------------------------------------------------------------*/
static PtrAbstractBox KillAllListItemMarkers (PtrAbstractBox pAbsBox,
                                              ThotBool self)
{
  PtrAbstractBox    pAb, pAbRedisp, pAbR;

  pAbRedisp = NULL;
  if (self)
    {
      pAb = pAbsBox->AbPrevious;
      while (pAb && pAb->AbElement == pAbsBox->AbElement)
        {
          if (pAb->AbPresentationBox && pAb->AbTypeNum == 0)
            /* it's a list item marker */
            {
              pAb->AbDead = TRUE;
              pAbRedisp = pAb->AbEnclosing;
            }
          pAb = pAb->AbPrevious;
        }
    }
  pAb = pAbsBox->AbFirstEnclosed;
  while (pAb)
    {
      if (pAb->AbPresentationBox)
        {
          if (pAb->AbTypeNum == 0)
            /* it's a list item marker */
            {
              pAb->AbDead = TRUE;
              if (!pAbRedisp)
                pAbRedisp = pAb->AbEnclosing;
            }
        }
      else
        {
          pAbR = KillAllListItemMarkers (pAb, False);
          if (!pAbRedisp)
            pAbRedisp = pAbR;
        }
      pAb = pAb->AbNext;
    }
  return (pAbRedisp);
}

/*----------------------------------------------------------------------
  CreateAllListItemMarkers
  Create a list item marker for each box in the pAbsBox substree that
  has "display: list-item"
  ----------------------------------------------------------------------*/
static void CreateAllListItemMarkers (PtrAbstractBox pAbsBox, PtrDocument pDoc)
{
  PtrAbstractBox   pAb;

  pAb = pAbsBox;
  if (!pAb->AbPresentationBox &&
      pAb->AbDisplay == 'L' && pAb->AbListStyleType != 'N')
    /* this box has "display: list-item", create its list item marker */
    CreateListItemMarker (pAb, pDoc, NULL);
  /* check its descendants */
  pAb = pAbsBox->AbFirstEnclosed;
  while (pAb)
    {
      if (!pAb->AbPresentationBox)
        CreateAllListItemMarkers (pAb, pDoc);
      pAb = pAb->AbNext;
    }
}

/*----------------------------------------------------------------------
  ApplyPRuleAndRedisplay: apply a presentation rule, update the view
  and apply the inheritance of the rule.
  ----------------------------------------------------------------------*/
static void ApplyPRuleAndRedisplay (PtrAbstractBox pAb, PtrDocument pDoc,
                                    PtrAttribute pAttr, PtrPRule pRP,
                                    PtrPSchema pSPR)
{
  PtrAbstractBox   pAbRedisp = NULL;
  
  if (pRP == NULL)
    return;

  if (pRP->PrType == PtListStyleType ||
      pRP->PrType == PtListStyleImage ||
      pRP->PrType == PtListStylePosition)
    /* it's a rule that may change list item markers associated with the box
       and its subtree. Delete all these list item markers */
    pAbRedisp = KillAllListItemMarkers (pAb, TRUE);
  ApplyRule (pRP, pSPR, pAb, pDoc, pAttr, pAb);
  /* indicate that the box has changed and has to be redisplayed */
  if (pRP->PrType == PtFunction)
    SetChange (pAb, pDoc, pRP->PrType, (FunctionType)pRP->PrPresFunction);
  else
    if (pRP->PrType == PtListStyleType ||
        pRP->PrType == PtListStyleImage ||
        pRP->PrType == PtListStylePosition)
      {
        ApplyInherit (pRP->PrType, pAb, pDoc, TRUE);
        CreateAllListItemMarkers (pAb, pDoc);
      }
    else
      SetChange (pAb, pDoc, pRP->PrType, (FunctionType)0);
  /* redisplay the box */
  if (pAbRedisp == pAb->AbEnclosing)
    RedispAbsBox (pAbRedisp, pDoc);
  else
    RedispAbsBox (pAb, pDoc);
  /* Apply the rule of this type to all boxes that inherit this propoerty
     from the modified box */
  if (pRP->PrType != PtListStyleType &&
      pRP->PrType != PtListStyleImage &&
      pRP->PrType != PtListStylePosition)
    ApplyInherit (pRP->PrType, pAb, pDoc, TRUE);
}

/*----------------------------------------------------------------------
  ApplyStandardRule	applique a l'element pEl du document	
  pDoc la regle de presentation standard de type ruleType	
  pour la view viewSch.						
  ----------------------------------------------------------------------*/
void  ApplyStandardRule (PtrElement pEl, PtrDocument pDoc,
                         PRuleType ruleType, FunctionType funcType,
                         int viewSch)
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
                if (pRP)
                  ApplyPRuleAndRedisplay(pAb, pDoc, pAttr, pRP, pSPR);
              }
          }
}

/*----------------------------------------------------------------------
  NextAbstractBox: search the next abstract box.
  ----------------------------------------------------------------------*/
static PtrAbstractBox     NextAbstractBox (PtrAbstractBox pAb)
{
  PtrAbstractBox      pNextAb;

  /* Il y a un premier fils different du pave reference ? */
  if (pAb->AbFirstEnclosed != NULL && !pAb->AbDead)
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
static void RemoveFunctionPRule (PtrPRule pPres, PtrAbstractBox pAb,
                                 PtrDocument pDoc)
{
  ThotPictInfo  *image;

  if (pPres->PrPresFunction == FnBackgroundPicture && pAb->AbPictBackground)
    {
      image = (ThotPictInfo *)pAb->AbPictBackground;
      CleanPictInfo (image);
      TtaFreeMemory (image->PicFileName);
      TtaFreeMemory (pAb->AbPictBackground);
      pAb->AbPictBackground = NULL;
    }
  else if (pPres->PrPresFunction == FnBackgroundRepeat &&
           pAb->AbPictBackground)
    ((ThotPictInfo *) (pAb->AbPictBackground))->PicPresent = FillFrame;
  else if (pPres->PrPresFunction == FnShowBox)
    pAb->AbFillBox = FALSE;
  
  pAb->AbAspectChange = TRUE;
  RedispAbsBox (pAb, pDoc);
}

/*----------------------------------------------------------------------
  ApplyASpecificStyleRule
  Redisplay boxes of element pEl that are concerned by removing the
  presentation function pRule
  Return TRUE if the rule is applied.
  ----------------------------------------------------------------------*/
void  ApplyASpecificStyleRule (PtrPRule pRule, PtrElement pEl,
                               PtrDocument pDoc, ThotBool remove)
{
  PtrAbstractBox  pAb, pParent;
  PtrPRule	      pCurrentRule, pRP;
  PtrElement      pPage;
  PtrPSchema	    pSPR;
  PtrAttribute	  pAttr;
  PRuleType	      ruleType;
  int             viewSch;
  int             view, d;
  ThotPictInfo   *image;
  ThotBool	      found, enclosed, complete;

  enclosed = FALSE;
  /* do nothing if the document no longer exists */
  if (pDoc)
    /* examine all abstract boxes of element */
    for (view = 0; view < MAX_VIEW_DOC; view++)
      {
        /* the abstract box of the root element */
        pAb = pEl->ElAbstractBox[view];
        if (pAb == NULL &&
          /* no abstract box */
            (pRule->PrType == PtVisibility || pRule->PrType == PtVis ||
             pRule->PrType == PtDisplay))
          {
            d = 0;
            while (d < MAX_DOCUMENTS - 1 && LoadedDocument[d] != pDoc)
              d++;
            if (d < MAX_DOCUMENTS - 1 && documentDisplayMode[d] != NoComputedDisplay)
              /* it's a visibility rule. Try to create the abstract box */
              pAb = AbsBoxesCreate (pEl, pDoc, view+1, True, True, &complete);
          }
        /* the schema view associatde with the current view */
        viewSch = pDoc->DocView[view].DvPSchemaView;
        while (pAb)
          {
            /* process each presentation rule */
            pCurrentRule = pRule;
            found = FALSE;
            if (pCurrentRule)
              {
                ruleType = pCurrentRule->PrType;
                /* is the view concerned by the presentation rule ? */
                if (pCurrentRule->PrViewNum == 1 ||
                    pCurrentRule->PrViewNum == viewSch)
                  {
                    if (ruleType == PtFunction &&
                        pCurrentRule->PrPresFunction == FnPage)
                      {
                        /* generate or remove a page break */
                        if (remove)
                          {
                            pPage = pEl->ElPrevious;
                            if (pPage && pPage->ElTypeNumber == PageBreak + 1)
                              DeleteElement (&pPage, pDoc);
                          }
                        else
                          pPage = CreateSibling (pDoc, pEl, TRUE, TRUE, PageBreak + 1, pEl->ElStructSchema, FALSE);
                        found = TRUE;
                      }
                    else
                      {
                        /* checks if the abstract box is concerned by the rule */
                        pRP = SearchRulepAb (pDoc, pAb, &pSPR, ruleType,
                                             pCurrentRule->PrPresFunction, TRUE, &pAttr);
                        if (pRP == pCurrentRule || remove)
                          {
                            found = TRUE;
                            if (remove &&
                                (ruleType == PtFunction || ruleType == PtListStyleImage) &&
                                pAb->AbLeafType == LtCompound)
                              {
                                if (ruleType == PtFunction)
                                /* remove a PtFunction rule */
                                  RemoveFunctionPRule (pCurrentRule, pAb, pDoc);
                                if (ruleType == PtListStyleImage &&
                                    pAb->AbPictListStyle)
                                  {
                                    image = (ThotPictInfo *)pAb->AbPictListStyle;
                                    CleanPictInfo (image);
                                    TtaFreeMemory (image->PicFileName);
                                    TtaFreeMemory (pAb->AbPictListStyle);
                                    pAb->AbPictListStyle = NULL;
                                  }
                                if (pRP && pRP != pCurrentRule)
                                  ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
                              }
                            else if (pRP && (!remove || pRP != pCurrentRule))
                                ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
                            else if (remove && pAb->AbLeafType == LtCompound &&
                                     pAb->AbPositioning)
                              {
                                if (ruleType == PtPosition)
                                  {
                                    TtaFreeMemory (pAb->AbPositioning);
                                    pAb->AbPositioning = NULL;
                                    pAb->AbPositionChange = TRUE;
                                  }
                                else if (ruleType == PtTop)
                                  {
                                    pAb->AbPositioning->PnTopDistance = 0;
                                    pAb->AbPositioning->PnTopUnit = UnUndefined;
                                    pAb->AbVertPosChange = TRUE;
                                    pAb->AbPositionChange = TRUE;
                                  }
                                else if (ruleType == PtRight)
                                  {
                                    pAb->AbPositioning->PnRightDistance = 0;
                                    pAb->AbPositioning->PnRightUnit = UnUndefined;
                                    pAb->AbHorizPosChange = TRUE;
                                    pAb->AbPositionChange = TRUE;
                                  }
                                else if (ruleType == PtBottom)
                                  {
                                    pAb->AbPositioning->PnBottomDistance = 0;
                                    pAb->AbPositioning->PnBottomUnit = UnUndefined;
                                    pAb->AbVertPosChange = TRUE;
                                    pAb->AbPositionChange = TRUE;
                                  }
                                else if (ruleType == PtLeft)
                                  {
                                    pAb->AbPositioning->PnLeftDistance = 0;
                                    pAb->AbPositioning->PnLeftUnit = UnUndefined;
                                    pAb->AbHorizPosChange = TRUE;
                                    pAb->AbPositionChange = TRUE;
                                  }
                              }
                          }
                      }
                  }
              }
            if (found)
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
void  ApplyAGenericStyleRule (Document doc, PtrSSchema pSS, int elType,
                              int attrType, int presBox, PtrPRule pRule,
                              ThotBool remove)
{
  PtrPRule        pCurrentRule, pRP;
  PtrPSchema      pSPR;
  PtrAttribute    pAttr;
  PtrDocument     pDoc, pSelDoc;
  PtrElement      pFirstSel, pLastSel;
  PtrAbstractBox  pAb, pAbRedisp;
  PRuleType       ruleType;
  int             firstChar, lastChar;
  int             viewSch;
  int             view;
  ThotPictInfo   *image;
  ThotBool        selectionOK;
  ThotBool        found;

  pDoc = LoadedDocument[doc - 1];
  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel,
                                     &firstChar, &lastChar);
  /* do nothing if the document no longer exists */
  if (pDoc != NULL)
    /* examine all abstract boxes of elements */
    for (view = 0; view < MAX_VIEW_DOC; view++)
      {
        /* the abstract box of the root element */
        pAb = pDoc->DocViewRootAb[view];
        /* the schema view associated with the current view */
        viewSch = pDoc->DocView[view].DvPSchemaView;
        while (pAb)
          {
            /* there is probably a problem with visibility rules: pAb doesn't
               exist */
            found = FALSE;
            pAbRedisp = NULL;
            if (elType > 0)
              /* presentation rules are associated with an element type */
              found = (elType == MAX_BASIC_TYPE /* any element */ ||
                       pAb->AbElement->ElTypeNumber == elType &&
                       !strcmp (pAb->AbElement->ElStructSchema->SsName,
                                pSS->SsName));
            else if (attrType > 0)
              {
                /* presentation rules are associated with an attribute type */
                pAttr = pAb->AbElement->ElFirstAttr;
                while (!found && pAttr != NULL)
                  {
                    found = (pAttr->AeAttrNum == attrType &&
                             !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName));
                    if (!found)
                      pAttr = pAttr->AeNext;
                  }
              }
            else if (presBox > 0)
              /* presentation rules are associated with a presentation box */
              found = (pAb->AbPresentationBox && pAb->AbTypeNum == presBox &&
                       pAb->AbPSchema == PresentationSchema (pSS, pDoc));
            if (found)
              {
                /* process each presentation rule */
                pCurrentRule = pRule;
                if (pCurrentRule)
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
                            if (remove &&
                                (ruleType == PtFunction || ruleType == PtListStyleImage) &&
                                pAb->AbLeafType == LtCompound)
                              {
                                if (ruleType == PtFunction)
                                /* remove a PtFunction rule */
                                  RemoveFunctionPRule (pCurrentRule, pAb, pDoc);
                                if (ruleType == PtListStyleImage &&
                                    pAb->AbPictListStyle)
                                  {
                                    image = (ThotPictInfo *)pAb->AbPictListStyle;
                                    CleanPictInfo (image);
                                    TtaFreeMemory (image->PicFileName);
                                    TtaFreeMemory (pAb->AbPictListStyle);
                                    pAb->AbPictListStyle = NULL;
                                  }
                                if (pRP && pRP != pCurrentRule)
                                  ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
                              }
                            else if (pRP && (!remove || pRP != pCurrentRule))
                              ApplyPRuleAndRedisplay (pAb, pDoc, pAttr, pRP, pSPR);
                          }
                      }
                  }
              }
            /* get the next abstract box */
            pAb = NextAbstractBox (pAb);
            while (pAb &&
                   ((presBox == 0 && pAb->AbPresentationBox) ||
                    (presBox > 0  && !pAb->AbPresentationBox) ||
                    pAb->AbDead))
                pAb = NextAbstractBox (pAb);
            /* redisplay the element if needed */
            if (found)
              {
                /* update abstract image and redisplay */
                AbstractImageUpdated (pDoc);
                RedisplayDocViews (pDoc);
              }
          }
      }
}

/*----------------------------------------------------------------------
  RemoveSpecPresTree supprime les regles de presentation specifiques	
  contenues dans 'RulesS' attachees aux elements du	
  sous-arbre de racine pElRoot				
  ----------------------------------------------------------------------*/
void  RemoveSpecPresTree (PtrElement pElRoot, PtrDocument pDoc, RuleSet RulesS, int viewToApply)
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
  TtaIsCSSPRule

  Check whether a presentation rule is associated with a CSS rule
  Parameters:
  pRule: the presentation rule to be tested.
  Return value:
  TRUE if pRule is a CSS rule
  ----------------------------------------------------------------------*/
ThotBool            TtaIsCSSPRule (PRule pRule)
{
  ThotBool         ret;

  ret = FALSE;
  UserErrorCode = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* parameter pRule is correct */
    ret = (((PtrPRule) pRule)->PrSpecificity > 0);
  return ret;
}

/*----------------------------------------------------------------------
  TtaRemovePRule

  Removes a presentation rule from an element and release that rule.
  Parameters:
  element: the element with which the presentation rule is associated.
  pRule: the presentation rule to be removed.
  document: the document to which the element belongs.
  ----------------------------------------------------------------------*/
void    TtaRemovePRule (Element element, PRule pRule, Document document)
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
          FreePresentRule (pPres, ((PtrElement) element)->ElStructSchema);
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
void TtaNextPRule (Element element, PRule * pRule)
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
