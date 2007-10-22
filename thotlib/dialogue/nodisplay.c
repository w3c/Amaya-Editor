/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "language.h"
#include "appaction.h"
#include "fileaccess.h"

#define THOT_EXPORT extern
#include "edit_tv.h"

/* procedures importees */
#include "schemas_f.h"
#include "memory_f.h"
#include "readpivot_f.h"

/* dummy procedures to make the link editor happy */

int TtaGetThotColor (unsigned short red, unsigned short green, unsigned short blue)
{
  return 1;
}

ThotBool CallEventType (NotifyEvent * notifyEvent, ThotBool pre)
{
   return FALSE;
}

ThotBool CallEventAttribute (NotifyAttribute * notifyAttr, ThotBool pre)
{
   return FALSE;
}

void CleanPictInfo (ThotPictInfo *esc)
{
}


void AddInSelection (PtrElement pEl, ThotBool dernier)
{
}


/* FreeAbEl est appele' par le module arbabs */
void FreeAbEl (PtrElement pEl,PtrDocument pDoc)
{
}

void ConfigTranslateSSchema (PtrSSchema pSS)
{
}

/* GlobalSearchRulepEl est appele' par le module pivlec */
PtrPRule GlobalSearchRulepEl (PtrElement pEl, PtrDocument pDoc, PtrPSchema * pSPR, PtrSSchema * pSSR, ThotBool presBox, int presNum, PtrPSchema pSchP, int Vue, PRuleType TRegle, FunctionType typeFunc, ThotBool Pag, ThotBool attr, PtrAttribute * pAttr)
{
   return NULL;
}

ThotBool IsASavedElement (PtrElement pEl)
{
   return FALSE;
}

ThotBool BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
{
   if (pEl1->ElFirstPRule == NULL && pEl2->ElFirstPRule == NULL)
      return TRUE;
   else
      return FALSE;
}

void AddSchemaGuestViews (PtrDocument pDoc, PtrSSchema pSS)
{
}
