
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   transform.c : gestion des transformations sur        les pixmaps
   Major changes
   PMA - juin 90  creation
   PMA - juin 91  remise en forme
 */
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "imagedrvr.h"
#include "frame.h"

#undef EXPORT
#define EXPORT extern
#include "frame.var"

#define MAX(x,y) (x>y?x:y)
#define MIN(x,y) (x<y?x:y)

#ifdef WWW_XWINDOWS

/* ---------------------------------------------------------------------- */
/* |    GetMeanColor retourne la couleur moyenne d'une zone de image    | */
/* |            definie par width, height, x_orig, y_orig.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static unsigned long GetMeanColor (XImage * image, float width, float height, float x_orig, float y_orig)

#else  /* __STDC__ */
static unsigned long GetMeanColor (image, width, height, x_orig, y_orig)
XImage             *image;
float               width;
float               height;
float               x_orig;
float               y_orig;

#endif /* __STDC__ */

{
   int                 black_total, white_total;
   int                 x, y, increment;

   black_total = 0;
   white_total = 0;

   /* la precision d'analyse depend de la taille de la zone a analyser */
   increment = (int) (width * height / 4.0) + 1;
   for (x = 0; x <= (int) (width + 0.5); x += increment)
     {
	for (y = 0; y <= (int) (height + 0.5); y += increment)
	  {
#ifndef NEW_WILLOWS
	     if (XGetPixel (image, (int) (x_orig + 0.5) + x, (int) (y_orig + 0.5) + y) == White_Color)
		white_total++;
	     else
#endif /* NEW_WILLOWS */
		black_total++;
	  }
     }
   if (black_total >= white_total)
      return Black_Color;
   else
      return White_Color;
}


/* ---------------------------------------------------------------------- */
/* |    GrowPixmap permet de creer un nouveau pixmap a` partir d'une    | */
/* |            image en l'agrandissant d'un facteur agrand_w et        | */
/* |            agrand_h.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static Pixmap       GrowPixmap (XImage * image, float agrand_w, float agrand_h)

#else  /* __STDC__ */
static Pixmap       GrowPixmap (image, agrand_w, agrand_h)
XImage             *image;
float               agrand_w;
float               agrand_h;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   Pixmap              newpix;
   int                 line, column, max_lines, max_columns;
   int                 current_largeur, pos_x, pos_y;
   unsigned long       pixel, current_color;
   int                 new_w, new_h, taille, conv1, const_h;
   Bool                aroundSize;
   float               arrondi;

   /* TODO: prevoir le cas ou new_w=max_columns et new_h=max_lines */
   aroundSize = (MIN (agrand_w, agrand_h) < 2.0);
   if (aroundSize)
      arrondi = 1.0;
   else
      arrondi = 0.5;
   max_columns = image->width;	/* on prend l'ancienne largeur */
   max_lines = image->height;	/* on prend l'ancienne hauteur */
   new_w = max_columns * agrand_w + 0.5;
   new_h = max_lines * agrand_h + 0.5;
   newpix = XCreatePixmap (GDp (0), GRootW (0), new_w, new_h, DefaultDepth (GDp (0), ThotScreen (0)));
   const_h = agrand_h + 1.0;
   if ((new_w == max_columns) && (new_h == max_lines))
     {
	XPutImage (GDp (0), newpix, GCwhite (0), image, 0, 0, 0, 0, new_w, new_h);
     }
   else
     {
	XFillRectangle (GDp (0), newpix, GCblack (0), 0, 0, new_w, new_h);
	for (line = 0; line <= max_lines; line++)
	  {
	     current_color = XGetPixel (image, 0, line);
	     current_largeur = 0;
	     pos_x = 0;
	     pos_y = line * agrand_h + 0.5;
	     for (column = 0; column <= max_columns; column++)
	       {
		  pixel = XGetPixel (image, column, line);
		  if (pixel == current_color)
		     current_largeur += 1;
		  else
		    {
		       conv1 = pos_x * agrand_w + 0.5;
		       taille = current_largeur * agrand_w + arrondi;
		       if (current_color == White_Color)
			  XFillRectangle (GDp (0), newpix, GCwhite (0), conv1, pos_y, taille, const_h);
		       else if (aroundSize)
			  XFillRectangle (GDp (0), newpix, GCblack (0), conv1, pos_y, taille, const_h);
		       pos_x = column;
		       current_largeur = 1;
		       current_color = pixel;
		    }
	       }
	     conv1 = pos_x * agrand_w + 0.5;
	     taille = current_largeur * agrand_w + arrondi;
	     if (current_color == White_Color)
		XFillRectangle (GDp (0), newpix, GCwhite (0), conv1, pos_y, taille, const_h);
	     else if (aroundSize)
		XFillRectangle (GDp (0), newpix, GCblack (0), conv1, pos_y, taille, const_h);
	  }
     }
   return newpix;
#endif /* NEW_WILLOWS */
}
#define MAXPOINT 200

#ifndef NEW_WILLOWS
static ThotPoint    imagepoints[MAXPOINT];

#endif /* NEW_WILLOWS */
static int          nbpoints;

#ifdef __STDC__
/* ---------------------------------------------------------------------- */
/* |    DrawThisPoint effectue une mise en buffer des points.           | */
/* ---------------------------------------------------------------------- */
static void         DrawThisPoint (Pixmap pix, int x, int y, int end)

#else  /* __STDC__ */
/* ---------------------------------------------------------------------- */
/* |    DrawThisPoint effectue une mise en buffer des points.           | */
/* ---------------------------------------------------------------------- */
static void         DrawThisPoint (pix, x, y, end)
Pixmap              pix;
int                 x;
int                 y;
int                 end;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   if (end == 0)
     {
	imagepoints[nbpoints].x = x;
	imagepoints[nbpoints].y = y;
	nbpoints++;
	if (nbpoints == MAXPOINT - 1)
	  {
	     XDrawPoints (GDp (0), pix, GCwhite (0), imagepoints, nbpoints, CoordModeOrigin);
	     nbpoints = 0;
	  }
     }
   else
     {
	XDrawPoints (GDp (0), pix, GCwhite (0), imagepoints, nbpoints, CoordModeOrigin);
     }
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ShrinkAllPixmap permet de creer un nouveau pixmap a` partir     | */
/* |            d'une image en la reduisant d'un facteur reduc_w et     | */
/* |            reduc_h.                                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static Pixmap       ShrinkAllPixmap (XImage * image, float reduc_w, float reduc_h)

#else  /* __STDC__ */
static Pixmap       ShrinkAllPixmap (image, reduc_w, reduc_h)
XImage             *image;
float               reduc_w;
float               reduc_h;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   Pixmap              newpix;
   int                 line;
   int                 column;
   int                 max_lines;
   int                 max_columns;
   unsigned long       pixel;
   float               pos_x, pos_y;

   nbpoints = 0;

   max_columns = image->width / reduc_w + 0.5;	/* on calcule la nouvelle largeur */
   max_lines = image->height / reduc_h + 0.5;	/* on calcule la nouvelle hauteur */
   newpix = XCreatePixmap (GDp (0), GRootW (0), max_columns, max_lines, DefaultDepth (GDp (0), ThotScreen (0)));
   XFillRectangle (GDp (0), newpix, GCblack (0), 0, 0, max_columns, max_lines);
   for (line = 0; line <= max_lines; line++)
     {
	for (column = 0; column <= max_columns; column++)
	  {
	     /* on calcule la couleur moyenne d'une zone de taille reduc_w, reduc_h */
	     /* situee en (column * reduc_w, line * reduc_h) dans image             */
	     pos_x = column * reduc_w;
	     pos_y = line * reduc_h;
	     pixel = GetMeanColor (image, reduc_w, reduc_h, pos_x, pos_y);
	     if (pixel == White_Color)
		DrawThisPoint (newpix, column, line, 0);
	  }
     }
   DrawThisPoint (newpix, column, line, 1);

   return newpix;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ShrinkW permet de creer un nouveau pixmap a` partir d'une image | */
/* |            - en la reduisant de reduc_w dans la largeur.           | */
/* |            - en l'agrandissant de agrand_h dans la hauteur.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static Pixmap       ShrinkW (XImage * image, float reduc_w, float agrand_h)

#else  /* __STDC__ */
static Pixmap       ShrinkW (image, reduc_w, agrand_h)
XImage             *image;
float               reduc_w;
float               agrand_h;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   Pixmap              newpix;
   int                 line, column;
   int                 max_lines, max_columns;
   unsigned long       pixel, current_color;
   int                 largeur, pos_x;
   int                 new_y;
   int                 conv1;
   int                 const_h;
   float               x_pos, y_pos;
   Bool                aroundSize;

   aroundSize = (agrand_h < 2.0);
   max_columns = image->width / reduc_w + 0.5;	/* on calcule la nouvelle largeur */
   max_lines = image->height;	/* on prend l'ancienne hauteur */
   new_y = max_lines * agrand_h + 0.5;
   newpix = XCreatePixmap (GDp (0), GRootW (0), max_columns,
			   new_y, DefaultDepth (GDp (0), ThotScreen (0)));
   const_h = agrand_h + 1.0;
   XFillRectangle (GDp (0), newpix, GCblack (0), 0, 0, max_columns, new_y);
   for (line = 0; line <= max_lines; line++)
     {
	y_pos = line;
	current_color = GetMeanColor (image, reduc_w, 1.0, 0.0, y_pos);
	largeur = 0;
	pos_x = 0;
	for (column = 0; column <= max_columns; column++)
	  {
	     /* on calcule la couleur moyenne d'une zone de reduc_w, 1 */
	     /* situee en (column * reduc_w, line) dans image          */
	     x_pos = column * reduc_w;
	     y_pos = line;
	     pixel = GetMeanColor (image, reduc_w, 1.0, x_pos, y_pos);
	     if (pixel == current_color)
		largeur += 1;
	     else
	       {
		  conv1 = line * agrand_h + 0.5;
		  if (current_color == White_Color)
		     XFillRectangle (GDp (0), newpix, GCwhite (0), pos_x, conv1, largeur, const_h);
		  else if (aroundSize)
		     XFillRectangle (GDp (0), newpix, GCblack (0), pos_x, conv1, largeur, const_h);
		  pos_x = column;
		  largeur = 1;
		  current_color = pixel;
	       }
	  }
	conv1 = line * agrand_h + 0.5;
	if (current_color == White_Color)
	   XFillRectangle (GDp (0), newpix, GCwhite (0), pos_x, conv1, largeur, const_h);
	else if (aroundSize)
	   XFillRectangle (GDp (0), newpix, GCblack (0), pos_x, conv1, largeur, const_h);
     }
   return newpix;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ShrinkH permet de creer un nouveau pixmap a` partir d'une image | */
/* |            - en l'agrandissant de agrand_w dans la largeur.        | */
/* |            - en la reduisant de reduc_h dans la hauteur.           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static Pixmap       ShrinkH (XImage * image, float agrand_w, float reduc_h)

#else  /* __STDC__ */
static Pixmap       ShrinkH (image, agrand_w, reduc_h)
XImage             *image;
float               agrand_w;
float               reduc_h;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   Pixmap              newpix;
   int                 line, column;
   int                 max_lines, max_columns;
   unsigned long       pixel, current_color;
   int                 largeur, pos_x;
   int                 new_x;
   int                 taille;
   int                 conv1;
   float               x_pos, y_pos;

   max_columns = image->width;	/* on prend l'ancienne largeur */
   max_lines = image->height / reduc_h + 0.5;	/* on calcule la nouvelle hauteur */
   new_x = max_columns * agrand_w + 0.5;
   newpix = XCreatePixmap (GDp (0), GRootW (0), new_x,
			 max_lines, DefaultDepth (GDp (0), ThotScreen (0)));

   XFillRectangle (GDp (0), newpix, GCblack (0), 0, 0, new_x, max_lines);
   for (line = 0; line <= max_lines; line++)
     {
	y_pos = line * reduc_h;
	current_color = GetMeanColor (image, 1.0, reduc_h, 0.0, y_pos);
	largeur = 0;
	pos_x = 0;
	for (column = 0; column <= max_columns; column++)
	  {
	     /* on calcule la couleur moyenne d'une zone de 1, reduc_h   */
	     /* situee en (column, line * reduc_h) dans image            */
	     x_pos = column;
	     y_pos = line * reduc_h;
	     pixel = GetMeanColor (image, 1.0, reduc_h, x_pos, y_pos);
	     if (pixel == current_color)
		largeur += 1;
	     else
	       {
		  conv1 = pos_x * agrand_w + 0.5;
		  taille = largeur * agrand_w + 0.5;
		  if (current_color == White_Color)
		     XFillRectangle (GDp (0), newpix, GCwhite (0), conv1, line, taille, 1);
		  pos_x = column;
		  largeur = 1;
		  current_color = pixel;
	       }
	  }
	conv1 = pos_x * agrand_w + 0.5;
	taille = largeur * agrand_w + 0.5;
	if (current_color == White_Color)
	   XFillRectangle (GDp (0), newpix, GCwhite (0), conv1, line, taille, 1);
     }
   return newpix;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    GetStretchSize calcule les dimensions de l'image a` afficher.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetStretchSize (int orig_w, int orig_h, int dest_w, int dest_h, PictureScaling pres, int *presW, int *presH)

#else  /* __STDC__ */
void                GetStretchSize (orig_w, orig_h, dest_w, dest_h, pres, presW, presH)
int                 orig_w;
int                 orig_h;
int                 dest_w;
int                 dest_h;
PictureScaling           pres;
int                *presW;
int                *presH;

#endif /* __STDC__ */

{
   float               Rapw, Raph;

   switch (pres)
	 {
	    case ReScale:
	       Rapw = (float) orig_w / (float) dest_w;
	       Raph = (float) orig_h / (float) dest_h;
	       if (Rapw <= Raph)
		 {
		    *presH = dest_h;
		    *presW = orig_w / Raph;
		 }
	       else
		 {
		    *presH = orig_h / Rapw;
		    *presW = dest_w;
		 }
	       break;
	    case FillFrame:
	       *presH = dest_h;
	       *presW = dest_w;
	       break;
	    case RealSize:
	       *presH = orig_h;
	       *presW = orig_w;
	       break;
	 }
}


/* ----------------------------------------------------------------------- */
/* | StretchPixmap permet de creer un nouveau pixmap a partir d'une      | */
/* | image X en modifiant ses dimensions pour qu'il rentre dans un       | */
/* | rectangle new_w, new_h. La fonction reduit ou agrandit le pixmap.   | */
/* ----------------------------------------------------------------------- */

#ifdef __STDC__
Pixmap              StretchPixmap (XImage * image, int new_w, int new_h)

#else  /* __STDC__ */
Pixmap              StretchPixmap (image, new_w, new_h)
XImage             *image;
int                 new_w;
int                 new_h;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   float               rap_reduc_w, rap_reduc_h, rap_agrand_w, rap_agrand_h;

   int                 orig_w, orig_h;
   Pixmap              newpix;

   if (image)
     {
	orig_w = image->width;
	orig_h = image->height;
	rap_agrand_w = 0;
	rap_agrand_h = 0;
	rap_reduc_h = 0;
	rap_reduc_w = 0;

	if (new_w < orig_w)
	   rap_reduc_w = ((float) orig_w / (float) new_w);
	else
	   rap_agrand_w = ((float) new_w / (float) orig_w);

	if (new_h < orig_h)
	   rap_reduc_h = ((float) orig_h / (float) new_h);
	else
	   rap_agrand_h = ((float) new_h / (float) orig_h);

	if (rap_agrand_h != 0)
	   if (rap_agrand_w != 0)	/* on agrandit les 2 dimensions */
	      newpix = GrowPixmap (image, rap_agrand_w, rap_agrand_h);
	   else			/* on diminue la largeur */
	      newpix = ShrinkW (image, rap_reduc_w, rap_agrand_h);
	else if (rap_agrand_w != 0)	/* on diminue la hauteur */
	   newpix = ShrinkH (image, rap_agrand_w, rap_reduc_h);
	else			/* on diminue les 2 dimensions */
	   newpix = ShrinkAllPixmap (image, rap_reduc_w, rap_reduc_h);

	XDestroyImage (image);
	return newpix;
     }
   else
      return None;
#endif /* NEW_WILLOWS */
}

#endif /* WWW_XWINDOWS */
