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
 * Handles error messages for Thot applications
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

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

#include "dialogapi_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "registry_f.h"

#ifdef _WINDOWS
#define isascii(c) __isascii(c)
#endif

/*----------------------------------------------------------------------
   AsciiTranslate convertit les code d'accents du fichier de message 
   en accents.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *AsciiTranslate (char *pBuffer)
#else  /* __STDC__ */
char               *AsciiTranslate (pBuffer)
char               *pBuffer;

#endif /* __STDC__ */
{
   char                nombre[4];
   int                 uniteid, dixid, centid;
   int                 i = 0, j = 0, k;

   while (pBuffer[i] != EOS)
     {
	/* On lit jusqu'au premier backslash rencontre */
	while ((pBuffer[i] != '\\') && (pBuffer[i] != EOS))
	   result[j++] = pBuffer[i++];

	/* Teste si on est en presence de deux backslashs ou */
	/* si on se trouve devant un caractere special */
	if (pBuffer[i] != EOS)
	   if (pBuffer[i + 1] == '\\')
	     {
		/* On est dans le cas de deux backslashs consecutifs; on les prend */
		result[j++] = pBuffer[i++];
		result[j++] = pBuffer[i++];
	     }
	   else if (pBuffer[i + 1] == 'n')
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
		while ((pBuffer[i] >= '0')
		       && (pBuffer[i] <= '9')
		       && (pBuffer[i] != EOS)
		       && (k <= 2))
		   nombre[k++] = pBuffer[i++];
		nombre[k] = EOS;

		switch (strlen (nombre))
		      {
			 case 0:
			    result[j++] = pBuffer[i++];
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
   result[j] = EOS;
   return (result);
}


/*----------------------------------------------------------------------
   TtaGetMessageTable alloue et initialise la table des            
   messages a` partir du fichier indique' par la variable msgName  
   pour l'application. Cet ensemble de messages va e^tre           
   identifie' par la valeur origine rendue par la fonction.        
   La fonction rend la valeur -1 si la table n'est pas alloue'e.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetMessageTable (CONST char *msgName, int msgNumber)
#else  /* __STDC__ */
int                 TtaGetMessageTable (msgName, msgNumber)
CONST char         *msgName;
int                 msgNumber;

#endif /* __STDC__ */
{
   char               *s;
   FILE               *file;
   int                 origineid;
   int                 num;
   PtrTabMsg           currenttable;
   PtrTabMsg           previoustable;
   char                pBuffer[MAX_TXT_LEN];
   char                fileName[200];

   /* contruction du nom $THOTDIR/bin/$LANG-msgName */
   s = TtaGetVarLANG ();
   strcpy (fileName, s);
   fileName[2] = '-';
   strcpy (&fileName[3], msgName);
   SearchFile (fileName, 2, pBuffer);
   file = fopen (pBuffer, "r");
   if (file == NULL)
     {
	printf ("WARNING: cannot open file %s\n", pBuffer);
	return (-1);
     }
   else
     {
	/* Alloue une nouvelle table */
	currenttable = (PtrTabMsg) TtaGetMemory (sizeof (struct _TabMsg));

	currenttable->TabMessages = (char **) TtaGetMemory (sizeof (char *) * msgNumber);

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
	while (((fscanf (file, "%d %[^#\r\n]", &num, pBuffer)) != EOF) && (num < msgNumber))
	  {
	     s = (char *) TtaGetMemory (strlen (pBuffer) + 1);
	     strcpy (s, AsciiTranslate (pBuffer));
	     currenttable->TabMessages[num] = s;
	  }
	fclose (file);
     }
   return (origineid);
}


/*----------------------------------------------------------------------
  FreeAllMessages
  ----------------------------------------------------------------------*/
void              FreeAllMessages ()
{
   int                 i;
   PtrTabMsg           currenttable;

   while (FirstTableMsg != NULL)
     {
       currenttable = FirstTableMsg;
       FirstTableMsg = FirstTableMsg->TabNext;
       for (i = 0; i < currenttable->TabLength; i++)
	 TtaFreeMemory (currenttable->TabMessages[i]);
       TtaFreeMemory (currenttable->TabMessages);
       TtaFreeMemory (currenttable);
     }
}


/*----------------------------------------------------------------------
   TtaGetMessage retourne le message correspondant a` l'origine et 
   l'indice 0 a` N donne'.                                 
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaDisplayMessage (int msgType, char *fmt,...)
#else  /* __STDC__ */
void                TtaDisplayMessage (msgType, fmt,...)
int                 msgType;
char               *fmt;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   va_list             pa;
   int                 i, lg, vald;
   char               *vals, *p;
   char                pBuffer[MAX_PATH];

   if (fmt)
     {
	/* construct the final message */
#ifdef STDC_HEADERS
	va_start (pa, fmt);
#else  /* STDC_HEADERS */
	va_start (pa);
#endif /* STDC_HEADERS */
	i = 0;
	for (p = fmt; *p && i + 1 < MAX_PATH; p++)
	  {
	     if (*p != '%')
	       {
		  pBuffer[i++] = *p;
	       }
	     else
	       {
		 p++;
		  switch (*p)
			{
			   case 'd':
			      /* it is a value */
			      vald = va_arg (pa, int);

			      if (i + 10 < MAX_PATH)
				{
				   sprintf (&pBuffer[i], "%d", vald);
				   i += strlen (&pBuffer[i]);
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
				   strcpy (&pBuffer[i], vals);
				   i += lg;
				}
			      else
				 i = MAX_PATH;
			      break;
			   default:
			      /* other value not allowed */
			      pBuffer[i++] = *p;
			      break;
			}
	       }
	  }
	/* Display the final message */
	pBuffer[i] = EOS;
	if (msgType == CONFIRM)
	   DisplayConfirmMessage (pBuffer);
	else
	   DisplayMessage (pBuffer, msgType);
     }
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   TtaDisplaySimpleMessage construit un message simple.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaDisplaySimpleMessage (int msgType, int origin, int number)
#else  /* __STDC__ */
void                TtaDisplaySimpleMessage (msgType, origin, number)
int                 msgType;
int                 origin;
int                 number;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   TtaDisplayMessage (msgType, TtaGetMessage (origin, number));
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   DisplayPivotMessage traite les erreurs survenues a` la lecture  
   d'un fichier pivot.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayPivotMessage (char *code)
#else  /* __STDC__ */
void                DisplayPivotMessage (code)
char               *code;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   char                pBuffer[THOT_MAX_CHAR];

   strncpy (pBuffer, code, THOT_MAX_CHAR);
   TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ERR_PIV), pBuffer);
#endif /* _WINDOWS */
}
