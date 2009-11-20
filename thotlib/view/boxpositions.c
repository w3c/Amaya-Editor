/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module dedicated to box positions managing.
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#ifdef _GL
#include "frame.h"
#include "content.h"
#include "glwindowdisplay.h"
#endif /* _GL */

#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "boxes_tv.h"
#include "platform_tv.h"
#ifdef _GL
#include "frame_tv.h"
#endif /* _GL */

#include "memory_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "boxpositions_f.h"
#include "font_f.h"
#include "exceptions_f.h"
#ifdef _GL
#include "appli_f.h"
#include "contentapi_f.h"
#endif /* _GL */
#include "tableH_f.h"
#include "units_f.h"

/*----------------------------------------------------------------------
  ExtraAbFlow returns TRUE if the abstract box is ignored by the
  standard flow.
  Return TRUE for PnAbsolute or PnFixed.
  ----------------------------------------------------------------------*/
ThotBool ExtraAbFlow (PtrAbstractBox pAb, int frame)
{
  if (pAb && frame > 0 &&
      pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
      pAb->AbLeafType == LtCompound &&
      pAb->AbPositioning &&
      (pAb->AbPositioning->PnAlgorithm == PnAbsolute ||
       pAb->AbPositioning->PnAlgorithm == PnFixed))
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  VertExtraAbFlow returns TRUE if the abstract box is ignored by the
  vertical standard flow.
  Return TRUE for PnAbsolute or PnFixed and Y positioning.
  ----------------------------------------------------------------------*/
ThotBool VertExtraAbFlow (PtrAbstractBox pAb, int frame)
{
  Positioning        *pos;

  if (pAb && frame > 0 &&
      pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
      pAb->AbLeafType == LtCompound &&
      pAb->AbPositioning &&
      (pAb->AbPositioning->PnAlgorithm == PnAbsolute ||
       pAb->AbPositioning->PnAlgorithm == PnFixed))
    {
      pos = pAb->AbPositioning;
      if ((pos->PnTopUnit == UnAuto || pos->PnTopUnit == UnUndefined) &&
          (pos->PnBottomUnit == UnAuto || pos->PnBottomUnit == UnUndefined))
        return FALSE;
      else
        return TRUE;
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  HorizExtraAbFlow returns TRUE if the abstract box is ignored by the
  horizontal standard flow.
  Return TRUE for PnAbsolute or PnFixed and X positioning.
  ----------------------------------------------------------------------*/
ThotBool HorizExtraAbFlow (PtrAbstractBox pAb, int frame)
{
  Positioning        *pos;

  if (pAb && frame > 0 &&
      pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
      pAb->AbLeafType == LtCompound &&
      pAb->AbPositioning &&
      (pAb->AbPositioning->PnAlgorithm == PnAbsolute ||
       pAb->AbPositioning->PnAlgorithm == PnFixed))
    {
      pos = pAb->AbPositioning;
      if ((pos->PnLeftUnit == UnAuto || pos->PnLeftUnit == UnUndefined) &&
          (pos->PnRightUnit == UnAuto || pos->PnRightUnit == UnUndefined))
        return FALSE;
      else
        return TRUE;
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  ExtraFlow returns TRUE if the box is ignored by the standard flow.
  Return TRUE for PnAbsolute or PnFixed.
  ----------------------------------------------------------------------*/
ThotBool ExtraFlow (PtrBox pBox, int frame)
{
  if (pBox)
    return ExtraAbFlow (pBox->BxAbstractBox, frame);
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  IsFlow says if the box is displayed in a different flow.
  Return TRUE for PnAbsolute, PnFixed or PnRelative (if shifted).
  ----------------------------------------------------------------------*/
ThotBool IsFlow (PtrBox pBox, int frame)
{
  Positioning        *pos;

  if (pBox && pBox->BxAbstractBox && frame > 0 &&
      pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
      pBox->BxAbstractBox->AbLeafType == LtCompound &&
      pBox->BxAbstractBox->AbPositioning)
    {
      pos = pBox->BxAbstractBox->AbPositioning;
      if (pos->PnAlgorithm == PnRelative &&
          (pos->PnTopUnit == UnAuto ||
           pos->PnTopUnit == UnUndefined) &&
          (pos->PnBottomUnit == UnAuto ||
           pos->PnBottomUnit == UnUndefined) &&
          (pos->PnLeftUnit == UnAuto ||
           pos->PnLeftUnit == UnUndefined) &&
          (pos->PnRightUnit == UnAuto ||
           pos->PnRightUnit == UnUndefined))
        return TRUE;
      else if ((pos->PnLeftUnit != UnUndefined ||
                pos->PnRightUnit != UnUndefined ||
                pos->PnTopUnit != UnUndefined ||
                pos->PnBottomUnit != UnUndefined) &&
               (pos->PnAlgorithm == PnAbsolute ||
                pos->PnAlgorithm == PnFixed ||
                pos->PnAlgorithm == PnRelative))
        return TRUE;
      else
        return FALSE;
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  GetRelativeFlow returns TRUE if the box is an extra flow or a relative flow.
  ----------------------------------------------------------------------*/
PtrFlow GetRelativeFlow (PtrBox pBox, int frame)
{
  PtrFlow             pFlow;

  //#ifdef POSITIONING
  if (pBox && pBox->BxAbstractBox && frame > 0)
    {
      pFlow = ViewFrameTable[frame - 1].FrFlow;
      while (pFlow)
        {
          if (pFlow->FlRootBox && pFlow->FlRootBox->AbBox &&
              IsParentBox (pFlow->FlRootBox->AbBox, pBox))
            return pFlow;
          pFlow = pFlow->FlNext;
        }     
    }
  //#endif /* POSITIONING */
  return NULL;
}


/*----------------------------------------------------------------------
  SetYCompleteForOutOfStruct checks if an external box has a Y position
  that depends on that box.
  ----------------------------------------------------------------------*/
static void SetYCompleteForOutOfStruct (PtrBox pBox, int visibility,
                                        int y, int frame)
{
  PtrPosRelations     pPosRel;
  PtrAbstractBox      pAb = pBox->BxAbstractBox;
  BoxRelation        *pRel;
  ThotBool            notEmpty;
  int                 i;

  pPosRel = pBox->BxPosRelations;
  /* Si la boite est elastique, les relations */
  /* hors-structure sont deja traitees.       */
  if (!pBox->BxVertFlex)
    while (pPosRel)
    {
      i = 0;
      notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
      while (i < MAX_RELAT_POS && notEmpty)
        {
          pRel = &pPosRel->PosRTable[i];
          if (pRel->ReBox->BxAbstractBox &&
              // don't move children of a new system origin
              !IsSystemOrigin (pAb, frame))
            {
              /* Relation hors-struture sur l'origine de la boite */
              if (pRel->ReOp == OpVertDep &&
                  pRel->ReBox->BxYOutOfStruct &&
                  pRel->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pAb)
                {
                  if (pRel->ReBox->BxVertFlex)
                    MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, y, frame, FALSE);
                  else if (!IsParentBox (pRel->ReBox, pBox))
                    /* don't move the enclosing box pBox */
                    YMoveAllEnclosed (pRel->ReBox, y, frame);
                  /* The rel box position is now computed */
                  pRel->ReBox->BxYToCompute = FALSE;
                }
              /* Relation sur la hauteur elastique de la boite */
              else if (pRel->ReOp == OpHeight)
                /* Pas de deplacement du contenu des boites qui */
                /*  dependent de la boite elastique             */
                  MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, y, frame, FALSE);
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
  SetXCompleteForOutOfStruct checks if an external box has a X position
  that depends on that box.
  ----------------------------------------------------------------------*/
static void SetXCompleteForOutOfStruct (PtrBox pBox, int visibility,
                                        int x, int frame)
{
  PtrPosRelations     pPosRel;
  PtrAbstractBox      pAb = pBox->BxAbstractBox;
  BoxRelation        *pRel;
  ThotBool            notEmpty;
  int                 i;

  pPosRel = pBox->BxPosRelations;
  if (!pBox->BxHorizFlex)
    while (pPosRel)
    {
      i = 0;
      notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
      while (i < MAX_RELAT_POS && notEmpty)
        {
          pRel = &pPosRel->PosRTable[i];
          if (pRel->ReBox->BxAbstractBox &&
              // don't move children of a new system origin
              (!IsParentBox (pBox, pRel->ReBox) || !IsSystemOrigin (pAb, frame)))
            {
              /* Relation hors-struture sur l'origine de la boite */
              if (pRel->ReOp == OpHorizDep &&
                  pRel->ReBox->BxXOutOfStruct &&
                  pRel->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pAb)
                {
                  /* Initialise la file des boites deplacees */
                  pBox->BxMoved = NULL;
                  if (pRel->ReBox->BxHorizFlex)
                    MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, x, frame, TRUE);
                  else if (!IsParentBox (pRel->ReBox, pBox))
                    /* don't move the enclosing box pBox */
                    XMoveAllEnclosed (pRel->ReBox, x, frame);
                  /* The rel box position is now computed */
                  pRel->ReBox->BxXToCompute = FALSE;
                }
              /* Relation sur la largeur elastique de la boite */
              else if (pRel->ReOp == OpWidth)
                MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, x, frame, TRUE);
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
static void OnPage (PtrAbstractBox pAb, int *height, ThotBool *isPageBreakChanged)
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
  GetEnclosingGather
  Returns the highest enclosing abstract box that forces a build all.
  ----------------------------------------------------------------------*/
PtrAbstractBox GetEnclosingGather (PtrAbstractBox pAb)
{
  PtrAbstractBox found = NULL;

  while (pAb)
    {
      if (pAb->AbBuildAll)
        found = pAb;
      pAb = pAb->AbEnclosing;
    }
  if (found && found->AbFloat != 'N')
    found = found->AbEnclosing;
  return found;
}

/*----------------------------------------------------------------------
  OutOfPage marque tous les paves ascendants comme coupe's     
  par la limite de page s'ils ne sont pas de'ja`          
  marque's comme sur la page ou hors de la page et si     
  le pave' est englobe' verticalement.                    
  ----------------------------------------------------------------------*/
static void OutOfPage (PtrAbstractBox pAb, int *height, ThotBool *isPageBreakChanged)
{
  PtrAbstractBox      pParentAb;

  if (pAb != NULL)
    {
      pAb->AbOnPageBreak = FALSE;
      pAb->AbAfterPageBreak = TRUE;
      pParentAb = pAb->AbEnclosing;
      if (pAb->AbVertEnclosing && pParentAb != NULL)
        {
          /* Le pere est sur la page ou hors de la page */
          if (pParentAb->AbBox->BxType == BoGhost ||
              pParentAb->AbBox->BxType == BoStructGhost)
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
static void SetPageIndicators (PtrAbstractBox pAb, PtrAbstractBox table,
                               int *height, ThotBool *isPageBreakChanged)
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
  if (!pAb->AbDead && pAb->AbBox != NULL)
    {
      /* verifie les limites de la boite du pave */
      pBox = pAb->AbBox;
      isCell = (table != NULL && pBox->BxType == BoCell);

      if (isCell)
        /* page break can be inserted only in the first column */
        IsFirstColumn (pAb, table, &toContinue);
      else
        toContinue = TRUE;
      if (toContinue)
        {
          if (pBox->BxType == BoSplit  || pBox->BxType == BoMulScript)
            {
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
              while (!*isPageBreakChanged && toContinue && pBox)
                {
                  /* Origine de la boite de coupure */
                  org = pBox->BxYOrg;
                  if (org + pBox->BxHeight <= *height)
                    ; /* La boite est a l'interieur de la page */
                  else if (org >= *height)
                    {
                      /* Il faut memoriser la boite de coupure coupee */
                      pBox->BxAbstractBox->AbBox->BxMoved = pBox;
                      if (pBox == pFirstBox)
                        /* La boite est hors page */
                        OutOfPage (pAb, height, isPageBreakChanged);
                      else if (pPreviousBox->BxType == BoDotted &&
                               pPreviousBox->BxNSpaces == 0)
                        {
                          /* La derniere boite de la page est hyphenee */
                          /* et n'est pas secable sur un blanc */
                          if (pPreviousBox == pFirstBox)
                            /* Le pave est note hors de la page */
                            OutOfPage (pAb, height, isPageBreakChanged);
                          else
                            {
                              /* deplace la limite de page */
                              *height = pPreviousBox->BxYOrg;
                              *isPageBreakChanged = TRUE;
                            }
                        }
                      else
                        /* La boite est sur la limite de page */
                        OnPage (pAb, height, isPageBreakChanged);
                      toContinue = FALSE;
                    }
                  else if (GetEnclosingGather (pAb) == NULL)
                    {
                      /* La boite est sur la limite de page */
                      /* deplace la limite de page sur l'origine de la boite */
                      *height = org;
                      *isPageBreakChanged = TRUE;
                    }
		  
                  pPreviousBox = pBox;
                  pBox = pBox->BxNexChild;
                }
            }
          else
            {
              /* --- cas genenral ----------------------------------------- */
              /* Si la boite composee n'est pas eclatee */
              if (pBox->BxType != BoGhost && pBox->BxType != BoStructGhost)
                {
                  /* Origine de la boite de coupure */
                  org = pBox->BxYOrg;
                  if (org + pBox->BxHeight <= *height)
                    {
                      /* La boite est dans la page */
                      pAb->AbAfterPageBreak = FALSE;
                      pAb->AbOnPageBreak = FALSE;
                    }
                  else if (org >= *height) /* La boite est hors page */
                    OutOfPage (pAb, height, isPageBreakChanged);
                  else if (!pAb->AbAcceptPageBreak || pAb->AbLeafType == LtText
                           || pAb->AbLeafType == LtSymbol)
                    {
                      /* La boite est sur la limite de page mais non secable */
                      /* deplace la limite de page sur l'origine de la boite */
                      *height = org;
                      *isPageBreakChanged = TRUE;
                    }
                  else if (pAb->AbVertEnclosing)
                    {
                      /* La boite est sur la limite de page, secable et englobee */
                      if (pAb->AbFirstEnclosed == NULL)
                        /* attend la boite terminale pour remonter l'indicateur */
                        OnPage (pAb, height, isPageBreakChanged);
                    }
                  else
                    {
                      /* La boite est sur la limite de page, secable et non englobee */
                      pAb->AbOnPageBreak = TRUE;
                      pAb->AbAfterPageBreak = FALSE;
                    }
                }
              else
                { /*if != BoGhost */
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
              while (pChildAb && !*isPageBreakChanged)
                {
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
ThotBool SetPageBreakPosition (PtrAbstractBox pAb, int *page)
{
  PtrAbstractBox      table;
  int                 height;
  ThotBool            result;

  /* height = PixelValue (*page, UnPoint, pAb, 0); */
  height = *page;
  result = TRUE;

  /* look at if there is a table ancestor */
  if (pAb->AbBox->BxType == BoTable)
    table = SearchEnclosingType (pAb->AbEnclosing, BoTable, BoFloatBlock, BoCellBlock);
  else
    table = SearchEnclosingType (pAb, BoTable, BoFloatBlock, BoCellBlock);
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
  AddBoxTranslations updates all included boxes of the abstract box
  pAb. The function adds the box position to enclosing box positions.
  Paramaters horizRef and vertRef are TRUE when the functions updates
  respectively the horizontal and the vertical position of children.
  The function checks also the status of the child box to know if the
  update is allowed or not.
  If necessary the enclosing of boxes is checked after the update.
  ----------------------------------------------------------------------*/
void AddBoxTranslations (PtrAbstractBox pAb, int visibility, int frame,
                         ThotBool horizRef, ThotBool vertRef)
{
  PtrBox              box1;
  PtrBox              pChildBox;
  PtrBox              pBox;
  PtrAbstractBox      pChildAb;
  int                 width, height;
  int                 x, y, i;
  ThotBool            ghost,  newX, newY;
  ThotBool            x_move = FALSE, y_move = FALSE;
  ThotBool            checkWidth, checkHeight;

  /* Origine de la boite du pave le plus englobant */
  pBox = pAb->AbBox;
  if (IsSystemOrigin (pAb, frame))
    {
      x = pBox->BxLMargin + pBox->BxLBorder;
      y = pBox->BxTMargin + pBox->BxTBorder;
    }
  else
    {
      x = pBox->BxXOrg;
      y = pBox->BxYOrg;
    }
#ifdef _GL
  pBox->BxClipX = x + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
  pBox->BxClipY = y + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding;
  pBox->BxClipW = pBox->BxW;
  pBox->BxClipH = pBox->BxH;
#endif /* _GL */

  width = pBox->BxW;
  height = pBox->BxH;
  pChildAb = pAb->AbFirstEnclosed;
  /* By default don't recheck parent sizes */
  checkWidth = FALSE;
  checkHeight = FALSE;
  /* horizRef et vertRef indiquent que la boite mere (pBox) transmet son decalage */
  /* newX et newY indiquent que la boite fille (pChildBox) accepte le decalage */
  /* x_move et y_move indiquent que la boite fille transmet son decalage */
  /* Transforme origines relatives des boites filles en origines absolues */
  if (pAb->AbVisibility >= visibility)
    while (pChildAb)
      {
        pChildBox = pChildAb->AbBox;
        if (pChildBox)
          {
            ghost = pBox->BxType == BoGhost || pBox->BxType == BoStructGhost ||
              pChildBox->BxType == BoGhost || pChildBox->BxType == BoStructGhost;
            /* Decale boites englobees dont l'origine depend de l'englobante */
            /* La boite est coupee, on decale les boites de coupure */
            if (pChildBox->BxType == BoSplit ||
                pChildBox->BxType == BoMulScript)
              {
                box1 = pChildBox->BxNexChild;
                while (box1)
                  {
                    if (horizRef)
                      {
                        box1->BxXOrg += x;
#ifdef _GL
                        box1->BxClipX += x;
#endif /* _GL */
                      }
                    if (vertRef)
                      {
                        box1->BxYOrg += y;
#ifdef _GL
                        box1->BxClipY += y;
#endif /* _GL */
                      }
                    box1 = box1->BxNexChild;
                  }
              }
            else
              {
                if (pChildBox->BxHorizFlex &&
                    (pChildBox->BxType == BoBlock ||
                     pChildBox->BxType == BoFloatBlock ||
                     pChildBox->BxType == BoCellBlock))
                  /* need to recheck the parent height */
                  checkHeight = TRUE;

                if (horizRef)
                  {
                    if (pChildBox->BxXOutOfStruct)
                      {
                        /* the box doesn't depend on the enclosing */
                        newX = FALSE;
                        x_move = FALSE;
                      }
                    else
                      {
                        /* streched boxes are already moved */
                        if (pChildBox->BxHorizFlex)
                          x_move = FALSE;
                        else
                          x_move = pChildBox->BxXToCompute;

                        /* Check if the box should be moved horizontally */
                        newX = TRUE;
                        box1 = GetHPosRelativeBox (pChildBox, NULL);
                        if (box1 == NULL)
                          newX = TRUE;
                        else if (box1->BxHorizFlex && box1 != pChildBox)
                          /* the box is moved with related boxes by MoveBoxEdge */
                          newX = FALSE;
                        if (newX && x == 0)
                          /* no move requested */
                          newX = FALSE;
                      }
                  }
                else
                  {
                    x_move = FALSE;
                    newX = FALSE;
                  }

                /* regarde si la boite doit etre placee en Y absolu */
                if (vertRef)
                  {
                    if (pChildBox->BxYOutOfStruct)
                      {
                        /* the box doesn't depend on enclosing */
                        newY = FALSE;
                        y_move = FALSE;
                      }
                    else
                      {
                        if (pChildBox->BxVertFlex)
                          y_move = FALSE;
                        else
                          y_move = pChildBox->BxYToCompute;

                        /* Check if the box should be moved vertically */
                        box1 = GetVPosRelativeBox (pChildBox, NULL);
                        if (box1 == NULL)
                          newY = TRUE;
                        else if (box1->BxVertFlex && box1 != pChildBox)
                          /* the box is moved with related boxes by MoveBoxEdge */
                          newY = FALSE;
                        else
                          newY = TRUE;
			
                        if (newY && y == 0)
                          /* no move requested */
                          newY = FALSE;
                      }
                  }
                else
                  {
                    y_move = FALSE;
                    newY = FALSE;
                  }

                /* decale la boite positionnee en X dans l'englobante */
                if (newX)
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
                    else if (!x_move)
                      /* il faut deplacer tout le contenu de la boite */
                      XMoveAllEnclosed (pChildBox, x, frame);
                    else
                      // floated boxes are already shifted
                      pChildBox->BxXOrg += x;
		    
                    pChildBox->BxXToCompute = FALSE;	/* La boite est placee */
                    /* detecte les debordements de la boite englobante */
                    if (pChildAb->AbHorizEnclosing
                        && !ghost
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
                    //SetXCompleteForOutOfStruct (pChildBox, visibility, x, frame);
                  }
                /* ne decale pas la boite, mais le fait de deplacer */
                /* l'englobante sans deplacer une englobee peut        */
                /* modifier la largeur de la boite englobante.         */
                else if (horizRef && pChildBox->BxXOutOfStruct)
                  checkWidth = TRUE;

                if (x_move)
                  {
                    /* Decale les boites qui ont des relations hors-structure avec */
                    /* la boite deplacee et met a jour les dimensions elastiques   */
                    /* des boites liees a la boite deplacee.                       */
                    SetXCompleteForOutOfStruct (pChildBox, visibility, x, frame);
                    /* La boite est placee */
                    pChildBox->BxXToCompute = FALSE;
                  }
                if (x && x_move &&
                    (pChildBox->BxType == BoBlock ||
                     pChildBox->BxType == BoFloatBlock ||
                     pChildBox->BxType == BoCellBlock))
                  // update included floated boxes
                  ShiftFloatingBoxes (pChildBox, x, frame);

                /* decale la boite positionnee en Y dans l'englobante */
                if (newY)
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
                    else if (!y_move)
                      /* il faut deplacer tout le contenu de la boite */
                      YMoveAllEnclosed (pChildBox, y, frame);
                    else
                      // floated boxes are already shifted
                      pChildBox->BxYOrg += y;
                    /* La boite est placee */
                    pChildBox->BxYToCompute = FALSE;
                    /* detecte les debordements en Y de la boite englobante */
                    if (pChildAb->AbVertEnclosing
                        && !ghost
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
                    //SetYCompleteForOutOfStruct (pChildBox, visibility, y, frame);
                  }
                /* ne decale pas la boite, mais le fait de deplacer */
                /* l'englobante sans deplacer une englobee peut        */
                /* modifier la hauteur de la boite englobante.         */
                else if (vertRef && pChildBox->BxYOutOfStruct)
                  checkHeight = TRUE;
		
                /* traite les relations hors-structures des boites non */
                /* decalees mais qui doivent etre placees en Y absolu     */
                if (y_move)
                  {
                    /* Decale les boites qui ont des relations hors-structure avec */
                    /* la boite deplacee et met a jour les dimensions elastiques   */
                    /* des boites liees a la boite deplacee.                       */
                    SetYCompleteForOutOfStruct (pChildBox, visibility, y, frame);
                    /* La boite est placee */
                    pChildBox->BxYToCompute = FALSE;
                  }

                /* traite les origines des boites de niveau inferieur */
                if (x_move || y_move)
                  AddBoxTranslations (pChildAb, visibility, frame, x_move, y_move);
              }
          }
#ifdef _GL
        if (pChildBox)
          {
            if (!IsSystemOrigin (pChildAb, frame))
              {
                /*if no transformation, make clipx, clipy OK*/
                pChildBox->BxClipX = pChildBox->BxXOrg + pChildBox->BxLMargin
                  + pChildBox->BxLBorder + pChildBox->BxLPadding;
                pChildBox->BxClipY = pChildBox->BxYOrg + pChildBox->BxTMargin
                  + pChildBox->BxTBorder + pChildBox->BxTPadding;
              }
            pChildBox->BxClipW = pChildBox->BxW;
            pChildBox->BxClipH = pChildBox->BxH;
            }
#endif /* _GL */

        /* next child */
        pChildAb = pChildAb->AbNext;
      }

  /* Si une dimension de la boite depend du contenu et qu'une des  */
  /* boites filles est positionnee par une relation hors-structure */
  /* --> il faut reevaluer la dimension correspondante.            */
  if (checkWidth && pBox->BxContentWidth)
    RecordEnclosing (pBox, TRUE);
  if (checkHeight && pBox->BxContentHeight)
    RecordEnclosing (pBox, FALSE);
}


/*----------------------------------------------------------------------
  SetPageHeight returns:
  - ht: the inside height of the box (in point or pixel).                   
  - pos: the position of the inside box (in point or pixel).      
  - nChars: the number of characters that can be inserted in the page
  or the volume of the box.
  ----------------------------------------------------------------------*/
void SetPageHeight (PtrAbstractBox pAb, int *ht, int *pos, int *nChars)
{
  PtrBox              box;
  PtrBox              pChildBox;
  PtrTextBuffer       adbuff;
  int                 height;
  int                 org, i;
  ThotBool            still;

  *nChars = 0;
  *pos = 0;
  *ht = 0;
  box = pAb->AbBox;
  if (box)
    {
      if (box->BxType == BoSplit || box->BxType == BoMulScript)
        {
          /* the box is split into different lines */
          pChildBox = box->BxNexChild;
          org = pChildBox->BxYOrg;
          /* look for the last piece */
          while (pChildBox->BxNexChild)
            pChildBox = pChildBox->BxNexChild;
	  
          height = pChildBox->BxYOrg + pChildBox->BxHeight - org;
          /* BxMoved gives the plit box */
          box = box->BxMoved;
          /* number of characters included in the page */
          if (pAb->AbOnPageBreak && box)
            {
              *nChars = box->BxFirstChar - 1;
              /* Il ne faut pas couper le dernier mot d'une page     */
              /* donc si la boite precedente est de type BoDotted */
              /* la limite de la page est deplacee sur le blanc qui  */
              /* precede ce mot */
              if (*nChars >= 0 &&
                  box->BxPrevious->BxType == BoDotted &&
                  box->BxPrevious->BxNSpaces)
                {
                  /* get the previous space */
                  adbuff = box->BxBuffer;
                  i = box->BxIndChar;
                  still = TRUE;
                  while (still)
                    if (adbuff->BuContent[i] == SPACE)
                      {
                        /* found */
                        still = FALSE;
                        /* the next word starts with the next character */
                        (*nChars)++;
                      }
                    else
                      {
                        (*nChars)--;
                        if (i == 0)
                          {
                            if (adbuff->BuPrevious)
                              {
                                adbuff = adbuff->BuPrevious;
                                i = adbuff->BuLength - 1;
                              }
                            else
                              /* stop */
                              still = FALSE;
                          }
                        else
                          i--;
                      }
                }
            }
        }
      else if (box->BxType == BoGhost || box->BxType == BoStructGhost)
        {
          /* check enclosed boxes */
          while (box->BxType == BoGhost || box->BxType == BoStructGhost)
            box = box->BxAbstractBox->AbFirstEnclosed->AbBox;
	  
          if (box->BxType == BoSplit || box->BxType == BoMulScript)
            box = box->BxNexChild;
          /* get the position of the first child */
          org = box->BxYOrg;
          height = 0;
          while (box != NULL)
            {
              /* get the bottom limit */
              if (box->BxType == BoSplit ||
                  box->BxType == BoMulScript ||
                  box->BxType == BoPiece ||
                  box->BxType == BoScript ||
                  box->BxType == BoDotted)
                /* look for the last piece */
                while (box->BxNexChild != NULL)
                  box = box->BxNexChild;
              i = box->BxYOrg + box->BxHeight;
              if (i > height)
                height = i;
              if (box->BxAbstractBox->AbNext == NULL)
                box = NULL;
              else
                box = box->BxAbstractBox->AbNext->AbBox;
            }
          /* box height */
          height -= org;
        }
      else
        {
          org = box->BxYOrg + box->BxTMargin + box->BxTBorder + box->BxTPadding;
          height = box->BxH;
        }
      
      *pos = org;
      *ht = height;
    }
}


/*----------------------------------------------------------------------
  SetBoxToTranslate marks boxes which are not already placed.
  ----------------------------------------------------------------------*/
void SetBoxToTranslate (PtrAbstractBox pAb, ThotBool horizRef, ThotBool vertRef)
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

  if (pBox->BxType != BoSplit && pBox->BxType != BoMulScript)
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
