/*
 * Copyright (c) 1996 INRIA, All rights reserved
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
 *
 * Handle actions of Thot library
 *
 * Author:  I. Vatton (INRIA)
 * Changed: S. Bonhomme (INRIA) 
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "app.h"
#include "constmedia.h"
#include "appdialogue.h"
#undef THOT_EXPORT
#define THOT_EXPORT extern

#include "appdialogue_tv.h"

/*----------------------------------------------------------------------
   TteConnectAction rend accessible une action locale (Callback).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TteConnectAction (int id, Proc procedure)
#else  /* __STDC__ */
void                TteConnectAction (id, procedure)
int                 id;
Proc                procedure;

#endif /* __STDC__ */
{
   ThotLocalActions[id] = procedure;
}

/*----------------------------------------------------------------------
   TtaSetBackup connect the backup function: procedure().
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetBackup (Proc procedure)
#else  /* __STDC__ */
void                TtaSetBackup (procedure)
int                 id;
Proc                procedure;

#endif /* __STDC__ */
{
  TteConnectAction (T_backuponfatal, procedure);
}
