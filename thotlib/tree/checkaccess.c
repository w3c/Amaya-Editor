/*
 * (c) COPYRIGHT INRIA, Grif, 1996. Please first read the full copyright statement
 * in file COPYRIGHT.
 * 
 */

/*
 * This module implements the operations on the abstract trees used by Thot.
 * 
 * Author: V. Quint (INRIA)
 * 
 */

#include "libmsg.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "res.h"
#include "labelAllocator.h"
#include "appdialogue.h"

#define EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "memory_f.h"
#include "schemas_f.h"
#include "absboxes_f.h"
#include "draw_f.h"
#include "content_f.h"
#include "references_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "labelalloc_f.h"

#include "tree_f.h"
#include "exceptions_f.h"
#include "abspictures_f.h"
#include "externalref_f.h"

/*--------------------------------------------------------------
    HierarchicalCheckHiddenAccess
    returns TRUE in the isHidden variable if the element is
    hidden to the user, or if it belongs to a hidden tree.
--------------------------------------------------------------*/

#ifdef __STDC__
void      HierarchicalCheckHiddenAccess (PtrElement pEl,
					           boolean * isHidden)
#else			     /* __STDC__ */
void      HierarchicalCheckHiddenAccess (pEl,
					           isHidden)
PtrElement pEl;
boolean  *isHidden;

#endif			     /* __STDC__ */

{
PtrElement pAsc;

   *isHidden = FALSE;
   pAsc = pEl;
   while (pAsc != NULL)
      if (pAsc->ElAccess == AccessHidden)
        {
          *isHidden = TRUE;
	  return;
        }
      else
        pAsc = pAsc->ElParent;

}

/*--------------------------------------------------------------
    HierarchicalCheckReadOnlyAccess
    returns TRUE in the isReadOnly variable if the element is
    protected against user modifications, or if it belongs to
    a protected tree.
--------------------------------------------------------------*/

#ifdef __STDC__
void      HierarchicalCheckReadOnlyAccess (PtrElement pEl,
					             boolean * isReadOnly)
#else			     /* __STDC__ */
void      HierarchicalCheckReadOnlyAccess (pEl,
					             isReadOnly)
PtrElement pEl;
boolean  *isReadOnly;

#endif			     /* __STDC__ */

{
PtrElement pAsc;
boolean   testRO;

   *isReadOnly = FALSE;
   testRO = TRUE;
   pAsc = pEl;

   while (pAsc != NULL)
      if (pAsc->ElAccess == AccessHidden ||
	  (testRO && pAsc->ElAccess == AccessReadOnly))
        {
	  *isReadOnly = TRUE;
	  return;
        }
      else
        {
	  if (pAsc->ElAccess == AccessReadWrite)
	    testRO = FALSE;
	  pAsc = pAsc->ElParent;
        }

}

/*--------------------------------------------------------------
    TtaSetCheckAccessFunctions
    overload default procedures to check if an element is hidden
    or in read-only mode.
--------------------------------------------------------------*/

#ifdef __STDC__
void      TtaSetCheckAccessFunctions (Proc checkHiddenProc,
				                Proc checkReadOnlyProc)
#else			     /* __STDC__ */
void      TtaSetCheckAccessFunctions (checkHiddenProc,
				                checkReadOnlyProc)
Proc      checkHiddenProc;
Proc      checkReadOnlyProc;

#endif			     /* __STDC__ */

{

   if (checkHiddenProc == NULL)
     TteConnectAction (T_checkHiddenElement,
		       (Proc) HierarchicalCheckHiddenAccess);
   else
     TteConnectAction (T_checkHiddenElement,
		       checkHiddenProc);

   if (checkReadOnlyProc == NULL)
     TteConnectAction (T_checkReadOnlyElement,
		       (Proc) HierarchicalCheckHiddenAccess);
   else
     TteConnectAction (T_checkReadOnlyElement,
		       checkReadOnlyProc);

}

/*--------------------------------------------------------------
    CheckAccessLoadResources load functionalities to authorize
    access checking to elements. So, it allows to specify special
    functions to determine the different cases in which elements
    may be considered as hidden or in read-only mode.
--------------------------------------------------------------*/

#ifdef __STDC__
void      CheckAccessLoadResources ()
#else			     /* __STDC__ */
void      CheckAccessLoadResources ()
#endif			     /* __STDC__ */

{

   if (ThotLocalActions[T_checkHiddenElement] == NULL)
     {
       TteConnectAction (T_checkHiddenElement,
			 (Proc) HierarchicalCheckHiddenAccess);
       TteConnectAction (T_checkReadOnlyElement,
			 (Proc) HierarchicalCheckReadOnlyAccess);
     }

}
