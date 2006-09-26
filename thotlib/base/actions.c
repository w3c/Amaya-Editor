/*
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 */

/*
 *
 * Handle actions of Thot library
 *
 * Authors: I. Vatton (INRIA)
 * Changed: S. Bonhomme (INRIA) 
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "appstruct.h"
#include "constmedia.h"
#include "appdialogue.h"


#undef THOT_EXPORT
#define THOT_EXPORT extern

#include "appdialogue_tv.h"

/*----------------------------------------------------------------------
   TteConnectAction rend accessible une action locale (Callback).
  ----------------------------------------------------------------------*/
void TteConnectAction (int id, Proc procedure)
{
   ThotLocalActions[id] = procedure;
}
 
/*----------------------------------------------------------------------
   TtaSetBackup connects the backup function: procedure().
  ----------------------------------------------------------------------*/
void TtaSetBackup (Proc procedure)
{
  TteConnectAction (T_backuponfatal, procedure);
}
 
/*----------------------------------------------------------------------
   TtaSetAutoSave connects the AutoSave function: procedure().
  ----------------------------------------------------------------------*/
void TtaSetAutoSave (Proc1 procedure)
{
  TteConnectAction (T_autosave, (Proc) procedure);
}
 
/*----------------------------------------------------------------------
   TtaSetDocStatusUpdate connects the function: procedure().
   That procedure will receive two paramters:
   - doc which is the concerned document
   - modified which gives the new status (TRUE if it's modified) of the document.
  ----------------------------------------------------------------------*/
void TtaSetDocStatusUpdate (Proc procedure)
{
  TteConnectAction (T_docmodified, procedure);
}

