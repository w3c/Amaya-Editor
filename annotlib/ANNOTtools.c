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
   list_add
   Adds a new element to the beginning of a linked
   list.
   ------------------------------------------------------------*/
void List_add (List **me, char *object)
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
   list_del
   Deletes the first element of a linked list.
   ------------------------------------------------------------*/
void List_del (List **me)
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
   AnnotList_free
   Frees a linked list of annotations.
   ------------------------------------------------------------*/
void AnnotList_free (List *annot_list)
{
  AnnotMeta *annot;
  List *list_ptr, *next;

  list_ptr = annot_list;
  while (list_ptr)
    {
      annot = (AnnotMeta *) list_ptr->object;
      if (annot->source_url) 
	free (annot->source_url);
      if (annot->author) 
	free (annot->author);
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
      next = list_ptr->next;
      free (list_ptr);
      list_ptr = next;
    }
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
  
  char *proto;

  AnnotMeta *annot;
  long content_length;

  /* we get the metadata associated to DocAnnot */
  annot = GetMetaData (DocumentMeta[doc]->source_doc, doc);

  if (!annot)
    return;

  /* @@ add a file: prefix if it's missing ... I could have
     called normalize URL! */
  if (!IsW3Path (annot->source_url) &&
      !IsFilePath (annot->source_url))
    proto = "file://";
  else
    proto = "";

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
		   "<a:annotates r:resource=\"%s%s\" />\n",
		   proto, 
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
	   "<http:Body r:parseType=\"literal\">\n",
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
	   "%02d/%02d/%04d %02d:%02d:%02d", 
	   localDate->tm_mday,
	   localDate->tm_mon+1,
           localDate->tm_year+1900,
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

  /* @@@ need to add a filter to use HTML_ATTR_Annotation */
  /* Searches the first anchor */
  elCour = TtaSearchTypedElement (elType, SearchForward, elCour);

  /* Searchs the anchor that points to the annotDoc */
  while (elCour != NULL) 
  {
    ancName = SearchAttributeInElt (doc, elCour, HTML_ATTR_NAME);
    if (ancName) 
      {
	if (!strcmp (ancName, annotDoc))
	  break;
	TtaFreeMemory (ancName);
      }
    elCour = TtaGetSuccessor (elCour);
    elCour = TtaSearchTypedElement (elType, SearchForward, elCour);
    ancName = SearchAttributeInElt (doc, elCour, HTML_ATTR_NAME);
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
   SearchAttributeInElt (doc, el, attrTypeNum)
   Returns the value of attribute type attrTypeNum if it exists in the
   document element or NULL otherwise.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
STRING SearchAttributeInElt (Document doc, Element el, int attrTypeNum)
#else /* __STDC__*/
STRING SearchAttributeInElt (doc, el, attrTypeNum)
     Document doc;
     Element  el;
     int      attrTypeNum;
#endif /* __STDC__*/
{
  AttributeType attrType;
  Attribute     attr;
  STRING        text = TtaGetMemory (50);
  int           text_lg;

  if (!el) 
    return NULL;

  text[0] = EOS;
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = attrTypeNum;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return NULL;
  else
  {
    /* @@@ bug! */
    text = TtaGetMemory (50);
    text_lg = 50;
    TtaGiveTextAttributeValue (attr, text, &text_lg);
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
  STRING text = SearchAttributeInElt (document, element, HTML_ATTR_Annotation);
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











