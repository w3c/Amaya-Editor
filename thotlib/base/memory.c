/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   Manage free-lists and memory allocation.
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typecorr.h"
#include "libmsg.h"
#include "message.h"
#include "storage.h"

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

#include "absboxlist_f.h"
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
void               *ThotRealloc (void *ptr, unsigned int n)

#else  /* __STDC__ */
void               *ThotRealloc (ptr, n)
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
#endif				/* __STDC__ */

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
   PtrTextBuffer       pBu1;

   if (PtFree_TextBuff == NULL)
     {
	/* pas de buffer dans la chaine des libres, acquiert un nouveau buffer */
	*pBT = (PtrTextBuffer) TtaGetMemory (sizeof (TextBuffer));
     }
   else
     {
	/* recupere un buffer en tete de la chaine des libres */
	*pBT = PtFree_TextBuff;
	PtFree_TextBuff = (*pBT)->BuNext;
	NbFree_TextBuff--;
     }
   NbUsed_TextBuff++;
   /* initialise le buffer */
   pBu1 = *pBT;
   memset (pBu1, 0, sizeof (TextBuffer));
   pBu1->BuNext = NULL;
   pBu1->BuPrevious = NULL;
   pBu1->BuLength = 0;
   pBu1->BuContent[0] = '\0';
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
   PtrElement          pEl1;


   if (PtFree_Element == NULL)
      *pEl = (PtrElement) TtaGetMemory (sizeof (ElementDescr));
   else
     {
	*pEl = PtFree_Element;
	PtFree_Element = (*pEl)->ElNext;
	NbFree_Element--;
     }
   NbUsed_Element++;
   pEl1 = *pEl;
   memset (pEl1, 0, sizeof (ElementDescr));
   pEl1->ElStructSchema = NULL;
   pEl1->ElTypeNumber = 0;
   pEl1->ElAssocNum = 0;
   pEl1->ElFirstAttr = NULL;
   pEl1->ElVolume = 0;
   pEl1->ElCopyDescr = NULL;
   pEl1->ElFirstPRule = NULL;
   pEl1->ElComment = NULL;
   for (i = 1; i <= MAX_VIEW_DOC; i++)
      pEl1->ElAbstractBox[i - 1] = NULL;
   pEl1->ElParent = NULL;
   pEl1->ElPrevious = NULL;
   pEl1->ElNext = NULL;
   pEl1->ElReferredDescr = NULL;
   pEl1->ElIsCopy = FALSE;
   pEl1->ElSource = NULL;
   pEl1->ElHolophrast = FALSE;
   pEl1->ElAccess = AccessInherited;
   pEl1->ElTransContent = FALSE;
   pEl1->ElTransAttr = FALSE;
   pEl1->ElTransPres = FALSE;
   pEl1->ElTerminal = FALSE;
   pEl1->ElFirstChild = NULL;
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

   pEl->ElNext = PtFree_Element;
   PtFree_Element = pEl;
   NbFree_Element++;
   NbUsed_Element--;
}

/*----------------------------------------------------------------------
   GetAttr alloue un attribut.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetAttr (PtrAttribute * pAttr)
#else  /* __STDC__ */
void                GetAttr (pAttr)
PtrAttribute       *pAttr;

#endif /* __STDC__ */
{
   PtrAttribute        pAt1;


   if (PtFree_Attr == NULL)
      *pAttr = (PtrAttribute) TtaGetMemory (sizeof (TtAttribute));
   else
     {
	*pAttr = PtFree_Attr;
	PtFree_Attr = (*pAttr)->AeNext;
	NbFree_Attr--;
     }
   NbUsed_Attr++;
   pAt1 = *pAttr;
   memset (pAt1, 0, sizeof (TtAttribute));
   pAt1->AeNext = NULL;
   pAt1->AeAttrSSchema = NULL;
   pAt1->AeAttrNum = 0;
   pAt1->AeAttrType = AtEnumAttr;
   pAt1->AeAttrValue = 0;
   pAt1->AeDefAttr = FALSE;
}

/*----------------------------------------------------------------------
   FreeAttr libere un attribut.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeAttr (PtrAttribute pAttr)
#else  /* __STDC__ */
void                FreeAttr (pAttr)
PtrAttribute        pAttr;

#endif /* __STDC__ */
{

   pAttr->AeNext = PtFree_Attr;
   PtFree_Attr = pAttr;
   NbFree_Attr++;
   NbUsed_Attr--;
}

/*----------------------------------------------------------------------
   GetDescReference alloue un descripteur de reference.            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                GetDescReference (PtrReferredDescr * pDR)

#else  /* __STDC__ */
void                GetDescReference (pDR)
PtrReferredDescr   *pDR;

#endif /* __STDC__ */

{
   PtrReferredDescr    pDe1;


   if (PtFree_DescRef == NULL)
      *pDR = (PtrReferredDescr) TtaGetMemory (sizeof (ReferredElemDescriptor));
   else
     {
	*pDR = PtFree_DescRef;
	PtFree_DescRef = (*pDR)->ReNext;
	NbFree_DescRef--;
     }
   NbUsed_DescRef++;
   pDe1 = *pDR;
   memset (pDe1, 0, sizeof (ReferredElemDescriptor));
   pDe1->ReFirstReference = NULL;
   pDe1->ReExtDocRef = NULL;
   pDe1->RePrevious = NULL;
   pDe1->ReNext = NULL;
   pDe1->ReReferredLabel[0] = '\0';
   pDe1->ReExternalRef = FALSE;
   pDe1->ReReferredElem = NULL;
}

/*----------------------------------------------------------------------
   FreeDescReference libere un descripteur de reference.           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FreeDescReference (PtrReferredDescr pDR)

#else  /* __STDC__ */
void                FreeDescReference (pDR)
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
   PtrCopyDescr        pDe1;

   if (PtFree_DescCopy == NULL)
      *pDC = (PtrCopyDescr) TtaGetMemory (sizeof (CopyDescriptor));
   else
     {
	*pDC = PtFree_DescCopy;
	PtFree_DescCopy = (*pDC)->CdNext;
	NbFree_DescCopy--;
     }
   NbUsed_DescCopy++;
   pDe1 = *pDC;
   memset (pDe1, 0, sizeof (CopyDescriptor));
   pDe1->CdCopiedAb = NULL;
   pDe1->CdCopiedElem = NULL;
   pDe1->CdCopyRule = NULL;
   pDe1->CdPrevious = NULL;
   pDe1->CdNext = NULL;
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
   PtrExternalDoc      pDo1;


   if (PtFree_ExternalDoc == NULL)
      *pDE = (PtrExternalDoc) TtaGetMemory (sizeof (ExternalDoc));
   else
     {
	*pDE = PtFree_ExternalDoc;
	PtFree_ExternalDoc = (*pDE)->EdNext;
	NbFree_ExternalDoc--;
     }
   NbUsed_ExternalDoc++;
   pDo1 = *pDE;
   memset (pDo1, 0, sizeof (ExternalDoc));
   pDo1->EdNext = NULL;
   ClearDocIdent (&pDo1->EdDocIdent);
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
   PtrReference        pPR1;

   if (PtFree_Reference == NULL)
      *pRef = (PtrReference) TtaGetMemory (sizeof (ReferenceDescriptor));
   else
     {
	*pRef = PtFree_Reference;
	PtFree_Reference = (*pRef)->RdNext;
	NbFree_Reference--;
     }
   NbUsed_Reference++;
   pPR1 = *pRef;
   memset (pPR1, 0, sizeof (ReferenceDescriptor));
   pPR1->RdNext = NULL;
   pPR1->RdPrevious = NULL;
   pPR1->RdReferred = NULL;
   pPR1->RdElement = NULL;
   pPR1->RdAttribute = NULL;
   pPR1->RdTypeRef = RefFollow;
   pPR1->RdInternalRef = TRUE;
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
   PtrOutReference     pRs1;

   if (PtFree_OutputRef == NULL)
      *pRS = (PtrOutReference) malloc (sizeof (OutReference));
   else
     {
	*pRS = PtFree_OutputRef;
	PtFree_OutputRef = (*pRS)->OrNext;
	NbFree_OutputRef--;
     }
   NbUsed_OutputRef++;
   pRs1 = *pRS;
   memset (pRs1, 0, sizeof (OutReference));
   pRs1->OrNext = NULL;
   pRs1->OrLabel[0] = '\0';
   ClearDocIdent (&pRs1->OrDocIdent);
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
   GetElemRefChng alloue un descripteur de changement de reference.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                GetElemRefChng (PtrChangedReferredEl * pER)

#else  /* __STDC__ */
void                GetElemRefChng (pER)
PtrChangedReferredEl *pER;

#endif /* __STDC__ */

{
   PtrChangedReferredEl pEr1;

   if (PtFree_ElemRefChng == NULL)
      *pER = (PtrChangedReferredEl) malloc (sizeof (ChangedReferredEl));
   else
     {
	*pER = PtFree_ElemRefChng;
	PtFree_ElemRefChng = (*pER)->CrNext;
	NbFree_ElemRefChng--;
     }
   NbUsed_ElemRefChng++;
   pEr1 = *pER;
   memset (pEr1, 0, sizeof (ChangedReferredEl));
   pEr1->CrNext = NULL;
   pEr1->CrOldLabel[0] = '\0';
   pEr1->CrNewLabel[0] = '\0';
   ClearDocIdent (&pEr1->CrOldDocument);
   ClearDocIdent (&pEr1->CrNewDocument);
   pEr1->CrReferringDoc = NULL;
}

/*----------------------------------------------------------------------
   FreeElemRefChng libere un descripteur de changement de reference
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FreeElemRefChng (PtrChangedReferredEl pER)

#else  /* __STDC__ */
void                FreeElemRefChng (pER)
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
   PtrEnteringReferences pRe1;

   if (PtFree_InputRef == NULL)
      *pRE = (PtrEnteringReferences) malloc (sizeof (EnteringReferences));
   else
     {
	*pRE = PtFree_InputRef;
	PtFree_InputRef = (*pRE)->ErNext;
	NbFree_InputRef--;
     }
   NbUsed_InputRef++;
   pRe1 = *pRE;
   memset (pRe1, 0, sizeof (EnteringReferences));
   pRe1->ErNext = NULL;
   pRe1->ErFirstReferredEl = NULL;
   ClearDocIdent (&pRe1->ErDocIdent);
   pRe1->ErFileName[0] = '\0';
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
   PtrReferenceChange  pFrc1;

   if (PtFree_UpdateRefFile == NULL)
      *pFRC = (PtrReferenceChange) malloc (sizeof (ReferenceChange));
   else
     {
	*pFRC = PtFree_UpdateRefFile;
	PtFree_UpdateRefFile = (*pFRC)->RcNext;
	NbFree_UpdateRefFile--;
     }
   NbUsed_UpdateRefFile++;
   pFrc1 = *pFRC;
   memset (pFrc1, 0, sizeof (ReferenceChange));
   pFrc1->RcNext = NULL;
   pFrc1->RcFirstChange = NULL;
   ClearDocIdent (&pFrc1->RcDocIdent);
   pFrc1->RcFileName[0] = '\0';
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
   pFRC->RcNext = PtFree_UpdateRefFile;
   PtFree_UpdateRefFile = pFRC;
   NbFree_UpdateRefFile++;
   NbUsed_UpdateRefFile--;
}

/*----------------------------------------------------------------------
   GetAbstractBox alloue un pave.                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                GetAbstractBox (PtrAbstractBox * pAb)

#else  /* __STDC__ */
void                GetAbstractBox (pAb)
PtrAbstractBox     *pAb;

#endif /* __STDC__ */

{
   PtrAbstractBox      pPa1;

   if (PtFree_AbsBox == NULL)
      *pAb = (PtrAbstractBox) TtaGetMemory (sizeof (AbstractBox));
   else
     {
	*pAb = PtFree_AbsBox;
	PtFree_AbsBox = (*pAb)->AbNext;
	NbFree_AbsBox--;
     }
   NbUsed_AbsBox++;
   pPa1 = *pAb;
   memset (pPa1, 0, sizeof (AbstractBox));
   pPa1->AbElement = NULL;
   pPa1->AbEnclosing = NULL;
   pPa1->AbPrevious = NULL;
   pPa1->AbNext = NULL;
   pPa1->AbFirstEnclosed = NULL;
   pPa1->AbVolume = 0;
   pPa1->AbNextRepeated = NULL;
   pPa1->AbPreviousRepeated = NULL;
   pPa1->AbCopyDescr = NULL;
   pPa1->AbCreatorAttr = NULL;
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
   int                 i;
   PtrDocument         pDo1;

   if (PtFree_Document == NULL)
      *pDoc = (PtrDocument) TtaGetMemory (sizeof (DocumentDescr));
   else
     {
	*pDoc = PtFree_Document;
	PtFree_Document = (*pDoc)->DocNext;
	NbFree_Document--;
     }
   NbUsed_Document++;
   /* initialise le contexte de document */
   pDo1 = *pDoc;
   memset (pDo1, 0, sizeof (DocumentDescr));
   pDo1->DocNext = NULL;
   pDo1->DocComment = NULL;
   pDo1->DocSSchema = NULL;
   pDo1->DocRootElement = NULL;
   for (i = 0; i < MAX_ASSOC_DOC; i++)
     {
	pDo1->DocAssocRoot[i] = NULL;
	pDo1->DocAssocSubTree[i] = NULL;
	pDo1->DocAssocFrame[i] = 0;
	pDo1->DocAssocVolume[i] = 0;
	pDo1->DocAssocFreeVolume[i] = 0;
	pDo1->DocAssocNPages[i] = 0;
	pDo1->DocAssocModifiedAb[i] = NULL;
     }
   for (i = 0; i < MAX_PARAM_DOC; i++)
      pDo1->DocParameters[i] = NULL;
   /* cree et initialise un descripteur bidon de reference, debut */
   /* de la chaine des descripteurs de references du document */
   GetDescReference (&pDo1->DocReferredEl);
   for (i = 0; i < MAX_VIEW_DOC; i++)
     {
	pDo1->DocView[i].DvSSchema = NULL;
	pDo1->DocView[i].DvPSchemaView = 0;
	pDo1->DocView[i].DvSync = FALSE;
	pDo1->DocViewRootAb[i] = NULL;
	pDo1->DocViewSubTree[i] = NULL;
	pDo1->DocViewFrame[i] = 0;
	pDo1->DocViewVolume[i] = 0;
	pDo1->DocViewFreeVolume[i] = 0;
	pDo1->DocViewNPages[i] = 0;
	pDo1->DocViewModifiedAb[i] = NULL;
     }
   pDo1->DocDName[0] = '\0';
   ClearDocIdent (&pDo1->DocIdent);
   pDo1->DocDirectory[0] = '\0';
   pDo1->DocSchemasPath[0] = '\0';
   pDo1->DocBackUpInterval = 0;
   pDo1->DocReadOnly = FALSE;
   pDo1->DocExportStructure = FALSE;
   pDo1->DocLabelExpMax = 1;
   pDo1->DocMaxPairIdent = 0;
   pDo1->DocModified = FALSE;
   pDo1->DocNTypedChars = 0;
   pDo1->DocNewOutRef = NULL;
   pDo1->DocDeadOutRef = NULL;
   pDo1->DocChangedReferredEl = NULL;
   pDo1->DocNLanguages = 0;
   pDo1->DocNNatures = 0;
   pDo1->DocPivotVersion = 0;
   pDo1->DocLabels = NULL;
   pDo1->DocToBeChecked = FALSE;
   pDo1->DocPivotError = FALSE;
   pDo1->DocNotifyAll = FALSE;
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

   pDoc->DocNext = PtFree_Document;
   PtFree_Document = pDoc;
   NbFree_Document++;
   NbUsed_Document--;
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
   int                 i;

   if (PtFree_SPres == NULL)
      *pSP = (PtrPSchema) TtaGetMemory (sizeof (PresentSchema));
   else
     {
	*pSP = PtFree_SPres;
	PtFree_SPres = (*pSP)->PsNext;
	NbFree_SchPres--;
     }
   memset ((*pSP), 0, sizeof (PresentSchema));
   (*pSP)->PsNext = NULL;
   (*pSP)->PsStructName[0] = '\0';
   (*pSP)->PsPresentName[0] = '\0';
   (*pSP)->PsStructCode = 0;
   (*pSP)->PsNViews = 0;
   for (i = 0; i < MAX_VIEW; i++)
     {
	(*pSP)->PsView[i][0] = '\0';
	(*pSP)->PsPaginatedView[i] = FALSE;
	(*pSP)->PsColumnView[i] = FALSE;
	(*pSP)->PsExportView[i] = FALSE;
     }
   (*pSP)->PsNPrintedViews = 0;
   for (i = 0; i < MAX_PRINT_VIEW; i++)
     {
	(*pSP)->PsPrintedView[i].VpAssoc = FALSE;
	(*pSP)->PsPrintedView[i].VpNumber = 0;
     }
   (*pSP)->PsNCounters = 0;
   (*pSP)->PsNConstants = 0;
   (*pSP)->PsNVariables = 0;
   (*pSP)->PsNPresentBoxes = 0;
   (*pSP)->PsFirstDefaultPRule = NULL;
   for (i = 0; i < MAX_PRES_BOX; i++)
     {
	(*pSP)->PsPresentBox[i].PbFirstPRule = NULL;
     }
   for (i = 0; i < MAX_ATTR_SSCHEMA; i++)
     {
	(*pSP)->PsAttrPRule[i] = NULL;
	(*pSP)->PsNAttrPRule[i] = 0;
	(*pSP)->PsNComparAttrs[i] = 0;
	(*pSP)->PsComparAttr[i] = NULL;
     }
   for (i = 0; i < MAX_RULES_SSCHEMA; i++)
     {
	(*pSP)->PsElemPRule[i] = NULL;
	(*pSP)->PsNInheritedAttrs[i] = 0;
	(*pSP)->PsInheritedAttr[i] = NULL;
	(*pSP)->PsAcceptPageBreak[i] = TRUE;
	(*pSP)->PsAcceptLineBreak[i] = TRUE;
	(*pSP)->PsBuildAll[i] = FALSE;
	(*pSP)->PsNotInLine[i] = FALSE;
	(*pSP)->PsInPageHeaderOrFooter[i] = FALSE;
	(*pSP)->PsAssocPaginated[i] = FALSE;
	(*pSP)->PsElemTransmit[i] = 0;
     }

   (*pSP)->PsNTransmElems = 0;

   NbUsed_SchPres++;
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
   if (PtFree_HandleSchPres == NULL)
      *pHSP = (PtrHandlePSchema) TtaGetMemory (sizeof (HandlePSchema));
   else
     {
	*pHSP = PtFree_HandleSchPres;
	PtFree_HandleSchPres = (*pHSP)->HdNextPSchema;
	NbFree_HandleSchPres--;
     }
   memset ((*pHSP), 0, sizeof (HandlePSchema));
   (*pHSP)->HdPSchema = NULL;
   (*pHSP)->HdNextPSchema = NULL;
   (*pHSP)->HdPrevPSchema = NULL;
   NbUsed_HandleSchPres++;
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

   if (PtFree_ExtenBlock == NULL)
      *pBE = (PtrExtensBlock) TtaGetMemory (sizeof (ExtensBlock));
   else
     {
	*pBE = PtFree_ExtenBlock;
	PtFree_ExtenBlock = (*pBE)->EbNextBlock;
	NbFree_ExtenBlock--;
     }
   memset (*pBE, 0, sizeof (ExtensBlock));
   (*pBE)->EbNextBlock = NULL;
   NbUsed_ExtenBlock++;
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
   if (PtFree_SStruct == NULL)
      *pSS = (PtrSSchema) TtaGetMemory (sizeof (StructSchema));
   else
     {
	*pSS = PtFree_SStruct;
	PtFree_SStruct = (*pSS)->SsNextExtens;
	NbFree_SchStruct--;
     }
   NbUsed_SchStruct++;
   memset ((*pSS), 0, sizeof (StructSchema));
   (*pSS)->SsNextExtens = NULL;
   (*pSS)->SsPrevExtens = NULL;
   (*pSS)->SsExtension = FALSE;
   (*pSS)->SsNExtensRules = 0;
   (*pSS)->SsExtensBlock = NULL;
   (*pSS)->SsFirstPSchemaExtens = NULL;
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
   if (pSS->SsExtensBlock != NULL)
      FreeExternalBlock (pSS->SsExtensBlock);
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

   if (PtFree_PresRule == NULL)
      *pRP = (PtrPRule) TtaGetMemory (sizeof (PresRule));
   else
     {
	*pRP = PtFree_PresRule;
	PtFree_PresRule = (*pRP)->PrNextPRule;
	NbFree_PresRule--;
     }
   NbUsed_PresRule++;
   memset ((*pRP), 0, sizeof (PresRule));
   (*pRP)->PrNextPRule = NULL;
   (*pRP)->PrSpecifAttr = 0;
   (*pRP)->PrSpecifAttrSSchema = NULL;
   (*pRP)->PrCond = NULL;
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
   if (PtFree_CondPresRule == NULL)
      *pCond = (PtrCondition) TtaGetMemory (sizeof (Condition));
   else
     {
	*pCond = PtFree_CondPresRule;
	PtFree_CondPresRule = (*pCond)->CoNextCondition;
	NbFree_CondPresRule--;
     }
   NbUsed_CondPresRule++;
   memset ((*pCond), 0, sizeof (Condition));
   (*pCond)->CoNextCondition = NULL;
   (*pCond)->CoNotNegative = FALSE;
   (*pCond)->CoTarget = FALSE;
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
   if (PtFree_DelayR == NULL)
      *pRR = (PtrDelayedPRule) TtaGetMemory (sizeof (DelayedPRule));
   else
     {
	*pRR = PtFree_DelayR;
	PtFree_DelayR = (*pRR)->DpNext;
	NbFree_DelayR--;
     }
   NbUsed_DelayR++;
   memset (*pRR, 0, sizeof (DelayedPRule));
   (*pRR)->DpPRule = NULL;
   (*pRR)->DpPSchema = NULL;
   (*pRR)->DpAbsBox = NULL;
   (*pRR)->DpNext = NULL;
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
#endif				/* __STDC__ */
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
void                GetPosBlock (PtrPosRelations * adbloc)

#else  /* __STDC__ */
void                GetPosBlock (adbloc)
PtrPosRelations    *adbloc;

#endif /* __STDC__ */

{
   int                 i;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   PtrPosRelations     pTa1;

   if (PtFrePosB == NULL)
      *adbloc = (PtrPosRelations) TtaGetMemory (sizeof (PosRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
	*adbloc = PtFrePosB;
	PtFrePosB = (*adbloc)->PosRNext;
	NbFree_PosB--;
     }
   NbUsed_PosB++;
   /* Initialisation du bloc */
   pTa1 = *adbloc;
   memset (pTa1, 0, sizeof (PosRelations));
   pTa1->PosRNext = NULL;

   for (i = 1; i <= MAX_RELAT_POS; i++)
      pTa1->PosRTable[i - 1].ReBox = NULL;
}

/*----------------------------------------------------------------------
   FreePosBlock libere le bloc de relations.                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FreePosBlock (PtrPosRelations * adbloc)

#else  /* __STDC__ */
void                FreePosBlock (adbloc)
PtrPosRelations    *adbloc;

#endif /* __STDC__ */

{

   /* Insere le bloc en tete de la chaine des libres */
   (*adbloc)->PosRNext = PtFrePosB;
   PtFrePosB = *adbloc;
   NbFree_PosB++;
   NbUsed_PosB--;
}

/*----------------------------------------------------------------------
   GetDimBlock alloue un bloc de relations de dimension.            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                GetDimBlock (PtrDimRelations * adbloc)

#else  /* __STDC__ */
void                GetDimBlock (adbloc)
PtrDimRelations    *adbloc;

#endif /* __STDC__ */

{
   int                 i;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   PtrDimRelations     pTa1;

   if (PtFreBDim == NULL)
      *adbloc = (PtrDimRelations) TtaGetMemory (sizeof (DimRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
	*adbloc = PtFreBDim;
	PtFreBDim = (*adbloc)->DimRNext;
	NbFree_BDim--;
     }
   NbUsed_BDim++;
   /* Initialisation du bloc */
   pTa1 = *adbloc;
   memset (pTa1, 0, sizeof (DimRelations));
   pTa1->DimRNext = NULL;

   for (i = 1; i <= MAX_RELAT_DIM; i++)
     {
	pTa1->DimRTable[i - 1] = NULL;
	pTa1->DimRSame[i - 1] = TRUE;
     }
}

/*----------------------------------------------------------------------
   FreeDimBlock libere le bloc de relations de dimension.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FreeDimBlock (PtrDimRelations * adbloc)

#else  /* __STDC__ */
void                FreeDimBlock (adbloc)
PtrDimRelations    *adbloc;

#endif /* __STDC__ */

{

   /* Insere le bloc en tete de la chaine des libres */
   (*adbloc)->DimRNext = PtFreBDim;
   PtFreBDim = *adbloc;
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
   PtrBox              adbox;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   if (PtFreBox == NULL)
      adbox = (PtrBox) malloc (sizeof (Box));
   /* Sinon recupere le contexte en tete de la chaine des libres */
   else
     {
	adbox = PtFreBox;
	PtFreBox = adbox->BxNexChild;
	NbFree_Box--;
     }
   NbUsed_Box++;

   /* Initialisation de la box */
   if (adbox != NULL)
     {
	memset (adbox, 0, sizeof (Box));
	adbox->BxAbstractBox = pAb;
	adbox->BxPrevious = NULL;
	adbox->BxNext = NULL;
	adbox->BxType = BoComplete;
	adbox->BxNexChild = NULL;
	adbox->BxXOrg = 0;
	adbox->BxYOrg = 0;
	adbox->BxWidth = 0;
	adbox->BxHeight = 0;
	adbox->BxHorizRef = 0;
	adbox->BxVertRef = 0;
	adbox->BxFont = NULL;
	adbox->BxUnderline = 0;
	adbox->BxThickness = 0;
	adbox->BxMoved = NULL;
	adbox->BxHorizInc = NULL;
	adbox->BxVertInc = NULL;
	adbox->BxNChars = 0;
	adbox->BxHorizEdge = Left;
	adbox->BxVertEdge = Top;
	adbox->BxNSpaces = 0;
	adbox->BxSpaceWidth = 0;
	adbox->BxPosRelations = NULL;
	adbox->BxWidthRelations = NULL;
	adbox->BxHeightRelations = NULL;
	adbox->BxEndOfBloc = 0;
	/* Il n'y a pas de relations hors-structure */
	adbox->BxXOutOfStruct = FALSE;
	adbox->BxYOutOfStruct = FALSE;
	adbox->BxWOutOfStruct = FALSE;
	adbox->BxHOutOfStruct = FALSE;
	/* La box n'est pas elastique */
	adbox->BxHorizFlex = FALSE;
	adbox->BxVertFlex = FALSE;
	adbox->BxHorizInverted = FALSE;
	adbox->BxVertInverted = FALSE;
	adbox->BxNew = FALSE;
	/* Initialisation du traitement des dimensions minimales */
	adbox->BxContentHeight = FALSE;
	adbox->BxContentWidth = FALSE;
	adbox->BxRuleHeigth = 0;
	adbox->BxRuleWidth = 0;
	adbox->BxBuffer = NULL;
	adbox->BxEndOfBloc = 0;
	adbox->BxNPixels = 0;
	adbox->BxFirstChar = 0;
	adbox->BxXToCompute = FALSE;
	adbox->BxYToCompute = FALSE;
     }
   return adbox;
}

/*----------------------------------------------------------------------
   FreeBox libere le contexte de boite adBox et retourne la boite 
   suivante.                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrBox              FreeBox (PtrBox adBox)

#else  /* __STDC__ */
PtrBox              FreeBox (adBox)
PtrBox              adBox;

#endif /* __STDC__ */

{
   PtrPosRelations     pPosRel;
   PtrPosRelations     nepos;
   PtrDimRelations     pDimRel;
   PtrDimRelations     nedim;
   PtrBox              pBo1;
   PtrBox              NextBox;

   /* Insere le contexte de boite en tete de la chaine des libres */
   pBo1 = adBox;
   pBo1->BxAbstractBox = NULL;
   pBo1->BxPrevious = NULL;
   pBo1->BxNext = NULL;
   /* On retire l'indicateur de fin de* bloc */
   pBo1->BxEndOfBloc = 0;
   pBo1->BxType = BoComplete;
   NextBox = pBo1->BxNexChild;
   pBo1->BxNexChild = PtFreBox;
   PtFreBox = adBox;
   NbFree_Box++;
   NbUsed_Box--;
   /* On libere les differents blocs attaches a la boite */
   pPosRel = pBo1->BxPosRelations;
   while (pPosRel != NULL)
     {
	nepos = pPosRel->PosRNext;
	FreePosBlock (&pPosRel);
	pPosRel = nepos;
     }

   pDimRel = pBo1->BxWidthRelations;
   while (pDimRel != NULL)
     {
	nedim = pDimRel->DimRNext;
	FreeDimBlock (&pDimRel);
	pDimRel = nedim;
     }

   pDimRel = pBo1->BxHeightRelations;
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
void                GetLine (PtrLine * adligne)

#else  /* __STDC__ */
void                GetLine (adligne)
PtrLine            *adligne;

#endif /* __STDC__ */

{
   PtrLine             pLi1;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   if (PtFree_Line == NULL)
      *adligne = (PtrLine) malloc (sizeof (Line));
   /* Sinon recupere le contexte en tete de la chaine des libres */
   else
     {
	*adligne = PtFree_Line;
	PtFree_Line = (*adligne)->LiNext;
	NbFree_Line--;
     }
   NbUsed_Line++;
   /* Initialisation de la ligne */
   if (*adligne != NULL)
     {
	pLi1 = *adligne;
	memset (pLi1, 0, sizeof (Line));
	pLi1->LiPrevious = NULL;
	pLi1->LiNext = NULL;
	pLi1->LiXOrg = 0;
	pLi1->LiYOrg = 0;
	pLi1->LiXMax = 0;
	pLi1->LiMinLength = 0;
	pLi1->LiHeight = 0;
	pLi1->LiHorizRef = 0;
	pLi1->LiNSpaces = 0;
	pLi1->LiSpaceWidth = 0;
	pLi1->LiNPixels = 0;
	pLi1->LiFirstBox = NULL;
	pLi1->LiLastBox = NULL;
	pLi1->LiFirstPiece = NULL;
	pLi1->LiLastPiece = NULL;
	pLi1->LiRealLength = 0;
     }
}

/*----------------------------------------------------------------------
   FreeLine libere le contexte de ligne.                           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FreeLine (PtrLine adligne)

#else  /* __STDC__ */
void                FreeLine (adligne)
PtrLine             adligne;

#endif /* __STDC__ */

{

   /* Insere le contexte de ligne en tete de la chaine des libres */
   adligne->LiNext = PtFree_Line;
   PtFree_Line = adligne;
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
	s = (PtrSearchContext) malloc (sizeof (SearchContext));
	/* Initialisation des champs */
	*pSearch = s;
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
