/*
   message.c : traitement des messages d'erreur venant de THOT, impression, traduction...
   Irene Vatton
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "thotconfig.h"
#include "thotkey.h"
#include "libmsg.h"
#include "message.h"
#include "dialog.h"
#include "language.h"
#include "message.h"

/* Description d'une table de messages */
typedef struct _TabMsg *PtrTabMsg;
typedef struct _TabMsg
  {
     PtrTabMsg           TabNext;
     int                 TabLength;
     char              **TabMessages;
  }
TabMsg;

 /* Identification des messages Thot */
static PtrTabMsg    FirstTableMsg = NULL;
static char         EmptyMsg[] = "";
static char         result[MAX_TXT_LEN];

#include "environ.f"
#include "memory.f"
#include "message.f"

#ifdef _WINDOWS
#define isascii(c) __isascii(c)
#endif

/* ---------------------------------------------------------------------- */
/* |    TransCani convertit les code d'accents du fichier de message    | */
/* |            en accents.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
char               *TransCani (char *buffer)

#else  /* __STDC__ */
char               *TransCani (buffer)
char               *buffer;

#endif /* __STDC__ */

{
   char                nombre[4];
   int                 uniteid, dixid, centid;
   int                 i = 0, j = 0, k;

   while (buffer[i] != '\0')
     {
	/* On lit jusqu'au premier backslash rencontre */
	while ((buffer[i] != '\\') && (buffer[i] != '\0'))
	   result[j++] = buffer[i++];

	/* Teste si on est en presence de deux backslashs ou */
	/* si on se trouve devant un caractere special */
	if (buffer[i] != '\0')
	   if (buffer[i + 1] == '\\')
	     {
		/* On est dans le cas de deux backslashs consecutifs; on les prend */
		result[j++] = buffer[i++];
		result[j++] = buffer[i++];
	     }
	   else if (buffer[i + 1] == 'n')
	     {
		/* On est dans le cas d'un \n */
		i += 2;
		result[j++] = '\n';
	     }
	   else
	     {
		/* on saute le backslash */
		i++;
		/* on construit le nombre correspondant au caractere */
		k = 0;
		while ((buffer[i] >= '0')
		       && (buffer[i] <= '9')
		       && (buffer[i] != '\0')
		       && (k <= 2))
		   nombre[k++] = buffer[i++];
		nombre[k] = '\0';

		switch (strlen (nombre))
		      {
			 case 0:
			    result[j++] = buffer[i++];
			    break;
			 case 1:
			    uniteid = nombre[0] - '0';
			    result[j++] = (char) uniteid;
			    break;
			 case 2:
			    uniteid = nombre[1] - '0';
			    dixid = nombre[0] - '0';
			    result[j++] = (char) (uniteid + 8 * dixid);
			    break;
			 case 3:
			    uniteid = nombre[2] - '0';
			    dixid = nombre[1] - '0';
			    centid = nombre[0] - '0';
			    result[j++] = (char) (uniteid + 8 * dixid + 64 * centid);
			    break;
		      }
	     }
     }
   result[j] = '\0';
   return (result);
}


/* ---------------------------------------------------------------------- */
/* |    TtaGetMessageTable alloue et initialise la table des            | */
/* |    messages a` partir du fichier indique' par la variable msgName  | */
/* |    pour l'application. Cet ensemble de messages va e^tre           | */
/* |    identifie' par la valeur origine rendue par la fonction.        | */
/* |    La fonction rend la valeur -1 si la table n'est pas alloue'e.   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaGetMessageTable (char *msgName, int msgNumber)
#else  /* __STDC__ */
int                 TtaGetMessageTable (msgName, msgNumber)
char               *msgName;
int                 msgNumber;
#endif /* __STDC__ */
{
   char               *s;
   FILE               *file;
   int                 origineid;
   int                 num;
   PtrTabMsg           currenttable;
   PtrTabMsg           previoustable;
   char                texte[MAX_TXT_LEN];
   char                nomfichier[200];

   /* contruction du nom $THOTDIR/bin/$LANG-msgName */
   s = TtaGetVarLANG ();
   strcpy (nomfichier, s);
   nomfichier[2] = '-';
   strcpy (&nomfichier[3], msgName);
   SearchFile (nomfichier, 2, texte);
   file = fopen (texte, "r");
   if (file == NULL)
     {
	printf ("WARNING: cannot open file %s\n", texte);
	return (-1);
     }
   else
     {
	/* Alloue une nouvelle table */
	currenttable = (PtrTabMsg) TtaGetMemory (sizeof (struct _TabMsg));

	currenttable->TabMessages = (char **)
	   TtaGetMemory (sizeof (char *) * msgNumber);

	currenttable->TabNext = NULL;
	currenttable->TabLength = msgNumber;
	for (num = 0; num < msgNumber; num++)
	   currenttable->TabMessages[num] = NULL;
	origineid = 0;

	/* Chaine la table */
	if (FirstTableMsg == NULL)
	   FirstTableMsg = currenttable;
	else
	  {
	     previoustable = FirstTableMsg;
	     origineid++;
	     while (previoustable->TabNext != NULL)
	       {
		  previoustable = previoustable->TabNext;
		  origineid++;
	       }
	     previoustable->TabNext = currenttable;
	  }

	/* Charge les messages */
	while (((fscanf (file, "%d %[^#\n]", &num, texte)) != EOF) && (num < msgNumber))
	  {
	    s = (char *) TtaGetMemory (strlen (texte) + 1);
	    strcpy (s, TransCani (texte));
	    currenttable->TabMessages[num] = s;
	  }
	fclose (file);
     }
   return (origineid);
}

/* ---------------------------------------------------------------------- */
/* |    TtaGetMessage retourne le message correspondant a` l'origine et | */
/* |            l'indice 0 a` N donne'.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
char               *TtaGetMessage (int origin, int num)
#else  /* __STDC__ */
char               *TtaGetMessage (origin, num)
int                 origin;
int                 num;
#endif /* __STDC__ */
{
   int                 i;
   PtrTabMsg           currenttable;

   /* recherche la table de messages */
   if (origin == -1)
      /* la table de messages n'est pas chargee */
      return (EmptyMsg);
   i = 0;
   currenttable = FirstTableMsg;
   while (currenttable && i < origin)
     {
	currenttable = currenttable->TabNext;
	i++;
     }

   if (!currenttable)
      /* on n'a pas trouve la table de messages */
      return (EmptyMsg);
   else if (num >= currenttable->TabLength || num < 0)
      /* il n'y a pas de message */
      return (EmptyMsg);
   else if (!currenttable->TabMessages[num])
      /* il n'y a pas de message */
      return (EmptyMsg);
   else
      return (currenttable->TabMessages[num]);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#include <stdarg.h>
#ifdef __STDC__
void                TtaDisplayMessage (int msgType, char *fmt, ...)
#else  /* __STDC__ */
void                TtaDisplayMessage (msgType, fmt, ...)
char                *fmt;
int                 msgType;
#endif /* __STDC__ */
{
  va_list             pa;
  int                 i, lg, vald;
  char                *vals, *p;
  char                buffer[MAX_PATH];

  if (fmt)
    {
      /* construct the final message */
      va_start (pa, fmt);
      i = 0;
      for (p = fmt; *p && i+1 < MAX_PATH; p++)
	{
	  if (*p != '%')
	    {
	      buffer[i++] = *p;
	    }
	  else
	    {
	      switch (*++p)
		{
		case 'd':
		  /* it is a value */
		  vald = va_arg (pa, int);
		  if (i + 10 < MAX_PATH)
		    {
		      sprintf (&buffer[i], "%d", vald);
		      i += strlen (&buffer[i]);
		    }
		  else
		    i = MAX_PATH;	      
		  break;
		case 's':
		  /* it is a string */
		  vals = va_arg (pa, char *);
		  lg = strlen (vals);
		  if (i + lg < MAX_PATH)
		    {
		      strcpy (&buffer[i], vals);
		      i += lg;
		    }
		  else
		    i = MAX_PATH;
		  break;
		default:
		  /* other value not allowed */
		  buffer[i++] = *p;
		  break;
		}
	    }
	}
      /* Display the final message */
      buffer[i] = '\0';
      if (msgType == CONFIRM)
	DisplayConfirmMessage (buffer);
      else
	DisplayMessage (buffer, msgType);
    }
}


/* ---------------------------------------------------------------------- */
/* |    TtaDisplaySimpleMessage construit un message simple.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaDisplaySimpleMessage (int msgType, int origin, int number)
#else  /* __STDC__ */
void                TtaDisplaySimpleMessage (msgType, origin, number)
int                 msgType;
int                 origin;
int                 number;
#endif /* __STDC__ */
{
  TtaDisplayMessage(msgType, TtaGetMessage (origin, number));
}


/* ---------------------------------------------------------------------- */
/* |    PivotFormatError traite les erreurs survenues a` la lecture d'un| */
/* |            fichier pivot.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PivotFormatError (char *code)

#else  /* __STDC__ */
void                PivotFormatError (code)
char               *code;

#endif /* __STDC__ */

{
   char                buffer[MAX_CHAR];

   strncpy (buffer, code, MAX_CHAR);
   TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_PIV_FORMAT_ERR), buffer);
}
