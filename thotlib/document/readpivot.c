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

extern CHARSET CharEncoding;

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
#include "viewcommands_f.h"

/*----------------------------------------------------------------------
  SetImageRule updates or creates the picture descriptor of an element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetImageRule (PtrElement pEl, int x, int y, int w, int h, int typeimage, PictureScaling presimage)
#else  /* __STDC__ */
static void         SetImageRule (pEl, x, y, w, h, typeimage, presimage)
PtrElement          pEl;
int                 x;
int                 y;
int                 w;
int                 h;
PictureScaling      presimage;
int                 typeimage;
#endif /* __STDC__ */

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
      image->PicPixmap = 0;
#     ifndef _WINDOWS
      image->PicMask = 0;
#     endif /* _WINDOWS */
      image->PicType = typeimage;
      image->PicPresent = presimage;
      image->PicXArea = x;
      image->PicYArea = y;
      image->PicWArea = w;
      image->PicHArea = h;
      image->PicWidth = 0;
      image->PicHeight = 0;
      image->PicColors = NULL;
      image->mapped = FALSE;
      image->created = FALSE;
    }
}

/*----------------------------------------------------------------------
   	PivotError							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PivotError (BinFile file)
#else  /* __STDC__ */
static void         PivotError (file)
BinFile             file;

#endif /* __STDC__ */
{
   int                 i;
   char                c;
   ThotBool            stop;

   stop = FALSE;
   i = 1;
   while (!stop && i < 200)
     {
	if (!TtaReadByte (file, &c))
	   stop = TRUE;
	else
	  {
	     if (c < SPACE)
	       {
		  printf ("^");
		  c = ((int) c + (int) '@');
		  i++;
	       }
	     printf ("%c", c);
	     i++;
	  }
     }
   printf ("\n");
   error = TRUE;
}

/*----------------------------------------------------------------------
   GetPtrDocument cherche si le document d'identificateur docIdent est	
   	charge'.							
   Retourne un pointeur sur son contexte, ou NULL s'il	
   n'est pas charge'.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrDocument         GetPtrDocument (DocumentIdentifier docIdent)
#else  /* __STDC__ */
PtrDocument         GetPtrDocument (docIdent)
DocumentIdentifier  docIdent;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void         FreeUnusedReferredElemDesc (PtrDocument pDoc)
#else  /* __STDC__ */
static void         FreeUnusedReferredElemDesc (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
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
#ifdef __STDC__
ThotBool            OpenDocument (CHAR_T* docName, PtrDocument pDoc, ThotBool loadIncludedDoc, ThotBool skeleton, PtrSSchema pSS, ThotBool withAppEvent, ThotBool removeExclusions)
#else  /* __STDC__ */
ThotBool            OpenDocument (docName, pDoc, loadIncludedDoc, skeleton, pSS, withAppEvent, removeExclusions)
CHAR_T*             docName;
PtrDocument         pDoc;
ThotBool            loadIncludedDoc;
ThotBool            skeleton;
PtrSSchema          pSS;
ThotBool            withAppEvent;
ThotBool		    removeExclusions
#endif /* __STDC__ */
{  
   ThotBool            ret;
   int                 i;
   PathBuffer          directoryName;
   PtrChangedReferredEl pChngRef;
   FILE               *pivotFile;
   CHAR_T                text[MAX_TXT_LEN];

   ret = FALSE;
   if (pDoc != NULL)
     {
	if (!DocIdentIsNull (pDoc->DocIdent))
	   /* l'identificateur du document est connu, on accede au document par */
	   /* cet identificateur */
	  {
	     /* on n'a pas d'autre outil de stockage des documents que le SGF UNIX */
	     /* On confond identificateur et nom de document */
	     ustrncpy (pDoc->DocDName, pDoc->DocIdent, MAX_NAME_LENGTH);
	     pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
	  }
	else
	   /* le document n'a pas d'identificateur, on l'accede par son nom */
	  {
	     ustrncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
	     pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
	     /* on n'a pas d'autre outil de stockage des documents que le SGF UNIX */
	     /* On confond identificateur et nom de document */
	     ustrncpy (pDoc->DocIdent, docName, MAX_DOC_IDENT_LEN);
	     pDoc->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
	  }
	if (pDoc->DocDName[0] > SPACE)
	   /* nom de document non vide */
	  {
	     /* compose le nom du fichier a ouvrir avec le nom du directory */
	     /* des documents... */
	     if (pDoc->DocDirectory[0] == EOS)
		ustrncpy (directoryName, DocumentPath, MAX_PATH);
	     else
		ustrncpy (directoryName, pDoc->DocDirectory, MAX_PATH);
	     MakeCompleteName (pDoc->DocDName, TEXT("PIV"), directoryName, text, &i);
	     /* ouvre le fichier 'PIV' */
	     pivotFile = TtaReadOpen (text);
	     if (pivotFile != 0)
		/* le fichier existe */
		/* internalise le fichier pivot sans charger les documents */
		/* externes qui contiennent des elements inclus. */
	       {
		  /* le document appartient au directory courant */
		  ustrncpy (pDoc->DocDirectory, directoryName, MAX_PATH);
		  LoadDocumentPiv (pivotFile, pDoc, loadIncludedDoc, skeleton, pSS, withAppEvent, removeExclusions);
		  TtaReadClose (pivotFile);
		  if (pDoc->DocRootElement != NULL)
		     /* le document lu n'est pas vide */
		    {
		       /* le nom de fichier devient le nom du document */
		       ret = TRUE;
		       /* lit le fichier des references externes s'il existe */
		       /* dans le meme directory que le fichier .PIV */
		       FindCompleteName (pDoc->DocDName, TEXT("EXT"), directoryName, text, &i);
		       pivotFile = TtaReadOpen (text);
		       if (pivotFile != 0)
			 {
			    LoadEXTfile (pivotFile, pDoc, NULL, FALSE);
			    TtaReadClose (pivotFile);
			 }
		       /* lit le fichier de mise a jour des references sortantes */
		       /* s'il existe dans le meme directory que le fichier .PIV */
		       FindCompleteName (pDoc->DocDName, TEXT("REF"), directoryName, text, &i);
		       pivotFile = TtaReadOpen (text);
		       if (pivotFile != 0)
			 {
			    LoadREFfile (pivotFile, &pChngRef);
			    TtaReadClose (pivotFile);
			    /* traite les mises a jour de */
			    /* references sortantes */
			    if (pChngRef != NULL)
			       UpdateREFdescriptors (pChngRef, pDoc);
			 }
		       /* libere les descripteurs d'element reference' inutilise's */
		       FreeUnusedReferredElemDesc (pDoc);
		    }
	       }
	  }
     }
   return ret;
}


/*----------------------------------------------------------------------
   DeleteAllTrees supprime les arbres abstraits d'un document et de tous 
   ses elements associes et parametres. Les schemas de     
   structure et de presentation utilises par le document   
   ne sont pas liberes...                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeleteAllTrees (PtrDocument pDoc)
#else  /* __STDC__ */
void                DeleteAllTrees (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   int                 i;
   PtrOutReference     pOutRef, pNextOutRef;
   PtrChangedReferredEl pChnRef, pNextChnRef;
   PtrTextBuffer       pBuf, pNextBuf;
   PtrExternalDoc      pExtDoc, pNextExtDoc;

   if (pDoc != NULL)
     {
	/* libere les buffers de commentaire */
	pBuf = pDoc->DocComment;
	while (pBuf != NULL)
	  {
	     pNextBuf = pBuf->BuNext;
	     FreeTextBuffer (pBuf);
	     pBuf = pNextBuf;
	  }
	pDoc->DocComment = NULL;
	/* libere tout l'arbre du document et ses descripteurs de reference */
	DeleteElement (&pDoc->DocRootElement, pDoc);
	/* libere les elements associes */
	for (i = 1; i <= MAX_ASSOC_DOC; i++)
	   if (pDoc->DocAssocRoot[i - 1] != NULL)
	      DeleteElement (&pDoc->DocAssocRoot[i - 1], pDoc);
	/* libere les parametres */
	for (i = 1; i <= MAX_PARAM_DOC; i++)
	   if (pDoc->DocParameters[i - 1] != NULL)
	      DeleteElement (&pDoc->DocParameters[i - 1], pDoc);
	/* libere le 1er descripteur de reference (bidon) */
	FreeReferredDescr (pDoc->DocReferredEl);
	pDoc->DocReferredEl = NULL;
	/* libere les descripteurs de references sortantes creees */
	pOutRef = pDoc->DocNewOutRef;
	while (pOutRef != NULL)
	  {
	     pNextOutRef = pOutRef->OrNext;
	     FreeOutputRef (pOutRef);
	     pOutRef = pNextOutRef;
	  }
	/* libere les descripteurs de references sortantes detruites */
	pOutRef = pDoc->DocDeadOutRef;
	while (pOutRef != NULL)
	  {
	     pNextOutRef = pOutRef->OrNext;
	     FreeOutputRef (pOutRef);
	     pOutRef = pNextOutRef;
	  }
	/* libere les decripteurs d'element reference's change's */
	pChnRef = pDoc->DocChangedReferredEl;

	while (pChnRef != NULL)
	  {
	     pNextChnRef = pChnRef->CrNext;
	     pExtDoc = pChnRef->CrReferringDoc;
	     pChnRef->CrReferringDoc = NULL;
	     while (pExtDoc != NULL)
	       {
		  pNextExtDoc = pExtDoc->EdNext;
		  FreeExternalDoc (pExtDoc);
		  pExtDoc = pNextExtDoc;
	       }
	     FreeChangedReferredEl (pChnRef);
	     pChnRef = pNextChnRef;
	  }

     }
}


/*----------------------------------------------------------------------
   NormalizeFileName recupere les informations contenues dans le      
   nom de fichier (cas des anciens fichiers pivots).       
   La presentation et le type de l'image sont code's sur   
   1 octet au debut du nom. On retourne dans name, le nom  
   correct et dans oldTypeImage et oldPres le type et la   
   presentation trouve ou 0.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       NormalizeFileName (STRING fileName, int *oldTypeImage, PictureScaling *oldPres, ThotBool *found)
#else  /* __STDC__ */
static STRING       NormalizeFileName (fileName, oldTypeImage, oldPres, found)
STRING              fileName;
int                *oldTypeImage;
PictureScaling     *oldPres;
ThotBool           *found;
#endif /* __STDC__ */
{
   STRING              name;

   name = fileName;
   if (*name < SPACE)
     {
	*oldTypeImage = *name++ - 1;
	*oldPres = (PictureScaling) (*oldTypeImage / 4);
	*oldTypeImage = *oldTypeImage % 4;
	*found = TRUE;
     }
   else
     {
	*found = FALSE;
	*oldPres = RealSize;
     }
   *oldTypeImage = UNKNOWN_FORMAT;
   return name;
}


/*----------------------------------------------------------------------
   ReadDimensionType lit un type de dimension dans le fichier et	
   	retourne sa valeur.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ReadDimensionType (BinFile file)
#else  /* __STDC__ */
static ThotBool     ReadDimensionType (file)
BinFile             file;

#endif /* __STDC__ */
{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file);
     }
   if (c == C_PIV_ABSOLUTE)
      return TRUE;
   else
      return FALSE;
}


/*----------------------------------------------------------------------
   ReadUnit lit une unite dans le fichier et retourne sa valeur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TypeUnit     ReadUnit (BinFile file)
#else  /* __STDC__ */
static TypeUnit     ReadUnit (file)
BinFile             file;

#endif /* __STDC__ */
{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
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


/*----------------------------------------------------------------------
   ReadSign lit un signe dans le fichier et retourne sa valeur.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ReadSign (BinFile file)
#else  /* __STDC__ */
static ThotBool     ReadSign (file)
BinFile             file;

#endif /* __STDC__ */
{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file);
     }
   if (c == C_PIV_PLUS)
      return TRUE;
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   ReadBoolean lit un booleen dans le fichier et retourne sa valeur
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     ReadBoolean (BinFile file)

#else  /* __STDC__ */
static ThotBool     ReadBoolean (file)
BinFile             file;

#endif /* __STDC__ */

{
   char c;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file);
     }
   if (c == C_PIV_TRUE)
      return TRUE;
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   ReadAlign lit un BAlignment dans le fichier et retourne sa valeur. 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static BAlignment   ReadAlign (BinFile file)

#else  /* __STDC__ */
static BAlignment   ReadAlign (file)
BinFile             file;

#endif /* __STDC__ */

{
   char       c;
   BAlignment align;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file);
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
	    default:
	       PivotError (file);
	       DisplayPivotMessage (TEXT("l"));
	       align = AlignLeft;
	       break;
	 }
   return align;
}

/*----------------------------------------------------------------------
   ReadPageType	lit un Type de page dans le fichier et retourne	
   sa valeur.                                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PageType     ReadPageType (BinFile file)

#else  /* __STDC__ */
static PageType     ReadPageType (file)
BinFile             file;

#endif /* __STDC__ */

{
   char     c;
   PageType typ;

   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file);
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
	       TtaDisplaySimpleMessage (INFO, LIB, TMSG_COLUMNS_PAGINATE_AGAIN);
	       break;
	    case C_PIV_START_COL:
	       typ = PgComputed;
	       TtaDisplaySimpleMessage (INFO, LIB, TMSG_COLUMNS_PAGINATE_AGAIN);
	       break;
	    case C_PIV_USER_COL:
	       typ = PgComputed;
	       TtaDisplaySimpleMessage (INFO, LIB, TMSG_COLUMNS_PAGINATE_AGAIN);
	       break;
	    case C_PIV_COL_GROUP:
	       typ = PgComputed;
	       TtaDisplaySimpleMessage (INFO, LIB, TMSG_COLUMNS_PAGINATE_AGAIN);
	       break;
	    case C_PIV_REPEAT_PAGE:
	       /* les tags page rappel sont transformees */
	       /* en tags page calculees, car le nouveau */
	       /* code ne traite plus ces types de tags */
	       typ = PgComputed;
	       break;
	    default:
	       PivotError (file);
	       DisplayPivotMessage (TEXT("C"));
	       typ = PgComputed;
	       break;
	 }
   return typ;
}

/*----------------------------------------------------------------------
   ReadComment lit un commentaire dans le fichier file et retourne 
   un pointeur sur le premier buffer du texte du           
   commentaire lu, si store est vrai. Si store est		
   faux, le commentaire est simplement saute' et la        
   fonction retourne NULL.                                 
   Si oldformat est vrai, le commentaire est lu selon      
   l'ancien format.                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrTextBuffer       ReadComment (BinFile file, ThotBool store, ThotBool oldformat)

#else  /* __STDC__ */
PtrTextBuffer       ReadComment (file, store, oldformat)
BinFile             file;
ThotBool            store;
ThotBool            oldformat;

#endif /* __STDC__ */

{
   PtrTextBuffer firstBuf, pBuf;
   int           n, len;
   char          c;

   len = 0;
   /* lit l'octet qui suit le tag commentaire */
   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file);
     }
   if (oldformat && c != C_PIV_BEGIN)
     {
	PivotError (file);
	/* tag debut de commentaire ancien format absente */
	DisplayPivotMessage (TEXT("y"));
	firstBuf = NULL;
     }
   else
     {
	if (!oldformat)
	   /* l'octet lu est l'octet de poids fort de la longueur */
	  {
	     len = 256 * ((int) c);
	     /* lit le 2eme octet de la longueur */
	     if (!TtaReadByte (file, &c))
	       {
		  c = EOS;
		  PivotError (file);
	       }
	     len += (int) c;
	  }
	if (!store)
	  {
	     /* on saute le texte du commentaire */
	     firstBuf = NULL;
	     if (oldformat)
		/* lit le fichier jusqu'au 1er zero */
		do
		   if (!TtaReadByte (file, &c))
		     {
			c = EOS;
			PivotError (file);
		     }
		while (c != EOS) ;
	     else
		while (len > 0)
		  {
		     /* lit le nombre d'octets indique' */
		     if (!TtaReadByte (file, &c))
			PivotError (file);
		     len--;
		  }
	  }
	else
	  {
	     /* on lit effectivement le texte du commentaire */
	     GetTextBuffer (&pBuf);
	     firstBuf = pBuf;
	     n = 0;
	     if (oldformat)
		/* il faut lire au moins le zero final */
		len = 1;
	     if (len > 0)
		do
		  {
		     /* lit le texte du commentaire */
		     if (n == THOT_MAX_CHAR - 1)
			/* le buffer courant est plein, on change de buffer */
		       {
			  pBuf->BuLength = n;
			  pBuf->BuContent[n] = EOS;
			  pBuf = NewTextBuffer (pBuf);
			  n = 0;

		       }
		     if (!TtaReadByte (file, &c))
		       {
			  c = EOS;
			  PivotError (file);
		       }
		     n++;
		     pBuf->BuContent[n - 1] = c;
		     if (!oldformat)
			/* decremente le nombre d'octets  restant a lire */
			len--;
		     else if (c == EOS)
		       {
			  n--;
			  len = 0;
		       }
		  }
		while (len != 0);
	     /* termine le buffer en cours */
	     pBuf->BuLength = n;
	     pBuf->BuContent[n] = EOS;
	  }
	if (oldformat)
	  {
	     /* lit le tag de fin */
	     if (!TtaReadByte (file, &c))
		c = EOS;
	     if (c != C_PIV_END)
	       {
		  PivotError (file);
		  DisplayPivotMessage (TEXT("c"));
	       }
	  }
     }
   return firstBuf;
}

/*----------------------------------------------------------------------
   ReadPicturePresentation lit la presentation d'un PictInfo	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PictureScaling ReadPicturePresentation (BinFile pivFile)
#else  /* __STDC__ */
static PictureScaling ReadPicturePresentation (pivFile)
BinFile             pivFile;
#endif /* __STDC__ */
{
   char           c;
   PictureScaling scaling;

   if (!TtaReadByte (pivFile, &c))
     {
	c = EOS;
	PivotError (pivFile);
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
       PivotError (pivFile);
       DisplayPivotMessage (TEXT("PICT"));
       scaling = RealSize;
       break;
     }

   return scaling;
}


/*----------------------------------------------------------------------
   LabelStringToInt convertit le label strn en un entier           
   retourne 0 si label mal construit.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LabelStringToInt (LabelString string, int *number)
#else  /* __STDC__ */
void                LabelStringToInt (string, number)
LabelString         string;
int                *number;
#endif /* __STDC__ */
{
   int                 i;
   int                 val;
   ThotBool            ok;
   CHAR_T                c;

   ok = FALSE;
   *number = 0;
   if (string[0] == TEXT('L'))
     {
	val = 0;
	i = 1;
	do
	  {
	     c = string[i];
	     i++;
	     if (c >= TEXT('0') && c <= TEXT('9'))
	       {
		  val = (val * 10) + (c - TEXT('0'));
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
#ifdef __STDC__
static void         ReadReference (ReferenceType * refType, LabelString label, ThotBool * refExt, DocumentIdentifier * docIdent, BinFile file)
#else  /* __STDC__ */
static void         ReadReference (refType, label, refExt, docIdent, file)
ReferenceType      *refType;
LabelString         label;
ThotBool           *refExt;
DocumentIdentifier *docIdent;
BinFile             file;
#endif /* __STDC__ */
{
   int  j;
   char c;

   ClearDocIdent (docIdent);
   /* lit un octet */
   if (!TtaReadByte (file, &c))
     {
	c = EOS;
	PivotError (file);
     }
   /* cet octet represente-t-il un type de reference correct ? */
   if (c != C_PIV_REF_FOLLOW && c != C_PIV_REF_INCLUSION &&
       c != C_PIV_REF_INCLUS_EXP)
      /* non, ancien format pivot */
      /* on envoie un message a l'utilisateur, si ce n'est deja fait */
     {
	if (msgOldFormat)
	  {
	     PivotError (file);
	     TtaDisplaySimpleMessage (INFO, LIB, TMSG_OLD_PIV_FORMAT_SAVE_DOC_WITH_THOT);
	     msgOldFormat = FALSE;	/* c'est fait */
	  }
	if (c == MOldRefInterne || c == MOldRefExterne || c == MOldRefInclusion)
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
		  PivotError (file);
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
		  PivotError (file);
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
	     PivotError (file);
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
#ifdef __STDC__
static PtrReferredDescr GetElRefer (LabelString label, DocumentIdentifier docIdent, PtrDocument pDoc)
#else  /* __STDC__ */
static PtrReferredDescr GetElRefer (label, docIdent, pDoc)
LabelString         label;
DocumentIdentifier  docIdent;
PtrDocument         pDoc;
#endif /* __STDC__ */
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
	   if (DocIdentIsNull (docIdent) && !pRefD->ReExternalRef)
	      /* on cherche une reference interne et c'en est une */
	      stop = TRUE;	/* trouve' */
	   else if (!DocIdentIsNull (docIdent) && pRefD->ReExternalRef)
	      /* on cherche une reference externe et c'en est une */
	      if (SameDocIdent (docIdent, pRefD->ReExtDocument))
		 stop = TRUE;
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
#ifdef __STDC__
static void         CreateReference (PtrReference RefPtr, ReferenceType TRef, LabelString lab, ThotBool RExt, DocumentIdentifier I, PtrDocument pDoc)
#else  /* __STDC__ */
static void         CreateReference (RefPtr, TRef, lab, RExt, I, pDoc)
PtrReference        RefPtr;
ReferenceType       TRef;
LabelString         lab;
ThotBool            RExt;
DocumentIdentifier  I;
PtrDocument         pDoc;
#endif /* __STDC__ */
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
#ifdef __STDC__
static int          ReadType (PtrDocument pDoc, PtrSSchema * pSS, BinFile pivFile, char* tag)
#else  /* __STDC__ */
static int          ReadType (pDoc, pSS, pivFile, tag)
PtrDocument         pDoc;
PtrSSchema         *pSS;
BinFile             pivFile;
char*               tag;
#endif /* __STDC__ */
{
   int                 nat, rule;
   ThotBool            Extension;

   rule = 0;
   if (*tag == C_PIV_NATURE)
     {
	/* lit le numero de nature */
	TtaReadShort (pivFile, &nat);
	if (nat < 0 || nat >= pDoc->DocNNatures)
	  {
	     PivotError (pivFile);
	     DisplayPivotMessage (TEXT("Nature Num"));
	  }
	/* lit le tag de type qui suit */
	if (!error)
	   if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile);
	/* teste si le numero lu est celui de la structure generique du document */
	   else if (nat == 0)
	      *pSS = pDoc->DocSSchema;
	   else
	     {
		/* teste s'il s'agit d'une extension de la structure generique du */
		/* document */
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
		     rule = CreateNature (pDoc->DocNatureName[nat], pDoc->DocNaturePresName[nat], *pSS);
		     /* recupere le numero de la regle de nature */
		     if (rule == 0)
		       {
			  PivotError (pivFile);
			  DisplayPivotMessage (TEXT("n"));
		       }
		     else
			*pSS = (*pSS)->SsRule[rule - 1].SrSSchemaNat;
		  }
	     }
     }
   if (!error)
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
	   PivotError (pivFile);
	   DisplayPivotMessage (TEXT("t"));
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
#ifdef __STDC__
static void         ExportedContent (ThotBool * createAll, int *elType, PtrSSchema * pSS, PtrSSchema * pContSS, int *contentType)
#else  /* __STDC__ */
static void         ExportedContent (createAll, elType, pSS, pContSS, contentType)
ThotBool           *createAll;
int                *elType;
PtrSSchema         *pSS;
PtrSSchema         *pContSS;
int                *contentType;
#endif /* __STDC__ */
{
   int                 i;
   ThotBool            ok;
   SRule              *pSRule;

   if (*contentType != 0 && *pContSS != NULL)
     {
	ok = FALSE;
	if ((*pContSS)->SsCode == (*pSS)->SsCode)
	   if (*elType == *contentType)
	      /* meme numero de type */
	      ok = TRUE;
	   else
	     {
		pSRule = &(*pContSS)->SsRule[*contentType - 1];
		if (pSRule->SrConstruct == CsChoice)
		   /* le contenu a creer est un choix */
		   if (pSRule->SrNChoices > 0)
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
	if (!ok)
	   if (*elType == (*pSS)->SsRootElem)
	     {
		pSRule = &(*pContSS)->SsRule[*contentType - 1];
		if (pSRule->SrConstruct == CsNatureSchema)
		  {
		     /* le contenu cherche' est justement une racine de nature */
		     if (pSRule->SrSSchemaNat != NULL)
			ok = pSRule->SrSSchemaNat->SsCode == (*pSS)->SsCode;
		  }
		else if (pSRule->SrConstruct == CsChoice)
		   /* le contenu cherche' est un choix. Y a-t-il, parmi les */
		   /* options de ce choix, la nature dont l'element courant est */
		   /* racine? */
		   if (pSRule->SrNChoices > 0)
		      /* choix explicite */
		     {
			i = 0;
			do
			  {
			     i++;
			     if ((*pContSS)->SsRule[pSRule->SrChoice[i - 1] - 1].SrConstruct == CsNatureSchema)
				/* l'option i est un changement de nature */
				if ((*pContSS)->SsRule[pSRule->SrChoice[i - 1] - 1].SrSSchemaNat != NULL)
				   ok = ((*pContSS)->SsRule[pSRule->SrChoice[i - 1] - 1].SrSSchemaNat->SsCode == (*pSS)->SsCode);
			  }
			while (!ok && i < pSRule->SrNChoices);
		     }
	     }
	if (ok)
	  {
	     *createAll = TRUE;	/* on cree toute la descendance de l'element */
	     *contentType = 0;	/* on ne creera plus de contenu pour cet element */
	  }
     }
}


/*----------------------------------------------------------------------
   CheckMandatAttrSRule verifie que l'element pointe' par pEl      
   possede les attributs requis indique's dans la regle pSRule du  
   schema de structure pSS et, si certains attributs requis        
   manquent, affiche un message d'erreur.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckMandatAttrSRule (PtrElement pEl, SRule * pSRule, PtrSSchema pSS)
#else  /* __STDC__ */
static void         CheckMandatAttrSRule (pEl, pSRule, pSS)
PtrElement          pEl;
SRule              *pSRule;
PtrSSchema          pSS;
#endif /* __STDC__ */
{
   PtrAttribute        pAttr;
   int                 i, att;
   ThotBool            found;

   /* parcourt tous les attributs locaux definis dans la regle */
   for (i = 0; i < pSRule->SrNLocalAttrs; i++)
      if (pSRule->SrRequiredAttr[i])
	 /* cet attribut local est requis */
	{
	   att = pSRule->SrLocalAttr[i];
	   /* cherche si l'element possede cet attribut */
	   pAttr = pEl->ElFirstAttr;
	   found = FALSE;
	   while (pAttr != NULL && !found)
	      if (pAttr->AeAttrNum == att &&
		  pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
		 found = TRUE;
	      else
		 pAttr = pAttr->AeNext;
	   if (!found)
	      /* l'element ne possede pas cet attribut requis */
	      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ATTR_REQUIRED_FOR_EL), pSS->SsAttribute[att - 1].AttrName, pSRule->SrName);
	}
}


/*----------------------------------------------------------------------
   CheckMandatoryAttr verifie que l'element pointe' par pEl possede  
   les attributs requis et, si certains attributs requis   
   manquent, affiche un message d'erreur.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckMandatoryAttr (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static void         CheckMandatoryAttr (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */
{

   SRule              *pSRule;
   PtrSSchema          pSS;

   if (pEl != NULL)
     {
	/* traite d'abord les attributs requis par la regle de structure */
	/* qui definit l'element */
	pSS = pEl->ElStructSchema;
	pSRule = &pSS->SsRule[pEl->ElTypeNumber - 1];
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
		  /* cherche dans ce schema d'extension la regle qui concerne */
		  /* le type de l'element */
		  pSRule = ExtensionRule (pEl->ElStructSchema, pEl->ElTypeNumber, pSS);
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
#ifdef __STDC__
void                ReadAttributePiv (BinFile pivFile, PtrElement pEl, PtrDocument pDoc, ThotBool create, PtrAttribute * pReadAttr, PtrAttribute * pAttr)

#else  /* __STDC__ */
void                ReadAttributePiv (pivFile, pEl, pDoc, create, pReadAttr, pAttr)
BinFile             pivFile;
PtrElement          pEl;
PtrDocument         pDoc;
ThotBool            create;
PtrAttribute       *pReadAttr;
PtrAttribute       *pAttr;

#endif /* __STDC__ */

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
     {
	DisplayPivotMessage (TEXT("Nature Num GetAttributeOfElement "));
	PivotError (pivFile);
     }
   else
      pSchAttr = pDoc->DocNatureSSchema[n];
   if (pSchAttr == NULL)
     {
	DisplayPivotMessage (TEXT("Nature GetAttributeOfElement"));
	PivotError (pivFile);
     }
   /* lit l'attribut */
   TtaReadShort (pivFile, &attr);
   if (pDoc->DocPivotVersion < 4)
      /* on tient compte de l'ajout de l'attribut Langue */
      attr++;
   /* lit le contenu de l'attribut selon son type */
   if (!error)
      switch (pSchAttr->SsAttribute[attr - 1].AttrType)
	    {
	       case AtEnumAttr:
		  TtaReadShort (pivFile, &val);
		  if (val > pSchAttr->SsAttribute[attr - 1].AttrNEnumValues)
		    {
		       printf ("Attribute value error: %s = %d\n", pSchAttr->SsAttribute[attr - 1].AttrOrigName, val);
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
			if (!TtaReadByte (pivFile, &c))
			  {
			     PivotError (pivFile);
			     DisplayPivotMessage (TEXT("A"));
			  }
		     while (!error && c != EOS) ;
		  else
		    { 
		       /* acquiert un premier buffer de texte */
		       GetTextBuffer (&pPremBuff);
		       pBT = pPremBuff;
		       /* lit tout le texte de l'attribut */
		       stop = FALSE;
		       do
			  if (!TtaReadWideChar (pivFile, &pBT->BuContent[pBT->BuLength++], CharEncoding))
			     /* erreur de lecture */
			    {
			       PivotError (pivFile);
			       DisplayPivotMessage (TEXT("A"));
			    }
			  else
			     /* on a lu correctement un caractere */
			  if (pBT->BuContent[pBT->BuLength - 1] == EOS)
			     /* c'est la fin du texte de l'attribut */
			     stop = TRUE;
			  else
			     /* ce n'est pas la fin du texte de l'attribut */
			  if (pBT->BuLength >= THOT_MAX_CHAR - 1)
			     /* le buffer courant est plein */
			    {
			       pBT->BuContent[pBT->BuLength] = EOS;	/* fin du buffer */
			       /* acquiert un nouveau buffer */
			       pBT = NewTextBuffer (pBT);
			    }
		       while (!error && !stop) ;
		       pBT->BuLength--;
		       /* convert old language names into RFC-1766 codes */
		       if (attr == 1)
			  /* language attribute */
			  if (ustrlen(pPremBuff->BuContent) != 2 &&
			      pPremBuff->BuContent[1] != TEXT('-') &&
			      pPremBuff->BuContent[2] != TEXT('-'))
			    /* it's not a valid language code. Convert it */
			    {
			    iso2wc_strcpy (pPremBuff->BuContent, TtaGetLanguageCodeFromName (pPremBuff->BuContent));
			    pBT->BuLength = ustrlen (pPremBuff->BuContent);
			    }
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
	      if (ustrcmp (pSchAttr->SsName, TEXT("ExtCorr")) == 0)
		 create = FALSE;
	      else if (ustrcmp (pSchAttr->SsName, TEXT("ExtMot")) == 0)
		 create = FALSE;
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
		       if (pA->AeAttrSSchema == pSchAttr)
			  if (pA->AeAttrNum == attr)
			     found = TRUE;
		       if (!found)
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
		  pA->AeAttrType = pA->AeAttrSSchema->SsAttribute[pA->AeAttrNum - 1].AttrType;
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
			 CreateReference (pA->AeAttrReference, refType, label, refExt, I, pDoc);
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
#ifdef __STDC__
static void         ReadAttribute (BinFile pivFile, PtrElement pEl, PtrDocument pDoc, ThotBool create, PtrAttribute * pReadAttr)
#else  /* __STDC__ */
static void         ReadAttribute (pivFile, pEl, pDoc, create, pReadAttr)
BinFile             pivFile;
PtrElement          pEl;
PtrDocument         pDoc;
ThotBool            create;
PtrAttribute       *pReadAttr;
#endif /* __STDC__ */

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
#ifdef __STDC__
void                ReadPRulePiv (PtrDocument pDoc, BinFile pivFile, PtrElement pEl, ThotBool create, PtrPRule * pRuleRead, ThotBool link)
#else  /* __STDC__ */
void                ReadPRulePiv (pDoc, pivFile, pEl, create, pRuleRead, link)
PtrDocument         pDoc;
BinFile             pivFile;
PtrElement          pEl;
ThotBool            create;
PtrPRule           *pRuleRead;
ThotBool            link;

#endif /* __STDC__ */
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
  int                 pictureType, val, view, box;
  int                 PicXArea, PicYArea, PicWArea, PicHArea;
  int                 red, green, blue;
  char                ch;
  ThotBool            absolute, sign, just;
  
  pres = (PictureScaling) 0;
  pictureType = 0;
  just = FALSE;
  sign = FALSE;
  absolute = FALSE;
  align = (BAlignment) 0;
  TypeRP = (PRuleType) 0;
  *pRuleRead = NULL;
  unit = UnRelative;
  /* lit le numero de vue */
  TtaReadShort (pivFile, &view);
  /* lit le numero de la boite de present. concernee par la regle */
  TtaReadShort (pivFile, &box);
  /* lit le type de la regle */
  if (!TtaReadByte (pivFile, &ch))
    PivotError (pivFile);
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
    case C_PR_WEIGHT:
      TypeRP = PtWeight;
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
      PivotError (pivFile);
      DisplayPivotMessage (TEXT("p"));
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
	absolute = ReadDimensionType (pivFile);
	TtaReadShort (pivFile, &val);
	unit = ReadUnit (pivFile);
	sign = ReadSign (pivFile);
	break;
      case PtVertPos:
      case PtHorizPos:
	TtaReadShort (pivFile, &val);
	unit = ReadUnit (pivFile);
	sign = ReadSign (pivFile);
	break;
      case PtBreak1:
      case PtBreak2:
      case PtIndent:
      case PtSize:
      case PtLineSpacing:
      case PtLineWeight:
	TtaReadShort (pivFile, &val);
	unit = ReadUnit (pivFile);
	if (TypeRP == PtIndent)
	  sign = ReadSign (pivFile);
	break;
      case PtFillPattern:
	TtaReadShort (pivFile, &val);
	break;
      case PtBackground:
      case PtForeground:
	if (pDoc->DocPivotVersion < 5)
	  TtaReadShort (pivFile, &val);
	else
	  {
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
      case PtLineStyle:
	if (!TtaReadByte (pivFile, &ch))
	  PivotError (pivFile);
	break;
      case PtJustify:
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
	    pDimRule->DrPosition = FALSE;
	    pDimRule->DrAbsolute = absolute;
	    if (!pDimRule->DrAbsolute)
	      /* c'est une dimension relative, on prend */
	      /* la regle qui devrait s'appliquer a     */
	      /* l'element, puis on la modifie selon    */
	      /* ce qui est lu dans le fichier          */
	      {
		pR1 = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, pPRule->PrViewNum,
					   pPRule->PrType, FnAny, FALSE, TRUE, &pAttr);
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
	    break;
	  case PtVertPos:
	  case PtHorizPos:
	    pPosRule = &pPRule->PrPosRule;
	    /* c'est une position relative, on prend */
	    /* la regle qui devrait s'appliquer a    */
	    /* l'element, puis on la modifie selon   */
	    /* ce qui est lu dans le fichier         */
	    pR1 = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, pPRule->PrViewNum,
				       pPRule->PrType, FnAny, FALSE, TRUE, &pAttr);
	    if (pR1 != NULL)
	      *pPRule = *pR1;
	    pPRule->PrViewNum = view;
	    pPRule->PrNextPRule = NULL;
	    pPRule->PrCond = NULL;
	    pPosRule->PoDistAttr = FALSE;
	    pPosRule->PoDistance = val;
	    pPosRule->PoDistUnit = unit;
	    if (!sign)
	      pPosRule->PoDistance = -pPosRule->PoDistance;
	    break;
	  case PtBreak1:
	  case PtBreak2:
	  case PtIndent:
	  case PtSize:
	  case PtLineSpacing:
	  case PtLineWeight:
	    pPRule->PrMinAttr = FALSE;
	    pPRule->PrMinValue = val;
	    pPRule->PrMinUnit = unit;
	    if (pPRule->PrType == PtIndent)
	      if (!sign)
		pPRule->PrMinValue = -pPRule->PrMinValue;
	    break;
	  case PtFillPattern:
	    pPRule->PrAttrValue = FALSE;
	    pPRule->PrIntValue = val;
	    break;
	  case PtBackground:
	  case PtForeground:
	    pPRule->PrAttrValue = FALSE;
	    /* convertit les couleurs des anciennes versions */
	    if (pDoc->DocPivotVersion < 4)
	      val = newColor[val];
	    if (pDoc->DocPivotVersion < 5)
	      pPRule->PrIntValue = val;
	    else
	      pPRule->PrIntValue = TtaGetThotColor ((unsigned short) red, (unsigned short) green, (unsigned short) blue);
	    break;
	  case PtFont:
	  case PtStyle:
	  case PtWeight:
	  case PtUnderline:
	  case PtThickness:
	  case PtLineStyle:
	    pPRule->PrChrValue = ch;
	    break;
	  case PtJustify:
	  case PtHyphenate:
	    pPRule->PrJustify = just;
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
	      !TypeHasException (ExcNoShowBox, pEl->ElTypeNumber, pEl->ElStructSchema))
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
#ifdef __STDC__
void         SendEventAttrRead (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void         SendEventAttrRead (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

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
	     notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
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
   - assocNum:  numero de la liste d'elements associes a`  
   laquelle appartient le texte a` internaliser. Zero si   
   c'est l'arbre principal.                                
   - createParam: indique s'il faut creer un parametre ou non.
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
#ifdef __STDC__
PtrElement          ReadTreePiv (BinFile pivFile, PtrSSchema pSSchema, PtrDocument pDoc, char* tag, int assocNum, ThotBool createParam, ThotBool createAll, int *contentType, PtrSSchema * pContSS, int *typeRead, PtrSSchema * pSSRead, ThotBool createPage, PtrElement pParent, ThotBool createDesc)
#else  /* __STDC__ */
PtrElement          ReadTreePiv (pivFile, pSSchema, pDoc, tag, assocNum, createParam, createAll, contentType, pContSS, typeRead, pSSRead, createPage, pParent, createDesc)
BinFile             pivFile;
PtrSSchema          pSSchema;
PtrDocument         pDoc;
char*               tag;
int                 assocNum;
ThotBool            createParam;
ThotBool            createAll;
int                *contentType;
PtrSSchema         *pContSS;
int                *typeRead;
PtrSSchema         *pSSRead;
ThotBool            createPage;
PtrElement          pParent;
ThotBool            createDesc;
#endif /* __STDC__ */

{
  PtrAttribute        pAttr;
  PtrTextBuffer       pBuf, pBufComment;
  PtrElement          pPrevEl, p, pEl, pEl2, pElInt, pElRead, pfutParent;
  PtrPRule            pPRule;
  PtrSSchema          pSS;
  PtrReferredDescr    pRefD;
  SRule              *pSRule;
  LabelString         label;
  DocumentIdentifier  docIdent;
  BasicType           leafType;
  PageType            pageType;
  ReferenceType       refType;
  PictureScaling      pres;
  NotifyElement       notifyEl;
  PictInfo           *image;
  int                 i, j, n, view, pictureType, elType, rule;
  CHAR_T              alphabet;
  char                c, ch;
  ThotBool            create, inclusion, modif, parameter;
  ThotBool            findtype, refExt, found, withReferences;
  
  pSRule = NULL;
  pEl = NULL;
  withReferences = FALSE;
  create = FALSE;
  if (*tag != C_PIV_TYPE && *tag != C_PIV_NATURE)
    {
      i = 1;
      while (!error && i < 200)
	{
	  if (!TtaReadByte (pivFile, &c))
	    PivotError (pivFile);
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
      DisplayPivotMessage (TEXT("I"));	/* erreur */
      PivotError (pivFile);
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
	    /* on ne cree que les elements de type exporte'. L'element en cours */
	    /* de lecture est-il du type qui constitue le contenu de l'element */
	    /* exporte' englobant ? */
	    ExportedContent (&createAll, &elType, &pSSchema, pContSS, contentType);
	  pSRule = &pSSchema->SsRule[elType - 1];
	  if (createAll)
	    create = TRUE;
	  else
	    /* on ne cree que les elements de type exporte' */
	    {
	      create = FALSE;
	      if (pSRule->SrExportedElem)	/* l'element est d'un type exporte' */
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
			while (pDoc->DocNatureName[i - 1] != pSRule->SrNatExpContent &&
			       i != pDoc->DocNNatures);
			if (pDoc->DocNatureName[i - 1] == pSRule->SrNatExpContent)
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
		    ExportedContent (&createAll, &elType, &pSSchema, pContSS, contentType);
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
		    ExportedContent (&createAll, &elType, &pSSchema, pContSS, contentType);
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
	      if (elType == pSSchema->SsRootElem)
		/* c'est la racine, on cree */
		create = TRUE;
	  if (!create)
	    pEl = NULL;
	  else
	    /* cree un element du type lu */
	    {
	      if (pSSchema->SsRule[elType - 1].SrParamElem && createParam)
		/* simule un element ordinaire, si c'est un parametre a creer */
		{
		  pSSchema->SsRule[elType - 1].SrParamElem = FALSE;
		  parameter = TRUE;
		}
	      else
		parameter = FALSE;
	      /* il ne faut pas que le label max. du document augmente */
	      pEl = NewSubtree (elType, pSSchema, pDoc, assocNum, FALSE, TRUE, FALSE, FALSE);
	      if (pEl != NULL)
		pEl->ElLabel[0] = EOS;
	      if (parameter)
		pSSchema->SsRule[elType - 1].SrParamElem = TRUE;
	    }
	  
	  if (!TtaReadByte (pivFile, tag))
	    PivotError (pivFile);
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
	      CreateReference (pEl->ElSource, refType, label, refExt, docIdent, pDoc);
	      pEl->ElIsCopy = TRUE;
	    }
	  if (!TtaReadByte (pivFile, tag))
	    PivotError (pivFile);
	}
      
      /* lit le tag "Element-reference'" si elle est presente */
      if (!error)
	if (*tag == C_PIV_REFERRED)
	  {
	    withReferences = TRUE;
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile);
	  }
	else
	  withReferences = FALSE;

      /* traite le label s'il est present */
      label[0] = EOS;
      if (!error)
	if (*tag == C_PIV_SHORT_LABEL || *tag == C_PIV_LONG_LABEL || *tag == C_PIV_LABEL)
	  {
	    ReadLabel (*tag, label, pivFile);
	    /* lit le tag qui suit le label */
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile);
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
		    DisplayPivotMessage (TEXT("L"));
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
	    PivotError (pivFile);
	}
      /* lit les attributs de l'element s'il y en a */
      while (*tag == C_PIV_ATTR && !error)
	{
	  ReadAttribute (pivFile, pEl, pDoc, create, &pAttr);
	  if (!error)
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile);
	}
      /* tous les attributs de l'element sont lus, on verifie qu'il ne */
      /* manque pas d'attributs locaux obligatoires pour l'element */
      if (!error && create)
	CheckMandatoryAttr (pEl, pDoc);
      
      /* lit les regles de presentation de l'element */
      /* etablit d'abord le chainage de l'element avec son pere pour que la */
      /* procedure GlobalSearchRulepEl appelee par ReadPRulePiv puisse trouver les */
      /* regles de presentation heritees des attributs des ascendants */
      if (pEl != NULL)
	pEl->ElParent = pParent;
      while (*tag == C_PIV_PRESENT && !error)
	{
	  ReadPRulePiv (pDoc, pivFile, pEl, create, &pPRule, TRUE);
	  if (!error)
	    /* lit l'octet qui suit la regle */
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile);
	}
      /* lit le commentaire qui accompagne eventuellement l'element */
      if (!error)
	if (*tag == C_PIV_COMMENT || *tag == C_PIV_OLD_COMMENT)
	  {
	    pBufComment = ReadComment (pivFile, create, (ThotBool) (*tag == C_PIV_OLD_COMMENT));	/*  */
	    if (create)
	      pEl->ElComment = pBufComment;
	    /* lit l'octet suivant le commentaire */
	    if (!TtaReadByte (pivFile, tag))
	      PivotError (pivFile);
	  }
      if (!error)
	/* si l'element est une copie par inclusion, il n'a pas de contenu */
	if (!inclusion)
	  /* lit le contenu de l'element create */
	  /* traitement specifique selon le constructeur de l'element */
	  switch (pSSchema->SsRule[elType - 1].SrConstruct)
	    {
	    case CsReference:
	      if (*tag != C_PIV_REFERENCE)
		{
		  PivotError (pivFile);
		  DisplayPivotMessage (TEXT("R"));	/* erreur */
		}
	      else
		/* traitement des references : on lit la reference */
		{
		  ReadReference (&refType, label, &refExt, &docIdent, pivFile);
		  if (create)
		    CreateReference (pEl->ElReference, refType, label, refExt, docIdent, pDoc);
		  if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile);
		}
	      break;
	    case CsPairedElement:
	      if (*tag != C_PIV_BEGIN)
		{
		  PivotError (pivFile);
		  DisplayPivotMessage (TEXT("M"));	/* erreur, pas de tag debut */
		}
	      else
		/* traitement des paires : on lit l'identificateur */
		{
		  TtaReadInteger (pivFile, &i);
		  if (create)
		    pEl->ElPairIdent = i;
		  if (i > pDoc->DocMaxPairIdent)
		    pDoc->DocMaxPairIdent = i;
		  if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile);
		  if (*tag != C_PIV_END)
		    /* erreur, pas de tag de fin */
		    {
		      PivotError (pivFile);
		      DisplayPivotMessage (TEXT("m"));
		    }
		  else if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile);
		}
	      break;
	    case CsBasicElement:
	      leafType = pSSchema->SsRule[elType - 1].SrBasicType;
	      if (leafType == CharString)
		if (pDoc->DocPivotVersion >= 4)
		  {
		    if (*tag != C_PIV_LANG)
		      /* pas de tag de langue, c'est la premiere langue de la */
		      /* table des langues du document */
		      i = 0;
		    else
		      {
			/* lit le numero de langue (pour la table des langues du document */
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile);
			else
			  i = (int) (*tag);
			/* lit l'octet suivant */
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile);
		      }
		    if (create && !error)
		      {
			/* i est le rang de la langue dans la table des */
			/* langues du document */
			if (i < 0 || i >= pDoc->DocNLanguages)
			  {
			    DisplayPivotMessage (TEXT("Invalid language"));
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
		    alphabet = TEXT('L');
		    /* dans le cas d'une inclusion sans expansion, il */
		    /* n'y a pas d'alphabet. */
		    /* dans les versions pivot anciennes, il peut y avoir une */
		    /* tag d'alphabet. On la saute */
		    if (*tag != C_PIV_BEGIN && *tag != C_PIV_END && *tag != C_PIV_TYPE && *tag != C_PIV_NATURE)
		      /* on a lu l'alphabet */
		      {
			alphabet = *tag;
			/* lit l'octet suivant */
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile);
		      }
		    if (create)
		      {
			pEl->ElLanguage = TtaGetLanguageIdFromAlphabet (alphabet);
			/* verifie que la langue est dans la table des langues */
			/* du document */
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
	      
	      if (*tag == C_PIV_BEGIN && !error)
		{
		  if (leafType != PageBreak)
		    if (!TtaReadByte (pivFile, tag))
		      PivotError (pivFile);
		  if (*tag != C_PIV_END)	/* il y a un contenu */
		    {
		      switch (leafType)
			{
			case CharString:
			  if (!create)
			    /* saute le texte de l'element */
			    {
			      ch = *tag;
			      while (ch != EOS && !error)
				if (!TtaReadByte (pivFile, &ch))
				  PivotError (pivFile);
			    }
			  else
			    /* lit le texte et remplit les buffers de texte    */
			    {
			      pBuf = pEl->ElText;
			      n = 0;
			      pEl->ElTextLength = 0;
			      ch = *tag;
			      do
				if (ch != EOS)
				  {
				    if (n == THOT_MAX_CHAR - 1)
				      {
					pEl->ElTextLength += n;
					pBuf->BuLength = n;
					pBuf->BuContent[n] = EOS;
					pBuf = NewTextBuffer (pBuf);
					n = 0;
				      }
				    n++;
				    /* mise a la norme iso des anciens pivots */
				    if (pDoc->DocPivotVersion < 3)
				      if (((int) ch) >= 1 && ch < SPACE)
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
					    ch = (((int) ch) + 223);
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
				    pBuf->BuContent[n - 1] = ch;
				    if (!TtaReadByte (pivFile, &ch))
				      PivotError (pivFile);
				  }
			      while (ch != EOS) ;
			      pEl->ElTextLength += n;
			      pBuf->BuLength = n;
			      pBuf->BuContent[n] = EOS;
			      pEl->ElVolume = pEl->ElTextLength;
			    }
			  if (!TtaReadByte (pivFile, tag))
			    PivotError (pivFile);
			  break;
			case Picture:
			  if (!create)
			    /* saute le texte de l'element */
			    {
			      ch = *tag;
			      while (ch != EOS)
				if (!TtaReadByte (pivFile, &ch))
				  PivotError (pivFile);
			    }
			  else
			    /* lit le texte et remplit les buffers de texte    */
			    {
			      pBuf = pEl->ElPictureName;
			      n = 0;
			      pEl->ElNameLength = 0;
			      ch = *tag;
			      do
				if (ch != EOS)
				  {
				    /* TODO : nom d'image > THOT_MAX_CHAR */
				    if (n == THOT_MAX_CHAR - 1)
				      {
					PivotError (pivFile);
					DisplayPivotMessage (TEXT("x"));
				      }
				    n++;
				    /* range le caractere et lit le suivant */
				    pBuf->BuContent[n - 1] = ch;
				    if (!TtaReadByte (pivFile, &ch))
				      PivotError (pivFile);
				  }
			      while (ch != EOS);

			      /* on suppose que le nom tient en entier dans un buffer */
			      /* on normalise le nom */
			      ustrcpy (pBuf->BuContent, NormalizeFileName (pBuf->BuContent, &pictureType,
									  &pres, &findtype));
			      if (findtype)
				{
				  /* on a trouve une image v1, on cree une regle */
				  /* de presentation PictInfo pour l'element */
				  SetImageRule (pEl, 0, 0, 0, 0, pictureType, pres);
				}
			      /* complete the Picture information block */
			      image = (PictInfo *)pEl->ElPictInfo;
			      if (image != NULL)
				image->PicFileName = pBuf->BuContent;
			      pEl->ElNameLength += n;
			      pBuf->BuLength = n;
			      pBuf->BuContent[n] = EOS;
			      pEl->ElVolume = pEl->ElNameLength;
			    }
			  if (!TtaReadByte (pivFile, tag))
			    PivotError (pivFile);
			  break;
			case Symbol:
			case GraphicElem:
			  /* on a lu le code representant la forme */
			  ch = *tag;
			  /* lit l'octet qui suit */
			  if (!TtaReadByte (pivFile, tag))
			    PivotError (pivFile);
			  else if (*tag != C_PIV_POLYLINE)
			    /* c'est un element graphique simple */
			    {
			      if (create)
				{
				  pEl->ElGraph = ch;
				  /* remplace les anciens rectangles trame's par */
				  /* de simple rectangles */
				  if (pEl->ElGraph >= '0' && pEl->ElGraph <= '9')
				    pEl->ElGraph = 'R';
				  else if (pEl->ElGraph >= '\260' && pEl->ElGraph <= '\270')
				    pEl->ElGraph = 'R';
				  if (ch == EOS)
				    pEl->ElVolume = 0;
				  else
				    pEl->ElVolume = 1;
				}
			    }
			  else
			    /* c'est une Polyline */
			    {
			      /* lit le nombre de points de la ligne */
			      if (!TtaReadShort (pivFile, &n))
				PivotError (pivFile);
			      /* lit tous les points */
			      else if (!create)
				for (i = 0; i < n; i++)
				  TtaReadInteger (pivFile, &j);
			      else
				{
				  /* transforme l'element graphique simple en Polyline */
				  pEl->ElLeafType = LtPolyLine;
				  GetTextBuffer (&pEl->ElPolyLineBuffer);
				  pEl->ElVolume = n;
				  pEl->ElPolyLineType = ch;
				  pEl->ElNPoints = n;
				  pBuf = pEl->ElPolyLineBuffer;
				  j = 0;
				  for (i = 0; i < n; i++)
				    {
				      if (j >= MAX_POINT_POLY)
					/* buffer courant plein */
					{
					  pBuf = NewTextBuffer (pBuf);
					  j = 0;
					}
				      TtaReadInteger (pivFile, &pBuf->BuPoints[j].XCoord);
				      TtaReadInteger (pivFile, &pBuf->BuPoints[j].YCoord);
				      pBuf->BuLength++;
				      j++;
				    }
				}
			      /* lit l'octet qui suit (tag de fin d'element) */
			      if (!TtaReadByte (pivFile, tag))
				PivotError (pivFile);
			    }
			  break;
			case PageBreak:
			  /* lit le numero de page et */
			  /* le type de page */
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
			    PivotError (pivFile);
			  break;
			default:
			  break;
			}
		      
		    }
		  if (*tag != C_PIV_END)
		    {
		      PivotError (pivFile);
		      DisplayPivotMessage (TEXT("F"));
		    }
		  
		  if (!TtaReadByte (pivFile, tag))
		    PivotError (pivFile);
		}
	      break;
	    default:
	      /* traite le contenu s'il y en a un */
	      if (*tag == C_PIV_BEGIN)
		{
		  if (pEl != NULL)
		    if (pEl->ElTerminal)
		      {
			PivotError (pivFile);
			DisplayPivotMessage (TEXT("f"));
		      }
		  /* erreur: feuille avec contenu */
		  if (!error)
		    {
		      if (!TtaReadByte (pivFile, tag))
			PivotError (pivFile);
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
			  p = ReadTreePiv (pivFile, pSSchema, pDoc, tag, assocNum, createParam, createAll, contentType, pContSS, &rule, &pSS, createPage, pfutParent, createDesc);
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
					if (p->ElStructSchema != pEl->ElStructSchema)
					  /* l'element a inserer dans l'arbre appartient       */
					  /* a un schema different de celui de son pere        */
					  if (p->ElTypeNumber != p->ElStructSchema->SsRootElem)
					    /* ce n'est pas la racine d'une nature, on ajoute  */
					    /* un element intermediaire */
					    {
					      pEl2 = p;
					      /* il ne faut pas que le label */
					      /* max. du document augmente */
					      pElInt = NewSubtree (pEl2->ElStructSchema->SsRootElem,
								   pEl2->ElStructSchema,
								   pDoc, assocNum, FALSE, TRUE, FALSE, FALSE);
					      
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
				/* Si l'element qu'on vient de lire n'a pas ete      */
				/* cree' (lecture squelette) mais que certains de    */
				/* ses descendants l'ont ete,ReadTreePiv a retourne' */
				/* un pointeur sur le premier descendant cree'. On   */
				/* cherche le dernier frere, qui devient l'element   */
				/* precedent du prochain element lu. */
				
				while (pPrevEl->ElNext != NULL)
				  pPrevEl = pPrevEl->ElNext;
			      }
			}
		      if (!error)
			if (!TtaReadByte (pivFile, tag))
			  PivotError (pivFile);
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
#ifdef __STDC__
ThotBool            AbstractTreeOK (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
ThotBool            AbstractTreeOK (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
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
		  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INVALID_SIBLING),
		  pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
				     pEl->ElPrevious->ElStructSchema->SsRule[pEl->ElPrevious->ElTypeNumber - 1].SrName,
				     pEl->ElLabel);
	       }
	  }
	else if (pEl->ElParent != NULL)
	  {
	     if (!AllowedFirstChild (pEl->ElParent, pDoc, pEl->ElTypeNumber,
				     pEl->ElStructSchema, FALSE, TRUE))
	       {
		  ok = FALSE;
		  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INVALID_CHILD),
		  pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
				     pEl->ElParent->ElStructSchema->SsRule[pEl->ElParent->ElTypeNumber - 1].SrName,
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
#ifdef __STDC__
void                AssociatePairs (PtrElement pRoot)
#else  /* __STDC__ */
void                AssociatePairs (pRoot)
PtrElement          pRoot;

#endif /* __STDC__ */
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
	   if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrFirstOfPair)
	      /* c'est un element de debut de paire */
	     {
		/* on cherche l'element de fin correspondant */
		pEl2 = pEl1;
		found = FALSE;
		do
		  {
		     pEl2 = FwdSearchTypedElem (pEl2, pEl1->ElTypeNumber + 1, pEl1->ElStructSchema);
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
#ifdef __STDC__
static void         SetLabel (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static void         SetLabel (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void         PutNatureInTable (PtrDocument pDoc, Name SSName, int rank)
#else  /* __STDC__ */
static void         PutNatureInTable (pDoc, SSName, rank)
PtrDocument         pDoc;
Name                SSName;
int                 rank;

#endif /* __STDC__ */
{
   int                 i;
   ThotBool            found;
   Name                N1, N2;
   PtrSSchema          pSS;

   if (rank > pDoc->DocNNatures + 1)
      /* le rang voulu pour la nature est invraissemblable */
      DisplayPivotMessage (TEXT("Err nature ???"));
   /* on cherche (par son nom) si la nature existe dans la table */
   i = 1;
   found = FALSE;
   while (i <= pDoc->DocNNatures && !found)
      if (ustrncmp (SSName, pDoc->DocNatureName[i - 1], MAX_NAME_LENGTH) == 0)
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
	     ustrncpy (N1, pDoc->DocNatureName[rank - 1], MAX_NAME_LENGTH);
	     ustrncpy (N2, pDoc->DocNaturePresName[rank - 1], MAX_NAME_LENGTH);
	     pDoc->DocNatureSSchema[rank - 1] = pDoc->DocNatureSSchema[i - 1];
	     ustrncpy (pDoc->DocNatureName[rank - 1], pDoc->DocNatureName[i - 1], MAX_NAME_LENGTH);
	     ustrncpy (pDoc->DocNaturePresName[rank - 1], pDoc->DocNaturePresName[i - 1], MAX_NAME_LENGTH);
	     pDoc->DocNatureSSchema[i - 1] = pSS;
	     ustrncpy (pDoc->DocNatureName[i - 1], N1, MAX_NAME_LENGTH);
	     ustrncpy (pDoc->DocNaturePresName[i - 1], N2, MAX_NAME_LENGTH);
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
	     pDoc->DocNatureSSchema[pDoc->DocNNatures] = pDoc->DocNatureSSchema[rank - 1];
	     ustrncpy (pDoc->DocNatureName[pDoc->DocNNatures], pDoc->DocNatureName[rank - 1], MAX_NAME_LENGTH);
	     ustrncpy (pDoc->DocNaturePresName[pDoc->DocNNatures], pDoc->DocNaturePresName[rank - 1], MAX_NAME_LENGTH);
	     pDoc->DocNNatures++;
	  }
	pDoc->DocNatureSSchema[rank - 1] = NULL;
	ustrncpy (pDoc->DocNatureName[rank - 1], SSName, MAX_NAME_LENGTH);
     }
}


/*----------------------------------------------------------------------
   	ReadSchemaNamesPiv lit les noms des schemas de structure et de	
   	presentation qui se trouvent dans le fichier file et charge ces	
   	schemas								
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ReadSchemaNamesPiv (BinFile file, PtrDocument pDoc, char* tag, PtrSSchema pLoadedSS, void (*withThisPSchema) (Document document, CHAR_T* natSchema, CHAR_T* presentSchema))
#else  /* __STDC__ */
void                ReadSchemaNamesPiv (file, pDoc, tag, pLoadedSS, withThisPSchema)
BinFile             file;
PtrDocument         pDoc;
char*               tag;
PtrSSchema          pLoadedSS;
void (*withThisPSchema) ();

#endif /* __STDC__ */

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
      if (!TtaReadWideChar (file, &SSName[i++], ISOLatin1))
         PivotError (file);
   while (!error && SSName[i - 1] != WC_EOS && i != MAX_NAME_LENGTH) ;
   if (SSName[i - 1] != WC_EOS)
     {
        PivotError (file);
        DisplayPivotMessage (TEXT("Z"));
     }
   else
     {
         if (pDoc->DocPivotVersion >= 4)
            /* Lit le code du schema de structure */
            if (!error)
               if (!TtaReadShort (file, &versionSchema))
                  PivotError (file);
            /* Lit le nom du schema de presentation associe' */
            i = 0;
            do
              if (!TtaReadWideChar (file, &PSchemaName[i++], ISOLatin1))
                 PivotError (file);
            while (!error && PSchemaName[i - 1] != WC_EOS && i != MAX_NAME_LENGTH) ;

         if (!TtaReadByte (file, tag))
            PivotError (file);
         if (withThisPSchema != NULL)
            (*withThisPSchema) ((Document) IdentDocument (pDoc), SSName, PSchemaName);
         PutNatureInTable (pDoc, SSName, rank);
         /* charge les schemas de structure et de presentation du document */
		 if (pDoc->DocSSchema == NULL) 
           LoadSchemas (SSName, PSchemaName, &pDoc->DocSSchema, pLoadedSS, FALSE);
		 
        if (pDoc->DocSSchema == NULL)
           PivotError (file);
        else if (pDoc->DocPivotVersion >= 4)
             /* on verifie que la version du schema charge' est la meme */
             /* que celle du document */
        if (pDoc->DocSSchema->SsCode != versionSchema)
           {
              pDoc->DocCheckingMode |= PIV_CHECK_MASK;
              TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_STR_SCH_CHANGED), pDoc->DocSSchema->SsName);
           }
     }
   if (pDoc->DocNatureSSchema[rank - 1] == NULL)
     {
        pDoc->DocNatureSSchema[rank - 1] = pDoc->DocSSchema;
        ustrncpy (pDoc->DocNatureName[rank - 1], SSName, MAX_NAME_LENGTH);
        ustrncpy (pDoc->DocNaturePresName[rank - 1], PSchemaName, MAX_NAME_LENGTH);
        if (pDoc->DocSSchema != NULL)
           if (pDoc->DocSSchema->SsPSchema == NULL)
              /* le schema de presentation n'a pas ete charge' (librairie  */
              /* Kernel, par exemple). On memorise dans le schema de */
              /* structure charge' le nom du schema P associe' */
              ustrncpy (pDoc->DocSSchema->SsDefaultPSchema, PSchemaName, MAX_NAME_LENGTH);
     }
   /* lit les noms des fichiers contenant les schemas de nature  */
   /* dynamiques et charge ces schemas, sauf si on ne charge que */
   /* les elements exportables. */
   while ((*tag == (CHAR_T) C_PIV_NATURE || *tag == (CHAR_T) C_PIV_SSCHEMA_EXT)
	  && !error)
     {
	ExtensionSch = (*tag == (CHAR_T) C_PIV_SSCHEMA_EXT);
	i = 0;
	rank++;
	do
	   if (!TtaReadWideChar (file, &SSName[i++], ISOLatin1))
	      PivotError (file);
	while (SSName[i - 1] != EOS && !error) ;
	if (pDoc->DocPivotVersion >= 4)
	   /* Lit le code du schema de structure */
	   if (!error)
	      if (!TtaReadShort (file, &versionSchema))
		 PivotError (file);
	/* Lit le nom du schema de presentation associe' */
	if (!error)
	   if (!TtaReadByte (file, tag))
	      PivotError (file);
	if (*tag >= TEXT('!') && *tag <= TEXT('~') && !error)
	   /* il y a un nom de schema de presentation */
	  {
	     PSchemaName[0] = *tag;
	     i = 1;
	     do
		if (!TtaReadWideChar (file, &PSchemaName[i++], ISOLatin1))
		   PivotError (file);
	     while (!error && PSchemaName[i - 1] != EOS && i != MAX_NAME_LENGTH) ;

	     if (!TtaReadByte (file, tag))
		PivotError (file);
	  }
	else
	   /* il n'y a pas de nom */
	   PSchemaName[0] = EOS;
	pSS = NULL;
	if (!error)
	  {
             if (withThisPSchema != NULL)
                (*withThisPSchema) ((Document) IdentDocument (pDoc), SSName, PSchemaName);
	     PutNatureInTable (pDoc, SSName, rank);
	     if (pDoc->DocNatureSSchema[rank - 1] == NULL)
		if (ExtensionSch)
		   /* charge l'extension de schema */
		   pSS = LoadExtension (SSName, PSchemaName, pDoc);
		else 
		  {
		     i = CreateNature (SSName, PSchemaName, pDoc->DocSSchema);
		     if (i == 0)
			PivotError (file);	/* echec creation nature */
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
		    pDoc->DocCheckingMode |= PIV_CHECK_MASK;
		    TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_STR_SCH_CHANGED), pSS->SsName);
		  }
	     pDoc->DocNatureSSchema[rank - 1] = pSS;
	     ustrncpy (pDoc->DocNaturePresName[rank - 1], PSchemaName, MAX_NAME_LENGTH);
	     if (pSS->SsPSchema == NULL)
		/* le schema de presentation n'a pas ete charge' (librairie
		   Kernel, par exemple). On memorise dans le schema de structure
		   charge' le nom du schema P associe' */
		ustrncpy (pSS->SsDefaultPSchema, PSchemaName, MAX_NAME_LENGTH);
	  }
     }
}


/*----------------------------------------------------------------------
   	ReadLanguageTablePiv
	lit la table des langues qui se trouve en tete du fichier pivot.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ReadLanguageTablePiv (BinFile file, PtrDocument pDoc, char* tag)
#else  /* __STDC__ */
void                ReadLanguageTablePiv (file, pDoc, tag)
BinFile             file;
PtrDocument         pDoc;
char*               tag;
#endif /* __STDC__ */

{
   Name             languageName;
   int              i;

   /* lit la table des langues utilisees par le document */
   pDoc->DocNLanguages = 0;
   if (pDoc->DocPivotVersion >= 4)
      while (*tag == (CHAR_T) C_PIV_LANG && !error)
	{
	   i = 0;
	   do
	      if (!TtaReadWideChar (file, &languageName[i++], ISOLatin1))
		 PivotError (file);
	   while (!error && languageName[i - 1] != WC_EOS && i != MAX_NAME_LENGTH) ;
	   if (languageName[i - 1] != WC_EOS)
	     {
		PivotError (file);
		DisplayPivotMessage (TEXT("Z"));
	     }
	   else
	     {
		if (languageName[0] != WC_EOS)
		   if (pDoc->DocNLanguages < MAX_LANGUAGES_DOC)
		      pDoc->DocLanguages[pDoc->DocNLanguages++] = TtaGetLanguageIdFromName (languageName);
		/* lit l'octet suivant le nom de langue */
		if (!TtaReadByte (file, tag))
		   PivotError (file);
	     }
	}
}


/*----------------------------------------------------------------------
   ReadVersionNumberPiv lit la version dans le fichier pivot et         
   met sa valeur dans le contexte pDoc. Retourne 0 si OK.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ReadVersionNumberPiv (BinFile file, PtrDocument pDoc)
#else  /* __STDC__ */
int                 ReadVersionNumberPiv (file, pDoc)
BinFile             file;
PtrDocument         pDoc;
#endif /* __STDC__ */

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

#ifdef __STDC__
void                ReadPivotHeader (BinFile file, PtrDocument pDoc, char* tag)

#else  /* __STDC__ */
void                ReadPivotHeader (file, pDoc, tag)
BinFile             file;
PtrDocument         pDoc;
char*               tag;

#endif /* __STDC__ */

{
   LabelString         label;
   int                 i;
   char                c;

   /* lit le numero de version s'il est present */
   if (!TtaReadByte (file, tag))
      PivotError (file);
   if (*tag == (CHAR_T) C_PIV_VERSION)
     {
	if (!TtaReadByte (file, tag))
	   PivotError (file);
	else if (!TtaReadByte (file, &c))
	   PivotError (file);
	else
	  {
	     pDoc->DocPivotVersion = (int) c;
	     if (!TtaReadByte (file, tag))
		PivotError (file);
	  }
     }
   else				/* Pivot sans tag de version */
      pDoc->DocPivotVersion = 1;

   /* lit le label max. du document s'il est present */
   if (!error && (*tag == (CHAR_T) C_PIV_SHORT_LABEL ||
		  *tag == (CHAR_T) C_PIV_LONG_LABEL ||
		  *tag == (CHAR_T) C_PIV_LABEL))
     {
	ReadLabel (*tag, label, file);
	LabelStringToInt (label, &i);
	SetCurrentLabel (pDoc, i);
	if (!TtaReadByte (file, tag))
	   PivotError (file);
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
#ifdef __STDC__
void                LoadDocumentPiv (BinFile file, PtrDocument pDoc, ThotBool loadExternalDoc, ThotBool skeleton, PtrSSchema pLoadedSS, ThotBool withEvent, ThotBool removeExclusions)
#else  /* __STDC__ */
void                LoadDocumentPiv (file, pDoc, loadExternalDoc, skeleton, pLoadedSS, withEvent, removeExclusions)
BinFile             file;
PtrDocument         pDoc;
ThotBool            loadExternalDoc;
ThotBool            skeleton;
PtrSSchema          pLoadedSS;
ThotBool            withEvent;
ThotBool		    removeExclusions
#endif /* __STDC__ */

{
   PtrElement          s, p, pFirst;
   PtrSSchema          pSS, pNat, pSchS1, curExtension, previousSSchema;
   PtrPSchema          pPSchema;
   PtrReferredDescr    pRefD, pNextRefD;
   SRule              *pSRule;
   NotifyDialog        notifyDoc;
   BinFile             EXTfile;
   int                 i, j, assoc, rule, typeRead;
   CHAR_T                buffer[MAX_TXT_LEN];
   char                tag;
   ThotBool            structureOK, createPages, found, ok;

/*    pDoc->DocToBeChecked = FALSE; */
   pDoc->DocCheckingMode &= ~PIV_CHECK_MASK;
   structureOK = TRUE;
   ok = FALSE;
   error = FALSE;
   msgOldFormat = TRUE;
   pDoc->DocRootElement = NULL;
   createPages = FALSE;
   pDoc->DocNNatures = 0;
   /* lit l'entete du fichier pivot */
   ReadPivotHeader (file, pDoc, &tag);
   /* lit le commentaire du document s'il est present */
   if (!error && (tag == (CHAR_T) C_PIV_COMMENT ||
		  tag == (CHAR_T) C_PIV_OLD_COMMENT))
     {
	pDoc->DocComment = ReadComment (file, TRUE, (ThotBool)(tag == (CHAR_T) C_PIV_OLD_COMMENT));
	/* lit l'octet suivant le commentaire */
	if (!TtaReadByte (file, &tag))
	   PivotError (file);
     }
   /* Lit le nom du schema de structure qui est en tete du fichier pivot */
   if (!error && tag != (CHAR_T) C_PIV_NATURE)
     {
	PivotError (file);
	DisplayPivotMessage (TEXT("N"));	/* tag classe absente */
     }
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
	   /* seulement si le schema de presentation a une vue THOT_EXPORT      */
	  {
	     createPages = FALSE;
	     ok = FALSE;
	     pPSchema = pDoc->DocSSchema->SsPSchema;
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
	/* Lit d'abord dans le fichier .EXT les labels des elements */
	/* reference's par d'autres documents (on en aura besoin */
	/* pendant la lecture du fichier .PIV). On cherche ce */
	/* fichier dans le meme directory que le fichier .PIV */
	FindCompleteName (pDoc->DocDName, TEXT("EXT"), pDoc->DocDirectory, buffer, &i);
	EXTfile = TtaReadOpen (buffer);
	if (EXTfile != 0)
	  {
	     LoadEXTfile (EXTfile, NULL, &pDoc->DocLabels, TRUE);
	     TtaReadClose (EXTfile);
	  }
	else
	   pDoc->DocLabels = NULL;
	/* ReadTreePiv le fichier .PIV */

	/* lit les parametres */
	for (i = 0; i < MAX_PARAM_DOC; i++)
	   pDoc->DocParameters[i] = NULL;
	i = 1;
	while (tag == (CHAR_T) C_PIV_PARAM && !error)
	   if (i > MAX_PARAM_DOC)
	     {
		PivotError (file);
		DisplayPivotMessage (TEXT("Y"));
	     }
	   else
	     {
		if (!TtaReadByte (file, &tag))
		   PivotError (file);
		rule = 0;
		pNat = NULL;
		p = ReadTreePiv (file, pDoc->DocSSchema, pDoc, &tag, 0, TRUE, (ThotBool)(!pDoc->DocExportStructure), &rule, &pNat, &typeRead, &pSS, createPages, NULL, TRUE);
		if (withEvent && pDoc->DocSSchema != NULL && !error)
		   SendEventAttrRead (p, pDoc);
		if (!error)
		  {
		     /* retire les elements exclus */
		     if (removeExclusions)
		        RemoveExcludedElem (&p, pDoc);
		     /* accouple les paires */
		     AssociatePairs (p);
		     pDoc->DocParameters[i - 1] = p;
		     if (pDoc->DocCheckingMode & PIV_CHECK_MASK)
			/* verifie que cet arbre est correct */
		       {
			  ok = AbstractTreeOK (p, pDoc);
			  structureOK = structureOK && ok;
		       }
		  }
		i++;
	     }
	/* lit les elements associes */
	for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
	   pDoc->DocAssocRoot[assoc] = NULL;
	assoc = 0;
	while (tag == (CHAR_T) C_PIV_ASSOC && !error)
	   /* debut d'un nouveau type d'element associe */
	  {
	     assoc++;
	     if (!TtaReadByte (file, &tag))
		PivotError (file);
	     /* lit et cree le premier element associe de ce type */
	     rule = 0;
	     pNat = NULL;

	     p = ReadTreePiv (file, pDoc->DocSSchema, pDoc, &tag, assoc, FALSE,
			      (ThotBool)(!pDoc->DocExportStructure), &rule, &pNat,
			      &typeRead, &pSS, createPages, NULL, TRUE);
	     if (withEvent && pDoc->DocSSchema != NULL && !error)
		SendEventAttrRead (p, pDoc);
	     pSRule = &pSS->SsRule[typeRead - 1];

	     if (pSRule->SrConstruct == CsList &&
		 pSS->SsRule[pSRule->SrListItem - 1].SrAssocElem)
		/* l'element lu est la racine d'un arbre d'elements associe'. */
		/* L'arbre a ete lu entierement */
	       {
		  pDoc->DocAssocRoot[assoc - 1] = p;
		  /* retire les elements exclus */
		  if (p != NULL)
		    {
		       if (removeExclusions)
		          RemoveExcludedElem (&pDoc->DocAssocRoot[assoc - 1], pDoc);
		       /* accouple les paires */
		       AssociatePairs (p);
		    }
	       }
	     else
	       {
		  /* c'est sans doute l'ancienne forme pivot, ou` la racine de */
		  /* l'arbre d'elements associes, n'est pas presente, mais */
		  /* seulement ses fils */
		  pFirst = p;
		  /* on lit les tags de pages jusqu'au premier element associe' */
		  if (!error && typeRead == PageBreak + 1)
		     while (typeRead == PageBreak + 1)
		       {
			  rule = 0;
			  pNat = NULL;
			  s = ReadTreePiv (file, pDoc->DocSSchema, pDoc, &tag, assoc,
			     FALSE, (ThotBool)(!pDoc->DocExportStructure), &rule, &pNat,
				  &typeRead, &pSS, createPages, NULL, TRUE);
			  if (withEvent && pDoc->DocSSchema != NULL && !error)
			     SendEventAttrRead (s, pDoc);
			  if (s != NULL)
			    {
			       if (pFirst == NULL)
				  pFirst = s;
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
					       pSchS1, typeRead, pSS, NULL))
				       found = TRUE;
				 if (!found)
				    j--;
			      }
			    while (!found && j != 1);
			    if (!found)
			       /* pas trouve', on cherche dans l'extension suivante du */
			       /* schema de structure du document */
			       pSchS1 = pSchS1->SsNextExtens;
			 }
		       while (!found && pSchS1 != NULL);
		       if (!found)
			 {
			    PivotError (file);
			    DisplayPivotMessage (TEXT("a"));
			 }
		       else
			  /* cree l'element liste pour ce type d'elements associes */
			 {
			    pDoc->DocAssocRoot[assoc - 1] = NewSubtree (j, pSchS1,
				     pDoc, assoc, FALSE, TRUE, FALSE, TRUE);
			    if (pFirst != NULL)
			      {
				 /* chaine le 1er elem. associe dans cette liste */
				 InsertFirstChild (pDoc->DocAssocRoot[assoc - 1], pFirst);
				 /* retire les elements exclus */
				 if (removeExclusions)
				    RemoveExcludedElem (&pDoc->DocAssocRoot[assoc - 1], pDoc);
				 /* accouple les paires */
				 AssociatePairs (pDoc->DocAssocRoot[assoc - 1]);
			      }
			 }
		    }
		  /* lit les elements associes suivants de meme type */
		  while (!error && (tag == (CHAR_T) C_PIV_TYPE || tag == (CHAR_T) C_PIV_NATURE))
		    {
		       rule = 0;
		       pNat = NULL;
		       s = ReadTreePiv (file, pDoc->DocSSchema, pDoc, &tag, assoc,
			     FALSE, (ThotBool)(!pDoc->DocExportStructure), &rule, &pNat,
					&typeRead, &pSS, createPages,
					pDoc->DocAssocRoot[assoc - 1], TRUE);
		       if (withEvent && pDoc->DocSSchema != NULL && !error)
			  SendEventAttrRead (s, pDoc);
		       if (s != NULL)
			 {
			    if (p == NULL)
			       InsertFirstChild (pDoc->DocAssocRoot[assoc - 1], s);
			    else
			       InsertElementAfter (p, s);
			    /* retire les elements exclus */
			    if (removeExclusions)
			       RemoveExcludedElem (&s, pDoc);
			    /* accouple les paires */
			    AssociatePairs (s);
			    if (s != NULL)
			       p = s;
			 }
		    }
	       }
	     if (!error && (pDoc->DocCheckingMode & PIV_CHECK_MASK))
		/* verifie que cet arbre est correct */
	       {
		  ok = AbstractTreeOK (pDoc->DocAssocRoot[assoc - 1], pDoc);
		  structureOK = structureOK && ok;
	       }
	  }

	/* lit le corps du document */
	if (!error)
	   if (tag != (CHAR_T) C_PIV_DOC_END)
	      if (tag != (CHAR_T) C_PIV_STRUCTURE)
		{
		   PivotError (file);
		   DisplayPivotMessage (TEXT("O"));
		}
	      else
		{
		   if (!TtaReadByte (file, &tag))
		      PivotError (file);
		   if (tag != (CHAR_T) C_PIV_TYPE && tag != (CHAR_T) C_PIV_NATURE)
		     {
			PivotError (file);
			DisplayPivotMessage (TEXT("P"));
		     }
		   else
		     {
			rule = 0;
			pNat = NULL;
			p = ReadTreePiv (file, pDoc->DocSSchema, pDoc, &tag, 0,
			     FALSE, (ThotBool)(!pDoc->DocExportStructure), &rule, &pNat,
				  &typeRead, &pSS, createPages, NULL, TRUE);
			if (withEvent && pDoc->DocSSchema != NULL && !error)
			   SendEventAttrRead (p, pDoc);
			/* force la creation d'un element racine */
			if (p == NULL)
			   /* rien n'a ete cree */
			   p = NewSubtree (pDoc->DocSSchema->SsRootElem, pDoc->DocSSchema, pDoc, 0,
					   FALSE, TRUE, TRUE, TRUE);
			else if (p->ElStructSchema != pDoc->DocSSchema
			 || p->ElTypeNumber != pDoc->DocSSchema->SsRootElem)
			   /* ce n'est pas la racine attendue */
			  {
			     s = p;
			     p = NewSubtree (pDoc->DocSSchema->SsRootElem, pDoc->DocSSchema, pDoc, 0,
					     FALSE, TRUE, TRUE, TRUE);
			     InsertFirstChild (p, s);
			  }
			/* traite les elements exclus */
			if (removeExclusions)
			   RemoveExcludedElem (&p, pDoc);
			/* accouple les paires */
			AssociatePairs (p);
			pDoc->DocRootElement = p;
			if (pDoc->DocCheckingMode & PIV_CHECK_MASK)
			   /* verifie que cet arbre est correct */
			  {
			     ok = AbstractTreeOK (p, pDoc);
			     structureOK = structureOK && ok;
			  }
		     }
		}
	if (!structureOK)
	   /* Le document n'est pas correct */
	  {
	     /* on previent l'utilisateur */
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_DOC_STRUCTURE), pDoc->DocDName);
#ifdef IV
             debugstr = TtaGetEnvString ("DEBUG");
	     if(debugstr == NULL || ustrcasecmp(debugstr,"YES"))
	       {
	         /* on met le document en Read-Only */
	         pDoc->DocReadOnly = TRUE;
	         TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LOCKED_DOC), pDoc->DocDName);
               }
#endif
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

	/* supprime les extensions de schemas ExtCorr et ExtMot */
	/* les attributs definis dans ces extensions ont deje ete retire's par */
	/* ReadAttributePiv (ces extensions ne definissent que des attributs) */
	if (!error && pDoc->DocPivotVersion < 4)
	  {
	     previousSSchema = pDoc->DocSSchema;
	     curExtension = previousSSchema->SsNextExtens;
	     while (curExtension != NULL)
		if (ustrcmp (curExtension->SsName, TEXT("ExtCorr")) == 0 ||
		    ustrcmp (curExtension->SsName, TEXT("ExtMot")) == 0)
		  {
		     previousSSchema->SsNextExtens = curExtension->SsNextExtens;
		     if (curExtension->SsNextExtens != NULL)
			curExtension->SsNextExtens->SsPrevExtens = previousSSchema;
		     FreeSchStruc (curExtension);
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
		SetLabel (pDoc->DocRootElement, pDoc);
	     /* on affecte des labels aux elements des arbres associes */
	     for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
		if (pDoc->DocAssocRoot[assoc] != NULL)
		   SetLabel (pDoc->DocAssocRoot[assoc], pDoc);
	     /* on affecte des labels aux elements des parametres */
	     for (i = 0; i < MAX_PARAM_DOC; i++)
		if (pDoc->DocParameters[i] != NULL)
		   SetLabel (pDoc->DocParameters[i], pDoc);

             /* Update the inclusions values */
             UpdateInclusionElements (pDoc, loadExternalDoc, removeExclusions);

	     /* verifie que les racines de tous les arbres du document possedent */
	     /* bien un attribut langue */
	     CheckLanguageAttr (pDoc, pDoc->DocRootElement);
	     pDoc->DocRootElement->ElAccess = AccessReadWrite;
	     for (i = 0; i < MAX_PARAM_DOC; i++)
		if (pDoc->DocParameters[i] != NULL)
		  {
		     CheckLanguageAttr (pDoc, pDoc->DocParameters[i]);
		     pDoc->DocParameters[i]->ElAccess = AccessReadOnly;
		  }
	     for (i = 0; i < MAX_ASSOC_DOC; i++)
		if (pDoc->DocAssocRoot[i] != NULL)
		  {
		     CheckLanguageAttr (pDoc, pDoc->DocAssocRoot[i]);
		     pDoc->DocAssocRoot[i]->ElAccess = AccessReadWrite;
		  }
	     if (ThotLocalActions[T_indexschema] != NULL)
		(*ThotLocalActions[T_indexschema]) (pDoc);
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
