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
 * Handles error messages for Thot applications
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
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
     STRING*             TabMessages;
  }
TabMsg;

 /* Identification des messages Thot */
static PtrTabMsg    FirstTableMsg = NULL;
static CHAR_T       EmptyMsg [1];
static CHAR_T       result[MAX_TXT_LEN];
static char         ISOresult[MAX_TXT_LEN];

#include "dialogapi_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "registry_f.h"
#include "ustring_f.h"

#ifdef _WINDOWS
#define isascii(c) __isascii(c)
#endif

/*----------------------------------------------------------------------
   AsciiTranslate convertit les code d'accents du fichier de message 
   en accents.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*             AsciiTranslate (CHAR_T* pBuffer)
#else  /* __STDC__ */
CHAR_T*             AsciiTranslate (pBuffer)
CHAR_T*             pBuffer;

#endif /* __STDC__ */
{
   CHAR_T              nombre[4];
   int                 uniteid, dixid, centid;
   int                 i = 0, j = 0, k;

   while (pBuffer[i] != WC_EOS)
     {
	/* On lit jusqu'au premier backslash rencontre */
	while ((pBuffer[i] != TEXT('\\')) && (pBuffer[i] != WC_EOS))
	   result[j++] = pBuffer[i++];

	/* Teste si on est en presence de deux backslashs ou */
	/* si on se trouve devant un caractere special */
	if (pBuffer[i] != WC_EOS)
	   if (pBuffer[i + 1] == TEXT('\\'))
	     {
		/* On est dans le cas de deux backslashs consecutifs; on les prend */
		result[j++] = pBuffer[i++];
		result[j++] = pBuffer[i++];
	     }
	   else if (pBuffer[i + 1] == TEXT('n'))
	     {
		/* On est dans le cas d'un \n */
		i += 2;
		result[j++] = TEXT('\n');
	     }
	   else
	     {
		/* on saute le backslash */
		i++;
		/* on construit le nombre correspondant au caractere */
		k = 0;
		while ((pBuffer[i] >= TEXT('0'))
		       && (pBuffer[i] <= TEXT('9'))
		       && (pBuffer[i] != WC_EOS)
		       && (k <= 2))
		   nombre[k++] = pBuffer[i++];
		nombre[k] = WC_EOS;

		switch (ustrlen (nombre))
		      {
			 case 0:
			    result[j++] = pBuffer[i++];
			    break;
			 case 1:
			    uniteid = nombre[0] - TEXT('0');
			    result[j++] = uniteid;
			    break;
			 case 2:
			    uniteid = nombre[1] - TEXT('0');
			    dixid = nombre[0] - TEXT('0');
			    result[j++] = uniteid + 8 * dixid;
			    break;
			 case 3:
			    uniteid = nombre[2] - TEXT('0');
			    dixid = nombre[1] - TEXT('0');
			    centid = nombre[0] - TEXT('0');
			    result[j++] = uniteid + 8 * dixid + 64 * centid;
			    break;
		      }
	     }
     }
   result[j] = WC_EOS;
   return (result);
}

/*----------------------------------------------------------------------
   AsciiTranslate convertit les code d'accents du fichier de message 
   en accents.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char*               ISOAsciiTranslate (char* pBuffer)
#else  /* __STDC__ */
char*               ISOAsciiTranslate (pBuffer)
char*               pBuffer;

#endif /* __STDC__ */
{
   char                nombre[4];
   int                 uniteid, dixid, centid;
   int                 i = 0, j = 0, k;

   while (pBuffer[i] != EOS)
     {
	/* On lit jusqu'au premier backslash rencontre */
	while ((pBuffer[i] != '\\') && (pBuffer[i] != EOS))
	   ISOresult[j++] = pBuffer[i++];

	/* Teste si on est en presence de deux backslashs ou */
	/* si on se trouve devant un caractere special */
	if (pBuffer[i] != EOS)
	   if (pBuffer[i + 1] == '\\')
	     {
		/* On est dans le cas de deux backslashs consecutifs; on les prend */
		ISOresult[j++] = pBuffer[i++];
		ISOresult[j++] = pBuffer[i++];
	     }
	   else if (pBuffer[i + 1] == 'n')
	     {
		/* On est dans le cas d'un \n */
		i += 2;
		ISOresult[j++] = '\n';
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
			    ISOresult[j++] = pBuffer[i++];
			    break;
			 case 1:
			    uniteid = nombre[0] - '0';
			    ISOresult[j++] = uniteid;
			    break;
			 case 2:
			    uniteid = nombre[1] - '0';
			    dixid = nombre[0] - '0';
			    ISOresult[j++] = uniteid + 8 * dixid;
			    break;
			 case 3:
			    uniteid = nombre[2] - '0';
			    dixid = nombre[1] - '0';
			    centid = nombre[0] - '0';
			    ISOresult[j++] = uniteid + 8 * dixid + 64 * centid;
			    break;
		      }
	     }
     }
   ISOresult[j] = EOS;
   return (ISOresult);
}


/*----------------------------------------------------------------------
   TtaGetMessageTable alloue et initialise la table des            
   messages a` partir du fichier indique' par la variable msgName  
   pour l'application. Cet ensemble de messages va e^tre           
   identifie' par la valeur origine rendue par la fonction.        
   La fonction rend la valeur -1 si la table n'est pas alloue'e.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetMessageTable (CONST CHAR_T* msgName, int msgNumber)
#else  /* __STDC__ */
int                 TtaGetMessageTable (msgName, msgNumber)
CONST CHAR_T*       msgName;
int                 msgNumber;

#endif /* __STDC__ */
{
   CHAR_T*             s;
   FILE*               file;
   int                 origineid;
   int                 num;
   PtrTabMsg           currenttable;
   PtrTabMsg           previoustable;
   CHAR_T              pBuffer[MAX_TXT_LEN];
   /* CHAR_T              string[MAX_TXT_LEN]; */
   CHAR_T*             string;
   CHAR_T              fileName[MAX_TXT_LEN];
   unsigned char       pBuff[MAX_TXT_LEN];
   unsigned char*      ptrBuff;
#  ifdef _I18N_ 
   unsigned char       txt[MAX_TXT_LEN];
   CHARSET             encoding;
#  endif /* _I18N_ */

   /* contruction du nom $THOTDIR/bin/$LANG-msgName */
   ustrcpy (fileName, TtaGetVarLANG ());
   fileName[2] = TEXT('-');
   ustrcpy (&fileName[3], msgName);
   SearchFile (fileName, 2, pBuffer);
   file = ufopen (pBuffer, TEXT("r")); 
   if (file == NULL)
     {
	printf ("WARNING: cannot open file %s\n", pBuffer);
	return (-1);
     }
   else
     {
	/* Alloue une nouvelle table */
	currenttable = (PtrTabMsg) TtaGetMemory (sizeof (struct _TabMsg));

	currenttable->TabMessages = (STRING*) TtaGetMemory (sizeof (STRING) * msgNumber);

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

#   ifdef _I18N_ 
    fscanf (file, "%[^=]=%[^#\r\n]", pBuff, txt);
    if (!strcasecmp (pBuff, "charset")) {
       if (!strcasecmp (txt, "ISO Latin 1"))
          encoding = ISOLatin1;
       else if (!strcasecmp (txt, "ISO Latin 2"))
            encoding = ISOLatin2;
       else if (!strcasecmp (txt, "ISO Latin 6"))
            encoding = ISOLatin6;
       else if (!strcasecmp (txt, "Windows-1256"))
            encoding = WIN1256;
       else if (!strcasecmp (txt, "UTF8"))
            encoding = UTF8;
	} else {
           rewind (file);
           encoding = ISOLatin1;
	}
#   endif /* _I18N_ */

	/* Charge les messages */
	while (((fscanf (file, "%d %[^#\r\n]", &num, pBuff)) != EOF) && (num < msgNumber))
	  {
#        ifdef _I18N_
         string = TtaAllocString (MAX_TXT_LEN);
         ptrBuff = pBuff;
         TtaMBS2WCS (&ptrBuff, &string, encoding);
#        else /* !_I18N_ */
         string = pBuff;
#        endif /* !_I18N_ */
	     s = TtaAllocString (ustrlen (string) + 1);
	     ustrcpy (s, AsciiTranslate (string));
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
STRING              TtaGetMessage (int origin, int num)
#else  /* __STDC__ */
STRING              TtaGetMessage (origin, num)
int                 origin;
int                 num;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaDisplayMessage (int msgType, STRING fmt,...)
#else  /* __STDC__ */
void                TtaDisplayMessage (msgType, fmt,...)
int                 msgType;
STRING              fmt;
#endif /* __STDC__ */
{
   va_list             pa;
   int                 i, lg, vald;
   STRING              vals, p;
   CHAR_T              pBuffer[MAX_PATH];

#  ifdef _WINDOWS
   int len = ustrlen (fmt);
    for (i = 0; i < len; i++)
        if (fmt [i] == TEXT('\n'))
           fmt [i] = SPACE;
#  endif /* _WINDOWS */

   if (fmt) {
      /* construct the final message */
#     ifdef STDC_HEADERS
      va_start (pa, fmt);
#     else  /* STDC_HEADERS */
      va_start (pa);
#     endif /* STDC_HEADERS */
      i = 0;
      for (p = fmt; *p && i + 1 < MAX_PATH; p++) {
          if (*p != TEXT('%')) 
             pBuffer[i++] = *p;
          else {
               p++;
               switch (*p) {
                      case TEXT('d'):
                           /* it is a value */
                           vald = va_arg (pa, int);

                           if (i + 10 < MAX_PATH) {
                              usprintf (&pBuffer[i], TEXT("%d"), vald);
                              i += ustrlen (&pBuffer[i]);
						   } else
                                  i = MAX_PATH;
			               break;

                      case TEXT('s'):
                           /* it is a string */
                           /* vals = va_arg (pa, char*); */
                           vals = va_arg (pa, STRING);

                           lg = ustrlen (vals);
                           if (i + lg < MAX_PATH) {
                              ustrcpy (&pBuffer[i], vals);
                              i += lg;
						   } else
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
   TtaDisplayMessage (msgType, TtaGetMessage (origin, number));
}


/*----------------------------------------------------------------------
   DisplayPivotMessage traite les erreurs survenues a` la lecture  
   d'un fichier pivot.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayPivotMessage (STRING code)
#else  /* __STDC__ */
void                DisplayPivotMessage (code)
STRING              code;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   CHAR_T                pBuffer[THOT_MAX_CHAR];

   ustrncpy (pBuffer, code, THOT_MAX_CHAR);
   TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ERR_PIV), pBuffer);
#endif /* _WINDOWS */
}
