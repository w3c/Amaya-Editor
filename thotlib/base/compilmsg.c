
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   printmsg.c : affichage des messages d'erreur de different niveau
 */
#include "compilmsg.h"
#include "message.h"
#include "thot_sys.h"
#include "constgrm.h"
#include "typegrm.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "compil.var"
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


/* ---------------------------------------------------------------------- */
/* |    CompilerError affiche un message d'erreur venant des            | */
/* |            compilateurs.                                           | */
/* |            - index: index du caractere erronne dans la ligne       | */
/* |            courante.                                               | */
/* |            - code: code de l'erreur.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CompilerError (int index, int origine, int niveau, int numero, char *inputline, int numline)
#else  /* __STDC__ */
void                CompilerError (index, origine, niveau, numero, inputline, numline)
int                 index;
int                 origine;
int                 niveau;
int                 numero;
char               *inputline;
int                 numline;
#endif /* __STDC__ */
{
   char                linecurseur[linelen];
   int                 i;

   TtaDisplaySimpleMessageNum (COMPIL, INFO, COMPIL_ERR_LINE, numline);

   if (index != 0)
     {
	for (i = 0; i < index - 1; linecurseur[i++] = ' ') ;
	linecurseur[index - 1] = '*';
	linecurseur[index] = '\0';
	TtaDisplayMessage (INFO, TtaGetMessage(COMPIL, COMPIL_STRING), inputline, linecurseur);
     }
   TtaDisplaySimpleMessage (origine, niveau, numero);
}


/* ---------------------------------------------------------------------- */
/* |    CompilerErrorString                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CompilerErrorString (int index, int origine, int niveau, int numero, char *inputline, int numline, char *string)
#else  /* __STDC__ */
void                CompilerErrorString (index, origine, niveau, numero, inputline, numline, string)
int                 index;
int                 origine;
int                 niveau;
int                 numero;
char               *inputline;
int                 numline;
char               *string;
#endif /* __STDC__ */

{
   char                linecurseur[linelen];
   int                 i;

   TtaDisplaySimpleMessageNum (COMPIL, INFO, COMPIL_ERR_LINE, numline);

   if (index != 0)
     {
	for (i = 0; i < index - 1; linecurseur[i++] = ' ') ;
	linecurseur[index - 1] = '*';
	linecurseur[index] = '\0';
	TtaDisplayMessage (INFO, TtaGetMessage(COMPIL, COMPIL_STRING), inputline, linecurseur);
     }
   TtaDisplaySimpleMessageString (origine, niveau, numero, string);
}
