/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
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
#include "fileaccess.h"

#define THOT_EXPORT extern
#include "edit_tv.h"

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
static unsigned char * EmptyMsg = (unsigned char *)"";
static unsigned char   result[MAX_TXT_LEN];
static char            Dial_lan[3] = {EOS, EOS, EOS};
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
#if !defined(_WINDOWS) && !defined(_WX)
  ThotBool            skipAmp = TRUE;
#endif /* #if !defined(_WINDOWS) && !defined(_WX) */
  
  while (pBuffer[i] != EOS)
    {
      /* On lit jusqu'au premier backslash rencontre */
      while ((pBuffer[i] != '\\') && (pBuffer[i] != EOS))
        {
#if !defined(_WINDOWS) && !defined(_WX)
          if (skipAmp && pBuffer[i] == '&')
            {
              i++;
              skipAmp = FALSE;
            }
          else
#endif /* #if !defined(_WINDOWS) && !defined(_WX) */
            result[j++] = pBuffer[i++];
        }
      
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
              
              switch (strlen ((const char *)number))
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
  char               *s, *lan, *ptr;
  unsigned char       pBuff[MAX_TXT_LEN];
  int                 origineid;
  int                 num;

  /* contruction du nom $THOTDIR/bin/$LANG-msgName */
  if (Dial_lan[0] == EOS)
    {
      // save the requested langage
      lan = TtaGetVarLANG ();
      if (lan)
        strncpy (Dial_lan, lan, 3);
      Dial_lan[2] = EOS;
    }
  else
    lan = Dial_lan;
  strcpy (fileName, lan);
  fileName[2] = '-';
  strcpy (&fileName[3], msgName);
  SearchFile (fileName, 2, pBuffer);
  file = TtaReadOpen (pBuffer); 
  if (file == NULL)
    {
      /* force the english version */
      fileName[0] = 'e';
      fileName[1] = 'n';
      SearchFile (fileName, 2, pBuffer);
      file = TtaReadOpen (pBuffer); 
    }
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
      if (!strncasecmp ((const char *)pBuff, "encoding=", 9))
        {
          fscanf (file, "%500s\n]", pBuff);
          if (!strncasecmp ((const char *)pBuff, "utf8", 4))
            encoding = UTF_8;
          else
            encoding = ISO_8859_1;
        }
      else
        {
          fseek (file, 0L, 0);
          encoding = ISO_8859_1;
        }
      DialogCharset = TtaGetDefaultCharset ();
      /* Load messages */
      while (fscanf (file, "%d %[^#\r\n]", &num, pBuff) != EOF &&
             num < msgNumber)
        {
          s = (char *)TtaGetMemory (strlen ((const char *)pBuff) + 1);
          if (pBuff[0] == '*' && pBuff[1] == '*')
            strcpy (s, (const char *)AsciiTranslate ((char *)&pBuff[2]));
          else
            strcpy (s, (const char *)AsciiTranslate ((char *)pBuff));
#ifndef _WX
          if (encoding == UTF_8 && DialogCharset != UTF_8)
            {
              /* convert the string */
              ptr = (char *)TtaConvertMbsToByte ((unsigned char *)s, DialogCharset);
              currenttable->TabMessages[num] = ptr;
              TtaFreeMemory (s);
            }
          else
            {
              if (currenttable->TabMessages[num])
                TtaFreeMemory (s);
              else
                currenttable->TabMessages[num] = s;
            }
#endif /* _WX */
#ifdef _WX
          /* now we convert every strings to UTF-8 (DialogCharset) */
          if (encoding == DialogCharset)
            {
              /* string source is already in utf8 so no need to convert */
              currenttable->TabMessages[num] = s;
            }
          else
            {
              /* convert the string from Byte (ISO-LATIN-X) to Mbs (UTF-8) */
              ptr = (char *)TtaConvertByteToMbs ((unsigned char *)s, encoding);
              currenttable->TabMessages[num] = ptr;
              TtaFreeMemory (s);
            }
#endif /* _WX */

        }
      TtaReadClose (file);
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
  TtaGetMessage retourne le message correspondant a l'origine et 
  l'indice 0 a N donne.                                 
  ----------------------------------------------------------------------*/
char *TtaGetMessage (int origin, int num)
{
  int                 i;
  PtrTabMsg           currenttable;

  /* recherche la table de messages */
  if (origin == -1)
    /* la table de messages n'est pas chargee */
    return ((char *)EmptyMsg);
  i = 0;
  currenttable = FirstTableMsg;
  while (currenttable && i < origin)
    {
      currenttable = currenttable->TabNext;
      i++;
    }

  if (!currenttable)
    /* on n'a pas trouve la table de messages */
    return ((char *)EmptyMsg);
  else if (num >= currenttable->TabLength || num < 0)
    /* il n'y a pas de message */
    return ((char *)EmptyMsg);
  else if (!currenttable->TabMessages[num])
    /* il n'y a pas de message */
    return ((char *)EmptyMsg);
  else
    return ((char *)currenttable->TabMessages[num]);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaDisplayMessage (int msgType, const char *fmt,...)
{
  va_list             pa;
  int                 i, lg, vald;
  const char         *vals, *p;
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
                      sprintf (&pBuffer[i], "%d", vald);
                      i += strlen (&pBuffer[i]);
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
                      lg = strlen (vals);
                      if (i + lg < MAX_PATH)
                        {
                          strcpy (&pBuffer[i], vals);
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
      if (msgType == CONFIRM || msgType == FATAL)
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
#ifdef _WINGUI
  if (msgType == FATAL)
    MessageBox (NULL, ptr, "Fatal error", MB_ICONERROR);
  else if (msgType == CONFIRM)
    MessageBox (NULL, ptr, "Error", MB_OK);
#else  /* _WINGUI */
  TtaDisplayMessage (msgType, ptr);   
#endif /* _WINGUI */
}
