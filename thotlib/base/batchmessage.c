/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
int                 UserErrorCode;


/*----------------------------------------------------------------------
   DisplayConfirmMessage displays the given message (text).        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayConfirmMessage (char *text)
#else  /* __STDC__ */
void                DisplayConfirmMessage (text)
char               *text;

#endif /* __STDC__ */
{
   fprintf (stderr, text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayMessage (char *text, int msgType)
#else  /* __STDC__ */
void                DisplayMessage (text, msgType)
char               *text;
int                 msgType;

#endif /* __STDC__ */
{
   fprintf (stderr, text);
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
