/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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

/* procedure bidon declarees localement et exportees */
#ifdef __STDC__
int                 TtaGetThotColor (unsigned short red, unsigned short green, unsigned short blue)
#else  /* __STDC__ */
int                 TtaGetThotColor (red, green, blue)
unsigned short      red;
unsigned short      green;
unsigned short      blue;
#endif /* __STDC__ */
{
  return 1;
}

#ifdef __STDC__
ThotBool            CallEventType (NotifyEvent * notifyEvent, ThotBool pre)
#else  /* __STDC__ */
ThotBool            CallEventType (notifyEvent, pre)
NotifyEvent        *notifyEvent;
ThotBool            pre;
#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
ThotBool            CallEventAttribute (NotifyAttribute * notifyAttr, ThotBool pre)
#else  /* __STDC__ */
ThotBool            CallEventAttribute (notifyAttr, pre)
NotifyAttribute    *notifyAttr;
ThotBool            pre;
#endif /* __STDC__ */
{
   return FALSE;
}


#ifdef __STDC__
void                FreePictInfo (PictInfo *esc)
#else  /* __STDC__ */
void                FreePictInfo (desc)
PictInfo           *desc;
#endif /* __STDC__ */
{
}


#ifdef __STDC__
void                AddInSelection (PtrElement pEl, ThotBool dernier)
#else  /* __STDC__ */
void                AddInSelection (pEl, dernier)
PtrElement          pEl;
ThotBool            dernier;
#endif /* __STDC__ */
{
}


/* FreeAbEl est appele' par le module arbabs */
#ifdef __STDC__
void                FreeAbEl (PtrElement pEl)
#else  /* __STDC__ */
void                FreeAbEl (pEl)
PtrElement          pEl;
#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ConfigTranslateSSchema (PtrSSchema pSS)
#else  /* __STDC__ */
void                ConfigTranslateSSchema (pSS)
PtrSSchema          pSS;
#endif /* __STDC__ */
{
}

/* GlobalSearchRulepEl est appele' par le module pivlec */
#ifdef __STDC__
PtrPRule            GlobalSearchRulepEl (PtrElement pEl, PtrPSchema * pSPR, PtrSSchema * pSSR, int NumPres, PtrPSchema pSchP, int Vue, PRuleType TRegle, FunctionType typeFunc, ThotBool Pag, ThotBool attr, PtrAttribute * pAttr)
#else  /* __STDC__ */
PtrPRule            GlobalSearchRulepEl (pEl, pSPR, pSSR, NumPres, pSchP, Vue, TRegle, typeFunc, Pag, attr, pAttr)
PtrElement          pEl;
PtrPSchema         *pSPR;
PtrSSchema         *pSSR;
int                 NumPres;
PtrPSchema          pSchP;
int                 Vue;
PRuleType           TRegle;
FunctionType        typeFunc;
ThotBool            Pag;
ThotBool            attr;
PtrAttribute       *pAttr;
#endif /* __STDC__ */
{
   return NULL;
}

#ifdef __STDC__
ThotBool            IsASavedElement (PtrElement pEl)
#else  /* __STDC__ */
ThotBool            IsASavedElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
ThotBool            BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
ThotBool            BothHaveNoSpecRules (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   if (pEl1->ElFirstPRule == NULL && pEl2->ElFirstPRule == NULL)
      return TRUE;
   else
      return FALSE;
}

