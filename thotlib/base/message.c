/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handles error messages for Thot applications
 *
 * Authors: I. Vatton (INRIA)
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
#include "appdialogue.h"

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
static PtrTabMsg       FirstTableMsg = NULL;
static unsigned char   EmptyMsg [1];
static unsigned char   result[MAX_TXT_LEN];

#include "dialogapi_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "registry_f.h"


/*----------------------------------------------------------------------
   AsciiTranslate convertit les code d'accents du fichier de message 
   en accents.                                             
  ----------------------------------------------------------------------*/
unsigned char *AsciiTranslate (char *pBuffer)
{
  unsigned char       number[4];
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
	{
	  if (pBuffer[i + 1] == '\\')
	    {
	      /* On est dans le cas de deux backslashs consecutifs;
		 on les prend */
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
	      /* on construit le number correspondant au caractere */
	      k = 0;
	      while (pBuffer[i] >= '0' && pBuffer[i] <= '9' &&
		     pBuffer[i] != EOS && k <= 2)
		number[k++] = pBuffer[i++];
	      number[k] = EOS;

	      switch (strlen (number))
		{
		case 0:
		  result[j++] = pBuffer[i++];
		  break;
		case 1:
		  uniteid = number[0] - '0';
		  result[j++] = uniteid;
		  break;
		case 2:
		  uniteid = number[1] - '0';
		  dixid = number[0] - '0';
		  result[j++] = uniteid + 8 * dixid;
		  break;
		case 3:
		  uniteid = number[2] - '0';
		  dixid = number[1] - '0';
		  centid = number[0] - '0';
		  result[j++] = uniteid + 8 * dixid + 64 * centid;
		  break;
		}
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
int TtaGetMessageTable (CONST char *msgName, int msgNumber)
{
  FILE               *file;
  PtrTabMsg           currenttable;
  PtrTabMsg           previoustable;
  CHARSET             encoding;
  char                pBuffer[MAX_TXT_LEN];
  char                fileName[MAX_TXT_LEN];
  char               *s;
  unsigned char       pBuff[MAX_TXT_LEN];
  int                 origineid;
  int                 num;

  /* contruction du nom $THOTDIR/bin/$LANG-msgName */
  strcpy (fileName, TtaGetVarLANG ());
  fileName[2] = '-';
  ustrcpy (&fileName[3], msgName);
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

      pBuff[0] = EOS;
      fscanf (file, "# %500s\n]", pBuff);
      if (!strcasecmp (pBuff, "encoding="))
	{
	  fscanf (file, "%500s\n]", pBuff);
	  if (!strcasecmp (pBuff, "utf8"))
	    encoding = UTF_8;
	  else
	    encoding = ISO_8859_1;
	}
      else
	{
	  fseek (file, 0L, 0);
	  encoding = ISO_8859_1;
	}

      /* Load messages */
      while (fscanf (file, "%d %[^#\r\n]", &num, pBuff) != EOF &&
	     num < msgNumber)
	{
    	  s = TtaAllocString (ustrlen (pBuff) + 1);
     	  ustrcpy (s, AsciiTranslate (pBuff));
     	  currenttable->TabMessages[num] = s;
	}
      fclose (file);
    }
  return (origineid);
}


/*----------------------------------------------------------------------
  FreeAllMessages
  ----------------------------------------------------------------------*/
void FreeAllMessages ()
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
char *TtaGetMessage (int origin, int num)
{
   int                 i;
   PtrTabMsg           currenttable;

   EmptyMsg [0] = EOS;

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
void TtaDisplayMessage (int msgType, char *fmt,...)
{
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
             pBuffer[i++] = *p;
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
		       usprintf (&pBuffer[i], "%d", vald);
		       i += ustrlen (&pBuffer[i]);
		     }
		   else
		     i = MAX_PATH;
		   break;
		 case 's':
		   /* it is a string */
		   /* vals = va_arg (pa, char*); */
		   vals = va_arg (pa, char *);
		   if (vals)
		     {
		       lg = ustrlen (vals);
		       if (i + lg < MAX_PATH)
			 {
			   ustrcpy (&pBuffer[i], vals);
			   i += lg;
			 }
		       else
			 i = MAX_PATH;
		     }
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
}


/*----------------------------------------------------------------------
   TtaDisplaySimpleMessage construit un message simple.            
  ----------------------------------------------------------------------*/
void TtaDisplaySimpleMessage (int msgType, int origin, int number)
{
  char        *ptr;

  ptr = TtaGetMessage (origin, number);
#ifdef _WINDOWS
  if (msgType == FATAL)
    MessageBox (NULL, ptr, "Fatal error", MB_ICONERROR);
#else  /* _WINDOWS */
   TtaDisplayMessage (msgType, ptr);
#endif /* _WINDOWS */
}
