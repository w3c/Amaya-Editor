/*
 * displaybox.c : all the stuff needed to display boxes in frames.
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appdialogue.h"

#define EXPORT extern
#include "img.var"
#include "edit.var"
#include "frame.var"
#include "appdialogue.var"

#include "windowdisplay_f.h"
#include "font_f.h"
#include "displaybox_f.h"
#include "picture_f.h"
#include "buildboxes_f.h"

/**
 *      ValEpaisseur compute the 
 **/
#ifdef __STDC__
static int          ValEpaisseur (PtrAbstractBox pAb)
#else  /* __STDC__ */
static int          ValEpaisseur (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */
{
   if (pAb == NULL)
      return (0);
   else
      return PixelValue (pAb->AbLineWeight, pAb->AbLineWeightUnit, pAb);
}


/**
 *      DisplayImage affiche une boite vide dimensionnee et positionnee dans
 *              la fenetre frame. On affiche une suite de caracteres
 *              grise's sur toute la surface de la boite.
 **/
#ifdef __STDC__
static void         DisplayImage (PtrBox pBox, int frame)
#else  /* __STDC__ */
static void         DisplayImage (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */
{
   int                 dx, dy;
   int                 larg, i;
   int                 haut;
   int                 op, RO;
   ViewFrame            *pFrame;
   PtrBox            pBo1;

/*IMG */ DrawPage ((FILE *) FrRef[frame]);

   if (pBox->BxAbstractBox->AbVisibility >= FntrTable[frame - 1].FrVisibility)
     {
	if (pBox->BxAbstractBox->AbSensitive)
	   op = 1;
	else
	   op = 0;
	if (pBox->BxAbstractBox->AbReadOnly)
	   RO = 1;
	else
	   RO = 0;

	/* On calcule la position et dimension de l'image dans la fenetre */
	dx = pBox->BxXOrg;
	larg = pBox->BxWidth;
	dy = pBox->BxYOrg;
	haut = pBox->BxHeight;

	/* On limite la surface de la fenetre a remplir a la boite englobante */
	pBo1 = pBox->BxAbstractBox->AbEnclosing->AbBox;
	i = pBo1->BxXOrg - dx;
	if (i > 0)
	  {
	     dx = pBo1->BxXOrg;
	     larg -= i;
	  }

	i = pBo1->BxXOrg + pBo1->BxWidth - dx;
	if (larg > i)
	   larg = i;

	i = pBo1->BxYOrg - dy;
	if (i > 0)
	  {
	     dy = pBo1->BxYOrg;
	     haut -= i;
	  }
	/* Juste pour charger la couleur du trace */
	DrawRectangle (frame, 0, 0, 0, 0, 0, 0, 0, 0, pBox->BxAbstractBox->AbForeground,
		     pBox->BxAbstractBox->AbBackground, 0);

	i = pBo1->BxYOrg + pBo1->BxHeight - dy;
	if (haut > i)
	   haut = i;

	DrawImage (pBox, (PictInfo *) pBox->BxPictInfo, frame);

	pFrame = &FntrTable[frame - 1];
	/* Est-ce qu'il faut completer la ligne avec des pointilles */
	if (pBox->BxEndOfBloc > 0)
	  {
	     /* On calcule l'alignement des bases */
	     dy = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	     DrawPoints (frame, pBox->BxXOrg + pBox->BxWidth - pFrame->FrXOrg, dy,
		    pBox->BxEndOfBloc, RO, op, pBox->BxAbstractBox->AbForeground);
	  }
     }
}				/* function DisplayImage */

/**
 *      DisplaySymbol affiche une boite symbole mathematique dimensionnee
 *              et positionnee dans la fenetre frame. On utilise les
 *              fontes greques et des traits pour dessiner.
 **/

#ifdef __STDC__
static void         DisplaySymbol (PtrBox pBox, int frame)

#else  /* __STDC__ */
static void         DisplaySymbol (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */

{
   int                 xd, yd, i;
   int                 fg;
   int                 bg;
   ptrfont             font;
   PtrBox            mbox;
   int                 op, RO;
   PtrAbstractBox             pAbbox1;
   ViewFrame            *pFrame;
   boolean             withbackground;

   /* On recherche la boite qui contraint l'englobement */
   pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
   mbox = pAbbox1->AbBox;

   /* Si le pave englobant est eclate on saute au pave englobant entier */
   if (mbox->BxType == BoGhost)
      while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	 mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   withbackground = (pBox->BxAbstractBox->AbFillPattern == 2);
   if (pBox->BxAbstractBox->AbVisibility >= FntrTable[frame - 1].FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth > pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight > pBox->BxYOrg)
	{
	   font = pBox->BxFont;
	   if (font != NULL)
	     {
		/* Position dans la fenetre */
		pFrame = &FntrTable[frame - 1];
		xd = pBox->BxXOrg - pFrame->FrXOrg;
		yd = pBox->BxYOrg - pFrame->FrYOrg;
		if (pBox->BxAbstractBox->AbSensitive)
		   op = 1;
		else
		   op = 0;
		if (pBox->BxAbstractBox->AbReadOnly)
		   RO = 1;
		else
		   RO = 0;

		if (withbackground)
		   DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, 0, 0, bg, 2);

		/* Epaisseur des traits */
		i = ValEpaisseur (pBox->BxAbstractBox);

		switch (pBox->BxAbstractBox->AbShape)
		      {
			 case 'r':
			    DrawRadical (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, font, RO, op, fg);
			    break;
			 case 'i':
			    DrawIntegral (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, font, RO, op, fg);
			    break;
			 case 'c':
			    DrawIntegral (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, font, RO, op, fg);
			    break;
			 case 'd':
			    DrawIntegral (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 2, font, RO, op, fg);
			    break;
			 case 'S':
			    DrawSigma (frame, xd, yd, pBox->BxWidth, pBox->BxHeight, font, RO, op, fg);
			    break;
			 case 'P':
			    DrawPi (frame, xd, yd, pBox->BxWidth, pBox->BxHeight, font, RO, op, fg);
			    break;
			 case 'I':
			    DrawIntersection (frame, xd, yd, pBox->BxWidth, pBox->BxHeight, font, RO, op, fg);
			    break;
			 case 'U':
			    DrawUnion (frame, xd, yd, pBox->BxWidth, pBox->BxHeight, font, RO, op, fg);
			    break;
			 case 'h':
			    DrawLine (frame, i, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, RO, op, fg);
			    break;
			 case 'v':
			    DrawTrait (frame, i, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, RO, op, fg);
			    break;
			 case '>':
			    DrawArrow (frame, i, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, RO, op, fg);
			    break;
			 case '^':
			    DrawArrow (frame, i, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, 90, RO, op, fg);
			    break;
			 case '<':
			    DrawArrow (frame, i, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, 180, RO, op, fg);
			    break;
			 case 'V':
			    DrawArrow (frame, i, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, 270, RO, op, fg);
			    break;
			 case '(':
			    DrawParenthesis (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, font, RO, op, fg);
			    break;
			 case ')':
			    DrawParenthesis (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, font, RO, op, fg);
			    break;
			 case '{':
			    DrawBrace (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, font, RO, op, fg);
			    break;
			 case '}':
			    DrawBrace (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, font, RO, op, fg);
			    break;
			 case '[':
			    DrawBracket (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, font, RO, op, fg);
			    break;
			 case ']':
			    DrawBracket (frame, i, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, font, RO, op, fg);
			    break;
			 default:
			    break;
		      }
		/* Est-ce qu'il faut completer la ligne avec des pointilles */
		if (pBox->BxEndOfBloc > 0)
		  {
		     pFrame = &FntrTable[frame - 1];
		     /* On calcule l'alignement des bases */
		     yd += pBox->BxHorizRef;
		     DrawPoints (frame, xd + pBox->BxWidth, yd, pBox->BxEndOfBloc, RO, op, fg);
		  }
	     }
	}
}				/* function DisplaySymbol */

/**
 *      DisplayEmptyBox affiche une boite vide dimensionnee et positionnee.
 *              On affiche une suite de caracteres de trame sur toute
 *              la surface de la boite.
 **/
#ifdef __STDC__
void                DisplayEmptyBox (PtrBox pBox, int frame, char modele)
#else  /* __STDC__ */
void                DisplayEmptyBox (pBox, frame, modele)
PtrBox            pBox;
int                 frame;
char                modele;

#endif /* __STDC__ */
{
   PtrBox            mbox;
   int                 op, RO;
   PtrAbstractBox             pAbbox1;
   ViewFrame            *pFrame;
   int                 xd, yd;

   /* On recherche la boite qui contraint l'englobement */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
	mbox = pAbbox1->AbBox;
	/* Si le pave englobant est eclate on saute au pave englobant entier */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   pFrame = &FntrTable[frame - 1];
   if (pBox->BxAbstractBox->AbVisibility >= pFrame->FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth > pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight > pBox->BxYOrg)
	{
	   if (pBox->BxAbstractBox->AbSensitive)
	      op = 1;
	   else
	      op = 0;
	   if (pBox->BxAbstractBox->AbReadOnly)
	      RO = 1;
	   else
	      RO = 0;
	   xd = pBox->BxXOrg - pFrame->FrXOrg;
	   yd = pBox->BxYOrg - pFrame->FrYOrg;

	   if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
	      DrawRectangle (frame, 2, 0, xd, yd, pBox->BxWidth,
			pBox->BxHeight, RO, op, pBox->BxAbstractBox->AbForeground,
			   pBox->BxAbstractBox->AbBackground, 0);
	   else
	      Trame (frame, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, RO, op,
		     pBox->BxAbstractBox->AbForeground,
		     pBox->BxAbstractBox->AbBackground, 4);
	}
}				/* function DisplayEmptyBox */


/**
 *      DisplayGraph affiche une boite graphique dimensionnee et positionnee
 *              dans la fenetre frame.
 **/
#ifdef __STDC__
static void         DisplayGraph (PtrBox pBox, int frame)
#else  /* __STDC__ */
static void         DisplayGraph (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, xd, yd;
   PtrBox            mbox;
   int                 op, RO;
   int                 fg;
   int                 bg;
   int                 style;
   PtrAbstractBox             pv;
   ViewFrame            *pFrame;

   /* On recherche la boite qui contraint l'englobement */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pv = pBox->BxAbstractBox->AbEnclosing;
	mbox = pv->AbBox;
	/* Si le pave englobant est eclate on saute au pave englobant entier */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   if (pBox->BxAbstractBox->AbVisibility >= FntrTable[frame - 1].FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth >= pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight >= pBox->BxYOrg)
	{
	   pFrame = &FntrTable[frame - 1];
	   xd = pBox->BxXOrg - pFrame->FrXOrg;
	   yd = pBox->BxYOrg - pFrame->FrYOrg;
	   if (pBox->BxAbstractBox->AbSensitive)
	      op = 1;
	   else
	      op = 0;
	   if (pBox->BxAbstractBox->AbReadOnly)
	      RO = 1;
	   else
	      RO = 0;

	   pv = pBox->BxAbstractBox;
	   /* Style et epaisseur du trace */
	   i = ValEpaisseur (pv);
	   switch (pv->AbLineStyle)
		 {
		    case 'S':
		       style = 0;
		       break;
		    case '-':
		       style = 3;
		       break;
		    case '.':
		       style = 1;
		       break;
		    default:
		       style = 0;
		 }

	   switch (pv->AbRealShape)
		 {
		    case '\260':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 2);
		       break;
		    case '\261':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 5);
		       break;
		    case '\262':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 6);
		       break;
		    case '\263':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 7);
		       break;
		    case '\264':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 8);
		       break;
		    case '\265':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 9);
		       break;
		    case '\266':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 1);
		       break;
		    case '\267':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 0);
		       break;
		    case '\270':
		       DrawRectangle (frame, 0, 0, xd, yd, pBox->BxWidth, pBox->BxHeight, RO, op, fg, bg, 4);
		       break;
		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case 'R':
		       DrawRectangle (frame, i, style, xd, yd, pBox->BxWidth,
			      pBox->BxHeight, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'C':
		       DrawOval (frame, i, style, xd, yd, pBox->BxWidth,
			      pBox->BxHeight, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'L':
		       DrawDiamond (frame, i, style, xd, yd, pBox->BxWidth,
			      pBox->BxHeight, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'c':
		       DrawEllips (frame, i, style, xd, yd, pBox->BxWidth,
			      pBox->BxHeight, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'h':
		       DrawLine (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, RO, op, fg);
		       break;
		    case 't':
		       DrawLine (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, RO, op, fg);
		       break;
		    case 'b':
		       DrawLine (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 2, RO, op, fg);
		       break;
		    case 'v':
		       DrawTrait (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, RO, op, fg);
		       break;
		    case 'l':
		       DrawTrait (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, RO, op, fg);
		       break;
		    case 'r':
		       DrawTrait (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 2, RO, op, fg);
		       break;
		    case '/':
		       DrawSlash (frame, i, style, xd, yd, pBox->BxWidth,
				  pBox->BxHeight, 0, RO, op, fg);
		       break;
		    case '\\':
		       DrawSlash (frame, i, style, xd, yd, pBox->BxWidth,
				  pBox->BxHeight, 1, RO, op, fg);
		       break;
		    case '>':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, RO, op, fg);
		       break;
		    case 'E':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth,
				 pBox->BxHeight, 45, RO, op, fg);
		       break;
		    case '^':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth,
				 pBox->BxHeight, 90, RO, op, fg);
		       break;
		    case 'O':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth,
				 pBox->BxHeight, 135, RO, op, fg);
		       break;
		    case '<':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth,
				 pBox->BxHeight, 180, RO, op, fg);
		       break;
		    case 'o':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth,
				 pBox->BxHeight, 225, RO, op, fg);
		       break;
		    case 'V':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth,
				 pBox->BxHeight, 270, RO, op, fg);
		       break;
		    case 'e':
		       DrawArrow (frame, i, style, xd, yd, pBox->BxWidth,
				 pBox->BxHeight, 315, RO, op, fg);
		       break;

		    case 'P':
		       DrawRectangleFrame (frame, i, style, xd, yd, pBox->BxWidth,
			      pBox->BxHeight, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'Q':
		       DrawEllipsFrame (frame, i, style, xd, yd, pBox->BxWidth,
			      pBox->BxHeight, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'W':
		       DrawAngle (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 0, RO, op, fg);
		       break;
		    case 'X':
		       DrawAngle (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 1, RO, op, fg);
		       break;
		    case 'Y':
		       DrawAngle (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 2, RO, op, fg);
		       break;
		    case 'Z':
		       DrawAngle (frame, i, style, xd, yd, pBox->BxWidth, pBox->BxHeight, 3, RO, op, fg);
		       break;

		    default:
		       break;
		 }

	   if (pBox->BxEndOfBloc > 0)
	     {
		/* On calcule l'alignement des bases */
		yd += pBox->BxHorizRef;
		DrawPoints (frame, xd + pBox->BxWidth, yd, pBox->BxEndOfBloc, RO, op, fg);
	     }
	}
}				/* function DisplayGraph */


/**
 *      PolyTransform regarde s'il faut de'former la boi^te polyline.
 *              Si le point limite correspond aux dimensions actuelles
 *              de la boi^te, la proce'dure ne fait rien. Sinon chaque
 *              point est de'cale' d'un rapport Largeur-Box/X-Limite
 *              en largeur et Hauteur-Box/Y-Limite en hauteur.
 **/

#ifdef __STDC__
void                PolyTransform (PtrBox pBox)

#else  /* __STDC__ */
void                PolyTransform (pBox)
PtrBox            pBox;

#endif /* __STDC__ */

{
   float               xRatio, yRatio, pointIndex;
   PtrTextBuffer      adbuff;
   int                 i;
   int                 j, val;

   /* calcul du rapport en X */
   val = PixelToPoint (pBox->BxWidth * 1000);
   if (val != pBox->BxBuffer->BuPoints[0].XCoord
       && pBox->BxBuffer->BuPoints[0].XCoord > 0)
     {
	pointIndex = (float) pBox->BxBuffer->BuPoints[0].XCoord / pBox->BxXRatio;
	/* memorise le nouveau rapport de deformation entre pave et boite */
	pBox->BxXRatio = (float) val / pointIndex;
	/* rapport applique sur la boite */
	xRatio = (float) val / (float) pBox->BxBuffer->BuPoints[0].XCoord;
	pBox->BxBuffer->BuPoints[0].XCoord = val;
     }
   else
      xRatio = 1;

   /* calcul du rapport en Y */
   val = PixelToPoint (pBox->BxHeight * 1000);
   if (val != pBox->BxBuffer->BuPoints[0].YCoord
       && pBox->BxBuffer->BuPoints[0].YCoord > 0)
     {
	pointIndex = (float) pBox->BxBuffer->BuPoints[0].YCoord / pBox->BxYRation;
	/* memorise le nouveau rapport de deformation entre pave et boite */
	pBox->BxYRation = (float) val / pointIndex;
	/* rapport applique sur la boite */
	yRatio = (float) val / (float) pBox->BxBuffer->BuPoints[0].YCoord;
	pBox->BxBuffer->BuPoints[0].YCoord = val;
     }
   else
      yRatio = 1;

   if (xRatio != 1 || yRatio != 1)
     {
	j = 1;
	adbuff = pBox->BxBuffer;
	val = pBox->BxNChars;
	for (i = 1; i < val; i++)
	  {
	     if (j >= adbuff->BuLength)
	       {
		  if (adbuff->BuNext != NULL)
		    {
		       /* Changement de buffer */
		       adbuff = adbuff->BuNext;
		       j = 0;
		    }
	       }
	     adbuff->BuPoints[j].XCoord = (int) ((float) adbuff->BuPoints[j].XCoord * xRatio);
	     adbuff->BuPoints[j].YCoord = (int) ((float) adbuff->BuPoints[j].YCoord * yRatio);
	     j++;
	  }
     }
}				/*PolyTransform */


/**
 *      DrawPolyLine affiche une boite polyline dimensionnee et
 *              positionnee dans la fenetre frame.
 **/

#ifdef __STDC__
static void         DrawPolyLine (PtrBox pBox, int frame)

#else  /* __STDC__ */
static void         DrawPolyLine (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */

{
   int                 i, xd, yd;
   PtrBox            mbox;
   int                 op, RO;
   int                 fg;
   int                 bg;
   int                 style, fleche;
   PtrAbstractBox             pv;
   ViewFrame            *pFrame;

   /* S'il n'y a pas de point defini, inutile de tracer la polyline */
   if (pBox->BxBuffer == NULL || pBox->BxNChars <= 1)
      return;

   /* Transformation de la polyline si la boite a change de taille */
   PolyTransform (pBox);

   /* On recherche la boite qui contraint l'englobement */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pv = pBox->BxAbstractBox->AbEnclosing;
	mbox = pv->AbBox;
	/* Si le pave englobant est eclate on saute au pave englobant entier */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   if (pBox->BxAbstractBox->AbVisibility >= FntrTable[frame - 1].FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth >= pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight >= pBox->BxYOrg)
	{
	   pFrame = &FntrTable[frame - 1];
	   xd = pBox->BxXOrg - pFrame->FrXOrg;
	   yd = pBox->BxYOrg - pFrame->FrYOrg;
	   if (pBox->BxAbstractBox->AbSensitive)
	      op = 1;
	   else
	      op = 0;

	   if (pBox->BxAbstractBox->AbReadOnly)
	      RO = 1;
	   else
	      RO = 0;

	   pv = pBox->BxAbstractBox;
	   /* Style et epaisseur du trace */
	   i = ValEpaisseur (pv);
	   switch (pv->AbLineStyle)
		 {
		    case 'S':
		       style = 0;
		       break;
		    case '-':
		       style = 3;
		       break;
		    case '.':
		       style = 1;
		       break;
		    default:
		       style = 0;
		 }

	   switch (pv->AbPolyLineShape)
		 {
		    case 'S':	/* Segments */
		    case 'U':	/* Segments vers avant */
		    case 'N':	/* Segments fleches vers arriere */
		    case 'M':	/* Segments fleches dans les deux sens */
		       if (pv->AbPolyLineShape == 'S')
			  fleche = 0;
		       else if (pv->AbPolyLineShape == 'U')
			  fleche = 1;
		       else if (pv->AbPolyLineShape == 'N')
			  fleche = 2;
		       else
			  fleche = 3;
		       DrawSegments (frame, i, style, xd, yd, pBox->BxBuffer, pBox->BxNChars, RO, op, fg, fleche);
		       break;
		    case 'B':	/* Beziers (ouvertes) */
		    case 'A':	/* Beziers (ouvertes) flechees vers avant */
		    case 'F':	/* Beziers (ouvertes) flechees vers arriere */
		    case 'D':	/* Beziers (ouvertes) flechees dns les deux sens */
		       if (pv->AbPolyLineShape == 'B')
			  fleche = 0;
		       else if (pv->AbPolyLineShape == 'A')
			  fleche = 1;
		       else if (pv->AbPolyLineShape == 'F')
			  fleche = 2;
		       else
			  fleche = 3;
		       /* calcul des points de controles */
		       if (pBox->BxPictInfo == NULL)
			  pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, pBox->BxNChars);
		       DrawCurb (frame, i, style, xd, yd, pBox->BxBuffer,
				 pBox->BxNChars, RO, op, fg, fleche, (C_points *) pBox->BxPictInfo);
		       break;
		    case 'p':	/* polygone */
		       DrawPolygon (frame, i, style, xd, yd, pBox->BxBuffer,
			     pBox->BxNChars, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 's':	/* spline fermee */
		       /* calcul des points de controles */
		       if (pBox->BxPictInfo == NULL)
			  pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, pBox->BxNChars);
		       DrawSpline (frame, i, style, xd, yd, pBox->BxBuffer,
				 pBox->BxNChars, RO, op, fg, bg, pv->AbFillPattern, (C_points *) pBox->BxPictInfo);
		       break;
		    default:
		       break;
		 }

	   if (pBox->BxEndOfBloc > 0)
	     {
		/* On calcule l'alignement des bases */
		yd += pBox->BxHorizRef;
		DrawPoints (frame, xd + pBox->BxWidth, yd, pBox->BxEndOfBloc, RO, op, fg);
	     }
	}
}				/* function DrawPolyLine */


/**
 *      DisplayJustifiedText affiche le contenu d'une boite en respectant la
 *              dimension des spaces pour ajuster la ligne dans la
 *              frame frame.
 *              Repartit les pixels restants (BxNPixels), un par un
 *              ajoute' a chaque space.
 **/

#ifdef __STDC__
static void         DisplayJustifiedText (PtrBox pBox, int frame)

#else  /* __STDC__ */
static void         DisplayJustifiedText (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */

{
   PtrTextBuffer      adbuff;
   int                 indbuff;
   int                 restbl;
   PtrTextBuffer      newbuff;
   int                 newind;
   int                 newbl, lg;
   int                 reste, dc;
   int                 buffrest;
   int                 indmax, bl;
   int                 nbcar, x, y;
   int                 lgspace;
   int                 fg;
   int                 bg;
   unsigned char       car;
   PtrBox            mbox;
   int                 RO;
   int                 op;
   PtrAbstractBox             pAbbox1;
   ViewFrame            *pFrame;
   PtrTextBuffer      pBu1;
   boolean             blockbegin;
   boolean             withbackground;
   boolean             withline;

   indmax = 0;
   buffrest = 0;
   /* On recherche la boite qui contraint l'englobement */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
	mbox = pAbbox1->AbBox;

	/* Si le pave englobant est eclate on saute au pave englobant entier */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   /* Est-ce que la boite est la premiere d'un bloc de ligne */
   /* ou une boite de texte isolee */
   if (mbox == pBox)
      blockbegin = TRUE;
   else if (mbox->BxType != BoBlock || mbox->BxFirstLine == NULL)
      blockbegin = TRUE;
   else if (pBox->BxType == BoComplete && mbox->BxFirstLine->LiFirstBox == pBox)
      blockbegin = TRUE;
   else if ((pBox->BxType == BoPiece || pBox->BxType == BoDotted)
	    && mbox->BxFirstLine->LiFirstPiece == pBox)
      blockbegin = TRUE;
   else
      blockbegin = FALSE;

   /* Faut-il engendrer un trait d'hyphenantion en fin de boite */
   if (pBox->BxType == BoDotted)
      withline = TRUE;
   else
      withline = FALSE;

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   withbackground = (pBox->BxAbstractBox->AbFillPattern == 2);

   if (pBox->BxNChars > 0)
      if (pBox->BxAbstractBox->AbVisibility >= FntrTable[frame - 1].FrVisibility)
	 if (mbox->BxXOrg + mbox->BxWidth > pBox->BxXOrg
	     && mbox->BxYOrg + mbox->BxHeight > pBox->BxYOrg)
	   {
	      /* Initialisation */
	      /* -------------- */
	      pFrame = &FntrTable[frame - 1];
	      x = pBox->BxXOrg - pFrame->FrXOrg;
	      y = pBox->BxYOrg - pFrame->FrYOrg;
	      bl = 0;
	      newind = pBox->BxFirstChar;
	      newbuff = pBox->BxBuffer;
	      reste = pBox->BxNChars;
	      newbl = pBox->BxNPixels;
	      lg = 0;
	      if (pBox->BxAbstractBox->AbSensitive)
		 op = 1;
	      else
		 op = 0;
	      if (pBox->BxAbstractBox->AbReadOnly)
		 RO = 1;
	      else
		 RO = 0;

	      lgspace = pBox->BxSpaceWidth;
	      if (lgspace == 0)
		 lgspace = CarWidth (BLANC, pBox->BxFont);

	      /* On recherche le premier caractere affichable dans la fenetre */
	      /* ------------------------------------------------------------ */
	      if (reste > 0)
		 do
		   {
		      pBu1 = newbuff;
		      indbuff = newind;
		      adbuff = newbuff;
		      restbl = newbl;
		      x += lg;
		      car = (unsigned char) (pBu1->BuContent[indbuff - 1]);
		      if (car == BLANC)
			{
			   lg = lgspace;
			   if (newbl > 0)
			     {
				newbl--;
				lg++;
			     }
			}
		      else
			 lg = CarWidth (car, pBox->BxFont);

		      reste--;
		      /* On passe au caractere suivant */
		      if (indbuff < pBu1->BuLength)
			 newind = indbuff + 1;
		      else
			{
			   if (pBu1->BuNext == NULL && reste > 0)
			      reste = 0;
			   newind = 1;
			   newbuff = pBu1->BuNext;
			}
		   }
		 while (!(x + lg > 0 || reste <= 0));

	      /* On affiche le texte contenu dans une liste de buffers pointee */
	      /* par adbuff a partir de l'index indbuff de longueur reste.     */
	      /* ------------------------------------------------------------- */
	      if (x + lg > 0)
		 reste++;
	      nbcar = 0;
	      if (adbuff == NULL)
		 reste = 0;
	      else
		{
		   pBu1 = adbuff;
		   buffrest = pBu1->BuLength - indbuff + 1;
		   if (reste > buffrest)
		      indmax = pBu1->BuLength;
		   else
		      indmax = indbuff - 1 + reste;
		}

	      /* Faut-il afficher une trame de fond */
	      if (withbackground)
		 DrawRectangle (frame, 0, 0, x, y,
			      pBox->BxWidth + pBox->BxXOrg - pFrame->FrXOrg - x,
			      FontHeight (pBox->BxFont), 0, 0, 0, bg, 2);
	      while (reste > 0)
		{
		   pBu1 = adbuff;
		   /* On passe en revue chaque caractere du buffer */
		   while (indbuff <= indmax)
		     {
			car = (unsigned char) (pBu1->BuContent[indbuff - 1]);

			if (car == BLANC || car == FINE
			    || car == DEMI_CADRATIN || car == BLANC_DUR)
			  {
			     /* Affiche les derniers caracteres passes */
			     dc = indbuff - nbcar;
			     x += DrawString (pBu1->BuContent, dc, nbcar, frame, x, y,
			     pBox->BxFont, 0, bl, 0, blockbegin, RO, op, fg);

			     if (!ShowSpace)
			       {
				  /* il faut visualiser les espaces */
				  if (car == BLANC)
				     DrawChar (SeeBlanc, frame, x, y, pBox->BxFont, RO, op, fg);
				  else if (car == FINE)
				     DrawChar (SeeFine, frame, x, y, pBox->BxFont, RO, op, fg);
				  else if (car == DEMI_CADRATIN)
				     DrawChar (SeeDemiCadratin, frame, x, y, pBox->BxFont, RO, op, fg);
				  else if (car == BLANC_DUR)
				     DrawChar (SeeBlancDur, frame, x, y, pBox->BxFont, RO, op, fg);
			       }

			     nbcar = 0;
			     if (car == BLANC)
				if (restbl > 0)
				  {
				     /* Repartition des pixels */
				     x = x + lgspace + 1;
				     restbl--;
				  }
				else
				   x += lgspace;
			     else
				x += CarWidth (car, pBox->BxFont);

			     bl = 1;
			  }

			else
			   /* Ajoute simplement le caractere */
			   nbcar++;

			indbuff++;	/* On passe au caractere suivant */
		     }

		   /* On affiche les derniers caracteres du buffer */
		   dc = indbuff - nbcar;
		   reste -= buffrest;
		   if (reste <= 0)
		     {
			/* C'est termine */
			x += DrawString (pBu1->BuContent, dc, nbcar, frame, x, y, pBox->BxFont,
			pBox->BxWidth, bl, withline, blockbegin, RO, op, fg);
			DisplayUnderline (frame, x, y, pBox->BxFont, pBox->BxUnderline,
				pBox->BxThickness, pBox->BxWidth, RO, op, fg);
			/* Regarde quel est le caractere suivant */
			if ((unsigned char) pBu1->BuContent[indbuff - 1] == SAUT_DE_LIGNE && !ShowSpace)
			   DrawChar (SeeCtrlRC, frame, x, y, pBox->BxFont, RO, op, fg);
		     }
		   else
		     {
			x += DrawString (pBu1->BuContent, dc, nbcar, frame, x, y,
			     pBox->BxFont, 0, bl, 0, blockbegin, RO, op, fg);
			bl = 0;
			/* On passe au buffer suivant */
			if (pBu1->BuNext == NULL)
			   reste = 0;
			else
			  {
			     indbuff = 1;
			     adbuff = pBu1->BuNext;
			     buffrest = adbuff->BuLength;
			     if (reste < buffrest)
				indmax = reste;
			     else
				indmax = buffrest;
			  }
		     }
		   nbcar = 0;
		}

	      /* Est-ce qu'il faut completer la ligne avec des pointilles */
	      if (pBox->BxEndOfBloc > 0)
		{
		   pFrame = &FntrTable[frame - 1];
		   /* On calcule l'alignement des bases */
		   y = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
		   DrawPoints (frame, pBox->BxXOrg + pBox->BxWidth - pFrame->FrXOrg, y,
			     pBox->BxEndOfBloc, RO, op, fg);
		}
	   }
}				/* function DisplayJustifiedText */

/**
 *      DisplayBox affiche une boite selon son contenu.
 **/
#ifdef __STDC__
void                DisplayBox (PtrBox pBox, int frame)
#else  /* __STDC__ */
void                DisplayBox (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */
{

   /* Empty */
   if (pBox->BxAbstractBox->AbVolume == 0)
     {
	if (ThotLocalActions[T_emptybox] != NULL)
	   (*ThotLocalActions[T_emptybox]) (pBox, frame, '2');
     }
   /* Text */
   else if (pBox->BxAbstractBox->AbLeafType == LtText)
      /* Affiche une boite texte justifiee ou non */
      DisplayJustifiedText (pBox, frame);
   /* Picture */
   else if (pBox->BxType == BoPicture)
      DisplayImage (pBox, frame);
   /* Symbol */
   else if (pBox->BxAbstractBox->AbLeafType == LtSymbol)
      if (pBox->BxAbstractBox->AbShape == '\0')
	 DisplayEmptyBox (pBox, frame, '2');
      else
	 DisplaySymbol (pBox, frame);
   /* Graphics */
   else if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
      if (pBox->BxAbstractBox->AbShape == '\0')
	 DisplayEmptyBox (pBox, frame, '2');
      else
	 DisplayGraph (pBox, frame);
   /* Polyline */
   else if (pBox->BxAbstractBox->AbLeafType == LtPlyLine)
      if (pBox->BxNChars == 1)
	 DisplayEmptyBox (pBox, frame, '2');
      else
	 DrawPolyLine (pBox, frame);
}				/*procedure DisplayBox */


/* End Of Module aff */
