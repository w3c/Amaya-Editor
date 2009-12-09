/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* 
 * This module computes Concrete Images.
 *
 * Author: I. Vatton (INRIA)
 */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "language.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"

#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#ifdef _GL
#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */
#include "displaybox_f.h"
#include "animbox_f.h"
#include "glwindowdisplay.h"
#endif /* _GL */
#include "tree_f.h"

extern Frame_Ctl   FrameTable[MAX_FRAME+1];

#define MAX_BOX_INWORK 10
static PtrAbstractBox  BoxInWork[MAX_BOX_INWORK];
static int             BiwIndex = 0;

#include "abspictures_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "boxpositions_f.h"
#include "boxrelations_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "content_f.h"
#include "displayselect_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "registry_f.h"
#include "scroll_f.h"
#include "stix_f.h"
#include "structselect_f.h"
#include "tableH_f.h"
#include "textcommands_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"
#include "spline_f.h"

#define		_2xPI		6.2832
#define		_1dSQR2		0.7071
#define		_SQR2		1.4142

#if defined(_WINGUI)
#define M_PI   3.14159265358979323846
#endif /* _WINGUI */
#if defined(_WINGUI) || defined(_WX)
#define M_PI_2 1.57079632679489661923
#endif /* _WINGUI  || _WX */
static ThotBool EmbeddedScript = FALSE;



/*----------------------------------------------------------------------
  IsSVGComponent returns TRUE if the element is a SVG component and
  then can be transformed.
  ----------------------------------------------------------------------*/
ThotBool IsSVGComponent (PtrElement pEl)
{
  if (pEl && !(pEl->ElTerminal) &&
      pEl->ElStructSchema &&
      pEl->ElStructSchema->SsName &&
      !strcmp (pEl->ElStructSchema->SsName,"SVG") &&
      pEl->ElParent && pEl->ElParent->ElStructSchema &&
      pEl->ElParent->ElParent != NULL &&
      pEl->ElParent->ElStructSchema->SsName &&
      !strcmp (pEl->ElParent->ElStructSchema->SsName,"SVG"))
     // the element and its parent are SVG elements
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  SearchNextAbsBox returns the first child or the next sibling or the
  next sibling of the father.
  When pRoot is not Null, the returned abstract box has to be included
  within the pRoot.
  ----------------------------------------------------------------------*/
PtrAbstractBox SearchNextAbsBox (PtrAbstractBox pAb, PtrAbstractBox pRoot)
{
  if (pAb == NULL)
    return (NULL);
  else if (pAb->AbFirstEnclosed != NULL)
    /*the first child */
    return (pAb->AbFirstEnclosed);
  else if (pAb == pRoot)
    /* there is no children within this box */
    return (NULL);
  else if (pAb->AbNext != NULL)
    /*the next sibling */
    return (pAb->AbNext);
  else
    {
      /* the next sibling of the father */
      do
        if (pAb->AbEnclosing != NULL && pAb->AbEnclosing != pRoot)
          pAb = pAb->AbEnclosing;
        else
          return (NULL);
      while (pAb->AbNext == NULL);
      return (pAb->AbNext);
    }
}

/*----------------------------------------------------------------------
  GetParentTable returns the enclosing table or NULL.                
  ----------------------------------------------------------------------*/
PtrAbstractBox GetParentTable (PtrBox pBox)
{
  PtrAbstractBox      pAb;
  ThotBool            found;

  /* check parents */
  found = FALSE;
  pAb = pBox->BxAbstractBox->AbEnclosing;
  while (pAb != NULL && !found)
    {
      if (pAb->AbBox != NULL && pAb->AbBox->BxType == BoTable)
        found = TRUE;
      else
        pAb = pAb->AbEnclosing;
    }
  return (pAb);
}


/*----------------------------------------------------------------------
  GetParentCell returns the enclosing cell or NULL.                
  ----------------------------------------------------------------------*/
PtrAbstractBox GetParentCell (PtrBox pBox)
{
  PtrAbstractBox      pAb;
  ThotBool            found;

  if (pBox == NULL || pBox->BxType == BoColumn ||  pBox->BxType == BoRow)
    pAb = NULL;
  else if (pBox->BxAbstractBox == NULL)
    pAb = NULL;
  else
    {
      /* check parents */
      found = FALSE;
      pAb = pBox->BxAbstractBox->AbEnclosing;
      while (pAb != NULL && !found)
        {
          if (pAb->AbBox != NULL && pAb->AbBox->BxType == BoCell)
            found = TRUE;
          else
            pAb = pAb->AbEnclosing;
        }
    }
  return (pAb);
}

/*----------------------------------------------------------------------
  GetParentDraw returns the enclosing Draw or NULL.                
  ----------------------------------------------------------------------*/
PtrAbstractBox GetParentDraw (PtrBox pBox)
{
  PtrAbstractBox      pAb;
  ThotBool            found;

  if (pBox == NULL)
    pAb = NULL;
  else if (pBox->BxAbstractBox == NULL)
    pAb = NULL;
  else
    {
      /* check parents */
      found = FALSE;
      pAb = pBox->BxAbstractBox->AbEnclosing;
      while (pAb != NULL && !found)
        {
          if (TypeHasException (ExcIsDraw, pAb->AbElement->ElTypeNumber,
                                pAb->AbElement->ElStructSchema))
            found = TRUE;
          else
            pAb = pAb->AbEnclosing;
        }
    }
  return (pAb);
}

/*----------------------------------------------------------------------
  GetParentGroup returns the enclosing Group or NULL.                
  ----------------------------------------------------------------------*/
PtrAbstractBox GetParentGroup (PtrBox pBox)
{
  PtrAbstractBox      pAb;
  ThotBool            found;

  if (pBox == NULL)
    pAb = NULL;
  else if (pBox->BxAbstractBox == NULL)
    pAb = NULL;
  else
    {
      /* check parents */
      found = FALSE;
      pAb = pBox->BxAbstractBox->AbEnclosing;
      while (pAb != NULL && !found)
        {
          if (TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
                                pAb->AbElement->ElStructSchema))
            found = TRUE;
          else
            pAb = pAb->AbEnclosing;
        }
    }
  return (pAb);
}

/*----------------------------------------------------------------------
  GetParentMarker returns the enclosing Marker or NULL.                
  ----------------------------------------------------------------------*/
PtrAbstractBox GetParentMarker (PtrBox pBox)
{
  PtrAbstractBox      pAb;
  ThotBool            found;

  if (pBox == NULL)
    pAb = NULL;
  else if (pBox->BxAbstractBox == NULL)
    pAb = NULL;
  else
    {
      /* check parents */
      found = FALSE;
      pAb = pBox->BxAbstractBox->AbEnclosing;
      while (pAb != NULL && !found)
        {
          if (TypeHasException (ExcIsMarker, pAb->AbElement->ElTypeNumber,
                                pAb->AbElement->ElStructSchema))
            found = TRUE;
          else
            pAb = pAb->AbEnclosing;
        }
    }
  return (pAb);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void SetControlPoints (float x, float y, float l1, float l2,
                              float theta1, float theta2, C_points * cp)
{
  float               s, theta, r;

  r = (float) (1.0 - 0.45);
  /* 0 <= theta1, theta2 < 2PI */
  theta = (theta1 + theta2) / 2;

  if (theta1 > theta2)
    {
      s = (float) sin ((double) (theta - theta2));
      theta1 = theta + (float) M_PI_2;
      theta2 = theta - (float) M_PI_2;
    }
  else
    {
      s = (float) sin ((double) (theta2 - theta));
      theta1 = theta - (float) M_PI_2;
      theta2 = theta + (float) M_PI_2;
    }

  if (s > _1dSQR2)
    s = (float) _SQR2 - s;
  s *= r;
  l1 *= s;
  l2 *= s;
  cp->lx = x + l1 * (float) cos ((double) theta1);
  cp->ly = y - l1 * (float) sin ((double) theta1);
  cp->rx = x + l2 * (float) cos ((double) theta2);
  cp->ry = y - l2 * (float) sin ((double) theta2);
}


/*----------------------------------------------------------------------
  ComputeControlPoints calcule les points de controle de la courbe.     
  Le parametre nb contient le nombre de points + 1        
  definis dans la polyline.                               
  ----------------------------------------------------------------------*/
C_points *ComputeControlPoints (PtrTextBuffer buffer, int nb)
{
  C_points           *controls;
  PtrTextBuffer       pBuffer;
  int                 i, j;
  float               dx, dy;
  float               x, y, x1, y1, x2, y2, x3, y3;
  float               l1, l2, theta1, theta2;

  /* alloue la liste des points de controle */
  controls = (C_points *) TtaGetMemory (sizeof (C_points) * nb);

  pBuffer = buffer;
  j = 1;
  x1 = (float) pBuffer->BuPoints[j].XCoord;
  y1 = (float) pBuffer->BuPoints[j].YCoord;
  j++;
  x2 = (float) pBuffer->BuPoints[j].XCoord;
  y2 = (float) pBuffer->BuPoints[j].YCoord;
  if (nb < 3)
    {
      /* cas particulier des courbes avec 2 points */
      x3 = x2;
      y3 = y2;
    }
  else
    {
      j++;
      x3 = (float) pBuffer->BuPoints[j].XCoord;
      y3 = (float) pBuffer->BuPoints[j].YCoord;
    }

  dx = x1 - x2;
  dy = y2 - y1;
  l1 = (float) sqrt ((double) (dx * dx + dy * dy));
  if (l1 == 0.0)
    theta1 = 0.0;
  else
    theta1 = (float) atan2 ((double) dy, (double) dx);

  dx = x3 - x2;
  dy = y2 - y3;
  l2 = (float) sqrt ((double) (dx * dx + dy * dy));
  if (l2 == 0.0)
    theta2 = 0.0;
  else
    theta2 = (float) atan2 ((double) dy, (double) dx);

  /* -PI <= theta1, theta2 <= PI */
  /* 0 <= theta1, theta2 < 2PI */
  if (theta1 < 0)
    theta1 += (float) _2xPI;
  if (theta2 < 0)
    theta2 += (float) _2xPI;
  SetControlPoints (x2, y2, l1, l2, theta1, theta2, &controls[2]);

  nb--;			/* dernier point */
  x = x2;			/* memorise les points pour fermer la courbe */
  y = y2;
  for (i = 3; i <= nb; i++)
    {
      x2 = x3;
      y2 = y3;
      l1 = l2;
      if (theta2 >= M_PI)
        theta1 = theta2 - (float) M_PI;
      else
        theta1 = theta2 + (float) M_PI;

      if (i == nb)
        {
          /* Traitement du dernier point */
          x3 = x1;
          y3 = y1;
        }
      else
        {
          j++;
          if (j >= pBuffer->BuLength)
            {
              if (pBuffer->BuNext != NULL)
                {
                  /* Changement de buffer */
                  pBuffer = pBuffer->BuNext;
                  j = 0;
                }
            }
          x3 = (float) pBuffer->BuPoints[j].XCoord;
          y3 = (float) pBuffer->BuPoints[j].YCoord;
        }
      dx = x3 - x2;
      dy = y2 - y3;
      l2 = (float) sqrt ((double) (dx * dx + dy * dy));
      if (l2 == 0.0)
        theta2 = 0.0;
      else
        theta2 = (float) atan2 ((double) dy, (double) dx);
      if (theta2 < 0)
        theta2 += (float) _2xPI;
      SetControlPoints (x2, y2, l1, l2, theta1, theta2, &controls[i]);
    }

  /* Traitement du 1er point */
  x2 = x3;
  y2 = y3;
  l1 = l2;
  if (theta2 >= M_PI)
    theta1 = theta2 - (float) M_PI;
  else
    theta1 = theta2 + (float) M_PI;

  dx = x - x2;
  dy = y2 - y;
  l2 = (float) sqrt ((double) (dx * dx + dy * dy));
  if (l2 == 0.0)
    theta2 = 0.0;
  else
    theta2 = (float) atan2 ((double) dy, (double) dx);
  if (theta2 < 0)
    theta2 += (float) _2xPI;
  SetControlPoints (x2, y2, l1, l2, theta1, theta2, &controls[1]);

  return (controls);
}

/*----------------------------------------------------------------------
  GiveTextParams returns the width and the number of included spaces
  of the box.
  Parameters:
  pBuffer points to the first buffer.
  ind gives the index of the first character in the buffer.
  nChars gives the number of characters concerned.
  font the used font.
  nSpaces equals 0 when the space width is default font value, or gives
  the requested value.
  dir gives the main direction (override, embed, or previous).
  bidi says 'O' for override, 'E' for embed, etc.
  embedded is TRUE when it's a embedded Script
  Returns:
  The script of the box.
  pBuffer points to the next buffer.
  ind returns the index of the next character in the buffer.
  nChars returns the number of characters not handled.
  width returns the width of the text.
  ----------------------------------------------------------------------*/
char GiveTextParams (PtrTextBuffer *pBuffer, int *ind, int *nChars,
                     SpecFont font, int variant, int *width, int *nSpaces,
                     char dir, char bidi, ThotBool *embedded, char prevscript)
{
  char                script, sc;
  char                newscript, embed = '*';
  int                 oldind = 0;
  int                 oldpos = 0;
  int                 oldspaces = 0;
  int                 oldwidth = 0;
  PtrTextBuffer       oldbuff = NULL;
  CHAR_T              car;
  int                 pos, max;
  int                 charWidth;
  int                 spaceWidth;
  char                ss;  

  /* space width */
  if (*nSpaces == 0)
    spaceWidth = BoxCharacterWidth (SPACE, 1, font);
  else
    spaceWidth = *nSpaces;
  /* first character in the buffer */
  max = *nChars;
  /* returned values */
  *nChars = 0;
  *nSpaces = 0;
  *width = 0;
  /* initial scripts */
  script = '*';
  pos = 0;
  while (pos < max)
    {
      /* check the current character */
      while (*ind >= (*pBuffer)->BuLength)
        {
          /* skip empty buffers */
          *pBuffer = (*pBuffer)->BuNext;
          if (*pBuffer == NULL)
            return script;
          *ind = 0;
        }
      car = (*pBuffer)->BuContent[*ind];
      if (bidi != 'O')
        {
          /* the text must be analysed and may be split */
          newscript = TtaGetCharacterScript (car);
          if (newscript == ' ')
            {
              if (script == '*' && *ind == 0)
                {
                  if (!(*embedded))
                    script = prevscript;
                  else if (prevscript == 'A')
                    script = 'L';
                  else
                    script = 'A';
                }
              if (script == 'A')
                {
                  if (car >= 0x060C && car <= 0x06B0) /* arabic char */
                    charWidth = BoxArabicCharacterWidth (car, pBuffer, ind, font);
                  else
                    charWidth = BoxCharacterWidth (car, 1, font);
                  *width += charWidth;
                  if (car == 0x28 || car == 0x29 )
                    *embedded = TRUE;
                  if (car != EOS)
                    pos++;
                  /* next character */
                  (*ind)++;
                }
              else if (script == 'L')
                {
                  if (*embedded)  /* embedded script compute the next */
                    {
                      if (*ind == *nChars && (*pBuffer)->BuNext)
                        ss = TtaGetCharacterScript ((*pBuffer)->BuNext->BuContent[0]);
                      else if (*ind == *nChars -1)
                        {
                          if (dir == 'R' )
                            ss = 'A';
                          else
                            ss = 'L';
                        }
                      else
                        ss = TtaGetCharacterScript((*pBuffer)->BuContent[*ind +1]);
		      
                      if ( ss  !='L')
                        {
                          newscript = 'A';
                          oldpos = pos;
                          oldind = *ind;
                          oldspaces = *nSpaces;
                          oldwidth = *width;
                          oldbuff = *pBuffer;
                        }
                    }
                }
              if (script == '*' && *embedded)
                {
                  script = newscript = 'A';
                  *embedded = FALSE;
                }
            }
  
          if (newscript == 'D')
            {
              if (script!='*' && embed !='*')
                embed= 'E';

              if (oldbuff == NULL && pos)
                {
                  /* keep in memory a possible splitting position */
                  oldpos = pos;
                  oldind = *ind;
                  oldspaces = *nSpaces;
                  oldwidth = *width;
                  oldbuff = *pBuffer;
                }
	    
              if (*ind == *nChars && (*pBuffer)->BuNext)
                ss = TtaGetCharacterScript ((*pBuffer)->BuNext->BuContent[0]); 
              else if (*ind == *nChars -1) 
                {
                  if (dir == 'R' )
                    ss = 'A'; 
                  else
                    ss = 'L';  
                }
              else
                ss = TtaGetCharacterScript ((*pBuffer)->BuContent[*ind +1]); 

              if (ss == 'D' && script == '*')
                script = newscript = 'L';
              if (ss == 'D' && script == 'A') 
                {
                  *nChars = max - oldpos;
                  *ind = oldind;
                  *nSpaces = oldspaces;
                  *width = oldwidth;
                  *pBuffer = oldbuff;
                  return script; 
                }
            }

          if (newscript == 'A')
            {
              if (*ind == *nChars && (*pBuffer)->BuNext)
                ss = TtaGetCharacterScript ((*pBuffer)->BuNext->BuContent[0]); 
              else if (*ind == *nChars -1) 
                {
                  if (dir == 'R' )
                    ss = 'A'; 
                  else
                    ss = 'L';  
                }
              else
                ss = TtaGetCharacterScript((*pBuffer)->BuContent[*ind +1]); 

              if (ss != 'D' && newscript == 'D')
                {
                  if (prevscript == 'A')
                    {
                      oldpos = pos;
                      oldind = *ind;
                      oldspaces = *nSpaces;
                      oldwidth = *width;
                      oldbuff = *pBuffer;
                      if (script == '*') script = 'L';
                    }
                  return script;
                }
              if ( ss == 'D' )
                {
                  oldpos = pos;
                  oldind = *ind;
                  oldspaces = *nSpaces;
                  oldwidth = *width;
                  oldbuff = *pBuffer;
                }
            }

          if (script == '*' && newscript != ' ' && newscript != 'D')
            {
              /* no script detected */
              if (oldbuff && bidi == 'E')
                {
                  /* neutral characters found before and direction requested */
                  if (dir == 'R' && newscript != 'A' && newscript != 'H')
                    script = embed = 'A';
                  else if (dir == 'L' && (newscript == 'A' || newscript == 'H'))
                    script = embed = 'L';
                  else
                    {
                      script = newscript;
                      sc = script;
                      if (embed == '*')
                        embed = script;
                    }
                }
              else
                {
                  /* the script is known now */
                  script = newscript;
                  if (embed == '*')
                    embed = script;
                }
            }
	  
          if (newscript == script && oldbuff)
            {
              /* keep in memory the current position */
              oldbuff = NULL;
              /* other scripts are enclosed */
            }
          else if (newscript != script &&
                   (newscript == 'A' || newscript == 'H' ||
                    newscript == 'L' || newscript == 'Z'))
            {
              /* the box must be split because the script changes */
              if (script == 'A' || script == 'H')
                {
                  if (oldbuff && dir == 'L')
                    {
                      /* attach neutral to the next substring */
                      *nChars = max - oldpos;
                      *ind = oldind;
                      *nSpaces = oldspaces;
                      *width = oldwidth;
                      *pBuffer = oldbuff;
                    }
                  else
                    /* attach neutral to the previous substring */
                    *nChars = max - pos ;
                }
              else if (script != 'A' && script != 'H')
                {
                  if (oldbuff && dir == 'R')
                    {
                      /* attach neutral to the next substring */
                      *nChars = max - oldpos;
                      *ind = oldind;
                      *nSpaces = oldspaces;
                      *width = oldwidth;
                      *pBuffer = oldbuff;
                    }
                  else
                    /* attach neutral to the previous substring */
                    *nChars = max - pos ;
                }
              else
                /* split here */
                *nChars = max - pos ;
              return script;
            }
        }

      if (car == SPACE)
        {
          (*nSpaces)++;	/* a space */
          charWidth = spaceWidth;
        }
      else if (car >= 0x060C && car <= 0x06B0) /* arabic char */
        charWidth = BoxArabicCharacterWidth (car, pBuffer, ind, font);
      else
        charWidth = BoxCharacterWidth (car, variant, font);
      *width += charWidth;
      if (car != EOS)
        pos++;
      /* next character */
      (*ind)++;
    }
  if (script == '*')
    script = 'L';
  return script;
}


/*----------------------------------------------------------------------
  GivePictureSize gives the internal size of a picture box.
  ----------------------------------------------------------------------*/
static void GivePictureSize (PtrAbstractBox pAb, int zoom, int *width,
                             int *height)
{
  ThotPictInfo       *picture;
  PtrBox              box;
 
  box = pAb->AbBox;
  picture = (ThotPictInfo *) box->BxPictInfo;
  if (pAb->AbVolume == 0 || picture == NULL)
    {
      *width = 0;
      *height = 0;
    }
  else
    {
      *width = PixelValue (picture->PicWArea, UnPixel, pAb, 0/*zoom*/);
      *height = PixelValue (picture->PicHArea, UnPixel, pAb, 0/*zoom*/);
    }
}


/*----------------------------------------------------------------------
  GiveSymbolSize gives the internal size of a symbol box.
  ----------------------------------------------------------------------*/
void GiveSymbolSize (PtrAbstractBox pAb, int *width, int *height)
{
  SpecFont            font;
  PtrBox              box;
  int                 hfont;
  ThotFont            pfont = NULL;

  box = pAb->AbBox;
  font = box->BxFont;
  hfont = BoxFontHeight (font, 'G');
  GetMathFontFromChar (pAb->AbShape, font, (void **) &pfont,
                       font->FontSize);
  if (pfont)
    {
      GiveStixSize (pfont, pAb, width, height, font->FontSize);
      return;      
    }

  if (pAb->AbVolume == 0)
    {
      /* empty Symbol */
      *width = BoxCharacterWidth (SPACE, 1, font);
      *height = hfont * 2;
    }
  else
    {
      //*height = hfont * 2;
      *height = hfont;
      switch (pAb->AbShape)
        {
        case 'c':	/*integrale curviligne */
        case 'i':	/*integrale */
          *width = BoxCharacterWidth (0xf3, 1, font);
          break;
        case 'd':	/*integrale double */
        case '1':	/* Clockwise Integral */
        case '2':	/* ClockwiseContourIntegral */
        case '3':	/* CounterClockwiseContourIntegral */
          *width = BoxCharacterWidth (0xf3, 1, font) * 3 / 2;
          break;
        case 'e':	/* double contour integral */
          *width = BoxCharacterWidth (0xf3, 1, font) * 2;
          break;
        case 't':	/*integrale triple */
        case 'f':	/* triple contour integral */
          *width = BoxCharacterWidth (0xf3, 1, font) * 5 / 2;
          break;
        case 'r':	/* root */
          *width = hfont;
          break;
        case 'S':	/*sigma */
        case 'P':	/*pi */
        case 'I':	/*intersection */
        case 'U':	/*union */
          *width = BoxCharacterWidth (229, 1, font);
          break;
        case 8 : /* HorizontalLine ; U02500 */
        case 9 : /* UnderBar ; U00332 */
        case 10 : /* OverBar ; U000AF */
        case '-':
        case '_':
        case 'h': /* overline */
          *height = hfont / 4;
          break;
        case 'b': /* Over bracket */
        case 'B': /* Under bracket */
        case 'o': /* over brace */
        case 'p': /* over parenthesis */
        case 'q': /* under parenthesis */
        case 'u': /* under brace */
        case 'L': /* LeftArrow */
        case 'R': /* RightArrow */
        case 'A': /* LeftRightArrow */
          *height = hfont / 2;
          break;
        case 'H': /* hat */
        case 'k': /* hacek */
        case 'T': /* Diacritical Tilde */
        case 160: /* LeftTeeArrow */
        case 162: /* RightTeeArrow */
        case 164: /* LeftVector */
        case 165: /* DownLeftVector */
        case 168: /* RightVector */
        case 169: /* DownRightVector */
        case 172: /* RightArrowLeftArrow */
        case 174: /* LeftArrowRightArrow */
        case 175: /* DoubleLeftArrow */
        case 177: /* DoubleRightArrow */
        case 179: /* DoubleLeftRightArrow */
        case 181: /* LeftArrowBar */
        case 182: /* RightArrowBar */
        case 184: /* LongLeftArrow */
        case 185: /* LongRightArrow */
        case 186: /* LongLeftRightArrow */
        case 187: /* DoubleLongLeftArrow */
        case 188: /* DoubleLongRightArrow */
        case 189: /* DoubleLongLeftRightArrow */
        case 192: /* LeftRightVector */
        case 194: /* DownLeftRightVector */
        case 196: /* LeftVectorBar */
        case 197: /* RightVectorBar */
        case 200: /* DownLeftVectorBar */
        case 201: /* DownRightVectorBar */
        case 204: /* LeftTeeVector */
        case 205: /* RightTeeVector */
        case 208: /* DownLeftTeeVector */
        case 209: /* DownRightTeeVector */
        case 214: /* DownRightTeeVector */
          *height = hfont;
          break;
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:  /* VerticalSeparator */
        case 11: /* VerticalBar */
        case 12: /* DoubleVerticalBar */
          *width = BoxCharacterWidth (0xe6, 1, font);
          if (pAb->AbShape == 12)
            *width *= 2;
           break;
        case '<':
        case '>':
          *width = BoxCharacterWidth (0xf1, 1, font) * 2;
          break;
        case '|':       /* VerticalLine */
          *width = BoxCharacterWidth (0x7c, 1, font);  /* | */
          break;
        case 'D':       /* double vertical bar */
          *width = BoxCharacterWidth (0x7c, 1, font) * 3;  /* | */
          break;

        case 'v':
        case '^': /* UpArrow */
        case 'V': /* DownArrow */
        case 155: /* UpDownArrow */
          *width = BoxCharacterWidth (0x6c, 1, font);	/* 'm' */
          break;

        case 161: /* UpTeeArrow */
        case 163: /* DownTeeArrow */
        case 166: /* RightUpVector */
        case 167: /* LeftUpVector */
        case 170: /* RightDownVector */
        case 171: /* LeftDownVector */
        case 173: /* UpArrowDownArrow */
        case 176: /* DoubleUpArrow */
        case 178: /* DoubleDownArrow */
        case 180: /* DoubleUpDownArrow */
        case 183: /* DownArrowUpArrow */
        case 190: /* UpArrowBar */
        case 191: /* DownArrowBar */
        case 193: /* RightUpDownVector */
        case 195: /* LeftUpDownVector */
        case 198: /* RightUpVectorBar */
        case 199: /* RightDownVectorBar */
        case 202: /* LeftUpVectorBar */
        case 203: /* LeftDownVectorBar */
        case 206: /* RightUpTeeVector */
        case 207: /* RightDownTeeVector */
        case 210: /* LeftUpTeeVector */
        case 211: /* LeftDownTeeVector */
        case 212: /* UpEquilibrium */
        case 213: /* ReverseUpEquilibrium */
          *width = BoxCharacterWidth (SPACE, 1, font) * 4;
          break;
        case '?':
          *width = BoxCharacterWidth (0x3f, 1, font);   /* ? */
          break;
        default:
          *width = BoxCharacterWidth (SPACE, 1, font);
          break;
        }
    }
}


/*----------------------------------------------------------------------
  GiveGraphicSize gives the internal size of a graphics box.
  ----------------------------------------------------------------------*/
void GiveGraphicSize (PtrAbstractBox pAb, int *width, int *height)
{
  SpecFont            font;
  PtrBox              box;
  int                 hfont;

  box = pAb->AbBox;
  font = box->BxFont;
  *width = BoxCharacterWidth (109, 1, font);	/*'m' */
  /* use the Symbols font if possible */
  hfont = BoxFontHeight (font, 'G');
  *height = hfont * 2;
  switch (pAb->AbShape)
    {
    case '<':
    case 'h':
    case '>':
    case 't':
    case 'b':
      *width = *height;
      *height = hfont / 3;
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 'C':
    case 'a':
    case 'c':
    case 'L':
    case '/':
    case '\\':
    case 'O':
    case 'o':
    case 'E':
    case 'e':
      *width = *height;
      break;
    default:
      break;
    }
}


/*----------------------------------------------------------------------
  GivePolylineSize gives the internal size of a polyline.  
  ----------------------------------------------------------------------*/
static void GivePolylineSize (PtrAbstractBox pAb, int zoom, int *width,
                              int *height)
{
  PtrTextBuffer       pBuffer;
  int                 max;

  /* Si le pave est vide on prend une dimension par defaut */
  pBuffer = pAb->AbPolyLineBuffer;
  *width = 1;
  *height = 1;
  max = pAb->AbVolume;
  if (max > 0 || pBuffer != NULL)
    {
      /* La largeur est donnee par le point limite */
      *width =  PixelValue (pBuffer->BuPoints[0].XCoord, UnPixel, NULL, zoom);
      /* La hauteur est donnee par le point limite */
      *height =  PixelValue (pBuffer->BuPoints[0].YCoord, UnPixel, NULL, zoom);
    }

}

#ifdef path_limits
/*----------------------------------------------------------------------
  UpdateLimits
  Update the values min and max so that min <= v <= max
  ----------------------------------------------------------------------*/
static void UpdateLimits(int *min, int *max, int v)
{
  if(min && max)
    {
      if(v < *min)*min = v;
      else if(v > *max)*max = v;
    }
}

/*----------------------------------------------------------------------
  UpdateLimits_QuadraticBezier

  Update the values min and max using f(0), f(1) and the values of f
  at its critical points. f is defined by:

  (%i1) f(t) := a0*(1-t)^2 + 2*a1*(1-t)*t + a2*t^2;
  (%o1) f(t):=a0*(1-t)^2+2*a1*(1-t)*t+a2*t^2
  (%i2) diff(f(t), t);
  (%o2) 2*a2*t-2*a1*t+2*a1*(1-t)-2*a0*(1-t)
  (%i3) partfrac(%, t);
  (%o3) (2*a2-4*a1+2*a0)*t+2*a1-2*a0

  ----------------------------------------------------------------------*/
static void UpdateLimits_QuadraticBezier(int a0, int a1, int a2, 
					int *min, int *max)
{
  int f_t;
  double a,b,t;

  UpdateLimits(min, max, a0);
  UpdateLimits(min, max, a2);

  a = 2*a2-4*a1+2*a0;
  b = 2*a1-2*a0;

  if(a != 0)
    {
      t = -b/a;
      
      if(t >= 0 && t <= 1)
	{
	  f_t = (int)(a0*(1-t)*(1-t)+
		      2*a1*(1-t)*t+
		      a2*t*t);
	  UpdateLimits(min, max, f_t);
	}
    }
}

/*----------------------------------------------------------------------
  UpdateLimits_EvaluateCubicBezier
  ----------------------------------------------------------------------*/
static void UpdateLimits_EvaluateCubicBezier(int a0, int a1, int a2, int a3,
						double t, int *min, int *max)
{
  int f_t;

  if(t >= 0 && t <= 1)
    {
      f_t = (int)(t*t*t*a3+
		  3*(1-t)*t*t*a2+
		  3*(1-t)*(1-t)*t*a1+
		  (1-t)*(1-t)*(1-t)*a0);
      UpdateLimits(min, max, f_t);
    }
}

/*----------------------------------------------------------------------
  UpdateLimits_CubicBezier

  Update the values min and max using f(0), f(1) and the values of f
  at its critical points. f is defined by:

  (%i1) f(t) := t^3*a3+3*(1-t)*t^2*a2+3*(1-t)^2*t*a1+(1-t)^3*a0;
  (%o1) f(t):=t^3*a3+3*(1-t)*t^2*a2+3*(1-t)^2*t*a1+(1-t)^3*a0
  (%i2) diff(f(t), t);
  (%o2) 3*a3*t^2-3*a2*t^2+6*a2*(1-t)*t-6*a1*(1-t)*t+3*a1*(1-t)^2-3*a0*(1-t)^2
  (%i3) partfrac(%, t);
  (%o3) (3*a3-9*a2+9*a1-3*a0)*t^2+(6*a2-12*a1+6*a0)*t+3*a1-3*a0

  ----------------------------------------------------------------------*/
static void UpdateLimits_CubicBezier(int a0, int a1, int a2, int a3,
				      int *min, int *max)
{
  double a,b,c,delta;

  UpdateLimits(min, max, a0);
  UpdateLimits(min, max, a3);

  a = 3*a3-9*a2+9*a1-3*a0;
  b = 6*a2-12*a1+6*a0;
  c = 3*a1-3*a0;

  if(a == 0)
    {
      /* bX + c = 0 ? */

      if(b != 0)
	UpdateLimits_EvaluateCubicBezier(a0, a1, a2, a3,
					  -c/b,
					 min, max);
    }
  else
    {
      /* aX^2 + bX + c = 0 ? */

      delta = b*b - 4*a*c;
      
      if(delta == 0)
	  /* One root */
	UpdateLimits_EvaluateCubicBezier(a0, a1, a2, a3,
					  -b/(2*a),
					 min, max);
      else if(delta > 0)
	{
	  /* Two roots */
	  UpdateLimits_EvaluateCubicBezier(a0, a1, a2, a3,
					    (-b + sqrt(delta))/(2*a),
					   min, max);

	  UpdateLimits_EvaluateCubicBezier(a0, a1, a2, a3,
					    (-b - sqrt(delta))/(2*a),
					   min, max);
	}
    }
}


/*----------------------------------------------------------------------
  UpdateLimits_EllipticalArc
  ----------------------------------------------------------------------*/
static void UpdateLimits_EllipticalArc(PtrPathSeg pPa,
				       int *xmin,
				       int *xmax,
				       int *ymin,
				       int *ymax)
{
#define ALLOC_POINTS    300

  ThotPoint           *points;
  int                  i,maxpoints, npoints;
  double               x1, y1, x2, y2, cx1, cy1;
  x1 = (double) pPa->XStart;
  y1 = (double) pPa->YStart;
  x2 = (double) pPa->XEnd;
  y2 = (double) pPa->YEnd;
  cx1 = (double) pPa->XRadius;
  cy1 = (double) pPa->YRadius;

  /* get a buffer to store the points of the polygon */
  maxpoints = ALLOC_POINTS;
  points = (ThotPoint *) TtaGetMemory (maxpoints * sizeof(ThotPoint));
  memset (points, 0, maxpoints * sizeof(ThotPoint));
  npoints = 0;

  /* Build the polyline that approximates the elliptic arc */
  EllipticSplit ( 0, 0, 0,
		  x1, y1,
		  x2, y2,
		  cx1, cy1,
		  (int)fmod((double)pPa->XAxisRotation, (double)360),
		  pPa->LargeArc, pPa->Sweep,
		  &points, &npoints, &maxpoints);

  /* Update the limit according to each point */
  for(i = 0; i < npoints; i++)
    {
      UpdateLimits(xmin, xmax, (int)points[i].x);
      UpdateLimits(ymin, ymax, (int)points[i].y);
    }

  TtaFreeMemory(points);
}

/*----------------------------------------------------------------------
  GivePathLimits
  ----------------------------------------------------------------------*/
static void GivePathLimits (PtrAbstractBox pAb, int zoom,
			    int *x0, int *y0, int *width, int *height)
{
  PtrPathSeg       pPa;
  int xmin, ymin, xmax, ymax;

  pPa = pAb->AbFirstPathSeg;

  if(!pPa)
    {
      if(x0 && y0)
	{
	  *x0 = 0;
	  *y0 = 0;
	}


      if(width && height)
	{
	  *width = 0;
	  *height = 0;
	}
    }
  else
    {
      /* Take the coordinates of the first point as the initial values for
	 xmin, ymin, xmax, ymax */
      xmin = pPa->XEnd;
      xmax = pPa->XEnd;
      ymin = pPa->YEnd;
      ymax = pPa->YEnd;

      /* Now look each fragment of the path */
      while(pPa)
	{
	  switch(pPa->PaShape)
	    {
	    case PtLine:
	      UpdateLimits(&xmin, &xmax, pPa->XStart);
	      UpdateLimits(&ymin, &ymax, pPa->YStart);
	      UpdateLimits(&xmin, &xmax, pPa->XEnd);
	      UpdateLimits(&ymin, &ymax, pPa->YEnd);
	      break;
	      
	    case PtEllipticalArc:
	      UpdateLimits_EllipticalArc(pPa,
					 &xmin,
					 &xmax,
					 &ymin,
					 &ymax);
	      break;

	    case PtQuadraticBezier:
	      /* Update horizontal limits */
	      UpdateLimits_QuadraticBezier(pPa->XStart,
					    pPa->XCtrlStart,
					    pPa->XEnd,
					    &xmin,
					    &xmax);

	      /* Update vertical limits */
	      UpdateLimits_QuadraticBezier(pPa->YStart,
					    pPa->YCtrlStart,
					    pPa->YEnd,
					    &ymin,
					    &ymax);
	      break;

	    case PtCubicBezier:
	      /* Update horizontal limits */
	      UpdateLimits_CubicBezier(pPa->XStart,
					pPa->XCtrlStart,
					pPa->XCtrlEnd,
					pPa->XEnd,
					&xmin,
					&xmax);

	      /* Update vertical limits */
	      UpdateLimits_CubicBezier(pPa->YStart,
					pPa->YCtrlStart,
					pPa->YCtrlEnd,
					pPa->YEnd,
					&ymin,
					&ymax);
	      break;
	    }
	  pPa = pPa -> PaNext;
	}

      if(x0 && y0)
	{
	  *x0 = PixelValue (xmin, UnPixel, NULL, zoom);
	  *y0 = PixelValue (ymin, UnPixel, NULL, zoom);
	}

      if(width && height)
	{
	  *width = PixelValue (xmax - xmin, UnPixel, NULL, zoom);
	  *height = PixelValue (ymax - ymin, UnPixel, NULL, zoom);
	}
    }
}
#endif /* path_limit */

/*----------------------------------------------------------------------
  FreePolyline frees buffers attached to the polyline box.
  ----------------------------------------------------------------------*/
static void FreePolyline (PtrBox pBox)
{
  PtrTextBuffer       pBuffer;

  if (pBox->BxBuffer != NULL)
    {
      /* Transformation polyline en graphique simple */
      /* il faut liberer les buffers */
      pBox->BxNChars = pBox->BxAbstractBox->AbVolume;
      pBox->BxXRatio = 1;
      pBox->BxYRatio = 1;
      pBuffer = pBox->BxBuffer;
      while (pBuffer != NULL)
        {
          pBox->BxBuffer = pBuffer->BuNext;
          FreeTextBuffer (pBuffer);
          pBuffer = pBox->BxBuffer;
        }
    }

  if (pBox->BxPictInfo != NULL)
    {
      free (pBox->BxPictInfo);
      pBox->BxPictInfo = NULL;
    }
}


/*----------------------------------------------------------------------
  FreePath frees all path segment descriptors attached to the path box.
  ----------------------------------------------------------------------*/
static void FreePath (PtrBox box)
{
  PtrPathSeg pPa, pPaNext;

  if (box->BxFirstPathSeg)
    {
      box->BxNChars = box->BxAbstractBox->AbVolume;
      box->BxXRatio = 1;
      box->BxYRatio = 1;
      pPa = box->BxFirstPathSeg;
      box->BxFirstPathSeg = NULL;
      while (pPa)
        {
          pPaNext = pPa->PaNext;
          FreePathSeg (pPa);
          pPa = pPaNext;
        }
    }
}

/*----------------------------------------------------------------------
  SplitForScript splits a box due to a change of script.
  Parameters:
  * script of the first box.
  * lg = number of characters in the first box.
  * width, height of the first box.
  * spaces = number of spaces in the first box.
  * ind = buffer index of the first character of the last box.
  * pBuffer = buffer that includes the first character of the last box.
  Return the last generated box.
  ----------------------------------------------------------------------*/
PtrBox SplitForScript (PtrBox box, PtrAbstractBox pAb, char script, int lg,
                       int width, int height, int spaces, int ind,
                       PtrTextBuffer pBuffer, int frame)
{
  PtrBox              ibox1, ibox2, pMainBox;
  PtrBox              pPreviousBox, pNextBox;
  int                 l, v;

  pPreviousBox = box->BxPrevious;
  pNextBox = box->BxNext;
  pMainBox = ViewFrameTable[frame - 1].FrAbstractBox->AbBox;
  if (box->BxType == BoComplete)
    {
      l = box->BxLBorder + box->BxLPadding;
      if (box->BxLMargin > 0)
        l += box->BxLMargin;
      v = box->BxTBorder + box->BxTPadding + box->BxBBorder + box->BxBPadding;
      if (box->BxLMargin > 0)
        v += box->BxTMargin;
      if (box->BxBMargin > 0)
        v += box->BxBMargin;
      /* Update the main box */
      box->BxType = BoMulScript;
      ibox1 = GetBox (pAb);
      ibox2 = GetBox (pAb);
      box->BxNexChild = ibox1;

      /* Initialize the first piece */
      ibox1->BxType = BoScript;
      ibox1->BxScript = script;
      ibox1->BxAbstractBox = pAb;
      ibox1->BxIndChar = box->BxIndChar;
      ibox1->BxShrink = FALSE;
      ibox1->BxContentWidth = TRUE;
      ibox1->BxContentHeight = TRUE;
      ibox1->BxFont = box->BxFont;
      ibox1->BxUnderline = box->BxUnderline;
      ibox1->BxThickness = box->BxThickness;
      ibox1->BxHorizRef = box->BxHorizRef;
      ibox1->BxH = BoxFontHeight (box->BxFont, script);
      ibox1->BxHeight = ibox1->BxH + v;
      ibox1->BxW = width;
      ibox1->BxWidth = width + l;
      ibox1->BxTMargin = box->BxTMargin;
      ibox1->BxTBorder = box->BxTBorder;
      ibox1->BxTPadding = box->BxTPadding;
      ibox1->BxBMargin = box->BxBMargin;
      ibox1->BxBBorder = box->BxBBorder;
      ibox1->BxBPadding = box->BxBPadding;
      ibox1->BxLMargin = box->BxLMargin;
      ibox1->BxLBorder = box->BxLBorder;
      ibox1->BxLPadding = box->BxLPadding;
      ibox1->BxBuffer = box->BxBuffer;
      ibox1->BxFirstChar = box->BxFirstChar;
      ibox1->BxNChars = lg;
      ibox1->BxNSpaces = spaces;
      ibox1->BxYOrg = box->BxYOrg;
      /* Initialize the second piece */
      ibox2->BxType = BoScript;
      ibox2->BxScript = box->BxScript;
      ibox2->BxAbstractBox = pAb;
      ibox2->BxIndChar = ind;
      ibox2->BxShrink = FALSE;
      ibox2->BxContentWidth = TRUE;
      ibox2->BxContentHeight = TRUE;
      ibox2->BxFont = box->BxFont;
      ibox2->BxUnderline = box->BxUnderline;
      ibox2->BxThickness = box->BxThickness;
      ibox2->BxHorizRef = box->BxHorizRef;
      ibox2->BxH = height;
      ibox2->BxHeight = height + v;
      ibox2->BxW = box->BxW - width;
      ibox2->BxWidth = box->BxWidth - width;
      ibox2->BxTMargin = box->BxTMargin;
      ibox2->BxTBorder = box->BxTBorder;
      ibox2->BxTPadding = box->BxTPadding;
      ibox2->BxBMargin = box->BxBMargin;
      ibox2->BxBBorder = box->BxBBorder;
      ibox2->BxBPadding = box->BxBPadding;
      ibox2->BxRMargin = box->BxRMargin;
      ibox2->BxRBorder = box->BxRBorder;
      ibox2->BxRPadding = box->BxRPadding;
      ibox2->BxBuffer = pBuffer;
      ibox2->BxFirstChar = ibox1->BxFirstChar + lg;
      ibox2->BxNChars = box->BxNChars - lg;
      ibox2->BxNSpaces = box->BxNSpaces - spaces;
      if (pAb->AbDirection == 'L')
        {
          XMove (ibox1, ibox1, box->BxXOrg - ibox1->BxXOrg, frame);
          XMove (ibox2, ibox2, ibox1->BxXOrg + ibox1->BxWidth - ibox2->BxXOrg, frame);
        }
      else
        {
          XMove (ibox2, ibox2, box->BxXOrg - ibox2->BxXOrg, frame);
          XMove (ibox1, ibox1, ibox2->BxXOrg + ibox2->BxWidth - ibox1->BxXOrg, frame);
        }
      ibox2->BxYOrg = box->BxYOrg;
      /* update the chain of leaf boxes */
      ibox1->BxNexChild = ibox2;
      ibox1->BxPrevious = pPreviousBox;
      if (pPreviousBox != NULL)
        pPreviousBox->BxNext = ibox1;
      else
        pMainBox->BxNext = ibox1;
      ibox1->BxNext = ibox2;
      ibox2->BxNexChild = NULL;
      ibox2->BxPrevious = ibox1;
      ibox2->BxNext = pNextBox;
      if (pNextBox != NULL)
        pNextBox->BxPrevious = ibox2;
      else
        pMainBox->BxPrevious = ibox2;
    }
  else
    {
      ibox1 = pAb->AbBox;
      l = ibox1->BxLBorder + ibox1->BxLPadding;
      if (ibox1->BxLMargin > 0)
        l += ibox1->BxLMargin;
      v = ibox1->BxTBorder + ibox1->BxTPadding + ibox1->BxBBorder + ibox1->BxBPadding;
      if (ibox1->BxLMargin > 0)
        v += ibox1->BxTMargin;
      if (ibox1->BxBMargin > 0)
        v += ibox1->BxBMargin;
      /* Initialize the second piece */
      ibox2 = GetBox (pAb);
      ibox2->BxType = BoScript;
      ibox2->BxScript = box->BxScript;
      ibox2->BxAbstractBox = pAb;
      ibox2->BxIndChar = ind;
      ibox2->BxShrink = FALSE;
      ibox2->BxContentWidth = TRUE;
      ibox2->BxContentHeight = TRUE;
      ibox2->BxFont = box->BxFont;
      ibox2->BxUnderline = box->BxUnderline;
      ibox2->BxThickness = box->BxThickness;
      ibox2->BxHorizRef = box->BxHorizRef;
      ibox2->BxH = height;
      ibox2->BxHeight = height + v;
      if (box->BxW - width > 0)
        ibox2->BxW = box->BxW - width;
      else
        ibox2->BxW = 0;
      if (box->BxWidth - width - l > 0)
        ibox2->BxWidth = box->BxWidth - width - l;
      else
        ibox2->BxWidth = 0;
      ibox2->BxTMargin = box->BxTMargin;
      ibox2->BxTBorder = box->BxTBorder;
      ibox2->BxTPadding = box->BxTPadding;
      ibox2->BxBMargin = box->BxBMargin;
      ibox2->BxBBorder = box->BxBBorder;
      ibox2->BxBPadding = box->BxBPadding;
      ibox2->BxRMargin = box->BxRMargin;
      ibox2->BxRBorder = box->BxRBorder;
      ibox2->BxRPadding = box->BxRPadding;
      ibox2->BxBuffer = pBuffer;
      ibox2->BxFirstChar = box->BxFirstChar + lg;
      ibox2->BxNChars =  box->BxNChars - lg;
      ibox2->BxNSpaces = box->BxNSpaces - spaces;
      ibox2->BxYOrg = box->BxYOrg;
      /* Update the first piece */
      box->BxScript = script; 
      box->BxRMargin = 0;
      box->BxRBorder = 0;
      box->BxRPadding = 0;
      box->BxW = width;
      box->BxWidth = width + l;
      box->BxRMargin = 0;
      box->BxRBorder = 0;
      box->BxRPadding = 0;
      box->BxNChars = lg;
      box->BxNSpaces = spaces;
      if (pAb->AbDirection == 'L')
        XMove (ibox2, ibox2, box->BxXOrg + box->BxWidth - ibox2->BxXOrg, frame);
      else
        {
          XMove (ibox2, ibox2, box->BxXOrg - ibox2->BxXOrg, frame);
          XMove (box, box, ibox2->BxWidth, frame);
        }

      /* update the chain of leaf boxes */
      ibox2->BxNexChild = box->BxNexChild;
      box->BxNexChild = ibox2;
      box->BxNext = ibox2;
      ibox2->BxPrevious = box;
      ibox2->BxNext = pNextBox;
      if (pNextBox)
        pNextBox->BxPrevious = ibox2;
      else
        pMainBox->BxPrevious = ibox2;
    }
  return ibox2;
}

/*----------------------------------------------------------------------
  UnsplitBox removes all child pieces and scripts.
  ----------------------------------------------------------------------*/
static void UnsplitBox (PtrBox pBox, int frame)
{
  ViewFrame          *pFrame;
  PtrBox              box;

  if (pBox->BxType != BoMulScript && pBox->BxType != BoSplit)
    return;
  box = pBox->BxNexChild;
  pBox->BxNexChild = NULL;
  pBox->BxType = BoComplete;
  pFrame = &ViewFrameTable[frame - 1];
  // child boxes will be removed
  if (box && box->BxPrevious != pBox)
    {
      pBox->BxPrevious = box->BxPrevious;
      if (box->BxPrevious)
        box->BxPrevious->BxNext = pBox;
      else
        pFrame->FrAbstractBox->AbBox->BxNext = pBox;
    }
  while (box && (box->BxType == BoScript || box->BxType == BoPiece))
    {
#ifdef _GL
      if (glIsList (box->DisplayList))
        {
          glDeleteLists (box->DisplayList, 1);
          box->DisplayList = 0;
        }
#endif /* _GL */
      if (box->BxNexChild == NULL && box->BxNext != pBox)
        {
          // it's the last child box
          pBox->BxNext = box->BxNext;
          if (box->BxNext)
            box->BxNext->BxPrevious = pBox;
          else
            pFrame->FrAbstractBox->AbBox->BxPrevious = pBox;
        }
      box = FreeBox (box);
    }
}

/*----------------------------------------------------------------------
  GiveTextSize gives the internal width and height of a text box.
  ----------------------------------------------------------------------*/
static void GiveTextSize (PtrAbstractBox pAb, int frame, int *width,
                          int *height, int *nSpaces)
{
  PtrTextBuffer       pBuffer;
  SpecFont            font;
  PtrBox              box;
  char                script, dir;
  int                 ind, nChars;
  int                 l, pos;
  int                 lg, spaces, bwidth;
  char                prevscript = '*';

  box = pAb->AbBox;
  font = box->BxFont;
  *height = BoxFontHeight (font, box->BxScript);
  /* Est-ce que le pave est vide ? */
  nChars = pAb->AbVolume;
  pBuffer = pAb->AbText;
  ind = box->BxIndChar;
  *nSpaces = 0;
  *width = 0;
  pos = 1;
  if (pAb->AbPrevious &&
      (pAb->AbPrevious->AbFloat != 'N' ||
       (pAb->AbPrevious->AbBox && pAb->AbPrevious->AbBox->BxType == BoFloatGhost)) &&
      ind == 0 && pBuffer && pBuffer->BuContent[0] == SPACE)
    {
      /* ignore the space that follows a floated box */
      ind++;
      nChars--;
    }
  else if (nChars == 0)
    *width = 2;

  if (nChars > 0)
    {
      /*
        There is a current text:
        generate one complete box or several boxes if several scripts are used
      */
      /* first character to be handled */
      dir = pAb->AbDirection;
      if (box->BxType == BoMulScript)
        /* remove multi script boxes */
        UnsplitBox (box, frame);
      while (nChars > 0)
        {
          bwidth = 0;
          spaces = 0; /* format with the standard space width */
          lg = nChars;
          if (box->BxPrevious && box->BxPrevious->BxScript != EOS)
            prevscript = box->BxPrevious->BxScript;
          script = GiveTextParams (&pBuffer, &ind, &nChars, font, pAb->AbFontVariant,
                                   &bwidth, &spaces, dir, pAb->AbUnicodeBidi,
                                   &EmbeddedScript, prevscript);
          box->BxScript = script;
          *width += bwidth;
          *nSpaces += spaces;
          lg -= nChars;
          if (nChars > 0)
            box = SplitForScript (box, pAb, script, lg, bwidth, *height, spaces,
                                  ind, pBuffer, frame);
          else if (box->BxType == BoScript)
            {
              box->BxW = bwidth;
              l = box->BxLBorder + box->BxLPadding;
              if (box->BxLMargin > 0)
                l += box->BxLMargin;
              box->BxWidth = bwidth + l;
              box->BxNChars = lg;
              box->BxNSpaces = spaces;
              box->BxScript = script;
              box->BxH = BoxFontHeight (font, script);
              box->BxHeight = box->BxH + box->BxTBorder + box->BxTPadding
                              + box->BxBBorder + box->BxBPadding;
              if (box->BxTMargin > 0)
                box->BxHeight += box->BxTMargin;
              if (box->BxBMargin > 0)
                box->BxHeight += box->BxBMargin;
            }
        }
    }
}


/*----------------------------------------------------------------------
  GiveEnclosureSize gives the size of a compound box.
  ----------------------------------------------------------------------*/
void GiveEnclosureSize (PtrAbstractBox pAb, int frame, int *width,
                        int *height)
{
  PtrAbstractBox      pChildAb, pFirstAb, pCurrentAb;
  PtrBox              pChildBox, box, pBox;
  PtrElement          pEl;
  int                 val, x, y;
  ThotBool            still, hMin, vMin, isExtra;

  box = NULL;
  pBox = pAb->AbBox;
  /* first child that is not dead */
  pFirstAb = pAb->AbFirstEnclosed;
  still = TRUE;
  hMin = !pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum;
  vMin = !pAb->AbHeight.DimIsPosition && pAb->AbHeight.DimMinimum;
  while (still)
    if (pFirstAb == NULL)
      still = FALSE;
    else if (pFirstAb->AbDead)
      pFirstAb = pFirstAb->AbNext;
    else
      still = FALSE;

  if (pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost)
    {
      /* This box doesn't really exist */
      *width = 0;
      *height = 0;
      if (pAb->AbInLine)
        {
          /* the block of line is ignored */
          pBox->BxFirstLine = NULL;
          pBox->BxLastLine = NULL;
        }
    }
  else if (pFirstAb == NULL)
    {
      /* The box is empty */
      *width = 0;
      *height = 0;
    }
  else if (pBox->BxType == BoBlock ||
           pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
    {
      /* It's a block of lines */
      /* we have to reformat the block of lines */
      RecomputeLines (pAb, NULL, NULL, frame);
      *height = pBox->BxH;
      *width = pBox->BxW;
    }
  else
    {
      /* It's a geometrical composition */
      /* Initially the inside left and the inside right are the equal */
      x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      *width = x;
      /* Initially the inside top and the inside bottom are the equal */
      y = pBox->BxYOrg + pBox->BxTMargin +
        pBox->BxTBorder + pBox->BxTPadding;
      *height = y;
      /* the box itself is positioned */
      isExtra = ExtraFlow (pBox, frame);
      /* Move misplaced boxes */
      pChildAb = pFirstAb;
      while (pChildAb)
        {
          pChildBox = pChildAb->AbBox;
          if (!pChildAb->AbDead && pChildBox &&
              pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
              (FrameTable[frame].FrView != 1 ||
               !TypeHasException (ExcIsMap, pChildAb->AbElement->ElTypeNumber,
                                  pChildAb->AbElement->ElStructSchema)) &&
              (isExtra || !ExtraFlow (pChildBox, frame)))
            {
              if ((hMin || pBox->BxContentWidth) &&
                  pChildAb->AbHorizEnclosing &&
                  pChildBox->BxXOrg < x &&
                  pChildAb->AbWidth.DimAbRef != pAb)
                {
                  /* the child box is misplaced */
                  box = GetHPosRelativeBox (pChildBox, NULL);
                  if (box != NULL)
                    {
                      /* mobile box */
                      pCurrentAb = box->BxAbstractBox;
                      if (pCurrentAb->AbHorizPos.PosAbRef == NULL)
                        XMove (pChildBox, NULL, x - pChildBox->BxXOrg, frame);
                    }
                }
              if ((vMin || pBox->BxContentHeight) &&
                  pChildAb->AbVertEnclosing &&
                  pChildBox->BxYOrg < y &&
                  pChildAb->AbHeight.DimAbRef != pAb)
                {
                  /* the child box is misplaced */
                  box = GetVPosRelativeBox (pChildBox, NULL);
                  if (box != NULL)
                    {
                      /* mobile box */
                      pCurrentAb = box->BxAbstractBox;
                      if (pCurrentAb->AbVertPos.PosAbRef == NULL)
                        YMove (pChildBox, NULL, y - pChildBox->BxYOrg, frame);
                    }
                }
            }
          pChildAb = pChildAb->AbNext;
        }

      /*
       * The left edge of the extra left enclosed box must be stuck
       * to the inside left edge and the inside width is delimited
       * by the right edge of the extra right enclosed box.
       * The top edge of the extra upper enclosed box must be stuck
       * to the inside top edge and the inside height is delimited
       * by the bottom edge of the extra lower enclosed box.
       */
      pChildAb = pFirstAb;
      while (pChildAb)
        {
          pChildBox = pChildAb->AbBox;
          if (!pChildAb->AbDead && pChildBox &&
              pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
              (FrameTable[frame].FrView != 1 ||
               !TypeHasException (ExcIsMap, pChildAb->AbElement->ElTypeNumber,
                                  pChildAb->AbElement->ElStructSchema)) &&
              (isExtra || !ExtraFlow (pChildBox, frame)))
            {
              if (pChildAb->AbHorizEnclosing &&
                  (pChildAb->AbWidth.DimAbRef != pAb ||
                   pChildBox->BxContentWidth ||
                   /* Sometimes table width doesn't follow the rule */
                   pChildBox->BxType == BoTable))
                {
                  /* the width of that box doesn't depend on the enclosing */
                  if (pChildBox->BxXOrg < x ||
                      (pChildAb->AbHorizPos.PosDistance < 0 &&
                       pChildAb->AbHorizPos.PosRefEdge == Left))
                    val = x + pChildBox->BxWidth;
                  else
                    val = pChildBox->BxXOrg + pChildBox->BxWidth;
                  if (val > *width)
                    *width = val;
                }
              else if ((pChildBox->BxType == BoBlock ||
                        pChildBox->BxType == BoFloatBlock ||
                        pChildBox->BxType == BoCellBlock) &&
                       !ExtraFlow (pChildBox, frame) &&
                      *width < pChildBox->BxXOrg + pChildBox->BxMinWidth)
                    *width = pChildBox->BxXOrg + pChildBox->BxMinWidth;
                
              if (pChildAb->AbVertEnclosing &&
                  (pChildAb->AbHeight.DimAbRef != pAb ||
                   pChildBox->BxContentHeight))
                {
                  /* the height of that box doesn't depend on the enclosing */
                  if (pChildBox->BxYOrg < y)
                    val = y + pChildBox->BxHeight;
                  else
                    val = pChildBox->BxYOrg + pChildBox->BxHeight;
                  if (val > *height)
                    *height = val;
                }
            }
          pChildAb = pChildAb->AbNext;
        }
      
      /* New width and height */
      *width -= x;
      *height -= y;

      /*
       * Now move included boxes which depend on the parent width and height
       */
      pChildAb = pFirstAb;
      if (Propagate == ToSiblings)
        while (pChildAb != NULL)
          {
            pChildBox = pChildAb->AbBox;
            if (!pChildAb->AbDead && pChildBox &&
                pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
                (FrameTable[frame].FrView != 1 ||
                 !TypeHasException (ExcIsMap, pChildAb->AbElement->ElTypeNumber,
                                    pChildAb->AbElement->ElStructSchema)))
              {
                if (pBox->BxContentWidth &&
                    !IsXPosComplete (pBox) && ! pChildBox->BxXOutOfStruct &&
                    pChildAb->AbHorizPos.PosAbRef == pAb)
                  {
                    /* update the box position */
                    val = pChildBox->BxXOrg;
                    if (pChildAb->AbHorizPos.PosEdge == VertMiddle)
                      val += pChildBox->BxWidth / 2;
                    else if (pChildAb->AbHorizPos.PosEdge == Right)
                      val += pChildBox->BxWidth;
                    pBox->BxMoved = NULL;
                    if (pChildAb->AbHorizPos.PosRefEdge == VertMiddle)
                      {
                        if (!pChildBox->BxHorizFlex)
                          XMove (pChildBox, pBox, x + *width / 2 - val, frame);
                      }
                    else if (pChildAb->AbHorizPos.PosRefEdge == Right)
                      {
                        if (!pChildBox->BxHorizFlex)
                          XMove (pChildBox, pBox, x + *width - val, frame);
                      }
                  }
		
                if (pBox->BxContentHeight &&
                    !IsYPosComplete (pBox) && ! pChildBox->BxYOutOfStruct &&
                    pChildAb->AbVertPos.PosAbRef == pAb)
                  {
                    /* update the box position */
                    val = pChildBox->BxYOrg;
                    pBox->BxMoved = NULL;
                    if (pChildAb->AbVertPos.PosEdge == HorizMiddle)
                      val += pChildBox->BxHeight / 2;
                    else if (pChildAb->AbVertPos.PosEdge == Bottom)
                      val += pChildBox->BxHeight;
		    
                    if (pChildAb->AbVertPos.PosRefEdge == HorizMiddle)
                      {
                        if (!pChildBox->BxVertFlex)
                          YMove (pChildBox, pBox, y + *height / 2 - val,frame);
                      }
                    else if (pChildAb->AbVertPos.PosRefEdge == Bottom)
                      {
                        if (!pChildBox->BxVertFlex)
                          YMove (pChildBox, pBox, y + *height - val, frame);
                      }
                  }
              }
            pChildAb = pChildAb->AbNext;
          }
    }
  
  // check if the box is visible
  if (!pAb->AbDead &&
      pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
    {
      /* Empty box */
      if ((pAb->AbWidth.DimAbRef != NULL || pAb->AbWidth.DimValue != 0 ||
           pAb->AbWidth.DimUnit == UnAuto) && *width == 0)
        *width = 2;
      pEl = pAb->AbElement;
      if (pEl &&
          !TypeHasException (ExcNoSelect, pEl->ElTypeNumber, pEl->ElStructSchema) &&
          (pAb->AbHeight.DimAbRef != NULL || pAb->AbHeight.DimValue != 0 ||
           pAb->AbHeight.DimUnit == UnAuto) && *height == 0)
        {
          if (pAb->AbLeafType != LtText && pAb->AbLeafType != LtSymbol)
            *height = GetCurrentFontHeight (pAb->AbSize, pAb->AbSizeUnit, frame);
          else
            *height = BoxFontHeight (pAb->AbBox->BxFont, EOS);
        }
    }
}

/*----------------------------------------------------------------------
  IsAbstractBoxEmpty returns TRUE is the abstract box is empty.
  ----------------------------------------------------------------------*/
static ThotBool IsAbstractBoxEmpty (PtrAbstractBox pAb)
{
  PtrAbstractBox      pChildAb;
  ThotBool            complete;

  pChildAb = pAb->AbFirstEnclosed;
  complete = (pChildAb == NULL);
  while (!complete)
    if (pChildAb->AbDead)
      {
        pChildAb = pChildAb->AbNext;
        complete = pChildAb == NULL;
      }
    else
      complete = TRUE;

  return (pChildAb == NULL);
}

/*----------------------------------------------------------------------
  PreviousLeafAbstractBox recherche le dernier pave terminal vivant 
  avant le pave designe'.                                 
  ----------------------------------------------------------------------*/
static PtrAbstractBox PreviousLeafAbstractBox (PtrAbstractBox pAb)
{
  PtrAbstractBox      pPreviousAb;
  PtrAbstractBox      pChildAb;
  PtrAbstractBox      result;
  ThotBool            still;

  pPreviousAb = pAb->AbPrevious;
  pChildAb = NULL;
  still = TRUE;

  /* On recherche en arriere un pave vivant */
  while (still)
    if (pPreviousAb == NULL)
      still = FALSE;
    else if (pPreviousAb->AbBox == NULL)
      pPreviousAb = pPreviousAb->AbPrevious;
    else if (pPreviousAb->AbFirstEnclosed == NULL)
      still = FALSE;
    else
      {
        /* On descend la hierarchie pour prendre son dernier fils */
        pChildAb = pPreviousAb->AbFirstEnclosed;
        while (pChildAb != NULL && pChildAb != pAb)
          {
            pPreviousAb = pChildAb;
            pChildAb = pChildAb->AbNext;

            /* On parcours la liste des fils */
            while (pChildAb != NULL && pChildAb != pAb)
              {
                pPreviousAb = pChildAb;
                pChildAb = pChildAb->AbNext;
              }

            /* On descend d'un niveau */
            if (pPreviousAb->AbBox != NULL)
              pChildAb = pPreviousAb->AbFirstEnclosed;
          }

        pChildAb = pPreviousAb->AbEnclosing;
        if (pPreviousAb->AbBox != NULL)
          still = FALSE;	/* On a trouve */
        else
          {
            /* Si ce dernier fils est mort -> on prend le dernier */
            /* pave vivant avant lui, ou a defaut l'englobant     */
            while (still)
              if (pPreviousAb->AbPrevious == NULL)
                {
                  still = FALSE;
                  pPreviousAb = NULL;	/* C'est l'englobant */
                }
              else if (pPreviousAb->AbPrevious->AbBox != NULL)
                still = FALSE;
              else
                pPreviousAb = pPreviousAb->AbPrevious;

            if (pPreviousAb != NULL)
              pPreviousAb = PreviousLeafAbstractBox (pPreviousAb);
          }
      }

  /* Resultat de la recherche */
  if (pPreviousAb != NULL)
    result = pPreviousAb;
  else if (pChildAb != NULL)
    result = pChildAb;
  else
    {
      /* On n'a pas trouve de precedent -> On remonte dans la hierarchie */
      pChildAb = pAb->AbEnclosing;
      if (pChildAb != NULL)
        result = PreviousLeafAbstractBox (pChildAb);
      else
        result = NULL;
    }
  return result;
}


/*----------------------------------------------------------------------
  FontStyleAndWeight
  returns the Highlight value for abstract box pAb, according to its
  FontStyle and FontWeight.
  ----------------------------------------------------------------------*/
static int FontStyleAndWeight (PtrAbstractBox pAb)
{
  int	i;

  i = 0;
  if (pAb)
    {
      if (pAb->AbFontWeight == 1)	/* Weight: Bold */
        {
          if (pAb->AbFontStyle == 1)		/* Style: Italic */
            i = 4;
          else if (pAb->AbFontStyle == 2)	/* Style: Oblique */
            i = 5;
          else					/* Style: Roman (default) */
            i = 1;
        }
      else				/* Weight: Normal */
        {
          if (pAb->AbFontStyle == 1)		/* Style: Italic */
            i = 2;
          else if (pAb->AbFontStyle == 2)	/* Style: Oblique */
            i = 3;
          else					/* Style: Roman (default) */
            i = 0;	 
        }
    }
  return i;
}


/*----------------------------------------------------------------------
  MarkDisplayedBox marks box that should be displayed
  ----------------------------------------------------------------------*/
static void  MarkDisplayedBox (PtrBox pBox)
{
  PtrAbstractBox  pAb;

  pAb = pBox->BxAbstractBox;
  if (pBox->BxType == BoCell && pAb->AbPictBackground)
    /* register the box */
    pBox->BxDisplay = TRUE;    
  else if (pBox->BxType != BoCell &&
           ((pAb->AbLeafType == LtCompound && pAb->AbFillBox) ||
            (pAb->AbTopStyle > 2 && pAb->AbTopBColor != -2 &&
             pAb->AbTopBorder > 0) ||
            (pAb->AbLeftStyle > 2 && pAb->AbLeftBColor != -2 &&
             pAb->AbLeftBorder > 0) ||
            (pAb->AbBottomStyle > 2 && pAb->AbBottomBColor != -2 &&
             pAb->AbBottomBorder > 0) ||
            (pAb->AbRightStyle > 2 && pAb->AbRightBColor != -2 &&
             pAb->AbRightBorder > 0)))
    /* register the box */
    pBox->BxDisplay = TRUE;
  else
    pBox->BxDisplay = FALSE;
}


/*----------------------------------------------------------------------
  TransmitMBP transmits margins, borders, and paddings of pRefBox to
  children.
  i = the added pixels at the beginning
  j = the added pixels at the end
  inLineFloat is TRUE when the inline is generated by a float element.
  first and last are TRUE when applying to the first and/or the last child.
  ----------------------------------------------------------------------*/
static void TransmitMBP (PtrBox pBox, PtrBox pRefBox, int frame,
                         int i, int j,
                         ThotBool horizontal, ThotBool inLineFloat,
                         ThotBool first, ThotBool last)
{
  PtrAbstractBox  pAb, pChild, pRefAb, pNext;

  if (pBox == NULL || pRefBox == NULL ||
      ((i == 0 || !first) && (j == 0 || !last)))
    return;
  pAb = pBox->BxAbstractBox;
  pRefAb = pRefBox->BxAbstractBox;
  if ((pBox->BxType == BoGhost &&
       pAb->AbDisplay != 'B' &&
       ( pAb->AbDisplay != 'U' || !pAb->AbInLine)) ||
      pBox->BxType == BoFloatGhost)
    {
      /* when it's a dummy box report changes to the children */
      pChild = pAb->AbFirstEnclosed;
      while (pChild)
        { 
          /* skip presentation boxes */
          pNext = pChild->AbNext;
          while (pNext && pNext->AbPresentationBox)
            pNext = pNext->AbNext;
          if (pChild->AbBox && !pChild->AbPresentationBox)
            {
              if ((horizontal && inLineFloat) ||
                  (!horizontal && !inLineFloat))
                TransmitMBP (pChild->AbBox, pRefBox, frame, i, j,
                             horizontal, inLineFloat, TRUE, TRUE);
              else if (first || (last && pChild->AbNext == NULL))
                {
                  /* the first and/or last element */
                  TransmitMBP (pChild->AbBox, pRefBox, frame, i, j,
                               horizontal, inLineFloat, first,
                               (last && pNext == NULL));
                }
              first = FALSE;
            }
          pChild = pNext;
        }
    }
  else if (!pAb->AbPresentationBox)
    {
      if (!inLineFloat)
        {
          /* transmit i to the first and j to the last */
          if (!first)
            i = 0;
          if (!last)
            j = 0;
        }
      if (horizontal && (i || j))
        {
          /* add left and rigth */
          if (i || j)
            {
              if (pAb->AbWidth.DimIsPosition || pAb->AbWidth.DimAbRef)
                /* the outside width is constrained */
                ResizeWidth (pBox, pBox, NULL, -i-j, i, j, 0, frame, FALSE);
              else
                /* the inside width is constrained */
                ResizeWidth (pBox, pBox, NULL, 0, i, j, 0, frame, FALSE);
            }
        }
      else if (!horizontal && (i || j))
        {
          /* add left and rigth */
          if (i)
            /* update the box base line before reformatting the block */
            pBox->BxHorizRef += i;
          if (i || j)
            {
              if (pAb->AbHeight.DimIsPosition || pAb->AbHeight.DimAbRef)
                /* the outside height is constrained */
                ResizeHeight (pBox, pBox, NULL, -i-j, i, j, frame);
              else
                /* the inside height is constrained */
                ResizeHeight (pBox, pBox, NULL, 0, i, j, frame);
            }
        }
    }
}

/*----------------------------------------------------------------------
  CheckMBP checks margins, borders and paddings of the current box.
  The parameter evalAuto says if auto values are computed or not.
  pAb and pBox point to the abstract box and the checked box.
  Return TRUE when any value was updated.
  ----------------------------------------------------------------------*/
ThotBool CheckMBP (PtrAbstractBox pAb, PtrBox pBox, int frame, ThotBool evalAuto)
{
  PtrAbstractBox      pParent;
  int                 lt, rb, delta;
  ThotBool            inLine, inLineFloat;

  /* update vertical margins, borders and paddings */
  lt = pBox->BxTMargin + pBox->BxTPadding + pBox->BxTBorder;
  rb = pBox->BxBMargin + pBox->BxBPadding + pBox->BxBBorder;
  ComputeMBP (pAb, frame, FALSE, evalAuto);
  lt = - lt + pBox->BxTMargin + pBox->BxTPadding + pBox->BxTBorder;
  rb = - rb + pBox->BxBMargin + pBox->BxBPadding + pBox->BxBBorder;
  /* Check if the changes affect the inside or the outside width */
  pAb->AbBox->BxHorizRef += lt;
  inLine = pAb->AbBox->BxType == BoGhost && pAb->AbDisplay != 'B';
  inLineFloat = pAb->AbBox->BxType == BoGhost && pAb->AbDisplay == 'B';
  if (inLine && !inLineFloat)
    {
      /* check the block type */
      pParent = pAb->AbEnclosing;
      while (pParent->AbEnclosing && pParent->AbBox && pParent->AbBox->BxType == BoGhost)
        pParent = pParent->AbEnclosing;
      inLineFloat = (pParent->AbBox->BxType == BoFloatBlock ||
                     pParent->AbBox->BxType == BoCellBlock);
    }

  if (inLine)
    TransmitMBP (pBox, pBox, frame, lt, rb, FALSE, inLineFloat, TRUE, TRUE);
  else if (lt != 0 || rb != 0)
    {
      if (pAb->AbHeight.DimIsPosition || pAb->AbHeight.DimAbRef)
        {
          /* the outside height is constrained */
          delta = -lt -rb;
          if (pBox->BxTMargin < 0)
            delta -= pBox->BxTMargin;
          if (pBox->BxBMargin < 0)
            delta -= pBox->BxBMargin;
          ResizeHeight (pBox, pBox, NULL, delta, lt, rb, frame);
        }
      else
        /* the inside height is constrained */
        ResizeHeight (pBox, pBox, NULL, 0, lt, rb, frame);
    }
  
  /* update horizontal margins, borders and paddings */
  lt = pBox->BxLMargin + pBox->BxLPadding + pBox->BxLBorder;
  rb = pBox->BxRMargin + pBox->BxRPadding + pBox->BxRBorder;
  ComputeMBP (pAb, frame, TRUE, evalAuto);
  lt = - lt + pBox->BxLMargin + pBox->BxLPadding + pBox->BxLBorder;
  rb = - rb + pBox->BxRMargin + pBox->BxRPadding + pBox->BxRBorder;
  /* Check if the changes affect the inside or the outside width */
  if (inLine)
    TransmitMBP (pBox, pBox, frame, lt, rb, TRUE, inLineFloat, TRUE, TRUE);
  else if (lt != 0 || rb != 0)
    {
      pAb->AbBox->BxVertRef += lt;
      /* not already updated by the table formatter */
      if (pAb->AbWidth.DimIsPosition || pAb->AbWidth.DimAbRef ||
          (pAb->AbWidth.DimValue == 100 && pAb->AbWidth.DimUnit == UnPercent))
        {
          /* the outside width is constrained */
          delta = -lt -rb;
          if (pBox->BxLMargin < 0)
            delta -= pBox->BxLMargin;
          if (pBox->BxRMargin < 0)
            delta -= pBox->BxRMargin;
          ResizeWidth (pBox, pBox, NULL, delta, lt, rb, 0, frame, FALSE);
        }
      else
        /* the inside width is constrained */
        ResizeWidth (pBox, pBox, NULL, 0, lt, rb, 0, frame, FALSE);
      return TRUE;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  InlineTextChildren returns TRUE if the block element encloses
  a simple text which must be set in lines.
  ----------------------------------------------------------------------*/
ThotBool InlineTextChildren (PtrAbstractBox pAb, int frame)
{
  PtrAbstractBox      pChildAb;

  if (pAb && !pAb->AbDead)
    {
      if (pAb->AbInLine)
        return TRUE;
      if (FrameTable[frame].FrView == 1 && /* only in formatted view */
          pAb->AbBox && pAb->AbBox->BxType != BoGhost &&
          pAb->AbDisplay != 'I' && pAb->AbDisplay != 'b' &&
          !pAb->AbWidth.DimIsPosition &&
          (pAb->AbWidth.DimAbRef || pAb->AbWidth.DimValue != -1))
        {
          /* check all enclosed boxes */
          pChildAb = pAb->AbFirstEnclosed;
          /* stop as soon as we know the result */
          while (pChildAb)
            {
              if (pChildAb->AbLeafType == LtText &&
                  !pChildAb->AbPresentationBox)
                return TRUE;
              else if (pChildAb->AbDisplay == 'I' || pChildAb->AbDisplay == 'b')
                return TRUE;
              pChildAb = pChildAb->AbNext;
            }
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  HasFloatingChild returns TRUE if a child is a floating box.
  directParent returns TRUE if a child is floating.
  uniqueChild returns TRUE if there is only one child.
  dummyChild returns TRUE if the box includes a unique dummy child
  (a pseudo paragraph).
  ----------------------------------------------------------------------*/
static ThotBool HasFloatingChild (PtrAbstractBox pAb, int frame,
                                  ThotBool *directParent, ThotBool *uniqueChild,
                                  ThotBool *dummyChild)
{
  PtrSSchema          pSS;
  PtrAbstractBox      pChildAb;
  int                 nb;
  ThotBool            found, unique, leaf;

  found = FALSE;
  *directParent = FALSE;
  *dummyChild = FALSE;
  nb = 0;
  if (pAb && !pAb->AbDead)
    {
      pSS = pAb->AbElement->ElStructSchema;
      if (TypeHasException (ExcIsColHead, pAb->AbElement->ElTypeNumber, pSS) ||
          TypeHasException (ExcIsTable, pAb->AbElement->ElTypeNumber, pSS) ||
          TypeHasException (ExcIsRow, pAb->AbElement->ElTypeNumber, pSS) ||
          TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber, pSS) ||
          TypeHasException (ExcNewRoot, pAb->AbElement->ElTypeNumber, pSS))
        return found;
      /* check all enclosed boxes */
      pChildAb = pAb->AbFirstEnclosed;
      /* stop as soon as we know the result */
      while (pChildAb)
        {
          if (pChildAb->AbLeafType == LtCompound &&
              pChildAb->AbFloat == 'N' &&
              TypeHasException (ExcHidden, pChildAb->AbElement->ElTypeNumber,
                                pChildAb->AbElement->ElStructSchema))
            *dummyChild = TRUE;
          if (pChildAb->AbFloat != 'N' &&
              (pChildAb->AbLeafType == LtPicture ||
               (pChildAb->AbLeafType == LtCompound &&
                !pChildAb->AbWidth.DimIsPosition &&
                pChildAb->AbWidth.DimAbRef == NULL)))
            {
              /* it's a floating child */
              found = TRUE;
              *directParent = TRUE;
              nb ++;
            }
          else if (pChildAb->AbBox &&
                   pChildAb->AbBox->BxType == BoFloatGhost)
            /* it's a floating child */
            found = TRUE;
          else if (!pChildAb->AbPresentationBox)
            {
              if (!found)
                {
                  /* check if this element includes only one child and this
                     child is a floated box or a ghost generated by a float */
                  found = HasFloatingChild (pChildAb, frame, directParent, &unique, &leaf);
                  //if (!unique && !*directParent)
                  if (found && !unique)
                    /* the child should be a floatblock */
                    found = FALSE;
                  *directParent = FALSE;
                }
              if (!TypeHasException (ExcNotAnElementNode, pChildAb->AbElement->ElTypeNumber,
                                     pChildAb->AbElement->ElStructSchema))
                {
                  // Comments and PIs should be ignored
                  if (!found ||
                      pChildAb->AbElement->ElTypeNumber != 1 ||
                      pChildAb->AbElement->ElVolume > 1 ||
                      pChildAb->AbElement->ElText == NULL ||
                      pChildAb->AbElement->ElText->BuContent[0] != SPACE)
                    // Spaces are ignored too
                    nb ++;
                }
            }
          pChildAb = pChildAb->AbNext;
        }
    }
  if (nb < 2 && !ExtraAbFlow (pAb, frame))
    /* there is only one child */
    *uniqueChild = TRUE;
  else
    {
      *uniqueChild = FALSE;
      *dummyChild = FALSE;
    }
  return found;
}

/*----------------------------------------------------------------------
  AddFlow registers the new flow.
  ----------------------------------------------------------------------*/
static void AddFlow (PtrAbstractBox pAb, int frame)
{
  ViewFrame          *pFrame;
  PtrFlow             pFlow, prev = NULL;
  Positioning        *pos;
  int                 w, h, zoom;

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox && pAb && pAb->AbLeafType == LtCompound &&
      pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
      pAb->AbPositioning && pAb->AbBox)
    {

      // zoom apply to SVG only
      //if (pAb->AbElement && pAb->AbElement->ElStructSchema &&
      //    pAb->AbElement->ElStructSchema->SsName &&
      //    !strcmp (pAb->AbElement->ElStructSchema->SsName, "SVG"))
      zoom = ViewFrameTable[frame - 1].FrMagnification;
      //else
      //  zoom = 0;

      /* check if the flow is already registered */
      pFlow = pFrame->FrFlow;
      while (pFlow)
        {
          if (pFlow->FlRootBox == pAb)
            /* already registered */
            return;
          else if (pFlow->FlRootBox &&
                   IsParentBox (pAb->AbBox, pFlow->FlRootBox->AbBox))
            /* register before its child */
            break;
          prev = pFlow;
          pFlow = pFlow->FlNext;
        }

      /* create a new flow */
      pFlow = (PtrFlow) TtaGetMemory (sizeof (Flow));
      if (prev)
        {
          pFlow->FlNext = prev->FlNext;
          prev->FlNext = pFlow;
        }
      else
        {
          pFlow->FlNext = pFrame->FrFlow;
          pFrame->FrFlow = pFlow;
        }
      if (pFlow->FlNext)
        pFlow->FlNext->FlPrevious = pFlow;
      pFlow->FlPrevious = prev;
      pFlow->FlRootBox = pAb;
      pFlow->FlType = pAb->AbPositioning->PnAlgorithm;
      pFlow->FlXStart = pFlow->FlYStart = 0;
      if (pFlow->FlType == PnRelative)
        {
          /* store the current shift */
          pos = pAb->AbPositioning;
          if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
            {
              w = pAb->AbEnclosing->AbBox->BxW;
              h = pAb->AbEnclosing->AbBox->BxH;
            }
          else
            w = h = 0;
	  
          if (pos->PnLeftUnit != UnUndefined && pos->PnLeftUnit != UnAuto)
            pFlow->FlXStart = GetPixelValue (pos->PnLeftDistance, pos->PnLeftUnit,
                                             w, pAb, zoom);
          else if (pos->PnRightUnit != UnUndefined && pos->PnRightUnit != UnAuto)
            pFlow->FlXStart = -GetPixelValue (pos->PnRightDistance, pos->PnRightUnit,
                                              w, pAb, zoom);
          if (pos->PnTopUnit != UnUndefined && pos->PnTopUnit != UnAuto)
            pFlow->FlYStart = GetPixelValue (pos->PnTopDistance, pos->PnTopUnit,
                                             h, pAb, zoom);
          else if (pos->PnBottomUnit != UnUndefined && pos->PnBottomUnit != UnAuto)
            pFlow->FlYStart = -GetPixelValue (pos->PnBottomDistance, pos->PnBottomUnit,
                                              h, pAb, zoom);
        }
#ifdef POSITIONING
      printf ("Adding flow x=%d y=%d\n",pFlow->FlXStart,pFlow->FlYStart);
#endif /* POSITIONING */
    }
}

/*----------------------------------------------------------------------
  RemoveFlow removes a flow.
  ----------------------------------------------------------------------*/
static void RemoveFlow (PtrAbstractBox pAb, int frame)
{
  ViewFrame          *pFrame;
  PtrFlow             pFlow, prev = NULL;

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox && pAb && pAb->AbLeafType == LtCompound &&
      pAb->AbPositioning)
    {
      /* check if the flow is already registered */
      pFlow = pFrame->FrFlow;
      while (pFlow)
        {
          if (pFlow->FlRootBox == pAb)
            {
#ifdef POSITIONING
              printf ("Removing flow x=%d y=%d\n",pFlow->FlXStart,pFlow->FlYStart);
#endif /* POSITIONING */
              /* remove the current flow */  
              if (prev)
                prev->FlNext = pFlow->FlNext;
              else
                pFrame->FrFlow = pFlow->FlNext;
              if (pFlow->FlNext)
                pFlow->FlNext->FlPrevious = prev;
              TtaFreeMemory (pFlow);
              return;
            }
          else
            {
              prev = pFlow;
              pFlow = pFlow->FlNext;
            }
        }
    }
}

/*----------------------------------------------------------------------
  AddFloatingBox registers the floating box in the parent block.
  ----------------------------------------------------------------------*/
static void AddFloatingBox (PtrAbstractBox pAb, int frame, ThotBool left)
{
  PtrBox              pBox, box;
  PtrAbstractBox      pParent, pBlock;
  PtrFloat            previous, new_;

  if (pAb && !pAb->AbDead)
    {
      box = pAb->AbBox;
      if (pAb->AbLeafType == LtCompound &&
          (pAb->AbWidth.DimIsPosition || ExtraFlow (box, frame) ||
           pAb->AbWidth.DimAbRef))
        /* cannot be a floated box */
        pAb->AbFloat = 'N';
      else if (box)
        {
          pParent = pAb->AbEnclosing;
          if (pParent->AbBox->BxType == BoCell)
            pParent->AbBox->BxType = BoCellBlock;
          while (pParent && pParent->AbBox &&
                 (pParent->AbBox->BxType == BoGhost ||
                  pParent->AbBox->BxType == BoStructGhost ||
                  pParent->AbBox->BxType == BoFloatGhost))
            pParent = pParent->AbEnclosing;
          while (pParent && pParent->AbBox &&
                 (pParent->AbBox->BxType != BoFloatBlock &&
                  pParent->AbBox->BxType != BoCellBlock &&
                  pParent->AbBox->BxType != BoBlock))
            {
              if (pParent->AbBox->BxType == BoCell)
                {
                  /* cannot be a floated box */
                  pAb->AbFloat = 'N';
                  return;
                }
              else
                pParent = pParent->AbEnclosing;
            }
          pBlock = pParent;
          pParent = pAb->AbEnclosing;
          if (pBlock == NULL)
            {
              /* cannot be a floated box */
              pAb->AbFloat = 'N';
              return;
            }
          while (pParent != pBlock && !ExtraAbFlow (pParent, frame))
            {
              if (pParent->AbBox->BxType == BoGhost ||
                  pParent->AbBox->BxType == BoFloatGhost ||
                  pParent->AbBox->BxType == BoStructGhost)
                pParent = pBlock;
              else
                {
                  pParent->AbBox->BxType = BoFloatGhost;
                  pParent = pParent->AbEnclosing;
                }
            }

          /* the x position is out of the structure */
          box->BxXOutOfStruct = TRUE;
          /* get a new float block */
          pBox = pParent->AbBox;
          new_ = (PtrFloat) TtaGetMemory (sizeof (BFloat));
          new_->FlBox = box;
          new_->FlNext = NULL;
          previous = NULL;
          if (left)
            {
              /* left side fixed */
              box->BxHorizEdge = Left;
              if (pBox->BxLeftFloat == NULL)
                {
                  new_->FlPrevious = NULL;
                  pBox->BxLeftFloat = new_;
                }
              else
                previous = pBox->BxLeftFloat;
            }
          else
            {
              /* right side fixed */
              box->BxHorizEdge = Right;
              if (pBox->BxRightFloat == NULL)
                {
                  new_->FlPrevious = NULL;
                  pBox->BxRightFloat = new_;
                }
              else
                previous = pBox->BxRightFloat;
            }
          if (previous)
            {
              while (previous->FlNext)
                previous = previous->FlNext;
              previous->FlNext = new_;
              new_->FlPrevious = previous;
            }
        }
    }
}

/*----------------------------------------------------------------------
  CheckGhost checks if the current box is set ghost
  ----------------------------------------------------------------------*/
static void CheckGhost (PtrAbstractBox pAb, int frame, ThotBool inLine,
                        ThotBool inLineFloat,
                        ThotBool *inlineChildren, ThotBool *inlineFloatC)
{
  PtrSSchema          pSS;
  PtrBox              pBox;
  ThotBool            uniqueChild, dummyChild, directParent;
  ThotBool            extraflow, isroot, displayI, cansplit;

  pSS = pAb->AbElement->ElStructSchema;
  isroot = TypeHasException (ExcNewRoot, pAb->AbElement->ElTypeNumber, pSS);
  if (isroot)
    {
      /* that element cannot become a ghost */
      *inlineChildren = FALSE;
      *inlineFloatC = FALSE;
      directParent = FALSE;
      uniqueChild = FALSE;
      dummyChild = FALSE;
      return;
    }
  else
    {
      *inlineFloatC = HasFloatingChild (pAb, frame, &directParent,
                                        &uniqueChild, &dummyChild);
      *inlineChildren = InlineTextChildren (pAb, frame);
    }
  pBox = pAb->AbBox;
  extraflow = IsFlow (pBox, frame);
  displayI = ((pAb->AbDisplay == 'I' || pAb->AbDisplay == 'b' ||
	       /* a sized box cannot be split by lines */
	       (!pAb->AbHeight.DimIsPosition && pAb->AbHeight.DimValue <= 0)) &&
	      !pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimValue <= 0);
  cansplit = (// a floated box cannot be split by lines
              pAb->AbFloat == 'N' &&
              /* a positioned box cannot be split by lines */
              !extraflow &&
              // not a block with borders or background
              (pAb->AbDisplay != 'B' ||
               (pAb->AbPictBackground == NULL &&
                pAb->AbFillPattern == 0 &&
                pAb->AbLeftBorder == 0 && pAb->AbRightBorder == 0 &&
                pAb->AbTopBorder == 0 && pAb->AbBottomBorder == 0)));

  // a template element within a block accept line break
  if (inLine && pAb->AbFloat == 'N' && pSS && !strcmp (pSS->SsName,"Template") &&
      !pAb->AbAcceptLineBreak) 
    pAb->AbAcceptLineBreak = TRUE;

  if ((inLine || inLineFloat) &&
      *inlineFloatC && uniqueChild && pAb->AbLeftMargin == 0 && pAb->AbRightMargin == 0)
    /* include a unique floated box */
    pBox->BxType = BoFloatGhost;
  else if ((inLine || inLineFloat) && // within a block of lines
           !*inlineFloatC && // no included floated boxes
           pAb->AbAcceptLineBreak &&
           displayI &&
           cansplit &&
           /* and not already registered as .... */
           pBox->BxType != BoFloatGhost &&
           pBox->BxType != BoFloatBlock &&
           pBox->BxType != BoCellBlock)
    {
      if (pAb->AbFirstEnclosed)
        {
          if (pAb->AbDisplay == 'B')
            // include a pseudo paragraph
             pBox->BxType = BoStructGhost;
          else
            pBox->BxType = BoGhost;
        }
      /* this element can be split */
      *inlineChildren = inLine;
      *inlineFloatC = inLineFloat;
    }
  else if (inLineFloat && // within a block of lines generated by a float
	   // a block or a possible floated ghost
	   (*inlineChildren || (*inlineFloatC && uniqueChild)) &&
           displayI &&
           cansplit &&
           pAb->AbFirstEnclosed &&
           /* and not already registered as .... */
           pBox->BxType != BoFloatGhost &&
           //pBox->BxType != BoFloatBlock &&
           pBox->BxType != BoCellBlock &&
           pBox->BxType != BoCell)
    {
      if (pAb->AbDisplay == 'B')
        pBox->BxType = BoStructGhost;
      else
        pBox->BxType = BoGhost;
      /* this element can be split */
      *inlineChildren = inLine;
      *inlineFloatC = inLineFloat;
    }
  else if (*inlineChildren)
    {
      /* block of lines */
      if (pBox->BxType == BoCell)
        {
          pBox->BxType = BoCellBlock;
          *inlineFloatC = FALSE;
          pBox->BxFirstLine = NULL;
          pBox->BxLastLine = NULL;
        }
      else if (displayI && cansplit &&
               pAb->AbEnclosing && pAb->AbEnclosing->AbBox &&
               pAb->AbEnclosing->AbBox->BxType == BoGhost)
        pBox->BxType = BoGhost;
      else if (*inlineFloatC &&
               pAb->AbEnclosing && pAb->AbEnclosing->AbBox &&
               pAb->AbEnclosing->AbBox->BxType == BoStructGhost)
        pBox->BxType = BoStructGhost;
      else
        {
          pBox->BxType = BoBlock;
          *inlineFloatC = FALSE;
          pBox->BxFirstLine = NULL;
          pBox->BxLastLine = NULL;
        }
    }
  else if (*inlineFloatC)
    {
      if (cansplit &&
          pAb->AbEnclosing && pAb->AbEnclosing->AbBox &&
          (pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
           pAb->AbEnclosing->AbBox->BxType == BoStructGhost))
        pBox->BxType = BoStructGhost;
      else if (pBox->BxType == BoCell)
        pBox->BxType = BoCellBlock;
      else
        pBox->BxType = BoFloatBlock;
      *inlineChildren = FALSE;
      pBox->BxFirstLine = NULL;
      pBox->BxLastLine = NULL;
    }
  else
    {
      /* no inline indicator for children */
      *inlineChildren = FALSE;
      *inlineFloatC = FALSE;
    }
}

/*----------------------------------------------------------------------
  CreateBox creates the box associated to the abstract box pAb.
  - compute its constrainted dimensions.
  - compute margins, paddings and borders.
  - create enclosed boxes.
  - compute its real dimensions and choose between inherited and real values.
  - compute reference axis (baseline).           
  - get the position of the box origin (left top corner) relativelly to its   
  enclosing when it's not set inLine.     
  Terminal boxes are chained (forward and back) from the BxPrevious and the
  BxNext of the root box.
  Store the box address into the abstract box.
  The parameter inLine is TRUE when the box is displayed within a block of
  line. The parameter inLineFloat is TRUE when the box is displayed within
  a block generated by a floating box.
  Return the box address.
  incfloat gives the depth increase generated by floated boxes.
  ----------------------------------------------------------------------*/
static PtrBox CreateBox (PtrAbstractBox pAb, int frame, ThotBool inLine,
                         ThotBool inLineFloat, int *carIndex, int incfloat)
{
  PtrSSchema          pSS;
  PtrAbstractBox      pChildAb, pParent;
  PtrBox              box, pMainBox;
  PtrBox              pBox;
  TypeUnit            unit;
  SpecFont            font;
  ThotPictInfo       *picture;
  BoxType             boxType;
  char                script = 'L';
  int                 width, i, height, zoom;
  ThotBool            enclosedWidth, enclosedHeight;
  ThotBool            inlineChildren, inlineFloatC;
  ThotBool            positioning;

  if (pAb->AbDead)
    return (NULL);
  /* by default it's not a table element */
  boxType = BoComplete;
  pSS = pAb->AbElement->ElStructSchema;
  if (pAb->AbLeafType == LtCompound)
    {
      if (TypeHasException (ExcIsColHead, pAb->AbElement->ElTypeNumber, pSS))
        boxType = BoColumn;
      else if (TypeHasException (ExcIsTable, pAb->AbElement->ElTypeNumber, pSS))
        boxType = BoTable;
      else if (TypeHasException (ExcIsRow, pAb->AbElement->ElTypeNumber, pSS))
        boxType = BoRow;
      else if (TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber, pSS))
        boxType = BoCell;
    }
  /* Chargement de la fonte attachee au pave */
  height = pAb->AbSize;
  unit = pAb->AbSizeUnit;
  if (pAb->AbLeafType == LtText &&
      pAb->AbElement->ElLanguage < TtaGetFirstUserLanguage ())
    /* ElLanguage is actually a script */
    script = TtaGetScript (pAb->AbLang);
  else if (pAb->AbLeafType == LtSymbol)
    script = 'G';
  else
    script = 'L';
  /* teste l'unite */
  if (pAb->AbLeafType == LtCompound && pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
    font = pAb->AbEnclosing->AbBox->BxFont;
  else
    font = ThotLoadFont (script, pAb->AbFont, FontStyleAndWeight(pAb),
                         height, unit, frame);

  /* Creation */
  pBox = pAb->AbBox;
  if (pBox == NULL)
    {
      pBox = GetBox (pAb);
      pAb->AbBox = pBox;
    }
  if (pBox)
    {
      // zoom apply to SVG only
      //if (pAb->AbElement && pAb->AbElement->ElStructSchema &&
      //    pAb->AbElement->ElStructSchema->SsName &&
      //    !strcmp (pAb->AbElement->ElStructSchema->SsName, "SVG"))
      zoom = ViewFrameTable[frame - 1].FrMagnification;
      //else
      //  zoom = 0;
      if (pAb->AbLeafType == LtCompound)
        pBox->BxType = boxType;
      /* pMainBox points to the root box of the view */
      pMainBox = ViewFrameTable[frame - 1].FrAbstractBox->AbBox;
      pBox->BxFont = font;
      pBox->BxUnderline = pAb->AbUnderline;
      pBox->BxThickness = pAb->AbThickness;

      /* Dimensionnement de la boite par contraintes */
      /* Il faut initialiser le trace reel et l'indication */
      /* des reperes inverses (en cas de boite elastique)  */
      /* avant d'evaluer la dimension de la boite si elle  */
      /* est de type graphique */
      if (pAb->AbLeafType == LtGraphics)
        {
          pAb->AbRealShape = pAb->AbShape;
          pBox->BxHorizInverted = FALSE;
          pBox->BxVertInverted = FALSE;
        }
      /* New values of margins, paddings and borders */
      pAb->AbMBPChange = FALSE;

      // check if place holder are displayed
      if (TypeHasException (ExcIsPlaceholder, pAb->AbElement->ElTypeNumber, pSS))
        {
          ThotBool show;
          TtaGetEnvBoolean ("SHOW_PLACEHOLDER", &show);
          if (!show || ElementIsReadOnly (pAb->AbElement))
            {
              pAb->AbTopBColor = -1;
              pAb->AbRightBColor = -1;
              pAb->AbBottomBColor = -1;
              pAb->AbLeftBColor = -1;
            }
        }
      ComputeMBP (pAb, frame, TRUE, FALSE);
      ComputeMBP (pAb, frame, FALSE, FALSE);

      pBox->BxXToCompute = FALSE;
      pBox->BxYToCompute = FALSE;
      enclosedWidth = ComputeDimRelation (pAb, frame, TRUE);
      enclosedHeight = ComputeDimRelation (pAb, frame, FALSE);
      if (boxType == BoRow || boxType == BoColumn || boxType == BoTable)
        {
          /* float and inlines are not allowed for rows and cells */
          inLine = FALSE;
          inLineFloat = FALSE;
          inlineChildren = FALSE;
          inlineFloatC = FALSE;
          if (boxType == BoRow || boxType == BoColumn)
            pAb->AbFloat = 'N';
          else if (pAb->AbFloat == 'L')
            AddFloatingBox (pAb, frame, TRUE);
          else if (pAb->AbFloat == 'R')
            AddFloatingBox (pAb, frame, FALSE);
        }
      else
        {
          if (boxType == BoCell)
            pAb->AbFloat = 'N';
          else if (pAb->AbFloat == 'L')
            AddFloatingBox (pAb, frame, TRUE);
          else if (pAb->AbFloat == 'R')
            AddFloatingBox (pAb, frame, FALSE);

          if (pAb->AbLeafType == LtCompound)
            CheckGhost (pAb, frame, inLine, inLineFloat, &inlineChildren, &inlineFloatC);
          else
            {
              inlineChildren = FALSE;
              inlineFloatC = FALSE;
            }
        }
      if (pAb->AbFloat == 'L' || pAb->AbFloat == 'R')
        incfloat += 1;
      pAb->AbDepth -= incfloat;
      if (pAb->AbLeafType != LtCompound)
        {
          /* Positionnement des axes de la boite construite */
          ComputeAxisRelation (pAb->AbVertRef, pBox, frame, TRUE);
          /* On traite differemment la base d'un bloc de lignes  */
          /* s'il depend de la premiere boite englobee           */
          ComputeAxisRelation (pAb->AbHorizRef, pBox, frame, FALSE);
        }
      /* On construit le chainage des boites terminales pour affichage */
      /* et on calcule la position des paves dans le document.         */
      if (pAb->AbFirstEnclosed == NULL)
        {
          /* This is a new displayed box */
          pBox->BxNew = TRUE;
          if (pMainBox == pBox)
            {
              /* It's the root box */
              pBox->BxPrevious = NULL;
              pBox->BxNext = NULL;
            }
          else
            {
              /* Add the new box at the end of the displayed boxes list */
              /* The list is pointed from the root box */
              /* BxNext(Root) -> First displayed box              */
              /* BxPrevious(Root) -> Last displayed box           */
              box = pMainBox->BxPrevious;
              pBox->BxPrevious = box;
              if (box)
                {
                  box->BxNext = pBox;
                  if (box->BxType == BoPiece || box->BxType == BoScript)
                    /* update also the split parent box */
                    box->BxAbstractBox->AbBox->BxNext = pBox;
                }
              pMainBox->BxPrevious = pBox;
              if (pMainBox->BxNext == NULL)
                pMainBox->BxNext = pBox;
            }
          pBox->BxIndChar = 0;
          *carIndex += pAb->AbVolume;
        }

      /* manage shadow exception */
      if (pAb->AbPresentationBox)
        pBox->BxShadow = FALSE;
      else if (TypeHasException (ExcShadow, pAb->AbElement->ElTypeNumber, pSS))
        pBox->BxShadow = TRUE;
      else if (pBox->BxAbstractBox->AbEnclosing &&
               pBox->BxAbstractBox->AbEnclosing->AbBox &&
               pBox->BxAbstractBox->AbEnclosing->AbBox->BxShadow)
        pBox->BxShadow = TRUE;
      else
        pBox->BxShadow = FALSE;
      /* Note if there is a border or a fill */
      MarkDisplayedBox (pBox);
      pAb->AbNew = FALSE;	/* The box is now created */
      
      /* Evaluation du contenu de la boite */
      switch (pAb->AbLeafType)
        {
        case LtPageColBreak:
          pBox->BxBuffer = NULL;
          pBox->BxNChars = pAb->AbVolume;
          width = 0;
          height = 0;
          break;
        case LtText:
          pBox->BxBuffer = pAb->AbText;
          pBox->BxNChars = pAb->AbVolume;
          pBox->BxFirstChar = 1;
          pBox->BxSpaceWidth = 0;
          /* get the default script */
          pBox->BxScript = script;
          GiveTextSize (pAb, frame, &width, &height, &i);
          pBox->BxNSpaces = i;
          break;
        case LtPicture:
          pBox->BxType = BoPicture;
          picture = (ThotPictInfo *) pAb->AbPictInfo;
          pBox->BxPictInfo = pAb->AbPictInfo;
          if (!pAb->AbPresentationBox && pAb->AbVolume &&
              pBox->BxPictInfo)
            {
              /* box size has to be positive */
              if (pBox->BxW < 0)
                pBox->BxW = 0;
              if (pBox->BxH < 0)
                pBox->BxH = 0;
            }
	  
#ifdef _GL
          if (!glIsTexture (picture->TextureBind))	    
#else /*_GL*/
            if (picture->PicPixmap == None)
#endif /*_GL*/
              LoadPicture (frame, pBox, picture);
          GivePictureSize (pAb, zoom, &width, &height);
          break;
        case LtSymbol:
          pBox->BxBuffer = NULL;
          pBox->BxNChars = pAb->AbVolume;
          /* Les reperes de la boite (elastique) ne sont pas inverses */
          pBox->BxHorizInverted = FALSE;
          pBox->BxVertInverted = FALSE;
          GiveSymbolSize (pAb, &width, &height);
          break;
        case LtGraphics:
          pBox->BxBuffer = NULL;
          pBox->BxNChars = pAb->AbVolume;
          GiveGraphicSize (pAb, &width, &height);
          if (pAb->AbShape == 1 || pAb->AbShape == 'C')
            {
              /* update radius of the rectangle with rounded corners */
              ComputeRadius (pAb, frame, TRUE);
              ComputeRadius (pAb, frame, FALSE);
            }
          break;
        case LtPolyLine:
          /* Prend une copie des points de controle */
          pBox->BxBuffer = CopyText (pAb->AbPolyLineBuffer, NULL);
          pBox->BxNChars = pAb->AbVolume;	/* Nombre de points */
          pBox->BxPictInfo = NULL;
          pBox->BxXRatio = 1;
          pBox->BxYRatio = 1;
          GivePolylineSize (pAb, zoom, &width, &height);
          break;
        case LtPath:
          /* Prend une copie du path */
          pBox->BxFirstPathSeg = CopyPath (pAb->AbFirstPathSeg);
          pBox->BxNChars = pAb->AbVolume;
          pBox->BxXRatio = 1;
          pBox->BxYRatio = 1;
          if (pAb->AbEnclosing)
            {
              /* the direct parent is the SVG path element */
              pParent = GetParentGroup (pBox);
              if (pParent == NULL)
                pParent = pAb->AbEnclosing->AbEnclosing;
              width = pParent->AbBox->BxWidth;
              height = pParent->AbBox->BxHeight;
              pAb->AbBox->BxWidth = width;
              pAb->AbBox->BxHeight = height;
              pParent = pAb->AbEnclosing;
              pParent->AbBox->BxWidth = width;
              pParent->AbBox->BxHeight = height;
            }
          else
            {
              width = 0;
              height = 0;
            }
          break;
        case LtCompound:
          if (pBox->BxType == BoTable)
            {
              pBox->BxColumns = NULL;
              pBox->BxRows = NULL;
              pBox->BxMaxWidth = 0;
              pBox->BxMinWidth = 0;
            }
          else if (pBox->BxType == BoColumn)
            {
              pBox->BxTable = NULL;
              pBox->BxRows = NULL;
              pBox->BxMaxWidth = 0;
              pBox->BxMinWidth = 0;
            }
          else if (pBox->BxType == BoRow)
            {
              pBox->BxTable = NULL;
              pBox->BxRows = NULL;
              pBox->BxMaxWidth = 0;
              pBox->BxMinWidth = 0;
            }
          else if (pBox->BxType == BoCell || pBox->BxType == BoCellBlock)
            {
              pBox->BxTable = NULL;
              pBox->BxRows = NULL;
              pBox->BxMaxWidth = 0;
              pBox->BxMinWidth = 0;
            }

          /* Is there a background image ? */
          if (pAb->AbPictBackground != NULL)
            {
              /* force filling */
              pAb->AbFillBox = TRUE;
              /* load the picture */
              picture = (ThotPictInfo *) pAb->AbPictBackground;
              if (picture->PicPixmap == None)
                LoadPicture (frame, pBox, picture);
            }

          if (pBox == pMainBox)
            {
              /* Set the right document background */
              if (pAb->AbFillBox ||
		  (!pAb->AbGradientBackground && pAb->AbBackground == -1))
                SetMainWindowBackgroundColor (frame, pAb->AbBackground);
              else
                SetMainWindowBackgroundColor (frame, DefaultBColor);
            }
          /* create enclosed boxes */
          pChildAb = pAb->AbFirstEnclosed;
          while (pChildAb)
            {
              box = CreateBox (pChildAb, frame, inlineChildren,
                                inlineFloatC, carIndex, incfloat);
              pChildAb = pChildAb->AbNext;
            }

          GiveEnclosureSize (pAb, frame, &width, &height);
          /* Position of box axis */
          ComputeAxisRelation (pAb->AbVertRef, pBox, frame, TRUE);
          /* don't manage the baseline of blocks here */
          if ((pBox->BxType != BoFloatBlock &&
               pBox->BxType != BoBlock &&
               pBox->BxType != BoCellBlock) ||
              pAb->AbHorizRef.PosAbRef != pAb->AbFirstEnclosed)
            ComputeAxisRelation (pAb->AbHorizRef, pBox, frame, FALSE);
          break;
        default:
          break;
        }

      /* Dimensionnement de la boite par le contenu ? */
      ChangeDefaultWidth (pBox, pBox, width, 0, frame);
      /* Il est possible que le changement de largeur de la boite modifie */
      /* indirectement (parce que la boite contient un bloc de ligne) la  */
      /* hauteur du contenu de la boite.                                  */
      if (enclosedWidth && enclosedHeight && pAb->AbLeafType == LtCompound)
        GiveEnclosureSize (pAb, frame, &width, &height);
      ChangeDefaultHeight (pBox, pBox, height, frame);
      /* recheck auto and % margins */
      CheckMBP (pAb, pBox, frame, TRUE);
      
      /* Positioning of the created box */
      i = 0;
      positioning = ComputePositioning (pBox, frame);
      if (IsFlow (pBox, frame))
        AddFlow (pAb, frame);
      if (!positioning)
        {
          if (!inLine && !inLineFloat &&
              pBox->BxType != BoFloatGhost &&
              pBox->BxType != BoGhost &&
              pBox->BxType != BoStructGhost)
            {
              ComputePosRelation (&pAb->AbHorizPos, pBox, frame, TRUE);
              ComputePosRelation (&pAb->AbVertPos, pBox, frame, FALSE);
            }
          else
            {
              if (!pAb->AbHorizEnclosing || pAb->AbNotInLine)
                /* the inline rule doesn't act on this box */
                ComputePosRelation (&pAb->AbHorizPos, pBox, frame, TRUE);
              else if (pAb->AbPresentationBox && pAb->AbTypeNum == 0 &&
                       pAb->AbHorizPos.PosAbRef && pAb->AbHorizPos.PosAbRef->AbFloat != 'N')
                {
                /* the inline rule doesn't act on this box */
                ComputePosRelation (&pAb->AbHorizPos, pBox, frame, TRUE);
                ComputePosRelation (&pAb->AbVertPos, pBox, frame, FALSE);
                }
              else
                /* the box position depends of its horizontal reference axis */
                SetPositionConstraint (VertRef, pBox, &i);
              if (!pAb->AbVertEnclosing)
                /* the inline rule doesn't act on this box */
                ComputePosRelation (&pAb->AbVertPos, pBox, frame, FALSE);
              else if (pAb->AbNotInLine)
                /* the inline rule doesn't act on this box */
                ComputePosRelation (&pAb->AbVertPos, pBox, frame, FALSE);
              else
                /* the box position depends of its horizontal reference axis */
                SetPositionConstraint (HorizRef, pBox, &i);
            }
        }
     
#ifdef _GL
      if (pAb->AbElement->ElAnimation)
        {
          i = ActiveFrame;
          ActiveFrame = frame;
          AnimatedBoxAdd ((PtrElement)pAb->AbElement);
          ActiveFrame = i;
        }   
#endif /* _GL */

      /* manage table exceptions */
      if (pBox->BxType == BoTable)
        UpdateTable (pAb, NULL, NULL, frame);
      else if (pBox->BxType == BoColumn)
        UpdateTable (NULL, pAb, NULL, frame);
      else if (pBox->BxType == BoRow)
        UpdateTable (NULL, NULL, pAb, frame);
      else if (pBox->BxType == BoCell)
        UpdateColumnWidth (pAb, NULL, frame);
    }

#ifdef _GL
  pBox->BxClipX = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
  pBox->BxClipY = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding;
  pBox->BxClipW = pBox->BxW;
  pBox->BxClipH = pBox->BxH;
#endif /* _GL */

  return (pBox);
}

/*----------------------------------------------------------------------
  SearchEnclosingType look for the enclosing table, row or block.
  ----------------------------------------------------------------------*/
PtrAbstractBox SearchEnclosingType (PtrAbstractBox pAb, BoxType type1,
                                    BoxType type2, BoxType type3)
{
  ThotBool       still;

  still = (pAb != NULL);
  while (still)
    {
      if (pAb->AbBox == NULL)
        {
          pAb = NULL;
          still = FALSE;
        }
      /* Is it the table box */
      else if (pAb->AbBox->BxType == type1 ||
               pAb->AbBox->BxType == type2 || pAb->AbBox->BxType == type3)
        still = FALSE;
      else
        {
          pAb = pAb->AbEnclosing;
          still = (pAb != NULL);
        }
    }
  return (pAb);
}


/*----------------------------------------------------------------------
  SearchLine looks for the line that includes the box pBox.
  ----------------------------------------------------------------------*/
PtrLine SearchLine (PtrBox pBox, int frame)
{
  PtrLine             pLine;
  PtrBox              pBoxPiece;
  PtrBox              pBoxInLine;
  PtrBox              pCurrentBox;
  PtrAbstractBox      pAb;
  ThotBool            still;

  /* check enclosing element */
  pLine = NULL;
  pAb = NULL;
  if (pBox)
    {
      pAb = pBox->BxAbstractBox;
      if (pAb == NULL || (pAb->AbNotInLine && pAb->AbDisplay == 'U'))
        pAb = NULL;
      else
        pAb = pAb->AbEnclosing;
    }

  /* look for an enclosing block of lines */
  if (pAb)
    {
      if (pAb->AbBox == NULL)
        pAb = NULL;
      else if (pAb->AbBox->BxType == BoGhost ||
               pAb->AbBox->BxType == BoStructGhost)
        {
          /* It's a ghost, look for the enclosing block */
          still = TRUE;
          while (still)
            {
              pAb = pAb->AbEnclosing;
              if (pAb == NULL)
                still = FALSE;
              else if (pAb->AbBox == NULL)
                {
                  pAb = NULL;
                  still = FALSE;
                }
              else if (pAb->AbBox->BxType != BoGhost &&
                       pAb->AbBox->BxType != BoStructGhost)
                still = FALSE;
            }
        }
      else if (pAb->AbBox->BxType == BoFloatGhost)
        pAb = pAb->AbEnclosing;
      else if (pAb->AbBox->BxType != BoBlock &&
               pAb->AbBox->BxType != BoFloatBlock &&
               pAb->AbBox->BxType != BoCellBlock)
        /* the box in not within a block of lines */
        pAb = NULL;
    }

  if (pAb)
    {
      pCurrentBox = pAb->AbBox;
      pLine = pCurrentBox->BxFirstLine;
      /* Look for the line which includes the current box */
      still = TRUE;
      while (still && pLine)
        {
          /* Locate the box in the set of lines */
          if (pLine->LiFirstPiece)
            pBoxInLine = pLine->LiFirstPiece;
          else
            pBoxInLine = pLine->LiFirstBox;
          if (pBoxInLine)
            do
              {
                if (pBoxInLine->BxType == BoSplit ||
                    pBoxInLine->BxType == BoMulScript)
                  pBoxPiece = pBoxInLine->BxNexChild;
                else
                  pBoxPiece = pBoxInLine;
                if (pBoxPiece == pBox)
                  {
                    /* the line is found */
                    still = FALSE;
                    pBoxPiece = pLine->LiLastBox;
                  }
                /* else get next box */
                else if ((pBoxPiece->BxType == BoScript ||
                          pBoxPiece->BxType == BoPiece) &&
                         pBoxPiece->BxNexChild)
                  pBoxInLine = pBoxPiece->BxNexChild;
                else
                  pBoxInLine = GetNextBox (pBoxInLine->BxAbstractBox, frame);
              }
            while (pBoxPiece != pLine->LiLastBox
                   && pBoxPiece != pLine->LiLastPiece
                   && pBoxInLine);

          if (still)
            /* get next line */
            pLine = pLine->LiNext;
        }
    }
  return pLine;
}


/*----------------------------------------------------------------------
  BoxUpdate updates the information about a leaf box:
  - number of characters
  - number os spaces
  - inside and outside width
  The added width is wDelta for the main box and either adjustDelta
  (wDelta if adjustDelta is zero) for pieces of the main box.
  The parameter splitBox is TRUE when that updat could change the break
  between two lines. In that case only the main box is updated and the
  algorithm that splits the ext in lines is called.
  ----------------------------------------------------------------------*/
PtrBox BoxUpdate (PtrBox pBox, PtrLine pLine, int charDelta, int spaceDelta,
                  int wDelta, int adjustDelta, int hDelta, int frame,
                  ThotBool splitBox)
{
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel, *pViewSelEnd;
  PtrBox              box, pMainBox, pParentBox, prev = NULL;
  Propagation         savpropage;
  PtrAbstractBox      pAb;
  AbPosition         *pPosAb;
  AbDimension        *pDimAb;
  int                 j;

  pAb = pBox->BxAbstractBox;
  pMainBox = pAb->AbBox;
  if (pBox->BxType == BoPiece || pBox->BxType == BoScript || pBox->BxType == BoDotted)
    {
      /* Update the initial box */
      pMainBox->BxNChars += charDelta;
      pMainBox->BxNSpaces += spaceDelta;
      pMainBox->BxW += wDelta;
      pMainBox->BxWidth += wDelta;
      pMainBox->BxH += hDelta;
      pMainBox->BxHeight += hDelta;
      /* Faut-il mettre a jour la base ? */
      pPosAb = &pAb->AbHorizRef;
      if (pPosAb->PosAbRef == NULL)
        {
          j = BoxFontBase (pMainBox->BxFont) + pMainBox->BxTMargin + pMainBox->BxTBorder + pMainBox->BxTPadding - pMainBox->BxHorizRef;
          MoveHorizRef (pAb->AbBox, NULL, j, frame);
        }
      else if (pPosAb->PosAbRef == pMainBox->BxAbstractBox)
        {
          if (pPosAb->PosRefEdge == HorizMiddle)
            MoveHorizRef (pAb->AbBox, NULL, hDelta / 2, frame);
          else if (pPosAb->PosRefEdge == Bottom)
            MoveHorizRef (pAb->AbBox, NULL, hDelta, frame);
        }

      /* Mise a jour des positions des boites suivantes */
      box = pBox->BxNexChild;
      while (box)
        {
          box->BxFirstChar += charDelta;
          if (box->BxBuffer == pBox->BxBuffer)
            box->BxIndChar += charDelta;
          box = box->BxNexChild;
        }
    }
  /* Traitement sur la boite passee en parametre */
  savpropage = Propagate;
  /* update the box itself */
  if (pAb->AbLeafType == LtText)
    {
      /* when a character between 2 boxes are removed the box is unchanged */
      pBox->BxNSpaces += spaceDelta;
      pBox->BxNChars += charDelta;
      pFrame = &ViewFrameTable[frame - 1];
      if (pBox->BxType == BoScript && pBox->BxNChars == 0)
        {
          // free the empty script box
          if (pMainBox->BxNexChild != pBox)
            {
              prev = pMainBox->BxNexChild;
              while (prev && prev->BxNexChild != pBox)
                prev = prev->BxNexChild;
            }

          // update box links
          if (prev)
            {
              // there is a previous child
              prev->BxNexChild = pBox->BxNexChild;
              prev->BxNext = pBox->BxNext;
              if (pBox->BxNext)
                pBox->BxNext->BxPrevious = prev;
              else
                pFrame->FrAbstractBox->AbBox->BxPrevious = prev;
            }
          else
            {
              pMainBox->BxNexChild = pBox->BxNexChild;
              if (pBox->BxPrevious)
                pBox->BxPrevious->BxNext = pBox->BxNext;
              else
                pFrame->FrAbstractBox->AbBox->BxNext = pBox->BxNext;
              
              if (pBox->BxNext)
                pBox->BxNext->BxPrevious = pBox->BxPrevious;
              else
                pFrame->FrAbstractBox->AbBox->BxPrevious = pBox->BxPrevious;
            }
#ifdef _GL
          if (glIsList (pBox->DisplayList))
            {
              glDeleteLists (pBox->DisplayList, 1);
              pBox->DisplayList = 0;
            }
#endif /* _GL */
          pLine = SearchLine (pBox->BxNexChild, frame);
          pViewSel = &pFrame->FrSelectionBegin;
          pViewSelEnd = &pFrame->FrSelectionEnd;
          box = FreeBox (pBox);
          // update the selection
          if (box)
            {
              pViewSel->VsBox = box;
              pViewSelEnd->VsBox = box;
            }
          else
            {
              box = pMainBox->BxNexChild;
              if (box)
                {
                  // there is almost a previous script box
                  pViewSel->VsBox = box;
                  pViewSelEnd->VsBox = box;
                  if (prev)
                    {
                      // select the end of previous box
                      pViewSel->VsXPos = box->BxW;
                      pViewSel->VsNSpaces = box->BxNSpaces;
                      pViewSel->VsIndBox = box->BxNChars;
                    }
                  pViewSelEnd->VsXPos = pViewSel->VsXPos + 2;
                  pViewSelEnd->VsNSpaces = pViewSel->VsNSpaces;
                  pViewSelEnd->VsIndBox = pViewSel->VsIndBox;
                }
              else
                {
                  pMainBox->BxType = BoComplete;
                  box = pMainBox;
                  pViewSel->VsBox = box;
                  pViewSelEnd->VsBox = box;
                }
            }
          /* Recompute the whole block? */
          if (Propagate == ToAll)
            RecomputeLines (pAb->AbEnclosing, pLine, box, frame);
          Propagate = savpropage;
          return box;
        }
    }

  /* does the box width depends on the content? */
  pDimAb = &(pBox->BxAbstractBox->AbWidth);
  if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
    pParentBox = pAb->AbEnclosing->AbBox;
  else
    pParentBox = NULL;
  if (pBox->BxContentWidth || (!pDimAb->DimIsPosition && pDimAb->DimMinimum) ||
      (pParentBox && pParentBox->BxShrink &&
       (pParentBox->BxW != pParentBox->BxRuleWidth ||
        (wDelta < 0 && pParentBox->BxW + wDelta < pParentBox->BxRuleWidth))))
    {
      /* space between two boxes */
      if (splitBox && pLine)
        {
          /* need to update the box width */
          if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript || adjustDelta == 0)
            {
              pBox->BxW += wDelta;
              pBox->BxWidth += wDelta;
            }
          else
            {
              pBox->BxW += adjustDelta;
              pBox->BxWidth += adjustDelta;
            }
          /* then recompute line */
          RecomputeLines (pAb->AbEnclosing, pLine, pBox, frame);
        }
      else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          /* split box */
          Propagate = ToSiblings;
          if (wDelta)
            ChangeDefaultWidth (pBox, pBox, pBox->BxW + wDelta, 0, frame);
          if (hDelta)
            ChangeDefaultHeight (pBox, pBox, pBox->BxH + hDelta, frame);
          Propagate = savpropage;
          /* Recompute the whole block? */
          if (Propagate == ToAll)
            {
              pLine = SearchLine (pBox->BxNexChild, frame);
              RecomputeLines (pAb->AbEnclosing, pLine, pBox, frame);
            }
        }
      else
        {
          /* unsplit box */
          if (adjustDelta)
            ChangeDefaultWidth (pBox, pBox, pBox->BxW + adjustDelta, spaceDelta, frame);
          else if (wDelta)
            ChangeDefaultWidth (pBox, pBox, pBox->BxW + wDelta, spaceDelta, frame);
          if (hDelta != 0)
            ChangeDefaultHeight (pBox, pBox, pBox->BxH + hDelta, frame);
        }
    }
  else if (pBox->BxContentHeight ||
           (!pBox->BxAbstractBox->AbHeight.DimIsPosition &&
            pBox->BxAbstractBox->AbHeight.DimMinimum))
    {
      /* box height depend on the content */
      if (hDelta)
        ChangeDefaultHeight (pBox, pBox, pBox->BxH + hDelta, frame);
    }
  else if (pBox->BxW > 0 && pBox->BxH > 0)
    /* if the box width doesn't depend on the contents
       redisplay the whole box */
    DefBoxRegion (frame, pBox, -1, -1, -1, -1);
  Propagate = savpropage;
#ifdef _GL
  pBox->VisibleModification = TRUE;
  return pBox;
#else
  return pBox;
#endif /* _GL */
}


/*----------------------------------------------------------------------
  RemoveBoxes removes the current box and all included boxes.
  The parameter toRemake is TRUE when the box will be recreated
  immediatly.
  ----------------------------------------------------------------------*/
void RemoveBoxes (PtrAbstractBox pAb, ThotBool rebuild, int frame)
{
  PtrAbstractBox      pChildAb;
  PtrBox              pBox;
  ThotBool            changeSelectBegin;
  ThotBool            changeSelectEnd;

  if (pAb)
    {
      if (pAb->AbBox)
        {
          /* Liberation des lignes et boites coupees */
          pBox = pAb->AbBox;
          /* Remove out of structure relations */
          ClearOutOfStructRelation (pBox, frame);
          if (pAb->AbFloat != 'N')
            ClearAFloat (pAb);

          if (pAb->AbLeafType == LtCompound)
            {
              if (pAb->AbPositioning)
                RemoveFlow (pAb, frame);
              /* unregister the box */
              pBox->BxDisplay = FALSE;
              if (pBox->BxType == BoBlock ||
                  pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
                {
                  /* free floating contexts */
                  ClearFloats (pBox);
                  RemoveLines (pBox, frame, pBox->BxFirstLine, TRUE,
                               &changeSelectBegin, &changeSelectEnd);
                }
              else if (pBox->BxType == BoTable)
                ClearTable (pAb);
              else if (pBox->BxType == BoColumn)
                UpdateTable (NULL, pAb, NULL, frame);
              else if (pBox->BxType == BoRow)
                UpdateTable (NULL, NULL, pAb, frame);
            }
          else if (pAb->AbLeafType == LtPolyLine)
            FreePolyline (pBox);
          else if (pAb->AbLeafType == LtPath)
            FreePath (pBox);
          else if (pAb->AbLeafType == LtPicture)
            CleanPictInfo ((ThotPictInfo *)pBox->BxPictInfo);
          else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
            /* free child boxes */
            UnsplitBox (pBox, frame);

          // Reset the abstract box status
          pAb->AbFloatChange = FALSE;
          pAb->AbPositionChange = FALSE;
          pAb->AbWidthChange = FALSE;
          pAb->AbHeightChange = FALSE;
          pAb->AbAspectChange = FALSE;
          pAb->AbChange = FALSE;
          pAb->AbSizeChange = FALSE;
          pAb->AbMBPChange = FALSE;
          pAb->AbHorizPosChange = FALSE;
          pAb->AbVertPosChange = FALSE;
          pAb->AbHorizRefChange = FALSE;
          pAb->AbVertRefChange = FALSE;
          if (pAb->AbDead)
            pAb->AbNew = FALSE;
          else
            pAb->AbNew = rebuild;
          if (pAb->AbNew)
            {
              /* Faut-il restaurer les regles d'une boite elastique */
              if (pBox->BxHorizFlex && pBox->BxHorizInverted)
                XEdgesExchange (pBox, OpHorizDep);
	      
              if (pBox->BxVertFlex && pBox->BxVertInverted)
                YEdgesExchange (pBox, OpVertDep);
            }

          /* Liberation des boites des paves inclus */
          pChildAb = pAb->AbFirstEnclosed;
          while (pChildAb)
            {
              RemoveBoxes (pChildAb, rebuild, frame);
              pChildAb = pChildAb->AbNext;
            }

          /* Suppression des references a pBox dans la selection */
          if (ViewFrameTable[frame - 1].FrSelectionBegin.VsBox == pBox)
            ViewFrameTable[frame - 1].FrSelectionBegin.VsBox = NULL;
          if (ViewFrameTable[frame - 1].FrSelectionEnd.VsBox == pBox)
            ViewFrameTable[frame - 1].FrSelectionEnd.VsBox = NULL;

          /* Liberation de la boite */
          if (pBox->BxType == BoTable)
            ClearTable (pAb);
#ifdef _GL
#ifdef _WX
          wxASSERT_MSG( !pAb->AbBox->DisplayList ||
                        glIsList (pAb->AbBox->DisplayList),
                        _T("GLBUG - RemoveBoxes : glIsList returns false"));
#endif /* _WX */
          if (glIsList (pAb->AbBox->DisplayList))
            {
              glDeleteLists (pAb->AbBox->DisplayList, 1);
              pAb->AbBox->DisplayList = 0;
            }
#endif /* _GL */
          ClearAllRelations (pAb->AbBox, frame);
          if (PackBoxRoot == pAb->AbBox)
            PackBoxRoot = NULL;
          pAb->AbBox = FreeBox (pAb->AbBox);
          pAb->AbBox = NULL;
        }
    }
}


/*----------------------------------------------------------------------
  CheckDefaultPositions reevalue les regles par defaut des paves  
  suivants si le pave cree ou detruit est positionne par 
  une regle par defaut.                                  
  ----------------------------------------------------------------------*/
static void CheckDefaultPositions (PtrAbstractBox pAb, int frame)
{
  PtrAbstractBox      pNextAb;

  /* Est-ce que la boite avait une regle par defaut ? */
  if (pAb->AbEnclosing &&
      pAb->AbEnclosing->AbBox->BxType != BoBlock &&
      pAb->AbEnclosing->AbBox->BxType != BoFloatBlock &&
      pAb->AbEnclosing->AbBox->BxType != BoCellBlock &&
      pAb->AbEnclosing->AbBox->BxType != BoGhost &&
      pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
      pAb->AbEnclosing->AbBox->BxType != BoFloatGhost)
    {
      if (pAb->AbHorizPos.PosAbRef == NULL)
        {
          /* On recherche le pave suivant ayant la meme regle par defaut */
          pNextAb = pAb->AbNext;
          while (pNextAb)
            {
              if (ExtraFlow (pNextAb->AbBox, frame))
                pNextAb = pNextAb->AbNext;
              else if (pNextAb->AbHorizPos.PosAbRef == NULL)
                {
                  /* Reevalue la regle du premier pave suivant non mort */
                  if (!pNextAb->AbDead && pNextAb->AbBox
                      /* si ce pave ne vient pas d'etre cree                */
                      && pNextAb->AbNum == 0)
                    {
                      /* Nouvelle position horizontale */
                      ClearPosRelation (pNextAb->AbBox, TRUE);
                      ComputePosRelation (&pNextAb->AbHorizPos, pNextAb->AbBox,
                                          frame, TRUE);
                    }
                  pNextAb = NULL;
		  
                }
              else
                pNextAb = pNextAb->AbNext;
            }
        }
      if (pAb->AbVertPos.PosAbRef == NULL)
        {
          /* On recherche le pave suivant ayant la meme regle par defaut */
          pNextAb = pAb->AbNext;
          while (pNextAb)
            {
              if (ExtraFlow (pNextAb->AbBox, frame))
                pNextAb = pNextAb->AbNext;
              else if (pNextAb->AbVertPos.PosAbRef == NULL)
                {
                  /* Reevalue la regle du premier pave suivant non mort */
                  if (!pNextAb->AbDead && pNextAb->AbBox != NULL
                      /* si ce pave ne vient pas d'etre cree                */
                      && pNextAb->AbNum == 0)
                    {
                      /* Nouvelle position verticale */
                      ClearPosRelation (pNextAb->AbBox, FALSE);
                      ComputePosRelation (&pNextAb->AbVertPos, pNextAb->AbBox, frame, FALSE);
                    }
                  pNextAb = NULL;
                }
              else
                pNextAb = pNextAb->AbNext;
            }
        }
    }
}


/*----------------------------------------------------------------------
  RecordEnclosing  registers differed packings.        
  ----------------------------------------------------------------------*/
void RecordEnclosing (PtrBox pBox, ThotBool horizRef)
{
  int                 i;
  PtrDimRelations     pDimRel;
  PtrDimRelations     pPreviousDimRel;
  ThotBool            toCreate;

  if (horizRef && pBox &&
      (pBox->BxType == BoCell || pBox->BxType == BoTable ||
       pBox->BxType == BoRow))
    /* width of these elements is computed in tableH.c */
    return;
  /* Look for an empty entry */
  pPreviousDimRel = NULL;
  pDimRel = DifferedPackBlocks;
  toCreate = TRUE;
  i = 0;
  while (toCreate && pDimRel != NULL)
    {
      i = 0;
      pPreviousDimRel = pDimRel;
      while (i < MAX_RELAT_DIM && pDimRel->DimRTable[i] != NULL)
        {
          if ((pDimRel->DimROp[i] == OpSame && horizRef) ||
              (pDimRel->DimROp[i] == OpReverse && !horizRef))
            {
              /* The right dimension */
              if (pBox == pDimRel->DimRTable[i])
                /* The box is already registered */
                return;
              else
                i++;
            }
          else
            i++;
        }
      
      if (i == MAX_RELAT_DIM)
        /* next block */
        pDimRel = pDimRel->DimRNext;
      else
        toCreate = FALSE;
    }

  if (toCreate)
    {
      /* Create a new block */
      i = 0;
      GetDimBlock (&pDimRel);
      if (pPreviousDimRel == NULL)
        DifferedPackBlocks = pDimRel;
      else
        pPreviousDimRel->DimRNext = pDimRel;
    }

  pDimRel->DimRTable[i] = pBox;
  /* packing the width or the height */
  if (horizRef)
    pDimRel->DimROp[i] = OpSame;
  else
    pDimRel->DimROp[i] = OpReverse;
}


/*----------------------------------------------------------------------
  UpdateFloat updates floating information of the abstract box pAb
  within pParent.
  The parameter inLine is TRUE is the parent was previously a block.
  ----------------------------------------------------------------------*/
static void UpdateFloat (PtrAbstractBox pAb, PtrAbstractBox pParent,
                         ThotBool inLine, int frame, ThotBool *computeBBoxes)
{
  PtrBox              pBox, pBlock;
  PtrAbstractBox      pChild;
  PtrFloat            pfloat;

  pBox = pAb->AbBox;
  if (pBox->BxType == BoRow ||
      pBox->BxType == BoColumn ||
      pBox->BxType == BoCell)
    pAb->AbFloat = 'N';
  if (pAb->AbFloat == 'L' || pAb->AbFloat == 'R')
    {
      // check if there is a previous or next element
      pChild = pAb->AbPrevious;
      while (pChild && pChild->AbPresentationBox)
        pChild = pChild->AbPrevious;
      if (pChild == NULL)
        {
          pChild = pAb->AbNext;
          while (pChild && pChild->AbPresentationBox)
            pChild = pChild->AbNext;
        }
      if (pParent->AbFloat == 'N' && !ExtraAbFlow (pParent, frame) &&
          pParent->AbBox &&
          pParent->AbInLine &&
          pParent->AbEnclosing &&
          (pParent->AbElement &&
           !TypeHasException (ExcNewRoot, pParent->AbElement->ElTypeNumber,
                              pParent->AbElement->ElStructSchema)) &&
          pChild == NULL && /* no previous or next */
          pAb->AbFloat != 'N' &&
          (pAb->AbLeafType == LtPicture ||
           (pAb->AbLeafType == LtCompound &&
            !pAb->AbWidth.DimIsPosition &&
            pAb->AbWidth.DimAbRef == NULL)))
        {
          /* a block with only one floating child */
          pParent->AbBox->BxType = BoFloatGhost;
          pParent = pParent->AbEnclosing;
        }
      if (pAb->AbFloat == 'L')
        AddFloatingBox (pAb, frame, TRUE);
      else if (pAb->AbFloat == 'R')
        AddFloatingBox (pAb, frame, FALSE);

      /* update all children of the new block */
      pChild = pParent->AbFirstEnclosed;
      while (pChild && !pChild->AbDead)
        {
          if (pChild->AbBox)
            {
              pChild->AbWidthChange = TRUE;
              pChild->AbHeightChange = TRUE;
              pChild->AbHorizPosChange = TRUE;
              pChild->AbVertPosChange = TRUE;
              ComputeUpdates (pChild, frame, computeBBoxes);
            }
          pChild = pChild->AbNext;
        }
      RecomputeLines (pParent, NULL, NULL, frame);
    }
  else if (inLine)
    {
      /* check if the float property is removed */
      while (pParent && pParent->AbBox &&
             pParent->AbBox->BxType == BoFloatGhost)
        {
          if (pParent->AbInLine)
            pParent->AbBox->BxType = BoBlock;
          else
            pParent->AbBox->BxType = BoComplete;
          pParent = pParent->AbEnclosing;
        }
      if (pParent && pParent->AbBox)
        {
          pBlock = pParent->AbBox;
          pfloat = pBlock->BxLeftFloat;
          while (pfloat && pfloat->FlBox != pBox)
            pfloat = pfloat->FlNext;
          if (pfloat == NULL)
            {
              pfloat = pBlock->BxRightFloat;
              while (pfloat && pfloat->FlBox != pBox)
                pfloat = pfloat->FlNext;
            }
          if (pfloat)
            {
              /* the float is removed */
              if (pfloat->FlNext)
                pfloat->FlNext->FlPrevious = pfloat->FlPrevious;
              if (pfloat == pBlock->BxLeftFloat)
                pBlock->BxLeftFloat = pfloat->FlNext;
              else if (pfloat == pBlock->BxRightFloat)
                pBlock->BxRightFloat = pfloat->FlNext;
              else
                pfloat->FlPrevious->FlNext = pfloat->FlNext;
              TtaFreeMemory (pfloat);
              pfloat = NULL;
            }

          if ((pBlock->BxType == BoFloatBlock ||
               pBlock->BxType == BoCellBlock) &&
              pBlock->BxLeftFloat == NULL &&
              pBlock->BxRightFloat == NULL)
            {
              pBlock->BxType = BoComplete;
              /* update all children of the old block */
              pChild = pParent->AbFirstEnclosed;
              while (pChild && !pChild->AbDead)
                {
                  if (pChild->AbBox)
                    {
                      pChild->AbWidthChange = TRUE;
                      pChild->AbHeightChange = TRUE;
                      pChild->AbHorizPosChange = TRUE;
                      pChild->AbVertPosChange = TRUE;
                      ComputeUpdates (pChild, frame, computeBBoxes);
                    }
                  pChild = pChild->AbNext;
                }
              /* update the current box */
              pParent = pAb->AbEnclosing;
              if (pParent && pParent->AbBox && pParent->AbBox->BxType == BoBlock)
                RecomputeLines (pParent, NULL, NULL, frame);
              else
                {
                  pAb->AbHorizPosChange = TRUE;
                  pAb->AbVertPosChange = TRUE;
                }
            }
          else if (pAb->AbClear != 'N' && pParent)
            /* probably a clear value changed */
            RecomputeLines (pParent, NULL, NULL, frame);
        }
    }
}

/*----------------------------------------------------------------------
  TransmitDeadStatus marks all children of a dead abstract box.
  ----------------------------------------------------------------------*/
static void TransmitDeadStatus (PtrAbstractBox pAb, ThotBool status)
{
  PtrAbstractBox  pChild;

  if (pAb)
    {
      pChild = pAb->AbFirstEnclosed;
      while (pChild)
        { 
          pChild->AbDead = status;
          TransmitDeadStatus (pChild, status);
          pChild = pChild->AbNext;
        }
    }
}

/*----------------------------------------------------------------------
  ComputeUpdates checks what is changing in the current Abstract Box.
  Return TRUE if there is at least one change.
  The parameter computeBBoxes is set TRUE if this bounding boxe should
  be recomputed else it's unchanged.
  ----------------------------------------------------------------------*/
ThotBool ComputeUpdates (PtrAbstractBox pAb, int frame, ThotBool *computeBBoxes)
{
  PtrLine             pLine;
  PtrAbstractBox      pCurrentAb, pCell, pBlock, pParent, curr, table, pChild;
  PtrBox              pNextBox, pCurrentBox = NULL;
  PtrBox              pMainBox, pLastBox, pBox, box = NULL;
  PtrElement          pEl;
  TypeUnit            unit;
  ThotPictInfo       *imageDesc = NULL;
  Propagation         savpropage;
  ViewFrame          *pFrame;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 width, height, zoom, incfloat = 0;
  int                 nSpaces, savedW, savedH;
  int                 i, charDelta, adjustDelta;
  ThotBool            condition, inLine, inLineFloat;
  ThotBool            withinblock, withinblockfloat;
  ThotBool            result, isCell, isImg;
  ThotBool            orgXComplete, orgYComplete, found;
#ifdef _GL
  ThotBool            FrameUpdatingStatus;

  FrameUpdatingStatus = FrameUpdating;
  FrameUpdating = TRUE;
#endif /* _GL */
  pFrame = &ViewFrameTable[frame - 1];
  pLastBox = NULL;
  nSpaces = 0;			/* nombre de blancs */
  charDelta = 0;		/* nombre de caracteres */
  adjustDelta = 0;		/* largeur avec des blancs justifies */
  width = 0;
  height = 0;
  if (pFrame->FrAbstractBox == NULL)
    pMainBox = NULL;
  else
    pMainBox = pFrame->FrAbstractBox->AbBox;	/* root box */
  pBox = pAb->AbBox;
  pBlock = NULL;
  result = FALSE;
  condition = FALSE;
  savpropage = Propagate;
  pParent = pAb->AbEnclosing;
  inLine = FALSE;
  inLineFloat = FALSE;
  isCell = FALSE;
  if (pParent && pParent->AbBox)
    {
      // check if the parent is already set inline
      if (pParent->AbEnclosing &&
          (pAb->AbNew || pAb->AbDead || pAb->AbFloatChange))
        curr = pParent->AbEnclosing;
      else
        curr = pParent;
      withinblock = FALSE;
      withinblockfloat = FALSE;
     if (curr->AbBox->BxType == BoBlock)
       /* within a block */
       withinblock = TRUE;
      else if (curr->AbBox->BxType == BoFloatBlock ||
               curr->AbBox->BxType == BoCellBlock)
        /* within a block generated by a floating box */
        withinblockfloat = TRUE;
      else if (curr->AbBox->BxType == BoGhost ||
               curr->AbBox->BxType == BoStructGhost ||
               curr->AbBox->BxType == BoFloatGhost)
        {
          pBlock = curr->AbEnclosing;
          while (pBlock && pBlock->AbBox &&
                 pBlock->AbBox->BxType != BoBlock &&
                 pBlock->AbBox->BxType != BoFloatBlock &&
                 pBlock->AbBox->BxType != BoCellBlock)
            pBlock = pBlock->AbEnclosing;
          if (pBlock && pBlock->AbBox)
            {
              if (pBlock->AbBox->BxType == BoBlock)
                /* within a block */
                withinblock = TRUE;
              else
                withinblockfloat = TRUE;
            }
        }

     if (pParent->AbEnclosing &&
         (pAb->AbNew || pAb->AbDead || pAb->AbFloatChange))
        /* Recheck the status of the parent box */
        CheckGhost (pParent, frame, withinblock, withinblockfloat, &inLine, &inLineFloat);
      else
        {
          inLine = withinblock;
          inLineFloat = withinblockfloat;
        }
      pBlock = pParent->AbEnclosing;
      while (pBlock && pBlock->AbBox &&
             pBlock->AbBox->BxType != BoBlock &&
             pBlock->AbBox->BxType != BoFloatBlock &&
             pBlock->AbBox->BxType != BoCellBlock)
        pBlock = pBlock->AbEnclosing;
    }

  /* Prepare the clipping */
  if (pAb->AbNew || pAb->AbDead || pAb->AbChange || pAb->AbMBPChange ||
      pAb->AbWidthChange || pAb->AbHeightChange ||
      pAb->AbHorizPosChange || pAb->AbVertPosChange ||
      pAb->AbHorizRefChange || pAb->AbVertRefChange ||
      pAb->AbAspectChange || pAb->AbSizeChange)
    {
      /* look at if the box or an enclosing box has a background */
      if (pAb->AbHorizEnclosing && pAb->AbVertEnclosing &&
          (pAb->AbNew || pAb->AbDead ||
           pAb->AbHorizPosChange || pAb->AbVertPosChange))
        {
          pCurrentAb = pParent;
          curr = pCurrentAb;
          while (curr)
            {
              if (curr->AbPictBackground ||
                  curr->AbFillBox ||
                  (curr->AbTopStyle > 2 && curr->AbTopBColor != -2 &&
                   curr->AbTopBorder > 0) ||
                  (curr->AbLeftStyle > 2 && curr->AbLeftBColor != -2 &&
                   curr->AbLeftBorder > 0) ||
                  (curr->AbBottomStyle > 2 && curr->AbBottomBColor != -2 &&
                   curr->AbBottomBorder > 0) ||
                  (curr->AbRightStyle > 2 && curr->AbRightBColor != -2 &&
                   curr->AbRightBorder > 0))
                {
                  pCurrentAb = curr;
                  curr = NULL;
                }
              else
                curr = curr->AbEnclosing;
            }
          if (pCurrentAb == NULL)
            /* no background and no parent: clip the current box */
            pCurrentAb = pAb;
        }
      else
        pCurrentAb = pAb;
      
      pCurrentBox = pCurrentAb->AbBox;
      if (pCurrentBox)
        {
          if (pCurrentBox->BxType == BoGhost ||
              pCurrentBox->BxType == BoStructGhost ||
              pCurrentBox->BxType == BoFloatGhost)
            {
              /* move to the enclosing block */
              while (pCurrentAb && pCurrentAb->AbBox &&
                     (pCurrentAb->AbBox->BxType == BoGhost ||
                      pCurrentAb->AbBox->BxType == BoStructGhost ||
                      pCurrentAb->AbBox->BxType == BoFloatGhost))
                pCurrentAb = pCurrentAb->AbEnclosing;
              if (pCurrentAb == NULL || pCurrentAb->AbBox == NULL)
                pCurrentBox = pBox;
              else
                pCurrentBox = pCurrentAb->AbBox;
            }
          if (pCurrentBox->BxType == BoSplit || pCurrentBox->BxType == BoMulScript)
            /* get the first displayed box */
            pCurrentBox = pCurrentBox->BxNexChild;
          if ((pCurrentBox->BxWidth > 0 && pCurrentBox->BxHeight > 0) ||
              pCurrentBox->BxType == BoPicture)
            {
              /* mark the zone to be displayed */
              if (TypeHasException (ExcSetWindowBackground, pAb->AbElement->ElTypeNumber,
                                    pAb->AbElement->ElStructSchema))
                /* the whole windows */
                DefClip (frame, -1, -1, -1, -1);
              else
                {
#ifdef _GL
                  if (NeedToComputeABoundingBox (pCurrentBox->BxAbstractBox, frame))
                    {
                      /* need to recompute almost this bounding box */
                      *computeBBoxes = TRUE;
                      pCurrentBox->BxBoundinBoxComputed = FALSE;
                    }
#endif /* _GL */
                  /* the whole box */
                  DefBoxRegion (frame, pCurrentBox, -1, -1, -1, -1);
                }
            }
        }

#ifdef _GL
      if (pCurrentBox)
        pCurrentBox->VisibleModification = TRUE;
      if (pBox)
        pBox->VisibleModification = TRUE;
#endif /* _GL */

      /* NEW AbstractBox */
      if (pAb->AbNew)
        {
          AnyWidthUpdate = TRUE;
          /* Check the position of the new box in the list of terminal boxes */
          pCurrentAb = PreviousLeafAbstractBox (pAb);
          /* Get the box just before */
          if (pCurrentAb)
            pCurrentBox = pCurrentAb->AbBox;
          else
            pCurrentBox = NULL;
          i = 0; /* Position within the document */
          if (pCurrentBox)
            {
              if (pCurrentBox->BxType == BoSplit || pCurrentBox->BxType == BoMulScript)
                /* take the last split box */
                while (pCurrentBox->BxNexChild)
                  pCurrentBox = pCurrentBox->BxNexChild;
              pNextBox = pCurrentBox->BxNext;
            }
          else
            {
              if (pMainBox)
                pNextBox = pMainBox->BxNext;
              else
                pNextBox = NULL;
            }

          if (pParent && pNextBox && pNextBox == pParent->AbBox)
            /* the new box will replace the previous one */
            pNextBox = pNextBox->BxNext;
          /* On etablit le chainage pour inserer en fin les nouvelles boites */
          /* Faut-il dechainer la boite englobante ? */
          if (pMainBox)
            {
              if (pCurrentBox == NULL)
                pMainBox->BxNext = NULL;	/* debut du chainage */
              pLastBox = pMainBox->BxPrevious;	/* memorise la derniere boite*/
              pMainBox->BxPrevious = pCurrentBox;/* fin provisoire du chainage*/
            }
          else
            pLastBox = NULL;	/* memorise la derniere boite*/
      
          /* Faut-il dechainer la boite englobante ? */
          ReadyToDisplay = FALSE;	/* On arrete l'evaluation du reaffichage */
          Propagate = ToSiblings;	/* Limite la propagation sur le descendance */
          // compute the depth generated by enclosing floated boxes
          curr = pParent;
          while (curr)
            {
              if (curr->AbFloat == 'L' || curr->AbFloat == 'R')
                incfloat += 1;
              curr = curr->AbEnclosing;
            }
          pBox = CreateBox (pAb, frame, inLine, inLineFloat, &i, incfloat);
      
          ReadyToDisplay = TRUE;	/* On retablit l'evaluation du reaffichage */      
          /* Mise a jour de la liste des boites terminales */
          if (pMainBox && pNextBox)
            {
              /* On ajoute en fin de chainage */
              pNextBox->BxPrevious = pMainBox->BxPrevious;
              /* derniere boite chainee */
              if (pMainBox->BxPrevious)
                pMainBox->BxPrevious->BxNext = pNextBox;
              pMainBox->BxPrevious = pLastBox;
              /* nouvelle fin de chainage */
              if (pMainBox->BxNext == NULL)
                pMainBox->BxNext = pNextBox;
            }

          if (pBox)
            {
              /* set the box position in the concrete image */
              IsXYPosComplete (pBox, &orgXComplete, &orgYComplete);
              if (!orgXComplete || !orgYComplete)
                {
                  /* si la boite n'a pas deja ete placee en absolu */
                  /* Initialise le placement des boites creees */
                  SetBoxToTranslate (pAb, !orgXComplete, !orgYComplete);
                  /* La boite racine va etre placee */
                  pBox->BxXToCompute = FALSE;
                  pBox->BxYToCompute = FALSE;
                  AddBoxTranslations (pAb, pFrame->FrVisibility, frame,
                                      !orgXComplete, !orgYComplete);
                }
#ifdef _GL
              else
                {
                  if (FrameTable[frame].FrView == 1 &&
                      (pBox->BxXOrg || pBox->BxYOrg) &&
                      !pAb->AbPresentationBox &&
                      pAb->AbElement->ElSystemOrigin)
                    {
                      pBox->BxClipX = 0;
                      pBox->BxClipY = 0;
                    }
                  else
                    {
                      pBox->BxClipX = pBox->BxXOrg;
                      pBox->BxClipY = pBox->BxYOrg;
                    }
                  pBox->BxClipW = pBox->BxW;
                  pBox->BxClipH = pBox->BxH;
                }
#endif /* _GL */

              /* On prepare le reaffichage */
              if (pAb->AbNotInLine || (!inLine && !inLineFloat))
                DefBoxRegion (frame, pBox, -1, -1, -1, -1);
	  
              pAb->AbChange = FALSE;
              pAb->AbFloatChange = FALSE;
              if (pBlock)
                {
                  /* update the enclosing block of line */
                  Propagate = ToChildren;
                  RecomputeLines (pBlock, NULL, NULL, frame);
                }
              /* check enclosing cell */
              pCell = GetParentCell (pBox);
              if (pBox->BxType != BoCell &&
                  TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber,
                                    pAb->AbElement->ElStructSchema))
                pCell = pAb->AbNext;
              if (pCell)
                {
                  Propagate = ToChildren;
                  UpdateColumnWidth (pCell, NULL, frame);
                }
              result = TRUE;
              pAb->AbWidthChange = FALSE;
              pAb->AbHeightChange = FALSE;
              pAb->AbAspectChange = FALSE;
              pAb->AbSizeChange = FALSE;
              pAb->AbChange = FALSE;
              pAb->AbMBPChange = FALSE;
            }
#ifdef _GL
          if (pBox)
            pBox->VisibleModification = TRUE;   
#endif /* _GL */
          /* Restore the propagation */
          Propagate = savpropage;
        }
      /* AbstractBox DEAD */
      else if (pAb->AbDead)
        {
          /* mark all children */
          TransmitDeadStatus (pAb, TRUE);
          AnyWidthUpdate = TRUE;
          if (pAb->AbLeafType == LtPolyLine)
            FreePolyline (pBox);	/* libere la liste des buffers de la boite */
          else if (pAb->AbLeafType == LtPath)
            FreePath (pBox);	/* libere la liste des descripteurs de path */

          /* On situe la boite dans le chainage des boites terminales */
          pCurrentAb = PreviousLeafAbstractBox (pAb);

          /* On recherche la derniere boite terminale avant */
          if (pCurrentAb == NULL)
            pCurrentBox = pMainBox;	/* debut du chainage */
          else
            {
              pCurrentBox = pCurrentAb->AbBox;
              /* Est-ce que la boite est coupee ? */
              if (pCurrentBox->BxType == BoSplit || pCurrentBox->BxType == BoMulScript)
                while (pCurrentBox->BxNexChild)
                  pCurrentBox = pCurrentBox->BxNexChild;
            }

          /* Check if the enclosing box becomes empty */
          if (IsAbstractBoxEmpty (pParent))
            {
              pNextBox = pParent->AbBox;
              /* Change the status of the parent box */
              if (pNextBox->BxType == BoGhost ||
                  pNextBox->BxType == BoStructGhost ||
                  pNextBox->BxType == BoFloatGhost)
                {
                  if (pParent->AbDisplay == 'B' ||
                      (pParent->AbDisplay == 'U' && pParent->AbInLine))
                    pNextBox->BxType = BoBlock;
                  else
                    pNextBox->BxType = BoComplete;
                }
	  
              /* On ne chaine qu'une seule fois la boite englobante */
              if (pNextBox != pCurrentBox->BxNext)
                {
                  if (pCurrentBox == pMainBox)
                    pNextBox->BxPrevious = NULL;
                  else
                    pNextBox->BxPrevious = pCurrentBox;
                  /* On defait l'ancien chainage */
		  if (pCurrentBox->BxNext)
		    pCurrentBox->BxNext->BxPrevious = pCurrentBox->BxNext;
                  pCurrentBox->BxNext = pNextBox;
                }
              pCurrentBox = pNextBox;
            }
      
          /* On recherche la premiere boite terminale apres */
          pCurrentAb = pAb;
          pNextBox = NULL;
          while (pNextBox == NULL)
            {
              while (pCurrentAb->AbEnclosing && !pCurrentAb->AbNext)
                pCurrentAb = pCurrentAb->AbEnclosing;
              pCurrentAb = pCurrentAb->AbNext;
              if (pCurrentAb == NULL)
                pNextBox = pMainBox;	/* 1ere boite suivante */
              else
                {
                  while (pCurrentAb->AbBox && pCurrentAb->AbFirstEnclosed)
                    pCurrentAb = pCurrentAb->AbFirstEnclosed;
                  pNextBox = pCurrentAb->AbBox;
                }
            }

          /* Est-ce que la boite est coupee ? */
          if (pNextBox->BxType == BoSplit || pNextBox->BxType == BoMulScript)
            pNextBox = pNextBox->BxNexChild;

          /* Destruction */
          pCell = GetParentCell (pCurrentBox);
          isCell = pCurrentBox->BxType == BoCell;
          RemoveBoxes (pAb, FALSE, frame);
      
          /* update the list of leaf boxes */
          if (pCurrentBox == pMainBox)
            /* first box in the list */
            pNextBox->BxPrevious = NULL;
          else
            {
              /* backward link */
              pNextBox->BxPrevious = pCurrentBox;
              if ((pNextBox->BxType == BoPiece || pNextBox->BxType == BoScript) &&
                  pNextBox->BxAbstractBox->AbBox)
                {
                  if (pCurrentBox->BxType == BoPiece || pCurrentBox->BxType == BoScript)
                    pNextBox->BxAbstractBox->AbBox->BxPrevious = pCurrentBox->BxAbstractBox->AbBox;
                  else
                    pNextBox->BxAbstractBox->AbBox->BxPrevious = pCurrentBox;
                }
            }
          if (pNextBox == pMainBox)
            /* last box in the list */
            pCurrentBox->BxNext = NULL;
          else
            {
              /* forward link */
              pCurrentBox->BxNext = pNextBox;
              if ((pCurrentBox->BxType == BoPiece || pCurrentBox->BxType == BoScript) &&
                  pCurrentBox->BxAbstractBox->AbBox)
                {
                  if (pNextBox->BxType == BoPiece || pNextBox->BxType == BoScript)
                    pCurrentBox->BxAbstractBox->AbBox->BxNext = pNextBox->BxAbstractBox->AbBox;
                  else
                    pCurrentBox->BxAbstractBox->AbBox->BxNext = pNextBox;
                }
            }
      
          /* Check table consistency */
          if (isCell && pCurrentBox && pCurrentBox->BxAbstractBox)
            UpdateColumnWidth (pCurrentBox->BxAbstractBox, NULL, frame);
          /* check enclosing cell */
          if (pCell && !IsDead (pAb) &&
              (pAb->AbNext || (!pAb->AbNext->AbDead && !pAb->AbNext->AbNew)))
            UpdateColumnWidth (pCell, NULL, frame);
          else if (pCell)
            UpdateCellHeight (pCell, frame);
          result = TRUE;
        }
      else
        {

          // zoom apply to SVG only
          //if (pAb->AbElement && pAb->AbElement->ElStructSchema &&
          //    pAb->AbElement->ElStructSchema->SsName &&
          //    !strcmp (pAb->AbElement->ElStructSchema->SsName, "SVG"))
          zoom = ViewFrameTable[frame - 1].FrMagnification;
          //else
          //   zoom = 0;

          /* CHANGE BOX ASPECT */
          if (pAb->AbAspectChange)
            {
              pAb->AbAspectChange = FALSE;
              if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                {
                  /* extend the clipping to the last piece when the box is split */
                  pCurrentBox = pBox;
                  result = TRUE;
                  while (result)
                    {
                      if (pCurrentBox->BxNexChild == NULL)
                        result = FALSE;
                      else if (pCurrentBox->BxNexChild->BxNChars == 0)
                        result = FALSE;
                      else
                        pCurrentBox = pCurrentBox->BxNexChild;
                    }
                }
              else if (pAb->AbLeafType == LtCompound)
                {
                  /* Is there a background image? */
                  if (pAb->AbPictBackground != NULL)
                    {
                      /* force filling */
                      pAb->AbFillBox = TRUE;
                      /* load the picture */
                      LoadPicture (frame, pBox, (ThotPictInfo *) pAb->AbPictBackground);
                    }
                }
              else if (pAb->AbLeafType == LtGraphics &&
		       (pAb->AbShape == 1 || pAb->AbShape == 'C'))
                {
                  /* update radius of the rectangle with rounded corners */
                  ComputeRadius (pAb, frame, TRUE);
                  ComputeRadius (pAb, frame, FALSE);
                }

              /* Note if there is a border or a fill */
              MarkDisplayedBox (pBox);
              /* Is it a filled box? */
              if (pAb->AbLeafType == LtCompound && pBox->BxType != BoCell &&
                  pAb->AbEnclosing == NULL)
                {
                  /* root abstractbox */
                  if (pBox->BxDisplay)
                    SetMainWindowBackgroundColor (frame, pAb->AbBackground);
                  else
                    SetMainWindowBackgroundColor (frame, DefaultBColor);
                }
            }
          /* CHANGE CHARACTER SIZE */
          if (pAb->AbSizeChange)
            {
              /* check the effect */
              pDimAb = &pAb->AbWidth;
              if (!pDimAb->DimIsPosition)
                {
                  if (pDimAb->DimAbRef || pDimAb->DimValue >= 0)
                    pAb->AbWidthChange = (pDimAb->DimUnit == UnRelative);
                  if (pAb->AbHorizPos.PosAbRef != NULL)
                    pAb->AbHorizPosChange = (pAb->AbHorizPos.PosUnit == UnRelative);
                }
	  
              pDimAb = &pAb->AbHeight;
              if (!pDimAb->DimIsPosition)
                {
                  if (pDimAb->DimAbRef != NULL || pDimAb->DimValue >= 0)
                    pAb->AbHeightChange = (pDimAb->DimUnit == UnRelative);
                  if (pAb->AbVertPos.PosAbRef != NULL)
                    pAb->AbVertPosChange = (pAb->AbVertPos.PosUnit == UnRelative);
                }
	    
              pAb->AbHorizRefChange = (pAb->AbHorizRef.PosUnit == UnRelative);
              pAb->AbVertRefChange = (pAb->AbVertRef.PosUnit == UnRelative);
            }
          /* CHANGE FLOAT/CLEAR */
          if (pAb->AbFloatChange)
            {
              Propagate = ToChildren;
              pAb->AbFloatChange = FALSE;
              UpdateFloat (pAb, pParent, inLine || inLineFloat, frame, computeBBoxes);
              /* Restore the propagation */
              Propagate = savpropage;
            }

          /* CHANGE THE CONTENTS */
          if (pAb->AbChange || pAb->AbSizeChange)
            {
              /* check the font of the abstract box */
              height = pAb->AbSize;
              unit = pAb->AbSizeUnit;
              if (pAb->AbLeafType == LtCompound &&
                  pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
                 pBox->BxFont = pAb->AbEnclosing->AbBox->BxFont;
              else if (pAb->AbLeafType == LtText)
                {
                  if (pAb->AbElement->ElLanguage < TtaGetFirstUserLanguage ())
                    /* ElLanguage is actually a script */
                    pBox->BxFont = ThotLoadFont (TtaGetScript (pAb->AbLang),
                                                 pAb->AbFont,
                                                 FontStyleAndWeight(pAb),
                                                 height, unit, frame);
                  else
                    pBox->BxFont = ThotLoadFont ('L', pAb->AbFont,
                                                 FontStyleAndWeight(pAb),
                                                 height, unit, frame);
                }
              else if (pAb->AbLeafType == LtSymbol)
                pBox->BxFont = ThotLoadFont ('G', pAb->AbFont, FontStyleAndWeight (pAb),
                                             height, unit, frame);
              else
                pBox->BxFont = ThotLoadFont ('L', 1, 0, height, unit, frame);
	  
              /* transmit underline and thickness */
              pBox->BxUnderline = pAb->AbUnderline;
              pBox->BxThickness = pAb->AbThickness;
              if (pAb->AbLeafType == LtCompound)
                {
                  /* update a compound element */
                  if (pBox->BxType == BoBlock ||
                      pBox->BxType == BoFloatBlock ||
                      pBox->BxType == BoCellBlock)
                    /* update the current block of lines */
                    RecomputeLines (pAb, NULL, pBox, frame);
                }
              else
                {
                  /* update a leaf element */
                  switch (pAb->AbLeafType)
                    {
                    case LtPageColBreak:
                      width = 0;
                      height = 0;
                      break;
                    case LtText:
                      GiveTextSize (pAb, frame, &width, &height, &nSpaces);
		  
                      /* Si la boite est justifiee */
                      if (pBox->BxSpaceWidth != 0)
                        {
                          i = pBox->BxSpaceWidth - BoxCharacterWidth (SPACE, 1, pBox->BxFont);
                          /* On prend la largeur justifiee */
                          width = width + i * nSpaces + pBox->BxNPixels;
                          /* width shift */
                          adjustDelta = width - pBox->BxW;
                        }
                      /* character shift */
                      charDelta = pAb->AbVolume - pBox->BxNChars;
                      /* space shift */
                      nSpaces -= pBox->BxNSpaces;
                      pBox->BxBuffer = pAb->AbText;
                      break;
                    case LtPicture:
                      if (pAb->AbChange)
                        {
                          /* the picture change, RAZ the structure */
                          CleanPictInfo ((ThotPictInfo *) pBox->BxPictInfo);
                          if (pAb->AbWidth.DimAbRef == NULL &&  pAb->AbWidth.DimValue == -1)
                            {
                              /* use the content width */
                              pBox->BxWidth -= pBox->BxW;
                              pBox->BxW = 0;
                              pBox->BxVertRef = 0;
                            }
                          if (pAb->AbHeight.DimAbRef == NULL &&  pAb->AbHeight.DimValue == -1)
                            {
                              /* use the content height */
                              pBox->BxHeight -= pBox->BxH;
                              pBox->BxH = 0;
                              pBox->BxHorizRef = 0;
                            }
                          SetCursorWatch (frame);
                          LoadPicture (frame, pBox, (ThotPictInfo *) pBox->BxPictInfo);
                          ResetCursorWatch (frame);
                          GivePictureSize (pAb, zoom, &width, &height);
                        }
                      else
                        {
                          width = pBox->BxW;
                          height = pBox->BxH;
                        }
                      break;
                    case LtSymbol:
                      GiveSymbolSize (pAb, &width, &height);
                      break;
                    case LtGraphics:
                      /* Si le trace graphique a change */
                      if (pAb->AbChange)
                        {
                          /* Si transformation polyline en graphique simple */
                          pAb->AbRealShape = pAb->AbShape;
                          /* remonte a la recherche d'un ancetre elastique */
                          pCurrentAb = pAb;
                          while (pCurrentAb != NULL)
                            {
                              pCurrentBox = pCurrentAb->AbBox;
                              if (pCurrentBox->BxHorizFlex || pCurrentBox->BxVertFlex)
                                {
                                  MirrorShape (pAb, pCurrentBox->BxHorizInverted,
                                               pCurrentBox->BxVertInverted,
                                               FALSE);
                                  pCurrentAb = NULL;		/* on arrete */
                                }
                              else
                                pCurrentAb = pCurrentAb->AbEnclosing;
                            }
                        }
                      GiveGraphicSize (pAb, &width, &height);
                      break;
                    case LtPolyLine:
                      if (pAb->AbChange)
                        {
                          /* Libere les anciens buffers */
                          FreePolyline (pBox);
                          /* Recopie les buffers du pave */
                          pBox->BxBuffer = CopyText (pAb->AbPolyLineBuffer, NULL);
                          pBox->BxNChars = pAb->AbVolume;
		      
                          /* remonte a la recherche d'un ancetre elastique */
                          pCurrentAb = pAb;
                          while (pCurrentAb != NULL)
                            {
                              pCurrentBox = pCurrentAb->AbBox;
                              if (pCurrentBox->BxHorizFlex ||
                                  pCurrentBox->BxVertFlex)
                                {
                                  MirrorShape (pAb,
                                               pCurrentBox->BxHorizInverted,
                                               pCurrentBox->BxVertInverted,
                                               FALSE);
                                  /* on arrete */
                                  pCurrentAb = NULL;	
                                }
                              else
                                pCurrentAb = pCurrentAb->AbEnclosing;
                            }
                        }
                      GivePolylineSize (pAb, zoom, &width, &height);
                      break;
                    case LtPath:
                      /* by default don't change the size */
                      width = pAb->AbBox->BxWidth;
                      height = pAb->AbBox->BxHeight;
                      if (pAb->AbChange)
                        {
                          /* free the old path descriptor */
                          FreePath (pBox);
                          /* copy the new one from the abstract box */
                          pBox->BxFirstPathSeg = CopyPath (pAb->AbFirstPathSeg);
                          pBox->BxNChars = pAb->AbVolume;
                          if (pAb->AbEnclosing)
                            {
                              /* the direct parent is the SVG path element */
                              pParent = pAb->AbEnclosing->AbEnclosing;
                              while (pParent &&
                                     TypeHasException (ExcIsGroup,
                                                       pParent->AbElement->ElTypeNumber,
                                                       pParent->AbElement->ElStructSchema))
                                pParent = pParent->AbEnclosing;
                              width = pParent->AbBox->BxWidth;
                              height = pParent->AbBox->BxHeight;
                              pAb->AbBox->BxWidth = width;
                              pAb->AbBox->BxHeight = height;
                              pParent = pAb->AbEnclosing;
                              pParent->AbBox->BxWidth = width;
                              pParent->AbBox->BxHeight = height;
                            }
                        }
                      break;
                    default:
                      break;
                    }
	      
                  /* update the box  */
                  pDimAb = &pAb->AbWidth;
                  if (pDimAb->DimIsPosition)
                    width = 0;
                  else if (pDimAb->DimAbRef || pDimAb->DimValue >= 0)
                    width = 0;
                  else
                    width -= pBox->BxW;	/* width delta */
                  pDimAb = &pAb->AbHeight;
                  if (pDimAb->DimIsPosition)
                    height = 0;
                  else if (pDimAb->DimAbRef != NULL || pDimAb->DimValue >= 0)
                    height = 0;
                  else
                    height -= pBox->BxH;	/* height delta */
                  pLine = NULL;
                  /* the picture box width changed but not the
                     block of lines */
                  if (pAb->AbLeafType == LtPicture || width || height ||
                      pBox->BxType == BoMulScript)
                    {
                      BoxUpdate (pBox, pLine, charDelta, nSpaces, width,
                                 adjustDelta, height, frame, FALSE);
#ifdef _GL
                      if (NeedToComputeABoundingBox (pAb, frame))
                        {
                          /* need to recompute almost this bounding box */
                          *computeBBoxes = TRUE;
                          pBox->BxBoundinBoxComputed = FALSE;
                        }
#endif /* _GL */
                      if (pAb->AbLeafType == LtPicture)
                        {
                          DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                          if (pBlock)
                            {
                              RecomputeLines (pBlock, NULL, NULL, frame);
                              /* we will have to pack enclosing box */
                              if (pBlock->AbEnclosing)
                                RecordEnclosing (pBlock->AbEnclosing->AbBox, FALSE);
                            }
                        }
                      /* check enclosing cell */
                      pCell = GetParentCell (pBox);
                      if (pCell && width)
                        UpdateColumnWidth (pCell, NULL, frame);
                      result = TRUE;
                    }
                  else
                    result = FALSE;
                }

              if (pAb->AbChange)
                pAb->AbChange = FALSE;
              if (pAb->AbSizeChange)
                pAb->AbSizeChange = FALSE;
            }

          /* keep in memory previous width and height of the box */
          savedW = width = pBox->BxW;
          savedH = height = pBox->BxH;
          pEl = pAb->AbElement;
          pChild = NULL;
          if (pEl)
            {
              isImg = TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema);
              if (isImg && (pAb->AbWidthChange || pAb->AbHeightChange))
                {
                  // update all dimensions due to the ratio
                  pAb->AbWidthChange = TRUE;
                  pAb->AbHeightChange = TRUE;
                  // Need to update update the width/height of the enclosed PICTURE
                  pChild = pAb->AbFirstEnclosed;
                  found = FALSE;
                  while (!found && pChild)
                    {
                      if (pChild->AbBox && !pChild->AbPresentationBox &&
                          pChild->AbLeafType == LtPicture)
                        {
                          /* Update its width */
                          imageDesc = (ThotPictInfo *) pChild->AbBox->BxPictInfo;
                          found = TRUE;
                        }
                      else
                        pChild = pChild->AbNext;
                    }
                  if (found)
                    box = pChild->AbBox;
                }
            }
          else
            isImg = FALSE;
          if (pAb->AbLeafType == LtPicture &&
              (pAb->AbWidthChange || pAb->AbHeightChange))
            {
              if (!pAb->AbWidthChange &&
                  !pAb->AbWidth.DimIsPosition &&
                  pAb->AbWidth.DimValue == -1 &&
                  pAb->AbWidth.DimAbRef == NULL)
                /* due to the ratio, the width changes */
                pBox->BxW = 0;
              if (!pAb->AbHeightChange &&
                  !pAb->AbHeight.DimIsPosition &&
                  pAb->AbHeight.DimValue == -1 &&
                  pAb->AbHeight.DimAbRef == NULL)
                /* due to the ratio, the height changes */
                pBox->BxH = 0;
            }

          /* CHANGE WIDTH */
          if (pAb->AbWidthChange)
            {
              AnyWidthUpdate = TRUE;
              /* Remove the old value */
              ClearDimRelation (pBox, TRUE, frame);
              /* New width */
              condition = ComputeDimRelation (pAb, frame, TRUE);
              if (condition || (!pAb->AbWidth.DimIsPosition &&
                                pAb->AbWidth.DimMinimum))
                {
                  switch (pAb->AbLeafType)
                    {
                    case LtText:
                      GiveTextSize (pAb, frame, &width, &height, &i);
                      break;
                    case LtPicture:
                      imageDesc = (ThotPictInfo *) pBox->BxPictInfo;
                      pBox->BxW = 0;
                      LoadPicture (frame, pBox, imageDesc);
                      GivePictureSize (pAb, zoom, &width, &height);
                      break;
                    case LtSymbol:
                      GiveSymbolSize (pAb, &width, &height);
                      break;
                    case LtGraphics:
                      GiveGraphicSize (pAb, &width, &height);
                      break;
                    case LtCompound:
                      if (pBox->BxType == BoBlock ||
                          pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
                        {
                          RecomputeLines (pAb, NULL, NULL, frame);
                          width = pBox->BxW;
                        }
                      else if (isCell)
                        /* Check table consistency */
                        UpdateColumnWidth (pAb, NULL, frame);
                      else if (isImg && box && imageDesc)
                        {
                          // set to the default image size
                          width = imageDesc->PicWidth;
                          if (!pChild->AbWidth.DimIsPosition &&
                              pChild->AbWidth.DimValue == -1 &&
                              pChild->AbWidth.DimAbRef == NULL)
                            {
                              box->BxW = width;
                              box->BxWidth = width;
                            }
                        }
                      else
                        GiveEnclosureSize (pAb, frame, &width, &height);
                      break;
                    default:
                      width = pBox->BxW;
                      break;
                    }
	      
                  /* Change the width of the contents */
                  if (!result)
                    result = width != savedW;
                  ChangeDefaultWidth (pBox, NULL, width, 0, frame);
                  // the width is updated
                  savedW = width;
                }
              else
                {
                  /* the box width is constrained */
                  width = pBox->BxW;
                  savedW = width;
                  result = TRUE;
                }

              /* check auto margins */
              CheckMBP (pAb, pCurrentBox, frame, TRUE);
              if (pAb->AbLeafType == LtGraphics &&
		  (pAb->AbShape == 1 || pAb->AbShape == 'C') &&
                  pAb->AbRxUnit == UnPercent)
                /* update radius of the rectangle with rounded corners */
                ComputeRadius (pAb, frame, TRUE);
	  
              /* Check table consistency */
              if (pCurrentBox->BxType == BoColumn)
                UpdateTable (NULL, pAb, NULL, frame);
              else if (pCurrentBox->BxType == BoCell)
                UpdateColumnWidth (pAb, NULL, frame);
              /* check enclosing cell */
              pCell = GetParentCell (pCurrentBox);
              if (pCell)
                {
                  pBlock = SearchEnclosingType (pAb, BoBlock, BoFloatBlock, BoCellBlock);
                  if (pBlock != NULL)
                    RecomputeLines (pBlock, NULL, NULL, frame);
                  UpdateColumnWidth (pCell, NULL, frame);
                }
            }
          /* CHANGE HEIGHT */
          if (pAb->AbHeightChange)
            {
              /* Remove the old value */
              ClearDimRelation (pBox, FALSE, frame);
              /* New height */
              condition = ComputeDimRelation (pAb, frame, FALSE);
              if (condition ||
                  (!pAb->AbHeight.DimIsPosition && pAb->AbHeight.DimMinimum))
                {
                  switch (pAb->AbLeafType)
                    {
                    case LtText:
                      GiveTextSize (pAb, frame, &width, &height, &i);
                      break;
                    case LtPicture:
                      imageDesc = (ThotPictInfo *) pBox->BxPictInfo;
                      pBox->BxH = 0;
                      LoadPicture (frame, pBox, imageDesc);
                      GivePictureSize (pAb, zoom, &width, &height);
                      break;
                    case LtSymbol:
                      GiveSymbolSize (pAb, &width, &height);
                      break;
                    case LtGraphics:
                      GiveGraphicSize (pAb, &width, &height);
                      break;
                    case LtCompound:
                      if (pBox->BxType == BoBlock ||
                          pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
                        height = pBox->BxH;
                      else if (isImg && box && imageDesc)
                        {
                          // set to the default image size
                          height = imageDesc->PicHeight;
                          if (!pChild->AbHeight.DimIsPosition &&
                              pChild->AbHeight.DimValue == -1 &&
                              pChild->AbHeight.DimAbRef == NULL)
                            {
                              box->BxH = height;
                              box->BxHeight = height;
                            }
                        }
                      else
                        GiveEnclosureSize (pAb, frame, &width, &height);
                      break;
                    default:
                      height = pBox->BxH;
                      break;
                    }

                  /* Change the height of the contents */
                  if (!result)
                    result = height != savedH;
                  /* force the reevaluation of the baseline */
                  ClearAxisRelation (pBox, FALSE);
                  /* new horizontal */
                  ComputeAxisRelation (pAb->AbHorizRef, pBox, frame, FALSE);
                  ChangeDefaultHeight (pBox, NULL, height, frame);
                  // the height is updated
                  savedH = height;
                }
              else
                {
                  /* the box height is constrained */
                  height = pBox->BxH;
                  savedH = height;
                  result = TRUE;
                }

              /* recheck auto and % margins */
              CheckMBP (pAb, pCurrentBox, frame, TRUE);
              if (pAb->AbLeafType == LtGraphics &&
		  (pAb->AbShape == 1 || pAb->AbShape == 'C') &&
                  pAb->AbRyUnit == UnPercent)
                /* update radius of the rectangle with rounded corners */
                ComputeRadius (pAb, frame, FALSE);
            }

          if (isImg && pChild)
            {
              // Need to update update the width/height of the enclosed PICTURE
              /* Update its width */
              ClearDimRelation (pChild->AbBox, TRUE, frame);
              ComputeDimRelation (pChild, frame, TRUE);
              /* Update its height */
              ClearDimRelation (pChild->AbBox, FALSE, frame);
              ComputeDimRelation (pChild, frame, FALSE);
              // regenerate the texture
              LoadPicture (frame, pChild->AbBox,
                           (ThotPictInfo *) pChild->AbBox->BxPictInfo);
            }
          else if (pAb->AbLeafType == LtPicture)
            // regenerate the texture
            LoadPicture (frame, pBox, (ThotPictInfo *) pBox->BxPictInfo);

          /* MARGIN PADDING BORDER */
          if (pAb->AbMBPChange)
            {
              pAb->AbMBPChange = FALSE;
              Propagate = ToAll;	/* On passe en mode normal de propagation */
              if (CheckMBP (pAb, pBox, frame, TRUE))
                {
                  /* Check table consistency */
                  if (pCurrentBox->BxType == BoColumn)
                    UpdateTable (NULL, pAb, NULL, frame);
                  else if (pCurrentBox->BxType == BoCell)
                    UpdateColumnWidth (pAb, NULL, frame);
                  /* check enclosing cell */
                  pCell = GetParentCell (pCurrentBox);
                  if (pBlock)
                    {
                      /* update the enclosing block of line */
                      Propagate = ToChildren;
                      RecomputeLines (pBlock, NULL, NULL, frame);
                    }
                  if (pCell)
                    UpdateColumnWidth (pCell, NULL, frame);
                  else
                    {
                      table = GetParentTable (pCurrentBox);
                      if (table)
                        UpdateTable (table, NULL, NULL, frame);
                    }
                }

              /* do we have to register that box as filled box */
              if (pAb->AbLeafType == LtCompound && pBox->BxType != BoCell)
                MarkDisplayedBox (pBox);
              /* Restore the propagation */
              Propagate = savpropage;
              result = TRUE;
            }

          if (ExtraFlow (pBox, frame))
            {
              /* ignore position rules */
              pAb->AbHorizPosChange = FALSE;
              pAb->AbVertPosChange = FALSE;
            }
          /* CHANGE HORIZONTAL POSITION */
          if (pAb->AbHorizPosChange)
            {
              AnyWidthUpdate = TRUE;
              /* Les cas de coherence sur les boites elastiques */
              /* Les reperes Position et Dimension doivent etre differents */
              /* Ces reperes ne peuvent pas etre l'axe de reference        */
              if (pAb->AbWidth.DimIsPosition
                  && (pAb->AbHorizPos.PosEdge == pAb->AbWidth.DimPosition.PosEdge
                      || pAb->AbHorizPos.PosEdge == VertMiddle
                      || pAb->AbHorizPos.PosEdge == VertRef))
                {
                  if (pAb->AbWidth.DimPosition.PosEdge == Left)
                    pAb->AbHorizPos.PosEdge = Right;
                  else if (pAb->AbWidth.DimPosition.PosEdge == Right)
                    pAb->AbHorizPos.PosEdge = Left;
                  pAb->AbHorizPosChange = FALSE;
                }
              else if (pAb->AbEnclosing == NULL)
                condition = TRUE;
              else if (pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
                       pAb->AbEnclosing->AbBox->BxType == BoBlock ||
                       pAb->AbEnclosing->AbBox->BxType == BoCellBlock ||
                       pAb->AbEnclosing->AbBox->BxType == BoGhost ||
                       pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
                       pAb->AbEnclosing->AbBox->BxType == BoFloatGhost)
                /* the positioning rule is ignored */
                condition = !pAb->AbHorizEnclosing;
              else
                condition = TRUE;
              if (condition)
                {
                  /* remove the old horizontal position */
                  ClearPosRelation (pBox, TRUE);
                  if (pAb->AbHorizPos.PosAbRef && pAb->AbHorizPos.PosAbRef == pAb->AbPrevious)
                    while (pAb->AbHorizPos.PosAbRef && ExtraAbFlow (pAb, frame))
                      {
                        pAb->AbHorizPos.PosAbRef = pAb->AbHorizPos.PosAbRef->AbPrevious;
                        if (pAb->AbHorizPos.PosAbRef && pAb->AbHorizPos.PosAbRef->AbDead)
                          pAb->AbHorizPos.PosAbRef = pAb->AbHorizPos.PosAbRef->AbPrevious;
                      }
                  /* new horizontal position */
                  ComputePosRelation (&pAb->AbHorizPos, pBox, frame, TRUE);
                  result = TRUE;
                }
              else
                pAb->AbHorizPosChange = FALSE;
            }
          /* CHANGE VERTICAL POSITION */
          if (pAb->AbVertPosChange)
            {
              /* Les cas de coherence sur les boites elastiques */
              /* Les reperes Position et Dimension doivent etre differents */
              /* Ces reperes ne peuvent pas etre l'axe de reference        */
              if (pAb->AbHeight.DimIsPosition &&
                  (pAb->AbVertPos.PosEdge == pAb->AbHeight.DimPosition.PosEdge ||
                   pAb->AbVertPos.PosEdge == HorizMiddle ||
                   pAb->AbVertPos.PosEdge == HorizRef))
                {
                  if (pAb->AbHeight.DimPosition.PosEdge == Top)
                    pAb->AbVertPos.PosEdge = Bottom;
                  else if (pAb->AbHeight.DimPosition.PosEdge == Bottom)
                    pAb->AbVertPos.PosEdge = Top;
                  pAb->AbVertPosChange = FALSE;
                }
              else if (pAb->AbEnclosing == NULL || !pAb->AbVertEnclosing)
                condition = TRUE;
              else if (pAb->AbEnclosing->AbBox->BxType == BoBlock ||
                       pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
                       pAb->AbEnclosing->AbBox->BxType == BoCellBlock ||
                       pAb->AbEnclosing->AbBox->BxType == BoGhost ||
                       pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
                       pAb->AbEnclosing->AbBox->BxType == BoFloatGhost)
                {
                  /* the position depends on the line? */
                  if (!pAb->AbHorizEnclosing && pBox->BxNChars > 0)
                    {
                      pPosAb = &pAb->AbVertPos;
                      pLine = SearchLine (pBox, frame);
                      if (pLine)
                        {
                          i = GetPixelValue (pPosAb->PosDistance,
                                             pPosAb->PosUnit, pAb->AbBox->BxW, pAb, zoom);
                          i += GetPixelValue (pPosAb->PosDistDelta,
                                              pPosAb->PosDeltaUnit, pAb->AbBox->BxW, pAb, zoom);
                          pLine->LiYOrg += i;
                          EncloseInLine (pBox, frame, pAb->AbEnclosing);
                        }
                    }
                  condition = FALSE;
                }
              else
                condition = TRUE;
	  
              if (condition)
                {
                  /* remove the old vertical position */
                  ClearPosRelation (pBox, FALSE);
                  if (pAb->AbVertPos.PosAbRef && pAb->AbVertPos.PosAbRef == pAb->AbPrevious)
                    while (pAb->AbVertPos.PosAbRef && ExtraAbFlow (pAb, frame))
                      {
                        pAb->AbVertPos.PosAbRef = pAb->AbVertPos.PosAbRef->AbPrevious;
                        if (pAb->AbVertPos.PosAbRef && pAb->AbVertPos.PosAbRef->AbDead)
                          pAb->AbVertPos.PosAbRef = pAb->AbVertPos.PosAbRef->AbPrevious;
                      }
                  /* new vertical position */
                  ComputePosRelation (&pAb->AbVertPos, pBox, frame, FALSE);
                  result = TRUE;
                }
              else
                pAb->AbVertPosChange = FALSE;
            }
          /* CHANGE HORIZONTAL AXIS */
          if (pAb->AbHorizRefChange)
            {
              /* remove the old horizontal axis */
              ClearAxisRelation (pBox, FALSE);
              /* new horizontal */
              ComputeAxisRelation (pAb->AbHorizRef, pBox, frame, FALSE);
              result = TRUE;
            }
          /* CHANGE VERTICAL AXIS */
          if (pAb->AbVertRefChange)
            {
              /* remove the vertical axis */
              ClearAxisRelation (pBox, TRUE);
              /* new vertical axis */
              ComputeAxisRelation (pAb->AbVertRef, pBox, frame, TRUE);
              result = TRUE;
            }
        }
    }
#ifdef _GL  
  FrameUpdating = FrameUpdatingStatus;  
#endif /* _GL */
  return result;
}


/*----------------------------------------------------------------------
  ComputeEnclosing manages differed enclosing rules
  ----------------------------------------------------------------------*/
void ComputeEnclosing (int frame)
{
  int                 i;
  PtrDimRelations     pDimRel, prev;

  /* Apply all differed WidthPack and HeightPack  */
  PackBoxRoot = NULL;
  while (DifferedPackBlocks)
    {
      pDimRel = DifferedPackBlocks;
      prev = NULL;
      while (pDimRel->DimRNext)
        {
          /* next block */
          prev = pDimRel;
          pDimRel = pDimRel->DimRNext;
        }
      /* On traite toutes les boites enregistrees */
      i = MAX_RELAT_DIM - 1;
      while (i >= 0)
        {
          if (pDimRel->DimRTable[i] &&
              pDimRel->DimRTable[i]->BxAbstractBox)
            {
              if (pDimRel->DimROp[i] == OpSame)
                {
                  if (pDimRel->DimRTable[i]->BxType == BoBlock ||
                      pDimRel->DimRTable[i]->BxType == BoFloatBlock ||
                      pDimRel->DimRTable[i]->BxType == BoCellBlock)
                    RecomputeLines (pDimRel->DimRTable[i]->BxAbstractBox, NULL, NULL, frame);
                  /* make sure that the enclosing of this box is updated */
                  WidthPack (pDimRel->DimRTable[i]->BxAbstractBox, NULL, frame);
                }
              else if (pDimRel->DimROp[i] == OpReverse)
                HeightPack (pDimRel->DimRTable[i]->BxAbstractBox, NULL, frame);
            }
          /* next entry */
          i--;
        }
      FreeDimBlock (&pDimRel);
      if (prev)
        prev->DimRNext = NULL;
      else
        DifferedPackBlocks = NULL;
    }
}

/*----------------------------------------------------------------------
  RebuildConcreteImage reevaluates the document view after a change
  in the frame (size, zoom).
  Redisplay within the frame supposed clean.
  scrollUpdate is TRUE when scrollbars must be recomputed
  ----------------------------------------------------------------------*/
void RebuildConcreteImage (int frame, ThotBool scrollUpdate)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb, pVisibleAb;
  PtrBox              pBox;
  int                 width, height;
  int                 position = 0;
  ThotBool            condition;
  ThotBool            status;
  ThotBool            lock = TRUE;

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox)
    {
      /* close any text editing in progress */
      CloseTextInsertion ();
      /* Box widths may change and the scroll must be recomputed */
      AnyWidthUpdate = TRUE;
      pAb = pFrame->FrAbstractBox;
      if (pAb->AbBox && pAb->AbElement)
        {
          /* get the first visible abstract box and its current position */
          GetSizesFrame (frame, &width, &height);
          pBox = pAb->AbBox;
          while (pBox->BxYOrg < pFrame->FrYOrg && pBox->BxNext != NULL)
            pBox = pBox->BxNext;
          if (pBox)
            {
              /* position of the box top in the frame given in % */
              position = (pBox->BxYOrg - pFrame->FrYOrg) * 100 / height;
              pVisibleAb = pBox->BxAbstractBox;
            }
          else
            pVisibleAb = NULL;
       
          /* lock tables formatting */
          TtaGiveTableFormattingLock (&lock);
          if (!lock)
            /* table formatting is not loked, lock it now */
            TtaLockTableFormatting ();
 
          pBox = pAb->AbBox;
          status = pFrame->FrReady;
          pFrame->FrReady = FALSE;	/* lock the frame */
          if (!pBox->BxContentWidth
              || (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum))
            {
              /* the width of the document depends on the window */
              condition = ComputeDimRelation (pAb, frame, TRUE);
              if (!condition)
                {
                  GiveEnclosureSize (pAb, frame, &width, &height);
                  ChangeDefaultWidth (pBox, pBox, width, 0, frame);
                }
            }
          if (!pBox->BxContentHeight
              || (!pAb->AbHeight.DimIsPosition && pAb->AbHeight.DimMinimum))
            {
              /* the height of the document depends on the window */
              condition = ComputeDimRelation (pAb, frame, FALSE);
              if (!condition)
                {
                  GiveEnclosureSize (pAb, frame, &width, &height);
                  ChangeDefaultHeight (pBox, pBox, height, frame);
                }
            }
	   
          /* check constraints */
          ComputePosRelation (&pAb->AbHorizPos, pBox, frame, TRUE);
          ComputePosRelation (&pAb->AbVertPos, pBox, frame, FALSE);

          if (!lock)
            /* unlock table formatting */
            TtaUnlockTableFormatting ();
	   
          /* La frame est affichable */
          pFrame->FrReady = status;
          /* Traitement des englobements retardes */
          ComputeEnclosing (frame);

          /* redisplay the window */
          if (pVisibleAb)
            ShowBox (frame, pVisibleAb->AbBox, 0, position, FALSE);
          else
            RedrawFrameBottom (frame, 0, NULL);
          /* if necessary show the selection */
          ShowSelection (pFrame->FrAbstractBox, TRUE);
	   
          /* recompute scrolls */
          CheckScrollingWidthHeight (frame);
          if (scrollUpdate)
            UpdateScrollbars (frame);
        }
    }
}


/*----------------------------------------------------------------------
  ClearFlexibility remove the box stretching on the current box and
  its children.
  ----------------------------------------------------------------------*/
static void ClearFlexibility (PtrAbstractBox pAb, int frame)
{
  PtrAbstractBox      pChildAb;
  PtrBox              pBox;

  if (pAb->AbNew)
    return;
  else if (pAb->AbBox != NULL)
    {
      pBox = pAb->AbBox;
      /* Faut-il reevaluer les regles d'une boite elastique */
      if (pBox->BxHorizFlex && pAb->AbWidthChange)
        {
          /* redisplay the whole box */
          DefBoxRegion (frame, pBox, -1, -1, -1, -1);
          /* clear previous position */
          if (pAb->AbHorizPosChange)
            {
              ClearPosRelation (pBox, TRUE);
              XMove (pBox, NULL, -pBox->BxXOrg, frame);
            }

          /* clear previous width */
          ClearDimRelation (pBox, TRUE, frame);
          /* Reinitialisation of the shape */
          MirrorShape (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);
        }

      if (pBox->BxVertFlex && pAb->AbHeightChange)
        {
          if (!pBox->BxHorizFlex || !pAb->AbWidthChange)
            /* redisplay the whole box */
            DefBoxRegion (frame, pBox, -1, -1, -1, -1);
          /* clear previous position */
          if (pAb->AbVertPosChange)
            {
              ClearPosRelation (pBox, FALSE);
              YMove (pBox, NULL, -pBox->BxYOrg, frame);
            }

          /* clear previous height */
          ClearDimRelation (pBox, FALSE, frame);
          /* Reinitialisation of the shape */
          MirrorShape (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);
        }

      /* Traitement des paves fils */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb != NULL)
        {
          ClearFlexibility (pChildAb, frame);
          pChildAb = pChildAb->AbNext;
        }
    }
}

/*----------------------------------------------------------------------
  IsDead returns TRUE is the abstract box or a parent abstract box
  has AbDead = TRUE.
  ----------------------------------------------------------------------*/
ThotBool IsDead (PtrAbstractBox pAb)
{
  while (pAb)
    {
      if (pAb->AbDead)
        return TRUE;
      else
        pAb = pAb->AbEnclosing;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  ClearConcreteImage removes all concrete boxes in the view.
  ----------------------------------------------------------------------*/
void ClearConcreteImage (int frame)
{
  ViewFrame          *pFrame;
  PtrFlow             pFlow;

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox != NULL)
    {
      pFrame->FrReady = FALSE;	/* La frame n'est pas affichable */
      /* Faut-il retirer les marques de selection dans la fenetre */
      CloseTextInsertion ();
      /* Liberation de la hierarchie */
      RemoveBoxes (pFrame->FrAbstractBox, FALSE, frame);
      pFrame->FrAbstractBox = NULL;
      /* free all extra flows */
      while (pFrame->FrFlow)
        {
          pFlow = pFrame->FrFlow;
          pFrame->FrFlow = pFlow->FlNext;
          TtaFreeMemory (pFlow);
        }
      pFrame->FrReady = TRUE;	/* the window is now ready */
      DefClip (frame, -1, -1, -1, -1);	/* effacer effectivement */
      DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
                      &pFrame->FrClipXBegin, &pFrame->FrClipYBegin,
                      &pFrame->FrClipXEnd, &pFrame->FrClipYEnd, 1);
    }
}


/*----------------------------------------------------------------------
  IsAbstractBoxUpdated checks the list of updates.
  Return TRUE if there is at least one change.
  The parameter computeBBoxes is set TRUE if this bounding boxe should
  be recomputed else it's unchanged.
  ----------------------------------------------------------------------*/
static ThotBool IsAbstractBoxUpdated (PtrAbstractBox pAb, int frame,
                                      ThotBool *computeBBoxes)
{
  PtrAbstractBox      pChildAb, pFirstAb;
  PtrAbstractBox      pNewAb;
  PtrLine             pLine;
  PtrBox              pBox;
  PtrBox              pCurrentBox;
  Propagation         propStatus;
  int                 index;
  ThotBool            toUpdate;
  ThotBool            result, change;

  change = FALSE;
  /* avoid to manage two times the same box update */
  for (index = 0; index < BiwIndex; index++)
    if (BoxInWork[index] == pAb)
      break;
  if (index < BiwIndex)
    result = FALSE;
  else
    {
      if (pAb->AbDead && (pAb->AbNew || pAb->AbBox == NULL))
        result = FALSE;
      else if (pAb->AbDead || pAb->AbNew)
        {
          /* work in progress in this new abstract box (index = BiwIndex) */
          BoxInWork[BiwIndex++] = pAb;
          result = ComputeUpdates (pAb, frame, computeBBoxes);
          /* work is done */
          BoxInWork[BiwIndex--] = NULL;
          propStatus = Propagate;
          Propagate = ToAll;	/* standard propagation */
          CheckDefaultPositions (pAb, frame);
          Propagate = propStatus;
        }
      else
        {
          /* Manage children updtates before current updates */
          /* On limite l'englobement a la boite courante */
          pBox = PackBoxRoot;
          PackBoxRoot = pAb->AbBox;
	   
          /* manage creations */
          pChildAb = pAb->AbFirstEnclosed;
          pNewAb = NULL;
          toUpdate = FALSE;
          while (pChildAb)
            {
              if (pChildAb->AbNew)
                {
                  /* On marque le pave qui vient d'etre cree */
                  /* pour eviter sa reinterpretation         */
                  pChildAb->AbNum = 1;
                  change = IsAbstractBoxUpdated (pChildAb, frame, computeBBoxes);
                  /* Une modification a repercurter sur l'englobante */
                  if (change && !toUpdate)
                    {
                      toUpdate = TRUE;
                      pNewAb = pChildAb;
                    }
                }
              else
                pChildAb->AbNum = 0;
              pChildAb = pChildAb->AbNext;
            }
	   
          /* Traitement des autres modifications */
          pChildAb = pAb->AbFirstEnclosed;
          pFirstAb = NULL;
          while (pChildAb != NULL)
            {
              /* On evite la reinterpretation des paves crees */
              if (pChildAb->AbNum == 0)
                change = IsAbstractBoxUpdated (pChildAb, frame, computeBBoxes);

              /* On enregistre le premier pave cree ou modifie */
              if (pNewAb == pChildAb && pFirstAb == NULL)
                pFirstAb = pNewAb;
              else if (change)
                {
                  /* Une modification a repercuter sur l'englobante */
                  toUpdate = TRUE;
                  if (pFirstAb == NULL)
                    pFirstAb = pChildAb;
                }

              pChildAb = pChildAb->AbNext;
            }
          /* Les liens entre boites filles ont peut-etre ete modifies */
          if (toUpdate)
            {
              pChildAb = pAb->AbFirstEnclosed;
              while (pChildAb != NULL)
                {
                  if (pChildAb->AbBox != NULL)
                    {
                      pCurrentBox = pChildAb->AbBox;
                      pCurrentBox->BxHorizInc = NULL;
                      pCurrentBox->BxVertInc = NULL;
                    }

                  pChildAb = pChildAb->AbNext;
                }
            }

          /* On restaure l'ancienne limite de l'englobement */
          PackBoxRoot = pBox;

          /* Faut-il reevaluer la boite composee ? */
          /* work in progress in this new abstract box (index = BiwIndex) */
          BoxInWork[BiwIndex++] = pAb;
          result = ComputeUpdates (pAb, frame, computeBBoxes);
          /* work is done */
          BoxInWork[BiwIndex--] = NULL;
          if (toUpdate || result)
            {
              pCurrentBox = pAb->AbBox;
              result = TRUE;
              /* Mise a jour d'un bloc de lignes */
              if (pFirstAb &&
                  (pCurrentBox->BxType == BoBlock ||
                   pCurrentBox->BxType == BoFloatBlock ||
                   pCurrentBox->BxType == BoCellBlock))
                {
                  if (pFirstAb == pNewAb || pFirstAb->AbDead)
                    {
                      /* On prend la derniere boite avant */
                      pFirstAb = pFirstAb->AbPrevious;
                      if (pFirstAb == NULL)
                        pLine = NULL;		/* toutes les lignes */
                      else
                        {
                          pBox = pFirstAb->AbBox;
                          if (pBox)
                            {
                              /* S'il s'agit d'une boite de texte coupee */
                              /* on recherche la derniere boite de texte */
                              if (pBox->BxType == BoSplit ||
                                  pBox->BxType == BoMulScript)
                                while (pBox->BxNexChild)
                                  pBox = pBox->BxNexChild;
                              pLine = SearchLine (pBox, frame);
                            }
                          else
                            pLine = NULL;
                        }
                    }
                  else
                    {
                      /* On prend la derniere boite avant */
                      pBox = pFirstAb->AbBox;
                      if (pBox != NULL)
                        {
                          if ((pBox->BxType == BoSplit || pBox->BxType == BoMulScript) &&
                              pBox->BxNexChild)
                            pBox = pBox->BxNexChild;
                          pLine = SearchLine (pBox, frame);
                        }
                      else
                        pLine = NULL;
                    }
		   
                  RecomputeLines (pAb, pLine, pAb->AbBox, frame);
                }
              /* Mise a jour d'une boite composee */
              else if (pAb->AbBox->BxType != BoBlock &&
                       pAb->AbBox->BxType != BoFloatBlock &&
                       pAb->AbBox->BxType != BoCellBlock &&
                       pAb->AbBox->BxType != BoGhost &&
                       pAb->AbBox->BxType != BoStructGhost &&
                       pAb->AbBox->BxType != BoFloatGhost &&
                       pAb->AbBox->BxType != BoCell &&
                       pAb->AbLeafType == LtCompound)
                {
                  if (Propagate == ToAll)
                    {
                      WidthPack (pAb, pAb->AbBox, frame);
                      HeightPack (pAb, pAb->AbBox, frame);
                    }
                  else
                    {
                      RecordEnclosing (pAb->AbBox, TRUE);
                      RecordEnclosing (pAb->AbBox, FALSE);
                    }
                }
            }
        }
    }
  /* On signale s'il y a eu modification du pave */
  return result;
}


/*----------------------------------------------------------------------
  CheckScrollingWidthHeight computes the maximum between the document width
  and the width needed to display all its contents.
  Check also the height of positioned boxes
  ----------------------------------------------------------------------*/
void CheckScrollingWidthHeight (int frame)
{
  PtrAbstractBox      pAb;
  PtrBox              pBox, box;
  ViewFrame          *pFrame;
  int                 xmax, xorg, ymax;
  int                 w, h, newmax;

  if  (AnyWidthUpdate)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pFrame->FrAbstractBox;
      GetSizesFrame (frame, &w, &h);
      ymax = 0;
      if (pAb && pAb->AbBox)
        {
          /* check if the document inherits its contents */
          pBox = pAb->AbBox;
          xmax = pBox->BxWidth;
          xorg = pBox->BxXOrg;
          if (!pBox->BxContentWidth)
            {
              /* take the first leaf box */
              pBox = pBox->BxNext;
              box = pBox;
              while (pBox)
                {
                  /* check if this box is displayed outside the document box */
#ifdef _GL
                  if (pBox->BxBoundinBoxComputed)
                    newmax = pBox->BxClipX + pBox->BxClipW;
                  else
#endif /*  _GL */
                    newmax = pBox->BxXOrg + pBox->BxWidth;
                  if (pBox->BxAbstractBox->AbFloat == 'R' && pBox->BxRMargin < 0)
                    newmax += pBox->BxRMargin;
                  if (newmax > xmax &&
                      (pBox->BxType != BoPiece || pBox->BxNChars > 0))
                    {
                      /* ignoge boxes with absolute or fixed positions */
                      pAb = GetEnclosingViewport (pBox->BxAbstractBox);
                      if (pAb == NULL ||
                          (pAb->AbPositioning &&
                           pAb->AbPositioning->PnAlgorithm == PnRelative))
                        {
                          if (pBox->BxAbstractBox &&
                              pBox->BxAbstractBox->AbLeafType == LtText)
                            {
                              pAb = pBox->BxAbstractBox->AbEnclosing;
                              while (pAb && pAb->AbBox &&
                                     (pAb->AbBox->BxType == BoGhost ||
                                      pAb->AbBox->BxType == BoStructGhost ||
                                      pAb->AbBox->BxType == BoFloatGhost))
                                pAb = pAb->AbEnclosing;
                              if (pAb && pAb->AbBox)
                                {
#ifdef _GL
                                if (pAb->AbBox->BxBoundinBoxComputed)
                                  newmax = pAb->AbBox->BxClipX + pAb->AbBox->BxClipW;
                                else
#endif /*  _GL */
                                  newmax = pAb->AbBox->BxXOrg + pAb->AbBox->BxWidth;
                                if (pAb->AbFloat == 'R' && pAb->AbBox->BxRMargin < 0)
                                  newmax += pAb->AbBox->BxRMargin;
                               }
                              if (xmax < newmax)
                                xmax = newmax;
                            }
                          else
                            xmax = newmax;
                          xmax += 4;
                        }
                    }
                  if (pBox->BxNext == box)
                    {
                      printf ("Cycle\n");
                      pBox = NULL;
                    }
                  else if (pBox->BxNext == pBox)
                    pBox = NULL;
                  else
                    pBox = pBox->BxNext;
                }
            }
          FrameTable[frame].FrScrollOrg = xorg;
          FrameTable[frame].FrScrollWidth = xmax - xorg;
          pBox = pFrame->FrAbstractBox->AbBox;
          if (ymax > pBox->BxYOrg + pBox->BxHeight)
            // force the height of the root element
            pBox->BxHeight = ymax - pBox->BxYOrg;
        }
      else
        {
          FrameTable[frame].FrScrollOrg = 0;
          FrameTable[frame].FrScrollWidth = w;
        }
      AnyWidthUpdate = FALSE;
    }
}

/*----------------------------------------------------------------------
  AnyFloatPosChange
  ----------------------------------------------------------------------*/
static ThotBool AnyFloatPosChange (PtrAbstractBox pAb)
{
  ThotBool     found = FALSE;

  if (pAb && !pAb->AbDead)
    {
      found = (pAb->AbFloatChange || pAb->AbPositionChange ||
               (pAb->AbNew && pAb->AbFloat != 'N'));
      if (!found)
        found = AnyFloatPosChange (pAb->AbFirstEnclosed);
      if (!found)
        found = AnyFloatPosChange (pAb->AbNext);
    }
  return found;
}

/*----------------------------------------------------------------------
  NewConcreteImage
  ----------------------------------------------------------------------*/
static void NewConcreteImage (int frame, Document doc)
{
  PtrAbstractBox      pAb, pv1, pvN;
  ViewSelection      *pViewSel;
  ViewFrame          *pFrame;
  DisplayMode         saveMode;
  int                 x, y;
  int                 c1;
  int                 cN;
  int                 h;
  ThotBool            caret;
  ThotBool            lock = TRUE;


  pFrame = &ViewFrameTable[frame - 1];
  c1 = 0;
  cN = 0;
  if (pFrame->FrAbstractBox)
    /* On sauvegarde la selection courante dans la fenetre */
    {
      /* lock the table formatting */
      TtaGiveTableFormattingLock (&lock);
      if (!lock)
        /* table formatting is not locked, lock it now */
        TtaLockTableFormatting ();

      saveMode = documentDisplayMode[doc - 1];
      if (saveMode == DisplayImmediately)
        documentDisplayMode[doc - 1] = DeferredDisplay;

      pViewSel = &pFrame->FrSelectionBegin;
      if (pViewSel->VsBox)
        {
          pv1 = pViewSel->VsBox->BxAbstractBox;
          if (pViewSel->VsIndBox == 0)
            c1 = 0;
          else
            c1 = pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox;
          /* On annule le debut de selection */
          pViewSel->VsBox = NULL;
        }
      else
        pv1 = NULL;
      pViewSel = &pFrame->FrSelectionEnd;
      if (pViewSel->VsBox != NULL)
        {
          pvN = pViewSel->VsBox->BxAbstractBox;
          if (pViewSel->VsIndBox == 0)
            cN = 0;
          else
            cN = pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox;
          /* On annule la fin de selection */
          pViewSel->VsBox = NULL;
        }
      else
        pvN = NULL;
      caret = pFrame->FrSelectOneBox;

      /* On libere de la hierarchie avant recreation */
      pAb = pFrame->FrAbstractBox;
      /* On sauvegarde la position de la fenetre dans le document */
      x = pFrame->FrXOrg;
      y = pFrame->FrYOrg;
      pAb->AbDead = TRUE;
      RemoveBoxes (pAb, TRUE, frame);

      /* On libere les polices de caracteres utilisees */
      pFrame->FrAbstractBox = NULL;
      /* Recreation de la vue */
      pAb->AbDead = FALSE;
      pAb->AbNew = TRUE;
      h = 0;
      (void) ChangeConcreteImage (frame, &h, pAb);
      /* On restaure la position de la fenetre dans le document */
      pFrame->FrXOrg = x;
      pFrame->FrYOrg = y;
      /* On restaure la selection courante dans la fenetre */
      if (caret)
        InsertViewSelMarks (frame, pv1, c1, cN, TRUE, TRUE, caret);
      /* La selection porte sur plusieurs paves */
      else
        {
          if (pv1 != NULL)
            InsertViewSelMarks (frame, pv1, c1, 0, TRUE, FALSE, FALSE);
          if (pvN != NULL)
            InsertViewSelMarks (frame, pvN, 0, cN, FALSE, TRUE, FALSE);
        }

      /* restore the current mode and  update tables if necessary */
      if (saveMode == DisplayImmediately)
        documentDisplayMode[doc - 1] = saveMode;

      if (!lock)
        /* unlock table formatting */
        TtaUnlockTableFormatting ();
      /* Est-ce que l'on a de nouvelles boites dont le contenu est */
      /* englobe et depend de relations hors-structure ?           */
      ComputeEnclosing (frame);
    }
}


/*----------------------------------------------------------------------
  ChangeConcreteImage traite la mise a jour d'une hierachie de paves 
  Pendant la creation d'une arborescence de boites on     
  place chaque boite a` l'interieur de son englobante     
  (Propagate=ToSiblings).                                   
  Quand tous les placements relatifs sont termine's on    
  place les boites dans la vue entiere du document en     
  cumulant les placements relatifs.                       
  Les autre modifications vont provoquer des mises a` jour
  sur la descendance (Propagate = ToChildren) puis en fin de
  traitement seulement, la mise a` jour des boites        
  englobantes.                                            
  En fonctionnement normal les modifications sont         
  immediatement propagees sur toutes les boites           
  (Propagate = ToAll).                                      
  Si le parametre pageHeight est non nul, il indique la hauteur 
  maximum d'une page (ce parametre est eventuellement mis 
  a` jour au retour).                                     
  La fonction ve'rifie que cette limite est respecte'e.   
  Elle rend la valeur Vrai si l'image n'est pas coupee.   
  Si le retour est Faux, tous les paves dont la boite     
  coupe la limite ou de'borde sont marque's.              
  ----------------------------------------------------------------------*/
ThotBool ChangeConcreteImage (int frame, int *pageHeight, PtrAbstractBox pAb)
{
  PtrAbstractBox      pParentAb;
  PtrAbstractBox      pChildAb;
  ViewFrame          *pFrame;
  PtrLine             pLine;
  PtrBox              pBox;
  PtrBox              pParentBox;
  PtrBox              pChildBox;
  DisplayMode         saveMode;
  int                 doc;
  int                 savevisu = 0;
  int                 savezoom = 0;
  ThotBool            change, computeBBoxes = FALSE;
  ThotBool            result = TRUE;
  ThotBool            lock = TRUE;

#ifdef _WINGUI
  WIN_GetDeviceContext (frame);
#endif /* _WINGUI */
  doc = FrameTable[frame].FrDoc;
  if (doc == 0)
    return result;

  pLine = NULL;
  if (pAb && frame >= 1 && frame <= MAX_FRAME)
    {
      while (pAb->AbEnclosing && pAb->AbEnclosing->AbNew)
        {
#ifdef THOT_DEBUG
          TtaDisplaySimpleMessage (INFO, LIB, TMSG_VIEW_MODIFIED_BEFORE_CREATION);
#endif /* THOT_DEBUG */
          pAb = pAb->AbEnclosing;
        }
      pFrame = &ViewFrameTable[frame - 1];
      if (pFrame->FrAbstractBox == NULL &&
          (pAb->AbEnclosing || pAb->AbPrevious || pAb->AbNext))
        {
          /* The view has another root element */
#ifdef THOT_DEBUG
          TtaDisplaySimpleMessage (INFO, LIB, TMSG_VIEW_MODIFIED_BEFORE_CREATION);
#endif /* THOT_DEBUG */
        }
      else if (pAb->AbEnclosing == NULL && pAb->AbDead)
        {
          /* Remove the view content */
          if (pAb == pFrame->FrAbstractBox)
            ClearConcreteImage (frame);
          else
            {
#ifdef THOT_DEBUG
            TtaDisplaySimpleMessage (INFO, LIB, TMSG_VIEW_MODIFIED_BEFORE_CREATION);
#endif /* THOT_DEBUG */
            }
        }
      else if (pFrame->FrAbstractBox && pAb->AbEnclosing == NULL && pAb->AbNew)
        {
          /* The view is already created */
#ifdef THOT_DEBUG
          TtaDisplaySimpleMessage (INFO, LIB, TMSG_OLD_VIEW_NOT_REPLACED);
#endif /* THOT_DEBUG */
        }
      else if (documentDisplayMode[doc - 1] != NoComputedDisplay)
        {
          /* Other cases without NoComputedDisplay mode */
          if (pFrame->FrAbstractBox == NULL)
            {
              /* first creation */
              DefClip (frame, 0, 0, 0, 0);
              pFrame->FrXOrg = 0;
              pFrame->FrYOrg = 0;
              pFrame->FrAbstractBox = pAb;
              pFrame->FrSelectOneBox = FALSE;
              pFrame->FrSelectOnePosition = FALSE;
              pFrame->FrSelectionBegin.VsBox = NULL;
              pFrame->FrSelectionBegin.VsLine = NULL;
              pFrame->FrSelectionBegin.VsBuffer = NULL;
              pFrame->FrSelectionEnd.VsBox = NULL;
              pFrame->FrSelectionEnd.VsLine = NULL;
              pFrame->FrSelectionEnd.VsBuffer = NULL;
              pFrame->FrSelectShown = FALSE;
              pFrame->FrReady = TRUE;
              pFrame->FrFlow = NULL;
            }

          /* rebuild the whole image if there is a float or positioning change */
          if ((pAb != pFrame->FrAbstractBox || !pAb->AbNew) && AnyFloatPosChange (pAb))
            {
              NewConcreteImage (frame, doc);
              DefClip (frame, -1, -1, -1, -1);
            }
          else
            {
              saveMode = documentDisplayMode[doc - 1];
              if (saveMode == DisplayImmediately)
                documentDisplayMode[doc - 1] = DeferredDisplay;

              /* On prepare le traitement de l'englobement apres modification */
              pFrame->FrReady = FALSE;	/* La frame n'est pas affichable */
              if (*pageHeight != 0)
                /* changement de la signification de la valeur de page */
                /* si negatif : mode pagination sans evaluation de coupure */
                /* si egal 0 : pas mode pagination */
                /* si superieur a 0 : mode pagination et coupure demandee */
                {
                  /* La pagination ignore le zoom et la visibilite courants */
                  savevisu = pFrame->FrVisibility;
                  pFrame->FrVisibility = 5;
                  savezoom = pFrame->FrMagnification;
                  pFrame->FrMagnification = 0;
                }
              Propagate = ToChildren;	/* Limite la propagation */
              /* On note le premier pave a examiner pour l'englobante */
              pParentAb = pAb->AbEnclosing;
              PackBoxRoot = NULL;
              /* On prepare la mise a jour d'un bloc de lignes */
              if (pParentAb && pParentAb->AbBox)
                {
                  /* differ enclosing rules for ancestor boxes */
                  PackBoxRoot = pParentAb->AbBox;
                  while (pParentAb->AbBox->BxType == BoGhost ||
                         pParentAb->AbBox->BxType == BoStructGhost ||
                         pParentAb->AbBox->BxType == BoFloatGhost)
                    /* get the enclosing block */
                    pParentAb = pParentAb->AbEnclosing;
                  
                  /* On prepare la mise a jour d'un bloc de lignes */
                  if (pParentAb->AbBox->BxType == BoBlock ||
                      pParentAb->AbBox->BxType == BoFloatBlock ||
                      pParentAb->AbBox->BxType == BoCellBlock ||
                      pParentAb->AbBox->BxType == BoGhost ||
                      pParentAb->AbBox->BxType == BoStructGhost ||
                      pParentAb->AbBox->BxType == BoFloatGhost)
                    {
                      if (pAb->AbNew || pAb->AbDead)
                        {
                          /* need to rebuild lines starting form
                             the previous box */
                          pChildAb = pAb->AbPrevious;
                          if (pChildAb == NULL || pChildAb->AbBox == NULL)
                            /* all lines */
                            pLine = NULL;
                          else
                            {
                              pBox = pChildAb->AbBox;
                              if (pBox->BxType == BoSplit ||
                                  pBox->BxType == BoMulScript)
                                while (pBox->BxNexChild)
                                  pBox = pBox->BxNexChild;
                              pLine = SearchLine (pBox, frame);
                            }
                        }
                      else
                        {
                          /* Il faut refaire la mise en lignes sur la premiere boite contenue */
                          pBox = pAb->AbBox;
                          if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                            pBox = pBox->BxNexChild;
                          pLine = SearchLine (pBox, frame);
                        }
                    }
                }

              /* lock the table formatting */
              TtaGiveTableFormattingLock (&lock);
              if (!lock)
                /* table formatting is not locked, lock it now */
                TtaLockTableFormatting ();

              /* Il faut annuler l'elasticite des boites dont les regles */
              /* de position et de dimension sont a reevaluer, sinon on  */
              /* risque d'alterer les nouvelles regles de position et de */
              /* dimension en propageant les autres modifications sur    */
              /* ces anciennes boites elastiques de l'image abstraite    */
              ClearFlexibility (pAb, frame);

              /* Manage all changes */
              change = IsAbstractBoxUpdated (pAb, frame, &computeBBoxes);
              if (computeBBoxes)
                ComputeChangedBoundingBoxes (frame);
              /* Les modifications sont traitees */
              Propagate = ToAll;	/* On passe en mode normal de propagation */
              /* Traitement des englobements retardes */
              ComputeEnclosing (frame);
              
              /* On ne limite plus le traitement de l'englobement */
              PackBoxRoot = NULL;
              
              /* Si l'image concrete est videe de son contenu */
              if (pFrame->FrAbstractBox->AbBox->BxNext == NULL)
                {
                  /* On annule le decalage de la fenetre dans l'image concrete */
                  pFrame->FrXOrg = 0;
                  pFrame->FrYOrg = 0;
                }

              /* Faut-il verifier l'englobement ? */
              if (change && pParentAb)
                {
                  /* On saute les boites fantomes de la mise en lignes */
                  if (pParentAb->AbBox->BxType == BoGhost ||
                      pParentAb->AbBox->BxType == BoStructGhost ||
                      pParentAb->AbBox->BxType == BoFloatGhost)
                    {
                      while (pParentAb->AbBox->BxType == BoGhost ||
                             pParentAb->AbBox->BxType == BoStructGhost ||
                             pParentAb->AbBox->BxType == BoFloatGhost)
                        pParentAb = pParentAb->AbEnclosing;
                      pLine = pParentAb->AbBox->BxFirstLine;
                    }

                  pParentBox = pParentAb->AbBox;
                  /* Mise a jour d'un bloc de lignes */
                  if (pParentBox->BxType == BoBlock ||
                      pParentBox->BxType == BoFloatBlock ||
                      pParentBox->BxType == BoCellBlock)
                    RecomputeLines (pParentAb, pLine, NULL, frame);

                  /* Mise a jour d'une boite composee */
                  else
                    {
                      /* Les liens entre boites filles ont peut-etre ete modifies */
                      pChildAb = pAb->AbFirstEnclosed;
                      while (pChildAb != NULL)
                        {
                          if (pChildAb->AbBox != NULL)
                            {
                              pChildBox = pChildAb->AbBox;
                              pChildBox->BxHorizInc = NULL;
                              pChildBox->BxVertInc = NULL;
                            }
                          pChildAb = pChildAb->AbNext;
                        }
                      if (pParentAb->AbBox && pParentAb->AbBox->BxType == BoCell)
                        UpdateColumnWidth (pParentAb, NULL, frame);
                      else if (pParentAb->AbBox && pParentAb->AbBox->BxType != BoRow)
                        WidthPack (pParentAb, NULL, frame);
                      HeightPack (pParentAb, NULL, frame);
                    }
                }

              /* restore the current mode and  update tables if necessary */
              if (saveMode == DisplayImmediately)
                documentDisplayMode[doc - 1] = saveMode;

              if (!lock)
                /* unlock table formatting */
                TtaUnlockTableFormatting ();
              /* Est-ce que l'on a de nouvelles boites dont le contenu est */
              /* englobe et depend de relations hors-structure ?           */
              ComputeEnclosing (frame);
            }

          /* Verification de la mise en page */
          if (*pageHeight > 0)
            /* changement de la signification de page */
            /* si superieur a 0 : mode pagination et coupure demandee */
            result = SetPageBreakPosition (pFrame->FrAbstractBox, pageHeight);
          if (*pageHeight != 0)
            {
              /* retablit le zoom et la visibilite courants */
              pFrame->FrVisibility = savevisu;
              pFrame->FrMagnification = savezoom;
            }
          pFrame->FrReady = TRUE;	/* La frame est affichable */
        }
    }
#ifdef _WINGUI
  WIN_ReleaseDeviceContext ();
#endif /* _WINGUI */
  return result;
}
