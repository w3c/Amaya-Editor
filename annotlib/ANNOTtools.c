/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTutil.c : different miscellaneous functions used by the other
 *               annotation modules
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "annotlib.h"
#include "fileaccess.h"
#include "AHTURLTools_f.h"
#include "HTML.h"

#ifdef _WINDOWS
#define TMPDIR "TMP"
#else
#define TMPDIR "TMPDIR"
#endif /* _WINDOWS */

/****************************************************************
 ** 
 ** List Handling functions
 **
 ****************************************************************/
/* ------------------------------------------------------------
   List_add
   Adds a new element to the beginning of a linked
   list.
   ------------------------------------------------------------*/
void List_add (List **me, CHAR_T *object)
{
  List *new;

  new = (List *) malloc (sizeof (List));
  new->object = object;
  if (!*me)
      new->next = NULL;
  else
      new->next = *me;
  *me = new;
}

/* ------------------------------------------------------------
   List_delFirst
   Deletes the first element of a linked list.
   ------------------------------------------------------------*/
void List_delFirst (List **me)
{
  List *ptr;

  if (*me)
    {
      ptr = (List *) (*me)->next;
      free (*me);
      *me = ptr;
    }
}

/* ------------------------------------------------------------
   list_delAll
   Deletes all the elements in a list and the list structure
   ------------------------------------------------------------*/
void List_delAll (List **me)
{
  List *ptr;

  while (*me)
    {
      ptr = (List *) (*me)->next;
      free (*me);
      *me = ptr;
    }
}

/* ------------------------------------------------------------
   List_delObject
   Removes an element of a linked list.
   ------------------------------------------------------------*/
void List_delObject (List **list, char *object)
{
  List *item = *list;

  while (item && (item->object != object)) {
    list = &item->next;
    item = *list;
  }

  if (item)
    {
      *list = item->next;
      free (item);
    }
}

/* ------------------------------------------------------------
   List_count
   Returns the number of items in a list
   ------------------------------------------------------------*/
int List_count (List *list)
{
  List *item = list;
  int count = 0;

  while (item)
    {
      count++;
      item = item->next;
    }

  return (count);
}

/* ------------------------------------------------------------
   AnnotFilter_add
   Adds a new element to the beginning of a linked
   list if it doesn't exist in the list.
   ------------------------------------------------------------*/
void AnnotFilter_add (List **me, CHAR_T *object)
{
  List *new;
  AnnotFilterData *filter;

  if (!object)
    return;

  if (*me && AnnotFilter_search (*me, object))
    return;

  /* initialize the filter */
  filter = TtaGetMemory (sizeof (AnnotFilterData));
  filter->object = TtaStrdup (object);
  filter->show = TRUE;

  /* and now add it to the list */
  new = (List *) malloc (sizeof (List));
  new->object = (void *) filter;
  if (!*me)
      new->next = NULL;
  else
      new->next = *me;
  *me = new;
}

/*------------------------------------------------------------
   AnnotFilter_search
   Returns list item that contains the object
   ------------------------------------------------------------*/
List *AnnotFilter_search (List *list, CHAR_T *object)
{
  List *list_item = list;
  AnnotFilterData *filter;
  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;
      if (!ustrcasecmp (filter->object, object))
	break;
      list_item = list_item->next;
    }

  return (list_item);
}

/*------------------------------------------------------------
   AnnotFilter_show
   Returns a boolean saying if a filter element containing
   a given object should be shown. If no filter element is
   found, it returns TRUE.
   ------------------------------------------------------------*/
ThotBool AnnotFilter_show (List *list, CHAR_T *object)
{
  List *list_item = list;
  AnnotFilterData *filter;

  if (!object)
    return TRUE;

  list_item = AnnotFilter_search (list, object);
  if (!list_item)
    return TRUE;
  filter = (AnnotFilterData *) list_item->object;
  if (filter)
    return filter->show;
  else
    return TRUE;
}

/*------------------------------------------------------------
   AnnotFilter_showServer
   Returns a boolean saying if a filter element containing
   a given object should be shown. If no filter element is
   found, it returns TRUE.
   ------------------------------------------------------------*/
ThotBool AnnotFilter_showServer (List *list, CHAR_T *url)
{
  List *list_item = list;
  AnnotFilterData *filter;
  CHAR_T server[MAX_LENGTH];

  if (!url)
    return TRUE;

  /* we first normalize the url name to get the server */
  GetServerName (url, server);

  list_item = AnnotFilter_search (list, server);
  if (!list_item)
    return TRUE;
  filter = (AnnotFilterData *) list_item->object;
  if (filter)
    return filter->show;
  else
    return TRUE;
}

/*------------------------------------------------------------
   AnnotFilter_showAuthor
   Returns a boolean saying if a filter element containing
   a given object should be shown. If no filter element is
   found, it returns TRUE.
   ------------------------------------------------------------*/
ThotBool AnnotFilter_showAuthor (List *list, CHAR_T *author, CHAR_T *url)
{
  List *list_item = list;
  AnnotFilterData *filter;
  CHAR_T server[MAX_LENGTH];
  CHAR_T *tmp;
  ThotBool result;

  if (!author || !url)
    return TRUE;

  /* we first normalize the url name to get the server */
  GetServerName (url, server);

  tmp = TtaGetMemory (ustrlen (author) + ustrlen (server) + 4);
  usprintf (tmp, "%s@%s", author, server);

  list_item = AnnotFilter_search (list, tmp);
  if (!list_item)
    result = TRUE;
  else 
    {
      filter = (AnnotFilterData *) list_item->object;
      if (filter)
	result = filter->show;
      else
	result = TRUE;
    }
  TtaFreeMemory (tmp);

  return result;
}

/*------------------------------------------------------------
   AnnotList_search
   Returns list item that contains the object
   ------------------------------------------------------------*/
List *AnnotList_search (List *list, CHAR_T *object)
{
  List *item = list;

  while (item && (ustrcasecmp ((CHAR_T *) item->object, (CHAR_T *) object)))
    {
      item = item->next;
    }

  return (item);
}

/*------------------------------------------------------------
   AnnotList_searchAnnot
   Returns the annot item that points to the same url
   ------------------------------------------------------------*/
AnnotMeta *AnnotList_searchAnnot (List *list, CHAR_T *url, ThotBool useAnnotUrl)
{
  List *item = list;
  AnnotMeta *annot;
  ThotBool found = FALSE;
  CHAR_T *ptr;

  while (item)
    {
      annot = (AnnotMeta *) item->object;
      /* @@ this crashes... why? */
      if (useAnnotUrl)
	ptr = annot->annot_url;
      else
	ptr = annot->body_url;

      if (ptr && !ustrcasecmp (ptr, url))
	{
	  found = TRUE;
	  break;
	}
      item = item->next;
    }

  return (found) ? annot : NULL;
}

/* ------------------------------------------------------------
   AnnotMeta_new
   Creates a new annotation metadata element
   ------------------------------------------------------------*/
AnnotMeta *AnnotMeta_new (void)
{
  AnnotMeta *new;

  new = (AnnotMeta *) malloc (sizeof (AnnotMeta));
  if (new)
    memset (new, 0, sizeof (AnnotMeta));
  return new;
}

/* ------------------------------------------------------------
   AnnotFilter_free
   Frees a linked list of annotation filters.
   ------------------------------------------------------------*/
void AnnotFilter_free (List *annot_list)
{
  List *list_ptr, *next;
  AnnotFilterData *filter;

  list_ptr = annot_list;
  while (list_ptr)
    {
      filter = (AnnotFilterData *) list_ptr->object;
      TtaFreeMemory (filter->object);
      TtaFreeMemory (filter);
      next = list_ptr->next;
      TtaFreeMemory (list_ptr);
      list_ptr = next;
    }
}

/* ------------------------------------------------------------
   AnnotList_free
   Frees a linked list of annotations.
   ------------------------------------------------------------*/
void AnnotList_free (List *annot_list)
{
  List *list_ptr, *next;

  list_ptr = annot_list;
  while (list_ptr)
    {
      Annot_free ((AnnotMeta *) list_ptr->object);
      next = list_ptr->next;
      free (list_ptr);
      list_ptr = next;
    }
}

/* ------------------------------------------------------------
   Annot_free
   Frees a single annotation.
   ------------------------------------------------------------*/
void Annot_free (AnnotMeta *annot)
{
  if (annot->annot_url)
    free (annot->annot_url);
  if (annot->source_url) 
    free (annot->source_url);
  if (annot->author) 
    free (annot->author);
  /* annot->type @@ */
  if (annot->cdate) 
    free (annot->cdate);
  if (annot->mdate) 
    free (annot->mdate);
  if (annot->content_type) 
    free (annot->content_type);
  if (annot->content_length) 
    free (annot->content_length);
  if (annot->body_url) 
    free (annot->body_url);
  if (annot->body) 
    free (annot->body);
  free (annot);
}
 
/* ------------------------------------------------------------
   AnnotList_print
   Prints the contents For each element of a linked list of 
   annotations metadata.
   ------------------------------------------------------------*/
void AnnotList_print (List *annot_list)
{
  AnnotMeta *annot;
  List *annot_ptr;

  annot_ptr = annot_list;
  while (annot_ptr)
    {
      annot = (AnnotMeta *) annot_ptr->object;
      printf("\n=====annotation meta data =========\n");  
      if (annot->source_url)
	printf ("annot source URL = %s\n", annot->source_url);
      if (annot->labf)
	printf ("annot labf = %s, c1 = %d\n", annot->labf, annot->c1);
      if (annot->labl)
	printf ("annot labl = %s, cl = %d\n", annot->labl, annot->cl);
      if (annot->author) 
	printf ("author is = %s\n", annot->author);
      if (annot->content_type)
	printf ("content_type is = %s\n", annot->content_type);
      if (annot->content_length) 
	printf ("content_length is = %s\n", annot->content_length);
      if (annot->body_url)
	printf ("body url is = %s\n", annot->body_url);
      if (annot->body)
	  printf ("======= body =============\n%s", annot->body);
      printf ("=========================\n");
      annot_ptr = annot_ptr->next;
    }
  printf ("\n");
}

/* ------------------------------------------------------------
   AnnotList_writeIndex
   Writes an RDF annotation index file from the contents
   of annot_list.
   ------------------------------------------------------------*/
void AnnotList_writeIndex (CHAR_T *indexFile, List *annot_list)
{
  AnnotMeta *annot;
  List *annot_ptr;
  FILE *fp;

  if (!annot_list || !indexFile || indexFile[0] == WC_EOS)
    return;

  fp = fopen (indexFile, "w");
  /* write the prologue */
  fprintf (fp,
	  "<?xml version=\"1.0\" ?>\n" 
	  "<r:RDF xmlns:r=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
	  "xmlns:a=\"http://www.w3.org/1999/xx/annotation-ns#\"\n"
	  "xmlns:xlink=\"http://www.w3.org/1999/xx/xlink#\"\n"
	  "xmlns:http=\"http://www.w3.org/1999/xx/http#\"\n"
 	  "xmlns:d=\"http://purl.org/dc/elements/1.0/\">\n");

  /*write each annotation entry */
  annot_ptr = annot_list;
  while (annot_ptr)
    {
      annot = (AnnotMeta *) annot_ptr->object;
     
      /* only save the local annotations */
      if (IsFilePath (annot->body_url))
	{
	  fprintf (fp, 
		   "<r:Description about=\"%s\">\n",
		   annot->body_url);

	  fprintf (fp, 
		   "<r:type resource=\"http://www.w3.org/1999/xx/annotation-ns#Annotation\" />\n");

	  fprintf (fp, 
		   "<r:type resource=\"http://www.w3.org/1999/xx/annotation-ns#%s\" />\n",
		   annot->type);

	  fprintf (fp, 
		   "<a:annotates r:resource=\"%s\" />\n",
		   annot->source_url);

	  fprintf (fp,
	      "<a:context>#id(%s|%d|%s|%d)</a:context>\n",
		   annot->labf,
		   annot->c1,
		   annot->labl,
		   annot->cl);
	  
	  fprintf (fp,
		   "<d:creator>%s</d:creator>\n",
		   annot->author);
	  
	  fprintf (fp,
		   "<a:created>%s</a:created>\n",
		   annot->cdate);

	  fprintf (fp,
		   "<d:date>%s</d:date>\n",
		   annot->mdate);

	  fprintf (fp,
		   "<a:body r:resource=\"%s\" />\n",
		   annot->body_url);
	  
	  fprintf (fp, 
		   "</r:Description>\n");
	}
      annot_ptr = annot_ptr->next;
    }

  /* write the epiloge */
  fprintf (fp, 
	   "</r:RDF>\n");
  fclose (fp);
}

/* ------------------------------------------------------------
   AnnotList_preparePostBody
   Writes an RDF file made from an annotations metadata
   and the annotations HTML body. 
   Stores the result in /tmp/rdf.tmp.
   ------------------------------------------------------------*/
#ifdef __STDC__
void ANNOT_PreparePostBody (Document doc)
#else
void ANNOT_PreparePostBody (doc)
Document doc;

#endif /* __STDC__ */
{
  FILE *fp;
  FILE *fp2;
  char tmp_str[80];
  char *ptr;
  
  AnnotMeta *annot;
  long content_length;

  /* we get the metadata associated to DocAnnot */
  annot = GetMetaData (DocumentMeta[doc]->source_doc, doc);

  if (!annot)
    return;

  if (IsFilePath(DocumentURLs[doc]))
    content_length = GetFileSize (DocumentURLs[doc]+7);	/* skip "file://" */
  else
    content_length = GetFileSize (DocumentURLs[doc]);

  fp = fopen ("/tmp/rdf.tmp", "w");
  /* write the prologue */
  /* write the prologue */
  fprintf (fp,
	  "<?xml version=\"1.0\" ?>\n" 
	  "<r:RDF xmlns:r=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
	  "xmlns:a=\"http://www.w3.org/1999/xx/annotation-ns#\"\n"
	  "xmlns:xlink=\"http://www.w3.org/1999/xx/xlink#\"\n"
	  "xmlns:http=\"http://www.w3.org/1999/xx/http#\"\n"
 	  "xmlns:d=\"http://purl.org/dc/elements/1.0/\">\n");

  /* beginning of the annotation's  metadata  */
  fprintf (fp,
	   "<r:Description>\n");

	  fprintf (fp, 
		   "<r:type resource=\"http://www.w3.org/1999/xx/annotation-ns#Annotation\" />\n");

	  fprintf (fp, 
		   "<r:type resource=\"http://www.w3.org/1999/xx/annotation-ns#%s\" />\n",
		   annot->type);

	  fprintf (fp, 
		   "<a:annotates r:resource=\"%s\" />\n",
		   annot->source_url);

	  fprintf (fp,
	      "<a:context>#id(%s|%d|%s|%d)</a:context>\n",
		   annot->labf,
		   annot->c1,
		   annot->labl,
		   annot->cl);
	  
	  fprintf (fp,
		   "<d:creator>%s</d:creator>\n",
		   annot->author);
	  
	  fprintf (fp,
		   "<a:created>%s</a:created>\n",
		   annot->cdate);

	  fprintf (fp,
		   "<d:date>%s</d:date>\n",
		   annot->mdate);

  /* the rest of the metadata */
  fprintf (fp,
	   "<a:body>\n"
	   "<r:Description>\n"
	   "<http:ContentType>%s</http:ContentType>\n"
	   "<http:ContentLength>%ld</http:ContentLength>\n"
	   "<http:Body r:parseType=\"Literal\">\n",
	   "text/html",
	   content_length);

 /* insert the HTML body */
  ptr = DocumentURLs[doc];
  /* skip any file: prefix */
  if (!ustrncmp (ptr, "file:", 5))
      ptr = ptr + 5;
  fp2 = fopen (ptr, "r");
  if (fp2)
    {
      /* skip the first 3 lines (to have a valid XML doc )*/
      /* ahem, skip the first 3 lines, in the hard way! */
      {
	int i;
	char c;
	for (i = 0; i<3; i++)
	  {
	    while ((c = getc (fp2)) != '\n');
	  }
      }
      fgets (tmp_str, 79, fp2);
      while (!feof (fp2)) {
	fprintf (fp, "  %s", tmp_str);
	fgets (tmp_str, 79, fp2);
      }
      fclose (fp2);
    }

  /* finish writing the annotation */
  fprintf (fp, 
	   "</http:Body>\n"
	   "</r:Description>\n"
	   "</a:body>\n"
	   "</r:Description>\n"
	   "</r:RDF>\n");

  fclose (fp);  
}

/* ------------------------------------------------------------
   StrDupDate
   Returns a pointer to a memalloc'd string containing the current date.
   It's up to the caller to free this memory.
   ------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T *StrdupDate (void)
#else
CHAR_T *StrdupDate ()
#endif /* __STDC__ */
{
  time_t      curDate;
  struct tm   *localDate;
  CHAR_T      *strDate;
  
  curDate = time (&curDate);
  localDate = localtime (&curDate);
  /* @@ possible memory bug */
  strDate = TtaGetMemory (25);
  sprintf (strDate,
	   "%04d-%02d-%02dT%02d:%02d:%02d",
	   localDate->tm_year+1900,
	   localDate->tm_mon+1,
	   localDate->tm_mday, 
           localDate->tm_hour,
	   localDate->tm_min,
	   localDate->tm_sec);
  return (strDate);
}

/***************************************************
 **
 **************************************************/

/*-----------------------------------------------------------------------
   SearchAnnotation
   Searches doc and returns the link element that points to annotDoc, or
   NULL if it doesn't exist.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
Element SearchAnnotation (Document doc, STRING annotDoc)
#else /* __STDC__*/
Element SearchAnnotation (doc, annotDoc)
     Document doc;
     STRING   annotDoc;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     elCour;
  STRING ancName = NULL;

  elCour = SearchElementInDoc (doc, HTML_EL_BODY);
  elType = TtaGetElementType (elCour);
  elType.ElTypeNum = HTML_EL_Anchor;

  /* @@@ need to add a filter to use HTML_ATTR_IsAnnotation */
  /* Searches the first anchor */
  elCour = TtaSearchTypedElement (elType, SearchForward, elCour);

  /* Searchs the anchor that points to the annotDoc */
  while (elCour != NULL) 
  {
    ancName = SearchAttributeInEl (doc, elCour, HTML_ATTR_NAME, TEXT("HTML"));
    if (ancName) 
      {
	if (!strcmp (ancName, annotDoc))
	  break;
	TtaFreeMemory (ancName);
      }
    elCour = TtaGetSuccessor (elCour);
    elCour = TtaSearchTypedElement (elType, SearchForward, elCour);
    ancName = SearchAttributeInEl (doc, elCour, HTML_ATTR_NAME, TEXT("HTML"));
  }

  if (ancName)
    TtaFreeMemory (ancName);

  return elCour;
}

/*-----------------------------------------------------------------------
   SearchElementInDoc
   Returns the first element of type elTypeNum found in the document
   or NULL if it doesn't exist.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
Element SearchElementInDoc (Document doc, int elTypeNum)
#else /* __STDC__*/
Element SearchElementInDoc (doc, elTypeNum)
     Document doc;
     int      elTypeNum;
#endif /* __STDC__*/
{
  Element     root;
  ElementType elType;

  if ((root = TtaGetMainRoot (doc)) == NULL)
    return NULL;
  elType = TtaGetElementType (root);
  elType.ElTypeNum = elTypeNum;
  return TtaSearchTypedElement (elType, SearchInTree, root);
}

/*-----------------------------------------------------------------------
   SearchAttributeInEl (doc, el, attrTypeNum, schema)
   Returns the value of attribute type attrTypeNum if it exists in the
   document element or NULL otherwise.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
STRING SearchAttributeInEl (Document doc, Element el, int attrTypeNum, 
			    CHAR_T *schema)
#else /* __STDC__*/
STRING SearchAttributeInEl (doc, el, attrTypeNum, schema)
     Document doc;
     Element  el;
     int      attrTypeNum;
     CHAR_T  *schema;
#endif /* __STDC__*/
{
  AttributeType  attrType;
  Attribute      attr;
  CHAR_T        *text;
  int            length;

  if (!el) 
    return NULL;

  attrType.AttrSSchema = TtaGetSSchema (schema, doc);
  attrType.AttrTypeNum = attrTypeNum;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return NULL;
  else
  {
    length = TtaGetTextAttributeLength (attr);
    text = TtaGetMemory (length + 1);
    TtaGiveTextAttributeValue (attr, text, &length);
    return text;
  }
}

/*-----------------------------------------------------------------------
   Procedure SubstituteCharInString (buffer, old, new)
  -----------------------------------------------------------------------
   Substitutes all occurences of old char with new char in string buffer
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void SubstituteCharInString (char *buffer, char old, char new)
#else
void SubstituteCharInString (buffer, old, new)
char *buffer;
char  old;
char new;
#endif /* __STDC__ */
{
  char *ptr;

  ptr = buffer;
  while (*ptr)
    {
      if (*ptr == old)
	*ptr = new;
      ptr++;
    }
}


/*-----------------------------------------------------------------------
  GetTempName
  Front end to the Unix tempnam function, which is independent of the
  value of the TMPDIR env value 
  Returns a dynamically allocated string with a tempname. The user
  must free this memory.
  -----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetTempName (const char *dir, const char *prefix)
#else
char *GetTempName (dir, prefix)
const char *dir;
const char *prefix;
#endif /* __STDC__ */
{
  char *tmpdir;
  char *tmp;
  char *name = NULL;

  /* save the value of TMPDIR */
  tmp = getenv (TMPDIR);

  if (tmp)
    {
      tmpdir = TtaStrdup (tmp);
    }
  else
    tmpdir = NULL;

  /* remove TMPDIR from the environment */
  if (tmpdir)
    {
      tmp = TtaGetMemory (strlen (tmpdir) + 2);
      sprintf (tmp, "%s=", TMPDIR);
#ifdef _WINDOWS
      _putenv (tmp);
#else
      putenv (tmp);
#endif /* _WINDOWS */
    }

  /* create the tempname */
#ifdef _WINDOWS
  /* @@ this function is broken under windows :-/ */
  name = _tempnam (dir, prefix);
#else
  name = tempnam (dir, prefix);
#endif /* _WINDOWS */

  /* restore the value of TMPDIR */
  if (tmpdir)
    {
#ifdef _WINDOWS
      _putenv (tmpdir);
#else
      putenv (tmpdir);
#endif /* _WINDOWS */
      /* no need to free this string */
      /* TtaFreeMemory (tmpdir); */
    }
  return (name);
}

#ifdef __STDC__
long GetFileSize (CHAR_T *filename)
#else
long GetFileSize (filename)
CHAR_T *filename;

#endif /* __STDC__ */
{
  ThotFileHandle      handle = ThotFile_BADHANDLE;
  ThotFileInfo        info;
 
  handle = TtaFileOpen (filename, ThotFile_READWRITE);
  if (handle == ThotFile_BADHANDLE)
    /* ThotFile_BADHANDLE */
    return 0L;
   if (TtaFileStat (handle, &info) == 0)
     /* bad stat */
     return 0L;

   return (info.size);
}

#ifdef __STDC__
void GetServerName (CHAR_T *url, CHAR_T *server)
#else
void GetServerName (url, server)
CHAR_T *url;
CHAR_T *server;

#endif /* __STDC__ */
{
  CHAR_T      *scratch_url;
  CHAR_T      *protocol;
  CHAR_T      *host;
  CHAR_T      *dir;
  CHAR_T      *file;

  if (IsFilePath (url))
      ustrcpy (server, TEXT("localhost"));
  else
    {
      scratch_url = TtaStrdup (url);
      ExplodeURL (scratch_url, &protocol, &host, &dir, &file);
      ustrcpy (server, host);
      if (dir[0])
	{
	  ustrcat (server, TEXT("/"));
	  ustrcat (server, dir);
	}
      TtaFreeMemory (scratch_url);
      /* remove the query string */
      scratch_url = ustrrchr (server, TEXT('?'));
      if (scratch_url)
	*scratch_url = WC_EOS;
    }
}

/*-----------------------------------------------------------------------
   ANNOT_GetHTMLTitle
   Returns the HTML title of the given document or NULL if this
   element doesn't exist.
  -----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T *ANNOT_GetHTMLTitle (Document doc)
#else
CHAR_T *ANNOT_GetHTMLTitle (doc)
Document doc;
#endif /* __STDC__ */
{
  Element          el;
  int              length;
  Language         lang;
  CHAR_T          *title;
  ElementType      elType;
  
   /* only HTML documents can be annotated */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = HTML_EL_TITLE;
  /* find the title */
  el = TtaGetMainRoot (doc);
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  /* no title */
  if (!el)
    return NULL;
  /* find the text content */
  el = TtaGetFirstChild (el);
  /* no content */
  if (!el)
    return NULL;
  length = TtaGetTextLength (el) + 1;
  title = TtaAllocString (length);
  TtaGiveTextContent (el, title, &length, &lang);
  return (title);
}

/*-----------------------------------------------------------------------
   ANNOT_SetType
   Sets the annotation type of annotation document doc to the value of
   type, if type is not empty.
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void ANNOT_SetType (Document doc, CHAR_T *type)
#else
CHAR_T *ANNOT_SetType (doc, type)
Document doc;
CHAR_T *type;
#endif /* __STDC__ */
{
  Element          el;
  ElementType      elType;
  CHAR_T          *url;
  CHAR_T          *ptr;
  int              i;
  AnnotMeta       *annot;
  
  if (!type || type[0] == WC_EOS)
    return;

   /* only HTML documents can be annotated */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = Annot_EL_RDFtype;
  /* find the type */
  el = TtaGetMainRoot (doc);
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  /* no such element */
  if (!el)
    return;
  /* change the text content */
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, type,
		     TtaGetDefaultLanguage (), doc);

  /* update the metadata */
  el = TtaGetMainRoot (doc);
  elType.ElTypeNum = Annot_EL_SourceDoc;
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  if (!el)
    return;
  url = SearchAttributeInEl (doc, el, Annot_ATTR_HREF_, TEXT("Annot"));
  if (!url)
    return;
  ptr = ustrchr (url, TEXT('#'));
  if (ptr)
    *ptr = WC_EOS;
  for (i = 1; i <=DocumentTableLength; i++)
    {
      if (!ustrcmp (url, DocumentURLs[i]))
	{
	  /* we found the source document, we now search and update
	     the annotation meta data */
	  /* @@ this doesn't work yet... we need to make file:/// here
	   and I need to remove the document modified thingy, as we have
	  to tell the user that he needs to save the document */
	  if (!IsW3Path (DocumentURLs[doc]) 
	      && !IsFilePath (DocumentURLs[doc]))
	    {
	      /* @@ add the file:// (why it wasn't there before? */
	      ptr = TtaGetMemory (strlen (DocumentURLs[doc])
				  + sizeof (TEXT("file://"))
				  + 1);
	      usprintf (ptr, "file://%s", DocumentURLs[doc]);
	    }
	  else
	    ptr = NULL;
	  annot = AnnotList_searchAnnot (AnnotMetaData[i].annotations,
					 (ptr) ? ptr : DocumentURLs[doc],
					 FALSE);
	  if (ptr)
	    TtaFreeMemory (ptr);
	  if (annot)
	    {
	      TtaFreeMemory (annot->type);
	      annot->type = TtaStrdup (type);
	    }
	  break;
	}
    }
  TtaFreeMemory (url);
}

/***************************************************
 I've not yet used/cleaning the following legacy functions 
***************************************************/

/*-----------------------------------------------------------------------
   Procedure IsAnnotationDocument (document)
  -----------------------------------------------------------------------
   Retourne vrai si le document est un document d'annotations
  -----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool IsAnnotationDocument (Document document)
#else /* __STDC__*/
ThotBool IsAnnotationDocument (document)
     Document document;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     root, dateAnnot;

  root = TtaGetMainRoot (document);
  elType = TtaGetElementType (root);
#if 0
  /* I could search for metadata here */
  elType.ElTypeNum = HTML_EL_AuteurAnnot;
#endif
  dateAnnot = TtaSearchTypedElement (elType, SearchInTree, root);

  return (dateAnnot != NULL);
}

/*-----------------------------------------------------------------------
   Procedure IsAnnotationLink (document, element)
  -----------------------------------------------------------------------
   Retourne vrai si l'element du document est un lien d'annotation
  -----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool IsAnnotationLink (Document document, Element element)
#else /* __STDC__*/
ThotBool IsAnnotationLink (document, element)
     Document document;
     Element  element;
#endif /* __STDC__*/
{
  /* @@ is this ok? */
  STRING text = SearchAttributeInEl (document, element, 
				     HTML_ATTR_IsAnnotation, TEXT("HTML"));
  return !strcmp (text, "Annotation");
}

/*-----------------------------------------------------------------------
   Procedure IsInTable (docName)
  -----------------------------------------------------------------------
   Retourne vrai si le document d'annotation docName est dana la table
   de references des annotations, ce qui veut dire qu'il est en cours
   d'edition
  -----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool IsInTable (STRING docName)
#else /* __STDC__*/
ThotBool IsInTable (docName)
     STRING docName;
#endif /* __STDC__*/
{
  return 0;
#if 0
  int i = 0;

  while ((i < 10) && (strcmp (docName, tabRefAnnot[i].docName)))
    i++;
  return (i != 10);
#endif
}

/*-----------------------------------------------------------------------
   Procedure AnnotationTargetDocument (annotDoc)
  -----------------------------------------------------------------------
   Retourne le document annote par annotDoc ou NULL si celui-ci n'est
   pas ouvert ou n'existe pas
  -----------------------------------------------------------------------*/

#ifdef __STDC__
Document AnnotationTargetDocument (Document annotDoc)
#else /* __STDC__*/
Document AnnotationTargetDocument (annotDoc)
     Document annotDoc;
#endif /* __STDC__*/
{
  Element  ptr_annotDoc, text;
  Language lang;
  int      lg = 200;
  STRING   docName = TtaGetMemory (200);

#if 0
  ptr_annotDoc = SearchElementInDoc (annotDoc, HTML_EL_PtrDocAnnot);
#endif
  text = TtaGetFirstChild (ptr_annotDoc);
  TtaGiveTextContent (text, docName, &lg, &lang);

  return TtaGetDocumentFromName (docName);
}
