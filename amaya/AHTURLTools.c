/* 
** -- Copyright (c) 1996-1997 Inria/CNRS  All rights reserved. -- 
*/

#include "amaya.h"

#include "dialog.h"
#include "content.h"
#include "view.h"
#include "interface.h"
#include "message.h"
#include "conststr.h"
#include "AHTMemConv.h"
#include "init.h"
#include "AHTURLTools.h" /** defined here **/

/*+--------------------------------------------------------------------+ */
/*| IsHTMLName                                                         | */
/*+--------------------------------------------------------------------+ */

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
    return FALSE;

  strcpy (temppath, path);
  ExtractSuffix (temppath, suffix);

  /* Normalize the suffix */
  i = 0;
  while (suffix[i] != EOS)
    nsuffix[i] = TOLOWER (suffix[i++]);
  nsuffix[i] = EOS;
  if ((strcmp (nsuffix, "html")) &&
      (strcmp (nsuffix, "htm")) &&
      (strcmp (nsuffix, "shtml")))
    return FALSE;
  return TRUE;
}

/*+--------------------------------------------------------------------+ */
/*| IsImageName                                                        | */
/*+--------------------------------------------------------------------+ */

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
    return FALSE;

  strcpy (temppath, path);
  ExtractSuffix (temppath, suffix);

  /* Normalize the suffix */
  i = 0;
  while (suffix[i] != EOS)
    nsuffix[i] = TOLOWER (suffix[i++]);
  nsuffix[i] = EOS;
  if ((strcmp (nsuffix, "gif")) && (strcmp (nsuffix, "xbm")) &&
      (strcmp (nsuffix, "xpm")) && (strcmp (nsuffix, "jpg")) &&
      (strcmp (nsuffix, "png")) && (strcmp (nsuffix, "au")))
    return FALSE;
  return TRUE;
}

/*+--------------------------------------------------------------------+ */
/*| IsTextName                                                         | */
/*+--------------------------------------------------------------------+ */

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
    return FALSE;

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
      (strcmp (nsuffix, "Z")) && (strcmp (nsuffix, "gz")) &&
      (strcmp (nsuffix, "tgz")) && (strcmp (nsuffix, "xpg")) &&
      (strcmp (nsuffix, "xpd")) && (strcmp (nsuffix, "ps")) &&
      (strcmp (nsuffix, "au")))
    return TRUE;
  return FALSE;
}

/*+--------------------------------------------------------------------+ */
/*| IsHTTPPath                                                         | */
/*+--------------------------------------------------------------------+ */

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

/*+--------------------------------------------------------------------+ */
/*| IsWithParameters                                                   | */
/*+--------------------------------------------------------------------+ */

#ifdef __STDC__
boolean             IsWithParameters (char *path)
#else  /* __STDC__ */
boolean             IsWithParameters (path)
char               *path;

#endif /* __STDC__ */
{
  int                 i;

  if ((!path) || (path[0] == EOS))
    return FALSE;

  i = strlen (path) - 1;
  while (i > 0 && path[i--] != '?')
    if (i < 0)
      return FALSE;

  /* There is a parameter */
  return TRUE;
}

/*+--------------------------------------------------------------------+ */
/*| IsW3Path                                                           | */
/*+--------------------------------------------------------------------+ */

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

/*+--------------------------------------------------------------------+ */
/*| IsValidProtocol                                                    | */
/*+--------------------------------------------------------------------+ */

#ifdef __STDC__
boolean             IsValidProtocol (char *path)
#else  /* __STDC__ */
boolean             IsValidProtocol (path)
char               *path;

#endif /* __STDC__ */
{
  if (!strncmp (path, "http:", 5)
      /***|| !strncmp (path, "ftp:", 4)
      || !strncmp (path, "news:", 5)***/)
    return (YES);
  else
    return (NO);
}

/*+--------------------------------------------------------------------+ */
/*| IsValidNormalizeURL  says which URL's may be normalized            | */
/*+--------------------------------------------------------------------+ */

#ifdef __STDC__
boolean             IsValidNormalizeURL (char *path)
#else  /* __STDC__ */
boolean             IsValidNormalizeURL (path)
char               *path;

#endif /* __STDC__ */
{
 if (strchr(path,':') && !strncmp (path, "http:", 5))
   return (YES);
 else
   return (NO);
}


/*+--------------------------------------------------------------------+ */
/*| NormalizeURL provides the new complete and normalized URL or file  | */
/*|            name path and the name of the document.                 | */
/*|            orgName is the original requested name.                 | */
/*|            doc identifies the document which provides the original | */
/*|            name.                                                   | */
/*|            newName is the resulting URL of file name.              | */
/*|            docName is the resulting document name.                 | */
/*+--------------------------------------------------------------------+ */

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
  char                tempname[MAX_LENGTH];
  int                 i;
  char               *ptr;
  char               *basename_ptr;
  int                 basename_flag;
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attrHREF;
  int                 length;

  /* Fix up orgName, by erasing leading and trailing white space */
  if (!newName || !docName)
    return;
  ptr = orgName;
  while (*ptr == ' ' && *ptr++ != EOS) ;
  strcpy (tempname, ptr);
  ptr = strchr (tempname, ' ');
  if (ptr)
    *ptr = EOS;

  /* 
  ** the following block to take into account the BASE element.
  ** This is not very optimized, as this procedure is repeated for
  ** each element which is retrieved. A better way would be to
  ** move this higher up in the function call hierarchy.
  */
  if (IsValidNormalizeURL (tempname) && doc)
    {
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
	      /* 
	      ** Use the base path of the document 
	      ** To do: verify length of the buffer
	      ** length > TtaGetTextAttributeLength (attrHREF) + strlen (orgName) 
	      */
	      TtaGiveTextAttributeValue (attrHREF, basename, &length);

	      /* 
	      ** base and orgName have to be separated by a DIR_SEP 
	      */
	      if (basename[strlen (basename) - 1] != DIR_SEP && tempname[0] != DIR_SEP)
		strcat (basename, DIR_STR);
	    }
	}
      else
	basename[0] = EOS;
    }
  else
    basename[0] = EOS;

  if (basename[0] == EOS)
    {
      /* 
      ** There is no BASE element in that document.
      ** A temporary fix as TtaExtractName does not tolerate a name
      ** ending in /. Here, we reinsert the slash, in order to
      ** parse the name in the following two lines. A bit
      ** redundant and has to be reviewed.  
      */
      if (DocumentURLs[(int) doc])
	{
	  basename_ptr = HTParse (DocumentURLs[(int) doc], "", PARSE_ALL);
	  basename_flag = TRUE;
	}
      else
	{
	  basename_ptr = "";
	  basename_flag = FALSE;
	}
    }
  else
    {
      basename_ptr = HTParse (basename, "", PARSE_ALL);
      basename_flag = TRUE;
    }				/* if-else tempname */

  ptr = HTParse (tempname, basename_ptr, PARSE_ALL);
  if (basename_flag)
    HT_FREE (basename_ptr);
  if (ptr)
    {
      ptr = HTSimplify (&ptr);
      strcpy (newName, ptr);
      HT_FREE (ptr);
    }
  else
    newName[0] = EOS;

  i = strlen (newName) - 1;
  if (i > 0)
    {
      /* 
      ** A temporary fix for an interfacing problem:
      ** TtaExtractName does not tolerate url's finished on DIR_SEP
      */
      ptr = strrchr (newName, DIR_SEP);
      if (ptr)
	ptr++;
      if (ptr && *ptr != EOS)
	strcpy (docName, ptr);
      else			
	/*
	** The docname was not comprised inside the URL, so let's 
	** assign a "noname.html" name :)
	*/
	strcpy (docName, "noname.html");

      /* 
      ** A temporary fix for an interfacing problem:
      ** TtaExtractName does not tolerate url's finished on DIR_SEP
      */
      if (newName[i] == DIR_SEP)
	newName[i] = EOS;
    }
}

/*+--------------------------------------------------------------------+ */
/*| IsSameHost                                                         | */
/*+--------------------------------------------------------------------+ */

#ifdef __STDC__
boolean             IsSameHost (char *url1, char *url2)
#else  /* __STDC__ */
boolean             IsSameHost (url1, url2)
char               *path;

#endif /* __STDC__ */
{
 char *basename_ptr1, *basename_ptr2;
 boolean result;

 basename_ptr1 = HTParse(url1, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);
 basename_ptr2 = HTParse(url2, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);

 if (strcmp (basename_ptr1, basename_ptr2))
   result = NO;
 else
   result = YES;

 HT_FREE(basename_ptr1);
 HT_FREE(basename_ptr2);

 return(result);
}


/*+--------------------------------------------------------------------+ */
/*| AHTMakeRelativeURL                                                | */
/*+--------------------------------------------------------------------+ */

#ifdef __STDC__
char *AHTMakeRelativeName(char *url, char *base_url)
#else  /* __STDC__ */
char *AHTMakeRelativeName(url, base_url)
char url;
char base_url;
#endif /* __STDC__ */
{
 char *base_ptr, *url_ptr;
 char *result;

 /* verify if we are in the same host */

 base_ptr = HTParse(base_url, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);
 url_ptr = HTParse(url, "", PARSE_ACCESS | PARSE_HOST | PARSE_PUNCTUATION);
 
 if (!strcmp (base_ptr, url_ptr)) {
   HT_FREE(base_ptr);
   HT_FREE(url_ptr);

   /* Normalize the URLs */

   base_ptr = HTParse(base_url, "", PARSE_ALL);
   url_ptr = HTParse(url, "", PARSE_ALL);

   /* Use libwww to make relative name */

   result = HTRelative(url_ptr, base_ptr);
   HT_FREE(base_ptr);
   HT_FREE(url_ptr);
 }
 else
   result = (char *) NULL;

 return(result);
}







