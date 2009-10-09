/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Manage box selections
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "creation_tv.h"
#include "boxes_tv.h"
#include "frame_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"
#include "edit_tv.h"

#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxlocate_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "displayselect_f.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "scroll_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "viewapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/
/*----------------------------------------------------------------------
  ClearAbstractBoxSelection parcours l'arborescence pour annuler  
  toutes ls selections de pave.                           
  ----------------------------------------------------------------------*/
static void ClearAbstractBoxSelection (PtrAbstractBox pAb)
{
  PtrAbstractBox      pChildAb;
  PtrAbstractBox      pAbbox1;

  pAbbox1 = pAb;
  if (pAbbox1->AbSelected)
    {
      /* ce pave est selectionne */
      pAbbox1->AbSelected = FALSE;
    }
  else if (pAb->AbLeafType == LtCompound)
    {
      /* on parcours le sous-arbre */
      pChildAb = pAbbox1->AbFirstEnclosed;
      while (pChildAb != NULL)
        {
          ClearAbstractBoxSelection (pChildAb);
          pChildAb = pChildAb->AbNext;
        }
    }
}


/*----------------------------------------------------------------------
  ClearViewSelMarks annule la selection courante dans la fenetre.   
  ----------------------------------------------------------------------*/
void ClearViewSelMarks (int frame)
{
  ViewFrame          *pFrame;

  if (frame > 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      if (pFrame->FrAbstractBox != NULL)
        ClearAbstractBoxSelection (pFrame->FrAbstractBox);
      pFrame->FrSelectOneBox = FALSE;
      pFrame->FrSelectionBegin.VsBox = NULL;
      pFrame->FrSelectionEnd.VsBox = NULL;
    }
}

/*----------------------------------------------------------------------
  ClearViewSelection bascule et annule la mise en evidence de la   
  selection dans la fenetre.                              
  ----------------------------------------------------------------------*/
void ClearViewSelection (int frame)
{
  ViewFrame          *pFrame;
  PtrBox              pBox, pBox1, pBox2;
  PtrAbstractBox      pAb1, pAb2;
  int                 x1, x2;

  if (frame > 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      SetNewSelectionStatus (frame, pFrame->FrAbstractBox, FALSE);
      if (pFrame->FrSelectionBegin.VsBox && pFrame->FrSelectionEnd.VsBox)
        {
          /* begginning of the selection */
          pBox1 = pFrame->FrSelectionBegin.VsBox;
          pAb1 = pBox1->BxAbstractBox;
          /* end of the selection */
          pBox2 = pFrame->FrSelectionEnd.VsBox;
          pAb2 = pBox2->BxAbstractBox;
          pFrame->FrSelectOneBox = FALSE;
          pFrame->FrSelectionBegin.VsBox = NULL;
          pFrame->FrSelectionEnd.VsBox = NULL;

          /* ready to un/display the current selection */
          if (pBox1 == pBox2)
            {
              /* only one box is selected */
              if (pBox1->BxType == BoGhost ||
                  (pAb1 && pAb1->AbElement &&
                   FrameTable[frame].FrView == 1 &&
                   TypeHasException (ExcHighlightChildren,
                                     pAb1->AbElement->ElTypeNumber,
                                     pAb1->AbElement->ElStructSchema)))
                /* the highlight is transmitted to children */
                DrawBoxSelection (frame, pBox1);
              else if (pAb1)
                {
                  if (/*pFrame->FrSelectionBegin.VsIndBox == 0 ||*/
                      pAb1->AbLeafType == LtPolyLine ||
                      pAb1->AbLeafType == LtGraphics ||
                      pAb1->AbLeafType == LtPath)
                    /* the whole box is selected */
                    DefBoxRegion (frame, pBox1, -1, -1, -1, -1);
                  else
                    {
                      x1 = pFrame->FrSelectionBegin.VsXPos;
                      x2 = pFrame->FrSelectionEnd.VsXPos;
                      if (x1 == x2)
                        /* removing the caret at the end of a text */
                        x2 = x1 + 2;
                      DefBoxRegion (frame, pBox1, x1, x2, -1, -1);
                    }
                }
            }
          else if (pAb1 == pAb2)
            {
              /* several pieces of a split box are selected */
              /* the first one */
              x1 = pFrame->FrSelectionBegin.VsXPos;
              x2 = pBox1->BxWidth;
              DefBoxRegion (frame, pBox1, x1, x2, -1, -1);
		
              /* intermediate boxes */
              pBox1 = pBox1->BxNexChild;
              while (pBox1 && pBox1 != pBox2)
                {
                  DefBoxRegion (frame, pBox1, -1, -1, -1, -1);
                  pBox1 = pBox1->BxNexChild;
                }
              /* the last one */
              x1 = 0;
              x2 = pFrame->FrSelectionEnd.VsXPos;
              DefBoxRegion (frame, pBox2, x1, x2, -1, -1);
            }
          else
            {
              /* undisplay the beginning of the selection */
              if (pBox1->BxType == BoGhost ||
                  (pAb1 != NULL && pAb1->AbElement &&
                   FrameTable[frame].FrView == 1 &&
                   TypeHasException (ExcHighlightChildren,
                                     pAb1->AbElement->ElTypeNumber,
                                     pAb1->AbElement->ElStructSchema)))
                /* the highlight is transmitted to children */
                DrawBoxSelection (frame, pBox1);
              else
                {
                  if (pFrame->FrSelectionBegin.VsIndBox == 0)
                    /* the whole box is selected */
                    DefBoxRegion (frame, pBox1, -1, -1, -1, -1);
                  else
                    {
                      x1 = pFrame->FrSelectionBegin.VsXPos;
                      x2 = pBox1->BxWidth;
                      DefBoxRegion (frame, pBox1, x1, x2, -1, -1);
                    }

                  if (pBox1->BxType == BoPiece ||
                      pBox1->BxType == BoScript ||
                      pBox1->BxType == BoDotted)
                    {
                      /* unselect the end of the split text */
                      pBox = pBox1->BxNexChild;
                      while (pBox)
                        {
                          DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                          pBox = pBox->BxNexChild;
                        }
                    }
                }
		
              /* undisplay the end of the selection */
              if (pBox2->BxType == BoGhost ||
                  (pAb2 != NULL && pAb2->AbElement &&
                   FrameTable[frame].FrView == 1 &&
                   TypeHasException (ExcHighlightChildren,
                                     pAb2->AbElement->ElTypeNumber,
                                     pAb2->AbElement->ElStructSchema)))
                /* the highlight is transmitted to children */
                DrawBoxSelection (frame, pBox2);
              else
                {
                  if (pFrame->FrSelectionEnd.VsIndBox == 0)
                    /* the whole box is selected */
                    DefBoxRegion (frame, pBox2, -1, -1, -1, -1);
                  else
                    {
                      x1 = 0;
                      x2 = pFrame->FrSelectionEnd.VsXPos;
                      DefBoxRegion (frame, pBox2, x1, x2, -1, -1);
                    }

                  if (pBox2->BxType == BoPiece ||
                      pBox2->BxType == BoScript ||
                      pBox1->BxType == BoDotted)
                    {
                      /* select the begin of the split text */
                      pBox =  pAb2->AbBox->BxNexChild;
                      while (pBox && pBox != pBox2)
                        {
                          DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                          pBox = pBox->BxNexChild;
                        }
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaCancelSelection removes the current selection. 
  ----------------------------------------------------------------------*/
void TtaCancelSelection (Document document)
{
  PtrDocument pDoc;

  if (document > 0 && document < MAX_DOCUMENTS)
    {
      NewSelection (document, NULL, NULL, 0, 0);
      pDoc = LoadedDocument[document - 1];
      if (pDoc == SelectedDocument || pDoc == DocSelectedAttr)
        CancelSelection ();
    }
}


/*----------------------------------------------------------------------
  TtaClearViewSelections unselects and clears all current displayed
  selections.                                  
  ----------------------------------------------------------------------*/
void TtaClearViewSelections ()
{
  int                 frame;

  /* manage all frames */
  for (frame = 1; frame <= MAX_FRAME; frame++)
    if (ViewFrameTable[frame - 1].FrAbstractBox &&
        FrameTable[frame].FrDoc > 0 &&
        documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
      {
        ClearViewSelection (frame);
        DisplayFrame (frame);
      }
}

/*----------------------------------------------------------------------
  UpdateViewSelMarks updates selection marks of the frame after characters
  edits.
  The parameter rtl is TRUE when the box is displayed right to left.
  ----------------------------------------------------------------------*/
void UpdateViewSelMarks (int frame, int xDelta, int spaceDelta,
                         int charDelta, ThotBool rtl)
{
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;

  if (rtl)
    /* reverse order */
    xDelta = - xDelta;
  pFrame = &ViewFrameTable[frame - 1];
  pViewSel = &pFrame->FrSelectionBegin;
  pViewSel->VsXPos += xDelta;
  pViewSel->VsIndBox += charDelta;
  pViewSel->VsNSpaces += spaceDelta;
  pViewSel = &pFrame->FrSelectionEnd;
  if (pViewSel->VsBox == pFrame->FrSelectionBegin.VsBox)
    {
      pViewSel->VsXPos += xDelta;
      pViewSel->VsIndBox += charDelta;
      pViewSel->VsNSpaces += spaceDelta;
    }
}


/*----------------------------------------------------------------------
  Detruit le buffer donne en parametre, met a jour les marques de 
  selection et rend le pointeur sur le buffer precedent.          
  ----------------------------------------------------------------------*/
PtrTextBuffer DeleteBuffer (PtrTextBuffer pBuffer, int frame)
{
  PtrTextBuffer       pNextBuffer;
  PtrTextBuffer       pPreviousBuffer;
  int                 length = 0;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;

  pNextBuffer = pBuffer->BuNext;
  pPreviousBuffer = pBuffer->BuPrevious;
  if (pPreviousBuffer)
    {
      pPreviousBuffer->BuNext = pNextBuffer;
      length = pPreviousBuffer->BuLength;
    }
  if (pNextBuffer)
    pNextBuffer->BuPrevious = pPreviousBuffer;

  /* Mise a jour des marques de selection courante */
  if (frame > 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pViewSel = &pFrame->FrSelectionBegin;
      if (pViewSel->VsBuffer == pBuffer)
        {
          if (pFrame->FrSelectionEnd.VsBuffer == pViewSel->VsBuffer)
            {
              pViewSelEnd = &pFrame->FrSelectionEnd;
              if (pPreviousBuffer)
                {
                  /* deplace la selection dans les buffers */
                  pViewSelEnd->VsIndBuf += length;
                  pViewSelEnd->VsBuffer = pPreviousBuffer;
                }
              else
                {
                  pViewSelEnd->VsIndBuf = 0;
                  pViewSelEnd->VsBuffer = pNextBuffer;
                }
            }
          pViewSel->VsBuffer = pPreviousBuffer;
	   
          if (pPreviousBuffer)
            {
              /* deplace la selection dans les buffers */
              pViewSel->VsIndBuf += length;
              pViewSel->VsBuffer = pPreviousBuffer;
            }
          else
            {
              pViewSel->VsIndBuf = 0;
              pViewSel->VsBuffer = pNextBuffer;
            }
        }
    }

  FreeTextBuffer (pBuffer);
  return pPreviousBuffer;
}


/*----------------------------------------------------------------------
  ComputeViewSelMarks computes selection marks knowing the main VsBox
  box, the VsBuffer buffer and the VsIndBuf index in the buffer.
  Generates the number of spaces before the character in the
  box (VsNSpaces), the character index in the box (VsIndBox), the
  position within the box (VsXPos) and the line which includes the 
  character (VsLine).
  ----------------------------------------------------------------------*/
void ComputeViewSelMarks (ViewSelection *selMark, int frame)
{
  PtrTextBuffer       pBuffer;
  PtrBox              pBox;
  PtrAbstractBox      pAb;
  int                 x;
  int                 t, b, l, r;
  int                 spaces;
  int                 ind, pos;
  ThotBool            embedded = FALSE, rtl;

  if (selMark->VsBox->BxAbstractBox->AbLeafType == LtText)
    {
      /* get the current selection info */
      pos = selMark->VsIndBox;
      pBox = selMark->VsBox;
      pAb = pBox->BxAbstractBox;
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        /* select the first child */
        pBox = pBox->BxNexChild;
      /* look for the right box */
      while (pBox->BxNexChild && pBox->BxNexChild->BxFirstChar <= pos)
        pBox = pBox->BxNexChild;

      pos -= pBox->BxFirstChar;
      selMark->VsIndBox = pos;
      selMark->VsBox = pBox;
      if (pAb->AbUnicodeBidi == 'O')
        rtl = (pAb->AbDirection == 'R');
      else
        rtl = (pBox->BxScript == 'A' || pBox->BxScript == 'H');

      /* look for the rigth buffer and the right index */
      pBuffer = pBox->BxBuffer;
      ind = pBox->BxIndChar + pos;
      while (ind >= pBuffer-> BuLength && pBuffer->BuNext)
        {
          ind -= pBuffer->BuLength;
          /* selection in the next buffer */
          pBuffer = pBuffer->BuNext;
        }
      selMark->VsIndBuf = ind;
      selMark->VsBuffer = pBuffer;

      /* update number of spaces and the position */
      if (pos >= pBox->BxNChars)
        {
          /* selection at the end of the box? */
          x = pBox->BxW;
          spaces = pBox->BxNSpaces;
        }
      else
        {
          spaces = pBox->BxSpaceWidth;
          pBuffer = pBox->BxBuffer;
          ind = pBox->BxIndChar;
          /* Only the width is requested: Override and Latin */
          GiveTextParams (&pBuffer, &ind, &pos, pBox->BxFont,
                          pAb->AbFontVariant, &x, &spaces, 'L',
                          'O', &embedded,'*');
          if (pBox->BxSpaceWidth)
            {
              /* treatment of justified lines */
              if (spaces < pBox->BxNPixels)
                x += spaces;
              else
                x += pBox->BxNPixels;
            }
        }

      if (rtl)
        x = - x + pBox->BxW;
      GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
      selMark->VsXPos = x + l + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      selMark->VsNSpaces = spaces;
    }
  selMark->VsLine = SearchLine (selMark->VsBox, frame);
}

/*----------------------------------------------------------------------
  InsertViewSelMarks inserts selection makes into the displayed boxes.
  The abstract box pAb concerns the beginning of the selection
  (startSelection=TRUE) and/or the end of the selection (endSelection=TRUE).
  The parameter firstChar gives the index of the first selected character
  or 0 if the whole abstract box is selected.
  The parameter lastChar gives the index of the character that follows
  the last selected character.
  The parameter alone is set to TRUE when only one abstract box is selected.
  ----------------------------------------------------------------------*/
void InsertViewSelMarks (int frame, PtrAbstractBox pAb, int firstChar,
                         int lastChar, ThotBool startSelection,
                         ThotBool endSelection, ThotBool alone)
{
  PtrLine             adline;
  PtrTextBuffer       pBuffer;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel, *pViewSelEnd;
  CHAR_T              c;
  int                 t, b, l, r, shift;
  ThotBool            graphSel, rtl;

  /* Check if a paragraph should be reformatted after an edit */
  CloseParagraphInsertion (pAb, frame);

  if (pAb && frame > 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      if (pAb->AbBox)
        {
          pBox = pAb->AbBox;
          GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
          if (pBox->BxAbstractBox->AbUnicodeBidi == 'O')
            rtl = (pBox->BxAbstractBox->AbDirection == 'R');
          else
            rtl = (pBox->BxScript == 'A' || pBox->BxScript == 'H');
          adline = SearchLine (pBox, frame);
          graphSel = (pAb->AbLeafType == LtPolyLine ||
                      pAb->AbLeafType == LtPath ||
                      pAb->AbLeafType == LtGraphics);
          /* check index values */
          if (pAb->AbLeafType == LtText)
            {
              /* it's a text */
              if (firstChar == 0 && lastChar == 0)
                {
                  if (!SelPosition &&
                      (startSelection || endSelection))
                    {
                      /* select the whole text */
                      firstChar = 1;
                      lastChar = pAb->AbVolume + 1;
                    }
                }
              else if (firstChar == 0)
                firstChar = 1;
              else if (firstChar > 1 && lastChar == 0)
                lastChar = pAb->AbVolume;
            }
          else if (pAb->AbLeafType == LtSymbol)
            {
              if (SelPosition)
                lastChar = firstChar - 1;
            }
          else if (!graphSel && pAb->AbLeafType != LtPicture)
            firstChar = 0;
	  
          /* register the unicity of the selection */
          pFrame->FrSelectOneBox = alone;
          pFrame->FrSelectOnePosition = SelPosition;
          pViewSel = &pFrame->FrSelectionBegin;
          pViewSelEnd = &pFrame->FrSelectionEnd;
          if (firstChar == 0 || pAb->AbVolume == 0 ||
              graphSel || pAb->AbLeafType == LtPicture ||
              pAb->AbLeafType == LtSymbol)
            {
              /* selection of a whole box or one position within
                 a picture or a polyline */
              if (pAb->AbLeafType == LtText)
                pBuffer = pAb->AbText;
              else
                pBuffer = NULL;
              /* text shift */
              shift = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
              l +=  shift;
              /* register selection marks */
              if (startSelection)
                {
                  pViewSel->VsBox = pBox;
                  if (endSelection && !graphSel &&
                      pAb->AbLeafType != LtPicture && pAb->AbLeafType != LtSymbol)
                    /* select the whole box */
                    pViewSel->VsIndBox = 0;
                  else
                    pViewSel->VsIndBox = firstChar;
                  pViewSel->VsIndBuf = 0;
                  pViewSel->VsBuffer = pBuffer;
                  pViewSel->VsLine = adline;
                  if (pAb->AbLeafType == LtPicture && firstChar > 0)
                    pViewSel->VsXPos = l + pBox->BxW;
                  else if (pAb->AbLeafType == LtSymbol && firstChar > 1)
                    pViewSel->VsXPos = l + pBox->BxW;
                  else if (pAb->AbLeafType == LtCompound)
                    /* select the whole compound box including paddings, etc. */
                    pViewSel->VsXPos = l - shift;
                  else
                    pViewSel->VsXPos = l;
                  pViewSel->VsNSpaces = 0;
                }
              if (endSelection || SelPosition)
                {
                  pViewSelEnd->VsBox = pBox;
                  pViewSelEnd->VsIndBox = 0;
                  pViewSelEnd->VsIndBuf = 0;
                  pViewSelEnd->VsBuffer = pBuffer;
                  pViewSelEnd->VsLine = adline;
                  if (pAb->AbLeafType == LtText)
                    /* select the whole text */
                    pViewSelEnd->VsXPos = l + pBox->BxW;
                  else if (pAb->AbLeafType == LtPicture)
                    {
                      if (!SelPosition)
                        pViewSelEnd->VsXPos = l + pBox->BxW;
                      else if (firstChar == 0)
                        pViewSelEnd->VsXPos = l + 2;
                      else
                        /* select the right side of the picture or symbol */
                        pViewSelEnd->VsXPos = pBox->BxW + l + 2;
                    }
                  else if (pAb->AbLeafType == LtSymbol && firstChar == 0)
                    /* select the right side of the picture or symbol */
                    pViewSelEnd->VsXPos = l + pBox->BxW;
                  else if (pAb->AbLeafType == LtSymbol)
                    {
                      if (!SelPosition)
                        /* select the right side of the picture or symbol */
                        pViewSelEnd->VsXPos = l + pBox->BxW;
                      else
                        pViewSelEnd->VsXPos = pViewSel->VsXPos + 2;
                    }
                  else if (pAb->AbLeafType == LtCompound)
                    /* select the whole compound box including paddings, etc. */
                    pViewSelEnd->VsXPos = pBox->BxWidth - r;
                  else
                    pViewSelEnd->VsXPos = l;
                  pViewSelEnd->VsNSpaces = 0;
                }
            }
          else
            {
              /* select a substring */
              if (startSelection)
                {
                  /* update the first selection mark */
                  pViewSel->VsBox = pBox;
                  pViewSel->VsIndBox = firstChar;
                  ComputeViewSelMarks (pViewSel, frame);
                }
              if (endSelection)
                {
                  /* update the first selection mark */
                  if (startSelection && firstChar >= lastChar)
                    {
                      /* startSelection and endSelection on the same character */
                      pViewSelEnd->VsBox = pViewSel->VsBox;
                      pViewSelEnd->VsIndBox = pViewSel->VsIndBox;
                      pViewSelEnd->VsLine = pViewSel->VsLine;
                      pViewSelEnd->VsBuffer = pViewSel->VsBuffer;
                      pViewSelEnd->VsIndBuf = pViewSel->VsIndBuf;
                      pViewSelEnd->VsXPos = pViewSel->VsXPos;
                      pViewSelEnd->VsNSpaces = pViewSel->VsNSpaces;
                    }
                  else
                    {
                      /* startSelection and endSelection differ */
                      pViewSelEnd->VsBox = pBox;
                      pViewSelEnd->VsIndBox = lastChar;
                      ComputeViewSelMarks (pViewSelEnd, frame);
                    }
		  
                  /* update the width of the selection */
                  pBox = pViewSelEnd->VsBox;
                  if (pBox->BxNChars == 0 && pBox->BxType == BoComplete)
                    /* select the whole box */
                    pViewSelEnd->VsXPos += pBox->BxW;
                  else if (pViewSelEnd->VsIndBox >= pBox->BxNChars)
                    /* select the end of the box */
                    pViewSelEnd->VsXPos += 2;
                  else if (SelPosition)
                    pViewSelEnd->VsXPos += 2;
                  else if (firstChar == lastChar && firstChar != 1)
                    {
                      c = pViewSelEnd->VsBuffer->BuContent[pViewSelEnd->VsIndBuf];
                      if (c == SPACE && pBox->BxSpaceWidth != 0)
                        pViewSelEnd->VsXPos += pBox->BxSpaceWidth;
                      else
                        pViewSelEnd->VsXPos += BoxCharacterWidth (c, pAb->AbFontVariant, pBox->BxFont);
                    }
                }
            }

          /* pViewSel points to the first selection mark */
          /* ready to display the current selection */
          if (startSelection && endSelection)
            {
              /* only one box is selected */
              pBox = pViewSel->VsBox;
              if (pBox->BxType == BoGhost ||
                  (pAb && FrameTable[frame].FrView == 1 &&
                   TypeHasException (ExcHighlightChildren,
                                     pAb->AbElement->ElTypeNumber,
                                     pAb->AbElement->ElStructSchema)))
                /* the highlight is transmitted to children */
                DrawBoxSelection (frame, pBox);
              else
                {
                  if (pBox != pViewSelEnd->VsBox)
                    {
                      /* several pieces of a split box are selected */
                      if (rtl)
                        {
                          /* the first one */
                          DefBoxRegion (frame, pBox, 0, pViewSel->VsXPos, -1, -1);
                          /* the last one */
                          pBox = pViewSelEnd->VsBox;
                          DefBoxRegion (frame, pBox, pViewSel->VsXPos,
                                        pBox->BxWidth, -1, -1);
                        }
                      else
                        {
                          /* the first one */
                          DefBoxRegion (frame, pBox, pViewSel->VsXPos,
                                        pBox->BxWidth, -1, -1);
                          /* the last one */
                          pBox = pViewSelEnd->VsBox;
                          DefBoxRegion (frame, pBox, 0, pViewSel->VsXPos, -1, -1);
                        }
                      /* intermediate boxes */
                      pBox = pViewSel->VsBox->BxNexChild;
                      while (pBox &&
                             (pBox != pViewSelEnd->VsBox ||
                              pViewSelEnd->VsIndBox == 0))
                        {
                          DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                          pBox = pBox->BxNexChild;
                        }
                      if (pBox)
                        DefBoxRegion (frame, pBox, 0, pViewSelEnd->VsXPos, -1, -1);
                    }
                  else if (graphSel /* && firstChar > 0*/)
                    DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                  else
                    /* a substring or a point of the box is selected */
                    DefBoxRegion (frame, pBox, pViewSel->VsXPos,
                                  pViewSelEnd->VsXPos, -1, -1);
                }
            }
          else if (endSelection)
            {
              /* display the end of the selection */
              pBox = pViewSelEnd->VsBox;
              if (pBox->BxType == BoGhost ||
                  (pAb && FrameTable[frame].FrView == 1 &&
                   TypeHasException (ExcHighlightChildren,
                                     pAb->AbElement->ElTypeNumber,
                                     pAb->AbElement->ElStructSchema)))
                /* the highlight is transmitted to children */
                DrawBoxSelection (frame, pBox);
              else
                {
                  if (pViewSelEnd->VsIndBox == 0)
                    /* the whole box is selected */
                    DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                  else if (rtl)
                    /* a substring or a point of the box is selected */
                    DefBoxRegion (frame, pBox, pViewSelEnd->VsXPos,
                                  pBox->BxWidth, -1, -1);
                  else
                    /* a substring or a point of the box is selected */
                    DefBoxRegion (frame, pBox, 0,
                                  pViewSelEnd->VsXPos, -1, -1);
                  if (pBox->BxType == BoPiece ||
                      pBox->BxType == BoScript ||
                      pBox->BxType == BoDotted)
                    {
                      /* select the begin of the split text */
                      pBox = pAb->AbBox->BxNexChild;
                      while (pBox != pViewSelEnd->VsBox)
                        {
                          DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                          pBox = pBox->BxNexChild;
                        }
                    }
                }
            }
          else
            {
              /* display the beginning of the selection */
              pBox = pViewSel->VsBox;
              if (pBox->BxType == BoGhost ||
                  (pAb && FrameTable[frame].FrView == 1 &&
                   TypeHasException (ExcHighlightChildren,
                                     pAb->AbElement->ElTypeNumber,
                                     pAb->AbElement->ElStructSchema)))
                /* the highlight is transmitted to children */
                DrawBoxSelection (frame, pBox);
              else
                {
                  if (pViewSel->VsIndBox == 0)
                    /* the whole box is selected */
                    DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                  else if (rtl)
                    /* a substring or a point of the box is selected */
                    DefBoxRegion (frame, pBox, 0, pViewSel->VsXPos, -1, -1);
                  else
                    /* a substring or a point of the box is selected */
                    DefBoxRegion (frame, pBox, pViewSelEnd->VsXPos,
                                  pBox->BxWidth, -1, -1);
                  if (pBox->BxType == BoPiece ||
                      pBox->BxType == BoScript ||
                      pBox->BxType == BoDotted)
                    {
                      /* select the end of the split text */
                      pBox = pBox->BxNexChild;
                      while (pBox)
                        {			  
                          DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                          pBox = pBox->BxNexChild;
                        }
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  IsAbstractBoxDisplayed rend la valeur vrai si le pave est affiche'
  dans le frame.                                          
  ----------------------------------------------------------------------*/
ThotBool IsAbstractBoxDisplayed (PtrAbstractBox pAb, int frame)
{
  int                 min, max;

  if (pAb == NULL)
    return FALSE;
  else
    while (pAb->AbBox == NULL)
      /* remonte au pavee englobant cree */
      if (pAb->AbEnclosing == NULL)
        return FALSE;
      else
        pAb = pAb->AbEnclosing;

  /* regarde si le pavee est affiche dans la fenetre */
  GetSizesFrame (frame, &min, &max);
  min = ViewFrameTable[frame - 1].FrYOrg;
  max += min;
  if (pAb->AbBox->BxYOrg + pAb->AbBox->BxHeight < min ||
      pAb->AbBox->BxYOrg > max)
    return FALSE;
  else
    return TRUE;
}
