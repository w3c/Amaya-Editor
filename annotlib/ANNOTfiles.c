/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTfiles.c : module for administrating annotation files.
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

/* annotlib includes */
#include "annotlib.h"

#include "ANNOTfiles_f.h"
#include "ANNOTlink_f.h"
#include "ANNOTschemas_f.h"
#include "ANNOTevent_f.h"
#include "ANNOTtools_f.h"

/* Amaya includes */
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "HTMLactions_f.h"
#include "HTMLsave_f.h"
#include "uconvert.h"

/* schema includes */
#include "Annot.h"

/*-----------------------------------------------------------------------
   Procedure ANNOT_SetPath (document)
  -----------------------------------------------------------------------
   Ajoute le repertoire du document d'annotations a la liste des
   repertoires de documents
  -----------------------------------------------------------------------*/
void ANNOT_SetPath (Document document)
{
  char  *dirList = (char *)TtaGetMemory (2000);

  TtaGetDocumentPath (dirList, 2000);
  strcat (dirList, ":");
  strcat (dirList, GetAnnotDir ());
  TtaSetDocumentPath (dirList);
}

/*-----------------------------------------------------------------------
  ANNOT_NewDocument
  Creates an annotation file for the current document. Returns the number
  of the created document or 0 in case of failure
  -----------------------------------------------------------------------*/
Document ANNOT_NewDocument (Document doc, AnnotMode mode)
{
  Document    annotDoc;
  char       *annot_dir;
  char       *fname;
  char       *docname;
  char       *tmpname;
  const char *tmp;

  /* Create a new annotation document */
  annot_dir = GetAnnotDir ();
  docname = TtaGetDocumentName (doc);
  tmpname = GetTempName (annot_dir, "annot");

  if (!tmpname) 
    {
     fprintf (stderr, "(ANNOT_NewDocument) ERROR : tmpnam couldn't create the annotation file\n");
     return 0;
    }
  
  fname = (char *)TtaGetMemory (strlen (tmpname) + 20);
  sprintf (fname, "%s.html", tmpname);
  TtaFreeMemory (tmpname);

  /* "annot is the title of the window */
  if (mode & ANNOT_isReplyTo)
    tmp = "reply to annotation";
  else
    tmp = "annotation";

  annotDoc = InitDocAndView (doc,
                             FALSE /* replaceOldDoc */,
                             TRUE /* inNewWindow */,
                             tmp, docAnnot, 0, FALSE, L_Annot, 0, CE_ABSOLUTE);

  if (annotDoc == 0) 
    {
      fprintf (stderr, "(ANNOT_NewDocument) ERROR : couldn't create the annotation file\n");
      TtaFreeMemory (fname);
    }
  else
    {
      /* intialize the (amaya) metadata related to a document */
      if (DocumentURLs[annotDoc])
        TtaFreeMemory (DocumentURLs[annotDoc]);
      DocumentURLs[annotDoc] = fname;
      DocumentMeta[annotDoc] = DocumentMetaDataAlloc ();
      DocumentMeta[annotDoc]->method = CE_ABSOLUTE;
      DocumentMeta[annotDoc]->xmlformat = TRUE;
      DocumentSource[annotDoc] = 0;
      /* update menu entries */
      UpdateEditorMenus (annotDoc);
      /* set the charset to be UTF-8 by default */
      TtaSetDocumentCharset (annotDoc, TtaGetCharset ("UTF-8"), FALSE);
      DocumentMeta[annotDoc]->charset = TtaStrdup ("UTF-8");
      DocumentMeta[annotDoc]->content_type = TtaStrdup (AM_XHTML_MIME_TYPE);
      
      /* activate, remove buttons from the annot view */
      ResetStop (annotDoc);
      ANNOT_PrepareAnnotView (annotDoc);
    }  
  return annotDoc;
}

/*-----------------------------------------------------------------------
  GetMetaDataInAnnotList
  If it exists, it returns the annotation object from the annot_list that
  corresponds to the document_url. Otherwise, it returns NULL.
  -----------------------------------------------------------------------*/
static AnnotMeta *GetMetaDataInAnnotList (char *doc_url, List *annot_list)
{
  List *ptr;
  AnnotMeta *annot = NULL;
  char *annot_url;
  char *tmp_doc_url = NULL;
  char *body_url;

  if (!IsW3Path (doc_url) && !IsFilePath (doc_url))
    tmp_doc_url = LocalToWWW (doc_url);
  else
    tmp_doc_url = doc_url;

  /* get a pointer to the annot list */
  ptr = annot_list;
  while (ptr)
    {
      annot = (AnnotMeta *) ptr->object;

      if (annot->annot_url)
	annot_url = annot->annot_url;
      else
	annot_url = NULL;

      /* RRS: newly created local annotations have only a body URI */
      if (annot->body_url)
	body_url = annot->body_url;
      else
	body_url = NULL;

      /* @@ maybe we could add calls to NormalizeFile here */
      if ((annot_url && !strcasecmp (tmp_doc_url, annot_url))
	  || (body_url && !strcasecmp (tmp_doc_url, body_url)))
	break;
      ptr = ptr->next;
    }
  
  if (tmp_doc_url != doc_url)
      TtaFreeMemory (tmp_doc_url);

  if (ptr)
    return annot;
  else
    return NULL;
}

/*-----------------------------------------------------------------------
  GetMetaData
  returns the annotation object that corresponds to the annotation
  doc_annot of document doc.
  -----------------------------------------------------------------------*/
AnnotMeta *GetMetaData (Document doc, Document doc_annot)
{
  AnnotMeta *annot = NULL;

  /* search the annotation list */
  annot = GetMetaDataInAnnotList (DocumentURLs[doc_annot], 
				 AnnotMetaData[doc].annotations);

  if (annot)
    return annot;

#ifdef ANNOT_ON_ANNOT
  if (AnnotMetaData[doc].thread)
    /* search the thread list associated to the annotation document */
    annot = GetMetaDataInAnnotList (DocumentURLs[doc_annot], 
				    AnnotMetaData[doc].thread->annotations);

  if (annot)
    return annot;
#endif /* ANNOT_ON_ANNOT */

  return NULL;
}

/*-----------------------------------------------------------------------
  ANNOT_LoadAnnotation
  If an annotation has already been loaded, we open a window and show
  its body.
  (@@ JK: weird function!)
  -----------------------------------------------------------------------*/
void ANNOT_LoadAnnotation (Document doc, Document docAnnot)
{
  AnnotMeta *annot;

  /* if it's a new annotation, we compute the stuff, otherwise,
     we copy it from the existing metadata */
  annot = GetMetaData (doc, docAnnot);
  if (annot)
    {
      ANNOT_InitDocumentStructure (doc, docAnnot, annot, ANNOT_initNone);
      /* save the annot_url to which we should reply (or annotate)
	 this annotation */
      if (AnnotMetaData[docAnnot].annot_url)
	TtaFreeMemory (AnnotMetaData[docAnnot].annot_url);
      /* Locally stored annotations have a parser generated URI, that
	 changes each time they are parsed. We use the body URL instead
	 to reply to them.
	 Remote annotations have a correct url, so we use it. When
         pubishing an annotation, we'll do the necessary changes */
      if (annot->annot_url && IsW3Path (annot->annot_url))
	AnnotMetaData[docAnnot].annot_url = TtaStrdup (annot->annot_url);
      else
	AnnotMetaData[docAnnot].annot_url = TtaStrdup (annot->body_url);
    }
}

/*-----------------------------------------------------------------------
  ANNOT_ReloadAnnotMeta
  -----------------------------------------------------------------------*/
void ANNOT_ReloadAnnotMeta (Document annotDoc)
{
  Document source_doc; 
  AnnotMeta *annot;

  source_doc = DocumentMeta[annotDoc]->source_doc;
  if (source_doc == 0)
    return;
  annot = GetMetaData (source_doc, annotDoc);
  if (!annot)
    return;
  /* clear the list of annotations, threads, and filters related to
     this annotation, as it is a reload */
  ANNOT_FreeDocumentResource (annotDoc);
  /* initialize the meta data */
  if (annot->annot_url && IsW3Path (annot->annot_url))
    AnnotMetaData[annotDoc].annot_url = TtaStrdup (annot->annot_url);
  else
    AnnotMetaData[annotDoc].annot_url = TtaStrdup (annot->body_url);

  ANNOT_InitDocumentStructure (source_doc, annotDoc, annot, ANNOT_initNone);
}

/*-----------------------------------------------------------------------
  ANNOT_InitDocumentMeta
  Initializes an annotation document by adding a BODY part
  and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
void ANNOT_InitDocumentMeta (Document doc, Document docAnnot, AnnotMeta *annot,
			      char *source_doc_title)
{
  ElementType    elType;
  Element        root, head, el;
  Attribute      attr;
  AttributeType  attrType;
  char          *user;
  char          *doc_anchor;
  char          *source_url;
  char          *cdate;
  char          *mdate;
  const char    *type;
#ifdef ANNOT_ON_ANNOT
  Document       thread_doc;
#endif /* ANNOT_ON_ANNOT */

  user = annot->author;
  source_url = annot->source_url;
  cdate = annot->cdate;
  mdate = annot->mdate;

  if (annot->type)
      type = ANNOT_GetLabel(&annot_schema_list, annot->type);
  else
      type = ""; /* All local annotations should have a type, but
			  a service may forget to give us a type. */

  /* save the docid of the annotated document */
#ifdef ANNOT_ON_ANNOT
  /* for annotations, the source document is the document where the thread is found */
  thread_doc = 0;
  if (AnnotMetaData[docAnnot].thread)
    thread_doc = AnnotThread_searchThreadDoc (DocumentURLs[docAnnot]);
  if (thread_doc > 0)
    DocumentMeta[docAnnot]->source_doc = thread_doc;
  else
#endif /* ANNOT_ON_ANNOT */
    DocumentMeta[docAnnot]->source_doc = doc;

  /*
  ** initialize the METADATA 
  */

  /* point to the first node */
  root = TtaGetRootElement (docAnnot);
  elType = TtaGetElementType (root);

  /* specify if the annotation body is an XML document. We systematically
   convert HTML into XHTML when saving */
  if (DocumentMeta[docAnnot]->xmlformat || annot->bodyType == docHTML)
    Annot_SetXMLBody (docAnnot);

  /* point to the metadata structure */
  elType.ElTypeNum = Annot_EL_Description;
  head = TtaSearchTypedElement (elType, SearchInTree, root);

  /* author metadata */
  elType.ElTypeNum = Annot_EL_Author;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, (unsigned char *)user, TtaGetDefaultLanguage (), docAnnot); 
  if (annot->creator)
    {
      RDFStatementP s;
      char *tmp;

      s = ANNOT_FindRDFStatement (annot->creator->statements, PROP_firstName);
      if (s)
	{
	  tmp = (char *) s->object->name;
	  elType.ElTypeNum = Annot_EL_CreatorGivenName;
	  el = TtaSearchTypedElement (elType, SearchInTree, head);
	  el = TtaGetFirstChild (el);
	  TtaSetTextContent (el, (unsigned char *)tmp,
			     TtaGetDefaultLanguage (),
			     docAnnot);
	}

      s = ANNOT_FindRDFStatement (annot->creator->statements, PROP_name);
      if (s)
	{
	  tmp = (char *) s->object->name;
	  elType.ElTypeNum = Annot_EL_CreatorFamilyName;
	  el = TtaSearchTypedElement (elType, SearchInTree, head);
	  el = TtaGetFirstChild (el);
	  TtaSetTextContent (el, (unsigned char *) tmp,
			     TtaGetDefaultLanguage (),
			     docAnnot);
	}

      s = ANNOT_FindRDFStatement (annot->creator->statements, PROP_Email);
      if (s)
	{
	  elType.ElTypeNum = Annot_EL_CreatorEmail;
	  el = TtaSearchTypedElement (elType, SearchInTree, head);
	  el = TtaGetFirstChild (el);
	  TtaSetTextContent (el, (unsigned char *) s->object->name,
			     TtaGetDefaultLanguage (),
			     docAnnot);
	}
    }

  /* title metadata */
  if (annot->title)
    {
      elType.ElTypeNum = Annot_EL_ATitle;
      el = TtaSearchTypedElement (elType, SearchInTree, head);
      el = TtaGetFirstChild (el);
      TtaSetTextContent (el, (unsigned char *)annot->title, TtaGetDefaultLanguage (), 
			 docAnnot); 
    }
  
  /* Creation Date metadata */
  elType.ElTypeNum = Annot_EL_AnnotCDate;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, (unsigned char *)cdate, TtaGetDefaultLanguage (), docAnnot); 

  /* Last modified date metadata */
  elType.ElTypeNum = Annot_EL_AnnotMDate;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, (unsigned char *)mdate, TtaGetDefaultLanguage (), docAnnot); 

  /* Source doc metadata (add a link to the annoted paragraph itself) */
  elType.ElTypeNum = Annot_EL_SourceDoc;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  /* add a reverse link to the source document */	
  attrType.AttrSSchema = TtaGetDocumentSSchema (docAnnot);
  attrType.AttrTypeNum = Annot_ATTR_HREF_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, docAnnot);

  doc_anchor = (char *)TtaGetMemory (strlen (annot->source_url)
			     + strlen (annot->name) 
			     + 20);
  sprintf (doc_anchor, "%s#%s", annot->source_url, annot->name);
  TtaSetAttributeText (attr, doc_anchor, el, docAnnot);
  TtaFreeMemory (doc_anchor);

  /* use the source_doc_title parameter as the value of the source
     document field */
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, (unsigned char *)source_doc_title,
		     TtaGetDefaultLanguage (), docAnnot);
  /* RDF type metadata */
  elType.ElTypeNum = Annot_EL_RDFtype;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  /* to set up the text (reply or annotation type), we use an attribute in this
   element */
  if (annot->inReplyTo)
    {
      attrType.AttrTypeNum = Annot_ATTR_isReply;
      attr = TtaGetAttribute (el, attrType);
      TtaSetAttributeValue (attr, Annot_ATTR_isReply_VAL_YES_, el, docAnnot);
    }
  /* set the rdf type value itself */
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, (unsigned char *)type,
		     TtaGetDefaultLanguage (), docAnnot);
}

/*-----------------------------------------------------------------------
  ANNOT_InitDocumentBody 
  Initializes an annotation document by adding a BODY part
  and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
void  ANNOT_InitDocumentBody (Document docAnnot, char *source_doc_title)
{
  ElementType elType;
  Element     root, head, body, el, child;
  char     *tmp;

  /*
  ** HTML initialization
  */
  /* we first add the HTML tree */
  ANNOT_CreateBodyTree (docAnnot, docHTML);

  /* we find the the HTML nature */
  root = TtaGetRootElement (docAnnot);
  elType = TtaGetElementType (root);
  head = TtaSearchTypedElement (elType, SearchInTree, root);
  elType.ElTypeNum = Annot_EL_Body;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  /* then move the root variable so that it points to the beginning of the
     HTML document */
  el = TtaGetFirstChild (el);
  root = el;
  /* and change the elType to HTML */
  elType = TtaGetElementType (root);

  /* memorize the head */
  elType.ElTypeNum = HTML_EL_HEAD;
  head = TtaSearchTypedElement (elType, SearchInTree, root);

  /* Add a document title */
  elType.ElTypeNum = HTML_EL_TITLE;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  el = TtaGetFirstChild (el);
  /* @@ maybe parse the URL here */
  tmp = (char *)TtaGetMemory (strlen (source_doc_title)
		      + sizeof ("Annotation of ") + 1);
  sprintf (tmp, "Annotation of %s", source_doc_title);
  TtaSetTextContent (el, (unsigned char *)tmp,
		     TtaGetDefaultLanguage (), docAnnot);
  TtaFreeMemory (tmp);
  /* add a document URL */
  elType.ElTypeNum = HTML_EL_Document_URL;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, (unsigned char *)DocumentURLs[docAnnot],
		     TtaGetDefaultLanguage (), docAnnot);
  
  /* create a META element in the HEAD with attributes name="GENERATOR" */
  /* and content="Amaya" */
  child = TtaGetLastChild (head);

  
  /* Create the BODY */
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchInTree, root);
  if (!body)
    {
      body = TtaNewElement (docAnnot, elType);
      TtaInsertSibling (body, head, FALSE, docAnnot);
      elType.ElTypeNum = HTML_EL_Paragraph;
      el = TtaNewTree (docAnnot, elType, "");
      TtaInsertFirstChild (&el, body, docAnnot);
    }
  // now select within the annottion
  child = body;
  do
  {
	el = child;
    child = TtaGetLastChild (el);
  }
  while (child);
  TtaSelectElement (docAnnot, el);
}

#ifdef ANNOT_ON_ANNOT
/*-----------------------------------------------------------------------
  ANNOT_ThreadItem_new
  -----------------------------------------------------------------------*/
static Element ANNOT_ThreadItem_new (Document doc)
{
  Element thread_item;
  ElementType elType;
  
  /* create the new item */
  elType.ElSSchema =  TtaGetSSchema ("Annot", doc);
  elType.ElTypeNum = Annot_EL_Thread_item;
  thread_item =  TtaNewTree (doc, elType, "");

  /* return the element we just created */
  return thread_item;
}
#endif /* ANNOT_ON_ANNOT */

#ifdef ANNOT_ON_ANNOT
/*-----------------------------------------------------------------------
  ANNOT_ThreadItem_init
  Inits a thread item default fields.
  If useSource == TRUE, we're initializing the thread, so the first
  item points to the root of thread.
  -----------------------------------------------------------------------*/
static void ANNOT_ThreadItem_init (Element thread_item, Document doc,
				   AnnotMeta *annot_doc, ThotBool useSource)
{
  Element             el;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  char               *tmp;
  AnnotMeta          *annot;

  if (useSource && DocumentMeta[doc]->source_doc > 0)
    annot = GetMetaData (DocumentMeta[doc]->source_doc, doc);
  else
    annot = annot_doc;
  
  if (!annot)
    tmp = NULL;

  attrType.AttrSSchema = elType.ElSSchema = TtaGetSSchema ("Annot", doc);
  /*
  **  initialize the reverse link 
  */
  attrType.AttrTypeNum = Annot_ATTR_HREF_;
  /* remove the previous reverse link if it exists already */
  attr = TtaGetAttribute (thread_item, attrType);
  if (attr)
    TtaRemoveAttribute (thread_item, attr, doc);
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (thread_item, attr, doc);
  TtaSetAttributeText (attr, annot->body_url, thread_item, doc);

  /* initialize the Annot_HREF_. This attribute helps
     sort the thread items in conjunction with the inreplyto URL */
  attrType.AttrTypeNum = Annot_ATTR_Annot_HREF_;
  /* remove the previous reverse link if it exists already */
  attr = TtaGetAttribute (thread_item, attrType);
  if (attr)
    TtaRemoveAttribute (thread_item, attr, doc);
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (thread_item, attr, doc);
  if (annot->annot_url && !IsFilePath (annot->annot_url))
    tmp = annot->annot_url;
  else
    tmp = annot->body_url;
  TtaSetAttributeText (attr, tmp, thread_item, doc);

  /* mark the thread item as orphan or not */
  if (annot->is_orphan_item)
    {
      attrType.AttrTypeNum = Annot_ATTR_Orphan_item;
      /* remove the previous reverse link if it exists already */
      attr = TtaGetAttribute (thread_item, attrType);
      if (!attr)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (thread_item, attr, doc);
	}
      TtaSetAttributeValue (attr, Annot_ATTR_Orphan_item_VAL_Yes_, thread_item, doc);
    }

  /* put the type of the annotation */
  elType.ElTypeNum = Annot_EL_TI_Type;
  el = TtaSearchTypedElement (elType, SearchInTree, thread_item);
  if (el)
    {
      if (annot && annot->type)
	{
	  el = TtaGetFirstChild (el);
      	  tmp =  ANNOT_GetLabel(&annot_schema_list, annot->type);
	  TtaSetTextContent (el, (unsigned char *)tmp, TtaGetDefaultLanguage (), doc);
	}
    }

  /* put the title of the annotation */
  elType.ElTypeNum = Annot_EL_TI_Title;
  el = TtaSearchTypedElement (elType, SearchInTree, thread_item);
  if (el)
    {
      el = TtaGetFirstChild (el);
      if (annot)
	tmp = annot->title;
      TtaSetTextContent (el, (unsigned char *)((tmp) ? tmp :  "no title"), 
			 TtaGetDefaultLanguage (), doc);
    }

  /* put the author of the annotation */
  elType.ElTypeNum = Annot_EL_TI_Author;
  el = TtaSearchTypedElement (elType, SearchInTree, thread_item);
  if (el)
    {
      el = TtaGetFirstChild (el);
      if (annot)
	tmp = annot->author;
      if (tmp)
	TtaSetTextContent (el, (unsigned char *)tmp, TtaGetDefaultLanguage (), doc);
    }

  /* put the date of the annotation */
  elType.ElTypeNum = Annot_EL_TI_Date;
  el = TtaSearchTypedElement (elType, SearchInTree, thread_item);
  if (el)
    {
      el = TtaGetFirstChild (el);
      if (annot)
	tmp = annot->mdate;
      if (tmp)
	TtaSetTextContent (el, (unsigned char *)tmp, TtaGetDefaultLanguage (), doc);
    }
}
#endif /* ANNOT_ON_ANNOT */

/*-----------------------------------------------------------------------
  ANNOT_AddThreadItem
  Experiment the S/P threads.

  root = element to which we want to attach the item.
  AsSibling = insert it as a sibling of root.

  If root == NULL, we find the thread element. If this one doesn't
  exist we create it and consider this item as the first one in the list.
  If the thread element exists, we will use the last child of the thread 
  as root.
  -----------------------------------------------------------------------*/
Element ANNOT_AddThreadItem (Document doc, AnnotMeta *annot)
{
#ifdef ANNOT_ON_ANNOT
  ElementType    elType;
  Element        root, thread_item, el, rootOfThread;
  Attribute      attr;
  AttributeType  attrType;
  char          *url;
  int            i;

  /* we find the the Thread element and make it our root */
  root = TtaGetRootElement (doc);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = Annot_EL_Thread;
  el = TtaSearchTypedElement (elType, SearchInTree, root);

  if (el)
    rootOfThread = el;
  else
    {
      /* create the thread element */
      elType.ElTypeNum = Annot_EL_Body;
      el = TtaSearchTypedElement (elType, SearchInTree, root);
      if (!el)
	return NULL; /* couldn't find a body! */
      elType.ElTypeNum = Annot_EL_Thread;
      rootOfThread =  TtaNewTree (doc, elType, "");
      /* insert the thread */
      TtaInsertSibling (rootOfThread, el, FALSE, doc);
      /* and initialize the root of the thread */
      el = TtaGetFirstChild (rootOfThread);
      ANNOT_ThreadItem_init (el, doc,  annot, TRUE);
    }

  /* try to find where to insert the element */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Annot_ATTR_Annot_HREF_;
  root = rootOfThread;
  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
  while (el)
    {
      i = TtaGetTextAttributeLength (attr) + 1;
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      if (!strcasecmp (url, annot->inReplyTo))
	{
	  TtaFreeMemory (url);
	  break;
	}
      TtaFreeMemory (url);
      root = el;
      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
    }
  
  if (el)
    root = el;
  else
    /* we didn't find the ReplyTo, so we insert it as a child of the root 
     @@ and maybe add an attribute (unknown thread... ) */
    root = TtaGetFirstChild (rootOfThread);
      
  /* find the container and insert it */
  el = TtaGetLastChild (root);
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != Annot_EL_TI_content)
    {
      /* there's no container, create it */
      root = el;
      elType.ElTypeNum = Annot_EL_TI_content;
      el = TtaNewElement (doc, elType);
      TtaInsertSibling (el, root, FALSE, doc);
    }
  /* create the thread_item */
  thread_item = ANNOT_ThreadItem_new (doc);
  /* and insert it as the last child of the above container */
  /* JK: @@ we should use sort here the thread item by date too */
  root = el;
  el = TtaGetLastChild (root);
  if (el)
    TtaInsertSibling (thread_item, el, FALSE, doc);
  else
    TtaInsertFirstChild (&thread_item, root, doc);

  /* init the thread item elements */
  ANNOT_ThreadItem_init (thread_item, doc,  annot, FALSE);

  return (thread_item);
#else
  return 0;
#endif /* ANNOT_ON_ANNOT */
}

/*-----------------------------------------------------------------------
  ANNOT_DeleteThread
  Experiment the S/P threads.

  Deletes the thread of thread_doc, if it exists
  -----------------------------------------------------------------------*/
void ANNOT_DeleteThread (Document thread_doc)
{
  Element el;
  ElementType elType;

  if (DocumentTypes[thread_doc] == docAnnot)
    {
      /* we find the the Thread element and make it our root */
      el = TtaGetRootElement (thread_doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = Annot_EL_Thread;
      el = TtaSearchTypedElement (elType, SearchInTree, el);
      if (el)
	TtaDeleteTree (el, thread_doc);
    }
}

/*-----------------------------------------------------------------------
  ANNOT_BuildThread
  Experiment the S/P threads.

  Builds a thread on thread_doc
  -----------------------------------------------------------------------*/
void ANNOT_BuildThread (Document thread_doc)
{
#ifdef ANNOT_ON_ANNOT
  List *annot_list;
  AnnotMeta *annot;

  annot_list = AnnotThread[thread_doc].annotations;
  /* sort the thread */
  AnnotThread_sortThreadList (&annot_list);
  AnnotThread[thread_doc].annotations = annot_list;

  /* erase previous thread */
  while (annot_list)
    {
      annot = (AnnotMeta *) annot_list->object;
      ANNOT_AddThreadItem (thread_doc, annot);
      annot_list = annot_list->next;
    }
#endif /* ANNOT_ON_ANNOT */
}

/*-----------------------------------------------------------------------
  ANNOT_ToggleThread
  Selects an item in the thread view.
  -----------------------------------------------------------------------*/
void ANNOT_ToggleThread (Document thread_doc, Document annot_doc, 
			 ThotBool turnOn)
{
#ifdef ANNOT_ON_ANNOT
  ElementType    elType;
  Element        root, el, ti_desc;
  Attribute      attr;
  AttributeType  attrType;
  char          *url;
  int            i;

  /* we find the the Thread element and make it our root */
  root = TtaGetRootElement (thread_doc);
  elType = TtaGetElementType (root);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Annot_ATTR_HREF_;
  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
  while (el)
    {
      i = TtaGetTextAttributeLength (attr) + 1;
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      /* grr... again a local compare problem! */
      elType.ElTypeNum = Annot_EL_TI_desc;
      ti_desc = TtaSearchTypedElement (elType, SearchInTree, el);
      if (ti_desc)
	{
	  attrType.AttrTypeNum = Annot_ATTR_Selected_;
	  attr = TtaGetAttribute (ti_desc, attrType);
	  if (!strcasecmp (url, DocumentURLs[annot_doc])
	      || !strcasecmp (url + sizeof ("file:/"), DocumentURLs[annot_doc]))
	    {
	      if (turnOn && !attr)
		{
		  /* turn on the selected status */
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (ti_desc, attr, thread_doc);
		  TtaSetAttributeValue (attr, Annot_ATTR_Selected__VAL_Yes_, ti_desc, thread_doc);
		}
	      if (!turnOn && attr)
		/* turn off the selected status */
		TtaRemoveAttribute (ti_desc, attr, thread_doc);
	    }
	  else if (strcasecmp (url, DocumentURLs[annot_doc]) && attr)
	    {
	      /* turn off the selected status */
	      TtaRemoveAttribute (ti_desc, attr, thread_doc);
	    }
	}
      TtaFreeMemory (url);
      root = el;
      attrType.AttrTypeNum = Annot_ATTR_HREF_;
      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
    }
#endif
}

/*-----------------------------------------------------------------------
  ANNOT_GetThreadDoc
  Returns the doc number of the doc that is currently selected in a thread.
  Returns 0 if no such doc is open.
  -----------------------------------------------------------------------*/
Document ANNOT_GetThreadDoc (Document thread_doc)
{
#ifdef ANNOT_ON_ANNOT
  Document       doc_annot;
  ElementType    elType;
  Element        root, el;
  Attribute      attr;
  AttributeType  attrType;
  char          *url;
  int            i;

  doc_annot = 0;
  /* we find the the Thread element and make it our root */
  root = TtaGetRootElement (thread_doc);
  if (!root)
    return 0;
  elType = TtaGetElementType (root);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Annot_ATTR_Selected_;
  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
  if (el)
    {
      el = TtaGetParent (el);
      attrType.AttrTypeNum = Annot_ATTR_HREF_;
      attr = TtaGetAttribute (el, attrType);
      i = TtaGetTextAttributeLength (attr) + 1;
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      for (i = 1 ; i <DocumentTableLength; i++)
	{
	  /* find the document */
	  /* @@ grr! */
	  if (!DocumentURLs[i])
	    continue;
	  else if (!strcasecmp (url, DocumentURLs[i])
		   || !strcasecmp (url + sizeof ("file:") - 1, DocumentURLs[i])
		   || !strcasecmp (url + sizeof ("file:/") - 1, DocumentURLs[i])
		   || !strcasecmp (url + sizeof ("file://") - 1, DocumentURLs[i]))
	    {
	      doc_annot = i;
	      break;
	    }
	}
      TtaFreeMemory (url);
    }
  return doc_annot;
#else
  return 0;
#endif
}

/*-----------------------------------------------------------------------
  ANNOT_UpdateThreadItem
  Updates the metadata of a thread item.
  @@ If we published the root of the thread, update all references
  to this item.
  -----------------------------------------------------------------------*/
void ANNOT_UpdateThreadItem (Document doc, AnnotMeta *annot, char *body_url)
{
#ifdef ANNOT_ON_ANNOT
  Document       thread_doc;
  ElementType    elType;
  Element        root, el;
  Attribute      attr;
  AttributeType  attrType;
  char          *url;
  int            i;

  if (!annot)
    return;

  /* find the document where the thread is being shown */
  if (!annot->inReplyTo)
    return;

  thread_doc = AnnotThread_searchRoot (AnnotMetaData[doc].thread->rootOfThread);
  if (thread_doc == 0)
    return;

  /* we find the the Thread element and make it our root */
  root = TtaGetRootElement (thread_doc);
  elType = TtaGetElementType (root);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Annot_ATTR_HREF_;
  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
  while (el)
    {
      i = TtaGetTextAttributeLength (attr) + 1;
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      if (!strcasecmp (url, body_url))
	{
	  TtaFreeMemory (url);
	  break;
	}
      TtaFreeMemory (url);
      root = el;
      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
    }

  /* update the element if we found it */
  if (el)
    ANNOT_ThreadItem_init (el, thread_doc, annot, FALSE);
#endif
}

/*-----------------------------------------------------------------------
  DisplayAnnotTitle
  Displays the ATitle element as the title of the document window.
  doc must be a Annot document.
  -----------------------------------------------------------------------*/
static void DisplayAnnotTitle (Document doc)
{
  Element       root, el;
  ElementType   elType;

  root = TtaGetRootElement (doc);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = Annot_EL_ATitle;
  el = TtaSearchTypedElement (elType, SearchForward, root);
  if (el)
    UpdateTitle (el, doc);
}

/*-----------------------------------------------------------------------
   ANNOT_InitDocumentStructure
   Initializes an annotation document by adding a BODY part
   and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
void ANNOT_InitDocumentStructure (Document doc, Document docAnnot, 
				  AnnotMeta *annot, AnnotMode mode)
{
  char *source_doc_title;
  const char *text;
#ifdef ANNOT_ON_ANNOT
  Element el;
  ElementType elType;
#endif /* ANNOT_ON_ANNOT */

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (docAnnot, NoComputedDisplay);
   /* prepare the title of the annotation */
  source_doc_title = ANNOT_GetHTMLTitle (doc);
  if (mode & ANNOT_initATitle)
    {
      if (mode & ANNOT_isReplyTo)
        text = "Reply to ";
      else
        text = "Annotation of ";
      annot->title = (char *)TtaGetMemory (strlen (text)
				   + strlen (source_doc_title) + 1);
      sprintf (annot->title, "%s%s", text, source_doc_title);
    }

  /* initialize the meta data */
  ANNOT_InitDocumentMeta (doc, docAnnot, annot, source_doc_title);

  /* initialize the html body */
  if (mode & ANNOT_initBody)
    ANNOT_InitDocumentBody (docAnnot, source_doc_title);

  TtaFreeMemory (source_doc_title);
  // Displaythe document title
  DisplayAnnotTitle (docAnnot);

#ifdef ANNOT_ON_ANNOT
  /* erase the thread */
  el = TtaGetRootElement (docAnnot);
  elType = TtaGetElementType (el);
  elType.ElTypeNum = Annot_EL_Thread;
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  if (el)
    TtaRemoveTree (el, docAnnot);
#endif /* ANNOT_ON_ANNOT */

  /* show the document */
  TtaSetDisplayMode (docAnnot, DisplayImmediately);
}


/*-----------------------------------------------------------------------
   Procedure ANNOT_PrepareAnnotView (document)
  -----------------------------------------------------------------------
   Removes all  unused buttons and menus). Returns the number of the opened 
   view or 0 in case of failure.
  -----------------------------------------------------------------------*/
void ANNOT_PrepareAnnotView (Document document)
{
  View view;

  view = TtaGetViewFromName (document, "Formatted_view");
  if (view == 0)
  {
    fprintf (stderr, "(ANNOT_OpenMainView) ERREUR : �chec ouverture vue principale fichier d'annotation\n");
    TtaCloseDocument (document);
  }
  else
  {
    /* turn off the menu items and menu bars we're not using in the annotation window */
    /* navigation bar */
    TtaSetItemOff (document, view, File, BOpenDoc);
    /*    TtaSetMenuOff (document, view, Help_); */
    TtaSetItemOff (document, view, File, BBack);
    TtaSetItemOff (document, view, File, BForward);
    TtaSetItemOff (document, view, File, BCss);
    TtaSetItemOff (document, view, File, BOpenDoc);
    TtaSetItemOff (document, view, File, BOpenInNewWindow);
    TtaSetItemOff (document, view, File, BSaveAs);

    TtaSetItemOff (document, view, Views, BShowAlternate);
    TtaSetItemOff (document, view, Views, BShowSource);
    TtaSetItemOff (document, view, Views, BShowLinks);

    /* annotations */
#ifdef ANNOT_ON_ANNOT
    TtaSetItemOn (document, view, Tools, BAnnotateSelection);
    TtaSetItemOn (document, view, Tools, BAnnotateDocument);
    TtaSetItemOn (document, view, Tools, BReplyToAnnotation);
#else
    TtaSetItemOff (document, view, Tools, BAnnotateSelection);
    TtaSetItemOff (document, view, Tools, BAnnotateDocument);
    TtaSetItemOff (document, view, Tools, BReplyToAnnotation);
#endif /* ANNOT_ON_ANNOT */
  }
  TtaRaiseView (document, view);
}

/*----------------------------------------------------------------------
  ANNOT_LocalSave
  Saves the annotation document doc_annot to the local filesystem
  ----------------------------------------------------------------------*/
ThotBool ANNOT_LocalSave (Document doc_annot, char *html_filename)
{
  Element el;
  ElementType elType;
  AnnotMeta *annot;
  ThotBool result;

  annot = GetMetaData (DocumentMeta[doc_annot]->source_doc, doc_annot);
  if (!annot)
    {
      fprintf (stderr, "ANNOT_LocalSaveDocument: missing annot struct for document\n");
      return TRUE;
    }

  /* update the modified date */
  if (annot->mdate)
    TtaFreeMemory (annot->mdate);
  annot->mdate = StrdupDate ();

  /* and show the new mdate on the document */
  /* point to the root node */
  el = TtaGetRootElement (doc_annot);
  elType = TtaGetElementType (el);

  /* point to the metadata structure */
  elType.ElTypeNum = Annot_EL_Description;
  el = TtaSearchTypedElement (elType, SearchInTree, el);

  /* Last modified date metadata */
  elType.ElTypeNum = Annot_EL_AnnotMDate;
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, (unsigned char *)annot->mdate, TtaGetDefaultLanguage (), doc_annot); 

  /* set up the charset and namespaces */
  SetNamespacesAndDTD (doc_annot, FALSE);
  result = TtaExportDocumentWithNewLineNumbers (doc_annot, html_filename, 
					       "AnnotT", FALSE);

  /* update the annotation icon to the new type */
  if (result && !Annot_IsReplyTo (doc_annot))
      LINK_UpdateAnnotIcon (DocumentMeta[doc_annot]->source_doc, annot);

  return result;
}

