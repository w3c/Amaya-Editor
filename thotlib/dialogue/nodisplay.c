/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"
#include "appaction.h"

#define EXPORT extern
#include "edit_tv.h"

/* procedures importees */
#include "schemas_f.h"
#include "dofile_f.h"
#include "ouvre_f.h"
#include "memory_f.h"

/* procedure bidon declarees localement et exportees */
#ifdef __STDC__
boolean             CallEventType (NotifyEvent * notifyEvent, boolean pre)
#else  /* __STDC__ */
boolean             CallEventType (notifyEvent, pre)
NotifyEvent        *notifyEvent;
boolean             pre;

#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
boolean             CallEventAttribute (NotifyAttribute * notifyAttr, boolean pre)
#else  /* __STDC__ */
boolean             CallEventAttribute (notifyAttr, pre)
NotifyAttribute    *notifyAttr;
boolean             pre;

#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
void                FreeImageDescriptor (int *desc)

#else  /* __STDC__ */
void                FreeImageDescriptor (desc)
int                *desc;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                FreePicture (int *desc)

#else  /* __STDC__ */
void                FreePicture (desc)
int                *desc;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
int                 Corr_Charger (Language langue)
#else  /* __STDC__ */
int                 Corr_Charger (langue)
Language            langue;

#endif /* __STDC__ */
{
   return -1;
}

#ifdef __STDC__
void                AddInSelection (PtrElement pEl, boolean dernier)
#else  /* __STDC__ */
void                AddInSelection (pEl, dernier)
PtrElement          pEl;
boolean             dernier;

#endif /* __STDC__ */
{
}


/* LibAbbEl est appele' par le module arbabs */
#ifdef __STDC__
void                LibAbbEl (PtrElement pEl)
#else  /* __STDC__ */
void                LibAbbEl (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ConfigTranslateSSchema (PtrSSchema pSS)
#else  /* __STDC__ */
void                ConfigTranslateSSchema (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */
{
}

/* GlobalSearchRulepEl est appele' par le module pivlec */
#ifdef __STDC__
PtrPRule        GlobalSearchRulepEl (PtrElement pEl, PtrPSchema * pSPR, int NumPres, PtrPSchema pSchP, int Vue, PRuleType TRegle, boolean Pag, boolean attr, PtrAttribute * pAttr)
#else  /* __STDC__ */
PtrPRule        GlobalSearchRulepEl (pEl, pSPR, NumPres, pSchP, Vue, TRegle, Pag, attr, pAttr)
PtrElement          pEl;
PtrPSchema         *pSPR;
int                 NumPres;
PtrPSchema          pSchP;
int                 Vue;
PRuleType           TRegle;
boolean             Pag;
boolean             attr;
PtrAttribute        *pAttr;

#endif /* __STDC__ */
{
   return NULL;
}

#ifdef __STDC__
boolean             FormeComplete ()
#else  /* __STDC__ */
boolean             FormeComplete ()
#endif				/* __STDC__ */
{
   return TRUE;
}

#ifdef __STDC__
void                CacheRemoveImage (PtrElement element)
#else  /* __STDC__ */
void                CacheRemoveImage (element)
PtrElement          element;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
int                 PixelEnPt (int x)
#else  /* __STDC__ */
int                 PixelEnPt (x)
int                 x;

#endif /* __STDC__ */
{
   return x;
}

#ifdef __STDC__
int                 PtEnPixel (int x)
#else  /* __STDC__ */
int                 PtEnPixel (x)
int                 x;

#endif /* __STDC__ */
{
   return x;
}

#ifdef __STDC__
boolean             IsASavedElement (PtrElement pEl)
#else  /* __STDC__ */
boolean             IsASavedElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
boolean             BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
boolean             BothHaveNoSpecRules (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   if (pEl1->ElFirstPRule == NULL && pEl2->ElFirstPRule == NULL)
      return TRUE;
   else
      return FALSE;
}


/*----------------------------------------------------------------------
   UnloadDocument libere le document dont le descripteur est pointe'  
   par pDoc.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UnloadDocument (PtrDocument * pDoc)
#else  /* __STDC__ */
void                UnloadDocument (pDoc)
PtrDocument        *pDoc;

#endif /* __STDC__ */
{
   int                 d;

   if (*pDoc != NULL)
      /* cherche dans la table le descripteur de document a liberer */
     {
	d = 1;
	while (LoadedDocument[d - 1] != *pDoc && d < MAX_DOCUMENTS)
	   d++;
	if (LoadedDocument[d - 1] == *pDoc)
	   /* supprime la selection dans ce document */
	  {
	     /* liberer tout l'arbre interne */
	     DeleteAllTrees (*pDoc);
	     /* liberer les schemas : document, natures, presentations */
	     FreeDocumentSchemas (*pDoc);
	     FreeDocument (LoadedDocument[d - 1]);
	     LoadedDocument[d - 1] = NULL;
	     *pDoc = NULL;
	  }
     }
}
