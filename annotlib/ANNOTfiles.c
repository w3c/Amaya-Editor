/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
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

#define BROKEN_SERVER 0		/* 1 if server URIs require munging */
#include "annotlib.h"
#include "AHTURLTools_f.h"

/*-----------------------------------------------------------------------
   Procedure ANNOT_SetPath (document)
  -----------------------------------------------------------------------
   Ajoute le repertoire du document d'annotations a la liste des
   repertoires de documents
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_SetPath (Document document)
#else /* __STDC__*/
void ANNOT_SetPath (document)
     Document document;
#endif /* __STDC__*/
{
  CHAR_T  *dirList = TtaGetMemory (2000);

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

#ifdef __STDC__
Document ANNOT_NewDocument (Document doc)
#else /* __STDC__*/
Document ANNOT_NewDocument (doc)
     Document doc;
#endif /* __STDC__*/
{
  Document annotDoc;
  char     *annot_dir;
  char     *fname;
  char     *docname;
  char     *tmpname;

  /* Create a new annotation document */
  annot_dir = GetAnnotDir ();
  docname = TtaGetDocumentName (doc);
  tmpname = GetTempName (annot_dir, "annot");
  fname = TtaGetMemory (strlen (tmpname) + 20);
  sprintf (fname, "%s.html", tmpname);
  TtaFreeMemory (tmpname);

  /* "annot is the title of the window */
  annotDoc = InitDocView (0, "annotation", docAnnot, 0);

  if (annotDoc == 0) 
    {
      fprintf (stderr, "(ANNOT_NewDocument) ERROR : couldn't create the annotation file\n");
      TtaFreeMemory (fname);
    }
  else
    {
#if 0
      TtaSetDocumentName (annotDoc, TEXT("Annotation");
      TtaSetDocumentDirectory (annotDoc, annot_dir);
#endif

      /* intialize the (amaya) metadata related to a document */
      if (DocumentURLs[annotDoc])
	TtaFreeMemory (DocumentURLs[annotDoc]);
      DocumentURLs[annotDoc] = fname;
      DocumentMeta[annotDoc] = (DocumentMetaDataElement *) TtaGetMemory (sizeof (DocumentMetaDataElement));
      DocumentMeta[annotDoc]->form_data = NULL;
      DocumentMeta[annotDoc]->method = CE_ABSOLUTE;
      DocumentSource[annotDoc] = 0;

      ANNOT_PrepareAnnotView (annotDoc);
    }  
  return annotDoc;
}

/*-----------------------------------------------------------------------
   GetMetaData
  -----------------------------------------------------------------------*/
#ifdef __STDC__
AnnotMeta *GetMetaData (Document doc, Document doc_annot)
#else
AnnotMeta *GetMetaData (doc, doc_annot)
Document doc;
 Document doc_annot;
#endif /* __STDC__ */
{
  List *ptr;
  AnnotMeta *annot = NULL;

  if (DocumentTypes[doc] == docAnnot
      && DocumentTypes[doc] == docAnnotRO)
    return (NULL);

  /* get a pointer to the annot list */
  ptr = AnnotMetaData[doc].annotations;
  if (!ptr)
    return (NULL);

  while (ptr)
    {
      annot = (AnnotMeta *) ptr->object;
      /* @@ maybe we could add calls to NormalizeFile here */
      if ((annot->annot_url 
	   && (!ustrcasecmp (DocumentURLs[doc_annot], annot->annot_url)
	       || !ustrcasecmp (DocumentURLs[doc_annot], annot->annot_url + 7)))
	   || !ustrcasecmp (DocumentURLs[doc_annot], annot->body_url) 
	   || !ustrcasecmp (DocumentURLs[doc_annot], annot->body_url + 7))
	break;
      ptr = ptr->next;
    }

  if (ptr)
    return annot;
  else
    return NULL;
}

/*-----------------------------------------------------------------------
  AnnotURI
  Recomputes the real URI for an annotation. 
  @@ This is bogus; the annotation server should tell us the truth from the start.
  -----------------------------------------------------------------------*/
#ifdef __STDC__
char* AnnotURI (char* givenURI)
#else /* __STDC__*/
char* AnnotURI (givenURI)
    char* givenURI;
#endif /* __STDC__*/
{
#if BROKEN_SERVER
  char *realURI  = TtaGetMemory (ustrlen (GetAnnotServers ()) 
				 + ustrlen (givenURI) 
				 + sizeof ("?w3c_annotation=")
				 + 20);

  usprintf (realURI,
	    TEXT("%s?w3c_annotation=%s"),
	    GetAnnotServers (),
	    givenURI);
#else /* BROKEN_SERVER */
  char *realURI = TtaStrdup (givenURI);
#endif /* BROKEN_SERVER */

  return realURI;
}

/*-----------------------------------------------------------------------
  ANNOT_LoadAnnotation
  Initializes an annotation document by adding a BODY part
  and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void ANNOT_LoadAnnotation (Document doc, Document docAnnot)
#else /* __STDC__*/
void ANNOT_LoadAnnotation (doc, annotDoc)
     Document doc;
     Document annotDoc;
#endif /* __STDC__*/
{
  AnnotMeta *annot;

  /* if it's a new annotation, we compute the stuff, otherwise,
     we copy it from the existing metadata */
  annot = GetMetaData (doc, docAnnot);
  if (annot)
    ANNOT_InitDocumentStructure (doc, docAnnot, annot);
}

/*-----------------------------------------------------------------------
  ANNOT_InitDocumentMeta
  Initializes an annotation document by adding a BODY part
  and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void  ANNOT_InitDocumentMeta (Document doc, Document docAnnot, AnnotMeta *annot, CHAR_T *title)
#else /* __STDC__*/
void  ANNOT_InitDocumentMeta (doc, docAnnot, annot, title)
     Document   doc;
     Document   docAnnot;
     AnnotMeta *annot;
     CHAR_T    *title;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     root, head, el;
  Attribute           attr;
  AttributeType       attrType;
  STRING      user;
  STRING      doc_anchor;
  STRING      source_url;
  CHAR_T     *cdate;
  CHAR_T     *mdate;
  CHAR_T     *type;

  user = annot->author;
  source_url = annot->source_url;
  cdate = annot->cdate;
  mdate = annot->mdate;
  /* @@@ if there's no annot->type use the default */
  type = (annot->type) ? annot->type : TEXT("(comment)");

  /* save the docid of the annotated document */
  DocumentMeta[docAnnot]->source_doc = doc;

  /*
  ** initialize the METADATA 
  */

  /* point to the first node */
  root = TtaGetMainRoot (docAnnot);
  elType = TtaGetElementType (root);

  /* point to the metadata structure */
  elType.ElTypeNum = Annot_EL_Description;
  head = TtaSearchTypedElement (elType, SearchInTree, root);

  /* author metadata */
  elType.ElTypeNum = Annot_EL_Author;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, user, TtaGetDefaultLanguage (), docAnnot); 

  /* Creation Date metadata */
  elType.ElTypeNum = Annot_EL_AnnotCDate;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, cdate, TtaGetDefaultLanguage (), docAnnot); 

  /* Last modified date metadata */
  elType.ElTypeNum = Annot_EL_AnnotMDate;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, mdate, TtaGetDefaultLanguage (), docAnnot); 

  /* Source doc metadata (add a link to the annoted paragraph itself) */
  elType.ElTypeNum = Annot_EL_SourceDoc;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  /* add a reverse link to the source document */	
  attrType.AttrSSchema = TtaGetDocumentSSchema (docAnnot);
  attrType.AttrTypeNum = Annot_ATTR_HREF_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, docAnnot);
#if 0
    {
      doc_anchor = TtaGetMemory (ustrlen (DocumentURLs[doc])
				 + ustrlen (user)
				 + ustrlen (DocumentURLs[docAnnot])
				 + 20);
      sprintf (doc_anchor, "%s#%s_%s_%s", DocumentURLs[doc],
	       ANNOT_ANAME, user, DocumentURLs[docAnnot]);
      annot->name = doc_anchor;
    }
#endif
  doc_anchor = TtaGetMemory (ustrlen (DocumentURLs[doc])
			     + ustrlen (annot->name) 
			     + 20);
  sprintf (doc_anchor, "%s#%s", DocumentURLs[doc], annot->name);
  TtaSetAttributeText (attr, doc_anchor, el, docAnnot);
  TtaFreeMemory (doc_anchor);
  /* use the title parameter as the value of the source document field */
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, title,
		     TtaGetDefaultLanguage (), docAnnot);
  /* RDF type metadata */
  elType.ElTypeNum = Annot_EL_RDFtype;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, type,
		     TtaGetDefaultLanguage (), docAnnot);
}

/*-----------------------------------------------------------------------
  ANNOT_InitDocumentBody 
  Initializes an annotation document by adding a BODY part
  and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void  ANNOT_InitDocumentBody (Document docAnnot, CHAR_T *title)
#else /* __STDC__*/
void  ANNOT_InitDocumentBody (docAnnot, title)
Document docAnnot;
CHAR_T *title;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     root, head, body, el, child;
  CHAR_T     *tmp;

  /*
  ** HTML initialization
  */

  /* we find the the HTML nature */
  root = TtaGetMainRoot (docAnnot);
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
  tmp = TtaGetMemory (ustrlen (title) + sizeof ("Annotation about ") + 1);
  sprintf (tmp, "Annotation of %s", title);
  TtaSetTextContent (el, tmp,
		     TtaGetDefaultLanguage (), docAnnot);
  TtaFreeMemory (tmp);
  /* add a document URL */
  elType.ElTypeNum = HTML_EL_Document_URL;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, DocumentURLs[docAnnot],
		     TtaGetDefaultLanguage (), docAnnot);
  
  /* create a META element in the HEAD with attributes name="GENERATOR" */
  /* and content="Amaya" */
  child = TtaGetLastChild (head);

#if 0
  elType.ElTypeNum = HTML_EL_META;
  meta = TtaNewElement (docAnnot, elType);
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (meta, attr, docAnnot);
  TtaSetAttributeText (attr, TEXT("GENERATOR"), meta, docAnnot);
  attrType.AttrTypeNum = HTML_ATTR_meta_content;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (meta, attr, docAnnot);
  ustrcpy (tempfile, HTAppName);
  ustrcat (tempfile, TEXT(" "));
  ustrcat (tempfile, HTAppVersion);
  TtaSetAttributeText (attr, tempfile, meta, docAnnot);
  TtaInsertSibling (meta, child, FALSE, docAnnot);
#endif
  
  /* Create the BODY */
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchInTree, root);
  if (!body)
    {
      body = TtaNewTree (docAnnot, elType, "");
      TtaInsertSibling (body, head, FALSE, docAnnot);
    }
}

/*-----------------------------------------------------------------------
   ANNOT_InitDocumentStructure
   Initializes an annotation document by adding a BODY part
   and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void  ANNOT_InitDocumentStructure (Document doc, Document docAnnot, AnnotMeta *annot)
#else /* __STDC__*/
void  ANNOT_InitDocumentStructure (doc, docAnnot, annot)
     Document document;
     Document docAnnot;
     AnnotMeta *annot;
     CHAR_T    *title;

#endif /* __STDC__*/
{
  ThotBool free_title;
  CHAR_T  *title;

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (docAnnot, NoComputedDisplay);

  title = ANNOT_GetHTMLTitle (doc);
  if (!title || title[0] == WC_EOS)
    {
      title = DocumentURLs[doc];
      free_title = FALSE;
    }
  else
    free_title = TRUE;

  /* initialize the meta data */
  ANNOT_InitDocumentMeta (doc, docAnnot, annot, title);
  /* initialize the html body */
  ANNOT_InitDocumentBody (docAnnot, title);
  if (free_title)
    TtaFreeMemory (title);

  /* show the document */
  TtaSetDisplayMode (docAnnot, DisplayImmediately);
}

#if 0
/*-----------------------------------------------------------------------
   Procedure ANNOT_InitDocumentStructure (docAnnot, document)
  -----------------------------------------------------------------------
   Initializes an annotation document by adding a BODY part
   and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void  ANNOT_InitDocumentStructure (Document docAnnot, Document document)
#else /* __STDC__*/
void  ANNOT_InitDocumentStructure (docAnnot, document)
     Document docAnnot;
     Document document;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     root, head, body, el, di, tl, top, child, meta;
  Attribute           attr;
  AttributeType       attrType;
  time_t      curDate;
  struct tm   *localDate;
  STRING      strDate;
  STRING      doc_anchor;
  CHAR_T      tempfile[MAX_LENGTH];
  char       *annot_user;

  annot_user = GetAnnotUser ();

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (docAnnot, NoComputedDisplay);
  root = TtaGetMainRoot (docAnnot);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_HEAD;
  head = TtaSearchTypedElement (elType, SearchInTree, root);

  /* Add a document title */
  elType.ElTypeNum = HTML_EL_TITLE;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  el = TtaGetFirstChild (el);
  /* @@ maybe parse the URL here */
  TtaSetTextContent (el, DocumentURLs[docAnnot], 
		     TtaGetDefaultLanguage (), docAnnot);

  /* add a document URL */
  elType.ElTypeNum = HTML_EL_Document_URL;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, DocumentURLs[docAnnot],
		     TtaGetDefaultLanguage (), docAnnot);
  
  /* create a META element in the HEAD with attributes name="GENERATOR" */
  /* and content="Amaya" */
  child = TtaGetLastChild (head);

#if 0
  elType.ElTypeNum = HTML_EL_META;
  meta = TtaNewElement (docAnnot, elType);
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (meta, attr, docAnnot);
  TtaSetAttributeText (attr, TEXT("GENERATOR"), meta, docAnnot);
  attrType.AttrTypeNum = HTML_ATTR_meta_content;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (meta, attr, docAnnot);
  ustrcpy (tempfile, HTAppName);
  ustrcat (tempfile, TEXT(" "));
  ustrcat (tempfile, HTAppVersion);
  TtaSetAttributeText (attr, tempfile, meta, docAnnot);
  TtaInsertSibling (meta, child, FALSE, docAnnot);
#endif
  
  /* Create the BODY */
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchInTree, root);
  if (!body)
    {
      body = TtaNewTree (docAnnot, elType, "");
      TtaInsertSibling (body, head, FALSE, docAnnot);
    }

  /*
   * Write the annotation file meta-info
   */

  /* create the DL structure */
  elType.ElTypeNum = HTML_EL_Definition_List;
  top = TtaCreateDescent (docAnnot, body, elType);
  el = top;

  /* Add a link to the annoted paragraph itself */
  elType.ElTypeNum = HTML_EL_Definition_Item;
  di = TtaCreateDescent (docAnnot, el, elType);
  elType.ElTypeNum = HTML_EL_Term_List;
  tl = TtaCreateDescent (docAnnot, di, elType);
  elType.ElTypeNum = HTML_EL_Term;
  el = TtaCreateDescent (docAnnot, tl, elType);
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  el = TtaCreateDescent (docAnnot, el, elType);
  TtaSetTextContent (el, "Annotated document", TtaGetDefaultLanguage (), docAnnot); 
  elType.ElTypeNum = HTML_EL_Definition;
  el = TtaNewElement (docAnnot, elType);
  TtaInsertSibling (el, tl, FALSE, docAnnot);
  elType.ElTypeNum = HTML_EL_Anchor;
  el = TtaCreateDescent (docAnnot, el, elType);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_HREF_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, docAnnot);
  doc_anchor = TtaGetMemory (strlen (DocumentURLs[document])
			     + strlen (annot_user)
			     + strlen (DocumentURLs[docAnnot])
			     + 20);
  sprintf (doc_anchor, "%s#%s_%s_%s", DocumentURLs[document],
	   ANNOT_ANAME, annot_user, DocumentURLs[docAnnot]);
  TtaSetAttributeText (attr, doc_anchor, el, docAnnot);
  TtaFreeMemory (doc_anchor);
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  el = TtaCreateDescent (docAnnot, el, elType);
  TtaSetTextContent (el, TtaGetDocumentName (document), TtaGetDefaultLanguage (), docAnnot);

  /* write the author's name */
  elType.ElTypeNum = HTML_EL_Definition_Item;
  el = TtaNewElement (docAnnot, elType);
  TtaInsertSibling (el, di, FALSE, docAnnot);
  di = el;
  elType.ElTypeNum = HTML_EL_Term_List;
  tl = TtaCreateDescent (docAnnot, di, elType);
  elType.ElTypeNum = HTML_EL_Term;
  el = TtaCreateDescent (docAnnot, tl, elType);
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  el = TtaCreateDescent (docAnnot, el, elType);
  TtaSetTextContent (el, "Author", TtaGetDefaultLanguage (), docAnnot); 
  elType.ElTypeNum = HTML_EL_Definition;
  el = TtaNewElement (docAnnot, elType);
  TtaInsertSibling (el, tl, FALSE, docAnnot);
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  el = TtaCreateDescent (docAnnot, el, elType);
  TtaSetTextContent (el, annot_user, TtaGetDefaultLanguage (), docAnnot); 

  /* write the date */
  elType.ElTypeNum = HTML_EL_Definition_Item;
  el = TtaNewElement (docAnnot, elType);
  TtaInsertSibling (el, di, FALSE, docAnnot);
  di = el;
  elType.ElTypeNum = HTML_EL_Term_List;
  tl = TtaCreateDescent (docAnnot, di, elType);
  elType.ElTypeNum = HTML_EL_Term;
  el = TtaCreateDescent (docAnnot, tl, elType);
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  el = TtaCreateDescent (docAnnot, el, elType);
  TtaSetTextContent (el, "Date", TtaGetDefaultLanguage (), docAnnot); 
  elType.ElTypeNum = HTML_EL_Definition;
  el = TtaNewElement (docAnnot, elType);
  TtaInsertSibling (el, tl, FALSE, docAnnot);
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  el = TtaCreateDescent (docAnnot, el, elType);
  curDate = time (&curDate);
  localDate = localtime (&curDate);
  /* @@ possible memory bug */
  strDate = TtaGetMemory (25);
  sprintf (strDate, "%04d-%02d-%02dT%02d:%02d", localDate->tm_year+1900,
	   localDate->tm_mon+1, localDate->tm_mday, 
           localDate->tm_hour, localDate->tm_min);
  TtaSetTextContent (el, strDate, TtaGetDefaultLanguage (), docAnnot); 
  TtaFreeMemory (strDate);

  /* write an HR to separate the annotation from the text itself */
  elType.ElTypeNum = HTML_EL_Horizontal_Rule;
  el = TtaNewElement (docAnnot, elType);
  TtaInsertSibling (el, top, FALSE, docAnnot);

  /* show the document */
  TtaSetDisplayMode (docAnnot, DisplayImmediately);
}
#endif

/*-----------------------------------------------------------------------
   Procedure ANNOT_PrepareannotView (document)
  -----------------------------------------------------------------------
   Removes all  unused buttons and menus). Returns the number of the opened 
   view or 0 in case of failure.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_PrepareAnnotView (Document document)
#else /* __STDC__*/
void ANNOT_PrepareAnnotView (document)
     Document document;
#endif /* __STDC__*/
{
  View view;

  view = TtaGetViewFromName (document, "Formatted_view");
  if (view == 0)
  {
    fprintf (stderr, "(ANNOT_OpenMainView) ERREUR : échec ouverture vue principale fichier d'annotation\n");
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
    TtaSetItemOff (document, view, File, BHtml);
    TtaSetItemOff (document, view, File, BTemplate);
    TtaSetItemOff (document, view, File, BCss);
    TtaSetItemOff (document, view, File, BOpenDoc);
    TtaSetItemOff (document, view, File, BOpenInNewWindow);
    TtaSetItemOff (document, view, File, BSaveAs);
    TtaSetTextZone (document, view, 1, DocumentURLs[document]);

#if 0
    TtaAddTextZone (document, 1, TtaGetMessage (AMAYA, AM_TITLE), TRUE,
		    NULL);
    TtcSwitchButtonBar (document, view); /* Cache barre de boutons */
    TtaAddTextZone (document, 1, TtaGetMessage (AMAYA, AM_LOCATION), TRUE,
		    NULL);
    TtcSwitchCommands (document, view);
    TtaSetItemOff (document, view, File, BReload);

    TtaSetMenuOff (document, view, Views);
    TtaSetMenuOff (document, view, Special);
#endif
  }
  TtaRaiseView (document, view);
}

/***************************************************
 I've not yet used/cleaned the following legacy functions 
***************************************************/

/*-----------------------------------------------------------------------
   Procedure ANNOT_CheckEmptyDoc (docAnnot)
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void  ANNOT_CheckEmptyDoc (Document docAnnot)
#else /* __STDC__*/
void  ANNOT_CheckEmptyDoc (docAnnot)
     Document docAnnot;
#endif /* __STDC__*/
{
  Element content, element;

  element = SearchElementInDoc (docAnnot, HTML_EL_Element);
  content = TtaGetFirstChild (element);
#if 0
  if (content != NULL)
    HE_SaveDocument (docAnnot, TtaGetViewFromName (docAnnot, "Formatted_view"));
#endif
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ANNOT_SaveDocument (Document doc_annot)
#else  /* __STDC__ */
ThotBool            ANNOT_SaveDocument (doc_annot)
Document            doc_annot;

#endif /* __STDC__ */
{
  CHAR_T *filename;
  Element el;
  ElementType elType;
  AnnotMeta *annot;

  if (!TtaIsDocumentModified (doc_annot))
    return TRUE; /* prevent Thot from performing normal save operation */

  annot = GetMetaData (DocumentMeta[doc_annot]->source_doc, doc_annot);
  if (!annot)
    {
      fprintf (stderr, "?oops! missing annot struct for document\n");
      return TRUE;
    }

  /* update the modified date */
  if (annot->mdate)
    TtaFreeMemory (annot->mdate);
  annot->mdate = StrdupDate ();

  /* and show the new mdate on the document */
  /* point to the root node */
  el = TtaGetMainRoot (doc_annot);
  elType = TtaGetElementType (el);

  /* point to the metadata structure */
  elType.ElTypeNum = Annot_EL_Description;
  el = TtaSearchTypedElement (elType, SearchInTree, el);

  /* Last modified date metadata */
  elType.ElTypeNum = Annot_EL_AnnotMDate;
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, annot->mdate, TtaGetDefaultLanguage (), doc_annot); 

  /* save the file */
  filename =  TtaStrdup (DocumentURLs[doc_annot]);
  /* we skip the file: prefix if it's present */
  NormalizeFile (DocumentURLs[doc_annot], filename, AM_CONV_ALL);
  TtaExportDocument (doc_annot, filename, TEXT("AnnotT"));
  TtaFreeMemory (filename);
  TtaSetDocumentUnmodified (doc_annot);

  return TRUE; /* prevent Thot from performing normal save operation */
}





