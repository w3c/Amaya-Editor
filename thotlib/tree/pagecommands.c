/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *  module de traitement des regles page
 * ce module contient les procedures utilisees par le
 * paginateur et l'imprimeur (extraites et modifiees
 * de page.c et print.c)
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Pagination at printing time
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "page_tv.h"
#include "frame_tv.h"


#include "absboxes_f.h"
#include "boxpositions_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "font_f.h"
#include "pagecommands_f.h"
#include "presrules_f.h"


/*----------------------------------------------------------------------
   GetPageRule cherche la regle page associee aux elements du        
   type de l'element pointe' par pEl dans la vue view       
   du schema de presentation ou elle est definie           
  ----------------------------------------------------------------------*/
PtrPRule GetPageRule (PtrElement pEl, PtrDocument pDoc, int view,
		      PtrPSchema * pSchP)
{
   PtrPRule            pRule, pRPage;
   PtrSSchema          pSchS;
   int                 entry;
   ThotBool            stop;

   pRPage = NULL;
   SearchPresSchema (pEl, pSchP, &entry, &pSchS, pDoc);
   pRule = (*pSchP)->PsElemPRule->ElemPres[entry - 1];
   /* 1ere regle de pres. du type */
   stop = FALSE;
   do
      if (pRule == NULL)
	 stop = TRUE;
      else if (pRule->PrType > PtFunction)
	 stop = TRUE;
      else if (pRule->PrType == PtFunction &&
	       pRule->PrViewNum == view &&
	       pRule->PrPresFunction == FnPage)
	/* c'est une regle Page */
	{
	   pRPage = pRule;
	   stop = TRUE;
	}
      else
	 pRule = pRule->PrNextPRule;
   while (!(stop));
   return pRPage;
}

/*----------------------------------------------------------------------
   GetPRulePage  cherche parmi les regles de presentation de la  
   page, la regle du type TypeR  et retourne un     
   pointeur sur la regle trouvee ou NULL si pas     
   trouvee.                                         
  ----------------------------------------------------------------------*/
PtrPRule GetPRulePage (PRuleType TypeR, int b, PtrPSchema pSchP)
{
   ThotBool            stop, exist;
   PtrPRule            pR;

   pR = pSchP->PsPresentBox->PresBox[b - 1]->PbFirstPRule;
   stop = FALSE;
   exist = FALSE;
   do
     {
	if (pR == NULL)
	   stop = TRUE;
	else if (pR->PrType > TypeR)
	   stop = TRUE;
	else if (pR->PrType == TypeR)
	  {
	     stop = TRUE;
	     exist = TRUE;
	  }
	if (!stop)
	   pR = pR->PrNextPRule;
     }
   while (!(stop));
   if (exist)
      return pR;
   else
      return NULL;
}


/*----------------------------------------------------------------------
    PageHeaderFooter met a jour les variables PageHeight et PageFooterHeight 
   selon le type de page auquel appartient l'element       
   Marque Page pointe par pElPage.                         
   view indique le numero de la vue dans le schema de       
   presentation pour laquelle on construit des pages.      
   Procedure utilisee pour la pagination et l'impression   
   les parametres b et pSchP sont utilises pour le print   
  ----------------------------------------------------------------------*/
void PageHeaderFooter (PtrElement pElPage, PtrDocument pDoc, int view,
		       int *b, PtrPSchema * pSchP)
{
   PtrPRule            pR;
   PtrPSchema          pSP;

   *b = 0;
   *pSchP = NULL;
   if (pElPage->ElPageType == PgBegin)
     {
	/* Nouveau type de page, on determine la hauteur des pages */
	pR = NULL;
	if (pElPage->ElParent != NULL)
	   /* l'element englobant porte-t-il une regle page ? */
	   pR = GetPageRule (pElPage->ElParent, pDoc, view, &pSP);
	if (pR != NULL)		/* on a trouve la regle page */
	  {
	     *b = pR->PrPresBox[0];	/* parametre retour */
	     /* Hauteur minimum du bas de page */
	     PageFooterHeight = PixelValue (pSP->PsPresentBox->PresBox[(*b) - 1]->PbFooterHeight, UnPoint, NULL, 0);
	     /* cherche la regle de hauteur de la boite page */
	     pR = GetPRulePage (PtHeight, *b, pSP);
	     if (pR != NULL)
		if (!pR->PrDimRule.DrPosition)
		   PageHeight = PixelValue (pR->PrDimRule.DrValue, UnPoint, NULL, 0) - PageFooterHeight;
	     *pSchP = pSP;	/* parametre retour */
	  }
     }
}


/*----------------------------------------------------------------------
   TagAbsBoxInPage marque tous les paves du sous arbre de racine pAb     
   comme faisant partie de la page courante                
  ----------------------------------------------------------------------*/
void TagAbsBoxInPage (PtrAbstractBox pAb)
{
   PtrAbstractBox      pPa1;

   pPa1 = pAb;
   pPa1->AbAfterPageBreak = FALSE;
   pPa1->AbOnPageBreak = FALSE;
   pPa1 = pPa1->AbFirstEnclosed;
   while (pPa1 != NULL)
     {
	TagAbsBoxInPage (pPa1);
	pPa1 = pPa1->AbNext;
     }
}

/*----------------------------------------------------------------------
   KillAbsBoxAboveLimit tue tous les paves contenus dans le            
   pave pointe' par pP et qui sont au-dessus de la limite. 
  ----------------------------------------------------------------------*/
void KillAbsBoxAboveLimit (PtrAbstractBox pP, int limit, int viewNb,
			   PtrDocument pDoc, PtrAbstractBox *RedispAbsBox)
{
  int               haut, y, NCar;

  pP = pP->AbFirstEnclosed;
  while (pP != NULL)
    {
      /* on ignore les paves deja morts */
      if (!pP->AbDead)
	if (pP->AbElement->ElTypeNumber != PageBreak + 1)
	  /* demande au Mediateur la position et la hauteur du pave */
	  {
	    SetPageHeight (pP, &haut, &y, &NCar);
	    if (y < limit)
	      {
	      /* le haut du pave est au-dessus de la limite */
	      /* on ne tue pas les paves qui contiennent une marque de page */
	      if (y + haut <= limit && !pP->AbOnPageBreak)
		{
		if (pP->AbPresentationBox)
		  /* Tue les paves de presentation */
		  {
		    SetDeadAbsBox (pP);
		    ApplyRefAbsBoxSupp (pP, RedispAbsBox, pDoc);
		  }
		else
		  DestroyAbsBoxesView (pP->AbElement, pDoc, FALSE, viewNb);
		}
	      else
		/* le pave est traverse par la limite */
		KillAbsBoxAboveLimit (pP, limit, viewNb, pDoc, RedispAbsBox);
	      }
	  }
      pP = pP->AbNext;
    }
}


/*----------------------------------------------------------------------
   KillAbsBoxBeforePage detruit tous les paves qui precedent le filet  
   marquant la frontiere de page qui est a l'interieur du pave Marque
   de Page pointe' par pPage sauf ceux qui appartiennent au BoTable
   englobant pPage.
   Retourne vrai si l'image restante est plus petite qu'une page.
  ----------------------------------------------------------------------*/
ThotBool KillAbsBoxBeforePage (PtrAbstractBox pPage, int frame,
			       PtrDocument pDoc, int viewNb, int *clipOrg)
{
  PtrAbstractBox    pAb, RedispAbsBox, rootAbsBox;
  PtrAbstractBox    pGather, pNext, pPageLine;
  int               h, yTop, NbCar, yRef;
  ThotBool          stop, ret;

  *clipOrg = 0;
  /* is there an enclosing table? */
  pGather = GetEnclosingGather (pPage);
  /* remove page footer boxes (above the page break line) within this page element */
  pAb = pPage->AbFirstEnclosed;
  stop = FALSE;
  do
    if (pAb == NULL)
      stop = TRUE;
    else if (!pAb->AbPresentationBox)
      /* the page break line is not a presentation box, all others are */
      /* presentation boxes */
      {
	stop = TRUE;
	/* get the new page break line position */
	pPageLine = pAb;
	SetPageHeight (pAb, &h, &yRef, &NbCar);
      }
    else
      {
	SetDeadAbsBox (pAb);
	pAb = pAb->AbNext;
      }
  while (!stop);

  /* remove all abstract boxes above the page element or gather element
     that includes the page */
  if (pGather)
    pAb = pGather;
  else
    pAb = pPage;
  while (pAb)
    {
      while (pAb->AbPrevious)
	{
	  pAb = pAb->AbPrevious;
	  /* avoid to kill abstract boxes linked to the current gather */
	  if (pGather == NULL || pGather->AbElement != pAb->AbElement)
	    {
	      if (pAb->AbPresentationBox)
		{
		  /* Kill all presentation abstract boxes */
		  SetDeadAbsBox (pAb);
		  ApplyRefAbsBoxSupp (pAb, &RedispAbsBox, pDoc);
		}
	      else if (!pAb->AbOnPageBreak && !pAb->AbAfterPageBreak)
		DestroyAbsBoxesView (pAb->AbElement, pDoc, FALSE, viewNb);
	      else
		{
		  /* clear previous indicators */
		  pAb->AbOnPageBreak = FALSE;
		  pAb->AbAfterPageBreak = FALSE;
		}
	    }
	  else
	    {
	      /* clear previous indicators */
	      pAb->AbOnPageBreak = FALSE;
	      pAb->AbAfterPageBreak = FALSE;
	    }
	}
      pAb = pAb->AbEnclosing;
      /* set AbOnPageBreak to all enclosing abstract boxes of the page element */
      if (pAb)
	pAb->AbOnPageBreak = TRUE;
    }

  /* check whether abstract boxes after the page element or the gather element
     are displayed above the page break line */
  if (pGather)
    pAb = pGather;
  else
    pAb = pPage;
  while (pAb)
    {
      pNext = pAb->AbNext;
      while (pNext)
	{
	  /* avoid to kill abstract boxes linked to the current gather */
	  if (pGather == NULL || pGather->AbElement != pNext->AbElement)
	    {
	      if (!pNext->AbDead)
		{
		  /* get the new page break line position */
		  SetPageHeight (pNext, &h, &yTop, &NbCar);
		  if (yTop < yRef)
		    {
		      if (yTop + h <= yRef && !pNext->AbOnPageBreak)
			{
			  /* the top of that box is above the page break */
			  if (pNext->AbPresentationBox)
			    {
			      /* Kill all presentation abstract boxes */
			      SetDeadAbsBox (pNext);
			      ApplyRefAbsBoxSupp (pNext, &RedispAbsBox, pDoc);
			    }
			  else
			    DestroyAbsBoxesView (pNext->AbElement, pDoc, FALSE, viewNb);
			}
		      else
			{
			  /* the page break line crosses that box */
			  KillAbsBoxAboveLimit (pNext, yRef, viewNb, pDoc, &RedispAbsBox);
			  /* clear previous indicators */
			  //pNext->AbOnPageBreak = FALSE;
			  pNext->AbAfterPageBreak = FALSE;
			}
		    }
		  else
		    {
		      /* clear previous indicators */
		      pNext->AbOnPageBreak = FALSE;
		      pNext->AbAfterPageBreak = FALSE;
		    }
		}
	    }
	  else
	    {
	      /* clear previous indicators */
	      pNext->AbOnPageBreak = FALSE;
	      pNext->AbAfterPageBreak = FALSE;
	    }
	    
	  pNext = pNext->AbNext;
	}
      pAb = pAb->AbEnclosing;
    }
  /* take killed abstract boxes into account in the Concrete Image */
  RealPageHeight = 0;
  rootAbsBox = ViewFrameTable[frame - 1].FrAbstractBox;
  ret = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
  /* free killed abstract boxes */
  FreeDeadAbstractBoxes (rootAbsBox, frame);
  SetPageHeight (pPage, &h, clipOrg, &NbCar);
  /* compute AbOnPageBreak and AbAfterPageBreak according to the new situation */
  RealPageHeight = PageHeight + *clipOrg;
  ret = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
  return ret;
}
