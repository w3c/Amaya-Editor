/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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


#include "buildboxes_f.h"
#include "absboxes_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "boxpositions_f.h"
#include "pagecommands_f.h"


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
   boolean             stop;

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
   boolean             stop, exist;
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

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   ReglePageDebut     recherche la regle page qui a cree la Marque de 
   Page Debut pElPage.                             
   La recherche parcourt les elements qui sont     
   apres pElPage dans l'arbre abstrait.            
   retourne dans pSchP le schema de presentation   
   procedure utilisee dans print et page           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPRule            ReglePageDebut (PtrElement pElPage, PtrPSchema * pSchP)

#else  /* __STDC__ */
PtrPRule            ReglePageDebut (pElPage, pSchP)
PtrElement          pElPage;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

{
   PtrPRule            pR;
   PtrElement          pNext;

   pR = NULL;

   if (pElPage->ElPageType == PgBegin)
     {
	/* Nouveau type de page, on determine la hauteur des pages */
	/* il faut rechercher la regle sur l'element suivant */
	/* car les elements marque page debut sont places AVANT les elements */
	/* qui portent la regle page */
	if (pElPage->ElNext != NULL)
	  {
	     pNext = pElPage->ElNext;
	     /* on saute les eventuelles marques de colonnes */
	     /* ou de page (pour d'autres vues par exemple ?) */
	     while (pNext != NULL
		    && pNext->ElTypeNumber == PageBreak + 1)
		pNext = pNext->ElNext;
	     /* on cherche uniquement sur pPsuiv car normalement l'element */
	     /* marque page debut a ete place juste devant l'element qui */
	     /* portait la regle page correspondante */
	     if (pNext != NULL)
		pR = GetPageRule (pNext, pElPage->ElViewPSchema, pSchP);
	  }
	if (pR == NULL && pElPage->ElParent != NULL)
	   /* si pSuiv ne portait pas de regle, */
	   /* l'element englobant porte-t-il une regle page ? */
	   pR = GetPageRule (pElPage->ElParent,
			     pElPage->ElViewPSchema, pSchP);
     }
   return pR;
}

#endif /* __COLPAGE */


#ifndef __COLPAGE__

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
	     PageFooterHeight = pSP->PsPresentBox[(*b) - 1].PbFooterHeight;
	     /* cherche la regle de hauteur de la boite page */
	     pR = GetPRulePage (PtHeight, *b, pSP);
	     if (pR != NULL)
		if (!pR->PrDimRule.DrPosition)
		   PageHeight = pR->PrDimRule.DrValue - PageFooterHeight;
	     *pSchP = pSP;	/* parametre retour */
	  }
     }
}

#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
 /* V4 : procedure TagAbsBoxInPage supprimee */
 /* V4 : procedure KillAbsBoxAboveLimit supprimee */
#else  /* __COLPAGE__ */

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
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
 /* V4 : la procedure KillAbsBoxBeforePage est completement changee */
/*----------------------------------------------------------------------
  KillAbsBoxBeforePage detruit tous les paves qui precedent la page pPage
  sauf ceux de l'element BoTable englobant pPage.
  On detruit les paves haut, corps, bas de page et filet des pages
  precedentes.
  Retourne vrai si l'image restante est plus petite qu'une page.       
  On garde la page courante pour savoir ou continuer.    
  Destruction page par page (y compris le filet) .         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             KillAbsBoxBeforePage (PtrAbstractBox pPage, int frame, PtrDocument pDoc, int viewNb, int *clipOrg)
#else  /* __STDC__ */
boolean             KillAbsBoxBeforePage (pPage, frame, pDoc, viewNb, clipOrg)
PtrAbstractBox      pPage;
int                 frame;
PtrDocument         pDoc;
int                 viewNb;
int                *clipOrg;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAb, RAbsBox, rootAbsBox;
  PtrAbstractBox      pPageTable;
  boolean             ret, det;

  ret = TRUE; /* initialisation de ret, est-ce bon ? */
  *clipOrg = 0;
  /* is there an enclosing table? */
  pPageTable = SearchEnclosingType (pPage, BoTable);
  /* on detruit les paves haut, corps, bas de page et filet */
  /* des pages precedentes */
  /* cette procedure est plus simple qu'avant */
  /* Test pour verifier que les paves sont bien des marques */
  /* de page et non de colonnes */
  if (pPage->AbElement->ElTypeNumber == PageBreak + 1
      && (pPage->AbElement->ElPageType == PgBegin
	  || pPage->AbElement->ElPageType == PgComputed
	  || pPage->AbElement->ElPageType == PgUser))
    {
      pAb = pPage->AbElement->ElAbstractBox[viewNb - 1];
      /* premier pave de la page */
      rootAbsBox = pAb->AbEnclosing;	/* racine */
      if (rootAbsBox == NULL || rootAbsBox->AbEnclosing != NULL)
	/* erreur image abstraite */
	AffPaveDebug (pPage);
      /* on detruit les paves precedents */
      /* mais on laisse le pave racine non coupe en tete pour */
      /* que les paves detruits ne soient pas recrees dans l'appel */
      /* de AfFinFenetre (pour le print) */
      pAb = pAb->AbPrevious;
      det = FALSE;		/* a priori pas de paves detruits */
      while (pAb != NULL)
	{
	  SetDeadAbsBox (pAb);
	  ApplyRefAbsBoxSupp (pAb, &RedispAbsBox, pDoc);
	  pAb = pAb->AbPrevious;
	  det = TRUE;	/* des paves ont ete detruits */
	}
      /* signale les paves morts au Mediateur */
      /* si pas de destruction, on appelle ChangeConcreteImage pour positionner ret */
      /* et refait evaluer la coupure de page */
      RealPageHeight = BreakPageHeight;
      ChangeRHPage (rootAbsBox, pDoc, viewNb);
      ret = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
      if (det)
	/* libere tous les paves morts de la vue */
	FreeDeadAbstractBoxes (rootAbsBox);
      /* cherche le rappel suivant de ce saut de page */
      /* supprime */
    } /* fin cas ou pPage est bien une marque de page */
  return ret;
}
#else  /* __COLPAGE__ */

/*----------------------------------------------------------------------
   KillAbsBoxBeforePage detruit tous les paves qui precedent le filet  
   marquant la frontiere de page qui est a l'interieur du pave Marque
   de Page pointe' par pPage sauf ceux qui appartiennent au BoTable
   englobant pPage.
   Retourne vrai si l'image restante est plus petite qu'une page.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             KillAbsBoxBeforePage (PtrAbstractBox pPage, int frame, PtrDocument pDoc, int viewNb, int *clipOrg)
#else  /* __STDC__ */
boolean             KillAbsBoxBeforePage (pPage, frame, pDoc, viewNb, clipOrg)
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
  boolean           stop, ret;

  /* cherche d'abord le pave racine de la vue */
  rootAbsBox = pPage;
  *clipOrg = 0;
  /* is there an enclosing table? */
  pPageTable = SearchEnclosingType (pPage, BoTable);
  while (rootAbsBox->AbEnclosing != NULL)
    rootAbsBox = rootAbsBox->AbEnclosing;
  /* marque tous les paves comme faisant partie de la page */
  TagAbsBoxInPage (rootAbsBox);
  /* detruit, dans le pave Marque Page, les boites de bas de page qui */
  /* precedent le filet marquant le saut de page. */
  pAb = pPage->AbFirstEnclosed;
  stop = FALSE;
  do
    if (pAb == NULL)
      stop = TRUE;
    else if (!pAb->AbPresentationBox)
      /* Note: le filet n'est pas un pave de presentation, alors que */
      /* toutes les autres boites de bas de page sont des paves de */
      /* presentation */
      {
	stop = TRUE;
	/* demande au Mediateur la position verticale du filet */
	pPageLine = pAb;
	SetPageHeight (pAb, TRUE, &h, &yThread, &NbCar);
      }
    else
      {
	SetDeadAbsBox (pAb);
	pAb = pAb->AbNext;
      }
  while (!(stop));

  /* detruit tous les paves qui precedent le pave Marque Page et ses */
  /* paves englobants, mais pas ceux qui contiennent un saut de page */
  pAb = pPage;
  while (pAb != NULL)
    {
      while (pAb->AbPrevious != NULL)
	{
	  pAb = pAb->AbPrevious;
	  if (pPageTable != NULL)
	    pTable = SearchEnclosingType (pAb, BoTable);
	  else
	    pTable = NULL;
	  if (!pAb->AbOnPageBreak &&
	      (pAb->AbElement->ElTypeNumber == PageBreak + 1 ||
	       pPageTable == NULL || pPageTable != pTable))
	    {
	      if (pTable != NULL)
		DestroyAbsBoxesView (pTable->AbElement, pDoc, FALSE, viewNb);
	      else if (pAb->AbPresentationBox)
		{
		  /* Tue les paves de presentation */
		  SetDeadAbsBox (pAb);
		  ApplyRefAbsBoxSupp (pAb, &RedispAbsBox, pDoc);
		}
	      else
		DestroyAbsBoxesView (pAb->AbElement, pDoc, FALSE, viewNb);
	    }
	}
      pAb = pAb->AbEnclosing;
      /* marque les paves englobant la marque de page */
      if (pAb != NULL)
	pAb->AbOnPageBreak = TRUE;
    }

  /* Verifie les paves suivant la marque de page aux niveaux superieurs */
  pAb = pPage;
  while (pAb != NULL)
    {
      pNext = pAb->AbNext;
      while (pNext != NULL)
	{
	  if (pPageTable != NULL)
	    pTable = SearchEnclosingType (pNext, BoTable);
	  else
	    pTable = NULL;
	  if (pPageTable == NULL || pPageTable != pTable)
	    {
	      if (!pNext->AbDead)
		{
		  /* demande au Mediateur la position et la hauteur du pave */
		  SetPageHeight (pNext, TRUE, &h, &yTop, &NbCar);
		  if (yTop < yThread)
		    /* le haut du pave est au-dessus du saut de page */
		    if (yTop + h <= yThread && !pNext->AbOnPageBreak)
		      {
			if (pNext->AbPresentationBox)
			  /* Tue les paves de presentation */
			  {
			    SetDeadAbsBox (pNext);
			    ApplyRefAbsBoxSupp (pNext, &RedispAbsBox, pDoc);
			  }
			else
			  DestroyAbsBoxesView (pNext->AbElement, pDoc, FALSE, viewNb);
		      }
		    else
		      /* le pave est traverse par le saut de page */
		      KillAbsBoxAboveLimit (pNext, yThread, viewNb, pDoc, &RedispAbsBox);
		}
	    }
	  pNext = pNext->AbNext;
	}
      pAb = pAb->AbEnclosing;
    }
  /* signale les paves morts au Mediateur */
  RealPageHeight = 0;
  ret = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
  /* libere tous les paves morts de la vue */
  FreeDeadAbstractBoxes (rootAbsBox);
  SetPageHeight (pPage, TRUE, &h, clipOrg, &NbCar);
  RealPageHeight = PageHeight + *clipOrg;
  ret = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
  return ret;
}
#endif /* __COLPAGE__ */
