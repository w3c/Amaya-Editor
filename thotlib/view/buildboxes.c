/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* 
 * This module computes Concrete Images.
 *
 * Author: I. Vatton (INRIA)
 */

#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "language.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#define THOT_EXPORT
#include "boxes_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#ifdef _GL
#include "animbox_f.h"
#include <GL/gl.h>
#endif /* _GL */
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
#include "scroll_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"

#define		_2xPI		6.2832
#define		_1dSQR2		0.7071
#define		_SQR2		1.4142

#ifdef _WINDOWS
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#endif /* _WINDOWS */

#include "stix.h"

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
  GetParentCell returns the enlcosing cell or NULL.                
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
C_points *ComputeControlPoints (PtrTextBuffer buffer, int nb, int zoom)
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
   x1 = (float) PixelValue (pBuffer->BuPoints[j].XCoord, UnPixel, NULL,
			    zoom);
   y1 = (float) PixelValue (pBuffer->BuPoints[j].YCoord, UnPixel, NULL,
			    zoom);
   j++;
   x2 = (float) PixelValue (pBuffer->BuPoints[j].XCoord, UnPixel, NULL,
			    zoom);
   y2 = (float) PixelValue (pBuffer->BuPoints[j].YCoord, UnPixel, NULL,
			    zoom);
   if (nb < 3)
     {
	/* cas particulier des courbes avec 2 points */
	x3 = x2;
	y3 = y2;
     }
   else
     {
	j++;
	x3 = (float) PixelValue (pBuffer->BuPoints[j].XCoord, UnPixel,
				 NULL, zoom);
	y3 = (float) PixelValue (pBuffer->BuPoints[j].YCoord, UnPixel,
				 NULL, zoom);
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
	     x3 = (float) PixelValue (pBuffer->BuPoints[j].XCoord,
				      UnPixel, NULL, zoom);
	     y3 = (float) PixelValue (pBuffer->BuPoints[j].YCoord,
				      UnPixel, NULL, zoom);
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
  Returns:
  The script of the box.
  pBuffer points to the next buffer.
  ind returns the index of the next character in the buffer.
  nChars returns the number of characters not handled.
  width returns the width of the text.
  ----------------------------------------------------------------------*/
char GiveTextParams (PtrTextBuffer *pBuffer, int *ind, int *nChars,
		     SpecFont font, int *width, int *nSpaces,
		     char dir, char bidi)
{
  char                script, embed;
#ifdef _I18N_
  char                newscript, prevscript = '*';
  int                 oldind = 0;
  int                 oldpos = 0;
  int                 oldspaces = 0;
  int                 oldwidth = 0;
  PtrTextBuffer       oldbuff = NULL;
#endif /* _I18N_ */
  CHAR_T              car;
  int                 pos, max;
  int                 charWidth;
  int                 spaceWidth;

  /* space width */
  if (*nSpaces == 0)
    spaceWidth = BoxCharacterWidth (SPACE, font);
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
  embed = '*';
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
#ifdef _I18N_
      if (bidi != 'O')
	{
	  /* the text must be analysed and may be split */
	  newscript = TtaGetCharacterScript (car);
	  if (newscript == ' ' || newscript == 'D')
	    {
	      if (newscript == prevscript && newscript == 'D')
		/* several digits are considered as Latin string */
		newscript = 'L';
	      prevscript = newscript;
	      if (oldbuff == NULL && pos)
		{
		  /* keep in memory a possible splitting position */
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
	      prevscript = newscript;
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
		    *nChars = max - pos;
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
		    *nChars = max - pos;
		}
	      else
		/* split here */
		*nChars = max - pos;
	      return script;
	    }
	}
#endif /* _I18N_ */
      if (car == SPACE)
	{
	  (*nSpaces)++;	/* a space */
	  charWidth = spaceWidth;
	}
      else
	charWidth = BoxCharacterWidth (car, font);
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
  PictInfo           *picture;
  PtrBox              box;

  box = pAb->AbBox;
  picture = (PictInfo *) box->BxPictInfo;
  if (pAb->AbVolume == 0 || picture == NULL)
    {
      *width = 0;
      *height = 0;
    }
  else
    {
      *width = PixelValue (picture->PicWidth, UnPixel, pAb, zoom);
      *height = PixelValue (picture->PicHeight, UnPixel, pAb, zoom);
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
  PtrFont             pfont = NULL;

  box = pAb->AbBox;
  font = box->BxFont;
  hfont = BoxFontHeight (font);
#ifdef _I18N_
  GetMathFontFromChar (pAb->AbShape, font, (void **) &pfont,
		       font->FontSize);
  if (pfont)
    {
      GiveStixSize (pfont, pAb, 
		    width, height, font->FontSize);
      
	return;      
    }
#endif /* _I18N_ */
  if (pAb->AbVolume == 0)
    {
      /* empty Symbol */
      *width = BoxCharacterWidth (SPACE, font);
      *height = hfont * 2;
    }
  else
    {
      *height = hfont * 2;
      switch (pAb->AbShape)
	{
	case 'c':	/*integrale curviligne */
	case 'i':	/*integrale */
	  *width = BoxCharacterWidth (243, font);
	  *height += hfont + hfont;
	  break;
	case 'd':	/*integrale double */
	  *width = BoxCharacterWidth (243, font) +
	           BoxCharacterWidth (243, font) / 2;
	  *height += hfont;
	  break;
	case 'r':	/* root */
	  *width = hfont;
	  break;
	case 'S':	/*sigma */
	case 'P':	/*pi */
	case 'I':	/*intersection */
	case 'U':	/*union */
	  *width = BoxCharacterWidth (229, font);
	  *height = hfont;
	  break;
	case 'L':       /* left arrow */
	case 'h':       /* horizontal bar */
	case 'R':       /* right arrow */
	  *width = *height;
	  *height = hfont / 3;
	  break;
	case 'o':       /* overbrace */
	case 'u':       /* underbrace */
	  *width = *height;
	  *height = hfont / 2;
	  break;
	case '(':
	case ')':
	case '[':
	case ']':
	case '{':
	case '}':
	  *width = BoxCharacterWidth (230, font);
	  *height = hfont;
	  break;
	case '<':
	case '>':
	  *width = BoxCharacterWidth (241, font);
	  *height = hfont;
	  break;
	case '|':       /* vertical bar */
	  *width = BoxCharacterWidth (124, font);  /* | */
	  *height = hfont;	  
	  break;
        case 'D':       /* double vertical bar */
	  *width = 3 * BoxCharacterWidth (124, font);  /* | */
	  *height = hfont;	  
	  break;
	case '^':
	case 'v':
	case 'V':
	  *width = BoxCharacterWidth (109, font);	/* 'm' */
	  break;
	case '?':
	  *width = BoxCharacterWidth (63, font);   /* ? */
	  *height = hfont;
	  break;
	default:
	  *width = BoxCharacterWidth (SPACE, font);
	  *height = hfont;
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
  *width = BoxCharacterWidth (109, font);	/*'m' */
  hfont = BoxFontHeight (font);
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
    case 'C':
    case 'a':
    case 'c':
    case 'g':
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
  Return the last box generated.
  ----------------------------------------------------------------------*/
PtrBox SplitForScript (PtrBox box, PtrAbstractBox pAb, char script, int lg,
		       int width, int height, int spaces, int ind,
		       PtrTextBuffer pBuffer, PtrBox pMainBox)
{
  PtrBox              ibox1, ibox2;
  PtrBox              pPreviousBox, pNextBox;
  int                 l, v;

  pPreviousBox = box->BxPrevious;
  pNextBox = box->BxNext;
  if (box->BxType == BoComplete)
    {
      l = box->BxLMargin + box->BxLBorder + box->BxLPadding;
      v = box->BxTMargin + box->BxTBorder + box->BxTPadding + box->BxBMargin + box->BxBBorder + box->BxBPadding;
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
      ibox1->BxContentWidth = TRUE;
      ibox1->BxContentHeight = TRUE;
      ibox1->BxFont = box->BxFont;
      ibox1->BxUnderline = box->BxUnderline;
      ibox1->BxThickness = box->BxThickness;
      ibox1->BxHorizRef = box->BxHorizRef;
      ibox1->BxH = height;
      ibox1->BxHeight = height + v;
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
	  ibox1->BxXOrg = box->BxXOrg;
	  ibox2->BxXOrg = ibox1->BxXOrg + ibox1->BxWidth;
	}
      else
	{
	  ibox2->BxXOrg = box->BxXOrg;
	  ibox1->BxXOrg = ibox2->BxXOrg + ibox2->BxWidth;
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
      l = ibox1->BxLMargin + ibox1->BxLBorder + ibox1->BxLPadding;
      v = ibox1->BxTMargin + ibox1->BxTBorder + ibox1->BxTPadding + ibox1->BxBMargin + ibox1->BxBBorder + ibox1->BxBPadding;
      /* Initialize the second piece */
      ibox2 = GetBox (pAb);
      ibox2->BxType = BoScript;
      ibox2->BxScript = box->BxScript;
      ibox2->BxAbstractBox = pAb;
      ibox2->BxIndChar = ind;
      ibox2->BxContentWidth = TRUE;
      ibox2->BxContentHeight = TRUE;
      ibox2->BxFont = box->BxFont;
      ibox2->BxUnderline = box->BxUnderline;
      ibox2->BxThickness = box->BxThickness;
      ibox2->BxHorizRef = box->BxHorizRef;
      ibox2->BxH = height;
      ibox2->BxHeight = height + v;
      ibox2->BxW = box->BxW - width;
      ibox2->BxWidth = box->BxWidth - width - l;
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
	ibox2->BxXOrg = box->BxXOrg + box->BxWidth;
      else
	{
	  ibox2->BxXOrg = box->BxXOrg;
	  box->BxXOrg = ibox2->BxXOrg + ibox2->BxWidth;
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
  GiveTextSize gives the internal width and height of a text box.
  ----------------------------------------------------------------------*/
static void GiveTextSize (PtrAbstractBox pAb, PtrBox pMainBox, int *width,
			  int *height, int *nSpaces)
{
  PtrTextBuffer       pBuffer;
  SpecFont            font;
  PtrBox              box;
  char                script, dir;
  int                 ind, nChars;
  int                 l, pos;
  int                 lg, spaces, bwidth;

  box = pAb->AbBox;
  font = box->BxFont;
  *height = BoxFontHeight (font);
  /* Est-ce que le pave est vide ? */
  nChars = pAb->AbVolume;
  if (nChars == 0)
    {
      *width = 2;
      *nSpaces = 0;
    }
  else
    {
      /*
	There is a current text:
	generate one complete box or several boxes if several scripts are used
      */
      /* first character to be handled */
      pBuffer = pAb->AbText;
      ind = box->BxIndChar;
      *nSpaces = 0;
      *width = 0;
      pos = 1;
      dir = pAb->AbDirection;
      while (nChars > 0)
	{
	  bwidth = 0;
	  spaces = 0; /* format with the standard space width */
	  lg = nChars;
	  script = GiveTextParams (&pBuffer, &ind, &nChars, font, &bwidth, &spaces,
				   dir, pAb->AbUnicodeBidi);
	  box->BxScript = script;
	  *width += bwidth;
	  *nSpaces += spaces;
	  lg -= nChars;
	  if (nChars > 0)
	      box = SplitForScript (box, pAb, script, lg, bwidth, *height, spaces,
				    ind, pBuffer, pMainBox);
	  else if (box->BxType == BoScript)
	    {
	      box->BxW = bwidth;
	      l = box->BxLMargin + box->BxLBorder + box->BxLPadding;
	      box->BxWidth = bwidth + l;
	      box->BxNChars = lg;
	      box->BxNSpaces = spaces;
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
  PtrAbstractBox      pChildAb;
  PtrAbstractBox      pFirstAb;
  PtrAbstractBox      pCurrentAb;
  PtrBox              pChildBox, pBox;
  PtrBox              pCurrentBox;
  int                 val, x, y;
  ThotBool            still, hMin, vMin;

  pBox = NULL;
  pCurrentBox = pAb->AbBox;
  /* PcFirst fils vivant */
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
  
  if (pCurrentBox->BxType == BoGhost)
    {
      /* This box doesn't really exist */
      *width = 0;
      *height = 0;
      if (pAb->AbInLine)
	{
	  /* the block of line is ignored */
	  pCurrentBox->BxFirstLine = NULL;
	  pCurrentBox->BxLastLine = NULL;
	}
    }
  else if (pFirstAb == NULL)
    {
      /* The box is empty */
      *width = 0;
      *height = 0;
    }
  else if (pCurrentBox->BxType == BoBlock || pCurrentBox->BxType == BoFloatBlock)
    {
      /* It's a block of lines */
      if (pCurrentBox->BxFirstLine)
	/* we have to reformat the block of lines */
	RecomputeLines (pAb, NULL, NULL, frame);
      else
	{
	  pCurrentBox->BxFirstLine = NULL;
	  pCurrentBox->BxLastLine = NULL;
	  ComputeLines (pCurrentBox, frame, height);
	  if (pCurrentBox->BxContentWidth ||
	      (hMin && pCurrentBox->BxMaxWidth > pCurrentBox->BxW))
	    /* it's an extensible line */
	    pCurrentBox->BxW = pCurrentBox->BxMaxWidth;
	}
      *width = pCurrentBox->BxW;
    }
  else
    {
      /* It's a geometrical composition */
      
      /* Initially the inside left and the inside right are the equal */
      x = pCurrentBox->BxXOrg + pCurrentBox->BxLMargin +
	  pCurrentBox->BxLBorder + pCurrentBox->BxLPadding;
      *width = x;
      /* Initially the inside top and the inside bottom are the equal */
      y = pCurrentBox->BxYOrg + pCurrentBox->BxTMargin +
          pCurrentBox->BxTBorder + pCurrentBox->BxTPadding;
      *height = y;
      /* Move misplaced boxes */
      pChildAb = pFirstAb;
      while (pChildAb != NULL)
	{
	  pChildBox = pChildAb->AbBox;
	  if (!pChildAb->AbDead && pChildBox != NULL)
	    {
	      if ((hMin || pCurrentBox->BxContentWidth) &&
		  pChildAb->AbHorizEnclosing &&
		  pChildBox->BxXOrg < x &&
		  pChildAb->AbWidth.DimAbRef != pAb)
		{
		  /* the child box is misplaced */
		  pBox = GetHPosRelativeBox (pChildBox, NULL);
		  if (pBox != NULL)
		    {
		      /* mobile box */
		      pCurrentAb = pBox->BxAbstractBox;
		      if (pCurrentAb->AbHorizPos.PosAbRef == NULL)
			XMove (pChildBox, NULL, x - pChildBox->BxXOrg, frame);
		    }
		}
	      if ((vMin || pCurrentBox->BxContentHeight) &&
		  pChildAb->AbVertEnclosing &&
		  pChildBox->BxYOrg < y &&
		  pChildAb->AbHeight.DimAbRef != pAb)
		{
		  /* the child box is misplaced */
		  pBox = GetVPosRelativeBox (pChildBox, NULL);
		  if (pBox != NULL)
		    {
		      /* mobile box */
		      pCurrentAb = pBox->BxAbstractBox;
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
      while (pChildAb != NULL)
	{
	  pChildBox = pChildAb->AbBox;
	  if (!pChildAb->AbDead && pChildBox != NULL)
	    {
	      if ((pChildAb->AbWidth.DimAbRef != pAb &&
		   pChildAb->AbHorizEnclosing) ||
		  (!pChildAb->AbWidth.DimIsPosition &&
		   pChildAb->AbWidth.DimMinimum))
		{
		  /* the width of that box doesn't depend on the enclosing */
		  if (pChildBox->BxXOrg < 0)
		    val = pChildBox->BxWidth;
		  else
		    val = pChildBox->BxXOrg + pChildBox->BxWidth;
		  if (val > *width)
		    *width = val;
		}
	      if ((pChildAb->AbHeight.DimAbRef != pAb &&
		   pChildAb->AbVertEnclosing) ||
		  (!pChildAb->AbHeight.DimIsPosition &&
		   pChildAb->AbHeight.DimMinimum))
		{
		  /* the height of that box doesn't depend on the enclosing */
		  if (pChildBox->BxYOrg < 0)
		    val = pChildBox->BxHeight;
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
       * Now we move included boxes which depend on the parent width and height
       */
      pChildAb = pFirstAb;
      if (Propagate == ToSiblings)
	while (pChildAb != NULL)
	  {
	    pChildBox = pChildAb->AbBox;
	    if (!pChildAb->AbDead && pChildBox != NULL)
	      {
		if (pCurrentBox->BxContentWidth &&
		    !IsXPosComplete (pCurrentBox) &&
		    pChildAb->AbHorizPos.PosAbRef == pAb)
		  {
		    /* update the box position */
		    val = pChildBox->BxXOrg;
		    if (pChildAb->AbHorizPos.PosEdge == VertMiddle)
		      val += pChildBox->BxWidth / 2;
		    else if (pChildAb->AbHorizPos.PosEdge == Right)
		      val += pChildBox->BxWidth;
		    
		    if (pChildAb->AbHorizPos.PosRefEdge == VertMiddle)
		      {
			if (!pChildBox->BxHorizFlex)
			  XMove (pChildBox, NULL, x + *width / 2 - val, frame);
		      }
		    else if (pChildAb->AbHorizPos.PosRefEdge == Right)
		      {
			if (!pChildBox->BxHorizFlex)
			  XMove (pChildBox, NULL, x + *width - val, frame);
		      }
		  }
		
		if (pCurrentBox->BxContentHeight &&
		    !IsYPosComplete (pCurrentBox) &&
		    pChildAb->AbVertPos.PosAbRef == pAb)
		  {
		    /* update the box position */
		    val = pChildBox->BxYOrg;
		    if (pChildAb->AbVertPos.PosEdge == HorizMiddle)
		      val += pChildBox->BxHeight / 2;
		    else if (pChildAb->AbVertPos.PosEdge == Bottom)
		      val += pChildBox->BxHeight;
		    
		    if (pChildAb->AbVertPos.PosRefEdge == HorizMiddle)
		      {
			if (!pChildBox->BxVertFlex)
			  YMove (pChildBox, NULL, y + *height / 2 - val,frame);
		      }
		    else if (pChildAb->AbVertPos.PosRefEdge == Bottom)
		      {
			if (!pChildBox->BxVertFlex)
			  YMove (pChildBox, NULL, y + *height - val, frame);
		      }
		  }
	      }
	    pChildAb = pChildAb->AbNext;
	  }
    }
  
  if (pFirstAb == NULL && pAb->AbVolume == 0)
    {
      /* Empty box */
      if (*width == 0)
	*width = 2;
      if (*height == 0)
	*height = BoxFontHeight (pAb->AbBox->BxFont);
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
  TransmitFill transmits the background to children boxes.
  ----------------------------------------------------------------------*/
static void  TransmitFill (PtrBox pBox, ThotBool state)
{
  PtrAbstractBox  pAb;

  /* when it's a dummy box report changes to the children */
  pAb = pBox->BxAbstractBox;
  pAb = pAb->AbFirstEnclosed;
  while (pAb)
    {
      if (pAb->AbBox)
	{
	  if (pAb->AbBox->BxType == BoGhost)
	    TransmitFill (pAb->AbBox, state);
	  else
	    {
	      pAb->AbBox->BxFill = state;
	      pAb->AbBox->BxDisplay = state;
	    }
	}
      pAb = pAb->AbNext;
    }
}


/*----------------------------------------------------------------------
  TransmitMBP transmits margins, borders, and paddings to children boxes.
  i = the added pixels at the beginning
  j = the added pixels at the end
  ----------------------------------------------------------------------*/
static void TransmitMBP (PtrBox pBox, int frame, int i, int j,
			 ThotBool horizontal)
{
  PtrAbstractBox  pAb, pCurrentAb;

  /* when it's a dummy box report changes to the children */
  pCurrentAb = pBox->BxAbstractBox;
  pAb = pCurrentAb;
  if (horizontal)
    {
      while (pAb && pAb->AbBox && pAb->AbBox->BxType == BoGhost)
	pAb = pAb->AbFirstEnclosed;
      if (pAb && pAb->AbBox)
	{
	  /* the first child */
	  pAb->AbBox->BxLMargin = pBox->BxLMargin;
	  pAb->AbBox->BxLBorder = pBox->BxLBorder;
	  pAb->AbBox->BxLPadding = pBox->BxLPadding;
	  pAb->AbLeftStyle = pCurrentAb->AbLeftStyle;
	  pAb->AbLeftBColor = pCurrentAb->AbLeftBColor;
	  if (i != 0)
	    {
	      if (pAb->AbWidth.DimIsPosition ||
		  pAb->AbWidth.DimAbRef/* ||
		  pAb->AbWidth.DimAbRef == pAb->AbEnclosing*/)
		/* the outside width is constrained */
		ResizeWidth (pAb->AbBox, pAb->AbBox, NULL, -i, i, 0, 0, frame);
	      else
		/* the inside width is constrained */
		ResizeWidth (pAb->AbBox, pAb->AbBox, NULL, 0, i, 0, 0, frame);
	    }
	}
      pAb = pCurrentAb;
      while (pAb && pAb->AbBox && pAb->AbBox->BxType == BoGhost)
	{
	  pAb = pAb->AbFirstEnclosed;
	  while (pAb && pAb->AbNext)
	    pAb = pAb->AbNext;
	}
      if (pAb && pAb->AbBox)
	{
	  /* the last child */
	  pAb->AbBox->BxRMargin = pBox->BxRMargin;
	  pAb->AbBox->BxRBorder = pBox->BxRBorder;
	  pAb->AbBox->BxRPadding = pBox->BxRPadding;
	  pAb->AbRightStyle = pCurrentAb->AbRightStyle;
	  pAb->AbRightBColor = pCurrentAb->AbRightBColor;
	  if (j != 0)
	    {
	      if (pAb->AbWidth.DimIsPosition ||
		  pAb->AbWidth.DimAbRef/* ||
		  pAb->AbWidth.DimAbRef == pAb->AbEnclosing*/)
		/* the outside width is constrained */
		ResizeWidth (pAb->AbBox, pAb->AbBox, NULL, -j, 0, j, 0, frame);
	      else
		/* the inside width is constrained */
		ResizeWidth (pAb->AbBox, pAb->AbBox, NULL, 0, 0, j, 0, frame);
	    }
	}
    }
  else
    {
      while (pAb && pAb->AbBox && pAb->AbBox->BxType == BoGhost)
	pAb = pAb->AbFirstEnclosed;
      while (pAb && pAb->AbBox)
	{
	  /* all children */
	  pAb->AbBox->BxTMargin = pBox->BxTMargin;
	  pAb->AbBox->BxTBorder = pBox->BxTBorder;
	  pAb->AbBox->BxTPadding = pBox->BxTPadding;
	  pAb->AbTopStyle = pCurrentAb->AbTopStyle;
	  pAb->AbTopBColor = pCurrentAb->AbTopBColor;

	  pAb->AbBox->BxBMargin = pBox->BxBMargin;
	  pAb->AbBox->BxBBorder = pBox->BxBBorder;
	  pAb->AbBox->BxBPadding = pBox->BxBPadding;
	  pAb->AbBottomStyle = pCurrentAb->AbBottomStyle;
	  pAb->AbBottomBColor = pCurrentAb->AbBottomBColor;
	  if (i || j)
	    {
	      if (pAb->AbHeight.DimIsPosition ||
		  pAb->AbHeight.DimAbRef/* ||
		  pAb->AbHeight.DimAbRef == pAb->AbEnclosing*/)
		/* the outside height is constrained */
		ResizeHeight (pAb->AbBox, pAb->AbBox, NULL, - i - j, i, j,
			      frame);
	      else
		/* the inside height is constrained */
		ResizeHeight (pAb->AbBox, pAb->AbBox, NULL, 0, i, j, frame);
	    }
	  pAb = pAb->AbNext;
	}
    }
}

/*----------------------------------------------------------------------
  CheckMBP checks margins, borders and paddings of the current box.
  Return TRUE when any value was updated.
  ----------------------------------------------------------------------*/
ThotBool CheckMBP (PtrAbstractBox pAb, PtrBox pBox, int frame,
		   ThotBool evalAuto)
{
  int lt, rb;

  /* update vertical margins, borders and paddings */
  lt = pBox->BxTMargin + pBox->BxTPadding + pBox->BxTBorder;
  rb = pBox->BxBMargin + pBox->BxBPadding + pBox->BxBBorder;
  ComputeMBP (pAb, frame, FALSE, evalAuto);
  lt = - lt + pBox->BxTMargin + pBox->BxTPadding + pBox->BxTBorder;
  rb = - rb + pBox->BxBMargin + pBox->BxBPadding + pBox->BxBBorder;
  /* Check if the changes affect the inside or the outside width */
  pAb->AbBox->BxHorizRef += lt;
  if (pAb->AbBox->BxType == BoGhost)
    TransmitMBP (pBox, frame, lt, rb, FALSE);
  else if (lt != 0 || rb != 0)
    {
      if (pAb->AbHeight.DimIsPosition ||
	  pAb->AbHeight.DimAbRef/* ||
	  pAb->AbHeight.DimAbRef == pAb->AbEnclosing*/)
	/* the outside height is constrained */
	ResizeHeight (pBox, pBox, NULL, - lt - rb, lt, rb, frame);
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
  if (pAb->AbBox->BxType == BoGhost)
    TransmitMBP (pBox, frame, lt, rb, TRUE);
  else if (lt != 0 || rb != 0)
    {
      pAb->AbBox->BxVertRef += lt;
      if (pAb->AbWidth.DimIsPosition ||
	  pAb->AbWidth.DimAbRef
	  /* ||
	     pAb->AbWidth.DimAbRef == pAb->AbEnclosing*/)
	/* the outside width is constrained */
	ResizeWidth (pBox, pBox, NULL, - lt - rb, lt, rb, 0, frame);
      else
	/* the inside width is constrained */
	ResizeWidth (pBox, pBox, NULL, 0, lt, rb, 0, frame);
      return TRUE;
    }
  return FALSE;
}


/*----------------------------------------------------------------------
  HasFloatingChild returns TRUE if a child is a floating box.
  ----------------------------------------------------------------------*/
ThotBool HasFloatingChild (PtrAbstractBox pAb)
{
#ifdef _CSS_FLOAT
  PtrAbstractBox      pChildAb;

  if (pAb && !pAb->AbDead)
    {
      /* check enclosed boxes */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb)
	{
	  if (pChildAb->AbLeafType == LtCompound &&
	      pChildAb->AbFloat != 'N' &&
	      !pChildAb->AbWidth.DimIsPosition &&
	      pChildAb->AbWidth.DimAbRef == NULL &&
	      pChildAb->AbWidth.DimValue != -1)
	    /* yes there is a floating child */
	    return TRUE;
	  else
	    pChildAb = pChildAb->AbNext;
	}
    } 
#endif /* _CSS_FLOAT */
  return FALSE;
}

/*----------------------------------------------------------------------
  AddFloatingBox registers the floating box in the parent block.
  ----------------------------------------------------------------------*/
static void AddFloatingBox (PtrAbstractBox pAb, ThotBool left)
{
#ifdef _CSS_FLOAT
  PtrBox              pBox, box;
  PtrAbstractBox      pParent;
  PtrFloat            previous, new;

  if (pAb && !pAb->AbDead)
    {
      box = pAb->AbBox;
      if (pAb->AbLeafType == LtCompound &&
	  (pAb->AbWidth.DimIsPosition ||
	   pAb->AbWidth.DimAbRef ||
	   pAb->AbWidth.DimValue == -1))
	{
	  /* the box cannot be float */
	  pAb->AbFloat = 'N';
	}
      else if (box)
	{
	  pParent = pAb->AbEnclosing;
	  while (pParent && pParent->AbBox && pParent->AbBox->BxType == BoGhost)
	    pParent = pParent->AbEnclosing;
	  if (pParent && pParent->AbBox &&
	      (pParent->AbBox->BxType == BoComplete ||
	       pParent->AbBox->BxType == BoFloatBlock ||
	       pParent->AbBox->BxType == BoBlock))
	    {
	      if (pParent->AbBox->BxType == BoComplete)
		/* mark this new block */
		pParent->AbBox->BxType = BoFloatBlock;
	      /* look for the float block */
	      pBox = pParent->AbBox;
	      new = (PtrFloat) TtaGetMemory (sizeof (BFloat));
	      new->FlBox = box;
	      new->FlNext = NULL;
	      previous = NULL;
	      if (left)
		{
		  if (pBox->BxLeftFloat == NULL)
		    {
		      new->FlPrevious = NULL;
		      pBox->BxLeftFloat = new;
		    }
		  else
		    previous = pBox->BxLeftFloat;
		}
	      else
		{
		  if (pBox->BxRightFloat == NULL)
		    {
		      new->FlPrevious = NULL;
		      pBox->BxRightFloat = new;
		    }
		  else
		    previous = pBox->BxRightFloat;
		}
	      if (previous)
		{
		  while (previous->FlNext)
		    previous = previous->FlNext;
		  previous->FlNext = new;
		  new->FlPrevious = previous;
		}
	    }
	}
    }
#endif /* _CSS_FLOAT */
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
  Return the box address.
  ----------------------------------------------------------------------*/
static PtrBox CreateBox (PtrAbstractBox pAb, int frame, ThotBool inLine,
			 int *carIndex)
{
  PtrSSchema          pSS;
  PtrAbstractBox      pChildAb, pParent;
  PtrBox              pBox;
  PtrBox              pMainBox;
  PtrBox              pCurrentBox;
  TypeUnit            unit;
  SpecFont            font;
  PictInfo           *picture;
  BoxType             tableType;
  char                script = 'L';
  int                 width, i, j;
  int                 height;
  ThotBool            enclosedWidth;
  ThotBool            enclosedHeight;
  ThotBool            inlineChildren;

  if (pAb->AbDead)
    return (NULL);

  /* by default it's not a table element */
  tableType = BoComplete;
  pSS = pAb->AbElement->ElStructSchema;
  if (pAb->AbLeafType == LtCompound)
    {
    if (TypeHasException (ExcIsColHead, pAb->AbElement->ElTypeNumber, pSS))
      tableType = BoColumn;
    else if (TypeHasException (ExcIsTable, pAb->AbElement->ElTypeNumber, pSS))
      tableType = BoTable;
    else if (TypeHasException (ExcIsRow, pAb->AbElement->ElTypeNumber, pSS))
      tableType = BoRow;
    else if (TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber, pSS))
      tableType = BoCell;
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
  font = ThotLoadFont (script, pAb->AbFont, FontStyleAndWeight(pAb),
		       height, unit, frame);
 
  /* Creation */
  pCurrentBox = pAb->AbBox;
  if (pCurrentBox == NULL)
    {
      pCurrentBox = GetBox (pAb);
      pAb->AbBox = pCurrentBox;
    }

  if (pCurrentBox)
    {
      /* pMainBox points to the root box of the view */
      pMainBox = ViewFrameTable[frame - 1].FrAbstractBox->AbBox;
      pCurrentBox->BxFont = font;
      pCurrentBox->BxUnderline = pAb->AbUnderline;
      pCurrentBox->BxThickness = pAb->AbThickness;
      if (tableType == BoRow || tableType == BoColumn || tableType == BoCell)
	{
	  /* float and inlines are not allowed for rows and cells */
	  inLine = FALSE;
	  pAb->AbFloat = 'N';
	  inlineChildren = FALSE;
	}
      else
	{
	  if (pAb->AbFloat == 'L')
	    AddFloatingBox (pAb, TRUE);
	  else if (pAb->AbFloat == 'R')
	    AddFloatingBox (pAb, FALSE);
	  if (pAb->AbLeafType == LtCompound)
	    {
	      if (inLine && pAb->AbAcceptLineBreak && pAb->AbFloat == 'N')
		{
		  inlineChildren = inLine;
		  if (pAb->AbFirstEnclosed)
		    /* that block will be skipped */
		    pCurrentBox->BxType = BoGhost;
		}
	      else if (pAb->AbInLine)
		{
		  tableType = BoBlock;
		  inlineChildren = TRUE;
		  pCurrentBox->BxType = BoBlock;
		  pCurrentBox->BxFirstLine = NULL;
		  pCurrentBox->BxLastLine = NULL;
		}
	      else if (HasFloatingChild (pAb))
		{
		  tableType = BoFloatBlock;
		  inlineChildren = TRUE;
		  pCurrentBox->BxType = BoFloatBlock;
		  pCurrentBox->BxFirstLine = NULL;
		  pCurrentBox->BxLastLine = NULL;
		}
	      else
		/* no inline indicator for children */
		inlineChildren = FALSE;
	    }
	  else
	    inlineChildren = inLine;
	}

      /* Dimensionnement de la boite par contraintes */
      /* Il faut initialiser le trace reel et l'indication */
      /* des reperes inverses (en cas de boite elastique)  */
      /* avant d'evaluer la dimension de la boite si elle  */
      /* est de type graphique */
      if (pAb->AbLeafType == LtGraphics)
	{
	  pAb->AbRealShape = pAb->AbShape;
	  pCurrentBox->BxHorizInverted = FALSE;
	  pCurrentBox->BxVertInverted = FALSE;
	}
      /* New values of margins, paddings and borders */
      pAb->AbMBPChange = FALSE;
      ComputeMBP (pAb, frame, TRUE, FALSE);
      ComputeMBP (pAb, frame, FALSE, FALSE);
      pCurrentBox->BxXToCompute = FALSE;
      pCurrentBox->BxYToCompute = FALSE;
      enclosedWidth = ComputeDimRelation (pAb, frame, TRUE);
      enclosedHeight = ComputeDimRelation (pAb, frame, FALSE);
      CheckMBP (pAb, pCurrentBox, frame, TRUE);
      if (pAb->AbLeafType != LtCompound)
	{
	  /* Positionnement des axes de la boite construite */
	  ComputeAxisRelation (pAb->AbVertRef, pCurrentBox, frame, TRUE);
	  /* On traite differemment la base d'un bloc de lignes  */
	  /* s'il depend de la premiere boite englobee           */
	  ComputeAxisRelation (pAb->AbHorizRef, pCurrentBox, frame, FALSE);
	}
      /* On construit le chainage des boites terminales pour affichage */
      /* et on calcule la position des paves dans le document.         */
      if (pAb->AbFirstEnclosed == NULL)
	{
	  /* On note pour l'affichage que cette boite est nouvelle */
	  pCurrentBox->BxNew = TRUE;
	  
	  /* Est-ce la boite racine ? */
	  if (pMainBox == pCurrentBox)
	    {
	      pCurrentBox->BxPrevious = NULL;
	      pCurrentBox->BxNext = NULL;
	    }
	  else
	    {
	      /* Add the new box at the end of the displayed boxes list */
	      /* The list is pointed from the root box */
	      /* BxNext(Root) -> First displayed box              */
	      /* BxPrevious(Root) -> Last displayed box           */
	      pBox = pMainBox->BxPrevious;
	      pCurrentBox->BxPrevious = pBox;
	      if (pBox)
		{
		  pBox->BxNext = pCurrentBox;
		  if (pBox->BxType == BoPiece || pBox->BxType == BoScript)
		    /* update also the split parent box */
		    pBox->BxAbstractBox->AbBox->BxNext = pCurrentBox;
		}
	      pMainBox->BxPrevious = pCurrentBox;
	      if (pMainBox->BxNext == NULL)
		pMainBox->BxNext = pCurrentBox;
	    }
	  pCurrentBox->BxIndChar = 0;
	  *carIndex += pAb->AbVolume;
	}

      /* manage shadow exception */
      if (pAb->AbPresentationBox)
	pCurrentBox->BxShadow = FALSE;
      else if (TypeHasException (ExcShadow, pAb->AbElement->ElTypeNumber, pSS))
	pCurrentBox->BxShadow = TRUE;
      else if (pCurrentBox->BxAbstractBox->AbEnclosing &&
	       pCurrentBox->BxAbstractBox->AbEnclosing->AbBox &&
	       pCurrentBox->BxAbstractBox->AbEnclosing->AbBox->BxShadow)
	pCurrentBox->BxShadow = TRUE;
      else
	pCurrentBox->BxShadow = FALSE;
      
      /* Evaluation du contenu de la boite */
      switch (pAb->AbLeafType)
	{
	case LtPageColBreak:
	  pCurrentBox->BxBuffer = NULL;
	  pCurrentBox->BxNChars = pAb->AbVolume;
	  width = 0;
	  height = 0;
	  break;
	case LtText:
	  pCurrentBox->BxBuffer = pAb->AbText;
	  pCurrentBox->BxNChars = pAb->AbVolume;
	  pCurrentBox->BxFirstChar = 1;
	  pCurrentBox->BxSpaceWidth = 0;
	  /* get the default script */
	  pCurrentBox->BxScript = script;
	  GiveTextSize (pAb, pMainBox, &width, &height, &i);
	  pCurrentBox->BxNSpaces = i;
	  break;
	case LtPicture:
	  pCurrentBox->BxType = BoPicture;
	  picture = (PictInfo *) pAb->AbPictInfo;
	  pCurrentBox->BxPictInfo = pAb->AbPictInfo;
	  if (!pAb->AbPresentationBox && pAb->AbVolume != 0 &&
	      pCurrentBox->BxPictInfo != NULL)
	    {
	      /* box size has to be positive */
	      if (pCurrentBox->BxW < 0)
		ChangeWidth (pCurrentBox, pCurrentBox, NULL,
			     10 - pCurrentBox->BxW, 0, frame);
	      if (pCurrentBox->BxH < 0)
		ChangeHeight (pCurrentBox, pCurrentBox, NULL,
			      10 - pCurrentBox->BxH, frame);
	    }
	  
	  if (picture->PicPixmap == None)
	    LoadPicture (frame, pCurrentBox, picture);
	  GivePictureSize (pAb, ViewFrameTable[frame - 1].FrMagnification,
			   &width, &height);
	  break;
	case LtSymbol:
	  pCurrentBox->BxBuffer = NULL;
	  pCurrentBox->BxNChars = pAb->AbVolume;
	  /* Les reperes de la boite (elastique) ne sont pas inverses */
	  pCurrentBox->BxHorizInverted = FALSE;
	  pCurrentBox->BxVertInverted = FALSE;
	  GiveSymbolSize (pAb, &width, &height);
	  break;
	case LtGraphics:
	  pCurrentBox->BxBuffer = NULL;
	  pCurrentBox->BxNChars = pAb->AbVolume;
	  GiveGraphicSize (pAb, &width, &height);
	  if (pAb->AbShape == 'C')
	    {
	      /* update radius of the rectangle with rounded corners */
	      ComputeRadius (pAb, frame, TRUE);
	      ComputeRadius (pAb, frame, FALSE);
	    }
	  break;
	case LtPolyLine:
	  /* Prend une copie des points de controle */
	  pCurrentBox->BxBuffer = CopyText (pAb->AbPolyLineBuffer, NULL);
	  pCurrentBox->BxNChars = pAb->AbVolume;	/* Nombre de points */
	  pCurrentBox->BxPictInfo = NULL;
	  pCurrentBox->BxXRatio = 1;
	  pCurrentBox->BxYRatio = 1;
	  GivePolylineSize (pAb, ViewFrameTable[frame - 1].FrMagnification,
			    &width, &height);
	  break;
	case LtPath:
	  /* Prend une copie du path */
	  pCurrentBox->BxFirstPathSeg = CopyPath (pAb->AbFirstPathSeg);
	  pCurrentBox->BxNChars = pAb->AbVolume;
	  pCurrentBox->BxXRatio = 1;
	  pCurrentBox->BxYRatio = 1;
	  if (pAb->AbEnclosing)
	    {
	      /* the direct parent is the SVG path element */
	      pParent = pAb->AbEnclosing->AbEnclosing;
	      while (pParent &&
		     TypeHasException (ExcIsGroup, pParent->AbElement->ElTypeNumber,
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
	  else
	    {
	      width = 0;
	      height = 0;
	    }
	  break;
	case LtCompound:
	  if (tableType == BoTable)
	    {
	      pCurrentBox->BxType = tableType;
	      pCurrentBox->BxColumns = NULL;
	      pCurrentBox->BxRows = NULL;
	      pCurrentBox->BxMaxWidth = 0;
	      pCurrentBox->BxMinWidth = 0;
	    }
	  else if (tableType == BoColumn)
	    {
	      pCurrentBox->BxType = tableType;
	      pCurrentBox->BxTable = NULL;
	      pCurrentBox->BxRows = NULL;
	      pCurrentBox->BxMaxWidth = 0;
	      pCurrentBox->BxMinWidth = 0;
	    }
	  else if (tableType == BoRow)
	    {
	      pCurrentBox->BxType = tableType;
	      pCurrentBox->BxTable = NULL;
	      pCurrentBox->BxRows = NULL;
	      pCurrentBox->BxMaxWidth = 0;
	      pCurrentBox->BxMinWidth = 0;
	    }
	  else if (tableType == BoCell)
	    {
	      pCurrentBox->BxType = tableType;
	      pCurrentBox->BxTable = NULL;
	      pCurrentBox->BxRows = NULL;
	      pCurrentBox->BxMaxWidth = 0;
	      pCurrentBox->BxMinWidth = 0;
	    }

	  /* Is there a background image ? */
	  if (pAb->AbPictBackground != NULL)
	    {
	      /* force filling */
	      pAb->AbFillBox = TRUE;
	      /* load the picture */
	      picture = (PictInfo *) pAb->AbPictBackground;
	      if (picture->PicPixmap == None)
		LoadPicture (frame, pCurrentBox, picture);
	    }

	  /* Is it a filled box? */
	  if (pCurrentBox->BxType != BoCell &&
	      ( pAb->AbFillBox ||
		(pAb->AbTopStyle > 2 && pAb->AbTopBColor != -2 &&
		 pAb->AbTopBorder > 0) ||
		(pAb->AbLeftStyle > 2 && pAb->AbLeftBColor != -2 &&
		 pAb->AbLeftBorder > 0) ||
		(pAb->AbBottomStyle > 2 && pAb->AbBottomBColor != -2 &&
		 pAb->AbBottomBorder > 0) ||
		(pAb->AbRightStyle > 2 && pAb->AbRightBColor != -2 &&
		 pAb->AbRightBorder > 0)))
	    {
	      /* register the box */
	      pCurrentBox->BxDisplay = TRUE;
	      pCurrentBox->BxFill = pAb->AbFillBox;
	    }

	  if (pCurrentBox == pMainBox)
	    {
	      /* Set the right document background */
	      if (pAb->AbFillBox)
		SetMainWindowBackgroundColor (frame, pAb->AbBackground);
	      else
		SetMainWindowBackgroundColor (frame, DefaultBColor);
	    }
	  /* create enclosed boxes */
	  pChildAb = pAb->AbFirstEnclosed;
	  while (pChildAb)
	    {
	      pBox = CreateBox (pChildAb, frame, inlineChildren, carIndex);
	      pChildAb = pChildAb->AbNext;
	    }
	  if (pCurrentBox->BxType == BoGhost)
	    {
	      /* the current box won't be displayed */
	      if (pAb->AbFillBox)
		TransmitFill (pCurrentBox, TRUE);
	      /* transmit margins, borders and paddings */
	      i = pCurrentBox->BxLMargin + pCurrentBox->BxLPadding + pCurrentBox->BxLBorder;
	      j = pCurrentBox->BxRMargin + pCurrentBox->BxRPadding + pCurrentBox->BxRBorder;
	      TransmitMBP (pCurrentBox, frame, i, j, TRUE);
	      i = pCurrentBox->BxTMargin + pCurrentBox->BxTPadding + pCurrentBox->BxTBorder;
	      j = pCurrentBox->BxBMargin + pCurrentBox->BxBPadding + pCurrentBox->BxBBorder;
	      TransmitMBP (pCurrentBox, frame, i, j, FALSE);
	    }
	  GiveEnclosureSize (pAb, frame, &width, &height);
	  /* Position of box axis */
	  ComputeAxisRelation (pAb->AbVertRef, pCurrentBox, frame, TRUE);
	  /* don't manage the baseline of blocks here */
	  if ((pCurrentBox->BxType != BoFloatBlock &&
	       pCurrentBox->BxType != BoBlock) ||
	      pAb->AbHorizRef.PosAbRef != pAb->AbFirstEnclosed)
	    ComputeAxisRelation (pAb->AbHorizRef, pCurrentBox, frame, FALSE);
	  break;
	default:
	  break;
	}

      if (pAb->AbLeafType != LtCompound)
	{
	  /* check if parent box transmit a background or borders */
	  pParent = pAb->AbEnclosing;
	  if (pParent && pParent->AbBox && pParent->AbBox->BxType == BoGhost)
	    {
	      /* the current box won't be displayed */
	      pBox = pParent->AbBox;
	      if (pParent->AbFillBox)
		TransmitFill (pBox, TRUE);
	      /* transmit margins, borders and paddings */
	      i = pBox->BxLMargin + pBox->BxLPadding + pBox->BxLBorder;
	      j = pBox->BxRMargin + pBox->BxRPadding + pBox->BxRBorder;
	      TransmitMBP (pBox, frame, i, j, TRUE);
	      i = pBox->BxTMargin + pBox->BxTPadding + pBox->BxTBorder;
	      j = pBox->BxBMargin + pBox->BxBPadding + pBox->BxBBorder;
	      TransmitMBP (pBox, frame, i, j, FALSE);
	    }
	}

      /* Dimensionnement de la boite par le contenu ? */
      ChangeDefaultWidth (pCurrentBox, pCurrentBox, width, 0, frame);
      /* Il est possible que le changement de largeur de la boite modifie */
      /* indirectement (parce que la boite contient un bloc de ligne) la  */
      /* hauteur du contenu de la boite.                                  */
      if (enclosedWidth && enclosedHeight && pAb->AbLeafType == LtCompound)
	GiveEnclosureSize (pAb, frame, &width, &height);
      ChangeDefaultHeight (pCurrentBox, pCurrentBox, height, frame);
      
      /* Positionnement des origines de la boite construite */
      i = 0;
      if (!inLine)
	{
	  ComputePosRelation (pAb->AbHorizPos, pCurrentBox, frame, TRUE);
	  ComputePosRelation (pAb->AbVertPos, pCurrentBox, frame, FALSE);
	}
      else
	{
	  if (!pAb->AbHorizEnclosing || pAb->AbNotInLine)
	    /* the inline rule doesn't act on this box */
	    ComputePosRelation (pAb->AbHorizPos, pCurrentBox, frame, TRUE);
	  else
	    /* the real position of the box depends of its horizontal
	       reference axis */
	    SetPositionConstraint (VertRef, pCurrentBox, &i);
	  if (!pAb->AbVertEnclosing)
	    /* the inline rule doesn't act on this box */
	    ComputePosRelation (pAb->AbHorizRef, pCurrentBox, frame, FALSE);
	  else if (pAb->AbNotInLine)
	    /* the inline rule doesn't act on this box */
	    ComputePosRelation (pAb->AbVertPos, pCurrentBox, frame, FALSE);
	  else
	    /* the real position of the box depends of its horizontal reference axis */
	    SetPositionConstraint (HorizRef, pCurrentBox, &i);
	}
     
#ifdef _GL
      if (pAb->AbElement->animation)
	{
	  i = ActiveFrame;
	  ActiveFrame = frame;
	  AnimatedBoxAdd ((PtrElement)pAb->AbElement);
	  ActiveFrame = i;
	}
      
#endif /* _GL */

      pAb->AbNew = FALSE;	/* la regle de creation est interpretee */
      /* manage table exceptions */
      if (tableType == BoTable && ThotLocalActions[T_checktable])
	(*ThotLocalActions[T_checktable]) (pAb, NULL, NULL, frame);
      else if (tableType == BoColumn && ThotLocalActions[T_checktable])
	(*ThotLocalActions[T_checktable]) (NULL, pAb, NULL, frame);
      else if (tableType == BoRow && ThotLocalActions[T_checktable])
	(*ThotLocalActions[T_checktable]) (NULL, NULL, pAb, frame);
      else if (tableType == BoCell && ThotLocalActions[T_checkcolumn])
	(*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
    }
#ifdef _GL
  pCurrentBox->BxClipX = pCurrentBox->BxXOrg + pCurrentBox->BxLMargin 
    + pCurrentBox->BxLBorder + pCurrentBox->BxLPadding;
  pCurrentBox->BxClipY = pCurrentBox->BxYOrg + pCurrentBox->BxTMargin 
    + pCurrentBox->BxTBorder + pCurrentBox->BxTPadding;
  pCurrentBox->BxClipW = pCurrentBox->BxWidth;/*  - pCurrentBox->BxLMargin - pCurrentBox->BxRMargin; */
  pCurrentBox->BxClipH = pCurrentBox->BxHeight; /* - pCurrentBox->BxTMargin - pCurrentBox->BxBMargin; */
#endif /* _GL */
  return (pCurrentBox);
}

/*----------------------------------------------------------------------
  SearchEnclosingType look for the enclosing table, row or block.
  ----------------------------------------------------------------------*/
PtrAbstractBox SearchEnclosingType (PtrAbstractBox pAb, BoxType type1,
				    BoxType type2)
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
      else if (pAb->AbBox->BxType == type1 || pAb->AbBox->BxType == type2)
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
PtrLine SearchLine (PtrBox pBox)
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
       if (pAb && (pAb->AbNotInLine || !pAb->AbHorizEnclosing))
	 pAb = NULL;
       else
	 pAb = pAb->AbEnclosing;
     }

   /* look for an enclosing block of lines */
   if (pAb)
     {
	if (pAb->AbBox == NULL)
	   pAb = NULL;
	else if (pAb->AbBox->BxType == BoGhost)
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
		  else if (pAb->AbBox->BxType != BoGhost)
		     still = FALSE;
	       }
	  }
	else if (pAb->AbBox->BxType != BoBlock &&
		 pAb->AbBox->BxType != BoFloatBlock)
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
		       /* the line is founs */
		       still = FALSE;
		       pBoxPiece = pLine->LiLastBox;
		    }
		  /* else get next box */
		  else if ((pBoxPiece->BxType == BoScript ||
			    pBoxPiece->BxType == BoPiece) &&
			   pBoxPiece->BxNexChild)
		    pBoxInLine = pBoxPiece->BxNexChild;
		  else
		     pBoxInLine = GetNextBox (pBoxInLine->BxAbstractBox);
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
void BoxUpdate (PtrBox pBox, PtrLine pLine, int charDelta, int spaceDelta,
		int wDelta, int adjustDelta, int hDelta, int frame,
		ThotBool splitBox)
{
   PtrBox              box1;
   PtrBox              pMainBox;
   Propagation         savpropage;
   PtrAbstractBox      pAb;
   AbPosition         *pPosAb;
   AbDimension        *pDimAb;
   int                 j;

   /* Traitement particulier aux boites de coupure */
   if (pBox->BxType == BoPiece ||
       pBox->BxType == BoScript ||
       pBox->BxType == BoDotted)
     {
	/* Mise a jour de sa boite mere (boite coupee) */
	pMainBox = pBox->BxAbstractBox->AbBox;
	pMainBox->BxNChars += charDelta;
	pMainBox->BxNSpaces += spaceDelta;
	pMainBox->BxW += wDelta;
	pMainBox->BxWidth += wDelta;
	pMainBox->BxH += hDelta;
	pMainBox->BxHeight += hDelta;
	/* Faut-il mettre a jour la base ? */
	pAb = pMainBox->BxAbstractBox;
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
	box1 = pBox->BxNexChild;
	while (box1)
	  {
	     box1->BxFirstChar += charDelta;
	     box1 = box1->BxNexChild;
	  }
     }

   /* Traitement sur la boite passee en parametre */
   savpropage = Propagate;
   pAb = pBox->BxAbstractBox;
   /* update the box itself */
   if (pAb->AbLeafType == LtText)
     {
       /* when a character between 2 boxes are removed the box is unchanged */
	pBox->BxNSpaces += spaceDelta;
	pBox->BxNChars += charDelta;
     }

   /* Est-ce que la largeur de la boite depend de son contenu ? */
   pDimAb = &(pBox->BxAbstractBox->AbWidth);
   if (pBox->BxContentWidth || (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
     /* Blanc entre deux boites de coupure */
     if (splitBox && pLine)
       {
	 /* Il faut mettre a jour la largeur de la boite coupee */
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
	 /* Puis refaire la mise en lignes */
	 RecomputeLines (pAb->AbEnclosing, pLine, pBox, frame);
       }
     else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
       {
	 /* Box coupee */
	 Propagate = ToSiblings;
	 if (wDelta != 0)
	   ChangeDefaultWidth (pBox, pBox, pBox->BxW + wDelta, 0, frame);
	 if (hDelta != 0)
	   ChangeDefaultHeight (pBox, pBox, pBox->BxH + hDelta, frame);
	 Propagate = savpropage;
	 /* Faut-il mettre a jour le bloc de ligne englobant ? */
	 if (Propagate == ToAll)
	   {
	     pLine = SearchLine (pBox->BxNexChild);
	     RecomputeLines (pAb->AbEnclosing, pLine, pBox, frame);
	   }
       }
     else
       {
	 /* Box entiere ou de coupure */
	 if (adjustDelta != 0)
	   ChangeDefaultWidth (pBox, pBox, pBox->BxW + adjustDelta, spaceDelta, frame);
	 else if (wDelta != 0)
	   {
	     /*if (pBox->BxNChars == charDelta)
	       ChangeDefaultWidth (pBox, pBox, wDelta + 2, spaceDelta, frame);
	       else*/
	       ChangeDefaultWidth (pBox, pBox, pBox->BxW + wDelta,
				   spaceDelta, frame);
	   }
	 if (hDelta != 0)
	   ChangeDefaultHeight (pBox, pBox, pBox->BxH + hDelta, frame);
       }
   else if (pBox->BxContentHeight || (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum))
     {
       /* La hauteur de la boite depend de son contenu */
       if (hDelta != 0)
	 ChangeDefaultHeight (pBox, pBox, pBox->BxH + hDelta, frame);
     }
   else if (pBox->BxW > 0 && pBox->BxH > 0)
     {
     /* Si la largeur de la boite ne depend pas de son contenu  */
     /* on doit forcer le reaffichage jusqua la fin de la boite */
#ifndef _GL
     if (pBox->BxLMargin < 0)
       DefClip (frame, pBox->BxXOrg + pBox->BxLMargin, pBox->BxYOrg,
		pBox->BxXOrg + pBox->BxWidth + pBox->BxLMargin,
		pBox->BxYOrg + pBox->BxHeight);
     else
       DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
		pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
#else /*  _GL */      
     DefRegion (frame, pBox->BxClipX, pBox->BxClipY,
	      pBox->BxClipX + pBox->BxClipW, pBox->BxClipY + pBox->BxClipH);
#endif /* _GL */
     }
   Propagate = savpropage;
#ifdef _GL
   pBox->VisibleModification = TRUE;
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
  PtrBox              pBox, pPieceBox;
  ThotBool            changeSelectBegin;
  ThotBool            changeSelectEnd;

  if (pAb != NULL)
    {
      if (pAb->AbBox != NULL)
	{
	  /* Liberation des lignes et boites coupees */
	  pBox = pAb->AbBox;
	  /* Remove out of structure relations */
	  ClearOutOfStructRelation (pBox);
	  if (pAb->AbLeafType == LtCompound)
	    {
	      /* unregister the box */
	      pBox->BxDisplay = FALSE;
	      pBox->BxFill = FALSE;
	    }
	  if (pBox->BxType == BoBlock || pBox->BxType == BoFloatBlock)
	    RemoveLines (pBox, frame, pBox->BxFirstLine, &changeSelectBegin, &changeSelectEnd);
	  else if (pBox->BxType == BoTable && ThotLocalActions[T_cleartable])
	    (*ThotLocalActions[T_cleartable]) (pAb);
	  else if (pBox->BxType == BoColumn && ThotLocalActions[T_checktable])
	    (*ThotLocalActions[T_checktable]) (NULL, pAb, NULL, frame);
	  else if (pBox->BxType == BoRow && ThotLocalActions[T_checktable])
	    (*ThotLocalActions[T_checktable]) (NULL, NULL, pAb, frame);
	  else if (pAb->AbLeafType == LtPolyLine)
	    FreePolyline (pBox);
	  else if (pAb->AbLeafType == LtPath)
	    FreePath (pBox);
	  else if (pAb->AbLeafType == LtPicture)
	    {
	      UnmapImage((PictInfo *)pBox->BxPictInfo);
	    }
	  else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
	    {
	      /* libere les boites generees pour la mise en lignes */
	      pPieceBox = pBox->BxNexChild;
	      pBox->BxNexChild = NULL;
	      while (pPieceBox != NULL)
		{
#ifdef _GL
		  if (glIsList (pPieceBox->DisplayList))
		    {
		      glDeleteLists (pPieceBox->DisplayList, 1);
		      pPieceBox->DisplayList = 0;
		    }
#endif /* _GL */
		  pPieceBox = FreeBox (pPieceBox);
		}
	    }

	  pChildAb = pAb->AbFirstEnclosed;
	  pAb->AbNew = rebuild;
	  if (rebuild)
	    {
	      /* Faut-il restaurer les regles d'une boite elastique */
	      if (pBox->BxHorizFlex && pBox->BxHorizInverted)
		XEdgesExchange (pBox, OpHorizDep);
	      
	      if (pBox->BxVertFlex && pBox->BxVertInverted)
		YEdgesExchange (pBox, OpVertDep);
	    }

	  /* Liberation des boites des paves inclus */
	  while (pChildAb != NULL)
	    {
	      RemoveBoxes (pChildAb, rebuild, frame);
	      pChildAb = pChildAb->AbNext;	/* while */
	    }

	  /* Suppression des references a pBox dans la selection */
	  if (ViewFrameTable[frame - 1].FrSelectionBegin.VsBox == pBox)
	    ViewFrameTable[frame - 1].FrSelectionBegin.VsBox = NULL;
	  if (ViewFrameTable[frame - 1].FrSelectionEnd.VsBox == pBox)
	    ViewFrameTable[frame - 1].FrSelectionEnd.VsBox = NULL;

	  /* Liberation de la boite */
	  if (pBox->BxType == BoTable && ThotLocalActions[T_cleartable])
	    (*ThotLocalActions[T_cleartable]) (pAb);
#ifdef _GL
	  if (glIsList (pAb->AbBox->DisplayList))
	    {
	      glDeleteLists (pAb->AbBox->DisplayList, 1);
	      pAb->AbBox->DisplayList = 0;
	    }
#endif /* _GL */
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
      pAb->AbEnclosing->AbBox->BxType != BoGhost)
    {
      if (pAb->AbHorizPos.PosAbRef == NULL)
	{
	  /* On recherche le pave suivant ayant la meme regle par defaut */
	  pNextAb = pAb->AbNext;
	  while (pNextAb != NULL)
	    {
	      if (pNextAb->AbHorizPos.PosAbRef == NULL)
		{
		  /* Reevalue la regle du premier pave suivant non mort */
		  if (!pNextAb->AbDead && pNextAb->AbBox != NULL
		      /* si ce pave ne vient pas d'etre cree                */
		      && pNextAb->AbNum == 0)
		    {
		      /* Nouvelle position horizontale */
		      ClearPosRelation (pNextAb->AbBox, TRUE);
		      ComputePosRelation (pNextAb->AbHorizPos, pNextAb->AbBox, frame, TRUE);
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
	  while (pNextAb != NULL)
	    {
	      if (pNextAb->AbVertPos.PosAbRef == NULL)
		{
		  /* Reevalue la regle du premier pave suivant non mort */
		  if (!pNextAb->AbDead && pNextAb->AbBox != NULL
		      /* si ce pave ne vient pas d'etre cree                */
		      && pNextAb->AbNum == 0)
		    {
		      /* Nouvelle position verticale */
		      ClearPosRelation (pNextAb->AbBox, FALSE);
		      ComputePosRelation (pNextAb->AbVertPos, pNextAb->AbBox, frame, FALSE);
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

  if (horizRef && (pBox->BxType == BoCell || pBox->BxType == BoTable))
    /* width of these elements are computed in tableH.c */
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
	  if (pDimRel->DimRSame[i] == horizRef)
	    {
	      if (pBox == pDimRel->DimRTable[i])
		/* The box is already registered */
		return;
#ifdef IV
	      else if (IsParentBox (pDimRel->DimRTable[i], pBox))
		{
		  /* store the child pBox instead of the parent */
		  pDimRel->DimRTable[i] = pBox;
		  return;
		}
#endif
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
  /* packing the width */
  pDimRel->DimRSame[i] = horizRef;
}


/*----------------------------------------------------------------------
  UpdateFloat updates floatting information of the abstract box pAb
  within pParent.
  The parameter inLine is TRUE is the parent was previously a block.
  ----------------------------------------------------------------------*/
static void UpdateFloat (PtrAbstractBox pAb, PtrAbstractBox pParent,
			 ThotBool inLine, int frame)
{
  PtrBox              pBox, pBlock;
  PtrFloat            pfloat;

  pBox = pAb->AbBox;
  if (pBox->BxType == BoRow ||
      pBox->BxType == BoColumn ||
      pBox->BxType == BoCell)
    pAb->AbFloat = 'N';
  if (pAb->AbFloat == 'L' || pAb->AbFloat == 'R')
    {
      if (pAb->AbFloat == 'L')
	AddFloatingBox (pAb, TRUE);
      else if (pAb->AbFloat == 'R')
	AddFloatingBox (pAb, FALSE);
      inLine = TRUE;
    }
  else if (inLine)
    {
      /* check if the float property is removed */
      while (pParent && pParent->AbBox &&
	     pParent->AbBox->BxType == BoFloatGhost)
	pParent->AbBox->BxType = BoComplete;
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
		pBlock->BxLeftFloat->FlNext = pfloat->FlNext;
	      else if (pfloat == pBlock->BxRightFloat)
		pBlock->BxRightFloat->FlNext = pfloat->FlNext;
	      else
		pfloat->FlPrevious->FlNext = pfloat->FlNext;
	      TtaFreeMemory (pfloat);
	      pfloat = NULL;
	    }
	  if (pBlock->BxType == BoFloatBlock &&
	      pBlock->BxLeftFloat == NULL &&
	      pBlock->BxRightFloat == NULL)
	    {
	      inLine = FALSE;
	      pParent->AbBox->BxType = BoComplete;
	    }
	}
    }
}

/*----------------------------------------------------------------------
  ComputeUpdates checks what is changing in the current Abstract Box.
  Return TRUE if there is at least one change.
  ----------------------------------------------------------------------*/
ThotBool ComputeUpdates (PtrAbstractBox pAb, int frame)
{
  PtrLine             pLine;
  PtrAbstractBox      pCurrentAb, pCell, pBlock, pParent;
  PtrBox              pNextBox;
  PtrBox              pCurrentBox = NULL;
  PtrBox              pMainBox;
  PtrBox              pLastBox;
  PtrBox              pBox;
  TypeUnit            unit;
  Propagation         savpropage;
  ViewFrame          *pFrame;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 width, height;
  int                 nSpaces;
  int                 i, k, charDelta, adjustDelta;
  ThotBool            condition, inLine;
  ThotBool            result, isCell;
  ThotBool            orgXComplete;
  ThotBool            orgYComplete;
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
  pMainBox = pFrame->FrAbstractBox->AbBox;	/* boite de la racine */
  pBox = pAb->AbBox;		/* boite du pave */
  result = FALSE;
  condition = FALSE;
  pParent = pAb->AbEnclosing;
  if (pParent == NULL)
    /* it's the root box */
    inLine = FALSE;
  else if (pParent->AbBox &&
	   (pParent->AbBox->BxType == BoBlock ||
	    pParent->AbBox->BxType == BoFloatBlock ||
	    pParent->AbBox->BxType == BoGhost ||
	    pParent->AbBox->BxType == BoFloatBlock))
    /* within a block */
    inLine = TRUE;
  else
    inLine = FALSE;

  /* Prepare the clipping */
  if (pAb->AbNew || pAb->AbDead || pAb->AbChange || pAb->AbMBPChange ||
      pAb->AbWidthChange || pAb->AbHeightChange ||
      pAb->AbHorizPosChange || pAb->AbVertPosChange ||
      pAb->AbHorizRefChange || pAb->AbVertRefChange ||
      pAb->AbAspectChange || pAb->AbSizeChange)
    {
      /* look at if the box or an enclosing box has a background */
      if (pAb->AbNew || pAb->AbDead ||
	  pAb->AbHorizPosChange || pAb->AbVertPosChange)
	{
	  pCurrentAb = pParent;	   
	  while (pCurrentAb && pCurrentAb->AbPictBackground == NULL &&
		 !pCurrentAb->AbFillBox)
	    pCurrentAb = pCurrentAb->AbEnclosing;
	  if (pCurrentAb == NULL || pCurrentAb->AbEnclosing == NULL)
	    /* no background found: clip the current box */
	    pCurrentAb = pAb;
	}
      else
	pCurrentAb = pAb;
      
      pCurrentBox = pCurrentAb->AbBox;
      if (pCurrentBox)
	{
	  if (pCurrentBox->BxType == BoGhost)
	    {
	      /* move to the enclosing block */
	      while (pCurrentAb && pCurrentAb->AbBox &&
		     pCurrentAb->AbBox->BxType == BoGhost)
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
	      if (pCurrentBox->BxAbstractBox->AbElement->ElTerminal)
		k = EXTRA_GRAPH;
	      else
		k = 0;
	      /* mark the zone to be displayed */
	      if (TypeHasException (ExcSetWindowBackground, pAb->AbElement->ElTypeNumber,
				    pAb->AbElement->ElStructSchema))
		DefClip (frame, -1, -1, -1, -1);
	      else
#ifndef _GL
		if (pCurrentBox->BxLMargin < 0)
		  DefClip (frame, pCurrentBox->BxXOrg + pBox->BxLMargin,
			   pCurrentBox->BxYOrg,
			   pCurrentBox->BxXOrg + pCurrentBox->BxWidth + pBox->BxLMargin,
			   pCurrentBox->BxYOrg + pCurrentBox->BxHeight);
		else
		  DefClip (frame, pCurrentBox->BxXOrg - k, pCurrentBox->BxYOrg - k,
			   pCurrentBox->BxXOrg + pCurrentBox->BxWidth + k,
			   pCurrentBox->BxYOrg + pCurrentBox->BxHeight + k);
#else /* _GL */  

	      /* Compute Bounding Box*/
	      ComputeABoundingBox (pCurrentBox->BxAbstractBox, frame);

	      DefRegion (frame, pCurrentBox->BxClipX - k,
			 pCurrentBox->BxClipY - k,
			 pCurrentBox->BxClipX + pCurrentBox->BxClipW + k,
			 pCurrentBox->BxClipY + pCurrentBox->BxClipH + k);
#endif /* _GL */
	    }
	}
#ifdef _GL
      if (pCurrentBox)
	pCurrentBox->VisibleModification = TRUE;
#endif /* _GL */
    }

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
	  if (pCurrentBox->BxType == BoSplit ||
	      pCurrentBox->BxType == BoMulScript)
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

      /* Get the box just after */
      if (pParent &&
	  pParent->AbAcceptLineBreak && pParent->AbFloat == 'N' &&
	  pParent->AbEnclosing && pParent->AbEnclosing->AbBox &&
	  (pParent->AbEnclosing->AbBox->BxType == BoBlock ||
	   pParent->AbEnclosing->AbBox->BxType == BoFloatBlock ||
	   pParent->AbEnclosing->AbBox->BxType == BoGhost))
	{
	  /* the parent was not set ghost because it was empty */
	  pParent->AbBox->BxType = BoGhost;
	  inLine = TRUE;
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
      savpropage = Propagate;
      Propagate = ToSiblings;	/* Limite la propagation sur le descendance */
      pBox = CreateBox (pAb, frame, inLine, &i);
      
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

      if (pBox == NULL)
	/* box not created */
	Propagate = savpropage;	/* Restaure la regle de propagation */
      else
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

	  /* On prepare le reaffichage */
	  if (!inLine)
	    {
#ifndef _GL
	      if (pBox->BxLMargin < 0)
		DefClip (frame, pBox->BxXOrg + pBox->BxLMargin, pBox->BxYOrg,
			 pBox->BxXOrg + pBox->BxWidth + pBox->BxLMargin,
			 pBox->BxYOrg + pBox->BxHeight);
	      else
		DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			 pBox->BxXOrg + pBox->BxWidth,
			 pBox->BxYOrg + pBox->BxHeight);
#else /* _GL */
	      DefRegion (frame, pBox->BxClipX, pBox->BxClipY,
		       pBox->BxClipX + pBox->BxClipW,
		       pBox->BxClipY + pBox->BxClipH);
#endif /* _GL */
	    }
	  
	  pAb->AbChange = FALSE;
	  pAb->AbFloatChange = FALSE;
	  /* check enclosing cell */
	  pCell = GetParentCell (pBox);
	  if (pCell && ThotLocalActions[T_checkcolumn])
	    {
	      Propagate = ToChildren;
	      pBlock = SearchEnclosingType (pAb, BoBlock, BoFloatBlock);
	      if (pBlock)
		RecomputeLines (pBlock, NULL, NULL, frame);
	      (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
	    }
	  Propagate = savpropage;	/* Restaure la regle de propagation */
	  result = TRUE;
	  pAb->AbWidthChange = FALSE;
	  pAb->AbHeightChange = FALSE;
	  pAb->AbAspectChange = FALSE;
	  pAb->AbSizeChange = FALSE;
	  pAb->AbChange = FALSE;
	  pAb->AbMBPChange = FALSE;
	}
    }
  /* AbstractBox MORT */
  else if (pAb->AbDead)
      {
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

      /* Est-ce que la boite englobante devient terminale ? */
      if (IsAbstractBoxEmpty (pParent))
	{
	  pNextBox = pParent->AbBox;
	  /* Si la boite etait eclatee, elle ne l'est plus */
	  if (pNextBox->BxType == BoGhost)
	    pNextBox->BxType = BoComplete;
	  
	  /* On ne chaine qu'une seule fois la boite englobante */
	  if (pNextBox != pCurrentBox->BxNext)
	    {
	      if (pCurrentBox == pMainBox)
		pNextBox->BxPrevious = NULL;
	      else
		pNextBox->BxPrevious = pCurrentBox;
	      
	      /* On defait l'ancien chainage */
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
      isCell = pAb->AbBox->BxType == BoCell;
      ClearAllRelations (pBox, frame);
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
		pCurrentBox->BxAbstractBox->AbBox->BxPrevious = pNextBox->BxAbstractBox->AbBox;
	      else
		pCurrentBox->BxAbstractBox->AbBox->BxPrevious = pNextBox;
	    }
	}
      
      /* Check table consistency */
      if (isCell && ThotLocalActions[T_checkcolumn])
	(*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
      /* check enclosing cell */
      else if (pCell != NULL && ThotLocalActions[T_checkcolumn] &&
	       !IsDead (pAb) &&
	       (pAb->AbNext == NULL ||
		(!pAb->AbNext->AbDead && !pAb->AbNext->AbNew)))
	(*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
      result = TRUE;
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
    }
  else
    {
      /* CHANGE BOX ASPECT */
      if (pAb->AbAspectChange)
	{
	  pAb->AbAspectChange = FALSE;
	  if (pAb->AbLeafType == LtPicture &&
	      ((PictInfo *) pBox->BxPictInfo)->PicType == XBM_FORMAT)
	    /* Reload the image */
	    LoadPicture (frame, pBox, (PictInfo *) pBox->BxPictInfo);
	  else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
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
		  LoadPicture (frame, pBox, (PictInfo *) pAb->AbPictBackground);
		}
	    }
	  else if (pAb->AbLeafType == LtGraphics && pAb->AbShape == 'C')
	    {
	      /* update radius of the rectangle with rounded corners */
	      ComputeRadius (pAb, frame, TRUE);
	      ComputeRadius (pAb, frame, FALSE);
	    }

	  /* Is it a filled box? */
	  if (pAb->AbLeafType == LtCompound && pBox->BxType != BoCell)
	    {
	      if (pAb->AbFillBox ||
		  (pAb->AbTopStyle > 2 && pAb->AbTopBColor != -2 &&
		   pAb->AbTopBorder > 0) ||
		  (pAb->AbLeftStyle > 2 && pAb->AbLeftBColor != -2 &&
		   pAb->AbLeftBorder > 0) ||
		  (pAb->AbBottomStyle > 2 && pAb->AbBottomBColor != -2 &&
		   pAb->AbBottomBorder > 0) ||
		  (pAb->AbRightStyle > 2 && pAb->AbRightBColor != -2 &&
		   pAb->AbRightBorder > 0))
		{
		  /* register the box */
		  pBox->BxDisplay = TRUE;
		  pBox->BxFill = pAb->AbFillBox;
		  if (pBox->BxType == BoGhost && pAb->AbFillBox)
		    TransmitFill (pBox, TRUE);
		  if (pAb->AbEnclosing == NULL)
		    /* root abstractbox */
		    SetMainWindowBackgroundColor (frame, pAb->AbBackground);
		}
	      else
		{
		  /* unregister the box in filled list */
		  pBox->BxDisplay = FALSE;
		  pBox->BxFill = FALSE;
		  if (pBox->BxType == BoGhost)
		    TransmitFill (pBox, FALSE);
		  if (pAb->AbEnclosing == NULL)
		    /* root abstractbox */
		    SetMainWindowBackgroundColor (frame, DefaultBColor);
		}
	    }
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
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
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
	}
      /* CHANGE FLOAT/CLEAR */
      if (pAb->AbFloatChange)
	{
	  UpdateFloat (pAb, pParent, inLine, frame);
	  pAb->AbFloatChange = FALSE;
	  pAb->AbChange = TRUE;	  
	}
      /* CHANGE THE CONTENTS */
      if (pAb->AbChange || pAb->AbSizeChange)
	{
	  /* check the font of the abstract box */
	  height = pAb->AbSize;
	  unit = pAb->AbSizeUnit;
	  if (pAb->AbLeafType == LtText)
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
		  pBox->BxType == BoFloatBlock)
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
		  GiveTextSize (pAb, pMainBox, &width, &height, &nSpaces);
		  
		  /* Si la boite est justifiee */
		  if (pBox->BxSpaceWidth != 0)
		    {
		      i = pBox->BxSpaceWidth - BoxCharacterWidth (SPACE, pBox->BxFont);
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
		      CleanPictInfo ((PictInfo *) pBox->BxPictInfo);
		      if (pAb->AbWidth.DimAbRef == NULL &&  pAb->AbWidth.DimValue == -1)
			{
			  /* use the content width */
			  pBox->BxWidth = 0;
			  pBox->BxW = 0;
			}
		      if (pAb->AbHeight.DimAbRef == NULL &&  pAb->AbHeight.DimValue == -1)
			{
			  /* use the content height */
			  pBox->BxHeight = 0;
			  pBox->BxH = 0;
			}
		      SetCursorWatch (frame);
		      LoadPicture (frame, pBox, (PictInfo *) pBox->BxPictInfo);
		      ResetCursorWatch (frame);
		      GivePictureSize (pAb,
				       ViewFrameTable[frame - 1].FrMagnification,
				       &width, &height);
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
		  GivePolylineSize (pAb,
				    ViewFrameTable[frame - 1].FrMagnification,
				    &width, &height);
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
		width -= pBox->BxW;	/* ecart de largeur */
	      pDimAb = &pAb->AbHeight;
	      if (pDimAb->DimIsPosition)
		height = 0;
	      else if (pDimAb->DimAbRef != NULL || pDimAb->DimValue >= 0)
		height = 0;
	      else
		height -= pBox->BxH;	/* ecart de hauteur */
	      pLine = NULL;
	      if (width || height)
		{
		  pCell = GetParentCell (pBox);
		  BoxUpdate (pBox, pLine, charDelta, nSpaces, width,
			     adjustDelta, height, frame, FALSE);
		  /* check enclosing cell */
		  if (pCell != NULL && width != 0 &&
		      ThotLocalActions[T_checkcolumn])
		    {
		      if (pAb->AbLeafType == LtPicture)
			{
			  pBlock = SearchEnclosingType (pAb, BoBlock, BoFloatBlock);
			  if (pBlock)
			    {
			      RecomputeLines (pBlock, NULL, NULL, frame);
				/* we will have to pack enclosing box */
			      if (pBlock->AbEnclosing)
				RecordEnclosing (pBlock->AbEnclosing->AbBox, FALSE);
			    }
			}
		      (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
		    }
		}
	      result = TRUE;
	    }

	  if (pAb->AbChange)
	    pAb->AbChange = FALSE;
	  if (pAb->AbSizeChange)
	    pAb->AbSizeChange = FALSE;
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
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
		  GiveTextSize (pAb, pMainBox, &width, &height, &i);
		  break;
		case LtPicture:
		  GivePictureSize (pAb, ViewFrameTable[frame - 1].FrMagnification,
				   &width, &height);
		  break;
		case LtSymbol:
		  GiveSymbolSize (pAb, &width, &height);
		  break;
		case LtGraphics:
		  GiveGraphicSize (pAb, &width, &height);
		  break;
		case LtCompound:
		  if (pBox->BxType == BoBlock || pBox->BxType == BoFloatBlock)
		    {
		      RecomputeLines (pAb, NULL, NULL, frame);
		      width = pBox->BxW;
		    }
		  else
		    GiveEnclosureSize (pAb, frame, &width, &height);
		  break;
		default:
		  width = pBox->BxW;
		  break;
		}
	      
	      /* Change the width of the contents */
	      ChangeDefaultWidth (pBox, NULL, width, 0, frame);
	      result = TRUE;
	    }
	  else
	    /* the box width doesn't depend on the contents */
	    result = TRUE;

	  if (pAb->AbLeafType == LtGraphics && pAb->AbShape == 'C' &&
	      pAb->AbRxUnit == UnPercent)
	    /* update radius of the rectangle with rounded corners */
	    ComputeRadius (pAb, frame, TRUE);
	  /* check auto margins */
	  CheckMBP (pAb, pCurrentBox, frame, TRUE);
	  
	  /* Check table consistency */
	  if (pCurrentBox->BxType == BoColumn && ThotLocalActions[T_checktable])
	    (*ThotLocalActions[T_checktable]) (NULL, pAb, NULL, frame);
	  else if (pCurrentBox->BxType == BoCell && ThotLocalActions[T_checkcolumn])
	    (*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
	  /* check enclosing cell */
	  pCell = GetParentCell (pCurrentBox);
	  if (pCell != NULL && ThotLocalActions[T_checkcolumn])
	    {
	      pBlock = SearchEnclosingType (pAb, BoBlock, BoFloatBlock);
	      if (pBlock != NULL)
		RecomputeLines (pBlock, NULL, NULL, frame);
	      (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
	    }
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
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
		  GiveTextSize (pAb, pMainBox, &width, &height, &i);
		  break;
		case LtPicture:
		  GivePictureSize (pAb, ViewFrameTable[frame -1].FrMagnification, &width, &height);
		  break;
		case LtSymbol:
		  GiveSymbolSize (pAb, &width, &height);
		  break;
		case LtGraphics:
		  GiveGraphicSize (pAb, &width, &height);
		  break;
		case LtCompound:
		  if (pBox->BxType == BoBlock || pBox->BxType == BoFloatBlock)
		    height = pBox->BxH;
		  else
		    GiveEnclosureSize (pAb, frame, &width, &height);
		  break;
		default:
		  height = pBox->BxH;
		  break;
		}

	      /* Change the height of the contents */
	      ChangeDefaultHeight (pBox, NULL, height, frame);
	      result = TRUE;
	    }
	  else
	    /* the box height doesn't depend on the contents */
	    result = TRUE;

	  if (pAb->AbLeafType == LtGraphics && pAb->AbShape == 'C' &&
	      pAb->AbRyUnit == UnPercent)
	    /* update radius of the rectangle with rounded corners */
	    ComputeRadius (pAb, frame, FALSE);
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
	}
      /* MARGIN PADDING BORDER */
      if (pAb->AbMBPChange)
	{
	  pAb->AbMBPChange = FALSE;
	  savpropage = Propagate;
	  Propagate = ToAll;	/* On passe en mode normal de propagation */
	  if (CheckMBP (pAb, pBox, frame, TRUE))
	    {
	      /* do we have to register that box as filled box */
	      if (pAb->AbLeafType == LtCompound && pBox->BxType != BoCell)
		{
		  if (pAb->AbFillBox ||
		      (pAb->AbTopStyle > 2 && pAb->AbTopBColor != -2 &&
		       pAb->AbTopBorder > 0) ||
		      (pAb->AbLeftStyle > 2 && pAb->AbLeftBColor != -2 &&
		       pAb->AbLeftBorder > 0) ||
		      (pAb->AbBottomStyle > 2 && pAb->AbBottomBColor != -2 &&
		       pAb->AbBottomBorder > 0) ||
		      (pAb->AbRightStyle > 2 && pAb->AbRightBColor != -2 &&
		       pAb->AbRightBorder > 0))
		    {
		      /* register the box in filled list */
		      pBox->BxDisplay = TRUE;
		      pBox->BxFill = pAb->AbFillBox;
		      if (pBox->BxType == BoGhost && pAb->AbFillBox)
			TransmitFill (pBox, TRUE);
		    }
		  else
		    {
		      /* unregister the box in filled list */
		      pBox->BxDisplay = FALSE;
		      pBox->BxFill = FALSE;
		      if (pBox->BxType == BoGhost)
			TransmitFill (pBox, FALSE);
		    }
		}	      /* Check table consistency */
	      if (pCurrentBox->BxType == BoColumn && ThotLocalActions[T_checktable])
		(*ThotLocalActions[T_checktable]) (NULL, pAb, NULL, frame);
	      else if (pCurrentBox->BxType == BoCell && ThotLocalActions[T_checkcolumn])
		(*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
	      /* check enclosing cell */
	      pCell = GetParentCell (pCurrentBox);
	      if (pCell && ThotLocalActions[T_checkcolumn])
		{
		  pBlock = SearchEnclosingType (pAb, BoBlock, BoFloatBlock);
		  if (pBlock)
		    RecomputeLines (pBlock, NULL, NULL, frame);
		  (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
		}
	    }
	  Propagate = savpropage;	/* Restaure la regle de propagation */
	  result = TRUE;
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
	}
      /* CHANGEMENT DE POSITION HORIZONTALE */
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
	  /* La regle de position est annulee par la mise en lignes */
	  else if (pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
		   pAb->AbEnclosing->AbBox->BxType == BoBlock ||
		   pAb->AbEnclosing->AbBox->BxType == BoGhost)
	    condition = !pAb->AbHorizEnclosing;
	  else
	    condition = TRUE;
	  if (condition)
	    {
	      /* Annulation ancienne position horizontale */
	      ClearPosRelation (pBox, TRUE);
	      /* Nouvelle position horizontale */
	      ComputePosRelation (pAb->AbHorizPos, pBox, frame, TRUE);
	      result = TRUE;
	    }
	  else
	    pAb->AbHorizPosChange = FALSE;
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
	}
      /* CHANGEMENT DE POSITION VERTICALE */
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
	  else if (pAb->AbEnclosing == NULL)
	    condition = TRUE;
	  else if (pAb->AbEnclosing->AbBox->BxType == BoBlock ||
		   pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
		   pAb->AbEnclosing->AbBox->BxType == BoGhost)
	    {
	      /* the positioning rule is ignored */
	      if (!pAb->AbHorizEnclosing && pBox->BxNChars > 0)
		{
		  pPosAb = &pAb->AbVertPos;
		  pLine = SearchLine (pBox);
		  if (pLine)
		    {
		      i = PixelValue (pPosAb->PosDistance,
				      pPosAb->PosUnit, pAb,
				      ViewFrameTable[frame - 1].FrMagnification);
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
	      /* Annulation ancienne position verticale */
	      ClearPosRelation (pBox, FALSE);
	      /* Nouvelle position verticale */
	      ComputePosRelation (pAb->AbVertPos, pBox, frame, FALSE);
	      result = TRUE;
	    }
	  else
	    pAb->AbVertPosChange = FALSE;
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
	}
      /* CHANGEMENT D'AXE HORIZONTAL */
      if (pAb->AbHorizRefChange)
	{
	  /* Annulation ancien axe horizontal */
	  ClearAxisRelation (pBox, FALSE);
	  /* Nouvel axe horizontal */
	  ComputeAxisRelation (pAb->AbHorizRef, pBox, frame, FALSE);
	  result = TRUE;
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
	}
      /* CHANGEMENT D'AXE VERTICAL */
      if (pAb->AbVertRefChange)
	{
#ifdef _GL
      if (pBox)
	  pBox->VisibleModification = TRUE;   
#endif /* _GL */
	  /* Annulation ancien axe verticale */
	  ClearAxisRelation (pBox, TRUE);
	  /* Nouvel axe vertical */
	  ComputeAxisRelation (pAb->AbVertRef, pBox, frame, TRUE);
	  result = TRUE;
	}
    }
#ifdef _GL  
  FrameUpdating = FrameUpdatingStatus;  
#endif /* _GL */
  return result;
}


/*----------------------------------------------------------------------
   ComputeEnclosing traite les contraintes d'englobement diffe're'es 
  ----------------------------------------------------------------------*/
void                ComputeEnclosing (int frame)
{
   int                 i;
   PtrDimRelations     pDimRel;

   /* Apply all differed WidthPack and HeightPack  */
   PackBoxRoot = NULL;
   pDimRel = DifferedPackBlocks;
   while (pDimRel != NULL)
     {
	/* On traite toutes les boites enregistrees */
	i = 0;
	while (i < MAX_RELAT_DIM)
	  {
	     if (pDimRel->DimRTable[i] == NULL)
		i = MAX_RELAT_DIM;
	     else if (pDimRel->DimRTable[i]->BxAbstractBox == NULL)
		;		/* doesn't exist anymore */
	     else if (pDimRel->DimRSame[i])
		WidthPack (pDimRel->DimRTable[i]->BxAbstractBox, NULL, frame);
	     else
		HeightPack (pDimRel->DimRTable[i]->BxAbstractBox, NULL, frame);
	     /* next entry */
	     i++;
	  }

	/* next block */
	pDimRel = pDimRel->DimRNext;
     }

   /* Free allocated blocks */
   while (DifferedPackBlocks != NULL)
     {
	pDimRel = DifferedPackBlocks;
	DifferedPackBlocks = DifferedPackBlocks->DimRNext;
	FreeDimBlock (&pDimRel);
     }
}

/*----------------------------------------------------------------------
   RebuildConcreteImage reevaluates the document view after a change
   in the frame (size, zoom).
   Redisplay within the frame supposed clean.
  ----------------------------------------------------------------------*/
void RebuildConcreteImage (int frame)
{
   ViewFrame          *pFrame;
   PtrAbstractBox      pAb, pVisibleAb;
   PtrBox              pBox;
   int                 width, height;
   int                 position = 0;
   ThotBool            condition;
   ThotBool            status;

   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrAbstractBox)
     {
       /* close any text editing in progress */
       CloseTextInsertion ();
       /* Box widths may change and the scroll must be recomputed */
       AnyWidthUpdate = TRUE;
       pAb = pFrame->FrAbstractBox;
       if (pAb->AbBox)
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
       
	   pBox = pAb->AbBox;
	   status = pFrame->FrReady;
	   pFrame->FrReady = FALSE;	/* La frame n'est pas affichable */
	   /* Faut-il changer les dimensions de la boite document */
	   if (!pBox->BxContentWidth
	       || (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum))
	     {
	       /* la dimension de la boite depend de la fenetre */
	       condition = ComputeDimRelation (pAb, frame, TRUE);
	       /* S'il y a une regle de minimum il faut la verifier */
	       if (!condition)
		 {
		   GiveEnclosureSize (pAb, frame, &width, &height);
		   ChangeDefaultWidth (pBox, pBox, width, 0, frame);
		 }
	     }
	   if (!pBox->BxContentHeight
	       || (!pAb->AbHeight.DimIsPosition && pAb->AbHeight.DimMinimum))
	     {
	       /* la dimension de la boite depend de la fenetre */
	       condition = ComputeDimRelation (pAb, frame, FALSE);
	       /* S'il y a une regle de minimum il faut la verifier */
	       if (!condition)
		 {
		   GiveEnclosureSize (pAb, frame, &width, &height);
		   ChangeDefaultHeight (pBox, pBox, height, frame);
		 }
	     }
	   
	   /* Faut-il deplacer la boite document dans la fenetre? */
	   ComputePosRelation (pAb->AbHorizPos, pBox, frame, TRUE);
	   ComputePosRelation (pAb->AbVertPos, pBox, frame, FALSE);
	   
	   /* On elimine le scroll horizontal */
	   GetSizesFrame (frame, &width, &height);
	   if (pFrame->FrXOrg != 0)
	     {
	       pFrame->FrXOrg = 0;
	       /* Force le reaffichage */
	       DefClip (frame, 0, 0, width, height);
	     }
	   /* Si la vue n'est pas coupee en tete on elimine le scroll vertical */
	   if (!pAb->AbTruncatedHead && pFrame->FrYOrg != 0)
	     {
	       pFrame->FrYOrg = 0;
	       /* Force le reaffichage */
	       DefClip (frame, 0, 0, width, height);
	     }
	   /* La frame est affichable */
	   pFrame->FrReady = status;
	   /* Traitement des englobements retardes */
	   ComputeEnclosing (frame);

	   /* redisplay the window */
	   if (pVisibleAb)
	     ShowBox (frame, pVisibleAb->AbBox, 0, position);
	   else
	     RedrawFrameBottom (frame, 0, NULL);
	   /* if necessary show the selection */
	   ShowSelection (pFrame->FrAbstractBox, TRUE);
	   
	   /* recompute scrolls */
	   CheckScrollingWidth (frame);
	   UpdateScrollbars (frame);
	   /* Restaure la selection */
	   /*ShowSelection (pAb, FALSE);*/
	 }
     }
}


/*----------------------------------------------------------------------
   ClearFlexibility annule l'e'lasticite' de la boite s'il s'agit     
   d'une boite elastique et les regles de position et de   
   dimension sont a` reevaluer. Traite recusivement        
   les paves fils.                                         
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
#ifndef _GL
	    if (pBox->BxLMargin < 0)
	     DefClip (frame, pBox->BxXOrg + pBox->BxLMargin, pBox->BxYOrg,
		      pBox->BxXOrg + pBox->BxWidth + pBox->BxLMargin,
		      pBox->BxYOrg + pBox->BxHeight);
	    else
	     DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
		      pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
#else  /* _GL */
	    DefRegion (frame, pBox->BxClipX, pBox->BxClipY,
		      pBox->BxClipX + pBox->BxClipW, pBox->BxClipY + pBox->BxClipH);
#endif /*  _GL */
	     /* Annulation de la position */
	     if (pAb->AbHorizPosChange)
	       {
		  ClearPosRelation (pBox, TRUE);
		  XMove (pBox, NULL, -pBox->BxXOrg, frame);
	       }

	     /* Annulation ancienne largeur */
	     ClearDimRelation (pBox, TRUE, frame);
	     /* Reinitialisation du trace reel */
	     MirrorShape (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);
	  }

	if (pBox->BxVertFlex && pAb->AbHeightChange)
	  {
	     if (!pBox->BxHorizFlex || !pAb->AbWidthChange)
	       {
#ifndef _GL
		 if (pBox->BxLMargin < 0)
		   DefClip (frame, pBox->BxXOrg + pBox->BxLMargin, pBox->BxYOrg,
			    pBox->BxXOrg + pBox->BxWidth,
			    pBox->BxYOrg + pBox->BxHeight);
		 else
		   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			    pBox->BxXOrg + pBox->BxWidth,
			    pBox->BxYOrg + pBox->BxHeight);
#else  /* _GL */
	    DefRegion (frame, pBox->BxClipX, pBox->BxClipY,
		      pBox->BxClipX + pBox->BxClipW, pBox->BxClipY + pBox->BxClipH);
#endif /*  _GL */
	       }
	     /* Annulation et reevaluation de la position */
	     if (pAb->AbVertPosChange)
	       {
		  ClearPosRelation (pBox, FALSE);
		  YMove (pBox, NULL, -pBox->BxYOrg, frame);
	       }

	     /* Annulation ancienne hauteur */
	     ClearDimRelation (pBox, FALSE, frame);
	     /* Reinitialisation du trace reel */
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

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox != NULL)
    {
      pFrame->FrReady = FALSE;	/* La frame n'est pas affichable */
      /* Faut-il retirer les marques de selection dans la fenetre */
      CloseInsertion ();
      /*ClearViewSelection (frame);*/
      /* Liberation de la hierarchie */
      RemoveBoxes (pFrame->FrAbstractBox, FALSE, frame);
      pFrame->FrAbstractBox = NULL;
      pFrame->FrReady = TRUE;	/* La frame est affichable */
      DefClip (frame, -1, -1, -1, -1);	/* effacer effectivement */
      DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
		      &pFrame->FrClipXBegin, &pFrame->FrClipYBegin,
		      &pFrame->FrClipXEnd, &pFrame->FrClipYEnd, 1);
     }
}


/*----------------------------------------------------------------------
   IsAbstractBoxUpdated rend la valeur Vrai si les changements ont 
   une re'percution sur le pave englobant.                 
  ----------------------------------------------------------------------*/
static ThotBool IsAbstractBoxUpdated (PtrAbstractBox pAb, int frame)
{
   PtrAbstractBox      pChildAb;
   PtrAbstractBox      pFirstAb;
   PtrAbstractBox      pNewAb;
   PtrLine             pLine;
   PtrBox              pBox;
   PtrBox              pCurrentBox;
   Propagation         propStatus;
   int                 index;
   ThotBool            toUpdate;
   ThotBool            result;
   ThotBool            change;

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
	   result = ComputeUpdates (pAb, frame);
	   /* work is done */
	   BoxInWork[BiwIndex--] = NULL;
	   propStatus = Propagate;
	   Propagate = ToAll;	/* On passe en mode normal de propagation */
	   CheckDefaultPositions (pAb, frame);
	   Propagate = propStatus;
	 }
       /* On traite tous les paves descendants modifies avant le pave lui-meme */
       else
	 {
	   /* On limite l'englobement a la boite courante */
	   pBox = PackBoxRoot;
	   PackBoxRoot = pAb->AbBox;
	   
	   /* Traitement des creations */
	   pChildAb = pAb->AbFirstEnclosed;
	   pNewAb = NULL;
	   toUpdate = FALSE;
	   while (pChildAb != NULL)
	     {
	       if (pChildAb->AbNew)
		 {
		   /* On marque le pave qui vient d'etre cree */
		   /* pour eviter sa reinterpretation         */
		   pChildAb->AbNum = 1;
		   change = IsAbstractBoxUpdated (pChildAb, frame);
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
		 change = IsAbstractBoxUpdated (pChildAb, frame);

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
	   result = ComputeUpdates (pAb, frame);
	   /* work is done */
	   BoxInWork[BiwIndex--] = NULL;
	   if (toUpdate || result)
	     {
	       pCurrentBox = pAb->AbBox;
	       result = TRUE;
	       /* Mise a jour d'un bloc de lignes */
	       if (pFirstAb &&
		   (pCurrentBox->BxType == BoBlock ||
		    pCurrentBox->BxType == BoFloatBlock))
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
			       pLine = SearchLine (pBox);
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
			   pLine = SearchLine (pBox);
			 }
		       else
			 pLine = NULL;
		     }
		   
		   RecomputeLines (pAb, pLine, pAb->AbBox, frame);
		 }
	       /* Mise a jour d'une boite composee */
	       else if (pAb->AbBox->BxType != BoBlock &&
			pAb->AbBox->BxType != BoFloatBlock &&
			pAb->AbBox->BxType != BoGhost &&
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
  CheckScrollingWidth computes the maximum between the document width
  and the width needed to display all its contents.
  ----------------------------------------------------------------------*/
void CheckScrollingWidth (int frame)
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  int                 max, org;
  int                 w, h;

  if  (AnyWidthUpdate)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pFrame->FrAbstractBox;
      GetSizesFrame (frame, &w, &h);
      if (pAb && pAb->AbBox)
	{
	  /* check if the document inherits its contents */
	  pBox = pAb->AbBox;
	  max = pBox->BxWidth;
	  org = pBox->BxXOrg;
	  if (!pBox->BxContentWidth)
	    {
	      /* take the first leaf box */
	      pBox = pBox->BxNext;
	      while (pBox)
		{
		  /* check if this box is displayed outside the document box */
		  if (pBox->BxXOrg + pBox->BxWidth > max)
		    max = pBox->BxXOrg + pBox->BxWidth;
		  if (pBox->BxXOrg < org)
		    org = pBox->BxXOrg;
		  pBox = pBox->BxNext;
		}
	    }
	  FrameTable[frame].FrScrollOrg = org;
	  FrameTable[frame].FrScrollWidth = max - org;
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
   Document            document;
   PtrAbstractBox      pParentAb;
   PtrAbstractBox      pChildAb;
   ViewFrame          *pFrame;
   PtrLine             pLine;
   PtrBox              pBox;
   PtrBox              pParentBox;
   PtrBox              pChildBox;
   DisplayMode         saveMode;
   int                 savevisu = 0;
   int                 savezoom = 0;
   ThotBool            change;
   ThotBool            result;
   ThotBool            lock = TRUE;

#ifdef _WINDOWS
     WIN_GetDeviceContext (frame);
#endif /* _WINDOWS */
   result = TRUE;
   document = FrameTable[frame].FrDoc;
   if (document == 0)
     return result;

   pLine = NULL;
   /* Pas de pave Engen parametre */
   if (pAb && frame >= 1 && frame <= MAX_FRAME)
     {
	pFrame = &ViewFrameTable[frame - 1];
	/* La vue n'est pas cree a la racine */
	if (pFrame->FrAbstractBox == NULL &&
	    (pAb->AbEnclosing || pAb->AbPrevious || pAb->AbNext))
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_VIEW_MODIFIED_BEFORE_CREATION);
	/* On detruit toute la vue */
	else if (pAb->AbEnclosing == NULL && pAb->AbDead)
	  {
	     if (pAb == pFrame->FrAbstractBox)
		ClearConcreteImage (frame);
	     else
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_VIEW_MODIFIED_BEFORE_CREATION);
	  }
	/* La vue est deja cree */
	else if (pFrame->FrAbstractBox && pAb->AbEnclosing == NULL && pAb->AbNew)
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_OLD_VIEW_NOT_REPLACED);
	/* Dans les autres cas */
	/* nothing to be done if in mode NoComputedDisplay */
	else if (documentDisplayMode[document - 1] != NoComputedDisplay)
	  {
	     /* Traitement de la premiere creation */
	     if (pFrame->FrAbstractBox == NULL)
	       {
		  DefClip (frame, 0, 0, 0, 0);
		  pFrame->FrXOrg = 0;
		  pFrame->FrYOrg = 0;
		  pFrame->FrAbstractBox = pAb;
		  pFrame->FrSelectOneBox = FALSE;
		  pFrame->FrSelectionBegin.VsBox = NULL;
		  pFrame->FrSelectionEnd.VsBox = NULL;
		  pFrame->FrReady = TRUE;
		  pFrame->FrSelectShown = FALSE;
	       }

	     saveMode = documentDisplayMode[document - 1];
	     if (saveMode == DisplayImmediately)
	       documentDisplayMode[document - 1] = DeferredDisplay;

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

	     /* On prepare la mise a jour d'un bloc de lignes */
	     if ((pParentAb != NULL) && (pParentAb->AbBox != NULL))
	       {
		  /* differ enclosing rules for ancestor boxes */
		  PackBoxRoot = pParentAb->AbBox;
		  while (pParentAb->AbBox->BxType == BoGhost)
		    /* get the enclosing block */
		    pParentAb = pParentAb->AbEnclosing;

		  /* On prepare la mise a jour d'un bloc de lignes */
		  if (pParentAb->AbBox->BxType == BoBlock ||
		      pParentAb->AbBox->BxType == BoFloatBlock ||
		      pParentAb->AbBox->BxType == BoGhost)
		    {
		     if (pAb->AbNew || pAb->AbDead)
		       {
			  /* need to rebuild lines starting form
			     the previous box */
			  pChildAb = pAb->AbPrevious;
			  if (pChildAb == NULL || pChildAb->AbBox == NULL)
			    /* all l ines */
			    pLine = NULL;
			  else
			    {
			       pBox = pChildAb->AbBox;
			       if (pBox->BxType == BoSplit ||
				   pBox->BxType == BoMulScript)
				  while (pBox->BxNexChild)
				     pBox = pBox->BxNexChild;
			       pLine = SearchLine (pBox);
			    }
		       }
		     else
		       {
			  /* Il faut refaire la mise en lignes sur la premiere boite contenue */
			  pBox = pAb->AbBox;
			  if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
			     pBox = pBox->BxNexChild;
			  pLine = SearchLine (pBox);
		       }
		    }
	       }

	     /* lock the table formatting */
	     if (ThotLocalActions[T_islock])
	       {
		 (*ThotLocalActions[T_islock]) (&lock);
		 if (!lock)
		   /* table formatting is not loked, lock it now */
		   (*ThotLocalActions[T_lock]) ();
	       }
	     /* Il faut annuler l'elasticite des boites dont les regles */
	     /* de position et de dimension sont a reevaluer, sinon on  */
	     /* risque d'alterer les nouvelles regles de position et de */
	     /* dimension en propageant les autres modifications sur    */
	     /* ces anciennes boites elastiques de l'image abstraite    */
	     ClearFlexibility (pAb, frame);

	     /* On traite toutes les modifications signalees */
	     change = IsAbstractBoxUpdated (pAb, frame);
       
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
	     if (change && pParentAb != NULL)
	       {
		  /* On saute les boites fantomes de la mise en lignes */
		  if (pParentAb->AbBox->BxType == BoGhost)
		    {
		       while (pParentAb->AbBox->BxType == BoGhost)
			  pParentAb = pParentAb->AbEnclosing;
		       pLine = pParentAb->AbBox->BxFirstLine;
		    }

		  pParentBox = pParentAb->AbBox;
		  /* Mise a jour d'un bloc de lignes */
		  if (pParentBox->BxType == BoBlock ||
		      pParentBox->BxType == BoFloatBlock)
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

		       WidthPack (pParentAb, NULL, frame);
		       HeightPack (pParentAb, NULL, frame);
		    }
	       }

	     /* restore the current mode and  update tables if necessary */
	     if (saveMode == DisplayImmediately)
	       documentDisplayMode[document - 1] = saveMode;
	      if (!lock)
		/* unlock table formatting */
		(*ThotLocalActions[T_unlock]) ();
	     /* Est-ce que l'on a de nouvelles boites dont le contenu est */
	     /* englobe et depend de relations hors-structure ?           */
	     ComputeEnclosing (frame);

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
#ifdef _WINDOWS
     WIN_ReleaseDeviceContext ();
#endif /* _WINDOWS */
   return result;
}




