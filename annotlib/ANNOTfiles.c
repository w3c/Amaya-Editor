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

#include "annotlib.h"

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
  STRING dirList = TtaGetMemory (2000);

  TtaGetDocumentPath (dirList, 2000);
  strcat (dirList, ":");
  strcat (dirList, GetAnnotDir ());
  TtaSetDocumentPath (dirList);
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_NewDocument (document)
  -----------------------------------------------------------------------
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
  char     *urlname;
  char     *fname;
  char     *docname;
  char     *tmpname;

  /* Create a new annotation document */
  annot_dir = GetAnnotDir ();
  docname = TtaGetDocumentName (doc);
  tmpname = GetTempName (annot_dir, "annot");
  urlname = TtaGetMemory (strlen (tmpname) + 20);
  sprintf (urlname, "file:%s.html", tmpname);
  TtaFreeMemory (tmpname);
  fname = urlname + 5;

  /* @@ 10 should be docAnnot, but I have a problem to set it up */
  annotDoc = InitDocView (0, "annot", 10, FALSE);

  if (annotDoc == 0) 
    {
      fprintf (stderr, "(ANNOT_NewDocument) ERROR : couldn't create the annotation file\n");
      TtaFreeMemory (urlname);
    }
  else
    {
#if 0
      TtaSetDocumentName (annotDoc, "Annotation");
      TtaSetDocumentDirectory (annotDoc, annot_dir);
#endif
      if (DocumentURLs[annotDoc])
	TtaFreeMemory (DocumentURLs[annotDoc]);
      DocumentURLs[annotDoc] = urlname;
      DocumentMeta[annotDoc] = (DocumentMetaDataElement *) TtaGetMemory (sizeof (DocumentMetaDataElement));
      DocumentMeta[annotDoc]->form_data = NULL;
      DocumentMeta[annotDoc]->method = CE_ABSOLUTE;
      DocumentSource[annotDoc] = 0;
      ANNOT_PrepareAnnotView (annotDoc);
      ANNOT_InitDocumentStructure (annotDoc, doc);      
    }  
  return annotDoc;
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_InitDocumentMeta (docAnnot, document)
  -----------------------------------------------------------------------
   Initializes an annotation document by adding a BODY part
   and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
#ifdef __STDC__
AnnotMetaDataElement *GetMetaData (Document doc, Document annotDoc)
#endif
{
  AnnotMetaDataElement *me;
  STRING annotFile;

  /* @@@ why I can't use docAnnot??? what is interfering? */
  if (DocumentTypes[doc] == 10
      && DocumentTypes[doc] == 11)
    return (NULL);

  me = AnnotMetaDataList[doc];
  if (me)
    annotFile = me->annotFile;

  while (me) 
    {
      if (ustrcasecmp (DocumentURLs[annotDoc], annotFile) ||
	  ustrcasecmp (DocumentURLs[annotDoc], annotFile + 5))
	break;
    }
  return me;
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_InitDocumentMeta (docAnnot, document)
  -----------------------------------------------------------------------
   Initializes an annotation document by adding a BODY part
   and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void  ANNOT_InitDocumentMeta (Document docAnnot, Document document)
#else /* __STDC__*/
void  ANNOT_InitDocumentMeta (docAnnot, document)
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
  STRING      annot_user;
  STRING      annot_date;
  STRING      doc_anchor;
  STRING      annot_type;
  STRING      annot_source;
  CHAR_T      tempfile[MAX_LENGTH];
  AnnotMetaDataElement *annot_metadata;

  /* if it's a new annotation, we compute the stuff, otherwise,
     we copy it from the existing metadata */
  annot_metadata = GetMetaData (document, docAnnot);

  if (annot_metadata) 
    {
      /* existing annotation, we reuse the existing data */
      annot_user = annot_metadata->author;
      annot_date = annot_metadata->date;
      annot_type = annot_metadata->type;
    }
  else
    {
      /* it's a new annotation */

      /* author */
      annot_user = GetAnnotUser ();

      /* date */
      curDate = time (&curDate);
      localDate = localtime (&curDate);
      /* @@ possible memory bug */
      annot_date = TtaGetMemory (25);
      sprintf (annot_date, "%02d/%02d/%04d %02d:%02d", localDate->tm_mday, 
	       localDate->tm_mon+1, localDate->tm_year+1900, localDate->tm_hour,
	       localDate->tm_min);

      /* Annotation type */
      annot_type = TEXT("Proto-Annotation");
    }

  /* endpoint of the annotation */
  annot_source = TtaGetDocumentName (document);

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
  TtaSetTextContent (el, annot_user, TtaGetDefaultLanguage (), docAnnot); 

  /* Date metadata */
  elType.ElTypeNum = Annot_EL_AnnotDate;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, annot_date, TtaGetDefaultLanguage (), docAnnot); 

  /* Source doc metadata (add a link to the annoted paragraph itself) */
  elType.ElTypeNum = Annot_EL_SourceDoc;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, DocumentURLs[docAnnot], 
		     TtaGetDefaultLanguage (), docAnnot);
#if 0
  /* this code is for adding a back link to the source document */
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
#endif
  TtaSetTextContent (el, DocumentURLs[document],
		     TtaGetDefaultLanguage (), docAnnot);

  /* RDF type metadata */
  elType.ElTypeNum = Annot_EL_RDFtype;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  el = TtaGetFirstChild (el);
  TtaSetTextContent (el, annot_type,
		     TtaGetDefaultLanguage (), docAnnot);

  /* memory cleanup when creating a new annotation */
  if (!annot_metadata) 
    {
      TtaFreeMemory (annot_date);
    }
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_InitDocumentBody (docAnnot, document)
  -----------------------------------------------------------------------
   Initializes an annotation document by adding a BODY part
   and adding META elements for title, author, date, and type
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void  ANNOT_InitDocumentBody (Document docAnnot, Document document)
#else /* __STDC__*/
void  ANNOT_InitDocumentBody (docAnnot, document)
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
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_InitDocumentStructureP (docAnnot, document)
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
  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (docAnnot, NoComputedDisplay);

  /* initialize the meta data */
  ANNOT_InitDocumentMeta (docAnnot, document);
  /* initialize the html body */
  ANNOT_InitDocumentBody (docAnnot, document);

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
  sprintf (strDate, "%02d/%02d/%04d %02d:%02d", localDate->tm_mday, localDate->tm_mon+1,
           localDate->tm_year+1900, localDate->tm_hour, localDate->tm_min);
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
 I've not yet used/cleaning the following legacy functions 
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
ThotBool            ANNOT_SaveDocument (Document doc)
#else  /* __STDC__ */
ThotBool            ANNOT_SaveDocument (doc)
Document            doc;

#endif /* __STDC__ */
{
  CHAR_T *filename;
  
  filename =  TtaAllocString (DocumentURLs[doc]);
  /* we skip the file: prefix if it's present */
  NormalizeFile (DocumentURLs[doc], filename);
  TtaExportDocument (doc, filename, TEXT("AnnotT"));

  TtaFreeMemory (filename);
  return TRUE; /* prevent Thot from performing normal save operation */
}





