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
 * Module dedicated to box positions managing.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"


#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "boxes_tv.h"
#include "platform_tv.h"

#include "memory_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "boxpositions_f.h"
#include "font_f.h"
#include "units_f.h"
#include "exceptions_f.h"


/*----------------------------------------------------------------------
   SetYCompleteForOutOfStruct regarde si des boites ont des        
   relations hors-structure avec la boite passee en parametre 
   et doivent etre placees en Y absolu.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetYCompleteForOutOfStruct (PtrBox pBox, int visibility, int frame)
#else  /* __STDC__ */
static void         SetYCompleteForOutOfStruct (pBox, visibility, frame)
PtrBox              pBox;
int                 visibility;
int                 frame;

#endif /* __STDC__ */
{
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   ThotBool            notEmpty;
   int                 i;

   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	i = 0;
	notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
	while (i < MAX_RELAT_POS && notEmpty)
	  {
	     pRelation = &pPosRel->PosRTable[i];
	     if (pRelation->ReBox->BxAbstractBox != NULL)
		/* Relation hors-struture sur l'origine de la boite */
		if (pRelation->ReOp == OpVertDep
		    && pRelation->ReBox->BxYOutOfStruct
		    && pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pBox->BxAbstractBox
		    && pRelation->ReBox->BxYToCompute)
		  {
		     /* La boite distante va etre placee */
		     pRelation->ReBox->BxYToCompute = FALSE;
		     AddBoxTranslations (pRelation->ReBox->BxAbstractBox, visibility, frame, FALSE, TRUE);
		     SetYCompleteForOutOfStruct (pRelation->ReBox, visibility, frame);
		  }
	     i++;
	     if (i < MAX_RELAT_POS)
		notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
	  }
	/* Bloc suivant */
	pPosRel = pPosRel->PosRNext;
     }
}


/*----------------------------------------------------------------------
   SetXCompleteForOutOfStruct regarde si des boites ont des        
   relations hors-structure avec la boite passee en parametre 
   et doivent etre placees en X absolu.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetXCompleteForOutOfStruct (PtrBox pBox, int visibility, int frame)
#else  /* __STDC__ */
static void         SetXCompleteForOutOfStruct (pBox, visibility, frame)
PtrBox              pBox;
int                 visibility;
int                 frame;

#endif /* __STDC__ */
{
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   ThotBool            notEmpty;
   int                 i;

   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	i = 0;
	notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
	while (i < MAX_RELAT_POS && notEmpty)
	  {
	     pRelation = &pPosRel->PosRTable[i];
	     if (pRelation->ReBox->BxAbstractBox != NULL)
		/* Relation hors-struture sur l'origine de la boite */
		if (pRelation->ReOp == OpHorizDep
		    && pRelation->ReBox->BxXOutOfStruct
		    && pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pBox->BxAbstractBox
		    && pRelation->ReBox->BxXToCompute)
		  {
		     /* La boite distante va etre placee */
		     pRelation->ReBox->BxXToCompute = FALSE;
		     AddBoxTranslations (pRelation->ReBox->BxAbstractBox, visibility, frame, TRUE, FALSE);
		     SetXCompleteForOutOfStruct (pRelation->ReBox, visibility, frame);
		  }
	     i++;
	     if (i < MAX_RELAT_POS)
		notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
	  }
	pPosRel = pPosRel->PosRNext;
     }
}


/*----------------------------------------------------------------------
   OnPage marque tous les paves ascendants comme coupe's par    
   la limite de page.                                      
   Quand la limite de page coupe un pave' non se'cable la  
   limite de page est alors remonte'e pour rejeter hors    
   page le pave' et le processus est repris au de'but.     
   Dans ce cas, au retour de la fonction le parame`tre     
   height est modifie' et isPageBreakChanged est Vrai.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         OnPage (PtrAbstractBox pAb, int *height, ThotBool * isPageBreakChanged)
#else  /* __STDC__ */
static void         OnPage (pAb, height, isPageBreakChanged)
PtrAbstractBox      pAb;
int                *height;
ThotBool           *isPageBreakChanged;
#endif /* __STDC__ */
{
  if (pAb != NULL)
    {
      if (!pAb->AbAcceptPageBreak && *height > pAb->AbBox->BxYOrg)
	{
	  /* La boite est sur la limite de page mais non secable */
	  /* deplace la limite de page sur l'origine de la boite */
	  *height = pAb->AbBox->BxYOrg;
	  *isPageBreakChanged = TRUE;
	}
      else if (!pAb->AbOnPageBreak)
	{
	  pAb->AbOnPageBreak = TRUE;
	  pAb->AbAfterPageBreak = FALSE;
	  /* traite le pave pere */
	  if (pAb->AbVertEnclosing)
	    OnPage (pAb->AbEnclosing, height, isPageBreakChanged);
	}
    }
}


/*----------------------------------------------------------------------
   OutOfPage marque tous les paves ascendants comme coupe's     
   par la limite de page s'ils ne sont pas de'ja`          
   marque's comme sur la page ou hors de la page et si     
   le pave' est englobe' verticalement.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         OutOfPage (PtrAbstractBox pAb, int *height, ThotBool *isPageBreakChanged)
#else  /* __STDC__ */
static void         OutOfPage (pAb, height, isPageBreakChanged)
PtrAbstractBox      pAb;
int                *height;
ThotBool           *isPageBreakChanged;

#endif /* __STDC__ */
{
  PtrAbstractBox      pParentAb;

  if (pAb != NULL)
    {
      pAb->AbOnPageBreak = FALSE;
      pAb->AbAfterPageBreak = TRUE;
      pParentAb = pAb->AbEnclosing;
      if (pAb->AbVertEnclosing && pParentAb != NULL)
	/* Le pere est sur la page ou hors de la page */
	if (pParentAb->AbBox->BxType == BoGhost)
	  {
	    if (!pParentAb->AbOnPageBreak)
	      OutOfPage (pParentAb, height, isPageBreakChanged);
	  }
	else if (!pAb->AbPresentationBox && pParentAb->AbBox->BxType == BoRow)
	  OutOfPage (pParentAb, height, isPageBreakChanged);
	else if (!pParentAb->AbAfterPageBreak)
	  {
	    if (pAb->AbAcceptPageBreak &&
		pAb->AbLeafType == LtGraphics &&
		pAb->AbPresentationBox)
	      {
		/* ignore this box */
		pAb->AbAfterPageBreak = FALSE;
		/**height -= 2;
		*isPageBreakChanged = TRUE;*/
	      }
	    else
	      OnPage (pParentAb, height, isPageBreakChanged);
	  }
    }
}


/*----------------------------------------------------------------------
   SetPageIndicators teste la position d'un pave' par rapport a`  
   la limite de page. Positionne les indicateurs du pave': 
   pave' sur la limite de page ou au dela` de la limite.   
   Le parame`tre height donne la position de la limite de  
   page exprime'e en pixels.                               
   Si un pave' de'borde verticalement de sa boi^te         
   englobante et que ce pave' n'est pas se'cable, alors    
   la boi^te englobante est conside're'e comme coupe'e par 
   la limite de page.                                      
   Quand la limite de page coupe un pave' non se'cable la  
   limite de page est alors remonte'e pour rejeter hors    
   page le pave' et le processus est repris au de'but.     
   Dans ce cas, au retour de la fonction le parame`tre     
   height est modifie' et isPageBreakChanged est Vrai.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetPageIndicators (PtrAbstractBox pAb, PtrAbstractBox table, int *height, ThotBool * isPageBreakChanged)
#else  /* __STDC__ */
static void         SetPageIndicators (pAb, table, height, isPageBreakChanged)
PtrAbstractBox      pAb;
PtrAbstractBox      table;
int                *height;
ThotBool           *isPageBreakChanged;
#endif /* __STDC__ */
{
  PtrAbstractBox      pChildAb;
  PtrBox              pBox;
  PtrBox              pPreviousBox;
  PtrBox              pFirstBox;
  int                 org;
  ThotBool            toContinue = FALSE;
  ThotBool            isCell;

  /* A priori la limite de page n'est pas deplacee */
  /* et il faut examiner les paves fils */
  *isPageBreakChanged = FALSE;

  if (!pAb->AbDead && pAb->AbBox != NULL) {
     /* verifie les limites de la boite du pave */
     pBox = pAb->AbBox;
     isCell = (table != NULL && pBox->BxType == BoCell);

     if (isCell && ThotLocalActions[T_firstcolumn])
        /* page break can be inserted only in the first column */
        (*ThotLocalActions[T_firstcolumn]) (pAb, table, &toContinue);
     else
         toContinue = TRUE;
     if (toContinue) {
        if (pBox->BxType == BoSplit) {
           /* --- mis en lignes ------------------------- */
           pPreviousBox = pBox;
           pBox = pBox->BxNexChild;
           pFirstBox = pBox;	/* memorise la premiere boite */
           /* A priori la boite est dans la page */
           pAb->AbAfterPageBreak = FALSE;
           pAb->AbOnPageBreak = FALSE;
           /* Ce n'est pas la peine de continuer le calcul */
           /* des coupures de boites quand la limite de    */
           /* page est deplacee */
           while (!*isPageBreakChanged && toContinue && pBox != NULL) {
                 /* Origine de la boite de coupure */
                 org = pBox->BxYOrg;
                 if (org + pBox->BxHeight <= *height)
                    ; /* La boite est a l'interieur de la page */
                 else if (org >= *height) {
                      /* Il faut memoriser la boite de coupure coupee */
                      pBox->BxAbstractBox->AbBox->BxMoved = pBox;
                      if (pBox == pFirstBox)
                         /* La boite est hors page */
                         OutOfPage (pAb, height, isPageBreakChanged);
                      else if (pPreviousBox->BxType == BoDotted && pPreviousBox->BxNSpaces == 0) {
                           /* La derniere boite de la page est hyphenee */
                           /* et n'est pas secable sur un blanc */
                           if (pPreviousBox == pFirstBox)
                              /* Le pave est note hors de la page */
                              OutOfPage (pAb, height, isPageBreakChanged);
                           else {
                                /* deplace la limite de page */
                                *height = pPreviousBox->BxYOrg;
                                *isPageBreakChanged = TRUE;
						   }
					  } else /* La boite est sur la limite de page */
                             OnPage (pAb, height, isPageBreakChanged);
                      toContinue = FALSE;
				 } else { 
                        /* La boite est sur la limite de page */
                        /* deplace la limite de page sur l'origine de la boite */
                        *height = org;
                        *isPageBreakChanged = TRUE;
				 }
		  
                 pPreviousBox = pBox;
                 pBox = pBox->BxNexChild;
		   }
		} else {
               /* --- cas genenral ----------------------------------------- */
               /* Si la boite composee n'est pas eclatee */
               if (pBox->BxType != BoGhost) {
                  /* Origine de la boite de coupure */
                  org = pBox->BxYOrg;
                  if (org + pBox->BxHeight <= *height) {
                     /* La boite est dans la page */
                     pAb->AbAfterPageBreak = FALSE;
                     pAb->AbOnPageBreak = FALSE;
				  } else if (org >= *height) /* La boite est hors page */
                         OutOfPage (pAb, height, isPageBreakChanged);
                  else if (!pAb->AbAcceptPageBreak || pAb->AbLeafType == LtText || pAb->AbLeafType == LtSymbol) {
                       /* La boite est sur la limite de page mais non secable */
                       /* deplace la limite de page sur l'origine de la boite */
                       *height = org;
                       *isPageBreakChanged = TRUE;
				  } else if (pAb->AbVertEnclosing) {
                         /* La boite est sur la limite de page, secable et englobee */
                         if (pAb->AbFirstEnclosed == NULL)
                            /* attend la boite terminale pour remonter l'indicateur */
                            OnPage (pAb, height, isPageBreakChanged);
				  } else {
                         /* La boite est sur la limite de page, secable et non englobee */
                         pAb->AbOnPageBreak = TRUE;
                         pAb->AbAfterPageBreak = FALSE;
				  }
			   } else { /*if != BoGhost */
                      pAb->AbOnPageBreak = FALSE;
                      pAb->AbAfterPageBreak = FALSE;
			   }
	      
               /* traite les paves fils */
               pChildAb = pAb->AbFirstEnclosed;
               if (pAb->AbBox->BxType == BoTable)
                  table = pAb;
               /* Ce n'est pas la peine de continuer le calcul */
               /* des coupures de boites quand la limite de    */
               /* page est deplacee */
               while (pChildAb != NULL && !*isPageBreakChanged) {
                            SetPageIndicators (pChildAb, table, height, isPageBreakChanged);
                            /* passe au suivant */
                            pChildAb = pChildAb->AbNext;
			   }
		}
	 }
  }
}


/*----------------------------------------------------------------------
   SetPageBreakPosition teste la position d'un pave' par rapport a`  
   la limite de page. Elle rend Vrai si la boi^te du pave' 
   est incluse dans la page. Sinon le pave' est coupe'     
   par la limite ou se situe au dela` de la limite et les  
   indicateurs correspondants du pave' sont positionne's.  
   Le parame`tre page donne la position de la limite de    
   page.                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            SetPageBreakPosition (PtrAbstractBox pAb, int *page)
#else  /* __STDC__ */
ThotBool            SetPageBreakPosition (pAb, page)
PtrAbstractBox      pAb;
int                *page;

#endif /* __STDC__ */
{
  PtrAbstractBox      table;
  int                 height;
  ThotBool            result;

   /* height = PixelValue (*page, UnPoint, pAb, 0); */
   height = *page;
   result = TRUE;

   /* look at if there is a table ancestor */
   if (pAb->AbBox->BxType == BoTable)
     table = SearchEnclosingType (pAb->AbEnclosing, BoTable);
   else
     table = SearchEnclosingType (pAb, BoTable);
   /* Tant que la limite de page change on recalcule */
   /* quelles sont les boites coupees */
   while (result)
      SetPageIndicators (pAb, table, &height, &result);
   result = !pAb->AbOnPageBreak;
   /* Faut-il traduire la hauteur de page ? */
   /* ******  *page = LogicalValue (height, UnPoint, pAb, 0); *******/
   *page = height;
   return result;
}

/*----------------------------------------------------------------------
   AddBoxTranslations met a` jour toutes les origines des boi^tes    
   correpondant aux pave's inclus dans pAb.                
   A chaque niveau la proce'dure additionnne le de'calage  
   de la boi^te englobante aux origines des boi^tes        
   incluses, en X et en Y selon la valeur de l'indicateur  
   horizRef et vertRef et du status de la boi^te englobe'e 
   Si ne'cessaire, la proce'dure ve'rifie l'englobement.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddBoxTranslations (PtrAbstractBox pAb, int visibility, int frame, ThotBool horizRef, ThotBool vertRef)
#else  /* __STDC__ */
void                AddBoxTranslations (pAb, visibility, frame, horizRef, vertRef)
PtrAbstractBox      pAb;
int                 visibility;
int                 frame;
ThotBool            horizRef;
ThotBool            vertRef;

#endif /* __STDC__ */
{
   int                 x, y, i;
   PtrBox              box1;
   PtrBox              pChildBox;
   PtrBox              pBox;
   BoxRelation        *pRelation;
   PtrAbstractBox      pChildAb;
   PtrPosRelations     pPosRel;
   int                 width, height;
   ThotBool            eclate;
   ThotBool            Peclate;
   ThotBool            notEmpty;
   ThotBool            newX;
   ThotBool            newY;
   ThotBool            placeenX;
   ThotBool            placeenY;
   ThotBool            reenglobx;
   ThotBool            reengloby;

   /* Origine de la boite du pave le plus englobant */
   pBox = pAb->AbBox;
   x = pBox->BxXOrg;
   y = pBox->BxYOrg;
   width = pBox->BxW;
   height = pBox->BxH;
   Peclate = (pBox->BxType == BoGhost);
   pChildAb = pAb->AbFirstEnclosed;

   /* Indique s'il faut reevaluer l'englobement du contenu apres mise a jour */
   reenglobx = FALSE;
   reengloby = FALSE;
   /* horizRef et vertRef indiquent que la boite mere (pBox) transmet son decalage */
   /* newX et newY indiquent que la boite fille (pChildBox) accepte le decalage */
   /* placeenX et placeenY indiquent que la boite fille transmet son decalage */

   /* Transforme origines relatives des boites filles en origines absolues */
   if (pAb->AbVisibility >= visibility)
      while (pChildAb != NULL)
	{

	   pChildBox = pChildAb->AbBox;
	   if (pChildBox != NULL)
	     {
		eclate = Peclate || pChildBox->BxType == BoGhost;

		/* Decale boites englobees dont l'origine depend de l'englobante */
		/* La boite est coupee, on decale les boites de coupure */
		if (pChildBox->BxType == BoSplit)
		  {
		     box1 = pChildBox->BxNexChild;
		     while (box1 != NULL)
		       {
			  if (horizRef)
			     box1->BxXOrg += x;
			  if (vertRef)
			     box1->BxYOrg += y;
			  box1 = box1->BxNexChild;
		       }
		  }
		else
		  {
		     /* S'il s'agit d'un bloc de ligne elastique */
		     /* il faut reevaluer l'englobement vertical */
		     if (pChildBox->BxType == BoBlock && pChildBox->BxHorizFlex)
			reengloby = TRUE;
		     /* Regarde si la boite est positionnee en X dans l'englobante */
		     box1 = GetHPosRelativePos (pChildBox, NULL);
		     placeenX = pChildBox->BxXToCompute;
		     if (box1 == NULL)
			newX = TRUE;
		     else if (pChildBox->BxXOutOfStruct)
			newX = FALSE;
		     else if (box1->BxHorizFlex && box1 != pChildBox)
			/* Le decalage des boites voisines liees a la boite elastique */
			/* est deja effectue par l'appel de MoveBoxEdge */
			newX = FALSE;
		     else
			newX = TRUE;

		     /* regarde si la boite doit etre placee en X absolu */
		     if (horizRef)
		       {
			  if (newX)
			    {
			       /* Si le deplacement est nul on ne peut executer XMove */
			       if (x == 0)
				  newX = FALSE;
			    }
			  /* Il faut placer les boites positionnees par rapport la racine */
			  else if (pChildAb->AbHorizPos.PosAbRef == ViewFrameTable[frame - 1].FrAbstractBox)
			     placeenX = TRUE;
		       }
		     else
			placeenX = FALSE;

		     /* regarde si la boite est positionnee en Y dans l'englobante */
		     box1 = GetVPosRelativeBox (pChildBox, NULL);
		     placeenY = pChildBox->BxYToCompute;
		     if (box1 == NULL)
			newY = TRUE;
		     else if (pChildBox->BxYOutOfStruct)
			newY = FALSE;
		     else if (box1->BxVertFlex && box1 != pChildBox)
			/* Le decalage des boites voisines liees a la boite elastique */
			/* est deja effectue par l'appel de MoveBoxEdge */
			newY = FALSE;
		     else
			newY = TRUE;

		     /* regarde si la boite doit etre placee en Y absolu */
		     if (vertRef)
		       {

			  if (newY)
			    {
			       /* Si le deplacement est nul on ne peut executer YMove */
			       if (y == 0)
				  newY = FALSE;
			    }
			  /* Il faut placer les boites positionnee par rapport la racine */
			  else if (pChildAb->AbVertPos.PosAbRef == ViewFrameTable[frame - 1].FrAbstractBox)
			     placeenY = TRUE;
		       }
		     else
			placeenY = FALSE;

		     /* Le contenu des boites elastiques et hors-structures est deja place */
		     if (pChildBox->BxHorizFlex || pChildBox->BxXOutOfStruct)
		       {
			  placeenX = FALSE;
			  /*if (pChildBox->BxType == BoBlock) */
			  /* La reevaluation du bloc de lignes place le contenu en Y */
			  /*placeenY = FALSE; */
		       }
		     if (pChildBox->BxVertFlex || pChildBox->BxYOutOfStruct)
			placeenY = FALSE;

		     /* decale la boite positionnee en X dans l'englobante */
		     if (horizRef && newX)
		       {
			  i = pChildBox->BxXOrg + pChildBox->BxW - width;
			  /* regarde s'il s'agit d'une boite elastique */
			  if (pChildBox->BxHorizFlex)
			    {
			       /* Initialise la file des boites deplacees */
			       box1 = pChildAb->AbHorizPos.PosAbRef->AbBox;
			       box1->BxMoved = NULL;
			       /* Pas de deplacement du contenu des boites qui */
			       /*  dependent de la boite elastique             */
			       MoveBoxEdge (pChildBox, box1, OpHorizDep, x, frame, TRUE);
			    }
			  else if (!placeenX)
			     /* il faut deplacer tout le contenu de la boite */
			     XMoveAllEnclosed (pChildBox, x, frame);
			  else
			     pChildBox->BxXOrg += x;

			  pChildBox->BxXToCompute = FALSE;	/* La boite est placee */

			  /* detecte les debordements de la boite englobante */
			  if (pChildAb->AbHorizEnclosing
			      && !eclate
			      && i > 1
			      && !pChildBox->BxHorizFlex
			      && !pBox->BxHorizFlex)
			    {
			       if (HighlightBoxErrors)
				  fprintf(stderr, "Box overflow %s/n", AbsBoxType (pChildAb, TRUE));
			    }
			  /* Decale les boites qui ont des relations hors-structure avec */
			  /* la boite deplacee et met a jour les dimensions elastiques   */
			  /* des boites liees a la boite deplacee.                       */
			  pPosRel = pChildBox->BxPosRelations;
			  while (pPosRel != NULL)
			    {
			       i = 1;
			       notEmpty = (pPosRel->PosRTable[i - 1].ReBox != NULL);
			       /* Si la boite est elastique, les relations */
			       /* hors-structure sont deja traitees.       */
			       if (!pChildBox->BxHorizFlex)
				  while (i <= MAX_RELAT_POS && notEmpty)
				    {
				       pRelation = &pPosRel->PosRTable[i - 1];
				       if (pRelation->ReBox->BxAbstractBox != NULL)
					 {
					    /* Initialise la file des boites deplacees */
					    pChildBox->BxMoved = NULL;
					    /* Relation hors-struture sur l'origine de la boite */
					    if (pRelation->ReOp == OpHorizDep
						&& pRelation->ReBox->BxXOutOfStruct
						&& pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pChildAb)
					      {
						 if (pRelation->ReBox->BxHorizFlex)
						    MoveBoxEdge (pRelation->ReBox, pChildBox, pRelation->ReOp, x, frame, TRUE);
						 else
						    XMove (pRelation->ReBox, pChildBox, x, frame);
						 /* La boite distante est placee */
						 pRelation->ReBox->BxXToCompute = FALSE;
					      }
					    /* Relation sur la largeur elastique de la boite */
					    else if (pRelation->ReOp == OpWidth)
					      {
						 /* Pas de deplacement du contenu des boites qui */
						 /*  dependent de la boite elastique             */
						 MoveBoxEdge (pRelation->ReBox, pChildBox, pRelation->ReOp, x, frame, TRUE);
					      }
					 }
				       i++;
				       if (i <= MAX_RELAT_POS)
					  notEmpty = (pPosRel->PosRTable[i - 1].ReBox != NULL);
				    }
			       pPosRel = pPosRel->PosRNext;
			    }
		       }
		     /* ne decale pas la boite, mais le fait de deplacer */
		     /* l'englobante sans deplacer une englobee peut        */
		     /* modifier la largeur de la boite englobante.         */
		     else if (horizRef && pChildBox->BxXOutOfStruct)
			reenglobx = TRUE;
		     /* traite les relations hors-structures des boites non */
		     /* decalees mais qui doivent etre placees en X absolu     */
		     if (placeenX && !newX)
		       {
			  SetXCompleteForOutOfStruct (pChildBox, visibility, frame);
			  /* La boite est placee */
			  pChildBox->BxXToCompute = FALSE;
		       }
		     /* decale la boite positionnee en Y dans l'englobante */
		     if (vertRef && newY)
		       {
			  i = pChildBox->BxYOrg + pChildBox->BxH - height;

			  /* regarde s'il s'agit d'une boite elastique */
			  if (pChildBox->BxVertFlex)
			    {
			       /* Initialise la file des boites deplacees */
			       box1 = pChildAb->AbVertPos.PosAbRef->AbBox;
			       box1->BxMoved = NULL;
			       /* Pas de deplacement du contenu des boites qui */
			       /*  dependent de la boite elastique */
			       MoveBoxEdge (pChildBox, box1, OpVertDep, y, frame, FALSE);
			    }
			  else if (!placeenY)
			     /* il faut deplacer tout le contenu de la boite */
			     YMoveAllEnclosed (pChildBox, y, frame);
			  else
			     pChildBox->BxYOrg += y;
			  /* La boite est placee */
			  pChildBox->BxYToCompute = FALSE;
			  /* detecte les debordements en Y de la boite englobante */
			  if (pChildAb->AbVertEnclosing
			      && !eclate
			      && i > 1
			      && !pChildBox->BxVertFlex
			      && !pBox->BxVertFlex)
			    {
			       if (HighlightBoxErrors)
				  fprintf (stderr, "Box overflow %s\n", AbsBoxType (pChildAb, TRUE));
			    }
			  /* Decale les boites qui ont des relations hors-structure avec */
			  /* la boite deplacee et met a jour les dimensions elastiques   */
			  /* des boites liees a la boite deplacee.                       */
			  pPosRel = pChildBox->BxPosRelations;
			  while (pPosRel != NULL)
			    {
			       i = 1;
			       notEmpty = (pPosRel->PosRTable[i - 1].ReBox != NULL);
			       /* Si la boite est elastique, les relations */
			       /* hors-structure sont deja traitees.       */
			       if (!pChildBox->BxVertFlex)
				  while (i <= MAX_RELAT_POS && notEmpty)
				    {
				       pRelation = &pPosRel->PosRTable[i - 1];
				       if (pRelation->ReBox->BxAbstractBox != NULL)
					 {
					    /* Initialise la file des boites deplacees */
					    pChildBox->BxMoved = NULL;
					    /* Relation hors-struture sur l'origine de la boite */
					    if (pRelation->ReOp == OpVertDep
						&& pRelation->ReBox->BxYOutOfStruct
						&& pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pChildAb)
					      {
						 if (pRelation->ReBox->BxVertFlex)
						    MoveBoxEdge (pRelation->ReBox, pChildBox, pRelation->ReOp, y, frame, FALSE);
						 else
						    YMove (pRelation->ReBox, pChildBox, y, frame);
						 /* La boite distante est placee */
						 pRelation->ReBox->BxYToCompute = FALSE;
					      }
					    /* Relation sur la hauteur elastique de la boite */
					    else if (pRelation->ReOp == OpHeight)
					      {
						 /* Pas de deplacement du contenu des boites qui */
						 /*  dependent de la boite elastique             */
						 MoveBoxEdge (pRelation->ReBox, pChildBox, pRelation->ReOp, y, frame, FALSE);
					      }
					 }
				       i++;
				       if (i <= MAX_RELAT_POS)
					  notEmpty = (pPosRel->PosRTable[i - 1].ReBox != NULL);
				    }
			       pPosRel = pPosRel->PosRNext;
			    }
		       }
		     /* ne decale pas la boite, mais le fait de deplacer */
		     /* l'englobante sans deplacer une englobee peut        */
		     /* modifier la hauteur de la boite englobante.         */
		     else if (vertRef && pChildBox->BxYOutOfStruct)
			reengloby = TRUE;

		     /* traite les relations hors-structures des boites non */
		     /* decalees mais qui doivent etre placees en Y absolu     */
		     if (placeenY && !newY)
		       {
			  SetYCompleteForOutOfStruct (pChildBox, visibility, frame);
			  /* La boite est placee */
			  pChildBox->BxYToCompute = FALSE;
		       }
		     /* traite les origines des boites de niveau inferieur */
		     if (placeenX || placeenY)
			AddBoxTranslations (pChildAb, visibility, frame, placeenX, placeenY);
		  }
	     }
	   /* passe au suivant */
	   pChildAb = pChildAb->AbNext;
	}
   /* Si une dimension de la boite depend du contenu et qu'une des  */
   /* boites filles est positionnee par une relation hors-structure */
   /* --> il faut reevaluer la dimension correspondante.            */
   if (reenglobx && pBox->BxContentWidth)
      RecordEnclosing (pBox, TRUE);

   if (reengloby && pBox->BxContentHeight)
      RecordEnclosing (pBox, FALSE);
}


/*----------------------------------------------------------------------
   SetPageHeight indique quelles sont les conditions de coupure du   
   pave' passe' en parame`tre :                            
   - ht = hauteur de la boi^te du pave'.                   
   - pos = position de la boite du pave dans la page.      
   - nChars = nombre de caracte`res du pave' qui entrent    
   dans la page s'il est de type texte, sinon le volume du 
   pave'.                                                  
   Les hauteurs sont exprime'es suivant la valeur du       
   parame`tre pointVal, en points typographiques (valeur Vrai) 
   ou en unite's logiques (Valeur Faux).                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetPageHeight (PtrAbstractBox pAb, ThotBool pointVal, int *ht, int *pos, int *nChars)
#else  /* __STDC__ */
void                SetPageHeight (pAb, pointVal, ht, pos, nChars)
PtrAbstractBox      pAb;
ThotBool            pointVal;
int                *ht;
int                *pos;
int                *nChars;

#endif /* __STDC__ */
{
   PtrBox              box1;
   PtrBox              pChildBox;
   PtrTextBuffer       adbuff;
   int                 height;
   int                 org, i;
   int                 hfont;
   ThotBool            still;

   *nChars = 0;
   *pos = 0;
   *ht = 0;
   /* calcule la position et la hauteur du pave */
   box1 = pAb->AbBox;
   if (box1 != NULL)
     {

	/* La boite du pave est coupee en lignes */
	if (box1->BxType == BoSplit)
	  {
	     pChildBox = box1->BxNexChild;
	     org = pChildBox->BxYOrg;

	     /* recherche la derniere boite de coupure */
	     while (pChildBox->BxNexChild != NULL)
		pChildBox = pChildBox->BxNexChild;

	     height = pChildBox->BxYOrg + pChildBox->BxHeight - org;

	     /* BxMoved est la boite sur laquelle passe la limite */
	     box1 = box1->BxMoved;

	     /* Nombre de caracteres qui entrent dans la page */
	     if (pAb->AbOnPageBreak && box1 != NULL)
	       {
		  *nChars = box1->BxIndChar;
		  /* Il ne faut pas couper le dernier mot d'une page     */
		  /* donc si la boite precedente est de type BtAvectrait */
		  /* la limite de la page est deplacee sur le blanc qui  */
		  /* precede ce mot */
		  if (*nChars != 0)
		     if (box1->BxPrevious->BxType == BoDotted)
			if (box1->BxPrevious->BxNSpaces != 0)
			  {
			     /* recheche en arriere le blanc precedent */
			     adbuff = box1->BxBuffer;
			     i = box1->BxFirstChar - 1;
			     still = TRUE;
			     while (still)
				if (adbuff->BuContent[i] == SPACE)
				  {
				     /* a trouve le blanc */
				     still = FALSE;
				     /* Debute le nouveau mot au caractere suivant */
				     (*nChars)++;
				  }
				else
				  {
				     (*nChars)--;
				     if (i == 0)
					if (adbuff->BuPrevious != NULL)
					  {
					     adbuff = adbuff->BuPrevious;
					     i = adbuff->BuLength - 1;
					  }
					else
					   /* arrete */
					   still = FALSE;
				     else
					i--;
				  }
			  }
	       }
	  }
	/* La boite du pave est eclatee sur plusieurs lignes */
	else if (box1->BxType == BoGhost)
	  {
	     /* Il faut descendre tant que l'on a des boites eclatees */
	     while (box1->BxType == BoGhost)
		box1 = box1->BxAbstractBox->AbFirstEnclosed->AbBox;
	     /* prend la position de la premiere boite */
	     if (box1->BxType == BoSplit)
		/* Il faut prendre la position de la 1ere boite de coupure */
		box1 = box1->BxNexChild;
	     org = box1->BxYOrg;
	     height = 0;
	     while (box1 != NULL)
	       {
		  /* prend la limite inferieur */
		  if (box1->BxType == BoPiece)
		     /* il faut aller chercher la derniere boite de coupure */
		     while (box1->BxNexChild != NULL)
			box1 = box1->BxNexChild;
		  i = box1->BxYOrg + box1->BxHeight;
		  if (i > height)
		     height = i;
		  if (box1->BxAbstractBox->AbNext == NULL)
		     box1 = NULL;
		  else
		     box1 = box1->BxAbstractBox->AbNext->AbBox;
	       }
	     /* La hauteur de la boite eclatee */
	     height -= org;
	  }
	else
	  {
	     org = box1->BxYOrg;
	     height = box1->BxHeight;
	  }

	/* traduit les valeurs pixel dans l'unite demandee */
	if (pointVal)
	  {
	     *pos = org;
	     *ht = height;
	  }
	else
	  {
	     hfont = FontHeight (box1->BxFont);
	     *pos = org * 10 / hfont;
	     *ht = height * 10 / hfont;
	  }
     }
}


/*----------------------------------------------------------------------
   SetBoxToTranslate marque le sous-arbre des paves en cours de    
   placement.                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetBoxToTranslate (PtrAbstractBox pAb, ThotBool horizRef, ThotBool vertRef)
#else  /* __STDC__ */
void                SetBoxToTranslate (pAb, horizRef, vertRef)
PtrAbstractBox      pAb;
ThotBool            horizRef;
ThotBool            vertRef;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrBox              pBox;

   pBox = pAb->AbBox;
   if (pBox == NULL)
      return;

   /* Les boites englobees des boites elastiques */
   /* sont toujours placees en absolue           */
   if (pBox->BxHorizFlex || pBox->BxXOutOfStruct)
      horizRef = FALSE;
   if (pBox->BxVertFlex || pBox->BxYOutOfStruct)
      vertRef = FALSE;

   if (pBox->BxType != BoSplit)
     {
	pBox->BxXToCompute = horizRef;
	pBox->BxYToCompute = vertRef;
     }

   /* Marque les paves englobes */
   pChildAb = pAb->AbFirstEnclosed;
   while (pChildAb != NULL)
     {
	SetBoxToTranslate (pChildAb, horizRef, vertRef);
	pChildAb = pChildAb->AbNext;
     }
}
