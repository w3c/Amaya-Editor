
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* >>>>>>>>>> preparation du code de trace des fleches */

/* ---------------------------------------------------------------------- */
/* |    AfFleche trace une fle^che oriente'e en fonction de l'angle     | */
/* |            donne' :0 (fle^che vers la droite), 45, 90, 135, 180,   | */
/* |            225, 270 ou 315.                                        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                AfFleche (int frame, int epais, int style, int x, int y, int l, int h, int orientation, int RO, int active, int fg)

#else  /* __STDC__ */
void                AfFleche (frame, epais, style, x, y, l, h, orientation, RO, active, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 orientation;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, ym, xf, yf;
   double              dx, dy;
   double              arrowlength;
   ThotPoint           points[4];
   Pixmap              modele;

#define HEAD 8

   xm = x + ((l - epais) / 2);
   xf = x + l - 1;
   ym = y + ((h - epais) / 2);
   yf = y + h - 1;
/**CO*/ preparerTrace (0, style, epais, RO, active, fg);
   /*angle = atan2(dx, dy); */
   /*base = HEAD - arrowlength; */

   if (orientation == 0)
     {
	/* fleche vers la droite */
	dx = (double) (-HEAD);
	dy = (double) (0);
	Tracer (frame, x, ym, xf, ym);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = xf;
	points[0].y = ym;
	arrowlength = sqrt ((dx * dx) + (dy * dy));
	points[1].x = points[0].x - arrowlength;
	points[1].y = points[0].y - HEAD / 2;
	points[2].x = points[1].x;
	points[2].y = points[0].y + HEAD / 2;
     }
   else if (orientation == 45)
     {
	Tracer (frame, x, yf, xf - epais / 2, y);
	dx = (double) (-HEAD);
	dy = (double) (HEAD);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = xf;
	points[0].y = y;
	arrowlength = sqrt ((dx * dx) + (dy * dy));
	points[1].x = points[0].x - arrowlength;
	points[1].y = points[0].y - HEAD / 2;
	points[2].x = points[1].x;
	points[2].y = points[0].y + HEAD / 2;
     }
   else if (orientation == 90)
     {
	/* fleche vers le haut */
	Tracer (frame, xm, y, xm, yf);
	dx = (double) (0);
	dy = (double) (HEAD);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = xm;
	points[0].y = y;
     }
   else if (orientation == 135)
     {
	Tracer (frame, x, y, xf - epais + 1, yf);
	dx = (double) (HEAD);
	dy = (double) (HEAD);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = x;
	points[0].y = y;
     }
   else if (orientation == 180)
     {
	/* Trace une fleche vers la gauche */
	Tracer (frame, x, ym, xf, ym);
	dx = (double) (HEAD);
	dy = (double) (0);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = x;
	points[0].y = ym;
     }
   else if (orientation == 225)
     {
	dx = (double) (HEAD);
	dy = (double) (-HEAD);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = x;
	points[0].y = yf;
     }
   else if (orientation == 270)
     {
	/* Trace une fleche vers le bas */
	Tracer (frame, xm, y, xm, yf);
	dx = (double) (0);
	dy = (double) (-HEAD);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = xm;
	points[0].y = yf;
     }
   else if (orientation == 315)
     {
	Tracer (frame, x, y, xf - epais / 2, yf);
	dx = (double) (-HEAD);
	dy = (double) (-HEAD);

	/* Calcule le contour de la pointe de la fleche */
	points[0].x = xf;
	points[0].y = yf;
     }
   points[3].x = points[0].x;
   points[3].y = points[0].y;
   /* On remplit le polygone */
   modele = CreatePattern (0, RO, active, fg, bg, 1);
   if (modele != NULL)
     {
	XSetTile (GDp (0), GCgrey (0), modele);
	XFillPolygon (GDp (0), FrRef[frame], GCgrey (0), points, 4, Complex, CoordModeOrigin);
	XFreePixmap (GDp (0), modele);
     }
   if (epais > 0)
     {
	XDrawLines (GDp (0), FrRef[frame], GCtrait (0), points, 4, CoordModeOrigin);
     }
/**CO*/ finirTrace (0, RO, active);
}
