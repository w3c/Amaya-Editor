/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Menu bar functions of Amaya application.
 *
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "undo.h"
 
#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#include "ANNOTmenu_f.h"
#endif /* ANNOTATIONS */

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#include "css_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "EDITimage_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "HTMLsave_f.h"
#include "HTMLtable_f.h"
#include "MENUconf_f.h"
#include "styleparser_f.h"
#include "UIcss_f.h"

#ifdef DAV
#include "davlib_f.h"
#endif


/*----------------------------------------------------------------------
  NewXHTMLBasic: Create a new Basic XHTML document
  ----------------------------------------------------------------------*/
void NewXHTMLBasic (Document document, View view)
{
  OpenNew (document, view, docHTML, L_Basic);
}

/*----------------------------------------------------------------------
  NewXHTMLStrict: Create a new Strict XHTML document
  ----------------------------------------------------------------------*/
void NewXHTMLStrict (Document document, View view)
{
  OpenNew (document, view, docHTML, L_Strict);
}

/*----------------------------------------------------------------------
  NewXHTML11: Create a new XHTML1.1 document
  ----------------------------------------------------------------------*/
void NewXHTML11 (Document document, View view)
{
  OpenNew (document, view, docHTML, L_Xhtml11);
}

/*----------------------------------------------------------------------
  NewXHTMLTransitional: Create a new Transitional XHTML document
  ----------------------------------------------------------------------*/
void NewXHTMLTransitional (Document document, View view)
{
  OpenNew (document, view, docHTML, L_Transitional);
}

/*----------------------------------------------------------------------
  NewMathML: Create a new MathML document
  ----------------------------------------------------------------------*/
void NewMathML (Document document, View view)
{
  OpenNew (document, view, docMath, L_MathML);
}

/*----------------------------------------------------------------------
  NewSVG: Create a new XHTML document
  ----------------------------------------------------------------------*/
void NewSVG (Document document, View view)
{
  OpenNew (document, view, docSVG, L_Other);
}

/*----------------------------------------------------------------------
   NewCss: Create a new CSS stylesheet
  ----------------------------------------------------------------------*/
void NewCss (Document document, View view)
{
  OpenNew (document, view, docCSS, L_Other);
}

/*--------------------------------------------------------------------------
  CreateDoctype
  Create a doctype declaration
  --------------------------------------------------------------------------*/
void CreateDoctype (Document doc, int profile)
{
  ElementType     elType, lineType;
  Element         docEl, doctype, doctypeLine, text, child;
  Language        language;
  char           *ptr;
  SSchema         nature;
  ThotBool	  useMathML;
  
  /* Check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (0, doc);
  
  /* We use the Latin_Script language to avoid */
  /* the spell_chekcer to check the doctype */
  language = Latin_Script;
  
  docEl = TtaGetMainRoot (doc);
  elType = TtaGetElementType (docEl);
  
  /* Add the new doctype */
  if ((profile == L_Basic) || (profile == L_Strict) ||
      (profile == L_Xhtml11) || (profile == L_Transitional))
    {
      elType.ElTypeNum = HTML_EL_DOCTYPE;
      lineType.ElTypeNum = HTML_EL_DOCTYPE_line;
    }
  else if (profile == L_SVG) 
    {
      elType.ElTypeNum = SVG_EL_DOCTYPE;
      lineType.ElTypeNum = SVG_EL_DOCTYPE_line;
    }
  else if (profile == L_MathML) 
    {
      elType.ElTypeNum = MathML_EL_DOCTYPE;
      lineType.ElTypeNum = MathML_EL_DOCTYPE_line;
    }
  
  if (profile == L_Xhtml11)
    {
      /* look for the MathML nature used in the document */
      nature = NULL;
      useMathML = FALSE;
      do
	{
	  TtaNextNature (doc, &nature);
	  if (nature && !useMathML)
	    {
	      ptr = TtaGetSSchemaName (nature);
	      if (!strcmp (ptr, "MathML"))
		useMathML = TRUE;
	    }
	}
      while (nature);
    }
  
  doctype = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&doctype, docEl, doc);
  /* Make the DOCTYPE element read-only */
  TtaSetAccessRight (doctype, ReadOnly, doc);
  /* Create the first DOCTYPE_line element */
  elType.ElTypeNum = lineType.ElTypeNum;
  doctypeLine = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&doctypeLine, doctype, doc);
  elType.ElTypeNum = 1;
  text = TtaNewElement (doc, elType);
  if (text != NULL)
    {
      TtaInsertFirstChild (&text, doctypeLine, doc);
      if (profile == L_Basic)
	TtaSetTextContent (text, DOCTYPE1_XHTML10_BASIC, language, doc);
      else if (profile == L_Strict && DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE1_XHTML10_STRICT, language, doc);
      else if (profile == L_Strict && !DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE1_HTML_STRICT, language, doc);
      else if (profile == L_Xhtml11 && !useMathML)
	TtaSetTextContent (text, DOCTYPE1_XHTML11, language, doc);
      else if (profile == L_Xhtml11 && useMathML)
	TtaSetTextContent (text, DOCTYPE1_XHTML11_PLUS_MATHML, language, doc);
      else if (profile == L_Transitional && DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE1_XHTML10_TRANSITIONAL, language, doc);
      else if (profile == L_Transitional && !DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE1_HTML_TRANSITIONAL, language, doc);
      else if (profile == L_MathML)
	TtaSetTextContent (text, DOCTYPE1_MATHML20, language, doc);
      else if (profile == L_SVG)
	TtaSetTextContent (text, DOCTYPE1_SVG10, language, doc);
    }
  
  /* Create the second DOCTYPE_line element */
  elType.ElTypeNum = lineType.ElTypeNum;
  doctypeLine = TtaNewElement (doc, elType);
  child = TtaGetLastChild (doctype);
  TtaInsertSibling (doctypeLine, child, FALSE, doc);
  elType.ElTypeNum = 1;
  text = TtaNewElement (doc, elType);
  if (text != NULL)
    {
      TtaInsertFirstChild (&text, doctypeLine, doc);
      if (profile == L_Basic)
	TtaSetTextContent (text, DOCTYPE2_XHTML10_BASIC, language, doc);
      else if (profile == L_Strict && DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE2_XHTML10_STRICT, language, doc);
      else if (profile == L_Strict && !DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE2_HTML_STRICT, language, doc);
      else if (profile == L_Xhtml11 && !useMathML)
	TtaSetTextContent (text, DOCTYPE2_XHTML11, language, doc);
      else if (profile == L_Xhtml11 && useMathML)
	TtaSetTextContent (text, DOCTYPE2_XHTML11_PLUS_MATHML, language, doc);
      else if (profile == L_Transitional && DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE2_XHTML10_TRANSITIONAL, language, doc);
      else if (profile == L_Transitional && !DocumentMeta[doc]->xmlformat)
	TtaSetTextContent (text, DOCTYPE2_HTML_TRANSITIONAL, language, doc);
      else if (profile == L_MathML)
	TtaSetTextContent (text, DOCTYPE2_MATHML20, language, doc);
      else if (profile == L_SVG)
	TtaSetTextContent (text, DOCTYPE2_SVG10, language, doc);
    }
  TtaSetStructureChecking (1, doc);

  return;
}

/*----------------------------------------------------------------------
  InitializeNewDoc builds the initial contents of a new document
  When the parameter doc is 0 the function creates a new document window.
  ----------------------------------------------------------------------*/
void InitializeNewDoc (char *url, int docType, Document doc, int profile)
{
  ElementType          elType;
  Element              docEl, root, title, text, el, head, child, meta, body;
  Element              doctype;
  AttributeType        attrType;
  Attribute            attr;
  Language             language;
  char                *pathname, *documentname;
  char                *s;
  char                 tempfile[MAX_LENGTH];
  char                 charsetName[MAX_LENGTH];

  pathname = TtaGetMemory (MAX_LENGTH);
  documentname = TtaGetMemory (MAX_LENGTH);
  NormalizeURL (url, 0, pathname, documentname, NULL);
  if (doc == 0 || InNewWindow)
    {
      doc = InitDocAndView (0, documentname, docType, 0, FALSE, profile);
      InitDocHistory (doc);
      InNewWindow = FALSE;
    }
  else
    {
      /* record the current position in the history */
      AddDocHistory (doc, DocumentURLs[doc], 
		     DocumentMeta[doc]->initial_url,
		     DocumentMeta[doc]->form_data,
		     DocumentMeta[doc]->method);
      doc = InitDocAndView (doc, documentname, docType, 0, FALSE, profile);
    }
  TtaFreeMemory (documentname);
  TtaFreeMemory (pathname);

  /* save the document name into the document table */
  s = TtaStrdup (url);
  TtaSetTextZone (doc, 1, 1, url);
  DocumentURLs[doc] = s;
  DocumentMeta[doc] = DocumentMetaDataAlloc ();
  DocumentMeta[doc]->form_data = NULL;
  DocumentMeta[doc]->initial_url = NULL;
  DocumentMeta[doc]->method = CE_ABSOLUTE;
  DocumentMeta[doc]->xmlformat = FALSE;
  DocumentSource[doc] = 0;

  /* store the document profile */
  TtaSetDocumentProfile (doc, profile);

  ResetStop (doc);
  language = TtaGetDefaultLanguage ();
  docEl = TtaGetMainRoot (doc);
  /* disable auto save */
  TtaSetDocumentBackUpInterval (doc, 0);
  /* Set the document charset */
  TtaSetDocumentCharset (doc, ISO_8859_1);
  strcpy (charsetName , "iso-8859-1");
  DocumentMeta[doc]->charset = TtaStrdup (charsetName);

  elType = TtaGetElementType (docEl);
  attrType.AttrSSchema = elType.ElSSchema;

  if (docType == docHTML)
    {
      /*-------------  New XHTML document ------------*/
      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = TRUE;

      /* create the DOCTYPE element corresponding to the document's profile */
      elType.ElTypeNum = HTML_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (doctype != NULL)
	TtaDeleteTree (doctype, doc);
      if (profile != L_Other)
	CreateDoctype (doc, profile);
      
      /* Load user's style sheet */
      LoadUserStyleSheet (doc);

      /* attach an attribute PrintURL to the root element */
      elType.ElTypeNum = HTML_EL_HTML;
      root = TtaSearchTypedElement (elType, SearchInTree, docEl);
      attrType.AttrTypeNum = HTML_ATTR_PrintURL;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);

      /* create a default title if there is no content in the TITLE element */
      elType.ElTypeNum = HTML_EL_TITLE;
      title = TtaSearchTypedElement (elType, SearchInTree, root);
      text = TtaGetFirstChild (title);
      if (TtaGetTextLength (text) == 0)
	TtaSetTextContent (text, "No title", language, doc);
      UpdateTitle (title, doc);

      elType.ElTypeNum = HTML_EL_HEAD;
      head = TtaSearchTypedElement (elType, SearchInTree, root);

      /* create a Document_URL element as the first child of HEAD */
      elType.ElTypeNum = HTML_EL_Document_URL;
      el = TtaSearchTypedElement (elType, SearchInTree, head);
      if (el == NULL)
	{
	  /* there is no Document_URL element, create one */
	  el = TtaNewElement (doc, elType);
	  TtaInsertFirstChild (&el, head, doc);
	}
      /* prevent the user from editing this element */
      TtaSetAccessRight (el, ReadOnly, doc);
      /* element Document_URL already exists */
      text = TtaGetFirstChild (el);
      if (text == NULL)
	{
	  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	  text = TtaNewElement (doc, elType);
	  TtaInsertFirstChild (&text, el, doc);
	}
      if (url != NULL && text != NULL)
	TtaSetTextContent (text, url, language, doc);

      /* create a META element in the HEAD with name="generator" */
      /* and content="Amaya" */
      child = TtaGetLastChild (head);
      elType.ElTypeNum = HTML_EL_META;
      meta = TtaNewElement (doc, elType);
      attrType.AttrTypeNum = HTML_ATTR_meta_name;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      TtaSetAttributeText (attr, "generator", meta, doc);
      attrType.AttrTypeNum = HTML_ATTR_meta_content;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      strcpy (tempfile, HTAppName);
      strcat (tempfile, " ");
      strcat (tempfile, HTAppVersion);
      strcat (tempfile, ", see http://www.w3.org/Amaya/");
      TtaSetAttributeText (attr, tempfile, meta, doc);
      TtaInsertSibling (meta, child, FALSE, doc);

      /* create a BODY element if there is not */
      elType.ElTypeNum = HTML_EL_BODY;
      body = TtaSearchTypedElement (elType, SearchInTree, root);
      if (!body)
	{
	  body = TtaNewTree (doc, elType, "");
	  TtaInsertSibling (body, head, FALSE, doc);
	}

      /* Search the first element in the BODY to set initial selection */
      elType.ElTypeNum = HTML_EL_Element;
      el = TtaSearchTypedElement (elType, SearchInTree, body);
      /* set the initial selection */
      TtaSelectElement (doc, el);
      if (SelectionDoc != 0)
	UpdateContextSensitiveMenus (SelectionDoc);
      SelectionDoc = doc;
      UpdateContextSensitiveMenus (doc);
      /* Activate the section numbering */
      if (SNumbering[doc])
	ChangeAttrOnRoot (doc, HTML_ATTR_SectionNumbering);
      /* Activate show areas */
      if (MapAreas[doc])
	ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
    }
  else if (docType == docMath)
    {
      /*-------------  New MathML document ------------*/
      /* create the MathML DOCTYPE element */
      elType.ElTypeNum = MathML_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (doctype != NULL)
	TtaDeleteTree (doctype, doc);
      CreateDoctype (doc, L_MathML);

      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = TRUE;
      /* Search the first Construct to set the initial selection */
      elType.ElTypeNum = MathML_EL_Construct;
      el = TtaSearchTypedElement (elType, SearchInTree, docEl);
      /* set the initial selection */
      TtaSelectElement (doc, el);
      if (SelectionDoc != 0)
	UpdateContextSensitiveMenus (SelectionDoc);
      SelectionDoc = doc;
      UpdateContextSensitiveMenus (doc);
     }
  else if (docType == docSVG)
    {
      /*-------------  New SVG document ------------*/
      /* create the SVG DOCTYPE element */
      elType.ElTypeNum = SVG_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (doctype != NULL)
	TtaDeleteTree (doctype, doc);
      CreateDoctype (doc, L_SVG);

      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = TRUE;
      /* Search the last element to set the initial selection */
      el = TtaGetLastLeaf (docEl);
      /* set the initial selection */
      TtaSelectElement (doc, el);
      if (SelectionDoc != 0)
	UpdateContextSensitiveMenus (SelectionDoc);
      SelectionDoc = doc;
      UpdateContextSensitiveMenus (doc);
     }
  else
    {
      /*-------------  Other documents ------------*/
      /* attach the default attribute PrintURL to the root element */
      elType.ElTypeNum = TextFile_EL_TextFile;
      root = TtaSearchTypedElement (elType, SearchInTree, docEl);
      attrType.AttrTypeNum = TextFile_ATTR_PrintURL;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);
      /* insert the Document_URL element */
      el = TtaGetFirstChild (root);
      if (el == NULL)
	{
	  elType.ElTypeNum = TextFile_EL_Document_URL;
	  el = TtaNewTree (doc, elType, "");
	  TtaInsertFirstChild (&el, root, doc);
	}
      /* prevent the user from editing this element */
      TtaSetAccessRight (el, ReadOnly, doc);
      el = TtaGetFirstChild (el);     
      TtaSetTextContent (el, url, language, doc);

      body = TtaGetLastChild (root);
      /* create a new line */
      elType.ElTypeNum = TextFile_EL_Line_;
      el = TtaNewTree (doc, elType, "");
      /* first line */
      TtaInsertFirstChild (&el, body, doc);

      /* set the initial selection */
      TtaSelectElement (doc, el);
      SelectionDoc = doc;
    }

  /* Update the Doctype menu */
  UpdateDoctypeMenu (doc);
  
  /* the document should be saved */
  TtaSetDocumentModified (doc);
}

/*--------------------------------------------------------------------------
  RemoveDoctype
  Remove the doctype declaration
  --------------------------------------------------------------------------*/
void RemoveDoctype (Document document, View view)
{

  ElementType     elType;
  Element         docEl, doctype;
  char           *s;

  docEl = TtaGetMainRoot (document);
  elType = TtaGetElementType (docEl);

  /* Search the doctype declaration according to the main schema */
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "HTML") == 0)
    elType.ElTypeNum = HTML_EL_DOCTYPE;
  else if (strcmp (s, "SVG") == 0)
    elType.ElTypeNum = SVG_EL_DOCTYPE;
  else if (strcmp (s, "MathML") == 0)
    elType.ElTypeNum = MathML_EL_DOCTYPE;
  else
    elType.ElTypeNum = XML_EL_doctype;
  doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);

  /* remove the existing doctype */
  if (doctype != NULL)
    {
      TtaDeleteTree (doctype, document);
      TtaSetDocumentProfile (document, L_Other);
      UpdateDoctypeMenu (document);
      TtaSetDocumentModified (document);
    }
}

/*--------------------------------------------------------------------------
  CreateOrChangeDoctype
  Create or change the doctype of a document
  --------------------------------------------------------------------------*/
static void CreateOrChangeDoctype (Document doc, View view, int profile)
{
  ElementType     elType;
  Element         docEl, doctype;
  char           *tempdocument = NULL;
  char            documentname[MAX_LENGTH];
  char            tempdir[MAX_LENGTH];

  /* Ask confirmation */
  if (TtaIsDocumentModified (doc))
    {
      InitConfirm3L (doc, view,
		     TtaGetMessage (AMAYA, AM_CHANGE_DOCTYPE1),
		     TtaGetMessage (AMAYA, AM_CHANGE_DOCTYPE2),
		     NULL, TRUE);
      if (!UserAnswer)
	return;
    } 
  
  docEl = TtaGetMainRoot (doc);
  elType = TtaGetElementType (docEl);
  /* Search the doctype declaration according to the main schema */
  if ((profile == L_Basic) || (profile == L_Strict) ||
      (profile == L_Xhtml11) || (profile == L_Transitional))
    elType.ElTypeNum = HTML_EL_DOCTYPE;
  else if (profile == L_MathML) 
    elType.ElTypeNum = MathML_EL_DOCTYPE;
  else if (profile == L_SVG) 
    elType.ElTypeNum = SVG_EL_DOCTYPE;
  doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);

  /* Remove the previous doctype */
  if (doctype != NULL)
    TtaDeleteTree (doctype, doc);

  /* Add the new doctype */
  CreateDoctype (doc, profile);

  /* Store the document's profile */
  TtaSetDocumentProfile (doc, profile);

  /* The document has to be reparsed */
  /* Save the current state of the document into the temporary file */
  tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
  SetNamespacesAndDTD (doc);
  if (DocumentTypes[doc] == docHTML)
    {
      if (TtaGetDocumentProfile(doc) == L_Xhtml11)
	TtaExportDocumentWithNewLineNumbers (doc, tempdocument, "HTMLT11");
      else if (DocumentMeta[doc]->xmlformat)
	TtaExportDocumentWithNewLineNumbers (doc, tempdocument, "HTMLTX");
      else
	TtaExportDocumentWithNewLineNumbers (doc, tempdocument, "HTMLT");
    }
  else if (DocumentTypes[doc] == docSVG)
    TtaExportDocumentWithNewLineNumbers (doc, tempdocument, "SVGT");
  else if (DocumentTypes[doc] == docMath)
    TtaExportDocumentWithNewLineNumbers (doc, tempdocument, "MathMLT");
  RedisplaySourceFile (doc);

  tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
  TtaExtractName (tempdocument, tempdir, documentname);
  RestartParser (doc, tempdocument, tempdir, documentname);
  TtaSetDocumentModified (doc);

  /* Clear all editing operations registered in the editing history of the document */
  TtaClearUndoHistory (doc);

  /* Clear the current selection */
  TtaClearViewSelections ();

  /* Update the Doctype menu */
  UpdateDoctypeMenu (doc);

  TtaFreeMemory (tempdocument);
}

/*--------------------------------------------------------------------------
  CreateDoctypeHtml11
  Create or change the doctype for a XHTML 1.1 document
  --------------------------------------------------------------------------*/
void CreateDoctypeHtml11 (Document document, View view)
{
  CreateOrChangeDoctype (document, view, L_Xhtml11);
}

/*--------------------------------------------------------------------------
  CreateDoctypeHtmlTransitional
  Create or change the doctype for a (X)HTML Transitional document
  --------------------------------------------------------------------------*/
void CreateDoctypeHtmlTransitional (Document document, View view)
{
    CreateOrChangeDoctype (document, view, L_Transitional);
}

/*--------------------------------------------------------------------------
  CreateDoctypeHtmlStrict
  Create or change the doctype for a (X)HTML 1.0 Strict document
  --------------------------------------------------------------------------*/
void CreateDoctypeHtmlStrict (Document document, View view)
{
  CreateOrChangeDoctype (document, view, L_Strict);
}

/*--------------------------------------------------------------------------
  CreateDoctypeHtmlBasic
  Create or change the doctype for a XHTML 1.0 Basic document
  --------------------------------------------------------------------------*/
void CreateDoctypeHtmlBasic (Document document, View view)
{
  CreateOrChangeDoctype (document, view, L_Basic);
}

/*--------------------------------------------------------------------------
  CreateDoctypeMathML
  Create or change the doctype for a MathML document
  --------------------------------------------------------------------------*/
void CreateDoctypeMathML (Document document, View view)
{
  CreateOrChangeDoctype (document, view, L_MathML);
}

/*--------------------------------------------------------------------------
  CreateDoctypeSVG
  Create or change the doctype for a SVG Basic document
  --------------------------------------------------------------------------*/
void CreateDoctypeSVG (Document document, View view)
{
  CreateOrChangeDoctype (document, view, L_SVG);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SpellCheck (Document document, View view)
{
   AttributeType       attrType;
   Attribute           attr;
   Element             docEl, el, body;
   ElementType         elType;
   int                 firstchar, lastchar;

   docEl = TtaGetMainRoot (document);
   elType = TtaGetElementType (docEl);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_BODY;
       body = TtaSearchTypedElement (elType, SearchInTree, docEl);
       if (body == NULL)
	 return;
       /* if there is no Language attribute on the BODY, create one */
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Language;
       attr = TtaGetAttribute (body, attrType);
       if (attr == NULL)
	 {
	   /* create the Language attribute */
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (body, attr, document);
	   TtaSetAttributeText (attr, "en", body, document);
	 }
       /* get the current selection */
       TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
       if (el == NULL)
	 {
	   /* no current selection in the document */
	   /* select the first character in the body */
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   el = TtaSearchTypedElement (elType, SearchInTree, body);
	   if (el != NULL)
	     TtaSelectString (document, el, 1, 0);
	 }
     }
   TtcSpellCheck (document, view);
}

/*----------------------------------------------------------------------
  CreateBreak
  ----------------------------------------------------------------------*/
void CreateBreak (Document document, View view)
{
   ElementType         elType;
   Element             el, br, parent;
   int                 firstChar, lastChar;

   TtaGiveLastSelectedElement (document, &el, &firstChar, &lastChar);
   if (el == NULL)
     return;
   elType = TtaGetElementType (el);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
     /* not within HTML element */
     return;

   elType.ElTypeNum = HTML_EL_BR;
   TtaCreateElement (elType, document);
   TtaGiveLastSelectedElement (document, &el, &firstChar, &lastChar);
   br = el;
   TtaNextSibling (&el);
   if (el == NULL)
     {
       /* Insert a text element after the BR */
       elType.ElTypeNum = HTML_EL_TEXT_UNIT;
       el = TtaNewElement (document, elType);
       TtaInsertSibling (el, br, FALSE, document);
       /* move the selection */
       TtaSelectString (document, el, 1, 0);
     }
   else
     {
       /* move the selection */
       parent = el;
       while (el != NULL && !TtaIsLeaf ( TtaGetElementType (el)))
	 {
	   parent = el;
	   el = TtaGetFirstChild (parent);
	 }
       if (el == NULL)
	 TtaSelectElement (document, parent);
       else
	 {
	   elType = TtaGetElementType (el);
	   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	     TtaSelectString (document, el, 1, 0);
	   else
	     TtaSelectString (document, el, 0, 0);
	 }
     }
}

/*----------------------------------------------------------------------
  InsertWithinHead moves the insertion point into the document Head to
  insert the element type.
  Return TRUE if it succeeds.
  ----------------------------------------------------------------------*/
static Element InsertWithinHead (Document document, View view, int elementT)
{
   ElementType         elType;
   Element             el, firstSel, lastSel, head, parent, new, title;
   SSchema             docSchema;
   int                 j, firstChar, lastChar;
   ThotBool            before;

   docSchema = TtaGetDocumentSSchema (document);
   if (strcmp(TtaGetSSchemaName (docSchema), "HTML") != 0)
     /* not within an HTML document */
     return (NULL);
   else
     {
       elType.ElSSchema = docSchema;
       elType.ElTypeNum = HTML_EL_HEAD;
       el = TtaGetMainRoot (document);
       head = TtaSearchTypedElement (elType, SearchForward, el);
       
       /* give current position */
       TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &j);
       TtaGiveLastSelectedElement (document, &lastSel, &j, &lastChar);
       el = firstSel;
       if (firstSel == NULL || firstSel == head ||
	   !TtaIsAncestor (firstSel, head))
	 {
	   /* the current selection is not within the head */
	   el = TtaGetLastChild (head);
	   /* insert after the last element in the head */
	   before = FALSE;
	 }
       else
	 {
	   /* the current selection is within the head */
	   parent = TtaGetParent (el);
	   /* does the selection precede the title? */
	   elType = TtaGetElementType (el);
	   if (elType.ElTypeNum == HTML_EL_TITLE)
	     /* the title is selected, insert after it */
	     before = FALSE;
	   else
	     {
	       elType.ElTypeNum = HTML_EL_TITLE;
	       title = TtaSearchTypedElement (elType, SearchForward, el);
	       if (title != NULL)
		 {
		   /* insert after the title */
		   before = FALSE;
		   el = title;
		 }
	       else
		 {
		   /* insert before the current element */
		   before = TRUE;
		   while (parent != head)
		     {
		       el = parent;
		       /* insert after the parent element */
		       before = FALSE;
		       parent = TtaGetParent (el);
		     }
		 }
	     }
	 }
       /* now insert the new element after el */
       elType.ElTypeNum = elementT;
       if (elementT == HTML_EL_BASE)
	 /* the element to be created is BASE */
	 {
	   /* return if this element already exists */
	   if (TtaSearchTypedElement (elType, SearchInTree, head))
	     return (NULL);
	 }
       new = TtaNewTree (document, elType, "");
       TtaInsertSibling (new, el, before, document);
       /* register this element in the editing history */
       TtaOpenUndoSequence (document, firstSel, lastSel, firstChar,
			    lastChar);
       TtaRegisterElementCreate (new, document);
       TtaCloseUndoSequence (document);
       return (new);
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateBase (Document document, View view)
{
  Element             el;

  el = InsertWithinHead (document, view, HTML_EL_BASE);
  if (el != NULL)
    TtaSelectElement (document, el);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMeta (Document document, View view)
{
  Element             el;

  el = InsertWithinHead (document, view, HTML_EL_META);
  if (el != NULL)
    TtaSelectElement (document, el);

}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateLinkInHead (Document document, View view)
{
  Element             el;

  el = InsertWithinHead (document, view, HTML_EL_LINK);
  if (el != NULL)
    {
      /* The link element is a new created one */
      UseLastTarget = FALSE;
      IsNewAnchor = TRUE;
      /* Select a new destination */
      SelectDestination (document, el, FALSE);
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateStyle (Document document, View view)
{
  Element             el, child;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;

  el = InsertWithinHead (document, view, HTML_EL_STYLE_);
  if (el != NULL)
    {
      /* create an attribute type="text/css" */
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_Notation;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, document);
      TtaSetAttributeText (attr, "text/css", el, document);
      child = TtaGetFirstChild (el);
      if (child)
	TtaSelectElement (document, child);
      else
	TtaSelectElement (document, el);
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateComment (Document document, View view)
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Comment_;
       TtaInsertElement (elType, document);
     }
}

/*----------------------------------------------------------------------
  CreateScript
  ----------------------------------------------------------------------*/
void CreateScript (Document document, View view)
{
   SSchema             docSchema;
   ElementType         elType;
   Element             el, head;
   int                 i, j;

   /* test if we have to insert a script in the document head */
   head = NULL;
   TtaGiveFirstSelectedElement (document, &el, &i, &j);
   docSchema = TtaGetDocumentSSchema (document);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (elType.ElTypeNum != HTML_EL_HEAD ||
	   strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	 {
	   if (!strcmp(TtaGetSSchemaName (docSchema), "HTML"))
	     {
	       /* it's a HTML document search the head element */
	       elType.ElTypeNum = HTML_EL_HEAD;
	       head = TtaSearchTypedElement (elType, SearchForward,
					     TtaGetMainRoot (document));
	     }
	 }
     }

   if (el == NULL || el == head  || TtaIsAncestor (el, head))
     /* insert within the head */
     InsertWithinHead (document, view, HTML_EL_SCRIPT_);
   else
     {
       /* create Script in the body if we are within an HTML document
	  and within an HTML element */
       elType = TtaGetElementType (el);
       if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
           !strcmp(TtaGetSSchemaName (docSchema), "HTML"))
	 {
	   elType.ElTypeNum = HTML_EL_SCRIPT_;
	   TtaCreateElement (elType, document);
	 }
     }
}

/*----------------------------------------------------------------------
  HTMLelementAllowed
  ----------------------------------------------------------------------*/
static ThotBool HTMLelementAllowed (Document document)
{
  ElementType         elType;
  Element             el, ancestor, sibling;
  int                 firstChar, lastChar;

  if (!TtaGetDocumentAccessMode (document))
    /* the document is in ReadOnly mode */
    return FALSE;

  TtaGiveFirstSelectedElement (document, &el, &firstChar, &lastChar);
  if (el == NULL)
    /* no selection */
    return FALSE;
         
  elType = TtaGetElementType (el);
  if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
    /* within an HTML element */
    return TRUE;
  else
    /* not within an HTML element */
    {
      /* if the selection is at the beginning or at the end of an equation
	 or a drawing, TtaCreateElement will create the new HTML element right
	 before or after the MathML or Graphics object. */
      if (!TtaIsSelectionEmpty())
        return FALSE;
      
      if (firstChar <= 1)
        /* selection starts at the beginning of an element */
        {
	  sibling = el;
	  ancestor = el;
	  TtaPreviousSibling (&sibling);
	  while (sibling == NULL && ancestor != NULL)
	    {
	      ancestor = TtaGetParent (ancestor);
	      if (ancestor)
		{
		  elType = TtaGetElementType (ancestor);
		  if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
		    /* this is an HTML element */
		    return TRUE;
		  sibling = ancestor;
		  TtaPreviousSibling (&sibling);
		}
	    }
        }
     
      if ((lastChar == 0 && firstChar == 0) ||
	  lastChar >=  TtaGetElementVolume (el))
        /* selection is at the end of an element */
        {
	  sibling = el;
	  ancestor = el;
	  TtaNextSibling (&sibling);
	  while (sibling == NULL && ancestor != NULL)
	    {
	      ancestor = TtaGetParent (ancestor);
	      if (ancestor)
		{
		  elType = TtaGetElementType (ancestor);
		  if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
		    /* this is an HTML element */
		    return TRUE;
		  sibling = ancestor;
		  TtaNextSibling (&sibling);
		}
	    }
        }
      return FALSE;
    }
}

/*----------------------------------------------------------------------
  CreateHTMLelement
  ----------------------------------------------------------------------*/
static void CreateHTMLelement (int typeNum, Document document)
{
  ElementType         elType;

  if (HTMLelementAllowed (document))
    {
      TtaSetDisplayMode (document, SuspendDisplay);
      elType.ElSSchema = TtaGetSSchema ("HTML", document);
      elType.ElTypeNum = typeNum;
      TtaCreateElement (elType, document);
      TtaSetDisplayMode (document, DisplayImmediately);
    }
}

/*----------------------------------------------------------------------
  CreateParagraph
  ----------------------------------------------------------------------*/
void CreateParagraph (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Paragraph, document);
}

/*----------------------------------------------------------------------
  CreateHeading1
  ----------------------------------------------------------------------*/
void CreateHeading1 (Document document, View view)
{
   CreateHTMLelement (HTML_EL_H1, document);
}

/*----------------------------------------------------------------------
  CreateHeading2
  ----------------------------------------------------------------------*/
void CreateHeading2 (Document document, View view)
{
   CreateHTMLelement (HTML_EL_H2, document);
}

/*----------------------------------------------------------------------
  CreateHeading3
  ----------------------------------------------------------------------*/
void CreateHeading3 (Document document, View view)
{
   CreateHTMLelement (HTML_EL_H3, document);
}

/*----------------------------------------------------------------------
  CreateHeading4
  ----------------------------------------------------------------------*/
void CreateHeading4 (Document document, View view)
{
   CreateHTMLelement (HTML_EL_H4, document);
}

/*----------------------------------------------------------------------
  CreateHeading5
  ----------------------------------------------------------------------*/
void CreateHeading5 (Document document, View view)
{
   CreateHTMLelement (HTML_EL_H5, document);
}


/*----------------------------------------------------------------------
  CreateHeading6
  ----------------------------------------------------------------------*/
void CreateHeading6 (Document document, View view)
{
   CreateHTMLelement (HTML_EL_H6, document);
}


/*----------------------------------------------------------------------
  CreateList
  ----------------------------------------------------------------------*/
void CreateList (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Unnumbered_List, document);
}

/*----------------------------------------------------------------------
  CreateNumberedList
  ----------------------------------------------------------------------*/
void CreateNumberedList (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Numbered_List, document);
}

/*----------------------------------------------------------------------
  CreateDefinitionList
  ----------------------------------------------------------------------*/
void CreateDefinitionList (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Definition_List, document);
}

/*----------------------------------------------------------------------
  CreateDefinitionTerm
  ----------------------------------------------------------------------*/
void CreateDefinitionTerm (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Term, document);
}

/*----------------------------------------------------------------------
  CreateDefinitionDef
  ----------------------------------------------------------------------*/
void CreateDefinitionDef (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Definition, document);
}

/*----------------------------------------------------------------------
  CreateHorizontalRule
  ----------------------------------------------------------------------*/
void CreateHorizontalRule (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Horizontal_Rule, document);
}

/*----------------------------------------------------------------------
  CreateBlockQuote
  ----------------------------------------------------------------------*/
void CreateBlockQuote (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Block_Quote, document);
}

/*----------------------------------------------------------------------
  CreatePreformatted
  ----------------------------------------------------------------------*/
void CreatePreformatted (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Preformatted, document);
}

/*----------------------------------------------------------------------
  CreateRuby
  Create a ruby element.
  If the current selection is simply a caret, create an empty simple ruby at
  that position.
  If some text/elements are selected, create a simple ruby element at that
  position and move the selected elements within the rb element.
  ----------------------------------------------------------------------*/
void CreateRuby (Document document, View view)
{
  ElementType   elType;
  Element       el, selEl, rbEl, rubyEl, firstEl, lastEl, nextEl, prevEl;
  int           i, j, lg, firstSelectedChar, lastSelectedChar,
                oldStructureChecking;
  ThotBool      error;
  DisplayMode   dispMode;

  if (HTMLelementAllowed (document))
    {
      elType.ElSSchema = TtaGetSSchema ("HTML", document);
      elType.ElTypeNum = HTML_EL_complex_ruby;
      /* if we are already within a ruby element, return immediately */
      TtaGiveFirstSelectedElement (document, &firstEl, &firstSelectedChar, &j);
      if (TtaGetTypedAncestor (firstEl, elType))
	return;
      elType.ElTypeNum = HTML_EL_simple_ruby;
      if (TtaGetTypedAncestor (firstEl, elType))
	return;
      /* stop displaying changes that will be made */
      dispMode = TtaGetDisplayMode (document);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (document, DeferredDisplay);
      selEl = NULL;
      rbEl = NULL;
      rubyEl = NULL;
      if (TtaIsSelectionEmpty())
	/* selection is simply a caret */
	{
	  /* create a simple_ruby element at the current position */
	  TtaCreateElement (elType, document);
	  /* get the rb element that has just been created within the ruby */
	  TtaGiveFirstSelectedElement (document, &el, &i, &j);
	  if (el)
	    {
	      selEl = el; /* empty leaf within the rb element */
	      rbEl = TtaGetParent (el);
	      /* get the CHOICE element that follows the rb element and
		 delete it */
	      el = rbEl;
	      TtaNextSibling (&el);
	      if (el)
		TtaDeleteTree (el, document);
	      rubyEl = TtaGetParent (rbEl);
	    }
	}
      else if (IsCharacterLevelElement (firstEl))
	/* there are some elements/text selected. Make it the content
	   of the rb element */
	{
	  TtaGiveLastSelectedElement(document, &lastEl, &i, &lastSelectedChar);
	  if (TtaGetParent (firstEl) == TtaGetParent (lastEl))
	    /* all selected elements are siblings */
	    {
	      /* check if there are some ruby elements within the selected
		 elements */
	      error = FALSE;
	      el = firstEl;
	      while (el && !error)
		{
		  elType = TtaGetElementType (el);
		  if ((elType.ElTypeNum == HTML_EL_simple_ruby ||
		       elType.ElTypeNum == HTML_EL_complex_ruby) &&
		      !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
		    /* it's a ruby element. Error */
		    error = TRUE;
		  else
		    {
		    elType.ElSSchema = TtaGetSSchema ("HTML", document);
		    elType.ElTypeNum = HTML_EL_simple_ruby;
		    if (TtaSearchTypedElement (elType, SearchInTree, el))
		      error = TRUE;
		    else
		      {
			elType.ElTypeNum = HTML_EL_complex_ruby;
			if (TtaSearchTypedElement (elType, SearchInTree, el))
			  error = TRUE;
		      }
		    }
		  TtaGiveNextElement (document, &el, lastEl);
		}
	      if (!error)
		{
		  TtaUnselect (document);
		  /* split the last element if it's a character string */
		  elType = TtaGetElementType (lastEl);
		  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		    /* it's a text element */
		    {
		      lg = TtaGetElementVolume (lastEl);
		      if (lastSelectedChar <= lg && lastSelectedChar > 1)
			/* the last selected element is only partly selected.
			   Split it */
			TtaSplitText (lastEl, lastSelectedChar, document);
		    }
		  /* process the first selected element */
		  elType = TtaGetElementType (firstEl);
		  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		    /* it's a text element */
		    if (firstSelectedChar > 1)
		      /* that element is only partly selected. Split it */
		      {
			el = firstEl;
			TtaSplitText (firstEl, firstSelectedChar, document);
			TtaNextSibling (&firstEl);
			if (lastEl == el)
			  /* we have to change the end of selection because the
			     last selected element was split */
			  lastEl = firstEl;
		      }
		  /* create a ruby element with a rb element and moves all
		     selected elements within the new rb element */
		  TtaOpenUndoSequence (document, firstEl, lastEl, 0, 0);
		  elType.ElTypeNum = HTML_EL_simple_ruby;
		  rubyEl = TtaNewElement (document, elType);
		  TtaInsertSibling (rubyEl, firstEl, TRUE, document);
		  elType.ElTypeNum = HTML_EL_rb;
		  rbEl = TtaNewElement (document, elType);
		  TtaInsertFirstChild (&rbEl, rubyEl, document);
		  TtaRegisterElementCreate (rubyEl, document);
		  el = firstEl;
		  prevEl = NULL;
		  while (el)
		    {
		      if (el == lastEl)
			nextEl = NULL;
		      else
			{
			  nextEl = el;
			  TtaGiveNextElement (document, &nextEl, lastEl);
			}
		      TtaRegisterElementDelete (el, document);
		      TtaRemoveTree (el, document);
		      if (!prevEl)
			TtaInsertFirstChild  (&el, rbEl, document);
		      else
			TtaInsertSibling (el, prevEl, FALSE, document);
		      prevEl = el;
		      TtaRegisterElementCreate (el, document);
		      el = nextEl;
		    }
		  TtaCloseUndoSequence (document);
		}
	    }
	}
      if (rbEl)
	/* a rb element has been created. create the other elements within
	   the new ruby element */
	{
	  /* create a first rp element after the rb element */
	  oldStructureChecking = TtaGetStructureChecking (document);
	  TtaSetStructureChecking (0, document);
	  elType.ElTypeNum = HTML_EL_rp;
	  el = TtaNewTree (document, elType, "");
	  TtaInsertSibling (el, rbEl, FALSE, document);
	  prevEl = el;
	  el = TtaGetFirstChild (el);
	  TtaSetTextContent (el, "(", TtaGetDefaultLanguage (), document);
	  /* create a rt element after the first rp element */
	  elType.ElTypeNum = HTML_EL_rt;
	  el = TtaNewTree (document, elType, "");
	  TtaInsertSibling (el, prevEl, FALSE, document);
	  if (!selEl)
	    /* nothing to be selected. Select the first leaf within the
	       new rt element */
	    selEl = TtaGetFirstChild (el);
	  prevEl = el;
	  /* create a second rp element after the rt element */
	  elType.ElTypeNum = HTML_EL_rp;
	  el = TtaNewTree (document, elType, "");
	  TtaInsertSibling (el, prevEl, FALSE, document);
	  el = TtaGetFirstChild (el);
	  TtaSetTextContent (el, ")", TtaGetDefaultLanguage (), document);
	  TtaSetStructureChecking (oldStructureChecking, document);
	  /* create a text element after the ruby element, to allow the
	     user to add some more text after the ruby */
	  el = rubyEl;
	  TtaNextSibling (&el);
	  if (!el)
	    {
	      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	      el = TtaNewElement (document, elType);
	      TtaInsertSibling (el, rubyEl, FALSE, document);
	    }
	}
      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (document, dispMode);
      /* update the selection */
      if (selEl)
	TtaSelectElement (document, selEl);
    }
}

/*----------------------------------------------------------------------
  CreateAddress
  ----------------------------------------------------------------------*/
void CreateAddress (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Address, document);
}

/*----------------------------------------------------------------------
  CreateTable
  ----------------------------------------------------------------------*/
void CreateTable (Document document, View view)
{
   ElementType         elType;
   Element             el, new, cell, row;
   AttributeType       attrType;
   Attribute           attr;
   char              stylebuff[50];
   int                 firstChar, i;

   if (!HTMLelementAllowed (document))
      return;

   elType.ElSSchema = TtaGetSSchema ("HTML", document);
   if (elType.ElSSchema)
     {
       /* check the selection */
       if (TtaIsSelectionEmpty ())
	 /* selection empty.  Display the Table dialogue box */
	 {
	   NumberRows = 2;
	   NumberCols = 2;
	   TBorder = 1;
#ifdef _WINDOWS
	   CreateTableDlgWindow (NumberCols, NumberRows, TBorder);
#else  /* !_WINDOWS */
	   TtaNewForm (BaseDialog + TableForm, TtaGetViewFrame (document, 1),
		       TtaGetMessage (1, BTable), TRUE, 1, 'L', D_CANCEL);
	   TtaNewNumberForm (BaseDialog + TableCols, BaseDialog + TableForm,
			     TtaGetMessage (AMAYA, AM_COLS), 1, 50, TRUE);
	   TtaNewNumberForm (BaseDialog + TableRows, BaseDialog + TableForm,
			     TtaGetMessage (AMAYA, AM_ROWS), 1, 200, TRUE);
	   TtaNewNumberForm (BaseDialog + TableBorder, BaseDialog + TableForm,
			     TtaGetMessage (AMAYA, AM_BORDER), 0, 50, TRUE);
	   TtaSetNumberForm (BaseDialog + TableCols, NumberCols);
	   TtaSetNumberForm (BaseDialog + TableRows, NumberRows);
	   TtaSetNumberForm (BaseDialog + TableBorder, TBorder);
	   TtaSetDialoguePosition ();
	   TtaShowDialogue (BaseDialog + TableForm, FALSE);
	   /* wait for an answer */
	   TtaWaitShowDialogue ();
#endif /* !_WINDOWS */
	   if (!UserAnswer)
	     return;
	 }
       else
	 {
	   NumberRows = 0;
	   NumberCols = 0;
	   TBorder = 1;
	 } 

       TtaSetDisplayMode (document, SuspendDisplay);
       TtaLockTableFormatting ();
       elType.ElTypeNum = HTML_EL_Table;
       TtaCreateElement (elType, document);

       /* get the new Table element */
       TtaGiveFirstSelectedElement (document, &el, &firstChar, &i);
       if (el != NULL)
         el = TtaGetTypedAncestor (el, elType);
       if (el != NULL)
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   if (TtaGetDocumentProfile(document) == L_Basic)
	     {
	       /* remove the Border attribute */
	       attrType.AttrTypeNum = HTML_ATTR_Border;
	       attr = TtaGetAttribute (el, attrType);
	       if (attr != NULL)
		 TtaRemoveAttribute (el, attr, document);
	       /* generate a border style */
	       attrType.AttrTypeNum = HTML_ATTR_Style_;
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (el, attr, document);
	       strcpy (stylebuff, "border:  ");
	       sprintf (stylebuff, "border: solid %dpx", TBorder);
	       TtaSetAttributeText (attr, stylebuff, el, document);	       
	     }
	   else
	     {
	       /* take care of the Border attribute */
	       attrType.AttrTypeNum = HTML_ATTR_Border;
	       attr = TtaGetAttribute (el, attrType);
	       if (attr != NULL && TBorder == 0)
		 /* the table has a Border attribute but the user don't want
		    any border. Remove the attribute */
		 TtaRemoveAttribute (el, attr, document);
	       else
		 {
		   if (attr == NULL)
		     /* the Table has no Border attribute, create one */
		     {
		       attr = TtaNewAttribute (attrType);
		       TtaAttachAttribute (el, attr, document);
		     }
		   TtaSetAttributeValue (attr, TBorder, el, document);
		 }
	     }

	   if (NumberCols > 1)
	     {
	       elType.ElTypeNum = HTML_EL_Table_cell;
	       cell = TtaSearchTypedElement (elType, SearchInTree, el);
	       if (cell == NULL)
		 {
		   /* no table cell found, it must be a data cell */
		   elType.ElTypeNum = HTML_EL_Data_cell;
		   cell = TtaSearchTypedElement (elType, SearchInTree, el);
		 } 
	       while (NumberCols > 1)
		 {
		   new = TtaNewTree (document, elType, "");
		   TtaInsertSibling (new, cell, FALSE, document);
		   NumberCols--;
		 }
	     } 
	   if (NumberRows > 1)
	     {
	       elType.ElTypeNum = HTML_EL_Table_row;
	       row = TtaSearchTypedElement (elType, SearchInTree, el);
	       while (NumberRows > 1)
		 {
		   new = TtaNewTree (document, elType, "");
		   TtaInsertSibling (new, row, FALSE, document);
		   NumberRows--;
		 }
	     } 
	   CheckAllRows (el, document, FALSE, FALSE);
	 }
       TtaUnlockTableFormatting ();
       TtaSetDisplayMode (document, DisplayImmediately);
     }
}

/*----------------------------------------------------------------------
  CreateCaption
  ----------------------------------------------------------------------*/
void CreateCaption (Document document, View view)
{
   ElementType         elType;
   Element             el;
   int                 i, j;

   TtaGiveFirstSelectedElement (document, &el, &i, &j);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	 {
	   /* it's an HTML element */
	   if (elType.ElTypeNum != HTML_EL_Table && TtaIsSelectionEmpty ())
	     {
	       /* move the selection to the enclosing table */
	       elType.ElTypeNum = HTML_EL_Table;
	       el = TtaGetTypedAncestor (el, elType);
	       if (el == NULL)
		 return;
	       else
		 {
		   el = TtaGetFirstChild (el);
		   TtaSelectElement (document, el);
		 }
	     }
	   /* create the Caption */
	   elType.ElTypeNum = HTML_EL_CAPTION;
	   TtaCreateElement (elType, document);
	 }
     }
}

/*----------------------------------------------------------------------
  CreateColgroup
  ----------------------------------------------------------------------*/
void CreateColgroup (Document document, View view)
{
   ElementType         elType;
   Element             el, child;
   int                 i, j;

   TtaGiveFirstSelectedElement (document, &el, &i, &j);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	 {
	   /* it's an HTML element */
	   if (elType.ElTypeNum == HTML_EL_COLGROUP ||
		    elType.ElTypeNum == HTML_EL_COL)
	     {
	       /* insert after the curent element */
	       child = el;
	       /* create the Colgroup element */
	       elType.ElTypeNum = HTML_EL_COLGROUP;
	       el = TtaNewTree (document, elType, "");
	       TtaInsertSibling (el, child, FALSE, document);
	       /* update the selection */
	       child = TtaGetFirstChild (el);
	       if (child == NULL)
		 TtaSelectElement (document, el);
	       else
		 TtaSelectElement (document, child);
	     }
	   else
	     {
	       if (elType.ElTypeNum != HTML_EL_Table)
		 {
		   /* move the selection after the CAPTION */
		   elType.ElTypeNum = HTML_EL_Table;
		   el = TtaGetTypedAncestor (el, elType);
		 }
	       if (el != NULL)
		 {
		   /* skip the CAPTION */
		   child = TtaGetFirstChild (el);
		   elType = TtaGetElementType (child);
		   if (elType.ElTypeNum == HTML_EL_CAPTION)
		     {
		       TtaNextSibling (&child);
		       elType = TtaGetElementType (child);
		     }
		   if (elType.ElTypeNum == HTML_EL_Cols)
		     /* select the first COL or COLGROUP */
		     child = TtaGetFirstChild (child);
		   /* move the selection if there is no extension */
		   if (TtaIsSelectionEmpty ())
		     TtaSelectElement (document, child);
		   /* create the COLGROUP element */
		   elType.ElTypeNum = HTML_EL_COLGROUP;
		   TtaCreateElement (elType, document);
		   TtaGiveFirstSelectedElement (document, &el, &i, &j);
		   /* create a COL element within */
		   elType.ElTypeNum = HTML_EL_COL;
		   child = TtaNewTree (document, elType, "");
		   TtaInsertFirstChild (&child, el, document);
		   /* update the selection */
		   TtaSelectElement (document, child);
		 }
	     }
	 }
     }
}

/*----------------------------------------------------------------------
  CreateCol
  ----------------------------------------------------------------------*/
void CreateCol (Document document, View view)
{
   ElementType         elType;
   Element             el, child;
   int                 i, j;

   TtaGiveFirstSelectedElement (document, &el, &i, &j);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	 {
	   /* it's an HTML element */
	   if (elType.ElTypeNum == HTML_EL_COLGROUP)
	     {
	       /* insert within the curent element */
	       /* create the Colgroup element */
	       elType.ElTypeNum = HTML_EL_COL;
	       child = TtaNewTree (document, elType, "");
	       TtaInsertFirstChild (&child, el, document);
	       /* update the selection */
	       TtaSelectElement (document, child);
	     }
           if (elType.ElTypeNum == HTML_EL_COL)
	     {
	       /* insert after the curent element */
	       child = el;
	       /* create the COL element */
	       elType.ElTypeNum = HTML_EL_COL;
	       el = TtaNewTree (document, elType, "");
	       TtaInsertSibling (el, child, FALSE, document);
	       /* update the selection */
	       TtaSelectElement (document, el);
	     }
	   else
	     {
	       if (elType.ElTypeNum != HTML_EL_Table)
		 {
		   /* move the selection after the CAPTION */
		   elType.ElTypeNum = HTML_EL_Table;
		   el = TtaGetTypedAncestor (el, elType);
		 }
	       if (el != NULL)
		 {
		   /* skip the CAPTION */
		   child = TtaGetFirstChild (el);
		   elType = TtaGetElementType (child);
		   if (elType.ElTypeNum == HTML_EL_CAPTION)
		     {
		       TtaNextSibling (&child);
		       elType = TtaGetElementType (child);
		     }
		   if (elType.ElTypeNum == HTML_EL_Cols)
		     {
		       /* select the first COL */
		       child = TtaGetFirstChild (child);
		       elType = TtaGetElementType (child);
		       if (elType.ElTypeNum == HTML_EL_COLGROUP)
			 child = TtaGetFirstChild (child);
		     }
		   /* move the selection if there is no extension */
		   if (TtaIsSelectionEmpty ())
		     TtaSelectElement (document, child);
		   /* create the COL element */
		   elType.ElTypeNum = HTML_EL_COL;
		   TtaCreateElement (elType, document);
		 }
	     }
	 }
     }
}

/*----------------------------------------------------------------------
  CreateTHead
  ----------------------------------------------------------------------*/
void CreateTHead (Document document, View view)
{
   CreateHTMLelement (HTML_EL_thead, document);
}

/*----------------------------------------------------------------------
  CreateTBody
  ----------------------------------------------------------------------*/
void CreateTBody (Document document, View view)
{
   CreateHTMLelement (HTML_EL_tbody, document);
}

/*----------------------------------------------------------------------
  CreateTFoot
  ----------------------------------------------------------------------*/
void CreateTFoot (Document document, View view)
{
   CreateHTMLelement (HTML_EL_tfoot, document);
}

/*----------------------------------------------------------------------
  CreateRow
  ----------------------------------------------------------------------*/
void CreateRow (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Table_row, document);
}

/*----------------------------------------------------------------------
  CreateDataCell
  ----------------------------------------------------------------------*/
void CreateDataCell (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Data_cell, document);
}

/*----------------------------------------------------------------------
  CreateHeadingCell
  ----------------------------------------------------------------------*/
void CreateHeadingCell (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Heading_cell, document);
}

/*----------------------------------------------------------------------
  DeleteColumn
  Delete a column in a table.
  ----------------------------------------------------------------------*/
void DeleteColumn (Document document, View view)
{
  Element             el, cell, colHead, selCell, leaf;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  Document            refDoc;
  SSchema	      HTMLSSchema;
  char                name[50];
  int                 firstchar, lastchar, len;
  ThotBool            selBefore;

  /* get the first selected element */
  TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
  if (el != NULL)
    {
      elType = TtaGetElementType (el);
      HTMLSSchema = TtaGetSSchema ("HTML", document);
      if (elType.ElSSchema == HTMLSSchema &&
	  elType.ElTypeNum == HTML_EL_Data_cell)
	  cell = el;
      else
	{
	  elType.ElSSchema = HTMLSSchema;
	  elType.ElTypeNum = HTML_EL_Data_cell;
	  cell = TtaGetTypedAncestor (el, elType);
	  if (cell == NULL)
	    {
	      elType.ElTypeNum = HTML_EL_Heading_cell;
	      cell = TtaGetTypedAncestor (el, elType);
	    }
	}
      if (cell != NULL)
	{
	  /* prepare the new selection */
	  selCell = cell;
	  TtaNextSibling (&selCell);
	  if (selCell)
	     selBefore = FALSE;
	  else
	     {
	     selCell = cell;
	     TtaPreviousSibling (&selCell);
	     selBefore = TRUE;
	     }
	  /* get current column */
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Ref_column;
	  attr = TtaGetAttribute (cell, attrType);
	  if (attr != NULL)
	    {
	      TtaGiveReferenceAttributeValue (attr, &colHead, name, &refDoc);
	      TtaOpenUndoSequence (document, el, el, firstchar, lastchar);
	      /* remove column */
	      RemoveColumn (colHead, document, FALSE, FALSE);
	      /* set new selection */
	      if (selBefore)
	         leaf = TtaGetLastLeaf (selCell);
	      else
	         leaf = TtaGetFirstLeaf (selCell);
	      elType = TtaGetElementType (leaf);
	      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	        if (selBefore)
	           {
	           len = TtaGetTextLength (leaf);
		   TtaSelectString (document, leaf, len+1, len);
		   }
	        else
		   TtaSelectString (document, leaf, 1, 0);
	      else
		TtaSelectElement (document, leaf);
	      TtaCloseUndoSequence (document);
	      TtaSetDocumentModified (document);
	    }
	}
    }
}

/*----------------------------------------------------------------------
   	GetEnclosingForm creates if necessary and returns the	
   		enclosing form element.				
  ----------------------------------------------------------------------*/
Element GetEnclosingForm (Document document, View view)
{
   Element             el;
   ElementType         elType;
   int                 firstchar, lastchar;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
   if (el != NULL)
     {
	/* there is a selection */
	elType = TtaGetElementType (el);
	while (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0
	       && elType.ElTypeNum != HTML_EL_BODY
	       && elType.ElTypeNum != HTML_EL_Form)
	  {
	     el = TtaGetParent (el);
	     if (el == NULL)
	       return (el);
	     elType = TtaGetElementType (el);
	  }

	if (elType.ElTypeNum != HTML_EL_Form)
	  {
	     /* it is not already a form */
	     elType.ElTypeNum = HTML_EL_Form;
	     TtaInsertElement (elType, document);
	     TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
	  }
     }
   return (el);
}

/*----------------------------------------------------------------------
  InsertForm creates a form element if there is no parent element
  which is a form.
  Return
   - the new created form element,
   - NULL if these element cannot be created
   - or the current selected element if there is already a parent form.

   withinP is TRUE if the current selection is within a paragraph in a form.
  ----------------------------------------------------------------------*/
static Element InsertForm (Document doc, View view, ThotBool *withinP)
{
   ElementType         elType;
   Element             el, parent, form;
   int                 firstchar, lastchar;
   int                 len;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
   *withinP = FALSE;
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       elType.ElTypeNum = HTML_EL_Form;
       /* within HTML element */
       form = TtaGetTypedAncestor (el, elType);
       if (form == NULL)
	 {
	   if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	     {
	       /* create the form element */
	       elType.ElTypeNum = HTML_EL_Form;
	       TtaCreateElement (elType, doc);
	       /* get the new created element */
	       TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
	       form = el;
	     }
	   else
	     /* cannot create a form element here */
	     el = NULL;
	 }
       else
	 {
	   /* there is a parent form element */
	   parent = el;
	   while (parent != form
		  && elType.ElTypeNum != HTML_EL_Paragraph
		  && elType.ElTypeNum != HTML_EL_Pseudo_paragraph
		  && !strcmp(TtaGetSSchemaName (elType.ElSSchema),
			      "HTML"))
	     {
	       parent = TtaGetParent (parent);
	       elType = TtaGetElementType (parent);
	     }
	   /* check whether the selected element is within a P element */
	   *withinP = (parent != form);
	   if (*withinP)
	     {
	       elType = TtaGetElementType (el);
	       if (elType.ElTypeNum == HTML_EL_Basic_Set)
		 {
		   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
		   parent = el;
		   el = TtaNewElement (doc, elType);
		   TtaInsertFirstChild (&el, parent, doc);
		 }
	       if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		 {
		   /* add a space if necessary */
		   len = TtaGetTextLength (el);
		   if (len == 0)
		     {
		       TtaSetTextContent (el, " ",
					  TtaGetDefaultLanguage (), doc);
		       TtaSelectString (doc, el, 2, 2);
		     }
		   
		 }
	     }
	 }
     }
   return (el);
}

/*----------------------------------------------------------------------
  CreateForm
  ----------------------------------------------------------------------*/
void CreateForm (Document doc, View view)
{
  Element           el;
  ThotBool          withinP;

  el = InsertForm (doc, view, &withinP);
}


/*----------------------------------------------------------------------
  CreateInputElement insert an input element:
  - within an existing paragraph generates input + text
  - in other case generates a paragraph including text + input + text
  ----------------------------------------------------------------------*/
static void CreateInputElement (Document doc, View view, int elInput)
{
   ElementType         elType;
   Element             el, input, parent;
   int                 firstchar, lastchar;
   ThotBool            withinP;

   /* create the form if necessary */
   el = InsertForm (doc, view, &withinP);
   if (el != NULL)
     {
       /* the element can be created */
       elType = TtaGetElementType (el);
       if (!withinP)
	 {
	   /* create the paragraph element */
	   elType.ElTypeNum = HTML_EL_Paragraph;
	   TtaInsertElement (elType, doc);
	   TtaGiveFirstSelectedElement (doc, &parent, &firstchar, &lastchar);
	   /* create the input element */
	   elType.ElTypeNum = elInput;
	   input = TtaNewTree (doc, elType, "");
	   TtaInsertFirstChild (&input, parent, doc);	   
	   /* Insert a text element before */
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   el = TtaNewElement (doc, elType);
	   TtaInsertSibling (el, input, TRUE, doc);
	 }
       else
	 {
	   /* create the input element */
	   elType.ElTypeNum = elInput;
	   TtaInsertElement (elType, doc);
	   TtaGiveFirstSelectedElement (doc, &input, &firstchar, &lastchar);
	   if (input)
	     {
	     elType = TtaGetElementType (input);
	     while (elType.ElTypeNum != elInput)
	       {
	       input = TtaGetParent (input);
	       elType = TtaGetElementType (input);
	       }
	     /* add a text before if needed */
	     elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	     el = input;
	     TtaPreviousSibling (&el);
	     if (el == NULL)
	       {
	       el = TtaNewElement (doc, elType);
	       TtaInsertSibling (el, input, TRUE, doc);
	       }
	     }
	 }
       /* Insert a text element after */
       if (input)
	 {
         el = input;
         TtaNextSibling (&el);
         if (el == NULL)
	   {
	   el = TtaNewElement (doc, elType);
	   TtaInsertSibling (el, input, FALSE, doc);
	   /* if it's not a HTML_EL_BUTTON_ or a SELECT
	      select the following text element */
	   if (elInput != HTML_EL_BUTTON_ && elInput != HTML_EL_Option_Menu)
	   TtaSelectElement (doc, el);
	   }
	 }
     }
}

/*----------------------------------------------------------------------
  CreateFieldset
  ----------------------------------------------------------------------*/
void CreateFieldset (Document document, View view)
{
   CreateHTMLelement (HTML_EL_FIELDSET, document);
}

/*----------------------------------------------------------------------
  CreateToggle
  ----------------------------------------------------------------------*/
void CreateToggle (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Checkbox_Input);
}

/*----------------------------------------------------------------------
  CreateRadio
  ----------------------------------------------------------------------*/
void CreateRadio (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Radio_Input);
}

/*----------------------------------------------------------------------
  UpdateAttrSelected
  ----------------------------------------------------------------------*/
void UpdateAttrSelected (NotifyAttribute * event)
{
   OnlyOneOptionSelected (event->element, event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrSelectedDeleted
  ----------------------------------------------------------------------*/
void AttrSelectedDeleted (NotifyAttribute * event)
{
   Element	menu;

   menu = TtaGetParent (event->element);
   OnlyOneOptionSelected (menu, event->document, FALSE);
}

/*----------------------------------------------------------------------
  DeleteAttrSelected
  ----------------------------------------------------------------------*/
ThotBool DeleteAttrSelected (NotifyAttribute * event)
{
   return TRUE;			/* refuse to delete this attribute */
}

/*----------------------------------------------------------------------
  CreateOption
  ----------------------------------------------------------------------*/
void CreateOption (Document doc, View view)
{
   ElementType         elType;
   Element             el, new;
   int                 firstchar, lastchar;

   /* create the form if necessary */
   CreateInputElement (doc, view, HTML_EL_Option_Menu);
   TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
   if (el != NULL)
     {
       /* create the option */
       elType = TtaGetElementType (el);
       elType.ElTypeNum = HTML_EL_Option;
       new = TtaNewTree (doc, elType, "");
       TtaInsertFirstChild (&new, el, doc);
       OnlyOneOptionSelected (new, doc, FALSE);
       /* Select the text element within the option */
       el = TtaGetFirstChild (new);
       TtaSelectElement (doc, el);
       TtaSelectView (doc, TtaGetViewFromName (doc, "Structure_view"));
     }
}

/*----------------------------------------------------------------------
  CreateOptGroup
  ----------------------------------------------------------------------*/
void CreateOptGroup (Document document, View view)
{
   CreateHTMLelement (HTML_EL_OptGroup, document);
}

/*----------------------------------------------------------------------
  CreateTextInput
  ----------------------------------------------------------------------*/
void CreateTextInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Text_Input);
}

/*----------------------------------------------------------------------
  CreatePasswordInput
  ----------------------------------------------------------------------*/
void CreatePasswordInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Password_Input);
}

/*----------------------------------------------------------------------
  CreateTextArea
  ----------------------------------------------------------------------*/
void CreateTextArea (Document doc, View view)
{
   CreateInputElement (doc, view, HTML_EL_Text_Area);
}


/*----------------------------------------------------------------------
  CreateImageInput
  ----------------------------------------------------------------------*/
void CreateImageInput (Document doc, View view)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  Element             el, input;
  char               *value;
  int                 length;
  int                 firstchar, lastchar;
  ThotBool            withinP;

  /* create the form if necessary */
  el = InsertForm (doc, view, &withinP);
  if (el != NULL)
    {
      /* the element can be created */
      elType = TtaGetElementType (el);
      if (!withinP)
	{
	  /* create the paragraph element */
	  elType.ElTypeNum = HTML_EL_Paragraph;
	  TtaInsertElement (elType, doc);
	}
      CreateImage (doc, view);
      TtaGiveFirstSelectedElement (doc, &input, &firstchar, &lastchar);
      if (input)
	{
	  elType = TtaGetElementType (input);
	  while (elType.ElTypeNum != HTML_EL_PICTURE_UNIT)
	    {
	      input = TtaGetParent (input);
	      elType = TtaGetElementType (input);
	    }
	  /* add the attribute isInput */
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_IsInput;
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (input, attr, doc);
	  
	  /* use the ALT value to generate the attribute NAME */
	  attrType.AttrTypeNum = HTML_ATTR_ALT;
	  attr = TtaGetAttribute (input, attrType);
	  if (attr)
	    {
	      length = TtaGetTextAttributeLength (attr) + 10;
	      value = TtaGetMemory (length);
	      TtaGiveTextAttributeValue (attr, value, &length);
	      attrType.AttrTypeNum = HTML_ATTR_NAME;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (input, attr, doc);
	      TtaSetAttributeText (attr, value, input, doc);
	      TtaFreeMemory (value);
	      /* Check attribute NAME or ID in order to make sure that its
		 value unique in the document */
	      MakeUniqueName (input, doc);
	    }
	  /* add a text before if needed */
	  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	  el = input;
	  TtaPreviousSibling (&el);
	  if (el == NULL)
	    {
	      el = TtaNewElement (doc, elType);
	      TtaInsertSibling (el, input, TRUE, doc);
	    }
	}
      /* Insert a text element after */
      el = input;
      TtaNextSibling (&el);
      if (el == NULL)
	{
	  el = TtaNewElement (doc, elType);
	  TtaInsertSibling (el, input, FALSE, doc);
	  /* if it's not a HTML_EL_BUTTON_ or a SELECT
	     select the following text element */
	  TtaSelectElement (doc, el);
	}
    }
}

/*----------------------------------------------------------------------
  CreateFileInput
  ----------------------------------------------------------------------*/
void CreateFileInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_File_Input);
}

/*----------------------------------------------------------------------
  CreateHiddenInput
  ----------------------------------------------------------------------*/
void CreateHiddenInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Hidden_Input);
}

/*----------------------------------------------------------------------
  CreateLabel
  ----------------------------------------------------------------------*/
void  CreateLabel (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_LABEL, doc);
}

/*----------------------------------------------------------------------
  CreatePushButton
  ----------------------------------------------------------------------*/
void  CreatePushButton (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_BUTTON_);
}

/*----------------------------------------------------------------------
  CreateSubmit
  ----------------------------------------------------------------------*/
void  CreateSubmit (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Submit_Input);
}

/*----------------------------------------------------------------------
  CreateReset
  ----------------------------------------------------------------------*/
void  CreateReset (Document doc, View view)
{
   CreateInputElement (doc, view, HTML_EL_Reset_Input);
}

/*----------------------------------------------------------------------
  CreateDivision
  ----------------------------------------------------------------------*/
void  CreateDivision (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Division, document);
}

/*----------------------------------------------------------------------
  CreateNOSCRIPT
  ----------------------------------------------------------------------*/
void  CreateNOSCRIPT (Document document, View view)
{
   CreateHTMLelement (HTML_EL_NOSCRIPT, document);
}

/*----------------------------------------------------------------------
  CreateObject
  ----------------------------------------------------------------------*/
void  CreateObject (Document document, View view)
{
   ElementType         elType;
   Element             child, el;
   Attribute           attr;
   AttributeType       attrType;
   char               *name1;
   int                 length;
   int                 firstchar, lastchar;

   if (HTMLelementAllowed (document))
     {
       TtaSetDisplayMode (document, SuspendDisplay);
       elType.ElSSchema = TtaGetSSchema ("HTML", document);
       elType.ElTypeNum = HTML_EL_Object;
       TtaCreateElement (elType, document);

       /* get the first selected element */
       TtaGiveFirstSelectedElement (document, &child, &firstchar, &lastchar);

       /* copy SRC attribute of Object_Image into data attribute of Object */
       el = TtaGetParent(child);
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_SRC;
       attr = TtaGetAttribute (child, attrType);
       if (attr != NULL)
	 {
	   length = TtaGetTextAttributeLength (attr);
	   if (length > 0)
	     {
	       name1 = TtaGetMemory (length + 1);
	       TtaGiveTextAttributeValue (attr, name1, &length);
	       attrType.AttrTypeNum = HTML_ATTR_data;
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (el, attr, document);
	       TtaSetAttributeText (attr, name1, el, document);
	       TtaFreeMemory (name1);
	     }
	 }
       TtaSetDisplayMode (document, DisplayImmediately);
     }
}

/*----------------------------------------------------------------------
  CreateParameter
  ----------------------------------------------------------------------*/
void  CreateParameter (Document document, View view)
{
   CreateHTMLelement (HTML_EL_Parameter, document);
}

/*----------------------------------------------------------------------
  CreateIFrame
  ----------------------------------------------------------------------*/
void  CreateIFrame (Document document, View view)
{
   CreateHTMLelement (HTML_EL_IFRAME, document);
}

/*----------------------------------------------------------------------
   CreateOrChangeLink
   If current selection is within an anchor, change that link, otherwise
   create a link.
  ----------------------------------------------------------------------*/
void  CreateOrChangeLink (Document doc, View view)
{
   Element             el;
   Attribute           attr;
   int                 firstSelectedChar, i;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

   UseLastTarget = FALSE;
   TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
   if (TtaIsReadOnly (el))
     /* the selected element is read-only */
     return;

   if (el != NULL)
     {
       /* Look if there is an enclosing anchor element */
       el = SearchAnchor (doc, el, &attr, TRUE);
       if (el == NULL)
	 {
	   /* The link element is a new created one */
	   IsNewAnchor = TRUE;
	   /* no anchor element, create a new link */
	   CreateAnchor (doc, view, TRUE);
	 }
       else
	 {
	   /* The link element already exists */
	   IsNewAnchor = FALSE;
	   /* There is an anchor. Just select a new destination */
	   SelectDestination (doc, el, TRUE);
	 }
     }
}

/*----------------------------------------------------------------------
   DeleteAnchor
   Delete the surrounding anchor.                    
  ----------------------------------------------------------------------*/
void DeleteAnchor (Document doc, View view)
{
   Element             firstSelectedElement, lastSelectedElement, anchor,
                       child, next, previous;
   int                 firstSelectedChar, lastSelectedChar, i;
   ElementType         elType;
   DisplayMode         dispMode;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &firstSelectedElement,
				&firstSelectedChar, &lastSelectedChar);
   if (firstSelectedElement == NULL)
      /* no selection. Do nothing */
      return;
   if (TtaIsReadOnly (firstSelectedElement))
     /* the selected element is read-only */
     return;

   elType = TtaGetElementType (firstSelectedElement);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
      /* the first selected element is not an HTML element. Do nothing */
      return;

   TtaGiveLastSelectedElement (doc, &lastSelectedElement, &i, 
			       &lastSelectedChar);
   TtaOpenUndoSequence (doc, firstSelectedElement, lastSelectedElement,
			firstSelectedChar, lastSelectedChar);
   if (elType.ElTypeNum == HTML_EL_Anchor)
     /* the first selected element is an anchor */
     {
       anchor = firstSelectedElement;
       /* prepare the elements to be selected later */
       firstSelectedElement = TtaGetFirstChild (anchor);
       lastSelectedElement = TtaGetLastChild (anchor);
       firstSelectedChar = 0;
       lastSelectedChar = 0;
     }
   else if (elType.ElTypeNum == HTML_EL_AREA)
     {
       anchor = firstSelectedElement;
       /* prepare the elements to be selected later */
       firstSelectedElement = NULL;
       lastSelectedElement = NULL;
       firstSelectedChar = 0;
       lastSelectedChar = 0;
     }
   else if (elType.ElTypeNum == HTML_EL_Cite)
     {
       SetOnOffCite (doc, 1);
       return;
     }
   else
     {
       /* search the surrounding Anchor element */
       elType.ElTypeNum = HTML_EL_Anchor;
       anchor = TtaGetTypedAncestor (firstSelectedElement, elType);
     }

   if (anchor != NULL)
     {
       /* ask Thot to stop displaying changes made in the document */
       dispMode = TtaGetDisplayMode (doc);
       if (dispMode == DisplayImmediately)
          TtaSetDisplayMode (doc, DeferredDisplay);
       TtaUnselect (doc);
       TtaRegisterElementDelete (anchor, doc);
       /* move all chidren of element anchor as sibling of the anchor */
       child = TtaGetFirstChild (anchor);
       previous = anchor;
       while (child != NULL)
	  {
	  next = child;
	  TtaNextSibling (&next);
	  TtaRemoveTree (child, doc);
	  TtaInsertSibling (child, previous, FALSE, doc);
          TtaRegisterElementCreate (child, doc);
	  previous = child;
	  child = next;
	  }
       /* prepare the next selection */
       if (firstSelectedElement == NULL)
	 firstSelectedElement = TtaGetPredecessor (anchor);
       if (firstSelectedElement == NULL)
	 firstSelectedElement = TtaGetSuccessor (anchor);
       if (firstSelectedElement == NULL)
	 firstSelectedElement = TtaGetParent (anchor);
       /* delete the anchor element itself */
       TtaDeleteTree (anchor, doc);
       TtaSetDocumentModified (doc);
       /* ask Thot to display changes made in the document */
       TtaSetDisplayMode (doc, dispMode);
     }

   TtaCloseUndoSequence (doc);
   /* set the selection */
   if (firstSelectedChar > 1)
     {
       if (firstSelectedElement == lastSelectedElement)
	  i = lastSelectedChar;
       else
	  i = TtaGetElementVolume (firstSelectedElement);
       TtaSelectString (doc, firstSelectedElement, firstSelectedChar, i);
     }
   else
      TtaSelectElement (doc, firstSelectedElement);
   if (firstSelectedElement != lastSelectedElement)
      TtaExtendSelection (doc, lastSelectedElement, lastSelectedChar);
}

/*----------------------------------------------------------------------
  CleanCache
  A frontend to the CleanCache function, called thru an Amaya menu
  ----------------------------------------------------------------------*/
void FlushCache (Document doc, View view)
{
  StopAllRequests (doc);
  libwww_CleanCache ();
}

/*----------------------------------------------------------------------
  ConfigColor
  A frontend to the Color configuration menu
  ----------------------------------------------------------------------*/
void ConfigColor (Document doc, View view)
{
   ColorConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigGeometry
  A frontend to the Geometry configuration menu
  ----------------------------------------------------------------------*/
void ConfigGeometry (Document doc, View view)
{
   GeometryConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigGeneral
  A frontend to the Browsing Editing configuration menu
  ----------------------------------------------------------------------*/
void ConfigGeneral (Document doc, View view)
{
   GeneralConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigPublish
  A frontend to the Browsing Editing configuration menu
  ----------------------------------------------------------------------*/
void ConfigPublish (Document doc, View view)
{
   PublishConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigCache
  A frontend to the cache configuration menu
  ----------------------------------------------------------------------*/
void ConfigCache (Document doc, View view)
{
  CacheConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigProxy
  A frontend to the proxy configuration menu
  ----------------------------------------------------------------------*/
void ConfigProxy (Document doc, View view)
{
  ProxyConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigLanNeg
  A frontend to the LanNeg configuration menu
  ----------------------------------------------------------------------*/
void ConfigLanNeg (Document doc, View view)
{
   LanNegConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigProfile
  A frontend to the profile configuration menu
  ----------------------------------------------------------------------*/
void ConfigProfile (Document doc, View view)
{
  ProfileConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigTemplates
  A frontend to the Templates configuration menu
  ----------------------------------------------------------------------*/
void ConfigTemplates (Document doc, View view)
{
   TemplatesConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  SaveOptions
  Saves the user modified configuration options
  ----------------------------------------------------------------------*/
void SaveOptions (Document doc, View view)
{
  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  ConfigAnnot
  A frontend to the Templates configuration menu
  ----------------------------------------------------------------------*/
void ConfigAnnot (Document doc, View view)
{
#ifdef ANNOTATIONS
     AnnotConfMenu (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  AnnotateDocument
  Frontend to the function that creates an annotation
  ----------------------------------------------------------------------*/
void AnnotateDocument (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Create (doc, view, ANNOT_useDocRoot);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  AnnotateDocument
  Frontend to the function that creates an annotation
  ----------------------------------------------------------------------*/
void AnnotateSelection (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Create (doc, view, ANNOT_useSelection);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  LoadAnnotations
  Frontend to the function that loads the annotations related to a document
  ----------------------------------------------------------------------*/
void LoadAnnotations (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Load (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  PostAnnotation
  Frontend to the function that posts an annotation to the server
  ----------------------------------------------------------------------*/
void PostAnnotation (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Post (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  DeleteAnnotation
  Frontend to the function that posts an annotation to the server
  ----------------------------------------------------------------------*/
void DeleteAnnotation (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Delete (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
   FilterAnnot
   Show/Hide the annotations
  ----------------------------------------------------------------------*/
void FilterAnnot (Document document, View view)
{
#ifdef ANNOTATIONS
  AnnotFilter (document, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  MoveAnnotationXPtr
  Move an annotation to the value stored in the XPointer
  ----------------------------------------------------------------------*/
void  MoveAnnotationXPtr (Document document, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Move (document, view, FALSE);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  MoveAnnotationSel
  Move an annotation in a document to the current selection
  ----------------------------------------------------------------------*/
void  MoveAnnotationSel (Document document, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Move (document, view, TRUE);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
   Show/Hide the annotations
  ----------------------------------------------------------------------*/
void ReplyToAnnotation (Document document, View view)
{
  /* for testing threading on the selection */
  ANNOT_Create (document, view, ANNOT_useDocRoot | ANNOT_isReplyTo);
}

/*----------------------------------------------------------------------
   Show/Hide the annotations
  ----------------------------------------------------------------------*/
void CustomQuery (Document document, View view)
{
#ifdef ANNOTATIONS
  CustomQueryMenuInit (document, view);
#endif /* ANNOTATIONS */
}

/*------------ Begin: WebDAV Modifications by Manuele Kirsch -----------*/
/* Note: There is no #include "davlib_f.h" in this file to prevent
 *       circular references between the header files.
 */        


/*----------------------------------------------------------------------
   LockDocument
   Lock document using WebDAV protocol
  ----------------------------------------------------------------------*/
void LockDocument (Document document, View view)
{
#ifdef DAV
    DAVLockDocument (document,view);
#endif /* DAV */
}


/*----------------------------------------------------------------------
   UnlockDocument
   Unlock document using WebDAV protocol
  ----------------------------------------------------------------------*/
void UnlockDocument (Document document, View view)
{
#ifdef DAV 
     DAVUnlockDocument (document,view);
#endif /* DAV */
}


/*----------------------------------------------------------------------
   PropDocument
   Get the document properties using WebDAV protocol
  ----------------------------------------------------------------------*/
void PropDocument (Document document, View view)
{
#ifdef DAV
    DAVProfindDocument (document,view);
#endif /* DAV */
}


/*----------------------------------------------------------------------
   CopyLockInformation
   Get the lock information of the document 
  ----------------------------------------------------------------------*/
void CopyLockInformation (Document document, View view)
{
#ifdef DAV
    DAVCopyLockInfo (document,view);
#endif /* DAV */
}




/*----------------------------------------------------------------------
   LockIndicator
   A toggle that indicates whether the document is locked.
  ----------------------------------------------------------------------*/
void LockIndicator (Document document, View view) 
{
#ifdef DAV
    DAVLockIndicator (document,view);
#endif /* DAV */

}


/*------------ End: WebDAV Modifications by Manuele Kirsch -----------*/
