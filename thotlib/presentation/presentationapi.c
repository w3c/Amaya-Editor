/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
 *
 */
 
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "presentation.h"
#include "tree.h"
#include "typecorr.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"

#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
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
#include "structmodif_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "viewapi_f.h"


/*----------------------------------------------------------------------
   	PRuleMessagePre	On veut ajouter ou modifier (selon isNew) la	
   	regle de presentation specifique pPRule a l'element pEl du	
   	document pDoc. On envoie le message APP correspondant a		
   	l'application et on retourne la reponse de l'application.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool        PRuleMessagePre (PtrElement pEl, PtrPRule pPRule, PtrDocument pDoc, ThotBool isNew)
#else  /* __STDC__ */
ThotBool        PRuleMessagePre (pEl, pPRule, pDoc, isNew)
PtrElement      pEl;
PtrPRule        pPRule;
PtrDocument     pDoc;
ThotBool        isNew;

#endif /* __STDC__ */
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
void           PRuleMessagePost (PtrElement pEl, PtrPRule pPRule, PtrDocument pDoc, ThotBool isNew)

#else  /* __STDC__ */
void           PRuleMessagePost (pEl, pPRule, pDoc, isNew)
PtrElement     pEl;
PtrPRule       pPRule;
PtrDocument    pDoc;
ThotBool       isNew;

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

/*----------------------------------------------------------------------
  NewPosition is called when the user moves a box.
  pAb is the abstract box	
  X and Y give the new positions in pixels.
  frame is the frame.				
  display is TRUE when it's necessary to redisplay the concrete image.
----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewPosition (PtrAbstractBox pAb, int X, int Y, int frame, ThotBool display)
#else  /* __STDC__ */
void                NewPosition (pAb, X, Y, frame, display)
PtrAbstractBox      pAb;
int                 X;
int                 Y;
int                 frame;
ThotBool            display;

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
   int                 x, y, dx, dy;
   int                 updateframe[MAX_VIEW_DOC];
   int                 viewSch;
   int                 view;
   int                 value;
   ThotBool            attr, stop, doit;
   ThotBool            isNew, reDisp, isLined, histOpen;

   /* nettoie la table des frames a reafficher */
   for (view = 1; view <= MAX_VIEW_DOC; view++)
      updateframe[view - 1] = 0;
   /* l'element auquel correspond le pave */
   pEl = pAb->AbElement;
   /* le document auquel il appartient */
   pDoc = DocumentOfElement (pEl);
   /* numero de cette view */
   viewSch = AppliedView (pEl, NULL, pDoc, pAb->AbDocView);
   /* rien a reafficher */
   reDisp = FALSE;
   histOpen = FALSE;
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
   doc = IdentDocument (pDoc);  
   /* traite la position verticale */
   if (pAb->AbBox != NULL && Y != pAb->AbBox->BxYOrg)
     {
       /* cherche d'abord la regle de position qui s'applique a l'element */
       pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtVertPos, FnAny, FALSE, TRUE, &pAttr);
       /* on ne decale pas les paves qui ont une position flottante ou qui */
       /* sont dans une mise en ligne */
       if (pRStd->PrPosRule.PoPosDef != NoEdge
	   && pAb->AbVertPos.PosAbRef != NULL
	   && !isLined)
	 {
	   dy = Y - pAb->AbBox->BxYOrg;
	   if (pRStd->PrPosRule.PoDistUnit == UnPercent)
	     {
	       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		 GetSizesFrame (frame, &x, &y);
	       else
		 y = pAb->AbEnclosing->AbBox->BxHeight;
	       dy = LogicalValue (dy, UnPercent, (PtrAbstractBox) y, 0);
	     }
	   else
	     {
	       dy = LogicalValue (dy, pRStd->PrPosRule.PoDistUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	     }

	   /* cherche si la position verticale de l'element est determinee */
	   /* par un attribut auquel est associee l'exception NewVPos */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewVPos, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle position sera rangee dans l'attribut */
	       attr = TRUE;
	   doit = TRUE;
	   if (!BoxCreating && !histOpen)
	     {
		OpenHistorySequence (pDoc, pEl, pEl, 0, 0);
		histOpen = TRUE;
	     }
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
		       if (!BoxCreating)
		         AddAttrEditOpInHistory (pAttr, pEl, pDoc, TRUE, TRUE);
		       pAttr->AeAttrValue += dy;
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
	       else
		 dy += pPRule->PrPosRule.PoDistance;
	       /* modify the distance in the specific rule */
	       value = pPRule->PrPosRule.PoDistance;
	       pPRule->PrPosRule.PoDistance = dy;
	       /* envoie un message APP a l'application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 /* reset previous values */
		 pPRule->PrPosRule.PoDistance = value;
	     }
	   
	   if (doit)
	     {
	       /* le document est modifie' */
	       SetDocumentModified (pDoc, TRUE, 0);
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
			 {
			   /* applique la nouvelle regle de position verticale */
			   ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr);
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
		     }
	       if (attr)
		 CallEventAttribute (&notifyAttr, FALSE);
	       else
		 PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	     }
	 }
     }

   /* traite la position horizontale */
   if (pAb->AbBox != NULL && X != pAb->AbBox->BxXOrg)
     /* cherche d'abord la regle de position qui s'applique a l'element */
     {
       pRStd = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch, PtHorizPos, FnAny, FALSE, TRUE, &pAttr);
       /* on ne decale pas les paves qui ont une position flottante ou qui */
       /* sont mis en lignes */
       if (pRStd->PrPosRule.PoPosDef != NoEdge
	   && pAb->AbHorizPos.PosAbRef != NULL
	   && !isLined)
	 {
	   dx = X - pAb->AbBox->BxXOrg;
	   if (pRStd->PrPosRule.PoDistUnit == UnPercent)
	     {
	       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		 GetSizesFrame (frame, &x, &y);
	       else
		 x = pAb->AbEnclosing->AbBox->BxWidth;
	       dx = LogicalValue (dx, UnPercent, (PtrAbstractBox) x, 0);
	     }
	   else
	     {
	       dx = LogicalValue (dx, pRStd->PrPosRule.PoDistUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	     }

	   /* cherche si la position horizontale de l'element est determinee */
	   /* par un attribut auquel est associee l'exception NewHPos */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewHPos, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle position sera rangee dans l'attribut */
	       attr = TRUE;
	   doit = TRUE;
	   if (!BoxCreating && !histOpen)
	     {
	       OpenHistorySequence (pDoc, pEl, pEl, 0, 0);
	       histOpen = TRUE;
	     }
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
		       if (!BoxCreating)
		         AddAttrEditOpInHistory (pAttr, pEl, pDoc, TRUE, TRUE);
		       pAttr->AeAttrValue += dx;
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
	       else
		 dx += pPRule->PrPosRule.PoDistance;
	       /* modify the distance in the specific rule */
	       value = pPRule->PrPosRule.PoDistance;
	       pPRule->PrPosRule.PoDistance = dx;
	       
	       /* envoie un message APP a l'application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 /* reset previous values */
		 pPRule->PrPosRule.PoDistance = value;
	     }
	   if (doit)
	     {
	       /* le document est modifie' */
	       SetDocumentModified (pDoc, TRUE, 0);
	       for (view = 1; view <= MAX_VIEW_DOC; view++)
		 if (pEl->ElAbstractBox[view - 1] != NULL)
		   /* l'element traite' a un pave dans cette view */
		   if ((pDoc->DocView[view - 1].DvSSchema == pDoc->DocView[pAb->AbDocView - 1].DvSSchema)
		       && (pDoc->DocView[view - 1].DvPSchemaView == pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView))
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
			 {
			   /* applique la nouvelle regle de position verticale */
			   ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr);
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
		     }
	       if (attr)
		 CallEventAttribute (&notifyAttr, FALSE);
	       else
		 PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	     }
	 }
     }

   if (!BoxCreating && histOpen)
     {
     CloseHistorySequence (pDoc);
     histOpen = FALSE;
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

/*----------------------------------------------------------------------
  NewDimension is called when the user resizes a box.
  pAb is the abstract box	
  width and height give the new dimensions in pixels.
  frame is the frame.				
  display is TRUE when it's necessary to redisplay the concrete image.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewDimension (PtrAbstractBox pAb, int width, int height, int frame, ThotBool display)
#else  /* __STDC__ */
void                NewDimension (pAb, width, height, frame, display)
PtrAbstractBox      pAb;
int                 width;
int                 height;
int                 frame;
ThotBool            display;
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
   int                 dx, dy;
   int                 heightRef, widthRef;
   int                 updateframe[MAX_VIEW_DOC];
   int                 viewSch;
   int                 view;
   int                 value;
   ThotBool            attr, stop, doit;
   ThotBool            isNew, reDisp, ok, histOpen;

   /* nettoie la table des frames a reafficher */
   for (view = 0; view < MAX_VIEW_DOC; view++)
     updateframe[view] = 0;
   /* l'element auquel correspond le pave */
   pEl = pAb->AbElement;
   /* le document auquel appartient le pave */
   pDoc = DocumentOfElement (pEl);
   doc = (Document) IdentDocument (pDoc);
   /* rien a reafficher */
   reDisp = FALSE;
   histOpen = FALSE;
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
	   dx = width - pAb->AbBox->BxWidth;
	   if (pRStd->PrDimRule.DrUnit == UnPercent)
	     {
	       if (!pRStd->PrDimRule.DrAbsolute)
		 /* the width is a percent of an other box width */
		 widthRef = pAb->AbWidth.DimAbRef->AbBox->BxWidth;
	       else if (pAb->AbEnclosing == NULL)
		 /* the width is a percent of the window width */
		 GetSizesFrame (frame, &widthRef, &heightRef);
	       else
		 /* the width is a percent of the parent box width */
		 widthRef = pAb->AbEnclosing->AbBox->BxWidth;

	       /* get the new percent value */
	       dx = LogicalValue (dx, UnPercent, (PtrAbstractBox) widthRef, 0);
	     }
	   else
	     {
	       /* convert the new height in logical value */
	       dx = LogicalValue (dx, pAb->AbWidth.DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	     }
	   
	   /* cherche si la largeur de l'element est determinee par un */
	   /* attribut auquel est associee l'exception NewWidth */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewWidth, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle largeur sera rangee dans l'attribut */
	       attr = TRUE;
	   
	   doit = TRUE;
	   if (!BoxCreating && !histOpen)
	     {
	       OpenHistorySequence (pDoc, pEl, pEl, 0, 0);
	       histOpen = TRUE;
	     }
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
		       if (!BoxCreating)
		         AddAttrEditOpInHistory (pAttr, pEl, pDoc, TRUE, TRUE);

		       pAttr->AeAttrValue += dx;
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
		 /* create a new rule for the element */
		 {
		   pR = pPRule->PrNextPRule;
		   /* copy the standard rule */
		   *pPRule = *pRStd;
		   pPRule->PrCond = NULL;
		   pPRule->PrNextPRule = pR;
		   pPRule->PrViewNum = viewSch;
		   /* if the rule is associated to an attribute */
		   /* keep the link to that attribute */
		   if (pAttr != NULL)
		     {
		       pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		       pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		     }
		   value = pPRule->PrDimRule.DrValue;
		 }
	       else if (pAb->AbLeafType == LtPicture &&
			pAb->AbBox->BxWidth != 0 &&
			pPRule->PrDimRule.DrValue == 0)
		 {
		   /* specific case for pictures:
		      the rule is NULL but the width is not */
		   if (pRStd->PrDimRule.DrUnit == UnPercent)
		     value = LogicalValue (width, UnPercent, (PtrAbstractBox) widthRef, 0);
		   else
		     value = LogicalValue (width, pAb->AbWidth.DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		 }
	       else
		 {
		   dx += pPRule->PrDimRule.DrValue;
		   value = pPRule->PrDimRule.DrValue;
		 }
	       /* set the absolute value into the rule */
	       pPRule->PrDimRule.DrValue = dx;
	       /* send the event message to the application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 /* reset the previous value */
		 pPRule->PrDimRule.DrValue = value;
	     }

	   if (doit)
	     {
	       /* the document is modified */
	       SetDocumentModified (pDoc, TRUE, 0);
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
			 if (ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr))
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
	   dy = height - pAb->AbBox->BxHeight;
	   if (pRStd->PrDimRule.DrUnit == UnPercent)
	     {
	       if (!pRStd->PrDimRule.DrAbsolute)
		 /* the height is a percent of an other box height */
		 heightRef = pAb->AbWidth.DimAbRef->AbBox->BxHeight;
	       else if (pAb->AbEnclosing == NULL)
		 /* the height is a percent of the window height */
		 GetSizesFrame (frame, &widthRef, &heightRef);
	       else
		 /* the height is a percent of the parent box height */
		 heightRef = pAb->AbEnclosing->AbBox->BxHeight;

	       /* get the new percent value */
	       dy = LogicalValue (dy, UnPercent, (PtrAbstractBox) heightRef, 0);
	     }
	   else
	     {
	       /* convert the new height in logical value */
	       dy = LogicalValue (dy, pAb->AbHeight.DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	     }
	   
	   /* cherche si la hauteur de l'element est determinee par un */
	   /* attribut auquel est associee l'exception NewHeight */
	   attr = FALSE;
	   if (pAttr != NULL)
	     if (AttrHasException (ExcNewHeight, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	       /* la nouvelle hauteur sera rangee dans l'attribut */
	       attr = TRUE;
	   doit = TRUE;
	   if (!BoxCreating && !histOpen)
	     {
	       OpenHistorySequence (pDoc, pEl, pEl, 0, 0);
	       histOpen = TRUE;
	     }
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
		       if (!BoxCreating)
		         AddAttrEditOpInHistory (pAttr, pEl, pDoc, TRUE, TRUE);
		       pAttr->AeAttrValue += dy;
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
		 /* create a new rule for the element */
		 {
		   pR = pPRule->PrNextPRule;
		   /* copy the standard rule */
		   *pPRule = *pRStd;
		   pPRule->PrCond = NULL;
		   pPRule->PrNextPRule = pR;
		   pPRule->PrViewNum = viewSch;
		   /* if the rule is associated to an attribute */
		   /* keep the link to that attribute */
		   if (pAttr != NULL)
		     {
		       pPRule->PrSpecifAttr = pAttr->AeAttrNum;
		       pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
		     }
		   value = pPRule->PrDimRule.DrValue;
		 }
	       else if (pAb->AbLeafType == LtPicture &&
			pAb->AbBox->BxHeight != 0 &&
			pPRule->PrDimRule.DrValue == 0)
		 {
		   /* specific case for pictures:
		      the rule is NULL but the height is not */
		   if (pRStd->PrDimRule.DrUnit == UnPercent)
		     value = LogicalValue (height, UnPercent, (PtrAbstractBox) heightRef, 0);
		   else
		     value = LogicalValue (height, pAb->AbHeight.DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		 }
	       else
		 {
		   dy += pPRule->PrDimRule.DrValue;
		   value = pPRule->PrDimRule.DrValue;
		 }

	       /* set the absolute value into the rule */
	       pPRule->PrDimRule.DrValue = dy;
	       /* send the event message to the application */
	       doit = !PRuleMessagePre (pEl, pPRule, pDoc, isNew);
	       if (!doit && !isNew)
		 /* reset the previous value */
		 pPRule->PrDimRule.DrValue = value;
	     }
	   
	   if (doit)
	     {
	       /* the document is modified */
	       SetDocumentModified (pDoc, TRUE, 0);
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
			 if (ApplyRule (pPRule, pSPR, pAbbCur, pDoc, pAttr))
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

   if (!BoxCreating && histOpen)
     {
     CloseHistorySequence (pDoc);
     histOpen = FALSE;
     }

   if (reDisp || oldDisplayMode == DisplayImmediately)
     {
       TtaSetDisplayMode (doc, DisplayImmediately);
       SwitchSelection (frame, TRUE);
       
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
   TtaNewPRule

   Creates a new presentation rule of a given type for a given view of a given
   document.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRWidth, PRHeight, PRVertPos, PRHorizPos.
   view: the view (this view must be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaNewPRule (int presentationType, View view, Document document)
#else  /* __STDC__ */
PRule               TtaNewPRule (presentationType, view, document)
int                 presentationType;
View                view;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres;
   int                 v;

   UserErrorCode = 0;
   pPres = NULL;
   if (presentationType < 0 || presentationType > PRHyphenate)
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
       v = 0;
       if (view < 100)
	 /* View of the main tree */
	 if (view < 1 || view > MAX_VIEW_DOC)
	   TtaError (ERR_invalid_parameter);
	 else if (LoadedDocument[document - 1]->DocView[view - 1].DvPSchemaView == 0)
	   /* this view is not open */
	   TtaError (ERR_invalid_parameter);
	 else
	   v = LoadedDocument[document - 1]->DocView[view - 1].DvPSchemaView;
       else
	 /* View of associated elements */
	 if (view - 100 < 1 || view - 100 > MAX_ASSOC_DOC)
	   TtaError (ERR_invalid_parameter);
	 else if (LoadedDocument[document - 1]->DocAssocFrame[view - 101] == 0)
	   /* this view is not open */
	   TtaError (ERR_invalid_parameter);
	 else
	   v = 1;
       if (v > 0)
	 {
	   GetPresentRule (&pPres);
	   pPres->PrType = (PRuleType) presentationType;
	   pPres->PrNextPRule = NULL;
	   pPres->PrViewNum = v;
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
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRShowBox, PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaNewPRuleForView (int presentationType, int view, Document document)
#else  /* __STDC__ */
PRule               TtaNewPRuleForView (presentationType, view, document)
int                 presentationType;
int                 view;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres;

   UserErrorCode = 0;
   pPres = NULL;
   if (presentationType != PRNotInLine &&
       presentationType != PRShowBox &&
       (presentationType < 0 || presentationType > PRHyphenate))
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
       if (presentationType == PRShowBox)
	 {
	   pPres->PrType = PtFunction;
	   pPres->PrPresFunction = FnShowBox;
	 }
       else if (presentationType == PRNotInLine)
	 {
	   pPres->PrType = PtFunction;
	   pPres->PrPresFunction = FnNotInLine;
	 }
       else
	 pPres->PrType = (PRuleType) presentationType;
     }
   return ((PRule) pPres);
}

/*----------------------------------------------------------------------
   TtaNewPRuleForNamedView

   Creates a new presentation rule of a given type for a given view of a given
   document. The view is identified by its name.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRShowBox, PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaNewPRuleForNamedView (int presentationType, STRING viewName, Document document)
#else  /* __STDC__ */
PRule               TtaNewPRuleForNamedView (presentationType, viewName, document)
int                 presentationType;
STRING              viewName;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres;
   PtrDocument         pDoc;
   PtrElement          pEl;
   int                 vue;
   int                 v;

   UserErrorCode = 0;
   pPres = NULL;
   if (presentationType != PRShowBox &&
       presentationType != PRNotInLine &&
       (presentationType < 0 || presentationType > PRHyphenate))
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
       pDoc = LoadedDocument[document - 1];
       vue = 0;
       /* Searching into the main tree views */
       if (pDoc->DocSSchema->SsPSchema != NULL)
	 for (v = 1; v <= MAX_VIEW && vue == 0; v++)
	   if (ustrcmp (pDoc->DocSSchema->SsPSchema->PsView[v - 1], viewName) == 0)
	     vue = v;
       /* If not found one search into associated elements */
       if (vue == 0)
	 for (v = 1; v <= MAX_ASSOC_DOC && vue == 0; v++)
	   {
	     pEl = pDoc->DocAssocRoot[v - 1];
	     if (pEl != NULL)
	       if (ustrcmp (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
		 vue = 1;
	   }
       if (vue == 0)
	 TtaError (ERR_invalid_parameter);
       else
	 {
	   GetPresentRule (&pPres);
	   pPres->PrNextPRule = NULL;
	   pPres->PrViewNum = vue;
	   if (presentationType == PRShowBox)
	     {
	       pPres->PrType = PtFunction;
	       pPres->PrPresFunction = FnShowBox;
	     }
	   else	if (presentationType == PRNotInLine)
	     {
	       pPres->PrType = PtFunction;
	       pPres->PrPresFunction = FnNotInLine;
	     }
	   else
	     pPres->PrType = (PRuleType) presentationType;
	 }
     }
   return ((PRule) pPres);
}

/*----------------------------------------------------------------------
   TtaCopyPRule

   Creates a new presentation rule and initializes it with a copy of an existing
   presentation rule.

   Parameter:
   pRule: presentation rule to be copied.

   Return value:
   new presentation rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaCopyPRule (PRule pRule)
#else  /* __STDC__ */
PRule               TtaCopyPRule (pRule)
PRule               pRule;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaAttachPRule (Element element, PRule pRule, Document document)
#else  /* __STDC__ */
void                TtaAttachPRule (element, pRule, document)
Element             element;
PRule               pRule;
Document            document;
#endif /* __STDC__ */
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
   PRSize: an integer between 6 and 72 (body size in points).
   PRStyle: StyleRoman, StyleItalics, StyleOblique.
   PRWeight: WeightNormal, WeightBold.
   PRFont: FontTimes, FontHelvetica, FontCourier.
   PRUnderline: NoUnderline, Underline, Overline, CrossOut.
   PRThickness: ThinUnderline, ThickUnderline.
   PRIndent: a positive, null or negative integer (indentation in points).
   PRLineSpacing: a positive integer (line spacing in points).
   PRDepth: a positive integer (depth of the element).
   PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots.
   PRJustify: Justified, NotJustified.
   PRHyphenate: Hyphenation, NoHyphenation.
   PRLineStyle: SolidLine, DashedLine, DottedLine.
   PRLineWeight: a positive or null integer (stroke width for graphics).
   PRFillPattern: rank of the pattern in the file thot.pattern.
   PRBackground: rank of the background color in the file thot.color.
   PRForeground: rank of the foreground color in the file thot.color.
   PRWidth, PRHeight, PRVertPos, PRHorizPos: a positive or null integer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPRuleValue (Element element, PRule pRule, int value, Document document)
#else  /* __STDC__ */
void                TtaSetPRuleValue (element, pRule, value, document)
Element             element;
PRule               pRule;
int                 value;
Document            document;
#endif /* __STDC__ */
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
	 case PtSize:
	   /* Body-size in typographic points */
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
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
	 case PtIndent:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
	   break;
	 case PtLineSpacing:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
	   break;
	 case PtDepth:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrIntValue = value;
	   ((PtrPRule) pRule)->PrAttrValue = FALSE;
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
	 case PtJustify:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case Justified:
	       ((PtrPRule) pRule)->PrJustify = TRUE;
	       break;
	     case NotJustified:
	       ((PtrPRule) pRule)->PrJustify = FALSE;
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtHyphenate:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case Hyphenation:
	       ((PtrPRule) pRule)->PrJustify = TRUE;
	       break;
	     case NoHyphenation:
	       ((PtrPRule) pRule)->PrJustify = FALSE;
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
	 case PtLineWeight:
	   /* value = thickness of the line in typo points. */
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
	   break;
	 case PtFillPattern:
	   if (value < 0)
	     TtaError (ERR_invalid_parameter);
	   else
	     {
	       ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	       ((PtrPRule) pRule)->PrIntValue = value;
	       ((PtrPRule) pRule)->PrAttrValue = FALSE;
	     }
	   break;
	 case PtBackground:
	 case PtForeground:
	   if (value < 0)
	     TtaError (ERR_invalid_parameter);
	   else
	     {
	       ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	       ((PtrPRule) pRule)->PrIntValue = value;
	       ((PtrPRule) pRule)->PrAttrValue = FALSE;
	     }
	   break;
	 case PtFunction:
	   if (((PtrPRule) pRule)->PrPresFunction == FnBackgroundPicture)
	     ((PtrPRule) pRule)->PrPresBox[0] = value;
	   else if (((PtrPRule) pRule)->PrPresFunction == FnPictureMode)
	     ((PtrPRule) pRule)->PrPresBox[0] = value;
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
   TtaSetPRuleView

   Sets the view to which a presentation rule applies. The presentation rule
   must not be attached yet to an element.

   Parameters:
   pRule: the presentation rule to be changed.
   view: the value to be set.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPRuleView (PRule pRule, int view)
#else  /* __STDC__ */
void                TtaSetPRuleView (pRule, view)
PRule               pRule;
int                 view;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaChangeBoxSize (Element element, Document document, View view, int deltaX, int deltaY, TypeUnit unit)
#else  /* __STDC__ */
void                TtaChangeBoxSize (element, document, view, deltaX, deltaY, unit)
Element             element;
View                view;
int                 deltaX;
int                 deltaY;
Document            document;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 v, frame;
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
	     if (view < 100)
		/* View of the main tree */
		v = view;
	     else
		/* View of associated elements */
		v = 1;
	     pAb = AbsBoxOfEl ((PtrElement) element, v);
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
			    x = pAb->AbEnclosing->AbBox->BxWidth;
			    y = pAb->AbEnclosing->AbBox->BxHeight;
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
#ifdef __STDC__
void                TtaChangeBoxPosition (Element element, Document document, View view, int deltaX, int deltaY, TypeUnit unit)
#else  /* __STDC__ */
void                TtaChangeBoxPosition (element, document, view, deltaX, deltaY, unit)
Element             element;
Document            document;
View                view;
int                 deltaX;
int                 deltaY;
TypeUnit            unit;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 v, frame;
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
	     if (view < 100)
		/* View of the main tree */
		v = view;
	     else
		/* View of associated elements */
		v = 1;
	     pAb = AbsBoxOfEl ((PtrElement) element, v);
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
			    x = pAb->AbEnclosing->AbBox->BxWidth;
			    y = pAb->AbEnclosing->AbBox->BxHeight;
			 }
		       deltaX = PixelValue (deltaX, UnPercent, (PtrAbstractBox) x, 0);
		       deltaY = PixelValue (deltaY, UnPercent, (PtrAbstractBox) y, 0);
		    }
		  else
		    {
		       deltaX = PixelValue (deltaX, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		       deltaY = PixelValue (deltaY, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		    }
		  NewPosition (pAb, deltaX + pAb->AbBox->BxXOrg, deltaY + pAb->AbBox->BxYOrg, frame, FALSE);
		  RedispNewGeometry (document, (PtrElement) element);
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
#ifdef __STDC__
void                TtaGiveBoxSize (Element element, Document document, View view, TypeUnit unit, int *width, int *height)
#else  /* __STDC__ */
void                TtaGiveBoxSize (element, document, view, unit, width, height)
Element             element;
Document            document;
View                view;
TypeUnit            unit;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 v, frame;
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
	     if (view < 100)
		/* View of the main tree */
		v = view;
	     else
		/* View of associated elements */
		v = 1;
	     pAb = AbsBoxOfEl ((PtrElement) element, v);
	     if (pAb == NULL)
		TtaError (ERR_element_has_no_box);
	     else
	       {
		  *width = pAb->AbBox->BxWidth;
		  *height = pAb->AbBox->BxHeight;

		  /* Convert values to pixels */
		  if (unit == UnPercent)
		    {
		       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
			  GetSizesFrame (frame, &x, &y);
		       else
			 {
			    x = pAb->AbEnclosing->AbBox->BxWidth;
			    y = pAb->AbEnclosing->AbBox->BxHeight;
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
   unit: the unit used for the values.

   Return parameters:
   xCoord: distance from the left edge of the parent box to the left
   edge of the box, in points.
   yCoord:  distance from the upper edge of the parent box to the upper
   edge of the box, in points.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveBoxPosition (Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord)
#else  /* __STDC__ */
void                TtaGiveBoxPosition (element, Document document, view, unit, xCoord, yCoord)
Element             element;
Document            document;
View                view;
TypeUnit            unit;
int                *xCoord;
int                *yCoord;

#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  int                 v, frame;
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
	  if (view < 100)
	    /* View of the main tree */
	    v = view;
	  else
	    /* View of associated elements */
	    v = 1;
	  pAb = AbsBoxOfEl ((PtrElement) element, v);
	  if (pAb == NULL)
	    TtaError (ERR_element_has_no_box);
	  else
	    {
	      pBox = pAb->AbBox;
	      if (pBox->BxType == BoSplit && pBox->BxNexChild != NULL)
		pBox = pBox->BxNexChild;
	      if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		{
		  GetSizesFrame (frame, &x, &y);
		  *xCoord = pBox->BxXOrg;
		  *yCoord = pBox->BxYOrg;
		}
	      else
		{
		  x = pAb->AbEnclosing->AbBox->BxWidth;
		  y = pAb->AbEnclosing->AbBox->BxHeight;
		  *xCoord = pBox->BxXOrg - pAb->AbEnclosing->AbBox->BxXOrg;
		  *yCoord = pBox->BxYOrg - pAb->AbEnclosing->AbBox->BxYOrg;
		}
	      
	      /* Convert values to pixels */
	      if (unit == UnPercent)
		{
		  *xCoord = PixelValue (*xCoord, UnPercent, (PtrAbstractBox) x, 0);
		  *yCoord = PixelValue (*yCoord, UnPercent, (PtrAbstractBox) y, 0);
		}
	      else
		{
		  *xCoord = PixelValue (*xCoord, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		  *yCoord = PixelValue (*yCoord, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
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
#ifdef __STDC__
void                TtaGiveBoxAbsPosition (Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord)
#else  /* __STDC__ */
void                TtaGiveBoxAbsPosition (element, Document document, view, unit, xCoord, yCoord)
Element             element;
Document            document;
View                view;
TypeUnit            unit;
int                *xCoord;
int                *yCoord;

#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  ViewFrame	      *pFrame;
  int                 v, frame;
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
	  if (view < 100)
	    /* View of the main tree */
	    v = view;
	  else
	    /* View of associated elements */
	    v = 1;
	  pAb = AbsBoxOfEl ((PtrElement) element, v);
	  if (pAb == NULL)
	    {
	      *xCoord = 32000;
	      *yCoord = 32000;
	    }
	  else
	    {
	      pBox = pAb->AbBox;
	      if (pBox->BxType == BoSplit && pBox->BxNexChild != NULL)
		pBox = pBox->BxNexChild;
	      pFrame = &ViewFrameTable[frame - 1];
	      *xCoord = pBox->BxXOrg - pFrame->FrXOrg;
	      *yCoord = pBox->BxYOrg - pFrame->FrYOrg;

	      /* Convert values to pixels */
	      if (unit == UnPercent)
		{
	          GetSizesFrame (frame, &x, &y);
		  *xCoord = PixelValue (*xCoord, UnPercent, (PtrAbstractBox) x, 0);
		  *yCoord = PixelValue (*yCoord, UnPercent, (PtrAbstractBox) y, 0);
		}
	      else
		{
		  *xCoord = PixelValue (*xCoord, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		  *yCoord = PixelValue (*yCoord, unit, pAb, ViewFrameTable[frame - 1].FrMagnification);
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
#ifdef __STDC__
void                TtaGiveWindowSize (Document document, View view, TypeUnit unit, int *width, int *height)
#else  /* __STDC__ */
void                TtaGiveWindowSize (document, view, unit, width, height)
Document            document;
View                view;
TypeUnit            unit;
int                *width;
int                *height;

#endif /* __STDC__ */
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
   values are PRSize, PtStyle, PtWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRShowBox, PRNotInLine.

   Return value:
   the presentation rule found, or NULL if the element
   does not have this type of presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaGetPRule (Element element, int presentationType)
#else  /* __STDC__ */
PRule               TtaGetPRule (element, presentationType)
Element             element;
int                 presentationType;
#endif /* __STDC__ */

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
	    (pPres->PrType == PtFunction && (int)(pPres->PrPresFunction) == func))
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
   type of that presentation rule. Available values are RSize, RStyle, RWeight,
   RFont, RUnderline, RThickness, PtIndent, RLineSpacing, RDepth, RAdjust,
   RJustify, RLineStyle, RLineWeight, RFillPattern, RBackground,
   RForeground, RHyphenate, PRShowBox, PRNotInLine.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetPRuleType (PRule pRule)
#else  /* __STDC__ */
int                 TtaGetPRuleType (pRule)
PRule               pRule;
#endif /* __STDC__ */
{
  int               presentationType;

  UserErrorCode = 0;
  presentationType = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      presentationType = ((PtrPRule) pRule)->PrType;
      if (presentationType == PtFunction && ((PtrPRule) pRule)->PrPresFunction == FnShowBox)
	presentationType = PRShowBox;
      else if (presentationType == PtFunction && ((PtrPRule) pRule)->PrPresFunction == FnNotInLine)
	presentationType = PRNotInLine;
    }
  return presentationType;
}

/*----------------------------------------------------------------------
   TtaGetPRuleValue

   Returns the value of a presentation rule.

   Parameters:
   pRule: the presentation rule of interest.

   Return values according to rule type:
   PRSize: an integer between 6 and 72 (body size in points).
   PRStyle: StyleRoman, StyleItalics, StyleOblique.
   PRWeight: WeightNormal, WeightBold.
   PRFont: FontTimes, FontHelvetica, FontCourier.
   RPUnderline: NoUnderline, Underline, Overline, CrossOut.
   PRThickness: ThinUnderline, ThickUnderline.
   PtIndent: a positive, null or negative integer (indentation in points).
   PRLineSpacing: a positive integer (line spacing in points).
   PRDepth: a positive integer (depth of the element).
   PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots.
   PRJustify: Justified, NotJustified.
   PRHyphenate: Hyphenation, NoHyphenation.
   PRLineStyle: SolidLine, DashedLine, DottedLine.
   PRLineWeight: a positive or null integer (stroke width for graphics).
   PRFillPattern: rank of the pattern in the file thot.pattern.
   PRBackground: rank of the background color in the file thot.color.
   PRForeground: rank of the foreground color in the file thot.color.
   PRWidth, PRHeight, PRVertPos, PRHorizPos: a positive or null integer.
 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetPRuleValue (PRule pRule)
#else  /* __STDC__ */
int                 TtaGetPRuleValue (pRule)
PRule               pRule;
#endif /* __STDC__ */
{
  int                 value;

  UserErrorCode = 0;
  value = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    switch (((PtrPRule) pRule)->PrType)
      {
      case PtSize:
	/* Body-size in typographic points */
	value = ((PtrPRule) pRule)->PrMinValue;
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
      case PtIndent:
	value = ((PtrPRule) pRule)->PrMinValue;
	break;
      case PtLineSpacing:
	value = ((PtrPRule) pRule)->PrMinValue;
	break;
      case PtDepth:
	value = ((PtrPRule) pRule)->PrIntValue;
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
	  case AlignLeftDots:
	    value = LeftWithDots;
	    break;
	  default:
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	break;
      case PtJustify:
	if (((PtrPRule) pRule)->PrJustify)
	  value = Justified;
	else
	  value = NotJustified;
	break;
      case PtHyphenate:
	if (((PtrPRule) pRule)->PrJustify)
	  value = Hyphenation;
	else
	  value = NoHyphenation;
	break;
      case PtLineStyle:
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
      case PtLineWeight:
	/* value = thickness of the line in typographic points */
	value = ((PtrPRule) pRule)->PrMinValue;
	break;
      case PtFillPattern:
      case PtBackground:
      case PtForeground:
	value = ((PtrPRule) pRule)->PrIntValue;
	break;
      case PtWidth:
      case PtHeight:
	value = ((PtrPRule) pRule)->PrDimRule.DrValue;
	break;
      case PtVertPos:
      case PtHorizPos:
	value = ((PtrPRule) pRule)->PrPosRule.PoDistance;
	break;
      default:
	TtaError (ERR_invalid_parameter);
	break;
      }
  return value;
}

/*----------------------------------------------------------------------
   TtaGetPRuleUnit

   Returns the unit of a presentation rule.

   Parameters:
   pRule: the presentation rule of interest.

   Return the unit of the rule type PRSize, PRIndent, PRLineSpacing, PRLineWeight,
   PRWidth, PRHeight, PRVertPos, PRHorizPos.
   This unit could be UnRelative, UnXHeight, UnPoint, UnPixel, UnPercent.
   Return UnRelative in other cases.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetPRuleUnit (PRule pRule)
#else  /* __STDC__ */
int                 TtaGetPRuleUnit (pRule)
PRule               pRule;
#endif /* __STDC__ */
{
  int                 value;

  UserErrorCode = 0;
  value = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    switch (((PtrPRule) pRule)->PrType)
      {
      case PtSize:
	/* Body-size in typographic points */
	value = ((PtrPRule) pRule)->PrMinUnit;
	break;
      case PtIndent:
	value = ((PtrPRule) pRule)->PrMinUnit;
	break;
      case PtLineSpacing:
	value = ((PtrPRule) pRule)->PrMinUnit;
	break;
      case PtLineWeight:
	/* value = thickness of the line in typographic points */
	value = ((PtrPRule) pRule)->PrMinUnit;
	break;
      case PtWidth:
      case PtHeight:
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
#ifdef __STDC__
int                 TtaGetPRuleView (PRule pRule)
#else  /* __STDC__ */
int                 TtaGetPRuleView (pRule)
PRule               pRule;
#endif /* __STDC__ */
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
#ifdef __STDC__
int                 TtaSamePRules (PRule pRule1, PRule pRule2)
#else  /* __STDC__ */
int                 TtaSamePRules (pRule1, pRule2)
PRule               pRule1;
PRule               pRule2;
#endif /* __STDC__ */
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
				   case PtFillPattern:
				   case PtBackground:
				   case PtForeground:
				      if (pR1->PrAttrValue == pR2->PrAttrValue)
					 if (pR1->PrIntValue == pR2->PrIntValue)
					    result = 1;
				      break;
				   case PtFont:
				   case PtStyle:
				   case PtWeight:
				   case PtUnderline:
				   case PtThickness:
				   case PtLineStyle:
				      if (pR1->PrChrValue == pR2->PrChrValue)
					 result = 1;
				      break;
				   case PtBreak1:
				   case PtBreak2:
				   case PtIndent:
				   case PtSize:
				   case PtLineSpacing:
				   case PtLineWeight:
				      if (pR1->PrMinUnit == pR2->PrMinUnit)
					 if (pR1->PrMinAttr == pR2->PrMinAttr)
					    if (pR1->PrMinValue == pR2->PrMinValue)
					       result = 1;
				      break;
				   case PtVertRef:
				   case PtHorizRef:
				   case PtVertPos:
				   case PtHorizPos:
				      if (pR1->PrPosRule.PoDistUnit == pR2->PrPosRule.PoDistUnit)
					 if (pR1->PrPosRule.PoDistance == pR2->PrPosRule.PoDistance)
					    result = 1;
				      break;
				   case PtHeight:
				   case PtWidth:
				      if (pR1->PrDimRule.DrUnit == pR2->PrDimRule.DrUnit)
					 if (pR1->PrDimRule.DrValue == pR2->PrDimRule.DrValue)
					    result = 1;
				      break;
				   case PtJustify:
				   case PtHyphenate:
				      if (pR1->PrJustify == pR2->PrJustify)
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
