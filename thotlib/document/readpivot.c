
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   Ce module charge un document depuis un fichier contenant une representation
   pivot et construit les arbres abstraits correspondants.
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "functions.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "picture.h"

#include "language.h"
#include "libmsg.h"
#include "message.h"
#include "storage.h"
#include "typecorr.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "appdialogue.var"

boolean             error;
static boolean      msgAncienFormat;

/* table d'equivalence entre les couleurs de la version pivot 3 et celles */
/* des versions 4 et suivantes */
static int          newColor[] =
{0, 1, 2, 2, 2, 3, 32, 16, 8,
 8, 25, 25, 25, 17, 17, 25, 24, 40, 33, 16, 72, 2, 88, 96,
 112, 144, 8, 7, 6, 6, 6, 127, 119, 107, 7, 115, 115, 123, 117,
 115, 116, 108, 100, 106, 106, 102, 4, 105, 105, 97, 93, 83, 83, 92,
 88, 6, 83, 82, 79, 7, 5, 87, 86, 94, 74, 85, 68, 76, 68,
 84, 60, 70, 62, 71, 63, 5, 34, 34, 49, 40, 44, 36, 26, 29,
 30, 5, 11, 31, 22, 30, 18, 16, 17, 19, 18, 21, 22, 22, 19,
 11, 19, 28, 29, 11, 10, 11, 20, 12, 147, 148, 9, 9, 147, 150,
 149, 149, 140, 130, 130, 131, 131, 133, 133, 133, 133, 123, 3, 1, 7,
 7, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 2};

#define MaxDocExt 10

#include "tree_f.h"
#include "structcreation_f.h"
#include "dofile_f.h"
#include "callback_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "indcharge_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "changeabsbox_f.h"
#include "ouvre_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "externalref_f.h"
#include "schemas_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PivotError (BinFile fich)
#else  /* __STDC__ */
static void         PivotError (fich)
BinFile             fich;

#endif /* __STDC__ */
{
   int                 i;
   char                c;
   boolean             stop;

   stop = FALSE;
   i = 1;
   while (!stop && i < 200)
     {
	if (!BIOreadByte (fich, &c))
	   stop = TRUE;
	else
	  {
	     if (c < ' ')
	       {
		  printf ("^");
		  c = (char) ((int) c + (int) '@');
		  i++;
	       }
	     printf ("%c", c);
	     i++;
	  }
     }
   printf ("\n");
   error = TRUE;
}

/* ---------------------------------------------------------------------- */
/* | NormalizeFileName recupere les informations contenues dans le      | */
/* |            nom de fichier (cas des anciens fichiers pivots).       | */
/* |            La presentation et le type de l'image sont code's sur   | */
/* |            1 octet au debut du nom. On retourne dans name, le nom  | */
/* |            correct et dans oldTypeImage et oldPres le type et la   | */
/* |            presentation trouve ou 0.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static char        *NormalizeFileName (char *fn, int *oldTypeImage, PictureScaling * oldPres, boolean * trouve)

#else  /* __STDC__ */
static char        *NormalizeFileName (fn, oldTypeImage, oldPres, trouve)
char               *fn;
int                *oldTypeImage;
PictureScaling          *oldPres;
boolean            *trouve;

#endif /* __STDC__ */

{
   char               *name;

   name = fn;
   if (*name < ' ')
     {
	*oldTypeImage = *name++ - 1;
	*oldPres = (PictureScaling) (*oldTypeImage / 4);
	*oldTypeImage = *oldTypeImage % 4;
	*trouve = TRUE;
     }
   else
     {
	*trouve = FALSE;
	*oldPres = RealSize;
     }
   *oldTypeImage = UNKNOWN_FORMAT;
   return name;
}


/* ---------------------------------------------------------------------- */
/* |    CreateReglePres cree une regle de presentation pour l'image     | */
/* |            contenue dans pEl.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         CreateReglePres (PtrElement pEl, int typeimage, PictureScaling pres, int Vue)

#else  /* __STDC__ */
static void         CreateReglePres (pEl, typeimage, pres, Vue)
PtrElement          pEl;
int                 typeimage;
PictureScaling           pres;
int              Vue;

#endif /* __STDC__ */

{
   PtrPRule        pRegle;

   GetReglePres (&pRegle);	/* acquiert une regle */
   /* initialise d'abord la nouvelle regle */
   pRegle->PrPresMode = PresImmediate;
   pRegle->PrSpecifAttr = 0;
   pRegle->PrSpecifAttrSSchema = NULL;
   pRegle->PrViewNum = Vue;
   pRegle->PrType = PtPictInfo;
   pRegle->PrPictInfo.PicXArea = 0;
   pRegle->PrPictInfo.PicYArea = 0;
   pRegle->PrPictInfo.PicWArea = 0;
   pRegle->PrPictInfo.PicHArea = 0;
   pRegle->PrPictInfo.PicPresent = pres;
   pRegle->PrPictInfo.PicType = typeimage;
   /* chaine la nouvelle regle */
   pRegle->PrNextPRule = pEl->ElFirstPRule;
   pEl->ElFirstPRule = pRegle;
}

/* ---------------------------------------------------------------------- */
/* |    rdTypeDim lit un type de dimension dans le fichier et retourne  | */
/* |            sa valeur.                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      rdTypeDim (BinFile fich)
#else  /* __STDC__ */
static boolean      rdTypeDim (fich)
BinFile             fich;

#endif /* __STDC__ */
{
   char                c;
   boolean             rdTypeDim;

   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   if (c == C_PIV_ABSOLUTE)
      rdTypeDim = TRUE;
   else
      rdTypeDim = FALSE;
   return rdTypeDim;
}


/* ---------------------------------------------------------------------- */
/* | rdUnit lit une unite dans le fichier et retourne sa valeur.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static TypeUnit     rdUnit (BinFile file)
#else  /* __STDC__ */
static TypeUnit     rdUnit (file)
BinFile             fich;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = '\0';
	PivotError (file);
     }
   switch (c)
	 {
	    case C_PIV_EM:
	       return UnRelative;
	       break;
	    case C_PIV_PT:
	       return UnPoint;
	       break;
	    case C_PIV_PERCENT:
	       return UnPercent;
	       break;
	    default:
	       return UnRelative;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    rdSigne lit un signe dans le fichier et retourne sa valeur.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      rdSigne (BinFile fich)
#else  /* __STDC__ */
static boolean      rdSigne (fich)
BinFile             fich;

#endif /* __STDC__ */
{
   char                c;
   boolean             rdSigne;

   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   if (c == C_PIV_PLUS)
      rdSigne = TRUE;
   else
      rdSigne = FALSE;
   return rdSigne;
}

/* ---------------------------------------------------------------------- */
/* |    rdBooleen lit un booleen dans le fichier et retourne sa valeur. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      rdBooleen (BinFile fich)

#else  /* __STDC__ */
static boolean      rdBooleen (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   boolean             rdBooleen;

   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   if (c == C_PIV_TRUE)
      rdBooleen = TRUE;
   else
      rdBooleen = FALSE;
   return rdBooleen;
}

/* ---------------------------------------------------------------------- */
/* |    rdCadr lit un BAlignment dans le fichier et retourne sa valeur.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static BAlignment      rdCadr (BinFile fich)

#else  /* __STDC__ */
static BAlignment      rdCadr (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   BAlignment             rdCadr;

   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   switch (c)
	 {
	    case C_PIV_LEFT:
	       rdCadr = AlignLeft;
	       break;
	    case C_PIV_RIGHT:
	       rdCadr = AlignRight;
	       break;
	    case C_PIV_CENTERED:
	       rdCadr = AlignCenter;
	       break;
	    case C_PIV_LEFTDOT:
	       rdCadr = AlignLeftDots;
	       break;
	    default:
	       PivotError (fich);
	       PivotFormatError ("l");
	       rdCadr = AlignLeft;
	       break;
	 }
   return rdCadr;
}

/* ---------------------------------------------------------------------- */
/* |    rdTypePage lit un Type de page dans le fichier et retourne sa   | */
/* |            valeur.                                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PageType     rdTypePage (BinFile fich)

#else  /* __STDC__ */
static PageType     rdTypePage (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   PageType            rdTypePage;

   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   switch (c)
	 {
	    case C_PIV_COMPUTED_PAGE:
	       rdTypePage = PgComputed;
	       break;
	    case C_PIV_START_PAGE:
	       rdTypePage = PgBegin;
	       break;
	    case C_PIV_USER_PAGE:
	       rdTypePage = PgUser;
	       break;
	       /* les marques colonnes sont traduites en marques pages pour */
	       /* la version qui ne gere pas les colonnes */
	       /* un message est affiche a l'utilisateur pour qu'il */
	       /* repagine s'il le desire */
	    case C_PIV_COMPUTED_COL:
#ifdef __COLPAGE__
	       rdTypePage = ColComputed;
#else  /* __COLPAGE__ */
	       rdTypePage = PgComputed;
	       TtaDisplaySimpleMessage (INFO, LIB, LIB_DOC_WITH_COLUMNS_PAGINATE_AGAIN);
#endif /* __COLPAGE__ */
	       break;
	    case C_PIV_START_COL:
#ifdef __COLPAGE__
	       rdTypePage = ColBegin;
#else  /* __COLPAGE__ */
	       rdTypePage = PgComputed;
	       TtaDisplaySimpleMessage (INFO, LIB, LIB_DOC_WITH_COLUMNS_PAGINATE_AGAIN);
#endif /* __COLPAGE__ */
	       break;
	    case C_PIV_USER_COL:
#ifdef __COLPAGE__
	       rdTypePage = ColUser;
#else  /* __COLPAGE__ */
	       rdTypePage = PgComputed;
	       TtaDisplaySimpleMessage (INFO, LIB, LIB_DOC_WITH_COLUMNS_PAGINATE_AGAIN);
#endif /* __COLPAGE__ */
	       break;
	    case C_PIV_COL_GROUP:
#ifdef __COLPAGE__
	       rdTypePage = ColGroup;
#else  /* __COLPAGE__ */
	       rdTypePage = PgComputed;
	       TtaDisplaySimpleMessage (INFO, LIB, LIB_DOC_WITH_COLUMNS_PAGINATE_AGAIN);
#endif /* __COLPAGE__ */
	       break;
	    case C_PIV_REPEAT_PAGE:
	       /* les marques page rappel sont transformees */
	       /* en marques page calculees, car le nouveau */
	       /* code ne traite plus ces types de marques */
	       rdTypePage = PgComputed;
	       break;
	    default:
	       PivotError (fich);
	       PivotFormatError ("C");
	       rdTypePage = PgComputed;
	       break;
	 }
   return rdTypePage;
}

/* ---------------------------------------------------------------------- */
/* |    readComment lit un commentaire dans le fichier fich et retourne | */
/* |            un pointeur sur le premier buffer du texte du           | */
/* |            commentaire lu, si effectif est vrai. Si effectif est   | */
/* |            faux, le commentaire est simplement saute' et la        | */
/* |            fonction retourne NULL.                                 | */
/* |            Si oldformat est vrai, le commentaire est lu selon      | */
/* |            l'ancien format.                                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrTextBuffer      readComment (BinFile fich, boolean effectif, boolean oldformat)

#else  /* __STDC__ */
PtrTextBuffer      readComment (fich, effectif, oldformat)
BinFile             fich;
boolean             effectif;
boolean             oldformat;

#endif /* __STDC__ */

{
   char                c;
   PtrTextBuffer      b;
   int                 n, lg;
   PtrTextBuffer      readComment;

   lg = 0;
   /* lit l'octet qui suit la marque commentaire */
   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   if (oldformat && c != (char) C_PIV_BEGIN)
     {
	PivotError (fich);
	PivotFormatError ("y");	/* marque debut de commentaire ancien format absente */
	readComment = NULL;
     }
   else
     {
	if (!oldformat)
	   /* l'octet lu est l'octet de poids fort de la longueur */
	  {
	     lg = 256 * ((int) c);	/* lit le 2eme octet de la longueur */
	     if (!BIOreadByte (fich, &c))
	       {
		  c = '\0';
		  PivotError (fich);
	       }
	     lg += (int) c;
	  }
	if (!effectif)
	  {
	     /* on saute le texte du commentaire */
	     readComment = NULL;
	     if (oldformat)
		/* lit le fichier jusqu'au 1er zero */
		do
		   if (!BIOreadByte (fich, &c))
		     {
			c = '\0';
			PivotError (fich);
		     }
		while (c != '\0') ;
	     else
		while (lg > 0)
		  {
		     /* lit le nombre d'octets indique' */
		     if (!BIOreadByte (fich, &c))
			PivotError (fich);
		     lg--;
		  }
	  }
	else
	  {
	     /* on lit effectivement le texte du commentaire */
	     GetBufTexte (&b);
	     readComment = b;
	     n = 0;
	     if (oldformat)
		lg = 1;		/* il faut lire au moins le zero final */
	     if (lg > 0)
		do
		  {
		     /* lit le texte du commentaire */
		     if (n == MAX_CHAR - 1)	/* le buffer courant est plein, on change de buffer */
		       {
			  b->BuLength = n;
			  b->BuContent[n] = '\0';
			  b = NewTextBuffer (b);
			  n = 0;

		       }
		     if (!BIOreadByte (fich, &c))
		       {
			  c = '\0';
			  PivotError (fich);
		       }
		     n++;
		     b->BuContent[n - 1] = c;
		     if (!oldformat)
			lg--;	/* decremente le nombre d'octets  restant a lire */
		     else if (c == '\0')
		       {
			  n--;
			  lg = 0;
		       }
		  }
		while (lg != 0);	/* termine le buffer en cours */
	     b->BuLength = n;
	     b->BuContent[n] = '\0';
	  }
	if (oldformat)
	  {
	     /* lit la marque de fin */
	     if (!BIOreadByte (fich, &c))
		c = '\0';
	     if (c != (char) C_PIV_END)
	       {
		  PivotError (fich);
		  PivotFormatError ("c");
	       }
	  }
     }
   return readComment;
}

/* ---------------------------------------------------------------------- */
/* |    rdImageType lit le type de l'image d'un PictInfo.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          rdImageType (BinFile fich)

#else  /* __STDC__ */
static int          rdImageType (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   int                 n;

   BIOreadShort (fich, &n);
   /* Pour assurer la compatibilite avec Linux et autre machine */
   if (n == 255)
      return -1;
   else
      return n;
}

/* ---------------------------------------------------------------------- */
/* |    rdCroppingFrame lit la cropping frame d'un PictInfo.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         rdCroppingFrame (BinFile fich, int *PicXArea, int *PicYArea, int *PicWArea, int *PicHArea)

#else  /* __STDC__ */
static void         rdCroppingFrame (fich, PicXArea, PicYArea, PicWArea, PicHArea)
BinFile             fich;
int                *PicXArea;
int                *PicYArea;
int                *PicWArea;
int                *PicHArea;

#endif /* __STDC__ */

{
   int                 n;

   BIOreadShort (fich, &n);
   *PicXArea = PointToPixel (n);
   BIOreadShort (fich, &n);
   *PicYArea = PointToPixel (n);
   BIOreadShort (fich, &n);
   *PicWArea = PointToPixel (n);
   BIOreadShort (fich, &n);
   *PicHArea = PointToPixel (n);
}

/* ---------------------------------------------------------------------- */
/* |    rdImagePresentation lit la presentation d'un PictInfo.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PictureScaling    rdImagePresentation (BinFile fich)

#else  /* __STDC__ */
static PictureScaling    rdImagePresentation (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   PictureScaling           rdImagePresentation;

   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   switch (c)
	 {
	    case C_PIV_REALSIZE:
	       rdImagePresentation = RealSize;
	       break;
	    case C_PIV_RESCALE:
	       rdImagePresentation = ReScale;
	       break;
	    case C_PIV_FILLFRAME:
	       rdImagePresentation = FillFrame;
	       break;
	    default:
	       PivotError (fich);
	       PivotFormatError ("PICT");
/***todo: definir un autre type d'erreur ? */
	       rdImagePresentation = RealSize;
	       break;
	 }

   return rdImagePresentation;
}


/* ---------------------------------------------------------------------- */
/* |    LabelStringToInt convertit le label strn en un entier           | */
/* |            retourne 0 si label mal construit.                      | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                LabelStringToInt (LabelString strn, int *num)

#else  /* __STDC__ */
void                LabelStringToInt (strn, num)
LabelString         strn;
int                *num;

#endif /* __STDC__ */

{
   int                 i;
   int                 val;
   boolean             ok;
   char                c;

   ok = FALSE;
   *num = 0;
   if (strn[0] == 'L')
     {
	val = 0;
	i = 1;
	do
	  {
	     c = strn[i];
	     i++;
	     if (c >= '0' && c <= '9')
	       {
		  val = (val * 10) + (c - '0');
		  ok = TRUE;
	       }
	     else if (c != '\0')
		ok = FALSE;
	  }
	while (ok && c != '\0');
	if (ok)
	   *num = val;
     }
}


/* ---------------------------------------------------------------------- */
/* |    rdReference lit une reference dans le fichier fich et retourne  | */
/* |            le type de la reference lue (TR), le label de l'element | */
/* |            reference' (lab), un booleen indiquant si la reference  | */
/* |            est interne ou externe (RExt) et, dans le cas d'une     | */
/* |            reference externe, le nom (N) du document contenant     | */
/* |            l'element reference'.                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         rdReference (ReferenceType * TR, LabelString lab, boolean * RExt, DocumentIdentifier * I, BinFile fich)

#else  /* __STDC__ */
static void         rdReference (TR, lab, RExt, I, fich)
ReferenceType          *TR;
LabelString         lab;
boolean            *RExt;
DocumentIdentifier    *I;
BinFile             fich;

#endif /* __STDC__ */

{
   int                 j;
   char                c;

   NulIdentDoc (I);
   /* lit un octet */
   if (!BIOreadByte (fich, &c))
     {
	c = '\0';
	PivotError (fich);
     }
   /* cet octet represente-t-il un type de reference correct ? */
   if (!(c == C_PIV_REF_FOLLOW || c == C_PIV_REF_INCLUSION || c == C_PIV_REF_INCLUS_EXP))
      /* non, ancien format pivot */
      /* on envoie un message a l'utilisateur, si ce n'est deja fait */
     {
	if (msgAncienFormat)
	  {
	     PivotError (fich);
	     TtaDisplaySimpleMessage (INFO, LIB, LIB_OLD_PIV_FORMAT_SAVE_THE_DOC_WITH_THOT);
	     msgAncienFormat = FALSE;	/* c'est fait */
	  }
	if (c == MOldRefInterne || c == MOldRefExterne || c == MOldRefInclusion)
	   /* ancien format version 2 */
	  {
	     /* l'octet lu represente le type de la reference */
	     switch (c)
		   {
		      case MOldRefInterne:
			 *RExt = FALSE;
			 *TR = RefFollow;
			 break;
		      case MOldRefExterne:
			 *RExt = TRUE;
			 *TR = RefFollow;
			 break;
		      case MOldRefInclusion:
			 *TR = RefInclusion;
			 *RExt = TRUE;
			 break;
		   }
	     /* lit le type de label */
	     if (!BIOreadByte (fich, &c))
	       {
		  c = '\0';
		  PivotError (fich);
	       }
	     /* lit la valeur du label */
	     rdLabel (c, lab, fich);
	     if (*RExt && lab[0] != '\0')
		/* lit le nom du document contenant l'element reference' */
		BIOreadIdentDoc (fich, I);
	  }
	else
	   /* on interprete comme dans la version 1 */
	   /* c'est une reference renvoi interne */
	  {
	     *TR = RefFollow;
	     *RExt = FALSE;	/* il n'y a qu'un label court, sans marque */
	     /* l'octet lu est l'octet de poids fort du label */
	     j = 256 * ((int) c);	/* lit le 2eme octet du label */
	     if (!BIOreadByte (fich, &c))
	       {
		  c = '\0';
		  PivotError (fich);
	       }
	     j += (int) c;
	     /* convertit le label numerique en chaine de caracteres */
	     LabelIntToString (j, lab);
	  }
     }
   else
      /* format pivot correct */
      /* on lit les champs qui constituent la reference */
     {
	/* l'octet lu represente le type de la reference */
	switch (c)
	      {
		 case C_PIV_REF_FOLLOW:
		    *TR = RefFollow;
		    break;
		 case C_PIV_REF_INCLUSION:
		    *TR = RefInclusion;
		    break;
		 case C_PIV_REF_INCLUS_EXP:
		    *TR = RefInclusion;
		    break;
	      }
	/* lit l'indicateur reference  interne/externe */
	*RExt = !rdBooleen (fich);
	/* lit le type de label */
	if (!BIOreadByte (fich, &c))
	  {
	     c = '\0';
	     PivotError (fich);
	  }
	/* lit la valeur du label */
	rdLabel (c, lab, fich);
	if (*RExt && lab[0] != '\0')
	   /* lit l'identificateur du document contenant l'element reference' */
	   BIOreadIdentDoc (fich, I);

     }
}

/* ---------------------------------------------------------------------- */
/* |    GetElRefer cherche s'il existe un descripteur de reference      | */
/* |            designant l'element de label Lab dans le document de    | */
/* |            IDoc. Si IDoc est un identificateur vide, l'element designe'    | */
/* |            est interne au document en cours de lecture, dont le    | */
/* |            contexte est pointe' par pDoc. Si le descripteur        | */
/* |            n'existe pas, il est cree' et chaine'.                  | */
/* |            La fonction rend un pointeur sur le descripteur trouve' | */
/* |            ou cree'.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrReferredDescr GetElRefer (LabelString Lab, DocumentIdentifier IDoc, PtrDocument pDoc)

#else  /* __STDC__ */
static PtrReferredDescr GetElRefer (Lab, IDoc, pDoc)
LabelString         Lab;
DocumentIdentifier     IDoc;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrReferredDescr    pr;
   boolean             stop;
   PtrReferredDescr    pDe1;
   int                 i;

   pr = pDoc->DocReferredEl;
   stop = FALSE;
   do
      /* parcourt la chaine des descripteurs de reference du document en   */
      /* cours de lecture. (Le premier descripteur de la chaine est bidon) */
     {
	pr = pr->ReNext;
	if (pr == NULL)
	   stop = TRUE;		/* fin de la chaine */
	else if (strcmp (pr->ReReferredLabel, Lab) == 0)	/* le label correspond */
	   if (IdentDocNul (IDoc) && !pr->ReExternalRef)
	      /* on cherche une reference interne et c'en est une */
	      stop = TRUE;	/* trouve' */
	   else if (!IdentDocNul (IDoc) && pr->ReExternalRef)
	      /* on cherche une reference externe et c'en est une */
	      if (MemeIdentDoc (IDoc, pr->ReExtDocument))
		 stop = TRUE;
     }
   while (!stop);
   if (pr == NULL)
      /* on n'a pas trouve' le descripteur */
      /* on cree et chaine un nouveau descripteur */
     {
	pr = NewReferredElDescr (pDoc);
	/* on initialise le descripteur de reference cree'. */
	pDe1 = pr;
	strncpy (pDe1->ReReferredLabel, Lab, MAX_LABEL_LEN);
	pDe1->ReExternalRef = !IdentDocNul (IDoc);
	if (pDe1->ReExternalRef)
	   CopyIdentDoc (&pDe1->ReExtDocument, IDoc);
	else
	  {
	     LabelStringToInt (Lab, &i);
	     if (i > GetCurrentLabel (pDoc))
		SetCurrentLabel (pDoc, i);
	  }
     }
   return pr;
}

/* ---------------------------------------------------------------------- */
/* |    CreeReference chaine le descripteur de reference pointe' par    | */
/* |            RefPtr appartenant au document dont le contexte est     | */
/* |            pointe' par pDoc et initialise ce descripteur pour      | */
/* |            qu'il designe l'element de label lab appartenant au     | */
/* |            document I. Si I est un identificateur vide, il s'agit du       | */
/* |            document pDoc.                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         CreeReference (PtrReference RefPtr, ReferenceType TRef, LabelString lab, boolean RExt, DocumentIdentifier I, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CreeReference (RefPtr, TRef, lab, RExt, I, pDoc)
PtrReference        RefPtr;
ReferenceType           TRef;
LabelString         lab;
boolean             RExt;
DocumentIdentifier     I;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrReferredDescr    r;
   PtrReference        pRf;
   PtrReference        pPR1;

   if (lab[0] != '\0')
      /* cherche le descripteur d'element reference' correspondant */
     {
	r = GetElRefer (lab, I, pDoc);
	pPR1 = RefPtr;
	/* met le descripteur de reference a la fin de la chaine des */
	/* descripteur de reference du document */
	if (r->ReFirstReference == NULL)
	  {
	     r->ReFirstReference = RefPtr;
	     pPR1->RdPrevious = NULL;
	  }
	else
	  {
	     pRf = r->ReFirstReference;
	     while (pRf->RdNext != NULL)
		pRf = pRf->RdNext;
	     pRf->RdNext = RefPtr;
	     pPR1->RdPrevious = pRf;
	  }
	pPR1->RdNext = NULL;	/* remplit le descripteur de reference */
	pPR1->RdReferred = r;
	pPR1->RdTypeRef = TRef;
	pPR1->RdInternalRef = !RExt;
     }
}

/* ---------------------------------------------------------------------- */
/* |    ReadType lit dans le fichier pivot, selon la valeur de marque,  | */
/* |            un numero de type ou un nom de nature et rend le numero | */
/* |            de la regle definissant le type lu. Retourne 0 si       | */
/* |            erreur. Si marque est une marque de nature, au retour   | */
/* |            pSchStr contient un pointeur sur le schema de structure | */
/* |            de cette nature.                                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int  ReadType (PtrDocument pDoc, PtrSSchema * pSchStr, BinFile fichpiv, char *marque)

#else  /* __STDC__ */
static int  ReadType (pDoc, pSchStr, fichpiv, marque)
PtrDocument         pDoc;
PtrSSchema       *pSchStr;
BinFile             fichpiv;
char               *marque;

#endif /* __STDC__ */

{
   int                 nat;
   int         nr;
   Name                 NomSchPrs;
   boolean             Extension;

   nr = 0;
   if (*marque == (char) C_PIV_NATURE)
     {
	/* lit le numero de nature */
	BIOreadShort (fichpiv, &nat);
	if (nat < 0 || nat >= pDoc->DocNNatures)
	  {
	     PivotError (fichpiv);
	     PivotFormatError ("Nature Num");
	  }
	/* lit la marque de type qui suit */
	if (!error)
	   if (!BIOreadByte (fichpiv, marque))
	      PivotError (fichpiv);
	/* teste si le numero lu est celui de la structure generique du document */
	   else if (nat == 0)
	      *pSchStr = pDoc->DocSSchema;
	   else
	     {
		/* teste s'il s'agit d'une extension de la structure generique du */
		/* document */
		Extension = FALSE;	/* a priori, non */
		if (pDoc->DocNatureSSchema[nat] != NULL)
		   if (pDoc->DocNatureSSchema[nat]->SsExtension)
		     {
			Extension = TRUE;
			*pSchStr = pDoc->DocNatureSSchema[nat];
		     }
		if (!Extension)
		  {
		     NomSchPrs[0] = '\0';	/* pas de presentation preferentielle */
		     nr = CreeNature (pDoc->DocNatureName[nat], pDoc->DocNaturePresName[nat], *pSchStr);
		     /* recupere le numero de la regle de nature */
		     if (nr == 0)
		       {
			  PivotError (fichpiv);
			  PivotFormatError ("n");
		       }
		     else
			*pSchStr = (*pSchStr)->SsRule[nr - 1].SrSSchemaNat;
		  }
	     }
     }
   if (!error)
      if (*marque == (char) C_PIV_TYPE)
	{
	   BIOreadShort (fichpiv, &nr);		/* lit le numero de type de l'element */
	   if (pDoc->DocPivotVersion < 4)
	      /* on tient compte de l'ajout du type de base PolyLine */
	      if (nr >= MAX_BASIC_TYPE)
		 nr++;
	}
      else
	{
	   nr = 0;
	   PivotError (fichpiv);
	   PivotFormatError ("t");
	}

   return nr;
}

/* ---------------------------------------------------------------------- */
/* |    ContenuElExp compare le type de l'element en cours de lecture   | */
/* |            avec le type qui doit constituer le contenu d'un        | */
/* |            element exporte'. Si le type correspond, positionne les | */
/* |            indicateurs marquant qu'on doit creer tout le contenu   | */
/* |            de l'element courant et qu'on ne cherche plus a` creer  | */
/* |            de contenu.                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ContenuElExp (boolean * creetout, int * TypeElement, PtrSSchema * pSchStr, PtrSSchema * pschcont, int * typecontenu)

#else  /* __STDC__ */
static void         ContenuElExp (creetout, TypeElement, pSchStr, pschcont, typecontenu)
boolean            *creetout;
int        *TypeElement;
PtrSSchema       *pSchStr;
PtrSSchema       *pschcont;
int        *typecontenu;

#endif /* __STDC__ */

{
   int                 i;
   boolean             ok;
   SRule              *pRe1;

   if (*typecontenu != 0 && *pschcont != NULL)
     {
	ok = FALSE;
	if ((*pschcont)->SsCode == (*pSchStr)->SsCode)
	   if (*TypeElement == *typecontenu)
	      /* meme numero de type */
	      ok = TRUE;
	   else
	     {
		pRe1 = &(*pschcont)->SsRule[*typecontenu - 1];
		if (pRe1->SrConstruct == CsChoice)
		   /* le contenu a creer est un choix */
		   /* TODO: on ignore UNIT et NATURE, on peut faire mieux */
		   if (pRe1->SrNChoices > 0)
		      /* on compare le type de l'element courant */
		      /* avec toutes les options du choix */
		      /* choix explicite */
		     {
			i = 0;
			do
			  {
			     i++;
			     ok = pRe1->SrChoice[i - 1] == *TypeElement;
			  }
			while (!(ok || i >= pRe1->SrNChoices));
		     }
	     }
	if (!ok)
	   if (*TypeElement == (*pSchStr)->SsRootElem)
	     {
		pRe1 = &(*pschcont)->SsRule[*typecontenu - 1];
		if (pRe1->SrConstruct == CsNatureSchema)
		  {
		     /* le contenu cherche' est justement une racine de nature */
		     if (pRe1->SrSSchemaNat != NULL)
			ok = pRe1->SrSSchemaNat->SsCode == (*pSchStr)->SsCode;
		  }
		else if (pRe1->SrConstruct == CsChoice)
		   /* le contenu cherche' est un choix. Y a-t-il, parmi les */
		   /* options de ce choix, la nature dont l'element courant est */
		   /* racine? */
		   if (pRe1->SrNChoices > 0)
		      /* choix explicite */
		     {
			i = 0;
			do
			  {
			     i++;
			     if ((*pschcont)->SsRule[pRe1->SrChoice[i - 1] - 1].SrConstruct == CsNatureSchema)
				/* l'option i est un changement de nature */
				if ((*pschcont)->SsRule[pRe1->SrChoice[i - 1] - 1].SrSSchemaNat != NULL)
				   ok = ((*pschcont)->SsRule[pRe1->SrChoice[i - 1] - 1].SrSSchemaNat->SsCode == (*pSchStr)->SsCode);
			  }
			while (!(ok || i >= pRe1->SrNChoices));
		     }
	     }
	if (ok)
	  {
	     *creetout = TRUE;	/* on cree toute la descendance de l'element */
	     *typecontenu = 0;	/* on ne creera plus de contenu pour cet element */
	  }
     }
}

 /* le code qui suit vient de la procedure VerifAttrRequis */
/* ---------------------------------------------------------------------- */
/* |    VerifAttrRequis1Regle verifie que l'element pointe' par pEl     | */
/* |    possede les attributs requis indique's dans la regle pRegle du  | */
/* |    schema de structure pSS et, si certains attributs requis        | */
/* |    manquent, affiche un message d'erreur.                          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         VerifAttrRequis1Regle (PtrElement pEl, SRule * pRegle, PtrSSchema pSS)

#else  /* __STDC__ */
static void         VerifAttrRequis1Regle (pEl, pRegle, pSS)
PtrElement          pEl;
SRule              *pRegle;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
  PtrAttribute         pAttr;
  int                 i, att;
  boolean             trouve;

  /* parcourt tous les attributs locaux definis dans la regle */
  for (i = 1; i <= pRegle->SrNLocalAttrs; i++)
    if (pRegle->SrRequiredAttr[i - 1])
      /* cet attribut local est requis */
      {
	att = pRegle->SrLocalAttr[i - 1];
	/* cherche si l'element possede cet attribut */
	pAttr = pEl->ElFirstAttr;
	trouve = FALSE;
	while (pAttr != NULL && !trouve)
	  if (pAttr->AeAttrNum == att &&
	      pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
	    trouve = TRUE;
	  else
	    pAttr = pAttr->AeNext;
	if (!trouve)
	  /* l'element ne possede pas cet attribut requis */
	  TtaDisplayMessage (INFO, TtaGetMessage (LIB, LIB_ATTR_REQUIRED_FOR_ELEM), pSS->SsAttribute[att - 1].AttrName, pRegle->SrName);
      }
}


/* ---------------------------------------------------------------------- */
/* |    VerifAttrRequis verifie que l'element pointe' par pEl possede   | */
/* |            les attributs requis et, si certains attributs requis   | */
/* |            manquent, affiche un message d'erreur.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         VerifAttrRequis (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static void         VerifAttrRequis (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */
{

   SRule              *pRegle;
   PtrSSchema        pSS;

   if (pEl != NULL)
     {
	/* traite d'abord les attributs requis par la regle de structure */
	/* qui definit l'element */
	pSS = pEl->ElSructSchema;
	pRegle = &pSS->SsRule[pEl->ElTypeNumber - 1];
	VerifAttrRequis1Regle (pEl, pRegle, pSS);
	/* traite les attributs requis par toutes les regles d'extension de */
	/* ce type d'element */
	pSS = pDoc->DocSSchema;
	if (pSS != NULL)
	  {
	     pSS = pSS->SsNextExtens;
	     /* parcourt tous les schemas d'extension du document */
	     while (pSS != NULL)
	       {
		  /* cherche dans ce schema d'extension la regle qui concerne */
		  /* le type de l'element */
		  pRegle = ExtensionRule (pEl->ElSructSchema, pEl->ElTypeNumber, pSS);
		  if (pRegle != NULL)
		     /* il y a une regle d'extension, on la traite */
		     VerifAttrRequis1Regle (pEl, pRegle, pSS);
		  /* passe au schema d'extension suivant */
		  pSS = pSS->SsNextExtens;
	       }
	  }

     }
}


/* ---------------------------------------------------------------------- */
/* |    ReadAttribut lit dans le fichier fichpiv un attribut qui est    | */
/* |    sous forme pivot. Le fichier doit etre positionne' juste apres  | */
/* |    la Marque-TtAttribute (qui a deja ete lue). Au retour, le fichier  | */
/* |    est positionne' sur le premier octet qui suit l'attribut        | */
/* |    (prochain octet qui sera lu).                                   | */
/* |    Si cree est faux, rien n'est cree', l'attribut est simplement   | */
/* |    saute' dans le fichier.                                         | */
/* |    Si cree est vrai, un attribut est cree' et  est retourne'       | */
/* |    dans pAttrLu.                                                   | */
/* |    ATTENTION: ReadAttribut utilise la table des natures du document| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ReadAttribut (BinFile fichpiv, PtrElement pEl, PtrDocument pDoc, boolean cree, PtrAttribute * pAttrLu, PtrAttribute * pAttr)

#else  /* __STDC__ */
void                ReadAttribut (fichpiv, pEl, pDoc, cree, pAttrLu, pAttr)
BinFile             fichpiv;
PtrElement          pEl;
PtrDocument         pDoc;
boolean             cree;
PtrAttribute        *pAttrLu;
PtrAttribute        *pAttr;

#endif /* __STDC__ */

{
   PtrSSchema        pSchAttr;
   int                 n;
   int                 attr;
   int                 val;
   boolean             signe;
   ReferenceType           TRef;
   boolean             RExt;
   DocumentIdentifier     I;
   LabelString         lab;
   PtrTextBuffer      pBT, pPremBuff;
   char                c;
   PtrAttribute         pA;
   PtrReference        pRf;
   boolean             trouve;
   boolean             stop;

   *pAttrLu = NULL;
   pSchAttr = NULL;
   pA = NULL;
   signe = FALSE;
   /* lit le numero du schema de structure definissant l'attribut */
   BIOreadShort (fichpiv, &n);
   if (n < 0 || n >= pDoc->DocNNatures)
     {
	PivotFormatError ("Nature Num GetAttributeOfElement ");
	PivotError (fichpiv);
     }
   else
      pSchAttr = pDoc->DocNatureSSchema[n];
   if (pSchAttr == NULL)
     {
	PivotFormatError ("Nature GetAttributeOfElement");
	PivotError (fichpiv);
     }
   BIOreadShort (fichpiv, &attr);	/* lit l'attribut */
   if (pDoc->DocPivotVersion < 4)
      /* on tient compte de l'ajout de l'attribut Langue */
      attr++;
   /* lit le contenu de l'attribut selon son type */
   if (!error)
      switch (pSchAttr->SsAttribute[attr - 1].AttrType)
	    {
	       case AtEnumAttr:
		  BIOreadShort (fichpiv, &val);
		  if (val > pSchAttr->SsAttribute[attr - 1].AttrNEnumValues)
		    {
		       printf ("Attribute value error: %s = %d\n", pSchAttr->SsAttribute[attr - 1].AttrOrigName, val);
		       cree = FALSE;
		    }
		  break;
	       case AtNumAttr:
		  BIOreadShort (fichpiv, &val);
		  signe = rdSigne (fichpiv);
		  break;
	       case AtReferenceAttr:
		  rdReference (&TRef, lab, &RExt, &I, fichpiv);
		  break;
	       case AtTextAttr:
		  if (!cree)
		     /* on consomme le texte de l'attribut, sans le garder */
		     do
			if (!BIOreadByte (fichpiv, &c))
			  {
			     PivotError (fichpiv);
			     PivotFormatError ("A");
			  }
		     while (!(error || c == '\0')) ;
		  else
		    {
		       /* acquiert un premier buffer de texte */
		       GetBufTexte (&pPremBuff);
		       pBT = pPremBuff;
		       /* lit tout le texte de l'attribut */
		       stop = FALSE;
		       do
			  if (!BIOreadByte (fichpiv, &pBT->BuContent[pBT->BuLength++]))
			     /* erreur de lecture */
			    {
			       PivotError (fichpiv);
			       PivotFormatError ("A");
			    }
			  else
			     /* on a lu correctement un caractere */
			  if (pBT->BuContent[pBT->BuLength - 1] == '\0')
			     /* c'est la fin du texte de l'attribut */
			     stop = TRUE;
			  else
			     /* ce n'est pas la fin du texte de l'attribut */
			  if (pBT->BuLength >= MAX_CHAR - 1)
			     /* le buffer courant est plein */
			    {
			       pBT->BuContent[pBT->BuLength] = '\0';	/* fin du buffer */
			       /* acquiert un nouveau buffer */
			       pBT = NewTextBuffer (pBT);
			    }
		       while (!(error || stop)) ;
		       pBT->BuLength--;
		    }
		  break;
	    }
   if (error)
      *pAttr = NULL;
   else
     {
	if (pDoc->DocPivotVersion < 4)
	   /* ignore les attributs definis dans les anciennes extensions */
	   /* ExtCorr et ExtMot */
	   if (pSchAttr->SsExtension)
	      if (strcmp (pSchAttr->SsName, "ExtCorr") == 0)
		 cree = FALSE;
	      else if (strcmp (pSchAttr->SsName, "ExtMot") == 0)
		 cree = FALSE;
	if (!cree)
	   *pAttr = NULL;
	else
	  {
	     /* Si c'est un attribut impose', a valeur fixe ou modifiable, */
	     /* il a deja ete cree' par NewSubtree. On cherche si l'element */
	     /* possede deja cet attribut */
	     trouve = FALSE;
	     if (pEl != NULL)
	       {
		  pA = pEl->ElFirstAttr;
		  while (pA != NULL && !trouve)
		    {
		       if (pA->AeAttrSSchema == pSchAttr)
			  if (pA->AeAttrNum == attr)
			     trouve = TRUE;
		       if (!trouve)
			  pA = pA->AeNext;
		    }
	       }
	     if (!trouve)
	       {
		  /* acquiert un bloc attribut pour l'element */
		  GetAttr (pAttr);
		  /* remplit ce bloc attribut avec ce qu'on vient de lire */
		  pA = *pAttr;
		  pA->AeAttrSSchema = pSchAttr;
		  pA->AeAttrNum = attr;
		  pA->AeDefAttr = FALSE;
		  /* prend le type de l'attribut dans le schema de structure */
		  pA->AeAttrType = pA->AeAttrSSchema->SsAttribute[pA->AeAttrNum - 1].AttrType;
	       }
	     else
		*pAttr = NULL;
	     *pAttrLu = pA;
	     /* lit la valeur de l'attribut selon son type */
	     switch (pA->AeAttrType)
		   {
		      case AtEnumAttr:
			 pA->AeAttrValue = val;
			 break;
		      case AtNumAttr:
			 pA->AeAttrValue = val;
			 if (!signe)
			    pA->AeAttrValue = -pA->AeAttrValue;
			 break;
		      case AtReferenceAttr:
			 pA->AeAttrReference = NULL;
			 /* acquiert une reference et l'initialise */
			 GetReference (&pRf);
			 pA->AeAttrReference = pRf;
			 pRf->RdElement = pEl;
			 pRf->RdAttribute = pA;
			 /* lie la reference a l'objet qu'elle designe */
			 CreeReference (pA->AeAttrReference, TRef, lab, RExt, I, pDoc);
			 break;
		      case AtTextAttr:
			 pA->AeAttrText = pPremBuff;
			 break;
		      default:
			 break;
		   }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    rdAttribut lit dans le fichier fichpiv un attribut qui est      | */
/* |    sous forme pivot. Le fichier doit etre positionne' juste apres  | */
/* |    la Marque-TtAttribute (qui a deja ete lue). Au retour, le fichier  | */
/* |    est positionne' sur le premier octet qui suit l'attribut        | */
/* |    (prochain octet qui sera lu).                                   | */
/* |    Si cree est faux, rien n'est cree' dans l'arbre abstrait,       | */
/* |    l'attribut est simplement saute' dans le fichier.               | */
/* |    Si cree est vrai, un attribut est cree' et attache' a l'element | */
/* |    pointe' par pEl; un pointeur sur cet attribut est retourne'     | */
/* |    dans pAttrLu.                                                   | */
/* |    ATTENTION: rdAttribut utilise la table des natures du document  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         rdAttribut (BinFile fichpiv, PtrElement pEl, PtrDocument pDoc, boolean cree, PtrAttribute * pAttrLu)

#else  /* __STDC__ */
static void         rdAttribut (fichpiv, pEl, pDoc, cree, pAttrLu)
BinFile             fichpiv;
PtrElement          pEl;
PtrDocument         pDoc;
boolean             cree;
PtrAttribute        *pAttrLu;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;
   PtrAttribute         pA;

   ReadAttribut (fichpiv, pEl, pDoc, cree, pAttrLu, &pAttr);
   if (pAttr != NULL)
      /* chaine ce bloc en queue de la chaine des attributs de */
      /* l'element */
     {
	if (pEl->ElFirstAttr == NULL)
	   /* pas encore d'attributs pour l'element */
	   pEl->ElFirstAttr = pAttr;
	else
	  {
	     pA = pEl->ElFirstAttr;
	     /* 1er attribut de l'element */
	     while (pA->AeNext != NULL)
		/* cherche le dernier attribut */
		pA = pA->AeNext;
	     pA->AeNext = pAttr;
	     /* chaine le nouvel attribut */
	  }
	pAttr->AeNext = NULL;	/* c'est le dernier attribut */
     }
}


/* ---------------------------------------------------------------------- */
/* |    rdReglePres lit dans le fichier fichpiv une regle de            | */
/* |    presentation qui est sous forme pivot. Le fichier doit etre     | */
/* |    positionne' juste apres la Marque-Presentation (qui a deja ete  | */
/* |    lue). Au retour, le fichier est positionne' sur le premier      | */
/* |    octet qui suit la regle de presentation lue (prochain octet qui | */
/* |    sera lu).                                                       | */
/* |    Si cree est faux, rien n'est cree' dans l'arbre abstrait, la    | */
/* |    regle de presentation est simplement saute'e dans le fichier.   | */
/* |    Si cree est vrai, une regle est cree'e et attache'e a l'element | */
/* |    pointe' par pEl; un pointeur sur cette regle est retourne'      | */
/* |    dans pRegleLue.                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                rdReglePres (PtrDocument pDoc, BinFile fichpiv, PtrElement pEl, boolean cree, PtrPRule * pRegleLue, boolean Attache)
#else  /* __STDC__ */
void                rdReglePres (pDoc, fichpiv, pEl, cree, pRegleLue, Attache)
PtrDocument         pDoc;
BinFile             fichpiv;
PtrElement          pEl;
boolean             cree;
PtrPRule       *pRegleLue;
boolean             Attache;

#endif /* __STDC__ */
{
   int                 Vue;
   int                 boite;
   char                ch;
   PRuleType           TypeRP;
   BAlignment             Cadre;
   boolean             dimabs;
   TypeUnit            unit;
   boolean             signe;
   boolean             just;
   int                 val;
   int                 typeimage;
   int                 PicXArea, PicYArea, PicWArea, PicHArea;
   PictureScaling           pres;
   PtrPRule        pR1;
   PtrPRule        pRegle;
   PtrPRule        pRegleP;
   PosRule           *pRegleP6;
   PtrPSchema          pSPR;
   PtrSSchema        pSSR;
   DimensionRule       *pRelD1;
   PtrAttribute         pAttr;

   pres = (PictureScaling) 0;
   typeimage = 0;
   just = FALSE;
   signe = FALSE;
   dimabs = FALSE;
   Cadre = (BAlignment) 0;
   TypeRP = (PRuleType) 0;
   *pRegleLue = NULL;
   unit = UnRelative;
   /* lit le numero de vue */
   BIOreadShort (fichpiv, &Vue);
   /* lit le numero de la boite de present. concernee par la regle */
   /* TODO a modifier lorsque les boites de presentation pourront */
   /* porter des regles specifiques */
   BIOreadShort (fichpiv, &boite);
   /* lit le type de la regle */
   if (!BIOreadByte (fichpiv, &ch))
      PivotError (fichpiv);
   switch (ch)
	 {
	    case C_PR_ADJUST:
	       TypeRP = PtAdjust;
	       break;
	    case C_PR_HEIGHT:
	       TypeRP = PtHeight;
	       break;
	    case C_PR_WIDTH:
	       TypeRP = PtWidth;
	       break;
	    case C_PR_VPOS:
	       TypeRP = PtVertPos;
	       break;
	    case C_PR_HPOS:
	       TypeRP = PtHorizPos;
	       break;
	    case C_PR_SIZE:
	       TypeRP = PtSize;
	       break;
	    case C_PR_STYLE:
	       TypeRP = PtStyle;
	       break;
	    case C_PR_FONT:
	       TypeRP = PtFont;
	       break;
	    case C_PR_UNDERLINE:
	       TypeRP = PtUnderline;
	       break;
	    case C_PR_UNDER_THICK:
	       TypeRP = PtThickness;
	       break;
	    case C_PR_BREAK1:
	       TypeRP = PtBreak1;
	       break;
	    case C_PR_BREAK2:
	       TypeRP = PtBreak2;
	       break;
	    case C_PR_PICTURE:
	       TypeRP = PtPictInfo;
	       break;
	    case C_PR_INDENT:
	       TypeRP = PtIndent;
	       break;
	    case C_PR_LINESPACING:
	       TypeRP = PtLineSpacing;
	       break;
	    case C_PR_JUSTIFY:
	       TypeRP = PtJustify;
	       break;
	    case C_PR_HYPHENATE:
	       TypeRP = PtHyphenate;
	       break;
	    case C_PR_LINESTYLE:
	       TypeRP = PtLineStyle;
	       break;
	    case C_PR_LINEWEIGHT:
	       TypeRP = PtLineWeight;
	       break;
	    case C_PR_FILLPATTERN:
	       TypeRP = PtFillPattern;
	       break;
	    case C_PR_BACKGROUND:
	       TypeRP = PtBackground;
	       break;
	    case C_PR_FOREGROUND:
	       TypeRP = PtForeground;
	       break;
	    default:
	       PivotError (fichpiv);
	       PivotFormatError ("p");
	       break;
	 }

   if (!error)
      /* lit les parametres de la regle selon son type */
      switch (TypeRP)
	    {
	       case PtAdjust:
		  Cadre = rdCadr (fichpiv);
		  break;
	       case PtHeight:
	       case PtWidth:
		  dimabs = rdTypeDim (fichpiv);
		  BIOreadShort (fichpiv, &val);
		  unit = rdUnit (fichpiv);
		  signe = rdSigne (fichpiv);
		  break;
	       case PtVertPos:
	       case PtHorizPos:
		  BIOreadShort (fichpiv, &val);
		  unit = rdUnit (fichpiv);
		  signe = rdSigne (fichpiv);
		  break;
	       case PtBreak1:
	       case PtBreak2:
	       case PtIndent:
	       case PtSize:
	       case PtLineSpacing:
	       case PtLineWeight:
		  BIOreadShort (fichpiv, &val);
		  unit = rdUnit (fichpiv);
		  if (TypeRP == PtIndent)
		     signe = rdSigne (fichpiv);
		  break;
	       case PtFillPattern:
	       case PtBackground:
	       case PtForeground:
		  BIOreadShort (fichpiv, &val);
		  break;
	       case PtFont:
	       case PtStyle:
	       case PtUnderline:
	       case PtThickness:
	       case PtLineStyle:
		  if (!BIOreadByte (fichpiv, &ch))
		     PivotError (fichpiv);
		  break;
	       case PtJustify:
	       case PtHyphenate:
		  just = rdBooleen (fichpiv);
		  break;
	       case PtPictInfo:
		  rdCroppingFrame (fichpiv, &PicXArea, &PicYArea, &PicWArea, &PicHArea);
		  pres = rdImagePresentation (fichpiv);
		  typeimage = rdImageType (fichpiv);
		  break;
	       default:
		  break;
	    }

   if (cree)
     {
	GetReglePres (&pRegle);	/* acquiert une regle */
	*pRegleLue = pRegle;
	/* initialise d'abord la nouvelle regle */
	pRegle->PrPresMode = PresImmediate;
	pRegle->PrSpecifAttr = 0;
	pRegle->PrSpecifAttrSSchema = NULL;
	pAttr = NULL;
	pRegle->PrViewNum = Vue;
	pRegle->PrType = TypeRP;
	/* charge les parametres de la regle selon son type */
	if (!error)
	   switch (pRegle->PrType)
		 {
		    case PtAdjust:
		       pRegle->PrAdjust = Cadre;
		       break;
		    case PtHeight:
		    case PtWidth:
		       pRelD1 = &pRegle->PrDimRule;
		       pRelD1->DrPosition = FALSE;
		       pRelD1->DrAbsolute = dimabs;
		       if (!pRelD1->DrAbsolute)
			  /* c'est une dimension relative, on prend */
			  /* la regle qui devrait s'appliquer a     */
			  /* l'element, puis on la modifie selon    */
			  /* ce qui est lu dans le fichier          */
			 {
			    pR1 = ReglePEl (pEl, &pSPR, &pSSR, 0, NULL, pRegle->PrViewNum,
				       pRegle->PrType, FALSE, TRUE, &pAttr);
			    if (pR1 != NULL)

			      {
				 *pRegle = *pR1;
				 pRegle->PrViewNum = Vue;
				 pRegle->PrNextPRule = NULL;
				 pRegle->PrCond = NULL;
			      }
			 }
		       pRelD1->DrAttr = FALSE;
		       pRelD1->DrValue = val;
		       pRelD1->DrUnit = unit;
		       if (!signe)
			  pRelD1->DrValue = -pRelD1->DrValue;
		       pRelD1->DrMin = FALSE;
		       break;
		    case PtVertPos:
		    case PtHorizPos:
		       pRegleP6 = &pRegle->PrPosRule;
		       /* c'est une position relative, on prend */
		       /* la regle qui devrait s'appliquer a    */
		       /* l'element, puis on la modifie selon   */
		       /* ce qui est lu dans le fichier         */
		       pR1 = ReglePEl (pEl, &pSPR, &pSSR, 0, NULL, pRegle->PrViewNum,
				       pRegle->PrType, FALSE, TRUE, &pAttr);
		       if (pR1 != NULL)
			  *pRegle = *pR1;
		       pRegle->PrViewNum = Vue;
		       pRegle->PrNextPRule = NULL;
		       pRegle->PrCond = NULL;
		       pRegleP6->PoDistAttr = FALSE;
		       pRegleP6->PoDistance = val;
		       pRegleP6->PoDistUnit = unit;
		       if (!signe)
			  pRegleP6->PoDistance = -pRegleP6->PoDistance;
		       break;
		    case PtBreak1:
		    case PtBreak2:
		    case PtIndent:
		    case PtSize:
		    case PtLineSpacing:
		    case PtLineWeight:
		       pRegle->PrMinAttr = FALSE;
		       pRegle->PrMinValue = val;
		       pRegle->PrMinUnit = unit;
		       if (pRegle->PrType == PtIndent)
			  if (!signe)
			     pRegle->PrMinValue = -pRegle->PrMinValue;
		       break;
		    case PtFillPattern:
		       pRegle->PrAttrValue = FALSE;
		       pRegle->PrIntValue = val;
		       break;
		    case PtBackground:
		    case PtForeground:
		       pRegle->PrAttrValue = FALSE;
		       /* convertit les couleurs des anciennes versions */
		       if (pDoc->DocPivotVersion < 4)
			  val = newColor[val];
		       pRegle->PrIntValue = val;
		       break;
		    case PtFont:
		    case PtStyle:
		    case PtUnderline:
		    case PtThickness:
		    case PtLineStyle:
		       pRegle->PrChrValue = ch;
		       break;
		    case PtJustify:
		    case PtHyphenate:
		       pRegle->PrJustify = just;
		       break;
		    case PtPictInfo:
		       pRegle->PrPictInfo.PicXArea = PicXArea;
		       pRegle->PrPictInfo.PicYArea = PicYArea;
		       pRegle->PrPictInfo.PicWArea = PicWArea;
		       pRegle->PrPictInfo.PicHArea = PicHArea;
		       pRegle->PrPictInfo.PicPresent = pres;
		       pRegle->PrPictInfo.PicType = typeimage;
		       break;
		    default:
		       break;
		 }
	/* si la regle copiee est associee a un attribut, garde le lien */
	/* avec cet attribut */
	if (pAttr != NULL)
	  {
	     pRegle->PrSpecifAttr = pAttr->AeAttrNum;
	     pRegle->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
	  }

	if (Attache)
	   /* chaine la nouvelle regle de presentation en queue de la */
	   /* chaine des regles de presentation de l'element */
	   if (pEl->ElFirstPRule == NULL)
	      /* pas encore de regle de presentation pour l'element */
	      pEl->ElFirstPRule = pRegle;
	   else
	     {
		/* 1ere regle de presentation de l'element */
		pRegleP = pEl->ElFirstPRule;
		/* cherche la derniere regle de l'element */
		while (pRegleP->PrNextPRule != NULL)
		   pRegleP = pRegleP->PrNextPRule;
		/* chaine la nouvelle regle */
		pRegleP->PrNextPRule = pRegle;
	     }
	pRegle->PrNextPRule = NULL;	/* c'est la derniere regle */
     }				/* end if cree */
}

	/* debut ajout */
/* ---------------------------------------------------------------------- */
/* |    SendEventAttrRead       envoie les evenements TteAttrRead       | */
/* |            pour les attributs de l'element pEl qui vient d'etre lu | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         SendEventAttrRead (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SendEventAttrRead (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;
   NotifyAttribute     notifyAttr;

   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)
     {
	/* prepare et envoie le message AttrRead.Pre s'il est demande' */
	notifyAttr.event = TteAttrRead;
	notifyAttr.document = (Document) IdentDocument (pDoc);
	notifyAttr.element = (Element) pEl;
	notifyAttr.attribute = NULL;
	notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
	notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
	if (SendAttributeMessage (&notifyAttr, TRUE))
	   /* l'application ne veut pas lire l'attribut */
	   /* on l'avait deja lu, on le retire */
	   DeleteAttribute (pEl, pAttr);
	else
	  {
	     /* prepare et envoie le message AttrRead.Post s'il est demande' */
	     notifyAttr.event = TteAttrRead;
	     notifyAttr.document = (Document) IdentDocument (pDoc);
	     notifyAttr.element = (Element) pEl;
	     notifyAttr.attribute = (Attribute) pAttr;
	     notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
	     notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
	     SendAttributeMessage (&notifyAttr, FALSE);
	  }
	/* passe a l'attribut suivant de l'element */
	pAttr = pAttr->AeNext;
     }
}
	/* fin ajout */

/* ---------------------------------------------------------------------- */
/* |    Internalise effectue la traduction de la forme pivot commencant | */
/* |            a` la position courante dans fichpiv vers la            | */
/* |            representation interne. Au retour le fichier est        | */
/* |            positionne' apres la partie traduite. Le fichier doit   | */
/* |            etre ouvert avant l'appel et il reste ouvert au retour. | */
/* |            Le fichier doit etre positionne' sur un numero de type  | */
/* |            ou un nom de nature.                                    | */
/* |            - pSchStr: pointeur sur le schema de structure courant. | */
/* |            - pDoc:    pointeur sur le contexte du document en      | */
/* |            cours de lecture.                                       | */
/* |            - marque:  marque precedent le numero de type ou le nom | */
/* |            de nature. Au retour: 1er octet suivant l'element.      | */
/* |            - NAssoc:  numero de la liste d'elements associes a`    | */
/* |            laquelle appartient le texte a` internaliser. Zero si   | */
/* |            c'est l'arbre principal.                                | */
/* |            - creepar: indique s'il faut creer un parametre ou non. | */
/* |            - creetout:indique qu'il faut creer tous les elements   | */
/* |            qui descendent de l'element courant et qui sont dans le | */
/* |            fichier. Si creetout est faux, on ne cree que les       | */
/* |            elements exportes et dans ceux-ci tout le sous-arbre    | */
/* |            des elements de type typecontenu definis dans le schema | */
/* |            de structure pointe' par pschcont.                      | */
/* |            - TypeLu:  au retour, indique le type de l'element lu,  | */
/* |            qu'il ait ete cree' ou pas.                             | */
/* |            - SchStrLu: au retour, pointeur sur le schema de        | */
/* |            structure de l'element lu, qu'il ait ete cree' ou pas.  | */
/* |            - Pere: element qui sera le pere de l'element lu.       | */
/* |            - creedesc: si creedesc est faux, on ne cree pas        | */
/* |            l'element lu ni sa descendance. Prioritaire sur creetout| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrElement          Internalise (BinFile fichpiv, PtrSSchema pSchStr, PtrDocument pDoc, char *marque, int NAssoc, boolean creepar, boolean creetout, int * typecontenu, PtrSSchema * pschcont, int * TypeLu, PtrSSchema * SchStrLu, boolean creepage, PtrElement Pere, boolean creedesc)

#else  /* __STDC__ */
PtrElement          Internalise (fichpiv, pSchStr, pDoc, marque, NAssoc, creepar, creetout, typecontenu, pschcont, TypeLu, SchStrLu, creepage, Pere, creedesc)
BinFile             fichpiv;
PtrSSchema        pSchStr;
PtrDocument         pDoc;
char               *marque;
int                 NAssoc;
boolean             creepar;
boolean             creetout;
int        *typecontenu;
PtrSSchema       *pschcont;
int        *TypeLu;
PtrSSchema       *SchStrLu;
boolean             creepage;
PtrElement          Pere;
boolean             creedesc;

#endif /* __STDC__ */

{
   char                ch;
   int                 i, j;
   int                 Vue;
   int                 n;
   LabelString         lab;
   int                 typeimage;
   boolean             cree;
   boolean             Inclusion;
   boolean             modif;
   boolean             parametre;
   boolean             trouvetype;	/* on a reconnu le type d'une image dans un pivot v1 */
   boolean             RExt;
   PictureScaling           pres;
   DocumentIdentifier     I;
   int         TypeElement;
   int         nR;
   PtrAttribute         pAttr;
   PtrTextBuffer      b;
   PtrTextBuffer      pBComment;
   PtrElement          ElPreced;
   PtrElement          p;
   PtrElement          pEl;
   PtrElement          pEl2;
   PtrElement          pElInt;
   PtrElement          pElLu;
   PtrPRule        pRegle;
   PtrSSchema        pSS;
   SRule              *pRe1;
   BasicType          TFeuille;
   PageType            TPage;
   ReferenceType           TRef;
   boolean             AvecReferences;
   PtrReferredDescr    pDR;
   char                Alphabet;
   boolean             trouve;
   char                c;
   PtrElement          futurPere;
   NotifyElement       notifyEl;

   pRe1 = NULL;
   pEl = NULL;
   AvecReferences = FALSE;
   cree = FALSE;
   if (*marque != (char) C_PIV_TYPE && *marque != (char) C_PIV_NATURE)
     {
/*************/
	i = 1;
	while (!error && i < 200)
	  {
	     if (!BIOreadByte (fichpiv, &c))
		PivotError (fichpiv);
	     else
	       {
		  if (c < ' ')
		    {
		       printf ("^");
		       c = (char) (((int) c) + ((int) '@'));
		       i++;
		    }
		  printf ("%c", c);
		  i++;
	       }
	  }
	printf ("\n");
/*************/
	PivotFormatError ("I");	/* erreur */
	PivotError (fichpiv);
     }
   else
     {
	/* lit le type de l'element dans le fichier */
	TypeElement = ReadType (pDoc, &pSchStr, fichpiv, marque);
	if (!error)
	  {
	     *TypeLu = TypeElement;	/* numero de type de l'element en cours */
	     *SchStrLu = pSchStr;	/* schema de structure de l'element en cours */
	     if (!creetout)
		/* on ne cree que les elements de type exporte'. L'element en cours */
		/* de lecture est-il du type qui constitue le contenu de l'element */
		/* exporte' englobant ? */
		ContenuElExp (&creetout, &TypeElement, &pSchStr, pschcont, typecontenu);
	     pRe1 = &pSchStr->SsRule[TypeElement - 1];
	     if (creetout)
		cree = TRUE;
	     else
		/* on ne cree que les elements de type exporte' */
	       {
		  cree = FALSE;
		  if (pRe1->SrExportedElem)	/* l'element est d'un type exporte' */
		     if (pRe1->SrExportContent != 0)
			/* on veut creer au moins une partie de son contenu */
		       {
			  cree = TRUE;	/* on le cree */
			  *typecontenu = pRe1->SrExportContent;
			  /* il faudra creer son contenu */
			  /* cherche le schema de structure ou est */
			  /* defini son contenu */
			  if (pRe1->SrNatExpContent[0] == '\0')
			     /* meme schema de structure */
			     *pschcont = pSchStr;
			  else
			     /* cherche dans la table des natures du document */
			    {
			       i = 0;
			       do
				  i++;
			       while (!(pDoc->DocNatureName[i - 1] == pRe1->SrNatExpContent ||
					i == pDoc->DocNNatures));
			       if (pDoc->DocNatureName[i - 1] == pRe1->SrNatExpContent)
				  /* trouve' */
				  *pschcont = pDoc->DocNatureSSchema[i - 1];
			       else
				  /* la nature du contenu n'est pas chargee */
				 {
				    *pschcont = NULL;
				    *typecontenu = 0;
				    /* il faut peut-etre aussi creer */
				    /* tout le contenu de l'element */
				 }
			    }
			  ContenuElExp (&creetout, &TypeElement, &pSchStr, pschcont, typecontenu);
		       }
		  if (TypeElement == PageBreak + 1)
		     if (creepage)
		       {
			  cree = TRUE;	/* on le cree */
			  *typecontenu = TypeElement;
			  *pschcont = pSchStr;

			  /* il faudra creer son contenu */
			  /* cherche le schema de structure ou est */
			  /* defini son contenu */
			  ContenuElExp (&creetout, &TypeElement, &pSchStr, pschcont, typecontenu);
		       }
	       }
	     if (creedesc)
	       {
		  notifyEl.event = TteElemRead;
		  notifyEl.document = (Document) IdentDocument (pDoc);
		  notifyEl.element = (Element) Pere;
		  notifyEl.elementType.ElTypeNum = *TypeLu;
		  notifyEl.elementType.ElSSchema = (SSchema) (*SchStrLu);
		  notifyEl.position = 0;
		  if (ThotSendMessage ((NotifyEvent *) & notifyEl, TRUE))
		     /* l'application ne veut pas lire le sous-arbre */
		    {
		       cree = FALSE;
		       creedesc = FALSE;
		    }
	       }
	     /* on cree toujours la racine du document */
	     if (!cree)
		if (pSchStr == pDoc->DocSSchema)
		   if (TypeElement == pSchStr->SsRootElem)
		      /* c'est la racine, on cree */
		      cree = TRUE;
	     if (!cree)
		pEl = NULL;
	     else
		/* cree un element du type lu */
	       {
		  if (pSchStr->SsRule[TypeElement - 1].SrParamElem && creepar)
		     /* simule un element ordinaire, si c'est un parametre a creer */
		    {
		       pSchStr->SsRule[TypeElement - 1].SrParamElem = FALSE;
		       parametre = TRUE;
		    }
		  else
		     parametre = FALSE;
		  /* il ne faut pas que le label max. du document augmente */
		  pEl = NewSubtree (TypeElement, pSchStr, pDoc, NAssoc, FALSE, TRUE, FALSE, FALSE);
		  if (pEl != NULL)
		     pEl->ElLabel[0] = '\0';
		  if (parametre)
		     pSchStr->SsRule[TypeElement - 1].SrParamElem = TRUE;
	       }

	     if (!BIOreadByte (fichpiv, marque))
		PivotError (fichpiv);
	  }
	Inclusion = FALSE;	/* est-ce une reference a un element inclus? */
	if (!error && *marque == (char) C_PIV_INCLUDED)
	   /* oui, lit la reference */
	  {
	     Inclusion = TRUE;
	     rdReference (&TRef, lab, &RExt, &I, fichpiv);
	     if (cree)
	       {
		  GetReference (&pEl->ElSource);
		  pEl->ElSource->RdElement = pEl;
		  CreeReference (pEl->ElSource, TRef, lab, RExt, I, pDoc);
		  pEl->ElIsCopy = TRUE;

	       }
	     if (!BIOreadByte (fichpiv, marque))
		PivotError (fichpiv);
	  }

	/* lit la marque "Element-reference'" si elle est presente */
	if (!error)
	   if (*marque == (char) C_PIV_REFERRED)
	     {
		AvecReferences = TRUE;
		if (!BIOreadByte (fichpiv, marque))
		   PivotError (fichpiv);
	     }
	   else
	      AvecReferences = FALSE;
	/* traite le label s'il est present */
	lab[0] = '\0';
	if (!error)
	   if (*marque == (char) C_PIV_SHORT_LABEL || *marque == (char) C_PIV_LONG_LABEL ||
	       *marque == (char) C_PIV_LABEL)
	     {
		rdLabel (*marque, lab, fichpiv);
		/* lit la marque qui suit le label */
		if (!BIOreadByte (fichpiv, marque))
		   PivotError (fichpiv);
	     }
	if (!error && lab[0] != '\0' && cree)
	   /* l'element porte un label */
	  {
	     strncpy (pEl->ElLabel, lab, MAX_LABEL_LEN);
	     if (!AvecReferences)
		/* on verifie si cet element (ou plutot son label) est dans la */
		/* chaine des elements reference's de l'exterieur */
	       {
		  pDR = pDoc->DocLabels;
		  while (pDR != NULL && !AvecReferences)
		     if (strcmp (pDR->ReReferredLabel, lab) == 0)
			AvecReferences = TRUE;
		     else
			pDR = pDR->ReNext;
	       }
	     if (!error)
		if (pDoc->DocPivotVersion < 3 || AvecReferences)
		   /* on associe a l'element un descripteur d'element reference' */

		  {
		     NulIdentDoc (&I);
		     pEl->ElReferredDescr = GetElRefer (lab, I, pDoc);
		     if (pEl->ElReferredDescr->ReReferredElem != NULL)
			/* on a deja lu dans ce document un element */
			/* portant ce label, erreur */
		       {
			  pEl->ElReferredDescr = NULL;
			  PivotFormatError ("L");
		       }
		     else
			pEl->ElReferredDescr->ReReferredElem = pEl;
		  }
	  }

	/* lit la marque d'holophraste si elle est presente */
	if (!error && cree)
	   pEl->ElHolophrast = FALSE;
	if (*marque == (char) C_PIV_HOLOPHRAST && !error)
	  {
	     if (cree)
		pEl->ElHolophrast = TRUE;
	     /* lit l'octet qui suit */
	     if (!BIOreadByte (fichpiv, marque))
		PivotError (fichpiv);
	  }
	/* lit les attributs de l'element s'il y en a */
	while (*marque == (char) C_PIV_ATTR && !error)
	  {
	     rdAttribut (fichpiv, pEl, pDoc, cree, &pAttr);
	     if (!error)
		if (!BIOreadByte (fichpiv, marque))
		   PivotError (fichpiv);
	  }
	/* tous les attributs de l'element sont lus, on verifie qu'il ne */
	/* manque pas d'attributs locaux obligatoires pour l'element */
	if (!error && cree)
	   VerifAttrRequis (pEl, pDoc);

	/* lit les regles de presentation de l'element */
	/* etablit d'abord le chainage de l'element avec son pere pour que la */
	/* procedure ReglePEl appelee par rdReglePres puisse trouver les */
	/* regles de presentation heritees des attributs des ascendants */
	if (pEl != NULL)
	   pEl->ElParent = Pere;
	while (*marque == (char) C_PIV_PRESENT && !error)
	  {
	     rdReglePres (pDoc, fichpiv, pEl, cree, &pRegle, TRUE);
	     if (!error)
		/* lit l'octet qui suit la regle */
		if (!BIOreadByte (fichpiv, marque))
		   PivotError (fichpiv);
	  }
	/* lit le commentaire qui accompagne eventuellement l'element */
	if (!error)
	   if (*marque == (char) C_PIV_COMMENT || *marque == (char) C_PIV_OLD_COMMENT)
	     {
		pBComment = readComment (fichpiv, cree, (boolean) (*marque == (char) C_PIV_OLD_COMMENT));	/*  */
		if (cree)
		   pEl->ElComment = pBComment;
		/* lit l'octet suivant le commentaire */
		if (!BIOreadByte (fichpiv, marque))
		   PivotError (fichpiv);
	     }
	if (!error)
	   /* si l'element est une copie par inclusion, il n'a pas de contenu */
	   if (!Inclusion)
	      /* lit le contenu de l'element cree */
	      /* traitement specifique selon le constructeur de l'element */
	      switch (pSchStr->SsRule[TypeElement - 1].SrConstruct)
		    {
		       case CsReference:
			  if (*marque != (char) C_PIV_REFERENCE)
			    {
			       PivotError (fichpiv);
			       PivotFormatError ("R");	/* erreur */
			    }
			  else
			     /* traitement des references : on lit la reference */
			    {
			       rdReference (&TRef, lab, &RExt, &I, fichpiv);
			       if (cree)
				  CreeReference (pEl->ElReference, TRef, lab, RExt, I, pDoc);
			       if (!BIOreadByte (fichpiv, marque))
				  PivotError (fichpiv);
			    }
			  break;
		       case CsPairedElement:
			  if (*marque != (char) C_PIV_BEGIN)
			    {
			       PivotError (fichpiv);
			       PivotFormatError ("M");	/* erreur, pas de marque debut */
			    }
			  else
			     /* traitement des paires : on lit l'identificateur */
			    {
			       BIOreadInteger (fichpiv, &i);
			       if (cree)
				  pEl->ElPairIdent = i;
			       if (i > pDoc->DocMaxPairIdent)
				  pDoc->DocMaxPairIdent = i;
			       if (!BIOreadByte (fichpiv, marque))
				  PivotError (fichpiv);
			       if (*marque != (char) C_PIV_END)
				  /* erreur, pas de marque de fin */
				 {
				    PivotError (fichpiv);
				    PivotFormatError ("m");
				 }
			       else if (!BIOreadByte (fichpiv, marque))
				  PivotError (fichpiv);
			    }
			  break;
		       case CsBasicElement:
			  TFeuille = pSchStr->SsRule[TypeElement - 1].SrBasicType;
			  if (TFeuille == CharString)
			     if (pDoc->DocPivotVersion >= 4)
			       {
				  if (*marque != (char) C_PIV_LANG)
				     /* pas de marque de langue, c'est la premiere langue de la */
				     /* table des langues du document */
				     i = 0;
				  else
				    {
				       /* lit le numero de langue (pour la table des langues du document */
				       if (!BIOreadByte (fichpiv, marque))
					  PivotError (fichpiv);
				       else
					  i = (int) (*marque);
				       /* lit l'octet suivant */
				       if (!BIOreadByte (fichpiv, marque))
					  PivotError (fichpiv);
				    }
				  if (cree && !error)
				    {
				       /* i est le rang de la langue dans la table des */
				       /* langues du document */
				       if (i < 0 || i >= pDoc->DocNLanguages)
					 {
					    PivotFormatError ("Invalid language");
					    pEl->ElLanguage = TtaGetDefaultLanguage ();
					 }
				       else
					  pEl->ElLanguage = pDoc->DocLanguages[i];
				    }
			       }
			     else
				/* version pivot < 4 */
			       {
				  /* alpabet par defaut = Latin */
				  Alphabet = 'L';
				  /* dans le cas d'une inclusion sans expansion, il */
				  /* n'y a pas d'alphabet. */
				  /* dans les versions pivot anciennes, il peut y avoir une */
				  /* marque d'alphabet. On la saute */
				  if (*marque != (char) C_PIV_BEGIN &&
				      *marque != (char) C_PIV_END &&
				      *marque != (char) C_PIV_TYPE &&
				      *marque != (char) C_PIV_NATURE)
				     /* on a lu l'alphabet */
				    {
				       Alphabet = *marque;
				       /* lit l'octet suivant */
				       if (!BIOreadByte (fichpiv, marque))
					  PivotError (fichpiv);
				    }
				  if (cree)
				    {
				       pEl->ElLanguage = TtaGetLanguageIdFromAlphabet (Alphabet);
				       /* verifie que la langue est dans la table des langues */
				       /* du document */
				       trouve = FALSE;
				       for (i = 0; i < pDoc->DocNLanguages && !trouve; i++)
					  if (pDoc->DocLanguages[i] == pEl->ElLanguage)
					     trouve = TRUE;
				       if (!trouve && pDoc->DocNLanguages < MAX_LANGUAGES_DOC)
					  /* elle n'y est pas, on la met */
					 {
					    pDoc->DocLanguages[pDoc->DocNLanguages] = pEl->ElLanguage;
					    pDoc->DocNLanguages++;
					 }
				    }
			       }

			  if (*marque == (char) C_PIV_BEGIN && !error)
			    {
			       if (TFeuille != PageBreak)
				  if (!BIOreadByte (fichpiv, marque))
				     PivotError (fichpiv);
			       if (*marque != (char) C_PIV_END)	/* il y a un contenu */
				 {
				    switch (TFeuille)
					  {
					     case CharString:
						if (!cree)
						   /* saute le texte de l'element */
						  {
						     ch = *marque;
						     while (ch != '\0' && !error)
							if (!BIOreadByte (fichpiv, &ch))
							   PivotError (fichpiv);
						  }
						else
						   /* lit le texte et remplit les buffers de texte    */
						  {
						     b = pEl->ElText;
						     n = 0;
						     pEl->ElTextLength = 0;
						     ch = *marque;
						     do
							if (ch != '\0')
							  {
							     if (n == MAX_CHAR - 1)
							       {
								  pEl->ElTextLength += n;
								  b->BuLength = n;
								  b->BuContent[n] = '\0';
								  b = NewTextBuffer (b);
								  n = 0;
							       }
							     n++;
							     /* mise a la norme iso des anciens pivots */
							     if (pDoc->DocPivotVersion < 3)
								if (((int) ch) >= 1 && ch < ' ')
								   switch (ch)
									 {
									    case '\021':
									       ch = '\040';
									       break;	/*space */
									    case '\030':
									       ch = '\230';
									       break;	/*oe */
									    case '\036':
									       ch = '\377';
									       break;	/*ydiaresis */
									    case '\037':
									       ch = '\351';
									       break;	/*eacute */
									    default:
									       ch = (char) (((int) ch) + 223);
									 }
							     /* changement des oe et OE */
							     if (pDoc->DocPivotVersion < 4)
								if (ch == '\230')
								   ch = '\367';
								else if (ch == '\367')
								   ch = '\230';
								else if (ch == '\231')
								   ch = '\327';
								else if (ch == '\327')
								   ch = '\231';
							     /* range le caractere et lit le suivant */
							     b->BuContent[n - 1] = ch;
							     if (!BIOreadByte (fichpiv, &ch))
								PivotError (fichpiv);
							  }
						     while (!(ch == '\0')) ;
						     pEl->ElTextLength += n;
						     b->BuLength = n;
						     b->BuContent[n] = '\0';
						     pEl->ElVolume = pEl->ElTextLength;
						  }
						if (!BIOreadByte (fichpiv, marque))
						   PivotError (fichpiv);
						break;
					     case Picture:
						if (!cree)
						   /* saute le texte de l'element */
						  {
						     ch = *marque;
						     while (ch != '\0')
							if (!BIOreadByte (fichpiv, &ch))
							   PivotError (fichpiv);
						  }
						else
						   /* lit le texte et remplit les buffers de texte    */
						  {
						     b = pEl->ElPictureName;
						     n = 0;
						     pEl->ElNameLength = 0;
						     pEl->ElImageDescriptor = NULL;
						     ch = *marque;
						     do
							if (ch != '\0')
							  {
							     /* TODO : nom d'image > MAX_CHAR */
							     if (n == MAX_CHAR - 1)
							       {
								  PivotError (fichpiv);
								  PivotFormatError ("x");
							       }
							     n++;
							     /* range le caractere et lit le suivant */
							     b->BuContent[n - 1] = ch;
							     if (!BIOreadByte (fichpiv, &ch))
								PivotError (fichpiv);
							  }
						     while (!(ch == '\0')) ;
						     /* on suppose que le nom tient en entier dans un buffer */
						     /* on normalize le nom */
						     strcpy (b->BuContent, NormalizeFileName (b->BuContent, &typeimage,
						       &pres, &trouvetype));
						     if (trouvetype)
						       {
							  /* on a trouve une image v1, on cree une regle */
							  /* de presentation PictInfo pour l'element */
							  CreateReglePres (pEl, typeimage, pres, Vue);
						       }
						     pEl->ElNameLength += n;
						     b->BuLength = n;
						     b->BuContent[n] = '\0';
						     pEl->ElVolume = pEl->ElNameLength;
						  }
						if (!BIOreadByte (fichpiv, marque))
						   PivotError (fichpiv);
						break;
					     case Symbol:
					     case GraphicElem:
						/* on a lu le code representant la forme */
						ch = *marque;
						/* lit l'octet qui suit */
						if (!BIOreadByte (fichpiv, marque))
						   PivotError (fichpiv);
						else if (*marque != (char) C_PIV_POLYLINE)
						   /* c'est un element graphique simple */
						  {
						     if (cree)
						       {
							  pEl->ElGraph = ch;
							  /* remplace les anciens rectangles trame's par */
							  /* de simple rectangles */
							  if (pEl->ElGraph >= '0' && pEl->ElGraph <= '9')
							     pEl->ElGraph = 'R';
							  else if (pEl->ElGraph >= '\260' && pEl->ElGraph <= '\270')
							     pEl->ElGraph = 'R';
							  if (ch == '\0')
							     pEl->ElVolume = 0;
							  else
							     pEl->ElVolume = 1;
						       }
						  }
						else
						   /* c'est une Polyline */
						  {
						     /* lit le nombre de points de la ligne */
						     if (!BIOreadShort (fichpiv, &n))
							PivotError (fichpiv);
						     /* lit tous les points */
						     else if (!cree)
							for (i = 0; i < n; i++)
							   BIOreadInteger (fichpiv, &j);
						     else
						       {
							  /* transforme l'element graphique simple en Polyline */
							  pEl->ElLeafType = LtPlyLine;
							  GetBufTexte (&pEl->ElPolyLineBuffer);
							  pEl->ElVolume = n;
							  pEl->ElPolyLineType = ch;
							  pEl->ElNPoints = n;
							  b = pEl->ElPolyLineBuffer;
							  j = 0;
							  for (i = 0; i < n; i++)
							    {
							       if (j >= MAX_POINT_POLY)
								  /* buffer courant plein */
								 {
								    b = NewTextBuffer (b);
								    j = 0;
								 }
							       BIOreadInteger (fichpiv, &b->BuPoints[j].XCoord);
							       BIOreadInteger (fichpiv, &b->BuPoints[j].YCoord);
							       b->BuLength++;
							       j++;
							    }
						       }
						     /* lit l'octet qui suit (marque de fin d'element) */
						     if (!BIOreadByte (fichpiv, marque))
							PivotError (fichpiv);
						  }
						break;
					     case PageBreak:
						/* lit le numero de page et */
						/* le type de page */
						BIOreadShort (fichpiv, &n);
						BIOreadShort (fichpiv, &Vue);
						TPage = rdTypePage (fichpiv);
						modif = rdBooleen (fichpiv);
						if (cree)
						  {
						     pEl->ElPageNumber = n;
						     pEl->ElViewPSchema = Vue;
						     pEl->ElPageType = TPage;
						     pEl->ElPageModified = modif;
						  }
						if (!BIOreadByte (fichpiv, marque))
						   PivotError (fichpiv);
						break;
					     default:
						break;
					  }

				 }
			       if (*marque != (char) C_PIV_END)
				 {
				    PivotError (fichpiv);
				    PivotFormatError ("F");
				 }

			       if (!BIOreadByte (fichpiv, marque))
				  PivotError (fichpiv);
			    }
			  break;
		       default:
			  /* traite le contenu s'il y en a un */
			  if (*marque == (char) C_PIV_BEGIN)
			    {
			       if (pEl != NULL)
				  if (pEl->ElTerminal)
				    {
				       PivotError (fichpiv);
				       PivotFormatError ("f");
				    }
			       /* erreur: feuille avec contenu */
			       if (!error)
				 {
				    if (!BIOreadByte (fichpiv, marque))
				       PivotError (fichpiv);
				    ElPreced = NULL;
				    while (*marque != (char) C_PIV_END && !error)
				       /* ce n'est pas un element vide, */
				       /* on lit son contenu */
				      {
					 if (ElPreced != NULL)
					    futurPere = ElPreced->ElParent;
					 else if (pEl != NULL)
					    futurPere = pEl;
					 else
					    futurPere = Pere;
					 p = Internalise (fichpiv, pSchStr, pDoc, marque, NAssoc,
							  creepar, creetout, typecontenu, pschcont, &nR,
							  &pSS, creepage, futurPere, creedesc);
					 pElLu = p;
					 if (!error)
					    if (p != NULL)
					      {
						 if (ElPreced != NULL)
						   {
						      if (pEl != NULL)
							 pEl->ElParent = NULL;
						      InsertElementAfter (ElPreced, p);
						      if (pEl != NULL)
							 pEl->ElParent = Pere;
						   }
						 else if (pEl != NULL)
						   {
						      if (!creetout)
							 if (p->ElTypeNumber != PageBreak + 1)
							    if (p->ElSructSchema != pEl->ElSructSchema)
							       /* l'element a inserer dans l'arbre appartient       */
							       /* a un schema different de celui de son pere        */
							       if (p->ElTypeNumber != p->ElSructSchema->SsRootElem)
								  /* ce n'est pas la racine d'une nature, on ajoute  */
								  /* un element intermediaire */
								 {
								    pEl2 = p;
								    /* il ne faut pas que le label */
								    /* max. du document augmente */
								    pElInt = NewSubtree (pEl2->ElSructSchema->SsRootElem,
											 pEl2->ElSructSchema,
											 pDoc, NAssoc, FALSE, TRUE, FALSE, FALSE);

								    pElInt->ElLabel[0] = '\0';
								    InsertFirstChild (pElInt, p);
								    p = pElInt;
								 }
						      pEl->ElParent = NULL;
						      InsertFirstChild (pEl, p);
						      pEl->ElParent = Pere;
						   }
						 else
						    pEl = p;

						 ElPreced = p;
						 SendEventAttrRead (pElLu, pDoc);
						 /* Si l'element qu'on vient de lire n'a pas ete      */
						 /* cree' (lecture squelette) mais que certains de    */
						 /* ses descendants l'ont ete,Internalise a retourne' */
						 /* un pointeur sur le premier descendant cree'. On   */
						 /* cherche le dernier frere, qui devient l'element   */
						 /* precedent du prochain element lu. */

						 while (ElPreced->ElNext != NULL)
						    ElPreced = ElPreced->ElNext;
					      }
				      }
				    if (!error)
				       if (!BIOreadByte (fichpiv, marque))
					  PivotError (fichpiv);
				 }
			    }
			  break;
		    }

     }
   if (!error)
     {
	if (creedesc && pEl != NULL)
	  {
	     notifyEl.event = TteElemRead;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElSructSchema);
	     notifyEl.position = 0;
	     ThotSendMessage ((NotifyEvent *) & notifyEl, FALSE);
	  }
	if (pEl != NULL)
	   pEl->ElParent = NULL;
	return pEl;
     }

   return NULL;
}

/* ---------------------------------------------------------------------- */
/* |    ArbreCorrect verifie que l'element pEl et tous ses descendants  | */
/* |    peuvent figurer a la place ou` ils sont dans leur arbre abstrait| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             ArbreCorrect (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
boolean             ArbreCorrect (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
  PtrElement          pFils;
  boolean             ok, filsOK;

  ok = TRUE;
  if (pEl != NULL)
    {
      if (pEl->ElPrevious != NULL)
	{
	  if (!AllowedSibling (pEl->ElPrevious, pDoc, pEl->ElTypeNumber,
			       pEl->ElSructSchema, FALSE, FALSE, TRUE))
	    {
	      ok = FALSE;
	      TtaDisplayMessage (INFO,  TtaGetMessage (LIB, LIB_X_INVALID_SIBLING_FOR_Y),
				 pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
				 pEl->ElPrevious->ElSructSchema->SsRule[pEl->ElPrevious->ElTypeNumber - 1].SrName,
				 pEl->ElLabel);
	    }
	}
      else if (pEl->ElParent != NULL)
	{
	  if (!AllowedFirstChild (pEl->ElParent, pDoc, pEl->ElTypeNumber,
				    pEl->ElSructSchema, FALSE, TRUE))
	    {
	      ok = FALSE;
	      TtaDisplayMessage (INFO,  TtaGetMessage (LIB, LIB_X_INVALID_CHILD_FOR_Y),
				 pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
				 pEl->ElParent->ElSructSchema->SsRule[pEl->ElParent->ElTypeNumber - 1].SrName,
				 pEl->ElLabel);
	    }
	}

      if (!pEl->ElTerminal)
	{
	  pFils = pEl->ElFirstChild;
	  while (pFils != NULL)
	    {
	      filsOK = ArbreCorrect (pFils, pDoc);
	      ok = ok && filsOK;
	      pFils = pFils->ElNext;
	    }
	}
    }
  return ok;
}


/* ---------------------------------------------------------------------- */
/* |    AccouplePaires  etablit les liens qui relient les elements de   | */
/* |    paires deux a deux dans tout l'arbre de racine pRacine.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                AccouplePaires (PtrElement pRacine)
#else  /* __STDC__ */
void                AccouplePaires (pRacine)
PtrElement          pRacine;
#endif /* __STDC__ */
{
   PtrElement          pEl1, pEl2;
   boolean             trouve;

   pEl1 = pRacine;
   /* cherche tous les elements produits par le constructeur CsPairedElement */
   while (pEl1 != NULL)
     {
	pEl1 = FwdSearchRefOrEmptyElem (pEl1, 3);
	if (pEl1 != NULL)
	   /* on a trouve' un element de paire */
	   if (pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrFirstOfPair)
	      /* c'est un element de debut de paire */
	     {
		/* on cherche l'element de fin correspondant */
		pEl2 = pEl1;
		trouve = FALSE;
		do
		  {
		     pEl2 = FwdSearchTypedElem (pEl2, pEl1->ElTypeNumber + 1, pEl1->ElSructSchema);
		     if (pEl2 != NULL)
			/* on a trouve' un element du type cherche' */
			/* c'est le bon s'il a le meme identificateur */
			trouve = (pEl2->ElPairIdent == pEl1->ElPairIdent);
		  }
		while ((pEl2 != NULL) && (!trouve));
		if (trouve)
		   /* On etablit le chainage entre les 2 elements */
		  {
		     pEl1->ElOtherPairedEl = pEl2;
		     pEl2->ElOtherPairedEl = pEl1;
		  }
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* | AffecteLabel       affecte un label a tous les elements du         | */
/* |    sous-arbre de racine pRac qui n'en ont pas.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         AffecteLabel (PtrElement pRac, PtrDocument pDoc)
#else  /* __STDC__ */
static void         AffecteLabel (pRac, pDoc)
PtrElement          pRac;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrElement          pFils;

   if (pRac != NULL)
     {
	if (pRac->ElLabel[0] == '\0')
	   /* l'element n'a pas de label, on lui en met un */
	  {
	     LabelIntToString (NewLabel (pDoc), pRac->ElLabel);
	  }
	if (!pRac->ElTerminal)
	   /* traite tous les fils de l'element */
	  {
	     pFils = pRac->ElFirstChild;
	     while (pFils != NULL)
	       {
		  AffecteLabel (pFils, pDoc);
		  pFils = pFils->ElNext;
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    NatureDansTable verifie si la nature de nom NomSchStr se trouve | */
/* |    dans la table des natures du document pDoc, au rang rangNature. | */
/* |    Si elle n'y est pas, on l'y met, soit en la deplacant, si elle  | */
/* |    figure deja dans la table, soit en creant une nouvelle entree   | */
/* |    au rang desire'.                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         NatureDansTable (PtrDocument pDoc, Name NomSchStr, int rangNature)
#else  /* __STDC__ */
static void         NatureDansTable (pDoc, NomSchStr, rangNature)
PtrDocument         pDoc;
Name                 NomSchStr;
int                 rangNature;
#endif /* __STDC__ */
{
   int                 i;
   boolean             found;
   Name                 N1, N2;
   PtrSSchema        pSS;

   if (rangNature > pDoc->DocNNatures + 1)
      /* le rang voulu pour la nature est invraissemblable */
      PivotFormatError ("Err nature ???");
   /* on cherche (par son nom) si la nature existe dans la table */
   i = 1;
   found = FALSE;
   while (i <= pDoc->DocNNatures && !found)
      if (strncmp (NomSchStr, pDoc->DocNatureName[i - 1], MAX_NAME_LENGTH) == 0)
	 found = TRUE;
      else
	 i++;
   if (found)
      /* la nature est deja dans la table */
     {
	/* si elle est au rang voulu, il n'y a rien a faire */
	if (i != rangNature)
	   /* elle n'est pas au rang voulu, on permute avec la nature qui */
	   /* y est */
	  {
	     pSS = pDoc->DocNatureSSchema[rangNature - 1];
	     strncpy (N1, pDoc->DocNatureName[rangNature - 1], MAX_NAME_LENGTH);
	     strncpy (N2, pDoc->DocNaturePresName[rangNature - 1], MAX_NAME_LENGTH);
	     pDoc->DocNatureSSchema[rangNature - 1] = pDoc->DocNatureSSchema[i - 1];
	     strncpy (pDoc->DocNatureName[rangNature - 1], pDoc->DocNatureName[i - 1], MAX_NAME_LENGTH);
	     strncpy (pDoc->DocNaturePresName[rangNature - 1], pDoc->DocNaturePresName[i - 1], MAX_NAME_LENGTH);
	     pDoc->DocNatureSSchema[i - 1] = pSS;
	     strncpy (pDoc->DocNatureName[i - 1], N1, MAX_NAME_LENGTH);
	     strncpy (pDoc->DocNaturePresName[i - 1], N2, MAX_NAME_LENGTH);
	  }
     }
   else
      /* la nature n'est pas dans la table, on l'y met */
     {
	/* si le rang voulu est a l'interieur de la table, on met d'abord */
	/* a la fin de la table la nature qui occupe le rang voulu */
	if (rangNature == pDoc->DocNNatures + 1)
	   pDoc->DocNNatures++;
	else
	  {
	     pDoc->DocNatureSSchema[pDoc->DocNNatures] = pDoc->DocNatureSSchema[rangNature - 1];
	     strncpy (pDoc->DocNatureName[pDoc->DocNNatures], pDoc->DocNatureName[rangNature - 1], MAX_NAME_LENGTH);
	     strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures], pDoc->DocNaturePresName[rangNature - 1], MAX_NAME_LENGTH);
	     pDoc->DocNNatures++;
	  }
	pDoc->DocNatureSSchema[rangNature - 1] = NULL;
	strncpy (pDoc->DocNatureName[rangNature - 1], NomSchStr, MAX_NAME_LENGTH);
     }
}


/* rdNomsSchemas lit les noms des schemas de structure et de presentation */
/* qui se trouvent dans le fichier fich et charge ces schemas */


#ifdef __STDC__
void                rdNomsSchemas (BinFile fich, PtrDocument pDoc, char *marque, PtrSSchema pSCharge)

#else  /* __STDC__ */
void                rdNomsSchemas (fich, pDoc, marque, pSCharge)
BinFile             fich;
PtrDocument         pDoc;
char               *marque;
PtrSSchema        pSCharge;

#endif /* __STDC__ */

{
   Name                 NomSchStr, NomSchPrs;
   PtrSSchema        pSS;
   int                 i, rangNature;
   boolean             ExtensionSch;
   int                 versionSchema;

   i = 0;
   rangNature = 1;
   /* lit le type du document */
   do
      if (!BIOreadByte (fich, &NomSchStr[i++]))
	 PivotError (fich);
   while (!(error || NomSchStr[i - 1] == '\0' || i == MAX_NAME_LENGTH)) ;
   if (NomSchStr[i - 1] != '\0')
     {
	PivotError (fich);
	PivotFormatError ("Z");
     }
   else
     {
	if (pDoc->DocPivotVersion >= 4)
	   /* Lit le code du schema de structure */
	   if (!error)
	      if (!BIOreadShort (fich, &versionSchema))
		 PivotError (fich);
	/* Lit le nom du schema de presentation associe' */
	i = 0;
	do
	   if (!BIOreadByte (fich, &NomSchPrs[i++]))
	      PivotError (fich);
	while (!(error || NomSchPrs[i - 1] == '\0' || i == MAX_NAME_LENGTH)) ;

	if (!BIOreadByte (fich, marque))
	   PivotError (fich);
	NatureDansTable (pDoc, NomSchStr, rangNature);
	/* charge les schemas de structure et de presentation du document */
	if (pDoc->DocSSchema == NULL)
	   LoadSchemas (NomSchStr, NomSchPrs, &pDoc->DocSSchema, pSCharge, FALSE);
	if (pDoc->DocSSchema == NULL)
	   PivotError (fich);
	else if (pDoc->DocPivotVersion >= 4)
	   /* on verifie que la version du schema charge' est la meme */
	   /* que celle du document */
	   if (pDoc->DocSSchema->SsCode != versionSchema)
	     {
		pDoc->DocToBeChecked = TRUE;
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_STRUCTURE_SCHEMA_X_CHANGED), pDoc->DocSSchema->SsName);
	     }
     }
   if (pDoc->DocNatureSSchema[rangNature - 1] == NULL)
     {
	pDoc->DocNatureSSchema[rangNature - 1] = pDoc->DocSSchema;
	strncpy (pDoc->DocNatureName[rangNature - 1], NomSchStr, MAX_NAME_LENGTH);
	strncpy (pDoc->DocNaturePresName[rangNature - 1], NomSchPrs, MAX_NAME_LENGTH);
	if (pDoc->DocSSchema != NULL)
	   if (pDoc->DocSSchema->SsPSchema == NULL)
	      /* le schema de presentation n'a pas ete charge' (librairie  */
	      /* Kernel, par exemple). On memorise dans le schema de */
	      /* structure charge' le nom du schema P associe' */
	      strncpy (pDoc->DocSSchema->SsDefaultPSchema, NomSchPrs, MAX_NAME_LENGTH);
     }
   /* lit les noms des fichiers contenant les schemas de nature  */
   /* dynamiques et charge ces schemas, sauf si on ne charge que */
   /* les elements exportables. */
   while ((*marque == (char) C_PIV_NATURE || *marque == (char) C_PIV_SSCHEMA_EXT)
	  && !error)
     {
	ExtensionSch = (*marque == (char) C_PIV_SSCHEMA_EXT);
	i = 0;
	rangNature++;
	do
	   if (!BIOreadByte (fich, &NomSchStr[i++]))
	      PivotError (fich);
	while (NomSchStr[i - 1] != '\0' && !error) ;
	if (pDoc->DocPivotVersion >= 4)
	   /* Lit le code du schema de structure */
	   if (!error)
	      if (!BIOreadShort (fich, &versionSchema))
		 PivotError (fich);
	/* Lit le nom du schema de presentation associe' */
	if (!error)
	   if (!BIOreadByte (fich, marque))
	      PivotError (fich);
	if (*marque >= '!' && *marque <= '~' && !error)
	   /* il y a un nom de schema de presentation */
	  {
	     NomSchPrs[0] = *marque;
	     i = 1;
	     do
		if (!BIOreadByte (fich, &NomSchPrs[i++]))
		   PivotError (fich);
	     while (!(error || NomSchPrs[i - 1] == '\0' || i == MAX_NAME_LENGTH)) ;

	     if (!BIOreadByte (fich, marque))
		PivotError (fich);
	  }
	else
	   /* il n'y a pas de nom */
	   NomSchPrs[0] = '\0';
	pSS = NULL;
	if (!error)
	  {
	     NatureDansTable (pDoc, NomSchStr, rangNature);
	     if (pDoc->DocNatureSSchema[rangNature - 1] == NULL)
		if (ExtensionSch)
		   /* charge l'extension de schema */
		   pSS = LoadExtension (NomSchStr, NomSchPrs, pDoc);
		else
		  {
		     i = CreeNature (NomSchStr, NomSchPrs, pDoc->DocSSchema);
		     if (i == 0)
			PivotError (fich);	/* echec creation nature */
		     else
			pSS = pDoc->DocSSchema->SsRule[i - 1].SrSSchemaNat;
		  }
	  }
	if (!error && pSS != NULL)
	  {
	     if (pDoc->DocPivotVersion >= 4)
		/* on verifie que la version du schema charge' est la meme */
		/* que celle du document */
		if (pSS->SsCode != versionSchema)
		  {
		     pDoc->DocToBeChecked = TRUE;
		     TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_STRUCTURE_SCHEMA_X_CHANGED), pSS->SsName);
		  }
	     pDoc->DocNatureSSchema[rangNature - 1] = pSS;
	     strncpy (pDoc->DocNaturePresName[rangNature - 1], NomSchPrs, MAX_NAME_LENGTH);
	     if (pSS->SsPSchema == NULL)
		/* le schema de presentation n'a pas ete charge' (librairie
		   Kernel, par exemple). On memorise dans le schema de structure
		   charge' le nom du schema P associe' */
		strncpy (pSS->SsDefaultPSchema, NomSchPrs, MAX_NAME_LENGTH);
	  }
     }
}



#ifdef __STDC__
void                rdTableLangues (BinFile fich, PtrDocument pDoc, char *marque)

#else  /* __STDC__ */
void                rdTableLangues (fich, pDoc, marque)
BinFile             fich;
PtrDocument         pDoc;
char               *marque;

#endif /* __STDC__ */

{
   Name                 NomLangue;
   int                 i;

   /* lit la table des langues utilisees par le document */
   pDoc->DocNLanguages = 0;
   if (pDoc->DocPivotVersion >= 4)
      while (*marque == (char) C_PIV_LANG && !error)
	{
	   i = 0;
	   do
	      if (!BIOreadByte (fich, &NomLangue[i++]))
		 PivotError (fich);
	   while (!(error || NomLangue[i - 1] == '\0' || i == MAX_NAME_LENGTH)) ;
	   if (NomLangue[i - 1] != '\0')
	     {
		PivotError (fich);
		PivotFormatError ("Z");
	     }
	   else
	     {
		if (NomLangue[0] != '\0')
		   if (pDoc->DocNLanguages < MAX_LANGUAGES_DOC)
		      pDoc->DocLanguages[pDoc->DocNLanguages++] =
			 TtaGetLanguageIdFromName (NomLangue);
		/* lit l'octet suivant le nom de langue */
		if (!BIOreadByte (fich, marque))
		   PivotError (fich);
	     }
	}
}


/* ---------------------------------------------------------------------- */
/* |    rdVersionNumber lit la version dans le fichier pivot et         | */
/* |    met sa valeur dans le contexte pDoc. Retourne 0 si OK.          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 rdVersionNumber (BinFile fich, PtrDocument pDoc)

#else  /* __STDC__ */
int                 rdVersionNumber (fich, pDoc)
BinFile             fich;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   char                c;
   int                 ret;

   ret = 0;
   pDoc->DocPivotVersion = 1;
   if (!BIOreadByte (fich, &c))
      ret = 10;
   else if (c != (char) C_PIV_VERSION)
      ret = 10;
   else if (!BIOreadByte (fich, &c))
      ret = 10;
   else if (c != (char) C_PIV_VERSION)
      ret = 10;
   else if (!BIOreadByte (fich, &c))
      ret = 10;
   else
      pDoc->DocPivotVersion = (int) c;
   return ret;
}

/* ---------------------------------------------------------------------- */
/* |    rdPivotHeader   lit l'entete d'un fichier pivot.                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                rdPivotHeader (BinFile fich, PtrDocument pDoc, char *marque)

#else  /* __STDC__ */
void                rdPivotHeader (fich, pDoc, marque)
BinFile             fich;
PtrDocument         pDoc;
char               *marque;

#endif /* __STDC__ */

{
   LabelString         lab;
   int                 i;
   char                c;

   /* lit le numero de version s'il est present */
   if (!BIOreadByte (fich, marque))
      PivotError (fich);
   if (*marque == (char) C_PIV_VERSION)
     {
	if (!BIOreadByte (fich, marque))
	   PivotError (fich);
	else if (!BIOreadByte (fich, &c))
	   PivotError (fich);
	else
	  {
	     pDoc->DocPivotVersion = (int) c;
	     if (!BIOreadByte (fich, marque))
		PivotError (fich);
	  }
     }
   else				/* Pivot sans marque de version */
      pDoc->DocPivotVersion = 1;

   /* lit le label max. du document s'il est present */
   if (!error && (*marque == (char) C_PIV_SHORT_LABEL || *marque == (char) C_PIV_LONG_LABEL
		  || *marque == (char) C_PIV_LABEL))
     {
	rdLabel (*marque, lab, fich);
	LabelStringToInt (lab, &i);
	SetCurrentLabel (pDoc, i);
	if (!BIOreadByte (fich, marque))
	   PivotError (fich);
     }
   /* lit la table des langues utilisees par le document */
   rdTableLangues (fich, pDoc, marque);
}


/* ---------------------------------------------------------------------- */
/* |    ChargeDoc charge en memoire (representation interne) un document| */
/* |            qui se trouve sous la forme pivot dans le fichier fich. | */
/* |            Le fichier doit etre ouvert et positionne' au debut.    | */
/* |            Il n'est pas ferme' par ChargeDoc.                      | */
/* |            pDoc est le pointeur sur le descripteur de document du  | */
/* |            document a` charger.                                    | */
/* |            Si ChargeDocExt est vrai, on charge temporairement les  | */
/* |            documents externes reference's pour pouvoir copier les  | */
/* |            elements inclus. Les documents externes ainsi charge's  | */
/* |            sont decharge's au retour. Si Squelette est vrai, le    | */
/* |            document est charge sous forme squelette.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChargeDoc (BinFile fich, PtrDocument pDoc, boolean ChargeDocExt, boolean Squelette, PtrSSchema pSCharge, boolean WithAPPEvent)

#else  /* __STDC__ */
void                ChargeDoc (fich, pDoc, ChargeDocExt, Squelette, pSCharge, WithAPPEvent)
BinFile             fich;
PtrDocument         pDoc;
boolean             ChargeDocExt;
boolean             Squelette;
PtrSSchema        pSCharge;
boolean             WithAPPEvent;

#endif /* __STDC__ */

{
   int                 d, i, j, assoc;
   boolean             found, ok;
   char                marque;
   PtrElement          s, p, premier;

   int         nR, typelu;
   PtrSSchema        pSS, pNat;
   PtrReferredDescr    pDR, pDRSuiv;
   PtrReference        pRef;
   PtrElement          pSource;
   DocumentIdentifier     IdentDocSource;
   PtrDocument         pDocSource;
   int                 NumDocExt;
   PtrDocument         TabDocExt[MaxDocExt];
   PtrPSchema          pSc1;
   PtrSSchema        pSchS1;
   PtrElement          pEl1;
   SRule              *pRe1;
   boolean             pagesacreer;
   PtrSSchema        curExtension, previousSSchema;
   BinFile             fichext;
   char                texte[MAX_TXT_LEN];
   boolean             DocumentCorrect;
   NotifyDialog        notifyDoc;

   pDoc->DocToBeChecked = FALSE;
   DocumentCorrect = TRUE;
   ok = FALSE;
   error = FALSE;
   msgAncienFormat = TRUE;
   pDoc->DocRootElement = NULL;
   pagesacreer = FALSE;
   pDoc->DocNNatures = 0;
   /* lit l'entete du fichier pivot */
   rdPivotHeader (fich, pDoc, &marque);
   /* lit le commentaire du document s'il est present */
   if (!error && (marque == (char) C_PIV_COMMENT || marque == (char) C_PIV_OLD_COMMENT))
     {
	pDoc->DocComment = readComment (fich, TRUE, (boolean) (marque == (char) C_PIV_OLD_COMMENT));
	/* lit l'octet suivant le commentaire */
	if (!BIOreadByte (fich, &marque))
	   PivotError (fich);
     }
   /* Lit le nom du schema de structure */
   /* qui est en tete du fichier pivot */
   if (!error && marque != (char) C_PIV_NATURE)
     {
	PivotError (fich);
	PivotFormatError ("N");	/* marque classe absente */
     }
   if (!error)
      /* lit les noms des schemas de structure et de presentation */
      rdNomsSchemas (fich, pDoc, &marque, pSCharge);
   if (WithAPPEvent && pDoc->DocSSchema != NULL && !error)
     {
	notifyDoc.event = TteDocOpen;
	notifyDoc.document = (Document) IdentDocument (pDoc);
	notifyDoc.view = 0;
	if (ThotSendMessage ((NotifyEvent *) & notifyDoc, TRUE))
	   error = TRUE;
     }
   if (pDoc->DocSSchema != NULL && !error)
     {
	if (pDoc->DocSSchema->SsExport)
	   /* le schema de structure du document prevoit l'exportation     */
	   /* d'elements. On demande a l'utilisateur s'il veut charger la  */
	   /* forme complete ou les seuls elements exportables, mais       */
	   /* seulement si le schema de presentation a une vue EXPORT      */
	  {
	     pagesacreer = FALSE;
	     ok = FALSE;
	     pSc1 = pDoc->DocSSchema->SsPSchema;
	     if (pSc1 != NULL)
		for (i = 1; i <= pSc1->PsNViews; i++)
		   ok = ok || pSc1->PsExportView[i - 1];
	     if (ok)
	       {
		  /* une vue EXPORT est prevue */
		  if (Squelette)
		    {
		       pDoc->DocExportStructure = TRUE;
		       /* Un document charge' sous sa forme export n'est pas */
		       /* modifiable */
		       pDoc->DocReadOnly = TRUE;
		       pagesacreer = TRUE;
		    }
	       }
	  }
	/* Lit d'abord dans le fichier .EXT les labels des elements */
	/* reference's par d'autres documents (on en aura besoin */
	/* pendant la lecture du fichier .PIV). On cherche ce */
	/* fichier dans le meme directory que le fichier .PIV */
	DoFileName (pDoc->DocDName, "EXT", pDoc->DocDirectory, texte, &i);
	fichext = BIOreadOpen (texte);
	if (fichext != 0)
	  {
	     ChargeExt (fichext, NULL, &pDoc->DocLabels, TRUE);
	     BIOreadClose (fichext);
	  }
	else
	   pDoc->DocLabels = NULL;
	/* Internalise le fichier .PIV */

	/* lit les parametres */
	for (i = 1; i <= MAX_PARAM_DOC; i++)
	   pDoc->DocParameters[i - 1] = NULL;
	i = 1;
	while (marque == (char) C_PIV_PARAM && !error)
	   if (i > MAX_PARAM_DOC)
	     {
		PivotError (fich);
		PivotFormatError ("Y");
	     }
	   else
	     {
		if (!BIOreadByte (fich, &marque))
		   PivotError (fich);
		nR = 0;
		pNat = NULL;
		p = Internalise (fich, pDoc->DocSSchema, pDoc, &marque, 0, TRUE,
				 !pDoc->DocExportStructure, &nR, &pNat,
				 &typelu, &pSS, pagesacreer, NULL, TRUE);
		if (WithAPPEvent && pDoc->DocSSchema != NULL && !error)
		   SendEventAttrRead (p, pDoc);
		if (!error)
		  {
		     /* retire les elements exclus (au sens SGML) */
		     RemoveExcludedElem (&p);
		     /* accouple les paires */
		     AccouplePaires (p);
		     pDoc->DocParameters[i - 1] = p;
		     if (pDoc->DocToBeChecked)
			/* verifie que cet arbre est correct */
		       {
			  ok = ArbreCorrect (p, pDoc);
			  DocumentCorrect = DocumentCorrect && ok;
		       }
		  }
		i++;
	     }
	/* lit les elements associes */
	for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
	   pDoc->DocAssocRoot[assoc - 1] = NULL;
	assoc = 0;
	while (marque == (char) C_PIV_ASSOC && !error)
	   /* debut d'un nouveau type d'element associe */
	  {
	     assoc++;
	     if (!BIOreadByte (fich, &marque))
		PivotError (fich);
	     /* lit et cree le premier element associe de ce type */
	     nR = 0;
	     pNat = NULL;

	     p = Internalise (fich, pDoc->DocSSchema, pDoc, &marque, assoc, FALSE,
			      !pDoc->DocExportStructure, &nR, &pNat,
			      &typelu, &pSS, pagesacreer, NULL, TRUE);
	     if (WithAPPEvent && pDoc->DocSSchema != NULL && !error)
		SendEventAttrRead (p, pDoc);
	     pRe1 = &pSS->SsRule[typelu - 1];

	     if (pRe1->SrConstruct == CsList &&
		 pSS->SsRule[pRe1->SrListItem - 1].SrAssocElem)
		/* l'element lu est la racine d'un arbre d'elements associe'. */
		/* L'arbre a ete lu entierement */
	       {
		  pDoc->DocAssocRoot[assoc - 1] = p;
		  /* On retire les elements exclus (au sens SGML) */
		  if (p != NULL)
		    {
		       RemoveExcludedElem (&pDoc->DocAssocRoot[assoc - 1]);
		       /* accouple les paires */
		       AccouplePaires (p);
		    }
	       }
	     else
	       {
		  /* c'est sans doute l'ancienne forme pivot, ou` la racine de */
		  /* l'arbre d'elements associes, n'est pas presente, mais */
		  /* seulement ses fils */
		  premier = p;
		  /* on lit les marques de pages jusqu'au premier element associe' */
		  if (!error && typelu == PageBreak + 1)
		     while (typelu == PageBreak + 1)
		       {
			  nR = 0;
			  pNat = NULL;
			  s = Internalise (fich, pDoc->DocSSchema, pDoc, &marque, assoc,
				     FALSE, !pDoc->DocExportStructure, &nR, &pNat,
				    &typelu, &pSS, pagesacreer, NULL, TRUE);
			  if (WithAPPEvent && pDoc->DocSSchema != NULL && !error)
			     SendEventAttrRead (s, pDoc);
			  if (s != NULL)
			    {
			       if (premier == NULL)
				  premier = s;
			       if (p != NULL)
				  InsertElementAfter (p, s);
			       p = s;
			    }
		       }
		  if (p != NULL)
		     /* cherche la regle liste de ce type d'element */
		    {
		       pSchS1 = pDoc->DocSSchema;
		       /* on cherche dans le schema du document et dans les */
		       /* extensions de ce schema */
		       do
			 {
			    j = pSchS1->SsNRules;
			    found = FALSE;
			    do
			      {
				 if (pSchS1->SsRule[j - 1].SrConstruct == CsList)
				    if (EquivalentSRules (pSchS1->SsRule[j - 1].SrListItem,
						 pSchS1, typelu, pSS, NULL))
				       found = TRUE;
				 if (!found)
				    j--;
			      }
			    while (!(found || j == 1));
			    if (!found)
			       /* pas trouve', on cherche dans l'extension suivante du */
			       /* schema de structure du document */
			       pSchS1 = pSchS1->SsNextExtens;
			 }
		       while (!found && pSchS1 != NULL);
		       if (!found)
			 {
			    PivotError (fich);
			    PivotFormatError ("a");
			 }
		       else
			  /* cree l'element liste pour ce type d'elements associes */
			 {
			    pDoc->DocAssocRoot[assoc - 1] = NewSubtree (j, pSchS1,
			      pDoc, assoc, FALSE, TRUE, FALSE, TRUE);
			    if (premier != NULL)
			      {
				 /* chaine le 1er elem. associe dans cette liste */
				 InsertFirstChild (pDoc->DocAssocRoot[assoc - 1], premier);
				 /* retire les elements exclus (au sens SGML) */
				 RemoveExcludedElem (&pDoc->DocAssocRoot[assoc - 1]);
				 /* accouple les paires */
				 AccouplePaires (pDoc->DocAssocRoot[assoc - 1]);
			      }
			 }
		    }
		  /* lit les elements associes suivants de meme type */
		  while (!error && (marque == (char) C_PIV_TYPE || marque == (char) C_PIV_NATURE))
		    {
		       nR = 0;
		       pNat = NULL;
		       s = Internalise (fich, pDoc->DocSSchema, pDoc, &marque, assoc,
				     FALSE, !pDoc->DocExportStructure, &nR, &pNat,
					&typelu, &pSS, pagesacreer,
				    pDoc->DocAssocRoot[assoc - 1], TRUE);
		       if (WithAPPEvent && pDoc->DocSSchema != NULL && !error)
			  SendEventAttrRead (s, pDoc);
		       if (s != NULL)
			 {
			    if (p == NULL)
			       InsertFirstChild (pDoc->DocAssocRoot[assoc - 1], s);
			    else
			       InsertElementAfter (p, s);
			    /* retire les elements exclus (au sens SGML) */
			    RemoveExcludedElem (&s);
			    /* accouple les paires */
			    AccouplePaires (s);
			    if (s != NULL)
			       p = s;
			 }
		    }
	       }
	     if (!error && pDoc->DocToBeChecked)
		/* verifie que cet arbre est correct */
	       {
		  ok = ArbreCorrect (pDoc->DocAssocRoot[assoc - 1], pDoc);
		  DocumentCorrect = DocumentCorrect && ok;
	       }
	  }

	/* lit le corps du document */
	if (!error)
	   if (marque != (char) C_PIV_DOC_END)
	      if (marque != (char) C_PIV_STRUCTURE)
		{
		   PivotError (fich);
		   PivotFormatError ("O");
		}
	      else
		{
		   if (!BIOreadByte (fich, &marque))
		      PivotError (fich);
		   if (marque != (char) C_PIV_TYPE && marque != (char) C_PIV_NATURE)
		     {
			PivotError (fich);
			PivotFormatError ("P");
		     }
		   else
		     {
			nR = 0;
			pNat = NULL;
			p = Internalise (fich, pDoc->DocSSchema, pDoc, &marque, 0,
				     FALSE, !pDoc->DocExportStructure, &nR, &pNat,
				    &typelu, &pSS, pagesacreer, NULL, TRUE);
			if (WithAPPEvent && pDoc->DocSSchema != NULL && !error)
			   SendEventAttrRead (p, pDoc);
			/* force la creation d'un element racine */
			if (p == NULL)
			   /* rien n'a ete cree */
			   p = NewSubtree (pDoc->DocSSchema->SsRootElem, pDoc->DocSSchema, pDoc, 0,
					   FALSE, TRUE, TRUE, TRUE);
			else if (p->ElSructSchema != pDoc->DocSSchema
				 || p->ElTypeNumber != pDoc->DocSSchema->SsRootElem)
			   /* ce n'est pas la racine attendue */
			  {
			     s = p;
			     p = NewSubtree (pDoc->DocSSchema->SsRootElem, pDoc->DocSSchema, pDoc, 0,
					     FALSE, TRUE, TRUE, TRUE);
			     InsertFirstChild (p, s);
			  }
			/* traite les elements exclus (au sens SGML) */
			RemoveExcludedElem (&p);
			/* accouple les paires */
			AccouplePaires (p);
			pDoc->DocRootElement = p;
			if (pDoc->DocToBeChecked)
			   /* verifie que cet arbre est correct */
			  {
			     ok = ArbreCorrect (p, pDoc);
			     DocumentCorrect = DocumentCorrect && ok;
			  }
		     }
		}
	if (!DocumentCorrect)
	   /* Le document n'est pas correct */
	  {
	     /* on previent l'utilisateur */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_STRUCTURE_OF_DOCUMENT_X_INVALID), pDoc->DocDName);
	     /* on met le document en Read-Only */
	     pDoc->DocReadOnly = TRUE;
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_DOCUMENT_X_LOCKED), pDoc->DocDName);
	  }
	/* libere les labels des elements reference's par d'autres */
	/* documents */
	pDR = pDoc->DocLabels;
	while (pDR != NULL)
	  {
	     /* inutile de liberer les descripteurs de documents */
	     /* externes, on ne les a pas charge's */
	     pDRSuiv = pDR->ReNext;
	     FreeDescReference (pDR);
	     pDR = pDRSuiv;
	  }

	/* supprime les extensions de schemas ExtCorr et ExtMot */
	/* les attributs definis dans ces extensions ont deje ete retire's par */
	/* ReadAttribut (ces extensions ne definissent que des attributs) */
	if (!error && pDoc->DocPivotVersion < 4)
	  {
	     previousSSchema = pDoc->DocSSchema;
	     curExtension = previousSSchema->SsNextExtens;
	     while (curExtension != NULL)
		if (strcmp (curExtension->SsName, "ExtCorr") == 0 ||
		    strcmp (curExtension->SsName, "ExtMot") == 0)
		  {
		     previousSSchema->SsNextExtens = curExtension->SsNextExtens;
		     if (curExtension->SsNextExtens != NULL)
			curExtension->SsNextExtens->SsPrevExtens = previousSSchema;
		     FreeSStruct (curExtension);
		     curExtension = previousSSchema->SsNextExtens;
		  }
		else
		  {
		     previousSSchema = curExtension;
		     curExtension = curExtension->SsNextExtens;
		  }
	  }
	if (!error)
	   /* affecte un label a tous les elements qui n'en ont pas et */
	   /* recherche toutes les references d'inclusion du document et */
	   /* copie les elements inclus */
	  {
	     /* on affecte des labels aux elements de l'arbre principal */
	     if (pDoc->DocRootElement != NULL)
		AffecteLabel (pDoc->DocRootElement, pDoc);
	     /* on affecte des labels aux elements des arbres associes */
	     for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
		if (pDoc->DocAssocRoot[assoc - 1] != NULL)
		   AffecteLabel (pDoc->DocAssocRoot[assoc - 1], pDoc);
	     /* on affecte des labels aux elements des parametres */
	     for (i = 1; i <= MAX_PARAM_DOC; i++)
		if (pDoc->DocParameters[i - 1] != NULL)
		   AffecteLabel (pDoc->DocParameters[i - 1], pDoc);

	     /* nettoie la table des documents externes charge's */
	     for (NumDocExt = 1; NumDocExt <= MaxDocExt; NumDocExt++)
		TabDocExt[NumDocExt - 1] = NULL;
	     /* parcourt la chaine des descripteurs d'elements reference's */
	     /* du document, pour traiter toutes les references */
	     pDR = pDoc->DocReferredEl->ReNext;
	     while (pDR != NULL)
	       {
		  pRef = pDR->ReFirstReference;
		  /* premiere reference a l'element reference' courant */
		  /* parcourt toutes les references designant l'element */
		  /* reference' courant */
		  while (pRef != NULL)
		    {
		       if (pRef->RdTypeRef == RefInclusion)
			  /* il s'agit bien d'une reference d'inclusion */
			  if (pRef->RdElement->ElSource == pRef)
			     /* c'est bien une inclusion avec expansion, on copie */
			     /* l'arbre abstrait de l'element inclus */
			    {
			       if (ChargeDocExt)
				  /* l'element inclus est-il accessible ? */
				 {
				    pSource = ReferredElement (pRef, &IdentDocSource, &pDocSource);
				    if (pSource == NULL)
				       if (!IdentDocNul (IdentDocSource))
					  if (pDocSource == NULL)
					     /* il y a bien un objet a inclure qui appartient au document */
					     /* IdentDocSource et ce document n'est pas charge'. */
					     /* On le charge. */
					     /* cherche une entree libre dans la table des documents */
					     /* externes charge's */
					    {
					       NumDocExt = 1;
					       while (TabDocExt[NumDocExt - 1] != NULL
						   && NumDocExt < MaxDocExt)
						  NumDocExt++;
					       if (TabDocExt[NumDocExt - 1] == NULL)
						  /* on a trouve' une entree libre, on */
						  /* charge  le document externe */
						 {
						    CreateDocument (&TabDocExt[NumDocExt - 1]);
						    if (TabDocExt[NumDocExt - 1] != NULL)
						      {
							 CopyIdentDoc (&TabDocExt[NumDocExt - 1]->DocIdent, IdentDocSource);
							 ok = OuvreDoc (NULL, TabDocExt[NumDocExt - 1],
									FALSE, FALSE,
									NULL, FALSE);
						      }
						    if (TabDocExt[NumDocExt - 1] != NULL)
						      {
							 CopyIdentDoc (&TabDocExt[NumDocExt - 1]->DocIdent, IdentDocSource);
							 if (!ok)
							   {
							      /* echec a l'ouverture du document */
							      TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_MISSING_FILE), IdentDocSource);
							      FreeDocument (TabDocExt[NumDocExt - 1]);
							      TabDocExt[NumDocExt - 1] = NULL;
							   }
						      }
						 }
					    }
				    pEl1 = pRef->RdElement;
				    pRe1 = &pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1];
				    /*TODO *//* mis en commentaire cas instruction if sans corps */
				    /* if (pRe1->SrConstruct == CsReference) */
				    /* il s'agit d'un element reference */
				    /* if (pRe1->SrRefImportedDoc) */
				    /* traitement des documents importes */
				 }
			       /* il s'agit plus precisement de l'inclusion */
			       /* d'un document externe */
			       CopyIncludedElem (pRef->RdElement, pDoc);
			    }
		       pRef = pRef->RdNext;
		       /* passe a la reference suivante */
		    }		/* passe au descripteur d'element reference' suivant */
		  pDR = pDR->ReNext;
	       }
	     if (ChargeDocExt)
		/* on decharge les documents externes qui ont ete charge's */
		/* pour copier des elements inclus */
		for (NumDocExt = 1; NumDocExt <= MaxDocExt; NumDocExt++)
		   if (TabDocExt[NumDocExt - 1] != NULL)
		     {
			SupprDoc (TabDocExt[NumDocExt - 1]);
			LibSchemas (TabDocExt[NumDocExt - 1]);
			/* cherche le document dans la table */
			/* des documents */
			d = 1;
			while (TabDocuments[d - 1] != TabDocExt[NumDocExt - 1]
			       && d < MAX_DOCUMENTS)
			   d++;
			/* libere l'entree de la table des documents */
			if (TabDocuments[d - 1] == TabDocExt[NumDocExt - 1])
			   TabDocuments[d - 1] = NULL;
			/* libere tout le document */
			FreeDocument (TabDocExt[NumDocExt - 1]);
		     }
	     /* verifie que les racines de tous les arbres du document possedent */
	     /* bien un attribut langue */
	     CheckLanguageAttr (pDoc, pDoc->DocRootElement);
	     pDoc->DocRootElement->ElAccess = AccessReadWrite;
	     for (i = 1; i <= MAX_PARAM_DOC; i++)
		if (pDoc->DocParameters[i - 1] != NULL)
		  {
		     CheckLanguageAttr (pDoc, pDoc->DocParameters[i - 1]);
		     pDoc->DocParameters[i - 1]->ElAccess = AccessReadOnly;
		  }
	     for (i = 1; i <= MAX_ASSOC_DOC; i++)
		if (pDoc->DocAssocRoot[i - 1] != NULL)
		  {
		     CheckLanguageAttr (pDoc, pDoc->DocAssocRoot[i - 1]);
		     pDoc->DocAssocRoot[i - 1]->ElAccess = AccessReadWrite;
		  }
	     if (ThotLocalActions[T_indexschema] != NULL)
		(*ThotLocalActions[T_indexschema]) (pDoc);
	     if (WithAPPEvent && pDoc->DocSSchema != NULL && !error)
	       {
		  notifyDoc.event = TteDocOpen;
		  notifyDoc.document = (Document) IdentDocument (pDoc);
		  notifyDoc.view = 0;
		  ThotSendMessage ((NotifyEvent *) & notifyDoc, FALSE);
	       }
	  }
     }
}

/* End Of Module pivlec */
