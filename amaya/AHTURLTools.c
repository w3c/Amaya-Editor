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
    char * access;		/* Now known as "scheme" */
    char * host;
    char * absolute;
    char * relative;
    char * fragment;
} HTURI;


/*----------------------------------------------------------------------
  ConvertToLowerCase
  Converts a string to lowercase.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ConvertToLowerCase (char *string)
#else  /* __STDC__ */
void         ConvertToLowerCase (string)
char                *string;

#endif /* __STDC__ */
{
 int i;

 if (!string)
   return;

 for (i = 0; string[i] != EOS; i++)
   string[i] = tolower (string[i]);
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
   while ((*curr != 0) && (*curr != SPACE) && (*curr != '\b') &&
	  (*curr != '\r') && (*curr != EOL))
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

   if (*curr == ':')
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
  IsHTMLName                                                         
  returns TRUE if path points to an HTML resource.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsHTMLName (const char *path)
#else  /* __STDC__ */
boolean             IsHTMLName (path)
const char         *path;
#endif /* __STDC__ */
{
   char                temppath[MAX_LENGTH];
   char                suffix[MAX_LENGTH];
   char                nsuffix[MAX_LENGTH];
   int                 i;

   if (!path)
      return (FALSE);

   strcpy (temppath, path);
   ExtractSuffix (temppath, suffix);

   /* Normalize the suffix */
   i = 0;
   while (suffix[i] != EOS && i < MAX_LENGTH -1)
     {
       nsuffix[i] = tolower (suffix[i]);
       i++;
     }
   nsuffix[i] = EOS;
   if ((!strcmp (nsuffix, "html")) ||
       (!strcmp (nsuffix, "htm")) ||
       (!strcmp (nsuffix, "shtml")))
     return (TRUE);
   else if (!strcmp (nsuffix, "gz"))
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
       if ((!strcmp (nsuffix, "html")) ||
	   (!strcmp (nsuffix, "htm")) ||
	   (!strcmp (nsuffix, "shtml")))
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
boolean             IsImageName (const char *path)
#else  /* __STDC__ */
boolean             IsImageName (path)
const char         *path;
#endif /* __STDC__ */
{
   char                temppath[MAX_LENGTH];
   char                suffix[MAX_LENGTH];
   char                nsuffix[MAX_LENGTH];
   int                 i;

   if (!path)
      return (FALSE);

   strcpy (temppath, path);
   ExtractSuffix (temppath, suffix);

   /* Normalize the suffix */
   i = 0;
   while (suffix[i] != EOS && i < MAX_LENGTH -1)
     {
       nsuffix[i] = tolower (suffix[i]);
       i++;
     }
   nsuffix[i] = EOS;
   if ((!strcmp (nsuffix, "gif")) || (!strcmp (nsuffix, "xbm")) ||
       (!strcmp (nsuffix, "xpm")) || (!strcmp (nsuffix, "jpg")) ||
       (!strcmp (nsuffix, "png")) || (!strcmp (nsuffix, "au")))
      return (TRUE);
   return (FALSE);
}

/*----------------------------------------------------------------------
  IsTextName                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsTextName (const char *path)
#else  /* __STDC__ */
boolean             IsTextName (path)
const char         *path;

#endif /* __STDC__ */
{
   char                temppath[MAX_LENGTH];
   char                suffix[MAX_LENGTH];
   char                nsuffix[MAX_LENGTH];
   int                 i;

   if (!path)
     return (FALSE);

   strcpy (temppath, path);
   ExtractSuffix (temppath, suffix);

   /* Normalize the suffix */
   i = 0;
   while (suffix[i] != EOS && i < MAX_LENGTH -1)
     {
	nsuffix[i] = tolower (suffix[i]);
	i++;
     }
   nsuffix[i] = EOS;

   if ((!strcmp (nsuffix, "txt")) || (!strcmp (nsuffix, "dtd")))
      return (TRUE);
   else if (!strcmp (nsuffix, "gz"))
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
       if ((!strcmp (nsuffix, "txt")) || (!strcmp (nsuffix, "dtd")))
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
boolean             IsHTTPPath (const char *path)
#else  /* __STDC__ */
boolean             IsHTTPPath (path)
const char         *path;
#endif /* __STDC__ */
{
   if (!path)
      return FALSE;

   if (strncmp (path, "http:", 5) != 0)
      return FALSE;
   return TRUE;
}

/*----------------------------------------------------------------------
  IsWithParameters                           
  returns TRUE if url has a concatenated query string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsWithParameters (const char *url)
#else  /* __STDC__ */
boolean             IsWithParameters (url)
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
boolean             IsW3Path (const char *path)
#else  /* __STDC__ */
boolean             IsW3Path (path)
const char               *path;
#endif /* __STDC__ */
{
   if ((strncmp (path, "http:", 5)) && (strncmp (path, "ftp:", 4)) &&
       (strncmp (path, "telnet:", 7)) && (strncmp (path, "wais:", 5)) &&
       (strncmp (path, "news:", 5)) && (strncmp (path, "gopher:", 7)) &&
       (strncmp (path, "mailto:", 7)) && (strncmp (path, "archie:", 7)))
      return FALSE;
   return TRUE;
}

/*----------------------------------------------------------------------
  IsValidProtocol                                                    
  returns true if the url protocol is supported by Amaya.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsValidProtocol (const char *url)
#else  /* __STDC__ */
boolean             IsValidProtocol (url)
const char         *url;
#endif /* __STDC__ */
{
   if (!strncmp (url, "http:", 5))
       /* experimental */
      /***  || !strncmp (url, "ftp:", 4)) ***/
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
char               *GetBaseURL (Document doc)
#else  /* __STDC__ */
char               *GetBaseURL (doc)
Document            doc;
#endif /* __STDC__ */
{
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char               *ptr, *basename;
  int                 length;

  basename = TtaGetMemory (MAX_LENGTH);
  strncpy (basename, DocumentURLs[doc], MAX_LENGTH-1);
  basename[MAX_LENGTH-1] = EOS;
  length = MAX_LENGTH -1;
  /* get the root element    */
  el = TtaGetMainRoot (doc);
  /* search the BASE element */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = HTML_EL_BASE;
  el = TtaSearchTypedElement (elType, SearchInTree, el);
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
	      ptr = AmayaParseUrl (basename, "", AMAYA_PARSE_ACCESS |
				                 AMAYA_PARSE_HOST |
				                 AMAYA_PARSE_PUNCTUATION);
	      if (ptr && !strcmp (ptr, basename))
		{
		  /* it has this form, we complete it by adding a URL_STR  */
		  if (strchr (basename, DIR_SEP))
		    strcat (basename, DIR_STR);
		  else
		    strcat (basename, URL_STR);
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
  length = strlen (basename) - 1;
  /* search for the last DIR_SEP char */
  while (length >= 0  && basename[length] != URL_SEP && basename[length] != DIR_SEP)
    length--;
  if (length >= 0)
    /* found the last DIR_SEP char, end the string there */
    basename[length + 1] = EOS;		   
  else
    /* search for the first PATH_STR char */
    {
      for (length = 0; basename[length] != ':' && 
	     basename[length] != EOS; length ++);
      if (basename[length] == ':')
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
char      *GetLocalPath (Document doc, char *url)
#else  /* __STDC__ */
char      *GetLocalPath (doc, url)
Document   doc;
char      *url;
#endif /* __STDC__ */
{
  char    *ptr, *n;
  char    *documentname;
  char     url_sep;
  int      len;
  boolean  noFile;

  if (url != NULL)
    {
      /* check whether the file name exists */
      len = strlen (url) - 1;
      if (IsW3Path)
	url_sep = '/';
      else 
	url_sep = DIR_SEP;
      noFile = (url[len] == url_sep);
      if (noFile)
	url[len] = EOS;
      ptr = TtaGetMemory (MAX_LENGTH);
      documentname = TtaGetMemory (MAX_LENGTH);
      TtaExtractName (url, ptr, documentname);
      sprintf (ptr, "%s%s%d%s", TempFileDirectory, DIR_STR, doc, DIR_STR);
      if (!TtaCheckDirectory (ptr))
	/* directory did not exist */
	mkdir (ptr, S_IRWXU);

      /* don't include the query string within document name */
      n = strrchr(documentname, '?');
      if (n != NULL)
	*n = EOS;
      /* don't include ':' within document name */
      n = strchr (documentname, ':');
      if (n != NULL)
	*n = EOS;
      strcat (ptr, documentname);
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
void                NormalizeURL (char *orgName, Document doc, char *newName, char *docName, char *otherPath)
#else  /* __STDC__ */
void                NormalizeURL (orgName, doc, newName, docName, otherPath)
char               *orgName;
Document            doc;
char               *newName;
char               *docName;
char               *otherPath;
#endif /* __STDC__ */
{
   char               *basename;
   char                tempOrgName[MAX_LENGTH];
   char               *ptr;
   char                used_sep;
   int                 length;
   boolean             check;

#  ifdef _WINDOWS
   int ndx;
#  endif /* _WINDOWS */

   if (!newName || !docName)
      return;

   if (doc != 0)
     basename = GetBaseURL (doc);
   else if (otherPath != NULL)
     basename = TtaStrdup (otherPath);
   else
     basename = NULL;

   /*
    * Clean orgName
    * Make sure we have a complete orgName, without any leading or trailing
    * white spaces, or trailinbg new lines
    */
   ptr = orgName;
   /* skip leading white space and new line characters */
   while ((*ptr == ' ' || *ptr == EOL) && *ptr++ != EOS);
   strncpy (tempOrgName, ptr, MAX_LENGTH -1);
   tempOrgName[MAX_LENGTH -1] = EOS;
   /*
    * Make orgName a complete URL
    * If the URL does not include a protocol, then try to calculate
    * one using the doc's base element (if it exists),
    */
   if (tempOrgName[0] == EOS)
     {
       newName[0] = EOS;
       TtaFreeMemory (basename);
       return;
     }

   /* clean trailing white space */
   length = strlen (tempOrgName) - 1;
   while (tempOrgName[length] == SPACE && tempOrgName[length] == EOL)
     {
       tempOrgName[length] = EOS;
       length--;
     }

   /* remove extra dot */
   if (tempOrgName[length] == '.')
      tempOrgName[length] = EOS;

   if (IsW3Path (tempOrgName))
     {
       /* the name is complete, go to the Sixth Step */
       strcpy (newName, tempOrgName);
       SimplifyUrl (&newName);
       /* verify if the URL has the form "protocol://server:port" */
       ptr = AmayaParseUrl (newName, "", AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);
       if (ptr && !strcmp (ptr, newName)) /* it has this form, we complete it by adding a DIR_STR  */
         strcat (newName, URL_STR);

       if (ptr)
         TtaFreeMemory (ptr);
     }
   else if ( basename == NULL)
     /* the name is complete, go to the Sixth Step */
     strcpy (newName, tempOrgName);
   else
     {
       /* Calculate the absolute URL, using the base or document URL */
#      ifdef _WINDOWS
       if (!IsW3Path (basename))
	 {
	   length = strlen (tempOrgName);
	   for (ndx = 0; ndx < length; ndx++)
	     if (tempOrgName [ndx] == '/')
	       tempOrgName [ndx] = '\\';
	 }
#      endif /* _WINDOWS */
       ptr = AmayaParseUrl (tempOrgName, basename, AMAYA_PARSE_ALL);
       if (ptr)
	 {
	   SimplifyUrl (&ptr);
	   strcpy (newName, ptr);
	   TtaFreeMemory (ptr);
	 }
       else
	 newName[0] = EOS;
     }

   TtaFreeMemory (basename);
   /*
    * Prepare the docname that will refer to this ressource in the
    * .amaya directory. If the new URL finishes on DIR_SEP, then use
    * noname.html as a default ressource name
   */
   if (newName[0] != EOS)
     {
       length = strlen (newName) - 1;
       if (newName[length] == URL_SEP || newName[length] == DIR_SEP)
	 {
	   used_sep = newName[length];
	   check = TRUE;
	   while (check)
	     {
               length--;
               while (length >= 0 && newName[length] != used_sep)
		 length--;
               if (!strncmp (&newName[length+1], "..", 2))
		 {
		   newName[length+1] = EOS;
		   /* remove also previous directory */
		   length--;
		   while (length >= 0 && newName[length] != used_sep)
		     length--;
		   if (strncmp (&newName[length+1], "//", 2))
		     /* don't remove server name */
                     newName[length+1] = EOS;
		 }
	       else if (!strncmp (&newName[length+1], ".", 1))
		 newName[length+1] = EOS;
               else
		 check = FALSE;
	     }
	   strcpy (docName, "noname.html");	       
	   /* docname was not comprised inside the URL, so let's */
	   /* assign the default ressource name */
	   strcpy (docName, "noname.html");
	 }
       else
	 { /* docname is comprised inside the URL */
           while (length >= 0 && newName[length] != URL_SEP && newName[length] != DIR_SEP)
	     length--;
	   if (length < 0)
             strcpy (docName, newName);
	   else
	     strcpy (docName, &newName[length+1]);
	 }
     }
   else
     docName[0] = EOS;
} 

/*----------------------------------------------------------------------
  IsSameHost                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsSameHost (const char *url1, const char *url2)
#else  /* __STDC__ */
boolean             IsSameHost (url1, url2)
const char         *url1;
const char         *url2;
#endif /* __STDC__ */
{
   char            *basename_ptr1, *basename_ptr2;
   boolean          result;

   basename_ptr1 = AmayaParseUrl (url1, "", AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);
   basename_ptr2 = AmayaParseUrl (url2, "", AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);

   if (strcmp (basename_ptr1, basename_ptr2))
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
boolean             HasKnownFileSuffix (const char *path)
#else  /* __STDC__ */
boolean             HasKnownFileSuffix (path)
const char         *path;
#endif /* __STDC__ */
{
   char            *root;
   char             temppath[MAX_LENGTH];
   char             suffix[MAX_LENGTH];

   if (!path || path[0] == EOS || path[strlen(path)] == DIR_SEP)
     return (FALSE);

   root = AmayaParseUrl(path, "", AMAYA_PARSE_PATH | AMAYA_PARSE_PUNCTUATION);

   if (root) 
     {
       strcpy (temppath, root);
       TtaFreeMemory (root);
       /* Get the suffix */
       ExtractSuffix (temppath, suffix); 

       if( suffix[0] == EOS)
	 /* no suffix */
	 return (FALSE);

       /* Normalize the suffix */
       ConvertToLowerCase (suffix);

       if (!strcmp (suffix, "gz"))
	 /* skip the compressed suffix */
	 {
	 ExtractSuffix (temppath, suffix);
	 if(suffix[0] == EOS)
	   /* no suffix */
	   return (FALSE);
         /* Normalize the suffix */
         ConvertToLowerCase (suffix);
	 }

       if ((strcmp (suffix, "gif")) && (strcmp (suffix, "xbm")) &&
	   (strcmp (suffix, "xpm")) && (strcmp (suffix, "jpg")) &&
	   (strcmp (suffix, "pdf")) && (strcmp (suffix, "png")) &&
	   (strcmp (suffix, "tgz")) && (strcmp (suffix, "xpg")) &&
	   (strcmp (suffix, "xpd")) && (strcmp (suffix, "ps")) &&
	   (strcmp (suffix, "au"))  && (strcmp (suffix, "html")) &&
	   (strcmp (suffix, "htm")) && (strcmp (suffix, "shtml")) &&
	   (strcmp (suffix, "txt")) && (strcmp (suffix, "css")) &&
	   (strcmp (suffix, "eps")))
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
static void scan (char * name, HTURI * parts)
#else  /* __STDC__ */
static void scan (name, parts)
char                *name;
HTURI               *parts;

#endif /* __STDC__ */
{
  char      *p;
  char      *after_access = name;

  memset (parts, '\0', sizeof (HTURI));
  /* Look for fragment identifier */
  if ((p = strchr(name, '#')) != NULL)
    {
      *p++ = '\0';
      parts->fragment = p;
    }
    
  for (p=name; *p; p++)
    {
      if (*p == URL_SEP || *p == DIR_SEP || *p=='#' || *p=='?')
	break;
      if (*p==':')
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
	  if (!strcasecmp("URL", parts->access))
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
	    p = strchr (parts->host, URL_SEP);
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
char          *AmayaParseUrl (const char *aName, char *relatedName, int wanted)
#else  /* __STDC__ */
char          *AmayaParseUrl (aName, relatedName, wanted)
const char    *aName;
char          *relatedName;
int            wanted;

#endif /* __STDC__ */
{
  char      *return_value;
  char       result[MAX_LENGTH];
  char       name[MAX_LENGTH];
  char       rel[MAX_LENGTH];
  char      *p, *access;
  HTURI      given, related;
  int        len;
  char       used_sep;
  char      *used_str;

  if (strchr (aName, DIR_SEP) || strchr (relatedName, DIR_SEP))
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
  strcpy (name, aName);
  if (relatedName != NULL)  
    strcpy (rel, relatedName);
  else
    relatedName[0] = EOS;
  
  scan (name, &given);
  scan (rel,  &related); 
  access = given.access ? given.access : related.access;
  if (wanted & AMAYA_PARSE_ACCESS)
    if (access)
      {
	strcat (result, access);
	if(wanted & AMAYA_PARSE_PUNCTUATION)
		strcat (result, ":");
      }
  
  if (given.access && related.access)
    /* If different, inherit nothing. */
    if (strcmp (given.access, related.access) != 0)
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
	  strcat (result, "//");
	strcat (result, given.host ? given.host : related.host);
      }
  
  if (given.host && related.host)
    /* If different hosts, inherit no path. */
    if (strcmp(given.host, related.host) != 0)
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
	    strcat (result, used_str);
	  strcat (result, given.absolute);
	}
      else if (related.absolute)
	{
	  /* Adopt path not name */
	  strcat (result, used_str);
	  strcat (result, related.absolute);
	  if (given.relative)
	    {
	      /* Search part? */
	      p = strchr (result, '?');
	      if (!p)
		p=result+strlen(result)-1;
	      for (; *p!=used_sep; p--);	/* last / */
	      /* Remove filename */
	      p[1]=0;
	      /* Add given one */
	      strcat (result, given.relative);
	    }
	}
      else if (given.relative)
	/* what we've got */
	strcat (result, given.relative);
      else if (related.relative)
	strcat (result, related.relative);
      else
	/* No inheritance */
	strcat (result, used_str);
    }
  
  if (wanted & AMAYA_PARSE_ANCHOR)
    if (given.fragment || related.fragment)
      {
	if (given.absolute && given.fragment)
	  {
	    /*Fixes for relURLs...*/
	    if (wanted & AMAYA_PARSE_PUNCTUATION)
	      strcat (result, "#");
	    strcat (result, given.fragment); 
	  }
	else if (!(given.absolute) && !(given.fragment))
	  strcat (result, "");
	else
	  {
	    if (wanted & AMAYA_PARSE_PUNCTUATION)
	      strcat (result, "#");
	    strcat (result, given.fragment ? given.fragment : related.fragment); 
	  }
      }
  len = strlen (result);
  if ((return_value = (char *) TtaGetMemory (len + 1)) != NULL)
    strcpy (return_value, result);
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
static char *HTCanon (char ** filename, char * host)
#else  /* __STDC__ */
static char *HTCanon (filename, host)
char       **filename;
char        *host;
#endif /* __STDC__ */
{
    char *newname = NULL;
    char *port;
    char *strptr;
    char *path;
    char *access = host-3;
    char       used_sep;

  
     if (*filename && strchr (*filename, URL_SEP))
       {
	 used_sep = URL_SEP;
       }
     else
       {
	 used_sep = DIR_SEP;
       }
  
    while (access>*filename && *(access-1)!= used_sep)       /* Find access method */
	access--;
    if ((path = strchr(host, used_sep)) == NULL)			/* Find path */
	path = host + strlen(host);
    if ((strptr = strchr(host, '@')) != NULL && strptr<path)	   /* UserId */
	host = strptr;
    if ((port = strchr(host, ':')) != NULL && port>path)      /* Port number */
	port = NULL;

    strptr = host;				    /* Convert to lower-case */
    while (strptr<path)
      {
	*strptr = tolower(*strptr);
	strptr++;
      }
    
    /* Does the URL contain a full domain name? This also works for a
       numerical host name. The domain name is already made lower-case
       and without a trailing dot. */
    {
      char *dot = port ? port : path;
      if (dot > *filename && *--dot=='.')
	{
	  char *orig=dot, *dest=dot+1;
	  while((*orig++ = *dest++));
	  if (port) port--;
	  path--;
	}
    }
    /* Chop off port if `:', `:80' (http), `:70' (gopher), or `:21' (ftp) */
    if (port)
      {
	if (!*(port+1) || *(port+1)==used_sep)
	  {
	    if (!newname)
	      {
		char *orig=port, *dest=port+1;
		while((*orig++ = *dest++));
	      }
	  }
	else if ((!strncmp(access, "http", 4) &&
		  (*(port+1)=='8'&&*(port+2)=='0'&&(*(port+3)==used_sep||!*(port+3)))) ||
		 (!strncmp(access, "gopher", 6) &&
		  (*(port+1)=='7'&&*(port+2)=='0'&&(*(port+3)==used_sep||!*(port+3)))) ||
		 (!strncmp(access, "ftp", 3) &&
		  (*(port+1)=='2'&&*(port+2)=='1'&&(*(port+3)==used_sep||!*(port+3))))) {
	  if (!newname)
	    {
	      char *orig=port, *dest=port+3;
	      while((*orig++ = *dest++));
	      /* Update path position, Henry Minsky */
	      path -= 3;
	    }
	}
	else if (newname)
	  strncat(newname, port, (int) (path-port));
      }

    if (newname)
      {
	char *newpath = newname+strlen(newname);
	strcat(newname, path);
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
void         SimplifyUrl (char ** url)
#else  /* __STDC__ */
void         SimplifyUrl (url)
char        **url;
#endif /* __STDC__ */
{
  char *path, *p;
  char *newptr, *access;
  char *orig, *dest, *end;

  char       used_sep;


  if (!url || !*url)
    return;

  if (strchr (*url, URL_SEP))
    {
      used_sep = URL_SEP;
    }
  else
    {
      used_sep = DIR_SEP;
    }

  /* Find any scheme name */
  if ((path = strstr(*url, "://")) != NULL)
    {
      /* Find host name */
      access = *url;
      while (access<path && (*access=tolower(*access)))
	access++;
      path += 3;
      while ((newptr = strstr(path, "://")) != NULL)
        /* For proxies */
	path = newptr+3;
      /* We have a host name */
      path = HTCanon(url, path);
    }
  else if ((path = strstr(*url, ":/")) != NULL)
    path += 2;
  else
    path = *url;

  if (*path == used_sep && *(path+1)==used_sep)
    /* Some URLs start //<foo> */
    path += 1;
  else if (!strncmp(path, "news:", 5))
    {
      newptr = strchr(path+5, '@');
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
      if (!((end = strchr (path, ';')) || (end = strchr (path, '?')) ||
	    (end = strchr (path, '#'))))
	end = path + strlen (path);
      
      /* Parse string second time to simplify */
      p = path;
      while (p < end)
	{
	  if (*p==used_sep)
	    {
	      if (p > *url && *(p+1) == '.' && (*(p+2) == used_sep || !*(p+2)))
		{
		  orig = p + 1;
		  dest = (*(p+2)!=used_sep) ? p+2 : p+3;
		  while ((*orig++ = *dest++)); /* Remove a used_sep and a dot*/
		  end = orig - 1;
		}
	      else if (*(p+1)=='.' && *(p+2)=='.' && (*(p+3)==used_sep || !*(p+3)))
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
    if (*path==used_sep && *(path+1)=='.' && *(path+2)=='.' && (!*(path+3) || *(path+3)==used_sep))
	*(path+1) = EOS;

  return;
}


/*----------------------------------------------------------------------
   NormalizeFile normalizes  local names.                             
   Return TRUE if target and src differ.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             NormalizeFile (char *src, char *target)
#else
boolean             NormalizeFile (src, target)
char               *src;
char               *target;

#endif
{
   char               *s;
   boolean             change;

   change = FALSE;
   if (strncmp (src, "file:", 5) == 0)
     {
	/* remove the prefix file: */
	if (src[5] == EOS)
	   strcpy (target, DIR_STR);
	else if (src[0] == '~')
	  {
	    /* replace ~ */
	    s = (char *) TtaGetEnvString ("HOME");
	    strcpy (target, s);
	    strcat (target, &src[5]);
	  }
	else
	   strcpy (target, &src[5]);
	change = TRUE;
     }
#  ifndef _WINDOWS
   else if (src[0] == '~')
     {
	/* replace ~ */
	s = (char *) TtaGetEnvString ("HOME");
	strcpy (target, s);
	if (src[1] != DIR_SEP)
	  strcat (target, DIR_STR);
	strcat (target, &src[1]);
	change = TRUE;
     }
#   endif /* _WINDOWS */
   else
      strcpy (target, src);

   /* remove /../ and /./ */
   SimplifyUrl (&target);
   if (!change)
     change = strcmp (src, target);
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
char            *MakeRelativeURL (char *aName, char *relatedName)
#else  /* __STDC__ */
char            *MakeRelativeURL (aName, relatedName)
char            *aName;
char            *relatedName;
#endif  /* __STDC__ */
{
  char          *return_value;
  char           result[MAX_LENGTH];
  char          *p;
  char          *q;
  char          *after_access;
  char          *last_slash = NULL;
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
      if (*p == ':')
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
      len = strlen (p);
      if ((return_value = (char *) TtaGetMemory (len + 1)) != NULL)
	strcpy (return_value, p);
    }
  else if ((slashes < 2 && after_access == NULL)
      || (slashes < 3 && after_access != NULL))
    {
      /* Two names whitout common path */
      /* exactly the right length */
      len = strlen (aName);
      if ((return_value = (char *) TtaGetMemory (len + 1)) != NULL)
	strcpy (return_value, aName);
    }
  else
    {
      /* Some path in common */
      if (slashes == 3 && strncmp (aName, "http:", 5) == 0)
	/* just the same server */
	strcpy (result, last_slash);
      else
	{
	  levels= 0; 
	  for (; *q && *q != '#' && *q!=';' && *q!='?'; q++)
	    if (*q == DIR_SEP)
	      levels++;
	  
	  result[0] = EOS;
	  for (;levels; levels--)
	    strcat (result, "../");
	  strcat (result, last_slash+1);
	} 

      if (!*result)
	strcat (result, "./");

      /* exactly the right length */
      len = strlen (result);
      if ((return_value = (char *) TtaGetMemory (len + 1)) != NULL)
	strcpy (return_value, result);

    }
# ifdef _WINDOWS
  len = strlen (return_value);
  for (ndx = 0; ndx < len; ndx ++)
	  if (return_value[ndx] == '\\')
	     return_value[ndx] = '/' ;
# endif /* _WINDOWS */
  return (return_value);
}


