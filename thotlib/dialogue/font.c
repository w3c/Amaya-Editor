
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   font.c -- Module de gestion des polices de caracteres
   I. Vatton - Juillet 87       
 */

#include "thot_sys.h"
#include "ctype.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "xpmP.h"

/*  tolower(c) is a macro defined in ctypes.h that returns
   something wrong if c is not an upper case letter. */
#define TOLOWER(c)	(isupper(c)? tolower(c) : (c))

#define EXPORT extern
#include "frame.var"
#undef EXPORT
#define EXPORT
#include "font.var"


/* La table des polices de caracteres */
static int          FirstRemovableFont = 1;	/* premiere fonte qu'on peut virer */

static char         EvidenceT[MAX_HIGHLIGHT] = "rbiogq";
static int          NbMaxTaille;	/* Nombre maximum de tailles de fontes */
static int          TenPoints[MAX_LOG_SIZE] =
{6, 8, 10, 12, 14, 16, 20, 24, 30, 40, 60};
static char        *FontFamily;
static boolean      Enlucida;
static boolean      EnBitStream;


#include "memory.f"
#include "environ.f"
#include "font.f"
#include "es.f"
#include "lig.f"

#ifdef __STDC__
extern char        *TtaGetEnvString (char *);

#else  /* __STDC__ */
extern char        *TtaGetEnvString ();

#endif /* __STDC__ */

#ifdef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |    WinLoadFont : Load a Windows font in a Device context.          | */
/* ---------------------------------------------------------------------- */
void                WinLoadFont (HDC hdc, ptrfont font)
{
#if 0
   int                 i;
   HFONT               hFont;

   for (i = 0; i < MAX_FONT; i++)
      if (font == Police (0, i))
	{
	   hFont = Police (0, i);
	   if (hFont != 0)
	      SelectObject (hdc, hFont);
	   return;
	}
#endif
   SelectObject (hdc, font);

}
#endif /* NEW_WILLOWS */

/* ---------------------------------------------------------------------- */
/* |    VolumCar transforme le volume pixels en volume equivalent       | */
/* |            caracteres.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 VolumCar (int volpixel)

#else  /* __STDC__ */
int                 VolumCar (volpixel)
int                 volpixel;

#endif /* __STDC__ */

{
   return volpixel / 200;
}

/* ---------------------------------------------------------------------- */
/* |    NumberOfFonts rend le nombre de tailles de caracteres definies. | */
/* ---------------------------------------------------------------------- */
int                 NumberOfFonts ()
{
   return NbMaxTaille + 1;
}

/* ---------------------------------------------------------------------- */
/* |    PtEnPixel transforme la valeur Point en valeur Pixel sur l'axe  | */
/* |            des x ou des y suivant la valeur de horiz.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 PtEnPixel (int valeur, int horiz)
#else  /* __STDC__ */
int                 PtEnPixel (valeur, horiz)
int                 valeur;
int                 horiz;

#endif /* __STDC__ */
{
   return ((valeur * PTS_POUCE) / PTS_POUCE);
}

/* ---------------------------------------------------------------------- */
/* |    PixelEnPt transforme la valeur Pixel en valeur Point sur l'axe  | */
/* |            des x ou des y suivant la valeur de horiz.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 PixelEnPt (int valeur, int horiz)
#else  /* __STDC__ */
int                 PixelEnPt (valeur, horiz)
int                 valeur;
int                 horiz;

#endif /* __STDC__ */
{
   return ((valeur * PTS_POUCE + PTS_POUCE / 2) / PTS_POUCE);
}

/* ---------------------------------------------------------------------- */
/* |    CarWidth rend la largeur du caractere c dans la fonte font.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 CarWidth (unsigned char c, ptrfont font)
#else  /* __STDC__ */
int                 CarWidth (c, font)
unsigned char       c;
ptrfont             font;

#endif /* __STDC__ */
{
   if (font == NULL)
      return (0);
   else
     {
#ifdef NEW_WILLOWS
	SIZE                size;

	WIN_GetDeviceContext (-1);
	WinLoadFont (WIN_curHdc, font);
	/* GetTextExtentPoint32(WIN_curHdc, ptcar, lg, &size); */
	GetTextExtentPoint (WIN_curHdc, &c, 1, &size);
	return (size.cx);
#else  /* NEW_WILLOWS */
	int                 l;

	if (((XFontStruct *) font)->per_char == NULL)
	   l = ((XFontStruct *) font)->max_bounds.width;
	else
	   l = ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].width;

	return l;
#endif /* !NEW_WILLOWS */
     }
}

/* ---------------------------------------------------------------------- */
/* |    CarHeight rend la hauteur du caractere c dans la fonte font.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 CarHeight (unsigned char c, ptrfont font)

#else  /* __STDC__ */
int                 CarHeight (c, font)
unsigned char       c;
ptrfont             font;

#endif /* __STDC__ */

{
   if (font == NULL)
      return (0);
#ifdef NEW_WILLOWS
   else
     {
	SIZE                size;

	WIN_GetDeviceContext (-1);
	WinLoadFont (WIN_curHdc, font);
	/* GetTextExtentPoint32(WIN_curHdc, ptcar, lg, &size); */
	GetTextExtentPoint (WIN_curHdc, &c, 1, &size);
	return (size.cy);
     }
#else  /* NEW_WILLOWS */
   else if (((XFontStruct *) font)->per_char == NULL)
      return FontHeight (font);
   else
      return ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].ascent
	 + ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].descent;
#endif /* !NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    CarAscent rend l'ascent du caractere c dans la fonte font.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 CarAscent (unsigned char c, ptrfont font)

#else  /* __STDC__ */
int                 CarAscent (c, font)
unsigned char       c;
ptrfont             font;

#endif /* __STDC__ */

{
   if (font == NULL)
      return (0);
#ifdef NEW_WILLOWS
   else
     {
	TEXTMETRIC          textMetric;
	BOOL                res;

	WIN_GetDeviceContext (-1);
	WinLoadFont (WIN_curHdc, font);
	res = GetTextMetrics (WIN_curHdc, &textMetric);
	if (res)
	   return (textMetric.tmAscent);
	else
	   return (0);
     }
#else  /* NEW_WILLOWS */
   else if (((XFontStruct *) font)->per_char == NULL)
      return ((XFontStruct *) font)->max_bounds.ascent;
   else
      return ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].ascent;
#endif /* !NEW_WILLOWS */

}

/* ---------------------------------------------------------------------- */
/* |    FontAscent rend l'ascent de la fonte font.                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 FontAscent (ptrfont font)

#else  /* __STDC__ */
int                 FontAscent (font)
ptrfont             font;

#endif /* __STDC__ */

{
   if (font == NULL)
      return (0);
#ifdef NEW_WILLOWS
   else
     {
	TEXTMETRIC          textMetric;
	BOOL                res;

	WIN_GetDeviceContext (-1);
	WinLoadFont (WIN_curHdc, font);
	res = GetTextMetrics (WIN_curHdc, &textMetric);
	if (res)
	   return (textMetric.tmAscent);
	else
	   return (0);
     }
#else  /* NEW_WILLOWS */
   else
      return ((XFontStruct *) font)->ascent;
#endif /* !NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    FontHeight retourne la hauteur de la police courante utilisee.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 FontHeight (ptrfont font)

#else  /* __STDC__ */
int                 FontHeight (font)
ptrfont             font;

#endif /* __STDC__ */

{
   if (font == NULL)
      return (0);
#ifdef NEW_WILLOWS
   else
     {
	TEXTMETRIC          textMetric;
	BOOL                res;

	WIN_GetDeviceContext (-1);
	WinLoadFont (WIN_curHdc, font);
	res = GetTextMetrics (WIN_curHdc, &textMetric);
	if (res)
	   return (textMetric.tmAscent + textMetric.tmDescent);
	else
	   return (0);
     }
#else  /* NEW_WILLOWS */
   else
      return ((XFontStruct *) font)->max_bounds.ascent + ((XFontStruct *) font)->max_bounds.descent;
#endif /* !NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    PixelValue calcule la valeur pixel en fonction de l'unite       | */
/* |            logique associee.                                       | */
/* |            Le parametre pAb donne l'adresse du pave courant,    | */
/* |            sauf dans le cas UnPercent, ou pAb contient la       | */
/* |            valeur de reference.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 PixelValue (int val, TypeUnit unit, PtrAbstractBox pAb)
#else  /* __STDC__ */
int                 PixelValue (val, unit, pAb)
int                 val;
TypeUnit            unit;
PtrAbstractBox             pAb;

#endif /* __STDC__ */
{
   int                 dist, i;

   switch (unit)
	 {
	    case UnRelative:
	       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
		  dist = 0;
	       else
		  dist = (val * FontHeight (pAb->AbBox->BxFont) + 5) / 10;
	       break;
	    case UnXHeight:
	       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
		  dist = 0;
	       else
		  dist = (val * CarHeight ('X', pAb->AbBox->BxFont)) / 10;
	       break;
	    case UnPoint:
	       dist = PtEnPixel (val, 1);
	       break;
	    case UnPixel:
	       dist = val;
	       break;
	    case UnPercent:
	       i = val * (int) pAb;
	       dist = i / 100;
	       break;
	 }
   return (dist);
}

/* ---------------------------------------------------------------------- */
/* |    PixelValue calcule la valeur logique a partir de la valeur      | */
/* |            pixel en fonction de l'unite logique associee.          | */
/* |            Le parametre pAb donne l'adresse du pave courant,    | */
/* |            sauf dans le cas UnPercent, ou pAb contient la       | */
/* |            valeur de reference.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 LogicalValue (int val, TypeUnit unit, PtrAbstractBox pAb)
#else  /* __STDC__ */
int                 LogicalValue (val, unit, pAb)
int                 val;
TypeUnit            unit;
PtrAbstractBox             pAb;

#endif /* __STDC__ */
{
   int                 dist, i;

   switch (unit)
	 {
	    case UnRelative:
	       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
		  dist = 0;
	       else
		  dist = val * 10 / FontHeight (pAb->AbBox->BxFont);
	       break;
	    case UnXHeight:
	       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
		  dist = 0;
	       else
		  dist = val * 10 / CarHeight ('x', pAb->AbBox->BxFont);
	       break;
	    case UnPoint:
	       dist = PixelEnPt (val, 1);
	       break;
	    case UnPixel:
	       dist = val;
	       break;
	    case UnPercent:
	       if (pAb == NULL)
		  dist = 0;
	       else
		 {
		    i = val * 100;
		    dist = i / (int) pAb;
		 }
	       break;
	 }
   return (dist);
}


/* ---------------------------------------------------------------------- */
/* |    FontBase rend le decalage de la ligne de base par rapport a`    | */
/* |            l'origine pour la fonte font.                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 FontBase (ptrfont font)

#else  /* __STDC__ */
int                 FontBase (font)
ptrfont             font;

#endif /* __STDC__ */

{
   if (font == NULL)
      return (0);
   else
      return (FontAscent (font));
}

/* ---------------------------------------------------------------------- */
/* | PseudoTaille convertit une taille en points en taille relative     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 PseudoTaille (int taille)
#else  /* __STDC__ */
int                 PseudoTaille (taille)
int                 taille;

#endif /* __STDC__ */
{
   int                 j;

   j = 0;
   while ((taille > TenPoints[j]) && (j < NbMaxTaille))
      j++;

   return (j);
}

/* ---------------------------------------------------------------------- */
/* | TailleEnPoints convertit une taille relative en taille en points   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TailleEnPoints (int taille)
#else  /* __STDC__ */
int                 TailleEnPoints (taille)
int                 taille;

#endif /* __STDC__ */
{

   if (taille > NbMaxTaille)
      taille = NbMaxTaille;
   else if (taille < 0)
      taille = 0;

   return (TenPoints[taille]);
}

/* ---------------------------------------------------------------------- */
/* |    LoadFont charge une fonte par acces via FONT_PATH.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
ptrfont             LoadFont (char name[100])
#else  /* __STDC__ */
ptrfont             LoadFont (name)
char                name[100];

#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   return (NULL);
#else  /* NEW_WILLOWS */
   char                tmp[200];
   XFontStruct        *result;
   int                 mincar;
   int                 spacewd;

   strcpy (tmp, name);

   result = XLoadQueryFont (GDp (0), tmp);
   if (result != NULL)
      if (result->per_char != NULL)
	{
	   mincar = result->min_char_or_byte2;
	   spacewd = result->per_char[32 - mincar].width;
	   if (result->max_char_or_byte2 > BLANC_DUR)
	      /* largeur(Ctrl Space) = largeur(Space) */
	      result->per_char[BLANC_DUR - mincar].width = spacewd;
	   if (result->max_char_or_byte2 > SAUT_DE_LIGNE)
	      /* largeur(Ctrl Return) = largeur(Space) */
	      result->per_char[SAUT_DE_LIGNE - mincar].width = spacewd;
	   if (result->max_char_or_byte2 > FINE)
	      /* largeur(Fine) = 1/4largeur(Space) */
	      result->per_char[FINE - mincar].width = (spacewd + 3) / 4;
	   if (result->max_char_or_byte2 > DEMI_CADRATIN)
	      /* largeur(DemiCadratin) = 1/2largeur(Space) */
	      result->per_char[DEMI_CADRATIN - mincar].width = (spacewd + 1) / 2;
	}
   return ((ptrfont) result);
#endif /* !NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    NomFonte calcule le nom Thot d'une fonte.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NomFonte (char alphabet, char police, int evidence, int taille, TypeUnit unit, char r_nom[10], char r_nomX[100])
#else  /* __STDC__ */
void                NomFonte (alphabet, police, evidence, taille, unit, r_nom, r_nomX)
char                alphabet;
char                police;
int                 evidence;
int                 taille;
TypeUnit            unit;
char                r_nom[10];
char                r_nomX[100];

#endif /* __STDC__ */
{

   if (evidence > MAX_HIGHLIGHT)
      evidence = MAX_HIGHLIGHT;
   if (alphabet == 'g' || alphabet == 'G')
     {
	evidence = 0;		/* romain uniquement pour les symboles */
	police = 's';		/* times uniquement pour les symboles */
	strcpy (r_nomX, "-*");
     }
   else
      strcpy (r_nomX, FontFamily);

   if (unit == UnRelative)
     {
	/* La taille est relative */
	if (taille < 0)
	   taille = TenPoints[0];
	else if (taille > NbMaxTaille)
	   taille = TenPoints[NbMaxTaille];
	else
	   taille = TenPoints[taille];
     }
   else if (unit == UnPixel)
      taille = PixelEnPt (taille, 0);

   if (Enlucida)
     {
	switch ((char) TOLOWER (police))
	      {
		 case 't':
		    strcat (r_nomX, "bright");
		    break;
		 case 'c':
		    strcat (r_nomX, "typewriter");
		    break;
		 default:
		    break;
	      }
     }
   else
     {
	switch ((char) TOLOWER (police))
	      {
		 case 't':
		    strcat (r_nomX, "-times");
		    break;
		 case 'h':
		    strcat (r_nomX, "-helvetica");
		    break;
		 case 'c':
		    strcat (r_nomX, "-courier");
		    break;
		 case 's':
		    strcat (r_nomX, "-symbol");
		    break;
		 default:
		    strcat (r_nomX, "-*");
	      }
     }

   switch ((char) TOLOWER (EvidenceT[evidence]))
	 {
	    case 'r':
	       strcat (r_nomX, "-medium-r");
	       break;
	    case 'i':
	    case 'o':
	       if ((char) TOLOWER (police) == 'h' || (char) TOLOWER (police) == 'c')
		  strcat (r_nomX, "-medium-o");
	       else
		  strcat (r_nomX, "-medium-i");
	       break;
	    case 'b':
	    case 'g':
	    case 'q':
	       if (Enlucida && (char) TOLOWER (police) == 't')
		  strcat (r_nomX, "-demibold-r");
	       else
		  strcat (r_nomX, "-bold-r");
	       break;
	 }

   if ((char) TOLOWER (police) == 'h')
      strcat (r_nomX, "-normal");	/* pas de narrow helvetica */
   else
      strcat (r_nomX, "-*");

   if ((char) TOLOWER (police) == 's')
     {
	if (EnBitStream)
	   /* il manque le champ corps */
	   sprintf (r_nomX, "%s-*-*-%d-83-83-p-*-*-fontspecific", r_nomX, taille * 10);
	else
	   sprintf (r_nomX, "%s-*-%d-*-75-75-p-*-*-fontspecific", r_nomX, taille);
     }
   else
     {
	if (EnBitStream)
	   sprintf (r_nomX, "%s-*-*-%d-83-83", r_nomX, taille * 10);
	else
	   sprintf (r_nomX, "%s-*-%d-*-75-75", r_nomX, taille);
	if ((char) TOLOWER (police) == 'c')
	   strcat (r_nomX, "-m-*");
	else
	   strcat (r_nomX, "-p-*");

	if ((char) TOLOWER (alphabet) == 'l')
	   strcat (r_nomX, "-iso8859-1");
	else
	   strcat (r_nomX, "-*-fontspecific");	/*adobe */
     }

   sprintf (r_nom, "%c%c%c%d",
	    TOLOWER (alphabet), TOLOWER (police),
	    EvidenceT[evidence], taille);
}

/* ---------------------------------------------------------------------- */
/* |    LireFonte fait un chargement simple d'une fonte Thot (pas       | */
/* |            d'utilisation du cache).                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
ptrfont             LireFonte (char alphabet, char police, int evidence, int taille, TypeUnit unit)
#else  /* __STDC__ */
ptrfont             LireFonte (alphabet, police, evidence, taille, unit)
char                alphabet;
char                police;
int                 evidence;
int                 taille;
TypeUnit            unit;

#endif /* __STDC__ */
{
   char                nom[10], nomX[100];

   NomFonte (alphabet, police, evidence, taille, unit, nom, nomX);
   return LoadFont (nomX);
}

#ifdef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |  WIN_LoadFont :  load a Windows TrueType with a defined set of     | */
/* |                  characteristics.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static HFONT        WIN_LoadFont (char alphabet, char police, int evidence,
				  int taille, TypeUnit unit, int frame)
#else  /* __STDC__ */
static HFONT        WIN_LoadFont (alphabet, police, evidence, taille, unit, frame)
char                alphabet;
char                police;
int                 evidence;
int                 taille;
TypeUnit            unit;
int                 frame;

#endif /* __STDC__ */
{
   char               *WIN_lpszFace;
   int                 WIN_nHeight = 0;
   int                 WIN_nWidth = 0;
   int                 WIN_fnWeight = FW_NORMAL;
   int                 WIN_fdwItalic = FALSE;
   int                 WIN_fdwUnderline = FALSE;
   int                 WIN_fdwStrikeOut = FALSE;
   HFONT               hFont;

   fprintf (stderr, "WIN_LoadFont('%c','%c',%d,%d,%d,%d)\n",
	    alphabet, police, evidence, taille, unit, frame);

   switch (alphabet)
	 {
	    case 'L':
	       /* Latin alphabet, Ok */
	       fprintf (stderr, "Latin, ");
	       break;
	    case 'G':
	       /* Graphics character set */
	       goto no_win;
	       break;
	    default:
	       fprintf (stderr, "unknown alphabet '%c'\n", alphabet);
	       goto no_win;
	 }
   switch (police)
	 {
	    case 'T':
	    case 't':
	       WIN_lpszFace = "Times New Roman";
	       break;
	    case 'H':
	    case 'h':
	       WIN_lpszFace = "Arial";
	       break;
	    case 'C':
	    case 'c':
	       WIN_lpszFace = "Courier New";
	       break;
	    default:
	       fprintf (stderr, "unknown police '%c'\n", police);
	       goto no_win;
	 }
   fprintf (stderr, "'%s', ", WIN_lpszFace);
   switch (EvidenceT[evidence])
	 {
	    case 'r':
	       break;
	    case 'i':
	    case 'o':
	       WIN_fdwItalic = TRUE;
	       fprintf (stderr, "italic, ");
	       break;
	    case 'b':
	    case 'g':
	    case 'q':
	       WIN_fnWeight = FW_BOLD;
	       fprintf (stderr, "bold, ");
	       break;
	    default:
	       fprintf (stderr, "unknown evidence %d\n", evidence);
	       goto no_win;
	 }
   fprintf (stderr, "%d pt, ", taille);
   WIN_nHeight = -MulDiv (taille, PTS_POUCE, 72);

   hFont = CreateFont (WIN_nHeight, WIN_nWidth, 0, 0, WIN_fnWeight,
		       WIN_fdwItalic, WIN_fdwUnderline, WIN_fdwStrikeOut,
		   DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
		       PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		       WIN_lpszFace);
   if (hFont == NULL)
     {
	fprintf (stderr, "Not found ...\n");
	WinErrorBox ();
     }
   else
      fprintf (stderr, "Loaded\n");
 no_win:
   return (hFont);
}
#endif /* NEW_WILLOWS */

/* ---------------------------------------------------------------------- */
/* |    ChargePoliceVoisine charge la police de caracteres identifie par| */
/* |            l'alphabet, la police, le niveau de mise en evidence et | */
/* |            la taille, exprimee en points typographiques (Enpt = 1) | */
/* |            ou relative, pour le compte de la fenetre frame.                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static ptrfont      ChargePoliceVoisine (char alphabet, char police, int evidence, int taille, TypeUnit unit, int frame, boolean Croissant)
#else  /* __STDC__ */
static ptrfont      ChargePoliceVoisine (alphabet, police, evidence, taille, unit, frame, Croissant)
char                alphabet;
char                police;
int                 evidence;
int                 taille;
TypeUnit            unit;
int                 frame;
boolean             Croissant;

#endif /* __STDC__ */
{
   int                 i, j, deb, index;
   int                 masque;
   char                texte[10], EnPs[10], texteX[100];
   ptrfont             ptfont;

   /* on s'aligne systematiquement sur des tailles standards */
   if (unit == UnPoint)
     {
	/* recherche la taille standard voisine */
	index = 0;
	while (TenPoints[index] < taille && index <= NbMaxTaille)
	   index++;
     }
   else
      index = taille;


   if (EnBitStream && taille == 11)
      /* accepte la police 11 pt dans le cas des polices bitstream */
      NomFonte (alphabet, police, evidence, taille, True, texte, texteX);
   else
      NomFonte (alphabet, police, evidence, index, False, texte, texteX);

   /* On prepare le nom de la police PostScript associee */
   strcpy (EnPs, texte);

   /* On recherche si la police est deja chargee */
   j = 0;			/* initialisation (pour le compilateur !) */
   i = 0;
   deb = 0;
   ptfont = NULL;
   while ((ptfont == NULL) && (i < MAX_FONT) && (Police (0, i) != NULL))
     {
	j = strcmp (&NomPolice (0, deb), texte);
	if (j == 0)
	  {
	     /* Entree trouvee */
	     ptfont = Police (0, i);
	     /*Fin de la recherche */
	  }
	else
	   i++;
	deb += MAX_NFONT;
     }

   /* On charge une nouvelle police de caracteres */
   if (ptfont == NULL)
     {
	/* Est-ce que la table des fontes est pleine */
	if (i >= MAX_FONT)
	   TtaDisplaySimpleMessageString (LIB, INFO, LIB_NO_MORE_PLACE_FOR_THE_FONT, texteX);
	else
	  {
	     strcpy (&NomPolice (0, i * MAX_NFONT), texte);
	     strcpy (&NomPs (0, i * 8), EnPs);

#ifdef NEW_WILLOWS
	     ptfont = WIN_LoadFont (alphabet, police, evidence, taille, unit, frame);
#else  /* NEW_WILLOWS */
	     ptfont = LoadFont (texteX);
#endif /* !NEW_WILLOWS */
	     /* Le chargement a echoue, il faut essayer une police voisine */
	     if (ptfont == NULL)
	       {
		  /* Il faut changer de taille */
		  if (index == NbMaxTaille)
		    {
		       /* on ne peut plus augmenter la taille */
		       Croissant = False;
		       index--;
		    }
		  else if (Croissant)
		     index++;
		  else
		     index--;

		  if (index < NbMaxTaille && index >= 0)
		     ptfont = ChargePoliceVoisine (alphabet, police, evidence, index, False, frame, Croissant);
		  else if (index >= NbMaxTaille)
		     ptfont = ChargePoliceVoisine (alphabet, police, evidence, NbMaxTaille, False, frame, False);
		  if (ptfont == NULL)
		     TtaDisplaySimpleMessageString (LIB, INFO, LIB_MISSING_FILE, texteX);
	       }

	  }

	if (ptfont == NULL)
	  {
	     /* On prend eventuellement une police de meme alphabet */
	     j = 0;
	     while (j < MAX_FONT)
	       {
		  if (Police (0, j) == NULL)
		     j = MAX_FONT;
		  else if (NomPolice (0, j * MAX_NFONT) == alphabet)
		    {
		       ptfont = Police (0, j);
		       j = MAX_FONT;
		    }
		  else
		     j++;
	       }

	     /* Ou a defaut la police standard */
	     if (ptfont == NULL)
	       {
		  ptfont = FontMenu;
		  j = 0;
	       }
	  }

	if (i >= MAX_FONT)
	   i = j;		/* c'est une ancienne entree */
	else
	  {
	     /* c'est une nouvelle entree qu'il faut initialiser */
	     Police (0, i) = ptfont;
	     PoliceVue (0, i) = 0;
	  }
     }

   /* On calcule le masque de la fenetre */
   masque = 1 << (frame - 1);
   /* On memorise le numero de la fenetre */
   PoliceVue (0, i) = PoliceVue (0, i) | masque;
   return (ptfont);
}

/* ---------------------------------------------------------------------- */
/* |    ChargeFonte charge la police de caracteres identifie par        | */
/* |            l'alphabet, la police, le niveau de mise en evidence et | */
/* |            la taille, exprimee en points typographiques (Enpt = 1) | */
/* |            ou relative, pour le compte de la fenetre frame.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
ptrfont             ChargeFonte (char alphabet, char police, int evidence, int taille, TypeUnit unit, int frame)
#else  /* __STDC__ */
ptrfont             ChargeFonte (alphabet, police, evidence, taille, unit, frame)
char                alphabet;
char                police;
int                 evidence;
int                 taille;
TypeUnit            unit;
int                 frame;

#endif /* __STDC__ */
{
   /* pas de police inferieure a 6 points */
   if (taille < 6 && unit == UnPoint)
      taille = 6;
   return ChargePoliceVoisine (alphabet, police, evidence, taille, unit, frame, True);
}


/* ---------------------------------------------------------------------- */
/* |    InitFont charge les intitules qui determineront les polices de  | */
/* |            caracteres.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitFont (char *name)
#else  /* __STDC__ */
void                InitFont (name)
char               *name;

#endif /* __STDC__ */
{
   int                 i;
   int                 ndir, ncurrent;
   char              **dirlist = NULL;
   char              **currentlist = NULL;
   char               *fontpath;
   char                FONT_PATH[128];
   char               *value;
   int                 f3, f4, f5;

   /* Lecture eventuelle de la famille de caracteres */
   MenuSize = 12;
   value = TtaGetEnvString ("FontFamily");
   NbMaxTaille = 10;
   if (value == NULL)
     {
	FontFamily = TtaGetMemory (8);
	strcpy (FontFamily, "-*");
     }
   else
     {
	FontFamily = TtaGetMemory (strlen (value) + 1);
	strcpy (FontFamily, value);
	if (!strcmp (FontFamily, "-b&h-lucida"))
	   Enlucida = True;
	else
	  {
	     Enlucida = False;
	     if (!strcmp (FontFamily, "gipsi-bitstream"))
	       {
		  EnBitStream = True;
		  /* Modifie les tailles 30, 40 et 60 en 36, 48 et 72 */
		  TenPoints[NbMaxTaille] = 72;
		  TenPoints[NbMaxTaille - 1] = 48;
		  TenPoints[NbMaxTaille - 2] = 36;
		  MenuSize = 11;
	       }
	     else
		EnBitStream = False;
	  }
     }
   PTS_POUCE = 72;		/* Nombre de points typographiques par pouce */


   /* Lecture eventuelle de la taille des caracteres dans les menus */
   value = TtaGetEnvString ("FontMenuSize");
   if (value != NULL)
      sscanf (value, "%d", &MenuSize);
   /* Autres valeurs des polices de menus choisies */
   f3 = MenuSize + 2;
   f4 = MenuSize - 2;
   f5 = MenuSize;

#ifndef NEW_WILLOWS
   fontpath = ThotPath ("THOTFONT");
   if (fontpath)
     {
	strcpy (FONT_PATH, fontpath);
	strcat (FONT_PATH, "/");

	/* Ajoute le repertoire FONT_PATH dans la liste du serveur */
	currentlist = XGetFontPath (GDp (0), &ncurrent);
	ndir = 1;
	/* 1 repertoire a ajouter */
	/* Verifie que le repertoire n'est pas deja enregistre */
	i = 0;
	while ((ndir == 1) && (i < ncurrent))
	  {
	     if (strncmp (currentlist[i], FONT_PATH, strlen (currentlist[i]) - 1) == 0)
		ndir = 0;
	     else
		i++;
	  }

	/* Faut-il mettre a jour la liste des repertoires ? */
	if (ndir > 0)
	  {
	     ndir += ncurrent;
	     dirlist = (char **) TtaGetMemory (ndir * sizeof (char *));

	     if (currentlist != NULL)
#ifdef SYSV
		memcpy (dirlist, currentlist, ncurrent * sizeof (char *));

#else
		bcopy (currentlist, dirlist, ncurrent * sizeof (char *));

#endif
	     dirlist[ncurrent] = FONT_PATH;
	     XSetFontPath (GDp (0), dirlist, ndir);
	     TtaFreeMemory ((char *) dirlist);
	  }
	TtaFreeMemory ((char *) currentlist);
     }
#endif /* NEW_WILLOWS */

   /* Initialisation des fontes locales au Mediateur */
   FontMenu = FontMenu2 = FontMenu3 = FontMenu4 = NULL;
   FontIS = NULL;
   FontIGr = NULL;
   FonteLeg = NULL;

   /* Initialisation des tables de fontes */
   for (i = 0; i < MAX_FONT; i++)
      Police (0, i) = NULL;

   /*premiere fonte chargee */
   FontMenu = ChargeFonte ('L', 't', 0, MenuSize, UnPoint, 0);
   if (FontMenu == NULL)
     {
	FontMenu = ChargeFonte ('L', 'l', 0, MenuSize, UnPoint, 0);
	if (FontMenu == NULL)
	   TtaDisplaySimpleMessage (LIB, FATAL, LIB_ERR_FONT_NOT_FOUND);
     }
   /*deuxieme fonte chargee */
   FontMenu2 = ChargeFonte ('L', 't', 2, 12, UnPoint, 0);
   if (FontMenu2 == NULL)
     {
	FontMenu2 = ChargeFonte ('L', 'l', 2, 12, UnPoint, 0);
	if (FontMenu2 == NULL)
	   FontMenu2 = FontMenu;
     }
   /*troisieme fonte chargee */
   FontMenu3 = ChargeFonte ('L', 't', 1, f3, UnPoint, 0);
   if (FontMenu3 == NULL)
     {
	FontMenu3 = ChargeFonte ('L', 't', 1, f3, UnPoint, 0);
	if (FontMenu3 == NULL)
	   FontMenu3 = FontMenu2;
     }
   /*quatrieme fonte chargee */
   FontMenu4 = ChargeFonte ('L', 'h', 1, f4, UnPoint, 0);
   if (FontMenu4 == NULL)
     {
	FontMenu4 = ChargeFonte ('L', 'h', 1, f4, UnPoint, 0);
	if (FontMenu4 == NULL)
	   FontMenu4 = FontMenu;
     }
   /*cinquieme fonte chargee */
   FontGraph = ChargeFonte ('L', 't', 1, f5, UnPoint, 0);
   if (FontGraph == NULL)
     {
	FontGraph = ChargeFonte ('L', 't', 1, f5, UnPoint, 0);
	if (FontGraph == NULL)
	   FontGraph = FontMenu;
     }

   FirstRemovableFont = 5;
}				/*InitFont */

/* ---------------------------------------------------------------------- */
/* |    LibFont libere les polices de caracteres chargees  a` la demande| */
/* |            pour le compte de la fenetre frame.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LibFont (int frame)
#else  /* __STDC__ */
void                LibFont (frame)
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, masque;
   int                 flag;

   if (frame > 0)
     {
	/* On calcule le masque de la fenetre */
	masque = 1 << (frame - 1);

	i = FirstRemovableFont;
	/* On garde les premieres fontes */
	while (i < MAX_FONT && Police (0, i) != NULL)
	  {
	     /* Est-ce que la police n'est utilisee que par cette frame */
	     if (PoliceVue (0, i) == masque)
	       {
		  /* Est-ce que la police de caracteres a une copie */
		  j = 0;
		  flag = 0;
		  while (flag == 0)
		    {
		       if (j == MAX_FONT)
			  flag = 1;
		       else if (j == i)
			  j++;
		       else if (Police (0, j) == Police (0, i))
			  flag = 1;
		       else
			  j++;
		    }
		  /* On libere la police ? */
#ifdef NEW_WILLOWS
		  if (j == MAX_FONT)
		    {
		       DeleteObject (Police (0, i));
		       DebugBreak ();
		    }
#else  /* NEW_WILLOWS */
		  if (j == MAX_FONT)
		     XFreeFont (GDp (0), (XFontStruct *) Police (0, i));
#endif /* NEW_WILLOWS */
		  Police (0, i) = NULL;
	       }
	     else
		PoliceVue (0, i) = PoliceVue (0, i) & (~masque);
	     i++;
	  }

	/* On reorganise la table des fontes */
	j = FirstRemovableFont;
	i--;
	while (j < i)
	  {
	     while (Police (0, j) != NULL)
	       {
		  j++;
		  /* On saute les entrees pleines */
	       }
	     while (Police (0, i) == NULL)
	       {
		  i--;
		  /* On saute les entrees vides */
	       }
	     if (j < i)
	       {
		  /* On remplace l'entree j par l'entree i */
		  Police (0, j) = Police (0, i);
		  Police (0, i) = NULL;
		  PoliceVue (0, j) = PoliceVue (0, i);
		  strcpy (&NomPolice (0, j * MAX_NFONT), &NomPolice (0, i * MAX_NFONT));
		  i--;
		  j++;
	       }
	  }
     }
}				/*LibFont */
