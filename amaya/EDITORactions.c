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
   char*               tempfile = (char *) TtaGetMemory (MAX_LENGTH);
   char*               suffix = (char *) TtaGetMemory (MAX_LENGTH);
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
#ifdef _WINDOWS
   strcat (tempfile, "New.htm");
#else /* _WINDOWS */
   strcat (tempfile, "New.html");
#endif /* _WINDOWS */
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
#ifdef _WINDOWS
	     sprintf (suffix, "New%d.htm", i);
#else /* _WINDOWS */
	     sprintf (suffix, "New%d.html", i);
#endif /* _WINDOWS */
	     strcpy (&tempfile[val], suffix);
	  }
     }

   doc = GetHTMLDocument (tempfile, NULL, 0, 0, CE_FALSE, FALSE, NULL, NULL);
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
   TtaFreeMemory (tempfile);
   TtaFreeMemory (suffix);
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
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
  MoveWithinHead moves the insertion point according with the element type
  to be inserted.
  Return TRUE if it succeeds.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      MoveWithinHead (Document document, View view, int elementT)
#else  /* __STDC__ */
static boolean      MoveWithinHead (document, view, elementT)
Document            document;
View                view;
int                 elementT;
#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, child, root;
   SSchema             docSchema;
   int                 i, j;

   docSchema = TtaGetDocumentSSchema (document);
   if (strcmp(TtaGetSSchemaName (docSchema), "HTML") != 0)
     /* not within an HTML document */
     return (FALSE);
   else
     {
       elType.ElSSchema = docSchema;
       elType.ElTypeNum = elementT;
       root = TtaGetMainRoot (document);
       child = TtaSearchTypedElement (elType, SearchForward, root);
       
       /* give current position */
       TtaGiveFirstSelectedElement (document, &el, &i, &j);
       if (el != NULL)
	 {
	   elType = TtaGetElementType (el);
	   if (elType.ElSSchema == docSchema && elType.ElTypeNum == elementT)
	     /* it's OK */
	     return (TRUE);
	   else if (elType.ElSSchema == docSchema
		    && elType.ElTypeNum == HTML_EL_HEAD && child == NULL)
	     return (TRUE);
	   else
	     {
	       /* it's not the same element or the HEAD element */
	       el = TtaGetParent (el);
	       if (el != NULL)
		 {
		   elType = TtaGetElementType (el);
		   if (elType.ElSSchema == docSchema && elType.ElTypeNum == HTML_EL_HEAD && child == NULL)
		     /* A child of the HEAD element is selected */
		     return (TRUE);
		   else
		     el = NULL;
		 }
	     }
	 }
       
       if (el == NULL && child == NULL)
	 {
	   /* locate the HEAD element */
	   elType.ElSSchema = docSchema;
	   elType.ElTypeNum = HTML_EL_HEAD;
	   el = TtaSearchTypedElement (elType, SearchForward, root);
	 }

       if (el != NULL && child == NULL)
	 /* select the last child of the HEAD */
	 child = TtaGetLastChild (el);

       if (child != NULL)
	 TtaSelectElement (document, child);
       else
	 TtaSelectElement (document, el);
       return (TRUE);
     }
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

   if (MoveWithinHead (document, view, HTML_EL_ISINDEX))
     {
       elType.ElSSchema = TtaGetDocumentSSchema (document);
       elType.ElTypeNum = HTML_EL_ISINDEX;
       TtaInsertElement (elType, document);
     }
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
   Element             el;
   int                 firstSelectedChar, i;


   if (MoveWithinHead (document, view, HTML_EL_LINK))
     {
       elType.ElSSchema = TtaGetDocumentSSchema (document);
       elType.ElTypeNum = HTML_EL_LINK;
       TtaInsertElement (elType, document);
       /* Select a new destination */
       TtaGiveFirstSelectedElement (document, &el, &firstSelectedChar, &i);
       elType = TtaGetElementType (el);
       while (elType.ElTypeNum != HTML_EL_LINK)
	 {
	   el = TtaGetParent (el);
	   elType = TtaGetElementType (el);
	 }
       SelectDestination (document, el);
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
   ElementType         elType;

   if (MoveWithinHead (document, view, HTML_EL_BASE))
     {
       elType.ElSSchema = TtaGetDocumentSSchema (document);
       elType.ElTypeNum = HTML_EL_BASE;
       TtaInsertElement (elType, document);
     }
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

   if (MoveWithinHead (document, view, HTML_EL_META))
     {
       elType.ElSSchema = TtaGetDocumentSSchema (document);
       elType.ElTypeNum = HTML_EL_META;
       TtaInsertElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Comment_;
       TtaInsertElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Paragraph;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_H1;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_H2;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_H4;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_H5;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_H6;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Unnumbered_List;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Numbered_List;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Menu;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Directory;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Definition_List;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Term;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Definition;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Horizontal_Rule;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Block_Quote;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Preformatted;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Address;
       TtaCreateElement (elType, document);
     }
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
   Element             el;
   int                 firstSelectedChar, i;
   AttributeType       attrType;
   Attribute           attr;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Table;
       TtaCreateElement (elType, document);
       /* get the new Table */
       TtaGiveFirstSelectedElement (document, &el, &firstSelectedChar, &i);
       elType = TtaGetElementType (el);
       while (elType.ElTypeNum != HTML_EL_Table && el != NULL)
	 {
	   el = TtaGetParent (el);
	   if (el != NULL)
	      elType = TtaGetElementType (el);
	 }
       if (el != NULL)
	 {
         /* if the Table has no Border attribute, create one */
         attrType.AttrSSchema = elType.ElSSchema;
         attrType.AttrTypeNum = HTML_ATTR_Border;
         attr = TtaGetAttribute (el, attrType);
         if (attr == NULL)
	   {
	   /* create the Border attribute */
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, document);
	   TtaSetAttributeValue (attr, 1, el, document);
	   }
	 }
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_CAPTION;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_thead;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_tbody;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_tfoot;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Table_row;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Data_cell;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Heading_cell;
       TtaCreateElement (elType, document);
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
#ifdef __STDC__
static Element      InsertForm (Document doc, View view, boolean *withinP)
#else  /* __STDC__ */
static Element      InsertForm (doc, view, withinP)
Document            doc;
View                view;
boolean            *withinP;
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
       /* within HTML element */
       form = el;
       while (form != NULL && elType.ElTypeNum != HTML_EL_Form)
	 {
	   form = TtaGetParent (form);
	   if (form != NULL)
	     elType = TtaGetElementType (form);
	 }

       if (form == NULL)
	 {
	   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
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
	   while (parent != form && elType.ElTypeNum != HTML_EL_Paragraph
		  && elType.ElTypeNum != HTML_EL_Text_Input_Line
		  && elType.ElTypeNum != HTML_EL_Command_Line
		  && elType.ElTypeNum != HTML_EL_Toggle_Menu
		  && elType.ElTypeNum != HTML_EL_Radio_Menu
		  && !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
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
		       TtaSetTextContent (el, " ", TtaGetDefaultLanguage (), doc);
		       TtaSelectString (doc, el, 2, 2);
		     }
		   
		 }
	     }
	 }
     }
   return (el);
}


/*----------------------------------------------------------------------
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
  boolean           withinP;

  el = InsertForm (doc, view, &withinP);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateToggle (Document doc, View view)
#else  /* __STDC__ */
void                CreateToggle (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, input;
   boolean             withinP;

   /* create the form if necessary */
   el = InsertForm (doc, view, &withinP);
   if (el != NULL)
     {
       /* the element can be created */
       elType = TtaGetElementType (el);
       elType.ElTypeNum = HTML_EL_Checkbox_Input;
       TtaInsertElement (elType, doc);
       if (withinP)
	 {
	   /* Insert a text element after */
	   input = TtaSearchTypedElement (elType, SearchForward, el);
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   el = TtaNewElement (doc, elType);
	   TtaInsertSibling (el, input, FALSE, doc);
	   /* Select this new element */
	   TtaSelectElement (doc, el);
	 }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateRadio (Document doc, View view)
#else  /* __STDC__ */
void                CreateRadio (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, input;
   boolean             withinP;

   /* create the form if necessary */
   el = InsertForm (doc, view, &withinP);
   if (el != NULL)
     {
       /* the element can be created */
       elType = TtaGetElementType (el);
       elType.ElTypeNum = HTML_EL_Radio_Input;
       TtaInsertElement (elType, doc);
       if (withinP)
	 {
	   /* Insert a text element after */
	   input = TtaSearchTypedElement (elType, SearchForward, el);
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   el = TtaNewElement (doc, elType);
	   TtaInsertSibling (el, input, FALSE, doc);
	   /* Select this new element */
	   TtaSelectElement (doc, el);
	 }
     }
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
   return TRUE;			/* refuse to delete this attribute */
}


/*----------------------------------------------------------------------
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
   Element             el;
   int                 firstchar, lastchar;
   boolean             withinP;

   /* create the form if necessary */
   el = InsertForm (doc, view, &withinP);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	 {
	   el = TtaGetParent (el);
	   elType = TtaGetElementType (el);
	 }
       /*el = GetEnclosingForm (doc, view);*/
       if (elType.ElTypeNum != HTML_EL_Option)
	 {
	   elType.ElTypeNum = HTML_EL_Option_Menu;
	   TtaInsertElement (elType, doc);
 	 }
       elType.ElTypeNum = HTML_EL_Option;
       TtaInsertElement (elType, doc);
       TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
       OnlyOneOptionSelected (el, doc, FALSE);
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTextInput (Document doc, View view)
#else  /* __STDC__ */
void                CreateTextInput (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, input;
   boolean             withinP;

   /* create the form if necessary */
   el = InsertForm (doc, view, &withinP);
   if (el != NULL)
     {
       /* the element can be created */
       elType = TtaGetElementType (el);
       if (withinP)
	 {
	   /* create only the paragraph */
	   elType.ElTypeNum = HTML_EL_Text_Input;
	   TtaInsertElement (elType, doc);
	   /* Insert a text element after */
	   input = TtaSearchTypedElement (elType, SearchForward, el);
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   el = TtaNewElement (doc, elType);
	   TtaInsertSibling (el, input, FALSE, doc);
	 }
       else
	 {
	   /* create the text and the paragraph */
	   elType.ElTypeNum = HTML_EL_Text_Input_Line;
	   TtaInsertElement (elType, doc);
	 }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTextArea (Document doc, View view)
#else  /* __STDC__ */
void                CreateTextArea (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el, input;
   boolean             withinP;

   /* create the form if necessary */
   el = InsertForm (doc, view, &withinP);
   if (el != NULL)
     {
       /* the element can be created */
       elType = TtaGetElementType (el);
       elType.ElTypeNum = HTML_EL_Text_Area;
       TtaInsertElement (elType, doc);
       if (withinP)
	 {
	   /* Insert a text element after */
	   input = TtaSearchTypedElement (elType, SearchForward, el);
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   el = TtaNewElement (doc, elType);
	   TtaInsertSibling (el, input, FALSE, doc);
	 }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateCommandLine (Document doc, View view)
#else  /* __STDC__ */
void                CreateCommandLine (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el;
   boolean             withinP;

   /* create the form if necessary */
   el = InsertForm (doc, view, &withinP);
   if (el != NULL)
     {
       /* the element can be created */
       elType = TtaGetElementType (el);
       elType.ElTypeNum = HTML_EL_Command_Line;
       TtaInsertElement (elType, doc);
     }
}


/*----------------------------------------------------------------------
   SetAreaCoords computes the coords attribute value from x, y,       
   width and height of the box.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetAreaCoords (Document document, Element element, int attrNum)
#else  /* __STDC__ */
void                SetAreaCoords (document, element, attrNum)
Document            document;
Element             element;
int                 attrNum;
#endif /* __STDC__ */
{
   ElementType         elType;
   Element             child, map;
   AttributeType       attrType;
   Attribute           attrCoords, attrX, attrY;
   Attribute           attrW, attrH, attrShape;
   char               *text, *buffer;
   int                 x1, y1, x2, y2;
   int                 w, h;
   int                 length, shape, i;

   /* Is it an AREA element */
   elType = TtaGetElementType (element);
   if (elType.ElTypeNum != HTML_EL_AREA)
      return;
   /* get size of the map */
   map = TtaGetParent (element);
   TtaGiveBoxSize (map, document, 1, UnPixel, &w, &h);
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
   length = 2000;
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
	attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
	attrW = TtaGetAttribute (element, attrType);
	if (attrW == NULL)
	   return;
	/* Search the height attribute */
	attrType.AttrTypeNum = HTML_ATTR_height_;
	attrH = TtaGetAttribute (element, attrType);
	if (attrH == NULL)
	   return;

	x1 = TtaGetAttributeValue (attrX);
	if (x1 < 0)
	  {
	    /* out of left side */
	    x1 = 0;
	    TtaSetAttributeValue (attrX, x1, element, document);	    
	  }
	y1 = TtaGetAttributeValue (attrY);
	if (y1 < 0)
	  {
	    /* out of top side */
	    y1 = 0;
	    TtaSetAttributeValue (attrY, y1, element, document);	    
	  }
	x2 = TtaGetAttributeValue (attrW);
	if (x1 + x2 > w)
	  {
	    /* out of right side */
	    x2 = w - x1;
	    TtaSetAttributeValue (attrW, x2, element, document);	    
	  }
	y2 = TtaGetAttributeValue (attrH);
	if (y1 + y2 > h)
	  {
	    /* out of bottom side */
	    y2 = h - y1;
	    TtaSetAttributeValue (attrH, y2, element, document);	    
	  }
	if (shape == HTML_ATTR_shape_VAL_rectangle)
	   sprintf (text, "%d,%d,%d,%d", x1, y1, x1 + x2, y1 + y2);
	else
	  {
	     /* to make a circle, height and width have to be equal */
	     if ((attrNum == 0 && x2 > y2) ||
		 attrNum == HTML_ATTR_height_)
	       {
		 /* we need to update the width */
		 w = y2;
		 h = w / 2;
		 TtaSetAttributeValue (attrW, w, element, document);
	       }
	     else if ((attrNum == 0 && x2 < y2) ||
		      attrNum == HTML_ATTR_IntWidthPxl)
	       {
		 /* we need to update the height */
		 w = x2;
		 h = w / 2;
		 TtaSetAttributeValue (attrH, w, element, document);
	       }
	     else
	       if (x2 > y2)
		 h = y2 / 2;
	       else
		 h = x2 / 2;
	     sprintf (text, "%d,%d,%d", x1 + h, y1 + h, h);
	  }
     }
   else if (shape == HTML_ATTR_shape_VAL_polygon)
     {
	child = TtaGetFirstChild (element);
	length = TtaGetPolylineLength (child);
	/* keep points */
	i = 1;
	buffer = (char *) TtaGetMemory (100);
	text[0] = EOS;
	while (i <= length)
	  {
	     TtaGivePolylinePoint (child, i, UnPixel, &x1, &y1);
	     sprintf (buffer, "%d,%d", x1, y1);
	     strcat (text, buffer);
	     if (i < length)
	       strcat (text, ",");
	     i++;
	  }
	TtaFreeMemory (buffer);
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
   char               *url;
   int                 length, w, h;
   int                 firstchar, lastchar;
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (doc);
   url = NULL;
   /* ask Thot to stop displaying changes made in the document */
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
   if (el == NULL)
     /* no selection */
     return;

   elType = TtaGetElementType (el);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
     /* not within HTML element */
     return;
   else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
        url = (char*) TtaGetMemory (MAX_LENGTH);
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
	     length = TtaGetTextAttributeLength (attr) + 1;
	     TtaGiveTextAttributeValue (attr, url, &length);
	     if (url[0] == '#')
		map = SearchNAMEattribute (doc, &url[1], NULL);
	  }
	if (map == NULL)
	  {
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
	     attrType.AttrTypeNum = HTML_ATTR_NAME;
	     attr = TtaGetAttribute (map, attrType);

	     /* create the USEMAP attribute */
	     length = TtaGetTextAttributeLength (attr) + 1;
	     url[0] = '#';
	     TtaGiveTextAttributeValue (attr, &url[1], &length);
	     attrType.AttrTypeNum = HTML_ATTR_USEMAP;
	     attr = TtaGetAttribute (image, attrType);
	     if (attr == NULL)
	       attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (image, attr, doc);
	     TtaSetAttributeText (attr, url, image, doc);

	     /* create the Ref_IMG attribute */
	     attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (map, attr, doc);
	     TtaSetAttributeReference (attr, map, doc, image, doc);
	  }
	TtaFreeMemory (url);
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
	     length = MAX_LENGTH;
	     url = (char*) TtaGetMemory (MAX_LENGTH);
	     TtaGiveReferenceAttributeValue (attr, &image, url, &length);
	     TtaFreeMemory (url);
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
	TtaSetDisplayMode (doc, dispMode);
	TtaSelectElement (doc, child);
	if (shape[0] == 'p')
	   TtcInsertGraph (doc, 1, 'p');
	/* Compute coords attribute */
	SetAreaCoords (doc, el, 0);
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Division;
       TtaCreateElement (elType, document);
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateObject (Document document, View view)
#else  /* __STDC__ */
void                CreateObject (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifdef COUGAR
   ElementType         elType;
   Element             child, el;
   Attribute           attr;
   AttributeType       attrType;
   char               *name1;
   int                 length;
   int                 firstchar, lastchar;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
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
     }
#endif /* COUGAR */
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Applet;
       TtaCreateElement (elType, document);
     }
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
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
     {
       elType.ElTypeNum = HTML_EL_Parameter;
       TtaInsertElement (elType, document);
     }
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
   if (el != NULL)
     {
       /* Search the anchor element */
       el = SearchAnchor (doc, el, TRUE);
       /* Select a new destination */
       if (el == NULL)
	 CreateLink (doc, view);
       else
	 SelectDestination (doc, el);
     }
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
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (doc);
   /* ask Thot to stop displaying changes made in the document */
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &firstSelectedElement,
				&firstSelectedChar, &lastSelectedChar);
   if (firstSelectedElement != NULL)
     {
       TtaGiveLastSelectedElement (doc, &lastSelectedElement, &i, &lastSelectedChar);
       TtaUnselect (doc);
	elType = TtaGetElementType (firstSelectedElement);
	if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
	  return;
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
	     TtaSetDocumentModified (doc);
	  }

	/* ask Thot to display changes made in the document */
	TtaSetDisplayMode (doc, dispMode);
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
}
