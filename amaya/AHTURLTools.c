/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * AHTURLTools.c: contains all the functions for testing, manipulating,
 * and normalizing URLs. It also contains a local copy of the libWWW
 * URL parsing functions.
 *
 * Authors: J. Kahan, I. Vatton
 *          R. Guetari (Stuff related to Windows).
 *
 */
 
#define THOT_EXPORT extern
#include "amaya.h"

#include "init_f.h"
#include "AHTURLTools_f.h"

#define MAX_PRINT_URL_LENGTH 50
typedef struct _HTURI {
    STRING  access;		/* Now known as "scheme" */
    STRING  host;
    STRING  absolute;
    STRING  relative;
    STRING  fragment;
} HTURI;


/*----------------------------------------------------------------------
  ConvertToLowerCase
  Converts a string to lowercase.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ConvertToLowerCase (STRING string)
#else  /* __STDC__ */
void         ConvertToLowerCase (string)
STRING       string;

#endif /* __STDC__ */
{
 int i;

 if (!string)
   return;

 for (i = 0; string[i] != EOS; i++)
   string[i] = utolower (string[i]);
}

/*----------------------------------------------------------------------
  EscapeChar
  writes the equivalent escape code of a char in a string
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         EscapeChar (STRING string, UCHAR_T c)
#else
void         EscapeChar (string, c)
STRING              string;
UCHAR_T               c;

#endif
{
   c &= 0xFF;                   /* strange behavior under solaris? */
   usprintf (string, TEXT("%02x"), (unsigned int) c);
}

/*----------------------------------------------------------------------
  EscapeURL
  Takes a URL and escapes all protected chars into
  %xx sequences. Also, removes any leading white spaces
  Returns either NULL or a new buffer, which must be freed by the caller
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING EscapeURL (const STRING url)
#else
STRING EscapeURL (url)
STRING url;
#endif /* __STDC__ */
{
  STRING buffer;
  int buffer_len;
  int buffer_free_mem;
  PCHAR_T ptr;
  int new_chars;
  void *status;

  if (url && *url)
    {
      buffer_free_mem = ustrlen (url) + 20;
      buffer = TtaAllocString (buffer_free_mem + 1);
      ptr = url;
      buffer_len = 0;

      while (*ptr)
        {
          switch (*ptr)
            {
              /* put here below all the chars that need to
                 be escaped into %xx */
            case 0x27: /* &amp */
            case 0x20: /* space */
              new_chars = 3; 
              break;

            default:
              new_chars = 1; 
              break;
            }

          /* see if we need extra room in the buffer */
          if (new_chars > buffer_free_mem)
            {
              buffer_free_mem = 20;
              status = TtaRealloc (buffer, sizeof (CHAR_T) 
				   * (buffer_len + buffer_free_mem + 1));
              if (status)
                buffer = (STRING) status;
              else {
                /* @@ maybe we should do some other behavior here, like
                   freeing the buffer and return a void thing */
                buffer[buffer_len] = EOS;
                break;
              }
            }
	  /* escape the char */
          if (new_chars == 3)
            {
              buffer[buffer_len] = TEXT('%');
              EscapeChar (&buffer[buffer_len+1], *ptr);
            }
          else
            buffer[buffer_len] = *ptr;

          /* update the status */
          buffer_len += new_chars;
          buffer_free_mem -= new_chars;
          /* examine the next char */
          ptr++;
        }
      buffer[buffer_len] = EOS;
    }
  else
    buffer = NULL;

  return (buffer);
}


/*----------------------------------------------------------------------
  ExplodeURL 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ExplodeURL (char *url, char **proto, char **host, char **dir, char **file)
#else
void                ExplodeURL (url, proto, host, dir, file)
char               *url;
char              **proto;
char              **host;
char              **dir;
char              **file;

#endif
{
   char            *curr, *temp;
   char             used_sep;

   if (url && strchr (url, URL_SEP))
     used_sep = URL_SEP;
   else
     used_sep = DIR_SEP;

   if ((url == NULL) || (proto == NULL) || (host == NULL) ||
       (dir == NULL) || (file == NULL))
      return;

   /* initialize every pointer */
   *proto = *host = *dir = *file = NULL;

   /* skip any leading space */
   while ((*url == SPACE) || (*url == TAB))
      url++;
   curr = url;
   if (*curr == 0)
      goto finished;

   /* go to the end of the URL */
   while ((*curr != EOS) && (*curr != SPACE) && (*curr != BSPACE) &&
	  (*curr != __CR__) && (*curr != EOL))
      curr++;

   /* mark the end of the chain */
   *curr = EOS;
   curr--;
   if (curr <= url)
      goto finished;

   /* search the next DIR_SEP indicating the beginning of the file name */
   do
     curr--;
   while ((curr >= url) && (*curr != used_sep));

   if (curr < url)
      goto finished;
   *file = curr + 1;

   /* mark the end of the dir */
   *curr = EOS;
   curr--;
   if (curr < url)
      goto finished;

   /* search for the DIR_STR indicating the host name start */
   while ((curr > url) && ((*curr != used_sep) || (*(curr + 1) != used_sep)))
      curr--;

   /* if we found it, separate the host name from the directory */
   if ((*curr == DIR_SEP) && (*(curr + 1) == used_sep))
     {
	*host = temp = curr + 2;
	while ((*temp != 0) && (*temp != used_sep))
	   temp++;
	if (*temp == used_sep)
	  {
	     *temp = EOS;
	     *dir = temp + 1;
	  }
     }
   else
     *dir = curr;

   if (curr <= url)
      goto finished;

   /* mark the end of the proto */
   *curr = EOS;
   curr--;
   if (curr < url)
      goto finished;

   if (*curr == TEXT(':'))
     {
	*curr = EOS;
	curr--;
     }
   else
      goto finished;

   if (curr < url)
      goto finished;
   while ((curr > url) && (isalpha (*curr)))
      curr--;
   *proto = curr;

 finished:;

#ifdef AMAYA_DEBUG
   fprintf (stderr, "ExplodeURL(%s)\n\t", url);
   if (*proto)
      fprintf (stderr, "proto : %s, ", *proto);
   if (*host)
      fprintf (stderr, "host : %s, ", *host);
   if (*dir)
      fprintf (stderr, "dir : %s, ", *dir);
   if (*file)
      fprintf (stderr, "file : %s ", *file);
   fprintf (stderr, "\n");
#endif

}


/*----------------------------------------------------------------------
   ExtractSuffix extract suffix from document nane.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ExtractSuffix (STRING aName, STRING aSuffix)
#else
void                ExtractSuffix (aName, aSuffix)
STRING              aName;
STRING              aSuffix;

#endif
{
   int                 lg, i;
   STRING              ptr, oldptr;

   if (!aSuffix || !aName)
     /* bad suffix */
     return;

   aSuffix[0] = EOS;
   lg = ustrlen (aName);
   if (lg)
     {
	/* the name is not empty */
	oldptr = ptr = &aName[0];
	do
	  {
	     ptr = ustrrchr (oldptr, TEXT('.'));
	     if (ptr)
		oldptr = &ptr[1];
	  }
	while (ptr);

	i = (int) (oldptr) - (int) (aName);	/* name length */
	if (i > 1)
	  {
	     aName[i - 1] = EOS;
	     if (i != lg)
		ustrcpy (aSuffix, oldptr);
	  }
     }
}

/*----------------------------------------------------------------------
  IsHTMLName                                                         
  returns TRUE if path points to an HTML resource.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsHTMLName (const CharUnit* path)
#else  /* __STDC__ */
ThotBool             IsHTMLName (path)
const CharUnit*      path;
#endif /* __STDC__ */
{
   CharUnit          temppath[MAX_LENGTH];
   CharUnit          suffix[MAX_LENGTH];
   CharUnit          nsuffix[MAX_LENGTH];
   int               i;

   if (!path)
      return (FALSE);

   StringCopy (temppath, path);
   ExtractSuffix (temppath, suffix);

   /* Normalize the suffix */
   i = 0;
   while (suffix[i] != CUS_EOS && i < MAX_LENGTH -1)
     {
       nsuffix[i] = ToLower (suffix[i]);
       i++;
     }
   nsuffix[i] = EOS;
   if (!StringCompare (nsuffix, CUSTEXT("html")) ||
       !StringCompare (nsuffix, CUSTEXT("htm")) ||
       !StringCompare (nsuffix, CUSTEXT("shtml")) ||
       !StringCompare (nsuffix, CUSTEXT("jsp")) ||
       !StringCompare (nsuffix, CUSTEXT("xht")) ||
       !StringCompare (nsuffix, CUSTEXT("xhtm")) ||
       !StringCompare (nsuffix, CUSTEXT("xhtml")))
     return (TRUE);
   else if (!StringCompare (nsuffix, CUSTEXT("gz")))
     {
       /* take into account compressed files */
       ExtractSuffix (temppath, suffix);       
       /* Normalize the suffix */
       i = 0;
       while (suffix[i] != CUS_EOS && i < MAX_LENGTH -1)
	 {
	   nsuffix[i] = ToLower (suffix[i]);
	   i++;
	 }
       nsuffix[i] = CUS_EOS;
       if (!StringCompare (nsuffix, CUSTEXT("html")) ||
           !StringCompare (nsuffix, CUSTEXT("htm")) ||
           !StringCompare (nsuffix, CUSTEXT("shtml")) ||
           !StringCompare (nsuffix, CUSTEXT("jsp")) ||
           !StringCompare (nsuffix, CUSTEXT("xht")) ||
           !StringCompare (nsuffix, CUSTEXT("xhtm")) ||
           !StringCompare (nsuffix, CUSTEXT("xhtml")))
 
	 return (TRUE);
       else
	 return (FALSE);
     }
   else
     return (FALSE);
}

/*----------------------------------------------------------------------
  IsXMLName                                                         
  returns TRUE if path points to an XML resource.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsXMLName (const STRING path)
#else  /* __STDC__ */
ThotBool             IsXMLName (path)
const STRING        path;
#endif /* __STDC__ */
{
   CHAR_T              temppath[MAX_LENGTH];
   CHAR_T              suffix[MAX_LENGTH];

   if (!path)
      return (FALSE);

   ustrcpy (temppath, path);
   ExtractSuffix (temppath, suffix);

   if (!ustrcasecmp (suffix, TEXT("xml")) ||
       !ustrcasecmp (suffix, TEXT("xht")) ||
       !ustrcmp (suffix, TEXT("xhtm")) ||
       !ustrcmp (suffix, TEXT("xhtml")))
     return (TRUE);
   else if (!ustrcmp (suffix, TEXT("gz")))
     {
       /* take into account compressed files */
       ExtractSuffix (temppath, suffix);       
       if (!ustrcasecmp (suffix, TEXT("xml")) ||
	   !ustrcasecmp (suffix, TEXT("xht")) ||
	   !ustrcmp (suffix, TEXT("xhtm")) ||
	   !ustrcmp (suffix, TEXT("xhtml")))
	 return (TRUE);
       else
	 return (FALSE);
     }
   else
     return (FALSE);
}

/*----------------------------------------------------------------------
  IsCSSName                                                         
  returns TRUE if path points to an XML resource.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsCSSName (const STRING path)
#else  /* __STDC__ */
ThotBool             IsCSSName (path)
const STRING        path;
#endif /* __STDC__ */
{
   CHAR_T              temppath[MAX_LENGTH];
   CHAR_T              suffix[MAX_LENGTH];

   if (!path)
      return (FALSE);

   ustrcpy (temppath, path);
   ExtractSuffix (temppath, suffix);

   if (!ustrcasecmp (suffix, TEXT("css")))
     return (TRUE);
   else if (!ustrcmp (suffix, TEXT("gz")))
     {
       /* take into account compressed files */
       ExtractSuffix (temppath, suffix);       
       if (!ustrcasecmp (suffix, TEXT("css")))
	 return (TRUE);
       else
	 return (FALSE);
     }
   else
     return (FALSE);
}

/*----------------------------------------------------------------------
  IsImageName                                
  returns TRUE if path points to an image resource.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsImageName (const STRING path)
#else  /* __STDC__ */
ThotBool             IsImageName (path)
const STRING        path;
#endif /* __STDC__ */
{
   CHAR_T                temppath[MAX_LENGTH];
   CHAR_T                suffix[MAX_LENGTH];
   CHAR_T                nsuffix[MAX_LENGTH];
   int                 i;

   if (!path)
      return (FALSE);

   ustrcpy (temppath, path);
   ExtractSuffix (temppath, suffix);

   /* Normalize the suffix */
   i = 0;
   while (suffix[i] != EOS && i < MAX_LENGTH -1)
     {
       nsuffix[i] = utolower (suffix[i]);
       i++;
     }
   nsuffix[i] = EOS;
   if ((!ustrcmp (nsuffix, TEXT("gif"))) || (!ustrcmp (nsuffix, TEXT("xbm"))) ||
       (!ustrcmp (nsuffix, TEXT("xpm"))) || (!ustrcmp (nsuffix, TEXT("jpg"))) ||
       (!ustrcmp (nsuffix, TEXT("png"))) || (!ustrcmp (nsuffix, TEXT("au"))))
      return (TRUE);
   return (FALSE);
}

/*----------------------------------------------------------------------
  IsImageType                                
  returns TRUE if type points to an image resource.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsImageType (const STRING type)
#else  /* __STDC__ */
ThotBool             IsImageType (type)
const STRING     type;
#endif /* __STDC__ */
{
   CHAR_T              temptype[MAX_LENGTH];
   int                 i;

   if (!type)
      return (FALSE);

   ustrcpy (temptype, type);
   /* Normalize the type */
   i = 0;
   while (temptype[i] != EOS)
     {
       temptype[i] = tolower (temptype[i]);
       i++;
     }
   if ((!ustrcmp (temptype, TEXT("gif"))) || (!ustrcmp (temptype, TEXT("x-xbitmap"))) ||
       (!ustrcmp (temptype, TEXT("x-xpixmap"))) || (!ustrcmp (temptype, TEXT("jpeg"))) ||
       (!ustrcmp (temptype, TEXT("png"))))
      return (TRUE);
   return (FALSE);
}

/*----------------------------------------------------------------------
  IsTextName                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsTextName (const STRING path)
#else  /* __STDC__ */
ThotBool             IsTextName (path)
const STRING        path;

#endif /* __STDC__ */
{
   CHAR_T                temppath[MAX_LENGTH];
   CHAR_T                suffix[MAX_LENGTH];
   CHAR_T                nsuffix[MAX_LENGTH];
   int                 i;

   if (!path)
     return (FALSE);

   ustrcpy (temppath, path);
   ExtractSuffix (temppath, suffix);

   /* Normalize the suffix */
   i = 0;
   while (suffix[i] != EOS && i < MAX_LENGTH -1)
     {
	nsuffix[i] = tolower (suffix[i]);
	i++;
     }
   nsuffix[i] = EOS;

   if ((!ustrcmp (nsuffix, TEXT("txt"))) || (!ustrcmp (nsuffix, TEXT("dtd"))))
      return (TRUE);
   else if (!ustrcmp (nsuffix, TEXT("gz")))
     {
       /* take into account compressed files */
       ExtractSuffix (temppath, suffix);       
       /* Normalize the suffix */
       i = 0;
       while (suffix[i] != EOS && i < MAX_LENGTH -1)
	 {
	   nsuffix[i] = tolower (suffix[i]);
	   i++;
	 }
       nsuffix[i] = EOS;
       if ((!ustrcmp (nsuffix, TEXT("txt"))) || (!ustrcmp (nsuffix, TEXT("dtd"))))
	 return (TRUE);
       else
	 return (FALSE);
     }
   else
     return (FALSE);
}

/*----------------------------------------------------------------------
  IsHTTPPath                                     
  returns TRUE if path is in fact an http URL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsHTTPPath (const STRING path)
#else  /* __STDC__ */
ThotBool             IsHTTPPath (path)
const STRING       path;
#endif /* __STDC__ */
{
   if (!path)
      return FALSE;

   if ((!ustrncmp (path, TEXT("http:"), 5) != 0)
       || !ustrncmp (path, TEXT("internal:"), 9))
      return TRUE;
   return FALSE;
}

/*----------------------------------------------------------------------
  IsWithParameters                           
  returns TRUE if url has a concatenated query string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsWithParameters (const char *url)
#else  /* __STDC__ */
ThotBool            IsWithParameters (url)
const char         *url;
#endif /* __STDC__ */
{
   int                 i;

   if ((!url) || (url[0] == EOS))
      return FALSE;

   i = strlen (url) - 1;
   while (i > 0 && url[i--] != '?')
      if (i < 0)
	 return FALSE;

   /* There is a parameter */
   return TRUE;
}

/*----------------------------------------------------------------------
  IsW3Path                                           
  returns TRUE if path is in fact a URL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsW3Path (const CharUnit* path)
#else  /* __STDC__ */
ThotBool             IsW3Path (path)
const CharUnit*      path;
#endif /* __STDC__ */
{
  if (StringNCompare (path, CUSTEXT("http:"), 5)   && 
      StringNCompare (path, CUSTEXT("ftp:"), 4)    &&
      StringNCompare (path, CUSTEXT("telnet:"), 7) && 
      StringNCompare (path, CUSTEXT("wais:"), 5)   &&
      StringNCompare (path, CUSTEXT("news:"), 5)   && 
      StringNCompare (path, CUSTEXT("gopher:"), 7) &&
      StringNCompare (path, CUSTEXT("mailto:"), 7) && 
      StringNCompare (path, CUSTEXT("archie:"), 7))
    return FALSE;
  return TRUE;
}

/*----------------------------------------------------------------------
  IsValidProtocol                                                    
  returns true if the url protocol is supported by Amaya.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsValidProtocol (const STRING url)
#else  /* __STDC__ */
ThotBool             IsValidProtocol (url)
const STRING       url;
#endif /* __STDC__ */
{
   if (!ustrncmp (url, TEXT("http:"), 5)
      || !ustrncmp (url, TEXT("internal:"), 9)
      || !ustrncmp (url, TEXT("ftp:"), 4))
       /* experimental */
      /***  || !strncmp (url, "ftp:", 4) ***/
     /*** || !strncmp (path, "news:", 5)***/ 
      return (TRUE);
   else
      return (FALSE);
}


/*----------------------------------------------------------------------
   GetBaseURL
   normalizes orgName according to a base associated with doc, and
   following the standard URL format rules.
   The function returns the base used to solve relative URL and SRC:
      - the base of the document,
      - or the document path (without document name).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CharUnit*           GetBaseURL (Document doc)
#else  /* __STDC__ */
CharUnit*           GetBaseURL (doc)
Document            doc;
#endif /* __STDC__ */
{
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  STRING              ptr, basename;
  int                 length;

  /* @@@ irene */
  if (!DocumentURLs[doc])
	  return NULL;
  basename = TtaAllocString (MAX_LENGTH);
  ustrncpy (basename, DocumentURLs[doc], MAX_LENGTH-1);
  basename[MAX_LENGTH-1] = EOS;
  length = MAX_LENGTH -1;
  /* get the root element    */
  el = TtaGetMainRoot (doc);
  /* search the BASE element */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = HTML_EL_HEAD;
  el = TtaSearchTypedElement (elType, SearchForward, el);
  if (el)
    {
      elType.ElTypeNum = HTML_EL_BASE;
      el = TtaSearchTypedElement (elType, SearchInTree, el);
    }
  if (el)
    {
      /*  The document has a BASE element -> Get the HREF attribute */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_HREF_;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	{
	  /* Use the base path of the document */
	  TtaGiveTextAttributeValue (attr, basename, &length);
	  /* base and orgName have to be separated by a DIR_SEP */
	  length--;
	  if (basename[0] != EOS && basename[length] != URL_SEP && basename[length] != DIR_SEP) 
	    /* verify if the base has the form "protocol://server:port" */
	    {
	      ptr = AmayaParseUrl (basename, _EMPTYSTR_, AMAYA_PARSE_ACCESS |
				                 AMAYA_PARSE_HOST |
				                 AMAYA_PARSE_PUNCTUATION);
	      if (ptr && !ustrcmp (ptr, basename))
		{
		  /* it has this form, we complete it by adding a URL_STR  */
		  if (ustrchr (basename, DIR_SEP))
		    ustrcat (basename, DIR_STR);
		  else
		    ustrcat (basename, URL_STR);
		  length++;
		}
	      if (ptr)
		TtaFreeMemory (ptr);
	    }
	}
      }
  
  /* Remove anything after the last DIR_SEP char. If no such char is found,
   * then search for the first ":" char, hoping that what's before that is a
   * protocol. If found, end the string there. If neither char is found,
   * then discard the whole base element.
   */
  length = ustrlen (basename) - 1;
  /* search for the last DIR_SEP char */
  while (length >= 0  && basename[length] != URL_SEP && basename[length] != DIR_SEP)
    length--;
  if (length >= 0)
    /* found the last DIR_SEP char, end the string there */
    basename[length + 1] = EOS;		   
  else
    /* search for the first PATH_STR char */
    {
      for (length = 0; basename[length] != TEXT(':') && 
	     basename[length] != EOS; length ++);
      if (basename[length] == TEXT(':'))
	/* found, so end the string there */
	basename[length + 1] = EOS;
      else
	/* not found, discard the base */
	basename[0] = EOS;
    }
  return (basename);
}


/*----------------------------------------------------------------------
   GetLocalPath
   Allocate and return the local document path associated to the url
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CharUnit*  GetLocalPath (Document doc, CharUnit* url)
#else  /* __STDC__ */
CharUnit*  GetLocalPath (doc, url)
Document   doc;
CharUnit*  url;
#endif /* __STDC__ */
{
  CharUnit* ptr;
  CharUnit* n;
  CharUnit* documentname;
  CharUnit  url_sep;
  int       len;
  ThotBool  noFile;

  if (url != NULL)
    {
      /* check whether the file name exists */
      len = StringLength (url) - 1;
      if (IsW3Path (url))
         url_sep = CUSTEXT('/');
      else 
          url_sep = CUS_DIR_SEP;
      noFile = (url[len] == url_sep);
      if (noFile)
         url[len] = CUS_EOS;
      ptr = TtaAllocCUString (MAX_LENGTH);
      documentname = TtaAllocCUString (MAX_LENGTH);
      TtaExtractName (url, ptr, documentname);
      cus_sprintf (ptr, CUSTEXT("%s%s%d%s"), TempFileDirectory, CUS_DIR_STR, doc, CUS_DIR_STR);
      if (!TtaCheckDirectory (ptr))
	/* directory did not exist */
	TtaMakeDirectory (ptr);

      /* don't include the query string within document name */
      n = StrRChr (documentname, CUSTEXT('?'));
      if (n != NULL)
         *n = CUS_EOS;
      /* don't include ':' within document name */
      n = StrChr (documentname, CUSTEXT(':'));
      if (n != NULL)
         *n = CUS_EOS;
      /* if after all this operations document name
	 is empty, let's use noname.html instead */
      if (documentname[0] == CUS_EOS)
         StringConcat (ptr, CUSTEXT("noname.html"));
      else
          StringConcat (ptr, documentname);
      TtaFreeMemory (documentname);
      /* restore the url */
      if (noFile)
	url[len] = url_sep;
      return (ptr);
    }
  else
    return (NULL);
}

/*----------------------------------------------------------------------
   ExtractTarget extract the target name from document nane.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ExtractTarget (CharUnit* aName, CharUnit* target)
#else
void         ExtractTarget (aName, target)
CharUnit*    aName;
CharUnit*    target;
#endif
{
   int       lg, i;
   CharUnit* ptr;
   CharUnit* oldptr;

   if (!target || !aName)
     /* bad target */
     return;

   target[0] = CUS_EOS;
   lg = StringLength (aName);
   if (lg)
     {
	/* the name is not empty */
	oldptr = ptr = &aName[0];
	do
	  {
	     ptr = StrRChr (oldptr, CUSTEXT('#'));
	     if (ptr)
		oldptr = &ptr[1];
	  }
	while (ptr);

	i = (int) (oldptr) - (int) (aName);	/* name length */
	if (i > 1)
	  {
	     aName[i - 1] = CUS_EOS;
	     if (i != lg)
		StringCopy (target, oldptr);
	  }
     }
}

/*----------------------------------------------------------------------
   ConvertFileURL
   If the URL starts with file: prefix, it removes the protocol so that we
   can use it as a local filename
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConvertFileURL (CharUnit* url)
#else
void ConvertFileURL (url)
CharUnit* url
#endif /* __STDC__ */
{
  if (!StringNCaseCompare (url, CUSTEXT("file:"), 5))
     StringCopy (url, url + 5);
}

/*----------------------------------------------------------------------
   NormalizeURL
   normalizes orgName according to a base associated with doc, and
   following the standard URL format rules.
   if doc is 0 and otherPath not NULL, normalizes orgName according to this
   other path.
   The function returns the new complete and normalized URL 
   or file name path (newName) and the name of the document (docName).        
   N.B. If the function can't find out what's the docName, it assigns
   the name "noname.html".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NormalizeURL (CharUnit* orgName, Document doc, CharUnit* newName, CharUnit* docName, CharUnit* otherPath)
#else  /* __STDC__ */
void                NormalizeURL (orgName, doc, newName, docName, otherPath)
CharUnit*           orgName;
Document            doc;
CharUnit*           newName;
CharUnit*           docName;
CharUnit*           otherPath;
#endif /* __STDC__ */
{
   CharUnit*        basename;
   CharUnit         tempOrgName[MAX_LENGTH];
   CharUnit*        ptr;
   CharUnit         used_sep;
   int              length;
   ThotBool         check;

#  ifdef _WINDOWS
   int ndx;
#  endif /* _WINDOWS */

   if (!newName || !docName)
      return;

   if (doc != 0)
     basename = GetBaseURL (doc);
   else if (otherPath != NULL)
     basename = StringDuplicate (otherPath);
   else
     basename = NULL;

   /*
    * Clean orgName
    * Make sure we have a complete orgName, without any leading or trailing
    * white spaces, or trailinbg new lines
    */
   ptr = orgName;
   /* skip leading white space and new line characters */
   while ((*ptr == CUS_SPACE || *ptr == CUS_EOL) && *ptr++ != CUS_EOS);
   StringNCopy (tempOrgName, ptr, MAX_LENGTH -1);
   tempOrgName[MAX_LENGTH -1] = CUS_EOS;
   /*
    * Make orgName a complete URL
    * If the URL does not include a protocol, then try to calculate
    * one using the doc's base element (if it exists),
    */
   if (tempOrgName[0] == CUS_EOS)
     {
       newName[0] = CUS_EOS;
       TtaFreeMemory (basename);
       return;
     }

   /* clean trailing white space */
   length = StringLength (tempOrgName) - 1;
   while (tempOrgName[length] == CUS_SPACE && tempOrgName[length] == CUS_EOL)
     {
       tempOrgName[length] = CUS_EOS;
       length--;
     }

   /* remove extra dot (which dot???) */
   /* ugly, but faster than a strcmp */
   if (tempOrgName[length] == CUSTEXT('.')
       && (length == 0 || tempOrgName[length-1] != CUSTEXT('.')))
	 tempOrgName[length] = CUS_EOS;

   if (IsW3Path (tempOrgName))
     {
       /* the name is complete, go to the Sixth Step */
       StringCopy (newName, tempOrgName);
       SimplifyUrl (&newName);
       /* verify if the URL has the form "protocol://server:port" */
       ptr = AmayaParseUrl (newName, _EMPTYSTR_, AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);
       if (ptr && !StringCompare (ptr, newName)) /* it has this form, we complete it by adding a DIR_STR  */
         StringConcat (newName, CUS_URL_STR);

       if (ptr)
         TtaFreeMemory (ptr);
     }
   else if ( basename == NULL)
     /* the name is complete, go to the Sixth Step */
     StringCopy (newName, tempOrgName);
   else
     {
       /* Calculate the absolute URL, using the base or document URL */
#      ifdef _WINDOWS
       if (!IsW3Path (basename))
	 {
	   length = ustrlen (tempOrgName);
	   for (ndx = 0; ndx < length; ndx++)
	     if (tempOrgName [ndx] == TEXT('/'))
	       tempOrgName [ndx] = TEXT('\\');
	 }
#      endif /* _WINDOWS */
       ptr = AmayaParseUrl (tempOrgName, basename, AMAYA_PARSE_ALL);
       if (ptr)
	 {
	   SimplifyUrl (&ptr);
	   StringCopy (newName, ptr);
	   TtaFreeMemory (ptr);
	 }
       else
	 newName[0] = CUS_EOS;
     }

   TtaFreeMemory (basename);
   /*
    * Prepare the docname that will refer to this ressource in the
    * .amaya directory. If the new URL finishes on DIR_SEP, then use
    * noname.html as a default ressource name
   */
   if (newName[0] != CUS_EOS)
     {
       length = StringLength (newName) - 1;
       if (newName[length] == CUS_URL_SEP || newName[length] == CUS_DIR_SEP)
	 {
	   used_sep = newName[length];
	   check = TRUE;
	   while (check)
	     {
               length--;
               while (length >= 0 && newName[length] != used_sep)
		 length--;
               if (!StringNCompare (&newName[length+1], CUSTEXT(".."), 2))
		 {
		   newName[length+1] = CUS_EOS;
		   /* remove also previous directory */
		   length--;
		   while (length >= 0 && newName[length] != used_sep)
		     length--;
		   if (StringNCompare (&newName[length+1], CUSTEXT("//"), 2))
		     /* don't remove server name */
                     newName[length+1] = CUS_EOS;
		 }
	       else if (!StringNCompare (&newName[length+1], CUSTEXT("."), 1))
		 newName[length+1] = CUS_EOS;
               else
		 check = FALSE;
	     }
	   StringCopy (docName, CUSTEXT("noname.html"));	       
	   /* docname was not comprised inside the URL, so let's */
	   /* assign the default ressource name */
	   StringCopy (docName, CUSTEXT("noname.html"));
	 }
       else
	 { /* docname is comprised inside the URL */
           while (length >= 0 && newName[length] != CUS_URL_SEP && newName[length] != CUS_DIR_SEP)
	     length--;
	   if (length < 0)
             StringCopy (docName, newName);
	   else
	     StringCopy (docName, &newName[length+1]);
	 }
     }
   else
     docName[0] = CUS_EOS;
} 

/*----------------------------------------------------------------------
  IsSameHost                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsSameHost (const STRING url1, const STRING url2)
#else  /* __STDC__ */
ThotBool             IsSameHost (url1, url2)
const STRING       url1;
const STRING       url2;
#endif /* __STDC__ */
{
   STRING           basename_ptr1, basename_ptr2;
   ThotBool          result;

   basename_ptr1 = AmayaParseUrl (url1, _EMPTYSTR_, AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);
   basename_ptr2 = AmayaParseUrl (url2, _EMPTYSTR_, AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);

   if (ustrcmp (basename_ptr1, basename_ptr2))
      result = FALSE;
   else
      result = TRUE;

   TtaFreeMemory (basename_ptr1);
   TtaFreeMemory (basename_ptr2);
   return (result);
}


/*----------------------------------------------------------------------
  HasKnownFileSuffix
  returns TRUE if path points to a file ending with a suffix.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             HasKnownFileSuffix (const STRING path)
#else  /* __STDC__ */
ThotBool             HasKnownFileSuffix (path)
const STRING    path;
#endif /* __STDC__ */
{
   STRING      root;
   CHAR_T      temppath[MAX_LENGTH];
   CHAR_T      suffix[MAX_LENGTH];

   if (!path || path[0] == EOS || path[ustrlen(path)] == DIR_SEP)
     return (FALSE);

   root = AmayaParseUrl(path, _EMPTYSTR_, AMAYA_PARSE_PATH | AMAYA_PARSE_PUNCTUATION);

   if (root) 
     {
       ustrcpy (temppath, root);
       TtaFreeMemory (root);
       /* Get the suffix */
       ExtractSuffix (temppath, suffix); 

       if( suffix[0] == EOS)
	 /* no suffix */
	 return (FALSE);

       /* Normalize the suffix */
       ConvertToLowerCase (suffix);

       if (!ustrcmp (suffix, TEXT("gz")))
	 /* skip the compressed suffix */
	 {
	 ExtractSuffix (temppath, suffix);
	 if(suffix[0] == EOS)
	   /* no suffix */
	   return (FALSE);
         /* Normalize the suffix */
         ConvertToLowerCase (suffix);
	 }

       if (ustrcmp (suffix, TEXT("gif")) &&
	   ustrcmp (suffix, TEXT("xbm")) &&
	   ustrcmp (suffix, TEXT("xpm")) &&
	   ustrcmp (suffix, TEXT("jpg")) &&
	   ustrcmp (suffix, TEXT("pdf")) &&
	   ustrcmp (suffix, TEXT("png")) &&
	   ustrcmp (suffix, TEXT("tgz")) &&
	   ustrcmp (suffix, TEXT("xpg")) &&
	   ustrcmp (suffix, TEXT("xpd")) &&
	   ustrcmp (suffix, TEXT("ps")) &&
	   ustrcmp (suffix, TEXT("au")) &&
	   ustrcmp (suffix, TEXT("html")) &&
	   ustrcmp (suffix, TEXT("htm")) &&
	   ustrcmp (suffix, TEXT("shtml")) &&
	   ustrcmp (suffix, TEXT("xht")) &&
	   ustrcmp (suffix, TEXT("xhtm")) &&
	   ustrcmp (suffix, TEXT("xhtml")) &&
	   ustrcmp (suffix, TEXT("txt")) &&
	   ustrcmp (suffix, TEXT("css")) &&
	   ustrcmp (suffix, TEXT("eps")))
	 return (FALSE);
       else
	 return (TRUE);
     }
   else
     return (FALSE);
}


/*----------------------------------------------------------------------
  ChopURL
  Gives back a URL no longer than MAX_PRINT_URL_LENGTH chars (outputURL). 
  If inputURL is  bigger than that size, outputURL receives
  MAX_PRINT_URL_LENGTH / 2 chars from the beginning of inputURL, "...", 
  and MAX_PRINT_URL_LENGTH / 2 chars from the end of inputURL.
  If inputURL is not longer than MAX_PRINT_URL_LENGTH chars, it gets
  copied into outputURL. 
  N.B.: outputURL must point to a memory block of MAX_PRINT_URL_LENGTH
  chars.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ChopURL (char *outputURL, const char *inputURL)
#else
void ChopURL (outputURL, inputURL)
char       *outputURL;
const char *inputURL;
#endif

{
  int len;

  len = strlen (inputURL);
  if (len <= MAX_PRINT_URL_LENGTH) 
    strcpy (outputURL, inputURL);
  else
    /* make a truncated urlName on the status window */
    {
      strncpy (outputURL, inputURL, MAX_PRINT_URL_LENGTH / 2);
      outputURL [MAX_PRINT_URL_LENGTH / 2] = EOS;
      strcat (outputURL, "...");
      strcat (outputURL, &(inputURL[len - MAX_PRINT_URL_LENGTH / 2 ]));
    }
}


/*----------------------------------------------------------------------
   scan
  	Scan a filename for its constituents
  	-----------------------------------
  
   On entry,
  	name	points to a document name which may be incomplete.
   On exit,
        absolute or relative may be nonzero (but not both).
  	host, fragment and access may be nonzero if they were specified.
  	Any which are nonzero point to zero terminated strings.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void scan (STRING name, HTURI * parts)
#else  /* __STDC__ */
static void scan (name, parts)
STRING      name;
HTURI       *parts;

#endif /* __STDC__ */
{
  STRING    p;
  STRING    after_access = name;

  memset (parts, '\0', sizeof (HTURI));
  /* Look for fragment identifier */
  if ((p = ustrchr(name, TEXT('#'))) != NULL)
    {
      *p++ = TEXT('\0');
      parts->fragment = p;
    }
    
  for (p=name; *p; p++)
    {
      if (*p == URL_SEP || *p == DIR_SEP || *p == TEXT('#') || *p == TEXT('?'))
	break;
      if (*p == TEXT(':'))
	{
	  *p = 0;
	  parts->access = after_access; /* Scheme has been specified */

	  /* The combination of gcc, the "-O" flag and the HP platform is
	     unhealthy. The following three lines is a quick & dirty fix, but is
	     not recommended. Rather, turn off "-O". */

	  /*		after_access = p;*/
	  /*		while (*after_access == 0)*/
	  /*		    after_access++;*/
	  after_access = p+1;
	  if (!ustrcasecmp(TEXT("URL"), parts->access))
	    /* Ignore IETF's URL: pre-prefix */
	    parts->access = NULL;
	  else
	    break;
	}
    }
    
    p = after_access;
    if (*p == URL_SEP || *p == DIR_SEP)
      {
	if (p[1] == URL_SEP)
	  {
	    parts->host = p+2;		/* host has been specified 	*/
	    *p = 0;			/* Terminate access 		*/
	    /* look for end of host name if any */
	    p = ustrchr (parts->host, URL_SEP);
	    if (p)
	      {
	        *p = EOS;			/* Terminate host */
	        parts->absolute = p+1;		/* Root has been found */
	      }
	  }
	else
	  /* Root found but no host */
	  parts->absolute = p+1;
      }
    else
      {
        parts->relative = (*after_access) ? after_access : 0; /* zero for "" */
      }
}


/*----------------------------------------------------------------------
  AmayaParseUrl: parse a Name relative to another name

  This returns those parts of a name which are given (and requested)
  substituting bits from the related name where necessary.
  
  On entry,
  	aName		A filename given
        relatedName     A name relative to which aName is to be parsed. Give
                        it an empty string if aName is absolute.
        wanted          A mask for the bits which are wanted.
  
  On exit,
  	returns		A pointer to a malloc'd string which MUST BE FREED
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING        AmayaParseUrl (const STRING aName, STRING relatedName, int wanted)
#else  /* __STDC__ */
STRING        AmayaParseUrl (aName, relatedName, wanted)
const STRING  aName;
STRING        relatedName;
int            wanted;

#endif /* __STDC__ */
{
  STRING     return_value;
  CHAR_T     result[MAX_LENGTH];
  CHAR_T     name[MAX_LENGTH];
  CHAR_T     rel[MAX_LENGTH];
  STRING     p, access;
  HTURI      given, related;
  int        len;
  CHAR_T     used_sep;
  STRING     used_str;

  if (ustrchr (aName, DIR_SEP) || ustrchr (relatedName, DIR_SEP))
    {
      used_str = DIR_STR;
      used_sep = DIR_SEP;
    }
  else
    {
      used_str = URL_STR;
      used_sep = URL_SEP;
    }

  /* Make working copies of input strings to cut up: */
  return_value = NULL;
  result[0] = 0;		/* Clear string  */
  ustrcpy (name, aName);
  if (relatedName != NULL)  
    ustrcpy (rel, relatedName);
  else
    relatedName[0] = EOS;
  
  scan (name, &given);
  scan (rel,  &related); 
  access = given.access ? given.access : related.access;
  if (wanted & AMAYA_PARSE_ACCESS)
    if (access)
      {
	ustrcat (result, access);
	if(wanted & AMAYA_PARSE_PUNCTUATION)
		ustrcat (result, TEXT(":"));
      }
  
  if (given.access && related.access)
    /* If different, inherit nothing. */
    if (ustrcmp (given.access, related.access) != 0)
      {
	related.host = 0;
	related.absolute = 0;
	related.relative = 0;
	related.fragment = 0;
      }
  
  if (wanted & AMAYA_PARSE_HOST)
    if(given.host || related.host)
      {
	if(wanted & AMAYA_PARSE_PUNCTUATION)
	  ustrcat (result, TEXT("//"));
	ustrcat (result, given.host ? given.host : related.host);
      }
  
  if (given.host && related.host)
    /* If different hosts, inherit no path. */
    if (ustrcmp (given.host, related.host) != 0)
      {
	related.absolute = 0;
	related.relative = 0;
	related.fragment = 0;
      }
  
  if (wanted & AMAYA_PARSE_PATH)
    {
      if (given.absolute)
	{
	  /* All is given */
	  if (wanted & AMAYA_PARSE_PUNCTUATION)
	    ustrcat (result, used_str);
	  ustrcat (result, given.absolute);
	}
      else if (related.absolute)
	{
	  /* Adopt path not name */
	  ustrcat (result, used_str);
	  ustrcat (result, related.absolute);
	  if (given.relative)
	    {
	      /* Search part? */
	      p = ustrchr (result, TEXT('?'));
	      if (!p)
		p=result+ustrlen(result)-1;
	      for (; *p!=used_sep; p--);	/* last / */
	      /* Remove filename */
	      p[1]=0;
	      /* Add given one */
	      ustrcat (result, given.relative);
	    }
	}
      else if (given.relative)
	/* what we've got */
	ustrcat (result, given.relative);
      else if (related.relative)
	ustrcat (result, related.relative);
      else
	/* No inheritance */
	ustrcat (result, used_str);
    }
  
  if (wanted & AMAYA_PARSE_ANCHOR)
    if (given.fragment || related.fragment)
      {
	if (given.absolute && given.fragment)
	  {
	    /*Fixes for relURLs...*/
	    if (wanted & AMAYA_PARSE_PUNCTUATION)
	      ustrcat (result, TEXT("#"));
	    ustrcat (result, given.fragment); 
	  }
	else if (!(given.absolute) && !(given.fragment))
	  ustrcat (result, _EMPTYSTR_);
	else
	  {
	    if (wanted & AMAYA_PARSE_PUNCTUATION)
	      ustrcat (result, TEXT("#"));
	    ustrcat (result, given.fragment ? given.fragment : related.fragment); 
	  }
      }
  len = ustrlen (result);
  if ((return_value = TtaAllocString (len + 1)) != NULL)
    ustrcpy (return_value, result);
  return (return_value);		/* exactly the right length */
}

/*----------------------------------------------------------------------
     HTCanon
  	Canonicalizes the URL in the following manner starting from the host
  	pointer:
  
  	1) The host name is converted to lowercase
  	2) Chop off port if `:80' (http), `:70' (gopher), or `:21' (ftp)
  
  	Return: OK	The position of the current path part of the URL
  			which might be the old one or a new one.
  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CharUnit* HTCanon (CharUnit** filename, CharUnit* host)
#else  /* __STDC__ */
static STRING HTCanon (filename, host)
CharUnit** filename;
CharUnit*  host;
#endif /* __STDC__ */
{
    CharUnit* newname = NULL;
    CharUnit  used_sep;
    CharUnit* path;
    CharUnit* strptr;
    CharUnit* port;
    CharUnit* access = host-3;

  
     if (*filename && StrChr (*filename, CUS_URL_SEP))
       {
	 used_sep = CUS_URL_SEP;
       }
     else
       {
	 used_sep = CUS_DIR_SEP;
       }
  
    while (access > *filename && *(access - 1) != used_sep)       /* Find access method */
	access--;
    if ((path = StrChr (host, used_sep)) == NULL)			/* Find path */
       path = host + StringLength (host);
    if ((strptr = StrChr (host, CUSTEXT('@'))) != NULL && strptr < path)	   /* UserId */
       host = strptr;
    if ((port = StrChr (host, CUSTEXT(':'))) != NULL && port > path)      /* Port number */
       port = NULL;

    strptr = host;				    /* Convert to lower-case */
    while (strptr < path)
      {
         *strptr = ToLower (*strptr);
         strptr++;
      }
    
    /* Does the URL contain a full domain name? This also works for a
       numerical host name. The domain name is already made lower-case
       and without a trailing dot. */
    {
      CharUnit* dot = port ? port : path;
      if (dot > *filename && *--dot == CUSTEXT('.'))
	{
	  CharUnit* orig = dot;
      CharUnit* dest = dot + 1;
	  while ((*orig++ = *dest++));
            if (port) port--;
	  path--;
	}
    }
    /* Chop off port if `:', `:80' (http), `:70' (gopher), or `:21' (ftp) */
    if (port)
      {
	if (!*(port+1) || *(port+1) == used_sep)
	  {
	    if (!newname)
	      {
		CharUnit* orig = port; 
        CharUnit* dest = port + 1;
		while ((*orig++ = *dest++));
	      }
	  }
	else if ((!StringNCompare (access, CUSTEXT("http"), 4)   &&
             (*(port + 1) == CUSTEXT('8')                    && 
             *(port+2) == CUSTEXT('0')                       && 
             (*(port+3) == used_sep || !*(port + 3))))       ||
             (!StringNCompare (access, CUSTEXT("gopher"), 6) &&
             (*(port+1) == CUSTEXT('7')                      && 
             *(port+2) == CUSTEXT('0')                       && 
             (*(port+3) == used_sep || !*(port+3))))         ||
             (!StringNCompare (access, CUSTEXT("ftp"), 3)    &&
             (*(port+1) == CUSTEXT('2')                      && 
             *(port + 2) == CUSTEXT('1')                     && 
             (*(port+3) == used_sep || !*(port+3))))) {
	  if (!newname)
	    {
	      CharUnit* orig = port; 
          CharUnit* dest = port + 3;
	      while((*orig++ = *dest++));
	      /* Update path position, Henry Minsky */
	      path -= 3;
	    }
	}
	else if (newname)
	  StringNConcat (newname, port, (int) (path - port));
      }

    if (newname)
      {
	CharUnit* newpath = newname + StringLength (newname);
	StringConcat (newname, path);
	path = newpath;
	/* Free old copy */
	TtaFreeMemory(*filename);
	*filename = newname;
      }
    return path;
}


/*----------------------------------------------------------------------
  SimplifyUrl: simplify a URI
  A URI is allowed to contain the sequence xxx/../ which may be
  replaced by "" , and the sequence "/./" which may be replaced by DIR_STR.
  Simplification helps us recognize duplicate URIs. 
  
  Thus, 	/etc/junk/../fred 	becomes	/etc/fred
                /etc/junk/./fred	becomes	/etc/junk/fred
  
  but we should NOT change
                http://fred.xxx.edu/../..
  
  	or	../../albert.html
  
  In order to avoid empty URLs the following URLs become:
  
  		/fred/..		becomes /fred/..
  		/fred/././..		becomes /fred/..
  		/fred/.././junk/.././	becomes /fred/..
  
  If more than one set of `://' is found (several proxies in cascade) then
  only the part after the last `://' is simplified.
  
  Returns: A string which might be the old one or a new one.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         SimplifyUrl (CharUnit** url)
#else  /* __STDC__ */
void         SimplifyUrl (url)
CharUnit**   url;
#endif /* __STDC__ */
{
  CharUnit* path;
  CharUnit* access;
  CharUnit* newptr; 
  STRING    p;
  STRING    orig, dest, end;

  CharUnit used_sep;
  ThotBool ddot_simplify; /* used to desactivate the double dot simplifcation:
			     something/../ simplification in relative URLs when they start with a ../ */


  if (!url || !*url)
    return;

  if (StrChr (*url, CUS_URL_SEP))
    {
      used_sep = CUS_URL_SEP;
    }
  else
    {
      used_sep = CUS_DIR_SEP;
    }

  /* should we simplify double dot? */
  path = *url;
  if (*path == CUSTEXT('.') && *(path + 1) == CUSTEXT('.'))
    ddot_simplify = FALSE;
  else
    ddot_simplify = TRUE;

  /* Find any scheme name */
  if ((path = StringSubstring(*url, CUSTEXT("://"))) != NULL)
    {
      /* Find host name */
      access = *url;
      while (access < path && (*access = ToLower (*access)))
            access++;
      path += 3;
      while ((newptr = StringSubstring (path, CUSTEXT ("://"))) != NULL)
            /* For proxies */
            path = newptr+3;
     /* We have a host name */
      path = HTCanon(url, path);
    }
  else if ((path = ustrstr(*url, TEXT(":/"))) != NULL)
    path += 2;
  else
    path = *url;

  if (*path == used_sep && *(path+1)==used_sep)
    /* Some URLs start //<foo> */
    path += 1;
  else if (!ustrncmp(path, TEXT("news:"), 5))
    {
      newptr = ustrchr(path+5, TEXT('@'));
      if (!newptr)
	newptr = path + 5;
      while (*newptr)
	{
	  /* Make group or host lower case */
	  *newptr = tolower (*newptr);
	  newptr++;
	}
      /* Doesn't need to do any more */
      return;
    }

  if ((p = path))
    {
      if (!((end = ustrchr (path, TEXT(';'))) || (end = ustrchr (path, TEXT('?'))) ||
	    (end = ustrchr (path, TEXT('#')))))
	end = path + ustrlen (path);
      
      /* Parse string second time to simplify */
      p = path;
      while (p < end)
	{
	  /* if we're pointing to a char, it's safe to reactivate the ../ convertion */
	  if (!ddot_simplify && *p != TEXT('.') && *p != used_sep)
	    ddot_simplify = TRUE;

	  if (*p==used_sep)
	    {
	      if (p > *url && *(p+1) == TEXT('.') && (*(p+2) == used_sep || !*(p+2)))
		{
		  orig = p + 1;
		  dest = (*(p+2)!=used_sep) ? p+2 : p+3;
		  while ((*orig++ = *dest++)); /* Remove a used_sep and a dot*/
		  end = orig - 1;
		}
	      else if (ddot_simplify && *(p+1) == TEXT('.') && *(p+2) == TEXT('.') 
		       && (*(p+3) == used_sep || !*(p+3)))
		{
		  newptr = p;
		  while (newptr>path && *--newptr!=used_sep); /* prev used_sep */
		  if (*newptr == used_sep)
		    orig = newptr + 1;
		  else
		    orig = newptr;

		  dest = (*(p+3) != used_sep) ? p+3 : p+4;
		  while ((*orig++ = *dest++)); /* Remove /xxx/.. */
		  end = orig-1;
		  /* Start again with prev slash */
		  p = newptr;
		}
	      else if (*(p+1) == used_sep)
		{
		  while (*(p+1) == used_sep)
		    {
		      orig = p;
		      dest = p + 1;
		      while ((*orig++ = *dest++));  /* Remove multiple /'s */
		      end = orig-1;
		    }
		}
	      else
		p++;
	    }
	  else
	    p++;
	}
    }

    /*
    **  Check for host/../.. kind of things
    */
    if (*path == used_sep && *(path+1) == TEXT('.') && *(path+2) == TEXT('.') 
	&& (!*(path+3) || *(path+3) == used_sep))
	*(path+1) = EOS;

  return;
}


/*----------------------------------------------------------------------
   NormalizeFile normalizes  local names.                             
   Return TRUE if target and src differ.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             NormalizeFile (CharUnit* src, CharUnit* target)
#else
ThotBool             NormalizeFile (src, target)
CharUnit*            src;
CharUnit*            target;

#endif
{
   CharUnit*         s;
   ThotBool          change;

   change = FALSE;
   if (StringNCompare (src, CUSTEXT("file:"), 5) == 0)
     {
	/* remove the prefix file: */
	if (src[5] == CUS_EOS)
	   StringCopy (target, CUS_DIR_STR);
	else if (src[0] == CUSTEXT('~'))
	  {
	    /* replace ~ */
	    s = TtaGetEnvString ("HOME");
	    StringCopy (target, s);
	    StringConcat (target, &src[5]);
	  }
	else
	   StringCopy (target, &src[5]);
	change = TRUE;
     }
#  ifndef _WINDOWS
   else if (src[0] == CUSTEXT('~'))
     {
	/* replace ~ */
	s = TtaGetEnvString ("HOME");
	StringCopy (target, s);
	if (src[1] != CUS_DIR_SEP)
	  strcat (target, CUS_DIR_STR);
	StringConcat (target, &src[1]);
	change = TRUE;
     }
#   endif /* _WINDOWS */
   else
      StringCopy (target, src);

   /* remove /../ and /./ */
   SimplifyUrl (&target);
   if (!change)
     change = StringCompare (src, target);
   return (change);
}


/*----------------------------------------------------------------------
  MakeRelativeURL: make relative name
  
  This function creates and returns a string which gives an expression of
  one address as related to another. Where there is no relation, an absolute
  address is retured.
  
  On entry,
  	Both names must be absolute, fully qualified names of nodes
  	(no fragment bits)
  
  On exit,
  	The return result points to a newly allocated name which, if
  	parsed by AmayaParseUrl relative to relatedName, will yield aName.
  	The caller is responsible for freeing the resulting name later.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING           MakeRelativeURL (STRING aName, STRING relatedName)
#else  /* __STDC__ */
STRING           MakeRelativeURL (aName, relatedName)
STRING           aName;
STRING           relatedName;
#endif  /* __STDC__ */
{
  STRING         return_value;
  CHAR_T         result[MAX_LENGTH];
  STRING         p;
  STRING         q;
  STRING         after_access;
  STRING         last_slash = NULL;
  int            slashes, levels, len;

# ifdef _WINDOWS
  int ndx;
# endif /* _WINDOWS */

  if (aName == NULL || relatedName == NULL)
    return (NULL);

  slashes = 0;
  after_access = NULL;
  p = aName;
  q = relatedName;
  for (; *p && (*p == *q); p++, q++)
    {
      /* Find extent of match */
      if (*p == TEXT(':'))
	after_access = p + 1;
      if (*p == DIR_SEP)
	{
	  /* memorize the last slash position and count them */
	  last_slash = p;
	  slashes++;
	}
    }
    
  /* q, p point to the first non-matching character or zero */
  if (*q == EOS)
    {
      /* New name is a subset of the related name */
      /* exactly the right length */
      len = ustrlen (p);
      if ((return_value = TtaAllocString (len + 1)) != NULL)
	ustrcpy (return_value, p);
    }
  else if ((slashes < 2 && after_access == NULL)
      || (slashes < 3 && after_access != NULL))
    {
      /* Two names whitout common path */
      /* exactly the right length */
      len = ustrlen (aName);
      if ((return_value = TtaAllocString (len + 1)) != NULL)
	ustrcpy (return_value, aName);
    }
  else
    {
      /* Some path in common */
      if (slashes == 3 && ustrncmp (aName, TEXT("http:"), 5) == 0)
	/* just the same server */
	ustrcpy (result, last_slash);
      else
	{
	  levels= 0; 
	  for (; *q && *q != TEXT('#') && *q != TEXT(';') && *q != TEXT('?'); q++)
	    if (*q == DIR_SEP)
	      levels++;
	  
	  result[0] = EOS;
	  for (;levels; levels--)
	    ustrcat (result, TEXT("../"));
	  ustrcat (result, last_slash+1);
	} 

      if (!*result)
	ustrcat (result, TEXT("./"));

      /* exactly the right length */
      len = ustrlen (result);
      if ((return_value = TtaAllocString (len + 1)) != NULL)
	ustrcpy (return_value, result);

    }
# ifdef _WINDOWS
  len = ustrlen (return_value);
  for (ndx = 0; ndx < len; ndx ++)
	  if (return_value[ndx] == TEXT('\\'))
	     return_value[ndx] = TEXT('/') ;
# endif /* _WINDOWS */
  return (return_value);
}


