/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * AHTURLTools.c: contains all the functions for testing, manipulating,
 * and normalizing URLs.
 *
 * Authors: J. Kahan, I. Vatton
 *
 */
 
/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"


#include "init_f.h"
#include "AHTURLTools_f.h"

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
       nsuffix[i] = TOLOWER (suffix[i]);
       i++;
     }
   nsuffix[i] = EOS;
   if ((strcmp (nsuffix, "html")) &&
       (strcmp (nsuffix, "htm")) &&
       (strcmp (nsuffix, "shtml")))
     return (FALSE);
   else if ((!strcmp (nsuffix, "gz")) ||
	    (!strcmp (nsuffix, "z")))
     {
       /* take in account compressed files */
       ExtractSuffix (temppath, suffix);       
       /* Normalize the suffix */
       i = 0;
       while (suffix[i] != EOS)
	 {
	   nsuffix[i] = TOLOWER (suffix[i]);
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
       nsuffix[i] = TOLOWER (suffix[i]);
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
	nsuffix[i] = TOLOWER (suffix[i]);
	i++;
     }
   nsuffix[i] = EOS;

   if ((strcmp (nsuffix, "gif")) && (strcmp (nsuffix, "xbm")) &&
       (strcmp (nsuffix, "xpm")) && (strcmp (nsuffix, "jpg")) &&
       (strcmp (nsuffix, "pdf")) && (strcmp (nsuffix, "png")) &&
       (strcmp (nsuffix, "tgz")) && (strcmp (nsuffix, "xpg")) &&
       (strcmp (nsuffix, "xpd")) && (strcmp (nsuffix, "ps")) &&
       (strcmp (nsuffix, "au")))
      return (TRUE);
   else if ((!strcmp (nsuffix, "gz")) || (!strcmp (nsuffix, "z")))
     {
       /* take in account compressed files */
       ExtractSuffix (temppath, suffix);       
       /* Normalize the suffix */
       i = 0;
       while (suffix[i] != EOS)
	 {
	   nsuffix[i] = TOLOWER (suffix[i]);
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
   if (!strncmp (url, "http:", 5)
      /***|| !strncmp (path, "ftp:", 4)
      || !strncmp (path, "news:", 5)***/ )
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
   while ((*ptr == ' ' || *ptr == '\n') && *ptr++ != EOS);
   strcpy (tempOrgName, ptr);
   /* clean trailing white space */
   ptr = strchr (tempOrgName, ' ');
   if (ptr)
      *ptr = EOS;
   /* clean trailing new lines */
   ptr = strchr (tempOrgName, '\n');
   if (ptr)
      *ptr = EOS;

   /*
   ** Second Step: make orgName a complete URL
   ** If the URL does not include a protocol, then
   ** try to calculate one using the doc's base element 
   ** (if it exists),
   */

   if (tempOrgName[0] == EOS || IsW3Path (tempOrgName) || doc == 0)
     /* the name is complete, go to the Fifth Step */
     strcpy (newName, tempOrgName);
   else
     {
       /* take into account the BASE element. */
       length = MAX_LENGTH;
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
	       if (basename[0] != EOS && basename[length] != '/') 
		 /* verify if the base has the form "protocol://server:port" */
		 {
		   ptr = HTParse (basename, "", PARSE_ACCESS | PARSE_HOST |
				                PARSE_PUNCTUATION);
		   if (ptr && !strcmp (ptr, basename))
		     {
		     /* it has this form, we complete it by adding a "/"  */
		     strcat (basename, "/");
		     length++;
		     }
		   if (ptr)
		     HT_FREE (ptr);
		 }
	       /* search for the first DIR_SEP char */
	       while (length >= 0  && basename[length] != DIR_SEP)
		 basename[length--] = EOS;		   
	     }
	   else
	     basename[0] = EOS;
	 }
       else
	 basename[0] = EOS;

       /*
       ** Third Step: 
       ** If there's no base element, and if we're following
       ** a link, use the URL of the current document as a base
       */

       if (!attrHREF)
	 {
	   if (DocumentURLs[(int) doc])
	     {
	       strcpy (basename, DocumentURLs[(int) doc]);
	       /* base and orgName have to be separated by a DIR_SEP */
	       length = strlen (basename) - 1;
	       /* search for the first DIR_SEP char */
	       while (length >= 0  && basename[length] != DIR_SEP)
		 basename[length--] = EOS;		   
	     }
	   else
	     {
	       basename [0] = EOS;
	     }
	 }
  
       /*
       ** Fourth Step, calculate the absolute URL, using the base
       */

       ptr = HTParse (tempOrgName, basename, PARSE_ALL);

       if (ptr)
	 {
	   ptr = HTSimplify (&ptr);
	   strcpy (newName, ptr);
	   HT_FREE (ptr);
	 }
       else
	   newName[0] = EOS;
     }

   /*
   ** Fifth step:
   ** Prepare the docname that will refer to this ressource in the
   ** .amaya directory. If the new URL finishes on "/", then use
   ** noname.html as a default ressource name
   */
   length = strlen (newName) - 1;
   if (length > 0)
     {
       if (newName[length] == DIR_SEP)
	 {
	   /* docname was not comprised inside the URL, so let's */
	   /* assign the default ressource name */
	   strcpy (docName, "noname.html");
	   /* remove DIR_SEP at the end of complete path */
	   newName[length] = EOS;
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

   basename_ptr1 = HTParse (url1, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);
   basename_ptr2 = HTParse (url2, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);

   if (strcmp (basename_ptr1, basename_ptr2))
      result = FALSE;
   else
      result = TRUE;

   HT_FREE (basename_ptr1);
   HT_FREE (basename_ptr2);

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

   base_ptr = HTParse (base_url, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);
   url_ptr = HTParse (url, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);

   if (!strcmp (base_ptr, url_ptr))
     {
	HT_FREE (base_ptr);
	HT_FREE (url_ptr);

	/* Normalize the URLs */

	base_ptr = HTParse (base_url, "", PARSE_ALL);
	url_ptr = HTParse (url, "", PARSE_ALL);

	/* Use libwww to make relative name */

	result = HTRelative (url_ptr, base_ptr);
	HT_FREE (base_ptr);
	HT_FREE (url_ptr);
     }
   else
      result = (char *) NULL;

   return (result);
}



