/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 * Manage free-lists and memory allocation.
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA) - Boxes and lines
 *          H. Richy (IRISA) - Dictionaries
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typecorr.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"

#undef  EXPORT
#define EXPORT
#include "boxes_tv.h"
#include "edit_tv.h"

int                 NbFree_TextBuff;
int                 NbUsed_TextBuff;
PtrTextBuffer       PtFree_TextBuff;

int                 NbFree_Element;
int                 NbUsed_Element;
PtrElement          PtFree_Element;

int                 NbFree_Attr;
int                 NbUsed_Attr;
PtrAttribute        PtFree_Attr;

int                 NbFree_DescRef;
int                 NbUsed_DescRef;
PtrReferredDescr    PtFree_DescRef;

int                 NbFree_ExternalDoc;
int                 NbUsed_ExternalDoc;
PtrExternalDoc      PtFree_ExternalDoc;

int                 NbFree_DescCopy;
int                 NbUsed_DescCopy;
PtrCopyDescr        PtFree_DescCopy;


int                 NbFree_Reference;
int                 NbUsed_Reference;
PtrReference        PtFree_Reference;

int                 NbFree_OutputRef;
int                 NbUsed_OutputRef;
PtrOutReference     PtFree_OutputRef;

int                 NbFree_ElemRefChng;
int                 NbUsed_ElemRefChng;
PtrChangedReferredEl PtFree_ElemRefChng;

int                 NbFree_InputRef;
int                 NbUsed_InputRef;
PtrEnteringReferences PtFree_InputRef;

int                 NbFree_UpdateRefFile;
int                 NbUsed_UpdateRefFile;
PtrReferenceChange  PtFree_UpdateRefFile;

int                 NbFree_AbsBox;
int                 NbUsed_AbsBox;
PtrAbstractBox      PtFree_AbsBox;

int                 NbFree_Document;
int                 NbUsed_Document;
PtrDocument         PtFree_Document;

int                 NbFree_SchPres;
int                 NbUsed_SchPres;
PtrPSchema          PtFree_SPres;

int                 NbFree_HandleSchPres;
int                 NbUsed_HandleSchPres;
PtrHandlePSchema    PtFree_HandleSchPres;

int                 NbFree_SchStruct;
int                 NbUsed_SchStruct;
PtrSSchema          PtFree_SStruct;

int                 NbFree_ExtenBlock;
int                 NbUsed_ExtenBlock;
PtrExtensBlock      PtFree_ExtenBlock;

int                 NbFree_PresRule;
int                 NbUsed_PresRule;
PtrPRule            PtFree_PresRule;

int                 NbFree_CondPresRule;
int                 NbUsed_CondPresRule;
PtrCondition        PtFree_CondPresRule;

int                 NbFree_DelayR;
int                 NbUsed_DelayR;
PtrDelayedPRule     PtFree_DelayR;

PtrBox              PtFreBox;
int                 NbFree_Box;
int                 NbUsed_Box;
PtrPosRelations     PtFrePosB;
int                 NbFree_PosB;
int                 NbUsed_PosB;
PtrDimRelations     PtFreBDim;
int                 NbFree_BDim;
int                 NbUsed_BDim;
PtrLine             PtFree_Line;
int                 NbFree_Line;
int                 NbUsed_Line;

int                 NbFree_Dict;
int                 NbUsed_Dict;
PtrDict             PtFree_Dict;

#include "memory_f.h"
#include "fileaccess_f.h"

/*----------------------------------------------------------------------
   TtaGetMemory

   Allocates dynamically a buffer of a specific size

   Parameters:
   size: size in bytes (like using malloc) of the desired allocated
   buffer.

   Return value:
   the address of the allocated buffer.

   See also:
   TtaFreeMemory.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               *TtaGetMemory (unsigned int n)
#else  /* __STDC__ */
void               *TtaGetMemory (n)
unsigned int        n;
#endif /* __STDC__ */

{
   void               *res;

   if (n == 0)
      n++;
   res = malloc ((size_t) n);
   if (!res)
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
   return (res);
}


/*----------------------------------------------------------------------
   TtaFreeMemory

   Frees the memory allocated to a buffer by TtaGetMemory

   Parameters:
   buffer: the address of the buffer

   See also:
   TtaGetMemory.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaFreeMemory (void *ptr)
#else  /* __STDC__ */
void                TtaFreeMemory (ptr)
void               *ptr;

#endif /* __STDC__ */

{
   if (ptr)
      free (ptr);
}


/*----------------------------------------------------------------------
   TtaStrdup

   Copy the string given in argument to a newly allocated piece of memory.

   Parameters:
   str: a zero terminated string.

   See also:
   TtaGetMemory.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
char               *TtaStrdup (char *str)

#else  /* __STDC__ */
char               *TtaStrdup (str)
char               *str;

#endif /* __STDC__ */

{
   char               *res;

   if (str == NULL)
      return (NULL);
   res = (char *) TtaGetMemory (strlen (str) + 1);
   if (res == NULL)
      return (res);
   strcpy (res, str);
   return (res);
}

/*----------------------------------------------------------------------
   ThotReAlloc effectue un realloc blinde'.                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void               *TtaRealloc (void *ptr, unsigned int n)

#else  /* __STDC__ */
void               *TtaRealloc (ptr, n)
void               *ptr;
unsigned int        n;

#endif /* __STDC__ */

{
   void               *res;

   if (n == 0)
      n++;
   res = realloc (ptr, (size_t) n);
   if (!res)			/* Plus de memoire */
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
   return res;
}

/*----------------------------------------------------------------------
   InitEditorMemory effectue l'initialisation des variables de gestion de 
   liste.                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitEditorMemory ()
#else  /* __STDC__ */
void                InitEditorMemory ()
#endif /* __STDC__ */
{

   NbFree_TextBuff = 0;
   NbUsed_TextBuff = 0;
   PtFree_TextBuff = NULL;

   NbFree_Element = 0;
   NbUsed_Element = 0;
   PtFree_Element = NULL;

   NbFree_Attr = 0;
   NbUsed_Attr = 0;
   PtFree_Attr = NULL;

   NbFree_DescRef = 0;
   NbUsed_DescRef = 0;
   PtFree_DescRef = NULL;

   NbFree_DescCopy = 0;
   NbUsed_DescCopy = 0;
   PtFree_DescCopy = NULL;

   NbFree_ExternalDoc = 0;
   NbUsed_ExternalDoc = 0;
   PtFree_ExternalDoc = NULL;

   NbFree_Reference = 0;
   NbUsed_Reference = 0;
   PtFree_Reference = NULL;

   NbFree_OutputRef = 0;
   NbUsed_OutputRef = 0;
   PtFree_OutputRef = NULL;

   NbFree_ElemRefChng = 0;
   NbUsed_ElemRefChng = 0;
   PtFree_ElemRefChng = NULL;

   NbFree_InputRef = 0;
   NbUsed_InputRef = 0;
   PtFree_InputRef = NULL;

   NbFree_UpdateRefFile = 0;
   NbUsed_UpdateRefFile = 0;
   PtFree_UpdateRefFile = NULL;

   NbFree_AbsBox = 0;
   NbUsed_AbsBox = 0;
   PtFree_AbsBox = NULL;

   NbFree_Document = 0;
   NbUsed_Document = 0;
   PtFree_Document = NULL;

   NbFree_SchPres = 0;
   NbUsed_SchPres = 0;
   PtFree_SPres = NULL;

   NbFree_HandleSchPres = 0;
   NbUsed_HandleSchPres = 0;
   PtFree_HandleSchPres = NULL;

   NbFree_SchStruct = 0;
   NbUsed_SchStruct = 0;
   PtFree_SStruct = NULL;

   NbFree_ExtenBlock = 0;
   NbUsed_ExtenBlock = 0;
   PtFree_ExtenBlock = NULL;

   NbFree_PresRule = 0;
   NbUsed_PresRule = 0;
   PtFree_PresRule = NULL;

   NbFree_CondPresRule = 0;
   NbUsed_CondPresRule = 0;
   PtFree_CondPresRule = NULL;

   NbFree_DelayR = 0;
   NbUsed_DelayR = 0;
   PtFree_DelayR = NULL;

   NbFree_Dict = 0;
   NbUsed_Dict = 0;
   PtFree_Dict = NULL;
}

/*----------------------------------------------------------------------
   GetTextBuffer alloue un buffer de texte.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetTextBuffer (PtrTextBuffer * pBT)
#else  /* __STDC__ */
void                GetTextBuffer (pBT)
PtrTextBuffer      *pBT;

#endif /* __STDC__ */
{
   PtrTextBuffer       pBuf;

   if (PtFree_TextBuff == NULL)
     {
	/* pas de buffer dans la chaine des libres, acquiert un nouveau buffer */
	pBuf = (PtrTextBuffer) TtaGetMemory (sizeof (TextBuffer));
     }
   else
     {
	/* recupere un buffer en tete de la chaine des libres */
	pBuf = PtFree_TextBuff;
	PtFree_TextBuff = pBuf->BuNext;
	NbFree_TextBuff--;
     }
   /* initialise le buffer */
   *pBT = pBuf;
   if (pBuf)
     {
       memset (pBuf, 0, sizeof (TextBuffer));
       pBuf->BuNext = NULL;
       pBuf->BuPrevious = NULL;
       pBuf->BuLength = 0;
       pBuf->BuContent[0] = '\0';
       NbUsed_TextBuff++;
     }
}

/*----------------------------------------------------------------------
   FreeTextBuffer libere un buffer de texte.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeTextBuffer (PtrTextBuffer pBT)
#else  /* __STDC__ */
void                FreeTextBuffer (pBT)
PtrTextBuffer       pBT;

#endif /* __STDC__ */
{

   /* insere le buffer en tete de la chaine des libres */
   if (pBT != NULL)
     {
	pBT->BuNext = PtFree_TextBuff;
	PtFree_TextBuff = pBT;
	NbFree_TextBuff++;
	NbUsed_TextBuff--;
     }
}

/*----------------------------------------------------------------------
   GetElement alloue un element.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetElement (PtrElement * pEl)
#else  /* __STDC__ */
void                GetElement (pEl)
PtrElement         *pEl;

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          pNewEl;


   if (PtFree_Element == NULL)
      pNewEl = (PtrElement) TtaGetMemory (sizeof (ElementDescr));
   else
     {
	pNewEl = PtFree_Element;
	PtFree_Element = pNewEl->ElNext;
	NbFree_Element--;
     }
   *pEl = pNewEl;
   if (pNewEl)
     {
       memset (pNewEl, 0, sizeof (ElementDescr));
       pNewEl->ElStructSchema = NULL;
       pNewEl->ElTypeNumber = 0;
       pNewEl->ElAssocNum = 0;
       pNewEl->ElFirstAttr = NULL;
       pNewEl->ElVolume = 0;
       pNewEl->ElCopyDescr = NULL;
       pNewEl->ElFirstPRule = NULL;
       pNewEl->ElComment = NULL;
       for (i = 1; i <= MAX_VIEW_DOC; i++)
	 pNewEl->ElAbstractBox[i - 1] = NULL;
       pNewEl->ElParent = NULL;
       pNewEl->ElPrevious = NULL;
       pNewEl->ElNext = NULL;
       pNewEl->ElReferredDescr = NULL;
       pNewEl->ElIsCopy = FALSE;
       pNewEl->ElSource = NULL;
       pNewEl->ElHolophrast = FALSE;
       pNewEl->ElAccess = AccessInherited;
       pNewEl->ElTransContent = FALSE;
       pNewEl->ElTransAttr = FALSE;
       pNewEl->ElTransPres = FALSE;
       pNewEl->ElTerminal = FALSE;
       pNewEl->ElFirstChild = NULL;
       NbUsed_Element++;
     }
}

/*----------------------------------------------------------------------
   FreeElement libere un element.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeElement (PtrElement pEl)
#else  /* __STDC__ */
void                FreeElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{

   if (pEl->ElLeafType == LtText && pEl->ElText != NULL)
     {
       FreeTextBuffer (pEl->ElText);
       pEl->ElText = NULL;
     }
   pEl->ElNext = PtFree_Element;
   PtFree_Element = pEl;
   NbFree_Element++;
   NbUsed_Element--;
}

/*----------------------------------------------------------------------
   GetAttribute alloue un attribut.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetAttribute (PtrAttribute * pAttr)
#else  /* __STDC__ */
void                GetAttribute (pAttr)
PtrAttribute       *pAttr;

#endif /* __STDC__ */
{
   PtrAttribute        pNewAttr;


   if (PtFree_Attr == NULL)
      pNewAttr = (PtrAttribute) TtaGetMemory (sizeof (TtAttribute));
   else
     {
	pNewAttr = PtFree_Attr;
	PtFree_Attr = pNewAttr->AeNext;
	NbFree_Attr--;
     }
   *pAttr = pNewAttr;
   if (pNewAttr)
     {
       memset (pNewAttr, 0, sizeof (TtAttribute));
       pNewAttr->AeNext = NULL;
       pNewAttr->AeAttrSSchema = NULL;
       pNewAttr->AeAttrNum = 0;
       pNewAttr->AeAttrType = AtEnumAttr;
       pNewAttr->AeAttrValue = 0;
       pNewAttr->AeDefAttr = FALSE;
       NbUsed_Attr++;
     }
}

/*----------------------------------------------------------------------
   FreeAttribute libere un attribut.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeAttribute (PtrAttribute pAttr)
#else  /* __STDC__ */
void                FreeAttribute (pAttr)
PtrAttribute        pAttr;

#endif /* __STDC__ */
{

   pAttr->AeNext = PtFree_Attr;
   PtFree_Attr = pAttr;
   NbFree_Attr++;
   NbUsed_Attr--;
}

/*----------------------------------------------------------------------
   GetReferredDescr alloue un descripteur de reference.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetReferredDescr (PtrReferredDescr * pDR)
#else  /* __STDC__ */
void                GetReferredDescr (pDR)
PtrReferredDescr   *pDR;
#endif /* __STDC__ */

{
   PtrReferredDescr    pNewDR;

   if (PtFree_DescRef == NULL)
      pNewDR = (PtrReferredDescr) TtaGetMemory (sizeof (ReferredElemDescriptor));
   else
     {
	pNewDR = PtFree_DescRef;
	PtFree_DescRef = pNewDR->ReNext;
	NbFree_DescRef--;
     }
   *pDR = pNewDR;
   if (pNewDR)
     {
       memset (pNewDR, 0, sizeof (ReferredElemDescriptor));
       pNewDR->ReFirstReference = NULL;
       pNewDR->ReExtDocRef = NULL;
       pNewDR->RePrevious = NULL;
       pNewDR->ReNext = NULL;
       pNewDR->ReReferredLabel[0] = '\0';
       pNewDR->ReExternalRef = FALSE;
       pNewDR->ReReferredElem = NULL;
       NbUsed_DescRef++;
     }
}

/*----------------------------------------------------------------------
   FreeReferredDescr libere un descripteur de reference.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeReferredDescr (PtrReferredDescr pDR)
#else  /* __STDC__ */
void                FreeReferredDescr (pDR)
PtrReferredDescr    pDR;
#endif /* __STDC__ */

{

   pDR->ReNext = PtFree_DescRef;
   PtFree_DescRef = pDR;
   NbFree_DescRef++;
   NbUsed_DescRef--;
}

/*----------------------------------------------------------------------
   GetDescCopy alloue un descripteur de copie.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDescCopy (PtrCopyDescr * pDC)
#else  /* __STDC__ */
void                GetDescCopy (pDC)
PtrCopyDescr       *pDC;
#endif /* __STDC__ */

{
   PtrCopyDescr        pNewDC;

   if (PtFree_DescCopy == NULL)
      pNewDC = (PtrCopyDescr) TtaGetMemory (sizeof (CopyDescriptor));
   else
     {
	pNewDC = PtFree_DescCopy;
	PtFree_DescCopy = pNewDC->CdNext;
	NbFree_DescCopy--;
     }
   *pDC = pNewDC;
   if (pNewDC)
     {
       memset (pNewDC, 0, sizeof (CopyDescriptor));
       pNewDC->CdCopiedAb = NULL;
       pNewDC->CdCopiedElem = NULL;
       pNewDC->CdCopyRule = NULL;
       pNewDC->CdPrevious = NULL;
       pNewDC->CdNext = NULL;
       NbUsed_DescCopy++;
     }
}

/*----------------------------------------------------------------------
   FreeDescCopy libere un descripteur de copie.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDescCopy (PtrCopyDescr pDC)

#else  /* __STDC__ */
void                FreeDescCopy (pDC)
PtrCopyDescr        pDC;

#endif /* __STDC__ */

{
   pDC->CdNext = PtFree_DescCopy;
   PtFree_DescCopy = pDC;
   NbFree_DescCopy++;
   NbUsed_DescCopy--;
}

/*----------------------------------------------------------------------
   GetExternalDoc alloue un descripteur de document externe.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetExternalDoc (PtrExternalDoc * pDE)
#else  /* __STDC__ */
void                GetExternalDoc (pDE)
PtrExternalDoc     *pDE;
#endif /* __STDC__ */

{
   PtrExternalDoc      pNewDE;


   if (PtFree_ExternalDoc == NULL)
      pNewDE = (PtrExternalDoc) TtaGetMemory (sizeof (ExternalDoc));
   else
     {
	pNewDE = PtFree_ExternalDoc;
	PtFree_ExternalDoc = pNewDE->EdNext;
	NbFree_ExternalDoc--;
     }
   *pDE = pNewDE;
   if (pNewDE)
     {
       memset (pNewDE, 0, sizeof (ExternalDoc));
       pNewDE->EdNext = NULL;
       ClearDocIdent (&pNewDE->EdDocIdent);
       NbUsed_ExternalDoc++;
     }
}

/*----------------------------------------------------------------------
   FreeExternalDoc libere un descripteur de document externe.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeExternalDoc (PtrExternalDoc pDE)
#else  /* __STDC__ */
void                FreeExternalDoc (pDE)
PtrExternalDoc      pDE;
#endif /* __STDC__ */

{

   pDE->EdNext = PtFree_ExternalDoc;
   PtFree_ExternalDoc = pDE;
   NbFree_ExternalDoc++;
   NbUsed_ExternalDoc--;
}

/*----------------------------------------------------------------------
   GetReference alloue une reference.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetReference (PtrReference * pRef)
#else  /* __STDC__ */
void                GetReference (pRef)
PtrReference       *pRef;
#endif /* __STDC__ */

{
   PtrReference        pNewRef;

   if (PtFree_Reference == NULL)
      pNewRef = (PtrReference) TtaGetMemory (sizeof (ReferenceDescriptor));
   else
     {
	pNewRef = PtFree_Reference;
	PtFree_Reference = pNewRef->RdNext;
	NbFree_Reference--;
     }
   NbUsed_Reference++;
   *pRef = pNewRef;
   if (pNewRef)
     {
       memset (pNewRef, 0, sizeof (ReferenceDescriptor));
       pNewRef->RdNext = NULL;
       pNewRef->RdPrevious = NULL;
       pNewRef->RdReferred = NULL;
       pNewRef->RdElement = NULL;
       pNewRef->RdAttribute = NULL;
       pNewRef->RdTypeRef = RefFollow;
       pNewRef->RdInternalRef = TRUE;
     }
}

/*----------------------------------------------------------------------
   FreeReference libere une reference.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FreeReference (PtrReference pRef)

#else  /* __STDC__ */
void                FreeReference (pRef)
PtrReference        pRef;

#endif /* __STDC__ */

{

   pRef->RdNext = PtFree_Reference;
   PtFree_Reference = pRef;
   NbFree_Reference++;
   NbUsed_Reference--;
}

/*----------------------------------------------------------------------
   GetOutputRef alloue un descripteur de reference sortante.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetOutputRef (PtrOutReference * pRS)
#else  /* __STDC__ */
void                GetOutputRef (pRS)
PtrOutReference    *pRS;
#endif /* __STDC__ */

{
   PtrOutReference     pNewRS;

   if (PtFree_OutputRef == NULL)
      pNewRS = (PtrOutReference) TtaGetMemory (sizeof (OutReference));
   else
     {
	pNewRS = PtFree_OutputRef;
	PtFree_OutputRef = pNewRS->OrNext;
	NbFree_OutputRef--;
     }
   *pRS = pNewRS;
   if (pNewRS)
     {
       memset (pNewRS, 0, sizeof (OutReference));
       pNewRS->OrNext = NULL;
       pNewRS->OrLabel[0] = '\0';
       ClearDocIdent (&(pNewRS->OrDocIdent));
       NbUsed_OutputRef++;
     }
}

/*----------------------------------------------------------------------
   FreeOutputRef libere un descripteur de reference sortante.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeOutputRef (PtrOutReference pRS)
#else  /* __STDC__ */
void                FreeOutputRef (pRS)
PtrOutReference     pRS;
#endif /* __STDC__ */

{
   pRS->OrNext = PtFree_OutputRef;
   PtFree_OutputRef = pRS;
   NbFree_OutputRef++;
   NbUsed_OutputRef--;
}

/*----------------------------------------------------------------------
   GetChangedReferredEl alloue un descripteur de changement de reference.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetChangedReferredEl (PtrChangedReferredEl * pER)
#else  /* __STDC__ */
void                GetChangedReferredEl (pER)
PtrChangedReferredEl *pER;
#endif /* __STDC__ */

{
   PtrChangedReferredEl pNewER;

   if (PtFree_ElemRefChng == NULL)
      pNewER = (PtrChangedReferredEl) TtaGetMemory (sizeof (ChangedReferredEl));
   else
     {
	pNewER = PtFree_ElemRefChng;
	PtFree_ElemRefChng = pNewER->CrNext;
	NbFree_ElemRefChng--;
     }
   *pER = pNewER;
   if (pNewER)
     {
       memset (pNewER, 0, sizeof (ChangedReferredEl));
       pNewER->CrNext = NULL;
       pNewER->CrOldLabel[0] = '\0';
       pNewER->CrNewLabel[0] = '\0';
       ClearDocIdent (&(pNewER->CrOldDocument));
       ClearDocIdent (&(pNewER->CrNewDocument));
       pNewER->CrReferringDoc = NULL;
       NbUsed_ElemRefChng++;
     }
}

/*----------------------------------------------------------------------
   FreeChangedReferredEl libere un descripteur de changement de reference
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeChangedReferredEl (PtrChangedReferredEl pER)
#else  /* __STDC__ */
void                FreeChangedReferredEl (pER)
PtrChangedReferredEl pER;
#endif /* __STDC__ */

{
   pER->CrNext = PtFree_ElemRefChng;
   PtFree_ElemRefChng = pER;
   NbFree_ElemRefChng++;
   NbUsed_ElemRefChng--;
}

/*----------------------------------------------------------------------
   GetInputRef alloue un descripteur de reference entrante.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetInputRef (PtrEnteringReferences * pRE)
#else  /* __STDC__ */
void                GetInputRef (pRE)
PtrEnteringReferences *pRE;
#endif /* __STDC__ */

{
   PtrEnteringReferences pNewRE;

   if (PtFree_InputRef == NULL)
      pNewRE = (PtrEnteringReferences) TtaGetMemory (sizeof (EnteringReferences));
   else
     {
	pNewRE = PtFree_InputRef;
	PtFree_InputRef = pNewRE->ErNext;
	NbFree_InputRef--;
     }
   *pRE = pNewRE;
   if (pNewRE)
     {
       memset (pNewRE, 0, sizeof (EnteringReferences));
       pNewRE->ErNext = NULL;
       pNewRE->ErFirstReferredEl = NULL;
       ClearDocIdent (&(pNewRE->ErDocIdent));
       pNewRE->ErFileName[0] = '\0';
       NbUsed_InputRef++;
     }
}

/*----------------------------------------------------------------------
   FreeInputRef libere un descripteur de reference entrante.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeInputRef (PtrEnteringReferences pRE)
#else  /* __STDC__ */
void                FreeInputRef (pRE)
PtrEnteringReferences pRE;

#endif /* __STDC__ */

{
   pRE->ErNext = PtFree_InputRef;
   PtFree_InputRef = pRE;
   NbFree_InputRef++;
   NbUsed_InputRef--;
}

/*----------------------------------------------------------------------
   GetFileRefChng alloue un descripteur de changement de reference 
   fichier.                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetFileRefChng (PtrReferenceChange * pFRC)
#else  /* __STDC__ */
void                GetFileRefChng (pFRC)
PtrReferenceChange *pFRC;
#endif /* __STDC__ */

{
   PtrReferenceChange  pNewFRC;

   if (PtFree_UpdateRefFile == NULL)
      pNewFRC = (PtrReferenceChange) TtaGetMemory (sizeof (ReferenceChange));
   else
     {
	pNewFRC = PtFree_UpdateRefFile;
	PtFree_UpdateRefFile = pNewFRC->RcNext;
	NbFree_UpdateRefFile--;
     }
   *pFRC = pNewFRC;
   if (pNewFRC)
     {
       memset (pNewFRC, 0, sizeof (ReferenceChange));
       NbUsed_UpdateRefFile++;
     }
}

/*----------------------------------------------------------------------
   FreeFileRefChng libere un descripteur de changement de reference
   fichier                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeFileRefChng (PtrReferenceChange pFRC)
#else  /* __STDC__ */
void                FreeFileRefChng (pFRC)
PtrReferenceChange  pFRC;
#endif /* __STDC__ */

{
   pFRC->RcFirstChange = NULL;
   pFRC->RcNext = PtFree_UpdateRefFile;
   PtFree_UpdateRefFile = pFRC;
   NbFree_UpdateRefFile++;
   NbUsed_UpdateRefFile--;
}

/*----------------------------------------------------------------------
   GetAbstractBox alloue un pave.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetAbstractBox (PtrAbstractBox *pAb)
#else  /* __STDC__ */
void                GetAbstractBox (pAb)
PtrAbstractBox     *pAb;
#endif /* __STDC__ */
{
   PtrAbstractBox      pNewAb;

   if (PtFree_AbsBox == NULL)
      pNewAb = (PtrAbstractBox) TtaGetMemory (sizeof (AbstractBox));
   else
     {
	pNewAb = PtFree_AbsBox;
	PtFree_AbsBox = pNewAb->AbNext;
	NbFree_AbsBox--;
     }
   NbUsed_AbsBox++;
   *pAb = pNewAb;
   if (pNewAb)
     {
       memset (pNewAb, 0, sizeof (AbstractBox));
       pNewAb->AbElement = NULL;
       pNewAb->AbEnclosing = NULL;
       pNewAb->AbPrevious = NULL;
       pNewAb->AbNext = NULL;
       pNewAb->AbFirstEnclosed = NULL;
       pNewAb->AbVolume = 0;
       pNewAb->AbNextRepeated = NULL;
       pNewAb->AbPreviousRepeated = NULL;
       pNewAb->AbCopyDescr = NULL;
       pNewAb->AbCreatorAttr = NULL;
     }
}

/*----------------------------------------------------------------------
   FreeAbstractBox libere un pave.                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeAbstractBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
void                FreeAbstractBox (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{

   if (pAb->AbLeafType == LtPicture)
     {
	/* FreePicture(pAb->AbBox, pAb->AbBox->BxPictInfo ); */
     }
   pAb->AbElement = NULL;
   pAb->AbEnclosing = NULL;
   pAb->AbPrevious = NULL;
   pAb->AbFirstEnclosed = NULL;
   pAb->AbNextRepeated = NULL;
   pAb->AbPreviousRepeated = NULL;
   pAb->AbNext = PtFree_AbsBox;
   PtFree_AbsBox = pAb;
   NbFree_AbsBox++;
   NbUsed_AbsBox--;
}

/*----------------------------------------------------------------------
   GetDocument alloue un descripteur de document.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDocument (PtrDocument * pDoc)
#else  /* __STDC__ */
void                GetDocument (pDoc)
PtrDocument        *pDoc;
#endif /* __STDC__ */
{
   PtrDocument         pNewDoc;

   if (PtFree_Document == NULL)
      pNewDoc = (PtrDocument) TtaGetMemory (sizeof (DocumentDescr));
   else
     {
	pNewDoc = PtFree_Document;
	PtFree_Document = pNewDoc->DocNext;
	NbFree_Document--;
     }
   /* initialise le contexte de document */
   *pDoc = pNewDoc;
   if (pNewDoc)
     {
       memset (pNewDoc, 0, sizeof (DocumentDescr));
       /* cree et initialise un descripteur bidon de reference, debut */
       /* de la chaine des descripteurs de references du document */
       GetReferredDescr (&pNewDoc->DocReferredEl);
       pNewDoc->DocLabelExpMax = 1;
       NbUsed_Document++;
     }
}

/*----------------------------------------------------------------------
   FreeDocument libere un descripteur de document.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDocument (PtrDocument pDoc)
#else  /* __STDC__ */
void                FreeDocument (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   int                 i;

   pDoc->DocComment = NULL;
   pDoc->DocSSchema = NULL;
   pDoc->DocRootElement = NULL;
   for (i = 0; i < MAX_ASSOC_DOC; i++)
     {
       pDoc->DocAssocRoot[i] = NULL;
       pDoc->DocAssocSubTree[i] = NULL;
       pDoc->DocAssocModifiedAb[i] = NULL;
     }
   for (i = 0; i < MAX_PARAM_DOC; i++)
     pDoc->DocParameters[i] = NULL;
   for (i = 0; i < MAX_VIEW_DOC; i++)
     {
       pDoc->DocView[i].DvSSchema = NULL;
       pDoc->DocViewRootAb[i] = NULL;
       pDoc->DocViewSubTree[i] = NULL;
       pDoc->DocViewModifiedAb[i] = NULL;
     }
   pDoc->DocNewOutRef = NULL;
   pDoc->DocDeadOutRef = NULL;
   pDoc->DocChangedReferredEl = NULL;
   pDoc->DocLabels = NULL;

   pDoc->DocNext = PtFree_Document;
   PtFree_Document = pDoc;
   NbFree_Document++;
   NbUsed_Document--;
}

/*----------------------------------------------------------------------
   GetAttributePres allocates a attribute presentation.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetAttributePres (AttributePres **pAP, int n)
#else  /* __STDC__ */
void                GetAttributePres (pAP, n)
AttributePres     **pAP;
int                 n;
#endif /* __STDC__ */

{
  AttributePres          *pNewAP;
  pNewAP = (AttributePres *) TtaGetMemory (n * sizeof (AttributePres));
  if (pNewAP)
    memset (pNewAP, 0, sizeof (AttributePres));
  *pAP = pNewAP;
}

/*----------------------------------------------------------------------
   FreeAttributePres frees a attribute presentation.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeAttributePres (AttributePres *pAP)
#else  /* __STDC__ */
void                FreeAttributePres (pAP)
AttributePres      *pAP;
#endif /* __STDC__ */

{
  if (pAP)
    {
      pAP->ApNextAttrPres = NULL;
      TtaFreeMemory (pAP);
    }
}


/*----------------------------------------------------------------------
   GetSchPres alloue un schema de presentation.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetSchPres (PtrPSchema * pSP)
#else  /* __STDC__ */
void                GetSchPres (pSP)
PtrPSchema         *pSP;
#endif /* __STDC__ */

{
   PtrPSchema          pNewSP;
   int                 i;

   if (PtFree_SPres == NULL)
      pNewSP = (PtrPSchema) TtaGetMemory (sizeof (PresentSchema));
   else
     {
	pNewSP = PtFree_SPres;
	PtFree_SPres = pNewSP->PsNext;
	NbFree_SchPres--;
     }
   *pSP = pNewSP;
   if (pNewSP)
     {
       memset (pNewSP, 0, sizeof (PresentSchema));
       for (i = 0; i < MAX_RULES_SSCHEMA; i++)
	 {
	   pNewSP->PsAcceptPageBreak[i] = TRUE;
	   pNewSP->PsAcceptLineBreak[i] = TRUE;
	 }
       NbUsed_SchPres++;
     }
}

/*----------------------------------------------------------------------
   FreeSchPres libere un schemas de presentation.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeSchPres (PtrPSchema pSP)
#else  /* __STDC__ */
void                FreeSchPres (pSP)
PtrPSchema          pSP;
#endif /* __STDC__ */

{
  int                 i;

  pSP->PsNext = NULL;
  pSP->PsFirstDefaultPRule = NULL;
  for (i = 0; i < MAX_PRES_BOX; i++)
    {
      pSP->PsPresentBox[i].PbFirstPRule = NULL;
    }
  for (i = 0; i < MAX_ATTR_SSCHEMA; i++)
    {
      FreeAttributePres ( pSP->PsAttrPRule[i]);
      pSP->PsAttrPRule[i] = NULL;
      pSP->PsComparAttr[i] = NULL;
    }
  for (i = 0; i < MAX_RULES_SSCHEMA; i++)
    {
      pSP->PsElemPRule[i] = NULL;
      pSP->PsInheritedAttr[i] = NULL;
    }
  
   pSP->PsNext = PtFree_SPres;
   PtFree_SPres = pSP;
   NbFree_SchPres++;
   NbUsed_SchPres--;
}

/*----------------------------------------------------------------------
   GetHandleSchPres alloue un element de chainage de schemas de    
   presentation.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetHandleSchPres (PtrHandlePSchema * pHSP)
#else  /* __STDC__ */
void                GetHandleSchPres (pHSP)
PtrHandlePSchema   *pHSP;
#endif /* __STDC__ */

{
  PtrHandlePSchema  pNewHSP;

   if (PtFree_HandleSchPres == NULL)
      pNewHSP = (PtrHandlePSchema) TtaGetMemory (sizeof (HandlePSchema));
   else
     {
	pNewHSP = PtFree_HandleSchPres;
	PtFree_HandleSchPres = pNewHSP->HdNextPSchema;
	NbFree_HandleSchPres--;
     }
   *pHSP = pNewHSP;
   if (pNewHSP)
     {
       memset (pNewHSP, 0, sizeof (HandlePSchema));
       pNewHSP->HdPSchema = NULL;
       pNewHSP->HdNextPSchema = NULL;
       pNewHSP->HdPrevPSchema = NULL;
       NbUsed_HandleSchPres++;
     }
}

/*----------------------------------------------------------------------
   FreeHandleSchPres libere un element de chainage de schemas de   
   presentation.                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeHandleSchPres (PtrHandlePSchema pHSP)
#else  /* __STDC__ */
void                FreeHandleSchPres (pHSP)
PtrHandlePSchema    pHSP;
#endif /* __STDC__ */

{
   pHSP->HdPrevPSchema = NULL;
   pHSP->HdPSchema = NULL;
   pHSP->HdNextPSchema = PtFree_HandleSchPres;
   PtFree_HandleSchPres = pHSP;
   NbFree_HandleSchPres++;
   NbUsed_HandleSchPres--;
}


/*----------------------------------------------------------------------
   GetTRule allocates a translation rule
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetTRule (PtrTRule *pR)
#else  /* __STDC__ */
void                GetTRule (pR)
PtrTRule           *pR;
#endif /* __STDC__ */
{
  PtrTRule    pNewR;

  pNewR = (PtrTRule) TtaGetMemory (sizeof (TranslRule));
  *pR = pNewR;
  if (pNewR)
    memset (pNewR, 0, sizeof (TranslRule));
}


/*----------------------------------------------------------------------
   FreeTRule frees a translation rule
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeTRule (PtrTRule pR)
#else  /* __STDC__ */
void                FreeTRule (pR)
PtrTRule            pR;
#endif /* __STDC__ */
{
  if (pR)
    TtaFreeMemory (pR);
}


/*----------------------------------------------------------------------
   GetSchTra allocates a translation schema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetSchTra (PtrTSchema *pST)
#else  /* __STDC__ */
void                GetSchTra (pST)
PtrTSchema           *pST;
#endif /* __STDC__ */
{
  PtrTSchema    pNewST;

  pNewST = (PtrTSchema) TtaGetMemory (sizeof (TranslSchema));
  *pST = pNewST;
  if (pNewST)
    memset (pNewST, 0, sizeof (TranslSchema));
}


/*----------------------------------------------------------------------
   FreeTRule frees a translation schema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeSchTra (PtrTSchema pST)
#else  /* __STDC__ */
void                FreeSchTra (pST)
PtrTSchema          pST;
#endif /* __STDC__ */
{
  int     i;

  if (pST)
    {
      for (i = 0; i < MAX_RULES_SSCHEMA; i++)
	pST->TsElemTRule[i] = NULL;
      TtaFreeMemory (pST);
    }
}


/*----------------------------------------------------------------------
   GetExternalBlock alloue un bloc d'extension pour un schema de   
   structure.                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetExternalBlock (PtrExtensBlock * pBE)
#else  /* __STDC__ */
void                GetExternalBlock (pBE)
PtrExtensBlock     *pBE;
#endif /* __STDC__ */

{
  PtrExtensBlock    pNewBE;

   if (PtFree_ExtenBlock == NULL)
      pNewBE = (PtrExtensBlock) TtaGetMemory (sizeof (ExtensBlock));
   else
     {
	pNewBE = PtFree_ExtenBlock;
	PtFree_ExtenBlock = pNewBE->EbNextBlock;
	NbFree_ExtenBlock--;
     }
   *pBE = pNewBE;
   if (pNewBE)
     {
       memset (pNewBE, 0, sizeof (ExtensBlock));
       pNewBE->EbNextBlock = NULL;
       NbUsed_ExtenBlock++;
     }
}

/*----------------------------------------------------------------------
   FreeExternalBlock libere un bloc extension de schema de structure. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeExternalBlock (PtrExtensBlock pBE)
#else  /* __STDC__ */
void                FreeExternalBlock (pBE)
PtrExtensBlock      pBE;
#endif /* __STDC__ */

{
   pBE->EbNextBlock = PtFree_ExtenBlock;
   PtFree_ExtenBlock = pBE;
   NbFree_ExtenBlock++;
   NbUsed_ExtenBlock--;
}

/*----------------------------------------------------------------------
   GetSchStruct alloue un schema de structure.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetSchStruct (PtrSSchema * pSS)
#else  /* __STDC__ */
void                GetSchStruct (pSS)
PtrSSchema         *pSS;
#endif /* __STDC__ */

{
  PtrSSchema    pNewSS;

  if (PtFree_SStruct == NULL)
    pNewSS = (PtrSSchema) TtaGetMemory (sizeof (StructSchema));
  else
    {
      pNewSS = PtFree_SStruct;
      PtFree_SStruct = pNewSS->SsNextExtens;
      NbFree_SchStruct--;
    }
  *pSS = pNewSS;
  if (pNewSS)
    {
      memset (pNewSS, 0, sizeof (StructSchema));
      NbUsed_SchStruct++;
    }
}

/*----------------------------------------------------------------------
   FreeSchStruc libere un schema de structure.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeSchStruc (PtrSSchema pSS)
#else  /* __STDC__ */
void                FreeSchStruc (pSS)
PtrSSchema          pSS;
#endif /* __STDC__ */

{
  PtrHandlePSchema  pHSP, pNewHSP;

  /* free HandleSchPres */
  if (pSS->SsFirstPSchemaExtens != NULL)
    {
      pHSP = pSS->SsFirstPSchemaExtens;
      while (pHSP != NULL)
	{
	  pNewHSP = pHSP->HdNextPSchema;
	  FreeHandleSchPres (pHSP);
	  pHSP = pNewHSP;
	}
      pSS->SsFirstPSchemaExtens = NULL;
    }
   if (pSS->SsExtensBlock != NULL)
     {
       FreeExternalBlock (pSS->SsExtensBlock);
       pSS->SsExtensBlock = NULL;
     }
   pSS->SsPrevExtens = NULL;
   pSS->SsExtension = FALSE;
   pSS->SsNExtensRules = 0;
   pSS->SsExtensBlock = NULL;
   pSS->SsFirstPSchemaExtens = NULL;
   pSS->SsNextExtens = PtFree_SStruct;

   PtFree_SStruct = pSS;
   NbFree_SchStruct++;
   NbUsed_SchStruct--;
}

/*----------------------------------------------------------------------
   GetPresentRule alloue une regle de presentation.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPresentRule (PtrPRule * pRP)
#else  /* __STDC__ */
void                GetPresentRule (pRP)
PtrPRule           *pRP;
#endif /* __STDC__ */

{
  PtrPRule    pNewRP;

  if (PtFree_PresRule == NULL)
    pNewRP = (PtrPRule) TtaGetMemory (sizeof (PresRule));
  else
    {
      pNewRP = PtFree_PresRule;
      PtFree_PresRule = pNewRP->PrNextPRule;
      NbFree_PresRule--;
    }
  *pRP = pNewRP;
  if (pNewRP)
    {
      memset (pNewRP, 0, sizeof (PresRule));
      pNewRP->PrNextPRule = NULL;
      pNewRP->PrSpecifAttr = 0;
      pNewRP->PrSpecifAttrSSchema = NULL;
      pNewRP->PrCond = NULL;
      NbUsed_PresRule++;
    }
}

/*----------------------------------------------------------------------
   FreePresentRule libere une regle de presentation.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePresentRule (PtrPRule pRP)
#else  /* __STDC__ */
void                FreePresentRule (pRP)
PtrPRule            pRP;
#endif /* __STDC__ */

{
   PtrCondition        pCond, nextCond;

   pCond = pRP->PrCond;
   while (pCond != NULL)
     {
	nextCond = pCond->CoNextCondition;
	FreePresentRuleCond (pCond);
	pCond = nextCond;
     }
   pRP->PrNextPRule = PtFree_PresRule;
   PtFree_PresRule = pRP;
   NbFree_PresRule++;
   NbUsed_PresRule--;
}

/*----------------------------------------------------------------------
   GetPresentRuleCond alloue une condition pour une regle de       
   presentation.                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPresentRuleCond (PtrCondition * pCond)
#else  /* __STDC__ */
void                GetPresentRuleCond (pCond)
PtrCondition       *pCond;
#endif /* __STDC__ */

{
  PtrCondition   pNewCond;

  if (PtFree_CondPresRule == NULL)
    pNewCond = (PtrCondition) TtaGetMemory (sizeof (Condition));
  else
    {
      pNewCond = PtFree_CondPresRule;
      PtFree_CondPresRule = pNewCond->CoNextCondition;
      NbFree_CondPresRule--;
    }
  *pCond = pNewCond;
  if (pNewCond)
    {
      memset (pNewCond, 0, sizeof (Condition));
      pNewCond->CoNextCondition = NULL;
      pNewCond->CoNotNegative = FALSE;
      pNewCond->CoTarget = FALSE;
      NbUsed_CondPresRule++;
    }
}

/*----------------------------------------------------------------------
   FreePresentRuleCond libere une condition sur une regle de       
   presentation.                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePresentRuleCond (PtrCondition pCond)
#else  /* __STDC__ */
void                FreePresentRuleCond (pCond)
PtrCondition        pCond;
#endif /* __STDC__ */

{
   pCond->CoNextCondition = PtFree_CondPresRule;
   PtFree_CondPresRule = pCond;
   NbFree_CondPresRule++;
   NbUsed_CondPresRule--;
}

/*----------------------------------------------------------------------
   GetDifferedRule alloue une regle retardee.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDifferedRule (PtrDelayedPRule * pRR)
#else  /* __STDC__ */
void                GetDifferedRule (pRR)
PtrDelayedPRule    *pRR;
#endif /* __STDC__ */

{
  PtrDelayedPRule   pNewRR;

  if (PtFree_DelayR == NULL)
    pNewRR = (PtrDelayedPRule) TtaGetMemory (sizeof (DelayedPRule));
  else
    {
      pNewRR = PtFree_DelayR;
      PtFree_DelayR = pNewRR->DpNext;
      NbFree_DelayR--;
    }
  *pRR = pNewRR;
  if (pNewRR)
    {
      memset (*pRR, 0, sizeof (DelayedPRule));
      pNewRR->DpPRule = NULL;
      pNewRR->DpPSchema = NULL;
      pNewRR->DpAbsBox = NULL;
      pNewRR->DpNext = NULL;
      NbUsed_DelayR++;
    }
}

/*----------------------------------------------------------------------
   FreeDifferedRule libere une regle retardee.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDifferedRule (PtrDelayedPRule pRR)
#else  /* __STDC__ */
void                FreeDifferedRule (pRR)
PtrDelayedPRule     pRR;
#endif /* __STDC__ */

{
   pRR->DpNext = PtFree_DelayR;
   PtFree_DelayR = pRR;
   NbFree_DelayR++;
   NbUsed_DelayR--;
}

/*----------------------------------------------------------------------
   InitKernelMemory initialise l'allocation memoire pour le kernel   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitKernelMemory ()
#else  /* __STDC__ */
void                InitKernelMemory ()
#endif /* __STDC__ */
{
   int                 i;
   ViewFrame          *pFrame;

   /* Toutes les frames sont libres */
   for (i = 1; i <= MAX_FRAME; i++)
     {
	pFrame = &ViewFrameTable[i - 1];
	pFrame->FrAbstractBox = NULL;
	pFrame->FrXOrg = 0;
	pFrame->FrYOrg = 0;
	pFrame->FrClipXBegin = 0;
	pFrame->FrClipXEnd = 0;
	pFrame->FrClipYBegin = 0;
	pFrame->FrClipYEnd = 0;
     }
   /* Aucune boite allouee et liberee */
   PtFreBox = NULL;
   NbFree_Box = 0;
   NbUsed_Box = 0;
   PtFrePosB = NULL;
   NbFree_PosB = 0;
   NbUsed_PosB = 0;
   PtFreBDim = NULL;
   NbFree_BDim = 0;
   NbUsed_BDim = 0;
   /* Aucune ligne allouee et liberee */
   PtFree_Line = NULL;
   NbFree_Line = 0;
   NbUsed_Line = 0;
   FrameUpdating = FALSE;
   TextInserting = FALSE;
}


/*----------------------------------------------------------------------
   GetPosBlock alloue un bloc de relations de position.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPosBlock (PtrPosRelations * pBlock)
#else  /* __STDC__ */
void                GetPosBlock (pBlock)
PtrPosRelations    *pBlock;
#endif /* __STDC__ */

{
   int                 i;
   PtrPosRelations     pNewBlock;

   if (PtFrePosB == NULL)
     pNewBlock  = (PtrPosRelations) TtaGetMemory (sizeof (PosRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
	pNewBlock = PtFrePosB;
	PtFrePosB = pNewBlock->PosRNext;
	NbFree_PosB--;
     }
   /* Initialisation du bloc */
   *pBlock = pNewBlock;
   if (pNewBlock)
     {
       memset (pNewBlock, 0, sizeof (PosRelations));
       pNewBlock->PosRNext = NULL;
       
       for (i = 1; i <= MAX_RELAT_POS; i++)
	 pNewBlock->PosRTable[i - 1].ReBox = NULL;
       NbUsed_PosB++;
     }
}

/*----------------------------------------------------------------------
   FreePosBlock libere le bloc de relations.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePosBlock (PtrPosRelations * pBlock)
#else  /* __STDC__ */
void                FreePosBlock (pBlock)
PtrPosRelations    *pBlock;
#endif /* __STDC__ */

{
   /* Insere le bloc en tete de la chaine des libres */
   (*pBlock)->PosRNext = PtFrePosB;
   PtFrePosB = *pBlock;
   NbFree_PosB++;
   NbUsed_PosB--;
}

/*----------------------------------------------------------------------
   GetDimBlock alloue un bloc de relations de dimension.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDimBlock (PtrDimRelations * pBlock)
#else  /* __STDC__ */
void                GetDimBlock (pBlock)
PtrDimRelations    *pBlock;
#endif /* __STDC__ */

{
   int                 i;
   PtrDimRelations     pNewBlock;

   if (PtFreBDim == NULL)
     pNewBlock = (PtrDimRelations) TtaGetMemory (sizeof (DimRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
       pNewBlock = PtFreBDim;
       PtFreBDim = pNewBlock->DimRNext;
       NbFree_BDim--;
     }
   /* Initialisation du bloc */
   *pBlock = pNewBlock;
   if (pNewBlock)
     {
       memset (pNewBlock, 0, sizeof (DimRelations));
       pNewBlock->DimRNext = NULL;
       
       for (i = 1; i <= MAX_RELAT_DIM; i++)
	 {
	   pNewBlock->DimRTable[i - 1] = NULL;
	   pNewBlock->DimRSame[i - 1] = TRUE;
	 }
       NbUsed_BDim++;
     }
}

/*----------------------------------------------------------------------
   FreeDimBlock libere le bloc de relations de dimension.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDimBlock (PtrDimRelations * pBlock)
#else  /* __STDC__ */
void                FreeDimBlock (pBlock)
PtrDimRelations    *pBlock;
#endif /* __STDC__ */

{

   /* Insere le bloc en tete de la chaine des libres */
   (*pBlock)->DimRNext = PtFreBDim;
   PtFreBDim = *pBlock;
   NbFree_BDim++;
   NbUsed_BDim--;
}

/*----------------------------------------------------------------------
   GetBox Alloue un nouveau contexte de boite pour le pave pAb.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox              GetBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
PtrBox              GetBox (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */

{
   PtrBox              pBox;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   if (PtFreBox == NULL)
      pBox = (PtrBox) TtaGetMemory (sizeof (Box));
   /* Sinon recupere le contexte en tete de la chaine des libres */
   else
     {
	pBox = PtFreBox;
	PtFreBox = pBox->BxNexChild;
	NbFree_Box--;
     }

   /* Initialisation de la box */
   if (pBox != NULL)
     {
	memset (pBox, 0, sizeof (Box));
	pBox->BxAbstractBox = pAb;
	pBox->BxPrevious = NULL;
	pBox->BxNext = NULL;
	pBox->BxType = BoComplete;
	pBox->BxNexChild = NULL;
	pBox->BxXOrg = 0;
	pBox->BxYOrg = 0;
	pBox->BxWidth = 0;
	pBox->BxHeight = 0;
	pBox->BxHorizRef = 0;
	pBox->BxVertRef = 0;
	pBox->BxFont = NULL;
	pBox->BxUnderline = 0;
	pBox->BxThickness = 0;
	pBox->BxMoved = NULL;
	pBox->BxHorizInc = NULL;
	pBox->BxVertInc = NULL;
	pBox->BxNChars = 0;
	pBox->BxHorizEdge = Left;
	pBox->BxVertEdge = Top;
	pBox->BxNSpaces = 0;
	pBox->BxSpaceWidth = 0;
	pBox->BxPosRelations = NULL;
	pBox->BxWidthRelations = NULL;
	pBox->BxHeightRelations = NULL;
	pBox->BxEndOfBloc = 0;
	/* Il n'y a pas de relations hors-structure */
	pBox->BxXOutOfStruct = FALSE;
	pBox->BxYOutOfStruct = FALSE;
	pBox->BxWOutOfStruct = FALSE;
	pBox->BxHOutOfStruct = FALSE;
	/* La box n'est pas elastique */
	pBox->BxHorizFlex = FALSE;
	pBox->BxVertFlex = FALSE;
	pBox->BxHorizInverted = FALSE;
	pBox->BxVertInverted = FALSE;
	pBox->BxNew = FALSE;
	/* Initialisation du traitement des dimensions minimales */
	pBox->BxContentHeight = FALSE;
	pBox->BxContentWidth = FALSE;
	pBox->BxRuleHeigth = 0;
	pBox->BxRuleWidth = 0;
	pBox->BxBuffer = NULL;
	pBox->BxEndOfBloc = 0;
	pBox->BxNPixels = 0;
	pBox->BxFirstChar = 0;
	pBox->BxXToCompute = FALSE;
	pBox->BxYToCompute = FALSE;
	NbUsed_Box++;
     }
   return pBox;
}

/*----------------------------------------------------------------------
   FreeBox libere le contexte de boite pBox et retourne la boite 
   suivante.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox              FreeBox (PtrBox pBox)
#else  /* __STDC__ */
PtrBox              FreeBox (pBox)
PtrBox              pBox;
#endif /* __STDC__ */

{
   PtrPosRelations     pPosRel;
   PtrPosRelations     nepos;
   PtrDimRelations     pDimRel;
   PtrDimRelations     nedim;
   PtrBox              NextBox;

   NextBox = pBox->BxNexChild;
   /* Insere le contexte de boite en tete de la chaine des libres */
   pBox->BxAbstractBox = NULL;
   pBox->BxPrevious = NULL;
   pBox->BxNext = NULL;
   /* free lines for box block of lines */
   if (pBox->BxType == BoBlock)
     {
       pBox->BxFirstLine = NULL;
       pBox->BxLastLine = NULL;
     }
   /* On retire l'indicateur de fin de bloc */
   pBox->BxEndOfBloc = 0;
   pBox->BxType = BoComplete;
   pBox->BxNexChild = PtFreBox;
   PtFreBox = pBox;
   NbFree_Box++;
   NbUsed_Box--;
   /* On libere les differents blocs attaches a la boite */
   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	nepos = pPosRel->PosRNext;
	FreePosBlock (&pPosRel);
	pPosRel = nepos;
     }

   pDimRel = pBox->BxWidthRelations;
   while (pDimRel != NULL)
     {
	nedim = pDimRel->DimRNext;
	FreeDimBlock (&pDimRel);
	pDimRel = nedim;
     }

   pDimRel = pBox->BxHeightRelations;
   while (pDimRel != NULL)
     {
	nedim = pDimRel->DimRNext;
	FreeDimBlock (&pDimRel);
	pDimRel = nedim;
     }
   return NextBox;
}


/*----------------------------------------------------------------------
   GetLine alloue un contexte de ligne.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetLine (PtrLine * pLine)
#else  /* __STDC__ */
void                GetLine (pLine)
PtrLine            *pLine;
#endif /* __STDC__ */

{
   PtrLine             pNewLine;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   if (PtFree_Line == NULL)
      pNewLine = (PtrLine) TtaGetMemory (sizeof (Line));
   /* Sinon recupere le contexte en tete de la chaine des libres */
   else
     {
	pNewLine = PtFree_Line;
	PtFree_Line = pNewLine->LiNext;
	NbFree_Line--;
     }
   /* Initialisation de la ligne */
   *pLine = pNewLine;
   if (pNewLine)
     {
	memset (pNewLine, 0, sizeof (Line));
	pNewLine->LiPrevious = NULL;
	pNewLine->LiNext = NULL;
	pNewLine->LiXOrg = 0;
	pNewLine->LiYOrg = 0;
	pNewLine->LiXMax = 0;
	pNewLine->LiMinLength = 0;
	pNewLine->LiHeight = 0;
	pNewLine->LiHorizRef = 0;
	pNewLine->LiNSpaces = 0;
	pNewLine->LiSpaceWidth = 0;
	pNewLine->LiNPixels = 0;
	pNewLine->LiFirstBox = NULL;
	pNewLine->LiLastBox = NULL;
	pNewLine->LiFirstPiece = NULL;
	pNewLine->LiLastPiece = NULL;
	pNewLine->LiRealLength = 0;
	NbUsed_Line++;
     }
}

/*----------------------------------------------------------------------
   FreeLine libere le contexte de ligne.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeLine (PtrLine pLine)
#else  /* __STDC__ */
void                FreeLine (pLine)
PtrLine             pLine;
#endif /* __STDC__ */

{

   /* Insere le contexte de ligne en tete de la chaine des libres */
   pLine->LiNext = PtFree_Line;
   PtFree_Line = pLine;
   PtFree_Line->LiPrevious = NULL;
   NbFree_Line++;
   NbUsed_Line--;
}


/*----------------------------------------------------------------------
   GetSearchContext alloue un contexte de recherche.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetSearchContext (PtrSearchContext * pSearch)
#else  /* __STDC__ */
void                GetSearchContext (pSearch)
PtrSearchContext   *pSearch;
#endif /* __STDC__ */

{
   PtrSearchContext    s;

   if (pSearch != NULL)
     {
	s = (PtrSearchContext) TtaGetMemory (sizeof (SearchContext));
	/* Initialisation des champs */
	*pSearch = s;
	if (s)
	  {
	    memset (s, 0, sizeof (SearchContext));
	    s->SDocument = NULL;
	    s->STree = 0;
	    s->SStartElement = NULL;
	    s->SStartChar = 0;
	    s->SEndElement = NULL;
	    s->SEndChar = 0;
	    s->SStartToEnd = TRUE;
	    s->SWholeDocument = FALSE;
	  }
     }
}

/*----------------------------------------------------------------------
   FreeSearchContext libe`re un contexte de recherche.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeSearchContext (PtrSearchContext * pSearch)
#else  /* __STDC__ */
void                FreeSearchContext (pSearch)
PtrSearchContext   *pSearch;
#endif /* __STDC__ */

{
   if (pSearch != NULL)
      if (*pSearch != NULL)
	{
	   free (*pSearch);
	   *pSearch = NULL;
	}
}


/*----------------------------------------------------------------------
   FreeStringInDict libere les entre'es d'un dictionnaire.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeStringInDict (PtrDict pDict)
#else  /* __STDC__ */
void                FreeStringInDict (pDict)
PtrDict             pDict;
#endif /* __STDC__ */
{
   PtrChaine           pCh1;
   PtrCommuns          pCommun;
   PtrMots             pMot;
   int                 i;

   pCh1 = pDict->chaine;
   if (pCh1 != NULL)
      free (pCh1);

   pMot = pDict->pdico;
   if (pMot != NULL)
      free (pMot);

   pCommun = pDict->commun;
   if (pCommun != NULL)
      free (pCommun);

   /* maj du contexte du dictionnaire : chaine et mots */
   pDict->nbcars = 0;
   pDict->chaine = NULL;
   pDict->nbmots = -1;
   pDict->pdico = NULL;
   pDict->commun = NULL;
   for (i = 0; i < MAX_WORD_LEN; i++)
      pDict->plgdico[i] = 0;
}

/*----------------------------------------------------------------------
   GetDictContext effectue un malloc controle.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        *GetDictContext (unsigned int n)
#else  /* __STDC__ */
static void        *GetDictContext (n)
unsigned int        n;
#endif /* __STDC__ */
{

   if (n > 0)
      return ((char *) malloc ((size_t) n));
   return (NULL);
}


/*----------------------------------------------------------------------
   GetStringInDict retourne -1 en cas de manque de memoire            
   retourne  0 si OK.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetStringInDict (PtrDict * pDict, boolean readonly)
#else  /* __STDC__ */
int                 GetStringInDict (pDict, readonly)
PtrDict            *pDict;
boolean             readonly;
#endif /* __STDC__ */
{
   PtrDict             pdict;
   unsigned int        i;

   pdict = *pDict;
   pdict->MAXcars += (pdict->DictReadOnly == FALSE) ? 600 : 2;
   i = pdict->MAXcars;
   /* alloue la chaine necessaire */
   pdict->chaine = (PtrChaine) GetDictContext (i);
   if (pdict->chaine == NULL)
     {
	FreeStringInDict (pdict);
	return (-1);
     }

   pdict->MAXmots += (pdict->DictReadOnly == FALSE) ? 50 : 2;
   i = pdict->MAXmots;
   pdict->commun = (PtrCommuns) GetDictContext (i);
   if (pdict->commun == NULL)
     {
	FreeStringInDict (pdict);
	return (-1);
     }

   /* ATTENTION : ce sont des entiers */
   pdict->pdico = (PtrMots) GetDictContext (i * sizeof (int));

   if (pdict->pdico == NULL)
     {
	FreeStringInDict (pdict);
	return (-1);
     }
   return (0);
}


/*----------------------------------------------------------------------
   GetDictionary retourne dans pDict un pointeur vers le dictionaire  
   alloue ou NULL si manque de memoire                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDictionary (PtrDict * pDict)
#else  /* __STDC__ */
void                GetDictionary (pDict)
PtrDict            *pDict;

#endif /* __STDC__ */
{
   int                 i;
   PtrDict             pdict;

   if (PtFree_Dict == NULL)
      *pDict = (PtrDict) GetDictContext (sizeof (Dictionnaire));
   else
     {
	*pDict = PtFree_Dict;
	PtFree_Dict = (*pDict)->DictSuivant;
	NbFree_Dict--;
     }

   if (*pDict != NULL)
     {
	NbUsed_Dict++;

	/* initialise le contexte de dictionnaire */
	pdict = *pDict;
	pdict->DictNom[0] = '\0';
	pdict->DictDirectory[0] = '\0';
	pdict->DictReadOnly = TRUE;
	/* readonly */
	pdict->DictCharge = FALSE;
	/* contenu non charge' */
	pdict->DictModifie = FALSE;
	/* contenu non modifie' */
	pdict->chaine = NULL;
	pdict->pdico = NULL;
	pdict->commun = NULL;

	for (i = 0; i < MAX_WORD_LEN; i++)
	   pdict->plgdico[i] = 0;
	pdict->MAXmots = 0;
	pdict->MAXcars = 0;
	pdict->nbmots = -1;
	pdict->nbcars = 0;
     }
}


/*----------------------------------------------------------------------
   FreeDictionary libere le dictionnaire.                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDictionary (PtrDict pDict)
#else  /* __STDC__ */
void                FreeDictionary (pDict)
PtrDict             pDict;

#endif /* __STDC__ */
{
   pDict->DictSuivant = PtFree_Dict;
   PtFree_Dict = pDict;
   NbFree_Dict++;
   NbUsed_Dict--;

}
