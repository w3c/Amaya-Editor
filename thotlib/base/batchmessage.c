/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * affichage des messages d'erreur sur le terminal 
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "message.h"

#define THOT_EXPORT extern
#include "edit_tv.h"


/*----------------------------------------------------------------------
   DisplayConfirmMessage displays the given message (text).        
  ----------------------------------------------------------------------*/
void DisplayConfirmMessage (const char *text)
{
   fprintf (stderr, text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DisplayMessage (char *text, int msgType)
{
   fprintf (stderr, text);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaError (int errorCode)
{
   UserErrorCode = errorCode;
}
