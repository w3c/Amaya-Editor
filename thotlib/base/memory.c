/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
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
#include "content.h"
/*#define DEBUG_MEMORY*/

#undef  THOT_EXPORT
#define THOT_EXPORT
#include "boxes_tv.h"
#include "edit_tv.h"

int                 NbFree_TextBuff;
int                 NbUsed_TextBuff;
PtrTextBuffer       PtFree_TextBuff;

int                 NbFree_PathSeg;
int                 NbUsed_PathSeg;
PtrPathSeg          PtFree_PathSeg;

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

int                 NbFree_AbsBox;
int                 NbUsed_AbsBox;
PtrAbstractBox      PtFree_AbsBox;

int                 NbFree_Document;
int                 NbUsed_Document;
PtrDocument         PtFree_Document;

int                 NbFree_SchPres;
int                 NbUsed_SchPres;
PtrPSchema          PtFree_SchPres;

int                 NbFree_HandleSchPres;
int                 NbUsed_HandleSchPres;
PtrHandlePSchema    PtFree_HandleSchPres;

int                 NbFree_DocSchemasDescr;
int                 NbUsed_DocSchemasDescr;
PtrDocSchemasDescr  PtFree_DocSchemasDescr;

int                 NbFree_SchStruct;
int                 NbUsed_SchStruct;
PtrSSchema          PtFree_SchStruct;

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

int                 NbFree_Box;
int                 NbUsed_Box;
PtrBox              PtFree_Box;

int                 NbFree_PosB;
int                 NbUsed_PosB;
PtrPosRelations     PtFree_PosB;

int                 NbFree_BDim;
int                 NbUsed_BDim;
PtrDimRelations     PtFree_BDim;

int                 NbFree_Line;
int                 NbUsed_Line;
PtrLine             PtFree_Line;

int                 NbFree_Dict;
int                 NbUsed_Dict;
PtrDict             PtFree_Dict;

#include "memory_f.h"
#include "fileaccess_f.h"



#ifndef IV_DEBUG
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
void  *TtaGetMemory (unsigned int n)
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
void TtaFreeMemory (void *ptr)
{
   if (ptr)	
      free (ptr);
}

/*----------------------------------------------------------------------
   TtaReAlloc increases the size of the memory block.                        
  ----------------------------------------------------------------------*/
void *TtaRealloc (void *ptr, unsigned int n)
{
   void               *res;

   if (n == 0)
      n++;
   res = realloc (ptr, (size_t) n);
#ifndef _WINDOWS 
   if (!res)
     TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
#endif /* _WINDOWS */
   return res;
}

#endif /*_DEBUG*/

/*----------------------------------------------------------------------
   FreeAll frees all allocated memory
  ----------------------------------------------------------------------*/
void                FreeAll ()
{
  void       *ptr;
  PtrSSchema pSS;
  int        i;

  while (PtFree_TextBuff != NULL)
    {
      ptr = (void *)PtFree_TextBuff;
      PtFree_TextBuff = PtFree_TextBuff->BuNext;
      TtaFreeMemory (ptr);
    }
  NbFree_TextBuff = 0;

  while (PtFree_PathSeg != NULL)
    {
      ptr = (void *)PtFree_PathSeg;
      PtFree_PathSeg = PtFree_PathSeg->PaNext;
      TtaFreeMemory (ptr);
    }
  NbFree_PathSeg = 0;

  while (PtFree_Element != NULL)
    {
      ptr = (void *)PtFree_Element;
      PtFree_Element = PtFree_Element->ElNext;
      TtaFreeMemory (ptr);
    }
  NbFree_Element = 0;

  while (PtFree_Attr != NULL)
    {
      ptr = (void *)PtFree_Attr;
      PtFree_Attr = PtFree_Attr->AeNext;
      TtaFreeMemory (ptr);
    }
  NbFree_Attr = 0;
    
  while (PtFree_DescRef != NULL)
    {
      ptr = (void *)PtFree_DescRef;
      PtFree_DescRef = PtFree_DescRef->ReNext;
      TtaFreeMemory (ptr);
    }
  NbFree_DescRef = 0;
    
  while (PtFree_ExternalDoc != NULL)
    {
      ptr = (void *)PtFree_ExternalDoc;
      PtFree_ExternalDoc = PtFree_ExternalDoc->EdNext;
      TtaFreeMemory (ptr);
    }
  NbFree_ExternalDoc = 0;
    
  while (PtFree_DescCopy != NULL)
    {
      ptr = (void *)PtFree_DescCopy;
      PtFree_DescCopy = PtFree_DescCopy->CdNext;
      TtaFreeMemory (ptr);
    }
  NbFree_DescCopy = 0;
    
  while (PtFree_Reference != NULL)
    {
      ptr = (void *)PtFree_Reference;
      PtFree_Reference = PtFree_Reference->RdNext;
      TtaFreeMemory (ptr);
    }
  NbFree_Reference = 0;
    
  while (PtFree_AbsBox != NULL)
    {
      ptr = (void *)PtFree_AbsBox;
      PtFree_AbsBox = PtFree_AbsBox->AbNext;
      TtaFreeMemory (ptr);
    }
  NbFree_AbsBox = 0;
    
  while (PtFree_Document != NULL)
    {
      ptr = (void *)PtFree_Document;
      PtFree_Document = PtFree_Document->DocNext;
      TtaFreeMemory (ptr);
    }
  NbFree_Document = 0;
    
  while (PtFree_SchPres != NULL)
    {
      ptr = (void *)PtFree_SchPres;
      PtFree_SchPres = PtFree_SchPres->PsNext;
      TtaFreeMemory (ptr);
    }
  NbFree_SchPres = 0;
    
  while (PtFree_HandleSchPres != NULL)
    {
      ptr = (void *)PtFree_HandleSchPres;
      PtFree_HandleSchPres = PtFree_HandleSchPres->HdNextPSchema;
      TtaFreeMemory (ptr);
    }
  NbFree_HandleSchPres = 0;

  while (PtFree_DocSchemasDescr != NULL)
    {
      ptr = (void *)PtFree_DocSchemasDescr;
      PtFree_DocSchemasDescr = PtFree_DocSchemasDescr->PfNext;
      TtaFreeMemory (ptr);
    }
  NbFree_DocSchemasDescr = 0;

  while (PtFree_SchStruct != NULL)
    {
      pSS = PtFree_SchStruct;
      PtFree_SchStruct = PtFree_SchStruct->SsNextExtens;
      for (i = 0; i < pSS->SsNAttributes; i++)
	free (pSS->SsAttribute->TtAttr[i]);
      free (pSS->SsAttribute);
      for (i = 0; i < pSS->SsNRules; i++)
	free (pSS->SsRule->SrElem[i]);
      free (pSS->SsRule);
      TtaFreeMemory ((void *) pSS);
    }
  NbFree_SchStruct = 0;
    
  while (PtFree_ExtenBlock != NULL)
    {
      ptr = (void *)PtFree_ExtenBlock;
      PtFree_ExtenBlock = PtFree_ExtenBlock->EbNextBlock;
      TtaFreeMemory (ptr);
    }
  NbFree_ExtenBlock = 0;
    
  while (PtFree_PresRule != NULL)
    {
      ptr = (void *)PtFree_PresRule;
      PtFree_PresRule = PtFree_PresRule->PrNextPRule;
      TtaFreeMemory (ptr);
    }
  NbFree_PresRule = 0;
    
  while (PtFree_CondPresRule != NULL)
    {
      ptr = (void *)PtFree_CondPresRule;
      PtFree_CondPresRule = PtFree_CondPresRule->CoNextCondition;
      TtaFreeMemory (ptr);
    }
  NbFree_CondPresRule = 0;
    
  while (PtFree_DelayR != NULL)
    {
      ptr = (void *)PtFree_DelayR;
      PtFree_DelayR = PtFree_DelayR->DpNext;
      TtaFreeMemory (ptr);
    }
  NbFree_DelayR = 0;
    
  while (PtFree_Box != NULL)
    {
      ptr = (void *)PtFree_Box;
      PtFree_Box = PtFree_Box->BxNexChild;
      TtaFreeMemory (ptr);
    }
  PtFree_Box = 0;
    
  while (PtFree_PosB != NULL)
    {
      ptr = (void *)PtFree_PosB;
      PtFree_PosB = PtFree_PosB->PosRNext;
      TtaFreeMemory (ptr);
    }
  NbFree_PosB = 0;
    
  while (PtFree_BDim != NULL)
    {
      ptr = (void *)PtFree_BDim;
      PtFree_BDim = PtFree_BDim->DimRNext;
      TtaFreeMemory (ptr);
    }
  NbFree_BDim = 0;
    
  while (PtFree_Line != NULL)
    {
      ptr = (void *)PtFree_Line;
      PtFree_Line = PtFree_Line->LiNext;
      TtaFreeMemory (ptr);
    }
  NbFree_Line = 0;
    
  while (PtFree_Dict != NULL)
    {
      ptr = (void *)PtFree_Dict;
      PtFree_Dict = PtFree_Dict->DictNext;
      TtaFreeMemory (ptr);
    }
  NbFree_Dict = 0;
}


/*----------------------------------------------------------------------
   TtaStrdup

   Copy the string given in argument to a newly allocated piece of memory.
   Parameters:
   str: a zero terminated string.
   See also:
   TtaGetMemory.
  ----------------------------------------------------------------------*/
char *TtaStrdup (char* str)
{
   char *res;

   if (str == NULL)
      return (NULL);
   res = TtaGetMemory (strlen (str) + 1);
   if (res == NULL)
      return (res);
   strcpy (res, str);
   return (res);
}

/*----------------------------------------------------------------------
   InitEditorMemory initializes block contexts.                                                  
  ----------------------------------------------------------------------*/
void InitEditorMemory ()
{

   NbFree_TextBuff = 0;
   NbUsed_TextBuff = 0;
   PtFree_TextBuff = NULL;

   NbFree_PathSeg = 0;
   NbUsed_PathSeg = 0;
   PtFree_PathSeg = NULL;

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

   NbFree_AbsBox = 0;
   NbUsed_AbsBox = 0;
   PtFree_AbsBox = NULL;

   NbFree_Document = 0;
   NbUsed_Document = 0;
   PtFree_Document = NULL;

   NbFree_SchPres = 0;
   NbUsed_SchPres = 0;
   PtFree_SchPres = NULL;

   NbFree_HandleSchPres = 0;
   NbUsed_HandleSchPres = 0;
   PtFree_HandleSchPres = NULL;

   NbFree_DocSchemasDescr = 0;
   NbUsed_DocSchemasDescr = 0;
   PtFree_DocSchemasDescr = NULL;

   NbFree_SchStruct = 0;
   NbUsed_SchStruct = 0;
   PtFree_SchStruct = NULL;

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
void GetTextBuffer (PtrTextBuffer *pBT)
{
  PtrTextBuffer       pBuf;

  if (PtFree_TextBuff == NULL)
    /* pas de buffer dans la chaine des libres, acquiert un nouveau buffer */
    pBuf = (PtrTextBuffer) TtaGetMemory (sizeof (TextBuffer));
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
      pBuf->BuContent[0] = EOS;
      NbUsed_TextBuff++;
    }
}

/*----------------------------------------------------------------------
   FreeTextBuffer libere un buffer de texte.                       
  ----------------------------------------------------------------------*/
void FreeTextBuffer (PtrTextBuffer pBT)
{

  /* insere le buffer en tete de la chaine des libres */
  if (pBT != NULL)
    {
#ifdef DEBUG_MEMORY
      TtaFreeMemory (pBT);
#else
      pBT->BuNext = PtFree_TextBuff;
      PtFree_TextBuff = pBT;
      NbFree_TextBuff++;
#endif
      NbUsed_TextBuff--;
    }
}

/*----------------------------------------------------------------------
   GetPathSeg
  ----------------------------------------------------------------------*/
void GetPathSeg (PtrPathSeg *pPE)
{
   PtrPathSeg       pPa;

   if (PtFree_PathSeg == NULL)
     {
     /* pas de buffer dans la chaine des libres, acquiert un nouveau buffer */
     pPa = (PtrPathSeg) TtaGetMemory (sizeof (PathSeg));
     }
   else
     {
	/* recupere un buffer en tete de la chaine des libres */
	pPa = PtFree_PathSeg;
	PtFree_PathSeg = pPa->PaNext;
	NbFree_PathSeg--;
     }
   /* initialise le buffer */
   *pPE = pPa;
   if (pPa)
     {
       memset (pPa, 0, sizeof (PathSeg));
       pPa->PaNext = NULL;
       pPa->PaPrevious = NULL;
       NbUsed_PathSeg++;
     }
}

/*----------------------------------------------------------------------
   FreePathSeg
  ----------------------------------------------------------------------*/
void FreePathSeg (PtrPathSeg pPE)
{

   /* insere le buffer en tete de la chaine des libres */
   if (pPE != NULL)
     {
#ifdef DEBUG_MEMORY
        TtaFreeMemory (pPE);
#else
	pPE->PaNext = PtFree_PathSeg;
	PtFree_PathSeg = pPE;
	NbFree_PathSeg++;
#endif
	NbUsed_PathSeg--;
     }
}

/*----------------------------------------------------------------------
   GetElement alloue un element.                                   
  ----------------------------------------------------------------------*/
void GetElement (PtrElement * pEl)
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
       pNewEl->ElParent = NULL;
       pNewEl->ElPrevious = NULL;
       pNewEl->ElNext = NULL;
       pNewEl->ElReferredDescr = NULL;
       pNewEl->ElFirstSchDescr = NULL;
       pNewEl->ElFirstAttr = NULL;
       pNewEl->ElFirstPRule = NULL;
       for (i = 1; i <= MAX_VIEW_DOC; i++)
	 pNewEl->ElAbstractBox[i - 1] = NULL;
       pNewEl->ElStructSchema = NULL;
       pNewEl->ElTypeNumber = 0;
       pNewEl->ElVolume = 0;
       pNewEl->ElCopyDescr = NULL;
       pNewEl->ElIsCopy = FALSE;
       pNewEl->ElSource = NULL;
       pNewEl->ElLineNb = 0;
       pNewEl->ElCopy = NULL;
       pNewEl->ElAccess = AccessInherited;
       pNewEl->ElHolophrast = FALSE;
       pNewEl->ElTransContent = FALSE;
       pNewEl->ElTransAttr = FALSE;
       pNewEl->ElTransPres = FALSE;
       pNewEl->ElTerminal = FALSE;
       pNewEl->ElSystemOrigin = FALSE;
       pNewEl->ElTransform = NULL;
       pNewEl->animation = NULL;       
       pNewEl->ElFirstChild = NULL;

       NbUsed_Element++;
     }
}

/*----------------------------------------------------------------------
   FreeElement libere un element.                                  
  ----------------------------------------------------------------------*/
void FreeElement (PtrElement pEl)
{
   PtrPathSeg       pPa, pPaNext;

#ifdef _GL
   if (pEl->ElTransform)
     TtaFreeTransform (pEl->ElTransform);
#endif /*_GL*/
   if (pEl->ElLeafType == LtText && pEl->ElText)
     {
       FreeTextBuffer (pEl->ElText);
       pEl->ElText = NULL;
     }
   else if (pEl->ElLeafType == LtPath && pEl->ElFirstPathSeg)
     {
       pPa = pEl->ElFirstPathSeg;
       do
	 {
	   pPaNext = pPa->PaNext;
	   FreePathSeg (pPa);
	   pPa = pPaNext;
	 }
       while (pPa);
     }
   pEl->ElStructSchema = NULL;
#ifdef DEBUG_MEMORY
   TtaFreeMemory (pEl);
#else
   pEl->ElNext = PtFree_Element;
   PtFree_Element = pEl;
   NbFree_Element++;
#endif
   NbUsed_Element--;
}

/*----------------------------------------------------------------------
   GetAttribute alloue un attribut.                                     
  ----------------------------------------------------------------------*/
void GetAttribute (PtrAttribute * pAttr)
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
void FreeAttribute (PtrAttribute pAttr)
{
#ifdef DEBUG_MEMORY
  TtaFreeMemory (pAttr);
#else
  pAttr->AeNext = PtFree_Attr;
  pAttr->AeAttrSSchema = NULL;
  pAttr->AeAttrNum = 0;
  PtFree_Attr = pAttr;
  NbFree_Attr++;
#endif
  NbUsed_Attr--;
}

/*----------------------------------------------------------------------
   GetReferredDescr alloue un descripteur de reference.            
  ----------------------------------------------------------------------*/
void GetReferredDescr (PtrReferredDescr * pDR)
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
       pNewDR->ReReferredLabel[0] = EOS;
       pNewDR->ReExternalRef = FALSE;
       pNewDR->ReReferredElem = NULL;
       NbUsed_DescRef++;
     }
}

/*----------------------------------------------------------------------
   FreeReferredDescr libere un descripteur de reference.           
  ----------------------------------------------------------------------*/
void FreeReferredDescr (PtrReferredDescr pDR)
{
  if (pDR != NULL)
    {
      TtaFreeMemory (pDR);
      NbUsed_DescRef--;
    }
}

/*----------------------------------------------------------------------
   GetDescCopy alloue un descripteur de copie.                     
  ----------------------------------------------------------------------*/
void GetDescCopy (PtrCopyDescr * pDC)
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
void FreeDescCopy (PtrCopyDescr pDC)
{
  TtaFreeMemory (pDC);
  NbUsed_DescCopy--;
}

/*----------------------------------------------------------------------
   GetExternalDoc alloue un descripteur de document externe.       
  ----------------------------------------------------------------------*/
void GetExternalDoc (PtrExternalDoc * pDE)
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
void FreeExternalDoc (PtrExternalDoc pDE)
{

  TtaFreeMemory (pDE);
  NbUsed_ExternalDoc--;
}

/*----------------------------------------------------------------------
   GetReference alloue une reference.                              
  ----------------------------------------------------------------------*/
void GetReference (PtrReference * pRef)
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
void FreeReference (PtrReference pRef)
{

  TtaFreeMemory (pRef);
  NbUsed_Reference--;
}

/*----------------------------------------------------------------------
   GetAbstractBox alloue un pave.                                  
  ----------------------------------------------------------------------*/
void GetAbstractBox (PtrAbstractBox *pAb)
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
   *pAb = pNewAb;
   if (pNewAb)
     {
       memset (pNewAb, 0, sizeof (AbstractBox));
       NbUsed_AbsBox++;
     }
}

/*----------------------------------------------------------------------
   FreeAbstractBox libere un pave.                                 
  ----------------------------------------------------------------------*/
void FreeAbstractBox (PtrAbstractBox pAb)
{

#ifdef DEBUG_MEMORY
       TtaFreeMemory (pAb);
#else
   pAb->AbElement = NULL;
   pAb->AbEnclosing = NULL;
   if (pAb->AbPrevious)
     {
       if (pAb->AbPrevious->AbNext == pAb)
	 pAb->AbPrevious->AbNext = NULL;
       pAb->AbPrevious = NULL;
     }
   pAb->AbFirstEnclosed = NULL;
   pAb->AbNextRepeated = NULL;
   pAb->AbPreviousRepeated = NULL;
   if (pAb->AbNext && pAb->AbNext->AbPrevious == pAb)
     pAb->AbNext->AbPrevious = NULL;
   pAb->AbNext = PtFree_AbsBox;
   PtFree_AbsBox = pAb;
   NbFree_AbsBox++;
#endif
   NbUsed_AbsBox--;
}

/*----------------------------------------------------------------------
  FreeElemNamespaceDeclarations
  Free the namespace declarations related to an element
  ----------------------------------------------------------------------*/
void FreeElemNamespaceDeclarations (PtrDocument pDoc, PtrElement pEl)
{
  PtrNsUriDescr      uriDecl, prevUriDecl, nextUriDecl;
  PtrNsPrefixDescr   prefixDecl, nextPrefixDecl, prevPrefixDecl;
  
  if (pDoc->DocNsUriDecl == NULL)
    return;
  
  uriDecl = pDoc->DocNsUriDecl;
  prevUriDecl = uriDecl;
  while (uriDecl != NULL)
    {
      nextUriDecl = uriDecl->NsNextUriDecl;
      prefixDecl = uriDecl->NsPtrPrefix;
      prevPrefixDecl = NULL;
      while (prefixDecl != NULL)
	{
	  nextPrefixDecl = prefixDecl->NsNextPrefixDecl;
	  if (prefixDecl->NsPrefixElem == pEl)
	    {
	      if (prefixDecl->NsPrefixName != NULL)
		{
		  TtaFreeMemory (prefixDecl->NsPrefixName);
		  prefixDecl->NsPrefixName = NULL;
		}
	      TtaFreeMemory (prefixDecl);
	      if (prevPrefixDecl == NULL)
		uriDecl->NsPtrPrefix = nextPrefixDecl;
	      else
		prevPrefixDecl->NsNextPrefixDecl = nextPrefixDecl;
	    }
	  else
	    prevPrefixDecl = prefixDecl;
	  prefixDecl = nextPrefixDecl;
	}
      if (uriDecl->NsPtrPrefix == NULL)
	{
	  /* This declaration is no longer used within this document */
	  if (uriDecl->NsUriName != NULL)
	    {
	      TtaFreeMemory (uriDecl->NsUriName);
	      uriDecl->NsUriName = NULL;
	    }
	  TtaFreeMemory (uriDecl);
	  uriDecl = NULL;
	  if (prevUriDecl == pDoc->DocNsUriDecl)
	    pDoc->DocNsUriDecl = nextUriDecl;
	  else
	    prevUriDecl->NsNextUriDecl = nextUriDecl;
	}
      else
	prevUriDecl = uriDecl;
      /* Nest declaration */
      uriDecl = nextUriDecl;
    } 
}

/*----------------------------------------------------------------------
  FreeNamespaceDeclarations
  Free all namespace declarations
  ----------------------------------------------------------------------*/
void FreeNamespaceDeclarations (PtrDocument pDoc)
{
  PtrNsUriDescr       uriDecl, nextUriDecl;
  PtrNsPrefixDescr    prefixDecl, nextPrefixDecl;
  
  if (pDoc->DocNsUriDecl == NULL)
    return;
  
  uriDecl = pDoc->DocNsUriDecl;
  while (uriDecl != NULL)
    {
      nextUriDecl = uriDecl->NsNextUriDecl;
      prefixDecl = uriDecl->NsPtrPrefix;
      while (prefixDecl != NULL)
	{
	  nextPrefixDecl = prefixDecl->NsNextPrefixDecl;
	  if (prefixDecl->NsPrefixName != NULL)
	    {
	      TtaFreeMemory (prefixDecl->NsPrefixName);
	      prefixDecl->NsPrefixName = NULL;
	    }
	  TtaFreeMemory (prefixDecl);
	  prefixDecl = nextPrefixDecl;
	}
      if (uriDecl->NsUriName != NULL)
	{
	  TtaFreeMemory (uriDecl->NsUriName);
	  uriDecl->NsUriName = NULL;
	}
      TtaFreeMemory (uriDecl);
      uriDecl = nextUriDecl;
    } 
  pDoc->DocNsUriDecl = NULL;
}

/*----------------------------------------------------------------------
   GetDocument alloue un descripteur de document.                  
  ----------------------------------------------------------------------*/
void GetDocument (PtrDocument *pDoc)
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
       pNewDoc->DocFirstSchDescr = NULL;
       pNewDoc->DocLabelExpMax = 1;
       pNewDoc->DocLastEdit = NULL;
       pNewDoc->DocNbEditsInHistory = 0;
       pNewDoc->DocEditSequence = FALSE;
       pNewDoc->DocLastUndone = NULL;
       pNewDoc->DocNbUndone = 0;
       NbUsed_Document++;
     }
}

/*----------------------------------------------------------------------
   FreeDocument libere un descripteur de document.                 
  ----------------------------------------------------------------------*/
void FreeDocument (PtrDocument pDoc)
{
   int                 i;

   pDoc->DocSSchema = NULL;
   pDoc->DocDocElement = NULL;
   for (i = 0; i < MAX_VIEW_DOC; i++)
     {
       pDoc->DocView[i].DvSSchema = NULL;
       pDoc->DocView[i].DvPSchemaView = 0;
       pDoc->DocView[i].DvSync = FALSE;
       pDoc->DocView[i].DvFirstGuestView = NULL;
       pDoc->DocViewRootAb[i] = NULL;
       pDoc->DocViewSubTree[i] = NULL;
       pDoc->DocViewModifiedAb[i] = NULL;
     }
   /* libere le 1er descripteur de reference (bidon) */
   FreeReferredDescr (pDoc->DocReferredEl);
   pDoc->DocReferredEl = NULL;
   pDoc->DocLabels = NULL;
   FreeNamespaceDeclarations (pDoc);
#ifdef DEBUG_MEMORY
   TtaFreeMemory (pDoc);
#else
   pDoc->DocNext = PtFree_Document;
   PtFree_Document = pDoc;
   NbFree_Document++;
#endif
   NbUsed_Document--;
}

/*----------------------------------------------------------------------
   GetAttributePres allocates a attribute presentation.                    
  ----------------------------------------------------------------------*/
void GetAttributePres (AttributePres **pAP)
{
  AttributePres          *pNewAP;

  pNewAP = (AttributePres *) TtaGetMemory (sizeof (AttributePres));
  *pAP = pNewAP;
  if (pNewAP)
    {
      memset (pNewAP, 0, sizeof (AttributePres));
      pNewAP->ApMatch = CoWord;
    }
}

/*----------------------------------------------------------------------
   FreeAttributePres frees a attribute presentation.                    
  ----------------------------------------------------------------------*/
void FreeAttributePres (AttributePres *pAP)
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
void GetSchPres (PtrPSchema *pSP)
{
   PtrPSchema          pNewSP;

   if (PtFree_SchPres == NULL)
      pNewSP = (PtrPSchema) TtaGetMemory (sizeof (PresentSchema));
   else
     {
	pNewSP = PtFree_SchPres;
	PtFree_SchPres = pNewSP->PsNext;
	NbFree_SchPres--;
     }
   *pSP = pNewSP;
   if (pNewSP)
     {
       memset (pNewSP, 0, sizeof (PresentSchema));
       NbUsed_SchPres++;
     }
}

/*----------------------------------------------------------------------
   FreeSchPres libere un schemas de presentation.                  
  ----------------------------------------------------------------------*/
void FreeSchPres (PtrPSchema pSP, PtrSSchema pSS)
{
  AttributePres      *pAP, *pNextAP;
  int                 i;
  PtrHostView         pHostView, pNextHostView;

  pSP->PsNext = NULL;
  pSP->PsFirstDefaultPRule = NULL;
  if (pSP->PsPresentBox)
    {
      for (i = 0; i < pSP->PsNPresentBoxes; i++)
	{
	  free (pSP->PsPresentBox->PresBox[i]);
	  pSP->PsPresentBox->PresBox[i] = NULL;
	}
      free (pSP->PsPresentBox);
      pSP->PsPresentBox = NULL;
    }
  if (pSP->PsAttrPRule)
    {
      for (i = 0; i < pSS->SsNAttributes; i++)
	{
	  pAP = pSP->PsAttrPRule->AttrPres[i];
	  while (pAP != NULL)
	    {
	      /* free all allocated blocks */
	      pNextAP = pAP->ApNextAttrPres;
	      FreeAttributePres (pAP);
	      pAP = pNextAP;
	    }
	  pSP->PsAttrPRule->AttrPres[i] = NULL;
	}
      free (pSP->PsAttrPRule);
    }
  if (pSP->PsNAttrPRule)
    free (pSP->PsNAttrPRule);
  if (pSP->PsElemPRule)
    {
      for (i = 0; i < pSS->SsNRules; i++)
	pSP->PsElemPRule->ElemPres[i] = NULL;
      free (pSP->PsElemPRule);
    }
  if (pSP->PsNHeirElems)
    free (pSP->PsNHeirElems);
  if (pSP->PsNInheritedAttrs)
    free (pSP->PsNInheritedAttrs);
  if (pSP->PsInheritedAttr)
    {
      for (i = 0; i < pSS->SsNRules; i++)
	{
	  if (pSP->PsInheritedAttr->ElInherit[i])
	    TtaFreeMemory (pSP->PsInheritedAttr->ElInherit[i]);
	  pSP->PsInheritedAttr->ElInherit[i] = NULL;
	}
      free (pSP->PsInheritedAttr);
      pSP->PsInheritedAttr = NULL;
    }
  if (pSP->PsNComparAttrs)
    free (pSP->PsNComparAttrs);
  if (pSP->PsComparAttr)
    {
      for (i = 0; i < pSS->SsNAttributes; i++)
	{
	  if (pSP->PsComparAttr->CATable[i])
	    TtaFreeMemory (pSP->PsComparAttr->CATable[i]);
	  pSP->PsComparAttr->CATable[i] = NULL;
	}
      free (pSP->PsComparAttr);
    }
  if (pSP->PsElemTransmit)
    free (pSP->PsElemTransmit);

  for (i = 0; i < MAX_VIEW; i++)
    {
      pHostView = pSP->PsHostViewList[i];
      pSP->PsHostViewList[i] = 0;
      while (pHostView)
	{
	  pNextHostView = pHostView->NextHostView;
	  TtaFreeMemory (pHostView);
	  pHostView = pNextHostView;
	}
    }
  TtaFreeMemory (pSP);
  NbUsed_SchPres--;
}

/*----------------------------------------------------------------------
   GetHandleSchPres alloue un element de chainage de schemas de    
   presentation.                                  
  ----------------------------------------------------------------------*/
void GetHandleSchPres (PtrHandlePSchema * pHSP)
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
void FreeHandleSchPres (PtrHandlePSchema pHSP)
{
  TtaFreeMemory (pHSP);
  NbUsed_HandleSchPres--;
}


/*----------------------------------------------------------------------
   GetDocSchemasDescr alloue un block de description des schemas de
   presentation associes a un schema de structure
  ----------------------------------------------------------------------*/
void GetDocSchemasDescr (PtrDocSchemasDescr * pPFS)
{
   PtrDocSchemasDescr  pNewPFS;

   if (PtFree_DocSchemasDescr == NULL)
      pNewPFS = (PtrDocSchemasDescr) TtaGetMemory (sizeof (DocSchemasDescr));
   else
     {
	pNewPFS = PtFree_DocSchemasDescr;
	PtFree_DocSchemasDescr = pNewPFS->PfNext;
	NbFree_DocSchemasDescr--;
     }
   *pPFS = pNewPFS;
   if (pNewPFS)
     {
       memset (pNewPFS, 0, sizeof (DocSchemasDescr));
       NbUsed_DocSchemasDescr++;
     }
}

/*----------------------------------------------------------------------
   FreeDocSchemasDescr libere un block de description des schemas de
   presentation associes a un schema de structure
  ----------------------------------------------------------------------*/
void FreeDocSchemasDescr (PtrDocSchemasDescr pPFS)
{
   TtaFreeMemory (pPFS);
   NbUsed_DocSchemasDescr--;
}

/*----------------------------------------------------------------------
   GetTRule allocates a translation rule
  ----------------------------------------------------------------------*/
void GetTRule (PtrTRule *pR)
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
void FreeTRule (PtrTRule pR)
{
  if (pR)
    TtaFreeMemory (pR);
}

/*----------------------------------------------------------------------
   GetAttributeTransl allocates a block representing translation rules for
   an attribute
  ----------------------------------------------------------------------*/
void GetAttributeTransl (PtrAttributeTransl *pB)
{
  PtrAttributeTransl    pNewB;
  int                   size;

  size = sizeof (AttributeTransl);
  pNewB = (PtrAttributeTransl) TtaGetMemory (size);
  *pB = pNewB;
  if (pNewB)
    memset (pNewB, 0, size);
}

/*----------------------------------------------------------------------
   FreeAttributeTransl frees a block representing translation rules for
   an attribute
  ----------------------------------------------------------------------*/
void FreeAttributeTransl (PtrAttributeTransl pB)
{
  if (pB)
    TtaFreeMemory (pB);
}

/*----------------------------------------------------------------------
   GetSchTra allocates a translation schema
  ----------------------------------------------------------------------*/
void GetSchTra (PtrTSchema *pST)
{
  PtrTSchema    pNewST;

  pNewST = (PtrTSchema) TtaGetMemory (sizeof (TranslSchema));
  *pST = pNewST;
  if (pNewST)
    memset (pNewST, 0, sizeof (TranslSchema));
}

/*----------------------------------------------------------------------
   FreeSchTra frees a translation schema
  ----------------------------------------------------------------------*/
void FreeSchTra (PtrTSchema pST, PtrSSchema pSS)
{
  int     i;

  if (pST)
    {
      if (pST->TsElemTRule)
	{
	  for (i = 0; i < pSS->SsNRules; i++)
	    pST->TsElemTRule->TsElemTransl[i] = NULL;
	  TtaFreeMemory (pST->TsElemTRule);
	  pST->TsElemTRule = NULL;
	}
      if (pST->TsInheritAttr)
	{
	  TtaFreeMemory (pST->TsInheritAttr);
	  pST->TsInheritAttr = NULL;
	}
      if (pST->TsAttrTRule)
	{
	  TtaFreeMemory (pST->TsAttrTRule);
	  pST->TsAttrTRule = NULL;
	}
      TtaFreeMemory (pST);
    }
}

/*----------------------------------------------------------------------
   GetExternalBlock alloue un bloc d'extension pour un schema de   
   structure.                                                      
  ----------------------------------------------------------------------*/
void GetExternalBlock (PtrExtensBlock * pBE)
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
void FreeExternalBlock (PtrExtensBlock pBE)
{
#ifdef DEBUG_MEMORY
       TtaFreeMemory (pBE);
#else
   pBE->EbNextBlock = PtFree_ExtenBlock;
   PtFree_ExtenBlock = pBE;
   NbFree_ExtenBlock++;
#endif
   NbUsed_ExtenBlock--;
}

/*----------------------------------------------------------------------
   GetSchStruct alloue un schema de structure.                     
  ----------------------------------------------------------------------*/
void GetSchStruct (PtrSSchema * pSS)
{
  PtrSSchema    pNewSS;

  if (PtFree_SchStruct == NULL)
      pNewSS = (PtrSSchema) TtaGetMemory (sizeof (StructSchema));
  else
    {
      pNewSS = PtFree_SchStruct;
      PtFree_SchStruct = pNewSS->SsNextExtens;
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
void FreeSchStruc (PtrSSchema pSS)
{
  int        i;

  if (pSS->SsExtensBlock != NULL)
    {
      FreeExternalBlock (pSS->SsExtensBlock);
      pSS->SsExtensBlock = NULL;
    }
  if (pSS->SsUriName != NULL)
    {
      TtaFreeMemory (pSS->SsUriName);
      pSS->SsUriName = NULL;
    }
  pSS->SsPrevExtens = NULL;
  pSS->SsExtension = FALSE;
  pSS->SsNExtensRules = 0;
  pSS->SsExtensBlock = NULL;
  for (i = 0; i < pSS->SsNAttributes; i++)
    free (pSS->SsAttribute->TtAttr[i]);
  free (pSS->SsAttribute);
  for (i = 0; i < pSS->SsNRules; i++)
    {
      if (pSS->SsRule->SrElem[i]->SrLocalAttr)
	TtaFreeMemory (pSS->SsRule->SrElem[i]->SrLocalAttr);
      if (pSS->SsRule->SrElem[i]->SrRequiredAttr)
	TtaFreeMemory (pSS->SsRule->SrElem[i]->SrRequiredAttr);
      free (pSS->SsRule->SrElem[i]);
    }
  free (pSS->SsRule);
  TtaFreeMemory (pSS);
  NbUsed_SchStruct--;
}

/*----------------------------------------------------------------------
   GetPresentRule alloue une regle de presentation.                
  ----------------------------------------------------------------------*/
void GetPresentRule (PtrPRule * pRP)
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
      pNewRP->PrImportant = FALSE;
      pNewRP->PrDuplicate = FALSE;
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
void FreePresentRule (PtrPRule pRP)
{
   PtrCondition        pCond, nextCond;

   pCond = pRP->PrCond;
   while (pCond != NULL)
     {
	nextCond = pCond->CoNextCondition;
	FreePresentRuleCond (pCond);
	pCond = nextCond;
     }
   TtaFreeMemory (pRP);
   NbUsed_PresRule--;
}

/*----------------------------------------------------------------------
   GetPresentRuleCond alloue une condition pour une regle de       
   presentation.                                                   
  ----------------------------------------------------------------------*/
void GetPresentRuleCond (PtrCondition * pCond)
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
void FreePresentRuleCond (PtrCondition pCond)
{
  TtaFreeMemory (pCond);
   NbUsed_CondPresRule--;
}

/*----------------------------------------------------------------------
   GetDifferedRule alloue une regle retardee.                      
  ----------------------------------------------------------------------*/
void GetDifferedRule (PtrDelayedPRule * pRR)
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
void FreeDifferedRule (PtrDelayedPRule pRR)
{
#ifdef DEBUG_MEMORY
       TtaFreeMemory (pRR);
#else
   pRR->DpNext = PtFree_DelayR;
   PtFree_DelayR = pRR;
   NbFree_DelayR++;
#endif
   NbUsed_DelayR--;
}

/*----------------------------------------------------------------------
   InitKernelMemory initialise l'allocation memoire pour le kernel   
  ----------------------------------------------------------------------*/
void InitKernelMemory ()
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
   PtFree_Box = NULL;
   NbFree_Box = 0;
   NbUsed_Box = 0;
   PtFree_PosB = NULL;
   NbFree_PosB = 0;
   NbUsed_PosB = 0;
   PtFree_BDim = NULL;
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
void GetPosBlock (PtrPosRelations * pBlock)
{
   int                 i;
   PtrPosRelations     pNewBlock;

   if (PtFree_PosB == NULL)
     pNewBlock  = (PtrPosRelations) TtaGetMemory (sizeof (PosRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
	pNewBlock = PtFree_PosB;
	PtFree_PosB = pNewBlock->PosRNext;
	NbFree_PosB--;
     }
   /* Initialisation du bloc */
   *pBlock = pNewBlock;
   if (pNewBlock)
     {
       memset (pNewBlock, 0, sizeof (PosRelations));
       pNewBlock->PosRNext = NULL;
       
       for (i = 0; i < MAX_RELAT_POS; i++)
	 pNewBlock->PosRTable[i].ReBox = NULL;
       NbUsed_PosB++;
     }
}

/*----------------------------------------------------------------------
   FreePosBlock libere le bloc de relations.                       
  ----------------------------------------------------------------------*/
void FreePosBlock (PtrPosRelations * pBlock)
{
  PtrPosRelations    pNextBlock;

  pNextBlock = (*pBlock)->PosRNext;
#ifdef DEBUG_MEMORY
  TtaFreeMemory (*pBlock);
#else
   /* Insere le bloc en tete de la chaine des libres */
  memset (*pBlock, 0, sizeof (PosRelations));
   (*pBlock)->PosRNext = PtFree_PosB;
   PtFree_PosB = *pBlock;
   NbFree_PosB++;
#endif
   NbUsed_PosB--;
   *pBlock = pNextBlock;
}

/*----------------------------------------------------------------------
   GetDimBlock alloue un bloc de relations de dimension.            
  ----------------------------------------------------------------------*/
void GetDimBlock (PtrDimRelations * pBlock)
{
   int                 i;
   PtrDimRelations     pNewBlock;

   if (PtFree_BDim == NULL)
     pNewBlock = (PtrDimRelations) TtaGetMemory (sizeof (DimRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
       pNewBlock = PtFree_BDim;
       PtFree_BDim = pNewBlock->DimRNext;
       NbFree_BDim--;
     }
   /* Initialisation du bloc */
   *pBlock = pNewBlock;
   if (pNewBlock)
     {
       memset (pNewBlock, 0, sizeof (DimRelations));
       pNewBlock->DimRNext = NULL;
       
       for (i = 0; i < MAX_RELAT_DIM; i++)
	 {
	   pNewBlock->DimRTable[i] = NULL;
	   pNewBlock->DimRSame[i] = TRUE;
	 }
       NbUsed_BDim++;
     }
}

/*----------------------------------------------------------------------
   FreeDimBlock libere le bloc de relations de dimension.              
  ----------------------------------------------------------------------*/
void FreeDimBlock (PtrDimRelations * pBlock)
{
  PtrDimRelations    pNextBlock;

  pNextBlock = (*pBlock)->DimRNext;
#ifdef DEBUG_MEMORY
  TtaFreeMemory (*pBlock);
#else
   /* Insere le bloc en tete de la chaine des libres */
  memset (*pBlock, 0, sizeof (DimRelations));
   (*pBlock)->DimRNext = PtFree_BDim;
   PtFree_BDim = *pBlock;
   NbFree_BDim++;
#endif
   NbUsed_BDim--;
   *pBlock = pNextBlock;
}

/*----------------------------------------------------------------------
   GetBox Alloue un nouveau contexte de boite pour le pave pAb.    
  ----------------------------------------------------------------------*/
PtrBox GetBox (PtrAbstractBox pAb)
{
   PtrBox              pBox;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   if (PtFree_Box == NULL)
      pBox = (PtrBox) TtaGetMemory (sizeof (Box));
   /* Sinon recupere le contexte en tete de la chaine des libres */
   else
     {
	pBox = PtFree_Box;
	PtFree_Box = pBox->BxNexChild;
	NbFree_Box--;
     }

   /* Initialisation de la box */
   if (pBox != NULL)
     {
	memset (pBox, 0, sizeof (Box));
	pBox->BxAbstractBox = pAb;
	pBox->BxType = BoComplete;
	pBox->BxHorizEdge = Left;
	pBox->BxVertEdge = Top;
	    
#ifdef _GL
      pBox->BxEditable = FALSE;
      pBox->BxBoundinBoxComputed = FALSE;
      pBox->BxTransformationComputed = FALSE;
      pBox->BxClipX = 0; 
      pBox->BxClipY = 0; 
      pBox->BxClipW = 0;
      pBox->BxClipH = 0;
      pBox->DisplayList = 0;      
#endif /*_GL*/

	NbUsed_Box++;
     }
   return pBox;
}

/*----------------------------------------------------------------------
   FreeBox libere le contexte de boite pBox et retourne la boite 
   suivante.                                               
  ----------------------------------------------------------------------*/
PtrBox FreeBox (PtrBox pBox)
{
   PtrPosRelations     pPosRel;
   PtrPosRelations     nepos;
   PtrDimRelations     pDimRel;
   PtrDimRelations     nedim;
   PtrBox              NextBox;

   /* get next child */
   if (pBox->BxType == BoSplit ||
       pBox->BxType == BoPiece ||
       pBox->BxType == BoScript ||
       pBox->BxType == BoDotted)
     NextBox = pBox->BxNexChild;
   else
     NextBox = NULL;

   /* Free remaining relation blocks */
   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	nepos = pPosRel->PosRNext;
	FreePosBlock (&pPosRel);
	pPosRel = nepos;
     }
   pBox->BxPosRelations = NULL;
   pDimRel = pBox->BxWidthRelations;
   while (pDimRel != NULL)
     {
	nedim = pDimRel->DimRNext;
	FreeDimBlock (&pDimRel);
	pDimRel = nedim;
     }
    pBox->BxWidthRelations = NULL;
   pDimRel = pBox->BxHeightRelations;
   while (pDimRel != NULL)
     {
	nedim = pDimRel->DimRNext;
	FreeDimBlock (&pDimRel);
	pDimRel = nedim;
     }
   pBox->BxHeightRelations = NULL;
   /* remove relations between boxes */
   /*if (pBox->BxNext && pBox->BxNext->BxPrevious == pBox)
     pBox->BxNext->BxPrevious =  pBox->BxPrevious;
   if (pBox->BxPrevious && pBox->BxPrevious->BxNext == pBox)
   pBox->BxPrevious->BxNext = pBox->BxNext;*/
   pBox->BxPrevious = NULL;
   pBox->BxNext = NULL;
#ifdef DEBUG_MEMORY
   TtaFreeMemory (pBox);
#else
   /* Don't use BxNext field because it's used when removing break lines */
   pBox->BxNexChild = PtFree_Box;
   PtFree_Box = pBox;
   NbFree_Box++;
#endif
   NbUsed_Box--;
   return NextBox;
}


/*----------------------------------------------------------------------
   GetLine alloue un contexte de ligne.                            
  ----------------------------------------------------------------------*/
void GetLine (PtrLine *pLine)
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
	NbUsed_Line++;
     }
}

/*----------------------------------------------------------------------
   FreeLine libere le contexte de ligne.                           
  ----------------------------------------------------------------------*/
void FreeLine (PtrLine pLine)
{

#ifdef DEBUG_MEMORY
   TtaFreeMemory (pLine);
#else
   /* Insere le contexte de ligne en tete de la chaine des libres */
   pLine->LiNext = PtFree_Line;
   PtFree_Line = pLine;
   PtFree_Line->LiPrevious = NULL;
   pLine->LiFirstBox = NULL;
   pLine->LiFirstPiece = NULL;
   pLine->LiLastBox = NULL;
   pLine->LiLastPiece = NULL;
   NbFree_Line++;
#endif
   NbUsed_Line--;
}


/*----------------------------------------------------------------------
   GetSearchContext alloue un contexte de recherche.                  
  ----------------------------------------------------------------------*/
void GetSearchContext (PtrSearchContext * pSearch)
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
	  s->SStartChar = 1;
	  s->SEndElement = NULL;
	  s->SEndChar = 1;
	  s->SStartToEnd = TRUE;
	  s->SWholeDocument = FALSE;
	}
    }
}

/*----------------------------------------------------------------------
   FreeSearchContext libe`re un contexte de recherche.                
  ----------------------------------------------------------------------*/
void FreeSearchContext (PtrSearchContext * pSearch)
{
  if (pSearch != NULL)
    if (*pSearch != NULL)
      {
	TtaFreeMemory (*pSearch);
	*pSearch = NULL;
      }
}


/*----------------------------------------------------------------------
   FreeStringInDict libere les entre'es d'un dictionnaire.             
  ----------------------------------------------------------------------*/
void FreeStringInDict (PtrDict pDict)
{
   PtrString           pString;
   PtrCommons          pCommon;
   PtrWords            pWord;
   int                 i;

   pString = pDict->DictString;
   if (pString != NULL)
     TtaFreeMemory (pString);

   pWord = pDict->DictWords;
   if (pWord != NULL)
     TtaFreeMemory (pWord);

   pCommon = pDict->DictCommon;
   if (pCommon != NULL)
     TtaFreeMemory (pCommon);

   /* maj du contexte du dictionnaire : chaine et mots */
   pDict->DictNbChars = 0;
   pDict->DictString = NULL;
   pDict->DictNbWords = -1;
   pDict->DictWords = NULL;
   pDict->DictCommon = NULL;
   for (i = 0; i < MAX_WORD_LEN; i++)
      pDict->DictLengths[i] = 0;
}

/*----------------------------------------------------------------------
   GetDictContext effectue un malloc controle.                     
  ----------------------------------------------------------------------*/
static void *GetDictContext (unsigned int n)
{
   if (n > 0)
      return (TtaGetMemory ((size_t) n));
   return (NULL);
}


/*----------------------------------------------------------------------
   GetStringInDict retourne -1 en cas de manque de memoire            
   retourne  0 si OK.                                      
  ----------------------------------------------------------------------*/
int GetStringInDict (PtrDict * pDict, ThotBool readonly)
{
   PtrDict             pdict;
   unsigned int        i;

   pdict = *pDict;
   pdict->DictMaxChars += (pdict->DictReadOnly == FALSE) ? 600 : 2;
   i = pdict->DictMaxChars + 1; /* take into account the EOS */
   /* alloue la chaine necessaire */
   pdict->DictString = (PtrString) GetDictContext (i);
   if (pdict->DictString == NULL)
     {
	FreeStringInDict (pdict);
	return (-1);
     }

   pdict->DictMaxWords += (pdict->DictReadOnly == FALSE) ? 50 : 2;
   i = pdict->DictMaxWords;
   pdict->DictCommon = (PtrCommons) GetDictContext (i);
   if (pdict->DictCommon == NULL)
     {
	FreeStringInDict (pdict);
	return (-1);
     }

   /* ATTENTION : ce sont des entiers */
   pdict->DictWords = (PtrWords) GetDictContext (i * sizeof (int));

   if (pdict->DictWords == NULL)
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
void GetDictionary (PtrDict * pDict)
{
   int                 i;
   PtrDict             pdict;

   if (PtFree_Dict == NULL)
      *pDict = (PtrDict) GetDictContext (sizeof (WordDict));
   else
     {
	*pDict = PtFree_Dict;
	PtFree_Dict = (*pDict)->DictNext;
	NbFree_Dict--;
     }

   if (*pDict != NULL)
     {
	NbUsed_Dict++;

	/* initialise le contexte de dictionnaire */
	pdict = *pDict;
	pdict->DictName[0] = EOS;
	pdict->DictDirectory = NULL;
	/* readonly */
	pdict->DictReadOnly = TRUE;
	/* contenu non charge' */
	pdict->DictLoaded = FALSE;
	/* contenu non modifie' */
	pdict->DictModified = FALSE;
	pdict->DictString = NULL;
	pdict->DictWords = NULL;
	pdict->DictCommon = NULL;

	for (i = 0; i < MAX_WORD_LEN; i++)
	   pdict->DictLengths[i] = 0;
	pdict->DictMaxWords = 0;
	pdict->DictMaxChars = 0;
	pdict->DictNbWords = -1;
	pdict->DictNbChars = 0;
     }
}


/*----------------------------------------------------------------------
   FreeDictionary libere le dictionnaire.                             
  ----------------------------------------------------------------------*/
void FreeDictionary (PtrDict pDict)
{
   pDict->DictNext = PtFree_Dict;
   PtFree_Dict = pDict;
   NbFree_Dict++;
   NbUsed_Dict--;

}
