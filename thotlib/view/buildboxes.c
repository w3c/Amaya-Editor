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
 * This module computes Concrete Images.
 *
 * Authors: I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
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

#define MAX_BOX_INWORK 10
static PtrAbstractBox  BoxInWork[MAX_BOX_INWORK];
static int             BiwIndex = 0;
static ThotBool        AnyWidthUpdate = FALSE;

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
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
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


/*----------------------------------------------------------------------
  SearchNextAbsBox returns the first child or the next sibling or the
  next sibling of the father.
  When pRoot is not Null, the returned abstract box has to be included
  within the pRoot.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox      SearchNextAbsBox (PtrAbstractBox pAb, PtrAbstractBox pRoot)
#else  /* __STDC__ */
PtrAbstractBox      SearchNextAbsBox (pAb, pRoot)
PtrAbstractBox      pAb;
PtrAbstractBox      pRoot;

#endif /* __STDC__ */
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
#ifdef __STDC__
PtrAbstractBox      GetParentCell (PtrBox pBox)
#else  /* __STDC__ */
PtrAbstractBox      GetParentCell (pBox)
PtrBox              pBox;
#endif /* __STDC__ */
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
  GetParentCell returns the enlcosing Draw or NULL.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox      GetParentDraw (PtrBox pBox)
#else  /* __STDC__ */
PtrAbstractBox      GetParentDraw (pBox)
PtrBox              pBox;
#endif /* __STDC__ */
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
	   if (TypeHasException (ExcIsDraw, pAb->AbElement->ElTypeNumber, pAb->AbElement->ElStructSchema))
	     found = TRUE;
	   else
	     pAb = pAb->AbEnclosing;
	 }
     }
   return (pAb);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetControlPoints (float x, float y, float l1, float l2, float theta1, float theta2, C_points * cp)
#else  /* __STDC__ */
static void         SetControlPoints (x, y, l1, l2, theta1, theta2, cp)
float               x, y, l1, l2, theta1, theta2;
C_points           *cp;

#endif /* __STDC__ */
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
#ifdef __STDC__
C_points           *ComputeControlPoints (PtrTextBuffer buffer, int nb)
#else  /* __STDC__ */
C_points           *ComputeControlPoints (buffer, nb)
PtrTextBuffer       buffer;
int                 nb;

#endif /* __STDC__ */
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
   x1 = (float) PointToPixel (pBuffer->BuPoints[j].XCoord / 1000);
   y1 = (float) PointToPixel (pBuffer->BuPoints[j].YCoord / 1000);
   j++;
   x2 = (float) PointToPixel (pBuffer->BuPoints[j].XCoord / 1000);
   y2 = (float) PointToPixel (pBuffer->BuPoints[j].YCoord / 1000);
   if (nb < 3)
     {
	/* cas particulier des courbes avec 2 points */
	x3 = x2;
	y3 = y2;
     }
   else
     {
	j++;
	x3 = (float) PointToPixel (pBuffer->BuPoints[j].XCoord / 1000);
	y3 = (float) PointToPixel (pBuffer->BuPoints[j].YCoord / 1000);
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
	     x3 = (float) PointToPixel (pBuffer->BuPoints[j].XCoord / 1000);
	     y3 = (float) PointToPixel (pBuffer->BuPoints[j].YCoord / 1000);
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
   GiveTextParams calcule les dimensions : hauteur, base, largeur et     
   nombre de blancs contenus (nSpaces).                    
   nSpaces contient initialement 0 si la largeur           
   du blanc est celle de la fonte sinon la valeur imposee. 
   width contient initialement l'index du premier          
   caractere du texte.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GiveTextParams (PtrTextBuffer pBuffer, int nChars, ptrfont font, int *width, int *nSpaces)
#else  /* __STDC__ */
void                GiveTextParams (pBuffer, nChars, font, width, nSpaces)
PtrTextBuffer       pBuffer;
int                 nChars;
ptrfont             font;
int                *width;
int                *nSpaces;

#endif /* __STDC__ */
{
   int                 i, j;
   int                 charWidth;
   int                 spaceWidth;
   UCHAR_T               car;

   /* Calcule la largeur des blancs */
   if (*nSpaces == 0)
      spaceWidth = CharacterWidth (_SPACE_, font);
   else
      spaceWidth = *nSpaces;
   i = *width;			/* Index dans le buffer */
   *nSpaces = 0;
   *width = 0;

   j = 1;
   while (j <= nChars)
     {
	/* On traite les differents caracteres */
	car = (UCHAR_T) (pBuffer->BuContent[i - 1]);
	if (car == _SPACE_)
	  {
	     (*nSpaces)++;	/* caractere blanc */
	     charWidth = spaceWidth;
	  }
	else
	   charWidth = CharacterWidth (car, font);
	*width += charWidth;
	/* Caractere suivant */
	if (i >= pBuffer->BuLength)
	  {
	     pBuffer = pBuffer->BuNext;
	     if (pBuffer == NULL)
		j = nChars;
	     i = 1;
	  }
	else
	   i++;
	if (car != EOS)
	  j++;
     }
}


/*----------------------------------------------------------------------
   GivePictureSize evalue les dimensions de la boite du pave Picture. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GivePictureSize (PtrAbstractBox pAb, int zoom, int *width, int *height)
#else  /* __STDC__ */
static void         GivePictureSize (pAb, zoom, width, height)
PtrAbstractBox      pAb;
int                 zoom;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   PtrBox              pBox;
   PictInfo           *picture;

   pBox = pAb->AbBox;
   picture = (PictInfo *) pBox->BxPictInfo;
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
   GiveSymbolSize evalue les dimensions de la boite du pave Symbol.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GiveSymbolSize (PtrAbstractBox pAb, int *width, int *height)
#else  /* __STDC__ */
void                GiveSymbolSize (pAb, width, height)
PtrAbstractBox      pAb;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   ptrfont             font;
   int                 hfont;
   float               value;

   font = pAb->AbBox->BxFont;
   hfont = FontHeight (font);
   if (pAb->AbVolume == 0)
     {
	/* Symbol vide */
	*width = CharacterWidth (_SPACE_, font);
	*height = hfont * 2;
     }
   else
     {
	*height = hfont * 2;
	value = 1 + ((float) (pAb->AbBox->BxHeight * 0.6) / (float) hfont);
	switch (pAb->AbShape)
	      {
		 case 'c':	/*integrale curviligne */
		 case 'i':	/*integrale */
		    *width = (int) ((float) (CharacterWidth (231, font)) * value);
		    *height += hfont;
		    break;
		 case 'd':	/*integrale double */
		    *width = CharacterWidth (231, font) + CharacterWidth (231, font) / 2;
		    *height += hfont;
		    break;
		 case 'r':	/*racine */
		    *width = hfont;
		    break;
		 case 'S':	/*sigma */
		 case 'P':	/*pi */
		 case 'I':	/*intersection */
		 case 'U':	/*union */
		    /* width := CharacterWidth(229, font) + 4; */
		    *width = CharacterWidth (229, font);
		    /* height := FontHeight(font); */
		    *height = hfont;
		    break;
		 case '<':
		 case 'h':
		 case '>':
		    *width = *height;
		    *height = hfont / 3;
		    break;
		 case '(':
		 case ')':
		 case '[':
		 case ']':
		 case '{':
		 case '}':
		    *width = (int) ((float) CharacterWidth (230, font) * value);
		    *height = hfont;
		    break;
		 case '^':
		 case 'v':
		 case 'V':
		    *width = CharacterWidth (109, font);	/*'m' */
		    break;
		 default:
		    *width = CharacterWidth (_SPACE_, font);
		    *height = hfont;
		    break;
	      }
     }
}


/*----------------------------------------------------------------------
   GiveGraphicSize evalue les dimensions de la boite du pave Graphique 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GiveGraphicSize (PtrAbstractBox pAb, int *width, int *height)
#else  /* __STDC__ */
void                GiveGraphicSize (pAb, width, height)
PtrAbstractBox      pAb;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   ptrfont             font;
   int                 hfont;

   font = pAb->AbBox->BxFont;
   *width = CharacterWidth (109, font);	/*'m' */
   hfont = FontHeight (font);
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
   GivePolylineSize evalue les dimensions de la boite du PolyLine.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GivePolylineSize (PtrAbstractBox pAb, int *width, int *height)
#else  /* __STDC__ */
static void         GivePolylineSize (pAb, width, height)
PtrAbstractBox      pAb;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   int                 max;
   PtrTextBuffer       pBuffer;

   /* Si le pave est vide on prend une dimension par defaut */
   pBuffer = pAb->AbPolyLineBuffer;
   *width = 1;
   *height = 1;
   max = pAb->AbVolume;
   if (max > 0 || pBuffer != NULL)
     {
	/* La largeur est donnee par le point limite */
	*width = pBuffer->BuPoints[0].XCoord;
	/* La hauteur est donnee par le point limite */
	*height = pBuffer->BuPoints[0].YCoord;

	/* Convertit en pixels */
	*width = PointToPixel (*width / 1000);
	*height = PointToPixel (*height / 1000);
     }
}


/*----------------------------------------------------------------------
   FreePolyline libe`re la liste des buffers attache's a` la       
   la boi^te Polyline.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreePolyline (PtrBox pBox)
#else  /* __STDC__ */
static void         FreePolyline (pBox)
PtrBox              pBox;

#endif /* __STDC__ */
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
	/* libere les points de controle */
	free (pBox->BxPictInfo);
	pBox->BxPictInfo = NULL;
     }
}


/*----------------------------------------------------------------------
   GiveTextSize evalue les dimensions de la boite du pave Texte.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GiveTextSize (PtrAbstractBox pAb, int *width, int *height, int *nSpaces)
#else  /* __STDC__ */
void                GiveTextSize (pAb, width, height, nSpaces)
PtrAbstractBox      pAb;
int                *width;
int                *height;
int                *nSpaces;

#endif /* __STDC__ */
{
   ptrfont             font;
   int                 nChars;

   font = pAb->AbBox->BxFont;
   *height = FontHeight (font);

   /* Est-ce que le pave est vide ? */
   nChars = pAb->AbVolume;
   if (nChars == 0)
     {
	*width = 2;
	*nSpaces = 0;
     }
   else
     {
	/* Texte -> Calcule directement ses dimensions */
	*width = 1;		/* Index du premier caractere a traiter */
	*nSpaces = 0;		/* On prend la largeur reelle du blanc */
	GiveTextParams (pAb->AbText, nChars, font, width, nSpaces);
     }
}


/*----------------------------------------------------------------------
   GiveEnclosureSize e'value les dimensions du contenu du pave' compose'    
   pAb dans la fenetree^tre frame.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GiveEnclosureSize (PtrAbstractBox pAb, int frame, int *width, int *height)
#else  /* __STDC__ */
void                GiveEnclosureSize (pAb, frame, width, height)
PtrAbstractBox      pAb;
int                 frame;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrAbstractBox      pFirstAb;
   PtrAbstractBox      pCurrentAb;
   PtrBox              pChildBox, pBox;
   PtrBox              pCurrentBox;
   int                 val, x, y;
   ThotBool            still;

   pBox = NULL;
   pCurrentBox = pAb->AbBox;
   /* PcFirst fils vivant */
   pFirstAb = pAb->AbFirstEnclosed;
   still = TRUE;
   while (still)
     if (pFirstAb == NULL)
       still = FALSE;
     else if (pFirstAb->AbDead)
       pFirstAb = pFirstAb->AbNext;
     else
       still = FALSE;
   
   /* Il est inutile de calculer les dimensions d'une boite eclatee */
   if (pCurrentBox->BxType == BoGhost)
     {
       *width = 0;
       *height = 0;
       /* Si la regle secable a supplante la regle de mise en lignes */
       if (pAb->AbInLine)
	 {
	   pCurrentBox->BxFirstLine = NULL;
	   pCurrentBox->BxLastLine = NULL;
	 }
     }
   /* La boite composee est vide */
   else if (pFirstAb == NULL)
     {
       *width = 0;
       *height = 0;
       if (pAb->AbInLine)
	 {
	   pCurrentBox->BxType = BoBlock;
	   pCurrentBox->BxFirstLine = NULL;
	   pCurrentBox->BxLastLine = NULL;
	 }
     }
   else if (pAb->AbInLine)
     /* La boite composee est mise en ligne */
     {
       if (pCurrentBox->BxType == BoBlock && pCurrentBox->BxFirstLine != NULL)
	 /* we have to reformat the block of lines */
	 RecomputeLines (pAb, NULL, NULL, frame);
       else
	 {
	   pCurrentBox->BxType = BoBlock;
	   pCurrentBox->BxFirstLine = NULL;
	   pCurrentBox->BxLastLine = NULL;
	   ComputeLines (pCurrentBox, frame, height);
	   if (pCurrentBox->BxContentWidth)
	     /* it's an extensible line */
	     pCurrentBox->BxWidth = pCurrentBox->BxMaxWidth;
	   else
	     {
	       /* Si la largeur du contenu depasse le minimum */
	       if (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum
		   && pCurrentBox->BxMaxWidth > pCurrentBox->BxWidth)
		 {
		   /* Il faut prendre la largeur du contenu */
		   pCurrentBox->BxContentWidth = TRUE;
		   RecomputeLines (pAb, NULL, NULL, frame);
		 }
	     }
	 }
       *width = pCurrentBox->BxWidth;
     }
   /* La boite est une composition geometrique */
   else
     {
       x = pCurrentBox->BxXOrg;
       *width = x;
       y = pCurrentBox->BxYOrg;
       *height = y;
       /* On verifie l'englobement s'il est impose */
       pChildAb = pFirstAb;
       while (pChildAb != NULL)
	 {
	   pChildBox = pChildAb->AbBox;
	   if (!pChildAb->AbDead && pChildBox != NULL)
	     {
	       /* Faut-il mettre a jour la position du pave ? */
	       if (pCurrentBox->BxContentWidth && pChildAb->AbHorizEnclosing && pChildBox->BxXOrg < x
		   && pChildAb->AbWidth.DimAbRef != pAb)
		 {
		   /* Est-ce que la boite est mobile ? */
		   pBox = GetHPosRelativePos (pChildBox, NULL);
		   if (pBox != NULL)
		     {
		       pCurrentAb = pBox->BxAbstractBox;
		       if (pCurrentAb->AbHorizPos.PosAbRef == NULL)
			 XMove (pChildBox, NULL, x - pChildBox->BxXOrg, frame);
		     }
		 }
	       /* Faut-il mettre a jour la position du pave ? */
	       if (pCurrentBox->BxContentHeight && pChildAb->AbVertEnclosing && pChildBox->BxYOrg < y
		   && pChildAb->AbHeight.DimAbRef != pAb)
		 {
		   /* Est-ce que la boite est mobile ? */
		   pBox = GetVPosRelativeBox (pChildBox, NULL);
		   if (pBox != NULL)
		     {
		       pCurrentAb = pBox->BxAbstractBox;
		       if (pCurrentAb->AbVertPos.PosAbRef == NULL)
			 YMove (pChildBox, NULL, y - pChildBox->BxYOrg, frame);
		     }
		 }
	     }
	   pChildAb = pChildAb->AbNext;
	 }
       /* On evalue les dimensions reelles de la boite composee */
       pChildAb = pFirstAb;
       while (pChildAb != NULL)
	 {
	   pChildBox = pChildAb->AbBox;
	   if (!pChildAb->AbDead && pChildBox != NULL)
	     {
	       /* La largeur de la boite composee peut dependre du pave ? */
	       if ((pChildAb->AbWidth.DimAbRef != pAb && pChildAb->AbHorizEnclosing)
		   || (!pChildAb->AbWidth.DimIsPosition && pChildAb->AbWidth.DimMinimum))
		 {
		   if (pChildBox->BxXOrg < 0)
		     val = pChildBox->BxWidth;
		   else
		     val = pChildBox->BxXOrg + pChildBox->BxWidth;
		   if (val > *width)
		     *width = val;
		 }
	       /* La hauteur de la boite composee peut dependre du pave ? */
	       if ((pChildAb->AbHeight.DimAbRef != pAb && pChildAb->AbVertEnclosing)
		   || (!pChildAb->AbHeight.DimIsPosition && pChildAb->AbHeight.DimMinimum))
		 {
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
       *width -= x;
       *height -= y;
       /* Decale les boites incluses dont la position depend des dimensions */
       pChildAb = pFirstAb;
       if (Propagate == ToSiblings)
	 while (pChildAb != NULL)
	   {
	     pChildBox = pChildAb->AbBox;
	     if (!pChildAb->AbDead && pChildBox != NULL)
	       {
		 /* La position horizontale du pave depend de la largeur calculee? */
		 if (pCurrentBox->BxContentWidth
		     && !IsXPosComplete (pCurrentBox)
		     && pChildAb->AbHorizPos.PosAbRef == pAb)
		   {
		     /* origine de la boite a deplacer */
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
		 
		 /* La position verticale du pave depend de la hauteur calculee ? */
		 if (pCurrentBox->BxContentHeight
		     && !IsYPosComplete (pCurrentBox)
		     && pChildAb->AbVertPos.PosAbRef == pAb)
		   {
		     val = pChildBox->BxYOrg;	/* origine de la boite a deplacer */
		     if (pChildAb->AbVertPos.PosEdge == HorizMiddle)
		       val += pChildBox->BxHeight / 2;
		     else if (pChildAb->AbVertPos.PosEdge == Bottom)
		       val += pChildBox->BxHeight;
		     
		     if (pChildAb->AbVertPos.PosRefEdge == HorizMiddle)
		       {
			 if (!pChildBox->BxVertFlex)
			   YMove (pChildBox, NULL, y + *height / 2 - val, frame);
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
   
   /* La boite composee est vide ? */
   if (pFirstAb == NULL && pAb->AbVolume == 0)
     {
       GiveTextSize (pAb, &x, &y, &val);
       if (*width == 0)
	 *width = x;
       if (*height == 0)
	 *height = y;
     }
}

/*----------------------------------------------------------------------
   IsAbstractBoxEmpty retourne la valeur Vrai si pAb est vide.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsAbstractBoxEmpty (PtrAbstractBox pAb)
#else  /* __STDC__ */
static ThotBool     IsAbstractBoxEmpty (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
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
#ifdef __STDC__
static PtrAbstractBox PreviousLeafAbstractBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrAbstractBox PreviousLeafAbstractBox (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
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
  SearchPreviousFillBox searchs the previous fill box in current view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAbstractBox SearchPreviousFilledBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrAbstractBox SearchPreviousFilledBox (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
  PtrAbstractBox    pPreviousAb;

  if (pAb == NULL)
    return (NULL);
  else
    {
      pPreviousAb = pAb->AbPrevious;
      /* don't take care of dead abstract boxes and not compound filled boxes */
      while (pPreviousAb != NULL &&
	     (pPreviousAb->AbDead ||
	      pPreviousAb->AbLeafType != LtCompound ||
	      !pPreviousAb->AbFillBox))
	pPreviousAb = pPreviousAb->AbPrevious;

      if (pPreviousAb == NULL && pAb->AbEnclosing != NULL)
        {
	  /* search at parent level */
          pPreviousAb = pAb->AbEnclosing;
          if (!pPreviousAb->AbFillBox)
	    pPreviousAb = SearchPreviousFilledBox (pAb->AbEnclosing);
        }
      return (pPreviousAb);
    }
}


/*----------------------------------------------------------------------
  AddFilledBox adds the box in the filled list if it's not already
  registered.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddFilledBox (PtrBox pBox, PtrBox pMainBox, int frame)
#else  /* __STDC__ */
static void         AddFilledBox (pBox, pMainBox, frame)
PtrBox              pBox;
PtrBox              pMainBox;
int                 frame;
#endif /* __STDC__ */
{
  PtrBox            pFilledBox;
  PtrAbstractBox    pAb;
  int               color;

  if (pBox != pMainBox)
    {
      pFilledBox = pMainBox->BxNextBackground;
      while (pFilledBox != NULL && pFilledBox != pBox)
	pFilledBox = pFilledBox->BxNextBackground;

      if (pFilledBox == NULL)
	{
	  /* the current filled box is not registered */
	  pAb = SearchPreviousFilledBox (pBox->BxAbstractBox);
	  if (pAb == NULL)
	    {
	      /* The new box is the first filled box */
	      pBox->BxNextBackground = pMainBox->BxNextBackground;
	      pMainBox->BxNextBackground = pBox;
	    }
	  else
	    {
	      /* Add it in the list */
	      pFilledBox = pAb->AbBox;
	      pBox->BxNextBackground = pFilledBox->BxNextBackground;
	      pFilledBox->BxNextBackground = pBox;
	    }
	}
    }
  else
    {
      color = pBox->BxAbstractBox->AbBackground;
      if (BackgroundColor[frame] != (ThotColor)color)
        {
          /* change the window background color */
          BackgroundColor[frame] = (ThotColor)color;
          SetMainWindowBackgroundColor (frame, color);
        }
    }
}


/*----------------------------------------------------------------------
  RemoveFilledBox adds the box in the filled list.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveFilledBox (PtrBox pBox, PtrBox pMainBox, int frame)
#else  /* __STDC__ */
static void         RemoveFilledBox (pBox, pMainBox, frame)
PtrBox              pBox;
PtrBox              pMainBox;
int                 frame;
#endif /* __STDC__ */
{
  PtrBox            pFilledBox;

  if (pBox != pMainBox)
    {
      pFilledBox = pMainBox;
      while (pFilledBox != NULL && pFilledBox->BxNextBackground != pBox)
	pFilledBox = pFilledBox->BxNextBackground;

      if (pFilledBox != NULL)
	{
	  /* remove the box from the list */
	  pFilledBox->BxNextBackground = pBox->BxNextBackground;
	  pBox->BxNextBackground = NULL;
	}
    }
  else
    {
      BackgroundColor[frame] = DefaultBColor;
      SetMainWindowBackgroundColor (frame, DefaultBColor);
    }
}

/*----------------------------------------------------------------------
   FontStyleAndWeight
   returns the Highlight value for abstract box pAb, according to its
   FontStyle and FontWeight.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          FontStyleAndWeight (PtrAbstractBox pAb)
#else  /* __STDC__ */
static int          FontStyleAndWeight (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
   int	i;

   i = 0;
   if (pAb)
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
   return i;
}

/*----------------------------------------------------------------------
   CreateBox cree la boite qui est associee au pave donne en       
   parametre et initialise son contenu :                   
   - Calcule sa place en caracteres dans le document tout  
   entier.                                                 
   - Calcule les dimensions contraintes (heritees).        
   - Cree les boites des paves englobes si le pave n'est   
   pas terminal.                                           
   - Calcule les dimensions reelles de la boite et choisit 
   entre les dimensions reelles et contraintes.            
   - Calcule les axes de references de la boite.           
   - Positionnne l'origine (haut gauche) de la boite par   
   rapport a` la boite englobante si inLines est faux.     
   Les boites terminales sont doublement (avant et arriere)
   chainees a` partir des champs BxPrevious BxNext de la   
   boite du pave racine.                                   
   On met a jour l'adresse de la boite dans le pave.       
   La fonction rend l'adresse de la boite.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrBox       CreateBox (PtrAbstractBox pAb, int frame, ThotBool inLines, int *carIndex)
#else  /* __STDC__ */
static PtrBox       CreateBox (pAb, frame, inLines, carIndex)
PtrAbstractBox      pAb;
int                 frame;
ThotBool            inLines;
int                *carIndex;

#endif /* __STDC__ */
{
   PtrSSchema          pSS;
   PtrAbstractBox      pChildAb;
   PtrBox              pBox;
   PtrBox              pMainBox;
   PtrBox              pCurrentBox;
   TypeUnit            unit;
   ptrfont             font;
   PictInfo           *picture;
   BoxType             tableType;
   CHAR_T                alphabet = 'L';
   int                 width, i;
   int                 height;
   ThotBool            enclosedWidth;
   ThotBool            enclosedHeight;
   ThotBool            split;

   if (pAb->AbDead)
      return (NULL);

   /* by default it's not a table element */
   tableType = BoComplete;
   pSS = pAb->AbElement->ElStructSchema;
   /* Chargement de la fonte attachee au pave */
   height = pAb->AbSize;
   unit = pAb->AbSizeUnit;
   height += ViewFrameTable[frame - 1].FrMagnification;

   if (pAb->AbLeafType == LtText)
     if (pAb->AbLanguage == EOS)
       alphabet = 'L';
     else
       alphabet = TtaGetAlphabet (pAb->AbLanguage);
   else if (pAb->AbLeafType == LtSymbol)
     alphabet = 'G';
   else if (pAb->AbLeafType == LtCompound)
     alphabet = 'L';
   /* teste l'unite */
   font = ThotLoadFont (alphabet, pAb->AbFont, FontStyleAndWeight(pAb),
			height, unit, frame);

   /* Creation */
   pCurrentBox = pAb->AbBox;
   if (pCurrentBox == NULL)
     {
	pCurrentBox = GetBox (pAb);
	pAb->AbBox = pCurrentBox;
     }

   if (pCurrentBox != NULL)
     {
        /* pMainBox points to the root box of the view */
        pMainBox = ViewFrameTable[frame - 1].FrAbstractBox->AbBox;
	pCurrentBox->BxFont = font;
	pCurrentBox->BxUnderline = pAb->AbUnderline;
	pCurrentBox->BxThickness = pAb->AbThickness;
	split = FALSE;		/* A priori la boite n'est pas eclatee */

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

	enclosedWidth = ComputeDimRelation (pAb, frame, TRUE);
	enclosedHeight = ComputeDimRelation (pAb, frame, FALSE);
	pCurrentBox->BxXToCompute = FALSE;
	pCurrentBox->BxYToCompute = FALSE;

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
		  if (pBox != NULL)
		    {
		      pBox->BxNext = pCurrentBox;
		      if (pBox->BxType == BoPiece)
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
	else if (pCurrentBox->BxAbstractBox->AbEnclosing != NULL &&
		 pCurrentBox->BxAbstractBox->AbEnclosing->AbBox != NULL &&
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
		    GiveTextSize (pAb, &width, &height, &i);
		    pCurrentBox->BxNSpaces = i;
		    break;
		 case LtPicture:
		    pCurrentBox->BxType = BoPicture;
		    picture = (PictInfo *) pAb->AbPictInfo;
		    pCurrentBox->BxPictInfo = pAb->AbPictInfo;
		    if (!pAb->AbPresentationBox && pAb->AbVolume != 0 && pCurrentBox->BxPictInfo != NULL)
		      {

			 /* box size has to be positive */
			 if (pCurrentBox->BxWidth < 0)
			    ChangeWidth (pCurrentBox, pCurrentBox, NULL, 10 - pCurrentBox->BxWidth, 0, frame);
			 if (pCurrentBox->BxHeight < 0)
			    ChangeHeight (pCurrentBox, pCurrentBox, NULL, 10 - pCurrentBox->BxHeight, frame);
		      }

		    if (picture->PicPixmap == None)
		       LoadPicture (frame, pCurrentBox, picture);
		    GivePictureSize (pAb, ViewFrameTable[frame -1].FrMagnification, &width, &height);
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
		    break;
		 case LtPolyLine:
		    /* Prend une copie des points de controle */
		    pCurrentBox->BxBuffer = CopyText (pAb->AbPolyLineBuffer, NULL);
		    pCurrentBox->BxNChars = pAb->AbVolume;	/* Nombre de points */
		    pCurrentBox->BxPictInfo = NULL;
		    pCurrentBox->BxXRatio = 1;
		    pCurrentBox->BxYRatio = 1;
		    GivePolylineSize (pAb, &width, &height);
		    break;
		 case LtCompound:
		   if (TypeHasException (ExcIsTable, pAb->AbElement->ElTypeNumber, pSS))
		     {
		       tableType = BoTable;
		       pCurrentBox->BxType = tableType;
		       pCurrentBox->BxColumns = NULL;
		       pCurrentBox->BxRows = NULL;
		       pCurrentBox->BxMaxWidth = 0;
		       pCurrentBox->BxMinWidth = 0;
		     }
		   else if (TypeHasException (ExcIsColHead, pAb->AbElement->ElTypeNumber, pSS) &&
			    !pAb->AbWidth.DimIsPosition &&
			    pAb->AbWidth.DimAbRef != pAb->AbEnclosing)
		     {
		       tableType = BoColumn;
		       pCurrentBox->BxType = tableType;
		       pCurrentBox->BxTable = NULL;
		       pCurrentBox->BxRows = NULL;
		       pCurrentBox->BxMaxWidth = 0;
		       pCurrentBox->BxMinWidth = 0;
		     }
		   else if (TypeHasException (ExcIsRow, pAb->AbElement->ElTypeNumber, pSS))
		     {
		       tableType = BoRow;
		       pCurrentBox->BxType = tableType;
		       pCurrentBox->BxTable = NULL;
		       pCurrentBox->BxRows = NULL;
		       pCurrentBox->BxMaxWidth = 0;
		       pCurrentBox->BxMinWidth = 0;
		     }
		   else if (TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber, pSS))
		     {
		       tableType = BoCell;
		       pCurrentBox->BxType = tableType;
		       pCurrentBox->BxTable = NULL;
		       pCurrentBox->BxRows = NULL;
		       pCurrentBox->BxMaxWidth = 0;
		       pCurrentBox->BxMinWidth = 0;
		     }

		    /* Si le pave est mis en ligne et secable -> la boite est eclatee */
		    if (inLines && pAb->AbAcceptLineBreak && pAb->AbFirstEnclosed != NULL)
		      {
			 split = TRUE;
			 pCurrentBox->BxType = BoGhost;
		      }
		    /* Is there a background image ? */
		    if (pAb->AbPictBackground != NULL)
		      {
			/* force filling */
			pAb->AbFillBox = TRUE;
			/* load the picture */
			LoadPicture (frame, pCurrentBox, (PictInfo *) pAb->AbPictBackground);
		      }
		    /* Is it a filled box ? */
		    if (pAb->AbFillBox)
		      /* register the box */
		      AddFilledBox (pCurrentBox, pMainBox, frame);

		    /* Il faut creer les boites des paves inclus */
		    pChildAb = pAb->AbFirstEnclosed;
		    while (pChildAb != NULL)
		      {
			 pBox = CreateBox (pChildAb, frame, (ThotBool) (split || pAb->AbInLine), carIndex);
			 pChildAb = pChildAb->AbNext;
		      }
		    GiveEnclosureSize (pAb, frame, &width, &height);
		    break;
		 default:
		    break;
	      }

	/* Dimensionnement de la boite par le contenu ? */
	ChangeDefaultWidth (pCurrentBox, pCurrentBox, width, 0, frame);
	/* Il est possible que le changement de largeur de la boite modifie */
	/* indirectement (parce que la boite contient un bloc de ligne) la  */
	/* hauteur du contenu de la boite.                                  */
	if (enclosedWidth && enclosedHeight && pAb->AbLeafType == LtCompound)
	  GiveEnclosureSize (pAb, frame, &width, &height);
	ChangeDefaultHeight (pCurrentBox, pCurrentBox, height, frame);
	/* Positionnement des axes de la boite construite */
	ComputeAxisRelation (pAb->AbVertRef, pCurrentBox, frame, TRUE);

	/* On traite differemment la base d'un bloc de lignes  */
	/* s'il depend de la premiere boite englobee           */
	if (pAb->AbLeafType != LtCompound
	    || !pAb->AbInLine
	    || pAb->AbHorizRef.PosAbRef != pAb->AbFirstEnclosed)
	   ComputeAxisRelation (pAb->AbHorizRef, pCurrentBox, frame, FALSE);

	/* Positionnement des origines de la boite construite */
	i = 0;
	if (!inLines)
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
	       /* the real position of the box depends of its horizontal reference axis */
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
   return (pCurrentBox);
}

/*----------------------------------------------------------------------
  SearchEnclosingType look for the enclosing table or row box
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox   SearchEnclosingType (PtrAbstractBox pAb, BoxType box_type)
#else
PtrAbstractBox   SearchEnclosingType (pAb, box_type)
PtrAbstractBox   pAb;
BoxType          colrow;
#endif
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
      else if (pAb->AbBox->BxType == box_type)
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
   SearchLine cherche l'adresse de la ligne englobant la boite       
   designee.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrLine             SearchLine (PtrBox pBox)
#else  /* __STDC__ */
PtrLine             SearchLine (pBox)
PtrBox              pBox;

#endif /* __STDC__ */
{
   PtrLine             pLine;
   PtrBox              pBoxPiece;
   PtrBox              pBoxInLine;
   PtrBox              pCurrentBox;
   PtrAbstractBox      pAb;
   ThotBool            still;

   /* Recherche la ligne englobante */
   pLine = NULL;
   pAb = NULL;
   if (pBox != NULL)
     {
	if (pBox->BxAbstractBox != NULL)
	   pAb = pBox->BxAbstractBox->AbEnclosing;
	if (pAb != NULL && pAb->AbNotInLine)
	  pAb = NULL;
     }

   /* On regarde si la boite appartient a un bloc de lignes */
   if (pAb != NULL)
     {
	if (pAb->AbBox == NULL)
	   pAb = NULL;
	/* Est-ce une boite fille d'une boite eclatee ? */
	else if (pAb->AbBox->BxType == BoGhost)
	  {
	     /* Si oui on saute les niveaux des paves eclates */
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
	/* Est-ce que la boite est directement incluse dans une ligne */
	else if (!pAb->AbInLine)
	   pAb = NULL;
     }

   if (pAb != NULL)
     {
	pCurrentBox = pAb->AbBox;
	pLine = pCurrentBox->BxFirstLine;
	/* Look for the line which includes the current box */
	still = TRUE;
	while (still && pLine != NULL)
	  {
	     /* On recherche la boite dans la ligne */
	     if (pLine->LiFirstPiece != NULL)
		pBoxInLine = pLine->LiFirstPiece;
	     else
		pBoxInLine = pLine->LiFirstBox;

	     /* Boucle sur les boites de la ligne */
	     do
	       {
		  if (pBoxInLine->BxType == BoSplit)
		     pBoxPiece = pBoxInLine->BxNexChild;
		  else
		     pBoxPiece = pBoxInLine;
		  if (pBoxPiece == pBox)
		    {
		       /* On a trouve la ligne */
		       still = FALSE;
		       pBoxPiece = pLine->LiLastBox;
		    }
		  /* Sinon on passe a la boite suivante */
		  else
		     pBoxInLine = GetNextBox (pBoxInLine->BxAbstractBox);
	       }
	     while (pBoxPiece != pLine->LiLastBox
		    && pBoxPiece != pLine->LiLastPiece
		    && pBoxInLine != NULL);

	     if (still)
	       /* On passe a la ligne suivante */
	       pLine = pLine->LiNext;
	  }
     }
   return pLine;
}


/*----------------------------------------------------------------------
   BoxUpdate met a jour les informations d'une boite terminale (nombre
   de caracteres, nombre de blancs, largeur).              
   La largeur ajoutee est wDelta pour la boite coupee ou   
   entiere, adjustDelta ou wDelta (si adjustDelta est 0)   
   pour la boite de piece.                                 
   Si splitBox est vrai la mise a jour modifie la coupure entre
   deux boites, donc seule la boite coupee est mise a jour.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BoxUpdate (PtrBox pBox, PtrLine pLine, int charDelta, int spaceDelta, int wDelta, int adjustDelta, int hDelta, int frame, ThotBool splitBox)
#else  /* __STDC__ */
void                BoxUpdate (pBox, pLine, charDelta, spaceDelta, wDelta, adjustDelta, hDelta, frame, splitBox)
PtrBox              pBox;
PtrLine             pLine;
int                 charDelta;
int                 spaceDelta;
int                 wDelta;
int                 adjustDelta;
int                 hDelta;
int                 frame;
ThotBool            splitBox;

#endif /* __STDC__ */
{
   PtrBox              box1;
   PtrBox              pMainBox;
   Propagation         savpropage;
   PtrAbstractBox      pAb;
   AbPosition         *pPosAb;
   AbDimension        *pDimAb;
   int                 j;

   /* Traitement particulier aux boites de coupure */
   if (pBox->BxType == BoPiece || pBox->BxType == BoDotted)
     {
	/* Mise a jour de sa boite mere (boite coupee) */
	pMainBox = pBox->BxAbstractBox->AbBox;
	pMainBox->BxNChars += charDelta;
	pMainBox->BxNSpaces += spaceDelta;
	pMainBox->BxWidth += wDelta;
	pMainBox->BxHeight += hDelta;
	/* Faut-il mettre a jour la base ? */
	pAb = pMainBox->BxAbstractBox;
	pPosAb = &pAb->AbHorizRef;
	if (pPosAb->PosAbRef == NULL)
	  {
	     j = FontBase (pMainBox->BxFont) - pMainBox->BxHorizRef;
	     MoveHorizRef (pAb->AbBox, NULL, j, frame);
	  }
	else if (pPosAb->PosAbRef == pMainBox->BxAbstractBox)
	   if (pPosAb->PosRefEdge == HorizMiddle)
	      MoveHorizRef (pAb->AbBox, NULL, hDelta / 2, frame);
	   else if (pPosAb->PosRefEdge == Bottom)
	      MoveHorizRef (pAb->AbBox, NULL, hDelta, frame);

	/* Mise a jour des positions des boites suivantes */
	box1 = pBox->BxNexChild;
	while (box1 != NULL)
	  {
	     box1->BxIndChar += charDelta;
	     box1 = box1->BxNexChild;
	  }
     }

   /* Traitement sur la boite passee en parametre */
   savpropage = Propagate;
   pAb = pBox->BxAbstractBox;
   /* Mise a jour de la boite elle-meme */
   if (pAb->AbLeafType == LtText)
     {
	pBox->BxNSpaces += spaceDelta;
	pBox->BxNChars += charDelta;
     }

   /* Est-ce que la largeur de la boite depend de son contenu ? */
   pDimAb = &(pBox->BxAbstractBox->AbWidth);
   if (pBox->BxContentWidth || (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
     /* Blanc entre deux boites de coupure */
     if (splitBox && pLine != NULL)
       {
	 /* Il faut mettre a jour la largeur de la boite coupee */
	 if ((pBox->BxType == BoSplit) || (adjustDelta == 0))
	   pBox->BxWidth += wDelta;
	 else
	   pBox->BxWidth += adjustDelta;
	 /* Puis refaire la mise en lignes */
	 RecomputeLines (pAb->AbEnclosing, pLine, pBox, frame);
       }
     else if (pBox->BxType == BoSplit)
       {
	 /* Box coupee */
	 Propagate = ToSiblings;
	 if (wDelta != 0)
	   ChangeDefaultWidth (pBox, pBox, pBox->BxWidth + wDelta, 0, frame);
	 if (hDelta != 0)
	   ChangeDefaultHeight (pBox, pBox, pBox->BxHeight + hDelta, frame);
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
	   ChangeDefaultWidth (pBox, pBox, pBox->BxWidth + adjustDelta, spaceDelta, frame);
	 else if (wDelta != 0)
	   ChangeDefaultWidth (pBox, pBox, pBox->BxWidth + wDelta, spaceDelta, frame);
	 if (hDelta != 0)
	   ChangeDefaultHeight (pBox, pBox, pBox->BxHeight + hDelta, frame);
       }
   else if (pBox->BxContentHeight || (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum))
     {
       /* La hauteur de la boite depend de son contenu */
       if (hDelta != 0)
	 ChangeDefaultHeight (pBox, pBox, pBox->BxHeight + hDelta, frame);
     }
   else if (pBox->BxWidth > 0 && pBox->BxHeight > 0)
     /* Si la largeur de la boite ne depend pas de son contenu  */
     /* on doit forcer le reaffichage jusqua la fin de la boite */
     DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
   
   Propagate = savpropage;
}


/*----------------------------------------------------------------------
   RemoveBoxes libere toutes les boites correpondant aux paves inclus
   dans pAb y compris celle du pave passe' en parametre    
   toRemake est vrai si la boite doit etre recree          
   immediatement apres.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveBoxes (PtrAbstractBox pAb, ThotBool toRemake, int frame)
#else  /* __STDC__ */
void                RemoveBoxes (pAb, toRemake, frame)
PtrAbstractBox      pAb;
ThotBool            toRemake;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrBox              pCurrentBox, pPieceBox;
   ThotBool            changeSelectBegin;
   ThotBool            changeSelectEnd;

   if (pAb != NULL)
     {
	if (pAb->AbBox != NULL)
	  {
	     /* Liberation des lignes et boites coupees */
	     pCurrentBox = pAb->AbBox;
	     if (pAb->AbLeafType == LtCompound)
		 /* unregister the box */
		 RemoveFilledBox (pCurrentBox, ViewFrameTable[frame - 1].FrAbstractBox->AbBox, frame);

	     if (pCurrentBox->BxType == BoBlock)
		RemoveLines (pCurrentBox, frame, pCurrentBox->BxFirstLine, &changeSelectBegin, &changeSelectEnd);
	     else if (pCurrentBox->BxType == BoTable && ThotLocalActions[T_cleartable])
	       {
		 (*ThotLocalActions[T_cleartable]) (pAb);
		 pAb->AbDead = TRUE;
	       }
	     else if (pCurrentBox->BxType == BoColumn && ThotLocalActions[T_checktable])
	       (*ThotLocalActions[T_checktable]) (NULL, pAb, NULL, frame);
	     else if (pCurrentBox->BxType == BoRow && ThotLocalActions[T_checktable])
	       (*ThotLocalActions[T_checktable]) (NULL, NULL, pAb, frame);
	     else if (pAb->AbLeafType == LtPolyLine)
		FreePolyline (pCurrentBox);
	     else if (pAb->AbLeafType == LtPicture)
	       {
		 UnmapImage((PictInfo *)pCurrentBox->BxPictInfo);
		 FreePictInfo ((PictInfo *)pAb->AbPictInfo);
	       }
	     else if (pCurrentBox->BxType == BoSplit)
	       {
		 /* libere les boites generees pour la mise en lignes */
		 pPieceBox = pCurrentBox->BxNexChild;
		 pCurrentBox->BxNexChild = NULL;
		 while (pPieceBox != NULL)
		   pPieceBox = FreeBox (pPieceBox);
	       }

	     pChildAb = pAb->AbFirstEnclosed;
	     pAb->AbNew = toRemake;
	     if (toRemake)
	       {
		  /* Faut-il restaurer les regles d'une boite elastique */
		  if (pCurrentBox->BxHorizFlex && pCurrentBox->BxHorizInverted)
		     XEdgesExchange (pCurrentBox, OpHorizDep);

		  if (pCurrentBox->BxVertFlex && pCurrentBox->BxVertInverted)
		     YEdgesExchange (pCurrentBox, OpVertDep);
		  pAb->AbDead = FALSE;
	       }

	     /* Liberation des boites des paves inclus */
	     while (pChildAb != NULL)
	       {
		  RemoveBoxes (pChildAb, toRemake, frame);
		  pChildAb = pChildAb->AbNext;	/* while */
	       }

	     /* Suppression des references a pCurrentBox dans la selection */
	     if (ViewFrameTable[frame - 1].FrSelectionBegin.VsBox == pCurrentBox)
		ViewFrameTable[frame - 1].FrSelectionBegin.VsBox = NULL;
	     if (ViewFrameTable[frame - 1].FrSelectionEnd.VsBox == pCurrentBox)
		ViewFrameTable[frame - 1].FrSelectionEnd.VsBox = NULL;

	     /* Liberation des liens eventuels hors hierarchie */
	     ClearXOutOfStructRelation (pCurrentBox);

	     /* Liberation de la boite */
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
#ifdef __STDC__
static void         CheckDefaultPositions (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static void         CheckDefaultPositions (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox      pNextAb;

   /* Est-ce que la boite avait une regle par defaut ? */
   if (pAb->AbEnclosing != NULL)
      if (!pAb->AbEnclosing->AbInLine
	  && !(pAb->AbEnclosing->AbBox->BxType == BoGhost))
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
			    }	/*if !pNextAb->AbDead */
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
			    }	/*if !pNextAb->AbDead */
			  pNextAb = NULL;
		       }
		     else
			pNextAb = pNextAb->AbNext;
		  }
	     }
	}
}


/*----------------------------------------------------------------------
   RecordEnclosing  enregistre les englobements diffe're's.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RecordEnclosing (PtrBox pBox, ThotBool horizRef)
#else  /* __STDC__ */
void                RecordEnclosing (pBox, horizRef)
PtrBox              pBox;
ThotBool            horizRef;

#endif /* __STDC__ */
{
  int                 i;
  PtrDimRelations     pDimRel;
  PtrDimRelations     pPreviousDimRel;
  ThotBool            toCreate;

  /* On recherche une entree libre */
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
	  if (pDimRel->DimRSame[i] == horizRef && pDimRel->DimRTable[i] == pBox)
	    /* La boite est deja enregistree */
	    return;
	  else
	    i++;
	}
      
      if (i == MAX_RELAT_DIM)
	/* Bloc suivant */
	pDimRel = pDimRel->DimRNext;
      else
	toCreate = FALSE;
    }

  /* Faut-il creer un nouveau bloc de relations ? */
  if (toCreate)
    {
      i = 0;
      GetDimBlock (&pDimRel);
      if (pPreviousDimRel == NULL)
	DifferedPackBlocks = pDimRel;
      else
	pPreviousDimRel->DimRNext = pDimRel;
    }

  pDimRel->DimRTable[i] = pBox;
  /* englobement horizontal */
  pDimRel->DimRSame[i] = horizRef;
}


/*----------------------------------------------------------------------
   ComputeUpdates traite les modifications d'un pave correspondant 
   a` la fenetre frame. Rend la valeur vrai s'il y a       
   modification sur la boite du pave.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool     ComputeUpdates (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
ThotBool     ComputeUpdates (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
   PtrLine             pLine;
   PtrAbstractBox      pCurrentAb, pCell, pBlock;
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
   int                 i, charDelta, adjustDelta;
   ThotBool            condition;
   ThotBool            result, isCell;
   ThotBool            orgXComplete;
   ThotBool            orgYComplete;

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
   /* On prepare le reaffichage */
   if (pAb->AbNew || pAb->AbDead || pAb->AbChange
       || pAb->AbWidthChange || pAb->AbHeightChange
       || pAb->AbHorizPosChange || pAb->AbVertPosChange
       || pAb->AbHorizRefChange || pAb->AbVertRefChange
       || pAb->AbAspectChange || pAb->AbSizeChange)
     {
       /* look at if the box or an enclosing box has a background */
       if (pAb->AbNew || pAb->AbDead || pAb->AbHorizPosChange || pAb->AbVertPosChange)
	 {
	 pCurrentAb = pAb->AbEnclosing;	   
	   while (pCurrentAb != NULL &&
		  pCurrentAb->AbPictBackground == NULL &&
		  !pCurrentAb->AbFillBox)
	     pCurrentAb = pCurrentAb->AbEnclosing;
	 }
       else
	 pCurrentAb = pAb;

       if (pCurrentAb == NULL || pCurrentAb->AbEnclosing == NULL)
	 /* no background found: clip the current box */
	 pCurrentBox = pBox;
       else
	 {
	   /* background found: clip the first box with background */
	   pCurrentBox = pCurrentAb->AbBox;
	   if (pCurrentBox != NULL && pCurrentBox->BxType == BoGhost)
	     {
	       /* move to the enclosing block */
	       while (pCurrentAb != NULL && pCurrentAb->AbBox != NULL
		      && pCurrentAb->AbBox->BxType == BoGhost)
		 pCurrentAb = pCurrentAb->AbEnclosing;
	       if (pCurrentAb == NULL)
		 pCurrentBox = pBox;
	       else
		 pCurrentBox = pCurrentAb->AbBox;
	     }
	 }
       /* Si la boite est coupee on prend la premiere boite de coupure */
       if (pCurrentBox != NULL)
	 {
	   if (pCurrentBox->BxType == BoSplit)
	     pCurrentBox = pCurrentBox->BxNexChild;
	   if ((pCurrentBox->BxWidth > 0 && pCurrentBox->BxHeight > 0) || pCurrentBox->BxType == BoPicture)
	     DefClip (frame, pCurrentBox->BxXOrg, pCurrentBox->BxYOrg, pCurrentBox->BxXOrg + pCurrentBox->BxWidth,
		      pCurrentBox->BxYOrg + pCurrentBox->BxHeight);
	 }
     }

   /* NOUVEAU AbstractBox */
   if (pAb->AbNew)
     {
        AnyWidthUpdate = TRUE;
	/* On situe la boite dans le chainage des boites terminales */
	pCurrentAb = PreviousLeafAbstractBox (pAb);
	/* 1ere boite precedente */
	if (pCurrentAb != NULL)
	   pCurrentBox = pCurrentAb->AbBox;
	else
	   pCurrentBox = NULL;

	if (pCurrentBox != NULL)
	  {
	     i = 0;		/* Place dans le document */
	     /* Est-ce que la boite est coupee ? */
	     if (pCurrentBox->BxType == BoSplit)
		while (pCurrentBox->BxNexChild != NULL)
		   pCurrentBox = pCurrentBox->BxNexChild;
	     pNextBox = pCurrentBox->BxNext;	/* 1ere boite suivante */
	  }
	else
	  {
	     i = 0;		/* Place dans le document */
	     if (pMainBox != NULL)
		pNextBox = pMainBox->BxNext;	/* suivante */
	     else
		pNextBox = NULL;
	  }

	/* Est-ce la boite racine ? */
	pCurrentAb = pAb->AbEnclosing;
	if (pCurrentAb == NULL)
	   condition = FALSE;
	else
	  {
	     /* Est-ce que la boite englobante doit etre eclatee ? */
	     if (pCurrentAb->AbAcceptLineBreak && pCurrentAb->AbEnclosing != NULL)
		if (pCurrentAb->AbEnclosing->AbInLine
		    || pCurrentAb->AbEnclosing->AbBox->BxType == BoGhost)
		   pCurrentAb->AbBox->BxType = BoGhost;

	     /* L'indicateur de mise en lignes depend de la boite englobante */
	     condition = pCurrentAb->AbInLine || pCurrentAb->AbBox->BxType == BoGhost;

	     /* Faut-il dechainer la boite englobante ? */
	     if ((pNextBox != NULL) && (pNextBox == pCurrentAb->AbBox))
		pNextBox = pNextBox->BxNext;
	     /* On etabli le chainage pour inserer en fin les nouvelles boites */
	     /* Faut-il dechainer la boite englobante ? */
	     if (pCurrentBox == NULL)
		pMainBox->BxNext = NULL;	/* debut du chainage */
	     pLastBox = pMainBox->BxPrevious;	/* on memorise la derniere boite */
	     pMainBox->BxPrevious = pCurrentBox;	/* fin provisoire du chainage */
	  }

	/* Faut-il dechainer la boite englobante ? */
	ReadyToDisplay = FALSE;	/* On arrete l'evaluation du reaffichage */
	savpropage = Propagate;
	Propagate = ToSiblings;	/* Limite la propagation sur le descendance */
	pBox = CreateBox (pAb, frame, condition, &i);

	ReadyToDisplay = TRUE;	/* On retablit l'evaluation du reaffichage */

	/* Mise a jour de la liste des boites terminales */
	if (pMainBox != NULL && pNextBox != NULL)
	  {
	     /* On ajoute en fin de chainage */
	     pNextBox->BxPrevious = pMainBox->BxPrevious;
	     /* derniere boite chainee */
	     if (pMainBox->BxPrevious != NULL)
		pMainBox->BxPrevious->BxNext = pNextBox;
	     pMainBox->BxPrevious = pLastBox;
	     /* nouvelle fin de chainage */
	     if (pMainBox->BxNext == NULL)
		pMainBox->BxNext = pNextBox;
	  }

	/* Si la boite a ete creee (pCurrentBox<>NULL) */
	if (pBox == NULL)
	   Propagate = savpropage;	/* Restaure la regle de propagation */
	else
	  {
	     /* place les origines des boites par rapport a la racine de la vue */
	     /* si la boite n'a pas deja ete placee en absolu */
	     IsXYPosComplete (pBox, &orgXComplete, &orgYComplete);
	     if (!orgXComplete || !orgYComplete)
	       {
		  /* Initialise le placement des boites creees */
		  SetBoxToTranslate (pAb, (ThotBool)(!orgXComplete), (ThotBool)!orgYComplete);
		  /* La boite racine va etre placee */
		  pBox->BxXToCompute = FALSE;
		  pBox->BxYToCompute = FALSE;
		  AddBoxTranslations (pAb, pFrame->FrVisibility, frame, (ThotBool)(!orgXComplete), (ThotBool)(!orgYComplete));
	       }

	     /* On prepare le reaffichage */
	     if (pCurrentAb == NULL)
		condition = TRUE;
	     else
		condition = !pCurrentAb->AbInLine;
	     if (condition)
	       {
		  DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg + pBox->BxWidth,
			   pBox->BxYOrg + pBox->BxHeight);
	       }

	     /* On verifie la coherence des positions par defaut */
	     /***CheckDefaultPositions(pAb, frame);***/
	     pAb->AbChange = FALSE;
	     /* check enclosing cell */
	     pCell = GetParentCell (pCurrentBox);
	     if (pCell != NULL && ThotLocalActions[T_checkcolumn])
	       {
		 Propagate = ToChildren;
		 pBlock = SearchEnclosingType (pAb, BoBlock);
		 if (pBlock != NULL)
		   RecomputeLines (pBlock, NULL, NULL, frame);
		 (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
	       }
	     Propagate = savpropage;	/* Restaure la regle de propagation */
	     result = TRUE;
	  }
     }
   /* AbstractBox MORT */
   else if (pAb->AbDead)
     {
        AnyWidthUpdate = TRUE;
	if (pAb->AbLeafType == LtPolyLine)
	   FreePolyline (pBox);	/* libere la liste des buffers de la boite */

	/* On situe la boite dans le chainage des boites terminales */
	pCurrentAb = PreviousLeafAbstractBox (pAb);

	/* On recherche la derniere boite terminale avant */
	if (pCurrentAb == NULL)
	   pCurrentBox = pMainBox;	/* debut du chainage */
	else
	  {
	     pCurrentBox = pCurrentAb->AbBox;
	     /* Est-ce que la boite est coupee ? */
	     if (pCurrentBox->BxType == BoSplit)
		while (pCurrentBox->BxNexChild != NULL)
		   pCurrentBox = pCurrentBox->BxNexChild;
	  }

	/* Est-ce que la boite englobante devient terminale ? */
	pCurrentAb = pAb->AbEnclosing;
	if (IsAbstractBoxEmpty (pCurrentAb))
	  {
	     pNextBox = pCurrentAb->AbBox;
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
	     while (pCurrentAb->AbEnclosing != NULL && pCurrentAb->AbNext == NULL)
		pCurrentAb = pCurrentAb->AbEnclosing;
	     pCurrentAb = pCurrentAb->AbNext;
	     if (pCurrentAb == NULL)
		pNextBox = pMainBox;	/* 1ere boite suivante */
	     else
	       {
		  while (pCurrentAb->AbBox != NULL && pCurrentAb->AbFirstEnclosed != NULL)
		     pCurrentAb = pCurrentAb->AbFirstEnclosed;
		  pNextBox = pCurrentAb->AbBox;
	       }
	  }

	/* Est-ce que la boite est coupee ? */
	if (pNextBox->BxType == BoSplit)
	   pNextBox = pNextBox->BxNexChild;

	/* Destruction */
	ClearAllRelations (pBox);
	ClearDimRelation (pBox, TRUE, frame);
	ClearDimRelation (pBox, FALSE, frame);
	pCell = GetParentCell (pCurrentBox);
	isCell = pAb->AbBox->BxType == BoCell;
	RemoveBoxes (pAb, FALSE, frame);

	/* update the list of leaf boxes */
	if (pCurrentBox == pMainBox)
	  /* first box in the list */
	   pNextBox->BxPrevious = NULL;
	else
	  {
	    /* backward link */
	    pNextBox->BxPrevious = pCurrentBox;
	    if (pNextBox->BxType == BoPiece &&
		pNextBox->BxAbstractBox->AbBox != NULL)
	      if (pCurrentBox->BxType == BoPiece)
		pNextBox->BxAbstractBox->AbBox->BxPrevious = pCurrentBox->BxAbstractBox->AbBox;
	      else
		pNextBox->BxAbstractBox->AbBox->BxPrevious = pCurrentBox;
	  }
	if (pNextBox == pMainBox)
	  /* last box in the list */
	   pCurrentBox->BxNext = NULL;
	else
	  {
	    /* forward link */
	    pCurrentBox->BxNext = pNextBox;
	    if (pCurrentBox->BxType == BoPiece &&
		pCurrentBox->BxAbstractBox->AbBox != NULL)
	      if (pNextBox->BxType == BoPiece)
		pCurrentBox->BxAbstractBox->AbBox->BxPrevious = pNextBox->BxAbstractBox->AbBox;
	      else
		pCurrentBox->BxAbstractBox->AbBox->BxPrevious = pNextBox;

	  }

	/* Check table consistency */
	if (isCell && ThotLocalActions[T_checkcolumn])
	  (*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
	/* check enclosing cell */
	else if (pCell != NULL && ThotLocalActions[T_checkcolumn] &&
		 !pAb->AbEnclosing->AbDead &&
		 (pAb->AbNext == NULL || (!pAb->AbNext->AbDead && !pAb->AbNext->AbNew)))
	  (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
	result = TRUE;
     }
   else
     {
	/* CHANGEMENT GRAPHIQUE */
	if (pAb->AbAspectChange)
	  {
	     pAb->AbAspectChange = FALSE;
	     if (pAb->AbLeafType == LtPicture
		 && ((PictInfo *) pBox->BxPictInfo)->PicType == XBM_FORMAT)
	       {
		 /* Il faut forcer le rechargement des images */
		 /* SetCursorWatch (frame); */
		 LoadPicture (frame, pBox, (PictInfo *) pBox->BxPictInfo);
		 /* ResetCursorWatch (frame); */
	       }
	     else if (pBox->BxType == BoSplit)
	       {
		  /* Si la boite est coupee on etend le clipping jusqu'a */
		  /* la derniere boite de coupure non vide */
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
		  /* mark the zone to be displayed */
		  if (pCurrentBox->BxWidth > 0 && pCurrentBox->BxHeight > 0)
		     DefClip (frame, pCurrentBox->BxXOrg, pCurrentBox->BxYOrg, pCurrentBox->BxXOrg + pCurrentBox->BxWidth,
			      pCurrentBox->BxYOrg + pCurrentBox->BxHeight);
	       }
	     else if (pAb->AbLeafType == LtCompound)
	       {
		 /* Is there a background image ? */
		 if (pAb->AbPictBackground != NULL)
		   {
		     if (pAb->AbFillPattern == 2)
		       /* change "backgroundcolor" into "nopattern" */
		       pAb->AbFillPattern = 0;
		     /* force filling */
		     pAb->AbFillBox = TRUE;
		     /* load the picture */
		     LoadPicture (frame, pBox, (PictInfo *) pAb->AbPictBackground);
		   }
		 
		 if (pAb->AbFillBox)
		   /* register the box in filled list */
		   AddFilledBox (pBox, pMainBox, frame);
		 else
		   /* unregister the box in filled list */
		   RemoveFilledBox (pBox, pMainBox, frame);
		 
		 /* mark the zone to be displayed */
		 DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg + pBox->BxWidth,
			  pBox->BxYOrg + pBox->BxHeight);
	       }
	  }
	/* Taille des caracteres du CONTENU DU PAVE MODIFIE */
	if (pAb->AbSizeChange)
	  {
	    /* Il faut regarder les consequences du changement de taille */
	    pDimAb = &pAb->AbWidth;
	    if (!pDimAb->DimIsPosition)
	      {
		if (pDimAb->DimAbRef != NULL || pDimAb->DimValue != 0)
		  pAb->AbWidthChange = (pDimAb->DimUnit == UnRelative);
		if (pAb->AbHorizPos.PosAbRef != NULL)
		  pAb->AbHorizPosChange = (pAb->AbHorizPos.PosUnit == UnRelative);
	      }
	    
	    pDimAb = &pAb->AbHeight;
	    if (!pDimAb->DimIsPosition)
	      {
		if (pDimAb->DimAbRef != NULL || pDimAb->DimValue != 0)
		  pAb->AbHeightChange = (pDimAb->DimUnit == UnRelative);
		if (pAb->AbVertPos.PosAbRef != NULL)
		  pAb->AbVertPosChange = (pAb->AbVertPos.PosUnit == UnRelative);
	      }
	    
	    pAb->AbHorizRefChange = (pAb->AbHorizRef.PosUnit == UnRelative);
	    pAb->AbVertRefChange = (pAb->AbVertRef.PosUnit == UnRelative);
	  }
	/* CONTENU DU PAVE MODIFIE */
	if (pAb->AbChange || pAb->AbSizeChange)
	  {
	     /* On verifie que la fonte attachee au pave est chargee */
	     height = pAb->AbSize;
	     unit = pAb->AbSizeUnit;
	     height += pFrame->FrMagnification;

	     if (pAb->AbLeafType == LtText)
		pBox->BxFont = ThotLoadFont (TtaGetAlphabet (pAb->AbLanguage),
					  pAb->AbFont, FontStyleAndWeight(pAb),
					  height, unit, frame);
	     else if (pAb->AbLeafType == LtSymbol)
		pBox->BxFont = ThotLoadFont ('G', pAb->AbFont,
				FontStyleAndWeight (pAb), height, unit, frame);
	     else
		pBox->BxFont = ThotLoadFont ('L', 'T', 0, height, unit, frame);

	     /* On transmet l'information souligne dans la boite */
	     pBox->BxUnderline = pAb->AbUnderline;
	     pBox->BxThickness = pAb->AbThickness;

	     /* On teste le type du pave pour mettre a jour la boite */
	     if (pAb->AbLeafType == LtCompound)
	       {
		  /* Si c'est un bloc de lignes qui est mis a jour */
		  if (pAb->AbChange && pAb->AbInLine
		      && pBox->BxType != BoGhost)
		     RecomputeLines (pAb, NULL, pBox, frame);
	       }
	     else
	       {
		  switch (pAb->AbLeafType)
			{
			   case LtPageColBreak:
			      width = 0;
			      height = 0;
			      break;
			   case LtText:
			      GiveTextSize (pAb, &width, &height, &nSpaces);

			      /* Si la boite est justifiee */
			      if (pBox->BxSpaceWidth != 0)
				{
				   i = pBox->BxSpaceWidth - CharacterWidth (_SPACE_, pBox->BxFont);
				   /* On prend la largeur justifiee */
				   width = width + i * nSpaces + pBox->BxNPixels;
				   /* Ecart de largeur */
				   adjustDelta = width - pBox->BxWidth;
				}
			      charDelta = pAb->AbVolume - pBox->BxNChars;	/* ecart de caracteres */
			      nSpaces -= pBox->BxNSpaces;	/* ecart de blancs */
			      pBox->BxBuffer = pAb->AbText;
			      break;
			   case LtPicture:
			      if (pAb->AbChange)
				{
				   /* the picture change, RAZ the structure */
				   FreePictInfo ((PictInfo *) pBox->BxPictInfo);
				   SetCursorWatch (frame);
				   LoadPicture (frame, pBox, (PictInfo *) pBox->BxPictInfo);
				   ResetCursorWatch (frame);
				   GivePictureSize (pAb, ViewFrameTable[frame -1].FrMagnification, &width, &height);
				}
			      else
				{
				   width = pBox->BxWidth;
				   height = pBox->BxHeight;
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
				   FreePolyline (pBox);
				   pAb->AbRealShape = pAb->AbShape;

				   /* remonte a la recherche d'un ancetre elastique */
				   pCurrentAb = pAb;
				   while (pCurrentAb != NULL)
				     {
					pCurrentBox = pCurrentAb->AbBox;
					if (pCurrentBox->BxHorizFlex || pCurrentBox->BxVertFlex)
					  {
					     MirrorShape (pAb, pCurrentBox->BxHorizInverted, pCurrentBox->BxVertInverted, FALSE);
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
					if (pCurrentBox->BxHorizFlex || pCurrentBox->BxVertFlex)
					  {
					     MirrorShape (pAb, pCurrentBox->BxHorizInverted, pCurrentBox->BxVertInverted, FALSE);
					     /* on arrete */
					     pCurrentAb = NULL;	
					  }
					else
					   pCurrentAb = pCurrentAb->AbEnclosing;
				     }
				}
			      GivePolylineSize (pAb, &width, &height);
			      break;
			   default:
			      break;
			}

		  /* On modifie la boite  */
		  pDimAb = &pAb->AbWidth;
		  if (pDimAb->DimIsPosition)
		     width = 0;
		  else if (pDimAb->DimAbRef != NULL || pDimAb->DimValue > 0)
		     width = 0;
		  else
		     width -= pBox->BxWidth;	/* ecart de largeur */
		  pDimAb = &pAb->AbHeight;
		  if (pDimAb->DimIsPosition)
		     height = 0;
		  else if (pDimAb->DimAbRef != NULL || pDimAb->DimValue > 0)
		     height = 0;
		  else
		     height -= pBox->BxHeight;	/* ecart de hauteur */
		  pLine = NULL;
		  if (width != 0 || height !=0)
		    {
		      pCell = GetParentCell (pBox);
		      BoxUpdate (pBox, pLine, charDelta, nSpaces, width, adjustDelta, height, frame, FALSE);
		      /* check enclosing cell */
		      if (pCell != NULL && width != 0 && ThotLocalActions[T_checkcolumn])
			{
			  if (pAb->AbLeafType == LtPicture)
			    {
			      pBlock = SearchEnclosingType (pAb, BoBlock);
			      if (pBlock != NULL)
				{
				  RecomputeLines (pBlock, NULL, NULL, frame);
				  /* we will have to pack enclosing box */
				  RecordEnclosing (pBlock->AbBox, FALSE);
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
	  }
	/* CHANGEMENT DE LARGEUR */
	if (pAb->AbWidthChange)
	  {
	     AnyWidthUpdate = TRUE;
	     /* Annulation ancienne largeur */
	     ClearDimRelation (pBox, TRUE, frame);
	     /* Nouvelle largeur */
	     condition = ComputeDimRelation (pAb, frame, TRUE);
	     if (condition || (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum))
	       {
		  switch (pAb->AbLeafType)
			{
			   case LtText:
			      GiveTextSize (pAb, &width, &height, &i);
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
			      if (pAb->AbInLine)
				{
				   /* La regle mise en lignes est prise en compte? */
				   if (pBox->BxType != BoGhost)
				      RecomputeLines (pAb, NULL, NULL, frame);
				   width = pBox->BxWidth;
				}
			      else
				 GiveEnclosureSize (pAb, frame, &width, &height);
			      break;
			   default:
			      width = pBox->BxWidth;
			      break;
			}

		  /* Mise a jour de la largeur du contenu */
		  ChangeDefaultWidth (pBox, NULL, width, 0, frame);
		  result = TRUE;
	       }
	     /* La boite ne depend pas de son contenu */
	     else
		result = TRUE;

	     /* Check table consistency */
	     if (pCurrentBox->BxType == BoColumn && ThotLocalActions[T_checktable])
	       (*ThotLocalActions[T_checktable]) (NULL, pAb, NULL, frame);
	     else if (pCurrentBox->BxType == BoCell && ThotLocalActions[T_checkcolumn])
	       (*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
	     /* check enclosing cell */
	     pCell = GetParentCell (pCurrentBox);
	     if (pCell != NULL && ThotLocalActions[T_checkcolumn])
	       {
		 pBlock = SearchEnclosingType (pAb, BoBlock);
		 if (pBlock != NULL)
		   RecomputeLines (pBlock, NULL, NULL, frame);
		 (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
	       }
	  }
	/* CHANGEMENT DE HAUTEUR */
	if (pAb->AbHeightChange)
	  {
	     /* Annulation ancienne hauteur */
	     ClearDimRelation (pBox, FALSE, frame);
	     /* Nouvelle hauteur */
	     condition = ComputeDimRelation (pAb, frame, FALSE);
	     if (condition || (!pAb->AbHeight.DimIsPosition && pAb->AbHeight.DimMinimum))
	       {
		  switch (pAb->AbLeafType)
			{
			   case LtText:
			      GiveTextSize (pAb, &width, &height, &i);
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
			      if (pAb->AbInLine)
				{
				   /* On evalue la hauteur du bloc de ligne */
				   pLine = pBox->BxLastLine;
				   if (pLine == NULL || pBox->BxType == BoGhost)
				      height = pBox->BxHeight;
				   else
				      height = pLine->LiYOrg + pLine->LiHeight;
				}
			      else
				 GiveEnclosureSize (pAb, frame, &width, &height);
			      break;
			   default:
			      height = pBox->BxHeight;
			      break;
			}


		  /* Mise a jour de la hauteur du contenu */
		  ChangeDefaultHeight (pBox, NULL, height, frame);
		  result = TRUE;
	       }
	     /* La boite ne depend pas de son contenu */
	     else
		result = TRUE;
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
	     else if (pAb->AbEnclosing->AbInLine
		      || pAb->AbEnclosing->AbBox->BxType == BoGhost)
		if (!pAb->AbHorizEnclosing)
		   condition = TRUE;
		else
		   condition = FALSE;
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
	  }
	/* CHANGEMENT DE POSITION VERTICALE */
	if (pAb->AbVertPosChange)
	  {
	     /* Les cas de coherence sur les boites elastiques */
	     /* Les reperes Position et Dimension doivent etre differents */
	     /* Ces reperes ne peuvent pas etre l'axe de reference        */
	     if (pAb->AbHeight.DimIsPosition
	     && (pAb->AbVertPos.PosEdge == pAb->AbHeight.DimPosition.PosEdge
		 || pAb->AbVertPos.PosEdge == HorizMiddle
		 || pAb->AbVertPos.PosEdge == HorizRef))
	       {
		  if (pAb->AbHeight.DimPosition.PosEdge == Top)
		     pAb->AbVertPos.PosEdge = Bottom;
		  else if (pAb->AbHeight.DimPosition.PosEdge == Bottom)
		     pAb->AbVertPos.PosEdge = Top;
		  pAb->AbVertPosChange = FALSE;
	       }
	     else if (pAb->AbEnclosing == NULL)
		condition = TRUE;
	     /* La regle de position est annulee par la mise en lignes */
	     else if (pAb->AbEnclosing->AbInLine
		      || pAb->AbEnclosing->AbBox->BxType == BoGhost)
	       {
		  if (!pAb->AbHorizEnclosing && pBox->BxNChars > 0)
		    {
		       pPosAb = &pAb->AbVertPos;
		       pLine = SearchLine (pBox);
		       if (pLine != NULL)
			 {
			    i = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
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
	  }
	/* CHANGEMENT D'AXE HORIZONTAL */
	if (pAb->AbHorizRefChange)
	  {
	     /* Annulation ancien axe horizontal */
	     ClearAxisRelation (pBox, FALSE);
	     /* Nouvel axe horizontal */
	     ComputeAxisRelation (pAb->AbHorizRef, pBox, frame, FALSE);
	     result = TRUE;
	  }
	/* CHANGEMENT D'AXE VERTICAL */
	if (pAb->AbVertRefChange)
	  {
	     /* Annulation ancien axe verticale */
	     ClearAxisRelation (pBox, TRUE);
	     /* Nouvel axe vertical */
	     ComputeAxisRelation (pAb->AbVertRef, pBox, frame, TRUE);
	     result = TRUE;
	  }
     }
   return result;
}


/*----------------------------------------------------------------------
   ComputeEnclosing traite les contraintes d'englobement diffe're'es 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ComputeEnclosing (int frame)
#else  /* __STDC__ */
void                ComputeEnclosing (frame)
int                 frame;

#endif /* __STDC__ */
{
   int                 i;
   PtrDimRelations     pDimRel;

   /* Il faut reevaluer la dimension des boites dont le contenu est */
   /* englobe et depend de relations hors-structure.                */
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
		;		/* le pave n'existe plus */
	     else if (pDimRel->DimRSame[i])
		WidthPack (pDimRel->DimRTable[i]->BxAbstractBox, NULL, frame);
	     else
		HeightPack (pDimRel->DimRTable[i]->BxAbstractBox, NULL, frame);
	     /* Entree suivante */
	     i++;
	  }

	/* On passe au bloc suivant */
	pDimRel = pDimRel->DimRNext;
     }

   /* On libere les blocs */
   while (DifferedPackBlocks != NULL)
     {
	pDimRel = DifferedPackBlocks;
	DifferedPackBlocks = DifferedPackBlocks->DimRNext;
	FreeDimBlock (&pDimRel);
     }
}

/*----------------------------------------------------------------------
   RebuildConcreteImage reevalue une vue de document suite a` la modification
   du frame. Reaffiche la vue dans la fenetre supposee     
   nettoyee.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RebuildConcreteImage (int frame)
#else  /* __STDC__ */
void                RebuildConcreteImage (frame)
int                 frame;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   PtrAbstractBox      pAb, pVisibleAb;
   PtrBox              pBox;
   int                 width, height;
   int                 position = 0;
   ThotBool            condition;
   ThotBool            status;

   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrAbstractBox != NULL)
     {
       pAb = pFrame->FrAbstractBox;
       if (pAb->AbBox != NULL)
	 {
	   /* get the first visible abstract box and its current postion */
	   GetSizesFrame (frame, &width, &height);
	   pBox = pAb->AbBox;
	   while (pBox != NULL && pBox->BxYOrg < pFrame->FrYOrg &&
		  pBox->BxNext != NULL)
	     pBox = pBox->BxNext;
	   if (pBox != NULL)
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
	   /* Remove la selection eventuelle */
	   if (ThotLocalActions[T_switchsel])
	     (*ThotLocalActions[T_switchsel]) (frame, FALSE);
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

	   /* Affiche la fenetre */
	   if (pVisibleAb != NULL)
	     {
	       if (ThotLocalActions[T_showbox] != NULL)
		 (*ThotLocalActions[T_showbox]) (frame, pVisibleAb->AbBox, 0, position);
	     }
	   else
	     RedrawFrameBottom (frame, 0);
	   /* Restaure la selection */
	   ShowSelection (pAb, FALSE);
	 }
     }
}


/*----------------------------------------------------------------------
   ClearFlexibility annule l'e'lasticite' de la boite s'il s'agit     
   d'une boite elastique et les regles de position et de   
   dimension sont a` reevaluer. Traite recusivement        
   les paves fils.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ClearFlexibility (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static void         ClearFlexibility (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
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
	     DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
		      pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);

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
		DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			 pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
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
   ClearConcreteImage libere toutes les boites de la vue dont on donne         
   le pave racine.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearConcreteImage (int frame)
#else  /* __STDC__ */
void                ClearConcreteImage (frame)
int                 frame;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;

   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrAbstractBox != NULL)
     {
	pFrame->FrReady = FALSE;	/* La frame n'est pas affichable */
	/* Faut-il retirer les marques de selection dans la fenetre */
	CloseInsertion ();
	ClearViewSelection (frame);
	/* Liberation de la hierarchie */
	RemoveBoxes (pFrame->FrAbstractBox, FALSE, frame);
	pFrame->FrAbstractBox = NULL;
	pFrame->FrReady = TRUE;	/* La frame est affichable */
	DefClip (frame, -1, -1, -1, -1);	/* effacer effectivement */
	DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &pFrame->FrClipXBegin, &pFrame->FrClipYBegin,
			&pFrame->FrClipXEnd, &pFrame->FrClipYEnd, 1);
     }
}


/*----------------------------------------------------------------------
   IsAbstractBoxUpdated rend la valeur Vrai si les changements ont 
   une re'percution sur le pave englobant.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsAbstractBoxUpdated (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static ThotBool     IsAbstractBoxUpdated (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
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
	       if (pCurrentBox->BxType == BoBlock && pFirstAb != NULL)
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
			   if (pBox != NULL)
			     {
			       /* S'il s'agit d'une boite de texte coupee */
			       /* on recherche la derniere boite de texte */
			       if (pBox->BxType == BoSplit)
				 while (pBox->BxNexChild != NULL)
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
			   if (pBox->BxType == BoSplit && pBox->BxNexChild != NULL)
			     pBox = pBox->BxNexChild;
			   pLine = SearchLine (pBox);
			 }
		       else
			 pLine = NULL;
		     }
		   
		   RecomputeLines (pAb, pLine, pAb->AbBox, frame);
		 }
	       /* Mise a jour d'une boite composee */
	       else if (!pAb->AbInLine
			&& pAb->AbBox->BxType != BoGhost
			&& pAb->AbLeafType == LtCompound)
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
#ifdef __STDC__
void     CheckScrollingWidth (int frame)
#else  /* __STDC__ */
void     CheckScrollingWidth (frame)
int      frame;
#endif /* __STDC__ */
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
#ifdef __STDC__
ThotBool            ChangeConcreteImage (int frame, int *pageHeight, PtrAbstractBox pAb)
#else  /* __STDC__ */
ThotBool            ChangeConcreteImage (frame, pageHeight, pAb)
int                 frame;
int                *pageHeight;
PtrAbstractBox      pAb;

#endif /* __STDC__ */
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

#  ifdef _WINDOWS
   WIN_GetDeviceContext (frame);
#  endif /* _WINDOWS */

   result = TRUE;
   document = FrameTable[frame].FrDoc;
   if (document == 0)
     return result;

   pLine = NULL;
   /* Pas de pave en parametre */
   if (pAb == NULL)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_EMPTY_VIEW);
   /* Le numero de frame est erronne */
   else if (frame < 1 || frame > MAX_FRAME)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_BAD_FRAME_NB);
   else
     {
	pFrame = &ViewFrameTable[frame - 1];
	/* La vue n'est pas cree a la racine */
	if (pFrame->FrAbstractBox == NULL && (pAb->AbEnclosing != NULL
			 || pAb->AbPrevious != NULL || pAb->AbNext != NULL))
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
	else if (pFrame->FrAbstractBox != NULL && pAb->AbEnclosing == NULL
		 && pAb->AbNew)
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

	     saveMode = documentDisplayMode[FrameTable[frame].FrDoc - 1];
	     if (saveMode == DisplayImmediately)
	       documentDisplayMode[FrameTable[frame].FrDoc - 1] = DeferredDisplay;

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
		  /* L'englobement doit etre repris a partir de boite du pave pere */
		  PackBoxRoot = pParentAb->AbBox;

		  /* On est dans une boite eclatee, on remonte jusqu'au bloc de lignes */
		  while (pParentAb->AbBox->BxType == BoGhost)
		     pParentAb = pParentAb->AbEnclosing;

		  /* On prepare la mise a jour d'un bloc de lignes */
		  if (pParentAb->AbInLine
		      || pParentAb->AbBox->BxType == BoGhost)
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
			       if (pBox->BxType == BoSplit)
				  while (pBox->BxNexChild != NULL)
				     pBox = pBox->BxNexChild;
			       pLine = SearchLine (pBox);
			    }
		       }
		     else
		       {
			  /* Il faut refaire la mise en lignes sur la premiere boite contenue */
			  pBox = pAb->AbBox;
			  if (pBox->BxType == BoSplit)
			     pBox = pBox->BxNexChild;
			  pLine = SearchLine (pBox);
		       }
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
		  if (pParentBox->BxType == BoBlock)
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
	       {
		 documentDisplayMode[FrameTable[frame].FrDoc - 1] = saveMode;
		 if (ThotLocalActions[T_colupdates] != NULL)
		   (*ThotLocalActions[T_colupdates]) (FrameTable[frame].FrDoc);
	       }

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
#  ifdef _WINDOWS
   WIN_ReleaseDeviceContext ();
#  endif /* _WINDOWS */
   return result;
}
