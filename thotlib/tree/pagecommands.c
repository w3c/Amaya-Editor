/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
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

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"


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

#ifdef __STDC__
PtrPRule            GetPageRule (PtrElement pEl, int view, PtrPSchema * pSchP)

#else  /* __STDC__ */
PtrPRule            GetPageRule (pEl, view, pSchP)
PtrElement          pEl;
int                 view;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

{
   PtrPRule            pRule, pRPage;
   PtrSSchema          pSchS;
   int                 entry;
   ThotBool            stop;

   pRPage = NULL;
   SearchPresSchema (pEl, pSchP, &entry, &pSchS);
   pRule = (*pSchP)->PsElemPRule[entry - 1];
   /* 1ere regle de pres. du type */
   stop = FALSE;
   do
      if (pRule == NULL)
	 stop = TRUE;
      else if (pRule->PrType > PtFunction)
	 stop = TRUE;
      else if (pRule->PrType == PtFunction &&
	 /**TODO*//* valeur de view si vue d'element associes ?? */
	       pRule->PrViewNum == view &&
	       pRule->PrPresFunction == FnPage)
	{			/* c'est une regle Page */
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

#ifdef __STDC__
PtrPRule            GetPRulePage (PRuleType TypeR, int b, PtrPSchema pSchP)

#else  /* __STDC__ */
PtrPRule            GetPRulePage (TypeR, b, pSchP)
PRuleType           TypeR;
int                 b;
PtrPSchema          pSchP;

#endif /* __STDC__ */

{
   ThotBool            stop, exist;
   PtrPRule            pR;

   pR = pSchP->PsPresentBox[b - 1].PbFirstPRule;
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
   (c'est toujours 1 pour les vues d'elements associes).   
   Procedure utilisee pour la pagination et l'impression   
   les parametres b et pSchP sont utilises pour le print   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PageHeaderFooter (PtrElement pElPage, int view, int *b, PtrPSchema * pSchP)

#else  /* __STDC__ */
void                PageHeaderFooter (pElPage, view, b, pSchP)
PtrElement          pElPage;
int                 view;
int                *b;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

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
	   pR = GetPageRule (pElPage->ElParent, view, &pSP);
	if (pR != NULL)		/* on a trouve la regle page */
	  {
	     *b = pR->PrPresBox[0];	/* parametre retour */
	     /* Hauteur minimum du bas de page */
	     PageFooterHeight = PixelValue (pSP->PsPresentBox[(*b) - 1].PbFooterHeight, UnPoint, NULL, 0);
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
#ifdef __STDC__
void                TagAbsBoxInPage (PtrAbstractBox pAb)
#else  /* __STDC__ */
void                TagAbsBoxInPage (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                KillAbsBoxAboveLimit (PtrAbstractBox pP, int limit, int viewNb, PtrDocument pDoc, PtrAbstractBox * RedispAbsBox)
#else  /* __STDC__ */
void                KillAbsBoxAboveLimit (pP, limit, viewNb, pDoc, RedispAbsBox)
PtrAbstractBox      pP;
int                 limit;
int                 viewNb;
PtrDocument         pDoc;
PtrAbstractBox     *RedispAbsBox;
#endif /* __STDC__ */
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
	    SetPageHeight (pP, TRUE, &haut, &y, &NCar);
	    if (y < limit)
	      /* le haut du pave est au-dessus de la limite */
	      /* on ne tue pas les paves qui contiennent une marque de page */
	      if (y + haut <= limit && !pP->AbOnPageBreak)
		if (pP->AbPresentationBox)
		  /* Tue les paves de presentation */
		  {
		    SetDeadAbsBox (pP);
		    ApplyRefAbsBoxSupp (pP, RedispAbsBox, pDoc);
		  }
		else
		  DestroyAbsBoxesView (pP->AbElement, pDoc, FALSE, viewNb);
	      else
		/* le pave est traverse par la limite */
		KillAbsBoxAboveLimit (pP, limit, viewNb, pDoc, RedispAbsBox);
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
#ifdef __STDC__
ThotBool            KillAbsBoxBeforePage (PtrAbstractBox pPage, int frame, PtrDocument pDoc, int viewNb, int *clipOrg)
#else  /* __STDC__ */
ThotBool            KillAbsBoxBeforePage (pPage, frame, pDoc, viewNb, clipOrg)
PtrAbstractBox      pPage;
int                 frame;
PtrDocument         pDoc;
int                 viewNb;
int                *clipOrg;
#endif /* __STDC__ */
{
  PtrAbstractBox    pAb, RedispAbsBox, rootAbsBox;
  PtrAbstractBox    pPageTable, pTable, pNext, pPageLine;
  int               h, yTop, NbCar, yThread;
  ThotBool          stop, ret;

  *clipOrg = 0;
  /* is there an enclosing table? */
  pPageTable = NULL;
  pTable = SearchEnclosingType (pPage, BoTable);
  while (pTable != NULL)
    {
      /* get the most enclosing table */
      pPageTable = pTable;
      pTable = SearchEnclosingType (pPageTable->AbEnclosing, BoTable);
    }

  /* look for the root abstract box in the view */
  rootAbsBox = pPage;
  while (rootAbsBox->AbEnclosing != NULL)
    rootAbsBox = rootAbsBox->AbEnclosing;

  /* remove all AbAfterPageBreak and AbOnPageBreak indicators in the view */
  TagAbsBoxInPage (rootAbsBox);
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
	SetPageHeight (pAb, TRUE, &h, &yThread, &NbCar);
      }
    else
      {
	SetDeadAbsBox (pAb);
	pAb = pAb->AbNext;
      }
  while (!stop);

  /* remove all abstract boxes above the page element or the table element
     that includes the page element */
  if (pPageTable == NULL)
    pAb = pPage;
  else
    pAb = pPageTable;
  while (pAb != NULL)
    {
      while (pAb->AbPrevious != NULL)
	{
	  pAb = pAb->AbPrevious;
	  if (pPageTable != NULL)
	    pTable = SearchEnclosingType (pAb, BoTable);
	  else
	    pTable = NULL;

	  /* avoid to kill abstract boxes linked to the current table */
	  if (pPageTable == NULL || pPageTable != pTable)
	    {
	      if (pTable != NULL)
		DestroyAbsBoxesView (pTable->AbElement, pDoc, FALSE, viewNb);
	      else if (pAb->AbPresentationBox)
		{
		  /* Kill all presentation abstract boxes */
		  SetDeadAbsBox (pAb);
		  ApplyRefAbsBoxSupp (pAb, &RedispAbsBox, pDoc);
		}
	      else
		DestroyAbsBoxesView (pAb->AbElement, pDoc, FALSE, viewNb);
	    }
	}
      pAb = pAb->AbEnclosing;
      /* set AbOnPageBreak to all enclosing abstract boxes of the page element */
      if (pAb != NULL)
	pAb->AbOnPageBreak = TRUE;
    }

  /* check whether abstract boxes after the page element or the table element
     are displayed above the page break line */
  if (pPageTable == NULL)
    pAb = pPage;
  else
    pAb = pPageTable;
  while (pAb != NULL)
    {
      pNext = pAb->AbNext;
      while (pNext != NULL)
	{
	  if (pPageTable != NULL)
	    pTable = SearchEnclosingType (pNext, BoTable);
	  else
	    pTable = NULL;

	  /* avoid to kill abstract boxes linked to the current table */
	  if (pPageTable == NULL || pPageTable != pTable)
	    {
	      if (!pNext->AbDead)
		{
		  /* get the new page break line position */
		  SetPageHeight (pNext, TRUE, &h, &yTop, &NbCar);
		  if (yTop < yThread)
		    if (yTop + h <= yThread && !pNext->AbOnPageBreak)
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
		      /* the page break line crosses that box */
		      KillAbsBoxAboveLimit (pNext, yThread, viewNb, pDoc, &RedispAbsBox);
		}
	    }
	  pNext = pNext->AbNext;
	}
      pAb = pAb->AbEnclosing;
    }
  /* take killed abstract boxes into account in the Concrete Image */
  RealPageHeight = 0;
  ret = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
  /* free killed abstract boxes */
  FreeDeadAbstractBoxes (rootAbsBox, frame);
  SetPageHeight (pPage, TRUE, &h, clipOrg, &NbCar);
  /* compute AbOnPageBreak and AbAfterPageBreak according to the new situation */
  RealPageHeight = PageHeight + *clipOrg;
  ret = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
  return ret;
}
