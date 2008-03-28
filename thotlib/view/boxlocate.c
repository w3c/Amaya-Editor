/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* 
 * locate what is designated in Concret Image.
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "ustring.h"
#include "libmsg.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appaction.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "edit_tv.h"
#include "boxes_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"
#include "select_tv.h"

#include "absboxes_f.h"
#include "applicationapi_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "dialogapi_f.h"
#include "displaybox_f.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "geom_f.h"
#include "hyphen_f.h"
#include "memory_f.h"
#include "presentationapi_f.h"
#include "spline_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "units_f.h"
#include "unstructlocate_f.h"
#include "views_f.h"
#include "word_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

static ThotBool     SkipClickEvent = FALSE;

#define Y_RATIO 200		/* penalisation en Y */
#define ALLOC_POINTS    300
#define ANCHOR_SIZE 3		/* taille des ancres */

/*----------------------------------------------------------------------
  APPgraphicModify sends a message TteElemGraphModif to parent elements
  of a graphic.
  The parameter openclose is TRUE when the history must be open and closed.
  ----------------------------------------------------------------------*/
ThotBool APPgraphicModify (PtrElement pEl, int value, int frame,
                           ThotBool pre , ThotBool openclose)
{
  PtrElement          pAsc;
  ThotBool            result;
  NotifyOnValue       notifyEl;
  PtrDocument         pDoc;
  int                 view;
  ThotBool            ok, isGraph, loop;

  GetDocAndView (frame, &pDoc, &view);
  result = FALSE;
  ok = FALSE;
  pAsc = pEl;
  notifyEl.event = TteElemGraphModify;
  notifyEl.document = (Document) IdentDocument (pDoc);
  notifyEl.target = (Element) pEl;
  notifyEl.value = value;
  isGraph = (pEl && pEl->ElTerminal &&
             (pEl->ElLeafType == LtGraphics ||
              pEl->ElLeafType == LtSymbol ||
              pEl->ElLeafType == LtPolyLine ||
              pEl->ElLeafType == LtPath));
  loop = isGraph;
  while (pAsc)
    {
      notifyEl.element = (Element) pAsc;
      ok = CallEventType ((NotifyEvent *) &notifyEl, pre);
      result = result || ok;
      if (loop)
        {
          /* generates a callback for the enclosing element too */
          pAsc = pAsc->ElParent;
          loop = FALSE;
        }
      else
        pAsc = NULL;
    }
  /* if it's before the actual change is made and if the application accepts
     the change, register the operation in the Undo queue (only if it's
     not a creation). */
  if (pre && openclose && !ok)
    {
      if (((pEl->ElLeafType == LtGraphics || pEl->ElLeafType == LtSymbol) &&
           pEl->ElGraph != EOS) ||
          (pEl->ElLeafType == LtPolyLine && pEl->ElPolyLineType != EOS) ||
          (pEl->ElLeafType == LtPath))
        {
          OpenHistorySequence (pDoc, pEl, pEl, NULL, 0, 0);
          AddEditOpInHistory (pEl, pDoc, TRUE, TRUE);
        }
    }
  if (!pre && openclose && isGraph)
    CloseHistorySequence (pDoc);
  return result;
}

/*----------------------------------------------------------------------
  NotifyClick sends a message event to parent elements of a graphic.
  ----------------------------------------------------------------------*/
static ThotBool NotifyClick (int event, ThotBool pre, PtrElement pEl, int doc)
{
  PtrElement          pAsc;
  NotifyElement       notifyEl;
  ThotBool            ok;

  if (!pEl)
    return FALSE;
  ok = FALSE;
  pAsc = pEl;
  if (pEl && pEl->ElTerminal &&
      (pEl->ElLeafType == LtGraphics ||
       pEl->ElLeafType == LtSymbol ||
       pEl->ElLeafType == LtPolyLine ||
       pEl->ElLeafType == LtPath))
    pAsc = pAsc->ElParent;
  notifyEl.event = (APPevent)event;
  notifyEl.document = doc;
  notifyEl.position = 0;
  notifyEl.element = (Element) pAsc;
  notifyEl.info = 0; /* not sent by undo */
  notifyEl.elementType.ElTypeNum = pAsc->ElTypeNumber;
  notifyEl.elementType.ElSSchema = (SSchema)(pAsc->ElStructSchema);
  ok = CallEventType ((NotifyEvent *) &notifyEl, pre);
  return ok;
}

/*----------------------------------------------------------------------
  LocateSelectionInView finds out the selected Abstract Box and if it's
  a TEXT element the selected character(s).
  The parameter button says what the editor wants to do with this
  new selection:
  0 -> extend the current selection
  1 -> extend the current selection by draging
  2 -> replace the old selection
  3 -> activate a link
  4 -> click an element
  5 -> click with the middle button
  6 -> click with the right button
  7 -> reset the selection without notification
// return TRUE if the event is already managed
  ----------------------------------------------------------------------*/
ThotBool LocateSelectionInView (int frame, int x, int y, int button)
{
  PtrBox              pBox;
  PtrElement          pEl = NULL, firstEl;
  PtrTextBuffer       pBuffer;
  PtrAbstractBox      pAb;
  PtrElement          el = NULL;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  PtrFlow             pFlow = NULL;
  int                 nChars;
  int                 nSpaces;
  int                 index, pos;
  int                 xOrg, yOrg;
  int                 width, height;
  int                 doc, view;
  int                 firstC;
  ThotBool            extend, ok, left = FALSE;

  if (frame >= 1)
    {
      /* check if a leaf box is selected */
      pFrame = &ViewFrameTable[frame - 1];
      pViewSel = &pFrame->FrSelectionBegin;
#ifndef _GL
      x += pFrame->FrXOrg;
      y += pFrame->FrYOrg;
#endif /*_GL*/
      if (x < 0)
        x = 0;
      if (y < 0)
        y = 0;
      pAb = pFrame->FrAbstractBox;
      nChars = 0;
      if (button == 6 && SelectedPointInPolyline != 0 &&
          FirstSelectedElement &&
          FirstSelectedElement == LastSelectedElement &&
          FirstSelectedElement->ElTerminal &&
          FirstSelectedElement->ElLeafType == LtPolyLine)
        ContentEditing (TEXT_INSERT);
      else
        {
          extend = (button == 0 || button == 1);
          /* get the selected box */
          GetClickedBox (&pBox, &pFlow, pAb, frame, x, y, Y_RATIO, &nChars);
          /* When it's an extended selection, avoid to extend to the
             enclosing box */
          if (extend)
            {
              if (pBox != pViewSel->VsBox &&
                  IsParentBox (pBox, pViewSel->VsBox))
                pBox = GetClickedLeafBox (frame, x, y, &pFlow);
            }
          else if (pBox && pBox->BxAbstractBox && FrameTable[frame].FrView == 1)
            {
              pEl = pBox->BxAbstractBox->AbElement;
              if (pEl)
                pEl = pEl->ElParent;
              if (pEl && pEl->ElPrevious &&
                  TypeHasException (ExcIsBreak, pEl->ElTypeNumber, pEl->ElStructSchema) &&
                  !TypeHasException (ExcIsBreak, pEl->ElPrevious->ElTypeNumber, pEl->ElPrevious->ElStructSchema))
                pBox = pBox->BxPrevious;
            }

          if (pBox)
            {
#ifndef _GL
              xOrg =  pBox->BxXOrg;
              yOrg =  pBox->BxYOrg;
              width = pBox->BxWidth;
              height = pBox->BxHeight;
#else /* _GL */
              xOrg =  pBox->BxClipX;
              yOrg =  pBox->BxClipY;
              width = pBox->BxClipW;
              height = pBox->BxClipH;
#endif /* _GL */
              if (pFlow)
                {
                  /* apply the box shift */
                  xOrg += pFlow->FlXStart;
                  yOrg += pFlow->FlYStart;
                }
              pAb = pBox->BxAbstractBox;
              if (pAb->AbLeafType == LtText &&
                  (!pAb->AbPresentationBox || pAb->AbCanBeModified))
                {
                  pos = x - xOrg;
                  LocateClickedChar (pBox, frame, extend, &pBuffer, &pos,
                                     &index, &nChars, &nSpaces);
                  nChars = pBox->BxFirstChar + nChars;
                  if (extend)
                    {
                      pEl = pAb->AbElement;
                      if (DocSelectedAttr)
                        {
                          /* work within an attribute */
                          firstC = FirstSelectedCharInAttr;
                          firstEl = NULL;
                        }
                      else
                        {
                          firstC = FirstSelectedChar;
                          firstEl = FirstSelectedElement;
                        }
                      if (pEl == firstEl && nChars < firstC)
                        left = TRUE;
                      else if (ElemIsBefore (pEl, firstEl))
                        left = TRUE;
                      else if (nChars == pBox->BxFirstChar &&
                               pBox->BxFirstChar > 1 && pEl == firstEl)
                        /* extension until the beginning of this box
                           select the end of the previous box */
                        nChars--;
                      else if (y > yOrg + height &&
                               pEl == LastSelectedElement &&
                               LastSelectedElement &&
                               nChars <= LastSelectedChar)
                        nChars = LastSelectedElement->ElVolume + 1;
                    }
                }
              else if (pAb->AbLeafType == LtSymbol && !extend)
                {
                  pos = x - xOrg;
                  if (pos < width/2)
                    nChars = 1;
                  else
                    nChars = 2;
                }
            }
          else
            {
              pAb = NULL;
              xOrg =  0;
              yOrg =  0;
              width = 0;
              height = 0;
            }

          FrameToView (frame, &doc, &view);
          if (pAb)
            {
              /* Initialization of the selection */
              switch (button)
                {
                case 0:
                  /* Extension of selection */
                  if (SkipClickEvent)
                    /* the application asks Thot to do nothing */
                    return SkipClickEvent;
                  ChangeSelection (frame, pAb, nChars, TRUE, left, FALSE, FALSE);
                  break;
                case 1:
                  /* Extension of selection */
                  if (SkipClickEvent)
                    /* the application asks Thot to do nothing */
                    return SkipClickEvent;
                  ChangeSelection (frame, pAb, nChars, TRUE, left, FALSE, TRUE);
                  break;
                case 2:
                  /* send event TteElemLClick.Pre to the application */
                  el = pAb->AbElement;
                  SkipClickEvent = NotifyClick (TteElemLClick, TRUE, el, doc);
                  if (SkipClickEvent)
                    /* the application asks Thot to do nothing */
                    return SkipClickEvent;
                  ChangeSelection (frame, pAb, nChars, FALSE, TRUE, FALSE, FALSE);
                  // the document can be reloaded
                  pAb = pFrame->FrAbstractBox;
                  nChars = 0;
                  GetClickedBox (&pBox, &pFlow, pAb, frame, x, y, Y_RATIO, &nChars);
                  if (pBox && pBox->BxAbstractBox)
                    {
                      el = pBox->BxAbstractBox->AbElement;
                      NotifyClick (TteElemLClick, FALSE, el, doc);
                    }
                  break;
                case 3:
                  if (!ChangeSelection (frame, pAb, nChars, FALSE, TRUE, TRUE, FALSE) &&
                      pAb->AbLeafType == LtText &&
                      (!pAb->AbPresentationBox || pAb->AbCanBeModified))
                    SelectCurrentWord (frame, pBox, nChars, index, pBuffer,
                                       TRUE);
                  break;
                case 4:
                  if (SkipClickEvent)
                    /* the application asks Thot to do nothing */
                    return SkipClickEvent;
                  /* check if the curseur is within the box */
                  if ((x >= xOrg && x <= xOrg + width &&
                       y >= yOrg && y <= yOrg + height) ||
                      GetParentWithException (ExcClickableSurface, pAb))
                    {
                      /* send event TteElemClick.Pre to the application */
                      el = pAb->AbElement;
                      if (NotifyClick (TteElemClick, TRUE, el, doc))
                        /* the application asks Thot to do nothing */
                        return TRUE;
                      /* send event TteElemClick.Post to the application */
                      NotifyClick (TteElemClick, FALSE, el, doc);
                    }
                  break;
                case 5:
                  /* check if the curseur is within the box */
                  if (x >= xOrg && x <= xOrg + width &&
                      y >= yOrg && y <= yOrg + height)
                    {
                      /* send event TteElemMClick.Pre to the application */
                      el = pAb->AbElement;
                      if (NotifyClick (TteElemMClick, TRUE, el, doc))
                        /* the application asks Thot to do nothing */
                        return TRUE;
                    }
#if defined(_UNIX) && !defined(_MACOS)
                  if (MenuActionList[CMD_PasteFromClipboard].Call_Action != NULL)
                    (*(Proc2)MenuActionList[CMD_PasteFromClipboard].Call_Action) (
                                                                                  (void*)doc,
                                                                                  (void*)view);
#endif /* _UNIX && !_MACOS */
                  break;
                case 6:
                  /* check if the curseur is within the box */
                  if (x >= xOrg && x <= xOrg + width &&
                      y >= yOrg && y <= yOrg + height)
                    {
                      /* send event TteElemRClick.Pre to the application */
                      el = pAb->AbElement;
                      if (NotifyClick (TteElemRClick, TRUE, el, doc))
                        /* the application asks Thot to do nothing */
                        return TRUE;
                    }
                  TtaSetDialoguePosition ();
                  if (ThotLocalActions[T_insertpaste] != NULL)
                    (*(Proc4)ThotLocalActions[T_insertpaste]) (
                                                               (void *)TRUE,
                                                               (void *)FALSE,
                                                               (void *)'R',
                                                               (void *)&ok);
                  else if (x >= xOrg && x <= xOrg + pBox->BxW &&
                           y >= yOrg && y <= yOrg + pBox->BxH)
                    /* send event TteElemRClick.Post to the application */
                    NotifyClick (TteElemRClick, FALSE, el, doc);
                  break;
                case 7: /* reset the previous selection */
                  ChangeSelection (frame, pAb, nChars, FALSE, TRUE, FALSE, FALSE);
                  break;
                default: break;
                }
            }
        }
    }
  return FALSE;
}


/*----------------------------------------------------------------------
  IsOnSegment checks if the point x, y is on the segment x1, y1 to
  x2, y2 with DELTA_SEL precision.
  Check if the segment is included by a rectangle of width DELTA_SEL
  around the line.
  Checking is performed after a rotation that provides an horizontal
  rectangle.
  ----------------------------------------------------------------------*/
static ThotBool IsOnSegment (int x, int y, int x1, int y1, int x2, int y2)
{
  int                 dX, dY, nX, nY;
  double              ra, cs, ss;

  x -= x1;
  y -= y1;
  dX = x2 - x1;
  dY = y2 - y1;
  /* ramene le 2e point sur l'horizontale */
  ra = sqrt ((double) dX * dX + dY * dY);
  if (ra == 0.0)
    return FALSE;
  cs = dX / ra;
  ss = dY / ra;
  nX = (int) (x * cs + y * ss);
  nY = (int) (y * cs - x * ss);
  /* test */
  return (nY <= DELTA_SEL
          && nY >= -DELTA_SEL
          && nX >= -DELTA_SEL
          && nX <= ra + DELTA_SEL);
}

/*----------------------------------------------------------------------
  CrossLine returns the next cross value
  ----------------------------------------------------------------------*/
static int CrossLine (int x, int y, int prevX, int prevY, int nextX,
                      int nextY, int cross)
{
  int          i;
  ThotBool     ok;

  /* y between nextY and prevY */
  i = cross;
  if ((ok = (prevX >= x)) == (nextX >= x))
    {
      /* x on the same side of both extremities */
      if (ok)
        /* nextX et prevX >= x */
        i = i + 1;
    }
  else if ((prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x)
    /* x between the extremities */
    i = i + 1;
  return i;
}

/*----------------------------------------------------------------------
  IsWithinPolyline returns TRUE if the point x, y is within the polyline pAb
  ----------------------------------------------------------------------*/
static ThotBool IsWithinPolyline (PtrAbstractBox pAb, int x, int y, int frame)
{
  PtrTextBuffer       buff, pLastBuffer;
  int                 cross;
  int                 i, max;
  int                 prevX, prevY;
  int                 nextX, nextY;
  PtrBox              box;
  ThotBool            ok;

  box = pAb->AbBox;
  x -= box->BxXOrg;
  y -= box->BxYOrg;
  max = box->BxNChars;
  if (max < 4)
    /* no space within polyline */
    return (FALSE);

  /* first and last points of the polyline */
  pLastBuffer = buff = box->BxBuffer;
  i = 1;
  while (pLastBuffer->BuNext != NULL)
    pLastBuffer = pLastBuffer->BuNext;
  max = pLastBuffer->BuLength - 1;

  cross = 0;
  nextX = PixelValue (buff->BuPoints[i].XCoord,
                      UnPixel, NULL,
                      ViewFrameTable[frame - 1].FrMagnification);
  nextY = PixelValue (buff->BuPoints[i].YCoord,
                      UnPixel, NULL,
                      ViewFrameTable[frame - 1].FrMagnification);
  prevX = PixelValue (pLastBuffer->BuPoints[max].XCoord,
                      UnPixel, NULL,
                      ViewFrameTable[frame - 1].FrMagnification);
  prevY = PixelValue (pLastBuffer->BuPoints[max].YCoord,
                      UnPixel, NULL,
                      ViewFrameTable[frame - 1].FrMagnification);
  if ((prevY >= y) != (nextY >= y))
    /* y between nextY and prevY */
    cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);

  i++;
  while (i <= max || buff != pLastBuffer)
    {
      prevX = nextX;
      prevY = nextY;
      nextX = PixelValue (buff->BuPoints[i].XCoord,
                          UnPixel, NULL,
                          ViewFrameTable[frame - 1].FrMagnification);
      nextY = PixelValue (buff->BuPoints[i].YCoord,
                          UnPixel, NULL,
                          ViewFrameTable[frame - 1].FrMagnification);
      if (prevY >= y)
        {
          while ((i <= max || buff != pLastBuffer) && (nextY >= y))
            {
              i++;		/* changement de point */
              if (i >= buff->BuLength && buff != pLastBuffer)
                {
                  buff = buff->BuNext;	/* passe au buffer suivant */
                  i = 0;
                }
              prevY = nextY;
              prevX = nextX;
              nextX = PixelValue (buff->BuPoints[i].XCoord,
                                  UnPixel, NULL,
                                  ViewFrameTable[frame - 1].FrMagnification);
              nextY = PixelValue (buff->BuPoints[i].YCoord,
                                  UnPixel, NULL,
                                  ViewFrameTable[frame - 1].FrMagnification);
            }
	   
          if (i > max && buff == pLastBuffer)
            break;
          cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
        }
      else
        {
          while ((i <= max || buff != pLastBuffer) && (nextY < y))
            {
              i++;		/* changement de point */
              if (i >= buff->BuLength && buff != pLastBuffer)
                {
                  buff = buff->BuNext;	/* passe au buffer suivant */
                  i = 0;
                }
              prevY = nextY;
              prevX = nextX;
              nextX = PixelValue (buff->BuPoints[i].XCoord,
                                  UnPixel, NULL,
                                  ViewFrameTable[frame - 1].FrMagnification);
              nextY = PixelValue (buff->BuPoints[i].YCoord,
                                  UnPixel, NULL,
                                  ViewFrameTable[frame - 1].FrMagnification);
            }
	   
          if (i > max && buff == pLastBuffer)
            break;
          cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
        }
    }
  ok = (ThotBool) (cross & 0x01);
  return (ok);
}

/*----------------------------------------------------------------------
  IsWithinPath returns TRUE if the point x, y is within the path represented
  by the polyline defined by points.
  ----------------------------------------------------------------------*/
static ThotBool IsWithinPath (int x, int y, ThotPoint *points, int npoints)
{
  int                 cross;
  int                 i;
  int                 prevX, prevY;
  int                 nextX, nextY;
  ThotBool            ok;

  ok = FALSE;
  cross = 0;
  nextX = (int) points[0].x;
  nextY = (int) points[0].y;
  prevX = (int) points[npoints - 1].x;
  prevY = (int) points[npoints - 1].y;
  if ((prevY >= y) != (nextY >= y))
    /* y is between nextY and prevY */
    cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
  i = 1;
  while (i < npoints)
    {
      prevX = nextX;
      prevY = nextY;
      nextX = (int) points[i].x;
      nextY = (int) points[i].y;
      if (prevY >= y)
        {
          while (i < npoints && nextY >= y)
            {
              i++;		/* changement de point */
              prevY = nextY;
              prevX = nextX;
              nextX = (int) points[i].x;
              nextY = (int) points[i].y;
            }
          if (i >= npoints)
            break;
          cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
        }
      else
        {
          while (i < npoints && nextY < y)
            {
              i++;		/* changement de point */
              prevY = nextY;
              prevX = nextX;
              if (i < npoints)
                {
                  nextX = (int) points[i].x;
                  nextY = (int) points[i].y;
                }
            }
          if (i >= npoints)
            break;
          cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
        }
    }
  ok = (ThotBool) (cross & 0x01);
  return (ok);
}

/*----------------------------------------------------------------------
  GetPolylinePoint teste qu'un point x,y est sur un segment de la    
  boi^te polyline.                                        
  Si oui, retourne l'adresse de la boi^te correspondante et le    
  point de contro^le se'lectionne' (0 pour toute la boi^te).      
  sinon, la valeur NULL.                                          
  ----------------------------------------------------------------------*/
static PtrBox  GetPolylinePoint (PtrAbstractBox pAb, int x, int y, int frame,
                                 int *pointselect)
{
  int                 i, j, nb;
  int                 X1, Y1;
  int                 X2, Y2;
  PtrTextBuffer       adbuff;
  PtrBox              box;
  ThotBool            OK;

  Y1 = 0;
  box = pAb->AbBox;
  nb = box->BxNChars;
  if (nb < 3)
    /* il n'y a pas au moins un segment defini */
    return (NULL);

  /* On calcule le point de controle de la polyline le plus proche */
  adbuff = box->BxBuffer;
  x -= box->BxXOrg;
  y -= box->BxYOrg;
  X1 = -1;			/* Pas de point X1, Y1 au depart */
  X2 = -1;
  *pointselect = 0;
  j = 1;
  for (i = 1; i < nb; i++)
    {
      if (j >= adbuff->BuLength &&
          adbuff->BuNext != NULL)
        {
          /* Changement de buffer */
          adbuff = adbuff->BuNext;
          j = 0;
        }

      /* Teste si le point est sur ce segment */
      X2 = PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL,
                       ViewFrameTable[frame - 1].FrMagnification);
      Y2 = PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL,
                       ViewFrameTable[frame - 1].FrMagnification);
      if (x >= X2 - DELTA_SEL && x <= X2 + DELTA_SEL &&
          y >= Y2 - DELTA_SEL && y <= Y2 + DELTA_SEL)
        {
          /* La selection porte sur un point de controle particulier */
          *pointselect = i;
          return (box);
        }
      else if (X1 == -1)
        OK = FALSE;
      else
        OK = IsOnSegment (x, y, X1, Y1, X2, Y2);

      if (OK)
        /* Le point est sur ce segment -> le test est fini */
        return (box);
      else
        {
          j++;
          X1 = X2;
          Y1 = Y2;
        }
    }
  /* traite le cas particulier des polylines fermees */
  if (pAb->AbPolyLineShape == 'p' || pAb->AbPolyLineShape == 's')
    {
      X2 = PixelValue (box->BxBuffer->BuPoints[1].XCoord, UnPixel, NULL,
                       ViewFrameTable[frame - 1].FrMagnification);
      Y2 = PixelValue (box->BxBuffer->BuPoints[1].YCoord, UnPixel, NULL,
                       ViewFrameTable[frame - 1].FrMagnification);
      OK = IsOnSegment (x, y, X1, Y1, X2, Y2);
      if (OK)
        return (box);
    }
  return (NULL);
}

/*----------------------------------------------------------------------
  BuildPolygonForPath
  Build the polygons that approximate a path
  A different 
  pPa: first segment of the path
  return a list of points and the number of points in this list (npoints).
  if the path is unique, subpathStart is NULL, but if there are several
  subpath, subpathStart is a table of integers: each of them is the rank
  of the first point of each subpath in the list of points returned.
  ----------------------------------------------------------------------*/
ThotPoint *BuildPolygonForPath (PtrPathSeg pPa, int frame,
                                int* npoints, int **subpathStart)
{
  double               x1, y1, cx1, cy1, x2, y2, cx2, cy2;
  double               ix1, ix2, iy1, iy2;
  ThotPoint           *points;
  int                 *tmp;
  int                 maxpoints, maxsubpaths, nbsubpaths;

  /* get a buffer to store the points of the polygon */
  maxpoints = ALLOC_POINTS;
  points = (ThotPoint *) TtaGetMemory (maxpoints * sizeof(ThotPoint));
  memset (points, 0, maxpoints * sizeof(ThotPoint));
  *npoints = 0;
  /* assume there is a single path */
  *subpathStart = NULL;
  nbsubpaths = 0;
  maxsubpaths = 10;
  /* process all segments of the path */
  while (pPa)
    {
      if (pPa->PaNewSubpath && pPa->PaPrevious)
        /* this path segment starts a new subpath */
        {
          if (*subpathStart == NULL)
            /* allocate a table of subpath start points */
            *subpathStart = (int *) TtaGetMemory (maxsubpaths * sizeof(int));
          else if (nbsubpaths >= maxsubpaths - 1)
            /* the current table is full. Extend it */
            {
              maxsubpaths += 10;
              tmp = (int *) realloc(*subpathStart, maxsubpaths * sizeof(int));
              *subpathStart = tmp;
            }
          /* register the rank of the point starting this subpath */
          (*subpathStart)[nbsubpaths++] = *npoints;
          (*subpathStart)[nbsubpaths] = 0;   /* indicate end of table */
        }

      switch (pPa->PaShape)
        {
        case PtLine:
          ix1 = PixelValue (pPa->XStart, UnPixel, NULL,
                            ViewFrameTable[frame - 1].FrMagnification);
          iy1 = PixelValue (pPa->YStart, UnPixel, NULL,
                            ViewFrameTable[frame - 1].FrMagnification);
          ix2 = PixelValue (pPa->XEnd, UnPixel, NULL,
                            ViewFrameTable[frame - 1].FrMagnification);
          iy2 = PixelValue (pPa->YEnd, UnPixel, NULL,
                            ViewFrameTable[frame - 1].FrMagnification);
          PolyNewPoint (ix1, iy1, &points, npoints, &maxpoints);
          PolyNewPoint (ix2, iy2, &points, npoints, &maxpoints);
          break;

        case PtCubicBezier:
          x1 = (double) (PixelValue (pPa->XStart, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          y1 = (double) (PixelValue (pPa->YStart, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          cx1 = (double) (PixelValue (pPa->XCtrlStart, UnPixel, NULL,
                                      ViewFrameTable[frame - 1].FrMagnification));
          cy1 = (double) (PixelValue (pPa->YCtrlStart, UnPixel, NULL,
                                      ViewFrameTable[frame - 1].FrMagnification));
          x2 = (double) (PixelValue (pPa->XEnd, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          y2 = (double) (PixelValue (pPa->YEnd, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          cx2 = (double) (PixelValue (pPa->XCtrlEnd, UnPixel, NULL,
                                      ViewFrameTable[frame - 1].FrMagnification));
          cy2 = (double) (PixelValue (pPa->YCtrlEnd, UnPixel, NULL,
                                      ViewFrameTable[frame - 1].FrMagnification));
          PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, &points, npoints,
                     &maxpoints);
          PolyNewPoint (x2, y2, &points, npoints, &maxpoints);
          break;

        case PtQuadraticBezier:
          x1 = (double) (PixelValue (pPa->XStart, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          y1 = (double) (PixelValue (pPa->YStart, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          cx1 = (double) (PixelValue (pPa->XCtrlStart, UnPixel, NULL,
                                      ViewFrameTable[frame - 1].FrMagnification));
          cy1 = (double) (PixelValue (pPa->YCtrlStart, UnPixel, NULL,
                                      ViewFrameTable[frame - 1].FrMagnification));
          x2 = (double) (PixelValue (pPa->XEnd, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          y2 = (double) (PixelValue (pPa->YEnd, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          QuadraticSplit (x1, y1, cx1, cy1, x2, y2, &points, npoints,
                          &maxpoints);
          PolyNewPoint (x2, y2, &points, npoints, &maxpoints);
          break;

        case PtEllipticalArc:
          x1 = (double) pPa->XStart;
          y1 = (double) pPa->YStart;
          x2 = (double) pPa->XEnd;
          y2 = (double) pPa->YEnd;
          cx1 = (double) pPa->XRadius; 
          cy1 = (double) pPa->YRadius; 	  
          EllipticSplit ( frame, 0, 0,
                          (double) x1, (double) y1, 
                          (double) x2, (double) y2, 
                          (double) cx1, (double) cy1,
                          (int)fmod((double)pPa->XAxisRotation, (double)360), 
                          pPa->LargeArc, pPa->Sweep,
                          &points, npoints, &maxpoints);
          x2 = (double) (PixelValue (pPa->XEnd, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          y2 = (double) (PixelValue (pPa->YEnd, UnPixel, NULL,
                                     ViewFrameTable[frame - 1].FrMagnification));
          PolyNewPoint (x2, y2, &points, npoints, &maxpoints);
          break;
        }
      pPa = pPa->PaNext;
    }
  return (points);
}

/*----------------------------------------------------------------------
  IsInShape returns TRUE if the point x, y is included by the drawing.
  ----------------------------------------------------------------------*/
static ThotBool IsInShape (PtrAbstractBox pAb, int x, int y)
{
  int                 point[8][2];
  int                 cross;
  int                 i, max;
  int                 prevX, prevY;
  int                 nextX, nextY;
  int                 arc;
  double               value1, value2;
  PtrBox              box;
  ThotBool            ok;
  int                 width, height;

  box = pAb->AbBox;
  x -= box->BxXOrg;
  y -= box->BxYOrg;
  width = box->BxWidth;
  height = box->BxHeight;
  max = 0;

  /* Is there a characteristic point of the drawing? */
  switch (pAb->AbRealShape)
    {
    case SPACE:
    case 'R':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':		/* rectangles */
      point[0][0] = 0;
      point[0][1] = 0;
      point[1][0] = 0;
      point[1][1] = height;
      point[2][0] = width;
      point[2][1] = height;
      point[3][0] = width;
      point[3][1] = 0;
      max = 3;
      break;
    case 'C':
    case 'P':		/* rectangles with rounded corners */
      arc = (int) ((3 * DOT_PER_INCH) / 25.4 + 0.5);
      point[0][0] = 0;
      point[0][1] = arc;
      point[1][0] = 0;
      point[1][1] = height - arc;
      point[2][0] = arc;
      point[2][1] = height;
      point[3][0] = width - arc;
      point[3][1] = height;
      point[4][0] = width;
      point[4][1] = height - arc;
      point[5][0] = width;
      point[5][1] = arc;
      point[6][0] = width - arc;
      point[6][1] = 0;
      point[7][0] = arc;
      point[7][1] = 0;
      max = 7;
      break;
    case 'L':		/* losange */
      point[0][0] = 0;
      point[0][1] = height / 2;
      point[1][0] = width / 2;
      point[1][1] = height;
      point[2][0] = width;
      point[2][1] = height / 2;
      point[3][0] = width / 2;
      point[3][1] = 0;
      max = 3;
      break;
    case 'a':		/* circles */
    case 'c':		/* ovals */
    case 'Q':		/* ellipses */
      value1 = x - ((double) width / 2);
      value2 = (y - ((double) height / 2)) *
        ((double) width / (double) height);
      value1 = value1 * value1 + value2 * value2;
      value2 = (double) width / 2;
      value2 = value2 * value2;
      if (value1 <= value2)
        return (TRUE);	/* within the circle */
      else
        return (FALSE);	/* out of the circle */
      break;
    default:
      break;
    }
  if (max < 2)
    /* only lines */
    return (FALSE);

  /* first and last points of the polyline */
  i = 0;
  cross = 0;
  nextX = point[0][0];
  nextY = point[0][1];
  prevX = point[max][0];
  prevY = point[max][1];
  if ((prevY >= y) != (nextY >= y))
    /* y between nextY and prevY */
    cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);

  i++;
  while (i <= max)
    {
      prevX = nextX;
      prevY = nextY;
      nextX = point[i][0];
      nextY = point[i][1];
      if (prevY >= y)
        {
          while (i <= max && nextY >= y)
            {
              i++;		/* get next point */
              prevY = nextY;
              prevX = nextX;
              if (i > max)
                {
                  nextX = 0;
                  nextY = 0;
                }
              else
                {
                  nextX = point[i][0];
                  nextY = point[i][1];
                }
            }
          if (i > max)
            break;
          cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
        }
      else
        {
          while (i <= max && nextY < y)
            {
              i++;		/* get next point */
              prevY = nextY;
              prevX = nextX;
              if (i > max)
                {
                  nextX = 0;
                  nextY = 0;
                }
              else
                {
                  nextX = point[i][0];
                  nextY = point[i][1];
                }
            }
          if (i > max)
            break;
          cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
        }
    }
  ok = (ThotBool) (cross & 0x01);
  return (ok);
}


/*----------------------------------------------------------------------
  IsOnShape checks if the point x, y is on the drawing of pAb.
  If yes, returns the box address, NULL in other cases.
  Return the control point for lines.
  ----------------------------------------------------------------------*/
static PtrBox IsOnShape (PtrAbstractBox pAb, int x, int y, int *selpoint)
{
  ThotFont            font;
  PtrBox              pBox;
  int                 controlPoint;
  int                 arc, xm, xp;
  double              value1, value2, value3;
  int                 width, height;

  /* relative coords of the box (easy work) */
  pBox = pAb->AbBox;
  x -= pBox->BxXOrg;
  y -= pBox->BxYOrg;
    width = pBox->BxWidth;
  height = pBox->BxHeight;
  *selpoint = 0;
  /* Keep in mind the selected caracteristic point       */
  /*            1-------------2-------------3            */
  /*            |                           |            */
  /*            |                           |            */
  /*            8                           4            */
  /*            |                           |            */
  /*            |                           |            */
  /*            7-------------6-------------5            */

  if (x < DELTA_SEL)
    if (y < DELTA_SEL)
      controlPoint = 1;
    else if (y > height / 2 - DELTA_SEL &&
             y < height / 2 + DELTA_SEL)
      controlPoint = 8;
    else if (y > height - 10)
      controlPoint = 7;
    else
      controlPoint = 0;
  else if (x > width / 2 - DELTA_SEL &&
           x < width / 2 + DELTA_SEL)
    if (y < DELTA_SEL)
      controlPoint = 2;
    else if (y > height - DELTA_SEL)
      controlPoint = 6;
    else
      controlPoint = 0;
  else if (x > width - DELTA_SEL)
    if (y < DELTA_SEL)
      controlPoint = 3;
    else if (y > height / 2 - DELTA_SEL &&
             y < height / 2 + DELTA_SEL)
      controlPoint = 4;
    else if (y > height - 10)
      controlPoint = 5;
    else
      controlPoint = 0;
  else
    controlPoint = 0;

  /* Est-ce un point caracteristique specifique du graphique ? */
  if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
    {
      GetFontAndIndexFromSpec (32, pBox->BxFont, 1, &font);
      xp =  FontHeight (font);
      xm = xp / 2;
      xp = xp / 4;
      if (IsOnSegment (x, y, 1, 2 * (height / 3), xp, height) ||
          IsOnSegment (x, y, xp, height, xm, 1) ||
          IsOnSegment (x, y, xm, 1, width, 1))
        return (pBox);
    }
  else
    switch (pAb->AbRealShape)
      {
      case SPACE:
      case 'R':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
        /* rectangle */
        if (IsOnSegment (x, y, 0, 0, width, 0) ||
            IsOnSegment (x, y, 0, height, width,
                         height) ||
            IsOnSegment (x, y, 0, 0, 0, height) ||
            IsOnSegment (x, y, width, 0, width, height))
          return (pBox);
        break;
      case 'L':
        if (IsOnSegment (x, y, 0, height / 2, width / 2, 0) ||
            IsOnSegment (x, y, 0, height / 2, width / 2,
                         height) ||
            IsOnSegment (x, y, width, height / 2,
                         width / 2, 0) ||
            IsOnSegment (x, y, width, height / 2,
                         width / 2, height))
          return (pBox);
        break;
      case 'C':
      case 'P':
        /* rectangle with rounded corners */
        arc = (int) ((3 * DOT_PER_INCH) / 25.4 + 0.5);
        if (IsOnSegment (x, y, arc, 0, width - arc, 0) ||
            IsOnSegment (x, y, 0, arc, 0, height - arc) ||
            IsOnSegment (x, y, arc, height, width - arc,
                         height) ||
            IsOnSegment (x, y, width, arc, width,
                         height - arc))
          return (pBox);
        break;
      case 'a': /* circle */
      case 'c': /* ellipse */
      case 'Q': /* ellipse with a bar */
        /* ellipse or circle */
        value1 = x - ((double) width / 2);
        value2 = (y - ((double) height / 2)) *
          ((double) width / (double) height);
        /* value1 = square of (distance from center to point) */
        value1 = value1 * value1 + value2 * value2;
        /* value2 = square of (radius - DELTA_SEL) */
        value2 = (double) width / 2;
        value3 = value2;
        value2 -= DELTA_SEL;
        value2 = value2 * value2;
        /* value3 = square of (radius + DELTA_SEL) */
        value3 += DELTA_SEL;
        value3 = value3 * value3;

        if (value1 >= value2 && value1 <= value3)
          return (pBox);	/* on the circle */
        break;
      case 'W':
        /* upper right corner of the box */
        if (controlPoint == 1 || controlPoint == 3 || controlPoint == 5 ||
            IsOnSegment (x, y, 0, 0, width, 0) ||
            IsOnSegment (x, y, width, 0, width, height))
          return (pBox);
        break;
      case 'X':
        /* lower right corner of the box */
        if (controlPoint == 3 || controlPoint == 5 || controlPoint == 7 ||
            IsOnSegment (x, y, width, 0, width,
                         height) ||
            IsOnSegment (x, y, width, height, 0,height))
          return (pBox);
        break;
      case 'Y':
        /* a segment with an arrow head at the end */
        if (controlPoint == 1 || controlPoint == 5 || controlPoint == 7 ||
            IsOnSegment (x, y, width, height, 0,
                         height) ||
            IsOnSegment (x, y, 0, height, 0, 0))
          return (pBox);
        break;
      case 'Z':
        /* the upper left corner of the box */
        if (controlPoint == 1 || controlPoint == 3 || controlPoint == 7 ||
            IsOnSegment (x, y, 0, height, 0, 0) ||
            IsOnSegment (x, y, 0, 0, width, 0))
          return (pBox);
        break;
      case 'h':
      case '<':
      case '>':
        /* a horizontal line or arrow */
        if (controlPoint == 4 || controlPoint == 8 ||
            IsOnSegment (x, y, 0, height / 2, width,
                         height / 2))
          return (pBox);
        break;
      case 't':
        /* a horizontal line along the upper side of the box */
        if (controlPoint == 1 || controlPoint == 2 || controlPoint == 3 ||
            IsOnSegment (x, y, 0, 0, width, 0))
          return (pBox);
        break;
      case 'b':
        /* a horizontal line along the lower side of the box */
        if (controlPoint == 5 || controlPoint == 6 || controlPoint == 7 ||
            IsOnSegment (x, y, width, height, 0,height))
          return (pBox);
        break;
      case 'v':
      case '^':
      case 'V':
        /* a vertical line or arrow as tall as the box and placed in its middle*/
        if (controlPoint == 2 || controlPoint == 6 ||
            IsOnSegment (x, y, width / 2, 0, width / 2,
                         height))
          return (pBox);
        break;
      case 'l':
        /* a vertical line on the left side of the box */
        if (controlPoint == 1 || controlPoint == 7 || controlPoint == 8 ||
            IsOnSegment (x, y, 0, height, 0, 0))
          return (pBox);
        break;
      case 'r':
        /* a vertical line on the right side of the box */
        if (controlPoint == 3 || controlPoint == 4 || controlPoint == 5 ||
            IsOnSegment (x, y, width, 0, width, height))
          return (pBox);
        break;
      case '\\':
      case 'O':
      case 'e':
        /* The northwest/southeast diagonal of the box possibly with an
           arrowhead */
        if (controlPoint == 1 || controlPoint == 5 ||
            IsOnSegment (x, y, 0, 0, width, height))
          return (pBox);
        break;
      case '/':
      case 'o':
      case 'E':
        /* The southwest/northeast diagonal of the box possibly with an
           arrowhead */
        if (controlPoint == 3 || controlPoint == 7 ||
            IsOnSegment (x, y, 0, height, width, 0))
          return (pBox);
        break;
      case 'g':
        /* a line from the origin of the box to its opposite corner */
        pAb = pAb->AbEnclosing;
        if ((pAb->AbHorizPos.PosEdge == Left && pAb->AbVertPos.PosEdge == Top) ||
            (pAb->AbHorizPos.PosEdge == Right && pAb->AbVertPos.PosEdge == Bottom))
          {
            /* draw a \ */
            if (controlPoint == 1 || controlPoint == 5)
              {
                *selpoint = controlPoint;
                return (pBox);
              }
            else  if (IsOnSegment (x, y, 0, 0, width, height))
              return (pBox);
          }
        else
          {
            /* draw a / */
            if (controlPoint == 3 || controlPoint == 7)
              {
                *selpoint = controlPoint;
                return (pBox);
              }
            else  if (IsOnSegment (x, y, 0, height, width, 0))
              return (pBox);
          }
        break;
      default:
        break;
      }
  return (NULL);
}

/*----------------------------------------------------------------------
  GetClickedAbsBox checks if the abstract box includes the reference point
  xRef, yRef.
  Returns the most elementary box (structural level) that includes the
  reference point.
  ----------------------------------------------------------------------*/
PtrAbstractBox GetClickedAbsBox (int frame, int xRef, int yRef)
{
  ViewFrame          *pFrame;
  PtrBox              pBox;
  PtrFlow             pFlow = NULL;
  int                 pointselect;

  pFrame = &ViewFrameTable[frame - 1];
  pBox = NULL;
  if (pFrame->FrAbstractBox != NULL)
#ifndef _GL
    GetClickedBox (&pBox, &pFlow, pFrame->FrAbstractBox,
                   frame, xRef + pFrame->FrXOrg,
                   yRef + pFrame->FrYOrg, Y_RATIO, &pointselect);
#else /* _GL */
  GetClickedBox (&pBox, &pFlow, pFrame->FrAbstractBox, frame, xRef,
                 yRef, Y_RATIO, &pointselect);
#endif /* _GL */

  if (pBox == NULL)
    return (NULL);
  else
    return (pBox->BxAbstractBox);
}

/*----------------------------------------------------------------------
  GetEnclosingClickedBox checks if the point x, y belongs to the abstract
  box pAb.
  Returns the box address or NULL.
  ----------------------------------------------------------------------*/
PtrBox GetEnclosingClickedBox (PtrAbstractBox pAb, int higherX,
                               int lowerX, int y, int frame,
                               int *pointselect, PtrFlow *pFlow)
{
  PtrBox              pBox;
  PtrElement          pParent;
  int                 i, x, orgx, orgy;
  ThotPoint           *points = NULL;
  int                 npoints, sub;
  int                 *subpathStart = NULL;
  ThotBool            OK, testSegment;

  *pointselect = 0;
  if (pAb->AbBox)
    {
      pBox = pAb->AbBox;


#ifdef _GL
      x = 0;
      /* Transform windows coordinate x, y to the transformed system 
         of the current box */
      GetBoxTransformedCoord (pAb, frame, &lowerX, &higherX, &x, &y);
#endif /* _GL */

      /* Is there a piece of split box? */
      orgx = pBox->BxXOrg;
      orgy = pBox->BxYOrg;
      if (*pFlow)
        {
          /* apply the box shift */
          orgx += (*pFlow)->FlXStart;
          orgy += (*pFlow)->FlYStart;
        }
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          for (pBox = pBox->BxNexChild; pBox != NULL; pBox = pBox->BxNexChild)
            {
              orgx = pBox->BxXOrg;
              orgy = pBox->BxYOrg;
              if (*pFlow)
                {
                  /* apply the box shift */
                  orgx += (*pFlow)->FlXStart;
                  orgy += (*pFlow)->FlYStart;
                }
              if (pBox->BxNChars > 0 &&
                  orgx <= lowerX &&
                  orgx + pBox->BxWidth >= higherX &&
                  orgy <= y &&
                  orgy + pBox->BxHeight >= y)
                return (pBox);

            }
          return (NULL);
        }
      else if (pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost)
        /* dummy box */
        return (NULL);
      else if (pAb->AbLeafType == LtGraphics &&
               pAb->AbPresentationBox && pAb->AbShape == '0')
        /* it's also a dummy box */
        return (NULL);

      if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
        {
          /* a radical */
          return (IsOnShape (pAb, lowerX, y, pointselect));
        }
      else if (pAb->AbLeafType == LtPolyLine || pAb->AbLeafType == LtPath ||
               /* If the box is not a polyline or a path, it must include
                  the point */
               (orgx <= lowerX &&
                orgx + pBox->BxWidth >= higherX &&
                orgy <= y &&
                orgy + pBox->BxHeight >= y))
        {
          pParent = pAb->AbElement->ElParent;
          if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
            /* It's a simple graphic shape */
            {
              pBox = IsOnShape (pAb, lowerX, y, pointselect);
              if (pBox != NULL)
                /* the point is on the outline */
                return (pBox);
              /* the point is not on the outline */
              if ((pAb->AbFillPattern > 0 && pAb->AbBackground >= 0) ||
                  TypeHasException (ExcClickableSurface, pParent->ElTypeNumber,
                                    pParent->ElStructSchema))
                /* the box is filled. Is the point within the shape? */
                {
                  if (IsInShape (pAb, lowerX, y))
                    return (pAb->AbBox);
                  else
                    return (NULL);
                }
              else
                return (pBox);
            }
          else if (pAb->AbLeafType == LtPolyLine && pAb->AbVolume > 2)
            {
              /* the polyline contains at least one segment */
              pBox = GetPolylinePoint (pAb, lowerX, y, frame, pointselect);
              if (pBox != NULL)
                /* the point doesn't belong to a segment */
                return (pBox);
              if ((pAb->AbFillPattern > 0 && pAb->AbBackground >= 0) ||
                  TypeHasException (ExcClickableSurface, pParent->ElTypeNumber,
                                    pParent->ElStructSchema))
                /* the shape is filled. Is the point within the shape? */
                {
                  if (IsWithinPolyline (pAb, lowerX, y, frame))
                    return (pAb->AbBox);
                  else
                    return (pBox);
                }
            }
          else if (pAb->AbLeafType == LtPath)
            {
              if (!pAb->AbFirstPathSeg)
                /* empty path */
                return (NULL);
              else
                /* it's a non-empty path */
                {
                  /* builds the list of points representing the path */
                  points = BuildPolygonForPath (pAb->AbFirstPathSeg, frame,
                                                &npoints, &subpathStart);
                  /* is the position of interest on the polyline represented by
                     these points? */
                  x = lowerX - pBox->BxXOrg;
                  y -= pBox->BxYOrg;
                  OK = FALSE;
                  sub = 0;
                  /* test every segment comprised between 2 successive points */
                  for (i = 0; (i < npoints - 1) && !OK; i++)
                    {
                      testSegment = TRUE;
                      if (subpathStart)
                        /* there are several subpaths in this path */
                        {
                          if (subpathStart[sub] == i + 1)
                            /* this segment corresponds to a moveto. Skip it */
                            {
                              testSegment = FALSE;
                              /* get prepared for the next subpath */
                              sub++;
                              if (subpathStart[sub] == 0)
                                /* this is the last subpath. Don't test more
                                   subpaths */
                                {
                                  free (subpathStart);
                                  subpathStart = NULL;
                                }
                            }
                        }
                      if (testSegment)
                        OK = IsOnSegment (x, y, (int)points[i].x, (int)points[i].y,
                                          (int)points[i + 1].x, (int)points[i + 1].y);
                    }
                  if (!OK)
                    /* the point is not on the path. Is it within the path ? */
                    {
                      /* check only if the path is filled */
                      if ((pAb->AbFillPattern > 0 && pAb->AbBackground >= 0) ||
                          TypeHasException (ExcClickableSurface,
                                            pParent->ElTypeNumber,
                                            pParent->ElStructSchema))
                        OK = IsWithinPath (x, y, points, npoints);
                    }
                  free (points);
                  if (subpathStart)
                    free (subpathStart);
                  if (OK)
                    return (pBox);
                  else
                    return (NULL);
                }
            }
          else
            return (pBox);
        }
    }
  return (NULL);
}

/*----------------------------------------------------------------------
  GetLeafBox returns the leaf box located at the position x+xDelta
  y+yDelta from pSourceBox box.
  ----------------------------------------------------------------------*/
PtrBox GetLeafBox (PtrBox pSourceBox, int frame, int *x, int *y,
                   int xDelta, int yDelta, PtrFlow *pFlow)
{
  int                 i;
  PtrBox              pBox, pLimitBox;
  PtrBox              box, lastBox;
  PtrLine             pLine;
  ViewFrame          *pFrame;
  int                 max;
  int                 h, lastY;
  ThotBool            found;

  found = FALSE;
  lastBox = NULL;
  lastY = *y;
  pFrame = &ViewFrameTable[frame - 1];

  while (!found)
    {
      pBox = pSourceBox;
      max = pFrame->FrAbstractBox->AbBox->BxWidth;
      /* locate the last box in the line */
      if (xDelta > 0)
        {
          pLimitBox = GetClickedLeafBox (frame, max, *y, pFlow);
          if (pLimitBox == NULL)
            pLimitBox = pSourceBox;
        }
      else if (xDelta < 0)
        {
          pLimitBox = GetClickedLeafBox (frame, 0, *y, pFlow);
          if (pLimitBox == NULL)
            pLimitBox = pSourceBox;
        }
      else
        pLimitBox = NULL;

      i = 0;
      found = TRUE;
      while (pBox == pSourceBox && i < 200 && found)
        {
          i++;
          *x += xDelta;
          *y += yDelta;
          /* Take the leaf box here */
          pBox = GetClickedLeafBox (frame, *x, *y, pFlow);
          if (pBox == NULL)
            pBox = pSourceBox;
          else if (pBox->BxAbstractBox->AbElement)
            {
              if (TypeHasException (ExcNoSelect,
                                    pBox->BxAbstractBox->AbElement->ElTypeNumber,
                                    pBox->BxAbstractBox->AbElement->ElStructSchema))
                pBox = pSourceBox;
              else if (TypeHasException (ExcSelectParent,
                                         pBox->BxAbstractBox->AbElement->ElTypeNumber,
                                         pBox->BxAbstractBox->AbElement->ElStructSchema) &&
                       pBox->BxAbstractBox->AbEnclosing)
                {
                  /* let the algorithm work if we obtain the same box */
                  if (pBox != pSourceBox)
                    pBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
                }
            }
          if (pBox == pSourceBox || pBox->BxAbstractBox->AbBox == pSourceBox ||
              IsParentBox (pSourceBox, pBox))
            {
              /* compute the height to be moved */
              pLine = SearchLine (pBox, frame);
              if (pLine != NULL)
                h = pLine->LiHeight / 2;
              else
                h = 10;
              if (xDelta > 0 && pLimitBox == pBox)
                {
                  /* move one line down */
                  *x = 0;
#ifdef _GL
                  if (pBox->BxBoundinBoxComputed)
                    *y = pBox->BxClipY + pBox->BxClipH;
                  else
                    *y = pBox->BxYOrg + pBox->BxHeight - pFrame->FrYOrg;
#else /* _GL */
                  *y = pBox->BxYOrg + pBox->BxHeight;
#endif /* _GL */
                  xDelta = 0;
                  yDelta = h;
                  found = FALSE;
                }
              else if (xDelta < 0 && pLimitBox == pBox)
                {
                  /* move one line up */
                  if (pLine  && pLine->LiPrevious)
                    {
                      box = pBox->BxAbstractBox->AbEnclosing->AbBox;
                      while (box->BxType == BoGhost || box->BxType == BoFloatGhost)
                        box = box->BxAbstractBox->AbEnclosing->AbBox;
#ifdef _GL
                      if (pBox->BxBoundinBoxComputed)
                        *x = box->BxClipX;
                      else
                        *x = box->BxXOrg - pFrame->FrXOrg;
                      *x += pLine->LiXOrg + pLine->LiPrevious->LiRealLength;
#else /* _GL */
                      *x = box->BxXOrg + pLine->LiXOrg + pLine->LiPrevious->LiRealLength;
#endif /* _GL */
                      yDelta = -2;
                    }
                  else
                    {
                      *x = max;
                      yDelta = -h;
                    }
#ifdef _GL
                  if (pBox->BxBoundinBoxComputed)
                    *y = pBox->BxClipY;
                  else
                    *y = pBox->BxYOrg - pFrame->FrYOrg;
#else /* _GL */
                  *y = pBox->BxYOrg;
#endif /* _GL */
                  xDelta = 0;
                  found = FALSE;
                }
            }
          else if (pBox->BxAbstractBox->AbLeafType != LtText &&
                   pBox->BxNChars != 0)
            {
              /* the box doesn't match, skip it */
#ifdef _GL
              if (pBox->BxBoundinBoxComputed)
                {
                  if (xDelta > 0)
                    *x = pBox->BxClipX + pBox->BxClipW;
                  else if (xDelta < 0)
                    *x = pBox->BxClipX;
                  if (yDelta > 0)
                    *y = pBox->BxClipY + pBox->BxClipH;
                  else if (yDelta < 0)
                    *y = pBox->BxClipY;
                }
              else
                {
                  if (xDelta > 0)
                    *x = pBox->BxXOrg - pFrame->FrXOrg + pBox->BxWidth;
                  else if (xDelta < 0)
                    *x = pBox->BxXOrg - pFrame->FrXOrg;
                  if (yDelta > 0)
                    *y = pBox->BxYOrg - pFrame->FrYOrg + pBox->BxHeight;
                  else if (yDelta < 0)
                    *y = pBox->BxYOrg - pFrame->FrYOrg;
                }
#else /* _GL */
              if (xDelta > 0)
                *x = pBox->BxXOrg + pBox->BxWidth;
              else if (xDelta < 0)
                *x = pBox->BxXOrg;
              if (yDelta > 0)
                *y = pBox->BxYOrg + pBox->BxHeight;
              else if (yDelta < 0)
                *y = pBox->BxYOrg;
#endif /* _GL */
	      
              pBox = pSourceBox;
            }
        }
    }
  return (pBox);
}

/*----------------------------------------------------------------------
  GetGetMinimumDistance computes the distance between a reference point
  xRef, yRef and an anchor x, y of a box.
  We apply a ratio to vertical distances to give a preference to the
  horizontal proximity.
  ----------------------------------------------------------------------*/
static int GetGetMinimumDistance (int xRef, int yRef, int x, int y, int dist)
{
  int                 value;

  value = GetDistance (x - xRef, ANCHOR_SIZE) +
    GetDistance (y - yRef, ANCHOR_SIZE) * Y_RATIO;
  if (value < dist)
    return (value);
  else
    return (dist);
}

/*----------------------------------------------------------------------
  GetShapeDistance computes the distance between a reference point
  xRef, yRef and a point of the graphic box (minimum distance to an anchor
  of the box.
  This selection takes only laf boxes into account.
  Returns the distance.
  ----------------------------------------------------------------------*/
int GetShapeDistance (int xRef, int yRef, PtrBox pBox, int value, int frame)
{
  int                 distance;
  int                 x, y, width, height;

  /* centrer la boite */
#ifdef _GL
  if (pBox->BxBoundinBoxComputed)
    {
      width = pBox->BxClipW / 2;
      x = pBox->BxClipX + width;
      height = pBox->BxClipH / 2;
      y = pBox->BxClipY + height;
    }
  else
    {
      width = pBox->BxWidth / 2;
      x = pBox->BxXOrg - ViewFrameTable[frame -1].FrXOrg + width;
      height = pBox->BxHeight / 2;
      y = pBox->BxYOrg - ViewFrameTable[frame -1].FrYOrg + height;
    }
#else /* _GL */
  width = pBox->BxWidth / 2;
  x = pBox->BxXOrg + width;
  height = pBox->BxHeight / 2;
  y = pBox->BxYOrg + height;
#endif /* _GL */
  distance = 1000;

  switch (value)
    {
    case 1:		/* root .. */
      distance = GetGetMinimumDistance (xRef, yRef, x - width + height / 3,
                                        y + height, distance);
      distance = GetGetMinimumDistance (xRef, yRef,
                                        x - width + (2 * height) / 3,
                                        y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
                                        distance);
      break;

    case 'c':		/* circle */
      if (width < height)
        height = width;
      else
        width = height;
      /* Warning: continue as a circle is also an ellipse */
    case 'C':		/* ellipse */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      height = (71 * height) / 100;
      width = (71 * width) / 100;
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
                                        distance);
      break;

    case 'L':		/* losange */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      break;

    case 't':		/* top line */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      break;

    case 'b':		/* bottom line */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      break;

    case 'h':		/* middle line or arrow */
    case '<':
    case '>':
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;

    case 'v':
    case 'V':		/* centerd line or arrow (^) */
    case '^':
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;

    case 'l':		/* left line */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      break;
    case 'r':		/* right line */
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      break;

    case '/':		/* diagonal (/) or arrow */
    case 'E':
    case 'o':
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;

    case '\\':		/* diagonal (\) or arrow */
    case 'e':
    case 'O':
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;
    default:
      /* rectangle or other cases */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
                                        distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
                                        distance);
      break;
    }
  return (distance);
}

/*----------------------------------------------------------------------
  GetClickedLeafBox looks for a leaf box located at a reference point
  xRef, yRef.
  ----------------------------------------------------------------------*/
PtrBox GetClickedLeafBox (int frame, int xRef, int yRef, PtrFlow *pFlow)
{
  PtrAbstractBox      pAb;
  PtrBox              pSelBox, pBox;
  PtrBox              box;
  PtrElement          matchCell = NULL, prevMatch;
  int                 max;
  int                 pointIndex;
  int                 d;
  ViewFrame          *pFrame;

  pBox = NULL;
  pSelBox = NULL;
  /* au-dela de max, on n'accepte pas la selection */
  max = THOT_MAXINT;
  pFrame = &ViewFrameTable[frame - 1];

  if (pFrame->FrAbstractBox != NULL)
    pBox = pFrame->FrAbstractBox->AbBox;
  if (pBox != NULL)
    {
      pBox = pBox->BxNext;
      while (pBox)
        {
          pAb = pBox->BxAbstractBox;
          prevMatch = matchCell;
          if (matchCell && !ElemIsAnAncestor (matchCell, pAb->AbElement))
            // the element is not within that cell
            ;
          else if (/*pAb->AbVisibility >= pFrame->FrVisibility &&*/
              (!pAb->AbPresentationBox || pAb->AbCanBeModified))
            {
              if (pAb->AbLeafType == LtGraphics ||
                  pAb->AbLeafType == LtPolyLine ||
                  pAb->AbLeafType == LtPath)
                {
                  box = GetEnclosingClickedBox (pAb, xRef, xRef, yRef,
                                                frame, &pointIndex, pFlow);
                  if (box == NULL)
                    d = max + 1;
                  else
                    d = 0;
                }
              else if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
                /* glitch for the root symbol */
                d = GetShapeDistance (xRef, yRef, pBox, 1, frame);
              else if (pAb->AbLeafType == LtText ||
                       pAb->AbLeafType == LtSymbol ||
                       pAb->AbLeafType == LtPicture ||
                        /* empty or compound box */
                       (pAb->AbLeafType == LtCompound &&
                        (pAb->AbVolume == 0 || 
                         pBox->BxType == BoBlock || pBox->BxNChars == 0)))
                d = GetBoxDistance (pBox, *pFlow, xRef, yRef, Y_RATIO, frame, &matchCell);
              else
                d = max + 1;

              /* get the closest element */
              if (prevMatch != matchCell && matchCell)
                // ignore previous boxes out of the current cell
                max = THOT_MAXINT;
              if (d < max)
                {
                  max = d;
                  pSelBox = pBox;
                }
              else if (d == max)
                {
                  /* If it's the first found box */
                  if (pSelBox == NULL)
                    {
                      max = d;
                      pSelBox = pBox;
                    }
                  /* If the box is over the previous selected box */
                  else if (pSelBox->BxAbstractBox->AbDepth >=
                           pBox->BxAbstractBox->AbDepth)
                    {
                      max = d;
                      pSelBox = pBox;
                    }
                }
            }
          pBox = pBox->BxNext;
        }
    }
  return pSelBox;
}

/*----------------------------------------------------------------------
  GiveMovingArea get limits of the box moving.
  ----------------------------------------------------------------------*/
static void GiveMovingArea (PtrAbstractBox pAb, int frame,
                            ThotBool horizRef, int *min, int *max)
{
#ifdef IV
  PtrAbstractBox      pParentAb;
#endif /* IV */
  /* default values */
  *min = 0;
  *max = 100000;

  /* It's the root box */
  if (pAb == ViewFrameTable[frame - 1].FrAbstractBox)
    {
      if (horizRef)
        {
          /* does the box depend on the window */
          if ((pAb->AbWidth.DimValue == 0)
              || (pAb->AbBox->BxHorizEdge == Right))
            *max = pAb->AbBox->BxWidth;
        }
      else
        {
          if ((pAb->AbHeight.DimValue == 0)
              || (pAb->AbBox->BxVertEdge == Bottom))
            *max = pAb->AbBox->BxHeight;
        }
    }
  /* Other cases */
  else if (horizRef)
    {
      *min = -50;
      *max = 100000;
#ifdef IV
      /* check the enclosing abstract box */
      if (pAb->AbHorizEnclosing)
        pParentAb = pAb->AbEnclosing;
      else
        pParentAb = ViewFrameTable[frame - 1].FrAbstractBox;

      /* by default the emclosing box gives limits */
      *min = pParentAb->AbBox->BxXOrg;
      *max = *min + pParentAb->AbBox->BxWidth;

      if (pParentAb->AbBox->BxContentWidth)
        /* the enclosing box gets the contents size and the box
           doesn't depend of the enclosing */
        switch (pAb->AbBox->BxHorizEdge)
          {
          case Left:
            *max = 100000;
            break;
          case Right:
            *min = 0;
            break;
          default:
            *min = 0;
            *max = 100000;
            break;
          }
#endif /* IV */
    }
  else
    {
      *min = -50;
      *max = 100000;
#ifdef IV
      /* check the enclosing abstract box */
      if (pAb->AbVertEnclosing && pAb->AbEnclosing != NULL)
        {
          pParentAb = pAb;
          do
            pParentAb = pParentAb->AbEnclosing;
          while (pParentAb->AbBox->BxType == BoGhost ||
                 pParentAb->AbBox->BxType == BoFloatGhost);
        }
      else
        pParentAb = ViewFrameTable[frame - 1].FrAbstractBox;

      /* by default the emclosing box gives limits */
      *min = pParentAb->AbBox->BxYOrg;
      *max = *min + pParentAb->AbBox->BxHeight;

      if (pParentAb->AbBox->BxContentHeight)
        /* the enclosing box gets the contents size and the box
           doesn't depend of the enclosing */
        switch (pAb->AbBox->BxVertEdge)
          {
          case Top:
            *max = 100000;
            break;
          case Bottom:
            *min = 0;
            break;
          default:
            *min = 0;
            *max = 100000;
            break;
          }
#endif /* IV */
    }
}

/*----------------------------------------------------------------------
  CanBeTranslated teste si un pave est modifiable en position     
  (X ou Y), et si oui, rend les positions extremes        
  de la boite.                                            
  ----------------------------------------------------------------------*/
static ThotBool     CanBeTranslated (PtrAbstractBox pAb, int frame,
                                     ThotBool horizRef, int *min, int *max)
{
  PtrAbstractBox      pParentAb;
  PtrBox              pBox;
  PtrElement          pEl;
  PtrDocument         pDoc;
  ThotBool            ok, found;

  pBox = pAb->AbBox;
  pParentAb = pAb->AbEnclosing;
  pEl = pAb->AbElement;
  pDoc = DocumentOfElement (pEl);

  /* Deplacement nul si ok est faux */
  if (horizRef)
    *min = pBox->BxXOrg;
  else
    *min = pBox->BxYOrg;

  *max = *min;
  ok = TRUE;
  if (pDoc->DocReadOnly)
    ok = FALSE;
  else if (pEl->ElIsCopy)
    ok = FALSE;
  else if (ElementIsReadOnly (pEl))
    ok = FALSE;
  else if (pAb->AbPresentationBox)
    /* presentation box */
    ok = FALSE;
  else if (horizRef && pAb->AbWidth.DimIsPosition)
    /* stretchable box */
    ok = FALSE;
  else if (!horizRef && pAb->AbHeight.DimIsPosition)
    /* stretchable box */
    ok = FALSE;
  else if (horizRef && pAb->AbWidth.DimAbRef == pParentAb)
    /* box width linked with its parent */
    ok = FALSE;
  else if (!horizRef && pAb->AbHeight.DimAbRef == pParentAb)
    /* box height linked with its parent */
    ok = FALSE;
  else if (horizRef && pAb->AbHorizPos.PosAbRef == NULL)
    /* no position rule */
    ok = FALSE;
  else if (!horizRef && pAb->AbVertPos.PosAbRef == NULL)
    /* no position rule */
    ok = FALSE;
  else if ( pParentAb->AbBox->BxType == BoBlock ||
            pParentAb->AbBox->BxType == BoFloatBlock ||
            pParentAb->AbBox->BxType == BoCellBlock ||
            pParentAb->AbBox->BxType == BoGhost ||
            pParentAb->AbBox->BxType == BoFloatGhost)
    /* box displayed in block of lines */
    ok = FALSE;
  else
    {
      /* search the first rule Move or NoMove */
      found = FALSE;
      while (!found && ok && pEl != NULL)
        {
          if (TypeHasException (ExcNoMove, pEl->ElTypeNumber,
                                pEl->ElStructSchema))
            ok = FALSE;
          else if (horizRef && TypeHasException (ExcNoHMove, pEl->ElTypeNumber,
                                                 pEl->ElStructSchema))
            ok = FALSE;
          else if (!horizRef && TypeHasException (ExcNoVMove,pEl->ElTypeNumber,
                                                  pEl->ElStructSchema))
            ok = FALSE;
          else if (TypeHasException (ExcMoveResize, pEl->ElTypeNumber,
                                     pEl->ElStructSchema))
            found = TRUE;
          /* if no directive is done, see the parent */
          pEl = pEl->ElParent;
        }

      if (!found && ok && pParentAb != NULL)
        {
          /* it's not the root box */
          if (horizRef &&
              /* et le pParentAb ne depend pas de son contenu */
              pParentAb->AbBox->BxContentWidth &&
              !pParentAb->AbWidth.DimIsPosition &&
              !pParentAb->AbWidth.DimMinimum &&
              pAb->AbHorizPos.PosAbRef == pParentAb &&
              pAb->AbHorizPos.PosRefEdge != Left)
            ok = FALSE;
          else if (!horizRef &&
                   /* et le pParentAb ne depend pas de son contenu */
                   pParentAb->AbBox->BxContentHeight &&
                   !pParentAb->AbHeight.DimIsPosition &&
                   !pParentAb->AbHeight.DimMinimum &&
                   pAb->AbVertPos.PosAbRef == pParentAb &&
                   pAb->AbVertPos.PosRefEdge != Top)
            ok = FALSE;
        }
    }

  if (horizRef)
    if (ok)
      {
        GiveMovingArea (pAb, frame, horizRef, min, max);
        /* La boite est-elle bloquee dans l'englobante ? */
        if (*min == pBox->BxXOrg && *max == *min + pBox->BxWidth)
          ok = FALSE;
      }
    else
      {
        *min = pBox->BxXOrg;
        *max = *min + pBox->BxWidth;
      }
  else if (ok)
    {
      GiveMovingArea (pAb, frame, horizRef, min, max);
      /* La boite est-elle bloquee dans l'englobante ? */
      if (*min == pBox->BxYOrg && *max == *min + pBox->BxHeight)
        ok = FALSE;
    }
  else
    {
      *min = pBox->BxYOrg;
      *max = *min + pBox->BxHeight;
    }

  return ok;
}

/*----------------------------------------------------------------------
  ApplyDirectTranslate looks for the selected box for a move.
  If the smallest box containing point (xm, ym) in the window cannot
  be moved, the function checks the encolsing box, etc.
  ----------------------------------------------------------------------*/
void ApplyDirectTranslate (int frame, int xm, int ym)
{
  PtrDocument         pDoc;
  PtrBox              pBox;
  PtrAbstractBox      pAb;
  PtrElement	      pEl;
  ViewFrame          *pFrame;
  PtrFlow             pFlow = NULL;
  int                 x, width;
  int                 y, height;
  int                 xmin, xmax;
  int                 ymin, ymax;
  int                 xref, yref;
  int                 pointselect;
  int                 view;
  ThotBool            open, still;
  ThotBool            okH, okV, send;
  
  pFrame = &ViewFrameTable[frame - 1];
  GetDocAndView (frame, &pDoc, &view);
  if (pDoc == NULL)
    return;
  
  open = !pDoc->DocEditSequence;
  /* by default no selected point */
  pointselect = 0;
  if (pFrame->FrAbstractBox != NULL)
    {
      /* Get positions in the window */
#ifndef _GL
      x = xm + pFrame->FrXOrg;
      y = ym + pFrame->FrYOrg;
#else /*_GL*/
      x = xm;
      y = ym;
#endif /* _GL */
      /* Look for the box displayed at that point */
      GetClickedBox (&pBox, &pFlow, pFrame->FrAbstractBox,
                     frame, x, y, Y_RATIO, &pointselect);
      if (pBox)
        {
          pAb = pBox->BxAbstractBox;
          if (pointselect && pBox->BxType != BoPicture)
            {
              /* moving a polyline point */
              still = FALSE;
              xmin = ymin = 0;
              xmax = ymax = 9999;
            }
          else
            {
              /* moving the whole box */
              still = TRUE;
              pointselect = 0;
            }
          /* Loop as long as a box that can be moved is not found */
          while (still)
            {
              /* check if the moving is allowed */
              okH = CanBeTranslated (pAb, frame, TRUE, &xmin, &xmax);
              okV = CanBeTranslated (pAb, frame, FALSE, &ymin, &ymax);
              if (okH || okV)
                still = FALSE;
              if (still)
                {
                  /* no box found yet, check the enclosing box */
                  if (pAb != NULL)
                    pAb = pAb->AbEnclosing;
                  if (pAb == NULL)
                    {
                      pBox = NULL;
                      still = FALSE;
                    }
                  else
                    pBox = pAb->AbBox;
                }
            }
          
          if (pBox != NULL)
            {
              /* A box is found */
              x = pBox->BxXOrg - pFrame->FrXOrg;
              y = pBox->BxYOrg - pFrame->FrYOrg;
              width = pBox->BxWidth;
              height = pBox->BxHeight;
              pEl = pBox->BxAbstractBox->AbElement;
              send = FALSE;
              if (pointselect != 0 && pBox->BxType != BoPicture)
                /* moving a single point */
                {
                  if (pAb->AbLeafType == LtGraphics && pAb->AbShape == 'g')
                    /* moving an end of a line */
                    {
                      LineModification (frame, pBox, pointselect, &x, &y);
                      /* get back current changes */
                      if (!pAb->AbWidth.DimIsPosition && pAb->AbEnclosing)
                        /* this rule is applied to the parent */
                        pAb = pAb->AbEnclosing;
                      pBox = pAb->AbBox;
                      switch (pointselect)
                        {
                        case 1:
                        case 7:
                          if (pBox->BxHorizInverted)
                            NewDimension (pAb, x, y, frame, TRUE);
                          else
                            NewPosition (pAb, x, 0, y, 0, frame, TRUE);
                          break;
                        case 3:
                        case 5:
                          if (pBox->BxHorizInverted)
                            NewPosition (pAb, x, 0, y, 0, frame, TRUE);
                          else
                            NewDimension (pAb, x, y, frame, TRUE);
                          break;
                        default: break;
                        }
                    }
                  else
                    /* moving a point in a polyline */
                    {
                      /* send an event to the application */
                      if (!APPgraphicModify (pEl, pointselect, frame, TRUE, open))
                        /* application agrees */
                        {
                          /* check if the polyline is open or closed */
                          still = (pAb->AbPolyLineShape == 'p' ||
                                   pAb->AbPolyLineShape == 's');
                          PolyLineModification (frame, &x, &y, pBox,
                                                pBox->BxNChars,
                                                pointselect, still);
                          NewContent (pAb);
                          send = TRUE;
                        }
                    }
                  
                  
                  /* redisplay the box */
                  DefBoxRegion (frame, pBox, 0, 0, width, height);
                  RedrawFrameBottom (frame, 0, NULL);
                }
              else
                /* moving the whole box */
                {
                  /* set positions related to the window */
                  xmin -= pFrame->FrXOrg;
                  xmax -= pFrame->FrXOrg;
                  ymin -= pFrame->FrYOrg;
                  ymax -= pFrame->FrYOrg;
                  /* execute the interaction */
                  GeometryMove (frame, &x, &y, width, height, pBox, xmin,
                                xmax, ymin, ymax, xm, ym);
                  /* get back changes */
                  x += pFrame->FrXOrg;
                  y += pFrame->FrYOrg;
                  /* get the position of reference point */
                  switch (pBox->BxHorizEdge)
                    {
                    case Right:
                      xref = width;
                      break;
                    case VertMiddle:
                      xref = width / 2;
                      break;
                    case VertRef:
                      xref = pBox->BxVertRef;
                      break;
                    default:
                      xref = 0;
                      break;
                    }
                  switch (pBox->BxVertEdge)
                    {
                    case Bottom:
                      yref = height;
                      break;
                    case HorizMiddle:
                      yref = height / 2;
                      break;
                    case HorizRef:
                      yref = pBox->BxHorizRef;
                      break;
                    default:
                      yref = 0;
                      break;
                    }
                  NewPosition (pAb, x, xref, y, yref, frame, TRUE);
                }
              if (send)
                APPgraphicModify (pEl, pointselect, frame, FALSE, open);
            }
        }
    }
}

/*----------------------------------------------------------------------
  CanBeResized teste si un pave est modifiable en Dimension.       
  ----------------------------------------------------------------------*/
static ThotBool   CanBeResized (PtrAbstractBox pAb, int frame,
                                ThotBool horizRef, int *min, int *max)
{
  PtrBox              pBox;
  PtrAbstractBox      pParentAb;
  PtrElement          pEl;
  PtrDocument         pDoc;
  ThotBool            ok, found;

  pBox = pAb->AbBox;
  pParentAb = pAb->AbEnclosing;
  pEl = pAb->AbElement;
  pDoc = DocumentOfElement (pEl);

  if (horizRef)
    *min = pBox->BxXOrg;
  else
    *min = pBox->BxYOrg;

  *max = *min;
  ok = TRUE;
  if (pDoc->DocReadOnly)
    ok = FALSE;
  else if (pEl->ElIsCopy)
    ok = FALSE;
  else if (ElementIsReadOnly (pEl))
    ok = FALSE;
  else if (pAb->AbPresentationBox)
    /* presentation box */
    ok = FALSE;
  else if (horizRef && pAb->AbWidth.DimIsPosition)
    /* stretchable box */
    ok = FALSE;
  else if (!horizRef && pAb->AbHeight.DimIsPosition)
    /* stretchable box */
    ok = FALSE;
  else if (pAb->AbLeafType == LtText &&
           (pParentAb->AbBox->BxType == BoBlock ||
            pParentAb->AbBox->BxType == BoFloatBlock ||
            pParentAb->AbBox->BxType == BoCellBlock ||
            pParentAb->AbBox->BxType == BoGhost ||
            pParentAb->AbBox->BxType == BoFloatGhost))
    /* text box displayed in block of lines */
    ok = FALSE;
  /* Il est impossible de modifier la dimension du contenu */
  /* d'une boite construite ou de type texte                  */
  /*****else if (pAb->AbLeafType == LtCompound || pAb->AbLeafType == LtText)
        {
        if (horizRef && pBox->BxContentWidth && !pAb->AbWidth.DimMinimum)
        ok = FALSE;
        else if (!horizRef && pBox->BxContentHeight && !pAb->AbHeight.DimMinimum)
        ok = FALSE;
        }********/
  else if (horizRef && pAb->AbWidth.DimAbRef == pParentAb)
    /* box width linked with its parent */
    ok = FALSE;
  else if (!horizRef && pAb->AbHeight.DimAbRef == pParentAb)
    /* box height linked with its parent */
    ok = FALSE;
  else
    {
      /* search the first rule Move or NoMove */
      found = FALSE;
      while (!found && ok && pEl != NULL)
        {
          if (TypeHasException (ExcNoResize, pEl->ElTypeNumber,
                                pEl->ElStructSchema))
            ok = FALSE;
          else if (horizRef && TypeHasException (ExcNoHResize,
                                                 pEl->ElTypeNumber,
                                                 pEl->ElStructSchema))
            ok = FALSE;
          else if (!horizRef && TypeHasException (ExcNoVResize,
                                                  pEl->ElTypeNumber,
                                                  pEl->ElStructSchema))
            ok = FALSE;
          else if (TypeHasException (ExcMoveResize, pEl->ElTypeNumber,
                                     pEl->ElStructSchema))
            found = TRUE;
          /* if no directive is done, see the parent */
          pEl = pEl->ElParent;
        }

      if (!found && ok && pParentAb != NULL)
        {
          /* it's not the root box */
          if ((horizRef && pAb->AbWidth.DimValue < 0) ||
              (!horizRef && pAb->AbHeight.DimValue < 0))
            ok = FALSE;
        }
    }

  if (horizRef)
    if (ok)
      GiveMovingArea (pAb, frame, horizRef, min, max);
    else
      {
        *min = pBox->BxXOrg;
        *max = *min + pBox->BxWidth;
      }
  else if (ok)
    GiveMovingArea (pAb, frame, horizRef, min, max);
  else
    {
      *min = pBox->BxYOrg;
      *max = *min + pBox->BxHeight;
    }

  return ok;
}

/*----------------------------------------------------------------------
  ApplyDirectResize looks for a box that can be resized at the current
  position (xm, ym).
  ----------------------------------------------------------------------*/
void ApplyDirectResize (int frame, int xm, int ym)
{
  PtrBox              pBox;
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  PtrFlow             pFlow;
  int                 x, width;
  int                 y, height;
  int                 xmin, xmax;
  int                 ymin, ymax;
  int                 percentW, percentH;
  int                 pointselect;
  ThotBool            still, okH, okV;

  okH = FALSE;
  okV = FALSE;
  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox != NULL)
    {
      /* On note les coordonnees par rapport a l'image concrete */
#ifndef _GL
      x = xm + pFrame->FrXOrg;
      y = ym + pFrame->FrYOrg;
#else /* _GL */
      x = xm;
      y = ym;
#endif /* _GL */
      /* On recherche la boite englobant le point designe */
      /* designation style Grenoble */
      GetClickedBox (&pBox, &pFlow, pFrame->FrAbstractBox,
                     frame, x, y, Y_RATIO, &pointselect);
      if (pBox == NULL)
        pAb = NULL;
      else
        pAb = pBox->BxAbstractBox;
      
      /* ctrlClick */
      still = TRUE;
      /* On boucle tant que l'on ne trouve pas une boite modifiable */
      while (still)
        {
          if (pAb == NULL)
            pBox = NULL;
          else
            pBox = pAb->AbBox;
          
          if (pBox == NULL)
            still = FALSE;	/* Il n'y a pas de boite */
          /* On regarde si les modifications sont autorisees */
          else
            {
              okH = CanBeResized (pAb, frame, TRUE, &xmin, &xmax);
              okV = CanBeResized (pAb, frame, FALSE, &ymin, &ymax);
              if (okH || okV)
                still = FALSE;
            }
          
          /* Si on n'a pas trouve, il faut remonter */
          if (still)
            {
              /* On passe a la boite englobante */
              if (pAb != NULL)
                pAb = pAb->AbEnclosing;
              else
                {
                  pBox = NULL;
                  still = FALSE;
                }
            }
        }
      
      /* Est-ce que l'on a trouve une boite ? */
      if (pBox != NULL)
        {
          x = pBox->BxXOrg - pFrame->FrXOrg;
          y = pBox->BxYOrg - pFrame->FrYOrg;
          width = pBox->BxWidth;
          height = pBox->BxHeight;
          /* On retablit les positions par rapport a la fenetre */
          xmin -= pFrame->FrXOrg;
          if (okH)
            xmax -= pFrame->FrXOrg;
          else
            xmax = xmin;
          ymin -= pFrame->FrYOrg;
          if (okV)
            ymax -= pFrame->FrYOrg;
          else
            ymax = ymin;
          /* On initialise la boite fantome */
          percentW = 0;
          percentH = 0;
          pAb = pBox->BxAbstractBox;
          if (!pAb->AbWidth.DimUserSpecified &&
              !pAb->AbWidth.DimIsPosition &&
              !pAb->AbWidth.DimSameDimension &&
              pAb->AbWidth.DimAbRef == pAb)
            {
              if (pAb->AbWidth.DimUnit == UnPercent)
                percentW = pAb->AbWidth.DimValue;
              else if (pAb->AbWidth.DimValue == 0)
                percentW = 100;
            }
          else if (!pAb->AbHeight.DimUserSpecified &&
                   !pAb->AbHeight.DimIsPosition &&
                   !pAb->AbHeight.DimSameDimension &&
                   pAb->AbHeight.DimAbRef == pAb)
            {
              if (pAb->AbHeight.DimUnit == UnPercent)
                percentH = pAb->AbHeight.DimValue;
              else if (pAb->AbHeight.DimValue == 0)
                percentH = 100;
            }
          GeometryResize (frame, x, y, &width, &height, pBox,
                          xmin, xmax, ymin, ymax, xm, ym,
                          percentW, percentH);
          /* Notify changes */
          if (percentW)
            NewDimension (pAb, 0, height, frame, TRUE);
          else if (percentH)
            NewDimension (pAb, width, 0, frame, TRUE);
          else
            NewDimension (pAb, width, height, frame, TRUE);
        }
    }
}

/*----------------------------------------------------------------------
  DirectCreation does the direct creation of graphics
  ----------------------------------------------------------------------*/
void DirectCreation (PtrBox pBox, int frame)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  PtrDocument         pDoc;
  int                 x, y, xref, yref;
  int                 width, height;
  int                 xmin, xmax;
  int                 Ymin, Ymax;
  int                 percentW, percentH;
  ThotBool            modPosition, modDimension;
  ThotBool            histOpen;

  /* Only one interaction at the same time */
  if (BoxCreating)
    return;
  else
    BoxCreating = TRUE;

  pFrame = &ViewFrameTable[frame - 1];
  /* Il faut verifier que la boite reste visible dans la fenetre */
  GetSizesFrame (frame, &width, &height);
  if (pBox->BxXOrg < pFrame->FrXOrg)
    x = 0;
  else if (pBox->BxXOrg > pFrame->FrXOrg + width)
    x = width;
  else
    x = pBox->BxXOrg - pFrame->FrXOrg;

  if (pBox->BxYOrg < pFrame->FrYOrg)
    y = 0;
  else if (pBox->BxYOrg > pFrame->FrYOrg + height)
    y = height;
  else
    y = pBox->BxYOrg - pFrame->FrYOrg;
  width = pBox->BxWidth;
  height = pBox->BxHeight;
  pAb = pBox->BxAbstractBox;
  modPosition = (CanBeTranslated (pAb, frame, TRUE, &xmin, &xmax) ||
                 CanBeTranslated (pAb, frame, FALSE, &Ymin, &Ymax));
  if (!modPosition)
    {
      pAb->AbHorizPos.PosUserSpecified = FALSE;
      pAb->AbVertPos.PosUserSpecified = FALSE;
    }
  modDimension = (CanBeResized (pAb, frame, TRUE, &xmin, &xmax) ||
                  CanBeResized (pAb, frame, FALSE, &Ymin, &Ymax));
  if (!modDimension)
    {
      pAb->AbWidth.DimUserSpecified = FALSE;
      pAb->AbHeight.DimUserSpecified = FALSE;
    }
  if (modPosition || modDimension)
    {
      /* Determine les limites de deplacement de la boite */
      GiveMovingArea (pAb, frame, TRUE, &xmin, &xmax);
      GiveMovingArea (pAb, frame, FALSE, &Ymin, &Ymax);
      /* On retablit les positions par rapport a la fenetre */
      xmin -= pFrame->FrXOrg;
      xmax -= pFrame->FrXOrg;
      Ymin -= pFrame->FrYOrg;
      Ymax -= pFrame->FrYOrg;
      /*
        calcule les rapports largeur sur hauteur et hauteur sur
        largeur si une des deux dimensions depend de l'autre
        percentW = 0 si la largeur ne depend pas de la hauteur
        percentH = 0 si la hauteur ne depend pas de la largeur
      */
      percentW = 0;
      percentH = 0;
      if (!pAb->AbWidth.DimUserSpecified &&
          !pAb->AbWidth.DimIsPosition &&
          !pAb->AbWidth.DimSameDimension &&
          pAb->AbWidth.DimAbRef == pAb)
        {
          if (pAb->AbWidth.DimUnit == UnPercent)
            percentW = pAb->AbWidth.DimValue;
          else if (pAb->AbWidth.DimValue == 0)
            percentW = 100;
        }
      else if (!pAb->AbHeight.DimUserSpecified &&
               !pAb->AbHeight.DimIsPosition &&
               !pAb->AbHeight.DimSameDimension &&
               pAb->AbHeight.DimAbRef == pAb)
        {
          if (pAb->AbHeight.DimUnit == UnPercent)
            percentH = pAb->AbHeight.DimValue;
          else if (pAb->AbHeight.DimValue == 0)
            percentH = 100;
        }
      GeometryCreate (frame, &x, &y, &width, &height,
                      xmin, xmax, Ymin, Ymax, pBox,
                      pAb->AbHorizPos.PosUserSpecified,
                      pAb->AbVertPos.PosUserSpecified,
                      pAb->AbWidth.DimUserSpecified,
                      pAb->AbHeight.DimUserSpecified,
                      percentW, percentH);

      /* Notification of the new created box */
      x = x + pFrame->FrXOrg;
      y = y + pFrame->FrYOrg;

      /* get the position of reference point */
      switch (pBox->BxHorizEdge)
        {
        case Right:
          xref = width;
          break;
        case VertMiddle:
          xref = width / 2;
          break;
        case VertRef:
          xref = pBox->BxVertRef;
          break;
        default:
          xref = 0;
          break;
        }
      switch (pBox->BxVertEdge)
        {
        case Bottom:
          yref = height;
          break;
        case HorizMiddle:
          yref = height / 2;
          break;
        case HorizRef:
          yref = 0;
          break;
        default:
          yref = 0;
          break;
        }

      pDoc = DocumentOfElement (pAb->AbElement);
      histOpen = pDoc->DocEditSequence;
      if (!histOpen)
        OpenHistorySequence (pDoc, pAb->AbElement, pAb->AbElement, NULL, 0, 0);
      NewPosition (pAb, x, xref, y, yref, frame, TRUE);
      if (percentW)
        NewDimension (pAb, 0, height, frame, TRUE);
      else if (percentH)
        NewDimension (pAb, width, 0, frame, TRUE);
      else
        NewDimension (pAb, width, height, frame, TRUE);
      DefBoxRegion (frame, pBox, 0, 0, width, height);

      pAb->AbHorizPos.PosUserSpecified = FALSE;
      pAb->AbVertPos.PosUserSpecified = FALSE;
      pAb->AbWidth.DimUserSpecified = FALSE;
      pAb->AbHeight.DimUserSpecified = FALSE;
      if (!histOpen)
        CloseHistorySequence (pDoc);	  
    }

  /* interaction finished */
  BoxCreating = FALSE;
}

/*----------------------------------------------------------------------
  LocateClickedChar looks for the character of the box displayed at the
  position x.
  The parameter extend is TRUE when the function looks for an extension
  of the current selection.
  Returns:
  - the buffer where the character is located,
  - the index in the buffer,
  - the index in the box,
  - an the number of white spaces before.
  The position x is updated too.
  ----------------------------------------------------------------------*/
void LocateClickedChar (PtrBox pBox, int frame, ThotBool extend,
                        PtrTextBuffer *pBuffer, int *x, int *index,
                        int *nChars, int *nSpaces)
{
  int                 dx, ind;
  int                 length;
  int                 extraSpace;
  int                 spaceWidth;
  int                 charWidth;
  int                 t, b, l, r, variant;
  SpecFont            font;
  CHAR_T              c;
  ThotBool            notfound, rtl;

  GetExtraMargins (pBox, NULL, frame, FALSE, &t, &b, &l, &r);
  *x = *x - l - pBox->BxLMargin - pBox->BxLBorder - pBox->BxLPadding;
  /* Nombre de caracteres qui precedent */
  *index = 0;
  c = EOS;
  dx = l;
  charWidth = 0;
  if (pBox->BxAbstractBox->AbUnicodeBidi == 'O')
    rtl = (pBox->BxAbstractBox->AbDirection == 'R');
  else
    rtl = (pBox->BxScript == 'A' || pBox->BxScript == 'H');
  /* locate the first character */
  LocateFirstChar (pBox, rtl, pBuffer, &ind);
  if (rtl)
    *nChars = pBox->BxNChars;
  else
    *nChars = 0;
  *nSpaces = 0;
  if (pBox->BxNChars == 0 || *x <= 0)
    *x = 0;
  else
    {
      font = pBox->BxFont;
      if (pBox->BxAbstractBox)
        variant = pBox->BxAbstractBox->AbFontVariant;
      else
        variant = 1;
      dx = 0;
      length = pBox->BxNChars;
      /* space width */
      if (pBox->BxSpaceWidth == 0)
        {
          spaceWidth = BoxCharacterWidth (SPACE, 1, font);
          extraSpace = 0;
        }
      else
        {
          spaceWidth = pBox->BxSpaceWidth;
          extraSpace = pBox->BxNPixels;
        }
      notfound = TRUE;
      while (notfound && length > 0)
        {
          /* look for the selected character */
          c = (*pBuffer)->BuContent[ind];
          if (c == 0)
            charWidth = 0;
          else if (c == SPACE)
            charWidth = spaceWidth;
          else
            {
              if ( c >= 0x060C && c <= 0x06B0 ) /*arabic char*/
                charWidth = BoxArabicCharacterWidth (c, pBuffer, &ind, font);
              else
                charWidth = BoxCharacterWidth (c, variant, font);
            }
          if (extend)
            notfound = (dx + (charWidth / 2) < *x);
          else
            {
              notfound = (dx + (charWidth / 2) < *x);
              if (!notfound)
                *x = dx;
            }

          if (notfound)
            {
              /* continue */
              if (c == SPACE)
                {
                  (*nSpaces)++;
                  if (extraSpace > 0)
                    {
                      dx++;
                      extraSpace--;
                    }
                }
              dx += charWidth;
              if (rtl)
                (*nChars)--;
              else
                (*nChars)++;
              /* next character */
              if (LocateNextChar (pBuffer, &ind, rtl))
                length--;
              else
                length = 0;
            }
        }

      /* character found, manage extensions */
      if (length == 0 && *x < pBox->BxW)
        {
          /* get the ending position of the character */
          if (rtl && *nChars > 0)
            {
              if (ind <= 0 && (*pBuffer)->BuPrevious)
                {
                  /* selection in the next buffer */
                  *pBuffer = (*pBuffer)->BuPrevious;
                  ind = (*pBuffer)->BuLength;
                }
              else
                ind++;
              (*nChars)--;
            }
          else if (*nChars < pBox->BxNChars)
            {
              if (ind >= (*pBuffer)->BuLength && (*pBuffer)->BuNext)
                {
                  /* selection in the next buffer */
                  *pBuffer = (*pBuffer)->BuNext;
                  ind = 0;
                }
              else
                ind++;
              (*nChars)++;
            }
        }
    }
  *x = dx;
  *index = ind;
}

