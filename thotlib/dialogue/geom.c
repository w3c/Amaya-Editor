
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   geom.c : gestion de la geometrie des boites
   I. Vatton - Juillet 88
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"

#undef EXPORT
#define EXPORT extern
#include "frame.var"
static int          PasGrille = 1;

#define ALIGNE(val) ((val + (PasGrille/2)) / PasGrille) * PasGrille

#include "appli.f"
#include "es.f"
#include "font.f"
#include "memory.f"
#include "textelem.f"


/* ---------------------------------------------------------------------- */
/* |    GeomBoite affiche la geometrie de la pseudo-boite dans la       | */
/* |            frame frame.                                            | */
/* |            Le parametre avecgrille indique s'il faut ou non        | */
/* |            visualiser la grille.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         GeomBoite (int frame, int x, int y, int large, int haut, int xr, int yr, boolean avecgrille)

#else  /* __STDC__ */
static void         GeomBoite (frame, x, y, large, haut, xr, yr, avecgrille)
int                 frame;
int                 x;
int                 y;
int                 large;
int                 haut;
int                 xr;
int                 yr;
boolean             avecgrille;

#endif /* __STDC__ */

{
#define pas 6

   if (large > 0)
      /*cote superieur */
      Invideo (frame, large, 1, x, y);
   if (haut > 1)
     {
	Invideo (frame, 1, haut - 1, x, y + 1);
	/*cote gauche */
	Invideo (frame, 1, haut - 1, x + large - 1, y + 1);
	/*cote droit */
     }
   if (large > 1)
      /*cote inferieur */
      Invideo (frame, large - 1, 1, x + 1, y + haut - 1);
   /*point de reference */
   Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xr, yr);
#ifdef IV
   if (avecgrille)
     {
	for (i = pas; i < large; i += pas)
	   Invideo (frame, 1, haut - 1, x + i, y + 1);
	for (i = pas; i < haut; i += pas)
	   Invideo (frame, large - 1, 1, x + 1, y + i);
     }
#endif
}

/*debut */
/* ---------------------------------------------------------------------- */
/* |    RedrawPolyLine affiche l'etat courant de la polyline, ferme'e   | */
/* |            ou non, dans la fenetree^tre frame.                             | */
/* |    Retourne les coordonnees des points de controles :              | */
/* |    - x1, y1 coordonnees du point point-1                           | */
/* |    - x2, y2 coordonnees du point point                             | */
/* |    - x3, y3 coordonnees du point point+1                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         RedrawPolyLine (int frame, int x, int y, PtrTextBuffer buffer, int nb, int point, boolean close, int *x1, int *y1, int *x2, int *y2, int *x3, int *y3)

#else  /* __STDC__ */
static void         RedrawPolyLine (frame, x, y, buffer, nb, point, close, x1, y1, x2, y2, x3, y3)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer      buffer;
int                 nb;
int                 point;
boolean             close;
int                *x1;
int                *y1;
int                *x2;
int                *y2;
int                *x3;
int                *y3;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   ThotPoint          *points;
   int                 i, j;
   PtrTextBuffer      adbuff;

   *x1 = *y1 = *x2 = *y2 = *x3 = *y3 = -1;
   /* Alloue une table de points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb));
   adbuff = buffer;
   j = 1;
   for (i = 1; i < nb; i++)
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
	points[i - 1].x = x + FrameTable[frame].FrLeftMargin + PtEnPixel (adbuff->BuPoints[j].XCoord / 1000, 0);
	points[i - 1].y = y + FrameTable[frame].FrTopMargin + PtEnPixel (adbuff->BuPoints[j].YCoord / 1000, 1);
	/* note les points i-1, i et i+1 */
	if (i == point - 1)
	  {
	     *x1 = points[i - 1].x;
	     *y1 = points[i - 1].y;
	  }
	else if (i == point)
	  {
	     *x2 = points[i - 1].x;
	     *y2 = points[i - 1].y;
	  }
	if (i == point + 1)
	  {
	     *x3 = points[i - 1].x;
	     *y3 = points[i - 1].y;
	  }
	j++;
     }

   /* On trace le contour */
   if (close && nb > 3)
     {
	/* La courbe est fermee et contient plus de 2 points */
	points[nb - 1].x = points[0].x;
	points[nb - 1].y = points[0].y;
	if (point == 1)
	  {
	     *x1 = points[nb - 2].x;
	     *y1 = points[nb - 2].y;
	  }
	if (point == nb - 1)
	  {
	     *x3 = points[0].x;
	     *y3 = points[0].y;
	  }
	XDrawLines (GDp (0), FrRef[frame], GCinvert (0), points, nb, CoordModeOrigin);
     }
   else
      XDrawLines (GDp (0), FrRef[frame], GCinvert (0), points, nb - 1, CoordModeOrigin);

   /* Libere la table de points */
   free ((char *) points);
#endif	/* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    PolyLineCreation saisit les points d'une polyline affiche'e     | */
/* |            dans la fenetre frame.                                  | */
/* |            Les valeurs x, y donnent la position de la boi^te dans  | */
/* |            la fenetree^tre. Le parame`tre Pbuffer pointe sur le    | */
/* |            premier buffer du pave', donc la liste des points de    | */
/* |            contro^le de la polyline d'origine. Le parame`tre       | */
/* |            Bbuffer pointe sur le premier buffer de la boi^te, donc | */
/* |            la liste des points de contro^le modifie's affiche's.   | */
/* |            Le premier point de chaque liste donne la largeur et la | */
/* |            hauteur limite de la polyline.                          | */
/* |            La procedure met a` jour les deux listes de points      | */
/* |            de contro^le.                                           | */
/* |            Retourne le nombre de points de la polyline.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 PolyLineCreation (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer)

#else  /* __STDC__ */
int                 PolyLineCreation (frame, x, y, Pbuffer, Bbuffer)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer      Pbuffer;
PtrTextBuffer      Bbuffer;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   float               rapportX, rapportY;
   int                 large, haut;
   int                 e, dx, dy;
   int                 ret, f;
   int                 newx, newy, lastx, lasty;
   int                 x1, y1, nbpoints;
   ThotWindow          w, wdum;
   XEvent              event;

   /* les dimensions de la boite */
   large = Bbuffer->BuPoints[0].XCoord;
   haut = Bbuffer->BuPoints[0].YCoord;
   /* calcule les rapports de deformation entre la boite et le pave */
   rapportX = (float) Pbuffer->BuPoints[0].XCoord / (float) large;
   rapportY = (float) Pbuffer->BuPoints[0].YCoord / (float) haut;
   large = PtEnPixel (large / 1000, 0);
   haut = PtEnPixel (haut / 1000, 1);

   /*Changement de curseur */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
   XMapRaised (GDp (0), w);
   XFlush (GDp (0));
   ThotGrab (w, CursDepHV (0), e, 0);

   /* Le pas de la grille commence sur l'origine de l'englobante */
   lastx = x + FrameTable[frame].FrLeftMargin;
   lasty = y + FrameTable[frame].FrTopMargin;
   x1 = -1;
   y1 = -1;
   nbpoints = 1;
   XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, lastx, lasty);
   XFlush (GDp (0));

   /* affiche la boite contour */
   /*Clear(frame, large, haut, x, y); */
   GeomBoite (frame, x, y, large, haut, x + large - 2, y + haut - 2, True);

   /* BOUCLE d'attente de definition d'un point de controle */
   ret = 0;
   while (ret == 0)
     {
	if (XPending (GDp (0)) == 0)
	  {

	     /* position actuelle du curseur */
	     XQueryPointer (GDp (0), w, &wdum, &wdum, &dx, &dy, &newx, &newy, &e);
	     /* Verification des coordonnees */
	     newx = ALIGNE (newx - FrameTable[frame].FrLeftMargin - x);
	     newx += x;
	     newy = ALIGNE (newy - FrameTable[frame].FrTopMargin - y);
	     newy += y;
	     if (newx < x || newx > x + large || newy < y || newy > y + haut)
	       {
		  /* Il faut revenir dans les bornes de la boite */
		  if (newx < x)
		     newx = x + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
		  else if (newx > x + large)
		     newx = x + large + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
		  else
		     newx += FrameTable[frame].FrLeftMargin;

		  if (newy < y)
		     newy = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
		  else if (newy > y + haut)
		     newy = y + haut + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
		  else
		     newy += FrameTable[frame].FrTopMargin;
		  XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, newx, newy);
	       }
	     else
	       {
		  newx += FrameTable[frame].FrLeftMargin;
		  newy += FrameTable[frame].FrTopMargin;
	       }

	     /* Affiche le segment en construction */
	     if (x1 != -1 && (newx != lastx || newy != lasty))
	       {
		  XDrawLine (GDp (0), FrRef[frame], GCinvert (0), x1, y1, lastx, lasty);
		  XDrawLine (GDp (0), FrRef[frame], GCinvert (0), x1, y1, newx, newy);
		  XFlush (GDp (0));
	       }
	     lastx = newx;
	     lasty = newy;
	  }
	else
	  {
	     XNextEvent (GDp (0), &event);
	     /* Verification des coordonnees */
	     newx = x + ALIGNE ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - x);
	     newy = y + ALIGNE ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
	     /* Il faut revenir dans les bornes de la boite */
	     if (newx < x)
		lastx = x + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
	     else if (newx > x + large)
		lastx = x + large + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
	     else
		lastx = newx + FrameTable[frame].FrLeftMargin;

	     if (newy < y)
		lasty = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
	     else if (newy > y + haut)
		lasty = y + haut + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
	     else
		lasty = newy + FrameTable[frame].FrTopMargin;

	     switch (event.type)
		   {
/*-BOUTON ENFONCE-*/
		      case ButtonPress:
			 if (newx < x || newx > x + large || newy < y || newy > y + haut)
			    XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, lastx, lasty);
			 break;

/*-BOUTON RELACHE-*/
		      case ButtonRelease:
			 if (event.xbutton.button != Button3)
			   {
			      /* Bouton gauche -> garde le dernier segment construit */
			      /* Note le point de depart du nouveau segment */
			      x1 = lastx;
			      y1 = lasty;
			      nbpoints++;

			      /* Met a jour les buffers de la boite */
			      newx = PixelEnPt (lastx - FrameTable[frame].FrLeftMargin - x, 0) * 1000;
			      newy = PixelEnPt (lasty - FrameTable[frame].FrTopMargin - y, 1) * 1000;
			      AddPointInPolyline (Bbuffer, nbpoints, newx, newy);
			      /* Met a jour les buffers du pave */
			      newx = (int) ((float) newx * rapportX);
			      newy = (int) ((float) newy * rapportY);
			      AddPointInPolyline (Pbuffer, nbpoints, newx, newy);

			      if (event.xbutton.button != Button1)
				 /* Un autre bouton -> fin de la construction */
				 ret = 1;
			   }

			 break;

		      case Expose:
			 f = GetFenetre (event.xexpose.window);
			 if (f <= MAX_FRAME + 1)
			    TraiteExpose (event.xexpose.window, f, (XExposeEvent *) & event);
			 XtDispatchEvent (&event);
			 break;

/*-OTHER-*/
		      default:
			 break;
		   }		/*switch */
	  }
     }

   /* efface la boite contour */
   GeomBoite (frame, x, y, large, haut, x + large - 2, y + haut - 2, True);

   /* On restaure l'etat anterieur */
   ThotUngrab ();
   XFlush (GDp (0));
   /* On impose au moins deux points de controle pour un polyline valide */
   if (nbpoints < 3)
     {
	TtaDisplaySimpleMessage (LIB, INFO, LIB_AT_LEAST_TWO_POINTS_IN_POLYLINE);
	return 1;
     }
   else
      return nbpoints;
#endif /* NEW_WILLOWS */
}				/*PolyLineCreation */


/* ---------------------------------------------------------------------- */
/* |    PolyLineModification de'place un point d'une polyline affiche'e | */
/* |            dans la fenetree^tre frame.                                     | */
/* |            Les valeurs x, y donnent la position de la boi^te dans  | */
/* |            la fenetree^tre. Le parame`tre Pbuffer pointe sur le    | */
/* |            premier buffer du pave', donc la liste des points de    | */
/* |            contro^le de la polyline d'origine. Le parame`tre       | */
/* |            Bbuffer pointe sur le premier buffer de la boi^te, donc | */
/* |            la liste des points de contro^le affiche's.             | */
/* |            Le premier point de chaque liste donne la largeur et la | */
/* |            hauteur limite de la polyline.                          | */
/* |            nbpoints indique le nombre de points de contro^le, y    | */
/* |            compris le point limite, point indique le point         | */
/* |            modifie' et close est vrai si la polyline est ferme'e.  | */
/* |            La procedure met a` jour les deux listes de points      | */
/* |            de contro^le.                                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                PolyLineModification (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close)

#else  /* __STDC__ */
void                PolyLineModification (frame, x, y, Pbuffer, Bbuffer, nbpoints, point, close)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer      Pbuffer;
PtrTextBuffer      Bbuffer;
int                 nbpoints;
int                 point;
boolean             close;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   float               rapportX, rapportY;
   int                 large, haut;
   int                 e;
   int                 ret, f;
   int                 newx, newy, lastx, lasty;
   int                 x1, y1, x3, y3;
   ThotWindow          w;
   XEvent              event;
   boolean             wrap;

   /* les dimensions de la boite */
   large = Bbuffer->BuPoints[0].XCoord;
   haut = Bbuffer->BuPoints[0].YCoord;
   /* calcule les rapports de deformation entre la boite et le pave */
   rapportX = (float) Pbuffer->BuPoints[0].XCoord / (float) large;
   rapportY = (float) Pbuffer->BuPoints[0].YCoord / (float) haut;
   large = PtEnPixel (large / 1000, 0);
   haut = PtEnPixel (haut / 1000, 1);

   /*Changement de curseur */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
   XMapRaised (GDp (0), w);
   XFlush (GDp (0));
   wrap = False;
   ThotGrab (w, CursDepHV (0), e, 0);

   /* affiche la boite contour */
   /*Clear(frame, large, haut, x, y); */
   GeomBoite (frame, x, y, large, haut, x + large - 2, y + haut - 2, True);
   RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close,
		   &x1, &y1, &lastx, &lasty, &x3, &y3);
   /* Le pas de la grille commence sur l'origine de l'englobante */
   XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, lastx, lasty);
   XFlush (GDp (0));

   /* BOUCLE d'attente de definition d'un point de controle */
   ret = 0;
   while (ret == 0)
     {
	XNextEvent (GDp (0), &event);

	/* Verification des coordonnees */
	newx = x + ALIGNE ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - x);
	newy = y + ALIGNE ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
	/* Il faut revenir dans les bornes de la boite */
	if (newx < x)
	  {
	     newx = x + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
	     wrap = True;
	  }
	else if (newx > x + large)
	  {
	     newx = x + large + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
	     wrap = True;
	  }
	else
	   newx += FrameTable[frame].FrLeftMargin;

	if (newy < y)
	  {
	     newy = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
	     wrap = True;
	  }
	else if (newy > y + haut)
	  {
	     newy = y + haut + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
	     wrap = True;
	  }
	else
	   newy += FrameTable[frame].FrTopMargin;

	switch (event.type)
	      {
/*-BOUTON RELACHE-*/
		 case ButtonRelease:
		    lastx = newx;
		    lasty = newy;
		    /* Met a jour les buffers de la boite */
		    newx = PixelEnPt (lastx - FrameTable[frame].FrLeftMargin - x, 0) * 1000;
		    newy = PixelEnPt (lasty - FrameTable[frame].FrTopMargin - y, 1) * 1000;
		    ModifyPointInPolyline (Bbuffer, point, newx, newy);
		    /* Met a jour les buffers du pave */
		    newx = (int) ((float) newx * rapportX);
		    newy = (int) ((float) newy * rapportY);
		    ModifyPointInPolyline (Pbuffer, point, newx, newy);
		    ret = 1;
		    break;

		 case MotionNotify:
		    /* Affiche les segments en construction */
		    if (newx != lastx || newy != lasty)
		      {
			 if (x1 != -1)
			   {
			      XDrawLine (GDp (0), FrRef[frame], GCinvert (0), x1, y1, lastx, lasty);
			      XDrawLine (GDp (0), FrRef[frame], GCinvert (0), x1, y1, newx, newy);
			   }
			 if (x3 != -1)
			   {
			      XDrawLine (GDp (0), FrRef[frame], GCinvert (0), lastx, lasty, x3, y3);
			      XDrawLine (GDp (0), FrRef[frame], GCinvert (0), newx, newy, x3, y3);
			   }
			 XFlush (GDp (0));
		      }
		    lastx = newx;
		    lasty = newy;
		    if (wrap)
		      {
			 XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, lastx, lasty);
			 wrap = False;
		      }
		    break;

		 case Expose:
		    f = GetFenetre (event.xexpose.window);
		    if (f <= MAX_FRAME + 1)
		       TraiteExpose (event.xexpose.window, f, (XExposeEvent *) & event);
		    XtDispatchEvent (&event);
		    break;

/*-OTHER-*/
		 default:
		    break;
	      }			/*switch */
     }

   /* efface la boite contour */
   GeomBoite (frame, x, y, large, haut, x + large - 2, y + haut - 2, True);

   /* On restaure l'etat anterieur */
   ThotUngrab ();
   XFlush (GDp (0));
#endif /* NEW_WILLOWS */
}				/*PolyModification */


/* ---------------------------------------------------------------------- */
/* |    PolyLineExtension ajoute des points apres un point donne' d'une | */
/* |            polyline affiche'e dans la fenetree^tre frame.          | */
/* |            Les valeurs x, y donnent la position de la boi^te dans  | */
/* |            la fenetree^tre. Le parame`tre Pbuffer pointe sur le    | */
/* |            premier buffer du pave', donc la liste des points de    | */
/* |            contro^le de la polyline d'origine. Le parame`tre       | */
/* |            Bbuffer pointe sur le premier buffer de la boi^te, donc | */
/* |            la liste des points de contro^le affiche's.             | */
/* |            Le premier point de chaque liste donne la largeur et la | */
/* |            hauteur limite de la polyline.                          | */
/* |            nbpoints indique le nombre de points de contro^le, y    | */
/* |            compris le point limite, point indique le point de      | */
/* |            re'fe'rence. close est vrai si la polyline est ferme'e. | */
/* |            La procedure met a` jour les deux listes de points      | */
/* |            de contro^le.                                           | */
/* |            Retourne le nombre de points de la polyline.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 PolyLineExtension (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close)

#else  /* __STDC__ */
int                 PolyLineExtension (frame, x, y, Pbuffer, Bbuffer, nbpoints, point, close)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer      Pbuffer;
PtrTextBuffer      Bbuffer;
int                 nbpoints;
int                 point;
boolean             close;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   float               rapportX, rapportY;
   int                 large, haut;
   int                 e, dx, dy;
   int                 ret, f;
   int                 newx, newy, lastx, lasty;
   int                 x1, y1, x3, y3;
   ThotWindow          w, wdum;
   XEvent              event;
   boolean             wrap;

   /* boolean ok; */

   /* les dimensions de la boite */
   large = Bbuffer->BuPoints[0].XCoord;
   haut = Bbuffer->BuPoints[0].YCoord;
   /* calcule les rapports de deformation entre la boite et le pave */
   rapportX = (float) Pbuffer->BuPoints[0].XCoord / (float) large;
   rapportY = (float) Pbuffer->BuPoints[0].YCoord / (float) haut;
   large = PtEnPixel (large / 1000, 0);
   haut = PtEnPixel (haut / 1000, 1);

   /*Changement de curseur */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
   wrap = False;
   XMapRaised (GDp (0), w);
   XFlush (GDp (0));
   ThotGrab (w, CursDepHV (0), e, 0);

   /* affiche la boite contour */
   /*Clear(frame, large, haut, x, y); */
   GeomBoite (frame, x, y, large, haut, x + large - 2, y + haut - 2, True);
   RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close,
		   &x1, &y1, &lastx, &lasty, &x3, &y3);
   /* Le pas de la grille commence sur l'origine de l'englobante */
   XFlush (GDp (0));
   XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, lastx, lasty);
   x1 = lastx;
   y1 = lasty;
   /*ok = False; *//* pas encore de point saisi */

   /* BOUCLE d'attente de definition d'un point de controle */
   ret = 0;
   while (ret == 0)
     {
	if (XPending (GDp (0)) == 0)
	  {

	     /* position actuelle du curseur */
	     XQueryPointer (GDp (0), w, &wdum, &wdum, &dx, &dy, &newx, &newy, &e);
	     /* Verification des coordonnees */
	     newx = ALIGNE (newx - FrameTable[frame].FrLeftMargin - x);
	     newx += x;
	     newy = ALIGNE (newy - FrameTable[frame].FrTopMargin - y);
	     newy += y;
	     if (newx < x || newx > x + large || newy < y || newy > y + haut)
	       {
		  /* Il faut revenir dans les bornes de la boite */
		  if (newx < x)
		     newx = x + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
		  else if (newx > x + large)
		     newx = x + large + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
		  else
		     newx += FrameTable[frame].FrLeftMargin;

		  if (newy < y)
		     newy = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
		  else if (newy > y + haut)
		     newy = y + haut + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
		  else
		     newy += FrameTable[frame].FrTopMargin;
		  XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, newx, newy);
	       }
	     else
	       {
		  newx += FrameTable[frame].FrLeftMargin;
		  newy += FrameTable[frame].FrTopMargin;
	       }

	     /* Affiche les segments en construction */
	     if (newx != lastx || newy != lasty)
	       {
		  if (x1 != -1)
		    {
		       XDrawLine (GDp (0), FrRef[frame], GCinvert (0), x1, y1, lastx, lasty);
		       XDrawLine (GDp (0), FrRef[frame], GCinvert (0), x1, y1, newx, newy);
		    }
		  if (x3 != -1)
		    {
		       XDrawLine (GDp (0), FrRef[frame], GCinvert (0), lastx, lasty, x3, y3);
		       XDrawLine (GDp (0), FrRef[frame], GCinvert (0), newx, newy, x3, y3);
		    }

		  XFlush (GDp (0));
		  lastx = newx;
		  lasty = newy;
	       }
	  }
	else
	  {
	     XNextEvent (GDp (0), &event);

	     /* Verification des coordonnees */
	     newx = x + ALIGNE ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - x);
	     newy = y + ALIGNE ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
	     /* Il faut revenir dans les bornes de la boite */
	     if (newx < x)
	       {
		  newx = x + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
		  wrap = True;
	       }
	     else if (newx > x + large)
	       {
		  newx = x + large + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
		  wrap = True;
	       }
	     else
		newx += FrameTable[frame].FrLeftMargin;

	     if (newy < y)
	       {
		  newy = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
		  wrap = True;
	       }
	     else if (newy > y + haut)
	       {
		  newy = y + haut + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
		  wrap = True;
	       }
	     else
		newy += FrameTable[frame].FrTopMargin;

	     switch (event.type)
		   {
/*-BOUTON ENFONCE-*/
		      case ButtonPress:
			 lastx = newx;
			 lasty = newy;
			 if (wrap)
			   {
			      XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, lastx, lasty);
			      wrap = False;
			   }
			 break;

/*-BOUTON RELACHE-*/
		      case ButtonRelease:
			 if (event.xbutton.button != Button3)
			   {
			      /* Bouton gauche -> garde le nouveau segment construit */
			      /* Note le point de depart du nouveau segment */
			      lastx = newx;
			      lasty = newy;
			      x1 = lastx;
			      y1 = lasty;
			      nbpoints++;
			      point++;
			      /*ok = True; */
			      /* Met a jour les buffers de la boite */
			      newx = PixelEnPt (lastx - FrameTable[frame].FrLeftMargin - x, 0) * 1000;
			      newy = PixelEnPt (lasty - FrameTable[frame].FrTopMargin - y, 1) * 1000;
			      AddPointInPolyline (Bbuffer, point, newx, newy);
			      /* Met a jour les buffers du pave */
			      newx = (int) ((float) newx * rapportX);
			      newy = (int) ((float) newy * rapportY);
			      AddPointInPolyline (Pbuffer, point, newx, newy);

			      if (event.xbutton.button != Button1)
				 /* Un autre bouton -> fin de la construction */
				 ret = 1;
			   }
			 break;

		      case Expose:
			 f = GetFenetre (event.xexpose.window);
			 if (f <= MAX_FRAME + 1)
			    TraiteExpose (event.xexpose.window, f, (XExposeEvent *) & event);
			 XtDispatchEvent (&event);
			 break;

/*-OTHER-*/
		      default:
			 break;
		   }		/*switch */
	  }
     }

   /* efface la boite contour */
   GeomBoite (frame, x, y, large, haut, x + large - 2, y + haut - 2, True);

   /* On restaure l'etat anterieur */
   ThotUngrab ();
   XFlush (GDp (0));
   /* On impose au moins deux points de controle pour un polyline valide */
   if (nbpoints < 3)
      return 1;
   else
      return nbpoints;
#endif /* NEW_WILLOWS */
}				/*PolyLineExtension */
/*fin */

/* ---------------------------------------------------------------------- */
/* |    GeomCreation affiche une boite fantome a la position (x,y de la | */
/* |            frame frame) et de dimensions (large,haut) quand        | */
/* |            l'utilisateur enfonce le bouton.                        | */
/* |            Les valeurs xmin, xmax, ymin, ymax donnent les limites  | */
/* |            du deplacement. La procedure retourne les position et   | */
/* |            dimension de la boite quand le bouton est relache.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GeomCreation (int frame, int *x, int *y, int xr, int yr, int *large, int *haut, int xmin, int xmax, int ymin, int ymax, int PosX, int PosY, int DimX, int DimY)

#else  /* __STDC__ */
void                GeomCreation (frame, x, y, xr, yr, large, haut, xmin, xmax, ymin, ymax, PosX, PosY, DimX, DimY)
int                 frame;
int                *x;
int                *y;
int                 xr;
int                 yr;
int                *large;
int                *haut;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 PosX;
int                 PosY;
int                 DimX;
int                 DimY;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   int                 xm, ym;
   int                 ret, e, dx, dy;
   int                 nx, ny, f;
   XEvent              event;
   ThotWindow          w, wdum;
   int                 Adroite, Enbas;
   int                 warpx, warpy;

   /* Note le decalage du point fixe par rapport a l'origine */
   xr -= *x;
   yr -= *y;

   /* Met a jour l'etat de l'ecran */
   /*Changement de curseur */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
   ThotGrab (w, CursDepHV (0), e, 0);

   /* Position par defaut */
   if (*x < xmin)
      *x = xmin;
   else if (*x > xmax)
      *x = xmax;
   if (*y < ymin)
      *y = ymin;
   else if (*y > ymax)
      *y = ymin;

   /* Le pas de la grille commence sur l'origine de l'englobante */
   dx = ALIGNE (*x - xmin);
   *x = xmin + dx;
   dy = ALIGNE (*y - ymin);
   *y = ymin + dy;
   XMapRaised (GDp (0), w);
   XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, *x + FrameTable[frame].FrLeftMargin, *y + FrameTable[frame].FrTopMargin);
   XFlush (GDp (0));

   /* Affiche la boite elastique initiale */
   GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);

   /* BOUCLE d'attente de definition du premier point */
   ret = 0;
   while (ret == 0)
     {
	if (XPending (GDp (0)) == 0)
	  {
	     /* Suivi du curseur */
	     XQueryPointer (GDp (0), w, &wdum, &wdum, &dx, &dy, &nx, &ny, &e);
	     /* Verification des coordonnees */
	     nx = ALIGNE (nx - FrameTable[frame].FrLeftMargin - xmin);
	     nx += xmin;
	     ny = ALIGNE (ny - FrameTable[frame].FrTopMargin - ymin);
	     ny += ymin;
	     if (nx < xmin || nx > xmax || ny < ymin || ny > ymax)
		XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, *x + FrameTable[frame].FrLeftMargin, *y + FrameTable[frame].FrTopMargin);
	     else if ((nx != *x && PosX) || (ny != *y && PosY))
	       {
		  GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);	/*Ancienne */
		  if (PosX)
		     *x = nx;
		  if (PosY)
		     *y = ny;
		  GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);	/*Nouvelle */
		  XFlush (GDp (0));
		  /* Si une position est bloquee */
		  if (!PosX || !PosY)
		     XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, *x + FrameTable[frame].FrLeftMargin,
				   *y + FrameTable[frame].FrTopMargin);
	       }
	  }
	else
	  {
	     XNextEvent (GDp (0), &event);

	     /* Le seul evenement traite est le bouton enfonce */
	     switch (event.type)
		   {
/*-BOUTON ENFONCE-*/
		      case ButtonPress:
			 if (PosX)
			   {
			      xm = xmin + ALIGNE ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - xmin);
			   }
			 else
			    xm = *x;
			 if (PosY)
			   {
			      ym = ymin + ALIGNE ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - ymin);
			   }
			 else
			    ym = *y;

			 /* Verification des coordonnees */
			 if (xm < xmin || xm > xmax || !PosX
			     || ym < ymin || ym > ymax || !PosY)
			   {
			      XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, xm + FrameTable[frame].FrLeftMargin,
					ym + FrameTable[frame].FrTopMargin);
			   }
			 else
			    ret = 1;
			 break;

		      case Expose:
			 f = GetFenetre (event.xexpose.window);
			 if (f <= MAX_FRAME + 1)
			    TraiteExpose (event.xexpose.window, f, (XExposeEvent *) & event);
			 XtDispatchEvent (&event);
			 break;

/*-OTHER-*/
		      default:
			 break;
		   }
	  }
     }

   /* Nouvelle reference pour l'affichage de la boite */
   GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);	/*Ancienne */
   *x = xm;
   *y = ym;
   xr = 2;
   yr = 2;
   xm += FrameTable[frame].FrLeftMargin;
   ym += FrameTable[frame].FrTopMargin;
   GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);
   /* On indique que l'on part de la valeur initiale */
   Adroite = 2;
   Enbas = 2;

   /* BOUCLE d'attente sur le deuxieme point de definition */
   ret = 0;
   while (ret == 0)
     {
	XNextEvent (GDp (0), &event);
	/* On analyse le type de l'evenement */
	switch (event.type)
	      {
/*-BOUTON RELACHE-*/
		 case ButtonRelease:
		    ret = 1;

/*-DEPLACEMENT DE LA SOURIS-*/
		 case MotionNotify:
		    warpx = -1;
		    warpy = -1;

		    /* Si l'evenement concerne la fenetreentre */
		    if (event.xmotion.window == w)
		      {
			 /* Nouvelles positions du curseur */
			 if (DimX)
			   {
			      dx = (int) event.xmotion.x - xm;
			      nx = xmin + ALIGNE (*x + *large + dx - xmin);
			      dx = nx - *x - *large;
			   }
			 else
			    dx = 0;

			 if (DimY)
			   {
			      dy = (int) event.xmotion.y - ym;
			      ny = ymin + ALIGNE (*y + *haut + dy - ymin);
			      dy = ny - *y - *haut;
			   }
			 else
			    dy = 0;

			 xm += dx;
			 ym += dy;
			 /* On remplace la valeur par defaut par la valeur */
			 /* reelle si la hauteur ou la largeur a saisir    */
			 /* ne sont pas nulles                             */
			 if ((dx != 0 || !DimX) && (dy != 0 || !DimY)
			     && Adroite == 2)
			   {
			      Adroite = 1;
			      Enbas = 1;
			      GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);
			      if (DimX)
				 *large = 0;
			      if (DimY)
				 *haut = 0;
			      GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);
			   }
		      }
		    else
		      {
			 dx = 0;
			 dy = 0;
		      }

		    /* Quel est le cote deplace en X ? */
		    if (dx != 0)
		       if (Adroite > 0)		/* le cote droit */
			 {
			    /* Faut-il inverser les cotes ? */
			    if (dx < 0 && -dx > *large)
			      {
				 Adroite = 0;	/* Inversion */
				 nx = *x + *large + dx;		/*nouvelle origine */
				 /* Verifie que l'on reste dans les limites */
				 if (nx < xmin)
				   {
				      nx = xmin;
				      warpx = xmin;
				   }
				 dx = *x - nx;	/*nouvelle largeur */
			      }
			    else
			      {
				 nx = *x;	/*nouvelle origine */
				 dx += *large;	/*nouvelle largeur */
				 /* Verifie que l'on reste dans les limites */
				 if (nx + dx > xmax)
				   {
				      dx = xmin + ALIGNE (xmax - xmin) - nx;
				      warpx = xmax;
				   }
			      }
			 }
		       else
			  /* le cote gauche */
			 {
			    /* Faut-il inverser les cotes ? */
			    if (dx > *large)
			      {
				 Adroite = 1;	/* Inversion */
				 nx = *x + *large;	/*nouvelle origine */
				 dx -= *large;	/*nouvelle largeur */
				 /* Verifie que l'on reste dans les limites */
				 if (nx + dx > xmax)
				   {
				      dx = xmin + ALIGNE (xmax - xmin) - nx;
				      warpx = xmax;
				   }
			      }
			    else
			      {
				 nx = *x + dx;	/*nouvelle origine */
				 /* Verifie que l'on reste dans les limites */
				 if (nx < xmin)
				   {
				      nx = xmin;
				      warpx = xmin;
				   }
				 dx = *x + *large - nx;		/*nouvelle largeur */
			      }
			 }
		    else
		      {
			 nx = *x;
			 dx = *large;
		      }

		    /* Quel est le cote deplace en Y ? */
		    if (dy != 0)
		       if (Enbas > 0)	/* le cote inferieur */
			 {
			    /* Faut-il inverser les cotes ? */
			    if (dy < 0 && -dy > *haut)
			      {
				 Enbas = 0;	/* Inversion */
				 ny = *y + *haut + dy;	/*nouvelle origine */
				 /* Verifie que l'on reste dans les limites */
				 if (ny < ymin)
				   {
				      ny = ymin;
				      warpy = ymin;
				   }
				 dy = *y - ny;	/*nouvelle hauteur */
			      }
			    else
			      {
				 ny = *y;	/*nouvelle origine */
				 dy += *haut;	/*nouvelle hauteur */
				 /* Verifie que l'on reste dans les limites */
				 if (ny + dy > ymax)
				   {
				      dy = ymin + ALIGNE (ymax - ymin) - ny;
				      warpy = ymax;
				   }
			      }
			 }
		       else
			  /* le cote superieur */
			 {
			    /* Faut-il inverser les cotes ? */
			    if (dy > *haut)
			      {
				 Enbas = 1;	/* Inversion */
				 ny = *y + *haut;	/*nouvelle origine */
				 dy -= *haut;	/*nouvelle hauteur */
				 /* Verifie que l'on reste dans les limites */
				 if (ny + dy > ymax)
				   {
				      dy = ymin + ALIGNE (ymax - ymin) - ny;
				      warpy = ymax;
				   }
			      }
			    else
			      {
				 ny = *y + dy;	/*nouvelle origine */
				 /* Verifie que l'on reste dans les limites */
				 if (ny < ymin)
				   {
				      ny = ymin;
				      warpy = ymin;
				   }
				 dy = *y + *haut - ny;	/*nouvelle hauteur */
			      }
			 }
		    else
		      {
			 ny = *y;
			 dy = *haut;
		      }

		    /* Faut-il deplacer la boite elastique */
		    if (nx != *x || ny != *y || dx != *large || dy != *haut)
		      {
			 GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);	/*Ancienne */
			 GeomBoite (frame, nx, ny, dx, dy, nx + xr, ny + yr, False);	/*Nouvelle */
			 XFlush (GDp (0));
			 *x = nx;
			 *y = ny;
			 *large = dx;
			 *haut = dy;
		      }

		    /* Faut-il deplacer le curseur */
		    if (warpx >= 0 || warpy >= 0)
		      {
			 if (warpx >= 0)
			    xm = warpx + FrameTable[frame].FrLeftMargin;
			 if (warpy >= 0)
			    ym = warpy + FrameTable[frame].FrTopMargin;
			 XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, xm, ym);
		      }
		    break;

/*-OTHER-*/
		 default:
		    break;
	      }
     }

   /* On supprime la boite elastique */
   GeomBoite (frame, *x, *y, *large, *haut, *x + xr, *y + yr, False);

   /* On restaure l'etat anterieur */
   ThotUngrab ();
   XFlush (GDp (0));
#endif /* NEW_WILLOWS */
}				/*GeomCreation */

/* ---------------------------------------------------------------------- */
/* |    ChPosition affiche une boite fantome a` la position (x,y de la  | */
/* |            frame frame) et de dimensions (large,haut) et la deplace| */
/* |            a` la demande de l'utilisateur.                         | */
/* |            Les parametres xr,yr donnent la position du point de    | */
/* |            reference. Les valeurs xmin, xmax, ymin, ymax donnent   | */
/* |            les limites du deplacement. Les parametres xm,ym donnent| */
/* |            la position initiale de la souris dans la fenetre.      | */
/* |            La procedure retourne la nouvelle position quand le     | */
/* |            bouton est relache'.                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChPosition (int frame, int *x, int *y, int large, int haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)

#else  /* __STDC__ */
void                ChPosition (frame, x, y, large, haut, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
int                 frame;
int                *x;
int                *y;
int                 large;
int                 haut;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   int                 ret, e, dx, dy, nx, ny;
   XEvent              event;
   ThotWindow          w;
   int                 warpx, warpy;

   /* On repositionne les coordonnees de la souris dans la fenetre */
   xm += FrameTable[frame].FrLeftMargin;
   ym += FrameTable[frame].FrTopMargin;

   /* On decale le point de reference pour l'affichage */
   if (xr == *x)
      xr += 2;
   else if (xr == *x + large)
      xr -= 2;

   if (yr == *y)
      yr += 2;
   else if (yr == *y + haut)
      yr -= 2;

   /* On choisit le bon curseur */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
   if ((xmin >= *x) && (xmax <= *x + large))
      ThotGrab (w, CursDepV (0), e, 0);
   else
     {
	if ((ymin >= *y) && (ymax <= *y + haut))
	   ThotGrab (w, CursDepH (0), e, 0);
	else
	   ThotGrab (w, CursDepHV (0), e, 0);
     }

   /* On affiche la boite elastique initiale */
   GeomBoite (frame, *x, *y, large, haut, xr, yr, False);

   /* BOUCLE de traitement des evenements */
   ret = 0;
   while (ret == 0)
     {
	XNextEvent (GDp (0), &event);
	switch (event.type)	/* On analyse le type de l'evenement */
	      {
/*-BOUTON ENFONCE-*/
		 case ButtonRelease:
		    ret = 1;
/*-DEPLACEMENT DE LA SOURIS-*/
		 case MotionNotify:

		    /* Si l'evenement concerne la fenetre */
		    if (event.xmotion.window == w)
		      {
			 /* On calcule la nouvelle origine de la boite */
			 nx = *x + (int) event.xmotion.x - xm;
			 dx = xmin + ALIGNE (nx - xmin) - *x;
			 ny = *y + (int) event.xmotion.y - ym;
			 dy = ymin + ALIGNE (ny - ymin) - *y;
		      }
		    else
		      {
			 dx = dy = 0;
		      }

		    nx = dx + *x;
		    ny = dy + *y;

		    /* On verifie que l'on reste dans la limite permise */
		    warpx = -1;
		    warpy = -1;
		    if (xmin == xmax)
		      {
			 nx = xmin;	/*cote gauche */
			 warpx = xm;
		      }
		    else if (nx < xmin)
		      {
			 nx = xmin;	/*cote gauche */
			 warpx = xm;
		      }
		    else if (nx + large > xmax)
		      {
			 if (xmin + large > xmax)
			   {
			      nx = xmin;	/*debordement => sur le cote gauche */
			      warpx = xm;
			   }
			 else
			   {
			      nx = xmin + ALIGNE (xmax - large - xmin);		/*cote droit */
			      warpx = xm + nx - *x;
			   }
		      }
		    else
		       xm += dx;	/*nouvelle position du curseur */

		    dx = nx - *x;
		    if (ymin == ymax)
		      {
			 ny = ymin;	/*cote superieur */
			 warpy = ym;
		      }
		    else if (ny < ymin)
		      {
			 ny = ymin;	/*cote superieur */
			 warpy = ym;
		      }
		    else if (ny + haut > ymax)
		      {
			 if (ymin + haut > ymax)
			   {
			      ny = ymin;	/*debordement => sur le cote superieur */
			      warpy = ym;
			   }
			 else
			   {
			      ny = ymin + ALIGNE (ymax - haut - ymin);	/*cote inferieur */
			      warpy = ym + ny - *y;
			   }
		      }
		    else
		       ym += dy;	/*nouvelle position du curseur */
		    dy = ny - *y;

		    /* Faut-il deplacer la boite elastique */
		    if ((dx != 0) || (dy != 0))
		      {
			 GeomBoite (frame, *x, *y, large, haut, xr, yr, False);		/*Ancienne */
			 xr += dx;
			 yr += dy;
			 GeomBoite (frame, nx, ny, large, haut, xr, yr, False);		/*Nouvelle */
			 XFlush (GDp (0));
			 *x = nx;
			 *y = ny;

		      }

		    /* Faut-il deplacer le curseur */
		    if (warpx >= 0 || warpy >= 0)
		      {
			 if (warpx >= 0)
			    xm = warpx;
			 if (warpy >= 0)
			    ym = warpy;
			 XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, xm, ym);
		      }
		    break;
/*-OTHER-*/
		 default:
		    break;
	      }
     }

   /* On supprime la boite elastique */
   GeomBoite (frame, *x, *y, large, haut, xr, yr, False);

   /* On retaure l'etat anterieur */
   ThotUngrab ();
   XFlush (GDp (0));

#endif /* NEW_WILLOWS */
}				/*ChPosition */

/* ---------------------------------------------------------------------- */
/* |    ChDimension affiche une boite fantome a` la position (x,y de la | */
/* |            frame frame) et de dimensions (large,haut) et la        | */
/* |            deforme a` la demande de l'utilisateur.                 | */
/* |            Les parametres xr,yr donnent la position du point de    | */
/* |            reference. Les valeurs xmin, xmax, ymin, ymax donnent   | */
/* |            les limites de deplacement. Les parametres xm,ym donnent| */
/* |            la position initiale de la souris dans la fenetre.      | */
/* |            La procedure retourne les nouvelles dimensions quand le | */
/* |            bouton est relache'.                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChDimension (int frame, int x, int y, int *large, int *haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)

#else  /* __STDC__ */
void                ChDimension (frame, x, y, large, haut, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
int                 frame;
int                 x;
int                 y;
int                *large;
int                *haut;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
#define c_sup 0
#define c_milieuh 1
#define c_inf 2
#define c_gauche 0
#define c_milieuv 1
#define c_droit 2

   int                 ret, e, dx, dy, dl, dh;
   int                 ref_h, ref_v, sensh, sensv;
   XEvent              event;
   ThotWindow          w;
   int                 warpx, warpy;

   /* On utilise le point de reference pour deplacer la boite */
   if (xr == x)
     {
	ref_v = c_gauche;
	xr += 2;		/* decalage pour l'affichage */
	/* La boite s'elargit quand le delta x de la souris augmente */
	sensh = 1;
     }
   else if (xr == x + *large)
     {
	ref_v = c_droit;
	xr -= 2;		/* decalage pour l'affichage */
	/* La boite se retrecit quand le delta x de la souris augmente */
	sensh = -1;
     }
   else
     {
	ref_v = c_milieuv;
	/* Le sens depend de la position initiale du curseur dans la boite */
	if (xm < xr)
	   sensh = -1;
	else
	   sensh = 1;
     }

   if (yr == y)
     {
	ref_h = c_sup;
	yr += 2;		/* decalage pour l'affichage */
	/* La boite grandit quand le delta y de la souris augmente */
	sensv = 1;
     }
   else if (yr == y + *haut)
     {
	ref_h = c_inf;
	yr -= 2;		/* decalage pour l'affichage */
	/* La boite retrecit quand le delta y de la souris augmente */
	sensv = -1;
     }
   else
     {
	ref_h = c_milieuh;
	/* Le sens depend de la position initiale du curseur dans la boite */
	if (ym < yr)
	   sensv = -1;
	else
	   sensv = 1;
     }

   /* On repositionne les coordonnees de la souris dans la fenetre */
   xm += FrameTable[frame].FrLeftMargin;
   ym += FrameTable[frame].FrTopMargin;

   /* On affiche la boite elastique initiale */
   GeomBoite (frame, x, y, *large, *haut, xr, yr, False);
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;

   /* On choisit le bon curseur */
   w = FrRef[frame];
   if (xmin == xmax)
      ThotGrab (w, CursDepV (0), e, 0);
   else if (ymin == ymax)
      ThotGrab (w, CursDepH (0), e, 0);
   else
      ThotGrab (w, CursDepHV (0), e, 0);

   /* BOUCLE de traitement des evenements */
   ret = 0;
   while (ret == 0)
     {
	/* Pour une raison mysterieuse si on consomme trop rapidement */
	/* les evenements de la file, de temps en temps X11R4         */
	/* retourne un evenement avec une valeur de event.xbutton.y=1 */
	/* Solution: consommer rapidement uniquement les MotionNotify */
	XNextEvent (GDp (0), &event);
	if (event.type == MotionNotify)

	   /* On pique le dernier evenement de la file */
	   while (XPending (GDp (0)))
	      XNextEvent (GDp (0), &event);
	switch (event.type)	/* On analyse le type de l'evenement */
	      {
/*-BOUTON ENFONCE-*/
		 case ButtonRelease:
		    ret = 1;
/*-DEPLACEMENT DE LA SOURIS-*/
		 case MotionNotify:
		    if (event.xbutton.window == w)
		      {
			 /* On calcule les ecarts de dimension de la boite */
			 dl = (int) event.xmotion.x - xm;
			 dh = (int) event.xmotion.y - ym;
		      }
		    else
		      {
			 dl = dh = 0;
		      }

		    /* On verifie que les dimensions sont modifiables */
		    /* et qu'elles restent toujours positives         */
		    /* La modification relle de largeur (hauteur) depend */
		    /* de la position du curseur et du point fixe de la  */
		    /* boite :                                           */
		    /* Fixee a gauche et dl>0 -> augmentation largeur    */
		    /* Fixee a gauche et dl<0 -> diminution largeur      */
		    /* Fixee a droite et dl>0 -> diminution largeur      */
		    /* Fixee a droite et dl<0 -> augmentation largeur    */
		    /* Centree, curseur a gauche et dl>0 -> diminution   */
		    /* Centree, curseur a gauche et dl<0 -> augmentation */
		    /* Centree, curseur a droite et dl>0 -> augmentation */
		    /* Centree, curseur a droite et dl<0 -> diminution   */
		    warpx = -1;
		    warpy = -1;
		    if (dl != 0)
		       if (xmin == xmax)
			  /* Box bloquee en X */
			  dl = 0;
		       else if (ref_v == c_milieuv
				&& *large + (2 * dl * sensh) < 0)
			 {
			    dl = -ALIGNE (*large / 2) * sensh;
			    warpx = xm + (dl * sensh);
			 }
		       else if (*large + (dl * sensh) < 0)
			 {
			    dl = -ALIGNE (*large) * sensh;
			    warpx = xm + (dl * sensh);
			 }

		    if (dh != 0)
		       if (ymin == ymax)
			  /* Box bloquee en Y */
			  dh = 0;
		       else if (ref_h == c_milieuh
				&& *haut + (2 * dh * sensv) < 0)
			 {
			    dh = -(*haut * sensv * PasGrille) / (2 * PasGrille);
			    warpy = ym + (dh * sensv);
			 }
		       else if (*haut + dh < 0)
			 {
			    dh = -(*haut * sensv * PasGrille) / PasGrille;
			    warpy = ym + (dh * sensv);
			 }

		    /* On evalue le deplacement horizontal de l'origine de la boite */
		    if (dl != 0)
		      {
			 dl = dl * sensh;	/* On tient compte du sens */
			 if (ref_v == c_milieuv)
			   {
			      dx = xmin + ALIGNE (x - (dl / 2) - xmin) - x;
			      /* On verifie la validite du deplacement */
			      if (x + dx < xmin)
				 dx = xmin - x;		/*cote gauche */
			      if (x + *large - dx > xmax)
				 dx = x + *large - xmin - ALIGNE (xmax - xmin);		/*cote droit */

			      /* modification effective de la largeur */
			      dl = -(dx * 2);
			      if (dx != 0)
				 warpx = xm - (dx * sensh);
			   }
			 else if (ref_v == c_droit)
			   {
			      dx = xmin + ALIGNE (x - dl - xmin) - x;
			      /* On verifie la validite du deplacement */
			      if (x + dx < xmin)
				 dx = xmin - x;		/*cote gauche */

			      /* modification effective de la largeur */
			      dl = -dx;
			      if (dx != 0)
				 warpx = xm + dx;
			   }
			 else
			   {
			      dx = 0;
			      dl = xmin + ALIGNE (x + *large + dl - xmin) - x - *large;
			      if (x + *large + dl > xmax)
				 dl = xmin + ALIGNE (xmax - xmin) - x - *large;		/*cote droit */
			      if (dl != 0)
				 warpx = xm + dl;
			   }
		      }
		    else
		       dx = 0;

		    /* On evalue le deplacement vertical de l'origine de la boite */
		    if (dh != 0)
		      {
			 dh = dh * sensv;	/* On tient compte du sens */
			 if (ref_h == c_milieuh)
			   {
			      dy = ymin + ALIGNE (y - (dh / 2) - ymin) - y;
			      /* On verifie la validite du deplacement */
			      if (y + dy < ymin)
				 dy = ymin - y;		/*cote superieur */
			      if (y + *haut - dy > ymax)
				 dy = y + *haut - ymin - ALIGNE (ymax - ymin);	/*cote inferieur */
			      /* modification effective de la hauteur */
			      dh = -(dy * 2);
			      if (dy != 0)
				 warpy = ym - (dy * sensv);
			   }
			 else if (ref_h == c_inf)
			   {
			      dy = ymin + ALIGNE (y - dh - ymin) - y;
			      /* On verifie la validite du deplacement */
			      if (y + dy < ymin)
				 dy = ymin - y;		/*cote superieur */
			      /* modification effective de la hauteur */
			      dh = -dy;
			      if (dy != 0)
				 warpy = ym + dy;
			   }
			 else
			   {
			      dy = 0;
			      dh = ymin + ALIGNE (y + *haut + dh - ymin) - y - *haut;
			      if (y + *haut + dh > ymax)
				 dh = ymin + ALIGNE (ymax - ymin) - y - *haut;	/*cote inf */
			      if (dh != 0)
				 warpy = ym + dh;
			   }
		      }
		    else
		       dy = 0;

		    /* Faut-il deplacer la boite elastique */
		    if ((dl != 0) || (dh != 0))
		      {
			 GeomBoite (frame, x, y, *large, *haut, xr, yr, False);		/*Ancienne */
			 *large += dl;
			 *haut += dh;
			 x += dx;
			 y += dy;
			 GeomBoite (frame, x, y, *large, *haut, xr, yr, False);		/*Nouvelle */
		      }

		    /* Faut-il deplacer le curseur */
		    if (warpx >= 0 || warpy >= 0)
		      {
			 if (warpx >= 0)
			    xm = warpx;
			 if (warpy >= 0)
			    ym = warpy;
			 XWarpPointer (GDp (0), None, w, 0, 0, 0, 0, xm, ym);
		      }
		    break;
/*-OTHER-*/
		 default:
		    break;
	      }
     }

   /* On supprime la boite elastique */
   GeomBoite (frame, x, y, *large, *haut, xr, yr, False);

   /* On restaure l'etat anterieur */
   ThotUngrab ();
   XFlush (GDp (0));
#endif /* NEW_WILLOWS */

}				/*ChDimension */
