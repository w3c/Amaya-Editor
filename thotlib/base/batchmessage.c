/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
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
 * affichage des messages d'erreur sur le terminal 
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "message.h"

#define THOT_EXPORT extern
#include "edit_tv.h"


/*----------------------------------------------------------------------
   DisplayConfirmMessage displays the given message (text).        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayConfirmMessage (STRING text)
#else  /* __STDC__ */
void                DisplayConfirmMessage (text)
STRING              text;

#endif /* __STDC__ */
{
   ufprintf (stderr, text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayMessage (STRING text, int msgType)
#else  /* __STDC__ */
void                DisplayMessage (text, msgType)
STRING              text;
int                 msgType;

#endif /* __STDC__ */
{
   ufprintf (stderr, text);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaError (int errorCode)

#else  /* __STDC__ */
void                TtaError (errorCode)
int                 errorCode;

#endif /* __STDC__ */
{
   UserErrorCode = errorCode;
}
