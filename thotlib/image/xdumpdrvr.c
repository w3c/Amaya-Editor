
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* xdumpdrvr.c -- Implementation  X11 XDump acces */


#include "thot.h"
#include "message.h"
#include "thot_gui.h"
#include "thot_sys.h"
#include "imagedrvr.h"

#include "imagedrvr.f"
#include "frame.h"
#include "cconst.h"

#define VSTATUS extern
#include "imagedrvr.var"
#include "frame.var"

char               *TtaGetMemory ( /*unsigned int n */ );

#ifndef NULL
#define NULL 0
#endif

/* ---------------------------------------------------------------------- */
/* |    Ouverture du driver                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 XDumpOpenImageDrvr (ImagingModel model)

#else  /* __STDC__ */
int                 XDumpOpenImageDrvr (model)
ImagingModel        model;

#endif /* __STDC__ */

{
   int                 refNum;

   refNum = GetImageDrvrID (XDump);
   if (refNum != NULLIMAGEDRVR)
     {
	/* take care of xwd connection ? */
     }
   return refNum;

}				/*XDumpOpenImageDrvr */


/* ---------------------------------------------------------------------- */
/* |    Fermeture du driver                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                XDumpCloseImageDrvr ()

#else  /* __STDC__ */
void                XDumpCloseImageDrvr ()
#endif				/* __STDC__ */

{
   /* take care of xwd connection ? */
   /* say goodbye ... */

}				/*XDumpCloseImageDrvr */


/* ---------------------------------------------------------------------- */
/* |    Initialisation avant une image                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                XDumpInitImage ()

#else  /* __STDC__ */
void                XDumpInitImage ()
#endif				/* __STDC__ */

{
}				/*XDumpInitImage */


/* ---------------------------------------------------------------------- */
/* |    On relit une image                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
Drawable            XDumpReadImage (char *fn, ImagePres pres, int wif, int hif, int xcf, int ycf, int wcf, int hcf)

#else  /* __STDC__ */
Drawable            XDumpReadImage (fn, pres, wif, hif, xcf, ycf, wcf, hcf)
char               *fn;
ImagePres           pres;
int                 wif;
int                 hif;
int                 xcf;
int                 ycf;
int                 wcf;
int                 hcf;

#endif /* __STDC__ */

{
   Pixmap              myPixmap;
   int                 pmw, pmh;

   if (!ReadWD (fn, &myPixmap, &pmw, &pmh))
     {
	return None;
     }
   else
     {
	return myPixmap;
     }
}				/*XDumpReadImage */


/* ---------------------------------------------------------------------- */
/* |    On dessine une image                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                XDumpDrawImage (char *fn, ImagePres pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, Drawable drawable)

#else  /* __STDC__ */
void                XDumpDrawImage (fn, pres, xif, yif, wif, hif, xcf, ycf, wcf, hcf, drawable)
char               *fn;
ImagePres           pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 xcf;
int                 ycf;
int                 wcf;
int                 hcf;
Drawable            drawable;

#endif /* __STDC__ */

{
}				/*XDumpDrawImage */


/* ---------------------------------------------------------------------- */
/* |    On cree une image                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
Drawable            XDumpCreateImage (char *fn, ImagePres pres, int *xif, int *yif, int *wif, int *hif)

#else  /* __STDC__ */
Drawable            XDumpCreateImage (fn, pres, xif, yif, wif, hif)
char               *fn;
ImagePres           pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;

#endif /* __STDC__ */

{
   int                 status;
   Pixmap              myPixmap;
   int                 pmw, pmh;

   if (!(status = ReadWD (fn, &myPixmap, &pmw, &pmh)))
     {
	TtaDisplaySimpleMessage (XDUMP, INFO, XDUMP_BAD_FORMAT);
	return None;
     }
   else
     {
	*xif = 0;
	*yif = 0;
	*wif = pmw;
	*hif = pmh;
	return myPixmap;
     }
}				/*XDumpCreateImage */



/* ---------------------------------------------------------------------- */
/*
 *  XDump Edit Picture Stuff. spm. 90 08 29.
 *
 */

/* True si l'utilisateur est ok pour remplacer le fichier */

/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static Bool         dialogueReplace (char *fn)

#else  /* __STDC__ */
static Bool         dialogueReplace (fn)
char               *fn;

#endif /* __STDC__ */

{
   return True;			/* pour l'instant, il est tjrs ok... */
}

/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static Bool         FindXDumpEditor (char *xdumpEditor, char *xdumpEditorAgrs)

#else  /* __STDC__ */
static Bool         FindXDumpEditor (xdumpEditor, xdumpEditorAgrs)
char               *xdumpEditor;
char               *xdumpEditorAgrs;

#endif /* __STDC__ */

{
   char               *Grif_Bin;

#define GRIFDEFAULTXDUMPEDITOR "xwdgip"
#define GRIFDEFAULTXDUMPEDITORARGS "-it"

  /***todo: il y a une gestion de configuration utilisateur
            a faire pour le choix de l'editeur et la configuration des args.
  */

   Grif_Bin = (char *) TtaGetEnvString ("GRIFBIN");
   if (Grif_Bin == NULL)
     {
	strcpy (xdumpEditor, GRIFDEFAULTXDUMPEDITOR);
     }
   else
     {
	strcpy (xdumpEditor, Grif_Bin);
	strcat (xdumpEditor, "/");
	strcat (xdumpEditor, GRIFDEFAULTXDUMPEDITOR);
     }
   strcpy (xdumpEditorAgrs, GRIFDEFAULTXDUMPEDITORARGS);
}

/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         GetImageFrame (int fn, int xif, int yif, int wif, int hif)

#else  /* __STDC__ */
static void         GetImageFrame (fn, xif, yif, wif, hif)
int                 fn;
int                 xif;
int                 yif;
int                 wif;
int                 hif;

#endif /* __STDC__ */

{

   int                 status;
   int                 w, h;
   Pixmap              bitmap = None;

   status = ReadWD (fn, &bitmap, &w, &h);
/***todo: ca en fait un peu trop... */
   if (bitmap != None)
     {
	XFreePixmap (GDp (0), bitmap);
     }
   xif = 0;
   yif = 0;
   wif = w;
   hif = h;

}

/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         GetCroppingFrame (char *fn, int *xcf, int *ycf, int *wcf, int *hcf)

#else  /* __STDC__ */
static void         GetCroppingFrame (fn, xcf, ycf, wcf, hcf)
char               *fn;
int                *xcf;
int                *ycf;
int                *wcf;
int                *hcf;

#endif /* __STDC__ */

{

   /* 
    * pour un xdump editer avec xwd c'est l'image frame...
    * normalement le format XWDFile n'a pas de cropping frame.
    *
    */

   GetImageFrame (fn, xcf, ycf, wcf, hcf);
}


/* 
 * retourne un fn, fichier d'une image, son image frame
 * et sa cropping frame. En entree on peut fixer des hints.
 * new est True si fn est un nouveau fichier.
 *
 */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         DoEditImage (char *fn, Bool new, int *xif, int *yif, int *wif, int *hif, int *xcf, int *ycf, int *wcf, int *hcf)

#else  /* __STDC__ */
static void         DoEditImage (fn, new, xif, yif, wif, hif, xcf, ycf, wcf, hcf)
char               *fn;
Bool                new;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
int                *xcf;
int                *ycf;
int                *wcf;
int                *hcf;

#endif /* __STDC__ */

{
   char                cmd[255];	/* faire un malloc fonction de la taille de la ligne de commande ? */
   char                xdumpEditor[255];
   char                xdumpEditorAgrs[255];

   if (FindXDumpEditor (xdumpEditor, xdumpEditorAgrs))
     {
	sprintf (cmd, "%s %s -out '%s'  ", xdumpEditor, xdumpEditorAgrs, fn);
	system (cmd);
/***todo: gestion des erreurs */
	GetCroppingFrame (fn, xcf, ycf, wcf, hcf);
	if (new)
	  {
	     /* on devrait faire un GetImageFrame(fn, xif, yif, wif, hif),
	      * mais il est tellement mal fait que je triche un peu...
	      */
	     xif = xcf;
	     yif = ycf;
	     wif = wcf;
	     hif = hcf;
	  }
     }
   else
     {
/***todo: definir la gestion des abort... */
	if (new)
	  {
	     *xcf = *ycf = *wcf = *hcf = 0;
	  }
     }


}

/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                XDumpEditImage (char *fn, int xif, int yif, int wif, int hif, int *xcf, int *ycf, int *wcf, int *hcf)

#else  /* __STDC__ */
void                XDumpEditImage (fn, xif, yif, wif, hif, xcf, ycf, wcf, hcf)
char               *fn;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                *xcf;
int                *ycf;
int                *wcf;
int                *hcf;

#endif /* __STDC__ */

{
   Bool                new = True;
   Bool                goahead;

   /* que faire si fn existe ? refuser la commande ou remplacer le fichier ? */
   /* dans tous les cas faire attention de ne remplacer que du XDump */

   if (XDumpIsFormat (fn))
     {
	/* le fichier existe, c'est du xdump... */
	/* warnning: on ecrase un fichier xdump ok ? */
	goahead = dialogueReplace (fn);
	if (goahead)
	  {
	     DoEditImage (fn, !new, &xif, &yif, &wif, &hif, xcf, ycf, wcf, hcf);
	  }
     }
   else
     {
	if (FileExist (fn))
	  {
	     /* le fichier existe, ce n'est pas du xdump */
	     TtaDisplaySimpleMessage (XDUMP, INFO, XDUMP_THIS_FILE_IS_NOT_AN_XWD_FILE);

	  }
	else
	  {
	     /* le fichier n'existe pas, ce n'est pas du xdump. (ah bon ?) */
	     /* besoin de le creer pour DoEditImage ??? */
	     DoEditImage (fn, new, xif, yif, wif, hif, xcf, ycf, wcf, hcf);

	  }

     }
}				/*XDumpEditImage */


/* ---------------------------------------------------------------------- */
/* |    On traduit une image en postscript                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                XDumpPrintImage (char *fn, ImagePres pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd)

#else  /* __STDC__ */
void                XDumpPrintImage (fn, pres, xif, yif, wif, hif, xcf, ycf, wcf, hcf, fd)
char               *fn;
ImagePres           pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 xcf;
int                 ycf;
int                 wcf;
int                 hcf;
int                 fd;

#endif /* __STDC__ */

{
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   XImage             *Im;
   Pixmap              pix;
   int                 wim, him;
   register int        line, column;
   int                 count;
   register char       hexdata;

   xtmp = 0;
   ytmp = 0;
   /* on va transformer le dump en un pixmap */
   pix = XDumpCreateImage (fn, pres, &xcf, &ycf, &wcf, &hcf);

   switch (pres)
	 {
	    case SansModification:
	       /* on centre l'image en x */
	       /* quelle place a-t-on de chaque cote ? */
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    xif += delta;
		    /* la largeur de la boite est celle du cf */
		    wif = wcf;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */
		    xtmp = -delta;
		    /* on met a jour wcf pour etre coherent */
		    wcf = wif;
		 }
	       /* on centre l'image en y */
	       delta = (hif - hcf) / 2;
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    yif += delta;
		    /* la hauteur de la boite est celle du cf */
		    hif = hcf;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */

		    ytmp = -delta;
		    /* on met a jour hcf pour etre coherent */
		    hcf = hif;
		 }
	       break;
	    case MemesProportions:
	       /* facteur d'expansion en x */
	       Scx = (float) wif / (float) wcf;
	       /* facteur d'expansion en y */
	       Scy = (float) hif / (float) hcf;
	       if (Scx <= Scy)
		 {
		    /* on ne touche pas a y */
		    xif += (wif - wcf * Scy) / 2;
		    /* recentrer en x */
		    wif = Scy / Scx * (float) wif;
		 }
	       else
		 {
		    /* on ne touche pas a x */
		    yif += (hif - hcf * Scx) / 2;
		    /* recentrer en y */
		    hif = Scx / Scy * (float) hif;
		 }
	       break;
	    case PleinCadre:
	       /* DumpImage fait du plein cadre avec wif et hif */
	       break;
	    default:
	       break;
	 }
   if (pix != None)
     {
	/* on transforme le pixmap en Ximage */
	/* si xtmp ou ytmp sont non nuls, ca veut dire qu'on retaille */
	/* dans le pixmap (cas SansModification) */
	Im = XGetImage (GDp (0), pix, xtmp, ytmp,
	(unsigned int) wcf, (unsigned int) hcf, (unsigned long) 1, XYPixmap);
	wim = Im->width;
	him = Im->height;
	fprintf (fd, "gsave %d -%d translate\n", PixelEnPt (xif, 1), PixelEnPt (yif + hif, 0));
	fprintf (fd, "%d %d %d %d DumpImage\n", wim, him, PixelEnPt (wif, 1), PixelEnPt (hif, 0));

	for (line = 0; line < him; line++)
	  {
	     hexdata = 0;
	     count = 0;
	     for (column = 0; column < wim; column++)
	       {
		  if ((XGetPixel (Im, column, line)) == WhitePixel (GDp (0), DefaultScreen (GDp (0))))
		     hexdata = hexdata | 0x01;
		  /* 1 est blanc pour DumpImage du prologue PostScript */
		  count++;
		  if (count == 8)
		    {
		       fprintf (fd, "%02x", (hexdata) & 0xff);
		       hexdata = 0;
		       count = 0;
		    }
		  hexdata = hexdata << 1;
	       }
	     if (count > 0)
	       {
		  hexdata = hexdata << (7 - count);
		  /* on remplit la fin de l'octet avec du blanc (padding) */
		  hexdata = hexdata | ((0x01 << (8 - count)) - 0x01);
		  fprintf (fd, "%02x", (hexdata) & 0xff);
	       }
	     fprintf (fd, "\n");
	  }
	fprintf (fd, "grestore\n");
	XDestroyImage (Im);
	XFreePixmap (GDp (0), pix);
     }
}				/*XDumpPrintImage */


/* ---------------------------------------------------------------------- */
/* |    On teste si c'est bien du xdump                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
Bool                XDumpIsFormat (char *fn)

#else  /* __STDC__ */
Bool                XDumpIsFormat (fn)
char               *fn;

#endif /* __STDC__ */

{
   int                 status;
   int                 w, h;
   Pixmap              bitmap = None;

   status = ReadWD (fn, &bitmap, &w, &h);
   if (bitmap != None)
     {
	XFreePixmap (GDp (0), bitmap);
     }
   return status;

}				/*XDumpIsFormat */


/* ---------------------------------------------------------------------- */
/* |    On recupere des infos supplementaires                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
ImageInfo          *XDumpGetImageInfo (char *fn)

#else  /* __STDC__ */
ImageInfo          *XDumpGetImageInfo (fn)
char               *fn;

#endif /* __STDC__ */

{
}				/*XDumpGetImageInfo */


/* ---------------------------------------------------------------------- */
/* |    On envoie une sequence au driver                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                XDumpEscape (char *arg)

#else  /* __STDC__ */
void                XDumpEscape (arg)
char               *arg;

#endif /* __STDC__ */

{
}				/*XDumpEscape */



/* ---------------------------------------------------------------------- */
/* |    Lit un fichier au format XWD (xdump)                            | */
/* |    Comparer avec xwud.c, fourni dans la distribution X du MIT.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          ReadWD (char *name, Pixmap * retPix, int *retW, int *retH)

#else  /* __STDC__ */
static int          ReadWD (name, retPix, retW, retH)
char               *name;
Pixmap             *retPix;
int                *retW;
int                *retH;

#endif /* __STDC__ */

{
   char               *w_name, *buffer;
   int                 fin, Nbb;
   XWDFileHeader       header;
   register int        i;
   XImage              image;
   ThotColorStruct    *colors;

   unsigned long       swaptest = 1;

   fin = open (name, 0);
   if (fin < 0)
      return 0;

   if (read (fin, (char *) &header, sizeof (header)) != sizeof (header))
     {
	TtaDisplaySimpleMessage (XDUMP, INFO, XDUMP_BAD_HEADER_SIZE);
	goto bad;
     }

   if (*(char *) &swaptest)
      _swaplong ((char *) &header, sizeof (header));

   if (header.file_version != XWD_FILE_VERSION ||
       header.header_size < sizeof (header) ||
       header.byte_order != header.bitmap_bit_order)
     {
	TtaDisplaySimpleMessage (XDUMP, INFO, XDUMP_BAD_HEADER_FORMAT);
	goto bad;
     }

   w_name = (char *) TtaGetMemory ((unsigned) (header.header_size - sizeof header));
   read (fin, w_name, (int) (header.header_size - sizeof header));

   if (header.ncolors)
     {
	colors = (ThotColorStruct *) TtaGetMemory ((unsigned) (header.ncolors * sizeof (ThotColorStruct)));
	read (fin, colors, (int) (header.ncolors * sizeof (ThotColorStruct)));
	if (*(char *) &swaptest)
	  {
	     for (i = 0; i < header.ncolors; i++)
	       {
		  _swaplong ((char *) &colors[i].pixel, sizeof (long));
		  _swapshort ((char *) &colors[i].red, 3 * sizeof (short));
	       }
	  }
     }
   else
      colors = 0;

   image.width = (int) header.pixmap_width;
   image.height = (int) header.pixmap_height;
   image.xoffset = (int) header.xoffset;
   image.format = (int) header.pixmap_format;
   image.byte_order = (int) header.byte_order;
   image.bitmap_unit = (int) header.bitmap_unit;
   image.bitmap_bit_order = (int) header.bitmap_bit_order;
   image.bitmap_pad = (int) header.bitmap_pad;
   image.depth = (int) header.pixmap_depth;
   image.bits_per_pixel = (int) header.bits_per_pixel;
   image.bytes_per_line = (int) header.bytes_per_line;
   image.red_mask = header.red_mask;
   image.green_mask = header.green_mask;
   image.blue_mask = header.blue_mask;
   image.obdata = NULL;
   _XInitImageFuncPtrs (&image);

   if (image.format != ZPixmap)
      Nbb = image.bytes_per_line * image.height * image.depth;
   else
      Nbb = image.bytes_per_line * image.height;
   buffer = (char *) TtaGetMemory (Nbb);
   read (fin, buffer, Nbb);
   image.data = buffer;
   close (fin);

   *retW = header.pixmap_width;
   *retH = header.pixmap_height;

   if (header.pixmap_depth == 1)
      image.format = XYBitmap;	/* TODO:   repondre a la question "pourquoi ?" */
   else if (header.pixmap_depth != DefaultDepth (GDp (0), DefaultScreen (GDp (0))))
     {
	TtaDisplaySimpleMessageNum (XDUMP, INFO, XDUMP_BAD_IMAGE_DEPTH, header.pixmap_depth);
	*retPix = None;
	return 1;
     }

   *retPix = XCreatePixmap (GDp (0), GRootW (0), *retW, *retH, DefaultDepth (GDp (0), DefaultScreen (GDp (0))));

   XPutImage (GDp (0), *retPix, graphicGC, &image, 0, 0, 0, 0, *retW, *retH);

   free (buffer);
   if (colors)
      free (colors);
   return 1;

 bad:
   close (fin);
   return 0;
}


/* ---------------------------------------------------------------------- */
/* |    Les deux routines suivantes empruntees integralement du         | */
/* |    programme xwud fourni avec la distribution X du MIT             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static              _swapshort (register char *bp, register unsigned n)

#else  /* __STDC__ */
static              _swapshort (bp, n)
register char      *bp;
register unsigned   n;

#endif /* __STDC__ */

{
   register char       c;
   register char      *ep = bp + n;

   while (bp < ep)
     {
	c = *bp;
	*bp = *(bp + 1);
	bp++;
	*bp++ = c;
     }
}

#ifdef __STDC__
static              _swaplong (register char *bp, register unsigned n)

#else  /* __STDC__ */
static              _swaplong (bp, n)
register char      *bp;
register unsigned   n;

#endif /* __STDC__ */

{
   register char       c;
   register char      *ep = bp + n;
   register char      *sp;

   while (bp < ep)
     {
	sp = bp + 3;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	sp = bp + 1;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	bp += 2;
     }
}
