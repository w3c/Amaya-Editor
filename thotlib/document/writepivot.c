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
#include "fileaccess.h"
#include "appaction.h"
#include "appstruct.h"
#include "typecorr.h"
#include "thotdir.h"
#include "fileaccess.h"
#include "labelAllocator.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#ifndef NODISPLAY
#include "modif_tv.h"
#endif
#include "platform_tv.h"
#include "edit_tv.h"

#include "applicationapi_f.h"
#include "attributes_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "externalref_f.h"
#include "fileaccess_f.h"
#include "inites_f.h"
#include "labelalloc_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "platform_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structschema_f.h"
#include "structmodif_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "units_f.h"
#include "writepivot_f.h"

/*----------------------------------------------------------------------
   Ecrit dans le fichier le numero de version pivot courant           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteVersionNumber (BinFile pivFile)
#else  /* __STDC__ */
void                WriteVersionNumber (pivFile)
BinFile             pivFile;

#endif /* __STDC__ */
{
   int                 version;

   TtaWriteByte (pivFile, (char) C_PIV_VERSION);
   TtaWriteByte (pivFile, (char) C_PIV_VERSION);
   /* Version courante de PIVOT: 5 */
   version = 5;
   TtaWriteByte (pivFile, (char) version);
}

/*----------------------------------------------------------------------
   PutShort   ecrit un entier court dans le fichier sur deux octets   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutShort (BinFile pivFile, int n)
#else  /* __STDC__ */
static void         PutShort (pivFile, n)
BinFile             pivFile;
int                 n;

#endif /* __STDC__ */
{
   TtaWriteByte (pivFile, (char) (n / 256));
   TtaWriteByte (pivFile, (char) (n % 256));
}

/*----------------------------------------------------------------------
   PutInteger ecrit un entier long dans le fichier, sur 4 octets      
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
   PutDimensionType ecrit dans le fichier un type de dimension sur	
   1 octet.								
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutDimensionType (BinFile pivFile, ThotBool b)
#else  /* __STDC__ */
static void         PutDimensionType (pivFile, b)
BinFile             pivFile;
ThotBool            b;

#endif /* __STDC__ */
{
   if (b)
      TtaWriteByte (pivFile, C_PIV_ABSOLUTE);
   else
      TtaWriteByte (pivFile, C_PIV_RELATIVE);
}

/*----------------------------------------------------------------------
   PutUnit ecrit dans le fichier l'unite                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutUnit (BinFile pivFile, TypeUnit unit)
#else  /* __STDC__ */
static void         PutUnit (pivFile, unit)
BinFile             pivFile;
TypeUnit            unit;

#endif /* __STDC__ */
{
   if (unit == UnPoint)
      TtaWriteByte (pivFile, C_PIV_PT);
   else
      TtaWriteByte (pivFile, C_PIV_EM);
}

/*----------------------------------------------------------------------
   PutSign   ecrit un signe dans le fichier sur un octet             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutSign (BinFile pivFile, ThotBool b)
#else  /* __STDC__ */
static void         PutSign (pivFile, b)
BinFile             pivFile;
ThotBool            b;

#endif /* __STDC__ */
{
   if (b)
      TtaWriteByte (pivFile, C_PIV_PLUS);
   else
      TtaWriteByte (pivFile, C_PIV_MINUS);
}

/*----------------------------------------------------------------------
   PutDimension ecrit dans le fichier le contenu de la regle de       
   dimension pointee par pRegle                            
  ----------------------------------------------------------------------*/
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
      TtaWriteByte (pivFile, C_PIV_PERCENT);
   else
      PutUnit (pivFile, pPRule->PrDimRule.DrUnit);
   PutSign (pivFile, (ThotBool) (pPRule->PrDimRule.DrValue >= 0));
}

/*----------------------------------------------------------------------
   PutBoolean ecrit un booleen dans le fichier sur un octet           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutBoolean (BinFile pivFile, ThotBool b)
#else  /* __STDC__ */
static void         PutBoolean (pivFile, b)
BinFile             pivFile;
ThotBool            b;

#endif /* __STDC__ */
{
   if (b)
      TtaWriteByte (pivFile, C_PIV_TRUE);
   else
      TtaWriteByte (pivFile, C_PIV_FALSE);
}

/*----------------------------------------------------------------------
   PutAlignment ecrit un BAlignment dans le fichier sur un octet      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutAlignment (BinFile pivFile, BAlignment c)
#else  /* __STDC__ */
static void         PutAlignment (pivFile, c)
BinFile             pivFile;
BAlignment          c;

#endif /* __STDC__ */
{
   switch (c)
	 {
	    case AlignLeft:
	       TtaWriteByte (pivFile, C_PIV_LEFT);
	       break;
	    case AlignRight:
	       TtaWriteByte (pivFile, C_PIV_RIGHT);
	       break;
	    case AlignCenter:
	       TtaWriteByte (pivFile, C_PIV_CENTERED);
	       break;
	    case AlignLeftDots:
	       TtaWriteByte (pivFile, C_PIV_LEFTDOT);
	       break;
	 }
}

/*----------------------------------------------------------------------
   PutPageType ecrit un type de page dans le fichier sur un octet     
  ----------------------------------------------------------------------*/
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
	       TtaWriteByte (pivFile, C_PIV_COMPUTED_PAGE);
	       break;
	    case PgBegin:
	       TtaWriteByte (pivFile, C_PIV_START_PAGE);
	       break;
	    case PgUser:
	       TtaWriteByte (pivFile, C_PIV_USER_PAGE);
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   PutReferenceType ecrit un type de reference dans le fichier sur un	
   octet. S'il s'agit d'une inclusion, expansion indique si c'est	
   une inclusion avec ou sans expansion a l'ecran.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutReferenceType (BinFile pivFile, ReferenceType t, ThotBool expansion)
#else  /* __STDC__ */
static void         PutReferenceType (pivFile, t, expansion)
BinFile             pivFile;
ReferenceType       t;
ThotBool            expansion;

#endif /* __STDC__ */
{
   switch (t)
	 {
	    case RefFollow:
	       TtaWriteByte (pivFile, C_PIV_REF_FOLLOW);
	       break;
	    case RefInclusion:
	       if (expansion)
		  TtaWriteByte (pivFile, C_PIV_REF_INCLUS_EXP);
	       else
		  TtaWriteByte (pivFile, C_PIV_REF_INCLUSION);
	       break;
	 }
}

/*----------------------------------------------------------------------
   PutComment ecrit dans le fichier un commentaire dont le		
   texte commence dans le buffer pointe par pBuf.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutComment (BinFile pivFile, PtrTextBuffer pBuf)
#else  /* __STDC__ */
static void         PutComment (pivFile, pBuf)
BinFile             pivFile;
PtrTextBuffer       pBuf;

#endif /* __STDC__ */
{
   int                 i, len;
   PtrTextBuffer       pBuf1;

   /* ecrit la marque de commentaire */
   TtaWriteByte (pivFile, (CHAR_T) C_PIV_COMMENT);
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
	     TtaWriteWideChar (pivFile, pBuf->BuContent[i - 1], ISO_8859_1);
	     i++;
	  }
	pBuf = pBuf->BuNext;
     }
}


/*----------------------------------------------------------------------
   EcritNat ecrit une marque de nature suivie du numero du schema    
   de structure pointe par pSS.                            
  ----------------------------------------------------------------------*/
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
   ThotBool            stop;

   TtaWriteByte (pivFile, (CHAR_T) C_PIV_NATURE);
   /* cherche le schema de structure */
   n = 0;
   stop = FALSE;
   do
      if (ustrcmp (pSS->SsName, pDoc->DocNatureName[n]) == 0)
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

/*----------------------------------------------------------------------
   PutLabel   ecrit le label label dans le fichier pivFile            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PutLabel (BinFile pivFile, LabelString label)
#else  /* __STDC__ */
void                PutLabel (pivFile, label)
BinFile             pivFile;
LabelString         label;

#endif /* __STDC__ */
{
   int              i;

   TtaWriteByte (pivFile, C_PIV_LABEL);
   i = 0;
   do
      TtaWriteByte (pivFile, label[i++]);
   while (label[i - 1] != EOS);
}

/*----------------------------------------------------------------------
   PutReference ecrit dans le fichier pivFile la reference pointee	
   par pRef.                                               
  ----------------------------------------------------------------------*/
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
   LabelString         labelNul;
   ThotBool            expansion;

   labelNul [0] = 0;
   /* ecrit le type de la reference */
   expansion = FALSE;
   if (pRef->RdElement != NULL)
      expansion = pRef->RdElement->ElSource == pRef;
   PutReferenceType (pivFile, pRef->RdTypeRef, expansion);
   PutBoolean (pivFile, pRef->RdInternalRef);
   if (pRef->RdReferred == NULL)
      /* la reference ne designe rien, on ecrit un label nul */
      PutLabel (pivFile, labelNul);
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
	     TtaWriteDocIdent (pivFile, pRefD->ReExtDocument);
	  }
	else
	   /* l'objet designe' est dans le meme document */
	  {
	     if (pRefD->ReReferredElem == NULL)
		/* pas d'element reference' */
		label[0] = EOS;
	     else
		/* cherche si l'element reference' */
		/* est dans le buffer (a la suite d'un Couper). */
	     if (IsASavedElement (pRefD->ReReferredElem))
		label[0] = EOS;
	     else
		/* label: label de l'element designe' */
		strncpy (label, pRefD->ReReferredElem->ElLabel, MAX_LABEL_LEN);
	     /* ecrit le label de l'objet designe' */
	     PutLabel (pivFile, label);
	  }
     }
}

/*----------------------------------------------------------------------
   PutAttribut ecrit dans le fichier pivFile l'attribut pointe' par  
   pAttr.                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PutAttribut (BinFile pivFile, PtrAttribute pAttr, PtrDocument pDoc)
#else  /* __STDC__ */
void                PutAttribut (pivFile, pAttr, pDoc)
BinFile             pivFile;
PtrAttribute        pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   ThotBool            attrOK;
   ThotBool            stop;
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
	   else if (IsASavedElement (pEl))
	      attrOK = FALSE;
	}
   if (attrOK)
      /* cherche le schema de structure ou est defini l'attribut */
     {
	n = 0;
	stop = FALSE;
	do
	   if (ustrcmp (pAttr->AeAttrSSchema->SsName, pDoc->DocNatureName[n]) == 0)
	      stop = TRUE;
	   else if (n < pDoc->DocNNatures - 1)
	      n++;
	   else
	     {
		n = 0;
		stop = TRUE;
	     }
	while (!stop);
	TtaWriteByte (pivFile, (CHAR_T) C_PIV_ATTR);
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
		    PutSign (pivFile, (ThotBool)(pAttr->AeAttrValue >= 0));
		    break;
		 case AtReferenceAttr:
		    PutReference (pivFile, pAttr->AeAttrReference);
		    break;
		 case AtTextAttr:
		    pBuf = pAttr->AeAttrText;
		    while (pBuf != NULL)
		      {
			 i = 0;
			 while (pBuf->BuContent[i] != EOS)
			    TtaWriteWideChar (pivFile, pBuf->BuContent[i++], ISO_8859_1);
			 pBuf = pBuf->BuNext;
		      }
		    TtaWriteByte (pivFile, EOS);
		    break;
		 default:
		    break;
	      }

     }
}

/*----------------------------------------------------------------------
   PutPresRule writes in file pivFile the specific presentation rule pPRule.
   The parameter isPicture is True if the element is a picture element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void   PutPresRule (BinFile pivFile, PtrPRule pPRule)
#else  /* __STDC__ */
static void   PutPresRule (pivFile, pPRule)
BinFile       pivFile;
PtrPRule      pPRule;
#endif /* __STDC__ */
{
  unsigned short     red, green, blue;

  /* s'il s'agit d'une regle de dimension elastique, on ne l'ecrit pas */
  if (pPRule->PrType == PtHeight || pPRule->PrType == PtWidth)
    if (pPRule->PrDimRule.DrPosition)
      return;

  /* on ne traite que les regles de presentation directes (ni heritage */
  /* ni fonction de presentation) dont le codage pivot est defini */
  if ((pPRule->PrType == PtHeight || pPRule->PrType == PtWidth ||
       pPRule->PrType == PtVertPos || pPRule->PrType == PtHorizPos ||
       pPRule->PrType == PtMarginTop || pPRule->PrType == PtMarginRight ||
       pPRule->PrType == PtMarginBottom || pPRule->PrType == PtMarginLeft ||
       pPRule->PrType == PtPaddingTop || pPRule->PrType == PtPaddingRight ||
       pPRule->PrType == PtPaddingBottom || pPRule->PrType == PtPaddingLeft ||
       pPRule->PrType == PtBorderTopWidth ||
       pPRule->PrType == PtBorderRightWidth ||
       pPRule->PrType == PtBorderBottomWidth ||
       pPRule->PrType == PtBorderLeftWidth ||
       pPRule->PrType == PtBorderTopColor ||
       pPRule->PrType == PtBorderRightColor ||
       pPRule->PrType == PtBorderBottomColor ||
       pPRule->PrType == PtBorderLeftColor ||
       pPRule->PrType == PtBorderTopStyle ||
       pPRule->PrType == PtBorderRightStyle ||
       pPRule->PrType == PtBorderBottomStyle ||
       pPRule->PrType == PtBorderLeftStyle ||
       pPRule->PrType == PtSize || pPRule->PrType == PtStyle ||
       pPRule->PrType == PtWeight || pPRule->PrType == PtFont ||
       pPRule->PrType == PtUnderline || pPRule->PrType == PtThickness ||
       pPRule->PrType == PtIndent || pPRule->PrType == PtLineSpacing ||
       pPRule->PrType == PtDepth ||
       pPRule->PrType == PtAdjust || pPRule->PrType == PtJustify ||
       pPRule->PrType == PtLineStyle || pPRule->PrType == PtLineWeight ||
       pPRule->PrType == PtFillPattern ||
       pPRule->PrType == PtBackground || pPRule->PrType == PtForeground ||
       pPRule->PrType == PtHyphenate ||
       pPRule->PrType == PtBreak1 || pPRule->PrType == PtBreak2 ||
       pPRule->PrType == PtPictInfo)
      && pPRule->PrPresMode == PresImmediate)
    {
      /* ecrit la marque de regle */
      TtaWriteByte (pivFile, (CHAR_T) C_PIV_PRESENT);
      /* ecrit le numero de vue */
      PutShort (pivFile, pPRule->PrViewNum);
      /* ecrit le numero de la boite de presentation concernee */
      PutShort (pivFile, 0);
      /* ecrit le type de la regle et ses parametres */
      switch (pPRule->PrType)
	{
	case PtAdjust:
	  /* mode de mise en ligne */
	  TtaWriteByte (pivFile, C_PR_ADJUST);
	  PutAlignment (pivFile, pPRule->PrAdjust);
	  break;
	case PtHeight:
	  if (!pPRule->PrDimRule.DrPosition)
	    {
	      TtaWriteByte (pivFile, C_PR_HEIGHT);
	      PutDimension (pivFile, pPRule);
	    }
	  break;
	case PtWidth:
	  if (!pPRule->PrDimRule.DrPosition)
	    {
	      TtaWriteByte (pivFile, C_PR_WIDTH);
	      PutDimension (pivFile, pPRule);
	    }
	  break;
	case PtVertPos:
	case PtHorizPos:
	  if (pPRule->PrType == PtVertPos)
	    TtaWriteByte (pivFile, C_PR_VPOS);
	  else
	    TtaWriteByte (pivFile, C_PR_HPOS);
	  PutShort (pivFile, abs (pPRule->PrPosRule.PoDistance));
	  PutUnit (pivFile, pPRule->PrPosRule.PoDistUnit);
	  PutSign (pivFile, (ThotBool)(pPRule->PrPosRule.PoDistance >= 0));
	  break;
	case PtFont:
	  TtaWriteByte (pivFile, C_PR_FONT);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
	case PtStyle:
	  TtaWriteByte (pivFile, C_PR_STYLE);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
	case PtWeight:
	  TtaWriteByte (pivFile, C_PR_WEIGHT);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
	case PtUnderline:
	  TtaWriteByte (pivFile, C_PR_UNDERLINE);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
	case PtThickness:
	  TtaWriteByte (pivFile, C_PR_UNDER_THICK);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
	case PtLineStyle:
	  TtaWriteByte (pivFile, C_PR_LINESTYLE);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
        case PtBorderTopStyle:
	  TtaWriteByte (pivFile, C_PR_BORDERTOPSTYLE);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
        case PtBorderRightStyle:
	  TtaWriteByte (pivFile, C_PR_BORDERRIGHTSTYLE);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
        case PtBorderBottomStyle:
	  TtaWriteByte (pivFile, C_PR_BORDERBOTTOMSTYLE);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
        case PtBorderLeftStyle:
	  TtaWriteByte (pivFile, C_PR_BORDERLEFTSTYLE);
	  TtaWriteByte (pivFile, pPRule->PrChrValue);
	  break;
	case PtBreak1:
	  TtaWriteByte (pivFile, C_PR_BREAK1);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
	case PtBreak2:
	  TtaWriteByte (pivFile, C_PR_BREAK2);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
	case PtIndent:
	  TtaWriteByte (pivFile, C_PR_INDENT);
	  PutShort (pivFile, abs (pPRule->PrMinValue));
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  PutSign (pivFile, (ThotBool)(pPRule->PrMinValue >= 0));
	  break;
	case PtSize:
	  TtaWriteByte (pivFile, C_PR_SIZE);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
	case PtLineSpacing:
	  TtaWriteByte (pivFile, C_PR_LINESPACING);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
	case PtLineWeight:
	  TtaWriteByte (pivFile, C_PR_LINEWEIGHT);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtMarginTop:
	  TtaWriteByte (pivFile, C_PR_MARGINTOP);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  PutSign (pivFile, (ThotBool)(pPRule->PrMinValue >= 0));
	  break;
        case PtMarginRight:
	  TtaWriteByte (pivFile, C_PR_MARGINRIGHT);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  PutSign (pivFile, (ThotBool)(pPRule->PrMinValue >= 0));
	  break;
        case PtMarginBottom:
	  TtaWriteByte (pivFile, C_PR_MARGINBOTTOM);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  PutSign (pivFile, (ThotBool)(pPRule->PrMinValue >= 0));
	  break;
        case PtMarginLeft:
	  TtaWriteByte (pivFile, C_PR_MARGINLEFT);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  PutSign (pivFile, (ThotBool)(pPRule->PrMinValue >= 0));
	  break;
        case PtPaddingTop:
	  TtaWriteByte (pivFile, C_PR_PADDINGTOP);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtPaddingRight:
	  TtaWriteByte (pivFile, C_PR_PADDINGRIGHT);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtPaddingBottom:
	  TtaWriteByte (pivFile, C_PR_PADDINGBOTTOM);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtPaddingLeft:
	  TtaWriteByte (pivFile, C_PR_PADDINGLEFT);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtBorderTopWidth:
	  TtaWriteByte (pivFile, C_PR_BORDERTOPWIDTH);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtBorderRightWidth:
	  TtaWriteByte (pivFile, C_PR_BORDERRIGHTWIDTH);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtBorderBottomWidth:
	  TtaWriteByte (pivFile, C_PR_BORDERBOTTOMWIDTH);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
        case PtBorderLeftWidth:
	  TtaWriteByte (pivFile, C_PR_BORDERLEFTWIDTH);
	  PutShort (pivFile, pPRule->PrMinValue);
	  PutUnit (pivFile, pPRule->PrMinUnit);
	  break;
	case PtJustify:
	  TtaWriteByte (pivFile, C_PR_JUSTIFY);
	  PutBoolean (pivFile, pPRule->PrJustify);
	  break;
	case PtHyphenate:
	  TtaWriteByte (pivFile, C_PR_HYPHENATE);
	  PutBoolean (pivFile, pPRule->PrJustify);
	  break;
	case PtDepth:
	  TtaWriteByte (pivFile, C_PR_DEPTH);
	  PutShort (pivFile, pPRule->PrIntValue);
	  break;
	case PtFillPattern:
	  TtaWriteByte (pivFile, C_PR_FILLPATTERN);
	  PutShort (pivFile, pPRule->PrIntValue);
	  break;
	case PtBackground:
	  TtaWriteByte (pivFile, C_PR_BACKGROUND);
	  TtaGiveThotRGB (pPRule->PrIntValue, &red, &green, &blue);
	  PutShort (pivFile, red);
	  PutShort (pivFile, green);
	  PutShort (pivFile, blue);
	  break;
	case PtForeground:
	  TtaWriteByte (pivFile, C_PR_FOREGROUND);
	  TtaGiveThotRGB (pPRule->PrIntValue, &red, &green, &blue);
	  PutShort (pivFile, red);
	  PutShort (pivFile, green);
	  PutShort (pivFile, blue);
	  break;
        case PtBorderTopColor:
	  TtaWriteByte (pivFile, C_PR_BORDERTOPCOLOR);
	  TtaGiveThotRGB (pPRule->PrIntValue, &red, &green, &blue);
	  PutShort (pivFile, red);
	  PutShort (pivFile, green);
	  PutShort (pivFile, blue);
	  break;
        case PtBorderRightColor:
	  TtaWriteByte (pivFile, C_PR_BORDERRIGHTCOLOR);
	  TtaGiveThotRGB (pPRule->PrIntValue, &red, &green, &blue);
	  PutShort (pivFile, red);
	  PutShort (pivFile, green);
	  PutShort (pivFile, blue);
	  break;
        case PtBorderBottomColor:
	  TtaWriteByte (pivFile, C_PR_BORDERBOTTOMCOLOR);
	  TtaGiveThotRGB (pPRule->PrIntValue, &red, &green, &blue);
	  PutShort (pivFile, red);
	  PutShort (pivFile, green);
	  PutShort (pivFile, blue);
	  break;
        case PtBorderLeftColor:
	  TtaWriteByte (pivFile, C_PR_BORDERLEFTCOLOR);
	  TtaGiveThotRGB (pPRule->PrIntValue, &red, &green, &blue);
	  PutShort (pivFile, red);
	  PutShort (pivFile, green);
	  PutShort (pivFile, blue);
	  break;
	default:
	  break;
	}
    }
}

/*----------------------------------------------------------------------
   Externalise effectue la traduction sous forme pivot du sous-arbre  
   ou de l'element pointe par pEl.                                 
   subTree indique si on veut ecrire le sous-arbre ou l'element    
   seul.                                                           
   Le fichier 'pivFile' ou est ecrit la representation pivot doit  
   deja etre ouvert a l'appel et il n'est pas ferme au retour.     
   Si l'element externalise est une feuille texte suivie d'autres  
   feuilles de texte ayant les m^emes attributs, ces elements sont 
   externalise's sous la forme d'un seul element, et au retour,    
   pEl pointe sur le dernier de ces elements successifs.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Externalise (BinFile pivFile, PtrElement * pEl, PtrDocument pDoc, ThotBool subTree)
#else  /* __STDC__ */
void                Externalise (pivFile, pEl, pDoc, subTree)
BinFile             pivFile;
PtrElement         *pEl;
PtrDocument         pDoc;
ThotBool            subTree;

#endif /* __STDC__ */
{
  PtrElement          pChild, pEl1;
  PtrTextBuffer       pBuf;
  PtrAttribute        pAttr;
  PtrPRule            pPRule;
  PtrSSchema          pSS;
  NotifyElement       notifyEl;
  NotifyAttribute     notifyAttr;
  int                 i, c;
  ThotBool            stop;

  /* on ecrit effectivement la forme pivot de l'element */
  pEl1 = *pEl;
  /* ecrit la marque de type */
  TtaWriteByte (pivFile, (CHAR_T) C_PIV_TYPE);
  /* ecrit le numero de la regle definissant le type */
  PutShort (pivFile, pEl1->ElTypeNumber);
  /* si c'est une copie d'element inclus, ecrit la reference a */
  /* l'element inclus */
  if (pEl1->ElSource != NULL)
    /* ecrit la marque d'element inclus */
    {
      TtaWriteByte (pivFile, (CHAR_T) C_PIV_INCLUDED);
      PutReference (pivFile, pEl1->ElSource);
    }
  /* ecrit la marque "Element-reference'" si l'element est */
  /* effectivement reference' */
  if (pEl1->ElReferredDescr != NULL)
    if (pEl1->ElReferredDescr->ReFirstReference != NULL ||
	pEl1->ElReferredDescr->ReExtDocRef != NULL)
      /* l'element est effectivement reference' */
      TtaWriteByte (pivFile, (CHAR_T) C_PIV_REFERRED);
  /* ecrit le label de l'element */
  PutLabel (pivFile, pEl1->ElLabel);
  
  /* Ecrit la marque d'holophraste si l'element est holophraste' */
  if (pEl1->ElHolophrast)
    TtaWriteByte (pivFile, (CHAR_T) C_PIV_HOLOPHRAST);
  
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

  /*****TO BE CHANGED**** */
  /* write a specific rule for each picture element */
  if (pEl1->ElTerminal && pEl1->ElLeafType == LtPicture && pEl1->ElPictInfo != NULL)
    {
      /* write the rule mark */
      TtaWriteByte (pivFile, (CHAR_T) C_PIV_PRESENT);
      /* write the view number */
      PutShort (pivFile, 1);
      /* write the presentation box number */
      PutShort (pivFile, 0);
      /* write the specific rule and its parameters */
      TtaWriteByte (pivFile, C_PR_PICTURE);
      PutShort (pivFile, 0);
      PutShort (pivFile, 0);
      PutShort (pivFile, 0);
      PutShort (pivFile, 0);
      switch (((PictInfo *)(pEl1->ElPictInfo))->PicPresent)
	{
	case RealSize:
	  TtaWriteByte (pivFile, C_PIV_REALSIZE);
	  break;
	case ReScale:
	  TtaWriteByte (pivFile, C_PIV_RESCALE);
	  break;
	case FillFrame:
	  TtaWriteByte (pivFile, C_PIV_FILLFRAME);
	  break;
	case XRepeat:
	  TtaWriteByte (pivFile, C_PIV_XREPEAT);
	  break;
	case YRepeat:
	  TtaWriteByte (pivFile, C_PIV_YREPEAT);
	  break;
	}
      PutShort (pivFile, ((PictInfo *)(pEl1->ElPictInfo))->PicType);
    }

  /* ecrit les regles de presentation de l'element */
  pPRule = pEl1->ElFirstPRule;
  while (pPRule != NULL)
    {
      PutPresRule (pivFile, pPRule);
      pPRule = pPRule->PrNextPRule;
    }
  /* ecrit les commentaires associes a l'element */
  if (pEl1->ElComment != NULL)
    PutComment (pivFile, pEl1->ElComment);

  /* ecrit le contenu de l'element */
  if (pEl1->ElSource == NULL)
    /* on n'ecrit pas le contenu d'un element inclus */
    {
      pSS = pEl1->ElStructSchema;
      if (pEl1->ElTerminal)
	{
	  /* feuille terminale: on ecrit son contenu entre C_PIV_BEGIN et C_PIV_END */
	  if (pSS->SsRule[pEl1->ElTypeNumber - 1].SrConstruct != CsConstant
	      && !pSS->SsRule[pEl1->ElTypeNumber - 1].SrParamElem)
	    /* on n'ecrit pas le texte des constantes, ni celui des */
	    /* parametres, il est cree automatiquement */
	    {
	      if (pEl1->ElTypeNumber == CharString + 1)
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
		      TtaWriteByte (pivFile, C_PIV_LANG);
		      TtaWriteByte (pivFile, (char)i);
		    }
		}
	      if (pEl1->ElLeafType != LtReference)
		TtaWriteByte (pivFile, (CHAR_T) C_PIV_BEGIN);
	      switch (pEl1->ElLeafType)
		{
		case LtPicture:
		case LtText:
		  /* ecrit dans le fichier le texte des buffers de l'element */
		  /* ou le nom de l'image */
		  do
		    {
		      c = 0;
		      pBuf = pEl1->ElText;
		      while (c < pEl1->ElTextLength && pBuf != NULL)
			{
			  i = 0;
			  while (pBuf->BuContent[i] != EOS && i < pBuf->BuLength)
			    TtaWriteWideChar (pivFile, pBuf->BuContent[i++], ISO_8859_1);
			  c = c + i;
			  /* buffer suivant du meme element */
			  pBuf = pBuf->BuNext;
			}
		      /* peut-on concatener l'element suivant ? */
		      stop = TRUE;
		      if (pEl1->ElLeafType == LtText)
			/* c'est du texte */
			if (pEl1->ElNext != NULL)
			  /* il y a un suivant.. */
			  if (pEl1->ElNext->ElTerminal)
			    if (pEl1->ElNext->ElLeafType == LtText)
			      /* qui est une feuille de text */
			      if (pEl1->ElNext->ElLanguage == pEl1->ElLanguage)
				if (pEl1->ElNext->ElSource == NULL)
				  /* le suivant n'est pas une inclusion */
				  if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct != CsConstant)
				    if (pEl1->ElNext->ElStructSchema->SsRule[pEl1->ElNext->ElTypeNumber - 1].SrConstruct != CsConstant)
				      if (SameAttributes (*pEl, pEl1->ElNext))
					/* il a les memes attributs */
					if (BothHaveNoSpecRules (*pEl, pEl1->ElNext))
					  /* il a les memes regles de */
					  /* presentation specifique  */
					  if (pEl1->ElNext->ElComment == NULL)
					    if (pEl1->ElComment == NULL)
					      /* aucun des deux n'a de */
					      /* commentaires, on concatene */
					      {
						stop = FALSE;
						pEl1 = pEl1->ElNext;
					      }
		    }
		  while (!stop);
		  /* update pEl value */
		  *pEl = pEl1;
		  TtaWriteByte (pivFile, EOS);
		  break;
		case LtReference:
		  /* ecrit une marque de reference et le label de */
		  /* l'element qui est reference' */
		  TtaWriteByte (pivFile, (CHAR_T) C_PIV_REFERENCE);
		  PutReference (pivFile, pEl1->ElReference);
		  break;
		case LtSymbol:
		case LtGraphics:
		  /* ecrit le code du symbole ou du graphique */
		  TtaWriteByte (pivFile, pEl1->ElGraph);
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
		case LtPolyLine:
		  /* ecrit le code representant la forme de la ligne */
		  TtaWriteByte (pivFile, pEl1->ElPolyLineType);
		  /* ecrit une marque indiquant que c'est une Polyline */
		  TtaWriteByte (pivFile, (CHAR_T) C_PIV_POLYLINE);
		  /* ecrit le nombre de points de la ligne */
		  PutShort (pivFile, pEl1->ElNPoints);
		  /* ecrit tous les points */
		  c = 0;
		  pBuf = pEl1->ElPolyLineBuffer;
		  while (c < pEl1->ElNPoints && pBuf != NULL)
		    {
		      for (i = 0; i < pBuf->BuLength; i++)
			{
			  PutInteger (pivFile, pBuf->BuPoints[i].XCoord);
			  PutInteger (pivFile, pBuf->BuPoints[i].YCoord);
			}
		      c += pBuf->BuLength;
		      pBuf = pBuf->BuNext;	/* buffer suivant du meme element */
		    }
		  break;
		default:
		  break;
		}
	      if (pEl1->ElLeafType != LtReference)
		TtaWriteByte (pivFile, (CHAR_T) C_PIV_END);
	    }
	}
      else
	/* ce n'est pas un element terminal */
	if (subTree)
	  /* on veut ecrire les fils de l'element */
	  if (!pSS->SsRule[pEl1->ElTypeNumber - 1].SrParamElem)
	    /* on n'ecrit pas le contenu des parametres */
	    {
	      /* ecrit une marque de debut */
	      TtaWriteByte (pivFile, (CHAR_T) C_PIV_BEGIN);
	      pChild = pEl1->ElFirstChild;
	      /* ecrit successivement la representation pivot de tous */
	      /* les fils de l'element */
	      while (pChild != NULL)
		{
		  /* envoie l'evenement ElemSave.Pre a l'application, si */
		  /* elle le demande */
		  notifyEl.event = TteElemSave;
		  notifyEl.document = (Document) IdentDocument (pDoc);
		  notifyEl.element = (Element) pChild;
		  notifyEl.elementType.ElTypeNum = pChild->ElTypeNumber;
		  notifyEl.elementType.ElSSchema = (SSchema) (pChild->ElStructSchema);
		  notifyEl.position = 0;
		  if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		    /* l'application accepte que Thot sauve l'element */
		    {
		      /* Ecrit d'abord le numero de la structure generique s'il y */
		      /* a changement de schema de structure par rapport au pere */
		      if (pEl1->ElStructSchema != pChild->ElStructSchema)
			EcritNat (pChild->ElStructSchema, pivFile, pDoc);
		      /* Ecrit un element fils */
		      Externalise (pivFile, &pChild, pDoc, subTree);
		      /* envoie l'evenement ElemSave.Post a l'application, si */
		      /* elle le demande */
		      notifyEl.event = TteElemSave;
		      notifyEl.document = (Document) IdentDocument (pDoc);
		      notifyEl.element = (Element) pChild;
		      notifyEl.elementType.ElTypeNum = pChild->ElTypeNumber;
		      notifyEl.elementType.ElSSchema = (SSchema) (pChild->ElStructSchema);
		      notifyEl.position = 0;
		      CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		    }
		  /* passe au fils suivant */
		  pChild = pChild->ElNext;
		}
	      /* ecrit une marque de fin */
	      TtaWriteByte (pivFile, (CHAR_T) C_PIV_END);
	    }
    }
}

/*----------------------------------------------------------------------
   	PutName								
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutName (BinFile pivFile, Name N)
#else  /* __STDC__ */
static void         PutName (pivFile, N)
BinFile             pivFile;
Name                N;

#endif /* __STDC__ */
{
   int                 j;

   for (j = 0; j < MAX_NAME_LENGTH - 1 && N[j] != WC_EOS; j++)
      TtaWriteWideChar (pivFile, N[j], ISO_8859_1);
   TtaWriteByte (pivFile, EOS);
}

/*----------------------------------------------------------------------
   WriteNomsSchemasDoc ecrit dans le fichier pivFile les noms de tous 
   les schemas de structure et de presentation utilises par le     
   document pDoc.                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteNomsSchemasDoc (BinFile pivFile, PtrDocument pDoc)
#else  /* __STDC__ */
void                WriteNomsSchemasDoc (pivFile, pDoc)
BinFile             pivFile;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 nat;

   BuildDocNatureTable (pDoc);
   /* ecrit les noms des natures utilisees dans le document. */
   for (nat = 0; nat < pDoc->DocNNatures; nat++)
     {
	/* ecrit la marque de classe ou d'extension */
	if (pDoc->DocNatureSSchema[nat]->SsExtension)
	   TtaWriteByte (pivFile, (CHAR_T) C_PIV_SSCHEMA_EXT);
	else
	   TtaWriteByte (pivFile, (CHAR_T) C_PIV_NATURE);
	/* ecrit le nom de schema de structure dans le fichier */
	PutName (pivFile, pDoc->DocNatureSSchema[nat]->SsName);
	/* ecrit le code du schema de structure */
	PutShort (pivFile, pDoc->DocNatureSSchema[nat]->SsCode);
	/* ecrit le nom du schema de presentation associe' */
	PutName (pivFile, pDoc->DocNatureSSchema[nat]->SsDefaultPSchema);
     }
}

/*----------------------------------------------------------------------
   UpdateLanguageTable met dans la table des langues du document pDoc 
   toutes les langues utilisees dans l'arbre de racine pEl et qui  
   ne sont pas encore dans la table.				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UpdateLanguageTable (PtrDocument pDoc, PtrElement pEl)
#else  /* __STDC__ */
static void         UpdateLanguageTable (pDoc, pEl)
PtrDocument         pDoc;
PtrElement          pEl;

#endif /* __STDC__ */
{
   int                 i;
   ThotBool            found;

   while (pEl != NULL)
     {
	pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	if (pEl != NULL)
	  {
	     found = FALSE;
	     for (i = 0; i < pDoc->DocNLanguages && !found; i++)
		if (pEl->ElLanguage == pDoc->DocLanguages[i])
		   found = TRUE;
	     if (!found)
		if (pDoc->DocNLanguages < MAX_LANGUAGES_DOC - 1)
		   pDoc->DocLanguages[pDoc->DocNLanguages++] = pEl->ElLanguage;
	  }
     }
}

/*----------------------------------------------------------------------
   WriteTableLangues	ecrit dans le fichier pivFile la table des	
   		langues utilisees par le document pDoc.			
  ----------------------------------------------------------------------*/
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
   for (i = 0; i < MAX_PARAM_DOC; i++)
      UpdateLanguageTable (pDoc, pDoc->DocParameters[i]);
   for (i = 0; i < MAX_ASSOC_DOC; i++)
      UpdateLanguageTable (pDoc, pDoc->DocAssocRoot[i]);
   UpdateLanguageTable (pDoc, pDoc->DocRootElement);
   for (i = 0; i < pDoc->DocNLanguages; i++)
     {
	TtaWriteByte (pivFile, (CHAR_T) C_PIV_LANG);
	PutName (pivFile, TtaGetLanguageCode (pDoc->DocLanguages[i]));
     }
}


/*----------------------------------------------------------------------
   WritePivotHeader ecrit l'entete d'un fichier pivot                 
  ----------------------------------------------------------------------*/
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
   ConvertIntToLabel (GetCurrentLabel (pDoc), label);
   PutLabel (pivFile, label);
   /* ecrit la table des langues utilisees par le document */
   WriteTableLangues (pivFile, pDoc);
}


/*----------------------------------------------------------------------
   SauveDoc	sauve le document pDoc dans le fichier pivFile, sous	
   la forme pivot. Le fichier doit etre ouvert avant l'appel et est	
   toujours ouvert au retour.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SauveDoc (BinFile pivFile, PtrDocument pDoc)
#else  /* __STDC__ */
void                SauveDoc (pivFile, pDoc)
BinFile             pivFile;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          pEl, pNextEl;
   ThotBool            stop;
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
   for (i = 0; i < MAX_PARAM_DOC; i++)
      if (pDoc->DocParameters[i] != NULL)
	{
	   pEl = pDoc->DocParameters[i];
	   pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrParamElem = FALSE;
	   /* envoie l'evenement ElemSave.Pre a l'application, si */
	   /* elle le demande */
	   notifyEl.event = TteElemSave;
	   notifyEl.document = (Document) IdentDocument (pDoc);
	   notifyEl.element = (Element) pEl;
	   notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	   notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	   notifyEl.position = 0;
	   if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	      /* l'application accepte que Thot sauve l'element */
	     {
		TtaWriteByte (pivFile, (CHAR_T) C_PIV_PARAM);
		/* Ecrit l'element */
		Externalise (pivFile, &pEl, pDoc, TRUE);
		/* envoie l'evenement ElemSave.Post a l'application, si */
		/* elle le demande */
		notifyEl.event = TteElemSave;
		notifyEl.document = (Document) IdentDocument (pDoc);
		notifyEl.element = (Element) pEl;
		notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
		notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
		notifyEl.position = 0;
		CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	     }
	   pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrParamElem = TRUE;
	}
   /* ecrit la representation pivot de tous les arbres d'elements */
   /* associes qui ne sont pas vides */
   for (i = 0; i < MAX_ASSOC_DOC; i++)
      if (pDoc->DocAssocRoot[i] != NULL)
	{
	   pEl = pDoc->DocAssocRoot[i]->ElFirstChild;
	   if (pEl != NULL)
	      /* y a-t-il autre chose que des sauts de page ? */
	     {
		pNextEl = pEl;
		stop = FALSE;
		do
		   if (pNextEl == NULL)
		      stop = TRUE;
		   else if (pNextEl->ElTypeNumber == PageBreak + 1)
		      pNextEl = pNextEl->ElNext;
		   else
		      stop = TRUE;
		while (!stop);
		if (pNextEl != NULL)
		   /* il n'y a pas que des sauts de pages */
		  {
		     pEl = pDoc->DocAssocRoot[i];
		     /* envoie l'evenement ElemSave.Pre a l'application, si */
		     /* elle le demande */
		     notifyEl.event = TteElemSave;
		     notifyEl.document = (Document) IdentDocument (pDoc);
		     notifyEl.element = (Element) pEl;
		     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
		     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
		     notifyEl.position = 0;
		     if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
			/* l'application accepte que Thot sauve l'element */
		       {
			  /* ecrit une marque d'element associe' */
			  TtaWriteByte (pivFile, (CHAR_T) C_PIV_ASSOC);
			  /* si ces elements associes sont definis dans une extension */
			  /* du schema de structure du document, on ecrit un */
			  /* changement de nature */
			  if (pEl->ElStructSchema != pDoc->DocSSchema)
			     EcritNat (pEl->ElStructSchema, pivFile, pDoc);
			  /* Ecrit l'element */
			  Externalise (pivFile, &pEl, pDoc, TRUE);
			  /* envoie l'evenement ElemSave.Post a l'application, si */
			  /* elle le demande */
			  notifyEl.event = TteElemSave;
			  notifyEl.document = (Document) IdentDocument (pDoc);
			  notifyEl.element = (Element) pEl;
			  notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
			  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
			  notifyEl.position = 0;
			  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		       }
		  }
	     }
	}
   /* ecrit la representation pivot de tout le corps du document */
   pEl = pDoc->DocRootElement;
   if (pEl != NULL)
     {
	/* envoie l'evenement ElemSave.Pre a l'application, si */
	/* elle le demande */
	notifyEl.event = TteElemSave;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	   /* l'application accepte que Thot sauve l'element */
	  {
	     TtaWriteByte (pivFile, (CHAR_T) C_PIV_STRUCTURE);
	     /* ecrit la forme pivot de tout l'arbre */
	     Externalise (pivFile, &pEl, pDoc, TRUE);
	     /* envoie l'evenement ElemSave.Post a l'application, si */
	     /* elle le demande */
	     notifyEl.event = TteElemSave;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
   TtaWriteByte (pivFile, (CHAR_T) C_PIV_DOC_END);
}


/*----------------------------------------------------------------------
   SauveRefSortantes							
   sauve dans le fichier de nom fileName la liste des			
   references du document pDoc qui designent des elements appartenant 
   a d'autres documents.                                              
   Le fichier n'est ecrit que s'il y a effectivement des references   
   sortantes. Dans ce cas, chaque reference sortante est e'crite      
   dans le fichier sous la meme forme que dans le fichier pivot.      
   S'il n'y a aucune reference sortante, le fichier est detruit.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SauveRefSortantes (STRING fileName, PtrDocument pDoc)
#else  /* __STDC__ */
void                SauveRefSortantes (fileName, pDoc)
STRING              fileName;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   BinFile             refFile;
   PtrReferredDescr    pRefD;
   PtrReference        pRef;
   ThotBool            fileOpen;
   ThotBool            noExtRef;

   refFile = NULL;
   pRefD = pDoc->DocReferredEl;
   if (pRefD != NULL)
      /* saute le premier descripteur d'element reference' bidon */
      pRefD = pRefD->ReNext;
   fileOpen = FALSE;
   /* le fichier n'est pas encore ouvert */
   noExtRef = TRUE;
   /* a priori, il n'y a pas de reference sortante */
   /* parcourt la chaine des descripteurs d'elements reference's */
   while (pRefD != NULL)
      /* on ne considere que les elements reference's externes au document */
     {
	if (pRefD->ReExternalRef)
	  {
	     pRef = pRefD->ReFirstReference;
	     /* parcourt la chaine des references a cet element externe */
	     while (pRef != NULL)
		/* on ignore les references qui sont dans */
		/* le tampon de couper-coller */
	       {
		  if (!IsASavedElement (pRef->RdElement))
		    {
		       noExtRef = FALSE;
		       /* au moins une reference sortante */
		       /* ouvre le fichier si ce n'est pas encore fait */
		       if (!fileOpen)
			 {
			    /* ouvre le fichier */
			    refFile = TtaWriteOpen (fileName);
			    if (refFile != 0)
			       fileOpen = TRUE;
			    else
			       /* ouverture fichier impossible */
			      {
				 TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_WRITING_IMP), fileName);
				 pRefD = NULL;
				 pRef = NULL;
			      }
			 }
		       if (fileOpen)
			  /* ecrit la ref dans le fichier */
			  PutReference (refFile, pRef);
		    }
		  /* passe a la reference suivante au meme element */
		  if (pRef != NULL)
		     pRef = pRef->RdNext;
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	if (pRefD != NULL)
	   pRefD = pRefD->ReNext;
     }
   /* fin du parcours des descripteurs d'elements reference's */
   if (fileOpen)
      TtaWriteClose (refFile);
   /* vide le buffer d'entree-sortie */
   if (noExtRef)
      /* il n'y a pas de reference sortante, on detruit le fichier */
      TtaFileUnlink (fileName);
}

/*----------------------------------------------------------------------
   SauveRef   ecrit dans le fichier de nom fileName le fichier	
   .REF dont la representation en memoire est pointee par firstChng
   Libere la representation en memoire                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SauveRef (PtrChangedReferredEl firstChng, PathBuffer fileName)
#else  /* __STDC__ */
void                SauveRef (firstChng, fileName)
PtrChangedReferredEl firstChng;
PathBuffer          fileName;

#endif /* __STDC__ */
{
   BinFile             refFile;
   PtrChangedReferredEl pChnRef, pNextChnRef;

   refFile = TtaWriteOpen (fileName);
   if (refFile != 0)
     {
	/* le fichier est ouvert */
	pChnRef = firstChng;
	while (pChnRef != NULL)
	  {
	     /* ecrit l'ancien label de l'element */
	     PutLabel (refFile, pChnRef->CrOldLabel);
	     /* ecrit le nouveau label de l'element */
	     PutLabel (refFile, pChnRef->CrNewLabel);
	     /* ecrit une marque de nom de document */
	     TtaWriteByte (refFile, (CHAR_T) C_PIV_DOCNAME);
	     /* ecrit le nom de l'ancien document de l'element */
	     TtaWriteDocIdent (refFile, pChnRef->CrOldDocument);
	     /* ecrit une marque de nom de document */
	     TtaWriteByte (refFile, (CHAR_T) C_PIV_DOCNAME);
	     /* ecrit l'identificateur du nouveau document de l'element */
	     TtaWriteDocIdent (refFile, pChnRef->CrNewDocument);
	     /* on libere le descripteur qu'on vient d'ecrire */
	     pNextChnRef = pChnRef->CrNext;
	     FreeChangedReferredEl (pChnRef);
	     pChnRef = pNextChnRef;
	  }
	TtaWriteClose (refFile);
     }
}


/*----------------------------------------------------------------------
   SauveExt   ecrit dans le fichier de nom fileName (de type .EXT)    
   la liste des descripteurs d'elements reference's dont le	
   premier est pointe' par pFirstRefD.				
   Libere tous ces descripteurs.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SauveExt (PtrReferredDescr pFirstRefD, PathBuffer fileName)
#else  /* __STDC__ */
void                SauveExt (pFirstRefD, fileName)
PtrReferredDescr    pFirstRefD;
PathBuffer          fileName;

#endif /* __STDC__ */
{
   BinFile             extFile;
   PtrReferredDescr    pRefD, pNextRefD;
   PtrExternalDoc      pExtDoc, pNextExtDoc;
   ThotBool            fileOpen, noExtRef;

   extFile = NULL;
   pRefD = pFirstRefD;
   /* le fichier n'est pas encore ouvert */
   fileOpen = FALSE;
   /* a priori, il n'y a pas de reference externe */
   noExtRef = TRUE;
   /* parcourt la chaine des descripteurs d'elements reference's */
   while (pRefD != NULL)
     {
	pExtDoc = pRefD->ReExtDocRef;
	if (pExtDoc != NULL)
	   /* l'element est reference' par au moins un document externe */
	  {
	     noExtRef = FALSE;	/* il y a au moins une reference externe */
	     /* ouvre le fichier si ce n'est pas encore fait */
	     if (!fileOpen)
	       {
		  /* ouvre le fichier */
		  extFile = TtaWriteOpen (fileName);
		  if (extFile != 0)
		     fileOpen = TRUE;
		  else
		     /* ouverture fichier impossible */
		    {
		       TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_WRITING_IMP),
					  fileName);
		       pRefD = NULL;
		    }
	       }
	     if (fileOpen)
		/* ecrit le label de l'element reference' */
	       {
		  PutLabel (extFile, pRefD->ReReferredLabel);
		  /* parcourt la chaine des documents referencant l'element */
		  do
		    {
		       /* ecrit une marque de nom de document */
		       TtaWriteByte (extFile, (CHAR_T) C_PIV_DOCNAME);
		       /* ecrit le nom du document referencant */
		       TtaWriteDocIdent (extFile, pExtDoc->EdDocIdent);
		       /* passe au descripteur de document referencant suivant */
		       pExtDoc = pExtDoc->EdNext;
		    }
		  while (pExtDoc != NULL);
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	if (pRefD != NULL)
	   pRefD = pRefD->ReNext;
     }
   if (fileOpen)
      TtaWriteClose (extFile);

   if (noExtRef)
      /* il n'y a pas de reference externe, on detruit le fichier */
      TtaFileUnlink (fileName);
   /* libere la chaine de decripteurs d'elements reference's */
   pRefD = pFirstRefD;
   while (pRefD != NULL)
     {
	/* libere la chaine de descripteurs de documents externes */
	pExtDoc = pRefD->ReExtDocRef;
	while (pExtDoc != NULL)
	  {
	     pNextExtDoc = pExtDoc->EdNext;
	     /* libere le descripteur de document externe */
	     FreeExternalDoc (pExtDoc);
	     pExtDoc = pNextExtDoc;
	  }
	/* libere le descripteur d'element reference' */
	pNextRefD = pRefD->ReNext;
	FreeReferredDescr (pRefD);
	/* passe au descripteur d'element reference' suivant */
	pRefD = pNextRefD;
     }

}


/*----------------------------------------------------------------------
   UpdateExt  Met a jour le fichier .EXT de chaque document contenant 
   des elements reference's par les references externes du document   
   pDoc qui ont ete creees ou detruites depuis la derniere sauvegarde 
   de ce document decidee par l'utilisateur (ou a defaut depuis le    
   chargement du document).                                           
   Un fichier .EXT contient, pour chaque element reference' de        
   l'exterieur,le label de cet element suivi des noms de tous les     
   documents qui font reference a l'element.                         
   S'il n'y a aucune reference externe entrante, le fichier .EXT est  
   detruit.                                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateExt (PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateExt (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   BinFile             extFile;
   int                 i;
   PathBuffer          fileName, directoryName;
   DocumentIdentifier  extDocIdent;
   PtrOutReference     pCreatedRef, pPrevCreatedRef, pNextCreatedRef;
   PtrOutReference     pDeadRef, pPrevDeadRef, pNextDeadRef;
   PtrOutReference     pOutRef;
   PtrReferredDescr    pFirstRefD, pRefD;
   PtrExternalDoc      pExtDoc, pPrevExtDoc;
   ThotBool            found;

   /* parcourt plusieurs fois la liste des references sortantes creees */
   /* et la liste des references sortantes detruites, chaque liste une */
   /* fois pour chaque document externe qui y figure */
   while (pDoc->DocNewOutRef != NULL || pDoc->DocDeadOutRef != NULL)
     {
	/* positionne les pointeurs courants au debut de chacune des */
	/* deux listes */
	pCreatedRef = pDoc->DocNewOutRef;
	pDeadRef = pDoc->DocDeadOutRef;
	pPrevCreatedRef = NULL;
	pPrevDeadRef = NULL;
	/* prend le nom du document externe qui est en tete de liste */
	if (pCreatedRef != NULL)
	   CopyDocIdent (&extDocIdent, pCreatedRef->OrDocIdent);
	else
	   CopyDocIdent (&extDocIdent, pDeadRef->OrDocIdent);
	pFirstRefD = NULL;
	/* Charge le fichier .EXT du document externe */
	/* demande d'abord dans quel directory se trouve le fichier .PIV */
	ustrncpy (directoryName, DocumentPath, MAX_PATH);
	MakeCompleteName (extDocIdent, TEXT("PIV"), directoryName, fileName, &i);
	/* cherche le fichier .EXT dans le meme directory */
	FindCompleteName (extDocIdent, TEXT("EXT"), directoryName, fileName, &i);
	if (fileName[0] != EOS)
	  {
	     extFile = TtaReadOpen (fileName);
	     if (extFile != 0)
	       {
		  /* le fichier .EXT existe, on le charge */
		  LoadEXTfile (extFile, NULL, &pFirstRefD, FALSE);
		  TtaReadClose (extFile);
	       }
	  }
	/* parcourt les deux listes de references sortantes et traite */
	/* toutes les references sortantes (creees ou detruites) qui */
	/* design(ai)ent un element du document externe courant */
	while (pCreatedRef != NULL || pDeadRef != NULL)
	  {
	     if (pCreatedRef != NULL)
		pOutRef = pCreatedRef;
	     else
		pOutRef = pDeadRef;
	     if (!SameDocIdent (pOutRef->OrDocIdent, extDocIdent))
		/* cette reference sortante ne designe pas le document */
		/* externe courant, on passe a la suivante */
		if (pOutRef == pCreatedRef)
		  {
		     pPrevCreatedRef = pCreatedRef;
		     pCreatedRef = pCreatedRef->OrNext;
		  }
		else
		  {
		     pPrevDeadRef = pDeadRef;
		     pDeadRef = pDeadRef->OrNext;
		  }
	     else
		/* cette reference designe le document externe courant */
	       {
		  /* cherche si l'element designe' par la reference sortante */
		  /* traitee a un descripteur d'element reference' dans le */
		  /* document externe */
		  pRefD = pFirstRefD;
		  found = FALSE;
		  while (!found && pRefD != NULL)
		     if (strcmp (pOutRef->OrLabel, pRefD->ReReferredLabel) == 0)
			found = TRUE;
		     else
			pRefD = pRefD->ReNext;
		  if (!found)
		     /* l'element designe' n'a pas de descripteur d'element */
		     /* reference' */
		    {
		       if (pOutRef == pCreatedRef)
			  /* il s'agit d'une reference creee, on ajoute un */
			  /* descripteur d'element reference' */
			 {
			    GetReferredDescr (&pRefD);
			    strncpy (pRefD->ReReferredLabel, pOutRef->OrLabel, MAX_LABEL_LEN);
			    /* chaine le descripteur en tete */
			    pRefD->ReNext = pFirstRefD;
			    pRefD->RePrevious = NULL;
			    if (pRefD->ReNext != NULL)
			       pRefD->ReNext->RePrevious = pRefD;
			    pFirstRefD = pRefD;
			    /* associe a ce descripteur un 1er descripteur de */
			    /* document referencant */
			    GetExternalDoc (&pExtDoc);
			    CopyDocIdent (&pExtDoc->EdDocIdent, pDoc->DocIdent);
			    pRefD->ReExtDocRef = pExtDoc;
			 }
		       /* s'il s'agit d'une reference detruite, on ne devrait */
		       /* pas arriver la... */
		    }
		  else
		     /* l'element designe' possede un descripteur d'element */
		     /* reference' */
		    {
		       /* le document est-il dans les documents referencants ? */
		       pExtDoc = pRefD->ReExtDocRef;
		       pPrevExtDoc = NULL;
		       found = FALSE;
		       while (!found && pExtDoc != NULL)
			  if (SameDocIdent (pExtDoc->EdDocIdent, pDoc->DocIdent))
			     found = TRUE;
			  else
			    {
			       pPrevExtDoc = pExtDoc;
			       pExtDoc = pExtDoc->EdNext;
			    }
		       if (found)
			  /* le document figure parmi les documents referencants */
			 {
			    /* s'il s'agit d'une reference creee, on ne fait rien */
			    if (pOutRef == pDeadRef)
			       /* il s'agit d'une reference detruite, on enleve le */
			       /* descripteur de document referencant */
			      {
				 if (pPrevExtDoc != NULL)
				    pPrevExtDoc->EdNext = pExtDoc->EdNext;
				 else
				   {
				      pRefD->ReExtDocRef = pExtDoc->EdNext;
				      if (pRefD->ReExtDocRef == NULL)
					 /* c'etait le dernier descripteur de document */
					 /* referencant, on enleve le descripteur */
					 /* d'element reference' */
					{
					   if (pRefD == pFirstRefD)
					     {
						pFirstRefD = pRefD->ReNext;
						if (pRefD->ReNext != NULL)
						   pRefD->ReNext->RePrevious = NULL;
					     }
					   else
					     {
						pRefD->RePrevious->ReNext =
						   pRefD->ReNext;
						if (pRefD->ReNext != NULL)
						   pRefD->ReNext->RePrevious =
						      pRefD->RePrevious;
					     }
					   FreeReferredDescr (pRefD);
					   pRefD = NULL;
					}
				   }
				 FreeExternalDoc (pExtDoc);
			      }
			 }
		       else
			  /* le document ne figure pas parmi les documents */
			  /* referencants */
			  /* s'il s'agit d'une reference detruite,on ne fait rien */
		       if (pOutRef == pCreatedRef)
			  /* il s'agit d'une reference creee, on ajoute un */
			  /* descripteur de document referencant */
			 {
			    GetExternalDoc (&pExtDoc);
			    CopyDocIdent (&pExtDoc->EdDocIdent, pDoc->DocIdent);
			    pExtDoc->EdNext = pRefD->ReExtDocRef;
			    pRefD->ReExtDocRef = pExtDoc;
			 }
		    }
		  /* dechaine et supprime cette reference sortante traitee */
		  if (pOutRef == pCreatedRef)
		    {
		       pNextCreatedRef = pCreatedRef->OrNext;
		       if (pPrevCreatedRef == NULL)
			  pDoc->DocNewOutRef = pCreatedRef->OrNext;
		       else
			  pPrevCreatedRef->OrNext = pCreatedRef->OrNext;
		       FreeOutputRef (pCreatedRef);
		       pCreatedRef = pNextCreatedRef;
		    }
		  else
		    {
		       pNextDeadRef = pDeadRef->OrNext;
		       if (pPrevDeadRef == NULL)
			  pDoc->DocDeadOutRef = pDeadRef->OrNext;
		       else
			  pPrevDeadRef->OrNext = pDeadRef->OrNext;
		       FreeOutputRef (pDeadRef);
		       pDeadRef = pNextDeadRef;
		    }
	       }
	  }
	/* on a traite' toutes les references sortantes creees et */
	/* detruites de pDoc qui designaient des elements du document */
	/* externe courant. On sauve maintenant les elements reference's */
	/* de ce document dans son fichier .EXT */
	SauveExt (pFirstRefD, fileName);
     }
}

/*----------------------------------------------------------------------
   UpdateRef  Met a jour le fichier .REF de chaque document contenant 
   des references a des elements qui appartenaient au document pointe'
   par pDoc et qui ont ete detruits ou ont change' de document depuis 
   la derniere sauvegarde de ce document decidee par l'utilisateur    
   (ou a defaut depuis le chargement du document).                    
   (Un fichier .REF contient un enregistrement pour chaque reference  
   dont l'element cible a ete detruit ou a change' de document).      
   met egalement a jour le fichier .EXT du document pour prendre en   
   compte les elements reference's qui ont ete detruits ou colle's    
   dans ce document.                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateRef (PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateRef (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PathBuffer          fileName;
   PathBuffer          directoryName;
   PtrEnteringReferences pExtFileD;
   PtrChangedReferredEl pChnRef, pNextChnRef;
   PtrReferenceChange  pFirstFile, pFile, pNextFile;
   PtrExternalDoc      pExtDoc, pNextExtDoc, pOriginExtDoc, pPrevExtDoc;
   ThotBool            found;
   BinFile             refFile;
   BinFile             extFile;
   int                 i;
   PtrChangedReferredEl pChnRefRead, pNewChnRef;
   PtrReferredDescr    pRefD;

   if (pDoc->DocChangedReferredEl != NULL)
      /* des elements reference's ont effectivement ete touches */
     {
	/* on va charger le fichier .EXT du document */
	/* acquiert d'abord un descripteur de ce fichier */
	GetInputRef (&pExtFileD);
	/* ce fichier est dans le meme directory que le document */
	ustrncpy (directoryName, pDoc->DocDirectory, MAX_PATH);
	FindCompleteName (pDoc->DocDName, TEXT("EXT"), directoryName, fileName, &i);
	/* initialise le descripteur du fichier .EXT */
	pExtFileD->ErFirstReferredEl = NULL;
	CopyDocIdent (&pExtFileD->ErDocIdent, pDoc->DocIdent);
	ustrncpy (pExtFileD->ErFileName, fileName, MAX_PATH);
	if (fileName[0] != EOS)
	  {
	     extFile = TtaReadOpen (fileName);
	     if (extFile != 0)
		/* ce fichier existe, on le charge */
	       {
		  LoadEXTfile (extFile, NULL, &(pExtFileD->ErFirstReferredEl), FALSE);
		  TtaReadClose (extFile);
	       }
	  }
	pChnRef = pDoc->DocChangedReferredEl;
	pFirstFile = NULL;
	/* parcourt la liste des elements reference's change's */
	while (pChnRef != NULL)
	  {
	     /* enregistre la modification dans la forme en memoire du */
	     /* fichier .EXT */
	     if (pChnRef->CrNewLabel[0] == EOS)
		/* cet element reference' a ete detruit, on supprime son */
		/* descripteur d'element reference' */
	       {
		  /* cherche le descripteur */
		  pRefD = pExtFileD->ErFirstReferredEl;
		  while (pRefD != NULL)
		     if (strcmp (pChnRef->CrOldLabel, pRefD->ReReferredLabel) == 0)
			/* supprime ce descripteur */
		       {
			  /* on le retire d'abord de sa chaine */
			  if (pRefD->RePrevious == NULL)
			     pExtFileD->ErFirstReferredEl = pRefD->ReNext;
			  else
			     pRefD->RePrevious->ReNext = pRefD->ReNext;
			  if (pRefD->ReNext != NULL)
			     pRefD->ReNext->RePrevious = pRefD->RePrevious;
			  /* on libere les descripteurs de documents externes */
			  pExtDoc = pRefD->ReExtDocRef;
			  while (pExtDoc != NULL)
			    {
			       pNextExtDoc = pExtDoc->EdNext;
			       /* libere le descripteur de document externe */
			       FreeExternalDoc (pExtDoc);
			       pExtDoc = pNextExtDoc;
			    }
			  /* libere le descripteur d'element reference' */
			  FreeReferredDescr (pRefD);
			  pRefD = NULL;
		       }
		     else
			/* passe au descripteur suivant */
			pRefD = pRefD->ReNext;
	       }
	     else
		/* cet element reference' a ete colle', on lui cree un */
		/* descripteur d'element reference' */
	       {

		  GetReferredDescr (&pRefD);
		  strncpy (pRefD->ReReferredLabel, pChnRef->CrNewLabel, MAX_LABEL_LEN);
		  /* chaine le descripteur en tete */
		  pRefD->ReNext = pExtFileD->ErFirstReferredEl;
		  pRefD->RePrevious = NULL;
		  if (pRefD->ReNext != NULL)
		     pRefD->ReNext->RePrevious = pRefD;
		  pExtFileD->ErFirstReferredEl = pRefD;
		  /* copie la chaine des descripteurs de documents externes */
		  pOriginExtDoc = pChnRef->CrReferringDoc;
		  pPrevExtDoc = NULL;
		  while (pOriginExtDoc != NULL)
		    {
		       GetExternalDoc (&pExtDoc);
		       CopyDocIdent (&pExtDoc->EdDocIdent, pOriginExtDoc->EdDocIdent);
		       /* chaine la copie */
		       if (pPrevExtDoc == NULL)
			  pRefD->ReExtDocRef = pExtDoc;
		       else
			  pPrevExtDoc->EdNext = pExtDoc;
		       pPrevExtDoc = pExtDoc;
		       /* original suivant */
		       pOriginExtDoc = pOriginExtDoc->EdNext;
		    }
	       }
	     pExtDoc = pChnRef->CrReferringDoc;
	     /* traite tous les documents qui font reference a cet element */
	     while (pExtDoc != NULL)
	       {
		  /* le fichier .REF de ce document est-il deja charge'? */
		  found = FALSE;
		  pFile = pFirstFile;
		  while (pFile != NULL && !found)
		     if (SameDocIdent (pExtDoc->EdDocIdent, pFile->RcDocIdent))
			found = TRUE;
		     else
			pFile = pFile->RcNext;
		  if (!found)
		     /* le fichier .REF n'est pas charge', on le charge */
		    {
		       GetFileRefChng (&pFile);
		       pFile->RcNext = pFirstFile;
		       pFirstFile = pFile;
		       pFile->RcFirstChange = NULL;
		       CopyDocIdent (&pFile->RcDocIdent, pExtDoc->EdDocIdent);
		       /* demande d'abord dans quel directory se trouve le */
		       /* fichier .PIV de ce document */
		       ustrncpy (directoryName, DocumentPath, MAX_PATH);
		       MakeCompleteName (pFile->RcDocIdent, TEXT("PIV"), directoryName, fileName, &i);
		       /* cherche le fichier .REF dans le meme directory */
		       FindCompleteName (pFile->RcDocIdent, TEXT("REF"), directoryName, fileName, &i);
		       ustrncpy (pFile->RcFileName, fileName, MAX_PATH);
		       if (fileName[0] != EOS)
			 {
			    refFile = TtaReadOpen (fileName);
			    if (refFile != 0)
			      {
				 /* le fichier .REF existe, on le charge */
				 LoadREFfile (refFile, &pChnRefRead);
				 TtaReadClose (refFile);
				 pFile->RcFirstChange = pChnRefRead;
			      }
			 }
		    }
		  if (pFile != NULL)
		    {
		       /* cherche dans le fichier .REF charge', si l'element */
		       /* change' ne figure pas deja */
		       pChnRefRead = pFile->RcFirstChange;
		       found = FALSE;
		       while (pChnRefRead != NULL && !found)
			 {
			    if (strcmp (pChnRefRead->CrOldLabel, pChnRef->CrOldLabel) == 0)
			       if (SameDocIdent (pChnRefRead->CrOldDocument, pChnRef->CrOldDocument))
				  /* il s'agit du meme ancien element */
				  if (pChnRefRead->CrNewLabel[0] == EOS)
				     /* on a lu une destruction */
				    {
				       found = TRUE;
				       if (pChnRef->CrNewLabel[0] != EOS)
					  /* c'est un deplacement, on transforme la */
					  /* destruction en deplacement */
					 {
					    strncpy (pChnRefRead->CrNewLabel, pChnRef->CrNewLabel, MAX_LABEL_LEN);
					    CopyDocIdent (&pChnRefRead->CrNewDocument,
						    pChnRef->CrNewDocument);
					 }
				    }
				  else
				     /* on a lu un changement de document */
				    {
				       if (pChnRef->CrNewLabel[0] == EOS)
					  /* c'est la destruction de l'element, on */
					  /* l'ignore : le deplacement a priorite' */
					  /* meme s'il est enregistre' apres */
					  found = TRUE;
				    }
			    if (!found)
			       if (pChnRefRead->CrNewLabel[0] != EOS &&
				   pChnRef->CrNewLabel[0] != EOS)
				  /* ce sont deux deplacements d'element */
				  if (strcmp (pChnRefRead->CrNewLabel, pChnRef->CrOldLabel) == 0)
				     if (SameDocIdent (pChnRefRead->CrNewDocument, pChnRef->CrOldDocument))
					/* deux deplacements successifs du meme element */
					/* on reduit a un seul deplacement */
				       {
					  found = TRUE;
					  strncpy (pChnRefRead->CrNewLabel, pChnRef->CrNewLabel, MAX_LABEL_LEN);
					  CopyDocIdent (&pChnRefRead->CrNewDocument,
						    pChnRef->CrNewDocument);
				       }
			    if (!found)
			       pChnRefRead = pChnRefRead->CrNext;
			 }
		       if (!found)
			 {
			    /* enregistre le changement dans le fichier .REF charge' */
			    GetChangedReferredEl (&pNewChnRef);
			    pNewChnRef->CrNext = pFile->RcFirstChange;
			    pFile->RcFirstChange = pNewChnRef;
			    strncpy (pNewChnRef->CrOldLabel, pChnRef->CrOldLabel, MAX_LABEL_LEN);
			    strncpy (pNewChnRef->CrNewLabel, pChnRef->CrNewLabel, MAX_LABEL_LEN);
			    CopyDocIdent (&pNewChnRef->CrOldDocument, pChnRef->CrOldDocument);
			    CopyDocIdent (&pNewChnRef->CrNewDocument, pChnRef->CrNewDocument);
			 }
		    }
		  pNextExtDoc = pExtDoc->EdNext;
		  /* libere le descripteur de document externe */
		  FreeExternalDoc (pExtDoc);
		  /* passe au document externe suivant */
		  pExtDoc = pNextExtDoc;
	       }
	     pNextChnRef = pChnRef->CrNext;
	     /* libere le descripteur qui a ete traite' */
	     FreeChangedReferredEl (pChnRef);
	     /* passe au descripteur suivant */
	     pChnRef = pNextChnRef;
	  }
	/* tous les descripteurs du document ont ete traites */
	pDoc->DocChangedReferredEl = NULL;
	/* ecrit le fichier .EXT mis a jour */
	SauveExt (pExtFileD->ErFirstReferredEl, pExtFileD->ErFileName);
	/* rend le descripteur de ce fichier */
	FreeInputRef (pExtFileD);
	/* ecrit les fichiers .REF mis a jour */
	pFile = pFirstFile;
	while (pFile != NULL)
	  {
	     /* ecrit ce fichier .REF */
	     if (pFile->RcFirstChange != NULL)
		SauveRef (pFile->RcFirstChange, pFile->RcFileName);
	     pNextFile = pFile->RcNext;
	     /* libere le descripteur de fichier */
	     FreeFileRefChng (pFile);
	     /* passe au fichier suivant */
	     pFile = pNextFile;
	  }
     }
}


/*----------------------------------------------------------------------
   TtaRemoveDocument

   Closes a document, releases all ressources allocated to that document,
   removes all files related to the document and updates all links connecting
   the removed document with other documents.

   Parameter:
   document: the document to be removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRemoveDocument (Document document)
#else  /* __STDC__ */
void                TtaRemoveDocument (document)
Document            document;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  int                 i;
  PathBuffer          DirectoryOrig;
  CHAR_T                text[MAX_TXT_LEN];

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      pDoc = LoadedDocument[document - 1];
      /* Keep all external referenced links into the document context */
      /* dealing with the main tree of the document */
      RegisterExternalRef (pDoc->DocRootElement, pDoc, FALSE);
      RegisterDeletedReferredElem (pDoc->DocRootElement, pDoc);
      /* dealing with the trees os associated elements */
      for (i = 1; i <= MAX_ASSOC_DOC; i++)
	if (pDoc->DocAssocRoot[i - 1] != NULL)
	  {
	    RegisterExternalRef (pDoc->DocAssocRoot[i - 1], pDoc, FALSE);
	    RegisterDeletedReferredElem (pDoc->DocAssocRoot[i - 1], pDoc);
	  }
      /* treats the parameters */
      for (i = 1; i <= MAX_PARAM_DOC; i++)
	if (pDoc->DocParameters[i - 1] != NULL)
	  {
	    RegisterExternalRef (pDoc->DocParameters[i - 1], pDoc, FALSE);
	    RegisterDeletedReferredElem (pDoc->DocParameters[i - 1], pDoc);
	  }
      /* modifies files .EXT of documents referenced by destroyed documents */
      UpdateExt (pDoc);
      /* modifies files .REF of documents referencing inexisting documents */
      UpdateRef (pDoc);
      /* destroys files .PIV, .EXT, .REF et .BAK of the document */
      ustrncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, TEXT("PIV"), DirectoryOrig, text, &i);
      TtaFileUnlink (text);
      ustrncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, TEXT("EXT"), DirectoryOrig, text, &i);
      TtaFileUnlink (text);
      ustrncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, TEXT("REF"), DirectoryOrig, text, &i);
      TtaFileUnlink (text);
      ustrncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, TEXT("BAK"), DirectoryOrig, text, &i);
      /* now close the document */
      TtaCloseDocument (document);
    }
}


/*----------------------------------------------------------------------
   ChangeNomExt       Le document pDoc change de nom.			
   Son ancien nom est dans pDoc->DocDName, son nouveau nom est        
   dans newName.                                                      
   Si copyDoc est vrai, il s'agit d'une copie de document : dans      
   tous les fichiers .EXT qui contiennent l'ancien nom on ajoute      
   le nouveau nom                                                     
   Si copyDoc est faux, il s'agit d'un renommage du document : on     
   change le nom du document dans tous les fichiers .EXT qui          
   contiennent le nom de ce document.                                 
   Dans les deux cas, on met a jour le nom du document dans les       
   descripteurs de document externes des autres documents charge's    
   en memoire.                                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeNomExt (PtrDocument pDoc, Name newName, ThotBool copyDoc)
#else  /* __STDC__ */
void                ChangeNomExt (pDoc, newName, copyDoc)
PtrDocument         pDoc;
Name                newName;
ThotBool            copyDoc;

#endif /* __STDC__ */
{
   BinFile             extFile;
   PtrEnteringReferences pFirstInRef, pInRef, pNextInRef;
   PtrReferredDescr    pRefD, pFirstRefD, pElemRefD;
   PathBuffer          fileName, directoryName;
   int                 i;
   ThotBool            load, found;
   PtrExternalDoc      pExtDoc, pOriginExtDoc;
   PtrDocument         pDocExt;

   pFirstInRef = NULL;
   pRefD = pDoc->DocReferredEl;
   if (pRefD != NULL)
      /* saute le 1er descripteur, bidon */
      pRefD = pRefD->ReNext;
   /* cherche tous les descripteurs d'elements reference's externes */
   while (pRefD != NULL)
     {
	if (pRefD->ReExternalRef)
	   /* il s'agit d'un element reference' externe */
	  {
	     /* le document contenant cet element externe est-il charge' ? */
	     pDocExt = GetPtrDocument (pRefD->ReExtDocument);
	     if (pDocExt != NULL)
		/* le document externe est charge' */
	       {
		  /* cherche dans ce document externe le descripteur de */
		  /* l'element reference' */
		  pElemRefD = pDocExt->DocReferredEl;
		  if (pElemRefD != NULL)
		     /* saute le 1er descripteur, bidon */
		     pElemRefD = pElemRefD->ReNext;
		  found = FALSE;
		  while (pElemRefD != NULL && !found)
		    {
		       if (!pElemRefD->ReExternalRef)
			  /* il s'agit d'un element reference' appartenant au */
			  /* document */
			  if (pElemRefD->ReReferredElem != NULL)
			     if (strcmp (pElemRefD->ReReferredElem->ElLabel, pRefD->ReReferredLabel) == 0)
				/* c'est le descripteur de notre element */
				found = TRUE;
		       if (!found)
			  /* passe au descripteur d'element reference' suivant */
			  /* dans le document externe */
			  pElemRefD = pElemRefD->ReNext;
		    }
		  if (found)
		     /* cherche le descripteur de document contenant des */
		     /* references a l'element et qui represente le document */
		     /* qui change de nom */
		    {
		       found = FALSE;
		       pExtDoc = pElemRefD->ReExtDocRef;
		       while (pExtDoc != NULL && !found)
			 {
			    if (SameDocIdent (pExtDoc->EdDocIdent, pDoc->DocIdent))
			      {
				 found = TRUE;
				 /* met a jour le nom du document dans le */
				 /* descripteur de document externe */
				 CopyDocIdent (&pExtDoc->EdDocIdent, newName);
			      }
			    else
			       pExtDoc = pExtDoc->EdNext;
			 }
		    }
	       }
	     /* le fichier .EXT du document contenant cet element est-il */
	     /* charge' ? */
	     load = FALSE;
	     pInRef = pFirstInRef;
	     /* parcourt la liste des fichiers .EXT charge's */
	     while (pInRef != NULL && !load)
		if (SameDocIdent (pInRef->ErDocIdent, pRefD->ReExtDocument))
		   load = TRUE;
		else
		   pInRef = pInRef->ErNext;
	     if (!load)
		/* le fichier .EXT de ce document n'est pas encore charge' */
	       {
		  /* charge le fichier .EXT du document externe */
		  pFirstRefD = NULL;
		  /* demande d'abord dans quel directory se trouve le */
		  /* fichier .PIV */
		  ustrncpy (directoryName, DocumentPath, MAX_PATH);
		  MakeCompleteName (pRefD->ReExtDocument, TEXT("PIV"), directoryName,
				    fileName, &i);
		  /* cherche le fichier .EXT dans le meme directory */
		  FindCompleteName (pRefD->ReExtDocument, TEXT("EXT"), directoryName, fileName, &i);
		  if (fileName[0] != EOS)
		    {
		       extFile = TtaReadOpen (fileName);
		       if (extFile != 0)
			 {
			    /* ce fichier existe, on le charge */
			    LoadEXTfile (extFile, NULL, &pFirstRefD, FALSE);
			    TtaReadClose (extFile);
			 }
		    }
		  if (pFirstRefD != NULL)
		     /* on a effectivement charge' le fichier .EXT */
		    {
		       /* on garde la chaine de descripteurs chargee et le */
		       /* nom du fichier fileName */
		       GetInputRef (&pInRef);
		       pInRef->ErNext = pFirstInRef;
		       pFirstInRef = pInRef;
		       pInRef->ErFirstReferredEl = pFirstRefD;
		       CopyDocIdent (&pInRef->ErDocIdent, pRefD->ReExtDocument);
		       ustrncpy (pInRef->ErFileName, fileName, MAX_PATH);
		    }
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	pRefD = pRefD->ReNext;
     }
   /* parcourt toutes les chaines de descripteurs chargees */
   pInRef = pFirstInRef;
   while (pInRef != NULL)
     {
	/* parcourt la chaine des descripteur d'elements reference's */
	pRefD = pInRef->ErFirstReferredEl;
	while (pRefD != NULL)
	  {
	     /* parcourt la chaine des descripteurs de documents */
	     /* referencant l'element treference' courant */
	     pExtDoc = pRefD->ReExtDocRef;
	     while (pExtDoc != NULL)
	       {
		  if (SameDocIdent (pExtDoc->EdDocIdent, pDoc->DocIdent))
		     /* il s'agit de notre document */
		    {
		       if (copyDoc)
			 {
			    /* ajoute un descripteur de document referencant */
			    pOriginExtDoc = pExtDoc;
			    GetExternalDoc (&pExtDoc);
			    pExtDoc->EdNext = pOriginExtDoc->EdNext;
			    pOriginExtDoc->EdNext = pExtDoc;
			 }
		       /* met le nouveau nom du document referencant */
		       CopyDocIdent (&pExtDoc->EdDocIdent, newName);
		    }
		  pExtDoc = pExtDoc->EdNext;
	       }
	     pRefD = pRefD->ReNext;
	  }
	/* ecrit le fichier .EXT traite' */
	SauveExt (pInRef->ErFirstReferredEl, pInRef->ErFileName);
	pNextInRef = pInRef->ErNext;
	FreeInputRef (pInRef);
	/* passe au fichier .EXT suivant en memoire */
	pInRef = pNextInRef;
     }
}

/*----------------------------------------------------------------------
   ChangeNomRef       Le document pointe' par pDoc change de nom.     
   Son ancien nom est dans pDoc->DocDName, son nouveau nom est dans	
   newName.								
   Indique le changement de nom dans les fichiers .REF de tous les    
   documents qui referencent celui qui change de nom.                 
   Si certains de ces documents referencant sont charge's, on met     
   a jour le nom de document dans leurs descripteurs d'element        
   reference's qui representent des elements se trouvant dans le      
   document qui change de nom                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeNomRef (PtrDocument pDoc, Name newName)
#else  /* __STDC__ */
void                ChangeNomRef (pDoc, newName)
PtrDocument         pDoc;
Name                newName;

#endif /* __STDC__ */
{
   BinFile             refFile;
   PtrReferredDescr    pRefD, pElemRefD;
   PtrExternalDoc      pExtDoc;
   PtrReferenceChange  pFirstREFfile, pFile, pNextFile;
   PtrChangedReferredEl pChnRef, pPrevChnRef;
   PtrDocument         pDocExt;
   PathBuffer          fileName, directoryName;
   ThotBool            found;
   int                 i;

   pFirstREFfile = NULL;
   /* parcourt tous les descripteurs d'elements reference's et pour */
   /* chacun parcourt la liste des documents referencant */
   /* saute d'abord le premier descripteur, bidon */
   pRefD = pDoc->DocReferredEl;
   if (pRefD != NULL)
      pRefD = pRefD->ReNext;
   /* parcourt les descripteurs d'elements reference's */
   while (pRefD != NULL)
     {
	if (!pRefD->ReExternalRef)
	   /* c'est un element reference' interne */
	  {
	     pExtDoc = pRefD->ReExtDocRef;
	     /* parcourt la liste des documents referencant */
	     while (pExtDoc != NULL)
	       {
		  /* ce document referencant a-t-il deja ete rencontre' ? */
		  found = FALSE;
		  pFile = pFirstREFfile;
		  while (pFile != NULL && !found)
		     if (SameDocIdent (pExtDoc->EdDocIdent, pFile->RcDocIdent))
			found = TRUE;
		     else
			pFile = pFile->RcNext;
		  if (!found)
		     /* le document referencant n'a pas encore ete rencontre' */
		     /* on le met dans la liste des document referencant */
		    {
		       GetFileRefChng (&pFile);
		       pFile->RcNext = pFirstREFfile;
		       pFirstREFfile = pFile;
		       pFile->RcFirstChange = NULL;
		       CopyDocIdent (&pFile->RcDocIdent, pExtDoc->EdDocIdent);
		       /* demande dans quel directory se trouve le fichier */
		       /* .PIV de ce document */
		       ustrncpy (directoryName, DocumentPath, MAX_PATH);
		       MakeCompleteName (pFile->RcDocIdent, TEXT("PIV"), directoryName, fileName, &i);
		       /* cherche le fichier .REF dans le meme directory */
		       FindCompleteName (pFile->RcDocIdent, TEXT("REF"), directoryName, pFile->RcFileName, &i);
		    }
		  /* passe au descripteur de document referencant suivant */
		  pExtDoc = pExtDoc->EdNext;
	       }
	  }
	/* passe au descripteur d'element reference' suivant */
	pRefD = pRefD->ReNext;
     }
   /* Tous les documents referencant sont dans la liste. On charge */
   /* leurs fichiers .REF, on y ajoute un enregistrement indiquant */
   /* le changement de nom du document et on les ecrit */
   pFile = pFirstREFfile;
   while (pFile != NULL)	/* parcourt la liste */
     {
	/* ce document referencant est-il charge' ? */
	pDocExt = GetPtrDocument (pFile->RcDocIdent);
	if (pDocExt != NULL)
	   /* le document referencant est charge' */
	  {
	     /* cherche dans ce document referencant les descripteurs */
	     /* d'elements reference's appartenant au document qui */
	     /* change de nom */
	     pElemRefD = pDocExt->DocReferredEl;
	     if (pElemRefD != NULL)
		/* saute le 1er descripteur, bidon */
		pElemRefD = pElemRefD->ReNext;
	     while (pElemRefD != NULL)
	       {
		  if (pElemRefD->ReExternalRef)
		     /* il s'agit d'un element reference' externe */
		     if (ustrcmp (pElemRefD->ReExtDocument, pDoc->DocIdent) == 0)
			/* l'element reference' se trouve dans le document */
			/* qui change de nom, on change le nom */
			ustrncpy (pElemRefD->ReExtDocument, newName, MAX_DOC_IDENT_LEN);
		  /* passe au descripteur d'element reference' suivant */
		  /* dans le document externe */
		  pElemRefD = pElemRefD->ReNext;
	       }
	  }
	if (pFile->RcFileName[0] != EOS)
	  {
	     refFile = TtaReadOpen (pFile->RcFileName);
	     if (refFile != 0)
	       {
		  /* le fichier .REF du document referencant existe, on le charge */
		  LoadREFfile (refFile, &(pFile->RcFirstChange));
		  TtaReadClose (refFile);
	       }
	  }
	/* ajoute un enregistrement en queue : s'il y a plusieurs */
	/* changements de noms successifs, ils doivent etre pris en */
	/* compte dans l'ordre chronologique */
	pPrevChnRef = NULL;
	pChnRef = pFile->RcFirstChange;
	while (pChnRef != NULL)
	  {
	     pPrevChnRef = pChnRef;
	     pChnRef = pChnRef->CrNext;
	  }
	GetChangedReferredEl (&pChnRef);
	pChnRef->CrNext = NULL;
	if (pPrevChnRef == NULL)
	   pFile->RcFirstChange = pChnRef;
	else
	   pPrevChnRef->CrNext = pChnRef;
	/* remplit cet enregistrement en indiquant que c'est un */
	/* changement de nom de document referencant. */
	pChnRef->CrOldLabel[0] = EOS;
	pChnRef->CrNewLabel[0] = EOS;
	CopyDocIdent (&pChnRef->CrOldDocument, pDoc->DocIdent);
	CopyDocIdent (&pChnRef->CrNewDocument, newName);
	/* ecrit le fichier .REF */
	SauveRef (pFile->RcFirstChange, pFile->RcFileName);
	pNextFile = pFile->RcNext;
	/* libere le descripteur de fichier */
	FreeFileRefChng (pFile);
	/* passe au fichier suivant */
	pFile = pNextFile;
     }
}
