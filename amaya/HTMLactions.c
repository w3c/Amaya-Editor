/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya browser functions called form Thot and declared in HTML.A.
 * These functions concern links and other HTML general features.
 *
 * Authors: V. Quint, I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"


#include "css_f.h"
#include "init_f.h"
#ifndef AMAYA_JAVA
#include "query_f.h"
#endif
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLactions_f.h"
#include "HTMLstyle_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLimage_f.h"
#include "trans_f.h"
#include "selection.h"

/*----------------------------------------------------------------------
   SetFontOrPhraseOnElement                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetFontOrPhraseOnElement (Document document, Element elem,
					      int eltype, boolean remove)
#else  /* __STDC__ */
static void         SetFontOrPhraseOnElement (document, elem, eltype, remove)
Document            document;
Element             elem;
int                 eltype;
boolean             remove;

#endif /* __STDC__ */
{
   Element             child, next;
   ElementType         elType;

   child = TtaGetFirstChild (elem);
   if (child == NULL)
     /* empty element. Create a text element in it */
     {
     elType = TtaGetElementType (elem);
     elType.ElTypeNum = HTML_EL_TEXT_UNIT;
     if (TtaCanInsertFirstChild(elType, elem, document))
	{
	child = TtaNewElement(document, elType);
	TtaInsertFirstChild (&child, elem, document);
	}
     }
   while (child != NULL)
     {
	next = child;
	TtaNextSibling (&next);
	elType = TtaGetElementType (child);
	if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	   if (remove)
	      ResetFontOrPhraseOnText (document, child, eltype);
	   else
	      SetFontOrPhraseOnText (document, &child, eltype);
	else if (!TtaIsLeaf (elType))
	   SetFontOrPhraseOnElement (document, child, eltype, remove);
	child = next;
     }
}

/*----------------------------------------------------------------------
   DoubleClick     The user has double-clicked an element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DoubleClick (NotifyElement * event)
#else  /* __STDC__ */
boolean             DoubleClick (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           HrefAttr, PseudoAttr, attr;
   Element             anchor, elFound;
   ElementType         elType;
   int                 length;
   char               *url, *info;
   char                documentURL[MAX_LENGTH];
   Document            targetDocument;
   View                view;

   /* Check if the current element is a text, an image or a graphics */
   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum != HTML_EL_LINK &&
       elType.ElTypeNum != HTML_EL_C_Empty &&
       elType.ElTypeNum != HTML_EL_PICTURE_UNIT &&
       elType.ElTypeNum != HTML_EL_TEXT_UNIT &&
       elType.ElTypeNum != HTML_EL_GRAPHICS_UNIT &&
       elType.ElTypeNum != HTML_EL_SYMBOL_UNIT &&
       elType.ElTypeNum != HTML_EL_Radio_Input &&
       elType.ElTypeNum != HTML_EL_Checkbox_Input &&
       elType.ElTypeNum != HTML_EL_Frame &&
       elType.ElTypeNum != HTML_EL_Option_Menu &&
       elType.ElTypeNum != HTML_EL_Submit_Input &&
       elType.ElTypeNum != HTML_EL_Reset_Input)
      /* DoubleClick is disabled on other elements */
      return FALSE;

   if (W3Loading)
      /* suspend current loading */
      StopTransfer (W3Loading, 1);

   if (elType.ElTypeNum == HTML_EL_Frame
       || elType.ElTypeNum == HTML_EL_Submit_Input
       || elType.ElTypeNum == HTML_EL_Reset_Input)
     {
	if (elType.ElTypeNum == HTML_EL_Frame)
	   elType = TtaGetElementType (TtaGetParent (event->element));
	if (elType.ElTypeNum == HTML_EL_Submit_Input
	    || elType.ElTypeNum == HTML_EL_Reset_Input)
	   /* it is a double click on submit element */
	   SubmitForm (event->document, event->element);
	return TRUE;
     }
   else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* it is a double click on graphic submit element? */
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_NAME;
       attr = TtaGetAttribute (event->element, attrType);
       if (attr)
	 {
	   /* it's a graphic submit element */
	   SubmitForm (event->document, event->element);
	   return TRUE;
	 }
     }
   else if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     {
	/* is it an option menu ? */
	elFound = TtaGetParent (event->element);
	elType = TtaGetElementType (elFound);
	if (elType.ElTypeNum == HTML_EL_Option)
	  {
	     SelectOneOption (event->document, elFound);
	     return TRUE;
	  }
     }
   else if (elType.ElTypeNum == HTML_EL_Option_Menu)
     {
	/* it is an option menu */
	elFound = TtaGetFirstChild (event->element);
	elType = TtaGetElementType (elFound);
	if (elType.ElTypeNum == HTML_EL_Option)
	  {
	     SelectOneOption (event->document, elFound);
	     return TRUE;
	  }
     }
   else if (elType.ElTypeNum == HTML_EL_Checkbox_Input)
     {
	SelectCheckbox (event->document, event->element);
	return TRUE;
     }
   else if (elType.ElTypeNum == HTML_EL_Radio_Input)
     {
	SelectOneRadio (event->document, event->element);
	return TRUE;
     }

   /* Search the anchor or LINK element */
   anchor = SearchAnchor (event->document, event->element, TRUE);
   if (anchor == NULL)
      if (elType.ElTypeNum == HTML_EL_LINK)
	 anchor = event->element;
      else
	{
	   elType.ElTypeNum = HTML_EL_LINK;
	   anchor = TtaGetTypedAncestor (event->element, elType);
	}

   if (anchor != NULL)
     {
	/* search HREF attribute */
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_HREF_;
	HrefAttr = TtaGetAttribute (anchor, attrType);
     }
   else
      HrefAttr = NULL;

   if (HrefAttr != NULL)
     {
	/* get a buffer for the URL */
	length = TtaGetTextAttributeLength (HrefAttr);
	length++;
	url = TtaGetMemory (length);
	targetDocument = 0;
	PseudoAttr = NULL;
	if (url != NULL)
	  {
	     elType = TtaGetElementType (anchor);
	     if (elType.ElTypeNum == HTML_EL_Anchor)
	       {
		  /* attach an attribute PseudoClass = active */
		  attrType.AttrSSchema = elType.ElSSchema;
		  attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
		  PseudoAttr = TtaGetAttribute (anchor, attrType);
		  if (PseudoAttr == NULL)
		    {
		       PseudoAttr = TtaNewAttribute (attrType);
		       TtaAttachAttribute (anchor, PseudoAttr, event->document);
		    }
		  TtaSetAttributeText (PseudoAttr, "active", anchor, event->document);
	       }
	     /* get the URL itself */
	     TtaGiveTextAttributeValue (HrefAttr, url, &length);

	     /* suppress white spaces at the end */
	     length--;
	     while (url[length] == ' ')
		url[length--] = EOS;
	     if (url[0] == '#')
	       {
		  /* the target element is part of the same document */
		  targetDocument = event->document;
	       }
	     else
	       {
		  /* the target element is in another document */
		  strcpy (documentURL, url);
		  url[0] = EOS;
		  /* is the source element an image map */
		  elType = TtaGetElementType (event->element);
		  attrType.AttrSSchema = elType.ElSSchema;
		  attrType.AttrTypeNum = HTML_ATTR_ISMAP;
		  attr = TtaGetAttribute (event->element, attrType);
		  if (attr != NULL) {
		    info = GetActiveImageInfo (event->document, event->element);
		    if (info != NULL)
		      {
			strcat (documentURL, info);
			TtaFreeMemory (info);
		      }
		  }

		  /* get the referred document */
		  targetDocument = GetHTMLDocument (documentURL, NULL,
				   event->document, event->document, DC_TRUE);
		  /* if the referred document has replaced the clicked
		     document, pseudo attribute "visited" should not be set */
		  if (targetDocument == event->document)
		     PseudoAttr = NULL;
	       }

	     TtaSetSelectionMode (TRUE);
	     if (PseudoAttr != NULL)
	       TtaSetAttributeText (PseudoAttr, "visited", anchor, event->document);
	     if (url[0] == '#' && targetDocument != 0)
	       {
		  /* attribute HREF contains the NAME of a target anchor */
		  elFound = SearchNAMEattribute (targetDocument, &url[1], NULL);
		  if (elFound != NULL)
		    {
		       /* show the target element in all views */
		       for (view = 1; view < 4; view++)
			  if (TtaIsViewOpened (targetDocument, view))
			     TtaShowElement (targetDocument, view, elFound, 10);
		    }
	       }
	     if (targetDocument > 0)
	       TtaRaiseView (targetDocument, 1);
	     TtaFreeMemory (url);
	     return TRUE;
	  }
     }
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   FreeDocumentResource                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDocumentResource (Document doc)
#else  /* __STDC__ */
void                FreeDocumentResource (doc)
Document       doc;

#endif /* __STDC__ */
{
   int                 i;
   char                tempdocument[MAX_LENGTH];
   char                documentname[MAX_LENGTH];

   if (doc == 0)
      return;
   if (DocumentURLs[doc] != NULL)
     {
	if (IsHTTPPath (DocumentURLs[doc]))
	  {
	     /* remove the temporary document file */
	     TtaExtractName (DocumentURLs[doc], tempdocument, documentname);
	     sprintf (tempdocument, "%s%s%d%s", TempFileDirectory, DIR_STR, doc, DIR_STR);
	     if (documentname[0] == '\0')
		strcat (tempdocument, "noname.html");
	     else
		strcat (tempdocument, documentname);
	     TtaFileUnlink (tempdocument);
	  }
	TtaFreeMemory (DocumentURLs[doc]);
	DocumentURLs[doc] = NULL;
	CleanDocumentCSS (doc);
	RemoveDocumentImages (doc);
     }

   if (!W3Loading)
     {
	/* is it the last loaded document ? */
	i = 1;
	while (i < DocumentTableLength && DocumentURLs[i] == NULL)
	   i++;

	if (i == DocumentTableLength)
	  {
	     /* now exit the application */
#ifdef AMAYA_JAVA
             CloseJava ();
#else /* !AMAYA_JAVA */
	     QueryClose ();
#endif /* !AMAYA_JAVA */
	     CloseCSS ();
	     TtaQuit ();
	  }
     }
}

/*----------------------------------------------------------------------
   DocumentClosed                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DocumentClosed (NotifyDialog * event)
#else  /* __STDC__ */
void                DocumentClosed (event)
NotifyDialog       *event;

#endif /* __STDC__ */
{
   if (event == NULL)
      return;
   FreeDocumentResource (event->document);
}

/*----------------------------------------------------------------------
   A new element has been selected. Update menus accordingly.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateContextSensitiveMenus (Document doc)
#else  /* __STDC__ */
void                UpdateContextSensitiveMenus (doc)
Document            doc
#endif				/* __STDC__ */
{
   ElementType         elType, elTypeSel;
   Element             firstSel;
   int                 firstChar, lastChar;
   boolean             NewSelInElem;

   TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
   /* 
    * elements PICTURE, Big_text, Small_text, Subscript, Superscript, Font_ 
    * are not permitted in a Preformatted element. The corresponding
    * entries in the menus must be turned off 
    */
   if (firstSel == NULL)
     NewSelInElem = FALSE;
   else
     {
	elType.ElSSchema = TtaGetDocumentSSchema (doc);
	elType.ElTypeNum = HTML_EL_Preformatted;
	NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if                  (NewSelInElem != SelectionInPRE)
     {
	SelectionInPRE = NewSelInElem;
	if (NewSelInElem)
	  {
	     TtaSetActionOff (doc, 1, Types, BImage);
	     TtaSetActionOff (doc, 1, Style, TBig);
	     TtaSetActionOff (doc, 1, Style, TSmall);
	     TtaSetActionOff (doc, 1, Style, BSub);
	     TtaSetActionOff (doc, 1, Style, BSup);
	     TtaSetActionOff (doc, 1, Style, BFont);
	  }
	else
	  {
	     TtaSetActionOn (doc, 1, Types, BImage);
	     TtaSetActionOn (doc, 1, Style, TBig);
	     TtaSetActionOn (doc, 1, Style, TSmall);
	     TtaSetActionOn (doc, 1, Style, BSub);
	     TtaSetActionOn (doc, 1, Style, BSup);
	     TtaSetActionOn (doc, 1, Style, BFont);
	  }
     }
   /* 
    * Disable the "Comment" entry of menu "Context" if current selection
    * is within a comment 
    */
   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Comment_;
	NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (NewSelInElem != SelectionInComment)
     {
	SelectionInComment = NewSelInElem;
	if (NewSelInElem)
	   TtaSetActionOff (doc, 2, StructTypes, BComment);
	else
	   TtaSetActionOn (doc, 2, StructTypes, BComment);
     }
   /* update toggle buttons in menus "Information Type" and */
   /* "Character Element" */
   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Emphasis;
	elTypeSel = TtaGetElementType (firstSel);
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInEM != NewSelInElem)
     {
	SelectionInEM = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TEmphasis, NewSelInElem);
	TtaSwitchButton (doc, 1, IButton);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Strong;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSTRONG != NewSelInElem)
     {
	SelectionInSTRONG = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TStrong, NewSelInElem);
	TtaSwitchButton (doc, 1, BButton);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Cite;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInCITE != NewSelInElem)
     {
	SelectionInCITE = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TCite, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Def;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInDFN != NewSelInElem)
     {
	SelectionInDFN = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TDefinition, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Code;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInCODE != NewSelInElem)
     {
	SelectionInCODE = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TCode, NewSelInElem);
	TtaSwitchButton (doc, 1, TTButton);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Variable;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInVAR != NewSelInElem)
     {
	SelectionInVAR = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TVariable, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Sample;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSAMP != NewSelInElem)
     {
	SelectionInSAMP = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TSample, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Keyboard;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInKBD != NewSelInElem)
     {
	SelectionInKBD = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TKeyboard, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Italic_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInI != NewSelInElem)
     {
	SelectionInI = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TItalic, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Bold_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInB != NewSelInElem)
     {
	SelectionInB = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TBold, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Teletype_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInTT != NewSelInElem)
     {
	SelectionInTT = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TTeletype, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Underlined_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInU != NewSelInElem)
     {
	SelectionInU = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TUnderline, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Struck_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSTRIKE != NewSelInElem)
     {
	SelectionInSTRIKE = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TStrikeOut, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Big_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInBIG != NewSelInElem)
     {
	SelectionInBIG = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TBig, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Small_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSMALL != NewSelInElem)
     {
	SelectionInSMALL = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TSmall, NewSelInElem);
     }
}

/*----------------------------------------------------------------------
   A new element has been selected. Update menus accordingly.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectionChanged (NotifyElement * event)
#else  /* __STDC__ */
void                SelectionChanged (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   if (event->document != SelectionDoc)
     {
	if (SelectionDoc != 0)
	   /* Reset buttons state in previous selected document */
	   UpdateContextSensitiveMenus (SelectionDoc);
	/* change the new selected document */
	SelectionDoc = event->document;
     }
   UpdateContextSensitiveMenus (event->document);
   TtaSelectView (SelectionDoc, 1);
}

/*----------------------------------------------------------------------
   A element type conversion has not been achieved by the editor.
   Trying with language-driven restructuration.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean            ElemToTransform (NotifyOnElementType * event)
#else  /* __STDC__ */
void               ElemToTransform (event)
NotifyOnElementType * event;
#endif  /* __STDC__ */
{
   return (!TransformIntoType(event->targetElementType, event->document));
}

/*----------------------------------------------------------------------
   CreateAnchor creates a link or target element.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAnchor (Document doc, View view, boolean createLink)
#else  /* __STDC__ */
void                CreateAnchor (document, view, createLink)
Document            document;
View                view;
boolean             createLink;

#endif /* __STDC__ */
{
   Element             first, last, el, next;
   Element             parag, prev, child, anchor;
   int                 c1, cN, lg, i;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   boolean             error;
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (doc);
   /* get the first and last selected element */
   TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
   TtaGiveLastSelectedElement (doc, &last, &i, &cN);

   /* Check whether the selected elements are a valid content for an anchor */
   elType = TtaGetElementType (first);
   if (elType.ElTypeNum == HTML_EL_Anchor && first == last)
      /* add an attribute on the current anchor */
      anchor = first;
   else
     {
	/* search if the selection is included into an anchor */
	el = SearchAnchor (doc, first, !createLink);
	if (el != NULL)
	   /* add an attribute on this anchor */
	   anchor = el;
	else
	  {
	     el = first;
	     error = FALSE;

	     while (!error && el != NULL)
	       {
		  elType = TtaGetElementType (el);
		  if (elType.ElTypeNum != HTML_EL_TEXT_UNIT &&
		      elType.ElTypeNum != HTML_EL_PICTURE_UNIT &&
		      elType.ElTypeNum != HTML_EL_Teletype_text &&
		      elType.ElTypeNum != HTML_EL_Italic_text &&
		      elType.ElTypeNum != HTML_EL_Bold_text &&
		      elType.ElTypeNum != HTML_EL_Underlined_text &&
		      elType.ElTypeNum != HTML_EL_Struck_text &&
		      elType.ElTypeNum != HTML_EL_Small_text &&
		      elType.ElTypeNum != HTML_EL_Big_text &&
		      elType.ElTypeNum != HTML_EL_Subscript &&
		      elType.ElTypeNum != HTML_EL_Superscript &&
		      elType.ElTypeNum != HTML_EL_Emphasis &&
		      elType.ElTypeNum != HTML_EL_Strong &&
		      elType.ElTypeNum != HTML_EL_Def &&
		      elType.ElTypeNum != HTML_EL_Code &&
		      elType.ElTypeNum != HTML_EL_Sample &&
		      elType.ElTypeNum != HTML_EL_Keyboard &&
		      elType.ElTypeNum != HTML_EL_Variable &&
		      elType.ElTypeNum != HTML_EL_Cite &&
		      elType.ElTypeNum != HTML_EL_Applet &&
		      elType.ElTypeNum != HTML_EL_Font_ &&
		      elType.ElTypeNum != HTML_EL_SCRIPT &&
		      elType.ElTypeNum != HTML_EL_MAP)
		     error = TRUE;
		  if (el == last)
		     el = NULL;
		  else
		     TtaGiveNextSelectedElement (doc, &el, &i, &i);
	       }

	     if (error)
	       {
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_ANCHOR1), NULL);
		  return;
	       }
	     /* check if the anchor to be created is within an anchor element */
	     else if (SearchAnchor (doc, first, TRUE) != NULL ||
		      SearchAnchor (doc, last, TRUE) != NULL)
	       {
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_ANCHOR2), NULL);
		  return;
	       }
	     /* ask Thot to stop displaying changes that will be made in the document */
	     if (dispMode == DisplayImmediately)
	       TtaSetDisplayMode (doc, DeferredDisplay);

	     /* process the first selected element */
	     elType = TtaGetElementType (first);
	     if (c1 > 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	       {
		  /* split the first selected text element */
		  el = first;
		  TtaSplitText (first, c1 - 1, doc);
		  TtaNextSibling (&first);
		  if (last == el)
		    {
		       /* we have to change last selection because the element was split */
		       last = first;
		       cN = cN - c1 + 1;
		    }
	       }
	     /* process the last selected element */
	     elType = TtaGetElementType (last);
	     if (cN > 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	       {
		  lg = TtaGetTextLength (last);
		  if (cN < lg)
		     /* split the last text */
		     TtaSplitText (last, cN, doc);
	       }
	     /* Create the corresponding anchor */
	     elType.ElTypeNum = HTML_EL_Anchor;
	     anchor = TtaNewElement (doc, elType);
	     if (createLink)
	       {
		/* create an attributeHREF for the new anchor */
		attrType.AttrSSchema = elType.ElSSchema;
		attrType.AttrTypeNum = HTML_ATTR_HREF_;
		attr = TtaGetAttribute (anchor, attrType);
		if (attr == NULL)
		  {
		     attr = TtaNewAttribute (attrType);
		     TtaAttachAttribute (anchor, attr, doc);
		  }
		}

	     /* Check if the first element is included within a paragraph */
	     elType = TtaGetElementType (TtaGetParent (first));
	     if (elType.ElTypeNum == HTML_EL_BODY ||
		 elType.ElTypeNum == HTML_EL_Division ||
#ifdef COUGAR
		 elType.ElTypeNum == HTML_EL_Object_Content ||
#endif /* COUGAR */
		 elType.ElTypeNum == HTML_EL_Data_cell ||
		 elType.ElTypeNum == HTML_EL_Heading_cell ||
		 elType.ElTypeNum == HTML_EL_Block_Quote)
	       {
		 elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
		 parag = TtaNewElement (doc, elType);
		 TtaInsertSibling (parag, first, TRUE, doc);
		 TtaInsertFirstChild (&anchor, parag, doc);
	       }
	     else
	       TtaInsertSibling (anchor, first, TRUE, doc);

	     /* move the selected elements within the new Anchor element */
	     child = first;
	     prev = NULL;
	     while (child != NULL)
	       {
		  /* prepare the next element in the selection, as the current element */
		  /* will be moved and its successor will no longer be accessible */
		  next = child;
		  TtaNextSibling (&next);
		  /* remove the current element */
		  TtaRemoveTree (child, doc);
		  /* insert it as a child of the new anchor element */
		  if (prev == NULL)
		     TtaInsertFirstChild (&child, anchor, doc);
		  else
		     TtaInsertSibling (child, prev, FALSE, doc);
		  /* get the next element in the selection */
		  prev = child;
		  if (child == last)
		     child = NULL;
		  else
		     child = next;
	       }
	  }
     }

   TtaSetDocumentModified (doc);
   /* ask Thot to display changes made in the document */
   TtaSetDisplayMode (doc, dispMode);
   TtaSelectElement (doc, anchor);
   if (createLink)
     {
	/* Select the destination. The anchor element must have an HREF attribute */
	SelectDestination (doc, anchor);
	/* create an attribute PseudoClass = link */
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
	attr = TtaGetAttribute (anchor, attrType);
	if (attr == NULL)
	  {
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (anchor, attr, doc);
	  }
	TtaSetAttributeText (attr, "link", anchor, doc);
     }
   else
      CreateTargetAnchor (doc, anchor);
}


/*----------------------------------------------------------------------
   an HTML attribute "size" has been modified for a Font element.  
   Create the corresponding internal attribute and delete the old. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrFontSizeModified (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrFontSizeModified (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (event->document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (event->document, DeferredDisplay);

   AttrFontSizeDelete (event);
   AttrFontSizeCreated (event);
   TtaSetDisplayMode (event->document, dispMode);
}

/*----------------------------------------------------------------------
   SetCharFontOrPhrase                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetCharFontOrPhrase (int document, int eltype)
#else  /* __STDC__ */
void                SetCharFontOrPhrase (document, eltype)
int                 document;
int                 eltype;

#endif /* __STDC__ */
{
   Element             selectedEl, elem, firstSelectedElem, lastSelectedElem,
                       child, next, elFont;
   int                 firstChar, nextFirstChar, lastChar, nextLastChar,
                       length, firstSelectedChar, lastSelectedChar;
   ElementType         elType, selType;
   boolean             remove, done;
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, DeferredDisplay);
   TtaGiveFirstSelectedElement (document, &selectedEl, &firstChar, &lastChar);
   /* get the first leaf in the first selected element */
   elem = selectedEl;
   do
     {
	child = TtaGetFirstChild (elem);
	if (child != NULL)
	   elem = child;
     }
   while (child != NULL);

   /* If the first leaf of the first selected element is within an element */
   /* of the requested type, the text leaves of selected elements should not */
   /* be any longer within an element of that type */
   /* else, they should all be within an element of that type */
   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = eltype;
   remove = (TtaGetTypedAncestor (elem, elType) != NULL);

   /* process all selected elements */
   firstSelectedElem = selectedEl;
   firstSelectedChar = firstChar;
   TtaClearViewSelections ();
   while (selectedEl != NULL)
     {
	/* get the element to be processed after the current element: the */
	/* current element may change during processing */
	next = selectedEl;
	TtaGiveNextSelectedElement (document, &next, &nextFirstChar, &nextLastChar);
	lastSelectedElem = selectedEl;
	lastSelectedChar = lastChar;
	selType = TtaGetElementType (selectedEl);
	if (!TtaIsLeaf (selType))
	   /* this selected element is not a leaf. Process all text leaves of */
	   /* that element */
	   SetFontOrPhraseOnElement ((Document) document, selectedEl, eltype, remove);
	else if (selType.ElTypeNum == HTML_EL_TEXT_UNIT)
	   /* this selected element is a text leaf */
	  {
	     /* is this element within an element of the requested type ? */
	     done = FALSE;
	     elFont = TtaGetTypedAncestor (selectedEl, elType);
	     if (remove)
		/* the element has to be removed from an element of type elType */
		/* If it is not within such an element, nothing to do */
		done = (elFont == NULL);
	     else
		/* the element should be witihn an element of type elType */
		/* If it is already within such an element, nothing to do */
		done = (elFont != NULL);
	     if (!done)
	       {
		  /* split that text leaf if it is not entirely selected */
		  elem = selectedEl;
		  length = TtaGetTextLength (elem);
		  if (lastChar != 0 && lastChar < length)
		     TtaSplitText (elem, lastChar, document);
		  if (firstChar > 1)
		    {
		       TtaSplitText (elem, firstChar - 1, document);
		       TtaNextSibling (&elem);
		       selectedEl = elem;
		       if (lastChar != 0 && lastChar <= length)
			  lastSelectedElem = elem;
		       firstSelectedElem = elem;
		       firstSelectedChar = 1;
		       if (lastChar > 0)
			  lastSelectedChar = lastChar - firstChar + 1;
		    }
		  /* process the text leaf */
		  if (remove)
		     ResetFontOrPhraseOnText (document, elem, eltype);
		  else
		    {
		       SetFontOrPhraseOnText (document, &elem, eltype);
		       if (elem != lastSelectedElem)
			 {
			    lastSelectedElem = elem;
			    lastSelectedChar = 0;
			 }
		    }
	       }
	  }
	/* next selected element */
	selectedEl = next;
	firstChar = nextFirstChar;
	lastChar = nextLastChar;
     }

   TtaSetDisplayMode (document, dispMode);
   if (firstSelectedElem == lastSelectedElem)
      if (firstSelectedChar > 1 || lastSelectedChar > 0)
	 TtaSelectString (document, firstSelectedElem, firstSelectedChar,
			  lastSelectedChar);
      else
	 TtaSelectElement (document, firstSelectedElem);
   else
     {
	if (firstSelectedChar > 1)
	   TtaSelectString (document, firstSelectedElem, firstSelectedChar, 0);
	else
	   TtaSelectElement (document, firstSelectedElem);
	TtaExtendSelection (document, lastSelectedElem, lastSelectedChar);
     }

   UpdateContextSensitiveMenus (document);

   switch (eltype)
	 {
	    case HTML_EL_Emphasis:
	       SelectionInEM = !remove;
	       break;
	    case HTML_EL_Strong:
	       SelectionInSTRONG = !remove;
	       break;
	    case HTML_EL_Cite:
	       SelectionInCITE = !remove;
	       break;
	    case HTML_EL_Def:
	       SelectionInDFN = !remove;
	       break;
	    case HTML_EL_Code:
	       SelectionInCODE = !remove;
	       break;
	    case HTML_EL_Variable:
	       SelectionInVAR = !remove;
	       break;
	    case HTML_EL_Sample:
	       SelectionInSAMP = !remove;
	       break;
	    case HTML_EL_Keyboard:
	       SelectionInKBD = !remove;
	       break;
	    case HTML_EL_Italic_text:
	       SelectionInI = !remove;
	       break;
	    case HTML_EL_Bold_text:
	       SelectionInB = !remove;
	       break;
	    case HTML_EL_Teletype_text:
	       SelectionInTT = !remove;
	       break;
	    case HTML_EL_Underlined_text:
	       SelectionInU = !remove;
	       break;
	    case HTML_EL_Struck_text:
	       SelectionInSTRIKE = !remove;
	       break;
	    case HTML_EL_Big_text:
	       SelectionInBIG = !remove;
	       break;
	    case HTML_EL_Small_text:
	       SelectionInSMALL = !remove;
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   SelectDestination selects the destination of the el Anchor.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectDestination (Document doc, Element el)
#else  /* __STDC__ */
void                SelectDestination (doc, el)
Document            doc;
Element             el;

#endif /* __STDC__ */
{
   Element             targetEl;
   Document            targetDoc;
   Attribute           attrNAME, attrHREF;
   AttributeType       attrType;
   char               *buffer;
   int                 length;

   /* select target document and target anchor */
   TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SEL_TARGET), NULL);
   TtaClickElement (&targetDoc, &targetEl);
   if (targetDoc != (Document) None && targetEl != (Element) NULL)
     {
	/* get attrName of the enclosing end anchor */
	attrNAME = GetNameAttr (targetDoc, targetEl);
	/* the document becomes the target doc */
	SetTargetContent (targetDoc, attrNAME);
     }
   else
     {
	targetDoc = doc;
	TargetName = NULL;
     }

   if (doc == targetDoc && TargetName == NULL)
     {
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
	/* Ask for the textual form of the value */
	AttrHREFelement = el;
	AttrHREFdocument = doc;
	/* Dialogue form to insert HREF name */
	TtaNewForm (BaseDialog + AttrHREFForm, TtaGetViewFrame (doc, 1),  TtaGetMessage (AMAYA, AM_ATTRIBUTE), TRUE, 2, 'L', D_DONE);
	TtaNewTextForm (BaseDialog + AttrHREFText, BaseDialog + AttrHREFForm,
			TtaGetMessage (AMAYA, AM_HREF_VALUE), 50, 1, FALSE);
	/* If the anchor has an HREF attribute, put its value in the form */
	attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
	attrType.AttrTypeNum = HTML_ATTR_HREF_;
	attrHREF = TtaGetAttribute (el, attrType);
	if (attrHREF != 0)
	  {
	     /* get a buffer for the attribute value */
	     length = TtaGetTextAttributeLength (attrHREF);
	     buffer = TtaGetMemory (length + 1);
	     /* copy the HREF attribute into the buffer */
	     TtaGiveTextAttributeValue (attrHREF, buffer, &length);
	     /* initialise the text field in the dialogue box */
	     TtaSetTextForm (BaseDialog + AttrHREFText, buffer);
	     TtaFreeMemory (buffer);
	  }
	TtaShowDialogue (BaseDialog + AttrHREFForm, FALSE);
     }
   else
     {
	/* create an attribute HREF for the Anchor */
	SetHREFattribute (el, doc, targetDoc);
	TtaSetStatus (doc, 1, " ", NULL);
     }
}
