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
 *          R.Guetari (W3C/INRIA) - Windows routines.
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"


#include "css_f.h"
#include "init_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLactions_f.h"
#include "HTMLstyle_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "trans_f.h"
#include "selection.h"

#ifdef _WINDOWS
HWND currentWindow = NULL;
static char WIN_buffer [1024];
#endif /* _WINDOWS */

/**** Some prototypes *****/
#ifdef __STDC__
static boolean      FollowTheLink (Element anchor, Element elSource, Document doc);
#else  /* __STDC__ */
static boolean      FollowTheLink (/* anchor, elSource, doc */);

#endif /* __STDC__ */

/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _FollowTheLink_context {
  Document             doc;
  Element              anchor;
  Element              elSource;
  char                *sourceDocUrl;
  char                *url;
} FollowTheLink_context;

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
   SearchNAMEattribute: search in document doc an element having an      
   attribut NAME or ID whose value is nameVal.         
   Return that element or NULL if not found.               
   If ignore is not NULL, it is an attribute that should be ignored when
   comparing NAME attributes.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             SearchNAMEattribute (Document doc, char *nameVal, Attribute ignore)
#else  /* __STDC__ */
Element             SearchNAMEattribute (doc, nameVal, ignore)
Document            doc;
char               *nameVal;
Attribute           ignore;

#endif /* __STDC__ */
{
   Element             el, elFound;
   AttributeType       attrType;
   Attribute           nameAttr;
   boolean             found;
   int                 length;
   char               *name;

   el = TtaGetMainRoot (doc);
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_NAME;
   found = FALSE;
   /* search all elements having an attribute NAME */
   do
     {
	TtaSearchAttribute (attrType, SearchForward, el, &elFound, &nameAttr);
	if (nameAttr != NULL && elFound != NULL)
	   if (nameAttr != ignore)
	     {
		length = TtaGetTextAttributeLength (nameAttr);
		length++;
		name = TtaGetMemory (length);
		if (name != NULL)
		  {
		     TtaGiveTextAttributeValue (nameAttr, name, &length);
		     /* compare the NAME attribute */
		     found = (strcmp (name, nameVal) == 0);
		     TtaFreeMemory (name);
		  }
	     }
	if (!found)
	   el = elFound;
     }
   while (!found && elFound != NULL);

   if (!found)
     {
       /* search all elements having an attribute ID */
       el = TtaGetMainRoot (doc);
       attrType.AttrTypeNum = HTML_ATTR_ID;
       found = FALSE;
       do
	 {
	   TtaSearchAttribute (attrType, SearchForward, el, &elFound, &nameAttr);
	   if (nameAttr != NULL && elFound != NULL)
	     if (nameAttr != ignore)
	       {
		 length = TtaGetTextAttributeLength (nameAttr);
		 length++;
		 name = TtaGetMemory (length);
		 if (name != NULL)
		   {
		     TtaGiveTextAttributeValue (nameAttr, name, &length);
		     /* compare the NAME attribute */
		     found = (strcmp (name, nameVal) == 0);
		     TtaFreeMemory (name);
		   }
	       }
	   if (!found)
	     el = elFound;
	 }
       while (!found && elFound != NULL);
     }
   if (found)
      return (elFound);
   else
      return (NULL);
}


/*----------------------------------------------------------------------
   FollowTheLink_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               FollowTheLink_callback (int targetDocument, int status, 
					   char *urlName,
					   char *outputfile, 
					   char *content_type,
					   void * context)
#else  /* __STDC__ */
void               FollowTheLink_callback (targetDocument, status, urlName,
                                             outputfile, content_type, 
                                             context)
int TargetDocument;
int status;
char *url, urlName;
char *outputfile;
char *content_type;
void *context;

#endif
{
  Element             elFound;
  ElementType         elType;
  Element             elSource;
  Document             doc;
  Element              anchor;
  char                *sourceDocUrl, *url;
  AttributeType       attrType;
  Attribute           PseudoAttr;
  SSchema             docSchema; 
  View                view;
  FollowTheLink_context      *ctx = (FollowTheLink_context *) context;

  /* retrieve the context */

  if (ctx == NULL)
    return;

  doc = ctx->doc;
  sourceDocUrl = ctx->sourceDocUrl;  
  anchor = ctx->anchor;
  url = ctx->url;
  elSource = ctx->elSource;
  docSchema = TtaGetDocumentSSchema (doc);

  if ((doc != targetDocument || url[0] == '#') && anchor != NULL) 
    {
      /* search PseudoAttr attribute */
      attrType.AttrSSchema = docSchema;
      attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
      PseudoAttr = TtaGetAttribute (anchor, attrType);
      /* if the target document has replaced the clicked
	 document, pseudo attribute "visited" should not be set */
      if (targetDocument == doc)
	/* the target document is in the same window as the
	   source document */
	if (strcmp (sourceDocUrl, DocumentURLs[targetDocument]))
	  /* both document have different URLs */
	  PseudoAttr = NULL;
      
      if (PseudoAttr != NULL)	
	TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
    }

  if (url[0] == '#' && targetDocument != 0)
    {
      /* attribute HREF contains the NAME of a target anchor */
      elFound = SearchNAMEattribute (targetDocument, &url[1], NULL);
      if (elFound != NULL)
	{
	  elType = TtaGetElementType (elFound);
	  if (elType.ElSSchema == docSchema && elType.ElTypeNum == HTML_EL_LINK)
	    {
	      /* the target is a link element, follow this link */
	      FollowTheLink (elFound, elSource, doc);
	      return;
	    }
	  else
	    {
	      if (targetDocument == doc)
		/* jump in the same document */
		{
		  /* record current position in the history */
		  AddDocHistory (doc, DocumentURLs[doc]);
		}
	      /* show the target element in all views */
	      for (view = 1; view < 4; view++)
		if (TtaIsViewOpened (targetDocument, view))
		  TtaShowElement (targetDocument, view, elFound, 0);
	    }
	}
    }
  if (targetDocument > 0)
    TtaRaiseView (targetDocument, 1);
  TtaFreeMemory (url);
  if (sourceDocUrl)
    TtaFreeMemory (sourceDocUrl);
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
  FollowTheLink follows the link given by the anchor element for a
  double click on the elSource element.
  The parameter doc is the document that includes the anchor element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      FollowTheLink (Element anchor, Element elSource, Document doc)
#else  /* __STDC__ */
static boolean      FollowTheLink (anchor, elSource, doc)
Element             anchor;
Element             elSource;
Document            doc;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           HrefAttr, PseudoAttr, attr;
   ElementType         elType;
   Document            targetDocument;
   SSchema             docSchema;
   char                documentURL[MAX_LENGTH];
   char               *url, *info, *sourceDocUrl;
   int                 length;
   FollowTheLink_context *ctx;

   docSchema = TtaGetDocumentSSchema (doc);
   if (anchor != NULL)
     {
	/* search HREF attribute */
	attrType.AttrSSchema = docSchema;
	attrType.AttrTypeNum = HTML_ATTR_HREF_;
	HrefAttr = TtaGetAttribute (anchor, attrType);
     }
   else
      HrefAttr = NULL;

   if (HrefAttr != NULL)
     {
	targetDocument = 0;
	PseudoAttr = NULL;
	/* get a buffer for the URL */
	length = TtaGetTextAttributeLength (HrefAttr);
	length++;
	url = TtaGetMemory (length);
	if (url != NULL)
	  {
	     elType = TtaGetElementType (anchor);
	     if (elType.ElTypeNum == HTML_EL_Anchor &&
		 elType.ElSSchema == docSchema)
	       {
		  /* attach an attribute PseudoClass = active */
		  attrType.AttrSSchema = docSchema;
		  attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
		  PseudoAttr = TtaGetAttribute (anchor, attrType);
		  if (PseudoAttr == NULL)
		    {
		       PseudoAttr = TtaNewAttribute (attrType);
		       TtaAttachAttribute (anchor, PseudoAttr, doc);
		    }
		  TtaSetAttributeText (PseudoAttr, "active", anchor, doc);
	       }
	     /* get the URL itself */
	     TtaGiveTextAttributeValue (HrefAttr, url, &length);

	     /* suppress white spaces at the end */
	     length--;
	     while (url[length] == ' ')
		url[length--] = EOS;
	     /* save the complete URL of the source document */
	     length = strlen (DocumentURLs[doc])+1;
	     sourceDocUrl = TtaGetMemory (length);
	     strcpy (sourceDocUrl, DocumentURLs[doc]);
	     /* save the context */
	     ctx = TtaGetMemory (sizeof (FollowTheLink_context));
	     ctx->anchor = anchor;
	     ctx->doc = doc;
	     ctx->url = url;
	     ctx->elSource = elSource;
	     ctx->sourceDocUrl = sourceDocUrl;
	     TtaSetSelectionMode (TRUE);
	     if (url[0] == '#')
	       {
		 /* the target element is part of the same document */
		 targetDocument = doc;
		 /* manually invoke the callback */
		 FollowTheLink_callback (targetDocument, 1, NULL, NULL, NULL, 
 					 (void *) ctx);
		 /*
		 if (PseudoAttr != NULL)
		   TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
		   */

	       }
	     else
		/* the target element seems to be in another document */
	       {
		  strcpy (documentURL, url);
		  url[0] = EOS;
		  /* is the source element an image map? */
		  attrType.AttrSSchema = docSchema;
		  attrType.AttrTypeNum = HTML_ATTR_ISMAP;
		  attr = TtaGetAttribute (elSource, attrType);
		  if (attr != NULL)
		    /* it's an image map */
		    {
		      info = GetActiveImageInfo (doc, elSource);
		      if (info != NULL)
			{
			  strcat (documentURL, info);
			  TtaFreeMemory (info);
			}
		    }
		  /* get the referred document */
		  targetDocument = GetHTMLDocument (documentURL, NULL,
				   doc, doc, CE_TRUE, TRUE, 
				   (void *) FollowTheLink_callback, (void *) ctx);
	       }
	return (TRUE);
	  }
     }
   return (FALSE);
}


/*----------------------------------------------------------------------
   ]lick     The user has double-clicked an element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DoubleClick (NotifyElement * event)
#else  /* __STDC__ */
boolean             DoubleClick (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             anchor, elFound;
   ElementType         elType;
   SSchema             docSchema;
   boolean	       ok;

   docSchema = TtaGetDocumentSSchema (event->document);

   /* Check if the current element is interested in double click */
   ok = FALSE;
   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
       elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
       elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
       elType.ElTypeNum == HTML_EL_SYMBOL_UNIT)
     ok = TRUE;
   else
     if (elType.ElSSchema == docSchema)
	if (elType.ElTypeNum == HTML_EL_LINK ||
	    elType.ElTypeNum == HTML_EL_C_Empty ||
	    elType.ElTypeNum == HTML_EL_Radio_Input ||
	    elType.ElTypeNum == HTML_EL_Checkbox_Input ||
	    elType.ElTypeNum == HTML_EL_Frame ||
	    elType.ElTypeNum == HTML_EL_Option_Menu ||
	    elType.ElTypeNum == HTML_EL_Submit_Input ||
	    elType.ElTypeNum == HTML_EL_Reset_Input)
	   ok = TRUE;
   if (!ok)
      /* DoubleClick is disabled */
      return (FALSE);

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
	return (TRUE);
     }
   else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* is it a double click on graphic submit element? */
       elType = TtaGetElementType (TtaGetParent (event->element));
       elType.ElTypeNum = HTML_EL_Form;
       elFound = TtaGetTypedAncestor (event->element, elType);
       if (elFound != NULL)
	 {
	   attrType.AttrSSchema = docSchema;
	   attrType.AttrTypeNum = HTML_ATTR_NAME;
	   attr = TtaGetAttribute (event->element, attrType);
	   if (attr)
	     {
	       /* it's a graphic submit element */
	       SubmitForm (event->document, event->element);
	       return (TRUE);
	     }
	 }
     }
   else if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     {
	/* is it an option menu ? */
	elFound = TtaGetParent (event->element);
	elType = TtaGetElementType (elFound);
	if (elType.ElTypeNum == HTML_EL_Option &&
	    elType.ElSSchema == docSchema)
	  {
	     SelectOneOption (event->document, elFound);
	     return (TRUE);
	  }
     }
   else if (elType.ElTypeNum == HTML_EL_Option_Menu &&
	    elType.ElSSchema == docSchema)
     {
	/* it is an option menu */
	elFound = TtaGetFirstChild (event->element);
	elType = TtaGetElementType (elFound);
	if (elType.ElTypeNum == HTML_EL_Option)
	  {
	     SelectOneOption (event->document, elFound);
	     return (TRUE);
	  }
     }
   else if (elType.ElTypeNum == HTML_EL_Checkbox_Input &&
	    elType.ElSSchema == docSchema)
     {
	SelectCheckbox (event->document, event->element);
	return (TRUE);
     }
   else if (elType.ElTypeNum == HTML_EL_Radio_Input &&
	    elType.ElSSchema == docSchema)
     {
	SelectOneRadio (event->document, event->element);
	return (TRUE);
     }

   /* Search the anchor or LINK element */
   anchor = SearchAnchor (event->document, event->element, TRUE);
   if (anchor == NULL)
      if (elType.ElTypeNum == HTML_EL_LINK &&
	  elType.ElSSchema == docSchema)
	 anchor = event->element;
      else
	{
	   elType.ElTypeNum = HTML_EL_LINK;
	   elType.ElSSchema = docSchema;
	   anchor = TtaGetTypedAncestor (event->element, elType);
	}

   return (FollowTheLink (anchor, event->element, event->document));
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
  int                i;
  char              *tempdocument;

  if (doc == 0)
    return;
  if (DocumentURLs[doc] != NULL)
    {
      if (IsHTTPPath (DocumentURLs[doc]))
	{
	  /* remove the temporary document file */
	  tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
	  TtaFileUnlink (tempdocument);
	  TtaFreeMemory (tempdocument);
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
#else
#ifdef AMAYA_ILU
#else
	  QueryClose ();
#endif
#endif
	  TtaFreeMemory (LastURLName);
	  TtaFreeMemory (DocumentName);
	  TtaFreeMemory (SavePath);
	  TtaFreeMemory (SaveName);
	  TtaFreeMemory (ObjectName);
	  TtaFreeMemory (SaveImgsURL);
	  TtaFreeMemory (SavingFile);
	  TtaFreeMemory (AttrHREFvalue);
	  FreeHTMLParser ();
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
	     TtaSetItemOff (doc, 1, Types, BImage);
	     TtaSetItemOff (doc, 1, Style, TBig);
	     TtaSetItemOff (doc, 1, Style, TSmall);
	     TtaSetItemOff (doc, 1, Style, BSub);
	     TtaSetItemOff (doc, 1, Style, BSup);
	     TtaSetItemOff (doc, 1, Style, BFont);
	  }
	else
	  {
	     TtaSetItemOn (doc, 1, Types, BImage);
	     TtaSetItemOn (doc, 1, Style, TBig);
	     TtaSetItemOn (doc, 1, Style, TSmall);
	     TtaSetItemOn (doc, 1, Style, BSub);
	     TtaSetItemOn (doc, 1, Style, BSup);
	     TtaSetItemOn (doc, 1, Style, BFont);
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
	   TtaSetItemOff (doc, 2, StructTypes, BComment);
	else
	   TtaSetItemOn (doc, 2, StructTypes, BComment);
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
	if (SelectionDoc != 0 && DocumentURLs[SelectionDoc] != NULL)
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
boolean               ElemToTransform (event)
NotifyOnElementType * event;
#endif  /* __STDC__ */
{
   return (!TransformIntoType(event->targetElementType, event->document));
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

   lastSelectedElem = NULL;
   lastSelectedChar = 0;
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
#       ifndef _WINDOWS 
	TtaNewForm (BaseDialog + AttrHREFForm, TtaGetViewFrame (doc, 1),  TtaGetMessage (AMAYA, AM_ATTRIBUTE), TRUE, 2, 'L', D_DONE);
	TtaNewTextForm (BaseDialog + AttrHREFText, BaseDialog + AttrHREFForm,
			TtaGetMessage (AMAYA, AM_HREF_VALUE), 50, 1, FALSE);
#       endif /* !__WINDOWS */
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
	     strcpy (AttrHREFvalue, buffer);
#            ifndef _WINDOWS
	     /* initialise the text field in the dialogue box */
	     TtaSetTextForm (BaseDialog + AttrHREFText, buffer);
#            endif /* _WINDOWS */
	     TtaFreeMemory (buffer);
	  }

#   ifndef _WINDOWS
	TtaShowDialogue (BaseDialog + AttrHREFForm, FALSE);
#   else  /* _WINDOWS */
	CreateLinkDlgWindow (currentWindow, AttrHREFvalue, BaseDialog, AttrHREFForm, AttrHREFText);
#   endif  /* _WINDOWS */
     }
   else
     {
       TtaSetDocumentModified (doc);
       /* create an attribute HREF for the Anchor */
       SetHREFattribute (el, doc, targetDoc);
       TtaSetStatus (doc, 1, " ", NULL);
     }
}
