/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   that module read documents in pivot format and build an abstract tree
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "picture.h"

#include "language.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"
#include "typecorr.h"
#include "appdialogue.h"
#include "labelAllocator.h"
#include "registry.h"
#include "picture.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"

ThotBool            error;
static ThotBool     msgOldFormat;

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

#ifdef _WINDOWS
#include "wininclude.h"
extern HWND         WIN_Main_Wd;
#endif /* _WINDOWS */

#define MAX_EXT_DOC 10

#include "abspictures_f.h"
#include "applicationapi_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "externalref_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "labelalloc_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structschema_f.h"
#include "tree_f.h"
#include "units_f.h"

/*----------------------------------------------------------------------
  SetImageRule updates or creates the picture descriptor of an element.
  ----------------------------------------------------------------------*/
static void SetImageRule (PtrElement pEl, int x, int y, int w, int h,
			  int typeimage, PictureScaling presimage)
{
  PictInfo           *image;

  if (pEl != NULL)
    {
      image = (PictInfo *) pEl->ElPictInfo;
      if (image == NULL)
	{
	  image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  memset (image, 0, sizeof (PictInfo));
	  pEl->ElPictInfo = (int *) image;
	}
      image->PicFileName = NULL;
      image->PicPixmap = None;
#if !defined(_WINDOWS) && !defined(_GL)
      image->PicMask = 0;
#endif /* _WINDOWS */
      image->PicType = typeimage;
      image->PicPresent = presimage;
      image->PicXArea = x;
      image->PicYArea = y;
      image->PicWArea = w;
      image->PicHArea = h;
      image->PicWidth = 0;
      image->PicHeight = 0;
      image->mapped = FALSE;
      image->created = FALSE;
    }
}


/*----------------------------------------------------------------------
   	PivotError							
  ----------------------------------------------------------------------*/
static void PivotError (BinFile file, char *code)
{
  int                 i, j;
  char                c;
  char                buffer[400];
  ThotBool            stop;

  stop = FALSE;
  i = 0;
  j = 0;
  while (!stop && i < 199)
  {
    if (!TtaReadByte (file, &c))
      stop = TRUE;
    else
	 {
	   if (c < SPACE)
	     {
           buffer[j++] = '^';
		   c = ((int) c + (int) '@');
		   i++;
	     }
         buffer[j++] = c;
	     i++;
	 }
  }
  buffer[j++] = '\n';
  buffer[j++] = EOS;
#ifdef _WIN_PRINT
  WinErrorBox (WIN_Main_Wd, code);
#else /* _WIN_PRINT */
  printf (buffer);
  printf (code);
#endif /* _WIN_PRINT*/
  error = TRUE;
}

/*----------------------------------------------------------------------
   GetPtrDocument cherche si le document d'identificateur docIdent est	
   	charge'.							
   Retourne un pointeur sur son contexte, ou NULL s'il	
   n'est pas charge'.                                      
  ----------------------------------------------------------------------*/
PtrDocument GetPtrDocument (DocumentIdentifier docIdent)
{
   int                 doc;
   PtrDocument         pDoc;

   pDoc = NULL;
   for (doc = 0; doc < MAX_DOCUMENTS; doc++)
      if (LoadedDocument[doc] != NULL)
	 if (SameDocIdent (LoadedDocument[doc]->DocIdent, docIdent))
	    pDoc = LoadedDocument[doc];
   return pDoc;
}

/*----------------------------------------------------------------------
   	FreeUnusedReferredElemDesc					
  ----------------------------------------------------------------------*/
static void FreeUnusedReferredElemDesc (PtrDocument pDoc)
{
   PtrReferredDescr    pRefD, pPrevRefD;

   pRefD = pDoc->DocReferredEl;
   do
      /* parcourt la chaine des descripteurs de reference du document */
     {
	/* (Le premier descripteur de la chaine est bidon) */
	pRefD = pRefD->ReNext;
	if (pRefD != NULL)
	   if (!pRefD->ReExternalRef)
	      if (pRefD->ReExtDocRef == NULL)
		 if (pRefD->ReFirstReference == NULL)
		   {
		      if (pRefD->ReReferredElem != NULL)
			 pRefD->ReReferredElem->ElReferredDescr = NULL;
		      pRefD->ReReferredElem = NULL;
		      pPrevRefD = pRefD->RePrevious;
		      pPrevRefD->ReNext = pRefD->ReNext;
		      if (pRefD->ReNext != NULL)
			 pRefD->ReNext->RePrevious = pPrevRefD;
		      FreeReferredDescr (pRefD);
		      pRefD = pPrevRefD;
		   }
     }
   while (pRefD != NULL);
}

/*----------------------------------------------------------------------
   OpenDocument ouvre le fichier document de nom docName et le charge  
   dans pDoc. loadIncludedDoc indique s'il faut charger ou 
   non les documents externes dont des parties sont        
   incluses dans le document a` ouvrir. Retourne faux si   
   le document n'a pas pu etre charge'. Charge le          
   squelette si skeleton est TRUE. Ne pas charger de       
   schema de structure et utiliser pSS si pSS <> NULL.     
  ----------------------------------------------------------------------*/
ThotBool OpenDocument (char *docName, PtrDocument pDoc, ThotBool loadIncludedDoc,
		       ThotBool skeleton, PtrSSchema pSS, ThotBool withAppEvent,
		       ThotBool removeExclusions)
{  
   FILE               *pivotFile;
   PathBuffer          directoryName;
   char                text[MAX_TXT_LEN];
   int                 i;
   ThotBool            ret;

   ret = FALSE;
   if (pDoc != NULL)
     {
	if (!DocIdentIsNull (pDoc->DocIdent))
	   /* l'identificateur du document est connu, on accede au document
	      par cet identificateur */
	  {
	     /* on n'a pas d'autre outil de stockage des documents que le SGF
		UNIX */
	     /* On confond identificateur et nom de document */
	     strncpy (pDoc->DocDName, pDoc->DocIdent, MAX_NAME_LENGTH);
	     pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
	  }
	else
	   /* le document n'a pas d'identificateur, on l'accede par son nom */
	  {
	     strncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
	     pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
	     /* on n'a pas d'autre outil de stockage des documents que le SGF
		UNIX */
	     /* On confond identificateur et nom de document */
	     strncpy (pDoc->DocIdent, docName, MAX_DOC_IDENT_LEN);
	     pDoc->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
	  }
	if (pDoc->DocDName[0] > SPACE)
	   /* nom de document non vide */
	  {
	     /* compose le nom du fichier a ouvrir avec le nom du directory */
	     /* des documents... */
	     if (pDoc->DocDirectory[0] == EOS)
		strncpy (directoryName, DocumentPath, MAX_PATH);
	     else
		strncpy (directoryName, pDoc->DocDirectory, MAX_PATH);
	     MakeCompleteName (pDoc->DocDName, "PIV", directoryName,
			       text, &i);
	     /* ouvre le fichier 'PIV' */
	     pivotFile = TtaReadOpen (text);
	     if (pivotFile != 0)
		/* le fichier existe */
		/* internalise le fichier pivot sans charger les documents */
		/* externes qui contiennent des elements inclus. */
	       {
		  /* le document appartient au directory courant */
		  strncpy (pDoc->DocDirectory, directoryName, MAX_PATH);
		  LoadDocumentPiv (pivotFile, pDoc, loadIncludedDoc, skeleton,
				   pSS, withAppEvent, removeExclusions);
		  TtaReadClose (pivotFile);
		  if (pDoc->DocDocElement != NULL)
		     /* le document lu n'est pas vide */
		    {
		       /* le nom de fichier devient le nom du document */
		       ret = TRUE;
		       /* libere les descripteurs d'element reference'
			  inutilise's */
		       FreeUnusedReferredElemDesc (pDoc);
		    }
	       }
	  }
     }
   return ret;
}


/*----------------------------------------------------------------------
   DeleteAllTrees
   Supprime l'arbre abstrait d'un document.
   Les schemas de structure et de presentation utilises par le document   
   ne sont pas liberes...                                  
  ----------------------------------------------------------------------*/
void                DeleteAllTrees (PtrDocument pDoc)
{
   int              view;

   if (pDoc != NULL)
     {
	/* libere tout l'arbre du document et ses descripteurs de reference */
	DeleteElement (&pDoc->DocDocElement, pDoc);
	/* document views are now empty */
	for (view = 0; view < MAX_VIEW_DOC; view++)
	   pDoc->DocViewRootAb[view] = NULL;
	/* libere le 1er descripteur de reference (bidon) */
	FreeReferredDescr (pDoc->DocReferredEl);
	pDoc->DocReferredEl = NULL;
     }
}


/*----------------------------------------------------------------------
   ReadDimensionType lit un type de dimension dans le fichier et	
   	retourne sa valeur.                                             
  ----------------------------------------------------------------------*/
static ThotBool     ReadDimensionType (BinFile file)
{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file, "Dimension");
     }
   if (c == C_PIV_ABSOLUTE)
      return TRUE;
   else
      return FALSE;
}


/*----------------------------------------------------------------------
   ReadUnit lit une unite dans le fichier et retourne sa valeur.	
  ----------------------------------------------------------------------*/
static TypeUnit     ReadUnit (BinFile file)
{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file, "Unit");
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
	    case C_PIV_PX:
	       return UnPixel;
	       break;
	    default:
	       return UnRelative;
	 }
}


/*----------------------------------------------------------------------
   ReadSign lit un signe dans le fichier et retourne sa valeur.    
  ----------------------------------------------------------------------*/
static ThotBool     ReadSign (BinFile file)
{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file, "Sign");
     }
   if (c == C_PIV_PLUS)
      return TRUE;
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   ReadBoolean lit un booleen dans le fichier et retourne sa valeur
  ----------------------------------------------------------------------*/
static ThotBool     ReadBoolean (BinFile file)
{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file, "Boolean");
     }
   if (c == C_PIV_TRUE)
      return TRUE;
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   ReadAlign lit un BAlignment dans le fichier et retourne sa valeur. 
  ----------------------------------------------------------------------*/
static BAlignment   ReadAlign (BinFile file)
{
   char       c;
   BAlignment align;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file, "Align");
     }
   switch (c)
	 {
	    case C_PIV_LEFT:
	       align = AlignLeft;
	       break;
	    case C_PIV_RIGHT:
	       align = AlignRight;
	       break;
	    case C_PIV_CENTERED:
	       align = AlignCenter;
	       break;
	    case C_PIV_LEFTDOT:
	       align = AlignLeftDots;
	       break;
	    case C_PIV_JUSTIFY:
	       align = AlignJustify;
	       break;
	    default:
	       PivotError (file, "PivotError (Align 1)");
	       align = AlignLeft;
	       break;
	 }
   return align;
}

/*----------------------------------------------------------------------
   ReadPageType	lit un Type de page dans le fichier et retourne	
   sa valeur.                                              
  ----------------------------------------------------------------------*/
static PageType ReadPageType (BinFile file)
{
   char     c;
   PageType typ;

   if (!TtaReadByte (file, &c))
     {
       c = EOS;
       PivotError (file, "PivotError (PageType)");
     }
   switch (c)
     {
     case C_PIV_COMPUTED_PAGE:
       typ = PgComputed;
       break;
     case C_PIV_START_PAGE:
       typ = PgBegin;
       break;
     case C_PIV_USER_PAGE:
       typ = PgUser;
       break;
       /* les tags colonnes sont traduites en tags pages pour */
       /* la version qui ne gere pas les colonnes */
       /* un message est affiche a l'utilisateur pour qu'il */
       /* repagine s'il le desire */
     case C_PIV_COMPUTED_COL:
       typ = PgComputed;
       break;
     case C_PIV_START_COL:
       typ = PgComputed;
       break;
     case C_PIV_USER_COL:
       typ = PgComputed;
       break;
     case C_PIV_COL_GROUP:
       typ = PgComputed;
       break;
     case C_PIV_REPEAT_PAGE:
       /* les tags page rappel sont transformees en tags page calculees,
	  car le nouveau code ne traite plus ces types de tags */
       typ = PgComputed;
       break;
     default:
       PivotError (file, "PivotError (PageType 1)");
       typ = PgComputed;
       break;
     }
   return typ;
}

/*----------------------------------------------------------------------
   ReadPicturePresentation lit la presentation d'un PictInfo	
  ----------------------------------------------------------------------*/
static PictureScaling ReadPicturePresentation (BinFile pivFile)
{
   char           c;
   PictureScaling scaling;

   if (!TtaReadByte (pivFile, &c))
     {
	c = EOS;
	PivotError (pivFile, "PicturePresentation)");
     }
   switch (c)
     {
     case C_PIV_REALSIZE:
       scaling = RealSize;
       break;
     case C_PIV_RESCALE:
       scaling = ReScale;
       break;
     case C_PIV_FILLFRAME:
       scaling = FillFrame;
       break;
     case C_PIV_XREPEAT:
       scaling = XRepeat;
       break;
     case C_PIV_YREPEAT:
       scaling = YRepeat;
       break;
     default:
       scaling = DefaultPres;
       break;
     }

   return scaling;
}


/*----------------------------------------------------------------------
   LabelStringToInt convertit le label strn en un entier           
   retourne 0 si label mal construit.                      
  ----------------------------------------------------------------------*/
void LabelStringToInt (LabelString string, int *number)
{
   int                 i;
   int                 val;
   ThotBool            ok;
   char                c;

   ok = FALSE;
   *number = 0;
   if (string[0] == 'L')
     {
	val = 0;
	i = 1;
	do
	  {
	     c = string[i];
	     i++;
	     if (c >= '0' && c <= '9')
	       {
		  val = (val * 10) + (c - '0');
		  ok = TRUE;
	       }
	     else if (c != EOS)
		ok = FALSE;
	  }
	while (ok && c != EOS);
	if (ok)
	   *number = val;
     }
}


/*----------------------------------------------------------------------
   ReadReference lit une reference dans le fichier file et retourne
   le type de la reference lue (refType), le label de l'element 
   reference' (label), un booleen indiquant si la reference	
   est interne ou externe (refExt) et, dans le cas d'une	
   reference externe, le nom (docIdent) du document contenant	
   l'element reference'. 					
  ----------------------------------------------------------------------*/
static void ReadReference (ReferenceType *refType, LabelString label,
			   ThotBool *refExt, DocumentIdentifier *docIdent,
			   BinFile file)
{
   int  j;
   char c;

   ClearDocIdent (docIdent);
   /* lit un octet */
   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file, "PivotError: Reference");
     }
   /* cet octet represente-t-il un type de reference correct ? */
   if (c != C_PIV_REF_FOLLOW && c != C_PIV_REF_INCLUSION &&
       c != C_PIV_REF_INCLUS_EXP)
      /* non, ancien format pivot */
      /* on envoie un message a l'utilisateur, si ce n'est deja fait */
     {
	if (msgOldFormat)
	  {
	     PivotError (file, "PivotError: Reference 1");
	     msgOldFormat = FALSE;	/* c'est fait */
	  }
	if (c == MOldRefInterne || c == MOldRefExterne ||
	    c == MOldRefInclusion)
	   /* ancien format version 2 */
	  {
	     /* l'octet lu represente le type de la reference */
	     switch (c)
		   {
		      case MOldRefInterne:
			 *refExt = FALSE;
			 *refType = RefFollow;
			 break;
		      case MOldRefExterne:
			 *refExt = TRUE;
			 *refType = RefFollow;
			 break;
		      case MOldRefInclusion:
			 *refType = RefInclusion;
			 *refExt = TRUE;
			 break;
		   }
	     /* lit le type de label */
	     if (!TtaReadByte (file, &c))
	       {
		  c = EOS;
		  PivotError (file, "PivotError: Reference 3");
	       }
	     /* lit la valeur du label */
	     ReadLabel (c, label, file);
	     if (*refExt && label[0] != EOS)
		/* lit le nom du document contenant l'element reference' */
		TtaReadDocIdent (file, docIdent);
	  }
	else
	   /* on interprete comme dans la version 1 */
	   /* c'est une reference renvoi interne */
	  {
	     *refType = RefFollow;
	     *refExt = FALSE;	/* il n'y a qu'un label court, sans tag */
	     /* l'octet lu est l'octet de poids fort du label */
	     j = 256 * ((int) c);	/* lit le 2eme octet du label */
	     if (!TtaReadByte (file, &c))
	       {
		  c = EOS;
		  PivotError (file, "PivotError: Reference 4");
	       }
	     j += (int) c;
	     /* convertit le label numerique en chaine de caracteres */
	     ConvertIntToLabel (j, label);
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
	    *refType = RefFollow;
	    break;
	  case C_PIV_REF_INCLUSION:
	    *refType = RefInclusion;
	    break;
	  case C_PIV_REF_INCLUS_EXP:
	    *refType = RefInclusion;
	    break;
	  }
	/* lit l'indicateur reference  interne/externe */
	*refExt = !ReadBoolean (file);
	/* lit le type de label */
	if (!TtaReadByte (file, &c))
	  {
	     c = EOS;
	     PivotError (file, "PivotError: Reference 5");
	  }
	/* lit la valeur du label */
	ReadLabel (c, label, file);
	if (*refExt && label[0] != EOS)
	  /* lit l'identificateur du document contenant l'element reference' */
	  TtaReadDocIdent (file, docIdent);

     }
}


/*----------------------------------------------------------------------
   GetElRefer cherche s'il existe un descripteur de reference	
   designant l'element de label label dans le document de	
   docIdent. Si docIdent est un identificateur vide,	
   		l'element designe'					
   est interne au document en cours de lecture, dont le    
   contexte est pointe' par pDoc. Si le descripteur        
   n'existe pas, il est cree' et chaine'.                  
   La fonction rend un pointeur sur le descripteur trouve' 
   ou cree'.                                               
  ----------------------------------------------------------------------*/
static PtrReferredDescr GetElRefer (LabelString label,
				    DocumentIdentifier docIdent, PtrDocument pDoc)
{
   PtrReferredDescr    pRefD;
   int                 i;
   ThotBool            stop;

   pRefD = pDoc->DocReferredEl;
   stop = FALSE;
   do
      /* parcourt la chaine des descripteurs de reference du document en   */
      /* cours de lecture. (Le premier descripteur de la chaine est bidon) */
     {
	pRefD = pRefD->ReNext;
	if (pRefD == NULL)
	   /* fin de la chaine */
	   stop = TRUE;
	else if (strcmp (pRefD->ReReferredLabel, label) == 0)
	   /* le label correspond */
	  {
	   if (DocIdentIsNull (docIdent) && !pRefD->ReExternalRef)
	      /* on cherche une reference interne et c'en est une */
	      stop = TRUE;	/* trouve' */
	   else if (!DocIdentIsNull (docIdent) && pRefD->ReExternalRef)
	      /* on cherche une reference externe et c'en est une */
	      if (SameDocIdent (docIdent, pRefD->ReExtDocument))
		 stop = TRUE;
	  }
     }
   while (!stop);
   if (pRefD == NULL)
      /* on n'a pas trouve' le descripteur */
      /* on cree et chaine un nouveau descripteur */
     {
	pRefD = NewReferredElDescr (pDoc);
	/* on initialise le descripteur de reference cree'. */
	strncpy (pRefD->ReReferredLabel, label, MAX_LABEL_LEN);
	pRefD->ReExternalRef = !DocIdentIsNull (docIdent);
	if (pRefD->ReExternalRef)
	   CopyDocIdent (&pRefD->ReExtDocument, docIdent);
	else
	  {
	     LabelStringToInt (label, &i);
	     if (i > GetCurrentLabel (pDoc))
		SetCurrentLabel (pDoc, i);
	  }
     }
   return pRefD;
}


/*----------------------------------------------------------------------
   CreateReference chaine le descripteur de reference pointe' par  
   pRef appartenant au document dont le contexte est       
   pointe' par pDoc et initialise ce descripteur pour      
   qu'il designe l'element de label label appartenant au   
   document docIdent. Si docIdent est un identificateur	
   vide, il s'agit du document pDoc.			
  ----------------------------------------------------------------------*/
static void CreateReference (PtrReference RefPtr, ReferenceType TRef,
			     LabelString lab, ThotBool RExt,
			     DocumentIdentifier I, PtrDocument pDoc)
{
   PtrReferredDescr    r;
   PtrReference        pRf;
   PtrReference        pPR1;

   if (lab[0] != EOS)
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


/*----------------------------------------------------------------------
   ReadType lit dans le fichier pivot, selon la valeur de tag,     
   un numero de type ou un nom de nature et rend le numero 
   de la regle definissant le type lu. Retourne 0 si       
   erreur. Si tag est une tag de nature, au retour		
   pSS contient un pointeur sur le schema de structure 
   de cette nature.                                        
  ----------------------------------------------------------------------*/
static int ReadType (PtrDocument pDoc, PtrSSchema *pSS, BinFile pivFile, char *tag)
{
   int                 nat, rule;
   ThotBool            Extension;

   rule = 0;
   if (*tag == C_PIV_NATURE)
     {
       /* lit le numero de nature */
       TtaReadShort (pivFile, &nat);
       if (nat < 0 || nat >= pDoc->DocNNatures)
	 PivotError (pivFile, "PivotError: Nature Num");
       /* lit le tag de type qui suit */
       if (!error)
	 {
	 if (!TtaReadByte (pivFile, tag))
	   PivotError (pivFile, "PivotError: TypeType");
         /* teste si le numero lu est celui de la structure generique du doc.*/
	 else if (nat == 0)
	   *pSS = pDoc->DocSSchema;
	 else
	   {
	     /* teste s'il s'agit d'une extension de la structure generique
		du document */
	     Extension = FALSE;	/* a priori, non */
	     if (pDoc->DocNatureSSchema[nat] != NULL)
	       if (pDoc->DocNatureSSchema[nat]->SsExtension)
		 {
		   Extension = TRUE;
		   *pSS = pDoc->DocNatureSSchema[nat];
		 }
	     if (!Extension)
	       {
		 /* pas de presentation preferentielle */
		 rule = CreateNature (pDoc->DocNatureName[nat],
				      pDoc->DocNaturePresName[nat], *pSS,pDoc);
		 /* recupere le numero de la regle de nature */
		 if (rule == 0)
		   PivotError (pivFile, "PivotError: Nature");
		 else
		   *pSS = (*pSS)->SsRule->SrElem[rule - 1]->SrSSchemaNat;
	       }
	   }
	 }
     }
   if (!error)
     {
     if (*tag == C_PIV_TYPE)
       {
	 /* lit le numero de type de l'element */
	 TtaReadShort (pivFile, &rule);
	 if (pDoc->DocPivotVersion < 4)
	   /* on tient compte de l'ajout du type de base PolyLine */
	   if (rule >= MAX_BASIC_TYPE)
	     rule++;
       }
     else
       {
	 rule = 0;
	 PivotError (pivFile, "PivotError: Type");
       }
     }
   return rule;
}

/*----------------------------------------------------------------------
   ExportedContent compare le type de l'element en cours de lecture 
   avec le type qui doit constituer le contenu d'un        
   element exporte'. Si le type correspond, positionne les 
   indicateurs marquant qu'on doit creer tout le contenu   
   de l'element courant et qu'on ne cherche plus a` creer  
   de contenu.                                             
  ----------------------------------------------------------------------*/
static void ExportedContent (ThotBool *createAll, int *elType, PtrSSchema *pSS,
			     PtrSSchema *pContSS, int *contentType)
{
  int                 i;
  ThotBool            ok;
  PtrSRule            pSRule;

  if (*contentType != 0 && *pContSS != NULL)
    {
      ok = FALSE;
      if (!strcmp ((*pContSS)->SsName, (*pSS)->SsName))
	{
	  if (*elType == *contentType)
	    /* meme numero de type */
	    ok = TRUE;
	  else
	    {
	      pSRule = (*pContSS)->SsRule->SrElem[*contentType - 1];
	      if (pSRule->SrConstruct == CsChoice &&
		  /* le contenu a creer est un choix */
		  pSRule->SrNChoices > 0)
		/* on compare le type de l'element courant */
		/* avec toutes les options du choix */
		/* choix explicite */
		{
		  i = 0;
		  do
		    {
		      i++;
		      ok = pSRule->SrChoice[i - 1] == *elType;
		    }
		  while (!ok && i < pSRule->SrNChoices);
		}
	    }
	}
      
      if (!ok && *elType == (*pSS)->SsRootElem)
	{
	  pSRule = (*pContSS)->SsRule->SrElem[*contentType - 1];
	  if (pSRule->SrConstruct == CsNatureSchema)
	    {
	      /* le contenu cherche' est justement une racine de nature */
	      if (pSRule->SrSSchemaNat != NULL)
		ok = !strcmp (pSRule->SrSSchemaNat->SsName, (*pSS)->SsName);
	    }
	  else if (pSRule->SrConstruct == CsChoice && pSRule->SrNChoices > 0)
	    /* le contenu cherche' est un choix. Y a-t-il, parmi les */
	    /* options de ce choix, la nature dont l'element courant est */
	    /* racine? */
	    /* choix explicite */
	    {
	      i = 0;
	      do
		{
		  i++;
		  if ((*pContSS)->SsRule->SrElem[pSRule->SrChoice[i - 1] - 1]->SrConstruct == CsNatureSchema &&
		      /* l'option i est un changement de nature */
		      (*pContSS)->SsRule->SrElem[pSRule->SrChoice[i - 1] - 1]->SrSSchemaNat != NULL)
		    ok = (!strcmp ((*pContSS)->SsRule->SrElem[pSRule->SrChoice[i - 1] - 1]->SrSSchemaNat->SsName, (*pSS)->SsName));
		}
	      while (!ok && i < pSRule->SrNChoices);
	    }
	}
      if (ok)
	{
	  *createAll = TRUE;	/* on cree toute la descendance de l'element */
	  *contentType = 0;	/* on ne creera plus de contenu pour cet elem*/
	}
    }
}

/*----------------------------------------------------------------------
   CheckMandatAttrSRule verifie que l'element pointe' par pEl      
   possede les attributs requis indique's dans la regle pSRule du  
   schema de structure pSS et, si certains attributs requis        
   manquent, affiche un message d'erreur.                          
  ----------------------------------------------------------------------*/
static void CheckMandatAttrSRule (PtrElement pEl, PtrSRule pSRule, PtrSSchema pSS)
{
  PtrAttribute        pAttr;
  int                 i, att;
  ThotBool            found;

  /* parcourt tous les attributs locaux definis dans la regle */
  for (i = 0; i < pSRule->SrNLocalAttrs; i++)
    if (pSRule->SrRequiredAttr->Bln[i])
      /* cet attribut local est requis */
      {
	att = pSRule->SrLocalAttr->Num[i];
	/* cherche si l'element possede cet attribut */
	pAttr = pEl->ElFirstAttr;
	found = FALSE;
	while (pAttr != NULL && !found)
	  if (pAttr->AeAttrNum == att &&
	      !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName))
	    found = TRUE;
	  else
	    pAttr = pAttr->AeNext;
      }
}

/*----------------------------------------------------------------------
   CheckMandatoryAttr verifie que l'element pointe' par pEl possede  
   les attributs requis et, si certains attributs requis   
   manquent, affiche un message d'erreur.                  
  ----------------------------------------------------------------------*/
static void CheckMandatoryAttr (PtrElement pEl, PtrDocument pDoc)
{

  PtrSRule            pSRule;
  PtrSSchema          pSS;

  if (pEl != NULL)
    {
      /* traite d'abord les attributs requis par la regle de structure */
      /* qui definit l'element */
      pSS = pEl->ElStructSchema;
      pSRule = pSS->SsRule->SrElem[pEl->ElTypeNumber - 1];
      CheckMandatAttrSRule (pEl, pSRule, pSS);
      /* traite les attributs requis par toutes les regles d'extension de */
      /* ce type d'element */
      pSS = pDoc->DocSSchema;
      if (pSS != NULL)
	{
	  pSS = pSS->SsNextExtens;
	  /* parcourt tous les schemas d'extension du document */
	  while (pSS != NULL)
	    {
	      /* cherche dans ce schema d'extension la regle qui concerne
		 le type de l'element */
	      pSRule = ExtensionRule (pEl->ElStructSchema,
				      pEl->ElTypeNumber, pSS);
	      if (pSRule != NULL)
		/* il y a une regle d'extension, on la traite */
		CheckMandatAttrSRule (pEl, pSRule, pSS);
	      /* passe au schema d'extension suivant */
	      pSS = pSS->SsNextExtens;
	    }
	}
    }
}


/*----------------------------------------------------------------------
   ReadAttributePiv lit dans le fichier pivFile un attribut qui est sous   
   forme pivot. Le fichier doit etre positionne' juste apres       
   la Marque-Attribut (qui a deja ete lue). Au retour, le fichier  
   est positionne' sur le premier octet qui suit l'attribut        
   (prochain octet qui sera lu).                                   
   Si create est faux, rien n'est cree', l'attribut est simplement 
   saute' dans le fichier.                                         
   Si create est vrai, un attribut est cree' et  est retourne'     
   dans pReadAttr.                                                 
   ATTENTION: ReadAttributePiv utilise la table des natures du document    
  ----------------------------------------------------------------------*/
void ReadAttributePiv (BinFile pivFile, PtrElement pEl,
		       PtrDocument pDoc, ThotBool create,
		       PtrAttribute *pReadAttr,
		       PtrAttribute *pAttr)

{
   PtrSSchema          pSchAttr;
   int                 n;
   int                 attr;
   int                 val;
   ThotBool            signe;
   ReferenceType       refType;
   ThotBool            refExt;
   DocumentIdentifier  I;
   LabelString         label;
   PtrTextBuffer       pBT, pPremBuff;
   char                c;
   PtrAttribute        pA;
   PtrReference        pRef;
   ThotBool            found;
   ThotBool            stop;

   *pReadAttr = NULL;
   pSchAttr = NULL;
   pA = NULL;
   signe = FALSE;
   /* lit le numero du schema de structure definissant l'attribut */
   TtaReadShort (pivFile, &n);
   if (n < 0 || n >= pDoc->DocNNatures)
     PivotError (pivFile, "PivotError: Nature Num 2");
   else
     pSchAttr = pDoc->DocNatureSSchema[n];
   if (pSchAttr == NULL)
     PivotError (pivFile, "PivotError: Nature Num 3");
   /* lit l'attribut */
   TtaReadShort (pivFile, &attr);
   if (pDoc->DocPivotVersion < 4)
     /* on tient compte de l'ajout de l'attribut Langue */
     attr++;
   /* lit le contenu de l'attribut selon son type */
   if (!error)
     switch (pSchAttr->SsAttribute->TtAttr[attr - 1]->AttrType)
       {
       case AtEnumAttr:
	 TtaReadShort (pivFile, &val);
	 if (val > pSchAttr->SsAttribute->TtAttr[attr - 1]->AttrNEnumValues)
	   {
	     printf ("Attribute value error: %s = %d\n",
		     pSchAttr->SsAttribute->TtAttr[attr - 1]->AttrOrigName,
		     val);
	     create = FALSE;
	   }
	 break;
       case AtNumAttr:
	 TtaReadShort (pivFile, &val);
	 signe = ReadSign (pivFile);
	 break;
       case AtReferenceAttr:
	 ReadReference (&refType, label, &refExt, &I, pivFile);
	 break;
       case AtTextAttr:
	 if (!create)
	   /* on consomme le texte de l'attribut, sans le garder */
	   do
	     {
	       if (!TtaReadByte (pivFile, &c))
		 PivotError (pivFile, "Attribute");
	     }
	   while (!error && c != EOS);
	 else
	   { 
	     /* acquiert un premier buffer de texte */
	     GetTextBuffer (&pPremBuff);
	     pBT = pPremBuff;
	     /* lit tout le texte de l'attribut */
	     stop = FALSE;
	     do
	       {
		 if (!TtaReadWideChar (pivFile,
				       &pBT->BuContent[pBT->BuLength++]))
		   /* erreur de lecture */
		   PivotError (pivFile, "Attribute1");
		 else if (pBT->BuContent[pBT->BuLength - 1] == EOS)
		   /* on a lu correctement un caractere */
		   /* c'est la fin du texte de l'attribut */
		   stop = TRUE;
		 else if (pBT->BuLength >= THOT_MAX_CHAR - 1)
		   /* ce n'est pas la fin du texte de l'attribut */
		   /* le buffer courant est plein */
		   {
		     /* fin du buffer */
		     pBT->BuContent[pBT->BuLength] = EOS;
		     /* acquiert un nouveau buffer */
		     pBT = NewTextBuffer (pBT);
		   }
	       }
	     while (!error && !stop);
	     pBT->BuLength--;
	   }
	 break;
       }
   if (error)
     *pAttr = NULL;
   else
     {
       if (!create)
	 *pAttr = NULL;
       else
	 {
	   /* Si c'est un attribut impose', a valeur fixe ou modifiable, */
	   /* il a deja ete cree' par NewSubtree. On cherche si l'element */
	   /* possede deja cet attribut */
	   found = FALSE;
	   if (pEl != NULL)
	     {
	       pA = pEl->ElFirstAttr;
	       while (pA != NULL && !found)
		 {
		   if (pA->AeAttrSSchema == pSchAttr && pA->AeAttrNum == attr)
		     found = TRUE;
		   else
		     pA = pA->AeNext;
		 }
	     }
	   if (!found)
	     {
	       /* acquiert un bloc attribut pour l'element */
	       GetAttribute (pAttr);
	       /* remplit ce bloc attribut avec ce qu'on vient de lire */
	       pA = *pAttr;
	       pA->AeAttrSSchema = pSchAttr;
	       pA->AeAttrNum = attr;
	       pA->AeDefAttr = FALSE;
	       /* prend le type de l'attribut dans le schema de structure */
	       pA->AeAttrType = pA->AeAttrSSchema->SsAttribute->TtAttr[pA->AeAttrNum - 1]->AttrType;
	     }
	   else
	     *pAttr = NULL;
	   *pReadAttr = pA;
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
	       GetReference (&pRef);
	       pA->AeAttrReference = pRef;
	       pRef->RdElement = pEl;
	       pRef->RdAttribute = pA;
	       /* lie la reference a l'objet qu'elle designe */
	       CreateReference (pA->AeAttrReference, refType, label, refExt,
				I, pDoc);
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


/*----------------------------------------------------------------------
   ReadAttribute lit dans le fichier pivFile un attribut qui est   
   sous forme pivot. Le fichier doit etre positionne' juste apres  
   la Marque-Attribut (qui a deja ete lue). Au retour, le fichier  
   est positionne' sur le premier octet qui suit l'attribut        
   (prochain octet qui sera lu).                                   
   Si create est faux, rien n'est cree' dans l'arbre abstrait,     
   l'attribut est simplement saute' dans le fichier.               
   Si create est vrai, un attribut est cree' et attache' a l'element 
   pointe' par pEl; un pointeur sur cet attribut est retourne'     
   dans pReadAttr.                                                 
   ATTENTION: ReadAttribute utilise la table des natures du document  
  ----------------------------------------------------------------------*/
static void ReadAttribute (BinFile pivFile, PtrElement pEl, PtrDocument pDoc,
			   ThotBool create, PtrAttribute * pReadAttr)
{
   PtrAttribute        pAttr, pA;

   ReadAttributePiv (pivFile, pEl, pDoc, create, pReadAttr, &pAttr);
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
	/* c'est le dernier attribut */
	pAttr->AeNext = NULL;
     }
}


/*----------------------------------------------------------------------
   ReadPRulePiv lit dans le fichier pivFile une regle de           
   presentation qui est sous forme pivot. Le fichier doit etre     
   positionne' juste apres la Marque-Presentation (qui a deja ete  
   lue). Au retour, le fichier est positionne' sur le premier      
   octet qui suit la regle de presentation lue (prochain octet qui 
   sera lu).                                                       
   Si create est faux, rien n'est cree' dans l'arbre abstrait, la  
   regle de presentation est simplement saute'e dans le fichier.   
   Si create est vrai, une regle est cree'e et attache'e a l'element
   pointe' par pEl; un pointeur sur cette regle est retourne'      
   dans pRuleRead.                                                 
  ----------------------------------------------------------------------*/
void ReadPRulePiv (PtrDocument pDoc, BinFile pivFile, PtrElement pEl,
		   ThotBool create, PtrPRule *pRuleRead, ThotBool link)
{
  PRuleType           TypeRP;
  BAlignment          align;
  PictureScaling      pres;
  PtrPRule            pR1, pPRule, pPRule1;
  PosRule            *pPosRule;
  PtrPSchema          pSPR;
  PtrSSchema          pSSR;
  PtrAttribute        pAttr;
  DimensionRule      *pDimRule;
  TypeUnit            unit;
  BoxEdge             ref, def;
  RefKind             refKind;
  Level               rel;
  int                 pictureType, val, view, box;
  int                 PicXArea, PicYArea, PicWArea, PicHArea;
  int                 red, green, blue, refIdent;
  char                ch;
  ThotBool            absolute, sign, just, immed, notRel, distAttr;
  ThotBool            dimpos;

  pres = (PictureScaling) 0;
  pictureType = 0;
  just = FALSE;
  sign = FALSE;
  immed = FALSE;
  dimpos = FALSE;
  absolute = FALSE;
  align = (BAlignment) 0;
  TypeRP = (PRuleType) 0;
  *pRuleRead = NULL;
  unit = UnRelative;
  /* lit le numero de vue */
  TtaReadShort (pivFile, &view);
  /* lit le numero de la boite de presentation concernee par la regle */
  TtaReadShort (pivFile, &box);
  /* lit le type de la regle */
  if (!TtaReadByte (pivFile, &ch))
    PivotError (pivFile, "PresRule");
  switch (ch)
    {
    case C_PR_ADJUST:
      TypeRP = PtAdjust;
      break;
    case C_PR_HEIGHTPOS:
    case C_PR_HEIGHT:
      TypeRP = PtHeight;
      break;
    case C_PR_WIDTHPOS:
    case C_PR_WIDTH:
      TypeRP = PtWidth;
      break;
    case C_PR_VPOS:
      TypeRP = PtVertPos;
      break;
    case C_PR_HPOS:
      TypeRP = PtHorizPos;
      break;
    case C_PR_FONT:
      TypeRP = PtFont;
      break;
    case C_PR_STYLE:
      TypeRP = PtStyle;
      break;
    case C_PR_WEIGHT:
      TypeRP = PtWeight;
      break;
    case C_PR_UNDERLINE:
      TypeRP = PtUnderline;
      break;
    case C_PR_UNDER_THICK:
      TypeRP = PtThickness;
      break;
    case C_PR_DIRECTION:
      TypeRP = PtDirection;
      break;
    case C_PR_UNICODEBIDI:
      TypeRP = PtUnicodeBidi;
      break;
    case C_PR_LINESTYLE:
      TypeRP = PtLineStyle;
      break;
    case C_PR_BORDERTOPSTYLE:
      TypeRP = PtBorderTopStyle;
      break;
    case C_PR_BORDERRIGHTSTYLE:
      TypeRP = PtBorderRightStyle;
      break;
    case C_PR_BORDERBOTTOMSTYLE:
      TypeRP = PtBorderBottomStyle;
      break;
    case C_PR_BORDERLEFTSTYLE:
      TypeRP = PtBorderLeftStyle;
      break;
    case C_PR_BREAK1:
      TypeRP = PtBreak1;
      break;
    case C_PR_BREAK2:
      TypeRP = PtBreak2;
      break;
    case C_PR_INDENT:
      TypeRP = PtIndent;
      break;
    case C_PR_SIZE:
      TypeRP = PtSize;
      break;
    case C_PR_LINESPACING:
      TypeRP = PtLineSpacing;
      break;
    case C_PR_LINEWEIGHT:
      TypeRP = PtLineWeight;
      break;
    case C_PR_MARGINTOP:
      TypeRP = PtMarginTop;
      break;
    case C_PR_MARGINRIGHT:
      TypeRP = PtMarginRight;
      break;
    case C_PR_MARGINBOTTOM:
      TypeRP = PtMarginBottom;
      break;
    case C_PR_MARGINLEFT:
      TypeRP = PtMarginLeft;
      break;
    case C_PR_PADDINGTOP:
      TypeRP = PtPaddingTop;
      break;
    case C_PR_PADDINGRIGHT:
      TypeRP = PtPaddingRight;
      break;
    case C_PR_PADDINGBOTTOM:
      TypeRP = PtPaddingBottom;
      break;
    case C_PR_PADDINGLEFT:
      TypeRP = PtPaddingLeft;
      break;
    case C_PR_BORDERTOPWIDTH:
      TypeRP = PtBorderTopWidth;
      break;
    case C_PR_BORDERRIGHTWIDTH:
      TypeRP = PtBorderRightWidth;
      break;
    case C_PR_BORDERBOTTOMWIDTH:
      TypeRP = PtBorderBottomWidth;
      break;
    case C_PR_BORDERLEFTWIDTH:
      TypeRP = PtBorderLeftWidth;
      break;
    case C_PR_XRADIUS:
      TypeRP = PtXRadius;
      break;
    case C_PR_YRADIUS:
      TypeRP = PtYRadius;
      break;
    case C_PR_HYPHENATE:
      TypeRP = PtHyphenate;
      break;
    case C_PR_DEPTH:
      TypeRP = PtDepth;
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
    case C_PR_BORDERTOPCOLOR:
      TypeRP = PtBorderTopColor;
      break;
    case C_PR_BORDERRIGHTCOLOR:
      TypeRP = PtBorderRightColor;
      break;
    case C_PR_BORDERBOTTOMCOLOR:
      TypeRP = PtBorderBottomColor;
      break;
    case C_PR_BORDERLEFTCOLOR:
      TypeRP = PtBorderLeftColor;
      break;
    case C_PR_PICTURE:
      TypeRP = PtPictInfo;
      break;
    case C_PR_DISPLAY:
      TypeRP = PtDisplay;
      break;
    case C_PR_VISIBILITY:
      TypeRP = PtVisibility;
      break;
    default:
      PivotError (pivFile, "PresRule1");
      break;
    }

  if (!error)
    /* lit les parametres de la regle selon son type */
    switch (TypeRP)
      {
      case PtAdjust:
	align = ReadAlign (pivFile);
	break;
      case PtHeight:
      case PtWidth:
	if (ch == C_PR_WIDTHPOS || ch == C_PR_HEIGHTPOS)
	  {
	    /* get a complete rule */
	    dimpos = TRUE;
	    TtaReadShort (pivFile, (int *) &def);
	    TtaReadShort (pivFile, (int *) &ref);
	    TtaReadShort (pivFile, (int *) &rel);
	    TtaReadShort (pivFile, &val);
	    unit = ReadUnit (pivFile);
	    sign = ReadSign (pivFile);
	  }
	else
	  {
	    dimpos = FALSE;
	    absolute = ReadDimensionType (pivFile);
	    TtaReadShort (pivFile, &val);
	    unit = ReadUnit (pivFile);
	    sign = ReadSign (pivFile);
	  }
	break;
      case PtVertPos:
      case PtHorizPos:
	if (pDoc->DocPivotVersion >= 6)
	  {
	    /* get a complete rule */
	    TtaReadShort (pivFile, (int *) &def);
	    TtaReadShort (pivFile, (int *) &ref);
	    distAttr = ReadBoolean (pivFile);
	    TtaReadShort (pivFile, (int *) &rel);
	    notRel = ReadBoolean (pivFile);
	    TtaReadShort (pivFile, (int *) &refKind);
	    TtaReadShort (pivFile, &refIdent);
	  }
	TtaReadShort (pivFile, &val);
	unit = ReadUnit (pivFile);
	sign = ReadSign (pivFile);
	break;
      case PtBreak1:
      case PtBreak2:
      case PtSize:
      case PtLineSpacing:
      case PtLineWeight:
      case PtPaddingTop:
      case PtPaddingRight:
      case PtPaddingBottom:
      case PtPaddingLeft:
      case PtBorderTopWidth:
      case PtBorderRightWidth:
      case PtBorderBottomWidth:
      case PtBorderLeftWidth:
      case PtXRadius:
      case PtYRadius:
	immed = ReadBoolean (pivFile);
	TtaReadShort (pivFile, &val);
	unit = ReadUnit (pivFile);
	break;
      case PtIndent:
      case PtMarginTop:
      case PtMarginRight:
      case PtMarginBottom:
      case PtMarginLeft:
	immed = ReadBoolean (pivFile);
	TtaReadShort (pivFile, &val);
	unit = ReadUnit (pivFile);
	sign = ReadSign (pivFile);
	break;
      case PtFillPattern:
      case PtDepth:
	TtaReadShort (pivFile, &val);
	break;
      case PtBackground:
      case PtForeground:
      case PtBorderTopColor:
      case PtBorderRightColor:
      case PtBorderBottomColor:
      case PtBorderLeftColor:
	if (pDoc->DocPivotVersion < 5)
	  TtaReadShort (pivFile, &val);
	else
	  {
	    if (pDoc->DocPivotVersion >= 6)
	      TtaReadShort (pivFile, &val);
	    TtaReadShort (pivFile, &red);
	    TtaReadShort (pivFile, &green);
	    TtaReadShort (pivFile, &blue);
	  }
	break;
      case PtFont:
      case PtStyle:
      case PtWeight:
      case PtUnderline:
      case PtThickness:
      case PtDirection:
      case PtUnicodeBidi:
      case PtLineStyle:
      case PtDisplay:
      case PtVisibility:
      case PtBorderTopStyle:
      case PtBorderRightStyle:
      case PtBorderBottomStyle:
      case PtBorderLeftStyle:
	if (!TtaReadByte (pivFile, &ch))
	  PivotError (pivFile, "PivotError: PresRule 3");
	break;
      case PtHyphenate:
	just = ReadBoolean (pivFile);
	break;
      case PtPictInfo:
	TtaReadShort (pivFile, &PicXArea);
	TtaReadShort (pivFile, &PicYArea);
	TtaReadShort (pivFile, &PicWArea);
	TtaReadShort (pivFile, &PicHArea);
	pres = ReadPicturePresentation (pivFile);
	TtaReadShort (pivFile, &pictureType);
	/* Pour assurer la compatibilite avec Linux et autre machine */
	if (pictureType == 255)
	  pictureType = -1;
	SetImageRule (pEl, PicXArea, PicYArea, PicWArea, PicHArea, pictureType, pres);
	create = FALSE;
	break;
      default:
	break;
      }

  if (create)
    {
      GetPresentRule (&pPRule);
      *pRuleRead = pPRule;
      /* initialise d'abord la nouvelle regle */
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrSpecifAttr = 0;
      pPRule->PrSpecifAttrSSchema = NULL;
      pAttr = NULL;
      pPRule->PrViewNum = view;
      pPRule->PrType = TypeRP;
      /* charge les parametres de la regle selon son type */
      if (!error)
	switch (pPRule->PrType)
	  {
	  case PtAdjust:
	    pPRule->PrAdjust = align;
	    break;
	  case PtHeight:
	  case PtWidth:
	    pDimRule = &pPRule->PrDimRule;
	    pDimRule->DrPosition = dimpos;
	    if (dimpos)
	      {
		pDimRule->DrPosRule.PoPosDef = def;
		pDimRule->DrPosRule.PoPosRef = ref;
		pDimRule->DrPosRule.PoRelation = rel;
		pDimRule->DrPosRule.PoDistance = val;
		pDimRule->DrPosRule.PoDistUnit = unit;
		if (!sign)
		  pDimRule->DrPosRule.PoDistance =
		                             -pDimRule->DrPosRule.PoDistance;
	      }
	    else
	      {
		pDimRule->DrAbsolute = absolute;
		if (!pDimRule->DrAbsolute)
		  /* c'est une dimension relative, on prend */
		  /* la regle qui devrait s'appliquer a     */
		  /* l'element, puis on la modifie selon    */
		  /* ce qui est lu dans le fichier          */
		  {
		    pR1 = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR, 0,
					       NULL, pPRule->PrViewNum,
					       pPRule->PrType, FnAny, FALSE,
					       TRUE, &pAttr);
		    if (pR1 != NULL)
		      {
			*pPRule = *pR1;
			pPRule->PrViewNum = view;
			pPRule->PrNextPRule = NULL;
			pPRule->PrCond = NULL;
		      }
		  }
		pDimRule->DrAttr = FALSE;
		pDimRule->DrValue = val;
		pDimRule->DrUnit = unit;
		if (!sign)
		  pDimRule->DrValue = -pDimRule->DrValue;
		pDimRule->DrMin = FALSE;
	      }
	    break;
	  case PtVertPos:
	  case PtHorizPos:
	    pPosRule = &pPRule->PrPosRule;
	    if (pDoc->DocPivotVersion >= 6)
	      {
		pPosRule->PoPosDef = def;
		pPosRule->PoPosRef = ref;
		pPosRule->PoDistAttr = distAttr;
		pPosRule->PoRelation = rel;
		pPosRule->PoNotRel = notRel;
		pPosRule->PoRefKind = refKind;
		pPosRule->PoRefIdent = refIdent;
	      }
	    else
	      {
		/* c'est une position relative, on prend */
		/* la regle qui devrait s'appliquer a    */
		/* l'element, puis on la modifie selon   */
		/* ce qui est lu dans le fichier         */
		pR1 = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR, 0, NULL,
					   pPRule->PrViewNum,
					   pPRule->PrType, FnAny, FALSE,
					   TRUE, &pAttr);
		if (pR1 != NULL)
		  *pPRule = *pR1;
		pPRule->PrViewNum = view;
		pPRule->PrNextPRule = NULL;
		pPRule->PrCond = NULL;
		pPosRule->PoDistAttr = FALSE;
	      }
	    pPosRule->PoUserSpecified = FALSE;
	    pPosRule->PoDistance = val;
	    pPosRule->PoDistUnit = unit;
	    if (!sign)
	      pPosRule->PoDistance = -pPosRule->PoDistance;
	    break;
	  case PtBreak1:
	  case PtBreak2:
	  case PtSize:
	  case PtLineSpacing:
	  case PtLineWeight:
	  case PtPaddingTop:
	  case PtPaddingRight:
	  case PtPaddingBottom:
	  case PtPaddingLeft:
	  case PtBorderTopWidth:
	  case PtBorderRightWidth:
	  case PtBorderBottomWidth:
	  case PtBorderLeftWidth:
          case PtXRadius:
          case PtYRadius:
	    if (immed)
	      {
		pPRule->PrMinAttr = FALSE;
		pPRule->PrMinValue = val;
		pPRule->PrMinUnit = unit;
	      }
	    else
	      {
		pPRule->PrPresMode = PresInherit;
		pPRule->PrInheritMode = InheritParent;
		pPRule->PrInhPercent = TRUE;
		pPRule->PrInhAttr = 0;
		pPRule->PrInhDelta = val;
		pPRule->PrMinMaxAttr = FALSE;
		pPRule->PrInhMinOrMax = 0;
		pPRule->PrInhUnit = unit;
	      }
	    break;
	  case PtIndent:
	  case PtMarginTop:
	  case PtMarginRight:
	  case PtMarginBottom:
	  case PtMarginLeft:
	    if (immed)
	      {
		pPRule->PrMinAttr = FALSE;
		pPRule->PrMinValue = val;
		pPRule->PrMinUnit = unit;
		if (!sign)
		  pPRule->PrMinValue = -pPRule->PrMinValue;
	      }
	    else
	      {
		pPRule->PrPresMode = PresInherit;
		pPRule->PrInheritMode = InheritParent;
		pPRule->PrInhPercent = TRUE;
		pPRule->PrInhAttr = 0;
		pPRule->PrInhDelta = val;
		pPRule->PrMinMaxAttr = FALSE;
		pPRule->PrInhMinOrMax = 0;
		pPRule->PrInhUnit = unit;
		if (!sign)
		  pPRule->PrInhDelta = -pPRule->PrInhDelta;
	      }
	    break;
	  case PtDepth:
	  case PtFillPattern:
	    pPRule->PrAttrValue = FALSE;
	    pPRule->PrIntValue = val;
	    break;
	  case PtBackground:
	  case PtForeground:
	  case PtBorderTopColor:
	  case PtBorderRightColor:
	  case PtBorderBottomColor:
	  case PtBorderLeftColor:
	    pPRule->PrAttrValue = FALSE;
	    /* convertit les couleurs des anciennes versions */
	    if (pDoc->DocPivotVersion < 4)
	      val = newColor[val];
	    if (pDoc->DocPivotVersion < 5)
	      pPRule->PrIntValue = val;
	    else if (pDoc->DocPivotVersion >= 6 && val == 1)
	      pPRule->PrIntValue = -2;
	    else
	      pPRule->PrIntValue = TtaGetThotColor ((unsigned short) red,
						    (unsigned short) green,
						    (unsigned short) blue);
	    break;
	  case PtFont:
	  case PtStyle:
	  case PtWeight:
	  case PtUnderline:
	  case PtThickness:
	  case PtDirection:
	  case PtUnicodeBidi:
	  case PtLineStyle:
	  case PtDisplay:
 	  case PtVisibility:
	  case PtBorderTopStyle:
          case PtBorderRightStyle:
          case PtBorderBottomStyle:
          case PtBorderLeftStyle:
	    pPRule->PrChrValue = ch;
	    break;
	  case PtHyphenate:
	    pPRule->PrBoolValue = just;
	    break;
	  default:
	    break;
	  }
      /* whether the presentation rule is related to an attribute */
      /* keep the link with that attribute */
      if (pAttr != NULL)
	{
	  pPRule->PrSpecifAttr = pAttr->AeAttrNum;
	  pPRule->PrSpecifAttrSSchema = pAttr->AeAttrSSchema;
	}
      
      if (link)
	{
	  /* add the new presentation rule at the end */
	  if (pEl->ElFirstPRule == NULL)
	    /* no presentation rule for the element */
	    pEl->ElFirstPRule = pPRule;
	  else
	    {
	      /* first presentation rule of the element */
	      pPRule1 = pEl->ElFirstPRule;
	      /* look for the last rule of the element */
	      while (pPRule1->PrNextPRule != NULL)
		pPRule1 = pPRule1->PrNextPRule;
	      /* add the new rule */
	      pPRule1->PrNextPRule = pPRule;
	    }
	  if (pPRule->PrType == PtBackground &&
	      !TypeHasException (ExcNoShowBox, pEl->ElTypeNumber,
				 pEl->ElStructSchema))
	    /* add a ShowBox rule for the Background rule */
	    {
	      GetPresentRule (&pPRule1);
	      pPRule->PrNextPRule = pPRule1;
	      pPRule = pPRule1;
	      pPRule->PrType = PtFunction;
	      pPRule->PrViewNum = view;
	      pPRule->PrPresMode = PresFunction;
	      pPRule->PrPresFunction = FnShowBox;
	      pPRule->PrPresBoxRepeat = FALSE;
	      pPRule->PrNPresBoxes = 0;
	    }
	}
      /* it's the last rule */
      pPRule->PrNextPRule = NULL;
    }
}


/*----------------------------------------------------------------------
   SendEventAttrRead       envoie les evenements TteAttrRead       
   pour les attributs de l'element pEl qui vient d'etre lu 
  ----------------------------------------------------------------------*/
void         SendEventAttrRead (PtrElement pEl, PtrDocument pDoc)
{
   PtrAttribute        pAttr, pNextAttr;
   NotifyAttribute     notifyAttr;

   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)
     {
       /* prepare et envoie l'evenement AttrRead.Pre s'il est demande' */
       notifyAttr.event = TteAttrRead;
       notifyAttr.document = (Document) IdentDocument (pDoc);
       notifyAttr.element = (Element) pEl;
       notifyAttr.attribute = NULL;
       notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
       notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
       pNextAttr = pAttr->AeNext;
       if (CallEventAttribute (&notifyAttr, TRUE))
	 /* l'application ne veut pas lire l'attribut */
	 /* on l'avait deja lu, on le retire */
	 {
	   RemoveAttribute (pEl, pAttr);
	   DeleteAttribute (pEl, pAttr);
	 }
       else
	 {
	   /* prepare et envoie l'evenement AttrRead.Post s'il est demande' */
	   notifyAttr.event = TteAttrRead;
	   notifyAttr.document = (Document) IdentDocument (pDoc);
	   notifyAttr.element = (Element) pEl;
	   notifyAttr.attribute = (Attribute) pAttr;
	   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
	   notifyAttr.attributeType.AttrSSchema =
	                                     (SSchema) (pAttr->AeAttrSSchema);
	   CallEventAttribute (&notifyAttr, FALSE);
	 }
       /* passe a l'attribut suivant de l'element */
       pAttr = pNextAttr;
     }
}


/*----------------------------------------------------------------------
   ReadTreePiv effectue la traduction de la forme pivot commencant 
   a` la position courante dans pivFile vers la            
   representation interne. Au retour le fichier est        
   positionne' apres la partie traduite. Le fichier doit   
   etre ouvert avant l'appel et il reste ouvert au retour. 
   Le fichier doit etre positionne' sur un numero de type  
   ou un nom de nature.                                    
   - pSSchema: pointeur sur le schema de structure courant.
   - pDoc:    pointeur sur le contexte du document en      
   cours de lecture.                                       
   - tag:  tag precedent le numero de type ou le nom 	
   de nature. Au retour: 1er octet suivant l'element.      
   - createAll:indique qu'il faut creer tous les elements  
   qui descendent de l'element courant et qui sont dans le 
   fichier. Si createAll est faux, on ne cree que les      
   elements exportes et dans ceux-ci tout le sous-arbre    
   des elements de type contentType definis dans le schema 
   de structure pointe' par pContSS.                       
   - typeRead:  au retour, indique le type de l'element lu,
   qu'il ait ete cree' ou pas.                             
   - pSSRead: au retour, pointeur sur le schema de         
   structure de l'element lu, qu'il ait ete cree' ou pas.  
   - Pere: element qui sera le pere de l'element lu.       
   - createDesc: si cree<desc est faux, on ne cree pas     
   l'element lu ni sa descendance. Prioritaire sur createAll
  ----------------------------------------------------------------------*/
PtrElement ReadTreePiv (BinFile pivFile, PtrSSchema pSSchema, PtrDocument pDoc,
			char *tag, ThotBool createAll,
			int *contentType, PtrSSchema *pContSS, int *typeRead,
			PtrSSchema *pSSRead, ThotBool createPage,
			PtrElement pParent, ThotBool createDesc)
{
  PtrAttribute        pAttr;
  PtrTextBuffer       pBuf;
  PtrPathSeg          pPa, pPaPrev;
  PtrElement          pPrevEl, p, pEl, pEl2, pElInt, pElRead, pfutParent;
  PtrPRule            pPRule;
  PtrSSchema          pSS;
  PtrReferredDescr    pRefD;
  PtrSRule            pSRule;
static  LabelString         label;
  DocumentIdentifier  docIdent;
  BasicType           leafType;
  PageType            pageType;
  ReferenceType       refType;
  NotifyElement       notifyEl;
  PictInfo           *image;
  int                 i, j, n, view, l, elType;
  int                 rule, n1, n2, n3, n4;
  char                script;
  char                c;
  CHAR_T              ctext;
  ThotBool            create, inclusion, modif, b1, b2;
  ThotBool            refExt, found, withReferences, sign, newSubPath;
  
  pSRule = NULL;
  pEl = NULL;
  pPaPrev = NULL;
  withReferences = FALSE;
  create = FALSE;
  if (*tag != C_PIV_TYPE && *tag != C_PIV_NATURE)
    {
      i = 1;
      while (!error && i < 200)
	{
	  if (!TtaReadByte (pivFile, &c))
	    PivotError (pivFile, "Type 1");
	  else
	    {
	      if (c < SPACE)
		{
		  printf ("^");
		  c = (((int) c) + ((int) '@'));
		  i++;
		}
	      printf ("%c", c);
	      i++;
	    }
	}
      printf ("\n");
      PivotError (pivFile, "I");
    }
  else
    {
      /* lit le type de l'element dans le fichier */
      elType = ReadType (pDoc, &pSSchema, pivFile, tag);
      if (!error)
	{
	  *typeRead = elType;	/* numero de type de l'element en cours */
	  *pSSRead = pSSchema;	/* schema de structure de l'element en cours */
	  if (!createAll)
	    /* on ne cree que les elements de type exporte'. L'element en cours
	       de lecture est-il du type qui constitue le contenu de l'element
	       exporte' englobant ? */
	    ExportedContent (&createAll, &elType, &pSSchema, pContSS,
			     contentType);
	  pSRule = pSSchema->SsRule->SrElem[elType - 1];
	  if (createAll)
	    create = TRUE;
	  else
	    /* on ne cree que les elements de type exporte' */
	    {
	      create = FALSE;
	      if (pSRule->SrExportedElem)
		/* l'element est d'un type exporte' */
		if (pSRule->SrExportContent != 0)
		  /* on veut creer au moins une partie de son contenu */
		  {
		    create = TRUE;	/* on le cree */
		    *contentType = pSRule->SrExportContent;
		    /* il faudra creer son contenu */
		    /* cherche le schema de structure ou est */
		    /* defini son contenu */
		    if (pSRule->SrNatExpContent[0] == EOS)
		      /* meme schema de structure */
		      *pContSS = pSSchema;
		    else
		      /* cherche dans la table des natures du document */
		      {
			i = 0;
			do
			  i++;
			while (pDoc->DocNatureName[i - 1] !=
			                        pSRule->SrNatExpContent &&
			       i != pDoc->DocNNatures);
			if (pDoc->DocNatureName[i - 1] ==
			                             pSRule->SrNatExpContent)
			  /* trouve' */
			  *pContSS = pDoc->DocNatureSSchema[i - 1];
			else
			  /* la nature du contenu n'est pas chargee */
			  {
			    *pContSS = NULL;
			    *contentType = 0;
			    /* il faut peut-etre aussi creer */
			    /* tout le contenu de l'element */
			  }
		      }
		    ExportedContent (&createAll, &elType, &pSSchema, pContSS,
				     contentType);
		  }
	      if (elType == PageBreak + 1)
		if (createPage)
		  {
		    create = TRUE;	/* on le cree */
		    *contentType = elType;
		    *pContSS = pSSchema;
		    
		    /* il faudra creer son contenu */
		    /* cherche le schema de structure ou est */
		    /* defini son contenu */
		    ExportedContent (&createAll, &elType, &pSSchema, pContSS,
				     contentType);
		  }
	    }
	  if (createDesc)
	    {
	      notifyEl.event = TteElemRead;
	      notifyEl.document = (Document) IdentDocument (pDoc);
	      notifyEl.element = (Element) pParent;
	      notifyEl.elementType.ElTypeNum = *typeRead;
	      notifyEl.elementType.ElSSchema = (SSchema) (*pSSRead);
	      notifyEl.position = 0;
	      if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		/* l'application ne veut pas lire le sous-arbre */
		{
		  create = FALSE;
		  createDesc = FALSE;
		}
	    }
	  /* on cree toujours la racine du document */
	  if (!create)
	    if (pSSchema == pDoc->DocSSchema)
	      if (elType == pSSchema->SsRootElem ||
		  elType == pSSchema->SsDocument)
		/* c'est la racine, on cree */
		create = TRUE;
	  if (!create)
	    pEl = NULL;
	  else
	    /* cree un element du type lu */
	    {
	      /* il ne faut pas que le label max. du document augmente */
	      pEl = NewSubtree (elType, pSSchema, pDoc, FALSE, TRUE, FALSE,
				FALSE);
	      if (pEl != NULL)
		pEl->ElLabel[0] = EOS;
	    }

	  if (!TtaReadByte (pivFile, tag))
	    PivotError (pivFile, "PivotError: Type 2");
	}
      inclusion = FALSE;	/* est-ce une reference a un element inclus? */
      if (!error && *tag == C_PIV_INCLUDED)
	/* oui, lit la reference */
	{
	  inclusion = TRUE;
	  ReadReference (&refType, label, &refExt, &docIdent, pivFile);
	  if (create)
	    {
	      GetReference (&pEl->ElSource);
	      pEl->ElSource->RdElement = pEl;
	      CreateReference (pEl->ElSource, refType, label, refExt,
			       docIdent, pDoc);
	      pEl->ElIsCopy = TRUE;
	    }
	  if (!TtaReadByte (pivFile, tag))
	    PivotError (pivFile, "PivotError: Type 3");
	}
      
      /* lit le tag "Element-reference'" si elle est presente */
      if (!error)
	{
	if (*tag == C_PIV_REFERRED)
	  {
	    withReferences = TRUE;
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile, "PivotError: Type 4");
	  }
	else
	  withReferences = FALSE;
	}

      /* traite le label s'il est present */
      label[0] = EOS;
      if (!error)
	if (*tag == C_PIV_SHORT_LABEL || *tag == C_PIV_LONG_LABEL ||
	    *tag == C_PIV_LABEL)
	  {
	    ReadLabel (*tag, label, pivFile);
	    /* lit le tag qui suit le label */
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile, "PivotError: Label");
	  }
      if (!error && label[0] != EOS && create)
	/* l'element porte un label */
	{
	  strncpy (pEl->ElLabel, label, MAX_LABEL_LEN);
	  if (!withReferences)
	    /* on verifie si cet element (ou plutot son label) est dans la */
	    /* chaine des elements reference's de l'exterieur */
	    {
	      pRefD = pDoc->DocLabels;
	      while (pRefD != NULL && !withReferences)
		if (strcmp (pRefD->ReReferredLabel, label) == 0)
		  withReferences = TRUE;
		else
		  pRefD = pRefD->ReNext;
	    }
	  if (!error)
	    if (pDoc->DocPivotVersion < 3 || withReferences)
	      /* on associe a l'element un descripteur d'element reference' */
	      {
		ClearDocIdent (&docIdent);
		pEl->ElReferredDescr = GetElRefer (label, docIdent, pDoc);
		if (pEl->ElReferredDescr->ReReferredElem != NULL)
		  /* on a deja lu dans ce document un element */
		  /* portant ce label, erreur */
		  {
		    pEl->ElReferredDescr = NULL;
		    /*uprintf ("Label");*/
		  }
		else
		  pEl->ElReferredDescr->ReReferredElem = pEl;
	      }
	}

      /* lit le tag d'holophraste si elle est presente */
      if (!error && create)
	pEl->ElHolophrast = FALSE;
      if (*tag == C_PIV_HOLOPHRAST && !error)
	{
	  if (create)
	    pEl->ElHolophrast = TRUE;
	  /* lit l'octet qui suit */
	  if (!TtaReadByte (pivFile, tag))
	    PivotError (pivFile, "PivotError: Holophrast");
	}
      /* lit les attributs de l'element s'il y en a */
      while (*tag == C_PIV_ATTR && !error)
	{
	  ReadAttribute (pivFile, pEl, pDoc, create, &pAttr);
	  if (!error)
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile, "PivotError: Attr");
	}
      /* tous les attributs de l'element sont lus, on verifie qu'il ne */
      /* manque pas d'attributs locaux obligatoires pour l'element */
      if (!error && create)
	CheckMandatoryAttr (pEl, pDoc);
      
      /* lit les regles de presentation de l'element. Etablit d'abord le
	 chainage de l'element avec son pere pour que la procedure
	 GlobalSearchRulepEl appelee par ReadPRulePiv puisse trouver les
         regles de presentation heritees des attributs des ascendants */
      if (pEl != NULL)
	pEl->ElParent = pParent;
      while (*tag == C_PIV_PRESENT && !error)
	{
	  ReadPRulePiv (pDoc, pivFile, pEl, create, &pPRule, TRUE);
	  if (!error)
	    /* lit l'octet qui suit la regle */
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile, "PivotError: Rule");
	}
      if (!error)
	/* si l'element est une copie par inclusion, il n'a pas de contenu */
	if (!inclusion)
	  /* lit le contenu de l'element create */
	  /* traitement specifique selon le constructeur de l'element */
	  switch (pSSchema->SsRule->SrElem[elType - 1]->SrConstruct)
	    {
	    case CsReference:
	      if (*tag != C_PIV_REFERENCE)
		PivotError (pivFile, "Reference");	/* erreur */
	      else
		/* traitement des references : on lit la reference */
		{
		  ReadReference (&refType, label, &refExt, &docIdent, pivFile);
		  if (create)
		    CreateReference (pEl->ElReference, refType, label, refExt,
				     docIdent, pDoc);
		  if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile, "Reference 1");
		}
	      break;
	    case CsPairedElement:
	      if (*tag != C_PIV_BEGIN)
		/* erreur, pas de tag debut */
		PivotError (pivFile, "PairedElement");
	      else
		/* traitement des paires : on lit l'identificateur */
		{
		  TtaReadInteger (pivFile, &i);
		  if (create)
		    pEl->ElPairIdent = i;
		  if (i > pDoc->DocMaxPairIdent)
		    pDoc->DocMaxPairIdent = i;
		  if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile, "PivotError: PairedElement 1");
		  if (*tag != C_PIV_END)
		    /* erreur, pas de tag de fin */
		    PivotError (pivFile, "PairedElement 2");
		  else if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile, "PivotError: PairedElement 3");
		}
	      break;
	    case CsBasicElement:
	      leafType = pSSchema->SsRule->SrElem[elType - 1]->SrBasicType;
	      if (leafType == CharString)
		{
		if (pDoc->DocPivotVersion >= 4)
		  {
		    if (*tag != C_PIV_LANG)
		      /* pas de tag de langue, c'est la premiere langue de la
		         table des langues du document */
		      i = 0;
		    else
		      {
			/* lit le numero de langue (pour la table des langues
			   du document */
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile, "Language");
			else
			  i = (int) (*tag);
			/* lit l'octet suivant */
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile, "Laguage 1");
		      }
		    if (create && !error)
		      {
			/* i est le rang de la langue dans la table des */
			/* langues du document */
			if (i < 0 || i >= pDoc->DocNLanguages)
			  {
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
		    script = 'L';
		    /* dans le cas d'une inclusion sans expansion, il */
		    /* n'y a pas d'script. */
		    /* dans les versions pivot anciennes, il peut y avoir un */
		    /* tag d'script. On le saute */
		    if (*tag != C_PIV_BEGIN && *tag != C_PIV_END &&
			*tag != C_PIV_TYPE && *tag != C_PIV_NATURE)
		      /* on a lu l'script */
		      {
			script = *tag;
			/* lit l'octet suivant */
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile, "PivotError: Script");
		      }
		    if (create)
		      {
			pEl->ElLanguage = TtaGetLanguageIdFromScript (script);
			/* verifie que la langue est dans la table des langues
			   du document */
			found = FALSE;
			for (i = 0; i < pDoc->DocNLanguages && !found; i++)
			  if (pDoc->DocLanguages[i] == pEl->ElLanguage)
			    found = TRUE;
			if (!found && pDoc->DocNLanguages < MAX_LANGUAGES_DOC)
			  /* elle n'y est pas, on la met */
			  {
			    pDoc->DocLanguages[pDoc->DocNLanguages] = pEl->ElLanguage;
			    pDoc->DocNLanguages++;
			  }
		      }
		  }
		}
	      
	      if (*tag == C_PIV_BEGIN && !error)
		{
		  if (leafType == CharString || leafType == Picture)
		    {
		      if (!TtaReadWideChar (pivFile, &ctext))
			PivotError (pivFile, "PivotError: Text 1");
		      if (ctext != (CHAR_T) C_PIV_END)
			{
			  /* not empty contents */
			  if (!create || ctext == EOS)
			    /* skip the text */
			    {
			      while (ctext != EOS && !error)
				if (!TtaReadWideChar (pivFile, &ctext))
				  PivotError (pivFile, "PivotError: Text 2");
			    }
			  else
			    /* read the text into the element buffers */
			    {
			      pBuf = pEl->ElText;
			      n = 0;
			      pEl->ElTextLength = 0;
			      do
				{
				  if (n == THOT_MAX_CHAR - 1)
				    {
				      if (leafType == Picture)
					PivotError (pivFile, "PivotError: Picture 2");
				      pEl->ElTextLength += n;
				      pBuf->BuLength = n;
				      pBuf->BuContent[n] = EOS;
				      pBuf = NewTextBuffer (pBuf);
				      n = 0;
				    }
				  n++;
				  /* store the character and read the next one */
				  pBuf->BuContent[n - 1] = ctext;
				  if (!TtaReadWideChar (pivFile, &ctext))
				    PivotError (pivFile, "PivotError: Text 3");
				}
			      while (ctext != EOS);
			      pEl->ElTextLength += n;
			      pBuf->BuLength = n;
			      pBuf->BuContent[n] = EOS;
			      pEl->ElVolume = pEl->ElTextLength;
			      if (leafType == Picture)
				{
				  /* complete the Picture information block */
				  image = (PictInfo *)pEl->ElPictInfo;
				  if (image != NULL)
				    {
				      l = ustrlen (pBuf->BuContent) * 2;
				      image->PicFileName = TtaGetMemory (l + 1);
				      CopyBuffer2MBs (pBuf, 0, image->PicFileName, l);
				    }
				}
			    }
			  if (!TtaReadByte (pivFile, tag))
			    PivotError (pivFile, "PivotError: Text 4");
			}
		    }
		  else if (leafType == PageBreak)
		    {
		      /* read the page number and the type of page */
		      TtaReadShort (pivFile, &n);
		      TtaReadShort (pivFile, &view);
		      pageType = ReadPageType (pivFile);
		      modif = ReadBoolean (pivFile);
		      if (create)
			{
			  pEl->ElPageNumber = n;
			  pEl->ElViewPSchema = view;
			  pEl->ElPageType = pageType;
			  pEl->ElPageModified = modif;
			}
		      if (!TtaReadByte (pivFile, tag))
			PivotError (pivFile, "PivotError: PageBreak");
		    }
		  else
		    {
		      if (!TtaReadByte (pivFile, tag))
			PivotError (pivFile, "PivotError: Text 1");
		      if (*tag != C_PIV_END &&
			  (leafType == Symbol || leafType == GraphicElem))
			{
			  /* the code that gives the form was read */
			  c = *tag;
			  /* read the next byte */
			  if (!TtaReadByte (pivFile, tag))
			    PivotError (pivFile, "PivotError: Graphic 1");
			  else if (*tag == C_PIV_POLYLINE)
			    {
			      /* read the number of points in the polyline */
			      if (!TtaReadShort (pivFile, &n))
				PivotError (pivFile, "PivotError: Polyline");
			      /* read all points */
			      else if (!create)
				for (i = 0; i < n; i++)
				  TtaReadInteger (pivFile, &j);
			      else
				{
				  /* transform into a Polyline */
				  pEl->ElLeafType = LtPolyLine;
				  GetTextBuffer (&pEl->ElPolyLineBuffer);
				  pEl->ElVolume = n;
				  pEl->ElPolyLineType = c;
				  pEl->ElNPoints = n;
				  pBuf = pEl->ElPolyLineBuffer;
				  j = 0;
				  for (i = 0; i < n; i++)
				    {
				      if ((unsigned)j >= MAX_POINT_POLY)
					/* buffer if full */
					{
					  pBuf = NewTextBuffer (pBuf);
					  j = 0;
					}
				      TtaReadInteger (pivFile,
						    &pBuf->BuPoints[j].XCoord);
				      TtaReadInteger (pivFile,
						    &pBuf->BuPoints[j].YCoord);
				      pBuf->BuLength++;
				      j++;
				    }
				}
			      /* lit l'octet qui suit (tag de fin d'element) */
			      if (!TtaReadByte (pivFile, tag))
				PivotError (pivFile, "PivotError: Polyline 1");
			    }
			  else if (*tag == C_PIV_PATH)
			    /* c'est un path graphique */
			    {
			      if (create)
				/* transforme l'element graphique simple
				   en un path */
				{
				  pEl->ElLeafType = LtPath;
				  pEl->ElFirstPathSeg = NULL;
				  pPaPrev = NULL;
				  n = 0;
				}
			      /* lit l'octet qui suit (type d'element de path
			         ou tag de fin d'element) */
			      if (!TtaReadByte (pivFile, tag))
				PivotError (pivFile, "PivotError: Path 1");
			      else
				{
				while (*tag != C_PIV_END)
				  {
				  newSubPath = ReadBoolean (pivFile);
				  sign = ReadSign (pivFile);
				  TtaReadInteger (pivFile, &n1);
				  if (!sign) n1 = -n1;
				  sign = ReadSign (pivFile);
				  TtaReadInteger (pivFile, &n2);
				  if (!sign) n2 = -n2;
				  sign = ReadSign (pivFile);
				  TtaReadInteger (pivFile, &n3);
				  if (!sign) n3 = -n3;
				  sign = ReadSign (pivFile);
				  TtaReadInteger (pivFile, &n4);
				  if (!sign) n4 = -n4;
				  if (create)
				    {
				    GetPathSeg (&pPa);
				    if (pPaPrev)
				      pPaPrev->PaNext = pPa;
				    else
				      pEl->ElFirstPathSeg = pPa;
				    pPa->PaPrevious = pPaPrev;
				    pPa->PaNext = NULL;
				    pPaPrev = pPa;
				    n++;
				    pPa->PaNewSubpath = newSubPath;
				    pPa->XStart = n1;
				    pPa->YStart = n2;
				    pPa->XEnd = n3;
				    pPa->YEnd = n4;
				    }
				  switch ((char) *tag)
				    {
				    case 'L':
				      if (create)
				        pPa->PaShape = PtLine;
				      break;
				    case 'C':
				      sign = ReadSign (pivFile);
				      TtaReadInteger (pivFile, &n1);
				      if (!sign) n1 = -n1;
				      sign = ReadSign (pivFile);
				      TtaReadInteger (pivFile, &n2);
				      if (!sign) n2 = -n2;
				      sign = ReadSign (pivFile);
				      TtaReadInteger (pivFile, &n3);
				      if (!sign) n3 = -n3;
				      sign = ReadSign (pivFile);
				      TtaReadInteger (pivFile, &n4);
				      if (!sign) n4 = -n4;
				      if (create)
					{
					pPa->PaShape = PtCubicBezier;
					pPa->XCtrlStart = n1;
					pPa->YCtrlStart = n2;
					pPa->XCtrlEnd = n3;
					pPa->YCtrlEnd = n4;
					}
				      break;
				    case 'Q':
				      sign = ReadSign (pivFile);
				      TtaReadInteger (pivFile, &n1);
				      if (!sign) n1 = -n1;
				      sign = ReadSign (pivFile);
				      TtaReadInteger (pivFile, &n2);
				      if (!sign) n2 = -n2;
				      if (create)
					{
					pPa->PaShape = PtQuadraticBezier;
					pPa->XCtrlStart = n1;
					pPa->YCtrlStart = n2;
					}
				      break;
				    case 'A':
				      TtaReadInteger (pivFile, &n1);
				      TtaReadInteger (pivFile, &n2);
				      sign = ReadSign (pivFile);
				      TtaReadShort (pivFile, &n3);
				      if (!sign) n3 = -n3;
				      b1 = ReadBoolean (pivFile);
				      b2 = ReadBoolean (pivFile);
				      if (create)
					{
					pPa->PaShape = PtEllipticalArc;
					pPa->XRadius = n1;
					pPa->YRadius = n2;
					pPa->XAxisRotation = n3;
					pPa->LargeArc = b1;
					pPa->Sweep = b2;
					}
				      break;
				    default:
				      PivotError (pivFile,
					          "PivotError: Path 2");
				      break;
				    }
				  /* lit l'octet qui suit (type d'element de
				     path ou tag de fin d'element) */
			          if (!TtaReadByte (pivFile, tag))
				     PivotError (pivFile,
					         "PivotError: Path 3");
				  }
				if (create)
				  /* the volume of a path is the number of
				     its elements */
				  pEl->ElVolume = n;
				}
			    }
			  else
			    /* c'est un element graphique simple */
			    {
			      if (create)
				{
				  pEl->ElGraph = c;
				  pEl->ElWideChar = 0;
				  /* remplace les anciens rectangles trame's */
				  /* par de simple rectangles */
				  if (pEl->ElGraph >= '0' &&
				      pEl->ElGraph <= '9')
				    pEl->ElGraph = 'R';
				  else if (pEl->ElGraph >= '\260' &&
					   pEl->ElGraph <= '\270')
				    pEl->ElGraph = 'R';
				  if (c == EOS)
				    pEl->ElVolume = 0;
				  else
				    pEl->ElVolume = 1;
				}
			    }
			}
		      
		    }
		  if (*tag != C_PIV_END)
		    PivotError (pivFile, "PivotError: End");
		  
		  if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile, "PivotError: End 1");
		}
	      break;
	    default:
	      /* traite le contenu s'il y en a un */
	      if (*tag == C_PIV_BEGIN)
		{
		  if (pEl != NULL)
		    if (pEl->ElTerminal)
		      PivotError (pivFile, "PivotError: Begin");
		  /* erreur: feuille avec contenu */
		  if (!error)
		    {
		      if (!TtaReadByte (pivFile, tag))
			PivotError (pivFile, "PivotError: Begin 1");
		      pPrevEl = NULL;
		      while (*tag != C_PIV_END && !error)
			/* ce n'est pas un element vide, */
			/* on lit son contenu */
			{
			  if (pPrevEl != NULL)
			    pfutParent = pPrevEl->ElParent;
			  else if (pEl != NULL)
			    pfutParent = pEl;
			  else
			    pfutParent = pParent;
			  p = ReadTreePiv (pivFile, pSSchema, pDoc, tag,
					   createAll, contentType,
					   pContSS, &rule, &pSS, createPage,
					   pfutParent, createDesc);
			  pElRead = p;
			  if (!error)
			    if (p != NULL)
			      {
				if (pPrevEl != NULL)
				  {
				    if (pEl != NULL)
				      pEl->ElParent = NULL;
				    InsertElementAfter (pPrevEl, p);
				    if (pEl != NULL)
				      pEl->ElParent = pParent;
				  }
				else if (pEl != NULL)
				  {
				    if (!createAll)
				      if (p->ElTypeNumber != PageBreak + 1)
					if (p->ElStructSchema !=
					                 pEl->ElStructSchema)
					  /* l'element a inserer dans l'arbre
					  appartient a un schema different de
					  celui de son pere */
					  if (p->ElTypeNumber !=
					         p->ElStructSchema->SsRootElem)
					    /* ce n'est pas la racine d'une
					       nature, on ajoute un element
					       intermediaire */
					    {
					      pEl2 = p;
					      /* il ne faut pas que le label */
					      /* max. du document augmente */
					      pElInt = NewSubtree (pEl2->ElStructSchema->SsRootElem,
						       pEl2->ElStructSchema,
						       pDoc, FALSE, TRUE,
						       FALSE, FALSE);
					      
					      pElInt->ElLabel[0] = EOS;
					      InsertFirstChild (pElInt, p);
					      p = pElInt;
					    }
				    pEl->ElParent = NULL;
				    InsertFirstChild (pEl, p);
				    pEl->ElParent = pParent;
				  }
				else
				  pEl = p;
				
				pPrevEl = p;
				SendEventAttrRead (pElRead, pDoc);
				/* Si l'element qu'on vient de lire n'a pas ete
				   cree' (lecture squelette) mais que certains
				   de ses descendants l'ont ete,ReadTreePiv a
				   retourne' un pointeur sur le premier
				   descendant cree'. On cherche le dernier
				   frere, qui devient l'element precedent du
				   prochain element lu. */
				while (pPrevEl->ElNext != NULL)
				  pPrevEl = pPrevEl->ElNext;
			      }
			}
		      if (!error)
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile,
				      "PivotError: End element");
		    }
		}
	      break;
	    }
    }
      
  if (!error)
    {
      if (createDesc && pEl != NULL)
	{
	  notifyEl.event = TteElemRead;
	  notifyEl.document = (Document) IdentDocument (pDoc);
	  notifyEl.element = (Element) pEl;
	  notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	  notifyEl.position = 0;
	  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	}
      if (pEl != NULL)
	pEl->ElParent = NULL;
      return pEl;
    }
  
  return NULL;
}

/*----------------------------------------------------------------------
   AbstractTreeOK verifie que l'element pEl et tous ses descendants  
   peuvent figurer a la place ou` ils sont dans leur arbre abstrait
  ----------------------------------------------------------------------*/
ThotBool AbstractTreeOK (PtrElement pEl, PtrDocument pDoc)
{
   PtrElement          pChild;
   ThotBool            ok, childOK;

   ok = TRUE;
   if (pEl != NULL)
     {
       if (pEl->ElPrevious != NULL)
	 {
	   if (!AllowedSibling (pEl->ElPrevious, pDoc, pEl->ElTypeNumber,
				pEl->ElStructSchema, FALSE, FALSE, TRUE))
	     {
	       ok = FALSE;
	       TtaDisplayMessage (INFO,
				  TtaGetMessage (LIB, TMSG_INVALID_SIBLING),
				  pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName,
				  pEl->ElPrevious->ElStructSchema->SsRule->SrElem[pEl->ElPrevious->ElTypeNumber - 1]->SrName,
				  pEl->ElLabel);
	     }
	 }
       else if (pEl->ElParent != NULL)
	 {
	   if (!AllowedFirstChild (pEl->ElParent, pDoc, pEl->ElTypeNumber,
				   pEl->ElStructSchema, FALSE, TRUE))
	     {
	       ok = FALSE;
	       TtaDisplayMessage (INFO,
				  TtaGetMessage (LIB, TMSG_INVALID_CHILD),
				  pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName,
				  pEl->ElParent->ElStructSchema->SsRule->SrElem[pEl->ElParent->ElTypeNumber - 1]->SrName,
				  pEl->ElLabel);
	     }
	 }
       
       if (!pEl->ElTerminal)
	 {
	   pChild = pEl->ElFirstChild;
	   while (pChild != NULL)
	     {
	       childOK = AbstractTreeOK (pChild, pDoc);
	       ok = ok && childOK;
	       pChild = pChild->ElNext;
	     }
	 }
     }
   return ok;
}


/*----------------------------------------------------------------------
   AssociatePairs  etablit les liens qui relient les elements de   
   paires deux a deux dans tout l'arbre de racine pRoot.		
  ----------------------------------------------------------------------*/
void                AssociatePairs (PtrElement pRoot)
{
   PtrElement          pEl1, pEl2;
   ThotBool            found;

   pEl1 = pRoot;
   /* cherche tous les elements produits par le constructeur CsPairedElement */
   while (pEl1 != NULL)
     {
       pEl1 = FwdSearchRefOrEmptyElem (pEl1, 3);
       if (pEl1 != NULL)
	 /* on a trouve' un element de paire */
	 if (pEl1->ElStructSchema->SsRule->SrElem[pEl1->ElTypeNumber-1]->SrFirstOfPair)
	   /* c'est un element de debut de paire */
	   {
	     /* on cherche l'element de fin correspondant */
	     pEl2 = pEl1;
	     found = FALSE;
	     do
	       {
		 pEl2 = FwdSearchTypedElem (pEl2, pEl1->ElTypeNumber + 1,
					    pEl1->ElStructSchema);
		 if (pEl2 != NULL)
		   /* on a trouve' un element du type cherche' */
		   /* c'est le bon s'il a le meme identificateur */
		   found = (pEl2->ElPairIdent == pEl1->ElPairIdent);
	       }
	     while ((pEl2 != NULL) && (!found));
	     if (found)
	       /* On etablit le chainage entre les 2 elements */
	       {
		 pEl1->ElOtherPairedEl = pEl2;
		 pEl2->ElOtherPairedEl = pEl1;
	       }
	   }
     }
}


/*----------------------------------------------------------------------
   SetLabel       affecte un label a tous les elements du         	
   sous-arbre de racine pEl qui n'en ont pas.                     
  ----------------------------------------------------------------------*/
static void         SetLabel (PtrElement pEl, PtrDocument pDoc)
{
   PtrElement          pChild;

   if (pEl != NULL)
     {
	if (pEl->ElLabel[0] == EOS)
	   /* l'element n'a pas de label, on lui en met un */
	   ConvertIntToLabel (NewLabel (pDoc), pEl->ElLabel);
	if (!pEl->ElTerminal)
	   /* traite tous les fils de l'element */
	  {
	     pChild = pEl->ElFirstChild;
	     while (pChild != NULL)
	       {
		  SetLabel (pChild, pDoc);
		  pChild = pChild->ElNext;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   PutNatureInTable verifie si la nature de nom SSName se trouve   
   dans la table des natures du document pDoc, au rang rank.       
   Si elle n'y est pas, on l'y met, soit en la deplacant, si elle  
   figure deja dans la table, soit en creant une nouvelle entree   
   au rang desire'.                                                
  ----------------------------------------------------------------------*/
static void PutNatureInTable (PtrDocument pDoc, Name SSName, int rank)
{
   int                 i;
   ThotBool            found;
   Name                N1, N2;
   PtrSSchema          pSS;

   /* on cherche (par son nom) si la nature existe dans la table */
   i = 1;
   found = FALSE;
   while (i <= pDoc->DocNNatures && !found)
      if (strncmp (SSName, pDoc->DocNatureName[i - 1], MAX_NAME_LENGTH) == 0)
	 found = TRUE;
      else
	 i++;
   if (found)
      /* la nature est deja dans la table */
     {
	/* si elle est au rang voulu, il n'y a rien a faire */
	if (i != rank)
	   /* elle n'est pas au rang voulu, on permute avec la nature qui */
	   /* y est */
	  {
	     pSS = pDoc->DocNatureSSchema[rank - 1];
	     strncpy (N1, pDoc->DocNatureName[rank - 1], MAX_NAME_LENGTH);
	     strncpy (N2, pDoc->DocNaturePresName[rank - 1], MAX_NAME_LENGTH);
	     pDoc->DocNatureSSchema[rank - 1] = pDoc->DocNatureSSchema[i - 1];
	     strncpy (pDoc->DocNatureName[rank - 1],
		       pDoc->DocNatureName[i - 1], MAX_NAME_LENGTH);
	     strncpy (pDoc->DocNaturePresName[rank - 1],
		       pDoc->DocNaturePresName[i - 1], MAX_NAME_LENGTH);
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
	if (rank == pDoc->DocNNatures + 1)
	   pDoc->DocNNatures++;
	else
	  {
	     pDoc->DocNatureSSchema[pDoc->DocNNatures] =
	                                    pDoc->DocNatureSSchema[rank - 1];
	     strncpy (pDoc->DocNatureName[pDoc->DocNNatures],
		       pDoc->DocNatureName[rank - 1], MAX_NAME_LENGTH);
	     strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
		       pDoc->DocNaturePresName[rank - 1], MAX_NAME_LENGTH);
	     pDoc->DocNNatures++;
	  }
	pDoc->DocNatureSSchema[rank - 1] = NULL;
	strncpy (pDoc->DocNatureName[rank - 1], SSName, MAX_NAME_LENGTH);
     }
}


/*----------------------------------------------------------------------
   	ReadSchemaNamesPiv lit les noms des schemas de structure et de	
   	presentation qui se trouvent dans le fichier file et charge ces	
   	schemas								
  ----------------------------------------------------------------------*/
void ReadSchemaNamesPiv (BinFile file, PtrDocument pDoc, char *tag,
			 PtrSSchema pLoadedSS,
			 void (*withThisPSchema) (Document document,
						  char *natSchema,
						  char *presentSchema))
{
   Name                SSName, PSchemaName;
   PtrSSchema          pSS;
   int                 i, rank;
   ThotBool            ExtensionSch;
   int                 versionSchema;

   i = 0;
   rank = 1;
   /* lit le type du document */
   do
     if (!TtaReadByte (file, &SSName[i++]))
       PivotError (file, "PivotError: Schema");
   while (!error && SSName[i - 1] != EOS && i != MAX_NAME_LENGTH) ;
   if (SSName[i - 1] != EOS)
     PivotError (file, "PivotError: Schema 1");
   else
     {
       if (pDoc->DocPivotVersion >= 4)
	 /* Lit le code du schema de structure */
	 if (!error)
	   if (!TtaReadShort (file, &versionSchema))
	     PivotError (file, "PivotError: Schema 2");
       /* Lit le nom du schema de presentation associe' */
       i = 0;
       do
	 if (!TtaReadByte (file, &PSchemaName[i++]))
	   PivotError (file, "PivotError: Schema 3");
       while (!error && PSchemaName[i - 1] != EOS && i != MAX_NAME_LENGTH);

       if (!TtaReadByte (file, tag))
	 PivotError (file, "PivotError: Schema 4");
       if (withThisPSchema != NULL)
	 (*withThisPSchema) ((Document) IdentDocument (pDoc), SSName,
			     PSchemaName);
       PutNatureInTable (pDoc, SSName, rank);
       /* charge les schemas de structure et de presentation du document */
       if (pDoc->DocSSchema == NULL) 
	 LoadSchemas (SSName, PSchemaName, &pDoc->DocSSchema, pDoc,
		      pLoadedSS, FALSE);

        if (pDoc->DocSSchema == NULL)
	  PivotError (file, "PivotError: Schema 5");
        else if (pDoc->DocPivotVersion >= 4)
	  /* on verifie que la version du schema charge' est la meme */
	  /* que celle du document */
	  if (pDoc->DocSSchema->SsCode != versionSchema)
	    pDoc->DocCheckingMode |= PIV_CHECK_MASK;
     }
   if (pDoc->DocNatureSSchema[rank - 1] == NULL)
     {
       pDoc->DocNatureSSchema[rank - 1] = pDoc->DocSSchema;
       strncpy (pDoc->DocNatureName[rank - 1], SSName, MAX_NAME_LENGTH);
       strncpy (pDoc->DocNaturePresName[rank - 1], PSchemaName,
		 MAX_NAME_LENGTH);
       if (pDoc->DocSSchema != NULL)
	 if (!PresentationSchema (pDoc->DocSSchema, pDoc))
	   /* le schema de presentation n'a pas ete charge' (librairie  */
	   /* Kernel, par exemple). On memorise dans le schema de */
	   /* structure charge' le nom du schema P associe' */
	   strncpy (pDoc->DocSSchema->SsDefaultPSchema, PSchemaName,
		    MAX_NAME_LENGTH);
     }
   /* lit les noms des fichiers contenant les schemas de nature  */
   /* dynamiques et charge ces schemas, sauf si on ne charge que */
   /* les elements exportables. */
   while ((*tag == (char) C_PIV_NATURE || *tag == (char) C_PIV_SSCHEMA_EXT)
	  && !error)
     {
       ExtensionSch = (*tag == (char) C_PIV_SSCHEMA_EXT);
       i = 0;
       rank++;
       do
	 if (!TtaReadByte (file, &SSName[i++]))
	   PivotError (file, "PivotError: Schema ext");
       while (SSName[i - 1] != EOS && !error) ;
       if (pDoc->DocPivotVersion >= 4)
	 /* Lit le code du schema de structure */
	 if (!error)
	   if (!TtaReadShort (file, &versionSchema))
	     PivotError (file, "PivotError: Schema 7");
       /* Lit le nom du schema de presentation associe' */
       if (!error)
	 if (!TtaReadByte (file, tag))
	   PivotError (file, "PivotError: Schema 8");
       if (*tag >= '!' && *tag <= '~' && !error)
	 /* il y a un nom de schema de presentation */
	 {
	   PSchemaName[0] = *tag;
	   i = 1;
	   do
	     if (!TtaReadByte (file, &PSchemaName[i++]))
	       PivotError (file, "PivotError: Schema 9");
	   while (!error && PSchemaName[i - 1] != EOS && i != MAX_NAME_LENGTH);

	   if (!TtaReadByte (file, tag))
	     PivotError (file, "PivotError: Schema 10");
	 }
       else
	 /* il n'y a pas de nom */
	 PSchemaName[0] = EOS;
       pSS = NULL;
       if (!error)
	 {
	   if (withThisPSchema != NULL)
	     (*withThisPSchema) ((Document) IdentDocument (pDoc), SSName,
				 PSchemaName);
	   PutNatureInTable (pDoc, SSName, rank);
	   if (pDoc->DocNatureSSchema[rank - 1] == NULL)
	     {
	       if (ExtensionSch)
		 /* charge l'extension de schema */
		 pSS = LoadExtension (SSName, PSchemaName, pDoc);
	       else 
		 {
		   i = CreateNature (SSName, PSchemaName, pDoc->DocSSchema,
				     pDoc);
		   if (i == 0)
		     /* echec creation nature */
		     PivotError (file, "PivotError: Schema 11");
		   else
		     pSS = pDoc->DocSSchema->SsRule->SrElem[i - 1]->SrSSchemaNat;
		 }
	     }
	 }
       if (!error && pSS != NULL)
	 {
	   if (pDoc->DocPivotVersion >= 4)
	     /* on verifie que la version du schema charge' est la meme */
	     /* que celle du document */
	     if (pSS->SsCode != versionSchema)
	       pDoc->DocCheckingMode |= PIV_CHECK_MASK;
	   pDoc->DocNatureSSchema[rank - 1] = pSS;
	   strncpy (pDoc->DocNaturePresName[rank - 1], PSchemaName,
		     MAX_NAME_LENGTH);
	   if (!PresentationSchema (pSS, pDoc))
	     /* le schema de presentation n'a pas ete charge' (librairie
		Kernel, par exemple). On memorise dans le schema de structure
		charge' le nom du schema P associe' */
	     strncpy (pSS->SsDefaultPSchema, PSchemaName, MAX_NAME_LENGTH);
	 }
     }
}


/*----------------------------------------------------------------------
   	ReadLanguageTablePiv
	lit la table des langues qui se trouve en tete du fichier pivot.
  ----------------------------------------------------------------------*/
void ReadLanguageTablePiv (BinFile file, PtrDocument pDoc, char *tag)
{
   Name             languageName;
   int              i;

   /* lit la table des langues utilisees par le document */
   pDoc->DocNLanguages = 0;
   if (pDoc->DocPivotVersion >= 4)
     while (*tag == (char) C_PIV_LANG && !error)
       {
	 i = 0;
	 do
	   if (!TtaReadByte (file, &languageName[i++]))
	     PivotError (file, "PivotError: Language 2");
	 while (!error && languageName[i - 1] != EOS &&
		i != MAX_NAME_LENGTH) ;
	 if (languageName[i - 1] != EOS)
	   PivotError (file, "PivotError: Language 3");
	 else
	   {
	     if (languageName[0] != EOS)
	       if (pDoc->DocNLanguages < MAX_LANGUAGES_DOC)
		 pDoc->DocLanguages[pDoc->DocNLanguages++] =
		                       TtaGetLanguageIdFromName (languageName);
	     /* lit l'octet suivant le nom de langue */
	     if (!TtaReadByte (file, tag))
	       PivotError (file, "PivotError: Laguage 4");
	   }
       }
}


/*----------------------------------------------------------------------
   ReadVersionNumberPiv lit la version dans le fichier pivot et         
   met sa valeur dans le contexte pDoc. Retourne 0 si OK.          
  ----------------------------------------------------------------------*/
int ReadVersionNumberPiv (BinFile file, PtrDocument pDoc)
{
   char                c;
   int                 ret;

   ret = 0;
   pDoc->DocPivotVersion = 1;
   if (!TtaReadByte (file, &c))
      ret = 10;
   else if (c != C_PIV_VERSION)
      ret = 10;
   else if (!TtaReadByte (file, &c))
      ret = 10;
   else if (c != C_PIV_VERSION)
      ret = 10;
   else if (!TtaReadByte (file, &c))
      ret = 10;
   else
      pDoc->DocPivotVersion = (int) c;
   return ret;
}

/*----------------------------------------------------------------------
   ReadPivotHeader   lit l'entete d'un fichier pivot.                
  ----------------------------------------------------------------------*/
void ReadPivotHeader (BinFile file, PtrDocument pDoc, char *tag)
{
   LabelString         label;
   int                 i;
   char                c;

   /* lit le numero de version s'il est present */
   if (!TtaReadByte (file, tag))
      PivotError (file, "PivotError: Header");
   if (*tag == (char) C_PIV_VERSION)
     {
	if (!TtaReadByte (file, tag))
	   PivotError (file, "PivotError: Header 1");
	else if (!TtaReadByte (file, &c))
	   PivotError (file, "PivotError: Header 2");
	else
	  {
	     pDoc->DocPivotVersion = (int) c;
	     if (!TtaReadByte (file, tag))
		PivotError (file, "PivotError: Header 3");
	  }
     }
   else				/* Pivot sans tag de version */
      pDoc->DocPivotVersion = 1;

   /* lit le label max. du document s'il est present */
   if (!error && (*tag == (char) C_PIV_SHORT_LABEL ||
		  *tag == (char) C_PIV_LONG_LABEL ||
		  *tag == (char) C_PIV_LABEL))
     {
	ReadLabel (*tag, label, file);
	LabelStringToInt (label, &i);
	SetCurrentLabel (pDoc, i);
	if (!TtaReadByte (file, tag))
	   PivotError (file, "PivotError: Header 4");
     }
   /* lit la table des langues utilisees par le document */
   ReadLanguageTablePiv (file, pDoc, tag);
}


/*----------------------------------------------------------------------
   LoadDocumentPiv charge en memoire (representation interne) un document
   qui se trouve sous la forme pivot dans le fichier file. 
   Le fichier doit etre ouvert et positionne' au debut.    
   Il n'est pas ferme' par LoadDocumentPiv.                
   pDoc est le pointeur sur le descripteur de document du  
   document a` charger.                                    
   Si loadExternalDoc est vrai, on charge temporairement les
   documents externes reference's pour pouvoir copier les  
   elements inclus. Les documents externes ainsi charge's  
   sont decharge's au retour. Si skeleton est vrai, le     
   document est charge sous forme squelette.               
  ----------------------------------------------------------------------*/
void LoadDocumentPiv (BinFile file, PtrDocument pDoc, ThotBool loadExternalDoc,
		      ThotBool skeleton, PtrSSchema pLoadedSS,
		      ThotBool withEvent, ThotBool removeExclusions)
{
   PtrElement          s, p, d;
   PtrSSchema          pSS, pNat;
   PtrPSchema          pPSchema;
   PtrReferredDescr    pRefD, pNextRefD;
   NotifyDialog        notifyDoc;
   int                 i, rule, typeRead;
   char                tag;
   ThotBool            structureOK, createPages, ok;

/*    pDoc->DocToBeChecked = FALSE; */
   pDoc->DocCheckingMode &= ~PIV_CHECK_MASK;
   structureOK = TRUE;
   ok = FALSE;
   error = FALSE;
   msgOldFormat = TRUE;
   pDoc->DocDocElement = NULL;
   createPages = FALSE;
   pDoc->DocNNatures = 0;
   /* lit l'entete du fichier pivot */
   ReadPivotHeader (file, pDoc, &tag);
   /* Lit le nom du schema de structure qui est en tete du fichier pivot */
   if (!error && tag != (char) C_PIV_NATURE)
     /* tag classe absent */
     PivotError (file, "PivotError: Class");
   if (!error)
     /* lit les noms des schemas de structure et de presentation */
     ReadSchemaNamesPiv (file, pDoc, &tag, pLoadedSS, NULL);
   if (withEvent && pDoc->DocSSchema != NULL && !error)
     {
       notifyDoc.event = TteDocOpen;
       notifyDoc.document = (Document) IdentDocument (pDoc);
       notifyDoc.view = 0;
       if (CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	 error = TRUE;
     }
   if (pDoc->DocSSchema != NULL && !error)
     {
       if (pDoc->DocSSchema->SsExport)
	 /* le schema de structure du document prevoit l'exportation     */
	 /* d'elements. On demande a l'utilisateur s'il veut charger la  */
	 /* forme complete ou les seuls elements exportables, mais       */
	 /* seulement si le schema de presentation a une vue THOT_EXPORT */
	 {
	   createPages = FALSE;
	   ok = FALSE;
	   pPSchema = PresentationSchema (pDoc->DocSSchema, pDoc);
	   if (pPSchema != NULL)
	     for (i = 0; i < pPSchema->PsNViews; i++)
	       ok = ok || pPSchema->PsExportView[i];
	   if (ok)
	     {
	       /* une vue THOT_EXPORT est prevue */
	       if (skeleton)
		 {
		   pDoc->DocExportStructure = TRUE;
		   /* Un document charge' sous sa forme export n'est pas */
		   /* modifiable */
		   pDoc->DocReadOnly = TRUE;
		   createPages = TRUE;
		 }
	     }
	 }
       pDoc->DocLabels = NULL;
       /* lit le fichier .PIV */
       
       /* lit le corps du document */
       if (!error &&
	   tag != (char) C_PIV_DOC_END)
	 {
	   if (tag != (char) C_PIV_STRUCTURE)
	     PivotError (file, "Open");
	   else
	     {
	       if (!TtaReadByte (file, &tag))
		 PivotError (file, "Open 1");
	       if (tag != (char) C_PIV_TYPE && tag != (char) C_PIV_NATURE)
		 PivotError (file, "Open 2");
	       else
		 {
		   rule = 0;
		   pNat = NULL;
		   p = ReadTreePiv (file, pDoc->DocSSchema, pDoc, &tag,
				  (ThotBool)(!pDoc->DocExportStructure),
                                  &rule, &pNat, &typeRead, &pSS, createPages,
                                  NULL, TRUE);
		   if (withEvent && pDoc->DocSSchema != NULL && !error)
		     SendEventAttrRead (p, pDoc);
		   /* force la creation d'un element racine */
		   if (p == NULL)
		     /* rien n'a ete cree */
		     {
		       d = NewSubtree (pDoc->DocSSchema->SsDocument,
				       pDoc->DocSSchema, pDoc,
				       FALSE, TRUE, TRUE, TRUE);
		       p = NewSubtree (pDoc->DocSSchema->SsRootElem,
				       pDoc->DocSSchema, pDoc,
				       FALSE, TRUE, TRUE, TRUE);
		       InsertFirstChild (d, p);
		     }
		   else if (p->ElStructSchema == pDoc->DocSSchema &&
			    p->ElTypeNumber == pDoc->DocSSchema->SsDocument)
		     d = p;
		   else
		     /* ce n'est pas la racine attendue */
		     {
		       d = NewSubtree (pDoc->DocSSchema->SsDocument,
				       pDoc->DocSSchema, pDoc,
				       FALSE, TRUE, TRUE, TRUE);
		       if (p->ElTypeNumber == pDoc->DocSSchema->SsRootElem &&
			   p->ElStructSchema == pDoc->DocSSchema)
			 InsertFirstChild (d, p);
		       else
			 {
			  s = p;
		          p = NewSubtree (pDoc->DocSSchema->SsRootElem,
				          pDoc->DocSSchema, pDoc,
				          FALSE, TRUE, TRUE, TRUE);
		          InsertFirstChild (d, p);
			  InsertFirstChild (p, s);
			 }
		     }
		   /* traite les elements exclus */
		   if (removeExclusions)
		     RemoveExcludedElem (&p, pDoc);
		   /* accouple les paires */
		   AssociatePairs (p);
		   pDoc->DocDocElement = d;
		   if (pDoc->DocCheckingMode & PIV_CHECK_MASK)
		     /* verifie que cet arbre est correct */
		     {
		       ok = AbstractTreeOK (p, pDoc);
		       structureOK = structureOK && ok;
		     }
		 }
	     }
	 }
       /* libere les labels des elements reference's par d'autres */
       /* documents */
       pRefD = pDoc->DocLabels;
       while (pRefD != NULL)
	 {
	   /* inutile de liberer les descripteurs de documents */
	   /* externes, on ne les a pas charge's */
	   pNextRefD = pRefD->ReNext;
	   FreeReferredDescr (pRefD);
	   pRefD = pNextRefD;
	 }
       
       if (!error)
	 /* affecte un label a tous les elements qui n'en ont pas et */
	 /* recherche toutes les references d'inclusion du document et */
	 /* copie les elements inclus */
	 {
	   /* on affecte des labels aux elements */
	   if (pDoc->DocDocElement != NULL)
	     SetLabel (pDoc->DocDocElement, pDoc);
	   
	   /* Update the inclusions values */
	   UpdateInclusionElements (pDoc, loadExternalDoc, removeExclusions);
	   
	   pDoc->DocDocElement->ElAccess = AccessReadWrite;
	   if (withEvent && pDoc->DocSSchema != NULL && !error)
	     {
	       notifyDoc.event = TteDocOpen;
	       notifyDoc.document = (Document) IdentDocument (pDoc);
	       notifyDoc.view = 0;
	       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	     }
	 }
     }
}
