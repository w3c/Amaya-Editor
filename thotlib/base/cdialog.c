
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* 
   cdialog.c : Module des entrees Unix.
   Aout 88
   Major changes 
   IV : Mai 92   adaptation Tool Kit
 */

#include "thot_sys.h"
#include "message.h"


static int          CptArg = 1;
static int          _argc;
static char       **_argv;


/*debut */
/* ---------------------------------------------------------------------- */
/* |    CommandArgs                                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CommandArgs (int argc, char **argv)

#else  /* __STDC__ */
void                CommandArgs (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */
{
   _argc = argc;
   _argv = argv;
}
 /* fin */

/*debut */
/* ---------------------------------------------------------------------- */
/* |    CPtGet cherche un argument unix, et si pas trouve' ecrit le     | */
/* |            message (origine, numero) et lit la re'ponse    | */
/* |             a` la console.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 CPtGet (int origine, int numero, char *buff, int maxlen)

#else  /* __STDC__ */
int                 CPtGet (origine, numero, buff, maxlen)
int                 origine;
int                 numero;
char               *buff;
int                 maxlen;

#endif /* __STDC__ */

{
   int                 i;
   char               *arg;
   int                 c;

   if (_argc > CptArg)
     {
	arg = _argv[CptArg++];
	i = strlen (arg);
	if (i >= maxlen)
	  {
	     strncpy (buff, arg, maxlen);
	     buff[maxlen - 1] = '\0';
	     i = maxlen;
	  }
	else
	   strcpy (buff, arg);
     }
   else
     {
	if (buff == NULL)
	   TtaDisplaySimpleMessage (origine, 0, numero);
	else
	   TtaDisplaySimpleMessageString (origine, 0, numero, buff);
	c = getchar ();
	for (i = 0; i < maxlen && (char) c != '\n' && c != EOF; i++)
	  {
	     buff[i] = (char) c;
	     c = getchar ();
	  }
	buff[i] = '\0';
     }
   return i;
}
/*fin */

/* ---------------------------------------------------------------------- */
/* |    CArgGetint cherche un entier en argument unix.                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 CArgGetint (int *res)

#else  /* __STDC__ */
int                 CArgGetint (res)
int                *res;

#endif /* __STDC__ */

{
   int                 i;
   char               *arg;

   if (CptArg >= _argc)
      return 0;
   arg = _argv[CptArg++];
   if (sscanf (arg, "%d", &i))
     {
	*res = i;
	return 1;
     }
   else
     {
	return 0;
     }
}
