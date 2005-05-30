/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
#include "thot_gui.h"
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
void TtaSetLabelAllocator (T_LabelAllocator * labelAlloc)
{
   labelAllocator = labelAlloc;
}

/*----------------------------------------------------------------------
   NewLabel allocate a new label for a document and returns it's value.
  ----------------------------------------------------------------------*/
int NewLabel (PtrDocument pDoc)
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
int GetCurrentLabel (PtrDocument pDoc)
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
void SetCurrentLabel (PtrDocument pDoc, int label)
{
  if (labelAllocator == NULL)
    pDoc->DocLabelExpMax = label;
  else
    /*** Set label value of new allocator ! ***/
    labelAllocator->SetCurLabel ((Document) IdentDocument (pDoc), label);
}
