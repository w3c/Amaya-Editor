/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * affichage des messages d'erreur sur le terminal 
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
