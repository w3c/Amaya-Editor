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

/*-----------------------------------------------------------------------
   LINK_GetAnnotationIndexFile 
   searches an entry in the main annot index file 
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
   Ajoute un lien de type annotation dans un document donne vers le
   document de nom annotName. Le lien est cree juste avant l'element
   target
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_AddLinkToSource (Document source_doc, AnnotMeta *annot)
#else /* __STDC__*/
void LINK_AddLinkToSource (source_doc, annot)
Document source_doc; 
AnnotMeta *annot;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       el, first, anchor;
  AttributeType attrType;
  Attribute     attr;
  CHAR_T       *annot_user;
  CHAR_T       *tmp;
  CHAR_T       server[MAX_LENGTH];
  int          c1, cN;

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

  /* create the anchor element */
  elType = TtaGetElementType (first);
  elType.ElTypeNum = HTML_EL_Anchor;
  anchor = TtaNewElement (source_doc, elType);

  /*
  ** insert the anchor 
  */

  /* is the user trying to annotate an anchor? */
  el = TtaGetTypedAncestor (first, elType);
  if (el)
    {
      /* yes, so we will add the anchor one element before
	in the struct tree */
      TtaInsertSibling (anchor, el, TRUE, source_doc);
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

  /* add the annotation attribute */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_IsAnnotation;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);  

  /* add the annotation icon */
  if (ustrcasecmp (annot->author, "tim"))
    attrType.AttrTypeNum = HTML_ATTR_AnnotationIcon1;
  else
    /* @@ aha, it's Tim */
    attrType.AttrTypeNum = HTML_ATTR_AnnotationIcon2;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);  

  /* add an HREF attribute pointing to the annotation */
  attrType.AttrTypeNum = HTML_ATTR_HREF_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  TtaSetAttributeText (attr, annot->body_url, anchor, source_doc);
  
  /* add a NAME attribute so that the annotation doc can point
     back to the source of the annotation link */
  attrType.AttrTypeNum = HTML_ATTR_NAME;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  /* set the anchor's name */
  TtaSetAttributeText (attr, annot->name, anchor, source_doc);
  TtaUnselect (source_doc);

  /* add the annotation to the filter list */
  AnnotFilter_add (&(AnnotMetaData[source_doc].types), annot->type);
  if (annot->annot_url)
    tmp = annot->annot_url;
  else
    tmp = annot->body_url;

  if (tmp)
    { /* @@ when creating a new annot, we don't yet know the URL;
         perhaps we should use the POST server name here? */
      GetServerName (tmp, server);
      AnnotFilter_add (&(AnnotMetaData[source_doc].servers), server);
    }
  else
    server[0] = WC_EOS;

  tmp = TtaGetMemory (ustrlen (annot->author) + ustrlen (server) + 4);
  usprintf (tmp, "%s@%s", annot->author, server);
  AnnotFilter_add (&(AnnotMetaData[source_doc].authors), tmp);
  TtaFreeMemory (tmp);
}

/*-----------------------------------------------------------------------
  LINK_RemoveLinkFromSource
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_RemoveLinkFromSource (Document source_doc, AnnotMeta *annot)
#else /* __STDC__*/
void LINK_RemoveLinkFromSource (source_doc, annot)
     Document source_doc;
     AnnotMeta *annot;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       anchor, elCour, elSuiv;
  AttributeType attrTypeAnnot, attrTypeName;
  Attribute     attrAnnot, attrName;
  int           lg;
  ThotBool       removeAnnot;

  CHAR_T *annotName = "xx";	/* @@ */
  CHAR_T *textName =  TtaGetMemory (200);

  elCour = SearchElementInDoc (source_doc, HTML_EL_BODY);
  elType.ElSSchema          = 
  attrTypeAnnot.AttrSSchema = 
  attrTypeName.AttrSSchema  = TtaGetSSchema ("HTML", source_doc);
  elType.ElTypeNum = HTML_EL_Anchor;
  attrTypeAnnot.AttrTypeNum = HTML_ATTR_IsAnnotation;
  attrTypeName.AttrTypeNum = HTML_ATTR_NAME;

  removeAnnot = FALSE;
  anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
  while (elCour && anchor && !removeAnnot)
  {
    /* Searches for an element of type anchor */
    if (anchor)
    {
      attrAnnot = TtaGetAttribute (anchor, attrTypeAnnot);
      attrName = TtaGetAttribute (anchor, attrTypeName);
      if (attrAnnot)
      {
        TtaGiveTextAttributeValue (attrName, textName, &lg);
        removeAnnot = !strcmp (textName, annotName);
	/* Suppress the annotation */
	if (attrAnnot && removeAnnot)
	  {
	    TtaRemoveTree (anchor, source_doc);
	    /* @and unlink the file, update the index ... */
	    break;
	  }
      }
      /* point to the next element */
      elCour = elSuiv = anchor;
      TtaNextSibling (&elSuiv);
      while ((elSuiv == NULL) && (elCour != NULL))
      {
        elCour = elSuiv = TtaGetParent (elCour);
        TtaNextSibling (&elSuiv);
      }
      elCour = elSuiv;
    }
    else
      elCour = anchor;
    anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
  }
}


/*-----------------------------------------------------------------------
   Procedure LINK_SaveLink (source_doc, annot_doc, labf, c1, labl, cN)
  -----------------------------------------------------------------------
   Writes the metadata describing an annotation file and its source
   link to an index file. This metadata consists of the
   name of the annotation file, the selected elements on the document
   (using thot labels for the moment), and the first and last characters.
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
  return;
}

/*-----------------------------------------------------------------------
  LINK_CreateMeta
  -----------------------------------------------------------------------*/

#ifdef __STDC__
AnnotMeta* LINK_CreateMeta (Document source_doc, Document annot_doc, CHAR_T *labf, int c1, CHAR_T *labl, int cl)
#else /* __STDC__*/
AnnotMeta* LINK_CreateMeta (source_doc, annot_doc, labf, c1, labl, cl)
     Document source_doc;
     Document annot_doc;
     CHAR_T *  labf;
     int      c1;
     CHAR_T *  labl;
     int      cl;
#endif /* __STDC__*/
{
  AnnotMeta   *annot;
  CHAR_T      *annot_user;

  /*
  **  Make a new annotation entry, add it to annotlist, and initialize it.
  */
  annot =  AnnotMeta_new ();
  List_add (&(AnnotMetaData[source_doc].annotations), (void *) annot);

  if (IsW3Path (DocumentURLs[source_doc])
      || IsFilePath(DocumentURLs[source_doc]))
    annot->source_url = TtaStrdup (DocumentURLs[source_doc]);
  else
    {
      annot->source_url = TtaAllocString (strlen (DocumentURLs[source_doc])+7);
      sprintf (annot->source_url, "file://%s", DocumentURLs[source_doc]);
    }

  ustrcpy (annot->labf, labf);
  annot->c1 = c1;
  ustrcpy (annot->labl, labl);
  annot->cl = cl;

  /* get the current date... cdate = mdate at this stage */
  annot->cdate = StrdupDate ();
  annot->mdate = TtaStrdup (annot->cdate);

  /* Annotation type */
  annot->type = TEXT("comment");

  /* Annotation XPointer */
  annot->xptr = XPointer_build (source_doc, 1);

  annot_user = GetAnnotUser ();
  annot->author = TtaStrdup (annot_user);
  annot->content_type = TtaStrdup ("text/html");
  annot->body_url = TtaStrdup (DocumentURLs[annot_doc]);

  /* memorize the anchor of the reverse link target */
  annot->name = TtaGetMemory (strlen (ANNOT_ANAME) + strlen (annot_user)
			      + (annot->body_url 
				 ? strlen (annot->body_url) : 0)
			      + 20);
  sprintf (annot->name, "%s_%s_%s", ANNOT_ANAME, annot_user,
	   annot->body_url ? annot->body_url : "");

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
  AnnotFilter_free (AnnotMetaData[doc].authors);
  AnnotMetaData[doc].authors = NULL;
  AnnotFilter_free (AnnotMetaData[doc].types);
  AnnotMetaData[doc].types = NULL;
  AnnotFilter_free (AnnotMetaData[doc].servers);
  AnnotMetaData[doc].servers = NULL;
}

/*-----------------------------------------------------------------------
   Procedure LINK_LoadAnnotationIndex (doc, char *annotIndex)
  -----------------------------------------------------------------------
   Searches for an annotation index related to the document. If it exists,
   it parses it and then displays all of its annotations
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_LoadAnnotationIndex (Document doc, CHAR_T *annotIndex)
#else /* __STDC__*/
void LINK_LoadAnnotationIndex (doc, annotIndex)
     Document doc;
     CHAR_T *annotIndex;
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
  /* @@@ possible memory bug */
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
	  /* @@ later, Ralph will add code to delete the old one */
	  old_annot = AnnotList_searchAnnot (AnnotMetaData[doc].annotations,
					     annot->annot_url, TRUE);
	  if (!old_annot)
	    {
	      LINK_AddLinkToSource (doc, annot);
	      List_add (&AnnotMetaData[doc].annotations, (void*) annot);
	    }
	  else
	    Annot_free (annot);
	}
      List_delFirst (&list_ptr);
    }
}

/***************************************************
 I've not yet used/cleaning the following legacy functions 
***************************************************/

/*-----------------------------------------------------------------------
   Procedure LINK_Remove (document, annotName)
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_Remove (Document document, CHAR_T *annotName)
#else /* __STDC__*/
void LINK_Remove (document, annotName)
     Document document;
     CHAR_T *  annotName;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       anchor, elCour, elSuiv;
  AttributeType attrTypeAnnot, attrTypeName;
  Attribute     attrAnnot, attrName;
  int           lg;
  ThotBool       removeAnnot;

  CHAR_T *textName =  TtaGetMemory (200);

  elCour = SearchElementInDoc (document, HTML_EL_BODY);
  elType.ElSSchema          = 
  attrTypeAnnot.AttrSSchema = 
  attrTypeName.AttrSSchema  = TtaGetSSchema ("HTML", document);
  elType.ElTypeNum = HTML_EL_Anchor;
  attrTypeAnnot.AttrTypeNum = HTML_ATTR_IsAnnotation;
  attrTypeName.AttrTypeNum = HTML_ATTR_NAME;

  removeAnnot = FALSE;
  anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
  while (elCour && anchor && !removeAnnot)
  {
    /* Searches for an element of type anchor */
    if (anchor)
    {
      attrAnnot = TtaGetAttribute (anchor, attrTypeAnnot);
      attrName = TtaGetAttribute (anchor, attrTypeName);
      if (attrAnnot)
      {
        TtaGiveTextAttributeValue (attrName, textName, &lg);
        removeAnnot = !strcmp (textName, annotName);
	/* Suppress the annotation */
	if (attrAnnot && removeAnnot)
	  {
	    TtaRemoveTree (anchor, document);
	    /* @and unlink the file, update the index ... */
	    break;
	  }
      }
      /* point to the next element */
      elCour = elSuiv = anchor;
      TtaNextSibling (&elSuiv);
      while ((elSuiv == NULL) && (elCour != NULL))
      {
        elCour = elSuiv = TtaGetParent (elCour);
        TtaNextSibling (&elSuiv);
      }
      elCour = elSuiv;
    }
    else
      elCour = anchor;
    anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
  }
}

#if 0
/*-----------------------------------------------------------------------
   Procedure LINK_SaveLink (doc, annotName, labf, c1, labl, cN)
  -----------------------------------------------------------------------
   Writes the metadata describing an annotation file and its source
   link to an index file. This metadata consists of the
   name of the annotation file, the selected elements on the document
   (using thot labels for the moment), and the first and last characters.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_SaveLink (Document document, CHAR_T *annotName, CHAR_T *labf, int c1, CHAR_T *labl, int cN)
#else /* __STDC__*/
void LINK_SaveLink (document, annotName, labf, c1, labl, cN)
     Document document;
     CHAR_T *  annotName;
     CHAR_T *  labf;
     int      c1;
     CHAR_T *  labl;
     int      cN;
#endif /* __STDC__*/
{
  FILE   *newLinkFile, *oldLinkFile;
  char   buffer[MAX_LENGTH];
  CHAR_T *newFileName;
  CHAR_T *oldFileName;

  /* Open the annotation index */
  oldFileName = LINK_GetAnnotationIndexFile (document);
  if (!oldFileName)
    {
      
    }

  newFileName = TtaGetMemory (strlen (oldFileName) + 5);
  sprintf (newFileName, "%s.new", oldFileName);
  oldLinkFile = fopen (oldFileName, "r");

  if (oldLinkFile)
    {
      /* Copy the existing links */
      while (fgets (buffer, MAX_LENGTH, oldLinkFile))
	fprintf (newLinkFile, buffer);
      fclose (oldLinkFile);
    }
  
  /* Add the new link */
  fprintf (newLinkFile, "%s|%s|%d|%s|%d\n", annotName, labf, c1, labl, cN);

  fclose (newLinkFile);
  TtaFileCopy (newFileName, oldFileName);
  TtaFileUnlink (newFileName);
  TtaFreeMemory (newFileName);
  TtaFreeMemory (oldFileName);
}
#endif

/*-----------------------------------------------------------------------
   Procedure LINK_RemoveLink (document, annotName)
  -----------------------------------------------------------------------
   Enleve le lien du document d'annotation dans le fichier de liens
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_RemoveLink (Document document, CHAR_T *annotName)
#else /* __STDC__*/
void LINK_RemoveLink (document, annotName)
     Document document;
     CHAR_T *  annotName;
#endif /* __STDC__*/
{
  CHAR_T   *newFileName, *oldFileName, *docName, *userName, *labf, *labl;
  FILE     *newLinkFile, *oldLinkFile;
  int      nbAnnot, c1, cN, i;

  printf ("(LINK_RemoveLink) DEBUT\n");

  /* Ouverture fichier de liens */
  oldFileName = TtaGetMemory (100);
  sprintf (oldFileName, "%s/%s", GetAnnotDir (), ANNOT_INDEX_SUFFIX);
  newFileName = TtaGetMemory (100);
  sprintf (newFileName, "%s.new", oldFileName);

#if 0
  oldLinkFile = fileOpen (oldFileName);
  newLinkFile = fileCreateAndOpen (newFileName);
#endif
  
  /* Mise a jour du nombre d'annotations */
  fscanf (oldLinkFile, "%d\n", &nbAnnot);
  nbAnnot--;
  fprintf (newLinkFile, "%d\n", nbAnnot);

  /* On recopie les liens deja existants */
  annotName = TtaGetMemory (MAX_LENGTH);
  userName = TtaGetMemory (MAX_LENGTH);
  labf = TtaGetMemory (10);
  labl = TtaGetMemory (10);
  for (i=0 ; i<nbAnnot ; i++)
  {
    fscanf (oldLinkFile, "%s %s %s %d %s %d\n", userName, docName, labf, &c1, labl, &cN);
    if (strcmp (docName, annotName))
      fprintf (newLinkFile, "%s|%s|%d|%s|%d\n", docName, labf, c1, labl, cN);
  }

  /* Fermeture du fichier de liens */
#if 0
  fileClose (oldLinkFile, oldFileName);
  fileClose (newLinkFile, newFileName);
#endif
  TtaFileCopy (newFileName, oldFileName);
  TtaFileUnlink (newFileName);

  printf ("(LINK_RemoveLink) FIN\n");
}

/*-----------------------------------------------------------------------
   Procedure LINK_HideAnnotations (document)
  -----------------------------------------------------------------------
   Enleve tous les liens d'annotation dans un document donne
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_HideAnnotations (Document document)
#else /* __STDC__*/
void LINK_HideAnnotations (document)
     Document document;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       anchor, elCour, elSuiv;
  AttributeType attrType;
  Attribute     attr;
  CHAR_T *       text;
  int           *text_lg;
  ThotBool       removeAnnot;

  elCour = SearchElementInDoc (document, HTML_EL_BODY);
  elType.ElSSchema = TtaGetSSchema ("HTML", document);
  elType.ElTypeNum = HTML_EL_Anchor;
  attrType.AttrSSchema = TtaGetSSchema ("HTML", document);
  attrType.AttrTypeNum = HTML_ATTR_Class;
  text = TtaGetMemory (200);
  text_lg = TtaGetMemory (4);

  while (elCour && anchor)
  {
    /* Recherche d'un element de type anchor */
    anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
    if (anchor)
    {
      attr = TtaGetAttribute (anchor, attrType);
      if (attr)
      {
        TtaGiveTextAttributeValue (attr, text, text_lg);
        removeAnnot = !strcmp (text, "Annotation");
      }
      /* Passage a l'element suivant */
      elCour = elSuiv = anchor;
      TtaNextSibling (&elSuiv);
      while ((elSuiv == NULL) && (elCour != NULL))
      {
        elCour = elSuiv = TtaGetParent (elCour);
        TtaNextSibling (&elSuiv);
      }
      elCour = elSuiv;
      /* Suppression de l'annotation */
      if (attr && removeAnnot)
      {
        TtaRemoveTree (anchor, document);
      }
    }
  }
}

/*-----------------------------------------------------------------------
   Procedure LINK_ParcoursAnchor (document)
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_ParcoursAnchor (Document document)
#else /* __STDC__*/
void LINK_ParcoursAnchor (document)
     Document document;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     body, elCour;

  CHAR_T *anchorClass = TtaGetMemory (20);
  CHAR_T *anchorName  = TtaGetMemory (100);

  body = SearchElementInDoc (document, HTML_EL_BODY);
  elType = TtaGetElementType (body);
  elType.ElTypeNum = HTML_EL_Anchor;
  elCour = TtaSearchTypedElement (elType, SearchForward, body);
  while (elCour != NULL)
  {
    anchorClass = SearchAttributeInEl (document, elCour, HTML_ATTR_Class,
				       TEXT("HTML"));
    if (anchorClass == NULL)
      printf ("Pas une annotation\n");
    else
    {
      anchorName = SearchAttributeInEl (document, elCour, HTML_ATTR_NAME, 
					TEXT("HTML"));
      printf ("ANNOTATION : %s\n", anchorName);
    }
    elCour = TtaGetSuccessor (elCour);
    elCour = TtaSearchTypedElement (elType, SearchForward, elCour);
  }
}

/*-----------------------------------------------------------------------
   Procedure LINK_UpdateAnnotations (document)
  -----------------------------------------------------------------------
   Rafraichit l'affichage des liens d'un document : les liens existants
   sont laisses, les liens manquants sont rajoutes.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_UpdateAnnotations (Document document)
#else /* __STDC__*/
void LINK_UpdateAnnotations (document)
     Document document;
#endif /* __STDC__*/
{
  Element       body, el;
  View          view;
  CHAR_T        *fileName, *docName, *prem, *der;
  FILE          *linkFile;
  int            nbAnnot, c_prem, c_der, i;

  /* Ouverture fichier de liens */
  fileName = TtaGetMemory (100);
  sprintf (fileName, "%s/%s", GetAnnotDir (), ANNOT_INDEX_SUFFIX);
#if 0
  if ((linkFile = fileOpen (fileName)) == NULL)
    return;
#endif
  /* Lecture du nombre d'annotations */
  fscanf (linkFile, "%d\n", &nbAnnot);

  /* Parcours et affichage de la liste de liens */
  docName = TtaGetMemory (50);
  prem    = TtaGetMemory (10);
  der     = TtaGetMemory (10);
  view = TtaGetViewFromName (document, "Formatted_view");
  body = SearchElementInDoc (document, HTML_EL_BODY);
  for (i=0 ; i<nbAnnot ; i++)
  {
    fscanf (linkFile, "%s %s %d %s %d\n", docName, prem, &c_prem, der, &c_der);
    if ((el = TtaSearchElementByLabel (prem, body)) == NULL)
    {
      printf ("Cette pauvre petite annotation est orpheline....\n");
    }
    else if (!SearchAnnotation (document, docName))
    {
#if 0
      LINK_AddLinkToSource (document, view, el, docName);
#endif
    }
  }
#if 0
  fileClose (linkFile, fileName);
#endif
}

#if 0
/*-----------------------------------------------------------------------
   Procedure LINK_AddMetaToMemory
  -----------------------------------------------------------------------
  Copies the parsed metadata to memory
  -----------------------------------------------------------------------*/
void LINK_AddMetaToMemory (Document doc, CHAR_T *annotUser, CHAR_T *annotDate, CHAR_T *annotType, CHAR_T *body_url)
{
  AnnotMeta *me;

  /* create a new element */
  me = TtaGetMemory (sizeof (AnnotMeta));
  me->author = TtaStrdup (annotUser);
  me->cdate = TtaStrdup (annotDate);
  me->type = TtaStrdup (annotType);
  me->body_url = TtaStrdup (body_url);

  /* add it to the list structure */
  if (!AnnotMetaData[doc]) 
    {
      /* the list was empty */
      me->next = NULL;
      AnnotMetaData[doc] = me;
    }
  else
    {
      /* adding new elements to the list */
      me->next = AnnotMetaDataList[doc];
      AnnotMetaDataList[doc] = me;
    }
}

#endif
