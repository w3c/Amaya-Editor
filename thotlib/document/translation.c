/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
  This module handles document translation.
  It travels the abstract trees of a document and produces an
  external representation according to a set of translation schemas.
*/
#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typetra.h"
#include "language.h"
#include "fileaccess.h"
#include "libmsg.h"
#include "appaction.h"
#include "appstruct.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "thotcolor_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
/*#include "HTMLnames.h"*/
/* maximum length of an output buffer */
#define MAX_BUFFER_LEN 1000
#define DATESTRLEN   80
/* maximum number of output buffers */
#define MAX_OUTPUT_FILES 10

/* information about an output file */
typedef struct _AnOutputFile
{
  FILE         *OfFileDesc;	        /* file descriptor */
  char          OfFileName[MAX_PATH];	/* file name */
  int           OfBufferLen;	        /* current length of output buffer */
  int           OfLineLen;              /* written length in the current line */
  int           OfIndent;	        /* current value of indentation */
  int           OfPreviousIndent;       /* previous value of indentation */
  int           OfLineNumber;           /* number of lines already written */
  unsigned char OfBuffer[MAX_BUFFER_LEN];	/* output buffer */
  ThotBool      OfStartOfLine;	        /* start a new line */
  ThotBool      OfAcceptLineBreak[MAX_BUFFER_LEN]; /* Line breaks are accepted
                                                      at that position */
  ThotBool      OfCannotOpen;	        /* open failure */
}
AnOutputFile;

/* Variables for date generation */
static ThotBool          StartDollar = FALSE;
static ThotBool          StartDate = FALSE;
static ThotBool          IgnoreDate = FALSE;
static ThotBool          Skip_Template = FALSE;
static ThotBool          RCSDollar = FALSE;
static ThotBool          RCSMarker = FALSE;
static unsigned char     RCSString[512];
static int               RCSIndex = 0;

/* number of output files in use */
static int          NOutFiles = 0;
/* the output files */
static AnOutputFile OutFile[MAX_OUTPUT_FILES];
/* entry 0: stdout    */
/* entry 1: main output file */
/* other entries: secondary output files */
/* directory of output files */
static char         fileDirectory[MAX_PATH];
/* name of main output file */
static char         fileName[MAX_PATH];
/* file extension */
static char         fileExtension[MAX_PATH];
static Proc4        GetEntityFunction = NULL;
static Proc3        GetDoctypeFunction = NULL;
static int          ExportLength = 0;
static ThotBool     ExportCRLF = FALSE;
static ThotBool     DocumentHasDocType = FALSE;
static ThotBool     WithMath = FALSE;

#include "absboxes_f.h"
#include "applicationapi_f.h"
#include "callback_f.h"
#include "content_f.h"
#include "exceptions_f.h"
#include "externalref_f.h"
#include "memory_f.h"
#include "presvariables_f.h"
#include "readprs_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "schtrad_f.h"
#include "structschema_f.h"
#include "translation_f.h"
#include "tree_f.h"
#include "thotmsg_f.h"
#include "uconvert_f.h"

static void ExportNsDeclaration (Document doc, PtrElement pNode);
static char* ExportElemNsPrefix (Document doc, PtrElement pNode);
static char* ExportAttrNsPrefix (Document doc, PtrElement pNode,
                                 PtrAttribute pAttr);
static void ExportXmlText (Document doc, PtrTextBuffer pBT, ThotBool lineBreak,
                           ThotBool translate, ThotBool entityName);
static void ApplyTRule (PtrTRule pTRule, PtrTSchema pTSch,
                        PtrSSchema pSSch, PtrElement pEl, ThotBool *transChar,
                        ThotBool *lineBreak, ThotBool *removeEl,
                        ThotBool *ignoreEl,
                        PtrPRule pRPres, PtrAttribute pAttr,
                        Document doc, ThotBool recordLineNb);
static void TranslateTree (PtrElement pEl, Document doc,
                           ThotBool transChar, ThotBool lineBreak,
                           ThotBool enforce, ThotBool recordLineNb);


/*----------------------------------------------------------------------
  TtaSetEntityFunction registers the function that gives entity names:
  procedure (int entityValue, Document doc, ThotBool withMath, char **entityName)
  ----------------------------------------------------------------------*/
void TtaSetEntityFunction (Proc4 procedure)
{
  GetEntityFunction = procedure;
}

/*----------------------------------------------------------------------
  TtaSetDoctypeFunction registers the function that say if the document
  has a doctype declaration.
  procedure (Document doc, ThotBool *found, ThotBool *useMath);
  ----------------------------------------------------------------------*/
void TtaSetDoctypeFunction (Proc3 procedure)
{
  GetDoctypeFunction = procedure;
}

/*----------------------------------------------------------------------
  SetVariableBuffer
  Copy string value into variable buffer named bufferName in translation
  schema pTSch.
  ----------------------------------------------------------------------*/
static void SetVariableBuffer (PtrTSchema pTSch, const char* bufferName, char* value)
{
  int     i, bufNum;

  bufNum = 0;
  for (i = 0; i < pTSch->TsNVarBuffers && bufNum == 0; i++)
    {
      if (!strcmp (pTSch->TsVarBuffer[i].VbIdent, bufferName))
        bufNum = pTSch->TsVarBuffer[i].VbNum;
    }
  if (bufNum > 0)
    {
      strncpy (pTSch->TsBuffer[bufNum - 1], value, MAX_TRANSL_BUFFER_LEN - 1);
      pTSch->TsBuffer[bufNum - 1][MAX_TRANSL_BUFFER_LEN-1] = EOS;
    }
}

/*----------------------------------------------------------------------
  GetSecondaryFile
  Return the secondary output file named fName.
  If open is True, the file is opened if it not open yet.
  Otherwise, the function returns 0 when the file is not open.
  ----------------------------------------------------------------------*/
static int GetSecondaryFile (char *fName, Document doc, ThotBool open)
{
  char                buff[MAX_BUFFER_LEN];
  int                 i;

  /* on cherche d'abord si ce nom de fichier est dans la table des */
  /* fichiers secondaires ouverts */
  /* on saute les deux premiers fichiers, qui sont stdout et le fichier de */
  /* sortie principal */
  for (i = 2;
       i < NOutFiles && strcmp (fName, OutFile[i].OfFileName);
       i++);
  if (i < NOutFiles && !strcmp (fName, OutFile[i].OfFileName))
    /* le fichier est dans la table, on retourne son rang */
    return i;
  else if (!open)
    return 0;
  else if (NOutFiles >= MAX_OUTPUT_FILES)
    /* table saturee */
    return -1;
  else
    {
      sprintf (buff, "%s%c%s", fileDirectory, DIR_SEP, fName);
      OutFile[NOutFiles].OfFileDesc = TtaWriteOpen (buff);
      if (OutFile[NOutFiles].OfFileDesc == NULL)
        {
          if (!OutFile[NOutFiles].OfCannotOpen)
            OutFile[NOutFiles].OfCannotOpen = TRUE;
        }
      else
        /* fichier ouvert */
        OutFile[NOutFiles].OfCannotOpen = FALSE;
      strcpy (OutFile[NOutFiles].OfFileName, fName);
      OutFile[NOutFiles].OfBufferLen = 0;
      OutFile[NOutFiles].OfLineLen = 0;
      OutFile[NOutFiles].OfIndent = 0;
      OutFile[NOutFiles].OfPreviousIndent = 0;
      OutFile[NOutFiles].OfLineNumber = 0;
      OutFile[NOutFiles].OfStartOfLine = TRUE;
      NOutFiles++;
      return (NOutFiles - 1);
    }
}


/*----------------------------------------------------------------------
  ExportChar writes the character c on the terminal or into the file buffer
  if fnum is not null.
  The file buffer is written when the line limit is reatched.
  If the parameter lineBreak is FALSE the pretty printing is desactivated.
  If the parameter translate is FALSE the character is written as this,
  in other case it will be translated according to the document encoding.
  If the parameter entityName is TRUE
  ----------------------------------------------------------------------*/
static void ExportChar (wchar_t c, int fnum, char *outBuf, Document doc,
                        ThotBool lineBreak, ThotBool translate, ThotBool entityName)
{
  PtrTSchema          pTSch;
  PtrDocument         pDoc;
  FILE               *fileDesc;
  CHARSET             charset;
  unsigned char       tmp[2];
  unsigned char       mbc[50], *ptr;
  char               *entity;
  int                 i, j, indent;
  int                 nb_bytes2write, index;
  int                 len, lineLen;
  Name                tsEOL, tsTranslEOL;

  pDoc = LoadedDocument[doc - 1];
  if (pDoc->DocDefaultCharset)
    // export entities when the charset is not explicit
    charset = US_ASCII;
  else
    charset = pDoc->DocCharset;
  nb_bytes2write = 0;
  if (translate)
    {
      if (c == START_ENTITY)
        {
          mbc[0] = '&';
          nb_bytes2write = 1;
        }
      else if (entityName &&
               (c == 0x22 || c == 0x26 || c == 0x3C || c == 0x3E || c == 0xA0))
        {
          if (c == 0x22) /* &quot; */
            {
              strcpy ((char *)&mbc[0], "&quot;");
              nb_bytes2write = 6;
            }
          else if (c == 0x26) /* &amp; */
            {
              strcpy ((char *)&mbc[0], "&amp;");
              nb_bytes2write = 5;
            }
          else if (c == 0x3C) /* &lt; */
            {
              strcpy ((char *)&mbc[0], "&lt;");
              nb_bytes2write = 4;
            }
          else if (c == 0x3E) /* &gt; */
            {
              strcpy ((char *)&mbc[0], "&gt;");
              nb_bytes2write = 4;
            }
          else if (c == 0xA0) /* &nbsp; */
            {
	      if (charset == UTF_8)
		{
		  nb_bytes2write = 2;
		  mbc[0] = (unsigned char)0xC2;
		  mbc[1] = (unsigned char)0xA0;
		}
	      else
		{
		  if (DocumentHasDocType && GetEntityFunction)
		    /* check if there is a DOCTYPE */
		    (*(Proc4)GetEntityFunction) ((void *)c, (void *)doc,
						 (void *)WithMath,
						 (void *)&entity);
		  else
		    entity = NULL;
		  if (entity)
		    {
		      /* alphanumeric entity accepted */
		      strcpy ((char *)&mbc[0], "&nbsp;");
		      nb_bytes2write = 6;
		    }
		  else
		    {
		      nb_bytes2write = 1;
		      mbc[0] = (unsigned char)0xA0;
		    }
		}
            }
        }
      /* translate the input character */
      else if ((c > 127 && charset == US_ASCII) ||
               (c > 255 && charset == ISO_8859_1))
        {
          /* generate an entity into an ASCII or ISO_8859_1 file */
          if (DocumentHasDocType && GetEntityFunction)
            (*(Proc4)GetEntityFunction) ((void *)c, (void *)doc,
                                         (void *)(WithMath && entityName),(void *)&entity);
          else
            entity = NULL;
          mbc[0] = '&';
          if (entity)
            {
              strncpy ((char *)&mbc[1], entity, 40);
              mbc[42] = EOS;
            }
          else
            {
              mbc[1] = '#';
              mbc[2] = 'x';
              sprintf ((char *)&mbc[3], "%x", (int)c);
            }
          nb_bytes2write = strlen ((char *)mbc);
          mbc[nb_bytes2write++] = ';';
        }
      else if (charset == UTF_8)
        {
          /* UTF-8 encoding */
          ptr = mbc;
          nb_bytes2write = TtaWCToMBstring ((wchar_t) c, &ptr);
        }
      else
        {
          /* other encodings */
          nb_bytes2write = 1;
          mbc[0] = TtaGetCharFromWC ((wchar_t) c, charset);
          if (mbc[0] == EOS && c != EOS)
            {
              /* generate an entity */
              if (DocumentHasDocType && GetEntityFunction)
                (*(Proc4)GetEntityFunction) ((void *)c, (void *)doc,
                                             (void *)WithMath, (void *)&entity);
              else
                entity = NULL;
              mbc[0] = '&';
              if (entity)
                {
                  strncpy ((char *)&mbc[1], entity, 40);
                  mbc[42] = EOS;
                }
              else
                {
                  mbc[1] = '#';
                  mbc[2] = 'x';
                  sprintf ((char *)&mbc[3], "%x", (int)c);
                }
              nb_bytes2write = strlen ((char *)mbc);
              mbc[nb_bytes2write++] = ';';
            }
        }
    }
  else
    {
      /* no translation */
      if (c == START_ENTITY)
        {
          mbc[0] = '&';
          nb_bytes2write = 1;
        }
      else
        {
          nb_bytes2write = 1;
          mbc[0] =  (unsigned char) c;
        }
    }

  if (outBuf != NULL)
    {
      /* write on the terminal */
      for (index = 0; index < nb_bytes2write; index++)
        {
          tmp[0] =  mbc[index];
          tmp[1] = EOS;
          strcat (outBuf, (char *)tmp);
        }
    }
  else if (fnum == 0)
    {
      /* write directly into the file */
      for (index = 0; index < nb_bytes2write; index++)
        putchar (mbc[index]);
    }
  else if (fnum > 0)
    {
      /* write into the file buffer */
      /* get the line length in the translation schema */
      pTSch = GetTranslationSchema (pDoc->DocSSchema);
      /* check if the user forced a line length */
      lineLen = ExportLength;
      strcpy (tsEOL, "\n");	      /* default end of line character */
      strcpy (tsTranslEOL, "\n"); /* default inserted end of line */
      if (lineLen == 0 && pTSch)
        {
          lineLen = pTSch->TsLineLength;
          strcpy (tsEOL, pTSch->TsEOL);
          strcpy (tsTranslEOL, pTSch->TsTranslEOL);
        }

      fileDesc = OutFile[fnum].OfFileDesc;
      if (fileDesc)
        {
          if (c == NEW_LINE)
            {
              /* end of line, write the buffer into the file */
              for (i = 0; i < OutFile[fnum].OfBufferLen; i++)
                putc ((int)OutFile[fnum].OfBuffer[i], fileDesc);

              if (ExportCRLF)
                /* generate a CR */
                putc (__CR__, fileDesc);
              /* generate a LF */		  
              fprintf (fileDesc, tsEOL);
              /* le buffer de sortie est vide maintenant */
              OutFile[fnum].OfBufferLen = 0;
              OutFile[fnum].OfLineLen = 0;
              OutFile[fnum].OfLineNumber++;
              OutFile[fnum].OfStartOfLine = TRUE;
            }
          else if (lineLen == 0)
            {
              /* no line length, write into the file directly */
              for (index = 0; index < nb_bytes2write; index++)
                putc (mbc[index], fileDesc);
              if (c == NEW_LINE)
                {
                  OutFile[fnum].OfLineNumber++;
                  if (ExportCRLF)
                    /* generate a CR */
                    putc (__CR__, fileDesc);
               }
            }
          else
            {
              /* other character */
              len = OutFile[fnum].OfBufferLen;
              if (lineBreak &&
                  OutFile[fnum].OfLineLen + len + nb_bytes2write > lineLen)
                {
                  /* cannot insert that new character in the line
                     look for a breaking space before */
                  i = len - 1;
                  while (i > 0 && (OutFile[fnum].OfBuffer[i] != SPACE ||
                                   !OutFile[fnum].OfAcceptLineBreak[i]))
                    i--;
                  j = i - 1;
                  /* check if there is a character before */
                  while (j > 0 && OutFile[fnum].OfBuffer[j] == SPACE)
                    j--;
                  if (j > 0)
                    {
                      /* write the content of the buffer */
                      for (j = 0; j < i; j++)
                        putc (OutFile[fnum].OfBuffer[j], fileDesc);
                      if (ExportCRLF)
                        /* generate a CR */
                        putc (__CR__, fileDesc);
                      /* generate a LF */
                      fprintf (fileDesc, tsTranslEOL);
                      OutFile[fnum].OfLineLen = 0;
                      OutFile[fnum].OfLineNumber++;
                      /* handle indentation */
                      if (OutFile[fnum].OfIndent > lineLen - 10)
                        indent = lineLen - 10;
                      else
                        indent = OutFile[fnum].OfIndent;
                      if (indent < 0)
                        indent = 0;
			    
                      for (j = 0; j < indent; j++)
                        OutFile[fnum].OfBuffer[j] = SPACE;
                      i = i - indent + 1;
                      if (i < 0)
                        i = 0;
                      /* on decale ce qui suit le blanc */
                      len -= i;
                      if (i > 0 )
                        for (j = indent; j < len; j++)
                          {
                            OutFile[fnum].OfBuffer[j] = OutFile[fnum].OfBuffer[i + j];
                            OutFile[fnum].OfAcceptLineBreak[j] = OutFile[fnum].OfAcceptLineBreak[i + j];
                          }
                      OutFile[fnum].OfStartOfLine = FALSE;
                    }
                }
              if (len + nb_bytes2write > MAX_BUFFER_LEN)
                {
                  /* the buffer is full, write it */
                  for (i = 0; i < len; i++)
                    putc (OutFile[fnum].OfBuffer[i], fileDesc);
                  /* register the previous written line lenght */
                  OutFile[fnum].OfLineLen = len;
                  len = 0;
                }
              if (OutFile[fnum].OfStartOfLine)
                {
                  if (OutFile[fnum].OfIndent >= MAX_BUFFER_LEN)
                    indent = MAX_BUFFER_LEN - 1;
                  else
                    {
                      indent = OutFile[fnum].OfIndent;
                      if (indent < 0)
                        indent = 0;
                    }
                  for (j = 0; j < indent; j++)
                    OutFile[fnum].OfBuffer[j] = SPACE;
                  len = indent;
                  OutFile[fnum].OfStartOfLine = FALSE;
                }

              /* avoid to generate a dot at position 0 of a line
                 as sendmail will duplication a such dot */
              if (len == 0 &&  mbc[0] == 46)
                OutFile[fnum].OfBuffer[len++] = SPACE;
              /* store the character into the buffer */
              for (index = 0; index < nb_bytes2write; index++)
                {
                  OutFile[fnum].OfBuffer[len] = mbc[index];
                  OutFile[fnum].OfAcceptLineBreak[len] = lineBreak;
                  len++;
                }
              OutFile[fnum].OfBufferLen = len;
            }
        }
    }
}


/*----------------------------------------------------------------------
  TtaGetTime returns the current date in a formatted string.
  Inspired from the hypermail source code: hypermail/src/date.c
  ----------------------------------------------------------------------*/
void TtaGetTime (char *s, CHARSET charset)
{
  time_t         tp;
  struct tm     *tmptr;
#ifdef _WINDOWS
  wchar_t        ws[DATESTRLEN];
  unsigned char *ptr;
#endif /* _WINDOWS */
  ThotBool   set_gmtime, set_isodate;

  time (&tp);
  s[0] = EOS;
  /* how to display dates */
  TtaGetEnvBoolean ("GTM_TIME", &set_gmtime);
  TtaGetEnvBoolean ("ISO_DATE", &set_isodate);
  if (set_gmtime)
    {
      tmptr = gmtime (&tp);
      if (set_isodate)
        strftime (s, DATESTRLEN, "%Y-%m-%d", tmptr);
      else
        strftime(s, DATESTRLEN, "%d-%m-%Y", tmptr);
    }
  else
    {
      tmptr =  localtime (&tp);
      if (set_isodate)
        {
#ifdef _WINDOWS
          wcsftime(ws, DATESTRLEN, L"%Y-%m-%d", tmptr);
          ptr = TtaConvertWCToByte (ws, charset);
          strncpy (s, (char *)ptr, DATESTRLEN);
          TtaFreeMemory (ptr);
#else /* _WINDOWS */
          strftime(s, DATESTRLEN, "%Y-%m-%d", tmptr);
#endif /* _WINDOWS */
        }
      else
        strftime(s, DATESTRLEN, "%d-%m-%Y", tmptr);
    }
}


/*----------------------------------------------------------------------
  CheckRCS checks if the current date should be generated and avoid
  to break the content of an RCS keyword.
  Returns TRUE if the character must be skipped.
  ----------------------------------------------------------------------*/
static ThotBool CheckRCS (wchar_t c, int fnum, char *outBuf, Document doc)
{
  PtrDocument pDoc;
  char        tm[DATESTRLEN];
  int         index;

  pDoc = LoadedDocument[doc - 1];

  if (StartDate)
    {
      if (c == '-' || c == '>' || c == SPACE)
        {
          /* keep this character */
          return FALSE;
        }
      else
        {
          /* generate the current date */
          if (pDoc->DocDefaultCharset)
            TtaGetTime (tm, US_ASCII);
          else
            TtaGetTime (tm, pDoc->DocCharset);
          for (index = 0; tm[index] != EOS; index++)
            ExportChar ((wchar_t) tm[index], fnum, outBuf, doc,
                        FALSE, FALSE, FALSE);
          StartDate = FALSE;
          IgnoreDate = TRUE;
        }
    }

  if (c == '$')
    {
      /* start/stop the analyse */
      if (IgnoreDate)
        /* close the previous date parsing */
        IgnoreDate = FALSE;
      if (!RCSDollar)
        {
          RCSDollar = TRUE;
          RCSIndex = 0;
        }
      else
        {
          RCSDollar = FALSE;
          RCSMarker = FALSE;
          RCSIndex = 0;
          ExportChar (c, fnum, outBuf, doc, FALSE, FALSE, FALSE);
          return TRUE;
        }
    }
  else if (RCSMarker)
    {
      ExportChar (c, fnum, outBuf, doc, FALSE, FALSE, FALSE);
      return TRUE;
    }
  else if (RCSDollar)
    {
      if (IgnoreDate)
        {
          if (c == '<')
            {
              /* implicit closing of the date parsing */
              IgnoreDate = FALSE;
              StartDollar = FALSE;
            }
          else
            /* it's a character of the previous date */
            return TRUE;
        }
      else if (c == ':' || c == '=')
        {
          if (RCSDollar && RCSIndex > 0 &&
	      ((!strncasecmp ((char *)RCSString, "Author", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "Header", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "Id", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "Locker", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "Log", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "Name", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "RCSfile", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "Revision", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "Source", RCSIndex)) ||
	       (!strncasecmp ((char *)RCSString, "State", RCSIndex))))
	    {
	      /* following characters will be skipped until the $ */
	      RCSMarker = TRUE;
	    }
	  else if (!StartDate && RCSIndex > 0 &&
		   !strncasecmp ((char *)RCSString, "Date", RCSIndex))
	    {
	      /* following characters will be skipped until the $ or EOL or EOS */
	      StartDate = TRUE;
	    }
	  else
	    {
	      RCSDollar = FALSE;
	      RCSMarker = FALSE;
	      return FALSE;
 	    }
          ExportChar ((wchar_t) c, fnum, outBuf, doc, FALSE, FALSE, FALSE);
	  return TRUE;
        }
      else if (c == EOS || c == EOL || RCSIndex > 8)
        {
          /* stop the analyse of the date */
          RCSDollar = FALSE;
	  RCSMarker = FALSE;
          StartDate = FALSE;
          IgnoreDate = FALSE;
        }
      else
	{
	  RCSString[RCSIndex++] = c;
	}
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  PutChar writes the character c on the terminal or into the file buffer
  if fnum is not null.
  The file buffer is written when the line limit is reatched.
  If the parameter lineBreak is FALSE the pretty printing is desactivated.
  If the parameter translate is FALSE the character is written as this,
  in other case it will be translated according to the document encoding.
  If the parameter entityName is TRUE
  ----------------------------------------------------------------------*/
static void PutChar (wchar_t c, int fnum, char *outBuf, Document doc,
                     ThotBool lineBreak, ThotBool translate,
                     ThotBool entityName)
{
  /* detect if the generation of a date is requested */
  if (fnum > 0 && CheckRCS (c, fnum, outBuf, doc))
    return;
  else
    ExportChar (c, fnum, outBuf, doc, lineBreak, translate, entityName);
}

/*----------------------------------------------------------------------
  PutColor generates the color name at the position n in the color table.
  ----------------------------------------------------------------------*/
static void PutColor (int n, int fnum, Document doc, ThotBool lineBreak)
{
  unsigned char       *ptr;
  unsigned char        c;
  int                  i;

  if (n < NColors && n >= 0)
    {
      ptr = (unsigned char *)Color_Table[n];
      i = 0;
      while (ptr[i] != EOS)
        {
          c = ptr[i++];
          PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, FALSE, FALSE);
        }
    }
}

/*----------------------------------------------------------------------
  PutPattern  sort dans fichier le nom du motif qui se trouve au	
  rang n dans la table des motifs.				
  ----------------------------------------------------------------------*/
static void PutPattern (int n, int fnum, Document doc, ThotBool lineBreak)
{
  unsigned char       *ptr;
  unsigned char        c;
  int                  i;

  if (n < NbPatterns && n >= 0)
    {
      ptr = (unsigned char *)Patterns[n];
      i = 0;
      while (ptr[i] != EOS)
        {
          c = ptr[i++];
          PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, FALSE, FALSE);
        }
    }
}

/*----------------------------------------------------------------------
  PutInt convertit le nombre n sous la forme d'une chaine de         
  caracteres et sort cette chaine de caracteres dans fichier      
  ----------------------------------------------------------------------*/
static void PutInt (int n, int fnum, char *outBuf, Document doc,
                    ThotBool lineBreak)
{
  unsigned char       buffer[20];
  int                 i;

  sprintf ((char *)buffer, "%d", n);
  i = 0;
  while (buffer[i] != EOS)
    PutChar ((wchar_t) buffer[i++], fnum, outBuf, doc, lineBreak,
             FALSE, FALSE);
}

/*----------------------------------------------------------------------
  ExportXmlBuffer                    
  ----------------------------------------------------------------------*/
static void ExportXmlBuffer (Document doc, unsigned char *buffer,
                    ThotBool lineBreak)
{
  unsigned char    c;
  int              i, fnum;
  
  fnum = 1;
  i = 0;
  if (buffer)
    while (buffer[i] != EOS)
      {
        c = buffer[i++];
        PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, FALSE, FALSE);
      }
}

/*----------------------------------------------------------------------
  GetTransSchForContent
  En examinant les elements ascendants de pEl, on cherche un schema de
  traduction qui contienne des regles de traduction de contenu pour les
  feuilles de type leafType.
  ----------------------------------------------------------------------*/
static PtrTSchema GetTransSchForContent (PtrElement pEl, LeafType leafType,
                                         ScriptTransl **pTransAlph)
{
  PtrTSchema   pTSch, pTSch1;
  PtrSSchema   pSS;
  PtrElement   pAncestor;
  int          i;
  char         script;
  ThotBool     transExist;
   
  pSS = NULL;
  transExist = FALSE;
  pTSch = NULL;
  pAncestor = pEl;
  *pTransAlph = NULL;
  if (pEl->ElTerminal && pEl->ElLeafType == LtText && pEl->ElLanguage < 4)
    script = TtaGetScript (pEl->ElLanguage);
  else
    script = 'L';
  do
    {
      if (pSS != pAncestor->ElStructSchema)
        /* un schema de structure different du precedent rencontre */
        {
          pSS = pAncestor->ElStructSchema;
          /* schema de traduction de cette structure */
          pTSch1 = GetTranslationSchema (pSS);
          if (pTSch1)
            /* there is a translation schema for this structure */
            {
              /* if we don't find any other translation schema, we will return
                 that one */
              pTSch = pTSch1;
              switch (leafType)
                {
                case LtText:
                  if (pTSch->TsNTranslScripts > 0)
                    /* il y a au moins un script a traduire */
                    /* cherche les regles de traduction pour l'script */
                    /* de la feuille */
                    {
                      i = 0;
                      do
                        {
                          *pTransAlph = &pTSch->TsTranslScript[i++];
                          if ((*pTransAlph)->AlScript == script &&
                              (*pTransAlph)->AlBegin > 0)
                            transExist = TRUE;
                          else
                            *pTransAlph = NULL;
                        }
                      while (!transExist && i < pTSch->TsNTranslScripts);
                    }
                  break;
                case LtSymbol:
                  transExist = pTSch->TsSymbolFirst != 0;
                  break;
                case LtGraphics:
                case LtPolyLine:
                  transExist = pTSch->TsGraphicsFirst != 0;
                  break;
                default:
                  break;
                }
            }
        }
      pAncestor = pAncestor->ElParent;
    }
  while (!transExist && pAncestor != NULL);
  return pTSch;
}


/*----------------------------------------------------------------------
  TranslateText translate the text according to the current translation
  table.
  When the parameter attrVal is TRUE doublequotes are translated into
  the entity &quot;.
  entityName is TRUE when displaying an entity string.
  markupPreserve is TRUE when <,>, and & should be preserved.
  ----------------------------------------------------------------------*/
static void TranslateText (PtrTextBuffer pBufT, PtrTSchema pTSch,
                           ScriptTransl *pTransAlph, ThotBool lineBreak,
                           int fnum, Document doc, ThotBool attrVal,
                           ThotBool entityName, ThotBool markupPreserve)
{
  PtrTextBuffer        pNextBufT, pPrevBufT;
  CHAR_T              c, cs;
  StringTransl        *pTrans;   
  int                  textTransBegin, textTransEnd;
  int                  i, j, k, b, ft, lt;
  ThotBool             continu, equal, stop;

  if (!pBufT)
    return;
  textTransBegin = pTransAlph->AlBegin;
  textTransEnd = pTransAlph->AlEnd;
  b = 0;       /* indice dans la chaine source de la regle de traduction */
  i = 1;       /* indice dans le buffer du caractere a traduire */
  ft = textTransBegin;/* indice de la 1ere regle de traduction a appliquer */
  lt = textTransEnd; /*indice de la derniere regle de traduction a appliquer*/
  pPrevBufT = NULL;  /* buffer source precedent */
  c = pBufT->BuContent[0];     /* 1er caractere a traduire */
   
  /* traduit la suite des buffers source */
  do
    /* Dans la table de traduction, les chaines sources sont */
    /* rangees par ordre scriptique. On cherche une chaine */
    /* source qui commence par le caractere a traduire. */
    {
      if (markupPreserve &&
          (c == 0x26 || c == 0x3C || c == 0x3E))
        /* preserve the markup <, >, and & */
        PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, FALSE, entityName);
      else
        {
          while (c > (CHAR_T) (pTSch->TsCharTransl[ft - 1].StSource[b]) &&
                 ft < lt)
            ft++;
          pTrans = &pTSch->TsCharTransl[ft - 1];
          if (c == (CHAR_T) pTrans->StSource[b])
            {
              /* le caractere correspond au caractere courant de la */
              /* chaine source de la regle ft */
              if (pTrans->StSource[b + 1] == EOS)
                /* chaine complete */
                /* cette regle de traduction s'applique, on traduit */
                /* cherche si les regles suivantes ne peuvent pas egalement */
                /* s'appliquer: recherche la plus longue chaine a traduire */
                {
                  continu = ft < textTransEnd;
                  while (continu)
                    {
                      j = 0;
                      equal = TRUE;
                      /* compare la regle ft avec la suivante */
                      do
                        {
                          if (pTSch->TsCharTransl[ft - 1].StSource[j] ==
                              pTSch->TsCharTransl[ft].StSource[j])
                            j++;
                          else
                            equal = FALSE;
                        }
                      while (equal && j <= b);
		      
                      if (!equal)
                        /* le debut de la regle suivante est different */
                        /* de la regle courante */
                        continu = FALSE;
                      else
                        /* la fin de la regle suivante est-il identique */
                        /* a la suite du texte a traduire ? */
                        {
                          k = i;
                          cs = c;
                          pNextBufT = pBufT;
                          /* cherche le caractere suivant du texte */
                          stop = FALSE;
                          do
                            {
                              if (cs != EOS)
                                cs = (CHAR_T) pNextBufT->BuContent[k++];
                              if (cs == EOS)
                                /* passe au buffer suivant du meme texte */
                                if (pNextBufT->BuNext != NULL)
                                  {
                                    pNextBufT = pNextBufT->BuNext;
                                    k = 1;
                                    cs = pNextBufT->BuContent[0];
                                  }
                              if (cs == EOS)
                                continu = FALSE;	/* fin du texte */
                              else
                                {
                                  continu = FALSE;
                                  if (cs == (CHAR_T) pTSch->TsCharTransl[ft].StSource[j])
                                    {
                                      stop = FALSE;
                                      continu = TRUE;
                                      j++;
                                    }
                                  if (pTSch->TsCharTransl[ft].StSource[j] == EOS)
                                    {
                                      ft++;
                                      b = j - 1;
                                      i = k;
                                      c = cs;
                                      pBufT = pNextBufT;
                                      continu = ft < textTransEnd;
                                      stop = TRUE;
                                    }
                                }
                            }
                          while (!stop && continu);
                        }
                    }

                  /* on applique la regle de traduction ft */
                  j = 0;
                  while (pTSch->TsCharTransl[ft - 1].StTarget[j] != EOS)
                    {
                      cs = pTSch->TsCharTransl[ft - 1].StTarget[j];
                      PutChar ((wchar_t) cs, fnum, NULL, doc, lineBreak,
                               FALSE, entityName);
                      j++;
                    }
                  /* prepare la prochaine recherche dans la table */
                  b = 0;
                  ft = textTransBegin;
                  lt = textTransEnd;
                }
              else
                /* ce n'est pas le dernier caractere de la chaine */
                /* source de la table de traduction, on restreint la */
                /* partie de la table de traduction dans laquelle on */
                /* cherchera les caracteres suivants */
                {
                  j = ft;
                  /* cherche parmi les regles suivantes la derniere */
                  /* qui contienne ce caractere a cette position dans */
                  /* la chaine source. On ne cherchera pas au-dela de */
                  /* cette regle. */
                  while (c == (CHAR_T) pTSch->TsCharTransl[j - 1].StSource[b] &&
                         j < lt)
                    j++;
                  if (c != (CHAR_T) pTSch->TsCharTransl[j - 1].StSource[b])
                    lt = j - 1;
                  /* passe au caractere suivant de la chaine source */
                  /* de la table de traduction */
                  b++;
                }
            }
          else if (b == 0)
            /* le caractere ne se trouve au debut d'aucune chaine source de */
            /* la table de traduction, on ne le traduit donc pas */
            {
              ft = textTransBegin;
              if (attrVal &&
                  (c == 0x22 || c == 0x26 || c == 0x3C || c == 0x3E || c == 0xA0))
                entityName = TRUE;
              if (c != EOS)
                PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, TRUE,
                         entityName);
            }
          else
            /* on avait commence' a analyser une sequence de caracteres. */
            /* Cette sequence ne se traduit pas, on sort le premier */
            /* caractere de la sequence et on cherche une sequence */
            /* traduisible a partir du caractere suivant. */
            {
              if (i - b >= 1)
                /* le premier caractere de la sequence est dans */
                /* le buffer courant */
                i -= b;
              else
                /* le premier caractere de la sequence est dans */
                /* le buffer precedent */
                {
                  pBufT = pPrevBufT;
                  i = pBufT->BuLength + i - b;
                }

              if (c != EOS)
                {
                  cs = (CHAR_T) pBufT->BuContent[i - 1];
                  if (attrVal &&
                      (cs == 0x22 || cs == 0x26 || cs == 0x3C ||
                       cs == 0x3E || cs == 0xA0))
                    entityName = TRUE;
                  PutChar ((wchar_t) cs, fnum, NULL, doc, lineBreak, TRUE,
                           entityName);
                }
              b = 0;
              ft = textTransBegin;
              lt = textTransEnd;
            }
        }

      /* cherche le caractere suivant a traiter */
      if (c != EOS)
        c = (CHAR_T) pBufT->BuContent[i++];
      if (c == EOS && pBufT->BuNext != NULL)
        {
          /* passe au buffer suivant du meme element de texte */
          pPrevBufT = pBufT;
          pBufT = pBufT->BuNext;
          i = 1;
          c = (CHAR_T) pBufT->BuContent[0];
        }
    }
  while (c != EOS);
  /* fin de la feuille de texte */
  /* Si on a commence' a analyser une sequence de caracteres, */
  /* on sort le debut de la sequence. */
  for (i = 0; i <= b - 1; i++)
    {
      c = (CHAR_T) pTSch->TsCharTransl[ft - 1].StSource[i];
      if (attrVal &&
          (c == 0x22 || c == 0x26 || c == 0x3C || c == 0x3E || c == 0xA0))
        entityName = TRUE;
      PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, TRUE, entityName);
    }
}


/*----------------------------------------------------------------------
  TranslateLeaf   traite l'element feuille pointe' par pEl, en	
  traduisant son contenu si transChar est vrai. Produit le	
  contenu dans le fichier de sortie fnum.			
  ----------------------------------------------------------------------*/
static void TranslateLeaf (PtrElement pEl, ThotBool transChar,
                           ThotBool lineBreak, int fnum, Document doc)
{
  PtrTSchema          pTSch;
  PtrTextBuffer       pBufT;
  ScriptTransl       *pTransAlph;
  StringTransl       *pTrans;
  PtrElement          pParent;
  CHAR_T              c = 0;
  char                ci;
  int                 i, j, b, ft, lt;
  ThotBool            entityName, encode, markupPreserve;
  
  pTransAlph = NULL;
  lt = 0;
  if (!(pEl->ElLeafType == LtText || pEl->ElLeafType == LtSymbol ||
        pEl->ElLeafType == LtGraphics || pEl->ElLeafType == LtPolyLine) ||
      !transChar)
    pTSch = GetTranslationSchema (pEl->ElStructSchema);
  else
    /* En examinant les elements englobants, on cherche un schema de */
    /* traduction qui contienne des regles pour ce type de feuille */
    pTSch = GetTransSchForContent (pEl, pEl->ElLeafType, &pTransAlph);
  switch (pEl->ElLeafType)
    {
    case LtText /* traitement d'une feuille de texte */ :
      if (pEl->ElTextLength > 0)
        /* la feuille n'est pas vide */
        {
          pBufT = pEl->ElText;	/* 1er buffer a traiter */
          /* characters are encoded
             except for hidden (internal) elements */
          pParent = pEl->ElParent;
          if (pParent)
            encode = !TypeHasException (ExcHidden, pParent->ElTypeNumber,
                                        pParent->ElStructSchema);
          else
            encode = TRUE;
          markupPreserve = TypeHasException (ExcMarkupPreserve,
                                             pParent->ElTypeNumber,
                                             pParent->ElStructSchema);
          entityName = !strcmp (pEl->ElStructSchema->SsName, "MathML");
          if (!pTransAlph || !transChar)
            /* on ne traduit pas quand la table de traduction est vide */
            /* parcourt les buffers de l'element */
            while (pBufT)
              {
                i = 0;
                while (pBufT->BuContent[i] != EOS)
                  {
                    c = pBufT->BuContent[i++];
                    if (encode)
                      PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, TRUE,
                               entityName);
                    else
                      PutChar ((wchar_t) c, fnum, NULL, doc, lineBreak, FALSE,
                               entityName);
                  }
                pBufT = pBufT->BuNext;
              }
          else if (pTSch)
            /* effectue les traductions de caracteres selon la table */
            TranslateText (pBufT, pTSch, pTransAlph, lineBreak, fnum,
                           doc, FALSE, entityName, markupPreserve);
        }
      break;
    case LtSymbol:
    case LtGraphics:
    case LtPolyLine:
      /* if it's an Unicode character, output its code */
      if (pEl->ElLeafType == LtSymbol && pEl->ElGraph == '?')
        PutChar ((wchar_t) pEl->ElWideChar, fnum, NULL, doc, lineBreak, TRUE,
                 FALSE);
      else if (pTSch != NULL)
        {
          if (!transChar)
            ft = 0;
          else if (pEl->ElLeafType == LtSymbol)
            /* cherche la premiere et la derniere regle de traduction */
            /* a appliquer a l'element */
            {
              ft = pTSch->TsSymbolFirst;
              lt = pTSch->TsSymbolLast;
            }
          else
            {
              ft = pTSch->TsGraphicsFirst;
              lt = pTSch->TsGraphicsLast;
            }
          /* prend dans c le caractere qui represente la forme graphique */
          if (pEl->ElLeafType == LtPolyLine)
            ci = pEl->ElPolyLineType;
          else
            ci = pEl->ElGraph;
          if (ft == 0)
            /* pas de traduction */
            {
              if (ci != EOS)
                {
                  if (pEl->ElLeafType != LtSymbol)
                    PutChar ((wchar_t) ci, fnum, NULL, doc, lineBreak, TRUE, FALSE);
                  else
                    switch (ci)
                      {
                      case 1:
                        c = 0x301a; /* LeftDoubleBracket */
                        break;
                      case 2:
                        c = 0x301b; /* RightDoubleBracket */
                        break;
                      case 3:
                        c = 0x230a; /* LeftFloor */
                        break;
                      case 4:
                        c = 0x230b; /* RightFloor */
                        break;
                      case 5:
                        c = 0x2308; /* LeftCeiling */
                        break;
                      case 6:
                        c = 0x2309; /* RightCeiling */
                        break;
                      case 7:
                        c = 0x2758; /* VerticalSeparator */
                        break;
                      case 8 : /* HorizontalLine */
                        c = 0x2500;
                        break;
                      case 9 : /* UnderBar */
                        c = 0x332;
                        break;
                      case 10 : /* OverBar */
                        c = 0xAF;
                        break;
                      case 11:
                        c = 0x2223; /* VerticalBar */
                        break;
                      case 12:
                        c = 0x2956; /* DoubleVerticalBar */
                        break;

                      case '1':
                        c = 0x2231; /* Clockwise Integral */
                        break;
                      case '2':
                        c = 0x2232; /* Clockwise Contour Integral */
                        break;
                      case '3':
                        c = 0x2233; /* Counter Clockwise Contour Integral */
                        break;
                      case 'b':
                        c = 0x23B4; /* Over bracket */
                        break;
                      case 'B':
                        c = 0x23B5; /* Under bracket */
                        break;
                      case 'c':
                        c = 0x222E; /* contour integral */
                        break;
                      case 'd':
                        c = 0x222C; /* double integral */
                        break;
                      case 'e':
                        c = 0x222F; /* double contour integral */
                        break;
                      case 'f':
                        c = 0x2230; /* triple contour integral */
                        break;
                      case 'h':
                        c = 0x00AF; /* overline */
                        break;
                      case 'H':
                        c = 0x0302; /* hat */
                        break;
                      case 'i':
                        c = 0x222B; /* integral */
                        break;
                      case 'k':
                        c = 0x02C7; /* hacek */
                        break;
                      case 'o':
                        c = 0xFE37; /* over brace */
                        break;
                      case 'p':
                        c = 0xFE35; /* over parenthesis */
                        break;
                      case 'q':
                        c = 0xFE36; /* under parenthesis */
                        break;
                      case 'r':
                        c = 0x221A; /* square root */
                        break;
                      case 'u':
                        c = 0xFE38; /* under brace */
                        break;
                      case 'v':
                        c = 0x007C; /* vertical line */
                        break;
                      case 'D':
                        c = 0x2225; /* double vertical line */
                        break;
                      case 'I':
                        c = 0x22C2; /* n-ary intersection */
                        break;
                      case 'P':
                        c = 0x220F; /* n-ary product */
                        break;
                      case 'S':
                        c = 0x2211; /* n-ary summation */
                        break;
                      case 't':
                        c = 0x222D; /* triple integral */
                        break;
                      case 'T':
                          c = 0x2DC; /* Diacritical Tilde */
                        break;
                      case 'U':
                        c = 0x22C3; /* n-ary union */
                        break;
                      case '<':
                        c = 0x27E8; /* mathematical left angle bracket */
                        break;
                      case '>':
                        c = 0x27E9; /* mathematical right angle bracket */
                        break;
                      case '|':
                        c = 0x2223; /* divides */
                        break;

                      case 'L': /* LeftArrow */
                        c = 0x2190;
                      break;
                      case '^': /* UpArrow */
                        c = 0x2191;
                      break;
                      case 'R': /* RightArrow */
                        c = 0x2192;
                      break;
                      case 'V': /* DownArrow */
                        c = 0x2193;
                      break;
                      case 'A': /* LeftRightArrow */
                        c = 0x2194;
                      break;
                      case 155: /* UpDownArrow */
                        c = 0x2195;
                      break;
                      case 156: /* UpperLeftArrow */
                        c = 0x2196;
                      break;
                      case 157: /* UpperRightArrow */
                        c = 0x2197;
                      break;
                      case 158: /* LowerRightArrow */
                        c = 0x2198;
                      break;
                      case 159: /* LowerLeftArrow */
                        c = 0x2199;
                      break;
                      case 160: /* LeftTeeArrow */
                        c = 0x21A4;
                      break;
                      case 161: /* UpTeeArrow */
                        c = 0x21A5;
                      break;
                      case 162: /* RightTeeArrow */
                        c = 0x21A6;
                      break;
                      case 163: /* DownTeeArrow */
                        c = 0x21A7;
                      break;
                      case 164: /* LeftVector */
                        c = 0x21BC;
                      break;
                      case 165: /* DownLeftVector */
                        c = 0x21BD;
                      break;
                      case 166: /* RightUpVector */
                        c = 0x21BE;
                      break;
                      case 167: /* LeftUpVector */
                        c = 0x21BF;
                      break;
                      case 168: /* RightVector */
                        c = 0x21C0;
                      break;
                      case 169: /* DownRightVector */
                        c = 0x21C1;
                      break;
                      case 170: /* RightDownVector */
                        c = 0x21C2;
                      break;
                      case 171: /* LeftDownVector */
                        c = 0x21C3;
                      break;
                      case 172: /* RightArrowLeftArrow */
                        c = 0x21C4;
                      break;
                      case 173: /* UpArrowDownArrow */
                        c = 0x21C5;
                      break;
                      case 174: /* LeftArrowRightArrow */
                        c = 0x21C6;
                      break;
                      case 175: /* DoubleLeftArrow */
                        c = 0x21D0;
                      break;
                      case 176: /* DoubleUpArrow */
                        c = 0x21D1;
                      break;
                      case 177: /* DoubleRightArrow */
                        c = 0x21D2;
                      break;
                      case 178: /* DoubleDownArrow */
                        c = 0x21D3;
                      break;
                      case 179: /* DoubleLeftRightArrow */
                        c = 0x21D4;
                      break;
                      case 180: /* DoubleUpDownArrow */
                        c = 0x21D5;
                      break;
                      case 181: /* LeftArrowBar */
                        c = 0x21E4;
                      break;
                      case 182: /* RightArrowBar */
                        c = 0x21E5;
                      break;
                      case 183: /* DownArrowUpArrow */
                        c = 0x21F5;
                      break;
                      case 184: /* LongLeftArrow */
                        c = 0x27F5;
                      break;
                      case 185: /* LongRightArrow */
                        c = 0x27F6;
                      break;
                      case 186: /* LongLeftRightArrow */
                        c = 0x27F7;
                      break;
                      case 187: /* DoubleLongLeftArrow */
                        c = 0x27F8;
                      break;
                      case 188: /* DoubleLongRightArrow */
                        c = 0x27F9;
                      break;
                      case 189: /* DoubleLongLeftRightArrow */
                        c = 0x27FA;
                      break;
                      case 190: /* UpArrowBar */
                        c = 0x2912;
                      break;
                      case 191: /* DownArrowBar */
                        c = 0x2913;
                      break;
                      case 192: /* LeftRightVector */
                        c = 0x294E;
                      break;
                      case 193: /* RightUpDownVector */
                        c = 0x294F;
                      break;
                      case 194: /* DownLeftRightVector */
                        c = 0x2950;
                      break;
                      case 195: /* LeftUpDownVector */
                        c = 0x2951;
                      break;
                      case 196: /* LeftVectorBar */
                        c = 0x2952;
                      break;
                      case 197: /* RightVectorBar */
                        c = 0x2953;
                      break;
                      case 198: /* RightUpVectorBar */
                        c = 0x2954;
                      break;
                      case 199: /* RightDownVectorBar */
                        c = 0x2955;
                      break;
                      case 200: /* DownLeftVectorBar */
                        c = 0x2956;
                      break;
                      case 201: /* DownRightVectorBar */
                        c = 0x2957;
                      break;
                      case 202: /* LeftUpVectorBar */
                        c = 0x2958;
                      break;
                      case 203: /* LeftDownVectorBar */
                        c = 0x2959;
                      break;
                      case 204: /* LeftTeeVector */
                        c = 0x295A;
                      break;
                      case 205: /* RightTeeVector */
                        c = 0x295B;
                      break;
                      case 206: /* RightUpTeeVector */
                        c = 0x295C;
                      break;
                      case 207: /* RightDownTeeVector */
                        c = 0x295D;
                      break;
                      case 208: /* DownLeftTeeVector */
                        c = 0x295E;
                      break;
                      case 209: /* DownRightTeeVector */
                        c = 0x295F;
                      break;
                      case 210: /* LeftUpTeeVector */
                        c = 0x2960;
                      break;
                      case 211: /* LeftDownTeeVector */
                        c = 0x2961;
                      break;
                      case 212: /* UpEquilibrium */
                        c = 0x296E;
                      break;
                      case 213: /* ReverseUpEquilibrium */
                        c = 0x296F;
                      break;

                      default:
                        c = (CHAR_T)ci;
                      }
                  PutChar (c, fnum, NULL, doc, lineBreak, TRUE, FALSE);
                }
            }
          else
            /* on traduit l'element */
            /* cherche le symbole dans les chaines sources de la */
            /* table de traduction */
            {
              while (pTSch->TsCharTransl[ft - 1].StSource[0] < ci && ft < lt)
                ft++;
              if (pTSch->TsCharTransl[ft - 1].StSource[0] == ci)
                /* il y a une regle de traduction pour ce symbole */
                {
                  b = 0;
                  pTrans = &pTSch->TsCharTransl[ft - 1];
                  while (pTrans->StTarget[b] != EOS)
                    {
                      ci = pTrans->StTarget[b];
                      PutChar ((wchar_t) ci, fnum, NULL, doc,
                               lineBreak, TRUE, FALSE);
                      b++;
                    }
                }
              else
                /* ce symbole ne se traduit pas */
                if (ci != EOS)
                  PutChar ((wchar_t) ci, fnum, NULL, doc, lineBreak, TRUE, FALSE);
            }
          if (pEl->ElLeafType == LtPolyLine && pEl->ElNPoints > 0)
            /* la ligne a au moins un point de controle */
            /* on ecrit les coordonnees des points de controle */
            {
              pBufT = pEl->ElPolyLineBuffer;	/* 1er buffer a traiter */
              /* parcourt les buffers de l'element */
              while (pBufT != NULL)
                {
                  for (i = 0; i < pBufT->BuLength; i++)
                    {
                      PutChar ((wchar_t) ' ', fnum, NULL, doc, lineBreak,
                               FALSE, FALSE);
                      PutInt (pBufT->BuPoints[i].XCoord, fnum, NULL, doc,
                              lineBreak);
                      PutChar ((wchar_t) ',', fnum, NULL, doc, lineBreak,
                               FALSE, FALSE);
                      PutInt (pBufT->BuPoints[i].YCoord, fnum, NULL, doc,
                              lineBreak);
                    }
                  pBufT = pBufT->BuNext;
                }
            }
        }
      break;
      
    case LtPicture:
      /* Si le schema de traduction comporte un buffer */
      /* pour les images, le nom du fichier contenant l'image */
      /* est range' dans ce buffer */
      if (pTSch != NULL && pTSch->TsPictureBuffer > 0)
        {
          b = pTSch->TsPictureBuffer;
          pTSch->TsBuffer[b - 1][0] = EOS;	/* raz du buffer */
          if (pEl->ElTextLength > 0)
            /* la feuille n'est pas vide */
            {
              j = 0;
              pBufT = pEl->ElText;	/* 1er buffer a traiter */
              /* parcourt les buffers de l'element */
              while (pBufT != NULL)
                {
                  i = 0;
                  do
                    pTSch->TsBuffer[b - 1][j++] = (char) pBufT->BuContent[i++];
                  while (pBufT->BuContent[i - 1] != EOS &&
                         b < MAX_TRANSL_BUFFER_LEN);
                  pBufT = pBufT->BuNext;
                }
              if (j > 0)
                pTSch->TsBuffer[b - 1][j - 1] = EOS;
            }
        }
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  PresRuleValue      retourne le code caractere de la valeur de la   
  regle de presentation specifique pointee par pPRule.            
  ----------------------------------------------------------------------*/
static unsigned char PresRuleValue (PtrPRule pPRule)
{
  unsigned char        val;

  val = SPACE;
  switch (pPRule->PrType)
    {
    case PtFont:
    case PtStyle:
    case PtWeight:
    case PtVariant:
    case PtUnderline:
    case PtThickness:
    case PtDirection:
    case PtUnicodeBidi:
    case PtVisibility:
    case PtLineStyle:
      val = pPRule->PrChrValue;
      break;
    case PtHyphenate:
      if (pPRule->PrBoolValue)
        val = 'Y';
      else
        val = 'N';
      break;
    case PtAdjust:
      switch (pPRule->PrAdjust)
        {
        case AlignLeft:
          val = 'L';
          break;
        case AlignRight:
          val = 'R';
          break;
        case AlignCenter:
          val = 'C';
          break;
        case AlignLeftDots:
          val = 'D';
          break;
        case AlignJustify:
          val = 'J';
          break;
        }
      break;
    default:
      val = SPACE;
      break;
    }
  return val;
}


/*----------------------------------------------------------------------
  EmptyElement       retourne TRUE si l'element pEl est vide ou n'a  
  que des descendants vides.                                      
  ----------------------------------------------------------------------*/
static ThotBool     EmptyElement (PtrElement pEl)
{
  PtrElement          pChild;
  ThotBool            empty, specialGraphic, isImg;

  empty = TRUE;
  if (pEl->ElTerminal)
    /* l'element est une feuille. On traite selon le type de feuille */
    switch (pEl->ElLeafType)
      {
      case LtText:
      case LtPicture:
        empty = (pEl->ElTextLength == 0);
        break;
      case LtGraphics:
      case LtSymbol:
        empty = (pEl->ElGraph == EOS);
        break;
      case LtPageColBreak:
        /* un saut de page est consideree comme vide */
        empty = TRUE;
        break;
      case LtReference:
        if (pEl->ElReference != NULL)
          if (pEl->ElReference->RdReferred != NULL)
            empty = FALSE;
        break;
      case LtPairedElem:
        /* un element de paire n'est jamais considere' comme vide */
        empty = FALSE;
        break;
      case LtPolyLine:
        empty = (pEl->ElNPoints == 0);
        break;
      case LtPath:
        empty = (pEl->ElFirstPathSeg == NULL);
      default:
        empty = FALSE;
        break;
      }
  else
    /* ce n'est pas une feuille, on traite recursivement tous les fils */
    {
      specialGraphic = TypeHasException (ExcEmptyGraphic, pEl->ElTypeNumber,
                                         pEl->ElStructSchema);
      isImg = TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema);
      pChild = pEl->ElFirstChild;
      while (pChild && empty)
        {
          if (specialGraphic)
            /* this is a SVG element such as rect, circle, ellipse, etc. that
               has to be considered empty even if it contains a Thot Graphic
	       Leaf element */
            {
              if (!pChild->ElTerminal ||
                  (pChild->ElLeafType != LtPicture &&
                   pChild->ElLeafType != LtGraphics &&
                   pChild->ElLeafType != LtPath &&
                   pChild->ElLeafType != LtPolyLine) ||
                  pEl->ElStructSchema != pChild->ElStructSchema)
                {
                  /* this is not a Thot graphics basic element */
                  if (!pChild->ElSource)
                    /* this is not a transcluded element */
                    empty = FALSE;
                  else if (pChild->ElSource->RdTypeRef != RefInclusion)
                    /* this is not a transcluded element */
                    empty = FALSE;
                }
            }
          else if (isImg)
            /* this is a HTML img of SVG image that
               has to be considered empty even if it contains a PICTURE Leaf
               element */
            {
              if (!pChild->ElTerminal || pChild->ElLeafType != LtPicture)
                /* this is not a Thot picture basic element */
                empty = FALSE;
            }
          else /* if (!EmptyElement (pChild)) */
            empty = FALSE;
          if (empty)
            pChild = pChild->ElNext;
        }
    }
  return empty;
}

/*----------------------------------------------------------------------
  ConditionIsTrue   evalue la condition du bloc de regles pointe' par
  pBlock pour l'element pointe' par pEl et l'attribut pointe' par 
  pAttr s'il est different de NULL. Retourne vrai si la condition est
  satisfaite, faux sinon.                                         
  ----------------------------------------------------------------------*/
static ThotBool ConditionIsTrue (PtrTRuleBlock pBlock, PtrElement pEl,
                                 PtrAttribute pAttr, Document doc)
{
  PtrAttribute        pAttrEl;
  PtrSSchema          pSS, pRefSS;
  PtrElement          pEl1, pElem, pSibling;
  PtrSRule            pSRule;
  PtrPRule            pPRule;
  TranslCondition    *Cond;
  PtrReference        pRef;
  int                 i, nCond;
  ThotBool            ret, possibleRef, typeOK, stop;

  if (pBlock == NULL || pBlock->TbNConditions == 0)
    /* no condition */
    ret = TRUE;
  else
    /* il y a au moins une condition a evaluer */
    {
      ret = TRUE;
      nCond = 0;
      /* evalue les conditions du bloc jusqu'a en trouver une fausse */
      while (ret && nCond < pBlock->TbNConditions)
        {
          Cond = &pBlock->TbCondition[nCond++];
          if (!Cond->TcTarget)
            pElem = pEl;
          else
            /* la condition porte sur l'element pointe' par pEl ou pAttr.
               on cherche cet element pointe' */
            {
              pElem = NULL;
              if (pAttr != NULL &&
                  pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum-1]->AttrType ==
                  AtReferenceAttr)
                /* c'est un attribut reference */
                pRef = pAttr->AeAttrReference;
              else
                /* l'element est-il une reference ? */
                if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
                  pRef = pEl->ElReference;
                else
                  /* c'est peut-etre une inclusion */
                  pRef = pEl->ElSource;
              if (pRef != NULL)
                pElem = ReferredElement (pRef);
            }
          if (pElem == NULL)
            ret = FALSE;
          else
            {
              /* on cherche l'element concerne' par la condition */
              if (Cond->TcAscendRelLevel > 0)
                /* la condition concerne l'ascendant de rang TcAscendRelLevel */
                for (i = 1; pElem != NULL && i <= Cond->TcAscendRelLevel; i++)
                  pElem = pElem->ElParent;
              else if (Cond->TcAscendType != 0)
                /* la condition concerne l'element ascendant de type TcAscendType */
                {
                  typeOK = FALSE;
                  pElem = pElem->ElParent;
                  while (!typeOK && pElem != NULL)
                    {
                      if (Cond->TcAscendNature[0] == EOS)
                        /* le type de l'ascendant est defini dans le meme schema de
                           structure que l'element traite' */
                        typeOK = EquivalentSRules (Cond->TcAscendType,
                                                   pEl->ElStructSchema,
                                                   pElem->ElTypeNumber,
                                                   pElem->ElStructSchema,
                                                   pElem->ElParent);
                      else
                        /* le type de l'ascendant est defini dans un autre schema */
                        if (strcmp (Cond->TcAscendNature,
                                    pElem->ElStructSchema->SsName) == 0)
                          typeOK = EquivalentSRules (Cond->TcAscendType,
                                                     pElem->ElStructSchema,
                                                     pElem->ElTypeNumber,
                                                     pElem->ElStructSchema,
                                                     pElem->ElParent);
                      pElem = pElem->ElParent;
                    }
                }
              if (pElem == NULL)
                ret = FALSE;
              else
                /* traitement selon le type de condition */
                switch (Cond->TcCondition)
                  {
                  case TcondFirst:
                    pSibling = pElem->ElPrevious;
                    /* on saute les marques de page precedentes */
                    BackSkipPageBreak (&pSibling);
                    ret = (pSibling == NULL);
                    break;

                  case TcondLast:
                    /* on saute les marques de page suivantes */
                    pSibling = pElem->ElNext;
                    stop = FALSE;
                    do
                      if (pSibling == NULL)
                        stop = TRUE;
                      else if (pSibling->ElTypeNumber == PageBreak + 1)
                        pSibling = pSibling->ElNext;
                      else
                        stop = TRUE;
                    while (!stop);
                    /* l'element est dernier s'il n'a pas de succcesseur */
                    ret = pSibling == NULL;
                    break;
	       
                  case TcondReferred:
                    /* la condition est satisfaite si l'element (ou le premier de
                       ses ascendants sur lequel peut porter une reference) est
                       reference' au moins une fois. */
                    ret = FALSE;
                    pEl1 = pElem;
                    possibleRef = FALSE;
                    do
                      {
                        if (pEl1->ElReferredDescr != NULL)
                          /* l'element est reference' */
                          ret = pEl1->ElReferredDescr->ReFirstReference != NULL;
                        if (Cond->TcAscendRelLevel > -1)
                          /* on s'interesse seulement a l'element pElem. On ne va pas
                             chercher plus loin. */
                          pEl1 = NULL;
                        else
                          /* on cherche le premier ascendant referencable si l'element
                             pElem lui-meme n'est pas reference' */
                          if (!ret)
                            /* l'element n'est pas reference' */
                            {
                              /* l'element peut-il etre designe' par une reference? */
                              pRefSS = pEl1->ElStructSchema;
                              /* on cherche toutes les references dans le schema de
                                 structure de l'element */
                              possibleRef = FALSE;
                              i = 1;
                              do
                                {
                                  pSRule = pRefSS->SsRule->SrElem[i++];
                                  if (pSRule->SrConstruct == CsReference)
                                    /* c'est une reference */
                                    if (pSRule->SrReferredType != 0)
                                      possibleRef = EquivalentSRules (pSRule->SrReferredType,
                                                                      pRefSS, pEl1->ElTypeNumber, pRefSS,
                                                                      pEl1->ElParent);
                                }
                              while (!possibleRef && i < pRefSS->SsNRules);
                              if (!possibleRef)
                                {
                                  /* l'element ne peut pas etre designe par un element
                                     reference on cherche s'il peut etre designe' par un
                                     attribut reference on cherche tous les attributs
                                     reference dans le schema de structure de l'element */
                                  i = 1;
                                  do
                                    {
                                      if (pRefSS->SsAttribute->TtAttr[i++]->AttrType ==
                                          AtReferenceAttr)
                                        /* c'est une reference */
                                        if (pRefSS->SsAttribute->TtAttr[i - 1]->AttrTypeRef != 0)
                                          possibleRef = (pRefSS->SsAttribute->TtAttr[i-1]->
                                                         AttrTypeRefNature[0] == EOS &&
                                                         /* meme schema de structure */
                                                         EquivalentSRules (pRefSS->SsAttribute->TtAttr[i-1]->
                                                                           AttrTypeRef,
                                                                           pRefSS, pEl1->ElTypeNumber, pRefSS,
                                                                           pEl1->ElParent));
                                    }
                                  while (!possibleRef && i < pRefSS->SsNAttributes);
                                }
                              if (!possibleRef)
                                /* l'element ne peut pas etre designe' par une reference
                                   type'e ; on examine l'element ascendant */
                                pEl1 = pEl1->ElParent;
                            }
                      }
                    while (!possibleRef && pEl1 != NULL && !ret);
                    break;
	       
                  case TcondFirstRef:
                  case TcondLastRef:
                    /* la condition est satisfaite s'il s'agit de la premiere
                       (ou de la derniere) reference a l'element reference' */
                    pRef = NULL;
                    if (pAttr != NULL &&
                        pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->
                        AttrType == AtReferenceAttr)
                      /* c'est un attribut reference */
                      pRef = pAttr->AeAttrReference;
                    else if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
                      /* l'element est une reference */
                      pRef = pElem->ElReference;
                    if (pRef != NULL)
                      if (Cond->TcCondition == TcondFirstRef)
                        ret = pRef->RdPrevious == NULL;
                      else
                        ret = pRef->RdNext == NULL;
                    else
                      ret = FALSE;
                    break;
	       
                  case TcondExternalRef:
                    /* la condition est satisfaite s'il s'agit d'un */
                    /* element ou d'un attribut reference externe */
                    pRef = NULL;
                    ret = FALSE;
                    if (pAttr != NULL &&
                        pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->
                        AttrType == AtReferenceAttr)
                      /* c'est un attribut reference */
                      pRef = pAttr->AeAttrReference;
                    else  if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
                      /* l'element est-il une reference ? */
                      pRef = pElem->ElReference;
                    else
                      /* c'est peut-etre une inclusion */
                      pRef = pElem->ElSource;
                    if (pRef && pRef->RdReferred)
                      ret = TRUE;
                    break;

                  case TcondFirstWithin:
                  case TcondWithin:
                    /* condition sur le nombre d'ancetres d'un type donne' */
                    pEl1 = pElem->ElParent;
                    if (pEl1 == NULL)
                      /* aucun ancetre, condition non satisfaite */
                      ret = FALSE;
                    else
                      {
                        if (Cond->TcCondition == TcondFirstWithin)
                          {
                            pSibling = pElem->ElPrevious;
                            /* on saute les marques de page precedentes */
                            BackSkipPageBreak (&pSibling);
                            ret = (pSibling == NULL);
                          }
                        if (ret)
                          {
                            if (Cond->TcElemNature[0] == EOS)
                              /* le type de l'ascendant est defini dans le meme schema
                                 de structure que l'element traite' */
                              pSS = pEl->ElStructSchema;
                            else
                              /*le type de l'ascendant est defini dans un autre schema*/
                              pSS = NULL;
                            i = 0;
                            if (Cond->TcImmediatelyWithin)
                              /* Condition: If immediately within n element-type */
                              /* Les n premiers ancetres successifs doivent etre du type
                                 TcElemType, sans comporter d'elements d'autres type */
                              /* on compte les ancetres successifs de ce type */
                              while (pEl1)
                                {
                                  if (pSS)
                                    typeOK = EquivalentType (pEl1, Cond->TcElemType, pSS);
                                  else if (strcmp (Cond->TcElemNature,
                                                   pEl1->ElStructSchema->SsName) == 0)
                                    typeOK = EquivalentType (pEl1, Cond->TcElemType,
                                                             pEl1->ElStructSchema);
                                  else
                                    typeOK = FALSE;
                                  if (typeOK)
                                    {
                                      i++;
                                      pEl1 = pEl1->ElParent;
                                    }
                                  else
                                    pEl1 = NULL;
                                }
                            else
                              /* Condition: If within n element-type */
                              /* on compte tous les ancetres de ce type */
                              while (pEl1 != NULL)
                                {
                                  if (pSS != NULL)
                                    typeOK = EquivalentType (pEl1, Cond->TcElemType, pSS);
                                  else if (strcmp (Cond->TcElemNature,
                                                   pEl1->ElStructSchema->SsName) == 0)
                                    typeOK = EquivalentType (pEl1, Cond->TcElemType,
                                                             pEl1->ElStructSchema);
                                  else
                                    typeOK = FALSE;
                                  if (typeOK)
                                    i++;
                                  pEl1 = pEl1->ElParent; /* passe a l'element ascendant */
                                }
                            if (Cond->TcAscendRel == RelEquals)
                              ret = i == Cond->TcAscendLevel;
                            else if (Cond->TcAscendRel == RelGreater)
                              ret = i > Cond->TcAscendLevel;
                            else if (Cond->TcAscendRel == RelLess)
                              ret = i < Cond->TcAscendLevel;
                          }
                      }
                    break;

                  case TcondAttr:
                    /* cherche si l'element ou un de ses ascendants possede */
                    /* l'attribut cherche' avec la valeur cherchee */
                    ret = FALSE;
                    pSS = pEl->ElStructSchema;
                    pEl1 = pElem;
                    while (pEl1 != NULL && !ret)
                      {
                        pAttrEl = pEl1->ElFirstAttr;	/* 1er attribut de l'element */
                        /* parcourt les attributs de l'element */
                        while (pAttrEl != NULL && !ret)
                          {
                            if (!strcmp (pAttrEl->AeAttrSSchema->SsName, pSS->SsName) &&
                                pAttrEl->AeAttrNum == Cond->TcAttr)
                              /* c'est l'attribut cherche', on teste sa valeur selon
                                 son type */
                              switch (pSS->SsAttribute->TtAttr[pAttrEl->AeAttrNum - 1]->AttrType)
                                {
                                case AtNumAttr:
                                  ret = pAttrEl->AeAttrValue <= Cond->TcUpperBound &&
                                    pAttrEl->AeAttrValue >= Cond->TcLowerBound;
                                  break;
                                case AtTextAttr:
                                  if (Cond->TcTextValue[0] == EOS)
                                    ret = TRUE;
                                  else
                                    ret = StringAndTextEqual (Cond->TcTextValue,
                                                              pAttrEl->AeAttrText);
                                  break;
                                case AtReferenceAttr:
                                  ret = TRUE;
                                  break;
                                case AtEnumAttr:
                                  ret = pAttrEl->AeAttrValue == Cond->TcAttrValue ||
                                    Cond->TcAttrValue == 0;
                                  break;
                                default:
                                  break;
                                }
                            if (!ret)
                              pAttrEl = pAttrEl->AeNext;
                          }
                        if (Cond->TcAscendRelLevel == -1)
                          /* on peut poursuivre la recherche parmi les ascendants */
                          pEl1 = pEl1->ElParent;   /* passe a l'element ascendant */
                        else
                          /* la recherche de l'attribut ne porte que sur l'element
                             lui-meme. On arrete la. */
                          pEl1 = NULL;
                      }
                    break;

                  case TcondPRule:
                    /* cherche si l'element possede la presentation cherchee */
                    /* avec la valeur cherchee */
                    ret = FALSE;
                    pPRule = pElem->ElFirstPRule;
                    /* parcourt les presentations specifiques de l'element */
                    while (pPRule != NULL && !ret)
                      {
                        if (pPRule->PrType == (PRuleType)(Cond->TcAttr))
                          {
                            /* c'est la presentation cherchee, on teste sa valeur */
                            if (pPRule->PrType == PtSize ||
                                pPRule->PrType == PtIndent||
                                pPRule->PrType == PtLineSpacing ||
                                pPRule->PrType == PtLineWeight)
                              /* c'est une presentation a valeur numerique */
                              ret = pPRule->PrMinValue <= Cond->TcUpperBound &&
                                pPRule->PrMinValue >= Cond->TcLowerBound;
                            else if (pPRule->PrType == PtFillPattern ||
                                     pPRule->PrType == PtBackground ||
                                     pPRule->PrType == PtForeground ||
				     pPRule->PrType == PtColor ||
				     pPRule->PrType == PtStopColor)
                              ret = pPRule->PrIntValue <= Cond->TcUpperBound &&
                                pPRule->PrIntValue >= Cond->TcLowerBound;
                            else
                              ret = Cond->TcPresValue == PresRuleValue (pPRule) ||
                                Cond->TcPresValue == EOS;
                          }
                        if (!ret)
                          pPRule = pPRule->PrNextPRule;
                      }
                    break;

                  case TcondElementType:
                    /* cherche si l'attribut porte sur un element
                       du type voulu */
                    ret = FALSE;
                    if (pAttr != NULL)
                      {
                        if (!pElem->ElParent)
                          /* That's the Document node */
                          {
                            if (pElem->ElTypeNumber == Cond->TcAttr)
                              ret = TRUE;
                          }
                        else
                          {
                            if (pElem->ElTypeNumber == Cond->TcAttr &&
                                !strcmp (pElem->ElStructSchema->SsName,
                                         pAttr->AeAttrSSchema->SsName))
                              ret = TRUE;
                          }
                      }
                    break;

                  case TcondPresentation:
                    /* la condition est satisfaite si l'element */
                    /* porte des regles de presentation specifique */
                    ret = pElem->ElFirstPRule != NULL;
                    break;

                  case TcondScript:
                    /* la condition porte sur l'script */
                    if (pElem->ElTypeNumber == CharString + 1)
                      ret = (TtaGetScript(pElem->ElLanguage) == Cond->TcScript);
                    break;

                  case TcondAttributes:
                    /* la condition est satisfaite si l'element porte des
                       attributs */
                    ret = pElem->ElFirstAttr != NULL;
                    break;

                  case TcondFirstAttr:
                    /* la condition est satisfaite si le bloc attribut pAttr est
                       le 1er de l'element */
                    if (pAttr != NULL)
                      ret = pAttr == pEl->ElFirstAttr;
                    break;

                  case TcondLastAttr:
                    /* la condition est satisfaite si le bloc attribut pAttr est le
                       dernier de l'element */
                    if (pAttr != NULL)
                      ret = pAttr->AeNext == NULL;
                    break;

                  case TcondComputedPage:
                    /* la condition est satisfaite si l'element est un saut de
                       page calcule */
                    if (pElem->ElTypeNumber == PageBreak + 1)
                      ret = pElem->ElPageType == PgComputed;
                    break;

                  case TcondStartPage:
                    /* la condition est satisfaite si l'element est un saut de
                       page de debut */
                    if (pElem->ElTypeNumber == PageBreak + 1)
                      ret = pElem->ElPageType == PgBegin;
                    break;

                  case TcondUserPage:
                    /* la condition est satisfaite si l'element est un saut de page
                       utilisateur */
                    if (pElem->ElTypeNumber == PageBreak + 1)
                      ret = pElem->ElPageType == PgUser;
                    break;

                  case TcondEmpty:
                    /* la condition est satisfaite si l'element est vide */
                    ret = EmptyElement (pElem);
                    break;

                  case TcondRoot:
                    /* la condition est satisfaite si l'element est la racine */
                    ret = (pElem->ElParent &&
                           pElem->ElParent->ElTypeNumber ==
                           pElem->ElParent->ElStructSchema->SsDocument);
                    break;

		  case TcondTransclusion:
		    /* condition is true if element is a transclusion */
                    ret = (pElem->ElSource &&
			   pElem->ElSource->RdTypeRef == RefInclusion);
		    break;

                  default:
                    break;
                  }
              if (Cond->TcNegativeCond)
                ret = !ret;
            }
        }
    }
  return ret;
}


/*----------------------------------------------------------------------
  Counter_Val     retourne la valeur du compteur de numero countNum	
  (defini dans le schema de traduction  pointe' par pTSch qui     
  s'applique au schema de structure pointe' par pSS) pour         
  l'element pointe' par pElNum.                                   
  ----------------------------------------------------------------------*/
static int Counter_Val (int countNum, PtrTSchema pTSch, PtrSSchema pSS,
                        PtrElement pElNum)
{
  PtrElement          pEl;
  PtrSSchema          pSSch;
  TCounter           *pCntr;
  PtrAttribute        pAttr;
  int                 val, valInit, level;
  ThotBool            initAttr, stop;

#define MAX_ANCESTOR 50
  PtrElement          pAncest[MAX_ANCESTOR];

  valInit = 0;
  pCntr = &pTSch->TsCounter[countNum - 1];
  if (pCntr->TnOperation == TCntrNoOp)
    val = pCntr->TnParam1;
  else
    {
      /* a priori, la valeur initiale du compteur ne depend pas d'un */
      /* attribut */
      initAttr = FALSE;
      if (pCntr->TnAttrInit > 0)
        /* la valeur initiale du compteur est definie par un attribut */
        {
          /* remonte a  la racine de l'arbre auquel appartient l'element */
          /* a numeroter */
          pEl = pElNum;
          while (pEl->ElParent != NULL)
            pEl = pEl->ElParent;
          /* cet attribut est-il present sur la racine de l'arbre auquel */
          /* appartient l'element a numeroter ? */
          pAttr = pEl->ElFirstAttr;
          stop = FALSE;	/* parcourt les attributs de la racine */
          do
            if (pAttr == NULL)
              stop = TRUE;	/* dernier attribut */
            else if (pAttr->AeAttrNum == pCntr->TnAttrInit &&
                     !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName))
              stop = TRUE;	/* c'est l'attribut cherche' */
            else
              pAttr = pAttr->AeNext;	/* au suivant */
          while (!stop);
          if (pAttr != NULL)
            /* la racine porte bien l'attribut qui initialise le compteur */
            {
              initAttr = TRUE;
              valInit = pAttr->AeAttrValue;	/* on prend la valeur de l'attribut */
            }
        }
      if (pCntr->TnOperation == TCntrRLevel)
        {
          pEl = pElNum;
          val = 0;
          /* parcourt les elements englobants de l'element sur lequel porte */
          /* le calcul du compteur */
          while (pEl != NULL)
            {
              if (pEl->ElTypeNumber == pCntr->TnElemType1 &&
                  !strcmp (pEl->ElStructSchema->SsName, pElNum->ElStructSchema->SsName))
                /* l'element rencontre' a la meme type que l'element traite' */
                val++;	/* incremente le compteur */
              pEl = pEl->ElParent;
            }
        }
      else
        {
          if (pCntr->TnElemType1 == PageBreak + 1)
            /* c'est un compteur de pages */
            {
              pSSch = NULL;
              pEl = pElNum;
            }
          else
            {
              /* schema de structure du compteur */
              pSSch = pElNum->ElStructSchema;
              if (pCntr->TnOperation != TCntrRank || pCntr->TnAcestorLevel == 0)
                /* Cherche le premier element de type TnElemType1 */
                /* englobant l'element a numeroter */
                pEl = GetTypedAncestor (pElNum, pCntr->TnElemType1, pSSch);
              else
                {
                  /* Cherche le nieme element de type TnElemType1 qui englobe */
                  /* l'element a numeroter */
                  if (pCntr->TnAcestorLevel < 0)
                    /* on compte les ascendants en remontant de l'element */
                    /* concerne' vers la racine */
                    level = -pCntr->TnAcestorLevel;
                  else
                    /* on compte les ascendants en descendant de la racine vers */
                    /* l'element concerne'. Pour cela on commence par enregistrer */
                    /* le chemin de l'element concerne' vers la racine */
                    level = MAX_ANCESTOR;
                  pEl = pElNum;
                  while (level > 0 && pEl != NULL)
                    {
                      if (pEl->ElTypeNumber == pCntr->TnElemType1 &&
                          !strcmp (pEl->ElStructSchema->SsName, pElNum->ElStructSchema->SsName))
                        /* cet element englobant a le type qui incremente le compteur */
                        {
                          level--;
                          pAncest[level] = pEl;
                        }
                      if (level > 0)
                        pEl = pEl->ElParent;
                    }
                  if (pCntr->TnAcestorLevel > 0)
                    /* il faut redescendre a partir de la racine */
                    pEl = pAncest[level + pCntr->TnAcestorLevel - 1];
                }
            }
          if (pEl == NULL)
            val = 0;	/* pas trouve' */
          else if (pCntr->TnOperation == TCntrRank)
            if (pCntr->TnElemType1 == PageBreak + 1)
              /* c'est un compteur de pages */
              {
                if (pEl->ElTypeNumber == PageBreak + 1 && pEl->ElViewPSchema == 1)
                  val = 1;
                else if (initAttr)
                  val = valInit;
                else
                  val = 0;
                /* compte les marques de page qui precedent l'element */
                do
                  {
                    pEl = BackSearchTypedElem (pEl, PageBreak + 1, NULL, NULL);
                    if (pEl != NULL)
                      /* cas page rappel supprime */
                      /* on ne compte que les marques de page de la vue 1 */
                      if (pEl->ElViewPSchema == 1)
                        val++;
                  }
                while (pEl != NULL);
              }
            else
              /* numero = rang de l'element dans la liste */
              /* Cherche le rang de l'element trouve' parmi ses freres */
              {
                if (initAttr)
                  val = valInit;
                else
                  val = 1;
                while (pEl->ElPrevious != NULL)
                  {
                    pEl = pEl->ElPrevious;
                    if (EquivalentType (pEl, pCntr->TnElemType1, pSSch))
                      /* on ne compte pas les eventuelles marques de page */
                      val++;
                  }
              }
          else
            /* l'element trouve est celui qui reinitialise le compteur */
            {
              if (initAttr)
                val = valInit - pCntr->TnParam2;
              else
                val = pCntr->TnParam1 - pCntr->TnParam2;
              /* a partir de l'element trouve', cherche en avant tous les */
              /* elements ayant le type qui incremente le compteur, */
              /* jusqu'a rencontrer celui pour lequel on calcule la valeur du */
              /* compteur. */
              do
                {
                  pEl = FwdSearchElem2Types (pEl, pCntr->TnElemType2,
                                             pElNum->ElTypeNumber, pSSch,
                                             pElNum->ElStructSchema, NULL);
                  if (pEl != NULL)
                    if (EquivalentType (pEl, pCntr->TnElemType2, pSSch))
                      /* on ignore les pages qui ne concernent pas la vue 1 */
                      if (pEl->ElTypeNumber != PageBreak + 1 ||
                          (pEl->ElTypeNumber == PageBreak + 1 &&
                           pEl->ElViewPSchema == 1))
                        val += pCntr->TnParam2;
                }
              while (pEl != NULL && pEl != pElNum);
            }
        }
    }
  return val;
}


/*----------------------------------------------------------------------
  SearchDescent   cherche dans le sous-arbre de racine pEl (racine	
  exclue), un element de type typeNum defini dans le schema de    
  structure pointe' par pSS, ou si pSS est nul dans le schema de  
  nom schemaName.                                                 
  ----------------------------------------------------------------------*/
static void SearchDescent (PtrElement * pEl, int typeNum, PtrSSchema pSS,
                           Name schemaName)
{
  PtrElement          pChild;
  PtrSSchema          pSSchema;
  ThotBool            SSchemaOK;

  if ((*pEl)->ElTerminal)
    *pEl = NULL;		/* element terminal, echec */
  else if ((*pEl)->ElFirstChild == NULL)
    *pEl = NULL;		/* pas de fils, echec */
  else
    /* il y a au moins un descendant direct */
    {
      pChild = (*pEl)->ElFirstChild;
      /* cherche dans le sous-arbre de chacun des fils */
      do
        {
          /* le fils a-t-il le type cherche' ? */
          if (pSS == NULL)
            {
              SSchemaOK = !strcmp (schemaName, pChild->ElStructSchema->SsName);
              pSSchema = pChild->ElStructSchema;
            }
          else
            {
              SSchemaOK = !strcmp (pChild->ElStructSchema->SsName, pSS->SsName);
              pSSchema = pSS;
            }
          if (SSchemaOK && EquivalentSRules (typeNum, pSSchema,
                                             pChild->ElTypeNumber, pSSchema, *pEl))
            /* trouve' */
            *pEl = pChild;
          else
            {
              *pEl = pChild;
              SearchDescent (pEl, typeNum, pSS, schemaName);
              if (*pEl == NULL)
                pChild = pChild->ElNext;
            }
        }
      while ((*pEl) == NULL && pChild != NULL);
    }
}


/*----------------------------------------------------------------------
  PutContent   e'crit le contenu des feuilles de l'element pEl dans  
  dans le fichier.                                                
  La traduction du contenu des feuilles a lieu seulement si       
  transChar est vrai.                                             
  ----------------------------------------------------------------------*/
static void PutContent (PtrElement pEl, ThotBool transChar, ThotBool lineBreak,
                        int fnum, Document doc)
{
  PtrElement          pChild;

  if (!pEl->ElTerminal)
    /* l'element n'est pas une feuille, on ecrit le contenu de chacun de */
    /* ses fils */
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL)
        {
          PutContent (pChild, transChar, lineBreak, fnum, doc);
          pChild = pChild->ElNext;
        }
    }
  else
    /* l'element est une feuille, on sort son contenu */
    TranslateLeaf (pEl, transChar, lineBreak, fnum, doc);
}


/*----------------------------------------------------------------------
  ApplyAttrRulesToElem applique a l'element pEl les regles de
  traduction associees a l'attribut pAttr.
  ----------------------------------------------------------------------*/
static void ApplyAttrRulesToElem (TOrder position, PtrElement pEl,
                                  PtrAttribute pAttr, ThotBool *removeEl,
                                  ThotBool *ignoreEl,
                                  ThotBool *transChar, ThotBool *lineBreak,
                                  Document doc, ThotBool recordLineNb)
{
  PtrTRuleBlock       pBlock;
  PtrTRule            pTRule;
  PtrTSchema          pTSchAttr;
  PtrAttributeTransl  pAttrTrans;
  TranslNumAttrCase  *pTCase;
  NotifyAttribute     notifyAttr;
  int                 i;

  if (*ignoreEl)
    return;
  /* prepare et envoie l'evenement AttrExport.Pre s'il est demande' */
  notifyAttr.event = TteAttrExport;
  notifyAttr.document = doc;
  notifyAttr.element = (Element) pEl;
  notifyAttr.info = 0; /* not sent by undo */
  notifyAttr.attribute = (Attribute) pAttr;
  notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
  notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
  if (CallEventAttribute (&notifyAttr, TRUE))
    /* l'application ne laisse pas l'editeur ecrire l'attribut */
    return;
  /* cherche le premier bloc de regles correspondant a l'attribut */
  pTSchAttr = GetTranslationSchema (pAttr->AeAttrSSchema);
  pBlock = NULL;
  if (pTSchAttr != NULL)
    {
      pAttrTrans = pTSchAttr->TsAttrTRule->TsAttrTransl[pAttr->AeAttrNum - 1];
      switch (pAttr->AeAttrType)
        {
        case AtNumAttr:
          i = 1;
          while (pBlock == NULL && i <= pAttrTrans->AtrNCases)
            {
              pTCase = &pAttrTrans->AtrCase[i - 1];
              if (pAttr->AeAttrValue <= pTCase->TaUpperBound &&
                  pAttr->AeAttrValue >= pTCase->TaLowerBound)
                pBlock = pTCase->TaTRuleBlock;
              i++;
            }
          break;
        case AtTextAttr:
          if (pAttrTrans->AtrTextValue[0] == EOS)
            pBlock = pAttrTrans->AtrTxtTRuleBlock;
          else if (StringAndTextEqual (pAttrTrans->AtrTextValue,
                                       pAttr->AeAttrText))
            pBlock = pAttrTrans->AtrTxtTRuleBlock;
          break;
        case AtReferenceAttr:
          pBlock = pAttrTrans->AtrRefTRuleBlock;
          break;
        case AtEnumAttr:
          pBlock = pAttrTrans->AtrEnuTRuleBlock[pAttr->AeAttrValue];
          if (pBlock == NULL)
            /* pas de regles de traduction pour cette valeur, on */
            /* prend les regles qui s'appliquent a toute valeur */
            pBlock = pAttrTrans->AtrEnuTRuleBlock[0];
          break;
        default:
          break;
        }
    }
  /* parcourt les blocs de regles de la valeur de l'attribut */
  while (pBlock && !*ignoreEl)
    {
      if (ConditionIsTrue (pBlock, pEl, pAttr, doc))
        /* la condition du bloc est verifiee */
        {
          pTRule = pBlock->TbFirstTRule;	/* premiere regle du bloc */
          /* parcourt les regles du bloc */
          while (pTRule != NULL && !*ignoreEl)
            {
              if (pTRule->TrOrder == position)
                {
                  /* c'est une regle a appliquer a cette position */
                  if (pTRule->TrType == TRemove)
                    *removeEl = TRUE;
                  else if (pTRule->TrType == TIgnore)
                    *ignoreEl = TRUE;
                  else if (pTRule->TrType == TNoTranslation)
                    *transChar = FALSE;
                  else if (pTRule->TrType == TNoLineBreak)
                    *lineBreak = FALSE;
                  else
                    /* on applique la regle */
                    ApplyTRule (pTRule, pTSchAttr, pAttr->AeAttrSSchema,
                                pEl, transChar, lineBreak, removeEl, ignoreEl, NULL,
                                pAttr, doc, recordLineNb);
                }
              /* passe a la regle suivante */
              pTRule = pTRule->TrNextTRule;
            }
        }
      /* passe au bloc suivant */
      pBlock = pBlock->TbNextBlock;
    }
  /* prepare et envoie l'evenement AttrExport.Post s'il est demande' */
  notifyAttr.event = TteAttrExport;
  notifyAttr.document = doc;
  notifyAttr.element = (Element) pEl;
  notifyAttr.info = 0; /* not sent by undo */
  notifyAttr.attribute = (Attribute) pAttr;
  notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
  notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
  CallEventAttribute (&notifyAttr, FALSE);
}


/*----------------------------------------------------------------------
  ApplyAttrRules                                                        
  ----------------------------------------------------------------------*/
static void ApplyAttrRules (TOrder position, PtrElement pEl,
                            ThotBool *removeEl, ThotBool *ignoreEl,
                            ThotBool *transChar,
                            ThotBool *lineBreak, Document doc,
                            ThotBool recordLineNb)
{
  PtrElement          pAsc;
  PtrAttribute        pAttr, nextAttr;
  PtrTtAttribute      pAttr1;
  PtrTSchema          pTSch;
  int                 att, nAttr = 0;
#define MAX_ATTR_TABLE 50
  PtrAttribute        AttrTable[MAX_ATTR_TABLE];
  char               *ns_prefix, *buffer;

  if (*ignoreEl)
    return;

  pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
  /* Si on applique les regles "After", on commence par le dernier attribut */
  /* et on traitera les attributs dans l'ordre inverse */
  if (position == TAfter && pAttr != NULL)
    {
      nAttr = 0;
      while (pAttr->AeNext != NULL && nAttr < MAX_ATTR_TABLE)
        {
          AttrTable[nAttr++] = pAttr;
          pAttr = pAttr->AeNext;
        }
    }
   
  /* parcourt les attributs de l'element */
  while (pAttr != NULL && !*ignoreEl)
    {
      /* get the next attribute to be processed: an action associated with
         event AttrExport could remove the current attribute */
      if (position == TAfter)
        /* passe a l'attribut precedent de l'element */
        {
          if (nAttr > 0)
            {
              nAttr--;
              nextAttr = AttrTable[nAttr];
            }
          else
            nextAttr = NULL;
        }
      else
        /* passe a l'attribut suivant de l'element */
        nextAttr = pAttr->AeNext;
      /* process the current attribute */
      pTSch = NULL;
      pTSch = GetTranslationSchema (pAttr->AeAttrSSchema);
      if (pTSch != NULL)
        {
          if (pTSch->TsAttrTRule->TsAttrTransl[pAttr->AeAttrNum - 1]->AtrElemType == 0)
            {
              if (pEl->ElStructSchema != pAttr->AeAttrSSchema)
                {
                  ns_prefix = ExportAttrNsPrefix (doc, pEl, pAttr);
                  if (ns_prefix != NULL)
                    {
                      buffer = (char *)TtaGetMemory (strlen (ns_prefix) + 2);
                      strcpy (buffer, ns_prefix);
                      strcat (buffer, ":");
                      SetVariableBuffer (pTSch, "AttrPrefixBuffer", buffer);
                      TtaFreeMemory (buffer);
                    }
                }
              /* les regles de traduction de l'attribut s'appliquent a */
              /* n'importe quel type d'element, on les applique */
              ApplyAttrRulesToElem (position, pEl, pAttr, removeEl, ignoreEl,
                                    transChar, lineBreak, doc, recordLineNb);
            }
        }
      else
        {
          /* translation of an attribute from an unknown namespace */
          if (position == TBefore)
            {
              ExportXmlBuffer (doc, (unsigned char *)" ", *lineBreak);
              ns_prefix = ExportAttrNsPrefix (doc, pEl, pAttr);
              if (ns_prefix != NULL)
                {
                  ExportXmlBuffer (doc, (unsigned char *)ns_prefix, *lineBreak); 
                  ExportXmlBuffer (doc, (unsigned char *)":", *lineBreak);
                }
              /* Export the attribute name */
              pAttr1 = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum-1];
              ExportXmlBuffer (doc, (unsigned char *)pAttr1->AttrName, *lineBreak);
              ExportXmlBuffer (doc, (unsigned char *)"=", *lineBreak);
              /* Export the attribute's value */
              switch (pAttr1->AttrType)
                {
                case AtNumAttr:
                  ExportXmlBuffer (doc, (unsigned char*)pAttr->AeAttrValue, FALSE);
                  break;
                case AtTextAttr:
                  if (pAttr->AeAttrText)
                    {
                      ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
                      /* Export the text buffer content */
                      ExportXmlText (doc, pAttr->AeAttrText, *lineBreak, TRUE, FALSE);
                      ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
                    }
                  break;
                case AtEnumAttr:
                  ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
                  ExportXmlBuffer (doc, (unsigned char *)pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1], FALSE);
                  ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
		   
                  break;
                default:
                  break;
                }
            }
        }
      /* next attribute to be processed */
      pAttr = nextAttr;
    }

  /* produit la traduction des attributs des elements ascendants qui */
  /* s'appliquent aux elements du type de notre element */
  pTSch = GetTranslationSchema (pEl->ElStructSchema);
  if (pTSch != NULL)
    if (pTSch->TsInheritAttr->Bln[pEl->ElTypeNumber - 1])
      /* il y a effectivement heritage d'attribut pour ce type d'element */
      {
        /* cherche tous les attributs dont ce type d'element peut heriter. */
        /* balaie la table des attributs */
        for (att = 1; att <= pEl->ElStructSchema->SsNAttributes; att++)
          if (pTSch->TsAttrTRule->TsAttrTransl[att - 1]->AtrElemType ==
              pEl->ElTypeNumber)
            /* cet attribut s'applique a ce type d'element */
            {
              /* Y a-t-il un element ascendant qui porte cet attribut? */
              pAsc = pEl;	/* on commence par l'element lui-meme */
              while (pAsc != NULL)		/* parcourt les ascendants */
                {
                  /* parcourt les attributs de chaque ascendant */
                  pAttr = pAsc->ElFirstAttr;
                  while (pAttr != NULL && !*ignoreEl)
                    if (!strcmp (pAttr->AeAttrSSchema->SsName,
                                 pEl->ElStructSchema->SsName)
                        && pAttr->AeAttrNum == att)
                      /* on a trouve' */
                      {
                        /* applique les regles de traduction de l'attribut a
                           l'element */
                        ApplyAttrRulesToElem (position, pEl, pAttr, removeEl,
                                              ignoreEl, transChar, lineBreak, doc,
                                              recordLineNb);
                        /* inutile de poursuivre la recherche */
                        pAttr = NULL;
                        pAsc = NULL;
                      }
                    else
                      /* passe a l'attribut suivant du meme ascendant */
                      pAttr = pAttr->AeNext;
                  /* passe a l'ascendant du dessus si on n'a pas trouve' */
                  if (pAsc != NULL)
                    pAsc = pAsc->ElParent;
                }
            }
      }
}

/*----------------------------------------------------------------------
  ApplyPresTRules applique a l'element pointe' par pEl les regles de	
  traduction associees aux presentations portees par l'element.   
  ----------------------------------------------------------------------*/
static void ApplyPresTRules (TOrder position, PtrElement pEl,
                             ThotBool *removeEl, ThotBool *ignoreEl,
                             ThotBool *transChar,
                             ThotBool *lineBreak, PtrAttribute pAttr,
                             Document doc, ThotBool recordLineNb)
{
  PtrPRule            pPRule;
  PtrTSchema          pTSch;
  PtrTRule            pTRule;
  PRuleTransl        *pPRuleTr;
  PtrTRuleBlock       pBlock = NULL;
  TranslNumAttrCase  *pTCase;
  int                 i, nPRules = 0;
  char                val;
#define MAX_PRULE_TABLE 50
  PtrPRule            PRuleTable[MAX_PRULE_TABLE];

  if (*ignoreEl || !pEl->ElFirstPRule)
    return;
  pTSch = GetTranslationSchema (pEl->ElStructSchema);
  if (pTSch == NULL)
    return;
  /* 1ere regle de presentation specifique de l'element */
  pPRule = pEl->ElFirstPRule;
  /* Si on applique les regles "After", on commence par la derniere regle */
  /* et on traitera les regles dans l'ordre inverse */
  if (position == TAfter && pPRule != NULL)
    {
      nPRules = 0;
      while (pPRule->PrNextPRule != NULL && nPRules < MAX_PRULE_TABLE)
        {
          PRuleTable[nPRules++] = pPRule;
          pPRule = pPRule->PrNextPRule;
        }
    }
   
  /* parcourt les regles de presentation specifique de l'element */
  while (pPRule != NULL && !*ignoreEl)
    {
      pPRuleTr = &pTSch->TsPresTRule[pPRule->PrType];
      if (pPRuleTr->RtExist)
        /* il y a des regles de traduction pour cette presentation */
        {
          /* cherche le premier bloc de regles correspondant a ce */
          /* type de regle de presentation */
          pBlock = NULL;
          if (pPRule->PrType == PtIndent ||
              pPRule->PrType == PtSize ||
              pPRule->PrType == PtLineSpacing ||
              pPRule->PrType == PtLineWeight)
            {
              i = 0;
              while (pBlock == NULL && i < pPRuleTr->RtNCase)
                {
                  pTCase = &pPRuleTr->RtCase[i++];
                  if (pPRule->PrMinValue <= pTCase->TaUpperBound &&
                      pPRule->PrMinValue >= pTCase->TaLowerBound)
                    pBlock = pTCase->TaTRuleBlock;
                }
            }
          else if (pPRule->PrType == PtFillPattern ||
                   pPRule->PrType == PtBackground ||
                   pPRule->PrType == PtForeground ||
		   pPRule->PrType == PtColor ||
		   pPRule->PrType == PtStopColor)
            {
              i = 0;
              while (pBlock == NULL && i < pPRuleTr->RtNCase)
                {
                  pTCase = &pPRuleTr->RtCase[i++];
                  if (pPRule->PrIntValue <= pTCase->TaUpperBound &&
                      pPRule->PrIntValue >= pTCase->TaLowerBound)
                    pBlock = pTCase->TaTRuleBlock;
                }
            }
          else
            {
              /* cherche si cette valeur de la presentation a un bloc de regles */
              /* Calcule d'abord la valeur caractere de la presentation */
              val = PresRuleValue (pPRule);
              pBlock = NULL;
              i = 1;
              while (pBlock == NULL && pPRuleTr->RtPRuleValue[i] != EOS &&
                     i <= MAX_TRANSL_PRES_VAL + 1)
                {
                  if (pPRuleTr->RtPRuleValue[i] == val)
                    pBlock = pPRuleTr->RtPRuleValueBlock[i];
                  i++;
                }
              if (pBlock == NULL)
                /* pas de regles de traduction pour cette valeur, on */
                /* prend les regles qui s'appliquent a toute valeur */
                pBlock = pPRuleTr->RtPRuleValueBlock[0];
            }
          /* parcourt les blocs de regles de la valeur de la presentation */
          while (pBlock != NULL && !*ignoreEl)
            {
              if (ConditionIsTrue (pBlock, pEl, NULL, doc))
                /* la condition du bloc est verifiee */
                {
                  pTRule = pBlock->TbFirstTRule;  /* premiere regle du bloc */
                  /* parcourt les regles du bloc */
                  while (pTRule != NULL && !*ignoreEl)
                    {
                      if (pTRule->TrOrder == position)
                        {
                          /* c'est une regle a appliquer a cette position */
                          if (pTRule->TrType == TRemove)
                            *removeEl = TRUE;
                          else if (pTRule->TrType == TIgnore)
                            *ignoreEl = TRUE;
                          else if (pTRule->TrType == TNoTranslation)
                            *transChar = FALSE;
                          else if (pTRule->TrType == TNoLineBreak)
                            *lineBreak = FALSE;
                          else
                            /* on applique la regle */
                            ApplyTRule (pTRule, pTSch, pEl->ElStructSchema,
                                        pEl, transChar, lineBreak, removeEl,
                                        ignoreEl, pPRule, pAttr, doc, recordLineNb);
                        }
                      /* passe a la regle suivante */
                      pTRule = pTRule->TrNextTRule;
                    }
                }
              /* passe au bloc suivant */
              pBlock = pBlock->TbNextBlock;
            }
        }
     
      if (position == TAfter)
        /* passe a la regle de presentation precedente de l'element */
        {
          if (nPRules > 0)
            {
              nPRules--;
              pPRule = PRuleTable[nPRules];
            }
          else
            pPRule = NULL;
        }
      else
        /* passe a la regle de presentation suivante de l'element */
        pPRule = pPRule->PrNextPRule;
    }
}


/*----------------------------------------------------------------------
  PutVariable                               				
  ----------------------------------------------------------------------*/
static void PutVariable (PtrElement pEl, PtrAttribute pAttr,
                         PtrTSchema pTSch, PtrSSchema pSS, int varNum,
                         ThotBool ref, char *outBuf, int fnum,
                         Document doc, ThotBool lineBreak)
{
  TranslVariable     *varTrans;
  TranslVarItem      *varItem;
  PtrElement          pRefEl, pAncest;
  PtrReference        pRef;
  PtrTtAttribute      attrTrans;
  PtrAttribute        pA;
  PtrDocument         pDoc;
  PtrTextBuffer       pBuf;
  unsigned char       number[20];
  CHAR_T              c;
  int                 item, i, j, k;
  ThotBool            found;

  pA = NULL;
  pDoc = LoadedDocument[doc - 1];
  if (outBuf != NULL)
    /* on vide le buffer avant de commencer a le remplir */
    outBuf[0] = EOS;
  varTrans = &pTSch->TsVariable[varNum - 1];
  /* parcourt les items qui constituent la variable */
  for (item = 0; item < varTrans->TrvNItems; item++)
    {
      varItem = &varTrans->TrvItem[item];
      /* traite selon le type de l'element de variable */
      switch (varItem->TvType)
        {
        case VtText:
          /* une constante de texte */
          i = pTSch->TsConstBegin[varItem->TvItem - 1];
          while (pTSch->TsConstant[i - 1] != EOS)
            {
              c = pTSch->TsConstant[i - 1];
              PutChar ((wchar_t) c, fnum, outBuf, doc,
                       lineBreak, TRUE, FALSE);
              i++;
            }
          break;
	  
        case VtCounter:
          /* valeur d'un compteur: si la regle porte sur une reference, */
          /* on prend la valeur du compteur pour l'element designepar */
          /* la reference si TrReferredObj est vrai*/
          pRef = NULL;
          if (ref)
            {
              if (pAttr != NULL &&
                  pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->
                  AttrType == AtReferenceAttr)
                /* c'est un attribut reference */
                pRef = pAttr->AeAttrReference;
              else
                /* l'element est-il une reference ? */
                if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
                  pRef = pEl->ElReference;
                else
                  /* c'est peut-etre une inclusion */
                  pRef = pEl->ElSource;
            }
          if (pRef != NULL)
            {
              pRefEl = ReferredElement (pRef);
              if (pRefEl == NULL)
                /* la reference ne designe rien */
                i = 0;
              else
                /* valeur du compteur pour l'element */
                /* designe' par la reference */
                i = Counter_Val (varItem->TvItem, pTSch, pSS, pRefEl);
            }
          else
            /* ce n'est pas une reference */
            /* valeur du compteur pour l'element meme */
            i = Counter_Val (varItem->TvItem, pTSch, pSS, pEl);
          /* produit quelques 0 si c'est demande' */
          if (varItem->TvCounterStyle == CntDecimal && varItem->TvLength > 0)
            {
              j = 1;
              for (k = 0; k < varItem->TvLength - 1; k++)
                {
                  j = j * 10;
                  if (j > i)
                    PutChar ((wchar_t) '0', fnum, outBuf,
                             doc, lineBreak, FALSE, FALSE);
                }
            }
          /* convertit la valeur du compteur dans le style demande' */
          GetCounterValue (i, varItem->TvCounterStyle, (char *)number, &j);
          /* sort la valeur du compteur */
          for (k = 0; k < j; k++)
            PutChar ((wchar_t) (number[k]), fnum, outBuf, doc,
                     lineBreak, TRUE, FALSE);
          break;
	 
        case VtBuffer:
          /* le contenu d'un buffer */
          i = 0;
          while (pTSch->TsBuffer[varItem->TvItem - 1][i] != EOS)
            {
              c = pTSch->TsBuffer[varItem->TvItem - 1][i];
              PutChar ((wchar_t) c, fnum, outBuf, doc,
                       lineBreak, TRUE, FALSE);
              i++;
            }
          break;

        case VtAttrVal:	/* la valeur d'un attribut */
          /* cherche si l'element traduit ou l'un de ses ascendants possede
             cet attribut */
          found = FALSE;
          pAncest = pEl;
          while (!found && pAncest != NULL)
            {
              pA = pAncest->ElFirstAttr;	/* premier attribut */
              while (!found && pA != NULL)
                if (pA->AeAttrNum == varItem->TvItem &&
                    !strcmp (pA->AeAttrSSchema->SsName, pSS->SsName))
                  found = TRUE;
                else
                  pA = pA->AeNext;
              if (!found)
                pAncest = pAncest->ElParent;
            }
          if (found)
            /* l'element possede l'attribut */
            {
              switch (pA->AeAttrType)
                {
                case AtNumAttr:
                  PutInt (pA->AeAttrValue, fnum, outBuf, doc, lineBreak);
                  break;
                case AtTextAttr:
                  pBuf = pA->AeAttrText;
                  while (pBuf != NULL)
                    {
                      i = 0;
                      while (i < pBuf->BuLength)
                        {
                          c = pBuf->BuContent[i++];
                          PutChar ((wchar_t) c, fnum, outBuf, doc, FALSE, TRUE, FALSE);
                        }
                      pBuf = pBuf->BuNext;
                    }
                  break;
                case AtReferenceAttr:
                  PutChar ((wchar_t) 'R', fnum, outBuf, doc, lineBreak, FALSE, FALSE);
                  PutChar ((wchar_t) 'E', fnum, outBuf, doc, lineBreak, FALSE, FALSE);
                  PutChar ((wchar_t) 'F', fnum, outBuf, doc, lineBreak, FALSE, FALSE);
                  break;
                case AtEnumAttr:
                  i = 0;
                  attrTrans = pA->AeAttrSSchema->SsAttribute->TtAttr[varItem->TvItem-1];
                  while (attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i] != EOS)
                    {
                      c = attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i++];
                      PutChar ((wchar_t) c, fnum, outBuf, doc,
                               lineBreak, TRUE, FALSE);
                    }
                  break;
                }
            }
          break;
	  
        case VtFileDir:	/* le nom du directory de sortie */
          i = 0;
          while (fileDirectory[i] != EOS)
            PutChar ((wchar_t) fileDirectory[i++], fnum, outBuf, doc, lineBreak,
                     TRUE, FALSE);
          break;
	  
        case VtFileName:	/* le nom du fichier de sortie */
          i = 0;
          while (fileName[i] != EOS)
            PutChar ((wchar_t) fileName[i++], fnum, outBuf, doc, lineBreak,
                     TRUE, FALSE);
          break;
	 
        case VtExtension:	/* le nom de l'extension de fichier */
          i = 0;
          while (fileExtension[i] != EOS)
            PutChar ((wchar_t) fileExtension[i++], fnum, outBuf, doc, lineBreak,
                     TRUE, FALSE);
          break;

        case VtDocumentName:	/* le nom du document */
          i = 0;
          while (pDoc->DocDName[i] != EOS)
            PutChar ((wchar_t) pDoc->DocDName[i++], fnum, outBuf, doc, lineBreak,
                     TRUE, FALSE);
          break;

        case VtDocumentDir:	/* le repertoire du document */
          i = 0;
          while (pDoc->DocDirectory[i] != EOS)
            PutChar ((wchar_t) pDoc->DocDirectory[i++], fnum, outBuf, doc,
                     lineBreak, TRUE, FALSE);
          break;
	  
        default:
          break;
        }
    }
}


/*----------------------------------------------------------------------
  ApplyTRule   applique la regle de traduction pTRule du schema de	
  traduction pTSch (qui correspond au schema de structure pointe' 
  par pSSch) a l'element pointe par pEl, en demandant la          
  traduction des caracteres contenus si transChar est vrai.       
  S'il s'agit de la traduction d'une presentation, pRPres pointe  
  sur la regle de presentation specifique traduite.               
  S'il s'agit de la traduction des regles d'un attribut,          
  pAttr pointe sur l'attribut que l'on traduit.			
  ----------------------------------------------------------------------*/
static void ApplyTRule (PtrTRule pTRule, PtrTSchema pTSch, PtrSSchema pSSch,
                        PtrElement pEl, ThotBool *transChar,
                        ThotBool *lineBreak, ThotBool *removeEl,
                        ThotBool *ignoreEl,
                        PtrPRule pRPres, PtrAttribute pAttr,
                        Document doc, ThotBool recordLineNb)
{
  PtrElement          pElGet, pRefEl;
  Document            docGet;
  PtrDocument         pDoc;
  PtrSSchema          pSS;
  Name                n;
  PtrAttribute        pA = NULL;
  PtrTextBuffer       pBuf;
  TtAttribute        *attrTrans;
  BinFile             includedFile;
  PtrReference        pRef;
  PtrTSchema          pTransTextSch;
  ScriptTransl      *pTransAlph;
  int                 fnum;
  int                 i;
  CHAR_T              c;
  unsigned char       car;
  char                secondaryFileName[MAX_PATH];
  char               *nameBuffer;
  char                fname[MAX_PATH];
  char                currentFileName[MAX_PATH]; /* nom du fichier principal*/
  FILE               *newFile;
  ThotBool            found, possibleRef, encode, entityName;

  if (*ignoreEl)
    return;
  n[0] = EOS;
  pDoc = LoadedDocument[doc - 1];
  /* on applique la regle selon son type */
  switch (pTRule->TrType)
    {
    case TCreate:
    case TWrite:
      /* regle d'ecriture dans un fichier de sortie ou au terminal */
      if (pTRule->TrType == TCreate)
        {
          if (pTRule->TrFileNameVar == 0)
            /* output into the main file */
            fnum = 1;
          else
            /* output into a secondary file */
            {
              /* build the filemane */
              PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrFileNameVar,
                           FALSE, secondaryFileName, 0, doc, *lineBreak);
              fnum = GetSecondaryFile (secondaryFileName, doc, TRUE);
            }
        }
      else		/* TWrite */
        fnum = 0;	/* on ecrit sur stdout */
      /* traitement selon le type d'objet a ecrire */
      switch (pTRule->TrObject)
        {
        case ToConst:
          /* ecriture d'une constante */
          i = pTSch->TsConstBegin[pTRule->TrObjectNum - 1];
          while (pTSch->TsConstant[i - 1] != EOS)
            {
              c = pTSch->TsConstant[i - 1];
              PutChar ((wchar_t) c, fnum, NULL, doc,
                       *lineBreak, TRUE, FALSE);
              i++;
            }
          break;
        case ToBuffer:
          /* ecriture du contenu d'un buffer */
          i = 0;
          while (pTSch->TsBuffer[pTRule->TrObjectNum - 1][i] != EOS)
            {
              c = pTSch->TsBuffer[pTRule->TrObjectNum - 1][i++];
              PutChar ((wchar_t) c, fnum,
                       NULL, doc, *lineBreak, TRUE, FALSE);
            }
          break;
        case ToVariable:	/* creation d'une variable */
          PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrObjectNum,
                       pTRule->TrReferredObj, NULL, fnum, doc, *lineBreak);
          break;
        case ToAttr:
        case ToTranslatedAttr:
          /* ecriture de la valeur d'un attribut */
          /* cherche si l'element ou un de ses ascendants possede l'attribut
             a sortir */
          found = FALSE;
          while (pEl != NULL && !found)
            {
              pA = pEl->ElFirstAttr;	/* 1er attribut de l'element */
              /* parcourt les attributs de l'element */
              while (pA != NULL && !found)
                if (pA->AeAttrNum == pTRule->TrObjectNum &&
                    !strcmp (pA->AeAttrSSchema->SsName, pSSch->SsName))
                  found = TRUE;
                else
                  pA = pA->AeNext;
              if (!found)
                pEl = pEl->ElParent;	/* passe a l'element ascendant */
            }
          /* si on a trouve' l'attribut, on sort sa valeur */
          if (found)
            switch (pA->AeAttrType)
              {
              case AtNumAttr:
                /* ecrit la valeur numerique de l'attribut */
                PutInt (pA->AeAttrValue, fnum, NULL, doc, *lineBreak);
                break;
              case AtTextAttr:
                /* ecrit la valeur de l'attribut */
                pTransAlph = NULL;
                pTransTextSch = NULL;
                if (pTRule->TrObject == ToTranslatedAttr)
                  pTransTextSch = GetTransSchForContent(pEl, LtText, &pTransAlph);
                pBuf = pA->AeAttrText;
                /* double quotes within attribute values are encoded
                   except for invisible (internal) attributes */
                encode = !AttrHasException (ExcInvisible, pA->AeAttrNum,
                                            pA->AeAttrSSchema);
                entityName = !strcmp (pA->AeAttrSSchema->SsName, "MathML");
                if (pBuf)
                  {
                    /* don't insert line breaks in attribute values */
                    if (!pTransTextSch || !pTransAlph)
                      /* no translation */
                      while (pBuf)
                        {
                          i = 0;
                          while (i < pBuf->BuLength)
                            {
                              c = pBuf->BuContent[i++];
                              if (encode && c == 0x22)
                                {
                                  /* write a numeric entity */
                                  PutChar ((wchar_t) '&', fnum, NULL, doc,
                                           FALSE, FALSE, FALSE);
                                  PutChar ((wchar_t) '#', fnum, NULL, doc,
                                           FALSE, FALSE, FALSE);
                                  PutInt (0x22, fnum, NULL, doc, FALSE);
                                  PutChar ((wchar_t) ';', fnum, NULL, doc,
                                           FALSE, FALSE, FALSE);
                                }
                              else
                                {
                                  if (encode &&
                                      (c == 0X26 || c == 0X3C || c == 0X3E || c == 0XA0))
                                    entityName = TRUE;
                                  PutChar ((wchar_t) c, fnum, NULL, doc,
                                           FALSE, encode, entityName);
                                }
                            }
                          pBuf = pBuf->BuNext;
                        }
                    else
                      /* translate the attribute value */
                      TranslateText (pBuf, pTransTextSch, pTransAlph, FALSE,
                                     fnum, doc, encode, entityName, FALSE);
                  }
                break;
              case AtReferenceAttr:
                /* cas non traite' */
                break;
              case AtEnumAttr:
                /* ecrit le nom de la valeur de l'attribut */
                attrTrans = pA->AeAttrSSchema->SsAttribute->TtAttr[pA->AeAttrNum-1];
                i = 0;
                while (attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i] != EOS)
                  {
                    c = attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i++];
                    PutChar ((wchar_t) c, fnum,
                             NULL, doc, *lineBreak, TRUE, FALSE);
                  }
                break;
              default:
                break;
              }
          break;
        case ToContent:
          /* produit le contenu des feuilles de l'element */
          PutContent (pEl, *transChar, *lineBreak, fnum, doc);
          break;
        case ToPRuleValue:
          /* produit la valeur numerique de la presentation a laquelle */
          /* se rapporte la regle */
          if (pRPres != NULL &&
              pRPres->PrPresMode == PresImmediate)
            switch (pRPres->PrType)
              {
              case PtFont:
              case PtStyle:
              case PtWeight:
              case PtVariant:
              case PtUnderline:
              case PtThickness:
              case PtDirection:
              case PtUnicodeBidi:
              case PtVisibility:
              case PtLineStyle:
                PutChar ((wchar_t) (pRPres->PrChrValue), fnum, NULL, doc,
                         *lineBreak, FALSE, FALSE);
                break;
              case PtIndent:
              case PtSize:
              case PtLineSpacing:
              case PtLineWeight:
                PutInt (pRPres->PrMinValue, fnum, NULL, doc,*lineBreak);
                break;
              case PtFillPattern:
                PutPattern (pRPres->PrIntValue, fnum, doc, *lineBreak);
                break;
              case PtFillOpacity:
              case PtStrokeOpacity:
              case PtOpacity:
              case PtStopOpacity:
              case PtMarker:
              case PtMarkerStart:
              case PtMarkerMid:
              case PtMarkerEnd:
                PutInt (pRPres->PrIntValue, fnum,  NULL, doc, *lineBreak);
                break;
              case PtBackground:
              case PtForeground:
              case PtColor:
              case PtStopColor:
                PutColor (pRPres->PrIntValue, fnum, doc, *lineBreak);
                break;
              case PtHyphenate:
                if (pRPres->PrBoolValue)
                  PutChar ((wchar_t) 'Y', fnum, NULL, doc, *lineBreak, FALSE, FALSE);
                else
                  PutChar ((wchar_t) 'N', fnum, NULL, doc, *lineBreak, FALSE, FALSE);
                break;
              case PtAdjust:
                switch (pRPres->PrAdjust)
                  {
                  case AlignLeft:
                    PutChar ((wchar_t) 'L', fnum, NULL, doc, *lineBreak, FALSE, FALSE);
                    break;
                  case AlignRight:
                    PutChar ((wchar_t) 'R', fnum, NULL, doc, *lineBreak, FALSE, FALSE);
                    break;
                  case AlignCenter:
                    PutChar ((wchar_t) 'C', fnum, NULL, doc, *lineBreak, FALSE, FALSE);
                    break;
                  case AlignLeftDots:
                    PutChar ((wchar_t) 'D', fnum, NULL, doc, *lineBreak, FALSE, FALSE);
                    break;
                  case AlignJustify:
                    PutChar ((wchar_t) 'J', fnum, NULL, doc, *lineBreak, FALSE, FALSE);
                    break;
                  }
                break;
              default:
                break;
              }
          break;
        case ToAllAttr:
          /* Export the namespace declarations associated with  
             this element before exporting its attributes */ 
          if (!pEl->ElTransAttr)
            {
              if (IsTranslateTag (pTSch, pSSch) != 0)
                ExportNsDeclaration (doc, pEl);
            }
	  
          /* produit la traduction de tous les attributs de l'element */
          ApplyAttrRules (pTRule->TrOrder, pEl, removeEl, ignoreEl, transChar,
                          lineBreak, doc, recordLineNb);
          /* les regles des attributs ont ete appliquees */
          pEl->ElTransAttr = TRUE;
          break;
        case ToAllPRules:
          /* produit la traduction de toutes les regles de presentation */
          /* specifique portees par l'element */
          ApplyPresTRules (pTRule->TrOrder, pEl, removeEl, ignoreEl,
                           transChar, lineBreak, pAttr, doc, recordLineNb);
          /* marque dans l'element que sa presentation a ete traduite */
          pEl->ElTransPres = TRUE;
          break;
        case ToPairId:
          /* traduit l'identificateur d'une paire */
          if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber-1]->SrConstruct ==
              CsPairedElement)
            /* l'element est bien une paire */
            PutInt (pEl->ElPairIdent, fnum, NULL, doc, *lineBreak);
          break;
        case ToFileDir:
          /* produit le nom du directory */
          i = 0;
          while (fileDirectory[i] != EOS)
            PutChar ((wchar_t) fileDirectory[i++], fnum, NULL, doc, *lineBreak,
                     TRUE, FALSE);
          break;
        case ToFileName:
          /* produit le nom de fichier */
          i = 0;
          while (fileName[i] != EOS)
            PutChar ((wchar_t) fileName[i++], fnum, NULL, doc, *lineBreak,
                     TRUE, FALSE);
          break;
        case ToExtension:
          i = 0;
          while (fileExtension[i] != EOS)
            PutChar ((wchar_t) fileExtension[i++], fnum, NULL, doc, *lineBreak,
                     TRUE, FALSE);
          break;
        case ToDocumentName:
          i = 0;
          while (pDoc->DocDName[i] != EOS)
            PutChar ((wchar_t) (pDoc->DocDName[i++]), fnum, NULL, doc, *lineBreak,
                     TRUE, FALSE);
          break;
        case ToDocumentDir:
          i = 0;
          while (pDoc->DocDirectory[i] != EOS)
            PutChar ((wchar_t) (pDoc->DocDirectory[i++]), fnum, NULL, doc,
                     *lineBreak, TRUE, FALSE);
          break;
        case ToReferredDocumentName:
        case ToReferredDocumentDir:
          pRef = NULL;
          if (pAttr != NULL &&
              pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->
              AttrType == AtReferenceAttr)
            /* c'est un attribut reference qu'on traduit */
            pRef = pAttr->AeAttrReference;
          else if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
            /* l'element est-il une reference ? */
            pRef = pEl->ElReference;
          else
            /* c'est peut-etre une inclusion */
            pRef = pEl->ElSource;
          if (pRef)
            {
              pRefEl = ReferredElement (pRef);
              nameBuffer = NULL;
              if (pTRule->TrObject == ToReferredDocumentName)
                {
                  if (pRefEl)
                    /* reference interne. On sort le nom du document lui-meme */
                    nameBuffer = pDoc->DocDName;
                }
              else if (pTRule->TrObject == ToReferredDocumentDir)
                {
                  if (pRefEl)
                    /* reference interne. On sort le directory du document
                       lui-meme */
                    nameBuffer = pDoc->DocDirectory;
                }
              if (nameBuffer)
                while (*nameBuffer != EOS)
                  {
                    PutChar ((wchar_t) (*nameBuffer), fnum, NULL, doc,
                             *lineBreak, TRUE, FALSE);
                    nameBuffer++;
                  }
            }
          break;
        case ToReferredElem:
          /* traduit l'element reference' de type pTRule->TrObjectNum */
          pRef = NULL;
          if (pAttr != NULL &&
              pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrType == AtReferenceAttr)
            /* c'est un attribut reference qu'on traduit */
            pRef = pAttr->AeAttrReference;
          else if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
            /* l'element est-il une reference ? */
            pRef = pEl->ElReference;
          else
            /* c'est peut-etre une inclusion */
            pRef = pEl->ElSource;
          if (pRef != NULL)
            {
              pRefEl = ReferredElement (pRef);
              if (pRefEl != NULL)
                /* la reference designe l'element pRefEl */
                /* On le prend s'il a le type voulu */
                {
                  if (pTRule->TrObjectNature[0] == EOS)
                    pSS = pEl->ElStructSchema;
                  else
                    pSS = NULL;
                  if (!((pSS != NULL &&
                         EquivalentSRules (pTRule->TrObjectNum, pSS,
                                           pRefEl->ElTypeNumber,
                                           pRefEl->ElStructSchema,
                                           pRefEl->ElParent))
                        || (pSS == NULL &&
                            strcmp (pTRule->TrObjectNature,
                                    pRefEl->ElStructSchema->SsName) == 0
                            && EquivalentSRules (pTRule->TrObjectNum,
                                                 pRefEl->ElStructSchema,
                                                 pRefEl->ElTypeNumber,
                                                 pRefEl->ElStructSchema,
                                                 pRefEl->ElParent))))
                    /* Il n'a pas le type voulu, on cherche dans */
                    /* le sous arbre de l'element designe' */
                    SearchDescent (&pRefEl, pTRule->TrObjectNum, pSS,
                                   pTRule->TrObjectNature);
                }
              if (pRefEl)
                {
                  /* traduit l'element reference', meme s'il a deja ete traduit */
                  TranslateTree (pRefEl, doc, *transChar, *lineBreak, TRUE,
                                 recordLineNb);
                }
            }
          break;
        case ToRefId:
        case ToReferredRefId:
          pElGet = NULL;
          if (pTRule->TrObject == ToReferredRefId)
            /* il faut traduire le label de l'element reference' */
            {
              pRef = NULL;
              /* si on traduit un attribut reference, on ne s'occupe que de */
              /* l'attribut */
              if (pAttr != NULL
                  && pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->
                  AttrType == AtReferenceAttr)
                /* c'est un attribut reference */
                pRef = pAttr->AeAttrReference;
              /* sinon on s'occupe de l'element */
              else
                {
                  /* l'element est-il une reference ? */
                  if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsReference)
                    pRef = pEl->ElReference;
                  /* ou est-il defini comme identique a une reference */
                  else if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->
                           SrConstruct == CsIdentity)
                    {
                      i = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->
                        SrIdentRule;
                      if (pEl->ElStructSchema->SsRule->SrElem[i - 1]->SrConstruct ==
                          CsBasicElement &&
                          pEl->ElStructSchema->SsRule->SrElem[i - 1]->SrBasicType ==
                          Refer)
                        pRef = pEl->ElReference;
                    }
                }
              if (pRef == NULL)
                /* c'est peut-etre une inclusion */
                pRef = pEl->ElSource;
              if (pRef != NULL)
                {
                  pElGet = ReferredElement (pRef);
                }
            }

          if (pTRule->TrObject == ToRefId)
            {
              /* on cherche si l'element (ou le premier de ses ascendants sur */
              /* lequel porte une reference) est reference' et on recupere la */
              /* reference. */
              pElGet = pEl;
              do
                {
                  pSS = pElGet->ElStructSchema;
                  possibleRef = FALSE;
                  /* l'element est-il reference'? */
                  if (pElGet->ElReferredDescr != NULL)
                    possibleRef = pElGet->ElReferredDescr->ReFirstReference != NULL;
                  if (!possibleRef)
                    {
                      /* l'element peut-il etre designe' par un element reference?
                         on cherche tous les elements references dans le schema de
                         structure de l'element */
                      i = 1;
                      do
                        {
                          if (pSS->SsRule->SrElem[i]->SrConstruct == CsReference &&
                              /* c'est une reference */
                              pSS->SsRule->SrElem[i]->SrReferredType != 0)
                            possibleRef = EquivalentSRules (pSS->SsRule->SrElem[i]->
                                                            SrReferredType, pSS,
                                                            pElGet->ElTypeNumber,
                                                            pSS, pElGet->ElParent);
                          i++;
                        }
                      while (!possibleRef && i < pSS->SsNRules);
                    }
                  if (!possibleRef)
                    {
                      /* l'element ne peut pas etre designe par un elem. reference
                         on cherche s'il peut etre designe' par un attr. reference
                         on cherche tous les attributs reference dans le schema de
                         structure de l'element */
                      i = 1;
                      do
                        {
                          if (pSS->SsAttribute->TtAttr[i]->AttrType == AtReferenceAttr &&
                              /* c'est une reference */
                              pSS->SsAttribute->TtAttr[i]->AttrTypeRef != 0)
                            possibleRef = (pSS->SsAttribute->TtAttr[i]->
                                           AttrTypeRefNature[0] == EOS &&
                                           EquivalentSRules (pSS->SsAttribute->TtAttr[i]->
                                                             AttrTypeRef, pSS,
                                                             pElGet->ElTypeNumber, pSS,
                                                             pElGet->ElParent));
                          i++;
                        }
                      while (!possibleRef && i < pSS->SsNAttributes);
                    }
                  if (!possibleRef)
                    /* l'element ne peut pas etre designe'; on examine */
                    /* l'element ascendant */
                    pElGet = pElGet->ElParent;
                }
              while (!possibleRef && pElGet != NULL);
            }
	   
          if (pElGet != NULL)
            /** if (pElGet->ElReferredDescr != NULL) **/
            {
              i = 0;
              while (pElGet->ElLabel[i] != EOS)
                PutChar ((wchar_t) pElGet->ElLabel[i++], fnum, NULL, doc,
                         *lineBreak, TRUE, FALSE);
            }
          break;
	  
        default:
          break;
        }
      break;
    case TChangeMainFile:
      PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrNewFileVar, FALSE,
                   currentFileName, 0, doc, *lineBreak);
      if (currentFileName[0] != EOS)
        {
          newFile = TtaWriteOpen (currentFileName);
          if (newFile)
            /* on a reussi a ouvrir le nouveau fichier */
            {
              /* on vide le buffer en cours dans l'ancien fichier */
              for (i = 0; i < OutFile[1].OfBufferLen; i++)
                putc (OutFile[1].OfBuffer[i], OutFile[1].OfFileDesc);
              /* on ferme l'ancien fichier */
              TtaWriteClose (OutFile[1].OfFileDesc);
              /* on bascule sur le nouveau fichier */
              OutFile[1].OfBufferLen = 0;
              OutFile[1].OfLineLen = 0;
              OutFile[1].OfIndent = 0;
              OutFile[1].OfPreviousIndent = 0;
              OutFile[1].OfLineNumber = 0;
              OutFile[1].OfStartOfLine = TRUE;
              OutFile[1].OfFileDesc = newFile;
              OutFile[1].OfCannotOpen = FALSE;
            }
        }
      break;
    case TRemoveFile:
      /* unlink a secondary file */
      PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrNewFileVar,
                   FALSE, secondaryFileName, 0, doc, *lineBreak);
      if (secondaryFileName[0] != EOS)
        {
          sprintf (fname, "%s%c%s", fileDirectory, DIR_SEP, secondaryFileName);
          TtaFileUnlink (fname);
        }
      break;
    case TSetCounter:
      pTSch->TsCounter[pTRule->TrCounterNum - 1].TnParam1 = pTRule->TrCounterParam;
      break;
    case TAddCounter:
      pTSch->TsCounter[pTRule->TrCounterNum - 1].TnParam1 += pTRule->TrCounterParam;
      break;
    case TIndent:
      if (pTRule->TrIndentFileNameVar == 0)
        /* sortie sur le fichier principal courant */
        fnum = 1;
      else
        /* sortie sur un fichier secondaire */
        {
          /* construit le nom du fichier secondaire */
          PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrIndentFileNameVar,
                       FALSE, secondaryFileName, 0, doc, *lineBreak);
          fnum = GetSecondaryFile (secondaryFileName, doc, TRUE);
        }
      if (fnum >= 0)
        {
          switch (pTRule->TrIndentType)
            {
            case ItRelative:
              OutFile[fnum].OfIndent += pTRule->TrIndentVal;
              break;
            case ItAbsolute:
              OutFile[fnum].OfIndent = pTRule->TrIndentVal;
              break;
            case ItSuspend:
              OutFile[fnum].OfPreviousIndent = OutFile[fnum].OfIndent;
              OutFile[fnum].OfIndent = 0;
              break;
            case ItResume:
              OutFile[fnum].OfIndent = OutFile[fnum].OfPreviousIndent;
              break;
            }
          if (OutFile[fnum].OfIndent < 0)
            OutFile[fnum].OfIndent = 0;
        }
      break;
    case TGet:
    case TCopy:
      /* on traduit l'element indique' dans la regle Get */
      /* cherche d'abord l'element a prendre */
      pElGet = pEl;
      docGet = doc;
      switch (pTRule->TrRelPosition)
        {
        case RpSibling:
          /* Cherche un frere ayant le type voulu */
          /* cherche d'abord le frere aine' */
          while (pElGet->ElPrevious != NULL)
            pElGet = pElGet->ElPrevious;
          /* cherche ensuite parmi les freres successifs */
          found = FALSE;
          do
            if ((!strcmp (pElGet->ElStructSchema->SsName,
                          pEl->ElStructSchema->SsName) ||
                 !strcmp (pTRule->TrElemNature,
                          pElGet->ElStructSchema->SsName)) &&
                EquivalentSRules (pTRule->TrElemType, pElGet->ElStructSchema,
                                  pElGet->ElTypeNumber,pElGet->ElStructSchema,
                                  pElGet->ElParent))
              found = TRUE;
            else
              pElGet = pElGet->ElNext;
          while (!found && pElGet != NULL);
          break;
        case RpDescend:
          /* Cherche dans le sous-arbre un element ayant le type voulu. */
          if (pTRule->TrElemNature[0] == EOS)
            pSS = pEl->ElStructSchema;
          else
            pSS = NULL;
          SearchDescent (&pElGet, pTRule->TrElemType, pSS, pTRule->TrElemNature);
          break;
        case RpReferred:
          /* Cherche dans le sous-arbre de l'element designe', un element
             ayant le type voulu. */
          /* cherche d'abord l'element designe' */
          pRef = NULL;
          if (pAttr != NULL &&
              pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->
              AttrType == AtReferenceAttr)
            /* c'est un attribut reference qu'on traduit */
            pRef = pAttr->AeAttrReference;
          else if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
            /* l'element est-il une reference ? */
            pRef = pEl->ElReference;
          else
            /* c'est peut-etre une inclusion */
            pRef = pEl->ElSource;
          if (pRef == NULL)
            pElGet = NULL;
          else
            pElGet = ReferredElement (pEl->ElReference);
          if (pElGet)
            /* il y a bien un element designe'. On le prend s'il */
            /* a le type voulu */
            {
              if (pTRule->TrElemNature[0] == EOS)
                pSS = pEl->ElStructSchema;
              else
                pSS = NULL;
              if (!((pSS != NULL &&
                     EquivalentSRules (pTRule->TrElemType, pSS,
                                       pElGet->ElTypeNumber,
                                       pElGet->ElStructSchema, pElGet->ElParent)
                     )
                    || (pSS == NULL &&
                        strcmp (pTRule->TrElemNature,
                                pElGet->ElStructSchema->SsName) == 0
                        && EquivalentSRules (pTRule->TrElemType,
                                             pElGet->ElStructSchema,
                                             pElGet->ElTypeNumber,
                                             pElGet->ElStructSchema,
                                             pElGet->ElParent))))
                /* Il n'a pas le type voulu, on cherche dans */
                /* le sous arbre de l'element designe' */
                SearchDescent (&pElGet, pTRule->TrElemType, pSS, pTRule->TrElemNature);
            }
          break;
        default:
          break;
        }
      if (pElGet != NULL)
        /* traduit l'element a prendre, sauf s'il a deja ete traduit et */
        /* qu'il s'agit d'une regle Get */
        TranslateTree (pElGet, docGet, *transChar, *lineBreak,
                       (ThotBool)(pTRule->TrType == TCopy), recordLineNb);
      break;
    case TUse:
      /* On ne fait rien. Cette regle est utilisee uniquement */
      /* lors du chargement des schemas de traduction, au debut */
      /* du chargement du document a traduire. */
    case TRemove:
    case TIgnore:
    case TNoTranslation:
    case TNoLineBreak:
    case TRead:
      break;
    case TInclude:
      /* include a secondary file */
      if (pTRule->TrBufOrConst == ToConst)
        {
          i = pTSch->TsConstBegin[pTRule->TrInclFile - 1] - 1;
          strncpy(secondaryFileName, (char *)&pTSch->TsConstant[i], MAX_PATH - 1);
        }
      else if (pTRule->TrBufOrConst == ToBuffer)
        /* le nom du fichier est dans un buffer */
        strncpy (secondaryFileName, pTSch->TsBuffer[pTRule->TrInclFile - 1], MAX_PATH-1);
      if (secondaryFileName[0] != EOS)
        {
          /* si le fichier a inclure est deja ouvert en ecriture, on le flush.  */
          i = GetSecondaryFile (secondaryFileName, doc, FALSE);
          if (i >= 0)
            {
              includedFile = OutFile[i].OfFileDesc;
              fflush (includedFile);
              TtaWriteClose (includedFile);
              OutFile[i].OfFileDesc = NULL;
              /* beginning of the file */
              sprintf (fname, "%s%c%s", fileDirectory, DIR_SEP, secondaryFileName);
              includedFile = TtaReadOpen (fname);
              if (includedFile)
                {
                  while (TtaReadByte (includedFile, &car))
                    /* write into the main file */
                    PutChar ((wchar_t) car, 1, NULL, doc, *lineBreak, TRUE, FALSE);
                  TtaReadClose (includedFile);
                }
            }
        }
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  ApplyElTypeRules
  Applique a l'element pointe par pEl les regles de traduction qui
  correspondent a son type et qui doivent s'appliquer a la position position.
  ----------------------------------------------------------------------*/
static void ApplyElTypeRules (TOrder position, ThotBool *transChar,
                              ThotBool *lineBreak, ThotBool *removeEl,
                              ThotBool *ignoreEl,
                              PtrElement pEl, int TypeEl, PtrTSchema pTSch,
                              PtrSSchema pSS, Document doc,
                              ThotBool recordLineNb)
{
  PtrTRuleBlock       pBlock;
  PtrTRule            pTRule;

  if (*ignoreEl)
    return;

  /* test the exception xml:space="preserve" */
  if (TtaIsElementWithSpacePreserve ((Element) pEl))
    *lineBreak = FALSE;

  /* premier bloc de regles correspondant au type de l'element */
  pBlock = pTSch->TsElemTRule->TsElemTransl[TypeEl - 1];
  /* parcourt les blocs de regles du type de l'element */
  while (pBlock != NULL && !*ignoreEl)
    {
      if (ConditionIsTrue (pBlock, pEl, NULL, doc))
        /* la condition du bloc est verifiee */
        {
          pTRule = pBlock->TbFirstTRule;	/* premiere regle du bloc */
          /* parcourt les regles du bloc */
          while (pTRule != NULL && !*ignoreEl)
            {
              if (pTRule->TrOrder == position)
                {
                  /* c'est une regle a appliquer a cette position */
                  if (pTRule->TrType == TRemove)
                    *removeEl = TRUE;
                  else if (pTRule->TrType == TIgnore)
                    *ignoreEl = TRUE;
                  else if (pTRule->TrType == TNoTranslation)
                    *transChar = FALSE;
                  else if (pTRule->TrType == TNoLineBreak)
                    *lineBreak = FALSE;
                  else
                    /* on applique la regle */
                    ApplyTRule (pTRule, pTSch, pSS, pEl, transChar, lineBreak,
                                removeEl, ignoreEl, NULL, NULL, doc, recordLineNb);
                }
              /* passe a la regle suivante */
              pTRule = pTRule->TrNextTRule;
            }
        }
      /* passe au bloc suivant */
      pBlock = pBlock->TbNextBlock;
    }
}


/*----------------------------------------------------------------------
  TranslateTree 
  Traduit le sous-arbre dont la racine est pEl	et applique les regles
  de traduction des feuilles si transChar est vrai. 
  ----------------------------------------------------------------------*/
static void TranslateTree (PtrElement pEl, Document doc,
                           ThotBool transChar, ThotBool lineBreak,
                           ThotBool enforce, ThotBool recordLineNb)
{
  PtrElement       pChild;
  PtrTSchema       pTSch, pTS;
  PtrSSchema       pSS, pParentSS;
  PtrSRule         pSRule;
  NotifyElement    notifyEl;
  int              elemType, i;
  ThotBool         found;
  ThotBool         removeEl, ignoreEl, skip;
  ThotBool         withBreak = lineBreak;
  char            *ns_prefix, *buffer;

  if (!pEl->ElTransContent || enforce)
    {
      /* cherche le schema de traduction qui s'applique a l'element */
      pTSch = GetTranslationSchema (pEl->ElStructSchema);
      if (pTSch == NULL)
        { 
          if ((strcmp (pEl->ElStructSchema->SsName, "MathML") != 0) &&
              (strcmp (pEl->ElStructSchema->SsName, "SVG") != 0) &&
              (strcmp (pEl->ElStructSchema->SsName, "Template") != 0) &&
              (strcmp (pEl->ElStructSchema->SsName, "XLink") != 0))
            ExportXmlElement (doc, pEl, lineBreak, TRUE);
          return;
        }
     
      /* Is this element associated with a namespace prefix ? */
      /* We search for a prefix in both cases :
         - when the structure schema of pEl is different from the parent's schema
         - if pEl is the (main) root element 
      */ 
      if ((pEl->ElTypeNumber == pEl->ElStructSchema->SsRootElem) ||
          ((pEl->ElParent != NULL) &&
           (pEl->ElStructSchema != pEl->ElParent->ElStructSchema)))
        {
          ns_prefix = ExportElemNsPrefix (doc, pEl);
          if (ns_prefix != NULL)
            {
              buffer = (char *)TtaGetMemory (strlen (ns_prefix) + 2);
              strcpy (buffer, ns_prefix);
              strcat (buffer, ":");
              SetVariableBuffer (pTSch, "ElemPrefixBuffer", buffer);
              TtaFreeMemory (buffer);
            }
        }

      removeEl = FALSE;
      ignoreEl = FALSE;
      pSS = pEl->ElStructSchema;
      elemType = pEl->ElTypeNumber;
      /* envoie l'evenement ElemExport.Pre a l'application, si elle */
      /* le demande */
      notifyEl.event = TteElemExport;
      notifyEl.document = doc;
      notifyEl.element = (Element) pEl;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = elemType;
      notifyEl.elementType.ElSSchema = (SSchema) pSS;
      notifyEl.position = 0;
      if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        /* l'application refuse que Thot sauve l'element */
        return;
      /* les attributs n'ont pas ete traduits */
      pEl->ElTransAttr = FALSE;
      /* la presentation n'a pas ete traduite */
      pEl->ElTransPres = FALSE;
      /* s'il s'agit de l'element racine d'une nature, on prend les regles */
      /* de presentation (s'il y en a) de la regle nature dans la structure */
      /* englobante. */
      /* on ne traite pas les marques de page */
      if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
        if (pEl->ElParent != NULL)
          /* il y a un englobant */
          if (pEl->ElParent->ElStructSchema != pEl->ElStructSchema)
            /* cherche la regle introduisant la nature dans le schema de */
            /* structure de l'englobant. */
            {
              pParentSS = pEl->ElParent->ElStructSchema;
              found = FALSE;
              i = 0;
              do
                {
                  pSRule = pParentSS->SsRule->SrElem[i++];
                  if (pSRule->SrConstruct == CsNatureSchema)
                    if (pSRule->SrSSchemaNat == pEl->ElStructSchema)
                      found = TRUE;
                }
              while (!found && i < pParentSS->SsNRules);
              if (found)
                {
                  pTS = GetTranslationSchema (pEl->ElParent->ElStructSchema);
                  if (pTS != NULL)
                    if (pTS->TsElemTRule->TsElemTransl[i - 1] != NULL)
                      /* il y a des regles de traduction pour la nature, on */
                      /* les prend */
                      {
                        pTSch = pTS;
                        pSS = pEl->ElParent->ElStructSchema;
                        elemType = i;
                      }
                }
            }
      if (pTSch)
        /* on ne traduit pas les elements dont le schema de structure n'a */
        /* pas de schema de traduction correspondant */
        {
          /* if needed, record the current line number of the main output file
             in the element being translated */
          if (recordLineNb)
            pEl->ElLineNb = OutFile[1].OfLineNumber + 1;
          // chek if it is a skipped XTiger element
          /* Cherche et applique les regles de traduction associees au type */
          /* de l'element et qui doivent s'appliquer avant la traduction du */
          /* contenu de l'element */
          skip = (Skip_Template &&
                  pTSch->TsStructName && !strcmp (pTSch->TsStructName, "Template"));
          if (skip)
            removeEl = ignoreEl = FALSE;
          else
            {
              ApplyElTypeRules (TBefore, &transChar, &withBreak, &removeEl, &ignoreEl,
                                pEl, elemType, pTSch, pSS, doc, recordLineNb);
              /* on ne traduit les attributs que si ce n'est pas deja fait par */
              /* une regle Create Attributes associee au type et si on n'a pas */
              /* rencontre' de re`gle Ignore */
              if (!pEl->ElTransAttr && !ignoreEl)
                {
                  /* Export the namespace declarations associated with  
                     this element before exporting its attributes */
                  if (IsTranslateTag (pTSch, pSS) != 0)
                    ExportNsDeclaration (doc, pEl);
                  /* Parcourt les attributs de l'element et applique les regles
                     des attributs qui doivent etre appliquees avant la
                     traduction du contenu de l'element */
                  ApplyAttrRules (TBefore, pEl, &removeEl, &ignoreEl, &transChar,
                                  &withBreak, doc, recordLineNb);
                }
              /* on ne traduit la presentation que si ce n'est pas deja fait par */
              /* une regle Create Presentation et si on n'a pas rencontre' de */
              /* regle Ignore */
              if (!pEl->ElTransPres && !ignoreEl)
                /* Parcourt les presentations de l'element et applique les regles
                   de traduction correspondantes qui doivent ^etre appliquees
                   avant la traduction du contenu de l'element */
                ApplyPresTRules (TBefore, pEl, &removeEl, &ignoreEl, &transChar,
                                 &withBreak, NULL, doc, recordLineNb);
            }
          /* traduit le contenu de l'element, sauf si on a deja rencontre' */
          /* une regle Remove ou Ignore pour cet element. */
          if (!removeEl && !ignoreEl)
            {
              /* pas de regle Remove ni Ignore */
              if (pEl->ElTerminal)
                /* c'est une feuille, applique les regles de traduction des */
                /* feuilles et sort le contenu dans le fichier principal */
                TranslateLeaf (pEl, transChar, withBreak, 1, doc);
              else
                /* ce n'est pas une feuille, traduit successivement tous les */
                /* fils de l'element */
                {
                  pChild = pEl->ElFirstChild;
                  while (pChild != NULL)
                    {
                      TranslateTree (pChild, doc, transChar, withBreak, enforce,
                                     recordLineNb);
                      pChild = pChild->ElNext;
                    }
                }
            }
          /* marque que les regles qui doivent etre appliquees apres */
          /* la traduction du contenu et qui sont associees aux attributs */
          /* et a la presentation n'ont pas encore ete appliquees */
          pEl->ElTransAttr = FALSE;    /* les attributs n'ont pas ete traduits */
          pEl->ElTransPres = FALSE;    /* la presentation n'a pas ete traduite */
          /* on ne traduit la presentation que si ce n'est pas deja fait par */
          /* une regle Create Presentation et si on n'a pas rencontre' de */
          /* regle Ignore */
          if (!skip)
            {
              if (!pEl->ElTransPres && !ignoreEl)
                /* Parcourt les presentations de l'element et applique les regles
                   de traduction correspondantes qui doivent ^etre appliquees
                   apres la traduction du contenu */
                ApplyPresTRules (TAfter, pEl, &removeEl, &ignoreEl, &transChar,
                                 &withBreak, NULL, doc, recordLineNb);
              if (!pEl->ElTransAttr && !ignoreEl)
                /* Parcourt les attributs de l'element et applique les regles des
                   attributs qui doivent etre appliquees apres la traduction du
                   contenu */
                ApplyAttrRules (TAfter, pEl, &removeEl, &ignoreEl, &transChar,
                                &withBreak, doc, recordLineNb);
              /* Cherche et applique les regles associees au type de l'element et
                 qui doivent s'appliquer apres la traduction du contenu */
              if (!ignoreEl)
                ApplyElTypeRules (TAfter, &transChar, &withBreak, &removeEl,
                                  &ignoreEl, pEl, elemType, pTSch, pSS, doc,
                                  recordLineNb);
            }
          if (!enforce)
            /* marque que l'element a ete traite' */
            pEl->ElTransContent = TRUE;
          /* envoie l'evenement ElemExport.Post a l'application, si elle */
          /* le demande */
          notifyEl.event = TteElemExport;
          notifyEl.document = doc;
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
        }
    }
}


/*----------------------------------------------------------------------
  ResetTranslTags   remet a zero tous les indicateurs "deja traduit" 
  de l'arbre de racine pEl.                                       
  ----------------------------------------------------------------------*/
static void ResetTranslTags (PtrElement pEl)
{
  PtrElement          pChild;

  if (pEl != NULL)
    {
      pEl->ElTransContent = FALSE;
      pEl->ElTransAttr = FALSE;
      pEl->ElTransPres = FALSE;
      if (!pEl->ElTerminal)
        {
          pChild = pEl->ElFirstChild;
          while (pChild != NULL)
            {
              ResetTranslTags (pChild);
              pChild = pChild->ElNext;
            }
        }
    }
}

/*----------------------------------------------------------------------
  InitOutputFiles initialise les fichiers de sortie.              
  ----------------------------------------------------------------------*/
static void InitOutputFiles (FILE *mainFile)
{
  /* Entry 0: standard output */
  OutFile[0].OfFileName[0] = EOS;
  OutFile[0].OfFileDesc = NULL;
  OutFile[0].OfBufferLen = 0;
  OutFile[0].OfLineLen = 0;
  OutFile[0].OfIndent = 0;
  OutFile[0].OfPreviousIndent = 0;
  OutFile[0].OfLineNumber = 0;
  OutFile[0].OfStartOfLine = TRUE;
  OutFile[0].OfCannotOpen = FALSE;

  /* Entry 1: main output file */
  OutFile[1].OfFileName[0] = EOS;
  OutFile[1].OfFileDesc = mainFile;
  OutFile[1].OfBufferLen = 0;
  OutFile[1].OfLineLen = 0;
  OutFile[1].OfIndent = 0;
  OutFile[1].OfPreviousIndent = 0;
  OutFile[1].OfLineNumber = 0;
  OutFile[1].OfStartOfLine = TRUE;
  OutFile[1].OfCannotOpen = FALSE;
  NOutFiles = 2;
}

/*----------------------------------------------------------------------
  FlushOutputFiles flushes all output buffers.
  ----------------------------------------------------------------------*/
static void FlushOutputFiles ()
{
  int                 i, f;

  for (f = 1; f < NOutFiles; f++)
    if (OutFile[f].OfFileDesc != NULL)
      {
        for (i = 0; i < OutFile[f].OfBufferLen; i++)
          putc (OutFile[f].OfBuffer[i], OutFile[f].OfFileDesc);
        if (OutFile[f].OfFileDesc != NULL)
          TtaWriteClose (OutFile[f].OfFileDesc);
      }
}

/*----------------------------------------------------------------------
  ExportDocument outputs the document doc with the translation schema
  tschema into the file fName.
  Returns TRUE if sucessful.
  ----------------------------------------------------------------------*/
ThotBool ExportDocument (Document doc, const char *fName, const char *tschema,
                         ThotBool recordLineNb)
{
  PtrDocument         pDoc;
  FILE               *outputFile; /* fichier de sortie principal */
  int                 i;
  ThotBool            ok = TRUE;

  /* tschema is null when we export a generic xml document */
  pDoc = LoadedDocument[doc - 1];  
  /* does it have to generate simple LF or CRLF */
  TtaGetEnvBoolean ("EXPORT_CRLF", &ExportCRLF);
  DocumentHasDocType = FALSE;
  WithMath = FALSE;
  /* check if the user forced a line length */
  if (pDoc->DocSSchema && strcmp (pDoc->DocSSchema->SsName, "TextFile"))
    {
      TtaGetEnvInt ("EXPORT_LENGTH", &(ExportLength));
      if (ExportLength == 0)
        /* the user asks for the default value */
        ExportLength = 80;
      /* check if the document has a doctype */
      if (GetDoctypeFunction)
        /* check if there is a DOCTYPE */
        (*(Proc3)GetDoctypeFunction) ((void *)doc, (void *)&DocumentHasDocType,
                                      (void *) &WithMath);
    }
  else
    // Keep lines as they are for text files
    ExportLength = 0;

  /* create the main output file */
  outputFile = TtaWriteOpen (fName);
  if (outputFile == NULL)
    /* not created */
    ok = FALSE;
  else
    {
      /* split directory name and file name */
      strncpy (fileDirectory, fName, MAX_PATH);
      fileDirectory[MAX_PATH - 1] = EOS;
      i = strlen (fileDirectory);
      while (i > 0 && fileDirectory[i] != DIR_SEP)
        i--;
      if (fileDirectory[i] == DIR_SEP)
        {
          strcpy (fileName, &fileDirectory[i + 1]);
          fileDirectory[i + 1] = EOS;
        }
      else
        {
          strcpy (fileName, &fileDirectory[i]);
          fileDirectory[i] = EOS;
        }
      /* charge le schema de traduction du document */
      if ((tschema != NULL) &&
          !LoadTranslationSchema (tschema, pDoc->DocSSchema) != 0)
        {
          /* echec au chargement du schema de traduction */
          TtaReadClose (outputFile);
          ok = FALSE;
        }
      else
        {
          /* separe nom de fichier et extension */
          fileExtension[0] = EOS;
          i = strlen (fileName);
          i--;
          while (i > 0 && fileName[i] != '.')
            i--;
          if (fileName[i] == '.')
            {
              strncpy (fileExtension, &fileName[i], MAX_PATH);
              fileName[i] = EOS;
            }
          InitOutputFiles (outputFile);
          /* remet a zero les indicateurs "deja traduit" de tous les elements*/
          /* du document */
          ResetTranslTags (pDoc->DocDocElement);
          /* traduit l'arbre principal du document */
          if (tschema == NULL)
            {
              if (pDoc->DocDocElement != NULL)
                ExportXmlElement (doc, pDoc->DocDocElement, TRUE, TRUE);
            }
          else
            TranslateTree (pDoc->DocDocElement, doc, TRUE, TRUE, FALSE,
                           recordLineNb);
          /* vide ce qui traine dans les buffers de sortie */
          /* et ferme ces fichiers */
          FlushOutputFiles ();
        }
    }
  ClearTranslationSchemasTable ();
  fflush (stdout);
  fflush (stderr);
  return (ok);
}

/*----------------------------------------------------------------------
  ExportAttrNsPrefix
  Search the namespace prefix associated with the attribute pAttr.
  ----------------------------------------------------------------------*/
static char* ExportAttrNsPrefix (Document doc, PtrElement pNode,
                                 PtrAttribute pAttr)
{
  PtrNsUriDescr    uriDecl;
  PtrNsPrefixDescr prefixDecl;
  PtrDocument      pDoc;
  ThotBool         found;
  int              i;
  char            *ns_prefix = NULL;

  if (pNode == NULL || pNode->ElTerminal)
    return (ns_prefix);
  pDoc = LoadedDocument[doc - 1];
  if (pNode->ElStructSchema == pAttr->AeAttrSSchema)
    /* The attribute belongs to the same namespace than the element */
    return (ns_prefix);

  if (pDoc->DocNsUriDecl == NULL)
    /* There is no namespace declaration for this document */
    return (ns_prefix);

  if (pAttr->AeAttrSSchema->SsUriName == NULL)
    /* No URI refefence fot this schema */
    return (ns_prefix);

  i = 0;
  /* Search all the namespace declarations declared for the document */
  uriDecl = pDoc->DocNsUriDecl;
  found = FALSE;
  while (uriDecl != NULL && !found)
    {
      if (uriDecl->NsUriName != NULL &&
          (strcmp (uriDecl->NsUriName, pAttr->AeAttrSSchema->SsUriName) == 0))
        {
          /* The attribute uri has been found */
          /* Search the associated prefix */
          found = TRUE;
          prefixDecl = uriDecl->NsPtrPrefix;
          while (prefixDecl != NULL)
            {
              if ((pNode == prefixDecl->NsPrefixElem) ||
                  (ElemIsAnAncestor (prefixDecl->NsPrefixElem, pNode)))
                {
                  ns_prefix = prefixDecl->NsPrefixName;
                  prefixDecl = NULL;
                }
              else
                prefixDecl = prefixDecl->NsNextPrefixDecl;
            }
        }
      uriDecl = uriDecl->NsNextUriDecl;
    }
  return (ns_prefix);
}

/*----------------------------------------------------------------------
  ExportElemNsPrefix
  Search the namespace prefix associated with the element pNode.
  ----------------------------------------------------------------------*/
static char* ExportElemNsPrefix (Document doc, PtrElement pNode)

{
  PtrNsUriDescr    uriDecl;
  PtrNsPrefixDescr prefixDecl;
  PtrDocument      pDoc;
  ThotBool         found;
  int              i;
  char            *ns_prefix = NULL;

  if (pNode == NULL || pNode->ElTerminal)
    return (ns_prefix);
  pDoc = LoadedDocument[doc - 1];
  if (pDoc->DocNsUriDecl == NULL)
    /* There is no namespace declaration for this document */
    return (ns_prefix);

  if (pNode->ElStructSchema->SsUriName == NULL)
    /* No URI refefence fot this schema */
    return (ns_prefix);

  i = 0;
  /* Search all the namespace declarations declared for this element */
  uriDecl = pDoc->DocNsUriDecl;
  found = FALSE;
  while (uriDecl != NULL && !found)
    {
      if (uriDecl->NsUriName != NULL &&
          (strcmp (uriDecl->NsUriName, pNode->ElStructSchema->SsUriName) == 0))
        {
          /* The element schema uri has been found */
          /* Search the associated prefix */
          found = TRUE;
          prefixDecl = uriDecl->NsPtrPrefix;
          while (prefixDecl != NULL)
            {
              if (pNode == prefixDecl->NsPrefixElem)
                {
                  ns_prefix = prefixDecl->NsPrefixName;
                  prefixDecl = NULL;
                }
              else if (ElemIsAnAncestor (prefixDecl->NsPrefixElem, pNode))
                {
                  ns_prefix = prefixDecl->NsPrefixName;
                  prefixDecl = prefixDecl->NsNextPrefixDecl;
                }
              else
                prefixDecl = prefixDecl->NsNextPrefixDecl;
            }
        }
      uriDecl = uriDecl->NsNextUriDecl;
    }
  return (ns_prefix);
}

/*----------------------------------------------------------------------
  ExportNsDeclaration
  Export the namespace attributes of the Element pNode into
  the main output file
  length: max length to export.                         
  ----------------------------------------------------------------------*/
static void ExportNsDeclaration (Document doc, PtrElement pNode)
{
  PtrNsUriDescr    uriDecl;
  PtrNsPrefixDescr prefixDecl;
  PtrDocument      pDoc;
  int              i, fnum;

  if (pNode == NULL || pNode->ElTerminal || ElementIsHidden (pNode))
    return;
  fnum = 1; /* main output file */
  pDoc = LoadedDocument[doc - 1];
  if (pDoc->DocNsUriDecl == NULL)
    /* There is no namespace declaration for this document */
    return;

  i = 0;
  /* Search all the namespace declarations declared for this element */
  uriDecl = pDoc->DocNsUriDecl;
  while (uriDecl != NULL)
    {
      prefixDecl = uriDecl->NsPtrPrefix;
      while (prefixDecl != NULL)
        {
          if (prefixDecl->NsPrefixElem == pNode &&
              (uriDecl->NsUriName || prefixDecl->NsPrefixName))
            {
              if (i > 0)
                ExportXmlBuffer (doc, (unsigned char *)"\n", FALSE);
              /* A Namespace declaration has been found for this element */
              ExportXmlBuffer (doc, (unsigned char *)" xmlns", FALSE);
              if (prefixDecl->NsPrefixName != NULL)
                {
                  ExportXmlBuffer (doc, (unsigned char *)":", FALSE);
                  ExportXmlBuffer (doc, (unsigned char *)prefixDecl->NsPrefixName, FALSE);
                }
              ExportXmlBuffer (doc, (unsigned char *)"=\"", FALSE);
              ExportXmlBuffer (doc, (unsigned char *)uriDecl->NsUriName, FALSE);
              ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
              i++;
            }
          prefixDecl = prefixDecl->NsNextPrefixDecl;
        }
      uriDecl = uriDecl->NsNextUriDecl;
    }

  return;
}

/*----------------------------------------------------------------------
  ExportXmlText 
  Exports in the fileDescriptor file the content of a list of buffers
  pointed by pBT.
  length gives the max length of exported lines or 0.                         
  ----------------------------------------------------------------------*/
static void ExportXmlText (Document doc, PtrTextBuffer pBT, ThotBool lineBreak,
                           ThotBool translate, ThotBool entityName)
{
  PtrTextBuffer       b;
  wchar_t             c;
  int                 i, fnum;

  fnum = 1; /* main output file */
  b = pBT;
  while (b)
    {
      i = 0;
      if (b->BuContent[i] == SPACE && lineBreak && !entityName)
        {
          // generate a linebreak
          PutChar (EOL, fnum, NULL, doc, lineBreak, translate, entityName);
          i++;
        }
      while (i < b->BuLength && b->BuContent[i] != EOS)
        {
          c = (wchar_t) b->BuContent[i];
         if (c == 0x22 || c == 0x26 || c == 0x3C || c == 0x3E || c == 0xA0)
           // generate an entity
           entityName = TRUE;
          PutChar (c, fnum, NULL, doc, lineBreak, translate, entityName);
          /* Next character */
          i++;
        }
      /* Export the following text buffer for the same element */
      b = b->BuNext;
    }
}

/*----------------------------------------------------------------------
  ExportXmlElText 
  Exports in the fileDescriptor file the content of the element pNode.
  pBT points the first text buffer.
  length gives the max length of exported lines or 0.                         
  ----------------------------------------------------------------------*/
static void ExportXmlElText (Document doc,  PtrElement pNode,
                             PtrTextBuffer pBT, ThotBool lineBreak)
{
  PtrElement          parent;
  PtrSRule            pRe1;
  ThotBool            translate;
  ThotBool            entityName;


  /* Don't translate predefined-entities for some elements */
  translate = TRUE;
  parent = pNode->ElParent;
  pRe1 = parent->ElStructSchema->SsRule->SrElem[parent->ElTypeNumber - 1];
  if (pRe1->SrOrigName != NULL &&
      ((strcmp (pRe1->SrOrigName, "xmlcomment_line") == 0) ||
       (strcmp (pRe1->SrOrigName, "xmlpi_line") == 0) ||
       (strcmp (pRe1->SrOrigName, "cdata_line") == 0) ||
       (strcmp (pRe1->SrOrigName, "doctype_line") == 0)))
    translate = FALSE;
  /* in MathML, try to generate the name of the char. */
  entityName = (strcmp (pNode->ElStructSchema->SsName, "MathML") == 0);
  /* Export the text buffer content */
  ExportXmlText (doc, pBT, lineBreak, translate, entityName);
}

/*----------------------------------------------------------------------
  ExportXmlElement
  Produces in a file a human-readable form of an XML abstract tree.
  Parameters:
  doc: the exported document.
  pEl: the root element of the tree to be exported.
  lineBreak: TRUE when no preverse space is requested
  This file must be open when calling the function.
  ----------------------------------------------------------------------*/
void ExportXmlElement (Document doc, PtrElement pEl,
                       ThotBool lineBreak, ThotBool recordLineNb)
{
  PtrElement          pChild;
  PtrSRule            pRe1;
  PtrAttribute        pAttr;
  PtrTtAttribute      pAttr1;
  PtrDocument         pDoc;
  char               *startName = NULL;
  char               *endName = NULL;
  char               *ns_prefix;
  int                 fnum = 1, len_ns;
  ThotBool            specialTag;

  if (pEl)
    {
      /* Main output file */
      if (strcmp (pEl->ElStructSchema->SsName, "HTML") == 0)
        {
          if (LoadTranslationSchema ("HTMLTX", pEl->ElStructSchema))
            TranslateTree (pEl, doc, TRUE, lineBreak, FALSE, FALSE);
        }
      else if (strcmp (pEl->ElStructSchema->SsName, "MathML") == 0)
        {
          if (LoadTranslationSchema ("MathMLT", pEl->ElStructSchema))
            TranslateTree (pEl, doc, TRUE, lineBreak, FALSE, FALSE);
        }
      else if (strcmp (pEl->ElStructSchema->SsName, "SVG") == 0)
        {
          if (LoadTranslationSchema ("SVGT", pEl->ElStructSchema))
            TranslateTree (pEl, doc, TRUE, lineBreak, FALSE, FALSE);
        }
      else if (strcmp (pEl->ElStructSchema->SsName, "Template") == 0)
        {
          if (LoadTranslationSchema ("TemplateT", pEl->ElStructSchema))
            TranslateTree (pEl, doc, TRUE, lineBreak, FALSE, FALSE);
        }
      else
        {
          pDoc = LoadedDocument[doc - 1];
          if (!pEl->ElTerminal)
            {
              if (pEl != pDoc->DocDocElement)
                {
                  specialTag = FALSE;
                  /* Export the element name */
                  pRe1 = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
                  len_ns = 0;
                  if (ExportElemNsPrefix (doc, pEl) != NULL)
                    len_ns = strlen (ExportElemNsPrefix (doc, pEl)) + 1;
                  startName = (char *)TtaGetMemory (strlen (pRe1->SrOrigName) + 2 + len_ns + 1);
                  endName = (char *)TtaGetMemory (strlen (pRe1->SrOrigName) + 3 + len_ns + 1);
                  if (TypeHasException (ExcHidden, pEl->ElTypeNumber,	pEl->ElStructSchema))
                    {
                      /* Don't export hidden elements */
                      startName[0] = EOS;
                      if (pEl->ElNext && strcmp (pRe1->SrOrigName, "xmlbr") == 0)
                        // but keep newlines
                        strcpy (startName, "\n");
                      pChild = pEl->ElNext;
                      endName[0] = EOS;
                      specialTag = TRUE;
                    }
                  else
                    {
                      startName[0] = EOS;
                      /* test the exception xml:space="preserve" */
                      if (TtaIsElementWithSpacePreserve ((Element) pEl))
                        lineBreak = FALSE;

                      if (strcmp (pRe1->SrOrigName, "xmlcomment") == 0)
                        {
                          strcat (startName, "<!--");
                          strcpy (endName, "-->");
                          specialTag = TRUE;
                        }
                      else if (strcmp (pRe1->SrOrigName, "xmlpi") == 0)
                        {
                          strcat (startName, "<?");
                          strcpy (endName, "?>\n");
                          specialTag = TRUE;
                        }
                      else if (strcmp (pRe1->SrOrigName, "doctype") == 0)
                        {
                          startName[0] = EOS;
                          endName[0] = EOS;
                          specialTag = TRUE;
                        }
                      else if (strcmp (pRe1->SrOrigName, "cdata") == 0)
                        {
                          strcat (startName, "<![CDATA[");
                          strcpy (endName, "]]>");
                          specialTag = TRUE;
                        }
                      else
                        {
                          strcat (startName, "<");
                          strcpy (endName, "</");
                          ns_prefix = ExportElemNsPrefix (doc, pEl);
                          if (ns_prefix != NULL)
                            {
                              strcat (startName, ns_prefix);
                              strcat (startName, ":");
                              strcat (endName, ns_prefix);
                              strcat (endName, ":");
                            }
                          strcat (startName, pRe1->SrOrigName);
                          strcat (endName, pRe1->SrOrigName);
                          strcat (endName, ">");
                        }
                    }

                  /* if needed, record the current line number of the main
                     output file in the element being translated */
                  if (recordLineNb)
                    pEl->ElLineNb = OutFile[fnum].OfLineNumber + 1;
                  ExportXmlBuffer (doc, (unsigned char *)startName, FALSE);

                  /* Export the namespace declarations */
                  if (!specialTag)
                    ExportNsDeclaration (doc, pEl);
		  
                  /* Export the attributes */
                  pAttr = pEl->ElFirstAttr;
                  while (pAttr != NULL)
                    {
                      if (!AttrHasException (ExcInvisible, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
                        /* Don't export invisible attributes */
                        {
                          ExportXmlBuffer (doc, (unsigned char *)" ", lineBreak);
                          /* Export the attribute prefix if it exists */
                          ns_prefix = ExportAttrNsPrefix (doc, pEl, pAttr);
                          if (ns_prefix != NULL)
                            {
                              ExportXmlBuffer (doc, (unsigned char *)ns_prefix, FALSE); 
                              ExportXmlBuffer (doc, (unsigned char *)":", FALSE);
                            }
                          /* Export the attribute name */
                          pAttr1 = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum-1];
                          ExportXmlBuffer (doc, (unsigned char *)pAttr1->AttrName, lineBreak);
                          ExportXmlBuffer (doc, (unsigned char *)"=", lineBreak);
                          /* Export the attribute's value */
                          switch (pAttr1->AttrType)
                            {
                            case AtNumAttr:
                              ExportXmlBuffer (doc, (unsigned char*)pAttr->AeAttrValue, FALSE);
                              break;
                            case AtTextAttr:
                              if (pAttr->AeAttrText)
                                {
                                  ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
                                  /* Export the text buffer content */
                                  ExportXmlText (doc, pAttr->AeAttrText, FALSE, TRUE, FALSE);
                                  ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
                                }
                              break;
                            case AtEnumAttr:
                              ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
                              ExportXmlBuffer (doc, (unsigned char *)pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1], FALSE);
                              ExportXmlBuffer (doc, (unsigned char *)"\"", FALSE);
			      
                              break;
                            default:
                              break;
                            }
                        }
                      pAttr = pAttr->AeNext;
                    }
		  
                  if ((startName[0] != EOS) && !specialTag)
                    ExportXmlBuffer (doc, (unsigned char *)">", lineBreak);
                  if (startName != NULL)
                    TtaFreeMemory (startName);
                }
	      
              /* Recursive export */
              pChild = pEl->ElFirstChild;
              while (pChild)
                {
                  ExportXmlElement (doc, pChild, lineBreak, recordLineNb);
                  pChild = pChild->ElNext;
                }
	      
              /* Export End tag */
              if (pEl != pDoc->DocDocElement)
                ExportXmlBuffer (doc, (unsigned char *)endName, lineBreak);
              if (endName != NULL)
                TtaFreeMemory (endName);
            }
          else
            {
              /* It is a terminal element */
              if (recordLineNb)
                pEl->ElLineNb = OutFile[fnum].OfLineNumber + 1;
              switch (pEl->ElLeafType)
                {
                case LtPicture:
                  ExportXmlElText (doc, pEl, pEl->ElText, lineBreak);
                  break;
                case LtText:
                  ExportXmlElText (doc, pEl, pEl->ElText, lineBreak);
                  break;
                default:
                  break;	
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  ExportTree  
  Exporte le sous arbre pointe par pEl du document doc,
  selon le schema de traduction de nom tschema et produit le resultat
  dans le fichier de nom fName ou dans le buffer.
  ----------------------------------------------------------------------*/
void ExportTree (PtrElement pEl, Document doc, const char *fName,
                 const char *tschema)
{
  PtrDocument         pDoc;
  int                 i;
  FILE               *outputFile; /* fichier de sortie principal */

  pDoc = LoadedDocument[doc - 1];
  /* cree le fichier de sortie principal */
  outputFile = TtaWriteOpen (fName);
  if (outputFile)
    /* le fichier de sortie principal a ete cree' */
    {
      /* separe nom de directory et nom de fichier */
      strncpy (fileDirectory, fName, MAX_PATH);
      fileDirectory[MAX_PATH - 1] = EOS;
      i = strlen (fileDirectory);
      while (i > 0 && fileDirectory[i] != DIR_SEP)
        i--;
      if (fileDirectory[i] == DIR_SEP)
        {
          strcpy (fileName, &fileDirectory[i + 1]);
          fileDirectory[i + 1] = EOS;
        }
      else
        {
          strcpy (fileName, &fileDirectory[i]);
          fileDirectory[i] = EOS;
        }
      /* charge le schema de traduction du document */
      if (!LoadTranslationSchema (tschema, pEl->ElStructSchema) != 0 ||
          !GetTranslationSchema (pEl->ElStructSchema) != 0)
        /* echec au chargement du schema de traduction */
        TtaReadClose (outputFile);
      else
        {
          /* separe nom de fichier et extension */
          fileExtension[0] = EOS;
          i = strlen (fileName);
          i--;
          while (i > 0 && fileName[i] != '.')
            i--;
          if (fileName[i] == '.')
            {
              strncpy (fileExtension, &fileName[i], MAX_PATH);
              fileName[i] = EOS;
            }
          InitOutputFiles (outputFile);
          /* remet a zero les indicateurs "deja traduit" de tous les elements */
          /* de l'arbre a traduire */
          ResetTranslTags (pEl);
          /* traduit l'arbre */
          TranslateTree (pEl, doc, TRUE, TRUE, FALSE, FALSE);
          /* vide ce qui traine dans les buffers de sortie */
          /* et ferme ces fichiers */
          FlushOutputFiles ();
        }
    }
  ClearTranslationSchemasTable ();
  fflush (stdout);
  fflush (stderr);
}

/*----------------------------------------------------------------------
  TtaExportDocument

  Saves a whole document into a file in a particular format. The output
  format is specified by a translation schema. The document is not closed
  by the function and it can still be accessed by the application program.

  Parameters:
  document: the document to be exported.
  fileName: name of the file in which the document must be saved,
  including the directory name.
  tschema: name of the translation schema to be used. The directory
  name must not be specified in parameter tschema. See
  function TtaSetSchemaPath.
  ----------------------------------------------------------------------*/
ThotBool TtaExportDocument (Document document, const char *fileName, const char *tschema)
{
  ThotBool ok = FALSE;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      Skip_Template = TRUE; // skip template elements
      ok = ExportDocument (document, fileName, tschema, FALSE);
      Skip_Template = FALSE;
    }
  return (ok);
}


/*----------------------------------------------------------------------
  TtaExportDocumentWithNewLineNumbers

  Saves a whole document into a file in a particular format. The output
  format is specified by a translation schema. The document is not closed
  by the function and it can still be accessed by the application program.
  Line numbers recorded in document elements are updated according to the
  generated file.

  Parameters:
  document: the document to be exported.
  fileName: name of the file in which the document must be saved,
  including the directory name.
  tschema: name of the translation schema to be used. The directory
  name must not be specified in parameter tschema (See TtaSetSchemaPath)
  skipXTiger is TRUE if XTiger elements are removed
  ----------------------------------------------------------------------*/
ThotBool TtaExportDocumentWithNewLineNumbers (Document document, const char *fileName,
                                              const char *tschema, ThotBool skipXTiger)
{
  ThotBool ok = FALSE;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (fileName == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      /* parameter document is correct */
      Skip_Template = skipXTiger; // should amaya skip template elements
      ok = ExportDocument (document, fileName, tschema, TRUE);
      Skip_Template = TRUE;
    }
  return (ok);
}
