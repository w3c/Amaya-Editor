/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTlink.c : module for handling the set of annotation indexes
 * and for merging them into documents.
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

#include "annotlib.h"
#include "ANNOTtools_f.h"
#include "AHTURLTools_f.h"
#include "ANNOTevent_f.h"
#include "XPointer.h"
#include "XPointer_f.h"
#include "XPointerparse_f.h"
#include "XLinkedit_f.h"
#include "fetchXMLname_f.h"

/*-----------------------------------------------------------------------
  LINK_CreateAName
  Creates an anchor name for the reverse link from the annotation to
  the document
  -----------------------------------------------------------------------*/

#ifdef __STDC__
static void LINK_CreateAName (AnnotMeta *annot)
#else /* __STDC__*/
static void LINK_CreateAName (annot)
AnnotMeta *annot;

#endif /* __STDC__ */
{
  char *ptr;

  /* memorize the anchor of the reverse link target */
  annot->name = TtaGetMemory (strlen (ANNOT_ANAME) + strlen (annot->author)
			      + (annot->body_url 
				 ? strlen (annot->body_url) : 0)
			      + 20);
  sprintf (annot->name, "%s_%s_%s", ANNOT_ANAME, annot->author,
	   annot->body_url ? annot->body_url : "");
  /* and remove all the ? links so that it all becomes a link, and not a target */
  ptr = annot->name;
  while (*ptr)
    {
      if (*ptr == '?')
	*ptr = '_';
      ptr++;
    }
}

/*-----------------------------------------------------------------------
   LINK_GetAnnotationIndexFile 
   searches an entry in the main annot index file 
   The caller must free the returned string.
   -----------------------------------------------------------------------*/
#ifdef __STDC__
char  *LINK_GetAnnotationIndexFile (char *source_url)
#else
char *LINK_GetAnnotationIndexFile (source_url)
char *source_url;
#endif /* __STDC__ */
{
  int found;
  CHAR_T *annot_dir;
  CHAR_T *annot_main_index;
  CHAR_T *annot_main_index_file;
  CHAR_T url[MAX_LENGTH];
  CHAR_T buffer[MAX_LENGTH];
  CHAR_T *index_file;
  FILE *fp;
  
  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index_file = TtaGetMemory (ustrlen (annot_dir) 
					+ ustrlen (annot_main_index) 
					+ 10);
  usprintf (annot_main_index_file, TEXT("%s%c%s"), 
	    annot_dir, 
	    DIR_SEP,  
	    annot_main_index);
  
  if (TtaFileExist (annot_main_index_file))
    {
      index_file = TtaGetMemory (MAX_PATH);
      found = 0;
      if ((fp = fopen (annot_main_index_file, "r")))
	{
	  while (ufgets (buffer, MAX_LENGTH, fp))
	    {
	      usscanf (buffer, TEXT("%s %s\n"), url, index_file);
	      if (!ustrcasecmp (source_url, url))
		{
		  found = 1;
		  break;
		}
	    }
	  fclose (fp);
	}
      if (!found)
	{
	  TtaFreeMemory (index_file);
	  index_file = NULL;
	}
    }
  else
    index_file = NULL;

  TtaFreeMemory (annot_main_index_file);
  
  return (index_file);
}

/*-----------------------------------------------------------------------
  AddAnnotationIndexFile 
  adds a new entry to the main annot index file
  -----------------------------------------------------------------------*/
#ifdef __STDC__
static void AddAnnotationIndexFile (char *source_url, char *index_file)
#else
static void AddAnnotationIndexFile (source_url, index_file)
char *source_url;
char *index_file;
#endif /* __STDC__ */
{
  CHAR_T *annot_dir;
  CHAR_T *annot_main_index;
  CHAR_T *annot_main_index_file;
  FILE *fp;
  
  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index_file = TtaGetMemory (ustrlen (annot_dir) 
					+ ustrlen (annot_main_index)
					+ 10);
  usprintf (annot_main_index_file, 
	    TEXT("%s%c%s"), 
	    annot_dir, 
	    DIR_SEP, 
	    annot_main_index);

  if ((fp = fopen (annot_main_index_file, "a")))
    {
      ufprintf (fp, TEXT("%s %s\n"), source_url, index_file);
      fclose (fp);
    }
  TtaFreeMemory (annot_main_index_file);
}

/*-----------------------------------------------------------------------
  LINK_AddLinkToSource
  Adds to a source document, an annotation link pointing to the annotation.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_AddLinkToSource (Document source_doc, AnnotMeta *annot)
#else /* __STDC__*/
void LINK_AddLinkToSource (source_doc, annot)
Document source_doc; 
AnnotMeta *annot;
#endif /* __STDC__*/
{
  ElementType   elType, firstElType;
  Element       el, first, anchor;
  AttributeType attrType;
  Attribute     attr;
  CHAR_T       *annot_user;
  CHAR_T       *tmp;
  CHAR_T       server[MAX_LENGTH];
  CHAR_T       *docSchemaName;
  int          c1, cN;
  SSchema      XLinkSchema;

  annot_user = GetAnnotUser ();
  
  if (annot->xptr)
    {
      XPointerContextPtr ctx;
      nodeInfo *node;
      ctx = XPointer_parse (source_doc, annot->xptr);
      node = XPointer_nodeStart (ctx);
      first = XPointer_el (node);
      c1 = XPointer_startC (node);
      cN = XPointer_endC (node);
      XPointer_free (ctx);
    }
  else
    first  = TtaSearchElementByLabel(annot->labf, TtaGetMainRoot (source_doc));

  if (!first)
    /* @@ JK: signal an error, orphan annotation */
    return;
  
  
  /* create the anotation element */
  XLinkSchema = GetXLinkSSchema (source_doc);
  elType.ElSSchema = XLinkSchema;
  elType.ElTypeNum = XLink_EL_XLink;
  anchor = TtaNewElement (source_doc, elType);
  
  /*
  ** insert the anchor 
  */

  firstElType = TtaGetElementType (first);
  docSchemaName = TtaGetSSchemaName (firstElType.ElSSchema);

  /* is the user trying to annotate an annotation? */
  el = TtaGetTypedAncestor (first, elType);
  if (el)
    {
      /* yes, so we will add the anchor one element before
	in the struct tree */
      TtaInsertSibling (anchor, el, TRUE, source_doc);
    }
  else if (!ustrcmp (docSchemaName, TEXT("MathML")))
    {
      /* An annotation on a MathMl structure. We backtrace the tree
	 until we find the Math root element and then add the annotation as 
	 its first child. */
      el = first;
      while (el)
	{
	  elType = TtaGetElementType (el);
	  if (elType.ElTypeNum == MathML_EL_MathML)
	    break;
	  el = TtaGetParent (el);
	}
      TtaInsertFirstChild (&anchor, el, source_doc);
    }
  else if (!ustrcmp (docSchemaName, TEXT("GraphML")))
    {
      /* An annotation on a GraphMl structure. We backtrace the tree
	 until we find the Math root element and then add the annotation as
	 its first child. */
      el = first;
      while (el)
	{
	  elType = TtaGetElementType (el);
	  if (elType.ElTypeNum == GraphML_EL_GraphML)
	    break;
	  el = TtaGetParent (el);
	}
      TtaInsertFirstChild (&anchor, el, source_doc);
    }
    else
    {
      if (c1 == 0)
	{
	  /* add it to the beginning */
	  TtaInsertSibling (anchor, first, TRUE, source_doc);
	}
      else if (c1 > 0)
	{
	  /* split the text */
	  int len;

	  len = TtaGetTextLength (first);
	  if (cN > len)
	    /* add it to the end */
	    TtaInsertSibling (anchor, first, FALSE, source_doc);
	  else
	    {
	      /* add it in the midle */
	      TtaSplitText (first, c1 - 1, source_doc);
	      TtaNextSibling (&first);
	      TtaInsertSibling (anchor, first, TRUE, source_doc);
	    }
	}
    }

  /* add the Xlink attribute */
  SetXLinkTypeSimple (anchor, source_doc, FALSE);

  /*
  ** add the other attributes
  */

  attrType.AttrSSchema = XLinkSchema;

  /* add the annotation icon */
  if (ustrcasecmp (annot->author, "tim"))
    attrType.AttrTypeNum = XLink_ATTR_AnnotIcon1;
  else
    /* @@ aha, it's Tim */
    attrType.AttrTypeNum = XLink_ATTR_AnnotIcon2;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);  

  /* add an HREF attribute pointing to the annotation */
  attrType.AttrTypeNum =  XLink_ATTR_href_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  TtaSetAttributeText (attr, annot->body_url, anchor, source_doc);
  
  /* add a ID attribute so that the annotation doc can point
     back to the source of the annotation link */
  attrType.AttrTypeNum = XLink_ATTR_id;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  /* set the ID value (anchor endpoint) */
  TtaSetAttributeText (attr, annot->name, anchor, source_doc);

  /* @@ JK: maybe add a role so that we know the annotation type */

  /* add the annotation to the filter list */
  AnnotFilter_add (&AnnotMetaData[source_doc], BY_TYPE, annot->type, annot);
  if (annot->annot_url)
    tmp = annot->annot_url;
  else
    tmp = annot->body_url;

  if (tmp)
    { /* @@ when creating a new annot, we don't yet know the URL;
         perhaps we should use the POST server name here? */
      GetServerName (tmp, server);
      AnnotFilter_add (&AnnotMetaData[source_doc], BY_SERVER, server, annot);
    }
  else
    server[0] = WC_EOS;

  tmp = TtaGetMemory (ustrlen (annot->author) + ustrlen (server) + 4);
  usprintf (tmp, "%s@%s", annot->author, server);
  AnnotFilter_add (&AnnotMetaData[source_doc], BY_AUTHOR, tmp, annot);
  TtaFreeMemory (tmp);
}

/*-----------------------------------------------------------------------
  LINK_RemoveLinkFromSource
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_RemoveLinkFromSource (Document source_doc, Element el)
#else /* __STDC__*/
void LINK_RemoveLinkFromSource (source_doc, annot)
     Document source_doc;
     AnnotMeta *annot;
#endif /* __STDC__*/
{
  TtaRemoveTree (el, source_doc);
}


/*-----------------------------------------------------------------------
   LINK_SaveLink
   Writes the metadata describing an annotation file and its source
   link to an index file. This metadata consists of the
   name of the annotation file, and the Xpointer that specifies the 
   selected  elements on the document
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_SaveLink (Document source_doc)
#else /* __STDC__*/
void LINK_SaveLink (source_doc)
     Document source_doc;
#endif /* __STDC__*/
{
  char   *indexName;
  List   *annot_list;

  /* Open the annotation index */
  indexName = LINK_GetAnnotationIndexFile (DocumentURLs[source_doc]);
  if (!indexName)
    {
      indexName = GetTempName (GetAnnotDir (), "index");
      AddAnnotationIndexFile (DocumentURLs[source_doc], indexName);
    }

  annot_list = AnnotMetaData[source_doc].annotations;  
  /* write the update annotation list */
  AnnotList_writeIndex (indexName, annot_list);
  TtaFreeMemory (indexName);
}

/*-----------------------------------------------------------------------
   LINK_DeleteLink
   For a given source doc, deletes the index entry from the main index and
   removes the documents index file.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_DeleteLink (Document source_doc)
#else /* __STDC__*/
void LINK_DeleteLink (source_doc)
     Document source_doc;
#endif /* __STDC__*/
{
  char   *doc_index;
  CHAR_T buffer[255];
  CHAR_T *annot_dir;
  CHAR_T *main_index;
  CHAR_T *main_index_file_old;
  CHAR_T *main_index_file_new;
  int len;
  int error;
  FILE *fp_old;
  FILE *fp_new;
  

  /* get the annotation index */
  doc_index = LINK_GetAnnotationIndexFile (DocumentURLs[source_doc]);
  if (!doc_index)
    return;

  error = 0;
  annot_dir = GetAnnotDir ();
  main_index = GetAnnotMainIndex ();
  main_index_file_old = TtaGetMemory (ustrlen (annot_dir) 
					+ ustrlen (main_index)
					+ 10);

  main_index_file_new = TtaGetMemory (ustrlen (annot_dir) 
				      + ustrlen (main_index)
				      + 14);
  usprintf (main_index_file_old, 
	    TEXT("%s%c%s"), 
	    annot_dir, 
	    DIR_SEP, 
	    main_index);

  usprintf (main_index_file_new, 
	    TEXT("%s%c%s.tmp"), 
	    annot_dir, 
	    DIR_SEP, 
	    main_index);

  if (!(fp_new = fopen (main_index_file_new, "w")))
    error++;

  if (!error && !(fp_old = fopen (main_index_file_old, "r")))
    {
      fclose (fp_new);
      error++;
    }

  if (!error)
    {
      /* search and remove the index entry */
      len = ustrlen (DocumentURLs[source_doc]);
      while (fgets (buffer, sizeof (buffer), fp_old))
	{
	  if (ustrncmp (DocumentURLs[source_doc], buffer, len))
	    fputs (buffer, fp_new);
	}
      fclose (fp_new);
      fclose (fp_old);
      
      /* rename the main index file */
      TtaFileUnlink (main_index_file_old);
      rename (main_index_file_new, main_index_file_old);
      /* remove the index file */
      TtaFileUnlink (doc_index);
    }

  TtaFreeMemory (main_index_file_old);
  TtaFreeMemory (main_index_file_new);
  TtaFreeMemory (doc_index);
}

/*-----------------------------------------------------------------------
  LINK_CreateMeta
  -----------------------------------------------------------------------*/

#ifdef __STDC__
AnnotMeta* LINK_CreateMeta (Document source_doc, Document annot_doc)
#else /* __STDC__*/
AnnotMeta* LINK_CreateMeta (source_doc, annot_doc)
Document source_doc;
Document annot_doc;
#endif /* __STDC__*/
{
  AnnotMeta   *annot;
  CHAR_T      *annot_user;
  
  /*
  **  Make a new annotation entry, add it to annotlist, and initialize it.
  */
  annot =  AnnotMeta_new ();
  if (!IsW3Path (DocumentURLs[annot_doc]) && 
      (!AnnotMetaData[source_doc].annotations 
       && !AnnotMetaData[source_doc].local_annot_loaded))
    {
      CHAR_T *annotIndex;

      /* download the local annotations, if they do exist, but mark them
	 invisible */
      annotIndex = LINK_GetAnnotationIndexFile (DocumentURLs[source_doc]);
      LINK_LoadAnnotationIndex (source_doc, annotIndex, FALSE);
      TtaFreeMemory (annotIndex);
      AnnotMetaData[source_doc].local_annot_loaded = TRUE;
    }

  List_add (&(AnnotMetaData[source_doc].annotations), (void *) annot);
  
  if (IsW3Path (DocumentURLs[source_doc])
      || IsFilePath(DocumentURLs[source_doc]))
    annot->source_url = TtaStrdup (DocumentURLs[source_doc]);
  else
    {
      annot->source_url = ANNOT_MakeFileURL (DocumentURLs[source_doc]);
    }

  /* get the current date... cdate = mdate at this stage */
  annot->cdate = StrdupDate ();
  annot->mdate = TtaStrdup (annot->cdate);

  /* Annotation type */
  annot->type = ANNOT_FindRDFResource (&annot_schema_list,
				       DEFAULT_ANNOT_TYPE,
				       FALSE);

  /* Annotation XPointer */
  annot->xptr = XPointer_build (source_doc, 1);

  annot_user = GetAnnotUser ();
  annot->author = TtaStrdup (annot_user);
  annot->content_type = TtaStrdup ("text/html");
  if (!IsW3Path (DocumentURLs[annot_doc]) 
      && !IsFilePath (DocumentURLs[annot_doc]))
    {
      annot->body_url = ANNOT_MakeFileURL (DocumentURLs[annot_doc]);
    }
  else
    annot->body_url = TtaStrdup (DocumentURLs[annot_doc]);
  
  /* create the reverse link name */
  LINK_CreateAName (annot);
  return annot;
}

/*-----------------------------------------------------------------------
  Procedure LINK_DelMetaFromMemory
  -----------------------------------------------------------------------
  Frees the memory used by the parsed metadata
  -----------------------------------------------------------------------*/
void LINK_DelMetaFromMemory (Document doc)
{
  if (!AnnotMetaData[doc].annotations)
    return;

  AnnotList_free (AnnotMetaData[doc].annotations);
  AnnotMetaData[doc].annotations = NULL;
  AnnotFilter_free (AnnotMetaData[doc].authors, List_delCharObj);
  AnnotMetaData[doc].authors = NULL;
  AnnotFilter_free (AnnotMetaData[doc].types, NULL);
  AnnotMetaData[doc].types = NULL;
  AnnotFilter_free (AnnotMetaData[doc].servers, List_delCharObj);
  AnnotMetaData[doc].servers = NULL;
}

/*-----------------------------------------------------------------------
   LINK_LoadAnnotationIndex
   -----------------------------------------------------------------------
   Searches for an annotation index related to the document. If it exists,
   it parses it and then, if the variable mark_visible is set true, adds
   links to them from the source document.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_LoadAnnotationIndex (Document doc, CHAR_T *annotIndex, ThotBool mark_visible)
#else /* __STDC__*/
void LINK_LoadAnnotationIndex (doc, annotIndex, mark_visible)
     Document doc;
     CHAR_T *annotIndex;
     ThotBool mark_visible;
#endif /* __STDC__*/
{
  View    view;
  Element body;
  List *annot_list, *list_ptr;
  AnnotMeta *annot;
  AnnotMeta *old_annot;

  if (!annotIndex || !(TtaFileExist (annotIndex)))
    /* there are no annotations */
    return;
  
  annot_list = RDF_parseFile (annotIndex, ANNOT_LIST);

  if (!annot_list)
    /* we didn't read any annotation */
    return;

  /* Insert the annotations in the body */
  view = TtaGetViewFromName (doc, "Formatted_view");
  body = SearchElementInDoc (doc, HTML_EL_BODY);
  
  list_ptr = annot_list;

  while (list_ptr)
    {
      annot = (AnnotMeta *) list_ptr->object;
      /* @@ JK: we need to do this operation,  but with the xptr */
#if 0
      if ((el = TtaSearchElementByLabel (annot->labf, body)) == NULL)
	{
	  fprintf (stderr, "This annotation has lost its parent!\n");
	  Annot_free (annot);
	}
      else 
#endif
	{
	  /* don't add an annotation if it's already on the list */
	  /* @@ JK: later, Ralph will add code to delete the old one.
	   We should remove the old annotations and preserve the newer
	   ones. Take into account that an anotation window may be open
	   and that we'll have to close it without saving... or just update
	   the metadata... */
	  
	  old_annot = AnnotList_searchAnnot (AnnotMetaData[doc].annotations,
					     annot->annot_url, AM_ANNOT_URL);
	  if (!old_annot)
	    {
	      if (mark_visible)
		{
		  /* create the reverse link name */
		  LINK_CreateAName (annot);
		  LINK_AddLinkToSource (doc, annot);
		  annot->is_visible = TRUE;
		}
	      else
		annot->is_visible = FALSE;
	      List_add (&AnnotMetaData[doc].annotations, (void*) annot);
	    }
	  else
	    Annot_free (annot);
	}
       List_delFirst (&list_ptr);
    }
}

/*-----------------------------------------------------------------------
   LINK_SelectSourceDoc
   Selects the text that was annotated in a document.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_SelectSourceDoc (Document doc, CONST CHAR_T *annot_url)
#else /* __STDC__*/
void LINK_SelectSourceDoc (doc, annot_url)
     Document doc;
     CHAR_T *annot_url;
#endif /* __STDC__*/
{
  XPointerContextPtr xptr_ctx;
  AnnotMeta *annot;
  CHAR_T *url;

  if (IsW3Path (annot_url) || IsFilePath (annot_url))
    url = annot_url;
  else
    url = ANNOT_MakeFileURL (annot_url);

  annot = AnnotList_searchAnnot (AnnotMetaData[doc].annotations,
				 url, AM_BODY_URL);
  if (url != annot_url)
    TtaFreeMemory (url);

    if (annot)
      {
	xptr_ctx = XPointer_parse (doc, annot->xptr);
	if (!xptr_ctx->error)
	  XPointer_select (xptr_ctx);
	else
	  fprintf (stderr, "LINK_SelectSource: impossible to set XPointer\n");
	XPointer_free (xptr_ctx);
      }
    else
      fprintf (stderr, "LINK_SelectSourceDoc: couldn't find annotation metadata\n");
}






