/*

 This module writes in an output file the pivot representation of a
 document.

 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "constpiv.h"
#include "storage.h"
#include "appaction.h"
#include "app.h"
#include "typecorr.h"

#undef EXPORT
#define EXPORT extern

#ifndef NODISPLAY
#include "modif.var"
#endif
#include "environ.var"

#include "tree_f.h"
#include "attributes_f.h"
#include "dofile_f.h"
#include "callback_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changepresent_f.h"
#include "ouvre_f.h"
#include "writepivot_f.h"
#include "references_f.h"
#include "externalref_f.h"
#include "schemas_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"

#ifdef __STDC__
extern int          RemoveFile (char *);

#else  /* __STDC__ */
extern int          RemoveFile ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    Retourne Vrai si les deux elements pointes par pEl1 et pEl2     | */
/* |    possedent les memes attributs avec les memes valeurs            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             MemesAttributs (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
boolean             MemesAttributs (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   PtrAttribute        pAttr1, pAttr2;
   int                 nAttr1, nAttr2;
   boolean             same = TRUE;

   /* nombre d'attributs du 1er element */
   pAttr1 = pEl1->ElFirstAttr;
   nAttr1 = 0;
   /* compte les attributs du 1er element */
   while (pAttr1 != NULL)
     {
	nAttr1++;
	pAttr1 = pAttr1->AeNext;
     }

   /* nombre d'attributs du 2eme element */
   pAttr2 = pEl2->ElFirstAttr;
   nAttr2 = 0;
   /* compte les attributs du 2eme element */
   while (pAttr2 != NULL)
     {
	nAttr2++;
	pAttr2 = pAttr2->AeNext;
     }

   /* compare le nombre d'attributs des deux elements */
   if (nAttr1 != nAttr2)
      same = FALSE;		/* nombres d'attributs differents, fin */
   else
      /* meme nombre d'attributs, compare les attributs et leurs valeurs */
     {
	pAttr1 = pEl1->ElFirstAttr;
	/* 1er attribut du 1er element */
	/* examine tous les attributs du 1er element */
	while (pAttr1 != NULL && same)
	   /* cherche si le 2eme element possede cet attribut du 1er elem */
	  {
	     pAttr2 = GetAttributeOfElement (pEl2, pAttr1);
	     if (pAttr2 == NULL)
		/* le 2eme element n'a pas cet attribut, fin */
		same = FALSE;
	     else
	       {
		  if (pAttr1->AeDefAttr != pAttr2->AeDefAttr)
		     /* valeurs differentes de cet attribut */
		     same = FALSE;
		  else
		     switch (pAttr1->AeAttrType)
			   {
			      case AtNumAttr:
			      case AtEnumAttr:
				 if (pAttr1->AeAttrValue != pAttr2->AeAttrValue)
				    same = FALSE;
				 break;
			      case AtReferenceAttr:
				 same = FALSE;
				 break;
			      case AtTextAttr:
				 same = TextsEqual (pAttr2->AeAttrText, pAttr1->AeAttrText);
				 break;
			      default:
				 break;
			   }
	       }
	     if (same)
		/* meme valeur,passe a l'attribut suivant du 1er element */
		pAttr1 = pAttr1->AeNext;
	  }
     }
   return same;
}

/* ---------------------------------------------------------------------- */
/* | Ecrit dans le fichier le numero de version pivot courant           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteVersionNumber (BinFile pivFile)
#else  /* __STDC__ */
void                WriteVersionNumber (pivFile)
BinFile             pivFile;

#endif /* __STDC__ */
{
   int                 version;

   BIOwriteByte (pivFile, (char) C_PIV_VERSION);
   BIOwriteByte (pivFile, (char) C_PIV_VERSION);
   /* Version courante de PIVOT: 4 */
   version = 4;
   BIOwriteByte (pivFile, (char) version);
}

/* ---------------------------------------------------------------------- */
/* | PutShort   ecrit un entier court dans le fichier sur deux octets   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void                PutShort (BinFile pivFile, int n)
#else  /* __STDC__ */
static void                PutShort (pivFile, n)
BinFile             pivFile;
int                 n;

#endif /* __STDC__ */
{
   BIOwriteByte (pivFile, (char) (n / 256));
   BIOwriteByte (pivFile, (char) (n % 256));
}

/* ---------------------------------------------------------------------- */
/* | PutInteger ecrit un entier long dans le fichier, sur 4 octets      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutInteger (BinFile pivFile, int n)
#else  /* __STDC__ */
static void         PutInteger (pivFile, n)
BinFile             pivFile;
int                 n;

#endif /* __STDC__ */
{
   PutShort (pivFile, n / 65536);
   PutShort (pivFile, n % 65536);
}

/* ---------------------------------------------------------------------- */
/* | PutDimensionType ecrit dans le fichier un type de dimension sur	| */
/* | 1 octet.								| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutDimensionType (BinFile pivFile, boolean b)
#else  /* __STDC__ */
static void         PutDimensionType (pivFile, b)
BinFile             pivFile;
boolean             b;

#endif /* __STDC__ */
{
   if (b)
      BIOwriteByte (pivFile, C_PIV_ABSOLUTE);
   else
      BIOwriteByte (pivFile, C_PIV_RELATIVE);
}

/* ---------------------------------------------------------------------- */
/* | PutUnit ecrit dans le fichier l'unite                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutUnit (BinFile pivFile, TypeUnit unit)
#else  /* __STDC__ */
static void         PutUnit (pivFile, unit)
BinFile             pivFile;
TypeUnit            unit;

#endif /* __STDC__ */
{
   if (unit == UnPoint)
      BIOwriteByte (pivFile, C_PIV_PT);
   else
      BIOwriteByte (pivFile, C_PIV_EM);
}

/* ---------------------------------------------------------------------- */
/* | PutSign   ecrit un signe dans le fichier sur un octet             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutSign (BinFile pivFile, boolean b)
#else  /* __STDC__ */
static void         PutSign (pivFile, b)
BinFile             pivFile;
boolean             b;

#endif /* __STDC__ */
{
   if (b)
      BIOwriteByte (pivFile, C_PIV_PLUS);
   else
      BIOwriteByte (pivFile, C_PIV_MINUS);
}

/* ---------------------------------------------------------------------- */
/* | PutDimension ecrit dans le fichier le contenu de la regle de       | */
/* |            dimension pointee par pRegle                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutDimension (BinFile pivFile, PtrPRule pPRule)
#else  /* __STDC__ */
static void         PutDimension (pivFile, pPRule)
BinFile             pivFile;
PtrPRule            pPRule;

#endif /* __STDC__ */
{

   PutDimensionType (pivFile, pPRule->PrDimRule.DrAbsolute);
   PutShort (pivFile, abs (pPRule->PrDimRule.DrValue));
   if (pPRule->PrDimRule.DrUnit == UnPercent)
      BIOwriteByte (pivFile, C_PIV_PERCENT);
   else
      PutUnit (pivFile, pPRule->PrDimRule.DrUnit);
   PutSign (pivFile, (boolean) (pPRule->PrDimRule.DrValue >= 0));
}

/* ---------------------------------------------------------------------- */
/* | PutBoolean ecrit un booleen dans le fichier sur un octet           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutBoolean (BinFile pivFile, boolean b)
#else  /* __STDC__ */
static void         PutBoolean (pivFile, b)
BinFile             pivFile;
boolean             b;

#endif /* __STDC__ */
{
   if (b)
      BIOwriteByte (pivFile, C_PIV_TRUE);
   else
      BIOwriteByte (pivFile, C_PIV_FALSE);
}

/* ---------------------------------------------------------------------- */
/* | PutAlignment ecrit un BAlignment dans le fichier sur un octet      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutAlignment (BinFile pivFile, BAlignment c)
#else  /* __STDC__ */
static void         PutAlignment (pivFile, c)
BinFile             pivFile;
BAlignment             c;

#endif /* __STDC__ */
{
   switch (c)
	 {
	    case AlignLeft:
	       BIOwriteByte (pivFile, C_PIV_LEFT);
	       break;
	    case AlignRight:
	       BIOwriteByte (pivFile, C_PIV_RIGHT);
	       break;
	    case AlignCenter:
	       BIOwriteByte (pivFile, C_PIV_CENTERED);
	       break;
	    case AlignLeftDots:
	       BIOwriteByte (pivFile, C_PIV_LEFTDOT);
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | PutPageType ecrit un type de page dans le fichier sur un octet     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutPageType (BinFile pivFile, PageType t)
#else  /* __STDC__ */
static void         PutPageType (pivFile, t)
BinFile             pivFile;
PageType            t;

#endif /* __STDC__ */
{
   switch (t)
	 {
	    case PgComputed:
	       BIOwriteByte (pivFile, C_PIV_COMPUTED_PAGE);
	       break;
	    case PgBegin:
	       BIOwriteByte (pivFile, C_PIV_START_PAGE);
	       break;
	    case PgUser:
	       BIOwriteByte (pivFile, C_PIV_USER_PAGE);
	       break;
#ifdef __COLPAGE__
	    case ColComputed:
	       BIOwriteByte (pivFile, C_PIV_COMPUTED_COL);
	       break;
	    case ColBegin:
	       BIOwriteByte (pivFile, C_PIV_START_COL);
	       break;
	    case ColUser:
	       BIOwriteByte (pivFile, C_PIV_USER_COL);
	       break;
	    case ColGroup:
	       BIOwriteByte (pivFile, C_PIV_COL_GROUP);
	       break;
#endif /* __COLPAGE__ */
	    default:
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | PutReferenceType ecrit un type de reference dans le fichier sur un	| */
/* |    octet. S'il s'agit d'une inclusion, expansion indique si c'est	| */
/* |    une inclusion avec ou sans expansion a l'ecran.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutReferenceType (BinFile pivFile, ReferenceType t, boolean expansion)
#else  /* __STDC__ */
static void         PutReferenceType (pivFile, t, expansion)
BinFile             pivFile;
ReferenceType       t;
boolean             expansion;

#endif /* __STDC__ */
{
   switch (t)
	 {
	    case RefFollow:
	       BIOwriteByte (pivFile, C_PIV_REF_FOLLOW);
	       break;
	    case RefInclusion:
	       if (expansion)
		  BIOwriteByte (pivFile, C_PIV_REF_INCLUS_EXP);
	       else
		  BIOwriteByte (pivFile, C_PIV_REF_INCLUSION);
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | PutComment ecrit dans le fichier un commentaire dont le		| */
/* |            texte commence dans le buffer pointe par pBuf.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutComment (BinFile pivFile, PtrTextBuffer pBuf)
#else  /* __STDC__ */
static void         PutComment (pivFile, pBuf)
BinFile             pivFile;
PtrTextBuffer      pBuf;

#endif /* __STDC__ */
{
   int                i, len;
   PtrTextBuffer      pBuf1;

   /* ecrit la marque de commentaire */
   BIOwriteByte (pivFile, (char) C_PIV_COMMENT);
   /* calcule la longeur du commentaire */
   pBuf1 = pBuf;
   len = 0;
   while (pBuf1 != NULL)
      /* parcourt tous les buffers de texte du commentaire */
     {
	len += pBuf1->BuLength;
	pBuf1 = pBuf1->BuNext;
     }
   /* ecrit la longueur du commentaire */
   PutShort (pivFile, len);
   /* ecrit dans le fichier le texte du commentaire */
   while (pBuf != NULL)
      /* lit tous les buffers de texte du commentaire */
     {
	i = 1;
	while (i <= pBuf->BuLength)
	  {
	     BIOwriteByte (pivFile, pBuf->BuContent[i - 1]);
	     i++;
	  }
	pBuf = pBuf->BuNext;
     }
}


/* ---------------------------------------------------------------------- */
/* | EcritNat ecrit une marque de nature suivie du numero du schema    | */
/* |            de structure pointe par pSS.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                EcritNat (PtrSSchema pSS, BinFile pivFile, PtrDocument pDoc)
#else  /* __STDC__ */
void                EcritNat (pSS, pivFile, pDoc)
PtrSSchema          pSS;
BinFile             pivFile;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 n;
   boolean             stop;

   BIOwriteByte (pivFile, (char) C_PIV_NATURE);
   /* cherche le schema de structure */
   n = 0;
   stop = FALSE;
   do
      if (strcmp (pSS->SsName, pDoc->DocNatureName[n]) == 0)
	 stop = TRUE;
      else if (n < pDoc->DocNNatures - 1)
	 n++;
      else
	{
	   n = 0;
	   stop = TRUE;
	}
   while (!stop);
   PutShort (pivFile, n);
}

/* ---------------------------------------------------------------------- */
/* | PutLabel   ecrit le label label dans le fichier pivFile                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PutLabel (BinFile pivFile, LabelString label)
#else  /* __STDC__ */
void                PutLabel (pivFile, label)
BinFile             pivFile;
LabelString         label;

#endif /* __STDC__ */
{
   int                 i;

   BIOwriteByte (pivFile, (char) C_PIV_LABEL);
   i = 0;
   do
	BIOwriteByte (pivFile, label[i++]);
   while (label[i - 1] != '\0');
}

/* ---------------------------------------------------------------------- */
/* | PutPictureArea							| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutPictureArea (BinFile pivFile, int *pictInfo)
#else  /* __STDC__ */
static void         PutPictureArea (pivFile, pictInfo)
BinFile             pivFile;
int                *pictInfo;

#endif /* __STDC__ */
{
   PictInfo    *pictDesc;

   pictDesc = (PictInfo *) pictInfo;
   PutShort (pivFile, PixelToPoint (pictDesc->PicXArea));
   PutShort (pivFile, PixelToPoint (pictDesc->PicYArea));
   PutShort (pivFile, PixelToPoint (pictDesc->PicWArea));
   PutShort (pivFile, PixelToPoint (pictDesc->PicHArea));
}

/* ---------------------------------------------------------------------- */
/* |	PutPresentation							| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutPresentation (BinFile pivFile, PictureScaling PicPresent)
#else  /* __STDC__ */
static void         PutPresentation (pivFile, PicPresent)
BinFile             pivFile;
PictureScaling           PicPresent;

#endif /* __STDC__ */
{
   switch (PicPresent)
	 {
	    case RealSize:
	       BIOwriteByte (pivFile, C_PIV_REALSIZE);
	       break;
	    case ReScale:
	       BIOwriteByte (pivFile, C_PIV_RESCALE);
	       break;
	    case FillFrame:
	       BIOwriteByte (pivFile, C_PIV_FILLFRAME);
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | PutReference ecrit dans le fichier pivFile la reference pointee	| */
/* |            par pRef.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutReference (BinFile pivFile, PtrReference pRef)
#else  /* __STDC__ */
static void         PutReference (pivFile, pRef)
BinFile             pivFile;
PtrReference        pRef;

#endif /* __STDC__ */
{
   PtrReferredDescr    pRefD;
   LabelString         label;
   boolean             expansion;

   /* ecrit le type de la reference */
   expansion = FALSE;
   if (pRef->RdElement != NULL)
      expansion = pRef->RdElement->ElSource == pRef;
   PutReferenceType (pivFile, pRef->RdTypeRef, expansion);
   PutBoolean (pivFile, pRef->RdInternalRef);
   if (pRef->RdReferred == NULL)
      /* la reference ne designe rien, on ecrit un label nul */
      PutLabel (pivFile, "");
   else
     {
	pRefD = pRef->RdReferred;
	if (pRefD->ReExternalRef)
	   /* la reference designe un objet dans un autre document */
	   /* ecrit le label de l'objet designe' */
	  {
	     PutLabel (pivFile, pRefD->ReReferredLabel);
	     /* ecrit l'identificateur du document auquel appartient l'objet */
	     /* designe' */
	     BIOwriteIdentDoc (pivFile, pRefD->ReExtDocument);
	  }
	else
	   /* l'objet designe' est dans le meme document */
	  {
	     if (pRefD->ReReferredElem == NULL)
		/* pas d'element reference' */
		label[0] = '\0';
	     else
		/* cherche si l'element reference' */
		/* est dans le buffer (a la suite d'un Couper). */
	     if (DansTampon (pRefD->ReReferredElem))
		label[0] = '\0';
	     else
		/* label: label de l'element designe' */
		strncpy (label, pRefD->ReReferredElem->ElLabel, MAX_LABEL_LEN);
	     /* ecrit le label de l'objet designe' */
	     PutLabel (pivFile, label);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* | PutAttribut ecrit dans le fichier pivFile l'attribut pointe' par  | */
/* |            pAttr.                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PutAttribut (BinFile pivFile, PtrAttribute pAttr, PtrDocument pDoc)
#else  /* __STDC__ */
void                PutAttribut (pivFile, pAttr, pDoc)
BinFile             pivFile;
PtrAttribute        pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   boolean             attrOK;
   boolean             stop;
   int                 n, i;
   PtrElement          pEl;
   DocumentIdentifier  docIdent;
   PtrDocument         pDocRef;
   PtrTextBuffer       pBuf;

   attrOK = TRUE;
   if (pAttr->AeDefAttr
       && pAttr->AeAttrType != AtReferenceAttr)
      /* on n'ecrit pas les attributs fixes, sauf les references */
      attrOK = FALSE;
   else if (pAttr->AeAttrType == AtReferenceAttr)
      /* c'est un attribut reference, on n'ecrit pas */
      /* les attributs references qui pointent sur rien. */
      if (pAttr->AeAttrReference == NULL)
	 attrOK = FALSE;
      else if (pAttr->AeAttrReference->RdReferred == NULL)
	 attrOK = FALSE;
      else
	{
	   pEl = ReferredElement (pAttr->AeAttrReference, &docIdent, &pDocRef);
	   if (pEl == NULL)
	      attrOK = FALSE;
	   else if (DansTampon (pEl))
	      attrOK = FALSE;
	}
   if (attrOK)
      /* cherche le schema de structure ou est defini l'attribut */
     {
	n = 0;
	stop = FALSE;
	do
	   if (strcmp (pAttr->AeAttrSSchema->SsName, pDoc->DocNatureName[n]) == 0)
	      stop = TRUE;
	   else if (n < pDoc->DocNNatures - 1)
	      n++;
	   else
	     {
		n = 0;
		stop = TRUE;
	     }
	while (!stop);
	BIOwriteByte (pivFile, (char) C_PIV_ATTR);
	PutShort (pivFile, n);
	/* numero de la nature de l'attribut */
	PutShort (pivFile, pAttr->AeAttrNum);
	/* numero de l'attribut */
	switch (pAttr->AeAttrType)
	      {
		 case AtEnumAttr:
		    PutShort (pivFile, pAttr->AeAttrValue);
		    /* valeur de cet attribut */
		    break;
		 case AtNumAttr:
		    PutShort (pivFile, abs (pAttr->AeAttrValue));
		    PutSign (pivFile, pAttr->AeAttrValue >= 0);
		    break;
		 case AtReferenceAttr:
		    PutReference (pivFile, pAttr->AeAttrReference);
		    break;
		 case AtTextAttr:
		    pBuf = pAttr->AeAttrText;
		    while (pBuf != NULL)
		      {
			 i = 0;
			 while (pBuf->BuContent[i] != '\0')
			      BIOwriteByte (pivFile, pBuf->BuContent[i++]);
			 pBuf = pBuf->BuNext;
		      }
		    BIOwriteByte (pivFile, '\0');
		    break;
		 default:
		    break;
	      }

     }
}

/* ---------------------------------------------------------------------- */
/* | PutReglePres ecrit dans le fichier pivFile la regle de presentation| */
/* |            specifique pointee par pPRule                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PutReglePres (BinFile pivFile, PtrPRule pPRule)
#else  /* __STDC__ */
void                PutReglePres (pivFile, pPRule)
BinFile             pivFile;
PtrPRule        pPRule;

#endif /* __STDC__ */
{

   /* s'il s'agit d'une regle de dimension elastique, on ne l'ecrit pas */
   if (pPRule->PrType == PtHeight || pPRule->PrType == PtWidth)
      if (pPRule->PrDimRule.DrPosition)
	 return;
   /* on ne traite que les regles de presentation directes (ni heritage */
   /* ni fonction de presentation) dont le codage pivot est defini */
   if ((pPRule->PrType == PtHeight || pPRule->PrType == PtWidth
	|| pPRule->PrType == PtVertPos || pPRule->PrType == PtHorizPos
	|| pPRule->PrType == PtSize || pPRule->PrType == PtStyle
	|| pPRule->PrType == PtUnderline || pPRule->PrType == PtThickness
	|| pPRule->PrType == PtFont || pPRule->PrType == PtBreak1
	|| pPRule->PrType == PtBreak2 || pPRule->PrType == PtPictInfo
	|| pPRule->PrType == PtIndent || pPRule->PrType == PtLineSpacing
	|| pPRule->PrType == PtAdjust || pPRule->PrType == PtJustify
	|| pPRule->PrType == PtHyphenate
	|| pPRule->PrType == PtLineStyle || pPRule->PrType == PtLineWeight
	|| pPRule->PrType == PtFillPattern
	|| pPRule->PrType == PtBackground || pPRule->PrType == PtForeground)
       && pPRule->PrPresMode == PresImmediate)
     {
	/* ecrit la marque de regle */
	BIOwriteByte (pivFile, (char) C_PIV_PRESENT);
	/* ecrit le numero de vue */
	PutShort (pivFile, pPRule->PrViewNum);
	/* ecrit le numero de la boite de presentation concernee */
	PutShort (pivFile, 0);
	/* ecrit le type de la regle et ses parametres */
	switch (pPRule->PrType)
	      {
		 case PtAdjust:
		    /* mode de mise en ligne */
		    BIOwriteByte (pivFile, C_PR_ADJUST);
		    PutAlignment (pivFile, pPRule->PrAdjust);
		    break;
		 case PtHeight:
		    if (!pPRule->PrDimRule.DrPosition)
		      {
			 BIOwriteByte (pivFile, C_PR_HEIGHT);
			 PutDimension (pivFile, pPRule);
		      }
		    break;
		 case PtWidth:
		    if (!pPRule->PrDimRule.DrPosition)
		      {
			 BIOwriteByte (pivFile, C_PR_WIDTH);
			 PutDimension (pivFile, pPRule);
		      }
		    break;
		 case PtVertPos:
		 case PtHorizPos:
		    if (pPRule->PrType == PtVertPos)
		       BIOwriteByte (pivFile, C_PR_VPOS);
		    else
		       BIOwriteByte (pivFile, C_PR_HPOS);
		    PutShort (pivFile, abs (pPRule->PrPosRule.PoDistance));
		    PutUnit (pivFile, pPRule->PrPosRule.PoDistUnit);
		    PutSign (pivFile, pPRule->PrPosRule.PoDistance >= 0);
		    break;
		 case PtSize:
		    BIOwriteByte (pivFile, C_PR_SIZE);
		    PutShort (pivFile, pPRule->PrMinValue);
		    PutUnit (pivFile, pPRule->PrMinUnit);
		    break;
		 case PtStyle:
		    BIOwriteByte (pivFile, C_PR_STYLE);
		    BIOwriteByte (pivFile, pPRule->PrChrValue);
		    break;
		 case PtUnderline:
		    BIOwriteByte (pivFile, C_PR_UNDERLINE);
		    BIOwriteByte (pivFile, pPRule->PrChrValue);
		    break;
		 case PtThickness:
		    BIOwriteByte (pivFile, C_PR_UNDER_THICK);
		    BIOwriteByte (pivFile, pPRule->PrChrValue);
		    break;
		 case PtFont:
		    BIOwriteByte (pivFile, C_PR_FONT);
		    BIOwriteByte (pivFile, pPRule->PrChrValue);
		    break;
		 case PtBreak1:
		    BIOwriteByte (pivFile, C_PR_BREAK1);
		    PutShort (pivFile, pPRule->PrMinValue);
		    PutUnit (pivFile, pPRule->PrMinUnit);
		    break;
		 case PtBreak2:
		    BIOwriteByte (pivFile, C_PR_BREAK2);
		    PutShort (pivFile, pPRule->PrMinValue);
		    PutUnit (pivFile, pPRule->PrMinUnit);
		    break;
		 case PtPictInfo:
		    BIOwriteByte (pivFile, C_PR_PICTURE);
		    PutPictureArea (pivFile, (int *) &(pPRule->PrPictInfo));
		    PutPresentation (pivFile, pPRule->PrPictInfo.PicPresent);
		    PutShort (pivFile, pPRule->PrPictInfo.PicType);
		    break;
		 case PtIndent:
		    BIOwriteByte (pivFile, C_PR_INDENT);
		    PutShort (pivFile, abs (pPRule->PrMinValue));
		    PutUnit (pivFile, pPRule->PrMinUnit);
		    PutSign (pivFile, pPRule->PrMinValue >= 0);
		    break;
		 case PtLineSpacing:
		    BIOwriteByte (pivFile, C_PR_LINESPACING);
		    PutShort (pivFile, pPRule->PrMinValue);
		    PutUnit (pivFile, pPRule->PrMinUnit);
		    break;
		 case PtJustify:
		    BIOwriteByte (pivFile, C_PR_JUSTIFY);
		    PutBoolean (pivFile, pPRule->PrJustify);
		    break;
		 case PtHyphenate:
		    BIOwriteByte (pivFile, C_PR_HYPHENATE);
		    PutBoolean (pivFile, pPRule->PrJustify);
		    break;
		 case PtLineWeight:
		    BIOwriteByte (pivFile, C_PR_LINEWEIGHT);
		    PutShort (pivFile, pPRule->PrMinValue);
		    PutUnit (pivFile, pPRule->PrMinUnit);
		    break;
		 case PtFillPattern:
		    BIOwriteByte (pivFile, C_PR_FILLPATTERN);
		    PutShort (pivFile, pPRule->PrIntValue);
		    break;
		 case PtBackground:
		    BIOwriteByte (pivFile, C_PR_BACKGROUND);
		    PutShort (pivFile, pPRule->PrIntValue);
		    break;
		 case PtForeground:
		    BIOwriteByte (pivFile, C_PR_FOREGROUND);
		    PutShort (pivFile, pPRule->PrIntValue);
		    break;
		 case PtLineStyle:
		    BIOwriteByte (pivFile, C_PR_LINESTYLE);
		    BIOwriteByte (pivFile, pPRule->PrChrValue);
		    break;
		 default:
		    break;
	      }
     }
}

/* ---------------------------------------------------------------------- */
/* | Externalise effectue la traduction sous forme pivot du sous-arbre  | */
/* |    ou de l'element pointe par pEl.                                 | */
/* |    subTree indique si on veut ecrire le sous-arbre ou l'element    | */
/* |    seul.                                                           | */
/* |    Le fichier 'pivFile' ou est ecrit la representation pivot doit  | */
/* |    deja etre ouvert a l'appel et il n'est pas ferme au retour.     | */
/* |    Si l'element externalise est une feuille texte suivie d'autres  | */
/* |    feuilles de texte ayant les m^emes attributs, ces elements sont | */
/* |    externalise's sous la forme d'un seul element, et au retour,    | */
/* |    El pointe sur le dernier de ces elements successifs.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Externalise (BinFile pivFile, PtrElement * pEl, PtrDocument pDoc, boolean subTree)
#else  /* __STDC__ */
void                Externalise (pivFile, pEl, pDoc, subTree)
BinFile             pivFile;
PtrElement         *pEl;
PtrDocument         pDoc;
boolean             subTree;

#endif /* __STDC__ */
{
   int                 i, c;
   PtrElement          p;
   PtrTextBuffer       b;
   boolean             stop;
   PtrAttribute        pAttr;
   PtrPRule            pPRule;
   PtrElement          pEl1;
   PtrSSchema          pSc1;
   PtrTextBuffer       pBu1;
   NotifyElement       notifyEl;
   NotifyAttribute     notifyAttr;
   boolean             ecrire;

   ecrire = TRUE;		/* on ecrit effectivement la forme pivot de l'element */
   pEl1 = *pEl;
   if (ecrire)
     {
	/* ecrit la marque de type */
	BIOwriteByte (pivFile, (char) C_PIV_TYPE);
	/* ecrit le numero de la regle definissant le type */
	PutShort (pivFile, pEl1->ElTypeNumber);
	/* si c'est une copie d'element inclus, ecrit la reference a */
	/* l'element inclus */
	if (pEl1->ElSource != NULL)
	      /* ecrit la marque d'element inclus */
	     {
		BIOwriteByte (pivFile, (char) C_PIV_INCLUDED);
		PutReference (pivFile, pEl1->ElSource);
	     }
	/* ecrit la marque "Element-reference'" si l'element est */
	/* effectivement reference' */
	if (pEl1->ElReferredDescr != NULL)
	   if (pEl1->ElReferredDescr->ReFirstReference != NULL ||
	       pEl1->ElReferredDescr->ReExtDocRef != NULL)
	      /* l'element est effectivement reference' */
	      BIOwriteByte (pivFile, (char) C_PIV_REFERRED);
	/* ecrit le label de l'element */
	PutLabel (pivFile, pEl1->ElLabel);

	/* Ecrit la marque d'holophraste si l'element est holophraste' */
	if (pEl1->ElHolophrast)
	   BIOwriteByte (pivFile, (char) C_PIV_HOLOPHRAST);

	/* ecrit les attributs de l'element, mais pas les attributs imposes, */
	/* a moins qu'ils soient du type reference */
	pAttr = pEl1->ElFirstAttr;
	while (pAttr != NULL)
	  {
	     /* prepare et envoie l'evenement AttrSave.Pre s'il est demande' */
	     notifyAttr.event = TteAttrSave;
	     notifyAttr.document = (Document) IdentDocument (pDoc);
	     notifyAttr.element = (Element) pEl1;
	     notifyAttr.attribute = (Attribute) pAttr;
	     notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
	     notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
	     if (!CallEventAttribute (&notifyAttr, TRUE))
		/* l'application laisse l'editeur ecrire l'attribut */
	       {
		  /* ecrit l'attribut */
		  PutAttribut (pivFile, pAttr, pDoc);
		  /* prepare et envoie l'evenement AttrSave.Post s'il est demande' */
		  notifyAttr.event = TteAttrSave;
		  notifyAttr.document = (Document) IdentDocument (pDoc);
		  notifyAttr.element = (Element) pEl1;
		  notifyAttr.attribute = (Attribute) pAttr;
		  notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
		  notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
		  CallEventAttribute (&notifyAttr, FALSE);
	       }
	     /* passe a l'attribut suivant de l'element */
	     pAttr = pAttr->AeNext;
	  }
	/* ecrit les regles de presentation de l'element */
	pPRule = pEl1->ElFirstPRule;
	while (pPRule != NULL)
	  {
	     PutReglePres (pivFile, pPRule);
	     pPRule = pPRule->PrNextPRule;
	  }
	/* ecrit les commentaires associes a l'element, s'il y en a. */
	if (pEl1->ElComment != NULL)
	   PutComment (pivFile, pEl1->ElComment);
     }				/* fin de "if (ecrire)" */

   /* ecrit le contenu de l'element */
   if (pEl1->ElSource == NULL)
      /* on n'ecrit pas le contenu d'un element inclus */
     {
	pSc1 = pEl1->ElSructSchema;
	if (pEl1->ElTerminal)
	  {
	     if (ecrire)
	       {
		  /* feuille terminale: on ecrit son contenu entre C_PIV_BEGIN et C_PIV_END */
		  if (pSc1->SsRule[pEl1->ElTypeNumber - 1].SrConstruct != CsConstant
		      && !pSc1->SsRule[pEl1->ElTypeNumber - 1].SrParamElem)
		     /* on n'ecrit pas le texte des constantes, ni celui des */
		     /* parametres, il est cree automatiquement */
		    {
		       if (pEl1->ElLeafType == LtText)
			  /* ecrit le numero de langue de la feuille de texte, si ce */
			  /* n'est pas la premiere langue de la table du document */
			 {
			    i = 0;
			    /* cherche le rang de la langue dans la table du document */
			    while (pDoc->DocLanguages[i] != pEl1->ElLanguage &&
				   i < pDoc->DocNLanguages)
			       i++;
			    if (i > 0)
			      {
				 BIOwriteByte (pivFile, (char) C_PIV_LANG);
				 BIOwriteByte (pivFile, (char) i);
			      }
			 }
		       if (pEl1->ElLeafType != LtReference)
			  BIOwriteByte (pivFile, (char) C_PIV_BEGIN);
		       switch (pEl1->ElLeafType)
			     {
				case LtPicture:
				case LtText:
				   /* ecrit dans le fichier le texte des buffers de l'element */
				   /* ou le nom de l'image */
				   do
				     {
					c = 0;
					b = (*pEl)->ElText;
					while (c < (*pEl)->ElTextLength && b != NULL)
					  {
					     i = 1;
					     pBu1 = b;
					     while (pBu1->BuContent[i - 1] != '\0' && i <= pBu1->BuLength)
					       {
						  BIOwriteByte (pivFile, b->BuContent[i - 1]);
						  i++;
					       }
					     c = c + i - 1;
					     b = b->BuNext;
					     /* buffer suivant du meme element */
					  }
					/* peut-on concatener l'element suivant ? */
					stop = TRUE;
					if ((*pEl)->ElLeafType == LtText)
					   /* c'est du texte */
					   if ((*pEl)->ElNext != NULL)
					      /* il y a un suivant.. */
					      if ((*pEl)->ElNext->ElTerminal)
						 if ((*pEl)->ElNext->ElLeafType == LtText)
						    /* qui est une feuille de text */
						    if ((*pEl)->ElNext->ElLanguage == (*pEl)->ElLanguage)
						       if ((*pEl)->ElNext->ElSource == NULL)
							  /* le suivant n'est pas une inclusion */
							  if ((*pEl)->ElSructSchema->SsRule[(*pEl)->ElTypeNumber - 1].SrConstruct != CsConstant)
							     if ((*pEl)->ElNext->ElSructSchema->SsRule[(*pEl)->ElNext->ElTypeNumber - 1].SrConstruct != CsConstant)
								if (MemesAttributs (*pEl, (*pEl)->ElNext))
								   /* il a les memes attributs */
								   if (MemesRegleSpecif (*pEl, (*pEl)->ElNext))
								      /* il a les memes regles de */
								      /* presentation specifique  */
								      if ((*pEl)->ElNext->ElComment == NULL)
									 if ((*pEl)->ElComment == NULL)
									    /* aucun des deux n'a de */
									    /* commentaires, on concatene */
									   {
									      stop = FALSE;
									      *pEl = (*pEl)->ElNext;
									   }
				     }
				   while (!stop);
				   BIOwriteByte (pivFile, '\0');
				   break;
				case LtReference:
				   /* ecrit une marque de reference et le label de */
				   /* l'element qui est reference' */
				   BIOwriteByte (pivFile, (char) C_PIV_REFERENCE);
				   PutReference (pivFile, pEl1->ElReference);
				   break;
				case LtSymbol:
				case LtGraphics:
				   /* ecrit le code du symbole ou du graphique */
				   BIOwriteByte (pivFile, pEl1->ElGraph);
				   break;
				case LtPageColBreak:
				   /* ecrit le numero de page et le type de page */
				   PutShort (pivFile, pEl1->ElPageNumber);
				   PutShort (pivFile, pEl1->ElViewPSchema);
				   PutPageType (pivFile, pEl1->ElPageType);
				   PutBoolean (pivFile, pEl1->ElPageModified);
				   break;
				case LtPairedElem:
				   PutInteger (pivFile, pEl1->ElPairIdent);
				   break;
				case LtPlyLine:
				   /* ecrit le code representant la forme de la ligne */
				   BIOwriteByte (pivFile, pEl1->ElPolyLineType);
				   /* ecrit une marque indiquant que c'est une Polyline */
				   BIOwriteByte (pivFile, (char) C_PIV_POLYLINE);
				   /* ecrit le nombre de points de la ligne */
				   PutShort (pivFile, pEl1->ElNPoints);
				   /* ecrit tous les points */
				   c = 0;
				   b = pEl1->ElPolyLineBuffer;
				   while (c < pEl1->ElNPoints && b != NULL)
				     {
					for (i = 0; i < b->BuLength; i++)
					  {
					     PutInteger (pivFile, b->BuPoints[i].XCoord);
					     PutInteger (pivFile, b->BuPoints[i].YCoord);
					  }
					c += b->BuLength;
					b = b->BuNext;	/* buffer suivant du meme element */
				     }
				   break;
				default:
				   break;
			     }
		       if (pEl1->ElLeafType != LtReference)
			  BIOwriteByte (pivFile, (char) C_PIV_END);
		    }
	       }
	  }
	else
	   /* ce n'est pas un element terminal */
	if (subTree)
	   /* on veut ecrire les fils de l'element */
	   if (!pSc1->SsRule[pEl1->ElTypeNumber - 1].SrParamElem)
	      /* on n'ecrit pas le contenu des parametres */
	     {
		if (ecrire)
		   /* ecrit une marque de debut */
		   BIOwriteByte (pivFile, (char) C_PIV_BEGIN);
		p = pEl1->ElFirstChild;
		/* ecrit successivement la representation pivot de tous */
		/* les fils de l'element */
		while (p != NULL)
		  {
		     /* envoie l'evenement ElemSave.Pre a l'application, si */
		     /* elle le demande */
		     notifyEl.event = TteElemSave;
		     notifyEl.document = (Document) IdentDocument (pDoc);
		     notifyEl.element = (Element) p;
		     notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
		     notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
		     notifyEl.position = 0;
		     if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
			/* l'application accepte que Thot sauve l'element */
		       {
			  /* Ecrit d'abord le numero de la structure generique s'il y */
			  /* a changement de schema de structure par rapport au pere */
			  if (pEl1->ElSructSchema != p->ElSructSchema)
			     EcritNat (p->ElSructSchema, pivFile, pDoc);
			  /* Ecrit un element fils */
			  Externalise (pivFile, &p, pDoc, subTree);
			  /* envoie l'evenement ElemSave.Post a l'application, si */
			  /* elle le demande */
			  notifyEl.event = TteElemSave;
			  notifyEl.document = (Document) IdentDocument (pDoc);
			  notifyEl.element = (Element) p;
			  notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
			  notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
			  notifyEl.position = 0;
			  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		       }
		     /* passe au fils suivant */
		     p = p->ElNext;
		  }
		/* ecrit une marque de fin */
		BIOwriteByte (pivFile, (char) C_PIV_END);
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PutName (BinFile pivFile, Name N)
#else  /* __STDC__ */
static void         PutName (pivFile, N)
BinFile             pivFile;
Name                 N;

#endif /* __STDC__ */
{
   int                 j;

   j = 1;
   while (j < MAX_NAME_LENGTH && N[j - 1] != '\0')
     {
	BIOwriteByte (pivFile, N[j - 1]);
	j++;
     }
   BIOwriteByte (pivFile, '\0');
}

/* ---------------------------------------------------------------------- */
/* | AddNature  met dans la table des natures du document pDoc          | */
/* |    les schemas references par le schema de structure pSchStr       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         AddNature (PtrSSchema pSchStr, PtrDocument pDoc)
#else  /* __STDC__ */
static void         AddNature (pSchStr, pDoc)
PtrSSchema        pSchStr;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 j, n, NbO;
   boolean             present;

#ifndef NODISPLAY
   PtrElement          pSauve;

#endif
   SRule              *pRe1;

   for (j = 0; j < pSchStr->SsNRules; j++)
     {
	pRe1 = &pSchStr->SsRule[j];
	if (pRe1->SrConstruct == CsNatureSchema)
	   if (pRe1->SrSSchemaNat != NULL)
	      if (pRe1->SrSSchemaNat->SsNObjects > 0)
		{
		   /* Decompte les objets de cette nature qui sont dans */
		   /* le buffer de Copier-Couper-Coller */
		   NbO = pRe1->SrSSchemaNat->SsNObjects;
#ifndef NODISPLAY
		   if (ElemSauve != NULL)
		     {
			pSauve = ElemSauve->PeElement;
			do
			  {
			     if (pSauve->ElSructSchema == pRe1->SrSSchemaNat
				 && pSauve->ElTypeNumber == pRe1->SrSSchemaNat->SsRootElem)
				NbO--;
			     pSauve = FwdSearchTypedElem (pSauve, pRe1->SrSSchemaNat->SsRootElem,
						 pRe1->SrSSchemaNat);
			  }
			while (pSauve != NULL);
		     }
#endif
		   if (NbO > 0)
		     {
			/* Si les natures contiennent elles-memes des natures  */
			/* on pourrait ecrire plusieurs fois un nom de nature. */
			/* On verifie que ce nom n'est pas dans la table       */
			n = 0;
			present = FALSE;
			while (n < pDoc->DocNNatures && !present)
			   if (strcmp (pDoc->DocNatureName[n],
				       pRe1->SrSSchemaNat->SsName) == 0)
			      present = TRUE;
			   else
			      n++;
			if (!present)	/* il n'y est pas */
			   /* met le schema dans la table */
			  {
			     if (pDoc->DocNNatures < MAX_NATURES_DOC)
			       {
				  strncpy (pDoc->DocNatureName[pDoc->DocNNatures],
				       pRe1->SrSSchemaNat->SsName, MAX_NAME_LENGTH);
				  strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
				      pRe1->SrSSchemaNat->SsDefaultPSchema, MAX_NAME_LENGTH);
				  pDoc->DocNatureSSchema[pDoc->DocNNatures] =
				     pRe1->SrSSchemaNat;
				  pDoc->DocNNatures++;
			       }
			  }
			/* cherche les natures utilisees par cette nature */
			/* meme si elle est deja dans la table : celle qui est */
			/* dans la table ne reference peut-etre pas des natures */
			/* qui sont referencees par celle-ci */
			AddNature (pRe1->SrSSchemaNat, pDoc);
		     }
		}
     }
}

/* ---------------------------------------------------------------------- */
/* | BuildDocNatureTable remplit la table des schemas utilises          | */
/* |            par le document pDoc.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                BuildDocNatureTable (PtrDocument pDoc)
#else  /* __STDC__ */
void                BuildDocNatureTable (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrSSchema        pSchExtens;

   /* met le schema de structure du document en tete de la table des */
   /* natures utilisees */
   pDoc->DocNatureSSchema[0] = pDoc->DocSSchema;
   strncpy (pDoc->DocNatureName[0], pDoc->DocSSchema->SsName, MAX_NAME_LENGTH);
   strncpy (pDoc->DocNaturePresName[0], pDoc->DocSSchema->SsDefaultPSchema, MAX_NAME_LENGTH);
   pDoc->DocNNatures = 1;
   /* met dans la table des natures du document les */
   /* extensions du schema de structure du document */
   pSchExtens = pDoc->DocSSchema->SsNextExtens;
   while (pSchExtens != NULL)
     {
	/* met ce schema d'extension dans la table des natures */
	if (pDoc->DocNNatures < MAX_NATURES_DOC)
	  {
	     strncpy (pDoc->DocNatureName[pDoc->DocNNatures],
		      pSchExtens->SsName, MAX_NAME_LENGTH);
	     strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
		      pSchExtens->SsDefaultPSchema, MAX_NAME_LENGTH);
	     pDoc->DocNatureSSchema[pDoc->DocNNatures] = pSchExtens;
	     pDoc->DocNNatures++;
	  }
	/* met dans la table les natures utilises par cette extension */
	AddNature (pSchExtens, pDoc);
	/* passe au schema d'extension suivant */
	pSchExtens = pSchExtens->SsNextExtens;
     }
   /* met dans la table des natures les schemas de structure */
   /* reference's par le schema de structure du document. */
   AddNature (pDoc->DocSSchema, pDoc);
}

/* ---------------------------------------------------------------------- */
/* | WriteNomsSchemasDoc ecrit dans le fichier pivFile les noms de tous    | */
/* |    les schemas de structure et de presentation utilises par le     | */
/* |    document pDoc.                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteNomsSchemasDoc (BinFile pivFile, PtrDocument pDoc)
#else  /* __STDC__ */
void                WriteNomsSchemasDoc (pivFile, pDoc)
BinFile             pivFile;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 n;

   BuildDocNatureTable (pDoc);
   /* ecrit les noms des natures utilisees dans le document. */
   for (n = 0; n < pDoc->DocNNatures; n++)
     {
	/* ecrit la marque de classe ou d'extension */
	if (pDoc->DocNatureSSchema[n]->SsExtension)
	   BIOwriteByte (pivFile, (char) C_PIV_SSCHEMA_EXT);
	else
	   BIOwriteByte (pivFile, (char) C_PIV_NATURE);
	/* ecrit le nom de schema de structure dans le fichier */
	PutName (pivFile, pDoc->DocNatureSSchema[n]->SsName);
	/* ecrit le code du schema de structure */
	PutShort (pivFile, pDoc->DocNatureSSchema[n]->SsCode);
	/* ecrit le nom du schema de presentation associe' */
	PutName (pivFile, pDoc->DocNatureSSchema[n]->SsDefaultPSchema);
     }
}


/* ---------------------------------------------------------------------- */
/* | LanguesDansTable met dans la table des langues du document pDoc    | */
/* |    toutes les langues utilisees dans l'arbre de racine RlRoot et   | */
/* |    qui ne sont pas encore dans la table.                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         LanguesDansTable (PtrDocument pDoc, PtrElement RlRoot)
#else  /* __STDC__ */
static void         LanguesDansTable (pDoc, RlRoot)
PtrDocument         pDoc;
PtrElement          RlRoot;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   int                 i;
   boolean             trouve;

   pEl = RlRoot;
   while (pEl != NULL)
     {
	pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	if (pEl != NULL)
	  {
	     trouve = FALSE;
	     for (i = 0; i < pDoc->DocNLanguages && !trouve; i++)
		if (pEl->ElLanguage == pDoc->DocLanguages[i])
		   trouve = TRUE;
	     if (!trouve)
		if (pDoc->DocNLanguages < MAX_LANGUAGES_DOC - 1)
		   pDoc->DocLanguages[pDoc->DocNLanguages++] = pEl->ElLanguage;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* | ecrit la table des langues utilisees par le document               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteTableLangues (BinFile pivFile, PtrDocument pDoc)
#else  /* __STDC__ */
void                WriteTableLangues (pivFile, pDoc)
BinFile             pivFile;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 i;

   pDoc->DocNLanguages = 0;
   for (i = 1; i <= MAX_PARAM_DOC; i++)
      LanguesDansTable (pDoc, pDoc->DocParameters[i - 1]);
   for (i = 1; i <= MAX_ASSOC_DOC; i++)
      LanguesDansTable (pDoc, pDoc->DocAssocRoot[i - 1]);
   LanguesDansTable (pDoc, pDoc->DocRootElement);
   for (i = 0; i < pDoc->DocNLanguages; i++)
     {
	BIOwriteByte (pivFile, (char) C_PIV_LANG);
	PutName (pivFile, TtaGetLanguageName (pDoc->DocLanguages[i]));
     }
}


/* ---------------------------------------------------------------------- */
/* | WritePivotHeader ecrit l'entet d'un fichier pivot                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WritePivotHeader (BinFile pivFile, PtrDocument pDoc)
#else  /* __STDC__ */
void                WritePivotHeader (pivFile, pDoc)
BinFile             pivFile;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   LabelString         label;

   /* ecrit le numero de version */
   WriteVersionNumber (pivFile);
   /* ecrit la valeur max. des labels */
   LabelIntToString (GetCurrentLabel (pDoc), label);
   PutLabel (pivFile, label);
   /* ecrit la table des langues utilisees par le document */
   WriteTableLangues (pivFile, pDoc);
}


/* ---------------------------------------------------------------------  */
/* | SauveDoc  sauve le document pointe par pDoc dans le fichier pivFile,  | */
/* | sous la forme pivot. Le fichier doit etre ouvert avant l'appel     | */
/* | et est toujours ouvert au retour.                                  | */
/* ---------------------------------------------------------------------  */
#ifdef __STDC__
void                SauveDoc (BinFile pivFile, PtrDocument pDoc)
#else  /* __STDC__ */
void                SauveDoc (pivFile, pDoc)
BinFile             pivFile;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          p, s;
   boolean             stop;
   NotifyElement       notifyEl;


   /* ecrit l'entete du fichier pivot */
   WritePivotHeader (pivFile, pDoc);
   /* ecrit le commentaire associe au fichier, s'il y en a un */
   if (pDoc->DocComment != NULL)
      PutComment (pivFile, pDoc->DocComment);
   /* ecrit les noms de tous les schemas de structure et de presentation */
   /* utilises par le document */
   WriteNomsSchemasDoc (pivFile, pDoc);

   /* ecrit la representation pivot de tous les parametres. */
   for (i = 1; i <= MAX_PARAM_DOC; i++)
      if (pDoc->DocParameters[i - 1] != NULL)
	{
	   p = pDoc->DocParameters[i - 1];
	   p->ElSructSchema->SsRule[p->ElTypeNumber - 1].SrParamElem = FALSE;
	   /* envoie l'evenement ElemSave.Pre a l'application, si */
	   /* elle le demande */
	   notifyEl.event = TteElemSave;
	   notifyEl.document = (Document) IdentDocument (pDoc);
	   notifyEl.element = (Element) p;
	   notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
	   notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
	   notifyEl.position = 0;
	   if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	      /* l'application accepte que Thot sauve l'element */
	     {
		BIOwriteByte (pivFile, (char) C_PIV_PARAM);
		/* Ecrit l'element */
		Externalise (pivFile, &p, pDoc, TRUE);
		/* envoie l'evenement ElemSave.Post a l'application, si */
		/* elle le demande */
		notifyEl.event = TteElemSave;
		notifyEl.document = (Document) IdentDocument (pDoc);
		notifyEl.element = (Element) p;
		notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
		notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
		notifyEl.position = 0;
		CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	     }
	   p->ElSructSchema->SsRule[p->ElTypeNumber - 1].SrParamElem = TRUE;
	}
   /* ecrit la representation pivot de tous les arbres d'elements */
   /* associes qui ne sont pas vides */
   for (i = 1; i <= MAX_ASSOC_DOC; i++)
      if (pDoc->DocAssocRoot[i - 1] != NULL)
	{
	   p = pDoc->DocAssocRoot[i - 1]->ElFirstChild;
	   if (p != NULL)
	      /* y a-t-il autre chose que des sauts de page ? */
	     {
		s = p;
		stop = FALSE;
		do
		   if (s == NULL)
		      stop = TRUE;
		   else if (s->ElTypeNumber == PageBreak + 1)
		      s = s->ElNext;
		   else
		      stop = TRUE;
		while (!stop);
		if (s != NULL)
		   /* il n'y a pas que des sauts de pages */
		  {
		     p = pDoc->DocAssocRoot[i - 1];
		     /* envoie l'evenement ElemSave.Pre a l'application, si */
		     /* elle le demande */
		     notifyEl.event = TteElemSave;
		     notifyEl.document = (Document) IdentDocument (pDoc);
		     notifyEl.element = (Element) p;
		     notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
		     notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
		     notifyEl.position = 0;
		     if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
			/* l'application accepte que Thot sauve l'element */
		       {
			  /* ecrit une marque d'element associe' */
			  BIOwriteByte (pivFile, (char) C_PIV_ASSOC);
			  /* si ces elements associes sont definis dans une extension */
			  /* du schema de structure du document, on ecrit un */
			  /* changement de nature */
			  if (p->ElSructSchema != pDoc->DocSSchema)
			     EcritNat (p->ElSructSchema, pivFile, pDoc);
			  /* Ecrit l'element */
			  Externalise (pivFile, &p, pDoc, TRUE);
			  /* envoie l'evenement ElemSave.Post a l'application, si */
			  /* elle le demande */
			  notifyEl.event = TteElemSave;
			  notifyEl.document = (Document) IdentDocument (pDoc);
			  notifyEl.element = (Element) p;
			  notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
			  notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
			  notifyEl.position = 0;
			  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		       }
		  }
	     }
	}
   /* ecrit la representation pivot de tout le corps du document */
   p = pDoc->DocRootElement;
   if (p != NULL)
     {
	/* envoie l'evenement ElemSave.Pre a l'application, si */
	/* elle le demande */
	notifyEl.event = TteElemSave;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) p;
	notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	   /* l'application accepte que Thot sauve l'element */
	  {
	     BIOwriteByte (pivFile, (char) C_PIV_STRUCTURE);
	     /* ecrit la forme pivot de tout l'arbre */
	     Externalise (pivFile, &p, pDoc, TRUE);
	     /* envoie l'evenement ElemSave.Post a l'application, si */
	     /* elle le demande */
	     notifyEl.event = TteElemSave;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) p;
	     notifyEl.elementType.ElTypeNum = p->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (p->ElSructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
   BIOwriteByte (pivFile, (char) C_PIV_DOC_END);
}


/* ---------------------------------------------------------------------- */
/* | SauveRefSortantes sauve dans le fichier de nom NomFich la liste des| */
/* | references du document pDoc qui designent des elements appartenant | */
/* | a d'autres documents.                                              | */
/* | Le fichier n'est ecrit que s'il y a effectivement des references   | */
/* | sortantes. Dans ce cas, chaque reference sortante est e'crite      | */
/* | dans le fichier sous la meme forme que dans le fichier pivot.      | */
/* | S'il n'y a aucune reference sortantne, le fichier est detruit.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SauveRefSortantes (char *NomFich, PtrDocument pDoc)
#else  /* __STDC__ */
void                SauveRefSortantes (NomFich, pDoc)
char               *NomFich;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   BinFile             refext;
   PtrReferredDescr    pDR;
   PtrReference        pRef;
   boolean             FichierOuvert;
   boolean             PasDeRefSortante;

   refext = NULL;
   pDR = pDoc->DocReferredEl;
   if (pDR != NULL)
      /* saute le premier descripteur d'element reference' bidon */
      pDR = pDR->ReNext;
   FichierOuvert = FALSE;
   /* le fichier n'est pas encore ouvert */
   PasDeRefSortante = TRUE;
   /* a priori, il n'y a pas de reference sortante */
   /* parcourt la chaine des descripteurs d'elements reference's */
   while (pDR != NULL)
      /* on ne considere que les elements reference's externes au document */
     {
	if (pDR->ReExternalRef)
	  {
	     pRef = pDR->ReFirstReference;
	     /* parcourt la chaine des references a cet element externe */
	     while (pRef != NULL)
		/* on ignore les references qui sont dans */
		/* le tampon de couper-coller */
	       {
		  if (!DansTampon (pRef->RdElement))
		    {
		       PasDeRefSortante = FALSE;
		       /* au moins une reference sortante */
		       /* ouvre le fichier si ce n'est pas encore fait */
		       if (!FichierOuvert)
			 {
			    /* ouvre le fichier */
			    refext = BIOwriteOpen (NomFich);
			    if (refext != 0)
			       FichierOuvert = TRUE;
			    else
			       /* ouverture fichier impossible */
			      {
				 TtaDisplayMessage (INFO, TtaGetMessage(LIB, WRITING_IMP), NomFich);
				 pDR = NULL;	/* abandon */
				 pRef = NULL;
			      }
			 }
		       if (FichierOuvert)
			  /* ecrit la ref dans le fichier */
			  PutReference (refext, pRef);
		    }
		  /* passe a la reference suivante au meme element */
		  if (pRef != NULL)
		     pRef = pRef->RdNext;
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	if (pDR != NULL)
	   pDR = pDR->ReNext;
     }
   /* fin du parcours des descripteurs d'elements reference's */
   if (FichierOuvert)
      BIOwriteClose (refext);
   /* vide le buffer d'entree-sortie */
   if (PasDeRefSortante)
      /* il n'y a pas de reference sortante, on detruit le fichier */
      RemoveFile (NomFich);
}

/* ---------------------------------------------------------------------- */
/* | SauveRef   ecrit dans le fichier de nom NomFich le fichier         | */
/* |    .REF dont la representation en memoire est pointee par PremChng.| */
/* |    Libere la representation en memoire                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SauveRef (PtrChangedReferredEl PremChng, PathBuffer NomFich)
#else  /* __STDC__ */
void                SauveRef (PremChng, NomFich)
PtrChangedReferredEl      PremChng;
PathBuffer          NomFich;

#endif /* __STDC__ */
{
   BinFile             ref;
   PtrChangedReferredEl      Chng, ChngSuiv;

   ref = BIOwriteOpen (NomFich);
   if (ref != 0)
     {
	/* le fichier est ouvert */
	Chng = PremChng;
	while (Chng != NULL)
	  {
	     /* ecrit l'ancien label de l'element */
	     PutLabel (ref, Chng->CrOldLabel);
	     /* ecrit le nouveau label de l'element */
	     PutLabel (ref, Chng->CrNewLabel);
	     /* ecrit une marque de nom de document */
	     BIOwriteByte (ref, (char) C_PIV_DOCNAME);
	     /* ecrit le nom de l'ancien document de l'element */
	     BIOwriteIdentDoc (ref, Chng->CrOldDocument);
	     /* ecrit une marque de nom de document */
	     BIOwriteByte (ref, (char) C_PIV_DOCNAME);
	     /* ecrit l'identificateur du nouveau document de l'element */
	     BIOwriteIdentDoc (ref, Chng->CrNewDocument);
	     /* on libere le descripteur qu'on vient d'ecrire */
	     ChngSuiv = Chng->CrNext;
	     FreeElemRefChng (Chng);
	     Chng = ChngSuiv;
	  }
	BIOwriteClose (ref);
     }
}


/* ---------------------------------------------------------------------- */
/* | SauveExt   ecrit dans le fichier de nom NomFich (de type .EXT)     | */
/* |    la liste des descripteurs d'elements reference's dont le premier| */
/* |    est pointe' par PremElemRef.                                    | */
/* |    Libere tous ces descripteurs.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SauveExt (PtrReferredDescr PremElemRef, PathBuffer NomFich)
#else  /* __STDC__ */
void                SauveExt (PremElemRef, NomFich)
PtrReferredDescr    PremElemRef;
PathBuffer          NomFich;

#endif /* __STDC__ */
{
   BinFile             refext;
   PtrReferredDescr    pDR;
   PtrExternalDoc       pDocExt;
   boolean             FichierOuvert;
   boolean             PasDeRefExt;
   PtrExternalDoc       DocExt, DocExtSuiv;
   PtrReferredDescr    ElemRef, ElemRefSuiv;

   refext = NULL;
   pDR = PremElemRef;
   FichierOuvert = FALSE;	/* le fichier n'est pas encore ouvert */
   PasDeRefExt = TRUE;		/*a priori, il n'y a pas de reference externe */
   /* parcourt la chaine des descripteurs d'elements reference's */
   while (pDR != NULL)
     {
	pDocExt = pDR->ReExtDocRef;
	if (pDocExt != NULL)
	   /* l'element est reference' par au moins un document externe */
	  {
	     PasDeRefExt = FALSE;	/* il y a au moins une reference externe */
	     /* ouvre le fichier si ce n'est pas encore fait */
	     if (!FichierOuvert)
	       {
		  /* ouvre le fichier */
		  refext = BIOwriteOpen (NomFich);
		  if (refext != 0)
		     FichierOuvert = TRUE;
		  else
		     /* ouverture fichier impossible */
		    {
		       TtaDisplayMessage (INFO, TtaGetMessage(LIB, WRITING_IMP),
						      NomFich);
		       pDR = NULL;	/* abandon */
		    }
	       }
	     if (FichierOuvert)
		/* ecrit le label de l'element reference' */
	       {
		  PutLabel (refext, pDR->ReReferredLabel);
		  /* parcourt la chaine des documents referencant l'element */
		  do
		    {
		       /* ecrit une marque de nom de document */
		       BIOwriteByte (refext, (char) C_PIV_DOCNAME);
		       /* ecrit le nom du document referencant */
		       BIOwriteIdentDoc (refext, pDocExt->EdDocIdent);
		       /* passe au descripteur de document referencant suivant */
		       pDocExt = pDocExt->EdNext;
		    }
		  while (pDocExt != NULL);
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	if (pDR != NULL)
	   pDR = pDR->ReNext;
     }				/* fin du parcours des descripteurs d'elements reference's */
   if (FichierOuvert)
      BIOwriteClose (refext);

   if (PasDeRefExt)
      /* il n'y a pas de reference externe, on detruit le fichier */
      RemoveFile (NomFich);
   /* libere la chaine de decripteurs d'elements reference's */
   ElemRef = PremElemRef;
   while (ElemRef != NULL)
     {
	/* libere la chaine de descripteurs de documents externes */
	DocExt = ElemRef->ReExtDocRef;
	while (DocExt != NULL)
	  {
	     DocExtSuiv = DocExt->EdNext;
	     /* libere le descripteur de document externe */
	     FreeDocExterne (DocExt);
	     DocExt = DocExtSuiv;
	  }
	/* libere le descripteur d'element reference' */
	ElemRefSuiv = ElemRef->ReNext;
	FreeDescReference (ElemRef);
	/* passe au descripteur d'element reference' suivant */
	ElemRef = ElemRefSuiv;
     }

}


/* ---------------------------------------------------------------------- */
/* | UpdateExt  Met a jour le fichier .EXT de chaque document contenant | */
/* | des elements reference's par les references externes du document   | */
/* | pDoc qui ont ete creees ou detruites depuis la derniere sauvegarde | */
/* | de ce document decidee par l'utilisateur (ou a defaut depuis le    | */
/* | chargement du document).                                           | */
/* | Un fichier .EXT contient, pour chaque element reference' de        | */
/* | l'exterieur,le label de cet element suivi des noms de tous les     | */
/* |  documents qui font reference a l'element.                         | */
/* | S'il n'y a aucune reference externe entrante, le fichier .EXT est  | */
/* | detruit.                                                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                UpdateExt (PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateExt (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   BinFile             refext;
   int                 i;
   PathBuffer          NomFich, NomDirectory;
   DocumentIdentifier     IdentDocExt;
   PtrOutReference      RefCreee, RefCreeePrec, RefCreeeSuiv;
   PtrOutReference      RefMorte, RefMortePrec, RefMorteSuiv;
   PtrOutReference      RefSort;
   PtrReferredDescr    PremElemRef, ElemRef;
   PtrExternalDoc       DocExt, DocExtPrec;
   boolean             trouve;

   /* parcourt plusieurs fois la liste des references sortantes creees */
   /* et la liste des references sortantes detruites, chaque liste une */
   /* fois pour chaque document externe qui y figure */
   while (pDoc->DocNewOutRef != NULL || pDoc->DocDeadOutRef != NULL)
     {
	/* positionne les pointeurs courants au debut de chacune des */
	/* deux listes */
	RefCreee = pDoc->DocNewOutRef;
	RefMorte = pDoc->DocDeadOutRef;
	RefCreeePrec = NULL;
	RefMortePrec = NULL;
	/* prend le nom du document externe qui est en tete de liste */
	if (RefCreee != NULL)
	   CopyIdentDoc (&IdentDocExt, RefCreee->OrDocIdent);
	else
	   CopyIdentDoc (&IdentDocExt, RefMorte->OrDocIdent);
	PremElemRef = NULL;
	/* Charge le fichier .EXT du document externe */
	/* demande d'abord dans quel directory se trouve le fichier .PIV */
	strncpy (NomDirectory, DirectoryDoc, MAX_PATH);
	BuildFileName (IdentDocExt, "PIV", NomDirectory, NomFich, &i);

	/* cherche le fichier .EXT dans le meme directory */
	DoFileName (IdentDocExt, "EXT", NomDirectory, NomFich, &i);
	if (NomFich[0] != '\0')
	  {
	     refext = BIOreadOpen (NomFich);
	     if (refext != 0)
	       {
		  /* le fichier .EXT existe, on le charge */
		  ChargeExt (refext, NULL, &PremElemRef, FALSE);
		  BIOreadClose (refext);
	       }
	  }
	/* parcourt les deux listes de references sortantes et traite */
	/* toutes les references sortantes (creees ou detruites) qui */
	/* design(ai)ent un element du document externe courant */
	while (RefCreee != NULL || RefMorte != NULL)
	  {
	     if (RefCreee != NULL)
		RefSort = RefCreee;
	     else
		RefSort = RefMorte;
	     if (!MemeIdentDoc (RefSort->OrDocIdent, IdentDocExt))
		/* cette reference sortante ne designe pas le document */
		/* externe courant, on passe a la suivante */
		if (RefSort == RefCreee)
		  {
		     RefCreeePrec = RefCreee;
		     RefCreee = RefCreee->OrNext;
		  }
		else
		  {
		     RefMortePrec = RefMorte;
		     RefMorte = RefMorte->OrNext;
		  }
	     else
		/* cette reference designe le document externe courant */
	       {
		  /* cherche si l'element designe' par la reference sortante */
		  /* traitee a un descripteur d'element reference' dans le */
		  /* document externe */
		  ElemRef = PremElemRef;
		  trouve = FALSE;
		  while (!trouve && ElemRef != NULL)
		     if (strcmp (RefSort->OrLabel, ElemRef->ReReferredLabel) == 0)
			trouve = TRUE;
		     else
			ElemRef = ElemRef->ReNext;
		  if (!trouve)
		     /* l'element designe' n'a pas de descripteur d'element */
		     /* reference' */
		    {
		       if (RefSort == RefCreee)
			  /* il s'agit d'une reference creee, on ajoute un */
			  /* descripteur d'element reference' */
			 {
			    GetDescReference (&ElemRef);
			    strncpy (ElemRef->ReReferredLabel, RefSort->OrLabel, MAX_LABEL_LEN);
			    /* chaine le descripteur en tete */
			    ElemRef->ReNext = PremElemRef;
			    ElemRef->RePrevious = NULL;
			    if (ElemRef->ReNext != NULL)
			       ElemRef->ReNext->RePrevious = ElemRef;
			    PremElemRef = ElemRef;
			    /* associe a ce descripteur un 1er descripteur de */
			    /* document referencant */
			    GetDocExterne (&DocExt);
			    CopyIdentDoc (&DocExt->EdDocIdent, pDoc->DocIdent);
			    ElemRef->ReExtDocRef = DocExt;
			 }
		       /* s'il s'agit d'une reference detruite, on ne devrait */
		       /* pas arriver la... */
		    }
		  else
		     /* l'element designe' possede un descripteur d'element */
		     /* reference' */
		    {
		       /* le document est-il dans les documents referencants ? */
		       DocExt = ElemRef->ReExtDocRef;
		       DocExtPrec = NULL;
		       trouve = FALSE;
		       while (!trouve && DocExt != NULL)
			  if (MemeIdentDoc (DocExt->EdDocIdent, pDoc->DocIdent))
			     trouve = TRUE;
			  else
			    {
			       DocExtPrec = DocExt;
			       DocExt = DocExt->EdNext;
			    }
		       if (trouve)
			  /* le document figure parmi les documents referencants */
			 {
			    /* s'il s'agit d'une reference creee, on ne fait rien */
			    if (RefSort == RefMorte)
			       /* il s'agit d'une reference detruite, on enleve le */
			       /* descripteur de document referencant */
			      {
				 if (DocExtPrec != NULL)
				    DocExtPrec->EdNext = DocExt->EdNext;
				 else
				   {
				      ElemRef->ReExtDocRef = DocExt->EdNext;
				      if (ElemRef->ReExtDocRef == NULL)
					 /* c'etait le dernier descripteur de document */
					 /* referencant, on enleve le descripteur */
					 /* d'element reference' */
					{
					   if (ElemRef == PremElemRef)
					     {
						PremElemRef = ElemRef->ReNext;
						if (ElemRef->ReNext != NULL)
						   ElemRef->ReNext->RePrevious = NULL;
					     }
					   else
					     {
						ElemRef->RePrevious->ReNext =
						   ElemRef->ReNext;
						if (ElemRef->ReNext != NULL)
						   ElemRef->ReNext->RePrevious =
						      ElemRef->RePrevious;
					     }
					   FreeDescReference (ElemRef);
					   ElemRef = NULL;
					}
				   }
				 FreeDocExterne (DocExt);
			      }
			 }
		       else
			  /* le document ne figure pas parmi les documents */
			  /* referencants */
			  /* s'il s'agit d'une reference detruite,on ne fait rien */
		       if (RefSort == RefCreee)
			  /* il s'agit d'une reference creee, on ajoute un */
			  /* descripteur de document referencant */
			 {
			    GetDocExterne (&DocExt);
			    CopyIdentDoc (&DocExt->EdDocIdent, pDoc->DocIdent);
			    DocExt->EdNext = ElemRef->ReExtDocRef;
			    ElemRef->ReExtDocRef = DocExt;
			 }
		    }
		  /* dechaine et supprime cette reference sortante traitee */
		  if (RefSort == RefCreee)
		    {
		       RefCreeeSuiv = RefCreee->OrNext;
		       if (RefCreeePrec == NULL)
			  pDoc->DocNewOutRef = RefCreee->OrNext;
		       else
			  RefCreeePrec->OrNext = RefCreee->OrNext;
		       FreeRefSortante (RefCreee);
		       RefCreee = RefCreeeSuiv;
		    }
		  else
		    {
		       RefMorteSuiv = RefMorte->OrNext;
		       if (RefMortePrec == NULL)
			  pDoc->DocDeadOutRef = RefMorte->OrNext;
		       else
			  RefMortePrec->OrNext = RefMorte->OrNext;
		       FreeRefSortante (RefMorte);
		       RefMorte = RefMorteSuiv;
		    }
	       }
	  }
	/* on a traite' toutes les references sortantes creees et */
	/* detruites de pDoc qui designaient des elements du document */
	/* externe courant. On sauve maintenant les elements reference's */
	/* de ce document dans son fichier .EXT */
	SauveExt (PremElemRef, NomFich);
     }
}

/* ---------------------------------------------------------------------- */
/* | UpdateRef  Met a jour le fichier .REF de chaque document contenant | */
/* | des references a des elements qui appartenaient au document pointe'| */
/* | par pDoc et qui ont ete detruits ou ont change' de document depuis | */
/* | la derniere sauvegarde de ce document decidee par l'utilisateur    | */
/* | (ou a defaut depuis le chargement du document).                    | */
/* | (Un fichier .REF contient un enregistrement pour chaque reference  | */
/* | dont l'element cible a ete detruit ou a change' de document).      | */
/* | met egalement a jour le fichier .EXT du document pour prendre en   | */
/* | compte les elements reference's qui ont ete detruits ou colle's    | */
/* | dans ce document.                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                UpdateRef (PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateRef (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PathBuffer          NomFich;
   PathBuffer          NomDirectory;
   PtrEnteringReferences     DescFichExt;
   PtrChangedReferredEl      Chng, ChngSuiv;
   PtrReferenceChange      PremFich, Fich, FichSuiv;
   PtrExternalDoc       DocExt, DocExtSuiv, DocExtOrig, DocExtPrec;
   boolean             trouve;
   BinFile             ref;
   BinFile             ext;
   int                 i;
   PtrChangedReferredEl      ChngLu, ChngNouv;
   PtrReferredDescr    ElemRef;

   if (pDoc->DocChangedReferredEl != NULL)
      /* des elements reference's ont effectivement ete touches */
     {
	/* on va charger le fichier .EXT du document */
	/* acquiert d'abord un descripteur de ce fichier */
	GetRefEntrantes (&DescFichExt);
	/* ce fichier est dans le meme directory que le document */
	strncpy (NomDirectory, pDoc->DocDirectory, MAX_PATH);
	DoFileName (pDoc->DocDName, "EXT", NomDirectory, NomFich, &i);
	/* initialise le descripteur du fichier .EXT */
	DescFichExt->ErFirstReferredEl = NULL;
	CopyIdentDoc (&DescFichExt->ErDocIdent, pDoc->DocIdent);
	strncpy (DescFichExt->ErFileName, NomFich, MAX_PATH);
	if (NomFich[0] != '\0')
	  {
	     ext = BIOreadOpen (NomFich);
	     if (ext != 0)
		/* ce fichier existe, on le charge */
	       {
		  ChargeExt (ext, NULL, &(DescFichExt->ErFirstReferredEl), FALSE);
		  BIOreadClose (ext);
	       }
	  }
	Chng = pDoc->DocChangedReferredEl;
	PremFich = NULL;
	/* parcourt la liste des elements reference's change's */
	while (Chng != NULL)
	  {
	     /* enregistre la modification dans la forme en memoire du */
	     /* fichier .EXT */
	     if (Chng->CrNewLabel[0] == '\0')
		/* cet element reference' a ete detruit, on supprime son */
		/* descripteur d'element reference' */
	       {
		  /* cherche le descripteur */
		  ElemRef = DescFichExt->ErFirstReferredEl;
		  while (ElemRef != NULL)
		     if (strcmp (Chng->CrOldLabel, ElemRef->ReReferredLabel) == 0)
			/* supprime ce descripteur */
		       {
			  /* on le retire d'abord de sa chaine */
			  if (ElemRef->RePrevious == NULL)
			     DescFichExt->ErFirstReferredEl = ElemRef->ReNext;
			  else
			     ElemRef->RePrevious->ReNext = ElemRef->ReNext;
			  if (ElemRef->ReNext != NULL)
			     ElemRef->ReNext->RePrevious = ElemRef->RePrevious;
			  /* on libere les descripteurs de documents externes */
			  DocExt = ElemRef->ReExtDocRef;
			  while (DocExt != NULL)
			    {
			       DocExtSuiv = DocExt->EdNext;
			       /* libere le descripteur de document externe */
			       FreeDocExterne (DocExt);
			       DocExt = DocExtSuiv;
			    }
			  /* libere le descripteur d'element reference' */
			  FreeDescReference (ElemRef);
			  ElemRef = NULL;
		       }
		     else
			/* passe au descripteur suivant */
			ElemRef = ElemRef->ReNext;
	       }
	     else
		/* cet element reference' a ete colle', on lui cree un */
		/* descripteur d'element reference' */
	       {

		  GetDescReference (&ElemRef);
		  strncpy (ElemRef->ReReferredLabel, Chng->CrNewLabel, MAX_LABEL_LEN);
		  /* chaine le descripteur en tete */
		  ElemRef->ReNext = DescFichExt->ErFirstReferredEl;
		  ElemRef->RePrevious = NULL;
		  if (ElemRef->ReNext != NULL)
		     ElemRef->ReNext->RePrevious = ElemRef;
		  DescFichExt->ErFirstReferredEl = ElemRef;
		  /* copie la chaine des descripteurs de documents externes */
		  DocExtOrig = Chng->CrReferringDoc;
		  DocExtPrec = NULL;
		  while (DocExtOrig != NULL)
		    {
		       GetDocExterne (&DocExt);
		       CopyIdentDoc (&DocExt->EdDocIdent, DocExtOrig->EdDocIdent);
		       /* chaine la copie */
		       if (DocExtPrec == NULL)
			  ElemRef->ReExtDocRef = DocExt;
		       else
			  DocExtPrec->EdNext = DocExt;
		       DocExtPrec = DocExt;
		       /* original suivant */
		       DocExtOrig = DocExtOrig->EdNext;
		    }
	       }
	     DocExt = Chng->CrReferringDoc;
	     /* traite tous les documents qui font reference a cet element */
	     while (DocExt != NULL)
	       {
		  /* le fichier .REF de ce document est-il deja charge'? */
		  trouve = FALSE;
		  Fich = PremFich;
		  while (Fich != NULL && !trouve)
		     if (MemeIdentDoc (DocExt->EdDocIdent, Fich->RcDocIdent))
			trouve = TRUE;
		     else
			Fich = Fich->RcNext;
		  if (!trouve)
		     /* le fichier .REF n'est pas charge', on le charge */
		    {
		       GetFichRefChng (&Fich);
		       Fich->RcNext = PremFich;
		       PremFich = Fich;
		       Fich->RcFirstChange = NULL;
		       CopyIdentDoc (&Fich->RcDocIdent, DocExt->EdDocIdent);
		       /* demande d'abord dans quel directory se trouve le */
		       /* fichier .PIV de ce document */
		       strncpy (NomDirectory, DirectoryDoc, MAX_PATH);
		       BuildFileName (Fich->RcDocIdent, "PIV", NomDirectory, NomFich, &i);
		       /* cherche le fichier .REF dans le meme directory */
		       DoFileName (Fich->RcDocIdent, "REF", NomDirectory, NomFich, &i);
		       strncpy (Fich->RcFileName, NomFich, MAX_PATH);
		       if (NomFich[0] != '\0')
			 {
			    ref = BIOreadOpen (NomFich);
			    if (ref != 0)
			      {
				 /* le fichier .REF existe, on le charge */
				 ChargeRef (ref, &ChngLu);
				 BIOreadClose (ref);
				 Fich->RcFirstChange = ChngLu;
			      }
			 }
		    }
		  if (Fich != NULL)
		    {
		       /* cherche dans le fichier .REF charge', si l'element */
		       /* change' ne figure pas deja */
		       ChngLu = Fich->RcFirstChange;
		       trouve = FALSE;
		       while (ChngLu != NULL && !trouve)
			 {
			    if (strcmp (ChngLu->CrOldLabel, Chng->CrOldLabel) == 0)
			       if (MemeIdentDoc (ChngLu->CrOldDocument, Chng->CrOldDocument))
				  /* il s'agit du meme ancien element */
				  if (ChngLu->CrNewLabel[0] == '\0')
				     /* on a lu une destruction */
				    {
				       trouve = TRUE;
				       if (Chng->CrNewLabel[0] != '\0')
					  /* c'est un deplacement, on transforme la */
					  /* destruction en deplacement */
					 {
					    strncpy (ChngLu->CrNewLabel,
						     Chng->CrNewLabel, MAX_LABEL_LEN);
					    CopyIdentDoc (&ChngLu->CrNewDocument,
							Chng->CrNewDocument);
					 }
				    }
				  else
				     /* on a lu un changement de document */
				    {
				       if (Chng->CrNewLabel[0] == '\0')
					  /* c'est la destruction de l'element, on */
					  /* l'ignore : le deplacement a priorite' */
					  /* meme s'il est enregistre' apres */
					  trouve = TRUE;
				    }
			    if (!trouve)
			       if (ChngLu->CrNewLabel[0] != '\0' &&
				   Chng->CrNewLabel[0] != '\0')
				  /* ce sont deux deplacements d'element */
				  if (strcmp (ChngLu->CrNewLabel, Chng->CrOldLabel) == 0)
				     if (MemeIdentDoc (ChngLu->CrNewDocument, Chng->CrOldDocument))
					/* deux deplacements successifs du meme element */
					/* on reduit a un seul deplacement */
				       {
					  trouve = TRUE;
					  strncpy (ChngLu->CrNewLabel, Chng->CrNewLabel, MAX_LABEL_LEN);
					  CopyIdentDoc (&ChngLu->CrNewDocument,
							Chng->CrNewDocument);
				       }
			    if (!trouve)
			       ChngLu = ChngLu->CrNext;
			 }
		       if (!trouve)
			 {
			    /* enregistre le changement dans le fichier .REF charge' */
			    GetElemRefChng (&ChngNouv);
			    ChngNouv->CrNext = Fich->RcFirstChange;
			    Fich->RcFirstChange = ChngNouv;
			    strncpy (ChngNouv->CrOldLabel, Chng->CrOldLabel, MAX_LABEL_LEN);
			    strncpy (ChngNouv->CrNewLabel, Chng->CrNewLabel, MAX_LABEL_LEN);
			    CopyIdentDoc (&ChngNouv->CrOldDocument, Chng->CrOldDocument);
			    CopyIdentDoc (&ChngNouv->CrNewDocument, Chng->CrNewDocument);
			 }
		    }
		  DocExtSuiv = DocExt->EdNext;
		  /* libere le descripteur de document externe */
		  FreeDocExterne (DocExt);
		  /* passe au document externe suivant */
		  DocExt = DocExtSuiv;
	       }
	     ChngSuiv = Chng->CrNext;
	     /* libere le descripteur qui a ete traite' */
	     FreeElemRefChng (Chng);
	     /* passe au descripteur suivant */
	     Chng = ChngSuiv;
	  }
	/* tous les descripteurs du document ont ete traites */
	pDoc->DocChangedReferredEl = NULL;
	/* ecrit le fichier .EXT mis a jour */
	SauveExt (DescFichExt->ErFirstReferredEl, DescFichExt->ErFileName);
	/* rend le descripteur de ce fichier */
	FreeRefEntrantes (DescFichExt);
	/* ecrit les fichiers .REF mis a jour */
	Fich = PremFich;
	while (Fich != NULL)
	  {
	     /* ecrit ce fichier .REF */
	     if (Fich->RcFirstChange != NULL)
		SauveRef (Fich->RcFirstChange, Fich->RcFileName);
	     FichSuiv = Fich->RcNext;
	     /* libere le descripteur de fichier */
	     FreeFichRefChng (Fich);
	     /* passe au fichier suivant */
	     Fich = FichSuiv;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* | ChangeNomExt       Le document pointe' par pDoc change de nom.     | */
/* | Son ancien nom est dans pDoc->DocDName, son nouveau nom est          | */
/* | dans NouveauNom.                                                   | */
/* | Si CopieDoc est vrai, il s'agit d'une copie de document : dans     | */
/* | tous les fichiers .EXT qui contiennent l'ancien nom on ajoute      | */
/* | le nouveau nom                                                     | */
/* | Si CopieDoc est faux, il s'agit d'un renommage du document : on    | */
/* | change le nom du document dans tous les fichiers .EXT qui          | */
/* | contiennent le nom de ce document.                                 | */
/* | Dans les deux cas, on met a jour le nom du document dans les       | */
/* | descripteurs de document externes des autres documents charge's    | */
/* | en memoire.                                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeNomExt (PtrDocument pDoc, Name NouveauNom, boolean CopieDoc)
#else  /* __STDC__ */
void                ChangeNomExt (pDoc, NouveauNom, CopieDoc)
PtrDocument         pDoc;
Name                 NouveauNom;
boolean             CopieDoc;

#endif /* __STDC__ */
{
   BinFile             ext;
   PtrEnteringReferences     PremExt, Ext, ExtSuiv;
   PtrReferredDescr    ElemRef, PremElemRef, ElemRefExt;
   PathBuffer          NomFich, NomDirectory;
   int                 i;
   boolean             charge, trouve;
   PtrExternalDoc       DocExt, DocExtOrig;
   PtrDocument         pDocExt;

   PremExt = NULL;
   ElemRef = pDoc->DocReferredEl;
   if (ElemRef != NULL)
      /* saute le 1er descripteur, bidon */
      ElemRef = ElemRef->ReNext;
   /* cherche tous les descripteurs d'elements reference's externes */
   while (ElemRef != NULL)
     {
	if (ElemRef->ReExternalRef)
	   /* il s'agit d'un element reference' externe */
	  {
	     /* le document contenant cet element externe est-il charge' ? */
	     pDocExt = GetPtrDocument (ElemRef->ReExtDocument);
	     if (pDocExt != NULL)
		/* le document externe est charge' */
	       {
		  /* cherche dans ce document externe le descripteur de */
		  /* l'element reference' */
		  ElemRefExt = pDocExt->DocReferredEl;
		  if (ElemRefExt != NULL)
		     /* saute le 1er descripteur, bidon */
		     ElemRefExt = ElemRefExt->ReNext;
		  trouve = FALSE;
		  while (ElemRefExt != NULL && !trouve)
		    {
		       if (!ElemRefExt->ReExternalRef)
			  /* il s'agit d'un element reference' appartenant au */
			  /* document */
			  if (ElemRefExt->ReReferredElem != NULL)
			     if (strcmp (ElemRefExt->ReReferredElem->ElLabel, ElemRef->ReReferredLabel) == 0)
				/* c'est le descripteur de notre element */
				trouve = TRUE;
		       if (!trouve)
			  /* passe au descripteur d'element reference' suivant */
			  /* dans le document externe */
			  ElemRefExt = ElemRefExt->ReNext;
		    }
		  if (trouve)
		     /* cherche le descripteur de document contenant des */
		     /* references a l'element et qui represente le document */
		     /* qui change de nom */
		    {
		       trouve = FALSE;
		       DocExt = ElemRefExt->ReExtDocRef;
		       while (DocExt != NULL && !trouve)
			 {
			    if (MemeIdentDoc (DocExt->EdDocIdent, pDoc->DocIdent))
			      {
				 trouve = TRUE;
				 /* met a jour le nom du document dans le */
				 /* descripteur de document externe */
				 CopyIdentDoc (&DocExt->EdDocIdent, NouveauNom);
			      }
			    else
			       DocExt = DocExt->EdNext;
			 }
		    }
	       }
	     /* le fichier .EXT du document contenant cet element est-il */
	     /* charge' ? */
	     charge = FALSE;
	     Ext = PremExt;
	     /* parcourt la liste des fichiers .EXT charge's */
	     while (Ext != NULL && !charge)
		if (MemeIdentDoc (Ext->ErDocIdent, ElemRef->ReExtDocument))
		   charge = TRUE;
		else
		   Ext = Ext->ErNext;
	     if (!charge)
		/* le fichier .EXT de ce document n'est pas encore charge' */
	       {
		  /* charge le fichier .EXT du document externe */
		  PremElemRef = NULL;
		  /* demande d'abord dans quel directory se trouve le */
		  /* fichier .PIV */
		  strncpy (NomDirectory, DirectoryDoc, MAX_PATH);
		  BuildFileName (ElemRef->ReExtDocument, "PIV", NomDirectory,
				 NomFich, &i);
		  /* cherche le fichier .EXT dans le meme directory */
		  DoFileName (ElemRef->ReExtDocument, "EXT", NomDirectory, NomFich, &i);
		  if (NomFich[0] != '\0')
		    {
		       ext = BIOreadOpen (NomFich);
		       if (ext != 0)
			 {
			    /* ce fichier existe, on le charge */
			    ChargeExt (ext, NULL, &PremElemRef, FALSE);
			    BIOreadClose (ext);
			 }
		    }
		  if (PremElemRef != NULL)
		     /* on a effectivement charge' le fichier .EXT */
		    {
		       /* on garde la chaine de descripteurs chargee et le */
		       /* nom du fichier NomFich */
		       GetRefEntrantes (&Ext);
		       Ext->ErNext = PremExt;
		       PremExt = Ext;
		       Ext->ErFirstReferredEl = PremElemRef;
		       CopyIdentDoc (&Ext->ErDocIdent, ElemRef->ReExtDocument);
		       strncpy (Ext->ErFileName, NomFich, MAX_PATH);
		    }
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	ElemRef = ElemRef->ReNext;
     }
   /* parcourt toutes les chaines de descripteurs chargees */
   Ext = PremExt;
   while (Ext != NULL)
     {
	/* parcourt la chaine des descripteur d'elements reference's */
	ElemRef = Ext->ErFirstReferredEl;
	while (ElemRef != NULL)
	  {
	     /* parcourt la chaine des descripteurs de documents */
	     /* referencant l'element treference' courant */
	     DocExt = ElemRef->ReExtDocRef;
	     while (DocExt != NULL)
	       {
		  if (MemeIdentDoc (DocExt->EdDocIdent, pDoc->DocIdent))
		     /* il s'agit de notre document */
		    {
		       if (CopieDoc)
			 {
			    /* ajoute un descripteur de document referencant */
			    DocExtOrig = DocExt;
			    GetDocExterne (&DocExt);
			    DocExt->EdNext = DocExtOrig->EdNext;
			    DocExtOrig->EdNext = DocExt;
			 }
		       /* met le nouveau nom du document referencant */
		       CopyIdentDoc (&DocExt->EdDocIdent, NouveauNom);
		    }
		  DocExt = DocExt->EdNext;
	       }
	     ElemRef = ElemRef->ReNext;
	  }
	/* ecrit le fichier .EXT traite' */
	SauveExt (Ext->ErFirstReferredEl, Ext->ErFileName);
	ExtSuiv = Ext->ErNext;
	FreeRefEntrantes (Ext);
	/* passe au fichier .EXT suivant en memoire */
	Ext = ExtSuiv;
     }
}

/* ---------------------------------------------------------------------- */
/* | ChangeNomRef       Le document pointe' par pDoc change de nom.     | */
/* | Son ancien nom est dans pDoc->DocDName, son nouveau nom est          | */
/* | dans NouveauNom.                                                   | */
/* | Indique le changement de nom dans les fichiers .REF de tous les    | */
/* | documents qui referencent celui qui change de nom.                 | */
/* | Si certains de ces documents referencant sont charge's, on met     | */
/* | a jour le nom de document dans leurs descripteurs d'element        | */
/* | reference's qui representent des elements se trouvant dans le      | */
/* | document qui change de nom                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeNomRef (PtrDocument pDoc, Name NouveauNom)
#else  /* __STDC__ */
void                ChangeNomRef (pDoc, NouveauNom)
PtrDocument         pDoc;
Name                 NouveauNom;

#endif /* __STDC__ */
{
   BinFile             ref;
   PtrReferredDescr    pDR;
   PtrExternalDoc       DocExt;
   PtrReferenceChange      PremFichREF, Fich, FichSuiv;
   PtrChangedReferredEl      Chng, ChngPrec;
   PtrDocument         pDocExt;
   PtrReferredDescr    ElemRefExt;
   PathBuffer          NomFich, NomDirectory;
   boolean             trouve;
   int                 i;

   PremFichREF = NULL;
   /* parcourt tous les descripteurs d'elements reference's et pour */
   /* chacun parcourt la liste des documents referencant */
   /* saute d'abord le premier descripteur, bidon */
   pDR = pDoc->DocReferredEl;
   if (pDR != NULL)
      pDR = pDR->ReNext;
   /* parcourt les descripteurs d'elements reference's */
   while (pDR != NULL)
     {
	if (!pDR->ReExternalRef)
	   /* c'est un element reference' interne */
	  {
	     DocExt = pDR->ReExtDocRef;
	     /* parcourt la liste des documents referencant */
	     while (DocExt != NULL)
	       {
		  /* ce document referencant a-t-il deja ete rencontre' ? */
		  trouve = FALSE;
		  Fich = PremFichREF;
		  while (Fich != NULL && !trouve)
		     if (MemeIdentDoc (DocExt->EdDocIdent, Fich->RcDocIdent))
			trouve = TRUE;
		     else
			Fich = Fich->RcNext;
		  if (!trouve)
		     /* le document referencant n'a pas encore ete rencontre' */
		     /* on le met dans la liste des document referencant */
		    {
		       GetFichRefChng (&Fich);
		       Fich->RcNext = PremFichREF;
		       PremFichREF = Fich;
		       Fich->RcFirstChange = NULL;
		       CopyIdentDoc (&Fich->RcDocIdent, DocExt->EdDocIdent);
		       /* demande dans quel directory se trouve le fichier */
		       /* .PIV de ce document */
		       strncpy (NomDirectory, DirectoryDoc, MAX_PATH);
		       BuildFileName (Fich->RcDocIdent, "PIV", NomDirectory, NomFich, &i);
		       /* cherche le fichier .REF dans le meme directory */
		       DoFileName (Fich->RcDocIdent, "REF", NomDirectory, Fich->RcFileName, &i);
		    }
		  /* passe au descripteur de document referencant suivant */
		  DocExt = DocExt->EdNext;
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	pDR = pDR->ReNext;
     }
   /* Tous les documents referencant sont dans la liste. On charge */
   /* leurs fichiers .REF, on y ajoute un enregistrement indiquant */
   /* le changement de nom du document et on les ecrit */
   Fich = PremFichREF;
   while (Fich != NULL)		/* parcourt la liste */
     {
	/* ce document referencant est-il charge' ? */
	pDocExt = GetPtrDocument (Fich->RcDocIdent);
	if (pDocExt != NULL)
	   /* le document referencant est charge' */
	  {
	     /* cherche dans ce document referencant les descripteurs */
	     /* d'elements reference's appartenant au document qui */
	     /* change de nom */
	     ElemRefExt = pDocExt->DocReferredEl;
	     if (ElemRefExt != NULL)
		/* saute le 1er descripteur, bidon */
		ElemRefExt = ElemRefExt->ReNext;
	     while (ElemRefExt != NULL)
	       {
		  if (ElemRefExt->ReExternalRef)
		     /* il s'agit d'un element reference' externe */
		     if (strcmp (ElemRefExt->ReExtDocument, pDoc->DocIdent) == 0)
			/* l'element reference' se trouve dans le document */
			/* qui change de nom, on change le nom */
			strncpy (ElemRefExt->ReExtDocument, NouveauNom, MAX_DOC_IDENT_LEN);
		  /* passe au descripteur d'element reference' suivant */
		  /* dans le document externe */
		  ElemRefExt = ElemRefExt->ReNext;
	       }
	  }
	if (Fich->RcFileName[0] != '\0')
	  {
	     ref = BIOreadOpen (Fich->RcFileName);
	     if (ref != 0)
	       {
		  /* le fichier .REF du document referencant existe, on le charge */
		  ChargeRef (ref, &(Fich->RcFirstChange));
		  BIOreadClose (ref);
	       }
	  }
	/* ajoute un enregistrement en queue : s'il y a plusieurs */
	/* changements de noms successifs, ils doivent etre pris en */
	/* compte dans l'ordre chronologique */
	ChngPrec = NULL;
	Chng = Fich->RcFirstChange;
	while (Chng != NULL)
	  {
	     ChngPrec = Chng;
	     Chng = Chng->CrNext;
	  }
	GetElemRefChng (&Chng);
	Chng->CrNext = NULL;
	if (ChngPrec == NULL)
	   Fich->RcFirstChange = Chng;
	else
	   ChngPrec->CrNext = Chng;
	/* remplit cet enregistrement en indiquant que c'est un */
	/* changement de nom de document referencant. */
	Chng->CrOldLabel[0] = '\0';
	Chng->CrNewLabel[0] = '\0';
	CopyIdentDoc (&Chng->CrOldDocument, pDoc->DocIdent);
	CopyIdentDoc (&Chng->CrNewDocument, NouveauNom);
	/* ecrit le fichier .REF */
	SauveRef (Fich->RcFirstChange, Fich->RcFileName);
	FichSuiv = Fich->RcNext;
	/* libere le descripteur de fichier */
	FreeFichRefChng (Fich);
	/* passe au fichier suivant */
	Fich = FichSuiv;
     }
}

/* End Of Module pivecr */
