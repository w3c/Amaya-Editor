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
 *
 */
 
/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"

#include "init_f.h"
#include "AHTURLTools_f.h"

/* Local definitions */

#define MAX_PRINT_URL_LENGTH 50

/* Private  functions */
#ifdef __STDC__
static void         ConvertToLowerCase (char *string);
#else
static void         ConvertToLowerCase (/*char *string*/);
#endif

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
   char               *curr, *temp;

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
   while ((curr >= url) && (*curr != DIR_SEP));

   if (curr < url)
      goto finished;
   *file = curr + 1;

   /* mark the end of the dir */
   *curr = EOS;
   curr--;
   if (curr < url)
      goto finished;

   /* search for the "/" indicating the host name start */
   while ((curr > url) && ((*curr != DIR_SEP) || (*(curr + 1) != DIR_SEP)))
      curr--;

   /* if we found it, separate the host name from the directory */
   if ((*curr == DIR_SEP) && (*(curr + 1) == DIR_SEP))
     {
	*host = temp = curr + 2;
	while ((*temp != 0) && (*temp != DIR_SEP))
	   temp++;
	if (*temp == DIR_SEP)
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
boolean             IsHTMLName (char *path)
#else  /* __STDC__ */
boolean             IsHTMLName (path)
char               *path;
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
   while (suffix[i] != EOS)
     {
       nsuffix[i] = tolower (suffix[i]);
       i++;
     }
   nsuffix[i] = EOS;
   if ((strcmp (nsuffix, "html")) &&
       (strcmp (nsuffix, "htm")) &&
       (strcmp (nsuffix, "shtml")))
     return (FALSE);
   else if (!strcmp (nsuffix, "gz"))
     {
       /* take in account compressed files */
       ExtractSuffix (temppath, suffix);       
       /* Normalize the suffix */
       i = 0;
       while (suffix[i] != EOS)
	 {
	   nsuffix[i] = tolower (suffix[i]);
	   i++;
	 }
       nsuffix[i] = EOS;
       if ((strcmp (nsuffix, "html")) &&
	   (strcmp (nsuffix, "htm")) &&
	   (strcmp (nsuffix, "shtml")))
	 return (FALSE);
       else
	 return (TRUE);
     }
   else
     return (TRUE);
}

/*----------------------------------------------------------------------
  IsImageName                                
  returns TRUE if path points to an image resource.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsImageName (char *path)
#else  /* __STDC__ */
boolean             IsImageName (path)
char               *path;
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
   while (suffix[i] != EOS)
     {
       nsuffix[i] = tolower (suffix[i]);
       i++;
     }
   nsuffix[i] = EOS;
   if ((strcmp (nsuffix, "gif")) && (strcmp (nsuffix, "xbm")) &&
       (strcmp (nsuffix, "xpm")) && (strcmp (nsuffix, "jpg")) &&
       (strcmp (nsuffix, "png")) && (strcmp (nsuffix, "au")))
      return (FALSE);
   return (TRUE);
}

/*----------------------------------------------------------------------
  IsTextName                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsTextName (char *path)
#else  /* __STDC__ */
boolean             IsTextName (path)
char               *path;

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
   while (suffix[i] != EOS)
     {
	nsuffix[i] = tolower (suffix[i]);
	i++;
     }
   nsuffix[i] = EOS;

   if ((strcmp (nsuffix, "gif")) && (strcmp (nsuffix, "xbm")) &&
       (strcmp (nsuffix, "xpm")) && (strcmp (nsuffix, "jpg")) &&
       (strcmp (nsuffix, "pdf")) && (strcmp (nsuffix, "png")) &&
       (strcmp (nsuffix, "tgz")) && (strcmp (nsuffix, "tar")) &&
       (strcmp (nsuffix, "xpg")) && (strcmp (nsuffix, "xpd")) &&
       (strcmp (nsuffix, "ps"))  && (strcmp (nsuffix, "au")))
      return (TRUE);
   else if (!strcmp (nsuffix, "gz"))
     {
       /* take in account compressed files */
       ExtractSuffix (temppath, suffix);       
       /* Normalize the suffix */
       i = 0;
       while (suffix[i] != EOS)
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
  IsHTTPPath                                     
  returns TRUE if path is in fact an http URL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsHTTPPath (char *path)
#else  /* __STDC__ */
boolean             IsHTTPPath (path)
char               *path;
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
boolean             IsWithParameters (char *url)
#else  /* __STDC__ */
boolean             IsWithParameters (url)
char               *url;
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
boolean             IsW3Path (char *path)
#else  /* __STDC__ */
boolean             IsW3Path (path)
char               *path;
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
boolean             IsValidProtocol (char *url)
#else  /* __STDC__ */
boolean             IsValidProtocol (url)
char               *url;
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
   NormalizeURL
   normalizes orgName according to a base associated with doc, and
   following the standard URL format rules.
   The function returns the new complete and normalized URL 
   or file name path (newName) and the name of the document (docName).        
   N.B. If the function can't find out what's the docName, it assigns
   the name "noname.html".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NormalizeURL (char *orgName, Document doc, char *newName, char *docName)
#else  /* __STDC__ */
void                NormalizeURL (orgName, doc, newName, docName)
char               *orgName;
Document            doc;
char               *newName;
char               *docName;
#endif /* __STDC__ */
{
   char                basename[MAX_LENGTH];
   char                tempOrgName[MAX_LENGTH];
   char               *ptr;
   Element             el;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attrHREF = NULL;
   int                 length;

   if (!newName || !docName)
      return;

   /*
   ** First Step: Clean orgName
   ** Make sure we have a complete orgName, without any leading or trailing
   ** white spaces, or trailinbg new lines
   */

   ptr = orgName;
   /* skip leading white space and new line characters */
   while ((*ptr == ' ' || *ptr == EOL) && *ptr++ != EOS);
   strcpy (tempOrgName, ptr);
   /* clean trailing white space */
   ptr = strchr (tempOrgName, ' ');
   if (ptr)
      *ptr = EOS;
   /* clean trailing new lines */
   ptr = strchr (tempOrgName, EOL);
   if (ptr)
      *ptr = EOS;

   /*
   ** Second Step: make orgName a complete URL
   ** If the URL does not include a protocol, then
   ** try to calculate one using the doc's base element 
   ** (if it exists),
   */
   if (tempOrgName[0] == EOS)
     {
       newName[0] = EOS;
       return;
     }
   else if (IsW3Path (tempOrgName))
     {
       /* the name is complete, go to the Sixth Step */
       strcpy (newName, tempOrgName);
       /* verify if the URL has the form "protocol://server:port" */
       ptr = AmayaParseUrl (newName, "", AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST |
		      AMAYA_PARSE_PUNCTUATION);
       if (ptr && !strcmp (ptr, newName))
	 {
	   /* it has this form, we complete it by adding a "/"  */
	   strcat (newName, "/");
	 }
       if (ptr)
	 TtaFreeMemory (ptr);
     }
   else if ( doc == 0)
     /* the name is complete, go to the Sixth Step */
     strcpy (newName, tempOrgName);
   else
     {
       /* take into account the BASE element. */
       length = MAX_LENGTH -1;
       /* get the root element    */
       el = TtaGetMainRoot (doc);
	   
       /* search the BASE element */
       elType.ElSSchema = TtaGetDocumentSSchema (doc);
       elType.ElTypeNum = HTML_EL_BASE;
       el = TtaSearchTypedElement (elType, SearchInTree, el);
       if (el)
	 {
	   /* 
	   ** The document has a BASE element 
	   ** Get the HREF attribute of the BASE Element 
	   */
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_HREF_;
	   attrHREF = TtaGetAttribute (el, attrType);
	   if (attrHREF)
	     {
	       /* Use the base path of the document */
	       TtaGiveTextAttributeValue (attrHREF, basename, &length);
	       /* base and orgName have to be separated by a DIR_SEP */
	       length--;
	       if (basename[0] != EOS && basename[length] != '/') 
		 /* verify if the base has the form "protocol://server:port" */
		 {
		   ptr = AmayaParseUrl (basename, "", AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST |
				                AMAYA_PARSE_PUNCTUATION);
		   if (ptr && !strcmp (ptr, basename))
		     {
		     /* it has this form, we complete it by adding a "/"  */
		     strcat (basename, "/");
		     length++;
		     }
		   if (ptr)
		     TtaFreeMemory (ptr);
		 }
	       /* Third Step: prepare the base
	       ** Removing anything after the
	       ** last DIR_SEP char. If no such char is found, then search for
	       ** the first ":" char, hoping that what's before that is a
	       ** protocol. If found, end the string there. If neither
	       ** char is found, then discard the whole base element.
	       */

	       /* search for the last DIR_SEP char */
	       while (length >= 0  && basename[length] != DIR_SEP)
		 length--;
	       if (length >= 0)
		 /* found the last DIR_SEP char, end the string there */
		 basename[length + 1] = EOS;		   
	       else
		 /* search for the first ":" char */
		 {
		   for (length = 0; basename[length] != ':' && 
			  basename[length] != EOS; length++);
		   if (basename[length] == ':')
		     /* found, so end the string there */
		     basename[length + 1] = EOS;
		   else
		     /* not found, discard the base */
		     basename[0] = EOS;
		 }
	     }
	   else
	     basename[0] = EOS;
	 }
     
       /*
       ** Fourth Step: 
       ** If there's no base element, and if we're following
       ** a link, use the URL of the current document as a base.
       */

       if (!attrHREF)
	 {
	   if (DocumentURLs[(int) doc])
	     {
	       strcpy (basename, DocumentURLs[(int) doc]);
	       /* base and orgName have to be separated by a DIR_SEP */
	       length = strlen (basename) - 1;
	       /* search for the last DIR_SEP char */
	       while (length >= 0  && basename[length] != DIR_SEP)
		 length--;
	       if (length >= 0)
		 /* found the last DIR_SEP char, end the string there */
		 basename[length + 1] = EOS;		   
	       else
		 /* search for the first ":" char */
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
	     }
	   else
	       basename[0] = EOS;
	 }
     
       /*
       ** Fifth Step, calculate the absolute URL, using the base
       */

       ptr = AmayaParseUrl (tempOrgName, basename, AMAYA_PARSE_ALL);

       if (ptr)
	 {
	   ptr = AmayaSimplifyUrl (&ptr);
	   strcpy (newName, ptr);
	   TtaFreeMemory (ptr);
	 }
       else
	   newName[0] = EOS;
     }

   /*
   ** Sixth and last Step:
   ** Prepare the docname that will refer to this ressource in the
   ** .amaya directory. If the new URL finishes on DIR_SEP, then use
   ** noname.html as a default ressource name
   */

   if (newName[0] != EOS)
     {
       length = strlen (newName) - 1;
       if (newName[length] == DIR_SEP)
	 {
	   /* docname was not comprised inside the URL, so let's */
	   /* assign the default ressource name */
	   strcpy (docName, "noname.html");
	   /* remove DIR_SEP at the end of complete path */
	   /* newName[length] = EOS; */
	 }
       else
	 {
	   /* docname is comprised inside the URL */
	   while (length >= 0  && newName[length] != DIR_SEP)
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
boolean             IsSameHost (char *url1, char *url2)
#else  /* __STDC__ */
boolean             IsSameHost (url1, url2)
char               *path;
#endif /* __STDC__ */
{
   char               *basename_ptr1, *basename_ptr2;
   boolean             result;

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
  AHTMakeRelativeURL                                                
  converts url into a relative url to base_url.
  If succesful, returns the new URL, otherwise, it returns NULL.
  The caller has to free the new URL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *AHTMakeRelativeName (char *url, char *base_url)
#else  /* __STDC__ */
char               *AHTMakeRelativeName (url, base_url)
char                url;
char                base_url;

#endif /* __STDC__ */
{
   char               *base_ptr, *url_ptr;
   char               *result;

   /* verify if we are in the same host */

   base_ptr = AmayaParseUrl (base_url, "", AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);
   url_ptr = AmayaParseUrl (url, "", AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);

   if (!strcmp (base_ptr, url_ptr))
     {
	TtaFreeMemory (base_ptr);
	TtaFreeMemory (url_ptr);

	/* Normalize the URLs */

	base_ptr = AmayaParseUrl (base_url, "", AMAYA_PARSE_ALL);
	url_ptr = AmayaParseUrl (url, "", AMAYA_PARSE_ALL);

	/* Use libwww to make relative name */

	result = AmayaRelativeUrl (url_ptr, base_ptr);
	TtaFreeMemory (base_ptr);
	TtaFreeMemory (url_ptr);
     }
   else
      result = (char *) NULL;

   return (result);
}
/*----------------------------------------------------------------------
  HasKnownFileSuffix
  returns TRUE if path points to a file ending with a suffix.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             HasKnownFileSuffix (char *path)
#else  /* __STDC__ */
boolean             HasKnownFileSuffix (path)
char               *path;
#endif /* __STDC__ */
{
   char                *root;
   char                temppath[MAX_LENGTH];
   char                suffix[MAX_LENGTH];

   if (!path || path[0] == EOS || path[strlen(path)] == DIR_SEP)
     return (FALSE);

   root = AmayaParseUrl(path, (char *) NULL, AMAYA_PARSE_PATH | AMAYA_PARSE_PUNCTUATION);

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
  ConvertToLowerCase
  Converts a string to lowercase.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ConvertToLowerCase (char *string)
#else  /* __STDC__ */
static void         ConvertToLowerCase (string)
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
void ChopURL (char *outputURL, char *inputURL)
#else
void ChopURL (outputURL, inputURL)
char *outputURL;
char *inputURL;
#endif

{
  int len;

  len = strlen (inputURL);
  if (len <= MAX_PRINT_URL_LENGTH) 
    {
      strcpy (outputURL, inputURL);
    }
  else
    /* make a truncated urlName on the status window */
    {
      strncpy (outputURL, inputURL, MAX_PRINT_URL_LENGTH / 2);
      outputURL [MAX_PRINT_URL_LENGTH / 2] = EOS;
      strcat (outputURL, "...");
      strcat (outputURL, &(inputURL[len - MAX_PRINT_URL_LENGTH / 2 ]));
    }
}

/************************************************************************
 *									*
 *	Local Adaptation of the libWWW Library/src/AmayaParseUrl.c code.	*
 *									*
 ************************************************************************/

#define StringAllocCopy(dest,src) {					\
    if (src == NULL) dest = NULL;					\
    else {								\
	if ((dest = (char *) TtaGetMemory(strlen(src) + 1)) == NULL)	\
		exit(1);						\
	else strcpy(dest, src); }}					\

typedef struct _HTURI {
    char * access;		/* Now known as "scheme" */
    char * host;
    char * absolute;
    char * relative;
    char * fragment;
} HTURI;

/*----------------------------------------------------------------------
   scan
  	Scan a filename for its consituents
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
    char * p;
    char * after_access = name;
    memset(parts, '\0', sizeof(HTURI));

    /* Look for fragment identifier */
    if ((p = strrchr(name, '#')) != NULL) {
	*p++ = '\0';
	parts->fragment = p;
    }
    
    for(p=name; *p; p++) {
	if (*p=='/' || *p=='#' || *p=='?')
	    break;
	if (*p==':') {
		*p = 0;
		parts->access = after_access; /* Scheme has been specified */

/* The combination of gcc, the "-O" flag and the HP platform is
   unhealthy. The following three lines is a quick & dirty fix, but is
   not recommended. Rather, turn off "-O". */

/*		after_access = p;*/
/*		while (*after_access == 0)*/
/*		    after_access++;*/

		after_access = p+1;

		if (0==strcasecmp("URL", parts->access)) {
		    parts->access = NULL;  /* Ignore IETF's URL: pre-prefix */
		} else break;
	}
    }
    
    p = after_access;
    if (*p=='/'){
	if (p[1]=='/') {
	    parts->host = p+2;		/* host has been specified 	*/
	    *p=0;			/* Terminate access 		*/
	    p=strchr(parts->host,'/');	/* look for end of host name if any */
	    if(p) {
	        *p=0;			/* Terminate host */
	        parts->absolute = p+1;		/* Root has been found */
	    }
	} else {
	    parts->absolute = p+1;		/* Root found but no host */
	}	    
    } else {
        parts->relative = (*after_access) ? after_access : 0; /* zero for "" */
    }
}


/*----------------------------------------------------------------------
    AmayaParseUrl
  	Parse a Name relative to another name
  	-------------------------------------
  
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
char * AmayaParseUrl (const char *aName, const char *relatedName, int wanted)
#else  /* __STDC__ */
char * AmayaParseUrl (aName, relatedName, wanted)
const char          *aName;
const char          *relatedName;
int                  wanted;

#endif /* __STDC__ */
{
    char * result = 0;
    char * return_value = 0;
    int len;
    char * name = 0;
    char * rel = 0;
    char * p;
    char * access;
    HTURI given, related;
    
    if (!relatedName)        /* HWL 23/8/94: dont dump due to NULL */
        relatedName = "";
    
    /* Make working copies of input strings to cut up: */
    len = strlen(aName)+strlen(relatedName)+10;
    if ((result=(char *) TtaGetMemory(len)) == NULL) /* Lots of space: more than enough */
	exit(1);
    StringAllocCopy(name, aName);
    StringAllocCopy(rel, relatedName);
    
    scan(name, &given);
    scan(rel,  &related); 
    result[0]=0;		/* Clear string  */
    access = given.access ? given.access : related.access;
    if (wanted & AMAYA_PARSE_ACCESS)
        if (access) {
	    strcat(result, access);
	    if(wanted & AMAYA_PARSE_PUNCTUATION) strcat(result, ":");
	}
	
    if (given.access && related.access)	/* If different, inherit nothing. */
        if (strcmp(given.access, related.access)!=0) {
	    related.host=0;
	    related.absolute=0;
	    related.relative=0;
	    related.fragment=0;
	}
	
    if (wanted & AMAYA_PARSE_HOST)
        if(given.host || related.host) {
	    if(wanted & AMAYA_PARSE_PUNCTUATION) strcat(result, "//");
	    strcat(result, given.host ? given.host : related.host);
	}
	
    if (given.host && related.host)  /* If different hosts, inherit no path. */
        if (strcmp(given.host, related.host)!=0) {
	    related.absolute=0;
	    related.relative=0;
	    related.fragment=0;
	}
	
    if (wanted & AMAYA_PARSE_PATH) {
        if(given.absolute) {				/* All is given */
	    if(wanted & AMAYA_PARSE_PUNCTUATION) strcat(result, "/");
	    strcat(result, given.absolute);
	} else if(related.absolute) {	/* Adopt path not name */
	    strcat(result, "/");
	    strcat(result, related.absolute);
	    if (given.relative) {
		p = strchr(result, '?');	/* Search part? */
		if (!p) p=result+strlen(result)-1;
		for (; *p!='/'; p--);	/* last / */
		p[1]=0;					/* Remove filename */
		strcat(result, given.relative);		/* Add given one */
#if 0
		result = AmayaSimplifyUrl (&result);
#endif
	    }
	} else if(given.relative) {
	    strcat(result, given.relative);		/* what we've got */
	} else if(related.relative) {
	    strcat(result, related.relative);
	} else {  /* No inheritance */
	    strcat(result, "/");
	}
    }
		
    if (wanted & AMAYA_PARSE_ANCHOR)
	if(given.fragment || related.fragment) {
	    if(given.absolute && given.fragment) {   /*Fixes for relURLs...*/
		if(wanted & AMAYA_PARSE_PUNCTUATION) strcat(result, "#");
		strcat(result, given.fragment); 
	    } else if (!(given.absolute) && !(given.fragment)) {
		strcat(result, "");
	    } else {
		if(wanted & AMAYA_PARSE_PUNCTUATION) strcat(result, "#");
		strcat(result, given.fragment ? given.fragment : related.fragment); 
	    }
	}
    TtaFreeMemory(rel);
    TtaFreeMemory(name);
    
    StringAllocCopy(return_value, result);
    TtaFreeMemory(result);
    return return_value;		/* exactly the right length */
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
static char * HTCanon (char ** filename, char * host)
#else  /* __STDC__ */
static char * HTCanon (filename,host)
char               **filename;
char                *host;
#endif /* __STDC__ */
{
    char *newname = NULL;
    char *port;
    char *strptr;
    char *path;
    char *access = host-3;

    while (access>*filename && *(access-1)!='/')       /* Find access method */
	access--;
    if ((path = strchr(host, '/')) == NULL)			/* Find path */
	path = host + strlen(host);
    if ((strptr = strchr(host, '@')) != NULL && strptr<path)	   /* UserId */
	host = strptr;
    if ((port = strchr(host, ':')) != NULL && port>path)      /* Port number */
	port = NULL;

    strptr = host;				    /* Convert to lower-case */
    while (strptr<path) {
	*strptr = tolower(*strptr);
	strptr++;
    }
    
    /* Does the URL contain a full domain name? This also works for a
       numerical host name. The domain name is already made lower-case
       and without a trailing dot. */
    {
	char *dot = port ? port : path;
	if (dot > *filename && *--dot=='.') {
	    char *orig=dot, *dest=dot+1;
	    while((*orig++ = *dest++));
	    if (port) port--;
	    path--;
	}
    }
    /* Chop off port if `:', `:80' (http), `:70' (gopher), or `:21' (ftp) */
    if (port) {
	if (!*(port+1) || *(port+1)=='/') {
	    if (!newname) {
		char *orig=port, *dest=port+1;
		while((*orig++ = *dest++));
	    }
	} else if ((!strncmp(access, "http", 4) &&
	     (*(port+1)=='8'&&*(port+2)=='0'&&(*(port+3)=='/'||!*(port+3)))) ||
	    (!strncmp(access, "gopher", 6) &&
	     (*(port+1)=='7'&&*(port+2)=='0'&&(*(port+3)=='/'||!*(port+3)))) ||
	    (!strncmp(access, "ftp", 3) &&
	     (*(port+1)=='2'&&*(port+2)=='1'&&(*(port+3)=='/'||!*(port+3))))) {
	    if (!newname) {
		char *orig=port, *dest=port+3;
		while((*orig++ = *dest++));
		path -= 3;   	       /* Update path position, Henry Minsky */
	    }
	} else if (newname)
	    strncat(newname, port, (int) (path-port));
    }

    if (newname) {
	char *newpath = newname+strlen(newname);
	strcat(newname, path);
	path = newpath;
	TtaFreeMemory(*filename);				    /* Free old copy */
	*filename = newname;
    }
    return path;
}


/*----------------------------------------------------------------------
    AmayaSimplifyUrl
  	        Simplify a URI
  		--------------
   A URI is allowed to contain the seqeunce xxx/../ which may be
   replaced by "" , and the seqeunce "/./" which may be replaced by "/".
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
char *AmayaSimplifyUrl (char ** url)
#else  /* __STDC__ */
char *AmayaSimplifyUrl (url)
char               **url;
#endif /* __STDC__ */
{
    char *path;
    char *p;
    if (!url || !*url) {
	return *url;
    }

    /* Find any scheme name */
    if ((path = strstr(*url, "://")) != NULL) {		   /* Find host name */
	char *newptr;
	char *access = *url;
	while (access<path && (*access=tolower(*access))) access++;
	path += 3;
	while ((newptr = strstr(path, "://")) != NULL)        /* For proxies */
	    path = newptr+3;
	path = HTCanon(url, path);       	      /* We have a host name */
    } else if ((path = strstr(*url, ":/")) != NULL) {
	path += 2;
    } else
	path = *url;
    if (*path == '/' && *(path+1)=='/') {	  /* Some URLs start //<foo> */
	path += 1;
    } else if (!strncmp(path, "news:", 5)) {
	char *ptr = strchr(path+5, '@');
	if (!ptr) ptr = path+5;
	while (*ptr) {			    /* Make group or host lower case */
	    *ptr = tolower(*ptr);
	    ptr++;
	}
	return *url;		      /* Doesn't need to do any more */
    }
    if ((p = path)) {
	char *end;
	if (!((end = strchr(path, ';')) || (end = strchr(path, '?')) ||
	      (end = strchr(path, '#'))))
	    end = path+strlen(path);

	/* Parse string second time to simplify */
	p = path;
	while(p<end) {
	    if (*p=='/') {
		if (p>*url && *(p+1)=='.' && (*(p+2)=='/' || !*(p+2))) {
		    char *orig = p+1;
		    char *dest = (*(p+2)!='/') ? p+2 : p+3;
		    while ((*orig++ = *dest++)); /* Remove a slash and a dot */
		    end = orig-1;
		} else if (*(p+1)=='.' && *(p+2)=='.' && (*(p+3)=='/' || !*(p+3))) {
		    char *q = p;
		    while (q>path && *--q!='/');	       /* prev slash */
		    if (strncmp(q, "/../", 4)) {
			char *orig = q+1;
			char *dest = (*(p+3)!='/') ? p+3 : p+4;
			while ((*orig++ = *dest++));	   /* Remove /xxx/.. */
			end = orig-1;
			p = q;		      /* Start again with prev slash */
		    } else
			p++;
		} else if (*(p+1)=='/') {
		    while (*(p+1)=='/') {
			char *orig=p, *dest=p+1;
			while ((*orig++ = *dest++));  /* Remove multiple /'s */
			end = orig-1;
		    }
		} else
		    p++;
	    } else
		p++;
	}
    }
    return *url;
}

/*----------------------------------------------------------------------
     AmayaRelativeUrl
  		Make Relative Name
  		------------------
  
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
char *AmayaRelativeUrl (const char * aName, const char * relatedName)
#else  /* __STDC__ */
char *AmayaRelativeUrl (const char * aName, const char * relatedName)
const char            *aName;
#endif  /* __STDC__ */
{
  char               *result = 0;
  const char         *p = aName;
  const char         *q = relatedName;
  const char         *after_access = 0;
  const char         *last_slash = 0;
  int                 slashes = 0;
    
  for(;*p; p++, q++)
    {
      /* Find extent of match */
      if (*p != *q)
	break;
      if (*p == ':')
	after_access = p+1;
      if (*p == '/')
	{
	  last_slash = p;
	  slashes++;
	}
    }
    
    /* q, p point to the first non-matching character or zero */
    if (!after_access)
      {
	/* Different access */
        StringAllocCopy (result, aName);
      }
    else if (slashes < 3)
      {
	/* Different nodes */
    	StringAllocCopy(result, after_access);
      }
    else
      {
	/* Some path in common */
        int levels= 0;
        for (; *q && (*q!='#'); q++)
	  if (*q=='/')
	    levels++;
	result = (char  *) TtaGetMemory (3*levels + strlen(last_slash) + 1);
	if (result == NULL)
	  exit(1);
	result[0] = 0;
	for (;levels; levels--)
	  strcat (result, "../");
	strcat (result, last_slash+1);
    }
    return result;
}
