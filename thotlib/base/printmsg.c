
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   printmsg.c : affichage des messages d'erreur de different niveau 
   B. Dulauroy  Avril 1991
   Major changes 
   IV : Mai 92  adaptation Tool Kit
 */

#include "thot_sys.h"
int                 UserErrorCode;

/* ---------------------------------------------------------------------- */
/* |    ThotFin termine l'application Thot.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ThotFin (int result)

#else  /* __STDC__ */
void                ThotFin (result)
int                 result;

#endif /* __STDC__ */

{
   fflush (stderr);
   fflush (stdout);
   exit (result);
}


/* ---------------------------------------------------------------------- */
/* |    DisplayConfirmMessage displays the given message (text).        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DisplayConfirmMessage (char *text)
#else  /* __STDC__ */
void                DisplayConfirmMessage (text)
char               *text;
#endif /* __STDC__ */
{
   fprintf (stderr, text);
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
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

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaError (int errorCode)

#else  /* __STDC__ */
void                TtaError (errorCode)
int                 errorCode;

#endif /* __STDC__ */
{
   UserErrorCode = errorCode;
}
