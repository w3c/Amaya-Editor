/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "presentation.h"
#include "tree.h"
#include "typecorr.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#ifndef NODISPLAY
#include "frame_tv.h"
#endif /* NODISPLAY */
#include "boxes_tv.h"


#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "buildboxes_f.h"
#include "boxpositions_f.h"
#include "boxselection_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "compilmsg_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "schemas_f.h"
#include "structmodif_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "viewapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

/*----------------------------------------------------------------------
  PRuleMessagePre	On veut ajouter ou modifier (selon isNew) la	
  regle de presentation specifique pPRule a l'element pEl du	
  document pDoc. On envoie le message APP correspondant a		
  l'application et on retourne la reponse de l'application.	
  ----------------------------------------------------------------------*/
ThotBool PRuleMessagePre (PtrElement pEl, PtrPRule pPRule, int newvalue,
                          PtrDocument pDoc, ThotBool isNew)
{
  NotifyPresentation  notifyPres;
  PtrPRule            pR, pRPrec;
  ThotBool            noApply;

  if (isNew)
    notifyPres.event = TtePRuleCreate;
  else
    notifyPres.event = TtePRuleModify;
  notifyPres.pRule = (PRule) pPRule;
  notifyPres.document = (Document) IdentDocument (pDoc);
  notifyPres.element = (Element) pEl;
  notifyPres.pRuleType = NumTypePRuleAPI (pPRule);
  notifyPres.value = newvalue;
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
              FreePresentRule (pPRule, pEl->ElStructSchema);
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
void PRuleMessagePost (PtrElement pEl, PtrPRule pPRule, PtrDocument pDoc, ThotBool isNew)
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

/*----------------------------------------------------------------------
  NewPosition is called when the user moves a box.
  pAb is the abstract box	
  X and Y give the new positions in pixels, xref, yref the position
  of the reference point in the box.
  frame is the frame.				
  display is TRUE when it's necessary to redisplay the concrete image.
  ----------------------------------------------------------------------*/
void NewPosition (PtrAbstractBox pAb, int X, int xref, int Y, int yref,
                  int frame, ThotBool display)
{
#ifndef NODISPLAY
  PtrPRule            pR, pRStd;
  PtrPRule            pRuleH, pRuleV;
  PtrPSchema          pSPR;
  PtrSSchema          pSSR;
  PtrAttribute        pAttrH, pAttrV;
  PtrDocument         pDoc;
  PtrElement          pEl;
  PtrAbstractBox      pParent;
  NotifyAttribute     notifyAttr;
  Document            doc;
  DisplayMode         dispMode;
  TypeUnit            unit;
  int                 x, y, dx, dy;
  int                 viewSch;
  int                 value;
  ThotBool            attr, doitV, doitH, isPos;
  ThotBool            isNew, reDisp, isLined, histOpen;

  /* get the element */
  pEl = pAb->AbElement;
  /* get the document */
  pDoc = DocumentOfElement (pEl);
  /* view number in the presentation schema that defines it */
  viewSch = AppliedView (pEl, NULL, pDoc, pAb->AbDocView);
  /* by default nothing to redisplay */
  reDisp = FALSE;
  histOpen = FALSE;
  /* by default the abstract box is not presented in a block of lines */
  isLined = FALSE;
  pParent = pAb->AbEnclosing;
  /* check enclosing abstract boxes */
  while (!isLined && pParent != NULL)
    {
      if (pParent->AbLeafType == LtCompound && pParent->AbInLine)
        /* the abstract box is presented in a block of lines */
        isLined = TRUE;
      else if (!pParent->AbAcceptLineBreak)
        /* the abstract box doesn't accept line breaks, stop the research */
        pParent = NULL;
      else
        /* go upper */
        pParent = pParent->AbEnclosing;
    }
  
  /* avoid too many redisplays */
  doc = FrameTable[frame].FrDoc;
  pParent = pAb->AbEnclosing;
  doitV = FALSE;
  doitH = FALSE;
  dispMode = documentDisplayMode[doc - 1];
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  
  /* manage the vertical position */
  if (pAb->AbBox != NULL && Y != pAb->AbBox->BxYOrg)
    {
      /* look for the position rule that applies to the element */
      pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR, FALSE, 0, NULL,
                                   viewSch, PtVertPos, FnAny, FALSE, TRUE, &pAttrV);
      /* doesn't move boxes with floating position or set in lines */
      if (pRStd->PrPosRule.PoPosDef != NoEdge &&
          pAb->AbVertPos.PosAbRef != NULL && !isLined)
        {
          /* check if the position is given by an attribute with the exception NewVPos */
          attr = (pAttrV != NULL &&
                  AttrHasException (ExcNewVPos, pAttrV->AeAttrNum, pAttrV->AeAttrSSchema));
          isPos = !(pAb->AbHeight.DimIsPosition);
          if (isPos)
            /* get the difference */
            dy = Y - pAb->AbBox->BxYOrg;
          else
            /* position rule of stretched box is related to the parent */
            dy = Y;
	  
          unit = pRStd->PrPosRule.PoDistUnit;
          if (unit == UnPercent)
            {
              if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                GetSizesFrame (frame, &x, &y);
              else
                y = pAb->AbEnclosing->AbBox->BxHeight;
              dy = LogicalValue (dy, UnPercent, (PtrAbstractBox) y, 0);
            }
          else
            dy = LogicalValue (dy, unit, pAb,
                               ViewFrameTable[frame - 1].FrMagnification);
	  
          if (!BoxCreating && !pDoc->DocEditSequence)
            {
              OpenHistorySequence (pDoc, pEl, pEl, NULL,0, 0);
              histOpen = TRUE;
            }

          if (attr)
            {
              pRuleV = pRStd;
              if (pAttrV->AeAttrType == AtNumAttr)
                {
                  /* change the attribute value */
                  notifyAttr.event = TteAttrModify;
                  notifyAttr.document = doc;
                  notifyAttr.element = (Element) pEl;
                  notifyAttr.info = 0; /* not sent by undo */
                  notifyAttr.attribute = (Attribute) pAttrV;
                  notifyAttr.attributeType.AttrSSchema =
                    (SSchema) (pAttrV->AeAttrSSchema);
                  notifyAttr.attributeType.AttrTypeNum = pAttrV->AeAttrNum;
                  doitV = !CallEventAttribute (&notifyAttr, TRUE);
                  if (doitV)
                    {
                      /* modify the distance in the attribute rule */
                      if (!BoxCreating)
                        AddAttrEditOpInHistory (pAttrV, pEl, pDoc, TRUE, TRUE);
                      pAttrV->AeAttrValue += dy;
                    }
                }
            }
          else
            /* the new position will be stored in a specific presentation rule */
            {
              /* check if the specific presentation rule already exists */
              pRuleV = SearchPresRule (pEl, PtVertPos, (FunctionType)0, &isNew, pDoc, viewSch);
	      
              if (isNew)
                {
                  /* create a new rule for the element */
                  pR = pRuleV->PrNextPRule;
                  unit = UnPixel;
                  /* copy the standard rule */
                  *pRuleV = *pRStd;
                  pRuleV->PrCond = NULL;
                  pRuleV->PrNextPRule = pR;
                  pRuleV->PrViewNum = viewSch;
                  pRuleV->PrPosRule.PoDistUnit = unit;
                  /* if the rule is associated to an attribute */
                  /* keep the link to that attribute */
                  if (pAttrV != NULL)
                    {
                      pRuleV->PrSpecifAttr = pAttrV->AeAttrNum;
                      pRuleV->PrSpecifAttrSSchema = pAttrV->AeAttrSSchema;
                    }
                  if (isPos)
                    {
                      /* get the initial position */
                      dy = Y + yref;
                      if (pParent)
                        dy -= pParent->AbBox->BxYOrg;
                    }
                  else
                    /* position rule of stretched box is related to the parent */
                    dy = Y;
                  dy = LogicalValue (dy, unit, pAb,
                                     ViewFrameTable[frame - 1].FrMagnification);
                  value = 0;
                }
              else
                value = pRuleV->PrPosRule.PoDistance;
              if (isPos)
                dy += value;
	      
              /* send the event message to the application */
              doitV = !PRuleMessagePre (pEl, pRuleV, dy, pDoc, isNew);
              if (doitV || isNew)
                {
                  /* store the new value in the specific rule */
                  pRuleV->PrPosRule.PoDistance = dy;
                  pRuleV->PrPosRule.PoDistUnit = unit;
                }
            }
	   
          if (doitV)
            {
              /* the document is modified */
              SetDocumentModified (pDoc, TRUE, 0);
              if (ApplyRule (pRuleV, pSPR, pAb, pDoc, pAttrV, pAb))
                {
                  pAb->AbVertPosChange = TRUE;
                  if (!isPos)
                    {
                      /* for stretched boxes: both position and dimension rules
                         have to be updated */
                      pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR,
                                                   FALSE, 0, NULL, viewSch, PtHeight,
                                                   FnAny, FALSE, TRUE, &pAttrV);
                      ApplyRule (pRStd, pSPR, pAb, pDoc, pAttrV, pAb);
                      pAb->AbHeightChange = TRUE;
                    }	       
                  reDisp = TRUE;
                }
              if (attr)
                {
                  /* redisplay presentation variables that use the attribute */
                  /*RedisplayAttribute (pAttrV, pEl, pDoc);*/
                  if (display)
                    /* redisplay copies */
                    RedisplayCopies (pEl, pDoc, TRUE);
                  CallEventAttribute (&notifyAttr, FALSE);
                }
              else
                PRuleMessagePost (pEl, pRuleV, pDoc, isNew);
            }
        }
    }
  
  /* manage horizontal position */
  if (pAb->AbBox != NULL && X != pAb->AbBox->BxXOrg)
    {
      /* look for the position rule that applies to the element */
      pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR, FALSE, 0, NULL,
                                   viewSch, PtHorizPos, FnAny, FALSE, TRUE, &pAttrH);
      /* doesn't move boxes with floating position or set in lines */
      if (pRStd->PrPosRule.PoPosDef != NoEdge
          && pAb->AbHorizPos.PosAbRef != NULL
          && !isLined)
        {
          /* check if the position is given by an attribute with the exception NewVPos */
          attr = (pAttrH != NULL &&
                  AttrHasException (ExcNewHPos, pAttrH->AeAttrNum, pAttrH->AeAttrSSchema));
          isPos = !(pAb->AbWidth.DimIsPosition);
          if (isPos)
            /* get the difference */
            dx = X - pAb->AbBox->BxXOrg;
          else
            /* position rule of stretched box is related to the parent */
            dx = X;
	     
          unit = pRStd->PrPosRule.PoDistUnit;
          if (unit == UnPercent)
            {
              if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                GetSizesFrame (frame, &x, &y);
              else
                x = pAb->AbEnclosing->AbBox->BxWidth;
              dx = LogicalValue (dx, UnPercent, (PtrAbstractBox) x, 0);
            }
          else
            dx = LogicalValue (dx, unit, pAb,
                               ViewFrameTable[frame - 1].FrMagnification);
	  
          if (!BoxCreating && !pDoc->DocEditSequence)
            {
              OpenHistorySequence (pDoc, pEl, pEl, NULL, 0, 0);
              histOpen = TRUE;
            }

          if (attr)
            {
              pRuleH = pRStd;
              if (pAttrH->AeAttrType == AtNumAttr)
                {
                  /* change the attribute value */
                  notifyAttr.event = TteAttrModify;
                  notifyAttr.document = doc;
                  notifyAttr.element = (Element) pEl;
                  notifyAttr.info = 0; /* not sent by undo */
                  notifyAttr.attribute = (Attribute) pAttrH;
                  notifyAttr.attributeType.AttrSSchema =
                    (SSchema) (pAttrH->AeAttrSSchema);
                  notifyAttr.attributeType.AttrTypeNum = pAttrH->AeAttrNum;
                  doitH = !CallEventAttribute (&notifyAttr, TRUE);
                  if (doitH)
                    {
                      /* modify the distance in the attribute rule */
                      if (!BoxCreating)
                        AddAttrEditOpInHistory (pAttrH, pEl, pDoc, TRUE, TRUE);
                      pAttrH->AeAttrValue += dx;
                    }
                }
            }
          else
            /* the new position will be stored in a specific presentation rule */
            {
              /* check if the specific presentation rule already exists */
              pRuleH = SearchPresRule (pEl, PtHorizPos, (FunctionType)0, &isNew, pDoc, viewSch);
              if (isNew)
                {
                  /* create a new rule for the element */
                  pR = pRuleH->PrNextPRule;
                  unit = UnPixel;
                  /* copy the standard rule */
                  *pRuleH = *pRStd;
                  pRuleH->PrCond = NULL;
                  pRuleH->PrNextPRule = pR;
                  pRuleH->PrViewNum = viewSch;
                  pRuleH->PrPosRule.PoDistUnit = unit;
                  /* if the rule is associated to an attribute */
                  /* keep the link to that attribute */
                  if (pAttrH != NULL)
                    {
                      pRuleH->PrSpecifAttr = pAttrH->AeAttrNum;
                      pRuleH->PrSpecifAttrSSchema = pAttrH->AeAttrSSchema;
                    }
                  if (isPos)
                    {
                      /* get the initial position */
                      dx = X + xref;
                      if (pParent)
                        dx -= pParent->AbBox->BxXOrg;
                    }
                  else
                    /* position rule of stretched box is related to the parent */
                    dx = X;
                  dx = LogicalValue (dx, unit, pAb,
                                     ViewFrameTable[frame - 1].FrMagnification);
                  value = 0;
                }
              else
                value = pRuleH->PrPosRule.PoDistance;
              if (isPos)
                dx += value;

              /* send the event message to the application */
              doitH = !PRuleMessagePre (pEl, pRuleH, dx, pDoc, isNew);
              if (doitH || isNew)
                {
                  /* store the new value in the specific rule */
                  pRuleH->PrPosRule.PoDistance = dx;
                  pRuleH->PrPosRule.PoDistUnit = unit;
                }
            }
          if (doitH)
            {
              /* the document is modified */
              SetDocumentModified (pDoc, TRUE, 0);
              if (ApplyRule (pRuleH, pSPR, pAb, pDoc, pAttrH, pAb))
                {
                  pAb->AbHorizPosChange = TRUE;
                  if (!isPos)
                    {
                      /* for stretched boxes: both position and dimension rules
                         have to be updated */
                      pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR,
                                                   FALSE, 0, NULL, viewSch, PtWidth, FnAny,
                                                   FALSE, TRUE, &pAttrH);
                      ApplyRule (pRStd, pSPR, pAb, pDoc, pAttrH, pAb);
                      pAb->AbWidthChange = TRUE;
                    }	       
                  reDisp = TRUE;
                }
              if (attr)
                {
                  /* redisplay presentation variables that use the attribute */
                  /*RedisplayAttribute (pAttrH, pEl, pDoc);*/
                  if (display)
                    /* redisplay copies */
                    RedisplayCopies (pEl, pDoc, TRUE);
                  CallEventAttribute (&notifyAttr, FALSE);
                }
              else
                PRuleMessagePost (pEl, pRuleH, pDoc, isNew);
            }
        }
    }

  if (!BoxCreating && histOpen)
    CloseHistorySequence (pDoc);
  
  if (reDisp && display)
    {
      /* update current displayed views (I'm not sure it's necessary) */
      RedispAbsBox (pAb, pDoc);
      AbstractImageUpdated (pDoc);
      RedisplayDocViews (pDoc);
    }
  /* restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
#endif /* NODISPLAY */
}

/*----------------------------------------------------------------------
  NewDimension is called when the user resizes a box.
  pAb is the abstract box	
  width and height give the new dimensions in pixels.
  frame is the frame.
  fromUI is TRUE when it's necessary to redisplay the concrete image.
  In other cases the request comes from the API.
  ----------------------------------------------------------------------*/
void NewDimension (PtrAbstractBox pAb, int width, int height, int frame,
                   ThotBool fromUI)
{
#ifndef NODISPLAY
  PtrPRule            pR, pRStd;
  PtrPRule            pRuleH, pRuleV;
  PtrPSchema          pSPRH, pSPRV;
  PtrSSchema          pSSR;
  PtrAttribute        pAttrH, pAttrV;
  PtrDocument         pDoc;
  PtrElement          pEl;
  NotifyAttribute     notifyAttrH, notifyAttrV;
  Document            doc;
  DisplayMode         dispMode;
  TypeUnit            unit;
  int                 dx, dy;
  int                 heightRef, widthRef;
  int                 viewSch;
  int                 value;
  ThotBool            attrV, attrH, doitV, doitH, ok;
  ThotBool            isNewH, isNewV, reDisp;
  ThotBool            isDimH, isDimV, histOpen;

  /* get the element */
  pEl = pAb->AbElement;
  /* get the document */
  pDoc = DocumentOfElement (pEl);
  /* view number in the presentation schema that defines it */
  viewSch = AppliedView (pEl, NULL, pDoc, pAb->AbDocView);
  doc = FrameTable[frame].FrDoc;
  /* nothing to redisplay */
  reDisp = FALSE;
  dispMode = documentDisplayMode[doc - 1];
  histOpen = FALSE;
  doitH = FALSE;
  doitV = FALSE;
  pRuleH = pRuleV = NULL;
  attrV = attrH = isDimH = isDimV = FALSE;
  if (dispMode == DisplayImmediately)
    {
      TtaSetDisplayMode (doc, DeferredDisplay);
      reDisp = TRUE;
    }

  /* manage the width change of the box */
  if (width != 0 && pAb->AbBox != NULL && width != pAb->AbBox->BxW)
    {
      /* look for the dimension rule applied to the element */
      pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPRH, &pSSR, FALSE, 0, NULL,
                                   viewSch, PtWidth, FnAny, FALSE, TRUE, &pAttrH);
      /* don't change the width when it depends on the contents or it's */
      /* a stretched box */
      ok = TRUE;
      if (!pRStd->PrDimRule.DrPosition)
        {
          if (pRStd->PrDimRule.DrRelation == RlEnclosed)
            {
              if (pAb->AbLeafType == LtPicture ||
                  (pAb->AbLeafType == LtCompound && pAb->AbFirstEnclosed &&
                   pAb->AbFirstEnclosed->AbLeafType == LtPicture &&
                   pAb->AbFirstEnclosed->AbNext == NULL))
                /*  the width depends on the contents but it's a picture */
                isDimH = TRUE;
              else
                ok = FALSE;
            }
          else
            isDimH = TRUE;
        }
      else
        isDimH = FALSE;

      if (ok)
        {
          /* check if the width is given by an attribute with the exception NewWidth */
          attrH = (pAttrH != NULL &&
                   AttrHasException (ExcNewWidth, pAttrH->AeAttrNum, pAttrH->AeAttrSSchema));
          if (isDimH)
            {
              /* a dimension rule */
              dx = width - pAb->AbBox->BxW;
              unit = pRStd->PrDimRule.DrUnit;
              if (pAb->AbWidth.DimAbRef)
                /* the width is a percent of an other box width */
                widthRef = pAb->AbWidth.DimAbRef->AbBox->BxW;
              else if (pAb->AbEnclosing == NULL)
                /* the width is a percent of the window width */
                GetSizesFrame (frame, &widthRef, &heightRef);
              else
                /* the width is a percent of the parent box width */
                widthRef = pAb->AbEnclosing->AbBox->BxW;
            }
          else
            {
              /* position rule of stretched box is related to the parent */
              dx = width;
              unit = pRStd->PrDimRule.DrPosRule.PoDistUnit;
              if (pAb->AbEnclosing == NULL)
                /* the width is a percent of the window width */
                GetSizesFrame (frame, &widthRef, &heightRef);
              else
                /* the width is a percent of the parent box width */
                widthRef = pAb->AbEnclosing->AbBox->BxW;
            }
          if (unit == UnPercent)
            /* get the new percent value */
            dx = LogicalValue (dx, UnPercent, (PtrAbstractBox) widthRef, 0);
          else
            /* convert the new height in logical value */
            dx = LogicalValue (dx, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	  
          if (!BoxCreating && !pDoc->DocEditSequence)
            {
              OpenHistorySequence (pDoc, pEl, pEl, NULL, 0, 0);
              histOpen = TRUE;
            }
          if (attrH)
            {
              pRuleH = pRStd;
              if (pAttrH->AeAttrType == AtNumAttr)
                /* change the attribute value */
                {
                  notifyAttrH.event = TteAttrModify;
                  notifyAttrH.document = doc;
                  notifyAttrH.element = (Element) pEl;
                  notifyAttrH.info = 0; /* not sent by undo */
                  notifyAttrH.attribute = (Attribute) pAttrH;
                  notifyAttrH.attributeType.AttrSSchema = (SSchema) (pAttrH->AeAttrSSchema);
                  notifyAttrH.attributeType.AttrTypeNum = pAttrH->AeAttrNum;
                  doitH = !CallEventAttribute (&notifyAttrH, TRUE);
                  if (doitH)
                    {
                      /* modify the distance in the attribute rule */
                      if (!BoxCreating)
                        AddAttrEditOpInHistory (pAttrH, pEl, pDoc, TRUE, TRUE);
                      pAttrH->AeAttrValue += dx;
                    }
                }
            }
          else
            /* the new width will be stored in a specific presentation rule */
            {
              /* check if the specific presentation rule already exists */
              pRuleH = SearchPresRule (pEl, PtWidth, (FunctionType)0, &isNewH, pDoc, pAb->AbDocView);
              if (isNewH)
                {
                  /* create a new rule for the element */
                  pR = pRuleH->PrNextPRule;
                  unit = UnPixel;
                  /* copy the standard rule */
                  *pRuleH = *pRStd;
                  pRuleH->PrCond = NULL;
                  pRuleH->PrNextPRule = pR;
                  pRuleH->PrViewNum = viewSch;
                  /* if the rule is associated to an attribute */
                  /* keep the link to that attribute */
                  if (pAttrH != NULL)
                    {
                      pRuleH->PrSpecifAttr = pAttrH->AeAttrNum;
                      pRuleH->PrSpecifAttrSSchema = pAttrH->AeAttrSSchema;
                    }
                  /* by default generate a rule in pixels */
                  if (isDimH)
                    pRuleH->PrDimRule.DrUnit = unit;
                  else
                    pRuleH->PrDimRule.DrPosRule.PoDistUnit = unit;
                  value = 0;
                }
              else if (isDimH)
                {
                  /* a dimension rule */
                  unit = pRuleH->PrDimRule.DrUnit;
                  value = pRuleH->PrDimRule.DrValue;
                }
              else
                {
                  /* a position rule */
                  unit = pRuleH->PrDimRule.DrPosRule.PoDistUnit;
                  value = pRuleH->PrDimRule.DrPosRule.PoDistance;
                }
              if (isNewH/* || (pAb->AbBox->BxWidth != 0 && isDim && value == 0)*/)
                {
                  /* the rule gives a default */
                  if (unit == UnPercent)
                    dx = LogicalValue (width, UnPercent, (PtrAbstractBox) widthRef, 0);
                  else
                    dx = LogicalValue (width, unit, pAb,
                                       ViewFrameTable[frame - 1].FrMagnification);
                  value = 0;
                }
              else if (isDimH)
                dx += value;

              if (fromUI)
                /* send the event message to the application */
                doitH = !PRuleMessagePre (pEl, pRuleH, dx, pDoc, isNewH);
              else
                doitH = TRUE;
              if (doitH /*|| isNewH*/)
                {
                  /* store the new value in the specific rule */
                  if (isDimH)
                    {
                      pRuleH->PrDimRule.DrValue = dx;
                      pRuleH->PrDimRule.DrUnit = unit;
                    }
                  else
                    pRuleH->PrDimRule.DrPosRule.PoDistance = dx;		  
                }
            }
        }
    }

  /* manage the height change of the box */
  if (height != 0 && pAb->AbBox != NULL && height != pAb->AbBox->BxH)
    {
      /* look for the dimension rule applied to the element */
      pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPRV, &pSSR, FALSE, 0, NULL,
                                   viewSch, PtHeight, FnAny, FALSE, TRUE, &pAttrV);
      /* don't change the height when it depends on the contents or it's */
      /* a stretched box */
      ok = TRUE;
      if (!pRStd->PrDimRule.DrPosition)
        {
          /*  the height depends on the contents and it's not a picture */
          if (pRStd->PrDimRule.DrRelation == RlEnclosed)
            {
              if (pAb->AbLeafType == LtPicture ||
                  (pAb->AbLeafType == LtCompound &&
                   pAb->AbFirstEnclosed &&
                   pAb->AbFirstEnclosed->AbLeafType == LtPicture &&
                   pAb->AbFirstEnclosed->AbNext == NULL))
                /*  the height depends on the contents but it's a picture */
                isDimV = TRUE;
              else
                ok = FALSE;
            }
          else
            isDimV = TRUE;
        }
      else
        isDimV = FALSE;

      if (ok)
        {
          /* check if the height is given by an attribute with the exception NewHeight */
          attrV = (pAttrV != NULL &&
                   AttrHasException (ExcNewHeight, pAttrV->AeAttrNum, pAttrV->AeAttrSSchema));
          if (isDimV)
            {
              /* a dimension rule */
              dy = height - pAb->AbBox->BxH;
              unit = pRStd->PrDimRule.DrUnit;
              if (pAb->AbHeight.DimAbRef)
                /* the height is a percent of an other box height */
                heightRef = pAb->AbHeight.DimAbRef->AbBox->BxH;
              else if (pAb->AbEnclosing == NULL)
                /* the height is a percent of the window height */
                GetSizesFrame (frame, &widthRef, &heightRef);
              else
                /* the height is a percent of the parent box height */
                heightRef = pAb->AbEnclosing->AbBox->BxH;
            }
          else
            {
              /* position rule of stretched box is related to the parent */
              dy = height;
              unit = pRStd->PrDimRule.DrPosRule.PoDistUnit;
              if (pAb->AbEnclosing == NULL)
                /* the height is a percent of the window width */
                GetSizesFrame (frame, &widthRef, &heightRef);
              else
                /* the height is a percent of the parent box height */
                heightRef = pAb->AbEnclosing->AbBox->BxH;
            }
          if (unit == UnPercent)
            /* get the new percent value */
            dy = LogicalValue (dy, UnPercent, (PtrAbstractBox)heightRef, 0);
          else
            /* convert the new height in logical value */
            dy = LogicalValue (dy, unit, pAb,
                               ViewFrameTable[frame - 1].FrMagnification);
	   
          if (!BoxCreating && !pDoc->DocEditSequence)
            {
              OpenHistorySequence (pDoc, pEl, pEl, NULL, 0, 0);
              histOpen = TRUE;
            }

          if (attrV)
            {
              pRuleV = pRStd;
              if (pAttrV->AeAttrType == AtNumAttr)
                /* change the attribute value */
                {
                  notifyAttrV.event = TteAttrModify;
                  notifyAttrV.document = doc;
                  notifyAttrV.element = (Element) pEl;
                  notifyAttrV.info = 0; /* not sent by undo */
                  notifyAttrV.attribute = (Attribute) pAttrV;
                  notifyAttrV.attributeType.AttrSSchema =
                    (SSchema) (pAttrV->AeAttrSSchema);
                  notifyAttrV.attributeType.AttrTypeNum = pAttrV->AeAttrNum;
                  doitV = !CallEventAttribute (&notifyAttrV, TRUE);
                  if (doitV)
                    {
                      /* modify the distance in the attribute rule */
                      if (!BoxCreating)
                        AddAttrEditOpInHistory (pAttrV, pEl, pDoc, TRUE, TRUE);
                      pAttrV->AeAttrValue += dy;
                    }
                }
            }
          else
            /* the new height will be stored in a specific presentation rule */
            {
              /* check if the specific presentation rule already exists */
              pRuleV = SearchPresRule (pEl, PtHeight, (FunctionType)0, &isNewV, pDoc, pAb->AbDocView);
              if (isNewV)
                {
                  /* create a new rule for the element */
                  pR = pRuleV->PrNextPRule;
                  unit = UnPixel;
                  /* copy the standard rule */
                  *pRuleV = *pRStd;
                  pRuleV->PrCond = NULL;
                  pRuleV->PrNextPRule = pR;
                  pRuleV->PrViewNum = viewSch;
                  /* if the rule is associated to an attribute */
                  /* keep the link to that attribute */
                  if (pAttrV != NULL)
                    {
                      pRuleV->PrSpecifAttr = pAttrV->AeAttrNum;
                      pRuleV->PrSpecifAttrSSchema = pAttrV->AeAttrSSchema;
                    }
                  /* by default generate a rule in pixels */
                  if (isDimV)
                    pRuleV->PrDimRule.DrUnit = unit;
                  else
                    pRuleV->PrDimRule.DrPosRule.PoDistUnit = unit;
                  value = 0;
                }
              else if (isDimV)
                {
                  /* a dimension rule */
                  unit = pRuleV->PrDimRule.DrUnit;
                  value = pRuleV->PrDimRule.DrValue;
                }
              else
                {
                  /* a position rule */
                  unit = pRuleV->PrDimRule.DrPosRule.PoDistUnit;
                  value = pRuleV->PrDimRule.DrPosRule.PoDistance;
                }
              if (isNewV/* ||(pAb->AbBox->BxHeight != 0 && isDimV && value == 0)*/)
                {
                  /* the rule gives a default height */
                  if (unit == UnPercent)
                    dy = LogicalValue (height, UnPercent, (PtrAbstractBox) heightRef, 0);
                  else
                    dy = LogicalValue (height, unit, pAb,
                                       ViewFrameTable[frame - 1].FrMagnification);
                  value = 0;
                }
              else if (isDimV)
                dy += value;
	       
              if (fromUI)
                /* send the event message to the application */
                doitV = !PRuleMessagePre (pEl, pRuleV, dy, pDoc, isNewV);
              else
                doitV = TRUE;
              if (doitV /*|| isNewV*/)
                {
                  /* store the new value in the specific rule */
                  if (isDimV)
                    {
                      pRuleV->PrDimRule.DrValue = dy;
                      pRuleV->PrDimRule.DrUnit = unit;
                    }
                  else
                    pRuleV->PrDimRule.DrPosRule.PoDistance = dy;
                }
            }
        }
    }

  if (!BoxCreating && histOpen)
    CloseHistorySequence (pDoc);

  if (doitH || doitV)
    {
      /* the document is modified */
      SetDocumentModified (pDoc, TRUE, 0);
      if (doitH && ApplyRule (pRuleH, pSPRH, pAb, pDoc, pAttrH, pAb))
        {
          pAb->AbWidthChange = TRUE;
          if (!isDimH)
            {
              /* for stretched boxes: both position and dimension rules
                 have to be updated */
              pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPRH, &pSSR, FALSE, 0,
                                           NULL, viewSch, PtHorizPos, FnAny, FALSE,
                                           TRUE, &pAttrH);
              ApplyRule (pRStd, pSPRH, pAb, pDoc, pAttrH, pAb);
              pAb->AbHorizPosChange = TRUE;
            }	       
          reDisp = TRUE;
        }	   
      if (doitV && ApplyRule (pRuleV, pSPRV, pAb, pDoc, pAttrV, pAb))
        {
          pAb->AbHeightChange = TRUE;
          if (!isDimV)
            {
              /* for stretched boxes: both position and dimension rules
                 have to be updated */
              pRStd = GlobalSearchRulepEl (pEl, pDoc, &pSPRV, &pSSR, FALSE, 0,
                                           NULL, viewSch, PtVertPos, FnAny, FALSE,
                                           TRUE, &pAttrV);
              ApplyRule (pRStd, pSPRV, pAb, pDoc, pAttrV, pAb);
              pAb->AbVertPosChange = TRUE;
            }
          reDisp = TRUE;
        }

      if (reDisp)
        {
          /* update current displayed views (I'm not sure it's necessary) */
          RedispAbsBox (pAb, pDoc);
          AbstractImageUpdated (pDoc);
          RedisplayDocViews (pDoc);
        }

      if (doitH)
        {
          if (attrH)
            {
              /* redisplay presentation variables using the attribute */
              /*RedisplayAttribute (pAttrH, pEl, pDoc);*/
              if (fromUI)
                /* the new attribute value must be transmitted */
                /* to copies and inclusions of the element */
                RedisplayCopies (pEl, pDoc, TRUE);
              CallEventAttribute (&notifyAttrH, FALSE);
            }
          else if (fromUI)
            PRuleMessagePost (pEl, pRuleH, pDoc, isNewH);
        }
      if (doitV)
        {
          if (attrV)
            {
              /* redisplay presentation variables using the attribute */
              RedisplayAttribute (pAttrV, pEl, pDoc);
              if (fromUI)
                /* the new attribute value must be transmitted */
                /* to copies and inclusions of the element */
                /*RedisplayCopies (pEl, pDoc, TRUE);*/
                CallEventAttribute (&notifyAttrV, FALSE);
            }
          else if (fromUI)
            PRuleMessagePost (pEl, pRuleV, pDoc, isNewV);
        }
    }
  
  /* restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
#endif /* NODISPLAY */
}

/*----------------------------------------------------------------------
  TtaNewPRule
  Creates a new presentation rule of a given type for a given view of a given
  document.
  Parameter:
  presentationType: type of the presentation rule to be created. Available
  values are defined in file presentation.h
  view: the view (this view must be open).
  document: the document.
  Return value:
  new presentation rule.
  ----------------------------------------------------------------------*/
PRule TtaNewPRule (int presentationType, View view, Document document)
{
  PtrPRule            pPres;
  int                 v;

  UserErrorCode = 0;
  pPres = NULL;
  if (presentationType < 0 || presentationType > LAST_PRES_RULE_TYPE)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (view < 1 || view > MAX_VIEW_DOC)
    TtaError (ERR_invalid_parameter);
  else if (LoadedDocument[document - 1]->DocView[view - 1].DvPSchemaView == 0)
    /* this view is not open */
    TtaError (ERR_invalid_parameter);
  else
    {
      v = LoadedDocument[document - 1]->DocView[view - 1].DvPSchemaView;
      if (v > 0)
        {
          GetPresentRule (&pPres);
          pPres->PrNextPRule = NULL;
          pPres->PrViewNum = v;
          switch (presentationType)
            {
            case PRShowBox:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnShowBox;
              break;
            case PRNotInLine:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnNotInLine;
              break;
            case PRBackgroundPicture:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnBackgroundPicture;
              break;
            case PRBackgroundRepeat:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnBackgroundRepeat;
              break;
            case PRCreateEnclosing:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnCreateEnclosing;
              break;
            default:
              pPres->PrType = (PRuleType) presentationType;
              break;
            }
        }
    }
  return ((PRule) pPres);
}

/*----------------------------------------------------------------------
  TtaNewPRuleForView

  Creates a new presentation rule of a given type for a given view of a given
  document. The view is identified by its number.
  Parameter:
  presentationType: type of the presentation rule to be created. Available
  values are defined in file presentation.h
  viewName: the name of the view (this view does not need to be open).
  document: the document.
  Return value:
  new presentation rule.
  ----------------------------------------------------------------------*/
PRule TtaNewPRuleForView (int presentationType, int view, Document document)
{
  PtrPRule            pPres;

  UserErrorCode = 0;
  pPres = NULL;
  if (presentationType < 0 || presentationType > LAST_PRES_RULE_TYPE)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if ((view < 1) || (view > MAX_VIEW))
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameters document and view are correct */
    {
      GetPresentRule (&pPres);
      pPres->PrNextPRule = NULL;
      pPres->PrViewNum = view;
      switch (presentationType)
        {
        case PRShowBox:
          pPres->PrType = PtFunction;
          pPres->PrPresFunction = FnShowBox;
          break;
        case PRNotInLine:
          pPres->PrType = PtFunction;
          pPres->PrPresFunction = FnNotInLine;
          break;
        case PRBackgroundPicture:
          pPres->PrType = PtFunction;
          pPres->PrPresFunction = FnBackgroundPicture;
          break;
        case PRBackgroundRepeat:
          pPres->PrType = PtFunction;
          pPres->PrPresFunction = FnBackgroundRepeat;
          break;
        case PRCreateEnclosing:
          pPres->PrType = PtFunction;
          pPres->PrPresFunction = FnCreateEnclosing;
          break;
        default:
          pPres->PrType = (PRuleType) presentationType;
          break;
        }
    }
  return ((PRule) pPres);
}

/*----------------------------------------------------------------------
  TtaNewPRuleForNamedView

  Creates a new presentation rule of a given type for a given view of a given
  document. The view is identified by its name.
  Parameter:
  presentationType: type of the presentation rule to be created. Available
  values are defined in file presentation.h
  viewName: the name of the view (this view does not need to be open).
  document: the document.
  Return value:
  new presentation rule.
  ----------------------------------------------------------------------*/
PRule TtaNewPRuleForNamedView (int presentationType, char *viewName,
                               Document document)
{
  PtrPRule            pPres;
  PtrDocument         pDoc;
  PtrPSchema          pPS;
  int                 view;
  int                 v;

  UserErrorCode = 0;
  pPres = NULL;
  if (presentationType < 0 || presentationType > LAST_PRES_RULE_TYPE)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      pDoc = LoadedDocument[document - 1];
      pPS = PresentationSchema (pDoc->DocSSchema, pDoc);
      view = 0;
      /* Searching into the main tree views */
      if (pPS != NULL)
        for (v = 1; v <= MAX_VIEW && view == 0; v++)
          if (strcmp (pPS->PsView[v - 1], viewName) == 0)
            view = v;
      if (view == 0)
        TtaError (ERR_invalid_parameter);
      else
        {
          GetPresentRule (&pPres);
          pPres->PrNextPRule = NULL;
          pPres->PrViewNum = view;
          switch (presentationType)
            {
            case PRShowBox:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnShowBox;
              break;
            case PRNotInLine:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnNotInLine;
              break;
            case PRBackgroundPicture:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnBackgroundPicture;
              break;
            case PRBackgroundRepeat:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnBackgroundRepeat;
              break;
            case PRCreateEnclosing:
              pPres->PrType = PtFunction;
              pPres->PrPresFunction = FnCreateEnclosing;
              break;
            default:
              pPres->PrType = (PRuleType) presentationType;
              break;
            }
        }
    }
  return ((PRule) pPres);
}

/*----------------------------------------------------------------------
  TtaCopyPRule

  Creates a new presentation rule and initializes it with a copy of an
  existing presentation rule.
  Parameter:
  pRule: presentation rule to be copied.
  Return value:
  new presentation rule.
  ----------------------------------------------------------------------*/
PRule TtaCopyPRule (PRule pRule)
{
  PtrPRule            copy;

  UserErrorCode = 0;
  copy = NULL;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      GetPresentRule (&copy);
      *copy = *((PtrPRule) pRule);
      copy->PrNextPRule = NULL;
    }
  return ((PRule) copy);
}

/*----------------------------------------------------------------------
  TtaAttachPRule

  Attaches a presentation rule to an element.

  Parameters:
  element: the element to which the presentation rule
  has to be attached.
  pRule: the presentation rule to be attached.
  document: the document to which the element belongs.

  ----------------------------------------------------------------------*/
void TtaAttachPRule (Element element, PRule pRule, Document document)
{
  PtrPRule            pPres;
  ThotBool            stop;
#ifndef NODISPLAY
  ThotBool            ok;

#endif
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
#ifndef NODISPLAY
      ok = TRUE;
#endif
      if (((PtrElement) element)->ElFirstPRule == NULL)
        ((PtrElement) element)->ElFirstPRule = (PtrPRule) pRule;
      else
        {
          pPres = ((PtrElement) element)->ElFirstPRule;
          stop = FALSE;
          do
            if (pPres->PrType == ((PtrPRule) pRule)->PrType &&
                pPres->PrViewNum == ((PtrPRule) pRule)->PrViewNum)
              {
                TtaError (ERR_duplicate_presentation_rule);
#ifndef NODISPLAY
                ok = FALSE;
#endif
                stop = TRUE;
              }
            else if (pPres->PrNextPRule == NULL)
              {
                pPres->PrNextPRule = (PtrPRule) pRule;
                ((PtrPRule) pRule)->PrNextPRule = NULL;
                stop = TRUE;
              }
            else
              pPres = pPres->PrNextPRule;
          while (!stop);
        }
#ifndef NODISPLAY
      if (ok)
        RedisplayNewPRule (document, (PtrElement) element, (PtrPRule) pRule);
#endif
    }
}


/*----------------------------------------------------------------------
  TtaSetPRuleValue

  Changes the value of a presentation rule. The presentation rule must
  be attached to an element that is part of a document.
  Parameters:
  element: the element to which the presentation rule is attached.
  pRule: the presentation rule to be changed.
  value: the value to be set.
  document: the document to which the element belongs.
  Valid values according to rule type:
  PRDepth: an integer (z-index of the element).
  PRVisibility: an integer >= 0 (visibility level).
  PRFillPattern: rank of the pattern in file thot.pattern.
  PRBackground: rank of the background color in file thot.color.
  PRForeground: rank of the foreground color in file thot.color.
  PRColor: rank of the color in file thot.color.
  PRStopColor: rank of the stop-color in file thot.color.
  PRBorderTopColor, PRBorderRightColor, PRBorderBottomColor,PRBorderLeftColor:
  if value >= 0: rank of the color in file thot.color.
  if value < 0 : -2 means transparent, -1 means same color as foreground, -3
                 means undefined
  PRFont: FontTimes, FontHelvetica, FontCourier.
  PRStyle: StyleRoman, StyleItalics, StyleOblique.
  PRWeight: WeightNormal, WeightBold.
  PRVariant: VariantNormal, VariantSmallCaps, VariantDoubleStruck,
             VariantFraktur, VariantScript.
  PRUnderline: NoUnderline, Underline, Overline, CrossOut.
  PRThickness: ThinUnderline, ThickUnderline.
  PRDirection: LeftToRight, RightToLeft.
  PRUnicodeBidi: Normal, Embed, Override.
  PRLineStyle, PRBorderTopStyle, PRBorderRightStyle, PRBorderBottomStyle,
  PRBorderLeftStyle: SolidLine, DashedLine, DottedLine.
  PRDisplay: Undefined, DisplayNone, Inline, Block, ListItem, RunIn,
  InlineBlock.
  PRListStyleType: Disc, Circle, Square, Decimal, DecimalLeadingZero,
  LowerRoman, UpperRoman, LowerGreek, UpperGreek, LowerLatin, UpperLatin,
  ListStyleTypeNone.
  PRListStylePosition: Inside, Outside.
  PRNoBreak1, PRNoBreak2: a positive integer (distance in points).
  PRIndent: a positive, null or negative integer (indentation in points).
  PRSize: an integer between 6 and 72 (body size in points).
  PRLineSpacing: a positive integer (line spacing in points).
  PRLineWeight: a positive or null integer (stroke width for graphics in
  points).
  PRMarginTop, PRMarginLeft, PRMarginBottom, PRMarginRight: a positive, null
  or negative integer (margin in points).
  PRPaddingTop, PRPaddingRight, PRPaddingBottom, PRPaddingLeft: an integer
  > 0 (padding in points).
  PRBorderTopWidth, PRBorderRightWidth, PRBorderBottomWidth, PRBorderLeftWidth:
  an integer > 0 (border width in points).
  PRXRadius, PRYRadius: an integer (radius in points)
  PRTop, PRRight, PRBottom, PRLeft, PRBackgroundHorizPos, PRBackgroundVertPos: an integer (distance)
  PRVertPos, PRHorizPos: an integer (distance in points)
  PRWidth, PRHeight: an integer (size in points)
  PRHyphenate: Hyphenation, NoHyphenation.
  PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots, Justify.
  PROpacity, PRStrokeOpacity, PRFillOpacity, PRStopOpacity: an integer.
  PRFillRule: NonZero, EvenOdd.
  ----------------------------------------------------------------------*/
void TtaSetPRuleValue (Element element, PRule pRule, int value, Document document)
{
#ifndef NODISPLAY
  ThotBool            done;
#endif

  UserErrorCode = 0;
  if (element == NULL || pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* verifies the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
#ifndef NODISPLAY
      done = TRUE;
#endif
      switch (((PtrPRule) pRule)->PrType)
        {
        case PtFunction:
          if (((PtrPRule) pRule)->PrPresFunction == FnBackgroundPicture)
            ((PtrPRule) pRule)->PrPresBox[0] = value;
          else if (((PtrPRule) pRule)->PrPresFunction == FnBackgroundRepeat)
            ((PtrPRule) pRule)->PrPresBox[0] = value;
          break;

        case PtDepth:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          ((PtrPRule) pRule)->PrIntValue = value;
          ((PtrPRule) pRule)->PrValueType = PrNumValue;
          break;
        case PtVisibility:
        case PtFillPattern:
        case PtOpacity:
        case PtFillOpacity:
        case PtStrokeOpacity:
        case PtStopOpacity:
        case PtBackground:
        case PtForeground:
        case PtColor:
        case PtStopColor:
          if (value < 0)
            TtaError (ERR_invalid_parameter);
          else
            {
              ((PtrPRule) pRule)->PrPresMode = PresImmediate;
              ((PtrPRule) pRule)->PrIntValue = value;
              ((PtrPRule) pRule)->PrValueType = PrNumValue;
            }
          break;
	case PtMarker:
	case PtMarkerStart:
	case PtMarkerMid:
	case PtMarkerEnd:
          if (value == 0)
	    ((PtrPRule) pRule)->PrValueType = PrNumValue;
          else
	    ((PtrPRule) pRule)->PrValueType = PrConstStringValue;
	  ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	  ((PtrPRule) pRule)->PrIntValue = value;
	  break;
        case PtVis:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case VsHidden:
              ((PtrPRule) pRule)->PrChrValue = 'H';
              break;
            case VsVisible:
              ((PtrPRule) pRule)->PrChrValue = 'V';
              break;
            case VsCollapse:
              ((PtrPRule) pRule)->PrChrValue = 'C';
              break;
            case VsInherit:
              ((PtrPRule) pRule)->PrChrValue = 'I';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtBorderTopColor:
        case PtBorderRightColor:
        case PtBorderBottomColor:
        case PtBorderLeftColor:
          if (value < -2)
            TtaError (ERR_invalid_parameter);
          else
            {
              ((PtrPRule) pRule)->PrPresMode = PresImmediate;
              ((PtrPRule) pRule)->PrIntValue = value;
              ((PtrPRule) pRule)->PrValueType = PrNumValue;
            }
          break;

        case PtFont:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case FontTimes:
              ((PtrPRule) pRule)->PrChrValue = 'T';
              break;
            case FontHelvetica:
              ((PtrPRule) pRule)->PrChrValue = 'H';
              break;
            case FontCourier:
              ((PtrPRule) pRule)->PrChrValue = 'C';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;
        case PtStyle:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case StyleRoman:
              ((PtrPRule) pRule)->PrChrValue = 'R';
              break;
            case StyleItalics:
              ((PtrPRule) pRule)->PrChrValue = 'I';
              break;
            case StyleOblique:
              ((PtrPRule) pRule)->PrChrValue = 'O';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;
        case PtWeight:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case WeightNormal:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            case WeightBold:
              ((PtrPRule) pRule)->PrChrValue = 'B';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;
        case PtVariant:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case VariantNormal:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            case VariantSmallCaps:
              ((PtrPRule) pRule)->PrChrValue = 'C';
              break;
            case VariantDoubleStruck:
              ((PtrPRule) pRule)->PrChrValue = 'D';
              break;
            case VariantFraktur:
              ((PtrPRule) pRule)->PrChrValue = 'F';
              break;
            case VariantScript:
              ((PtrPRule) pRule)->PrChrValue = 'S';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;
        case PtUnderline:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case NoUnderline:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            case Underline:
              ((PtrPRule) pRule)->PrChrValue = 'U';
              break;
            case Overline:
              ((PtrPRule) pRule)->PrChrValue = 'O';
              break;
            case CrossOut:
              ((PtrPRule) pRule)->PrChrValue = 'C';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;
        case PtThickness:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case ThinUnderline:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            case ThickUnderline:
              ((PtrPRule) pRule)->PrChrValue = 'T';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;
        case PtDirection:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case LeftToRight:
              ((PtrPRule) pRule)->PrChrValue = 'L';
              break;
            case RightToLeft:
              ((PtrPRule) pRule)->PrChrValue = 'R';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;
        case PtUnicodeBidi:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case Normal:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            case Embed:
              ((PtrPRule) pRule)->PrChrValue = 'E';
              break;
            case Override:
              ((PtrPRule) pRule)->PrChrValue = 'O';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtLineStyle:
        case PtBorderTopStyle:
        case PtBorderRightStyle:
        case PtBorderBottomStyle:
        case PtBorderLeftStyle:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case SolidLine:
              ((PtrPRule) pRule)->PrChrValue = 'S';
              break;
            case DashedLine:
              ((PtrPRule) pRule)->PrChrValue = '-';
              break;
            case DottedLine:
              ((PtrPRule) pRule)->PrChrValue = '.';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtDisplay:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case Undefined:
              ((PtrPRule) pRule)->PrChrValue = 'U';
              break;
            case DisplayNone:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            case Inline:
              ((PtrPRule) pRule)->PrChrValue = 'I';
              break;
            case Block:
              ((PtrPRule) pRule)->PrChrValue = 'B';
              break;
            case ListItem:
              ((PtrPRule) pRule)->PrChrValue = 'L';
              break;
            case RunIn:
              ((PtrPRule) pRule)->PrChrValue = 'R';
              break;
            case InlineBlock:
              ((PtrPRule) pRule)->PrChrValue = 'b';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtListStyleType:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case Disc:
              ((PtrPRule) pRule)->PrChrValue = 'D';
              break;
            case Circle:
              ((PtrPRule) pRule)->PrChrValue = 'C';
              break;
            case Square:
              ((PtrPRule) pRule)->PrChrValue = 'S';
              break;
            case Decimal:
              ((PtrPRule) pRule)->PrChrValue = '1';
              break;
            case DecimalLeadingZero:
              ((PtrPRule) pRule)->PrChrValue = 'Z';
              break;
            case LowerRoman:
              ((PtrPRule) pRule)->PrChrValue = 'i';
              break;
            case UpperRoman:
              ((PtrPRule) pRule)->PrChrValue = 'I';
              break;
            case LowerGreek:
              ((PtrPRule) pRule)->PrChrValue = 'g';
              break;
            case UpperGreek:
              ((PtrPRule) pRule)->PrChrValue = 'G';
              break;
            case LowerLatin:
              ((PtrPRule) pRule)->PrChrValue = 'a';
              break;
            case UpperLatin:
              ((PtrPRule) pRule)->PrChrValue = 'A';
              break;
            case ListStyleTypeNone:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtListStylePosition:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case Inside:
              ((PtrPRule) pRule)->PrChrValue = 'I';
              break;
            case Outside:
              ((PtrPRule) pRule)->PrChrValue = 'O';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtFloat:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case FloatLeft:
              ((PtrPRule) pRule)->PrChrValue = 'L';
              break;
            case FloatRight:
              ((PtrPRule) pRule)->PrChrValue = 'R';
              break;
            case FloatNone:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtClear:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case ClearLeft:
              ((PtrPRule) pRule)->PrChrValue = 'L';
              break;
            case ClearRight:
              ((PtrPRule) pRule)->PrChrValue = 'R';
              break;
            case ClearBoth:
              ((PtrPRule) pRule)->PrChrValue = 'B';
              break;
            case ClearNone:
              ((PtrPRule) pRule)->PrChrValue = 'N';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtPosition:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case PnStatic:
              ((PtrPRule) pRule)->PrChrValue = 'S';
              break;
            case PnRelative:
              ((PtrPRule) pRule)->PrChrValue = 'R';
              break;
            case PnAbsolute:
              ((PtrPRule) pRule)->PrChrValue = 'A';
              break;
            case PnFixed:
              ((PtrPRule) pRule)->PrChrValue = 'F';
              break;
            case PnInherit:
              ((PtrPRule) pRule)->PrChrValue = 'I';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtBreak1:
        case PtBreak2:
        case PtIndent:
        case PtSize:
        case PtLineSpacing:
        case PtLineWeight:
        case PtMarginTop:
        case PtMarginRight:
        case PtMarginBottom:
        case PtMarginLeft:
        case PtPaddingTop:
        case PtPaddingRight:
        case PtPaddingBottom:
        case PtPaddingLeft:
        case PtBorderTopWidth:
        case PtBorderRightWidth:
        case PtBorderBottomWidth:
        case PtBorderLeftWidth:
        case PtXRadius:
        case PtYRadius:
        case PtTop:
        case PtRight:
        case PtBottom:
        case PtLeft:
        case PtBackgroundHorizPos:
        case PtBackgroundVertPos:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          ((PtrPRule) pRule)->PrMinUnit = UnPoint;
          ((PtrPRule) pRule)->PrMinAttr = FALSE;
          ((PtrPRule) pRule)->PrMinValue = value;
          break;

        case PtVertPos:
        case PtHorizPos:
          ((PtrPRule) pRule)->PrPosRule.PoDistUnit = UnPoint;
          ((PtrPRule) pRule)->PrPosRule.PoDistAttr = FALSE;
          ((PtrPRule) pRule)->PrPosRule.PoDistance = value;
          ((PtrPRule) pRule)->PrPosRule.PoUserSpecified = FALSE;
          break;

        case PtWidth:
        case PtHeight:
          if (((PtrPRule) pRule)->PrDimRule.DrPosition)
            {
              ((PtrPRule) pRule)->PrDimRule.DrPosRule.PoDistUnit = UnPixel;
              ((PtrPRule) pRule)->PrDimRule.DrPosRule.PoDistAttr = FALSE;
              ((PtrPRule) pRule)->PrDimRule.DrPosRule.PoDistance = value;
            }
          else
            {
              ((PtrPRule) pRule)->PrDimRule.DrPosition = FALSE;
              ((PtrPRule) pRule)->PrDimRule.DrAbsolute = TRUE;
              ((PtrPRule) pRule)->PrDimRule.DrSameDimens = FALSE;
              ((PtrPRule) pRule)->PrDimRule.DrUnit = UnPoint;
              ((PtrPRule) pRule)->PrDimRule.DrAttr = FALSE;
              ((PtrPRule) pRule)->PrDimRule.DrMin = 0;
              ((PtrPRule) pRule)->PrDimRule.DrUserSpecified = FALSE;
              ((PtrPRule) pRule)->PrDimRule.DrValue = value;
            }
          break;

        case PtHyphenate:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case Hyphenation:
              ((PtrPRule) pRule)->PrBoolValue = TRUE;
              break;
            case NoHyphenation:
              ((PtrPRule) pRule)->PrBoolValue = FALSE;
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtAdjust:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case AdjustLeft:
              ((PtrPRule) pRule)->PrAdjust = AlignLeft;
              break;
            case AdjustRight:
              ((PtrPRule) pRule)->PrAdjust = AlignRight;
              break;
            case Centered:
              ((PtrPRule) pRule)->PrAdjust = AlignCenter;
              break;
            case Justify:
              ((PtrPRule) pRule)->PrAdjust = AlignJustify;
              break;
            case LeftWithDots:
              ((PtrPRule) pRule)->PrAdjust = AlignLeftDots;
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        case PtFillRule:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          switch (value)
            {
            case NonZero:
              ((PtrPRule) pRule)->PrChrValue = 'n';
              break;
            case EvenOdd:
              ((PtrPRule) pRule)->PrChrValue = 'e';
              break;
            default:
#ifndef NODISPLAY
              done = FALSE;
#endif
              TtaError (ERR_invalid_parameter);
              break;
            }
          break;

        default:
          TtaError (ERR_invalid_parameter);
#ifndef NODISPLAY
          done = FALSE;
#endif
          break;
        }
#ifndef NODISPLAY
      if (done)
        RedisplayNewPRule (document, (PtrElement) element, (PtrPRule) pRule);
#endif
    }
}

/*----------------------------------------------------------------------
  TtaSetPRuleValueWithUnit

  Changes the value and the length unit of a presentation rule.
  The presentation rule must be attached to an element that is part of
  a document.
  Parameters:
  element: the element to which the presentation rule is attached.
  pRule: the presentation rule to be changed.
  value: the value to be set.
  unit: the length unit in which the value is expressed.
  document: the document to which the element belongs.

  Valid values according to rule type:
  PRNoBreak1, PRNoBreak2: a positive integer (distance).
  PRIndent: a positive, null or negative integer (indentation).
  PRSize: an integer between 6 and 72 (body size).
  PRLineSpacing: a positive integer (line spacing).
  PRLineWeight: a positive or null integer (stroke width for graphics).
  PRMarginTop, PRMarginLeft, PRMarginBottom, PRMarginRight: a positive, null
  or negative integer (margin).
  PRPaddingTop, PRPaddingRight, PRPaddingBottom, PRPaddingLeft: an integer
  > 0 (padding).
  PRBorderTopWidth, PRBorderRightWidth, PRBorderBottomWidth, PRBorderLeftWidth:
  an integer > 0 (border width).
  PRXRadius, PRYRadius: a positive integer (radius)
  PRTop, PRRight, PRBottom, PRLeft, PRBackgroundHorizPos, PRBackgroundVertPos: an integer (distance)
  PRVertPos, PRHorizPos: an integer (distance)
  PRWidth, PRHeight: an integer (width or height)
  ----------------------------------------------------------------------*/
void TtaSetPRuleValueWithUnit (Element element, PRule pRule, int value,
                               TypeUnit unit, Document document)
{
#ifndef NODISPLAY
  ThotBool            done;
#endif

  UserErrorCode = 0;
  if (element == NULL || pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* verifies the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
#ifndef NODISPLAY
      done = TRUE;
#endif
      switch (((PtrPRule) pRule)->PrType)
        {
        case PtBreak1:
        case PtBreak2:
        case PtIndent:
        case PtSize:
        case PtLineSpacing:
        case PtLineWeight:
        case PtMarginTop:
        case PtMarginRight:
        case PtMarginBottom:
        case PtMarginLeft:
        case PtPaddingTop:
        case PtPaddingRight:
        case PtPaddingBottom:
        case PtPaddingLeft:
        case PtBorderTopWidth:
        case PtBorderRightWidth:
        case PtBorderBottomWidth:
        case PtBorderLeftWidth:
        case PtXRadius:
        case PtYRadius:
        case PtTop:
        case PtRight:
        case PtBottom:
        case PtLeft:
        case PtBackgroundHorizPos:
        case PtBackgroundVertPos:
          ((PtrPRule) pRule)->PrPresMode = PresImmediate;
          ((PtrPRule) pRule)->PrMinUnit = unit;
          ((PtrPRule) pRule)->PrMinAttr = FALSE;
          ((PtrPRule) pRule)->PrMinValue = value;
          break;
        case PtVertPos:
        case PtHorizPos:
          ((PtrPRule) pRule)->PrPosRule.PoDistUnit = unit;
          ((PtrPRule) pRule)->PrPosRule.PoDistAttr = FALSE;
          ((PtrPRule) pRule)->PrPosRule.PoDistance = value;
          ((PtrPRule) pRule)->PrPosRule.PoUserSpecified = FALSE;
          break;
        case PtWidth:
        case PtHeight:
          ((PtrPRule) pRule)->PrDimRule.DrPosition = FALSE;
          ((PtrPRule) pRule)->PrDimRule.DrAbsolute = TRUE;
          ((PtrPRule) pRule)->PrDimRule.DrSameDimens = FALSE;
          ((PtrPRule) pRule)->PrDimRule.DrUnit = unit;
          ((PtrPRule) pRule)->PrDimRule.DrAttr = FALSE;
          ((PtrPRule) pRule)->PrDimRule.DrMin = 0;
          ((PtrPRule) pRule)->PrDimRule.DrUserSpecified = FALSE;
          ((PtrPRule) pRule)->PrDimRule.DrValue = value;
          break;
        default:
          TtaError (ERR_invalid_parameter);
#ifndef NODISPLAY
          done = FALSE;
#endif
          break;
        }
#ifndef NODISPLAY
      if (done)
        RedisplayNewPRule (document, (PtrElement) element, (PtrPRule) pRule);
#endif
    }
}

/*----------------------------------------------------------------------
  TtaSetPositionPRuleDelta

  Changes the delta value and the unit of a position presentation rule.
  The presentation rule must be attached to an element that is part of
  a document.
  Parameters:
  element: the element to which the presentation rule is attached.
  pRule: the presentation rule to be changed. It must be a position rule.
  delta: the value to be set.
  unit: the unit in which the delta value is expressed.
  document: the document to which the element belongs.
  ----------------------------------------------------------------------*/
void TtaSetPositionPRuleDelta (Element element, PRule pRule, int delta,
                               TypeUnit unit, Document document)
{
  UserErrorCode = 0;
  if (element == NULL || pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* verifies the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else  if (((PtrPRule) pRule)->PrType != PtVertPos &&
            ((PtrPRule) pRule)->PrType != PtHorizPos)
    TtaError (ERR_invalid_parameter);
  else
    {
      ((PtrPRule) pRule)->PrPosRule.PoDeltaUnit = unit;
      ((PtrPRule) pRule)->PrPosRule.PoDistDelta = delta;
#ifndef NODISPLAY
      RedisplayNewPRule (document, (PtrElement) element, (PtrPRule) pRule);
#endif
    }
}

/*----------------------------------------------------------------------
  TtaSetPRuleView

  Sets the view to which a presentation rule applies. The presentation rule
  must not be attached yet to an element.
  Parameters:
  pRule: the presentation rule to be changed.
  view: the value to be set.
  ----------------------------------------------------------------------*/
void TtaSetPRuleView (PRule pRule, int view)
{
  UserErrorCode = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else if (view < 1 || view > MAX_VIEW)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameters are OK */
    ((PtrPRule) pRule)->PrViewNum = view;
}


#ifndef NODISPLAY
/*----------------------------------------------------------------------
  TtaGetPixelValue converts a logical value into a pixel value for
  a given element.
  ----------------------------------------------------------------------*/
int TtaGetPixelValue (int val, int unit, Element element, Document document)
{
  PtrAbstractBox      pAb = NULL;
  TypeUnit            int_unit;
  int                 frame;
  int                 w, h;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      frame = GetWindowNumber (document, 1);
      if (frame)
        pAb = AbsBoxOfEl ((PtrElement) element, 1);
      if (pAb == NULL)
        TtaError (ERR_element_has_no_box);
      else
        {
          // get the internal unit
          switch (unit)
            {
            case UNIT_REL:
            case UNIT_EM:
              int_unit = UnRelative;
              val *= 10;
              break;
            case UNIT_PT:
              int_unit = UnPoint;
              break;
            case UNIT_PC:
              int_unit = UnPoint;
              val *= 12;
              break;
            case UNIT_IN:
              int_unit = UnPoint;
              val *= 72;
              break;
            case UNIT_CM:
              int_unit = UnPoint;
              val *= 28;
              break;
            case UNIT_MM:
              int_unit = UnPoint;
              val *= 28;
              val /= 10;
              break;
            case UNIT_PERCENT:
              int_unit = UnPercent;
              break;
            case UNIT_XHEIGHT:
              int_unit = UnXHeight;
              val *= 10;
              break;
            case VALUE_AUTO:
              int_unit = UnAuto;
              break;
            default:
              int_unit = UnPixel;
              break;
            }
          if (int_unit == UnPercent || int_unit == UnAuto)
            {
              if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                GetSizesFrame (frame, &w, &h);
              else
                w = pAb->AbEnclosing->AbBox->BxW;
              if (int_unit == UnAuto)
                val = w;
              else
                val = PixelValue (val, UnPercent, (PtrAbstractBox) w,
                                  ViewFrameTable[0].FrMagnification);
            }
          else
            val = PixelValue (val, int_unit, pAb, 0);
        }
    }
  return val;
}

/*----------------------------------------------------------------------
  TtaGetLogicalValue converts a pixel value into a logical value for
  a given element.
  ----------------------------------------------------------------------*/
int TtaGetLogicalValue (int val, int unit, Element element, Document document)
{
  PtrAbstractBox      pAb = NULL;
  TypeUnit            int_unit;
  int                 frame;
  int                 w, h;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      frame = GetWindowNumber (document, 1);
      if (frame)
        pAb = AbsBoxOfEl ((PtrElement) element, 1);
      if (pAb == NULL)
        TtaError (ERR_element_has_no_box);
      else
        {
          // get the internal unit
          switch (unit)
            {
            case UNIT_REL:
            case UNIT_EM:
              int_unit = UnRelative;
              break;
            case UNIT_PT:
            case UNIT_PC:
            case UNIT_IN:
            case UNIT_CM:
            case UNIT_MM:
              int_unit = UnPoint;
              break;
            case UNIT_PERCENT:
              int_unit = UnPercent;
              break;
            case UNIT_XHEIGHT:
              int_unit = UnXHeight;
              break;
            case VALUE_AUTO:
              int_unit = UnAuto;
              break;
            default:
              int_unit = UnPixel;
              break;
            }
          if (int_unit == UnPercent || int_unit == UnAuto)
            {
              if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                GetSizesFrame (frame, &w, &h);
              else
                w = pAb->AbEnclosing->AbBox->BxW;
              if (int_unit == UnAuto)
                val = w;
              else
                val = LogicalValue (val, UnPercent, (PtrAbstractBox) w,
                                    ViewFrameTable[frame - 1].FrMagnification);
            }
          else
            val = LogicalValue (val, int_unit, pAb, 0);
          /* translate to external units */
          if (unit == UNIT_REL || unit == UNIT_EM || unit == UNIT_XHEIGHT)
            val /= 10;
        }
    }
  return val;
}

/*----------------------------------------------------------------------
  TtaChangeBoxSize

  Changes the height and width of the box corresponding to an element in
  a given view.
  Parameters:
  element: the element of interest.
  document: the document to which the element belongs.
  view: the view.
  deltaX: width increment in points (positive, negative or zero).
  deltaY: height increment in points (positive, negative or zero).
  unit: the unit used for the values.
  ----------------------------------------------------------------------*/
void TtaChangeBoxSize (Element element, Document document, View view,
                       int deltaX, int deltaY, TypeUnit unit)
{
  PtrAbstractBox      pAb;
  int                 frame;
  int                 x, y;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      frame = GetWindowNumber (document, view);
      if (frame)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL)
            TtaError (ERR_element_has_no_box);
          else
            {
              /* Convert values to pixels */
              if (unit == UnPercent)
                {
                  if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                    GetSizesFrame (frame, &x, &y);
                  else
                    {
                      x = pAb->AbEnclosing->AbBox->BxW;
                      y = pAb->AbEnclosing->AbBox->BxH;
                    }
                  deltaX = PixelValue (deltaX, UnPercent, (PtrAbstractBox) x, 0);
                  deltaY = PixelValue (deltaY, UnPercent, (PtrAbstractBox) y, 0);
                }
              else
                {
                  deltaX = PixelValue (deltaX, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                  deltaY = PixelValue (deltaY, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                }
              NewDimension (pAb, deltaX+pAb->AbBox->BxWidth, deltaY+pAb->AbBox->BxHeight, frame, FALSE);
              RedispNewGeometry (document, (PtrElement) element);
            }
        }
    }
}


/*----------------------------------------------------------------------
  TtaChangeBoxPosition

  Moves the box of an element in a given view.
  Parameters:
  element: the element of interest.
  document: the document to which the element belongs.
  view: the view.
  deltaX: horizontal shift in points (positive, negative or zero).
  deltaY: vertical shift in points (positive, negative or zero).
  unit: the unit used for the values.
  ----------------------------------------------------------------------*/
void TtaChangeBoxPosition (Element element, Document document, View view,
                           int deltaX, int deltaY, TypeUnit unit)
{
#ifndef NODISPLAY

  PtrAbstractBox      pAb;
  int                 frame;
  int                 x, y;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL)
            TtaError (ERR_element_has_no_box);
          else
            {
              /* Convert values to pixels */
              if (unit == UnPercent)
                {
                  if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                    GetSizesFrame (frame, &x, &y);
                  else
                    {
                      x = pAb->AbEnclosing->AbBox->BxW;
                      y = pAb->AbEnclosing->AbBox->BxH;
                    }
                  deltaX = PixelValue (deltaX, UnPercent, (PtrAbstractBox) x, 0);
                  deltaY = PixelValue (deltaY, UnPercent, (PtrAbstractBox) y, 0);
                }
              else
                {
                  deltaX = PixelValue (deltaX, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                  deltaY = PixelValue (deltaY, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                }
              NewPosition (pAb, deltaX + pAb->AbBox->BxXOrg, 0, deltaY + pAb->AbBox->BxYOrg, 0, frame, FALSE);
              RedispNewGeometry (document, (PtrElement) element);
            }
        }
    }
#endif /* NODISPLAY */
}


/*----------------------------------------------------------------------
  TtaGetDepth
  Returns the depth of the box corresponding to an element in a given view.

  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  ----------------------------------------------------------------------*/
int TtaGetDepth (Element element, Document document, View view)
{
  PtrAbstractBox      pAb;
  int                 frame;
  int                 val;

  UserErrorCode = 0;
  val = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL)
            TtaError (ERR_element_has_no_box);
          else
            val = pAb->AbDepth;
        }
    }
  return val;
}


/*----------------------------------------------------------------------
  TtaGiveBoxColors
  Returns the color and background color of the box corresponding to an
  element in a given view.
  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  Return parameters:
  color the thot color
  bg_color the thot background color
  ----------------------------------------------------------------------*/
void TtaGiveBoxColors (Element element, Document document, View view,
                       int *color, int *bg_color)
{
  PtrAbstractBox      pAb;
  int                 frame;

  UserErrorCode = 0;
  *color = 1;
  *bg_color = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL || pAb->AbBox == NULL)
            TtaError (ERR_element_has_no_box);
          else
            {
              *color = pAb->AbForeground;
              if (pAb->AbFillPattern == 0)
                // no fill color
                *bg_color = -1;
              else
                *bg_color = pAb->AbBackground;
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaGiveBoxFontInfo
  Returns the font description of the box corresponding to an
  element in a given view.
  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  Return parameters:
  size the font size value
  unit the unit of the font size
  family the font family
  ----------------------------------------------------------------------*/
void TtaGiveBoxFontInfo (Element element, Document document, View view,
                         int *size, TypeUnit *unit, int *family)
{
  PtrAbstractBox      pAb;
  int                 frame;

  UserErrorCode = 0;
  *size = -1;
  *unit = UnPixel;
  *family = 1;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL || pAb->AbBox == NULL)
            TtaError (ERR_element_has_no_box);
          else
            {
              *size = pAb->AbSize;
              *unit = pAb->AbSizeUnit;
              *family = pAb->AbFont;
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaGiveBoxSize
  Returns the height and width of the box corresponding to an element in
  a given view.
  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  unit: the unit used for the values.
  Return parameters:
  width: box width in units.
  height: box height in units.
  ----------------------------------------------------------------------*/
void TtaGiveBoxSize (Element element, Document document, View view,
                     TypeUnit unit, int *width, int *height)
{
  PtrAbstractBox      pAb;
  int                 frame;
  int                 x, y;

  UserErrorCode = 0;
  *width = 0;
  *height = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL || pAb->AbBox == NULL)
            TtaError (ERR_element_has_no_box);
          else if (pAb->AbEnclosing &&
                   pAb->AbEnclosing->AbElement->ElStructSchema &&
                   (strcmp (pAb->AbElement->ElStructSchema->SsName,"SVG") ||
                    TypeHasException (ExcIsDraw, pAb->AbElement->ElTypeNumber,
                                      pAb->AbElement->ElStructSchema)))
            {
              *width = pAb->AbBox->BxW;
              *height = pAb->AbBox->BxH;
            }
          else
            {
#ifdef _GL
              *width = pAb->AbBox->BxClipW;
              *height = pAb->AbBox->BxClipH;
#else /* _GL */
              *width = pAb->AbBox->BxW;
              *height = pAb->AbBox->BxH;
#endif /* _GL */

              /* Convert values to pixels */
              if (unit == UnPercent)
                {
                  if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                    GetSizesFrame (frame, &x, &y);
                  else
                    {
                      x = pAb->AbEnclosing->AbBox->BxW;
                      y = pAb->AbEnclosing->AbBox->BxH;
                    }
                  *width = LogicalValue (*width, UnPercent, (PtrAbstractBox) x, 0);
                  *height = LogicalValue (*height, UnPercent, (PtrAbstractBox) y, 0);
                }
              else
                {
                  *width = LogicalValue (*width, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                  *height = LogicalValue (*height, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaGiveBoxPosition

  Returns the x and y coordinates of the box corresponding to an element in
  a given view. The returned coordinates indicate the distance
  between the upper left corner of the box and the upper left corner of its
  parent box.
  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  unit: the unit used for the returned values.
  Return parameters:
  xCoord: distance from the left edge of the parent box to the left
  edge of the box.
  yCoord:  distance from the upper edge of the parent box to the upper
  edge of the box.
  inner is TRUE when the inner parent origin is considered else the outside
  of the parent origin is considered.
  ----------------------------------------------------------------------*/
void TtaGiveBoxPosition (Element element, Document document, View view,
                         TypeUnit unit, ThotBool inner, int *xCoord, int *yCoord)
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  ViewFrame	         *pFrame;
  int                 frame;
  int                 x, y, w, h;

  UserErrorCode = 0;
  *xCoord = 0;
  *yCoord = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL)
            TtaError (ERR_element_has_no_box);
          else
            {
              pBox = pAb->AbBox;
              pFrame = &ViewFrameTable[frame - 1];
              if (pBox->BxType == BoSplit && pBox->BxNexChild != NULL)
                pBox = pBox->BxNexChild;
              
              if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
                {
                  GetSizesFrame (frame, &x, &y);
                  *xCoord = pBox->BxXOrg - pFrame->FrXOrg;
                  *yCoord = pBox->BxYOrg - pFrame->FrYOrg;
                }
              else
                {
                 if (pAb->AbEnclosing &&
                      pAb->AbEnclosing->AbElement->ElStructSchema &&
                      (strcmp (pAb->AbElement->ElStructSchema->SsName,"SVG") ||
                        TypeHasException (ExcIsDraw,
                                          pAb->AbEnclosing->AbElement->ElTypeNumber,
                                          pAb->AbEnclosing->AbElement->ElStructSchema)))
                   //if (IsSVGComponent (pAb->AbElement))
                    {
                      w = pAb->AbEnclosing->AbBox->BxW;
                      h = pAb->AbEnclosing->AbBox->BxH;
                      x = pAb->AbEnclosing->AbBox->BxXOrg - pFrame->FrXOrg;
                      y = pAb->AbEnclosing->AbBox->BxYOrg - pFrame->FrYOrg;
                      if (inner)
                        {
                        x +=  pAb->AbEnclosing->AbBox->BxLMargin
                          + pAb->AbEnclosing->AbBox->BxLBorder
                          + pAb->AbEnclosing->AbBox->BxLPadding;
                        y += pAb->AbEnclosing->AbBox->BxTMargin
                          + pAb->AbEnclosing->AbBox->BxTBorder
                          + pAb->AbEnclosing->AbBox->BxTPadding;
                        }
                    }
                  else
                    {
#ifdef _GL
                      w = pAb->AbEnclosing->AbBox->BxClipW;
                      h = pAb->AbEnclosing->AbBox->BxClipH;
                      x = pAb->AbEnclosing->AbBox->BxClipX;
                      y = pAb->AbEnclosing->AbBox->BxClipY;
#else /* _GL */
                      w = pAb->AbEnclosing->AbBox->BxW;
                      h = pAb->AbEnclosing->AbBox->BxH;
                      x = pAb->AbEnclosing->AbBox->BxXOrg - pFrame->FrXOrg;
                      y = pAb->AbEnclosing->AbBox->BxYOrg - pFrame->FrYOrg;
#endif /* _GL */
                    }
#ifdef _GL
                  *xCoord = pBox->BxClipX - x;
                  *yCoord = pBox->BxClipY - y;
#else /* _GL */
                  *xCoord = pBox->BxXorg - pFrame->FrXorg - x;
                  *yCoord = pBox->BxYOrg - pFrame->FrYorg - y;
#endif /* _GL */
                }
	      
              /* Convert values to the requested unit */
              if (unit == UnPercent)
                {
                  *xCoord = LogicalValue (*xCoord, UnPercent, (PtrAbstractBox) w, 0);
                  *yCoord = LogicalValue (*yCoord, UnPercent, (PtrAbstractBox) h, 0);
                }
              else
                {
                  *xCoord = LogicalValue (*xCoord, unit, pAb, pFrame->FrMagnification);
                  *yCoord = LogicalValue (*yCoord, unit, pAb, pFrame->FrMagnification);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaGiveBoxAbsPosition

  Returns the x and y coordinates of the box corresponding to an element in
  a given view. The returned coordinates indicate the distance
  between the upper left corner of the box and the upper left corner of its
  window.
  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  unit: the unit used for the values.
  Return parameters:
  xCoord: distance from the left edge of the window to the left
  edge of the box.
  yCoord:  distance from the upper edge of the window to the upper
  edge of the box.
  ----------------------------------------------------------------------*/
void TtaGiveBoxAbsPosition (Element element, Document document, View view,
                            TypeUnit unit, int *xCoord, int *yCoord)
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  ViewFrame	      *pFrame;
  int                 frame;
  int                 x, y;

  UserErrorCode = 0;
  *xCoord = 0;
  *yCoord = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          pAb = AbsBoxOfEl ((PtrElement) element, view);
          if (pAb == NULL)
            {
              *xCoord = 32000;
              *yCoord = 32000;
            }
          else if (pAb->AbBox == NULL)
            {
              *xCoord = 0;
              *yCoord = 0;
            }
          else
            {
              pBox = pAb->AbBox;
              if (pBox->BxType == BoSplit && pBox->BxNexChild != NULL)
                pBox = pBox->BxNexChild;
              pFrame = &ViewFrameTable[frame - 1];
#ifndef _GL
              *xCoord = pBox->BxXOrg - pFrame->FrXOrg;
              *yCoord = pBox->BxYOrg - pFrame->FrYOrg;
#else /* _GL */
              *xCoord = pBox->BxClipX;
              *yCoord = pBox->BxClipY;
#endif /* _GL */
	     
              /* Convert values to the requested unit */
              if (unit == UnPercent)
                {
                  GetSizesFrame (frame, &x, &y);
                  *xCoord = LogicalValue (*xCoord, UnPercent, (PtrAbstractBox) x, 0);
                  *yCoord = LogicalValue (*yCoord, UnPercent, (PtrAbstractBox) y, 0);
                }
              else
                {
                  *xCoord = LogicalValue (*xCoord, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                  *yCoord = LogicalValue (*yCoord, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaGiveWindowSize

  Returns the height and width of the window corresponding to a given view.

  Parameters:
  document: the document of interest.
  view: the view.
  unit: the unit used for the values (UnPixel or UnPoint only)

  Return parameters:
  width: window width in units.
  height: window height in units.

  ----------------------------------------------------------------------*/
void TtaGiveWindowSize (Document document, View view, TypeUnit unit,
                        int *width, int *height)
{
  int                 frame;
  int                 w, h;

  UserErrorCode = 0;
  *width = 0;
  *height = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (unit == UnPercent || unit == UnRelative || unit == UnXHeight)
    TtaError (ERR_invalid_parameter);
  else
    {
      /* parameter document is correct */
      frame = GetWindowNumber (document, view);
      if (frame != 0)
        {
          GetSizesFrame (frame, &w, &h);
          /* Convert values */
          *width = PixelValue (w, unit, NULL, ViewFrameTable[frame - 1].FrMagnification);
          *height = PixelValue (h, unit, NULL, ViewFrameTable[frame - 1].FrMagnification);
        }
    }
}
#endif /* NODISPLAY */

/*----------------------------------------------------------------------
  TtaGetPRule

  Returns a presentation rule of a given type associated with a given element.
  Parameters:
  element: the element of interest.
  presentationType: type of the desired presentation rule. Available
  values are defined in file presentation.h
  Return value:
  the presentation rule found, or NULL if the element
  does not have this type of presentation rule.
  ----------------------------------------------------------------------*/
PRule               TtaGetPRule (Element element, int presentationType)
{
  PtrPRule          pRule;
  PtrPRule          pPres;
  int               func;
  ThotBool          found;

  UserErrorCode = 0;
  pRule = NULL;
  if (element == NULL || presentationType < 0)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (presentationType == PRShowBox)
        func = FnShowBox;
      else if (presentationType == PRNotInLine)
        func = FnNotInLine;
      else
        func = -1;

      pPres = ((PtrElement) element)->ElFirstPRule;
      found = FALSE;
      while (pPres != NULL && !found)
        if (((int)(pPres->PrType) == presentationType && func == -1) ||
            (pPres->PrType == PtFunction &&
             (int)(pPres->PrPresFunction) == func))
          {
            pRule = pPres;
            found = TRUE;
          }
        else
          pPres = pPres->PrNextPRule;
    }
  return ((PRule) pRule);
}

/*----------------------------------------------------------------------
  TtaGetPRuleType

  Returns the type of a presentation rule.
  Parameters:
  pRule: the presentation rule of interest.
  Return value:
  type of that presentation rule. Available values are defined in
  file presentation.h
  ----------------------------------------------------------------------*/
int                 TtaGetPRuleType (PRule pRule)
{
  int               presentationType;

  UserErrorCode = 0;
  presentationType = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (presentationType == PtFunction &&
          ((PtrPRule) pRule)->PrPresFunction == FnShowBox)
        presentationType = PRShowBox;
      else if (presentationType == PtFunction &&
               ((PtrPRule) pRule)->PrPresFunction == FnNotInLine)
        presentationType = PRNotInLine;
      else
        presentationType = ((PtrPRule) pRule)->PrType;
    }
  return presentationType;
}

/*----------------------------------------------------------------------
  TtaGetPRuleValue

  Returns the value of a presentation rule.

  Parameters:
  pRule: the presentation rule of interest.

  Return values according to rule type:
  PRVisibility: an integer >= 0 (visibility level).
  PRDepth: an integer (z-index of the element).
  PRFillPattern: rank of the pattern in file thot.pattern.
  PRBackground: rank of the background color in file thot.color.
  PRForeground: rank of the foreground color in file thot.color.
  PRColor: rank of the color in file thot.color.
  PRStopColor: rank of the stop-color in file thot.color.
  PRBorderTopColor, PRBorderRightColor, PRBorderBottomColor,PRBorderLeftColor:
  if value >= 0: rank of the color in file thot.color.
  if value < 0 : -2 means transparent, -1 means same color as foreground, -3
                 means undefined
  PRFont: FontTimes, FontHelvetica, FontCourier.
  PRStyle: StyleRoman, StyleItalics, StyleOblique.
  PRWeight: WeightNormal, WeightBold.
  PRVariant: VariantNormal, VariantSmallCaps, VariantDoubleStruck,
             VariantFraktur, VariantScript.
  PRUnderline: NoUnderline, Underline, Overline, CrossOut.
  PRThickness: ThinUnderline, ThickUnderline.
  PRDirection: LeftToRight, RightToLeft.
  PRUnicodeBidi: Normal, Embed, Override.
  PRLineStyle, PRBorderTopStyle, PRBorderRightStyle, PRBorderBottomStyle,
  PRBorderLeftStyle: SolidLine, DashedLine, DottedLine.
  PRDisplay: Undefined, DisplayNone, Inline, Block, ListItem, RunIn,
  InlineBlock.
  PRListStyleType: Disc, Circle, Square, Decimal, DecimalLeadingZero,
  LowerRoman, UpperRoman, LowerGreek, UpperGreek, LowerLatin, UpperLatin,
  ListStyleTypeNone.
  PRListStylePosition: Inside, Outside.
  PRNoBreak1, PRNoBreak2: a positive integer (distance in points).
  PRIndent: a positive, null or negative integer (indentation in points).
  PRSize: an integer between 6 and 72 (body size in points).
  PRLineSpacing: a positive integer (line spacing in points).
  PRLineWeight: a positive or null integer (stroke width for graphics).
  PRMarginTop, PRMarginLeft, PRMarginBottom, PRMarginRight: a positive, null
  or negative integer (margin in points).
  PRPaddingTop, PRPaddingRight, PRPaddingBottom, PRPaddingLeft: an integer
  > 0 (padding in points).
  PRBorderTopWidth, PRBorderRightWidth, PRBorderBottomWidth, PRBorderLeftWidth:
  an integer > 0 (border width).
  PRXRadius, PRYRadius: radius
  PRTop, PRRight, PRBottom, PRLeft, PRBackgroundHorizPos, PRBackgroundVertPos: distance
  PtVertPos, PtHorizPos: distance
  PtWidth, PtHeight: distance
  PRHyphenate: Hyphenation, NoHyphenation.
  PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots, Justify.
  PROpacity, PRStrokeOpacity, PRFillOpacity, PRStopOpacity: an integer.
  PRFillRule: NonZero, EvenOdd.
  ----------------------------------------------------------------------*/
int TtaGetPRuleValue (PRule pRule)
{
  int                 value;

  UserErrorCode = 0;
  value = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    switch (((PtrPRule) pRule)->PrType)
      {
      case PtVisibility:
      case PtDepth:
      case PtFillPattern:
      case PtOpacity:
      case PtFillOpacity:
      case PtStrokeOpacity:
      case PtStopOpacity:
      case PtMarker:
      case PtMarkerStart:
      case PtMarkerMid:
      case PtMarkerEnd:
      case PtBackground:
      case PtForeground:
      case PtColor:
      case PtStopColor:
      case PtBorderTopColor:
      case PtBorderRightColor:
      case PtBorderBottomColor:
      case PtBorderLeftColor:
        value = ((PtrPRule) pRule)->PrIntValue;
        break;

      case PtVis:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'H':
            value = VsHidden;
            break;
          case 'V':
            value = VsVisible;
            break;
          case 'C':
            value = VsCollapse;
            break;
            break;
          case 'I':
            value = VsInherit;
            break;
          default:
            TtaError (ERR_invalid_parameter);
          }
        break;

      case PtFont:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'T':
            value = FontTimes;
            break;
          case 'H':
            value = FontHelvetica;
            break;
          case 'C':
            value = FontCourier;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;
      case PtStyle:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'R':
            value = StyleRoman;
            break;
          case 'I':
            value = StyleItalics;
            break;
          case 'O':
            value = StyleOblique;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;
      case PtWeight:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'N':
            value = WeightNormal;
            break;
          case 'B':
            value = WeightBold;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;
      case PtVariant:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'N':
            value = VariantNormal;
            break;
          case 'C':
            value = VariantSmallCaps;
            break;
          case 'D':
            value = VariantDoubleStruck;
            break;
          case 'F':
            value = VariantFraktur;
            break;
          case 'S':
            value = VariantScript;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;
      case PtUnderline:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'N':
            value = NoUnderline;
            break;
          case 'U':
            value = Underline;
            break;
          case 'O':
            value = Overline;
            break;
          case 'C':
            value = CrossOut;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;
      case PtThickness:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'N':
            value = ThinUnderline;
            break;
          case 'T':
            value = ThickUnderline;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;
      case PtDirection:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'L':
            value = LeftToRight;
            break;
          case 'R':
            value = RightToLeft;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;
      case PtUnicodeBidi:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'N':
            value = Normal;
            break;
          case 'E':
            value = Embed;
            break;
          case 'O':
            value = Override;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtLineStyle:
      case PtBorderTopStyle:
      case PtBorderRightStyle:
      case PtBorderBottomStyle:
      case PtBorderLeftStyle:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'S':
            value = SolidLine;
            break;
          case '-':
            value = DashedLine;
            break;
          case '.':
            value = DottedLine;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtDisplay:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'U':
            value = Undefined;
            break;
          case 'N':
            value = DisplayNone;
            break;
          case 'I':
            value = Inline;
            break;
          case 'B':
            value = Block;
            break;
          case 'L':
            value = ListItem;
            break;
          case 'R':
            value = RunIn;
            break;
          case 'b':
            value = InlineBlock;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtListStyleType:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'D':
            value = Disc;
            break;
          case 'C':
            value = Circle;
            break;
          case 'S':
            value = Square;
            break;
          case '1':
            value = Decimal;
            break;
          case 'Z':
            value = DecimalLeadingZero;
            break;
          case 'i':
            value = LowerRoman;
            break;
          case 'I':
            value = UpperRoman;
            break;
          case 'g':
            value = LowerGreek;
            break;
          case 'G':
            value = UpperGreek;
            break;
          case 'a':
            value = LowerLatin;
            break;
          case 'A':
            value = UpperLatin;
            break;
          case 'N':
            value = ListStyleTypeNone;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtListStylePosition:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'I':
            value = Inside;
            break;
          case 'O':
            value = Outside;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtFloat:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'L':
            value = FloatLeft;
            break;
          case 'R':
            value = FloatRight;
            break;
          case 'N':
            value = FloatNone;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtClear:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'L':
            value = ClearLeft;
            break;
          case 'R':
            value = ClearRight;
            break;
          case 'B':
            value = ClearBoth;
            break;
          case 'N':
            value = ClearNone;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtPosition:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'S':
            value = PnStatic;
            break;
          case 'R':
            value = PnRelative;
            break;
          case 'A':
            value = PnAbsolute;
            break;
          case 'F':
            value = PnFixed;
            break;
          case 'I':
            value = PnInherit;
            break;
          }
        break;

      case PtBreak1:
      case PtBreak2:
      case PtIndent:
      case PtSize:
      case PtLineSpacing:
      case PtLineWeight:
      case PtMarginTop:
      case PtMarginRight:
      case PtMarginBottom:
      case PtMarginLeft:
      case PtPaddingTop:
      case PtPaddingRight:
      case PtPaddingBottom:
      case PtPaddingLeft:
      case PtBorderTopWidth:
      case PtBorderRightWidth:
      case PtBorderBottomWidth:
      case PtBorderLeftWidth:
      case PtXRadius:
      case PtYRadius:
      case PtTop:
      case PtRight:
      case PtBottom:
      case PtLeft:
      case PtBackgroundHorizPos:
      case PtBackgroundVertPos:
        value = ((PtrPRule) pRule)->PrMinValue;
        break;

      case PtVertPos:
      case PtHorizPos:
        value = ((PtrPRule) pRule)->PrPosRule.PoDistance;
        break;

      case PtWidth:
      case PtHeight:
        if (((PtrPRule) pRule)->PrDimRule.DrPosition)
          value = ((PtrPRule) pRule)->PrDimRule.DrPosRule.PoDistance;
        else
          value = ((PtrPRule) pRule)->PrDimRule.DrValue;
        break;

      case PtHyphenate:
        if (((PtrPRule) pRule)->PrBoolValue)
          value = Hyphenation;
        else
          value = NoHyphenation;
        break;

      case PtAdjust:
        switch (((PtrPRule) pRule)->PrAdjust)
          {
          case AlignLeft:
            value = AdjustLeft;
            break;
          case AlignRight:
            value = AdjustRight;
            break;
          case AlignCenter:
            value = Centered;
            break;
          case AlignJustify:
            value = Justify;
            break;
          case AlignLeftDots:
            value = LeftWithDots;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      case PtFillRule:
        switch (((PtrPRule) pRule)->PrChrValue)
          {
          case 'n':
            value = NonZero;
            break;
          case 'e':
            value = EvenOdd;
            break;
          default:
            TtaError (ERR_invalid_parameter);
            break;
          }
        break;

      default:
        TtaError (ERR_invalid_parameter);
        break;
      }
  return value;
}

/*----------------------------------------------------------------------
  TtaGetPositionPRuleDelta

  Returns the delta value of a position rule.

  Parameters:
  pRule: the presentation rule of interest.

  Return:
  delta value.
  ----------------------------------------------------------------------*/
int TtaGetPositionPRuleDelta (PRule pRule)
{
  int                 value;

  UserErrorCode = 0;
  value = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    if (((PtrPRule) pRule)->PrType != PtVertPos &&
        ((PtrPRule) pRule)->PrType != PtHorizPos)
      TtaError (ERR_invalid_parameter);
    else
      value = ((PtrPRule) pRule)->PrPosRule.PoDistDelta;
  return value;
}

/*----------------------------------------------------------------------
  TtaGetPRuleUnit

  Returns the unit of a presentation rule.
  Parameters:
  pRule: the presentation rule of interest.
  Return the unit of the rule type PRNoBreak1, PRNoBreak2,
  PRIndent, PRSize, PRLineSpacing, PRLineWeight,
  PRMarginTop, PRMarginRight, PRMarginBottom, PRMarginLeft,
  PRPaddingTop, PRPaddingRight, PRPaddingBottom, PRPaddingLeft,
  PRBorderTopWidth, PRBorderRightWidth, PRBorderBottomWidth,
  PRBorderLeftWidth,
  PRXRadius, PRYRadius,
  PRTop, PRRight, PRBottom, PRLeft, PRBackgroundHorizPos, PRBackgroundVertPos,
  PRVertPos, PRHorizPos, PRWidth, PRHeight.
  This unit could be UnRelative, UnXHeight, UnPoint, UnPixel, UnPercent.
  Return UnRelative in other cases.
  ----------------------------------------------------------------------*/
int TtaGetPRuleUnit (PRule pRule)
{
  int                 value;

  UserErrorCode = 0;
  value = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    switch (((PtrPRule) pRule)->PrType)
      {
      case PtBreak1:
      case PtBreak2:
      case PtIndent:
      case PtSize:
      case PtLineSpacing:
      case PtLineWeight:
      case PtMarginTop:
      case PtMarginRight:
      case PtMarginBottom:
      case PtMarginLeft:
      case PtPaddingTop:
      case PtPaddingRight:
      case PtPaddingBottom:
      case PtPaddingLeft:
      case PtBorderTopWidth:
      case PtBorderRightWidth:
      case PtBorderBottomWidth:
      case PtBorderLeftWidth:
      case PtXRadius:
      case PtYRadius:
      case PtTop:
      case PtRight:
      case PtBottom:
      case PtLeft:
      case PtBackgroundHorizPos:
      case PtBackgroundVertPos:
        value = ((PtrPRule) pRule)->PrMinUnit;
        break;
      case PtHeight:
        if (((PtrPRule) pRule)->PrDimRule.DrPosition)
          value = ((PtrPRule) pRule)->PrDimRule.DrPosRule.PoDistUnit;
        else
          value = ((PtrPRule) pRule)->PrDimRule.DrUnit;
        break;
      case PtVertPos:
      case PtHorizPos:
        value = ((PtrPRule) pRule)->PrPosRule.PoDistUnit;
        break;
      default:
        TtaError (ERR_invalid_parameter);
        break;
      }
  return value;
}

/*----------------------------------------------------------------------
  TtaGetPRuleView

  Returns the number of the view to which a presentation rule applies.
  Parameters:
  pRule: the presentation rule of interest.
  Return value:
  number of the view to which the presentation rule applies.
  ----------------------------------------------------------------------*/
int TtaGetPRuleView (PRule pRule)
{
  int                 view;

  UserErrorCode = 0;
  view = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    view = ((PtrPRule) pRule)->PrViewNum;
  return view;
}

/*----------------------------------------------------------------------
  TtaSamePRules

  Compares two presentation rules associated with the same element or with
  different elements.
  Parameters:
  pRule1: first presentation rule to be compared.
  pRule2: second presentation rule to be compared.
  Return value:
  0 if both rules are different, 1 if they are identical.
  ----------------------------------------------------------------------*/
int TtaSamePRules (PRule pRule1, PRule pRule2)
{
  int                 result;
  PtrPRule            pR1, pR2;

  UserErrorCode = 0;
  result = 0;
  if (pRule1 == NULL || pRule2 == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      pR1 = (PtrPRule) pRule1;
      pR2 = (PtrPRule) pRule2;
      if (pR1->PrType == pR2->PrType)
        if (pR1->PrViewNum == pR2->PrViewNum)
          if (pR1->PrSpecifAttr == pR2->PrSpecifAttr)
            if (pR1->PrSpecifAttrSSchema == pR2->PrSpecifAttrSSchema)
              if (pR1->PrPresMode == pR2->PrPresMode)
                if (pR1->PrPresMode == PresImmediate)
                  switch (pR1->PrType)
                    {
                    case PtVisibility:
                    case PtDepth:
                    case PtListStyleImage:
                    case PtFillPattern:
                    case PtOpacity:
                    case PtStrokeOpacity:
                    case PtFillOpacity:
                    case PtStopOpacity:
		    case PtMarker:
		    case PtMarkerStart:
		    case PtMarkerMid:
		    case PtMarkerEnd:
                    case PtBackground:
                    case PtForeground:
                    case PtColor:
                    case PtStopColor:
                    case PtBorderTopColor:
                    case PtBorderRightColor:
                    case PtBorderBottomColor:
                    case PtBorderLeftColor:
                      if (pR1->PrValueType == pR2->PrValueType)
                        if (pR1->PrIntValue == pR2->PrIntValue)
                          result = 1;
                      break;
                    case PtVis:
                    case PtFont:
                    case PtStyle:
                    case PtWeight:
                    case PtVariant:
                    case PtUnderline:
                    case PtThickness:
                    case PtDirection:
                    case PtUnicodeBidi:
                    case PtLineStyle:
                    case PtDisplay:
                    case PtListStyleType:
                    case PtListStylePosition:
                    case PtBorderTopStyle:
                    case PtBorderRightStyle:
                    case PtBorderBottomStyle:
                    case PtBorderLeftStyle:
		    case PtFillRule:
                      if (pR1->PrChrValue == pR2->PrChrValue)
                        result = 1;
                      break;
                    case PtBreak1:
                    case PtBreak2:
                    case PtIndent:
                    case PtSize:
                    case PtLineSpacing:
                    case PtLineWeight:
                    case PtMarginTop:
                    case PtMarginRight:
                    case PtMarginBottom:
                    case PtMarginLeft:
                    case PtPaddingTop:
                    case PtPaddingRight:
                    case PtPaddingBottom:
                    case PtPaddingLeft:
                    case PtBorderTopWidth:
                    case PtBorderRightWidth:
                    case PtBorderBottomWidth:
                    case PtBorderLeftWidth:
                    case PtXRadius:
                    case PtYRadius:
                    case PtTop:
                    case PtRight:
                    case PtBottom:
                    case PtLeft:
                    case PtBackgroundHorizPos:
                    case PtBackgroundVertPos:
                      if (pR1->PrMinUnit == pR2->PrMinUnit)
                        if (pR1->PrMinAttr == pR2->PrMinAttr)
                          if (pR1->PrMinValue == pR2->PrMinValue)
                            result = 1;
                      break;
                    case PtVertRef:
                    case PtHorizRef:
                    case PtVertPos:
                    case PtHorizPos:
                      if (pR1->PrPosRule.PoDistUnit ==
                          pR2->PrPosRule.PoDistUnit)
                        if (pR1->PrPosRule.PoDistance ==
                            pR2->PrPosRule.PoDistance)
                          if (pR1->PrPosRule.PoDistDelta ==
                              pR2->PrPosRule.PoDistDelta)
                            if (pR1->PrPosRule.PoDeltaUnit ==
                                pR2->PrPosRule.PoDeltaUnit)
                              result = 1;
                      break;
                    case PtHeight:
                    case PtWidth:
                      if (pR1->PrDimRule.DrUnit ==
                          pR2->PrDimRule.DrUnit)
                        if (pR1->PrDimRule.DrValue ==
                            pR2->PrDimRule.DrValue)
                          result = 1;
                      break;
                    case PtHyphenate:
                      if (pR1->PrBoolValue == pR2->PrBoolValue)
                        result = 1;
                      break;
                    case PtAdjust:
                      if (pR1->PrAdjust == pR2->PrAdjust)
                        result = 1;
                      break;
                    default:
                      break;
                    }
    }
  return result;
}
