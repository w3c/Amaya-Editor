/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * AHTURLTools.c: contains all the functions for testing, manipulating,
 * and normalizing URLs. It also contains a local copy of the libWWW
 * URL parsing functions.
 *
 * Authors: J. Kahan, I. Vatton
 *          R. Guetari: Windows.
 *
 */
#define THOT_EXPORT extern
#include "amaya.h"

#include "init_f.h"
#include "AHTURLTools_f.h"

#define MAX_PRINT_URL_LENGTH 50
typedef struct _HTURI
{
  char *access;		/* Now known as "scheme" */
  char *host;
  char *absolute;
  char *relative;
  char *fragment;
} HTURI;

#ifdef _WINDOWS
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#define stat _stat
#define uint64_t unsigned __int64
#define getpid _getpid
#else /* _WINDOWS */
#if HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  ConvertToLowerCase
  Converts a string to lowercase.
  ----------------------------------------------------------------------*/
void ConvertToLowerCase (char *string)
{
  int i;
 
  if (!string)
    return;

  for (i = 0; string[i] != EOS; i++)
    string[i] = tolower (string[i]);
}

/*----------------------------------------------------------------------
  EscapeChar
  writes the equivalent escape code of a char in a string
  ----------------------------------------------------------------------*/
void EscapeChar (char *string, char c)
{
  unsigned int i;

  i = (unsigned char) c & 0xFF;
  sprintf (string, "%02x", i);
}

/*----------------------------------------------------------------------
  UnEscapeChar
  writes the equivalent hex code to a %xx coded char
  ----------------------------------------------------------------------*/
static char UnEscapeChar (char c)
{
  return  c >= '0' && c <= '9' ?  c - '0'
    : c >= 'A' && c <= 'F' ? c - 'A' + 10
    : c - 'a' + 10;   /* accept small letters just in case */
}

/*----------------------------------------------------------------------
  EscapeURL
  Takes a URL and escapes all protected chars into
  %xx sequences. Also, removes any leading white spaces
  Returns either NULL or a new buffer, which must be freed by the caller
  ----------------------------------------------------------------------*/
char *EscapeURL (const char *url)
{
  char *buffer;
  char *ptr, *server, *param;
  int   buffer_len, par_len = 0;
  int   new_chars, len = 0, url_len;
  int   buffer_free_mem;
  void *status;

  if (url && *url)
    {
      url_len =  strlen (url);
      buffer_free_mem = url_len + 20;
      // a patch for sweetwiki
      server = TtaGetEnvString ("WIKI_SERVER");
      param = TtaGetEnvString ("WIKI_GET_PARAMS");
      if (server)
        len = strlen(server);
      if (len == 0 || param == NULL ||
          strncmp (url, server, len) ||
          // or already included
          url_len < 4 || strncmp (&url[url_len-4], ".jsp", 4))
        // it's not necessary to add these parameters
        par_len = 0;
      else
        par_len = strlen (param);

      buffer_free_mem += par_len;
      buffer = (char *)TtaGetMemory (buffer_free_mem + 2);
      ptr = (char *) url;
      buffer_len = 0;
      while (*ptr)
        {
          switch (*ptr)
            {
              /* put here below all the chars that need to
                 be escaped into %xx */
            case 0x20: /* space */
              /*case 0x26:*/ /* &amp */
            case 0x27: /* antislash */
              new_chars = 3; 
              break;

            default:
              if ((unsigned char )*ptr > 127)
                new_chars = 3;
              else
                new_chars = 1; 
              break;
            }

          /* see if we need extra room in the buffer */
          if (new_chars > buffer_free_mem)
            {
              buffer_free_mem = 20;
              status = TtaRealloc (buffer, sizeof (char) 
                                   * (buffer_len + buffer_free_mem + 2));
              if (status)
                buffer = (char *) status;
              else
                {
                  /* @@ maybe we should do some other behavior here, like
                     freeing the buffer and return a void thing */
                  buffer[buffer_len] = EOS;
                  break;
                }
            }
          /* escape the char */
          if (new_chars == 3)
            {
              buffer[buffer_len] = '%';
              EscapeChar (&buffer[buffer_len+1], *ptr);
            }
          else
            buffer[buffer_len] = *ptr;

          /* update the status */
          buffer_len += new_chars;
          buffer_free_mem -= new_chars;
          /* examine the next char */
          ptr++;

          if (*ptr == EOS && par_len)
            {
              // add parameters
              if (param[0] != '?' && strstr (url, "?") == NULL)
                {
                  // add the missing character
                  buffer[buffer_len++] = '?';
                  buffer_free_mem--;
                }
              ptr = param;
              par_len = 0;
            }
        }
      buffer[buffer_len] = EOS;
    }
  else
    buffer = NULL;

  return (buffer);
}

/*----------------------------------------------------------------------
  EscapeXML
  Takes a string and escapes all protected chars into entity
  sequences.
  Returns either NULL or a new buffer, which must be freed by the caller
  ----------------------------------------------------------------------*/
char *EscapeXML (const char *string)
{
  char *buffer;
  int   buffer_len;
  int   buffer_free_mem;
  char *ptr;
  const char *entity = NULL;
  int   new_chars;
  void *status;

  if (string && *string)
    {
      buffer_free_mem = strlen (string) + 20;
      buffer = (char *)TtaGetMemory (buffer_free_mem + 1);
      ptr = (char *) string;
      buffer_len = 0;

      while (*ptr)
        {
          switch (*ptr)
            {
            case 0x26: /* &amp */
              entity = "&amp;";
              new_chars = sizeof (entity) - 1;	    
              break;
	       
            case '<':  /* &lt; */
              entity = "&lt;";
              new_chars = sizeof (entity) - 1;	    
              break;

            case '>':  /* &gt; */
              entity = "&gt;";
              new_chars = sizeof (entity) - 1;	    
              break;

            case '"':  /* &quote; */
              entity = "&quote;";
              new_chars = sizeof (entity) - 1;	    
              break;

            default:
              new_chars = 1; 
              break;
            }

          /* see if we need extra room in the buffer */
          if (new_chars > buffer_free_mem)
            {
              buffer_free_mem = 20;
              status = TtaRealloc (buffer, sizeof (char) 
                                   * (buffer_len + buffer_free_mem + 1));
              if (status)
                buffer = (char *) status;
              else
                {
                  /* @@ maybe we should do some other behavior here, like
                     freeing the buffer and return a void thing */
                  buffer[buffer_len] = EOS;
                  break;
                }
            }
          /* escape the char */
          if (entity)
            {
              sprintf (&buffer[buffer_len], "%s", entity);
              entity = NULL;
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
void ExplodeURL (char *url, char **proto, char **host, char **dir,
                 char **file)
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
  if ((*curr == used_sep) && (*(curr + 1) == used_sep))
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
  PicTypeToMime
  Converts a Thot PicType into the equivalent MIME type. If no convertion
  is possible, it returns NULL.
  ----------------------------------------------------------------------*/
const char *PicTypeToMIME (PicType contentType)
{
  const char *mime_type;
  
  switch (contentType)
    {
    case xbm_type:
      mime_type ="image/x-xbitmap";
      break;
    case eps_type:
      mime_type ="application/postscript";
      break;
    case xpm_type:
      mime_type ="image/x-xpicmap";
      break;
    case gif_type:
      mime_type ="image/gif";
      break;
    case jpeg_type:
      mime_type ="image/jpeg";
      break;
    case png_type:
      mime_type ="image/png";
      break;
    case svg_type:
      mime_type = AM_SVG_MIME_TYPE;
      break;
    case html_type:
      mime_type = AM_XHTML_MIME_TYPE;
      break;
    case mathml_type:
      mime_type = AM_MATHML_MIME_TYPE;
      break;
    case unknown_type:
    default:
      mime_type = NULL;
    }

  return mime_type;
}

/*----------------------------------------------------------------------
  ImageElement
  Returns the element (image parameter) and URL (url parameter) of an
  image in a docImage document. The user must free the memory associated
  with the url parameter if the function is succesful. 
  If the url parameter is NULL, we won't initialize it.
  Returns TRUE if succesful, FALSE otherwise.
  ----------------------------------------------------------------------*/
ThotBool ImageElement (Document doc, char **url, Element *image)
{
  Element             el, imgEl;
  Attribute           attr, srcAttr;
  AttributeType       attrType;
  int                 length;
  char               *utf8value;

  if (DocumentTypes[doc] != docImage)
    return FALSE;

  /* find the value of the src attribute */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_SRC;
  el = TtaGetRootElement (doc);
  TtaSearchAttribute (attrType, SearchInTree, el, &imgEl, &srcAttr);

  if (!imgEl)
    return FALSE;
  *image = imgEl;

  if (url)
    {
      attr = TtaGetAttribute (imgEl, attrType);
      length = TtaGetTextAttributeLength (srcAttr) + 1;
      utf8value = (char *)TtaGetMemory (length);
      TtaGiveTextAttributeValue (srcAttr, utf8value, &length);
      *url = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                          TtaGetDefaultCharset ());
      TtaFreeMemory (utf8value);
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  DocImageMimeType
  Returns the MIME type of a docImage document.
  ----------------------------------------------------------------------*/
const char *DocImageMimeType (Document doc)
{
  const char *mime_type;
  LoadedImageDesc *pImage;
  PicType type;
  Element image;

  if (DocumentTypes[doc] != docImage)
    return NULL;

  mime_type = NULL;
  if (!IsHTTPPath (DocumentURLs[doc]))
    {
      /* it is a local image */
      if (ImageElement (doc, NULL, &image))
        {
          type = TtaGetPictureType (image);
          mime_type = PicTypeToMIME (type);
        }
    }
  else
    {
      /* find the value of the src attribute */
      pImage = ImageURLs;
      while (pImage != NULL)
        {
          if (pImage->document == doc)
            {
              if (pImage->content_type)
                mime_type = pImage->content_type;
              else if (pImage->elImage && pImage->elImage->currentElement)
                {
                  type = TtaGetPictureType (pImage->elImage->currentElement);
                  mime_type = PicTypeToMIME (type);
                }
              break;
            }  
        }
    }
  return (mime_type);
}

/*----------------------------------------------------------------------
  IsHTMLName                                                         
  returns TRUE if path points to an HTML resource.
  ----------------------------------------------------------------------*/
ThotBool IsHTMLName (const char *path)
{
  char      *temppath;
  char      *suffix;
  char      nsuffix[MAX_LENGTH];
  int       i; 

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);
  i = 0;
  while (suffix[i] != EOS)
    {
      /* Normalize the suffix */
      i = 0;
      while (suffix[i] != EOS && i < MAX_LENGTH -1)
        {
          nsuffix[i] = tolower (suffix[i]);
          i++;
        }
      nsuffix[i] = EOS;
      if (!strcmp (nsuffix, "html") ||
          !strcmp (nsuffix, "htm") ||
          !strcmp (nsuffix, "shtml") ||
          !strcmp (nsuffix, "asp") ||
          !strcmp (nsuffix, "jsp") ||
          !strcmp (nsuffix, "tpl") ||
          !strcmp (nsuffix, "xht") ||
          !strcmp (nsuffix, "xhtm") ||
          !strcmp (nsuffix, "lhtml") ||
          !strcmp (nsuffix, "xhtml"))
        {
          TtaFreeMemory (temppath);
          TtaFreeMemory (suffix);
          return (TRUE);
        }
      else if (!strcmp (nsuffix, "gz"))
        {
          /* take into account compressed files */
          TtaExtractSuffix (temppath, suffix);       
          /* Normalize the suffix */
          i = 0;
          while (suffix[i] != EOS && i < MAX_LENGTH -1)
            {
              nsuffix[i] = tolower (suffix[i]);
              i++;
            }
          nsuffix[i] = EOS;
          TtaFreeMemory (temppath);
          TtaFreeMemory (suffix);
          if (!strcmp (nsuffix, "html") ||
              !strcmp (nsuffix, "htm") ||
              !strcmp (nsuffix, "shtml") ||
              !strcmp (nsuffix, "asp") ||
              !strcmp (nsuffix, "jsp") ||
              !strcmp (nsuffix, "tpl") ||
              !strcmp (nsuffix, "xht") ||
              !strcmp (nsuffix, "xhtm") ||
              !strcmp (nsuffix, "lhtml") ||
              !strcmp (nsuffix, "xhtml"))
            return (TRUE);
          else
            return (FALSE);
        }
      else
        /* check if there is another suffix */
        TtaExtractSuffix (temppath, suffix);
    }
  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (FALSE);
}

/*----------------------------------------------------------------------
  IsMathMLName                                                         
  returns TRUE if path points to an MathML resource.
  ----------------------------------------------------------------------*/
ThotBool IsMathMLName (const char *path)
{
  char        *temppath;
  char        *suffix;
  ThotBool    ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "mml"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "mml"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsSVGName                                                         
  returns TRUE if path points to an SVG resource.
  ----------------------------------------------------------------------*/
ThotBool IsSVGName (const char *path)
{
  char        *temppath;
  char        *suffix;
  ThotBool    ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "svg") || !strcasecmp (suffix, "svgz"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "svg"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsXMLName                                                         
  returns TRUE if path points to an XML resource.
  ----------------------------------------------------------------------*/
ThotBool IsXMLName (const char *path)
{
  char        *temppath;
  char        *suffix;
  ThotBool    ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "xml") ||
      !strcasecmp (suffix, "xht") ||
      !strcasecmp (suffix, "xtd") ||
      !strcasecmp (suffix, "xtl") ||
      !strcmp (suffix, "xhtm") ||
      !strcmp (suffix, "xhtml") ||
      !strcmp (suffix, "smi") ||
      !strcmp (suffix, "xsd") |
      !strcmp (suffix, "xslt") |
      !strcmp (suffix, "xsl") |
      !strcmp (suffix, "zsl"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "xml") ||
          !strcasecmp (suffix, "xht") ||
          !strcasecmp (suffix, "xtd") ||
          !strcasecmp (suffix, "xtl") ||
          !strcmp (suffix, "xhtm") ||
          !strcmp (suffix, "xhtml") ||
          !strcmp (suffix, "xsd") |
          !strcmp (suffix, "xslt") |
          !strcmp (suffix, "xsl") |
          !strcmp (suffix, "smi"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
           IsXMLStruct                                                
  returns TRUE if path points to an XML transformation or schema.
  ----------------------------------------------------------------------*/
ThotBool IsXMLStruct (const char *path)
{
  char        *temppath;
  char        *suffix;
  ThotBool    ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcmp (suffix, "xsd") |
      !strcmp (suffix, "xslt") |
      !strcmp (suffix, "xsl"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcmp (suffix, "xsd") |
          !strcmp (suffix, "xslt") |
          !strcmp (suffix, "xsl"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsXTigerLibrary                                                        
  returns TRUE if path points to an XTiger resource.
  ----------------------------------------------------------------------*/
ThotBool IsXTigerLibrary (const char *path)
{
  char        *temppath;
  char        *suffix;
  ThotBool    ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "xtl"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "xtl"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsXTiger
  returns TRUE if path points to an XTiger resource.
  ----------------------------------------------------------------------*/
ThotBool IsXTiger (const char *path)
{
  char        *temppath;
  char        *suffix;
  ThotBool    ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "xtd"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "xtd"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsResourceName 
  returns TRUE if path points to an undisplayed resource.
  ----------------------------------------------------------------------*/
ThotBool IsResourceName (const char *path)
{
  char               *temppath, *suffix = NULL, *ptr = NULL;
  ThotBool            ret = FALSE;

  temppath = TtaStrdup ((char *)path);
  if (temppath == NULL)
    return FALSE;

  if (!strncmp (temppath, "http://", 7))
    ptr = &temppath[7];
  else if (!strncmp (temppath, "http://", 8))
    ptr = &temppath[8];
  else
    ptr = temppath;
  if (ptr != temppath)
    {
      // skip the host name
      while (*ptr != EOS && *ptr != '/')
        ptr++;
    }
  if (*ptr == EOS)
    // no name
    return FALSE;

  suffix = (char *)TtaGetMemory (strlen (ptr) + 1);
  TtaExtractSuffix (ptr, suffix);
  if (*suffix == EOS || *suffix == '/' ||
      strstr (suffix, "#") || strstr (suffix, "?") ||
      !strncasecmp (suffix, "htm", 3) ||
      !strncasecmp (suffix, "shtm", 4) ||
      !strncasecmp (suffix,"xhtm", 4) ||
      !strncasecmp (suffix, "php", 3) ||
      !strncasecmp (suffix, "mml", 3) ||
      !strncasecmp (suffix, "svg", 3) ||
      !strncasecmp (suffix, "xml", 3))
    ret = FALSE;
  else
    ret = TRUE;
      
  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return ret;
}

/*----------------------------------------------------------------------
  IsUndisplayedName 
  returns TRUE if path points to an undisplayed resource.
  ----------------------------------------------------------------------*/
ThotBool IsUndisplayedName (const char *path)
{
  char                *temppath;
  char                *suffix;
  ThotBool            ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "exe") ||
      !strcasecmp (suffix, "zip") ||
      !strcasecmp (suffix, "ppt") ||
      !strcasecmp (suffix, "pdf") ||
      !strcasecmp (suffix, "ps")  ||
      !strcasecmp (suffix, "eps") ||
      !strcasecmp (suffix, "tar") ||
      !strcasecmp (suffix, "tgz") ||
      !strcasecmp (suffix, "ddl") ||
      !strcasecmp (suffix, "deb") ||
      !strcasecmp (suffix, "dmg") ||
      !strcasecmp (suffix, "mpg") ||
      !strcasecmp (suffix, "mpeg") ||
      !strcasecmp (suffix, "rpm") ||
      !strcasecmp (suffix, "wmv") ||
      !strcasecmp (suffix, "wma") ||
      !strcasecmp (suffix, "doc") ||
      !strcasecmp (suffix, "odt") ||
      !strcasecmp (suffix, "o"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "exe") ||
          !strcasecmp (suffix, "zip") ||
          !strcasecmp (suffix, "ppt") ||
          !strcasecmp (suffix, "pdf") ||
          !strcasecmp (suffix, "ps")  ||
          !strcasecmp (suffix, "eps") ||
          !strcasecmp (suffix, "tar") ||
          !strcasecmp (suffix, "ddl") ||
          !strcasecmp (suffix, "dmg") ||
          !strcasecmp (suffix, "mpg") ||
          !strcasecmp (suffix, "mpeg") ||
          !strcasecmp (suffix, "wmv") ||
          !strcasecmp (suffix, "wma") ||
          !strcasecmp (suffix, "doc") ||
          !strcasecmp (suffix, "odt") ||
          !strcasecmp (suffix, "o"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsCSSName                                                         
  returns TRUE if path points to an XML resource.
  ----------------------------------------------------------------------*/
ThotBool IsCSSName (const char *path)
{
  char                *temppath;
  char                *suffix;
  ThotBool            ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "css"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "css"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  MultipleBookmarks
  returns the value of the multiple bookmarks environment variable.
  (TRUE if enabled).
  ----------------------------------------------------------------------*/
ThotBool MultipleBookmarks (void)
{
  ThotBool multiple_bm;

  TtaGetEnvBoolean ("DISABLE_MULTIPLE_BM", &multiple_bm);

  return (multiple_bm == FALSE);
}

/*----------------------------------------------------------------------
  IsRDFName                                                         
  returns TRUE if path points to an RDF resource.
  ----------------------------------------------------------------------*/
ThotBool IsRDFName (const char *path)
{
  char                *temppath;
  char                *suffix;
  ThotBool            ret;

  /* temporarily disabling this function */
  if (!MultipleBookmarks ())
    return (FALSE);

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  if (!strcasecmp (suffix, "rdf"))
    ret = TRUE;
  else if (!strcmp (suffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      if (!strcasecmp (suffix, "rdf"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;
   
  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsImageName                                
  returns TRUE if path points to an image resource.
  ----------------------------------------------------------------------*/
ThotBool IsImageName (const char *path)
{
  char                *temppath;
  char                *suffix;
  char                nsuffix[MAX_LENGTH];
  int                 i;
  ThotBool            ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

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
    ret = TRUE;
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsImageType                                
  returns TRUE if type points to an image resource.
  ----------------------------------------------------------------------*/
ThotBool IsImageType (const char *type)
{
  char                *temptype;
  int                 i;
  ThotBool            ret;

  if (!type)
    return (FALSE);

  temptype = TtaStrdup ((char *)type);
  /* Normalize the type */
  i = 0;
  while (temptype[i] != EOS)
    {
      temptype[i] = tolower (temptype[i]);
      i++;
    }
  if (!strncmp (temptype, "image/", sizeof ("image/") - 1))
    i = sizeof ("image/") - 1;
  else
    i = 0;
  if (!strcmp (&temptype[i], "gif") ||
      !strcmp (&temptype[i], "x-xbitmap") ||
      !strcmp (&temptype[i], "x-xpixmap") ||
      !strcmp (&temptype[i], "jpeg") ||
      !strcmp (&temptype[i], "png"))
    ret = TRUE;
  else
    ret = FALSE;
  TtaFreeMemory (temptype);
  return (ret);
}

/*----------------------------------------------------------------------
  IsTextName                                                         
  ----------------------------------------------------------------------*/
ThotBool IsTextName (const char *path)
{
  char                *temppath;
  char                *suffix;
  char                nsuffix[MAX_LENGTH];
  int                 i;
  ThotBool            ret;

  if (!path)
    return (FALSE);

  temppath = TtaStrdup ((char *)path);
  suffix = (char *)TtaGetMemory (strlen (path) + 1);
  TtaExtractSuffix (temppath, suffix);

  /* Normalize the suffix */
  i = 0;
  while (suffix[i] != EOS && i < MAX_LENGTH -1)
    {
      nsuffix[i] = tolower (suffix[i]);
      i++;
    }
  nsuffix[i] = EOS;

  if (!strcmp (nsuffix, "txt") ||
      !strcmp (nsuffix, "dtd"))
    ret = TRUE;
  else if (!strcmp (nsuffix, "gz"))
    {
      /* take into account compressed files */
      TtaExtractSuffix (temppath, suffix);       
      /* Normalize the suffix */
      i = 0;
      while (suffix[i] != EOS && i < MAX_LENGTH -1)
        {
          nsuffix[i] = tolower (suffix[i]);
          i++;
        }
      nsuffix[i] = EOS;
      if (!strcmp (nsuffix, "txt") ||
          !strcmp (nsuffix, "dtd"))
        ret = TRUE;
      else
        ret = FALSE;
    }
  else
    ret = FALSE;

  TtaFreeMemory (temppath);
  TtaFreeMemory (suffix);
  return (ret);
}

/*----------------------------------------------------------------------
  IsHTTPPath                                     
  returns TRUE if path is in fact an http URL.
  ----------------------------------------------------------------------*/
ThotBool IsHTTPPath (const char *path)
{
  if (!path)
    return FALSE;

  if ((!strncmp (path, "http:", 5) != 0)
#ifdef SSL
      || !strncmp (path, "https:", 6)
#endif /* SSL */
      || !strncmp (path, "ftp:", 4)
      || !strncmp (path, "internal:", 9))
    return TRUE;
  return FALSE;
}

/*----------------------------------------------------------------------
  IsWithParameters                           
  returns TRUE if url has a concatenated query string.
  ----------------------------------------------------------------------*/
ThotBool IsWithParameters (const char *url)
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
ThotBool IsW3Path (const char *path)
{
  if (path == NULL)
    return FALSE;
  if (strncmp (path, "http:", 5)   && 
      strncmp (path, "https:", 6)  &&
      strncmp (path, "ftp:", 4)    &&
      strncmp (path, "telnet:", 7) && 
      strncmp (path, "wais:", 5)   &&
      strncmp (path, "news:", 5)   && 
      strncmp (path, "gopher:", 7) &&
      strncmp (path, "mailto:", 7) && 
      strncmp (path, "archie:", 7))
    return FALSE;
  return TRUE;
}

/*----------------------------------------------------------------------
  IsFilePath                                           
  returns TRUE if path is in fact a URL.
  ----------------------------------------------------------------------*/
ThotBool IsFilePath (const char *path)
{
  if (strncmp (path, "file:", 5))
    return FALSE;
  return TRUE;
}

/*----------------------------------------------------------------------
  IsValidProtocol                                                    
  returns true if the url protocol is supported by Amaya.
  ----------------------------------------------------------------------*/
ThotBool IsValidProtocol (const char *url)
{
  if (!strncmp (url, "http:", 5)
#ifdef SSL
      || !strncmp (url, "https:", 6)
#endif /* SSL */
      || !strncmp (url, "internal:", 9)
      || !strncmp (url, "ftp:", 4))
    /* experimental */
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
char  *GetBaseURL (Document doc)
{
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char               *ptr, *basename, *utf8path;
  char                documentname[MAX_LENGTH];
  int                 length;
  ThotBool            hasDocBase;

  if (doc == 0 || !DocumentURLs[doc])
    return NULL;
  /* the other functions expect basename to have no more than MAX_LENGTH chars */
  basename = (char *)TtaGetMemory (MAX_LENGTH);
  basename[0] = EOS;
  length = MAX_LENGTH -1;
  hasDocBase = FALSE;

  /* If the document has a base URL, it has a priority over the headers. */
  /*  @@ We need to do this too when we support XML:base */

  /* is it a HTML document ? */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    /* it's a HTML document */
    {
      /* get the document element */
      el = TtaGetMainRoot (doc);
      /* search the BASE element */
      elType.ElTypeNum = HTML_EL_HEAD;
      el = TtaSearchTypedElement (elType, SearchForward, el);
      if (el)
        /* there is a HEAD element */
        {
          /* look for a BASE element within the HEAD */
          elType.ElTypeNum = HTML_EL_BASE;
          el = TtaSearchTypedElement (elType, SearchInTree, el);
        }
      if (el)
        {
          /*  The document has a BASE element. Get the HREF attribute of the
              BASE element */
          hasDocBase = TRUE;
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = HTML_ATTR_HREF_;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              /* Use the base path of the document */
              TtaGiveTextAttributeValue (attr, basename, &length);
              utf8path = (char *)TtaConvertMbsToByte ((unsigned char *)basename,
                                                       TtaGetDefaultCharset ());
              if (IsW3Path (DocumentURLs[doc]) && utf8path[0] == '/')
                NormalizeURL (utf8path, 0, basename, documentname, DocumentURLs[doc]);
              else
                strncpy (basename, utf8path, MAX_LENGTH-1);
              TtaFreeMemory (utf8path);
            }
        }
    }

  /* there was no BASE. Do we have a location header? */
  if (!hasDocBase && DocumentMeta[doc] && DocumentMeta[doc]->full_content_location
      && DocumentMeta[doc]->full_content_location[0] != EOS)
    {
      strncpy (basename, DocumentMeta[doc]->full_content_location, MAX_LENGTH-1);
      basename[MAX_LENGTH-1] = EOS;
      length = strlen (basename);
    }

  if (basename[0] != EOS)
    {
      /* base and orgName have to be separated by a DIR_SEP */
      length--;
      if (basename[0] != EOS && basename[length] != URL_SEP &&
          basename[length] != DIR_SEP) 
        /* verify if the base has the form "protocol://server:port" */
        {
          ptr = AmayaParseUrl (basename, "", AMAYA_PARSE_ACCESS |
                               AMAYA_PARSE_HOST |
                               AMAYA_PARSE_PUNCTUATION);
          if (ptr && !strcmp (ptr, basename))
            {
              /* it has this form, complete it by adding a URL_STR  */
              if (strchr (basename, DIR_SEP))
                strcat (basename, DIR_STR);
              else
                strcat (basename, URL_STR);
              length++;
            }
          else if (!ptr || ptr[0] == EOS)
            {
              /* no host was detected, we may have a relative URL. We test
                 if it begins with a URL_SEP, DIR_SEP or period. If yes, it's
                 relative. */
              if (! (basename[0] == '.' || basename[0] == URL_SEP 
                     || basename[0] == DIR_SEP))
                basename[0] = EOS;
            }
          if (ptr)
            TtaFreeMemory (ptr);
        }
    }

  /* there was no base element and no location header, we use the DocumentURL  */
  if (basename[0] == EOS)
    {
      strncpy (basename, DocumentURLs[doc], MAX_LENGTH-1);
      basename[MAX_LENGTH-1] = EOS;
    }
  
  /* Remove anything after the last DIR_SEP char. If no such char is found,
   * then search for the first ":" char, hoping that what's before that is a
   * protocol. If found, end the string there. If neither char is found,
   * then discard the whole base element.
   */
  length = strlen (basename) - 1;
  /* search for the last DIR_SEP char */
  while (length >= 0  && basename[length] != URL_SEP  && basename[length] != DIR_SEP)
    length--;
  if (length > 1 && basename[length-1] == URL_SEP && basename[length-2] == ':')
    return (basename);
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
char *GetLocalPath (Document doc, char  *url)
{
  char     *ptr;
  char     *n;
  char     *documentname;
  char      url_sep;
  int       len;
  ThotBool  noFile;

  if (url)
    {
      /* check whether the file name exists */
      len = strlen (url) - 1;
      if (IsW3Path (url))
        url_sep = URL_SEP;
      else 
        url_sep = DIR_SEP;
      noFile = (url[len] == url_sep);
      if (noFile)
        url[len] = EOS;
      ptr = (char *)TtaGetMemory (MAX_LENGTH);
      documentname = (char *)TtaGetMemory (MAX_LENGTH);
      TtaExtractName (url, ptr, documentname);
      sprintf (ptr, "%s%s%d%s", TempFileDirectory, DIR_STR, doc, DIR_STR);
      if (!TtaCheckDirectory (ptr))
        /* directory did not exist */
        TtaMakeDirectory (ptr);

      if (doc == 0)
        {
          n = strrchr (documentname, '.');
          if (n)
            *n = EOS;
          if (documentname[0] == EOS)
            strcpy (documentname, "noname");
          n = GetTempName (ptr, documentname);
          TtaFreeMemory (ptr);
          ptr = n;
        }
      else
        {
          /* don't include the query string within document name */
          n = strrchr (documentname, '?');
          if (n)
            *n = EOS;
          /* don't include ':' within document name */
          n = strchr (documentname, ':');
          if (n)
            *n = EOS;
          /* if after all this operations document name
             is empty, let's use noname.html instead */
          if (documentname[0] == EOS)
            strcat (ptr, "noname.html");
          else
            strcat (ptr, documentname);
        }
      TtaFreeMemory (documentname);
      /* substitute invalid chars in file names by a _ */
      n = strrchr(ptr,DIR_SEP);
      while (*n)
        {
          if (*n == '*' || *n == ',')
            *n = '_';
          if ((unsigned char)*n >= 0x80) /* avoid non-ASCII */
            *n = 'A' + ((unsigned char)*n % 26);
          n++;
        }
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
void ExtractTarget (char *aName, char *target)
{
  long int    lg, i;
  char  *ptr;
  char  *oldptr;

  if (!target || !aName)
    /* bad target */
    return;

  target[0] = EOS;
  lg = strlen (aName);
  if (lg)
    {
      /* the name is not empty */
      oldptr = ptr = &aName[0];
      do
        {
          ptr = strrchr (oldptr, '#');
          if (ptr)
            oldptr = &ptr[1];
        }
      while (ptr);

      i = (long int) (oldptr) - (long int) (aName);	/* name length */
      if (i > 1)
        {
          aName[i - 1] = EOS;
          if (i != lg)
            strcpy (target, oldptr);
        }
    }
}

/*----------------------------------------------------------------------
  RemoveNewLines (text)
  Removes any '\n' chars that are found in text. 
  Returns TRUE if it did the operation, FALSE otherwise.
  ----------------------------------------------------------------------*/
ThotBool RemoveNewLines (char *text)
{
  ThotBool   change = FALSE;
  char      *src;
  char      *dest;

  src = text;
  dest = text;

  /* remove any preceding whitespace */
  while (*src && *src == ' ')
    {
      src++;
      change = 1;
    }
  
  while (*src)
    {
      switch (*src)
        {
        case '\n':
          /* don't copy the newline */
          change = 1;
          break;
        default:
          *dest = *src;
          dest++;
          break;
        }
      src++;
    }
  /* copy the last EOS char */
  *dest = *src;

  return (change);
}

/*----------------------------------------------------------------------
  CleanCopyFileURL
  Copies a file url from a src string to destination string.
  convertion says which type of convertion (none, %xx, URL_SEP into DIR_SEP
  we want to do).
  ----------------------------------------------------------------------*/
static void CleanCopyFileURL (char *dest, char *src,
                              ConvertionType convertion)
{
  while (*src)
    {
      switch (*src)
        {
#ifdef _WINDOWS
        case URL_SEP:
          /* make DIR_SEP transformation */
          if (convertion & AM_CONV_URL_SEP)
            *dest = DIR_SEP;
          else
            *dest = *src;
          dest++;
          src++;
          break;
#endif /* _WINDOWS */

        case '%':
          if (convertion & AM_CONV_PERCENT)
            {
              /* (code adapted from libwww's HTUnEscape function */
              src++;
              if (*src != EOS)
                {
                  *dest = UnEscapeChar (*src) * 16;
                  src++;
                }
              if (*src != EOS)
                {
                  *dest = *dest + UnEscapeChar (*src);
                  src++;
                }
              dest++;
            }
          else
            {
              *dest = *src;
              dest++;
              src++;
            }
          break;

        default:
          *dest = *src;
          dest++;
          src++;
          break;
        }
    }
  /* copy the EOS char */
  *dest = *src;
}

/*----------------------------------------------------------------------
  NormalizeURL
  normalizes orgName according to a base associated with doc, and
  following the standard URL format rules.
  if doc is < 0, use as a base the URL of the document that contains
  (or contained) the elements that are now in the copy/cut buffer.
  if doc is 0 and otherPath not NULL, normalizes orgName according to this
  other path.
  The function returns the new complete and normalized URL 
  or file name path (newName) and the name of the document (docName).        
  N.B. If the function can't find out what's the docName, it assigns
  the name "noname.html".
  ----------------------------------------------------------------------*/
void NormalizeURL (char *orgName, Document doc, char *newName,
                   char *docName, const char *otherPath)
{
  char          *basename;
  char           tempOrgName[MAX_LENGTH];
  char          *ptr;
  char           used_sep;
  int            length;
  ThotBool       check;

#ifdef _WINDOWS
  int ndx;
#endif /* _WINDOWS */

  if (!newName || !docName)
    return;

  if (doc < 0)
    basename = TtaStrdup (SavedDocumentURL);
  else if (doc > 0)
    basename = GetBaseURL (doc);
  else if (otherPath)
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
  while ((*ptr == SPACE || *ptr == EOL) && *ptr++ != EOS);
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
      docName[0] = EOS;
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

  /* remove extra dot (which dot???) */
  /* ugly, but faster than a strcmp */
  if (tempOrgName[length] == '.'
      && (length == 0 || tempOrgName[length-1] != '.'))
    tempOrgName[length] = EOS;

  if (IsW3Path (tempOrgName))
    {
      /* the name is complete, go to the Sixth Step */
      strcpy (newName, tempOrgName);
      SimplifyUrl (&newName);
      /* verify if the URL has the form "protocol://server:port" */
      ptr = AmayaParseUrl (newName, "", AMAYA_PARSE_ACCESS |
                           AMAYA_PARSE_HOST |
                           AMAYA_PARSE_PUNCTUATION);
      if (ptr && !strcmp (ptr, newName))
        /* it has this form, we complete it by adding a DIR_STR  */
        strcat (newName, URL_STR);

      if (ptr)
        TtaFreeMemory (ptr);
    }
  else if (basename == NULL)
    /* the name is complete, go to the Sixth Step */
    strcpy (newName, tempOrgName);
  else
    {
      /* Calculate the absolute URL, using the base or document URL */
#ifdef _WINDOWS
      if (!IsW3Path (basename))
        {
          length = strlen (tempOrgName);
          for (ndx = 0; ndx < length; ndx++)
            if (tempOrgName[ndx] == '/')
              tempOrgName[ndx] = '\\';
        }
#endif /* _WINDOWS */
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
          /* docname was not comprised inside the URL, so let's */
          /* assign the default ressource name */
          strcpy (docName, "noname.html");
        }
      else
        { /* docname is comprised inside the URL */
          while (length >= 0 && newName[length] != URL_SEP &&
                 newName[length] != DIR_SEP)
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
ThotBool IsSameHost (const char *url1, const char *url2)
{
  char          *basename_ptr1, *basename_ptr2;
  ThotBool       result;

  basename_ptr1 = AmayaParseUrl (url1, "",
                                 AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);
  basename_ptr2 = AmayaParseUrl (url2, "",
                                 AMAYA_PARSE_ACCESS | AMAYA_PARSE_HOST | AMAYA_PARSE_PUNCTUATION);

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
ThotBool HasKnownFileSuffix (const char *path)
{
  char       *root;
  char        temppath[MAX_LENGTH];
  char        suffix[MAX_LENGTH];

  if (!path || path[0] == EOS || path[strlen(path)] == DIR_SEP)
    return (FALSE);

  root = AmayaParseUrl(path, "", AMAYA_PARSE_PATH | AMAYA_PARSE_PUNCTUATION);

  if (root) 
    {
      strcpy (temppath, root);
      TtaFreeMemory (root);
      /* Get the suffix */
      TtaExtractSuffix (temppath, suffix); 

      if( suffix[0] == EOS)
        /* no suffix */
        return (FALSE);

      /* Normalize the suffix */
      ConvertToLowerCase (suffix);

      if (!strcmp (suffix, "gz"))
        /* skip the compressed suffix */
        {
          TtaExtractSuffix (temppath, suffix);
          if(suffix[0] == EOS)
            /* no suffix */
            return (FALSE);
          /* Normalize the suffix */
          ConvertToLowerCase (suffix);
        }

      if (strcmp (suffix, "gif") &&
          strcmp (suffix, "xbm") &&
          strcmp (suffix, "xpm") &&
          strcmp (suffix, "jpg") &&
          strcmp (suffix, "pdf") &&
          strcmp (suffix, "png") &&
          strcmp (suffix, "tgz") &&
          strcmp (suffix, "xpg") &&
          strcmp (suffix, "xpd") &&
          strcmp (suffix, "ps") &&
          strcmp (suffix, "au") &&
          strcmp (suffix, "html") &&
          strcmp (suffix, "htm") &&
          strcmp (suffix, "shtml") &&
          strcmp (suffix, "xht") &&
          strcmp (suffix, "xhtm") &&
          strcmp (suffix, "xhtml") &&
          strcmp (suffix, "txt") &&
          strcmp (suffix, "css") &&
          strcmp (suffix, "eps"))
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
void ChopURL (char *outputURL, const char *inputURL)
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
static void scan (char *name, HTURI *parts)
{
  char *   p;
  char *   after_access = name;

  memset (parts, 0, sizeof (HTURI));
  /* Look for fragment identifier */
  if ((p = strchr(name, '#')) != NULL)
    {
      *p++ = EOS;
      parts->fragment = p;
    }
    
  for (p=name; *p; p++)
    {
      if (*p == URL_SEP || *p == DIR_SEP || *p == '#' || *p == '?')
        break;
      if (*p == ':')
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
char   *AmayaParseUrl (const char *aName, const char *relatedName, int wanted)
{
  char      *return_value;
  char       result[MAX_LENGTH];
  char       name[MAX_LENGTH];
  char       rel[MAX_LENGTH];
  char      *p, *access;
  HTURI      given, related;
  int        len, l;
  char       used_sep;
  const char*used_str;

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
  rel[0] = EOS;
  strncpy (name, aName, MAX_LENGTH - 1);
  name[MAX_LENGTH - 1] = EOS;
  if (relatedName != NULL)
    {
      strncpy (rel, relatedName, MAX_LENGTH - 1);
      rel[MAX_LENGTH - 1] = EOS;
    }
  
  scan (name, &given);
  scan (rel,  &related); 
  access = given.access ? given.access : related.access;
  if (wanted & AMAYA_PARSE_ACCESS)
    if (access)
      {
        strcat (result, access);
        if (wanted & AMAYA_PARSE_PUNCTUATION)
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
    if (given.host || related.host)
      {
        if (wanted & AMAYA_PARSE_PUNCTUATION)
          strcat (result, "//");
        strcat (result, given.host ? given.host : related.host);
      }
  
  if (given.host && related.host)
    /* If different hosts, inherit no path. */
    if (strcmp (given.host, related.host) != 0)
      {
        related.absolute = 0;
        related.relative = 0;
        related.fragment = 0;
      }
  
  len = MAX_LENGTH - 1 - strlen (result);
  if (wanted & AMAYA_PARSE_PATH)
    {
      if (given.absolute)
        {
          /* All is given */
          if (wanted & AMAYA_PARSE_PUNCTUATION)
            {
              strcat (result, used_str);
              len--;
            }
          l = strlen (given.absolute);
          if (l <= len)
            {
              strcat (result, given.absolute);
              len -= l;
            }
        }
      else if (related.absolute)
        {
          /* Adopt path not name */
          strcat (result, used_str);
          len--;
          l = strlen (related.absolute);
          if (l <= len)
            {
              strcat (result, related.absolute);
              len -= l;
            }
          if (given.relative)
            {
              /* Search part? */
              p = strchr (result, '?');
              if (!p)
                p = result + strlen(result) - 1;
              for (; *p != used_sep; p--);	/* last / */
              /* Remove filename */
              p[1] = EOS;
              /* Add given one */
              l = strlen (given.relative);
              if (l <= len)
                {
                  strcat (result, given.relative);
                  len -= l;
                }
           }
        }
      else if (given.relative)
        {
        /* what we've got */
          l = strlen (given.relative);
          if (l <= len)
            {
              strcat (result, given.relative);
              len -= l;
            }
        }
      else if (related.relative)
        {
          l = strlen (related.relative);
          if (l <= len)
            {
              strcat (result, related.relative);
              len -= l;
            }
        }
      else
        {
          /* No inheritance */
          strcat (result, used_str);
          len--;
        }
    }
  
  if (wanted & AMAYA_PARSE_ANCHOR)
    if (len && (given.fragment || related.fragment))
      {
        if (given.absolute && given.fragment)
          {
            /*Fixes for relURLs...*/
            if (wanted & AMAYA_PARSE_PUNCTUATION)
              {
                strcat (result, "#");
                len --;
              }
                l = strlen (given.fragment);
                if (l <= len)
                  strcat (result, given.fragment);
          }
        else if (given.absolute || given.fragment)
          {
            if (wanted & AMAYA_PARSE_PUNCTUATION)
              {
                strcat (result, "#");
                len--;
              }
            if (given.fragment)
              {
                l = strlen (given.fragment);
                if (l <= len)
                  strcat (result, given.fragment);
               }
            else
              {
                l = strlen (given.fragment);
                if (l <= len)
                  strcat (result, related.fragment);
              }
          }
      }
  return_value = TtaStrdup (result);
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
static char *HTCanon (char **filename, char *host)
{
  char   *newname = NULL;
  char    used_sep;
  char   *path;
  char   *strptr;
  char   *port;
  char   *access = host-3;
  
  if (*filename && strchr (*filename, URL_SEP))
    used_sep = URL_SEP;
  else
    used_sep = DIR_SEP;
  
  while (access > *filename && *(access - 1) != used_sep) /* Find access method */
    access--;
  if ((path = strchr (host, used_sep)) == NULL)	        /* Find path */
    path = host + strlen (host);
  if ((strptr = strchr (host, '@')) != NULL && strptr < path)	   /* UserId */
    host = strptr;
  if ((port = strchr (host, ':')) != NULL && port > path)   /* Port number */
    port = NULL;

  strptr = host;				    /* Convert to lower-case */
  while (strptr < path)
    {
      *strptr = tolower (*strptr);
      strptr++;
    }
    
  /* Does the URL contain a full domain name? This also works for a
     numerical host name. The domain name is already made lower-case
     and without a trailing dot. */
  {
    char  *dot = port ? port : path;
    if (dot > *filename && *--dot == '.')
      {
        char  *orig = dot;
        char  *dest = dot + 1;
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
              char  *orig = port; 
              char  *dest = port + 1;
              while ((*orig++ = *dest++));
            }
        }
      else if ((!strncmp (access, "http", 4)   &&
                (*(port + 1) == '8'                    && 
                 *(port+2) == '0'                       && 
                 (*(port+3) == used_sep || !*(port + 3))))       ||
               (!strncmp (access, "gopher", 6) &&
                (*(port+1) == '7'                      && 
                 *(port+2) == '0'                       && 
                 (*(port+3) == used_sep || !*(port+3))))         ||
               (!strncmp (access, "ftp", 3)    &&
                (*(port+1) == '2'                      && 
                 *(port + 2) == '1'                     && 
                 (*(port+3) == used_sep || !*(port+3))))) {
        if (!newname)
          {
            char  *orig = port; 
            char  *dest = port + 3;
            while((*orig++ = *dest++));
            /* Update path position, Henry Minsky */
            path -= 3;
          }
      }
      else if (newname)
        strncat (newname, port, (int) (path - port));
    }

  if (newname)
    {
      char  *newpath = newname + strlen (newname);
      strcat (newname, path);
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
void SimplifyUrl (char **url)
{
  char   *path;
  char   *access;
  char   *newptr; 
  char   *p;
  char   *orig, *dest, *end;

  char      used_sep;
  ThotBool ddot_simplify; /* used to desactivate the double dot simplifcation:
                             something/../ simplification in relative URLs when they start with a ../ */

  if (!url || !*url)
    return;

  if (strchr (*url, URL_SEP))
    used_sep = URL_SEP;
  else
    used_sep = DIR_SEP;

  /* should we simplify double dot? */
  path = *url;
  if (*path == '.' && *(path + 1) == '.')
    ddot_simplify = FALSE;
  else
    ddot_simplify = TRUE;

  /* Find any scheme name */
  if ((path = strstr (*url, "://")) != NULL)
    {
      /* Find host name */
      access = *url;
      while (access < path && (*access = tolower (*access)))
        access++;
      path += 3;
      while ((newptr = strstr (path, "://")) != NULL)
        /* For proxies */
        path = newptr + 3;
      /* We have a host name */
      path = HTCanon (url, path);
    }
  else if ((path = strstr (*url, ":/")) != NULL)
    path += 2;
  else
    path = *url;
#ifndef _WINDOWS
  if (*path == used_sep && *(path+1) == used_sep)
    /* Some URLs start //<foo> */
    path += 1;
  else
#endif /* _WINDOWS */
  if (IsFilePath (path))
    {
      /* doesn't need to do anything more */
      return;
    }
  else if (!strncmp (path, "news:", 5))
    {
      newptr = strchr (path+5, '@');
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

  /* remove initial multiple /'s */
  p = path - 1;
  if (path != *url && *p == used_sep && *path == used_sep)
    {
      while (*path == used_sep)
        {
          orig = path;
          dest = path + 1;
          while ((*orig++ = *dest++));  /* Remove multiple /'s */
          end = orig-1;
        }
    }

  if (path)
    {
      if (!((end = strchr (path, ';')) || (end = strchr (path, '?')) ||
            (end = strchr (path, '#'))))
        end = path + strlen (path);
      
      /* Parse string second time to simplify */
      p = path;
      while (p < end)
        {
          /* if we're pointing to a char, it's safe to reactivate the 
             ../ convertion */
          if (!ddot_simplify && *p != '.' && *p != used_sep)
            ddot_simplify = TRUE;

          if (*p == used_sep)
            {
              if (p > *url && *(p+1) == '.' && (*(p+2) == used_sep || !*(p+2)))
                {
                  orig = p + 1;
                  dest = (*(p+2) != used_sep) ? p+2 : p+3;
                  while ((*orig++ = *dest++)); /* Remove a used_sep and a dot*/
                  end = orig - 1;
                }
              else if (ddot_simplify && *(p+1) == '.' && *(p+2) == '.' 
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
              else if (p != path && *(p+1) == used_sep)
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
  if (*path == used_sep && *(path+1) == '.' && *(path+2) == '.' 
      && (!*(path+3) || *(path+3) == used_sep))
    *(path+1) = EOS;
  return;
}


/*----------------------------------------------------------------------
  NormalizeFile normalizes local names.
  convertion is AM_CONV_NONE or AM_CONV_ALL
  Return TRUE if target and src differ.                           
  ----------------------------------------------------------------------*/
ThotBool NormalizeFile (char *src, char *target, ConvertionType convertion)
{
#ifndef _WINDOWS
  char             *s;
  int               i;
#endif /* _WINDOWS */
  int               start_index; /* the first char that we'll copy */
  ThotBool          change;

  change = FALSE;
  start_index = 0;

  if (!src || src[0] == EOS)
    {
      target[0] = EOS;
      return FALSE;
    }

  /* @@ do I need file: or file:/ here? */
  if (strncmp (src, "file:", 5) == 0)
    {
      /* remove the prefix file: */
      start_index += 5;
   
      /* remove the localhost prefix */
      if (strncmp (&src[start_index], "//localhost/", 12) == 0)
        start_index += 11;
       
      /* remove the first two slashes in / / /path */
      if (src[start_index] == '/' && src[start_index + 1] == '/')
        start_index +=2;
#ifdef _WINDOWS
      /* remove any extra slash before the drive name */
      if (src[start_index] == '/' && src[start_index+2] == ':')
        start_index++;
#endif /* _WINDOWS */

      if (src[start_index] == EOS)
        /* if there's nothing afterwards, add a DIR_STR */
        strcpy (target, DIR_STR);
      else
        /* as we're inside a file: URL, we'll apply all the convertions
           we know */
        CleanCopyFileURL (target, &src[start_index], AM_CONV_ALL);

      change = TRUE;
    }
  else if (convertion != AM_CONV_NONE)
    {
      /* we are following a "local" relative link, we do all the
         convertions except for the HOME_DIR ~ one */
      CleanCopyFileURL (target, src, convertion);
    }
#ifdef _WINDOWS
#ifdef IV
  else if (src[0] == DIR_SEP && src[1] == DIR_SEP)
    {
      s = getenv ("HOMEDRIVE");
      strcpy (target, s);
      i = strlen (target);
      strcpy (&target[i], &src[1]);
      change = TRUE;	    
    }
#endif /* IV */
#else /* _WINDOWS */
  else if (src[0] == '~')
    {
      /* it must be a URL typed in a text input field */
      /* do the HOME_DIR ~ substitution */
      s = TtaGetEnvString ("HOME");
      strcpy (target, s);
      i = strlen (target);
      strcpy (&target[i], &src[1]);
      change = TRUE;
    }
#endif /* _WINDOWS */
  else
    /* leave it as it is */
    strcpy (target, src);
   
  /* remove /../ and /./ */
  SimplifyUrl (&target);
  if (!change)
    change = (strcmp (src, target) != 0);
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
char      *MakeRelativeURL (const char *aName, const char *relatedName)
{
  char        *return_value;
  char         result[MAX_LENGTH];
  const char  *p;
  const char  *q;
  const char  *after_access;
  const char  *last_slash = NULL;
  int          slashes, levels, len;
#ifdef _WINDOWS
  int ndx;
#endif /* _WINDOWS */

  if (aName == NULL || relatedName == NULL)
    return (NULL);

  slashes = 0;
  after_access = NULL;
  p = aName;
  q = relatedName;
  len = 0;
  for (; *p && !strncasecmp (p, q, 1); p++, q++, len++)
    {
      /* Find extent of match */
      if (*p == ':')
        {
          after_access = p + 1;
#ifdef _WINDOWS
          if (len == 1)
            {
              /* it's a local Windows path like c:... */
              slashes+=2;
            }
#endif /* _WINDOWS */
        }
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
      if ((return_value = (char *)TtaGetMemory (len + 1)) != NULL)
        strcpy (return_value, p);
    }
  else if ((slashes < 2 && after_access == NULL)
           || (slashes < 3 && after_access != NULL))
    {
      /* Two names whitout common path */
      /* exactly the right length */
      len = strlen (aName);
      if ((return_value = (char *)TtaGetMemory (len + 1)) != NULL)
        strcpy (return_value, aName);
    }
  else
    {
      /* Some path in common */
#ifdef IV
      if (slashes == 3 && strncmp (aName, "http:", 5) == 0)
        /* just the same server */
        strcpy (result, last_slash);
      else
#endif
        {
          levels= 0; 
          for (; *q && *q != '#' && *q != ';' && *q != '?'; q++)
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
      if ((return_value = (char *)TtaGetMemory (len + 1)) != NULL)
        strcpy (return_value, result);

    }
#ifdef _WINDOWS
  len = strlen (return_value);
  for (ndx = 0; ndx < len; ndx ++)
	  if (return_value[ndx] == '\\')
      return_value[ndx] = '/' ;
#endif /* _WINDOWS */
  return (return_value);
}

/*----------------------------------------------------------------------
  AM_GetFileSize
  Returns TRUE and the filesize in the 2nd parameter.
  Otherwise, in case of a system error, returns FALSE, with a 
  filesize of 0L.
  ---------------------------------------------------------------------*/
ThotBool AM_GetFileSize (const char *filename, unsigned long *file_size)
{
  if (!TtaFileExist (filename))
    return FALSE;
  *file_size = TtaGetFileSize (filename);
  return TRUE;
}

/*----------------------------------------------------------------------
  AM_UseXHTMLMimeType
  Returns TRUE if the user has configured Amaya to use this MIME type,
  FALSE otherwise.
  ---------------------------------------------------------------------*/
ThotBool AM_UseXHTMLMimeType (void)
{
  ThotBool xhtml_mimetype;
  
  /* does the user wants to use the new MIME type? */
  TtaGetEnvBoolean ("ENABLE_XHTML_MIMETYPE", &xhtml_mimetype);

  return (xhtml_mimetype);
}


/********************************************
 The following routines were adapted from the GNU libc functions
 for generating a tmpnam.
*********************************************/

/* These are the characters used in temporary filenames.  */
static const char letters[] =
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

/* Generate a temporary file name based on TMPL.  TMPL must match the
   rules for mk[s]temp (i.e. end in "XXXXXX").  The name constructed
   does not exist at the time of the call to __gen_tempname.  TMPL is
   overwritten with the result.

   We use a clever algorithm to get hard-to-predict names. */
void
AM_gen_tempname (char *tmpl)
{
  int len;
  char *XXXXXX;
  static uint64_t value;
  uint64_t random_time_bits;
  unsigned int count;
  int save_errno = errno;
  struct stat st;

  /* A lower bound on the number of temporary files to attempt to
     generate.  The maximum total number of temporary file names that
     can exist for a given template is 62**6.  It should never be
     necessary to try all these combinations.  Instead if a reasonable
     number of names is tried (we define reasonable as 62**3) fail to
     give the system administrator the chance to remove the problems.  */
  unsigned int attempts_min = 62 * 62 * 62;

  /* The number of times to attempt to generate a temporary file.  To
     conform to POSIX, this must be no smaller than TMP_MAX.  */
  unsigned int attempts = attempts_min < TMP_MAX ? TMP_MAX : attempts_min;

  len = strlen (tmpl);
  if (len < 6 || strcmp (&tmpl[len - 6], "XXXXXX"))
    {
      /* @@ JK ? */
      errno = EINVAL;
      return;
    }

  /* This is where the Xs start.  */
  XXXXXX = &tmpl[len - 6];

  /* Get some more or less random data.  */
#ifdef RANDOM_BITS
  RANDOM_BITS (random_time_bits);
#else
# if HAVE_GETTIMEOFDAY || _LIBC
  {
    struct timeval tv;
    gettimeofday (&tv, NULL);
    random_time_bits = ((uint64_t) tv.tv_usec << 16) ^ tv.tv_sec;
  }
# else
  random_time_bits = time (NULL);
# endif
#endif
  value += random_time_bits ^ getpid ();

  for (count = 0; count < attempts; value += 7777, ++count)
    {
      uint64_t v = value;

      /* Fill in the random bits.  */
      XXXXXX[0] = letters[v % 62];
      v /= 62;
      XXXXXX[1] = letters[v % 62];
      v /= 62;
      XXXXXX[2] = letters[v % 62];
      v /= 62;
      XXXXXX[3] = letters[v % 62];
      v /= 62;
      XXXXXX[4] = letters[v % 62];
      v /= 62;
      XXXXXX[5] = letters[v % 62];

      /* This case is backward from the other three.  AM_gen_tempname
         succeeds if __xstat fails because the name does not exist.
         Note the continue to bypass the common logic at the bottom
         of the loop.  */
      if (stat (tmpl, &st) < 0)
        break;

      continue;
    }
  
  if (count == attempts || errno != ENOENT)
    tmpl[0] = EOS;
  else
    errno = save_errno;

  return;
}

#define JOSE 1

/*-----------------------------------------------------------------------
  GetTempName
  Front end to the Unix tempnam function, which is independent of the
  value of the TMPDIR env value 
  Returns a dynamically allocated string with a tempname. The user
  must free this memory.
  -----------------------------------------------------------------------*/
char *GetTempName (const char *dir, const char *prefix)
{
#ifdef JOSE

  static char tmpbufmem[PATH_MAX + 1];
  int len;
  int i;

  if (!dir || *dir == EOS || !TtaDirExists (dir))
    return NULL;

  /* make sure that the name is no bigger than PATH_MAX + the 6 tempname chars we
     will add */

  len = strlen (dir);
  if (len + 6 > PATH_MAX)
    return NULL;

  /* copy the dir name, and add a DIR_SEP if it's missing */
  if (dir[strlen (dir) - 1] == DIR_SEP)
    strcpy (tmpbufmem, dir);
  else
    {
      sprintf (tmpbufmem, "%s%c", dir, DIR_SEP);
      len++;
    }

  /* copy the prefix (no more than L_tmpnam chars, to respect POSIX). Save
     space for the 6 X and EOS chars that will become the random bits */
  if (prefix)
    { 
      i = 0;
      while (prefix[i] != EOS && i < L_tmpnam - 8)
        tmpbufmem[len++] = prefix[i++];
      tmpbufmem[len] = EOS;
    }

  /* Add the 6 X chars */
  len = strlen (tmpbufmem);
  i = 0;
  while (i < 6)
    {
      tmpbufmem[len++] = 'X';
      i++;
    }	 
  tmpbufmem[len] = EOS;

  AM_gen_tempname (tmpbufmem);

  if (tmpbufmem[0] == EOS)
    return NULL;
  else
    return (TtaStrdup (tmpbufmem));

#else
  char *tmpdir;
  char *tmp = NULL;
  char *name = NULL;

  /* save the value of TMPDIR */
  tmpdir = getenv ("TMPDIR");
  if (tmpdir)
    {
      /* remove TMPDIR from the environment */
      tmp = TtaGetMemory (strlen (tmpdir) + 20);
      sprintf (tmp, "TMPDIR=");
#ifdef _WINDOWS
      _putenv (tmp);
#else
      putenv (tmp);
#endif /* _WINDOWS */
      /* prepare the string to restore the value of TMPDIR */
      strrcat (tmp, tmpdir);
    }

  /* create the tempname */
#ifdef _WINDOWS
  /* Under Windows, _tempnam returns the same name until the file is created */
  {
    char *altprefix;
    name = tmpnam (NULL);	/* get a possibly unique string */
    altprefix = TtaGetMemory (strlen (prefix) + strlen(name) + 1);
    sprintf (altprefix, "%s%s", prefix, name + strlen(_P_tmpdir));
    name = _tempnam (dir, altprefix); /* get a name that isn't yet in use */
    TtaFreeMemory (altprefix);
  }
#else
  name = tempnam (dir, prefix);
#endif /* _WINDOWS */

  if (tmpdir)
    {
      /* restore the value of TMPDIR */
#ifdef _WINDOWS
      _putenv (tmpdir);
#else
      putenv (tmpdir);
#endif /* _WINDOWS */
      TtaFreeMemory (tmpdir);
    }
  return (name);
#endif
}
