/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
#include "HTMLtable_f.h"
#include "MENUconf_f.h"
#include "styleparser_f.h"
#include "UIcss_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                New (Document document, View view)
#else  /* __STDC__ */
void                New (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
  OpenNew (document, view, TRUE);
}
 
/*----------------------------------------------------------------------
   NewCss: Creates a new Css document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               NewCss (Document document, View view)
#else  /* __STDC__ */
void               NewCss (document, view)
Document           document; 
View               view;
#endif /* __STDC__ */
{
  OpenNew (document, view, FALSE);
}


/*----------------------------------------------------------------------
  InitializeNewDoc builds the initial contents of a new document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitializeNewDoc (STRING url, ThotBool isHTML)
#else  /* __STDC__ */
void                InitializeNewDoc (url, isHTML)
STRING              url;
ThotBool            isHTML;
#endif /* __STDC__ */
{
  ElementType          elType;
  Element              root, title, text, el, head, child, meta, body;
  AttributeType        attrType;
  Attribute            attr;
  Language             language;
  STRING               pathname;
  STRING               documentname;
  STRING               s;
  CHAR_T               tempfile[MAX_LENGTH];
  int                  doc;

  pathname = TtaAllocString (MAX_LENGTH);
  documentname = TtaAllocString (MAX_LENGTH);
  NormalizeURL (url, 0, pathname, documentname, NULL);
  if (isHTML)
    doc = InitDocView (0, documentname, docHTML, 0, FALSE);
  else
    doc = InitDocView (0, documentname, docCSS, 0, FALSE);
  TtaFreeMemory (documentname);
  TtaFreeMemory (pathname);

  /* save the document name into the document table */
  s = TtaWCSdup (url);
  TtaSetTextZone (doc, 1, 1, url);
  DocumentURLs[doc] = s;
  DocumentMeta[doc] = (DocumentMetaDataElement *) TtaGetMemory (sizeof (DocumentMetaDataElement));
  DocumentMeta[doc]->form_data = NULL;
  DocumentMeta[doc]->method = CE_ABSOLUTE;
  DocumentMeta[doc]->put_default_name = FALSE;
  DocumentMeta[doc]->xmlformat = FALSE;
  DocumentSource[doc] = 0;

  ResetStop (doc);
  language = TtaGetDefaultLanguage ();
  root = TtaGetMainRoot (doc);
  /* disable auto save */
  TtaSetDocumentBackUpInterval (doc, 0);

  elType = TtaGetElementType (root);
  attrType.AttrSSchema = elType.ElSSchema;
  if (isHTML)
    {
      LoadUserStyleSheet (doc);

      /* attach an attribute PrintURL to the document root */
      attrType.AttrTypeNum = HTML_ATTR_PrintURL;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);

      /* create a default title if there is no content in the TITLE element */
      elType.ElTypeNum = HTML_EL_TITLE;
      title = TtaSearchTypedElement (elType, SearchInTree, root);
      text = TtaGetFirstChild (title);
      if (TtaGetTextLength (text) == 0)
	TtaSetTextContent (text, TEXT("No title"), language, doc);
      UpdateTitle (title, doc);

      elType.ElTypeNum = HTML_EL_HEAD;
      head = TtaSearchTypedElement (elType, SearchInTree, root);

      /* create a Document_URL element as the first child of HEAD */
      elType.ElTypeNum = HTML_EL_Document_URL;
      el = TtaSearchTypedElement (elType, SearchInTree, head);
      if (el == NULL)
	{
	  /* there is no Document_URL element */
	  el = TtaNewElement (doc, elType);
	  TtaInsertFirstChild (&el, head, doc);
	  TtaSetAccessRight (el, ReadOnly, doc);
	}
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

      /* create a META element in the HEAD with attributes name="GENERATOR" */
      /* and content="Amaya" */
      child = TtaGetLastChild (head);
      elType.ElTypeNum = HTML_EL_META;
      meta = TtaNewElement (doc, elType);
      attrType.AttrTypeNum = HTML_ATTR_meta_name;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      TtaSetAttributeText (attr, TEXT("GENERATOR"), meta, doc);
      attrType.AttrTypeNum = HTML_ATTR_meta_content;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      ustrcpy (tempfile, HTAppName);
      ustrcat (tempfile, TEXT(" "));
      ustrcat (tempfile, HTAppVersion);
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
    }
  else
    {
      /* attach the default attribute PrintURL to the document root */
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
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SpellCheck (Document document, View view)
#else  /* __STDC__ */
void                SpellCheck (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             root, el, body;
   ElementType         elType;
   int                 firstchar, lastchar;

   root = TtaGetMainRoot (document);
   elType = TtaGetElementType (root);
   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
     {
       elType.ElTypeNum = HTML_EL_BODY;
       body = TtaSearchTypedElement (elType, SearchInTree, root);
       if (body == NULL)
	 return;
       /* if there is no Language attribute on the BODY, create one */
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Langue;
       attr = TtaGetAttribute (body, attrType);
       if (attr == NULL)
	 {
	   /* create the Language attribute */
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (body, attr, document);
	   TtaSetAttributeText (attr, TEXT("en"), body, document);
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
#ifdef __STDC__
void                CreateBreak (Document document, View view)
#else  /* __STDC__ */
void                CreateBreak (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, br, parent;
   int                 firstChar, lastChar;

   TtaGiveLastSelectedElement (document, &el, &firstChar, &lastChar);
   if (el == NULL)
     return;
   elType = TtaGetElementType (el);
   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) != 0)
     /* not within HTML element */
     return;

   elType.ElTypeNum = HTML_EL_BR;
   TtaCreateElement (elType, document);
   TtaGiveLastSelectedElement (document, &el, &firstChar, &lastChar);
   br = TtaGetParent (el);
   el = br;
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
#ifdef __STDC__
static Element   InsertWithinHead (Document document, View view, int elementT)
#else  /* __STDC__ */
static Element   InsertWithinHead (document, view, elementT)
Document         document;
View             view;
int              elementT;
#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, firstSel, lastSel, head, parent, new, title;
   SSchema             docSchema;
   int                 j, firstChar, lastChar;
   ThotBool            before;

   docSchema = TtaGetDocumentSSchema (document);
   if (ustrcmp(TtaGetSSchemaName (docSchema), TEXT("HTML")) != 0)
     /* not within an HTML document */
     return (NULL);
   else
     {
       elType.ElSSchema = docSchema;
       elType.ElTypeNum = HTML_EL_HEAD;
       parent = TtaGetMainRoot (document);
       head = TtaSearchTypedElement (elType, SearchForward, parent);
       
       /* give current position */
       TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &j);
       TtaGiveLastSelectedElement (document, &lastSel, &j, &lastChar);
       el = firstSel;
       if (firstSel == NULL || firstSel == head || !TtaIsAncestor (firstSel, head))
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
       TtaOpenUndoSequence (document, firstSel, lastSel, firstChar, lastChar);
       TtaRegisterElementCreate (new, document);
       TtaCloseUndoSequence (document);
       return (new);
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateBase (Document document, View view)
#else  /* __STDC__ */
void                CreateBase (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  Element             el;

  el = InsertWithinHead (document, view, HTML_EL_BASE);
  if (el != NULL)
    TtaSelectElement (document, el);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMeta (Document document, View view)
#else  /* __STDC__ */
void                CreateMeta (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  Element             el;

  el = InsertWithinHead (document, view, HTML_EL_META);
  if (el != NULL)
    TtaSelectElement (document, el);

}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateLinkInHead (Document document, View view)
#else  /* __STDC__ */
void                CreateLinkInHead (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                CreateStyle (Document document, View view)
#else  /* __STDC__ */
void                CreateStyle (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  Element             el;


  el = InsertWithinHead (document, view, HTML_EL_STYLE_);
  if (el != NULL)
    TtaSelectElement (document, el);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateComment (Document document, View view)
#else  /* __STDC__ */
void                CreateComment (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
     {
       elType.ElTypeNum = HTML_EL_Comment_;
       TtaInsertElement (elType, document);
     }
}

/*----------------------------------------------------------------------
  CreateScript
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateScript (Document document, View view)
#else  /* __STDC__ */
void                CreateScript (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
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
	   ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
	 {
	   if (!ustrcmp(TtaGetSSchemaName (docSchema), TEXT("HTML")))
	     {
	       /* it's a HTML document search the head element */
	       elType.ElTypeNum = HTML_EL_HEAD;
	       head = TtaSearchTypedElement (elType, SearchForward,  TtaGetMainRoot (document));
	     }
	 }
     }

   if (el == NULL || el == head  || TtaIsAncestor (el, head))
     /* insert within the head */
     InsertWithinHead (document, view, HTML_EL_SCRIPT);
   else
     {
       /* create Script in the body if we are within an HTML document
	  and within an HTML element */
       elType = TtaGetElementType (el);
       if (!ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) &&
           !ustrcmp(TtaGetSSchemaName (docSchema), TEXT("HTML")))
	 {
	   elType.ElTypeNum = HTML_EL_SCRIPT;
	   TtaCreateElement (elType, document);
	 }
     }
}

/*----------------------------------------------------------------------
  HTMLelementAllowed
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     HTMLelementAllowed (Document document)
#else  /* __STDC__ */
static ThotBool     HTMLelementAllowed (document)
Document            document;

#endif /* __STDC__ */
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
   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
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
		if (!ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
		  /* this is an HTML element */
		  return TRUE;
		sibling = ancestor;
		TtaPreviousSibling (&sibling);
	      }
	  }
        }
     
     if (lastChar == 0 || lastChar >=  TtaGetElementVolume (el))
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
		if (!ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
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
#ifdef __STDC__
static void         CreateHTMLelement (int typeNum, Document document)
#else  /* __STDC__ */
static void         CreateHTMLelement (typeNum, document)
int                 typeNum
Document            document;

#endif /* __STDC__ */
{
   ElementType         elType;

   if (HTMLelementAllowed (document))
     {
     TtaSetDisplayMode (document, SuspendDisplay);
     elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), document);
     elType.ElTypeNum = typeNum;
     TtaCreateElement (elType, document);
     TtaSetDisplayMode (document, DisplayImmediately);
     }
}

/*----------------------------------------------------------------------
  CreateParagraph
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateParagraph (Document document, View view)
#else  /* __STDC__ */
void                CreateParagraph (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Paragraph, document);
}

/*----------------------------------------------------------------------
  CreateHeading1
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading1 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading1 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_H1, document);
}

/*----------------------------------------------------------------------
  CreateHeading2
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading2 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading2 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_H2, document);
}

/*----------------------------------------------------------------------
  CreateHeading3
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading3 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading3 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_H3, document);
}

/*----------------------------------------------------------------------
  CreateHeading4
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading4 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading4 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_H4, document);
}

/*----------------------------------------------------------------------
  CreateHeading5
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading5 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading5 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_H5, document);
}


/*----------------------------------------------------------------------
  CreateHeading6
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading6 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading6 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_H6, document);
}


/*----------------------------------------------------------------------
  CreateList
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateList (Document document, View view)
#else  /* __STDC__ */
void                CreateList (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Unnumbered_List, document);
}

/*----------------------------------------------------------------------
  CreateNumberedList
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateNumberedList (Document document, View view)
#else  /* __STDC__ */
void                CreateNumberedList (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Numbered_List, document);
}

/*----------------------------------------------------------------------
  CreateDefinitionList
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDefinitionList (Document document, View view)
#else  /* __STDC__ */
void                CreateDefinitionList (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Definition_List, document);
}

/*----------------------------------------------------------------------
  CreateDefinitionTerm
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDefinitionTerm (Document document, View view)
#else  /* __STDC__ */
void                CreateDefinitionTerm (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Term, document);
}

/*----------------------------------------------------------------------
  CreateDefinitionDef
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDefinitionDef (Document document, View view)
#else  /* __STDC__ */
void                CreateDefinitionDef (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Definition, document);
}

/*----------------------------------------------------------------------
  CreateHorizontalRule
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHorizontalRule (Document document, View view)
#else  /* __STDC__ */
void                CreateHorizontalRule (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Horizontal_Rule, document);
}

/*----------------------------------------------------------------------
  CreateBlockQuote
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateBlockQuote (Document document, View view)
#else  /* __STDC__ */
void                CreateBlockQuote (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Block_Quote, document);
}

/*----------------------------------------------------------------------
  CreatePreformatted
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreatePreformatted (Document document, View view)
#else  /* __STDC__ */
void                CreatePreformatted (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Preformatted, document);
}

/*----------------------------------------------------------------------
  CreateAddress
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAddress (Document document, View view)
#else  /* __STDC__ */
void                CreateAddress (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Address, document);
}

/*----------------------------------------------------------------------
  CreateTable
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTable (Document document, View view)
#else  /* __STDC__ */
void                CreateTable (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, new, cell, row;
   AttributeType       attrType;
   Attribute           attr;
   int                 firstChar, i;

   if (!HTMLelementAllowed (document))
      return;

   elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), document);
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
	   CreateTableDlgWindow (BaseDialog, TableForm, TableCols, TableRows, TableBorder, NumberCols, NumberRows, TBorder);
#else  /* !_WINDOWS */
	   TtaNewForm (BaseDialog + TableForm, TtaGetViewFrame (document, 1), TtaGetMessage (1, BTable), TRUE, 1, 'L', D_CANCEL);
	   TtaNewNumberForm (BaseDialog + TableCols, BaseDialog + TableForm, TtaGetMessage (AMAYA, AM_COLS), 1, 50, TRUE);
	   TtaNewNumberForm (BaseDialog + TableRows, BaseDialog + TableForm, TtaGetMessage (AMAYA, AM_ROWS), 1, 200, TRUE);
	   TtaNewNumberForm (BaseDialog + TableBorder, BaseDialog + TableForm, TtaGetMessage (AMAYA, AM_BORDER), 1, 50, TRUE);
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
	   /* if the Table has no Border attribute, create one */
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Border;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr != NULL && TBorder == 1)
	     TtaRemoveAttribute (el, attr, document);
	   else
	     {
	       if (attr == NULL)
		 {
		   /* create the Border attribute */
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (el, attr, document);
		 }
	       TtaSetAttributeValue (attr, TBorder, el, document);
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
	   CheckAllRows (el, document);
	 }
       TtaUnlockTableFormatting ();
       TtaSetDisplayMode (document, DisplayImmediately);
     }
}

/*----------------------------------------------------------------------
  CreateCaption
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateCaption (Document document, View view)
#else  /* __STDC__ */
void                CreateCaption (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el;
   int                 i, j;

   TtaGiveFirstSelectedElement (document, &el, &i, &j);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (!ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
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
#ifdef __STDC__
void                CreateColgroup (Document document, View view)
#else  /* __STDC__ */
void                CreateColgroup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, child;
   int                 i, j;

   TtaGiveFirstSelectedElement (document, &el, &i, &j);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (!ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
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
#ifdef __STDC__
void                CreateCol (Document document, View view)
#else  /* __STDC__ */
void                CreateCol (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, child;
   int                 i, j;

   TtaGiveFirstSelectedElement (document, &el, &i, &j);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (!ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
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
#ifdef __STDC__
void                CreateTHead (Document document, View view)
#else  /* __STDC__ */
void                CreateTHead (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_thead, document);
}

/*----------------------------------------------------------------------
  CreateTBody
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTBody (Document document, View view)
#else  /* __STDC__ */
void                CreateTBody (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_tbody, document);
}

/*----------------------------------------------------------------------
  CreateTFoot
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTFoot (Document document, View view)
#else  /* __STDC__ */
void                CreateTFoot (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_tfoot, document);
}

/*----------------------------------------------------------------------
  CreateRow
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateRow (Document document, View view)
#else  /* __STDC__ */
void                CreateRow (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Table_row, document);
}

/*----------------------------------------------------------------------
  CreateDataCell
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDataCell (Document document, View view)
#else  /* __STDC__ */
void                CreateDataCell (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Data_cell, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeadingCell (Document document, View view)
#else  /* __STDC__ */
void                CreateHeadingCell (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Heading_cell, document);
}

/*----------------------------------------------------------------------
  DeleteColumn
  Delete a column in a table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeleteColumn (Document document, View view)
#else  /* __STDC__ */
void                DeleteColumn (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  Element             el, cell, colHead, selCell, leaf;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  Document            refDoc;
  SSchema	      HTMLSSchema;
  CHAR_T                name[50];
  int                 firstchar, lastchar, len;
  ThotBool            selBefore;

  /* get the first selected element */
  TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
  if (el != NULL)
    {
      elType = TtaGetElementType (el);
      HTMLSSchema = TtaGetSSchema (TEXT("HTML"), document);
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
#ifdef __STDC__
Element             GetEnclosingForm (Document document, View view)
#else  /* __STDC__ */
Element             GetEnclosingForm (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
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
	while (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) != 0
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
#ifdef __STDC__
static Element      InsertForm (Document doc, View view, ThotBool *withinP)
#else  /* __STDC__ */
static Element      InsertForm (doc, view, withinP)
Document            doc;
View                view;
ThotBool           *withinP;
#endif /* __STDC__ */
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
	   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
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
		  && !ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
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
		       TtaSetTextContent (el, TEXT(" "), TtaGetDefaultLanguage (), doc);
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
#ifdef __STDC__
void                CreateForm (Document doc, View view)
#else  /* __STDC__ */
void                CreateForm (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void         CreateInputElement (Document doc, View view, int elInput)
#else  /* __STDC__ */
static void         CreateInputElement (doc, view, elInput)
Document            doc;
View                view;
int                 elInput;
#endif /* __STDC__ */
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
	   /* if it's not a HTML_EL_BUTTON or a SELECT
	      select the following text element */
	   if (elInput != HTML_EL_BUTTON && elInput != HTML_EL_Option_Menu)
	   TtaSelectElement (doc, el);
	   }
	 }
     }
}

/*----------------------------------------------------------------------
  CreateFieldset
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateFieldset (Document document, View view)
#else  /* __STDC__ */
void                CreateFieldset (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_FIELDSET, document);
}

/*----------------------------------------------------------------------
  CreateToggle
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateToggle (Document doc, View view)
#else  /* __STDC__ */
void                CreateToggle (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_Checkbox_Input);
}

/*----------------------------------------------------------------------
  CreateRadio
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateRadio (Document doc, View view)
#else  /* __STDC__ */
void                CreateRadio (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_Radio_Input);
}

/*----------------------------------------------------------------------
  UpdateAttrSelected
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateAttrSelected (NotifyAttribute * event)
#else  /* __STDC__ */
void                UpdateAttrSelected (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   OnlyOneOptionSelected (event->element, event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrSelectedDeleted
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrSelectedDeleted (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrSelectedDeleted (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   Element	menu;

   menu = TtaGetParent (event->element);
   OnlyOneOptionSelected (menu, event->document, FALSE);
}

/*----------------------------------------------------------------------
  DeleteAttrSelected
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            DeleteAttrSelected (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            DeleteAttrSelected (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   return TRUE;			/* refuse to delete this attribute */
}

/*----------------------------------------------------------------------
  CreateOption
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateOption (Document doc, View view)
#else  /* __STDC__ */
void                CreateOption (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                CreateOptGroup (Document document, View view)
#else  /* __STDC__ */
void                CreateOptGroup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_OptGroup, document);
}

/*----------------------------------------------------------------------
  CreateTextInput
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTextInput (Document doc, View view)
#else  /* __STDC__ */
void                CreateTextInput (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_Text_Input);
}

/*----------------------------------------------------------------------
  CreatePasswordInput
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreatePasswordInput (Document doc, View view)
#else  /* __STDC__ */
void                CreatePasswordInput (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_Password_Input);
}

/*----------------------------------------------------------------------
  CreateTextArea
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTextArea (Document doc, View view)
#else  /* __STDC__ */
void                CreateTextArea (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   CreateInputElement (doc, view, HTML_EL_Text_Input);
}


/*----------------------------------------------------------------------
  CreateImageInput
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateImageInput (Document doc, View view)
#else  /* __STDC__ */
void                CreateImageInput (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  Element             el, input;
  STRING              value;
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
	      value = TtaAllocString (length);
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
	  /* if it's not a HTML_EL_BUTTON or a SELECT
	     select the following text element */
	  TtaSelectElement (doc, el);
	}
    }
}

/*----------------------------------------------------------------------
  CreateFileInput
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateFileInput (Document doc, View view)
#else  /* __STDC__ */
void                CreateFileInput (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_File_Input);
}

/*----------------------------------------------------------------------
  CreateHiddenInput
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHiddenInput (Document doc, View view)
#else  /* __STDC__ */
void                CreateHiddenInput (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_Hidden_Input);
}

/*----------------------------------------------------------------------
  CreateLabel
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateLabel (Document doc, View view)
#else  /* __STDC__ */
void                CreateLabel (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateHTMLelement (HTML_EL_LABEL, doc);
}

/*----------------------------------------------------------------------
  CreatePushButton
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreatePushButton (Document doc, View view)
#else  /* __STDC__ */
void                CreatePushButton (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_BUTTON);
}

/*----------------------------------------------------------------------
  CreateSubmit
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateSubmit (Document doc, View view)
#else  /* __STDC__ */
void                CreateSubmit (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  CreateInputElement (doc, view, HTML_EL_Submit_Input);
}

/*----------------------------------------------------------------------
  CreateReset
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateReset (Document doc, View view)
#else  /* __STDC__ */
void                CreateReset (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   CreateInputElement (doc, view, HTML_EL_Reset_Input);
}

/*----------------------------------------------------------------------
  CreateDivision
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDivision (Document document, View view)
#else  /* __STDC__ */
void                CreateDivision (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Division, document);
}

/*----------------------------------------------------------------------
  CreateNOSCRIPT
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateNOSCRIPT (Document document, View view)
#else  /* __STDC__ */
void                CreateNOSCRIPT (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_NOSCRIPT, document);
}

/*----------------------------------------------------------------------
  CreateObject
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateObject (Document document, View view)
#else  /* __STDC__ */
void                CreateObject (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             child, el;
   Attribute           attr;
   AttributeType       attrType;
   STRING              name1;
   int                 length;
   int                 firstchar, lastchar;

   if (HTMLelementAllowed (document))
     {
       TtaSetDisplayMode (document, SuspendDisplay);
       elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), document);
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
	       name1 = TtaAllocString (length + 1);
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
#ifdef __STDC__
void                CreateParameter (Document document, View view)
#else  /* __STDC__ */
void                CreateParameter (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_Parameter, document);
}

/*----------------------------------------------------------------------
  CreateIFrame
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateIFrame (Document document, View view)
#else  /* __STDC__ */
void                CreateIFrame (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateHTMLelement (HTML_EL_IFRAME, document);
}

/*----------------------------------------------------------------------
   CreateOrChangeLink
   If current selection is within an anchor, change that link, otherwise
   create a link.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateOrChangeLink (Document doc, View view)
#else  /* __STDC__ */
void                CreateOrChangeLink (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   Element             el;
   int                 firstSelectedChar, i;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

   UseLastTarget = FALSE;
   TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
   if (el != NULL)
     {
       /* Look if there is an enclosing anchor element */
       el = SearchAnchor (doc, el, TRUE, TRUE);
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
#ifdef __STDC__
void                DeleteAnchor (Document doc, View view)
#else  /* __STDC__ */
void                DeleteAnchor (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   Element             firstSelectedElement, lastSelectedElement, anchor,
                       child, next, previous;
   int                 firstSelectedChar, lastSelectedChar, i;
   ElementType         elType;
   DisplayMode         dispMode;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &firstSelectedElement,
				&firstSelectedChar, &lastSelectedChar);
   if (firstSelectedElement == NULL)
      /* no selection. Do nothing */
      return;
   elType = TtaGetElementType (firstSelectedElement);
   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) != 0)
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
	  i = TtaGetTextLength (firstSelectedElement);
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
#ifdef __STDC__
void FlushCache (Document doc, View view)
#else 
void FlushCache (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
  StopAllRequests (doc);
  libwww_CleanCache ();
}

/*----------------------------------------------------------------------
  ConfigColor
  A frontend to the Color configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigColor (Document doc, View view)
#else 
void ConfigColor (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
   ColorConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigGeometry
  A frontend to the Geometry configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigGeometry (Document doc, View view)
#else 
void ConfigGeometry (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
   GeometryConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigGeneral
  A frontend to the Browsing Editing configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigGeneral (Document doc, View view)
#else 
void ConfigGeneral (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
   GeneralConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigPublish
  A frontend to the Browsing Editing configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigPublish (Document doc, View view)
#else 
void ConfigPublish (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
   PublishConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigCache
  A frontend to the cache configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigCache (Document doc, View view)
#else 
void ConfigCache (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
  CacheConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigProxy
  A frontend to the proxy configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigProxy (Document doc, View view)
#else 
void ConfigProxy (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
  ProxyConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigLanNeg
  A frontend to the LanNeg configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigLanNeg (Document doc, View view)
#else 
void ConfigLanNeg (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
   LanNegConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigProfile
  A frontend to the profile configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigProfile (Document doc, View view)
#else
void ConfigProfile (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
  ProfileConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  ConfigTemplates
  A frontend to the Templates configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ConfigTemplates (Document doc, View view)
#else 
void ConfigTemplates (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
   TemplatesConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  SaveOptions
  Saves the user modified configuration options
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void SaveOptions (Document doc, View view)
#else 
void SaveOptions (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  AnnotateDocument
  Frontend to the function that creates an annotation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void AnnotateDocument (Document doc, View view)
#else 
void AnnotateDocument (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
#ifdef ANNOTATIONS
  ANNOT_Create (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  LoadAnnotations
  Frontend to the function that loads the annotations related to a document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void LoadAnnotations (Document doc, View view)
#else 
void LoadAnnotations (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
#ifdef ANNOTATIONS
  ANNOT_Load (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  PostAnnotation
  Frontend to the function that posts an annotation to the server
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void PostAnnotation (Document doc, View view)
#else 
void PostAnnotation (doc, view)
Document doc;
View view;
#endif /* __STDC__ */
{
#ifdef ANNOTATIONS
  ANNOT_Post (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
   Show/Hide the annotations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowAnnotations (Document document, View view)
#else
void                ShowAnnotations (document, view)
Document            doc;
View                view;
#endif
{
#ifdef ANNOTATIONS
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
   Show/Hide the annotations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CustomQuery (Document document, View view)
#else
void                CustomQuery (document, view)
Document            doc;
View                view;
#endif
{
#ifdef ANNOTATIONS
  CustomQueryMenuInit (document, view);
#endif /* ANNOTATIONS */
}
