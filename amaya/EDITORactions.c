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

#include "css_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLstyle_f.h"


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
   char                tempfile[MAX_LENGTH];
   char                suffix[MAX_LENGTH];
   int                 val, i, j;
   Document            doc;
   boolean             exist;
   ElementType         elType;
   Element             root, title, text, el, head, child, metas, meta;
   AttributeType       attrType;
   Attribute	       attr;

   strcpy (tempfile, DirectoryName);
   strcat (tempfile, DIR_STR);
   val = strlen (tempfile);
   i = 0;
   strcat (tempfile, "New");

   /* check if a previous new file is open in Amaya */
   exist = TRUE;
   while (exist)
     {
	exist = FALSE;
	j = 1;
	while (!exist && j < DocumentTableLength)
	  {
	     if (DocumentURLs[j] != NULL)
		exist = (strcmp (DocumentURLs[j], tempfile) == 0);
	     j++;
	  }
	if (!exist)
	   exist = TtaFileExist (tempfile);
	if (exist)
	  {
	     /* the file exists -> generate a new name */
	     i++;
	     sprintf (suffix, "New%d", i);
	     strcpy (&tempfile[val], suffix);
	  }
     }

   doc = GetHTMLDocument (tempfile, NULL, 0, DC_FALSE);
   ResetStop (doc);
   ApplyFinalStyle (doc);
   root = TtaGetMainRoot (doc);
   elType = TtaGetElementType (root);
   /* create a default title if there is no content in the TITLE element */
   elType.ElTypeNum = HTML_EL_TITLE;
   title = TtaSearchTypedElement (elType, SearchInTree, root);
   text = TtaGetFirstChild (title);
   if (TtaGetTextLength (text) == 0)
      TtaSetTextContent (text, "No title", TtaGetDefaultLanguage (), doc);
   /* create a META element in the HEAD with attributes name="GENERATOR" */
   /* and content="Amaya" */
   elType.ElTypeNum = HTML_EL_HEAD;
   head = TtaSearchTypedElement (elType, SearchInTree, root);
   child = TtaGetLastChild (head);
   elType.ElTypeNum = HTML_EL_Metas;
   metas = TtaNewElement (doc, elType);
   TtaInsertSibling (metas, child, FALSE, doc);
   elType.ElTypeNum = HTML_EL_META;
   meta = TtaNewElement (doc, elType);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_meta_name;
   attr = TtaNewAttribute (attrType);
   TtaAttachAttribute (meta, attr, doc);
   TtaSetAttributeText (attr, "GENERATOR", meta, doc);
   attrType.AttrTypeNum = HTML_ATTR_meta_content;
   attr = TtaNewAttribute (attrType);
   TtaAttachAttribute (meta, attr, doc);
   strcpy (tempfile, HTAppName);
   strcat (tempfile, " ");
   strcat (tempfile, HTAppVersion);
   TtaSetAttributeText (attr, tempfile, meta, doc);
   TtaInsertFirstChild (&meta, metas, doc);
   /* Search the first element in the BODY to set initial selection */
   elType.ElTypeNum = HTML_EL_Element;
   el = TtaSearchTypedElement (elType, SearchInTree, root);
   /* set the initial selection */
   TtaSelectElement (doc, el);
   if (SelectionDoc != 0)
       UpdateContextSensitiveMenus (SelectionDoc);
   SelectionDoc = doc;
   UpdateContextSensitiveMenus (doc);

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                EnSpellCheck (Document document, View view)
#else  /* __STDC__ */
void                EnSpellCheck (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             root, el;
   ElementType         elType;
   int                 firstchar, lastchar;

   /* enforce the English language for the whole document */
   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = HTML_ATTR_Langue;
   root = TtaGetMainRoot (document);
   attr = TtaGetAttribute (root, attrType);
   if (attr == NULL)
     {
	/* create the Language attribute */
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (root, attr, document);
     }
   /* get the current selection */
   TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
   TtaSetAttributeText (attr, "English", root, document);
   if (el == NULL)
     {
	/* no current selection in the document */
	/* select the first character in the body */
	elType = TtaGetElementType (root);
	elType.ElTypeNum = HTML_EL_BODY;
	el = TtaSearchTypedElement (elType, SearchInTree, root);
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	el = TtaSearchTypedElement (elType, SearchInTree, el);
	TtaSelectString (document, el, 1, 0);
     }
   TtcSpellCheck (document, view);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FrSpellCheck (Document document, View view)
#else  /* __STDC__ */
void                FrSpellCheck (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             root, el;
   ElementType         elType;
   int                 firstchar, lastchar;

   /* enforce the French language for the whole document */
   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = HTML_ATTR_Langue;
   root = TtaGetMainRoot (document);
   attr = TtaGetAttribute (root, attrType);
   if (attr == NULL)
     {
	/* create the Language attribute */
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (root, attr, document);
     }
   /* get the current selection */
   TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
   TtaSetAttributeText (attr, "Fran\347ais", root, document);
   if (el == NULL)
     {
	/* no current selection in the document */
	/* select the first character in the body */
	elType = TtaGetElementType (root);
	elType.ElTypeNum = HTML_EL_BODY;
	el = TtaSearchTypedElement (elType, SearchInTree, root);
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	el = TtaSearchTypedElement (elType, SearchInTree, el);
	TtaSelectString (document, el, 1, 0);
     }
   TtcSpellCheck (document, view);
}

/*----------------------------------------------------------------------
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

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_BR;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateIsIndex (Document document, View view)
#else  /* __STDC__ */
void                CreateIsIndex (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_ISINDEX;
   TtaCreateElement (elType, document);
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
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_LINK;
   TtaCreateElement (elType, document);
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
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_BASE;
   TtaCreateElement (elType, document);
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
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_META;
   TtaCreateElement (elType, document);
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
   elType.ElTypeNum = HTML_EL_Comment_;
   TtaInsertElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateParagraph (Document document, View view)
#else  /* __STDC__ */
void                CreateParagraph (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Paragraph;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading1 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading1 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_H1;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading2 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading2 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_H2;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading3 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading3 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_H3;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading4 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading4 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_H4;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading5 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading5 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_H5;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHeading6 (Document document, View view)
#else  /* __STDC__ */
void                CreateHeading6 (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_H6;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateList (Document document, View view)
#else  /* __STDC__ */
void                CreateList (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Unnumbered_List;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateNumberedList (Document document, View view)
#else  /* __STDC__ */
void                CreateNumberedList (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Numbered_List;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotCreateMenu (Document document, View view)
#else  /* __STDC__ */
void                ThotCreateMenu (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Menu;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotCreateDirectory (Document document, View view)
#else  /* __STDC__ */
void                ThotCreateDirectory (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Directory;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDefinitionList (Document document, View view)
#else  /* __STDC__ */
void                CreateDefinitionList (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Definition_List;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDefinitionTerm (Document document, View view)
#else  /* __STDC__ */
void                CreateDefinitionTerm (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Term;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDefinitionDef (Document document, View view)
#else  /* __STDC__ */
void                CreateDefinitionDef (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Definition;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateHorizontalRule (Document document, View view)
#else  /* __STDC__ */
void                CreateHorizontalRule (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Horizontal_Rule;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateBlockQuote (Document document, View view)
#else  /* __STDC__ */
void                CreateBlockQuote (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Block_Quote;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreatePreformatted (Document document, View view)
#else  /* __STDC__ */
void                CreatePreformatted (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Preformatted;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAddress (Document document, View view)
#else  /* __STDC__ */
void                CreateAddress (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Address;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
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

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Table;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
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

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_CAPTION;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTHead (Document document, View view)
#else  /* __STDC__ */
void                CreateTHead (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_thead;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTBody (Document document, View view)
#else  /* __STDC__ */
void                CreateTBody (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_tbody;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTFoot (Document document, View view)
#else  /* __STDC__ */
void                CreateTFoot (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_tfoot;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateRow (Document document, View view)
#else  /* __STDC__ */
void                CreateRow (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Table_row;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDataCell (Document document, View view)
#else  /* __STDC__ */
void                CreateDataCell (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Data_cell;
   TtaCreateElement (elType, document);
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
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Heading_cell;
   TtaCreateElement (elType, document);
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
	while (elType.ElTypeNum != HTML_EL_BODY && elType.ElTypeNum != HTML_EL_Form)
	  {
	     el = TtaGetParent (el);
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
   return el;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateForm (Document document, View view)
#else  /* __STDC__ */
void                CreateForm (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Form;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateToggle (Document document, View view)
#else  /* __STDC__ */
void                CreateToggle (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Toggle_Item;
   TtaInsertElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateRadio (Document document, View view)
#else  /* __STDC__ */
void                CreateRadio (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Radio_Item;
   TtaInsertElement (elType, document);
}


/*----------------------------------------------------------------------
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
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DeleteAttrSelected (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             DeleteAttrSelected (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   return True;			/* refuse to delete this attribute */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateOption (Document document, View view)
#else  /* __STDC__ */
void                CreateOption (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el;
   int                 firstchar, lastchar;

   el = GetEnclosingForm (document, view);
   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Option;
   TtaInsertElement (elType, document);
   TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
   OnlyOneOptionSelected (el, document, FALSE);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTextInput (Document document, View view)
#else  /* __STDC__ */
void                CreateTextInput (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Text_Input_Line;
   TtaInsertElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTextArea (Document document, View view)
#else  /* __STDC__ */
void                CreateTextArea (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Text_Area;
   TtaInsertElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateCommandLine (Document document, View view)
#else  /* __STDC__ */
void                CreateCommandLine (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Command_Line;
   TtaInsertElement (elType, document);
}


/*----------------------------------------------------------------------
   SetAreaCoords computes the coords attribute value from x, y,       
   width and height of the box.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetAreaCoords (Document document, Element element)
#else  /* __STDC__ */
void                SetAreaCoords (document, element)
Document            document;
Element             element;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             child;
   AttributeType       attrType;
   Attribute           attrCoords, attrX, attrY;
   Attribute           attrW, attrH, attrShape;
   char               *text, buffer[100];
   int                 x1, y1, x2, y2;
   int                 length, shape, i;

   /* Is it an AREA element */
   elType = TtaGetElementType (element);
   if (elType.ElTypeNum != HTML_EL_AREA)
      return;

   /* Search the coords attribute */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_coords;
   attrCoords = TtaGetAttribute (element, attrType);
   if (attrCoords == NULL)
      return;

   /* Search the shape attribute */
   attrType.AttrTypeNum = HTML_ATTR_shape;
   attrShape = TtaGetAttribute (element, attrType);
   if (attrShape == NULL)
      return;
   shape = TtaGetAttributeValue (attrShape);
   /* prepare the coords string */
   length = 200;
   text = TtaGetMemory (length);
   if (shape == HTML_ATTR_shape_VAL_rectangle || shape == HTML_ATTR_shape_VAL_circle)
     {
	/* Search the x_coord attribute */
	attrType.AttrTypeNum = HTML_ATTR_x_coord;
	attrX = TtaGetAttribute (element, attrType);
	if (attrX == NULL)
	   return;
	/* Search the y_coord attribute */
	attrType.AttrTypeNum = HTML_ATTR_y_coord;
	attrY = TtaGetAttribute (element, attrType);
	if (attrY == NULL)
	   return;
	/* Search the width attribute */
	attrType.AttrTypeNum = HTML_ATTR_width_;
	attrW = TtaGetAttribute (element, attrType);
	if (attrW == NULL)
	   return;
	/* Search the height attribute */
	attrType.AttrTypeNum = HTML_ATTR_height_;
	attrH = TtaGetAttribute (element, attrType);
	if (attrH == NULL)
	   return;

	x1 = TtaGetAttributeValue (attrX);
	y1 = TtaGetAttributeValue (attrY);
	x2 = TtaGetAttributeValue (attrW);
	y2 = TtaGetAttributeValue (attrH);
	if (shape == HTML_ATTR_shape_VAL_rectangle)
	   sprintf (text, "%d,%d,%d,%d", x1, y1, x1 + x2, y1 + y2);
	else
	  {
	     /* to make a circle, height and width have to be equal */
	     if (x2 < y2)
	       {
		  TtaSetAttributeValue (attrW, y2, element, document);
		  x2 = y2;
	       }
	     else if (x2 > y2)
		TtaSetAttributeValue (attrH, x2, element, document);
	     x2 = x2 / 2;
	     sprintf (text, "%d,%d,%d", x1 + x2, y1 + x2, x2);
	  }
     }
   else if (shape == HTML_ATTR_shape_VAL_polygon)
     {
	child = TtaGetFirstChild (element);
	length = TtaGetPolylineLength (child);
	/* keep points */
	i = 1;
	while (i <= length)
	  {
	     TtaGivePolylinePoint (child, i, UnPixel, &x1, &y1);
	     sprintf (buffer, "%d,%d", x1, y1);
	     strcat (text, buffer);
	     if (i < length)
	       strcat (text, ",");
	     i++;
	  }
     }
   TtaSetAttributeText (attrCoords, text, element, document);
   TtaFreeMemory (text);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateAreaMap (Document doc, View view, char *shape)
#else  /* __STDC__ */
static void         CreateAreaMap (document, view, shape)
Document            document;
View                view;
char               *shape;

#endif /* __STDC__ */
{
   Element             el, map, parent, image, child;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr, attrRef, attrShape;
   char               *text, url[MAX_LENGTH];
   int                 length, w, h;
   int                 firstchar, lastchar;

   /* ask Thot to stop displaying changes made in the document */
   TtaSetDisplayMode (doc, DeferredDisplay);

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
   if (el == NULL)
      /* no selection */
      return;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
	/* The selection is on a IMG */
	image = el;
	/* Search the USEMAP attribute */
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_USEMAP;
	attr = TtaGetAttribute (image, attrType);
	map = NULL;
	if (attr != NULL)
	  {
	     /* Search the MAP element associated with IMG element */
	     length = TtaGetTextAttributeLength (attr);
	     length++;
	     text = TtaGetMemory (length);
	     TtaGiveTextAttributeValue (attr, text, &length);
	     if (text[0] == '#')
		map = SearchNAMEattribute (doc, &text[1], NULL);
	     TtaFreeMemory (text);
	  }
	else
	  {
	     /* create the USEMAP attribute */
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (image, attr, doc);
	     /* create the MAP element */
	     elType.ElTypeNum = HTML_EL_MAP;
	     map = TtaNewElement (doc, elType);
	     parent = image;
	     do
	       {
		  el = parent;
		  parent = TtaGetParent (el);
		  elType = TtaGetElementType (parent);
	       }
	     while (elType.ElTypeNum != HTML_EL_BODY);
	     TtaInsertSibling (map, el, FALSE, doc);
	     CreateTargetAnchor (doc, map);
	     text = TtaGetElementLabel (map);
	     strcpy (url, "#");
	     strcat (url, text);
	     TtaSetAttributeText (attr, url, image, doc);
	     /* create the Ref_IMG attribute */
	     attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (map, attr, doc);
	     TtaSetAttributeReference (attr, map, doc, image, doc);
	  }
     }
   else
     {
	/* Is the selection within a MAP ? */
	if (elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT)
	  {
	     el = TtaGetParent (el);
	     map = TtaGetParent (el);
	  }
	else if (elType.ElTypeNum == HTML_EL_AREA)
	   map = TtaGetParent (el);
	else if (elType.ElTypeNum == HTML_EL_MAP)
	   map = el;
	else
	   /* cannot create the AREA */
	   return;

	/* Search the Ref_IMG attribute */
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
	attr = TtaGetAttribute (map, attrType);
	image = NULL;
	if (attr != NULL)
	  {
	     /* Search the IMAGE element associated with the MAP */
	     length = 200;
	     text = TtaGetMemory (length);
	     TtaGiveReferenceAttributeValue (attr, &image, text, &length);
	     TtaFreeMemory (text);
	  }
     }

   /* Create the AREA */
   if (map != NULL && image != NULL)
     {
	elType.ElTypeNum = HTML_EL_AREA;
	/* Is it necessary to ask user coordinates */
	if (shape[0] == 'R' || shape[0] == 'c')
	   TtaAskFirstCreation ();

	el = TtaNewTree (doc, elType, "");
	child = TtaGetLastChild (map);
	if (child == NULL)
	   TtaInsertFirstChild (&el, map, doc);
	else
	   TtaInsertSibling (el, child, FALSE, doc);
	child = TtaGetFirstChild (el);
	/* For polygons, sets the value after the Ref_IMG attribute is created */
	if (shape[0] != 'p')
	   TtaSetGraphicsShape (child, shape[0], doc);

	/* create the shape attribute */
	attrType.AttrTypeNum = HTML_ATTR_shape;
	attrShape = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attrShape, doc);

	/* Create the coords attribute */
	attrType.AttrTypeNum = HTML_ATTR_coords;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);

	if (shape[0] == 'R')
	   TtaSetAttributeValue (attrShape, HTML_ATTR_shape_VAL_rectangle, el, doc);
	else if (shape[0] == 'c')
	   TtaSetAttributeValue (attrShape, HTML_ATTR_shape_VAL_circle, el, doc);
	else if (shape[0] == 'p')
	  {
	     /* create the AreaRef_IMG attribute */
	     attrType.AttrTypeNum = HTML_ATTR_AreaRef_IMG;
	     attrRef = TtaNewAttribute (attrType);
	     TtaAttachAttribute (el, attrRef, doc);
	     TtaSetAttributeReference (attrRef, el, doc, image, doc);
	     TtaSetAttributeValue (attrShape, HTML_ATTR_shape_VAL_polygon, el, doc);
	     TtaGiveBoxSize (image, doc, 1, UnPixel, &w, &h);
	     /*TtaChangeLimitOfPolyline (child, UnPixel, w, h, doc);*/
	  }
	/* ask Thot to display changes made in the document */
	TtaSetDisplayMode (doc, DisplayImmediately);
	TtaSelectElement (doc, child);
	if (shape[0] == 'p')
	   TtcInsertGraph (doc, 1, 'p');
	/* Compute coords attribute */
	SetAreaCoords (doc, el);
	/* FrameUpdating creation of Area and selection of destination */
	SelectDestination (doc, el);
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAreaRect (Document doc, View view)
#else  /* __STDC__ */
void                CreateAreaRect (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAreaMap (doc, view, "R");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAreaCircle (Document doc, View view)
#else  /* __STDC__ */
void                CreateAreaCircle (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAreaMap (doc, view, "c");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAreaPoly (Document doc, View view)
#else  /* __STDC__ */
void                CreateAreaPoly (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAreaMap (doc, view, "p");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDivision (Document document, View view)
#else  /* __STDC__ */
void                CreateDivision (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Division;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateApplet (Document document, View view)
#else  /* __STDC__ */
void                CreateApplet (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Applet;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateParameter (Document document, View view)
#else  /* __STDC__ */
void                CreateParameter (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Parameter;
   TtaInsertElement (elType, document);
}

/*----------------------------------------------------------------------
   ChangeLink selects the new destination of the Link anchor.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeLink (Document doc, View view)
#else  /* __STDC__ */
void                ChangeLink (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   Element             el;
   int                 firstSelectedChar, i;

   TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
   /* Search the anchor element */
   el = SearchAnchor (doc, el, TRUE);
   /* Select a new destination */
   if (el != NULL)
      SelectDestination (doc, el);
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

   TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
   /* Search the anchor element */
   el = SearchAnchor (doc, el, TRUE);
   /* Select a new destination */
   if (el == NULL)
      CreateLink (doc, view);
   else
      SelectDestination (doc, el);
}

/*----------------------------------------------------------------------
   DeleteAnchor deletes the surrounding anchor.                    
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

   /* ask Thot to stop displaying changes made in the document */
   TtaSetDisplayMode (doc, DeferredDisplay);

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &firstSelectedElement,
				&firstSelectedChar, &lastSelectedChar);
   TtaGiveLastSelectedElement (doc, &lastSelectedElement, &i, &lastSelectedChar);
   TtaUnselect (doc);
   if (firstSelectedElement != NULL)
     {
	elType = TtaGetElementType (firstSelectedElement);
	if (elType.ElTypeNum == HTML_EL_Anchor)
	   /* the first selected element is an anchor */
	  {
	     anchor = firstSelectedElement;
	     /* the selected element will be deleted */
	     /* prepare the elements to be selected later */
	     firstSelectedElement = TtaGetFirstChild (anchor);
	     lastSelectedElement = TtaGetLastChild (anchor);
	     firstSelectedChar = 0;
	     lastSelectedChar = 0;
	  }
	else
	  {
	     /* search the surrounding Anchor element */
	     elType.ElTypeNum = HTML_EL_Anchor;
	     anchor = TtaGetTypedAncestor (firstSelectedElement, elType);
	  }
	if (anchor != NULL)
	  {
	     /* move all chidren of element anchor as sibling of that element */
	     child = TtaGetFirstChild (anchor);
	     previous = anchor;
	     while (child != NULL)
	       {
		  next = child;
		  TtaNextSibling (&next);
		  TtaRemoveTree (child, doc);
		  TtaInsertSibling (child, previous, FALSE, doc);
		  previous = child;
		  child = next;
	       }
	     TtaDeleteTree (anchor, doc);
	  }
     }
   /* ask Thot to display changes made in the document */
   TtaSetDisplayMode (doc, DisplayImmediately);
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
