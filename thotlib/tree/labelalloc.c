/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   This module implements the standard label allocator used by the
   Thot kernel when loading a document. A new alllocation strategy can
   be redefined specifically to an applications.
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */	    

#include "thot_sys.h"
#include "constint.h"
#include "typeint.h"
#include "document.h"
#include "labelAllocator.h"

#include "applicationapi_f.h"

static T_LabelAllocator *labelAllocator = NULL;

/*----------------------------------------------------------------------
   TtaSetLabelAllocator override the default label allocator strategy
   with user defined routines.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaSetLabelAllocator (T_LabelAllocator * labelAlloc)

#else  /* __STDC__ */
void                TtaSetLabelAllocator (labelAlloc)
T_LabelAllocator   *labelAlloc;

#endif /* __STDC__ */

{
   labelAllocator = labelAlloc;
}

/*----------------------------------------------------------------------
   NewLabel allocate a new label for a document and returns it's value.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 NewLabel (PtrDocument pDoc)

#else  /* __STDC__ */
int                 NewLabel (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   if (labelAllocator == NULL)
     {
	pDoc->DocLabelExpMax++;
	return pDoc->DocLabelExpMax;
     }
   else
/*** New label allocator request! ***/
      return (labelAllocator->NewLabel ((Document) IdentDocument (pDoc)));
}

/*----------------------------------------------------------------------
   GetCurrentLabel returns the biggest label currently allocated to
   a document pDoc.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GetCurrentLabel (PtrDocument pDoc)

#else  /* __STDC__ */
int                 GetCurrentLabel (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   if (labelAllocator == NULL)
      return pDoc->DocLabelExpMax;
   else
/*** Current label value of new allocator ! ***/
      return (labelAllocator->GetCurLabel ((Document) IdentDocument (pDoc)));
}

/*----------------------------------------------------------------------
   SetCurrentLabel changes the value of the biggest label currently
   set to a document element.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SetCurrentLabel (PtrDocument pDoc, int label)

#else  /* __STDC__ */
void                SetCurrentLabel (pDoc, label)
PtrDocument         pDoc;
int                 label;

#endif /* __STDC__ */

{
   if (labelAllocator == NULL)
      pDoc->DocLabelExpMax = label;
   else
/*** Set label value of new allocator ! ***/
      labelAllocator->SetCurLabel ((Document) IdentDocument (pDoc),
				   label);
}
