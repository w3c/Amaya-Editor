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
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "html2thot_f.h"
#include "XMLparser_f.h"
#include "trans_f.h"
#include "selection.h"
#include "styleparser_f.h"

#ifdef _WINDOWS
#include "wininclude.h"

HWND currentWindow = NULL;
static CHAR_T WIN_buffer [1024];
#endif /* _WINDOWS */

/**** Some prototypes *****/
#ifdef __STDC__
static ThotBool     FollowTheLink (Element anchor, Element elSource, Document doc);
#else  /* __STDC__ */
static ThotBool     FollowTheLink (/* anchor, elSource, doc */);

#endif /* __STDC__ */

/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _FollowTheLink_context {
  Document             doc;
  Element              anchor;
  Element              elSource;
  STRING               sourceDocUrl;
  STRING               url;
} FollowTheLink_context;

extern ThotBool HTMLErrorsFound;

/*----------------------------------------------------------------------
   ResetFontOrPhraseOnText: The text element elem should
   not be any longer within an element of type notType.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ResetFontOrPhraseOnText (Document document, Element elem, int notType)
#else  /* __STDC__ */
static void         ResetFontOrPhraseOnText (document, elem, notType)
Document            document;
Element             elem;
int                 notType;

#endif /* __STDC__ */
{
   ElementType         elType, parentType;
   Element             elFont, parent, prev, next, added, child, last;

   elType.ElSSchema = TtaGetSSchema ("HTML", document);
   elType.ElTypeNum = notType;
   /* is this element already within an element of the requested type? */
   elFont = TtaGetTypedAncestor (elem, elType);
   if (elFont != NULL)
     {
	do
	  {
	     parent = TtaGetParent (elem);
	     parentType = TtaGetElementType (parent);
	     prev = elem;
	     TtaPreviousSibling (&prev);
	     next = elem;
	     TtaNextSibling (&next);
	     if (prev != NULL)
	       {
		  added = TtaNewElement (document, parentType);
		  TtaInsertSibling (added, parent, TRUE, document);
		  TtaRegisterElementCreate (added, document);
		  child = prev;
		  TtaPreviousSibling (&prev);
		  TtaRegisterElementDelete (child, document);
		  TtaRemoveTree (child, document);
		  TtaInsertFirstChild (&child, added, document);
		  TtaRegisterElementCreate (child, document);
		  while (prev != NULL)
		    {
		       last = child;
		       child = prev;
		       TtaPreviousSibling (&prev);
		       TtaRegisterElementDelete (child, document);
		       TtaRemoveTree (child, document);
		       TtaInsertSibling (child, last, TRUE, document);
		       TtaRegisterElementCreate (child, document);
		    }
	       }
	     if (next != NULL)
	       {
		  added = TtaNewElement (document, parentType);
		  TtaInsertSibling (added, parent, FALSE, document);
		  TtaRegisterElementCreate (added, document);
		  child = next;
		  TtaNextSibling (&next);
		  TtaRegisterElementDelete (child, document);
		  TtaRemoveTree (child, document);
		  TtaInsertFirstChild (&child, added, document);
		  TtaRegisterElementCreate (child, document);
		  while (next != NULL)
		    {
		       last = child;
		       child = next;
		       TtaNextSibling (&next);
		       TtaRegisterElementDelete (child, document);
		       TtaRemoveTree (child, document);
		       TtaInsertSibling (child, last, FALSE, document);
		       TtaRegisterElementCreate (child, document);
		    }
	       }
	     elem = parent;
	  }
	while (elFont != elem);
	child = TtaGetFirstChild (elem);
	TtaRegisterElementDelete (child, document);
	TtaRemoveTree (child, document);
	TtaInsertSibling (child, elem, TRUE, document);
	TtaRegisterElementCreate (child, document);
	TtaRegisterElementDelete (elem, document);
	TtaDeleteTree (elem, document);
	TtaSetDocumentModified (document);
     }
}


/*----------------------------------------------------------------------
   SetFontOrPhraseOnText: The text element elem should be 
   within an element of type newtype.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetFontOrPhraseOnText (Document document, Element elem,
				 int newtype)
#else  /* __STDC__ */
static void         SetFontOrPhraseOnText (document, elem, newtype)
Document            document;
Element             elem;
int                 newtype;

#endif /* __STDC__ */
{
   ElementType         elType, siblingType;
   Element             prev, next, child, added, parent;

   elType.ElSSchema = TtaGetSSchema ("HTML", document);
   elType.ElTypeNum = newtype;
   /* is this element already within an element of the requested type? */
   if (TtaGetTypedAncestor (elem, elType) == NULL)
     {
	/* it is not within an element of type newtype */
	prev = elem;
	TtaPreviousSibling (&prev);
	if (prev != NULL)
	  {
	     siblingType = TtaGetElementType (prev);
	     if (siblingType.ElTypeNum == newtype)
	       {
		  child = TtaGetLastChild (prev);
		  if (child != NULL)
		    {
		       if (TtaCanInsertSibling (TtaGetElementType (elem),
						child, FALSE, document))
			 {
			    TtaRegisterElementDelete (elem, document);
			    TtaRemoveTree (elem, document);
			    TtaInsertSibling (elem, child, FALSE, document);
			    TtaRegisterElementCreate (elem, document);
			    TtaSetDocumentModified (document);
			 }
		    }
		  else
		    {
		       if (TtaCanInsertFirstChild (TtaGetElementType (elem),
						   prev, document))
			 {
			    TtaRegisterElementDelete (elem, document);
			    TtaRemoveTree (elem, document);
			    TtaInsertFirstChild (&elem, prev, document);
			    TtaRegisterElementCreate (elem, document);
			    TtaSetDocumentModified (document);
			 }
		    }
	       }
	     else
	       {
		  if (TtaCanInsertSibling (elType, prev, FALSE, document))
		    {
		       added = TtaNewElement (document, elType);
		       TtaRegisterElementDelete (elem, document);
		       TtaRemoveTree (elem, document);
		       TtaInsertSibling (added, prev, FALSE, document);
		       TtaRegisterElementCreate (added, document);
		       TtaInsertFirstChild (&elem, added, document);
		       TtaRegisterElementCreate (elem, document);
		       TtaSetDocumentModified (document);
		    }
	       }
	  }
	else
	  {
	     next = elem;
	     TtaNextSibling (&next);
	     if (next != NULL)
	       {
		  siblingType = TtaGetElementType (next);
		  if (siblingType.ElTypeNum == newtype)
		    {
		       child = TtaGetFirstChild (next);
		       if (child != NULL)
			 {
			    if (TtaCanInsertSibling (TtaGetElementType (elem), child, TRUE, document))
			      {
				 TtaRegisterElementDelete (elem, document);
				 TtaRemoveTree (elem, document);
				 TtaInsertSibling (elem, child, TRUE, document);
				 TtaRegisterElementCreate (elem, document);
				 TtaSetDocumentModified (document);
			      }
			 }
		       else
			 {
			    if (TtaCanInsertFirstChild (TtaGetElementType (elem), next, document))
			      {
				 TtaRegisterElementDelete (elem, document);
				 TtaRemoveTree (elem, document);
				 TtaInsertFirstChild (&elem, next, document);
				 TtaRegisterElementCreate (elem, document);
				 TtaSetDocumentModified (document);
			      }
			 }
		    }
		  else
		    {
		       if (TtaCanInsertSibling (elType, next, TRUE, document))
			 {
			    TtaRegisterElementDelete (elem, document);
			    TtaRemoveTree (elem, document);
			    added = TtaNewElement (document, elType);
			    TtaInsertSibling (added, next, TRUE, document);
			    TtaRegisterElementCreate (added, document);
			    TtaInsertFirstChild (&elem, added, document);
			    TtaRegisterElementCreate (elem, document);
			    TtaSetDocumentModified (document);
			 }
		    }
	       }
	     else
	       {
		  parent = TtaGetParent (elem);
		  if (TtaCanInsertFirstChild (elType, parent, document))
		    {
		       TtaRegisterElementDelete (elem, document);
		       TtaRemoveTree (elem, document);
		       added = TtaNewElement (document, elType);
		       TtaInsertFirstChild (&added, parent, document);
		       TtaRegisterElementCreate (added, document);
		       TtaInsertFirstChild (&elem, added, document);
		       TtaRegisterElementCreate (elem, document);
		       TtaSetDocumentModified (document);
		    }
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   SetFontOrPhraseOnElement                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetFontOrPhraseOnElement (Document document, Element elem,
					      int elemtype, ThotBool remove)
#else  /* __STDC__ */
static void         SetFontOrPhraseOnElement (document, elem, elemtype, remove)
Document            document;
Element             elem;
int                 elemtype;
ThotBool            remove;

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
	TtaRegisterElementCreate (child, document);
	}
     }
   while (child != NULL)
     {
	next = child;
	TtaNextSibling (&next);
	elType = TtaGetElementType (child);
	if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	   if (remove)
	      ResetFontOrPhraseOnText (document, child, elemtype);
	   else
	      SetFontOrPhraseOnText (document, child, elemtype);
	else if (!TtaIsLeaf (elType))
	   SetFontOrPhraseOnElement (document, child, elemtype, remove);
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
Element             SearchNAMEattribute (Document doc, STRING nameVal, Attribute ignore)
#else  /* __STDC__ */
Element             SearchNAMEattribute (doc, nameVal, ignore)
Document            doc;
STRING              nameVal;
Attribute           ignore;

#endif /* __STDC__ */
{
   Element             el, elFound;
   AttributeType       attrType;
   Attribute           nameAttr;
   ThotBool            found;
   int                 length;
   STRING              name;

   el = TtaGetMainRoot (doc);
   attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
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
		     found = (ustrcmp (name, nameVal) == 0);
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
		     found = (ustrcmp (name, nameVal) == 0);
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
					   STRING urlName,
					   STRING outputfile, 
					   STRING content_type,
					   void * context)
#else  /* __STDC__ */
void               FollowTheLink_callback (targetDocument, status, urlName,
                                             outputfile, content_type, 
                                             context)
int TargetDocument;
int status;
STRING url, urlName;
STRING outputfile;
STRING content_type;
void *context;

#endif
{
  Element             elFound;
  ElementType         elType;
  Element             elSource;
  Document            doc;
  Element             anchor;
  STRING              sourceDocUrl, url;
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

  if (url[0] == '#' && targetDocument != 0)
    /* attribute HREF contains the NAME of a target anchor */
    elFound = SearchNAMEattribute (targetDocument, &url[1], NULL);

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
	if (ustrcmp (sourceDocUrl, DocumentURLs[targetDocument]))
	  /* both document have different URLs */
	  PseudoAttr = NULL;
     
      /* only turn off the link if it points that exists or that we can
	 follow */
      if (PseudoAttr != NULL && status != -1)
	{
	  if (url[0] == '#')
	    {
	      if (targetDocument != 0 && elFound)
		TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
	    }
	  else
	    TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
	}
    }

  if (url[0] == '#' && targetDocument != 0)
    {
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
		{
		/* jump in the same document */
		  /* record current position in the history */
		  AddDocHistory (doc, DocumentURLs[doc], 
				 DocumentMeta[doc]->form_data, 
				 DocumentMeta[doc]->method);
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
static ThotBool     FollowTheLink (Element anchor, Element elSource, Document doc)
#else  /* __STDC__ */
static ThotBool     FollowTheLink (anchor, elSource, doc)
Element             anchor;
Element             elSource;
Document            doc;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           HrefAttr, PseudoAttr, attr;
   ElementType         elType;
   Document            targetDocument;
   SSchema             HTMLSSchema;
   CHAR_T                documentURL[MAX_LENGTH];
   STRING              url, info, sourceDocUrl;
   int                 length;
   ThotBool		isHTML;
   FollowTheLink_context *ctx;

   info = NULL;
   HrefAttr = NULL;
   HTMLSSchema = TtaGetSSchema ("HTML", doc);

   if (anchor != NULL)
     {
        elType = TtaGetElementType (anchor);
	isHTML = TtaSameSSchemas (elType.ElSSchema, HTMLSSchema);
	/* search the HREF or CITE attribute */
        if (isHTML &&
		(elType.ElTypeNum == HTML_EL_Quotation ||
		 elType.ElTypeNum == HTML_EL_Block_Quote ||
		 elType.ElTypeNum == HTML_EL_INS ||
		 elType.ElTypeNum == HTML_EL_DEL))
	   attrType.AttrTypeNum = HTML_ATTR_cite;
	else if (isHTML && elType.ElTypeNum == HTML_EL_FRAME)
	   attrType.AttrTypeNum = HTML_ATTR_FrameSrc;
	else
	   attrType.AttrTypeNum = HTML_ATTR_HREF_;
	attrType.AttrSSchema = HTMLSSchema;
	HrefAttr = TtaGetAttribute (anchor, attrType);
     }

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
		 elType.ElSSchema == HTMLSSchema)
	       {
		  /* attach an attribute PseudoClass = active */
		  attrType.AttrSSchema = HTMLSSchema;
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
	     length = ustrlen (DocumentURLs[doc])+1;
	     sourceDocUrl = TtaGetMemory (length);
	     ustrcpy (sourceDocUrl, DocumentURLs[doc]);
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
		 FollowTheLink_callback (targetDocument, 0, NULL, NULL, NULL, 
 					 (void *) ctx);
		 /*
		 if (PseudoAttr != NULL)
		   TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
		   */

	       }
	     else
		/* the target element seems to be in another document */
	       {
		 ustrncpy (documentURL, url, MAX_LENGTH - 1);
		 documentURL[MAX_LENGTH - 1] = EOS;
		 url[0] = EOS;
		 /* is the source element an image map? */
		 attrType.AttrSSchema = HTMLSSchema;
		 attrType.AttrTypeNum = HTML_ATTR_ISMAP;
		 attr = TtaGetAttribute (elSource, attrType);
		 if (attr != NULL)
		   /* it's an image map */
		   {
		     info = GetActiveImageInfo (doc, elSource);
		     if (info != NULL)
		       {
			 /* @@ what do we do with the precedent parameters? */
			ustrcat (documentURL, info);
			TtaFreeMemory (info);
		       }
		   }
		 /* interrupt current transfer */
		 StopTransfer (doc, 1);	   
		 /* get the referred document */
		 targetDocument = GetHTMLDocument (documentURL, NULL,
				   doc, doc, CE_RELATIVE, TRUE, 
				   (void *) FollowTheLink_callback, (void *) ctx);
	       }
	return (TRUE);
	  }
     }
   return (FALSE);
}


/*----------------------------------------------------------------------
  DblClickOnButton     The user has double-clicked a BUTTON element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DblClickOnButton (Element element, Document document)
#else  /* __STDC__ */
static void         DblClickOnButton (element, document)
Element		element;
Document	document;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   int		       type;

   elType = TtaGetElementType (element);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Button_type;
   attr = TtaGetAttribute (element, attrType);
   if (!attr)
      /* default value of attribute type is submit */
      type = HTML_ATTR_Button_type_VAL_submit;
   else
      type = TtaGetAttributeValue (attr);
   if (type == HTML_ATTR_Button_type_VAL_button)
      {
      /**** Activate the corresponding event ****/;
      }
   else
      {
	/* interrupt current transfer */
	StopTransfer (document, 1);	   
	SubmitForm (document, element);
      }
}


/*----------------------------------------------------------------------
  DoubleClick     The user has double-clicked an element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            DoubleClick (NotifyElement * event)
#else  /* __STDC__ */
ThotBool            DoubleClick (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             anchor, elFound, ancestor, element;
   ElementType         elType, elType1;
   ThotBool	       ok, isHTML;

   element = event->element;
   elType = TtaGetElementType (element);
   isHTML = (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0);

   /* Check if the current element is interested in double click */
   ok = FALSE;
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
       elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
       elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
       elType.ElTypeNum == HTML_EL_SYMBOL_UNIT)
     /* it's a basic element */
     ok = TRUE;
   else
     if (isHTML)
	if (elType.ElTypeNum == HTML_EL_LINK ||
	    elType.ElTypeNum == HTML_EL_C_Empty ||
	    elType.ElTypeNum == HTML_EL_Radio_Input ||
	    elType.ElTypeNum == HTML_EL_Checkbox_Input ||
	    elType.ElTypeNum == HTML_EL_Frame ||
	    elType.ElTypeNum == HTML_EL_Option_Menu ||
	    elType.ElTypeNum == HTML_EL_Submit_Input ||
	    elType.ElTypeNum == HTML_EL_Reset_Input ||
	    elType.ElTypeNum == HTML_EL_BUTTON ||
	    elType.ElTypeNum == HTML_EL_File_Input ||
	    elType.ElTypeNum == HTML_EL_FRAME)
	   ok = TRUE;
   if (!ok)
      /* DoubleClick is disabled for this element type */
      return (FALSE);

   if (isHTML && (elType.ElTypeNum == HTML_EL_Frame ||
		  elType.ElTypeNum == HTML_EL_Submit_Input ||
		  elType.ElTypeNum == HTML_EL_Reset_Input))
     {
	if (elType.ElTypeNum == HTML_EL_Frame)
	   {
	     element = TtaGetParent (element);
	     elType1 = TtaGetElementType (element);
	   }
	else
	   elType1.ElTypeNum = elType.ElTypeNum;
	if (elType1.ElTypeNum == HTML_EL_Submit_Input ||
	    elType1.ElTypeNum == HTML_EL_Reset_Input)
	   /* it 's a double click on a submit or reset button */
	   {
	     /* interrupt current transfer */
	     StopTransfer (event->document, 1);	   
	     SubmitForm (event->document, element);
	   }
	else if (elType1.ElTypeNum == HTML_EL_BUTTON)
	   DblClickOnButton (element, event->document);
	return (TRUE);
     }
   else if (isHTML && (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
		       elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
		       elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
		       elType.ElTypeNum == HTML_EL_SYMBOL_UNIT))
     {
       /* is it a double click in a BUTTON element? */
       elType1.ElSSchema = elType.ElSSchema;
       elType1.ElTypeNum = HTML_EL_BUTTON;
       elFound = TtaGetTypedAncestor (element, elType1);
       if (elFound)
	 {
	  DblClickOnButton (elFound, event->document);
	  return (TRUE);
	 }
     }
   if (isHTML && elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* is it a double click on a graphic submit element? */
       elType.ElTypeNum = HTML_EL_Form;
       elFound = TtaGetTypedAncestor (element, elType);
       if (elFound != NULL)
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_NAME;
	   attr = TtaGetAttribute (element, attrType);
	   if (attr)
	     /* it's a graphic submit element */
	     {
	       /* interrupt current transfer */
	       StopTransfer (event->document, 1);	   
	       SubmitForm (event->document, element);
	       return (TRUE);
	     }
	 }
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     {
	/* is it an option menu ? */
	elFound = TtaGetParent (element);
	elType1 = TtaGetElementType (elFound);
	if (elType1.ElTypeNum == HTML_EL_Option)
	  {
	     SelectOneOption (event->document, elFound);
	     return (TRUE);
	  }
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_Option_Menu)
     {
	/* it is an option menu */
        elType1.ElSSchema = elType.ElSSchema;
        elType1.ElTypeNum = HTML_EL_Option;
        elFound = TtaSearchTypedElement (elType1, SearchInTree, element);
        if (elFound)
	  {
	     SelectOneOption (event->document, elFound);
	     return (TRUE);
	  }
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_Checkbox_Input)
     {
	SelectCheckbox (event->document, element);
	return (TRUE);
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_Radio_Input)
     {
	SelectOneRadio (event->document, element);
	return (TRUE);
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_File_Input)
     {
	ActivateFileInput (event->document, element);
	return (TRUE);
     }

   /* Search the anchor or LINK element */
   anchor = SearchAnchor (event->document, element, TRUE);
   if (anchor == NULL)
      if (isHTML && (elType.ElTypeNum == HTML_EL_LINK ||
		     elType.ElTypeNum == HTML_EL_FRAME))
	   anchor = element;
      else
	{
	   elType1.ElTypeNum = HTML_EL_LINK;
	   elType1.ElSSchema = TtaGetSSchema ("HTML", event->document);
	   anchor = TtaGetTypedAncestor (element, elType1);
	}
   /* if not found, search a cite or href attribute on an ancestor */
   if (anchor == NULL)
      {
	ancestor = element;
	attrType.AttrSSchema = TtaGetSSchema ("HTML", event->document);
	do
	   {
	   attrType.AttrTypeNum = HTML_ATTR_HREF_;
	   attr = TtaGetAttribute (ancestor, attrType);
	   if (!attr)
	      {
	      attrType.AttrTypeNum = HTML_ATTR_cite;
	      attr = TtaGetAttribute (ancestor, attrType);
	      }
	   if (attr)
	      anchor = ancestor;
	   else
	      ancestor = TtaGetParent (ancestor);
	   }
	while (anchor == NULL && ancestor != NULL);
      }

   return (FollowTheLink (anchor, element, event->document));
}


/*----------------------------------------------------------------------
  SimpleClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            SimpleClick (NotifyElement * event)
#else  /* __STDC__ */
ThotBool            SimpleClick (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);  
  if (usedouble)
    return TRUE;
  else
    /* don't let Thot perform normal operation */
    return (DoubleClick (event));
}

/*----------------------------------------------------------------------
   UpdateTitle update the content of the Title field on top of the 
   main window, according to the contents of element el.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateTitle (Element el, Document doc)
#else  /* __STDC__ */
void                UpdateTitle (el, doc)
Element             el;
Document            doc;

#endif /* __STDC__ */
{
   Element             textElem;
   int                 length;
   Language            lang;
   STRING              text;

   if (TtaGetViewFrame (doc, 1) == 0)
      /* this document is not displayed */
      return;
   textElem = TtaGetFirstChild (el);
   if (textElem != NULL)
     {
	length = TtaGetTextLength (textElem) + 1;
	text = TtaGetMemory (length);
	TtaGiveTextContent (textElem, text, &length, &lang);
	TtaSetTextZone (doc, 1, 2, text);
	UpdateAtom (doc, DocumentURLs[doc], text);
	TtaFreeMemory (text);
     }
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
  STRING             tempdocument;
  CHAR_T               htmlErrFile [80];

  if (doc == 0)
    return;

  TtaSetItemOff (doc, 1, Special, BShowLogFile);
  HTMLErrorsFound = FALSE;

  if (DocumentURLs[doc] != NULL)
    {
      if (IsHTTPPath (DocumentURLs[doc]))
	{
	  tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
	  TtaFileUnlink (tempdocument);
	  TtaFreeMemory (tempdocument);
	  sprintf (htmlErrFile, "%s%c%d%cHTML.ERR", TempFileDirectory, DIR_SEP, doc, DIR_SEP);
	  if (TtaFileExist (htmlErrFile))
	    TtaFileUnlink (htmlErrFile);
	}
      else if (DocumentTypes[doc] == docImage ||
	       DocumentTypes[doc] == docImageRO) 
	{
	  /* remove the HTML container for the local images */
	  tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
	  TtaFileUnlink (tempdocument); 
	  TtaFreeMemory (tempdocument);
	}

      if (DocumentTypes[doc] == docImage)
	DocumentTypes[doc] = docHTML;
      else if (DocumentTypes[doc] == docImageRO)
	DocumentTypes[doc] = docReadOnly;

      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = NULL;
      if (DocumentMeta[doc]->form_data)
	TtaFreeMemory (DocumentMeta[doc]->form_data);
      TtaFreeMemory (DocumentMeta[doc]);
      DocumentMeta[doc] = NULL;
      RemoveDocCSSs (doc);
      /* avoid to free images of backup documents */
      if (BackupDocument != doc)
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
	  TtaFreeMemory (UserCSS);
	  FreeHTMLParser ();
	  FreeXMLParser ();
	  FreeDocHistory ();
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
   ThotBool            NewSelInElem;

   TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
   /* 
    * elements PICTURE, Object, Applet, Big_text, Small_text, Subscript,
    * Superscript, Font_  are not permitted in a Preformatted element.
    * The corresponding menu entries must be turned off 
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
	     TtaSetItemOff (doc, 1, Types, BObject);
	     TtaSetItemOff (doc, 1, Style, TBig);
	     TtaSetItemOff (doc, 1, Style, TSmall);
	     TtaSetItemOff (doc, 1, Style, BSub);
	     TtaSetItemOff (doc, 1, Style, BSup);
	  }
	else
	  {
	     TtaSetItemOn (doc, 1, Types, BImage);
	     TtaSetItemOn (doc, 1, Types, BObject);
	     TtaSetItemOn (doc, 1, Style, TBig);
	     TtaSetItemOn (doc, 1, Style, TSmall);
	     TtaSetItemOn (doc, 1, Style, BSub);
	     TtaSetItemOn (doc, 1, Style, BSup);
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
	elType.ElTypeNum = HTML_EL_ABBR;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInABBR != NewSelInElem)
     {
	SelectionInABBR = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TAbbreviation, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_ACRONYM;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInACRONYM != NewSelInElem)
     {
	SelectionInACRONYM = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TAcronym, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_INS;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInINS != NewSelInElem)
     {
	SelectionInINS = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TInsertion, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_DEL;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInDEL != NewSelInElem)
     {
	SelectionInDEL = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TDeletion, NewSelInElem);
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
#ifdef IV
/*----------------------------------------------------------------------
   A element type conversion has not been achieved by the editor.
   Trying with language-driven restructuration.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool           ElemToTransform (NotifyOnElementType * event)
#else  /* __STDC__ */
ThotBool              ElemToTransform (event)
NotifyOnElementType * event;
#endif  /* __STDC__ */
{
   return (!TransformIntoType(event->targetElementType, event->document));
}

#endif /*IV*/

/*----------------------------------------------------------------------
   SetCharFontOrPhrase                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetCharFontOrPhrase (int document, int elemtype)
#else  /* __STDC__ */
void                SetCharFontOrPhrase (document, elemtype)
int                 document;
int                 elemtype;

#endif /* __STDC__ */
{
   Element             selectedEl, elem, firstSelectedElem, lastSelectedElem,
                       child, next, elFont, lastEl;
   ElementType         elType, selType;
   DisplayMode         dispMode;
   int                 length, firstSelectedChar, lastSelectedChar, i;
   ThotBool            remove, done, toset;

   toset = TRUE;
   TtaClearViewSelections ();
   dispMode = TtaGetDisplayMode (document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, DeferredDisplay);
   TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
				&lastSelectedChar);
   /* get the first leaf in the first selected element */
   elem = selectedEl;
   do
     {
	child = TtaGetFirstChild (elem);
	if (child != NULL)
	   elem = child;
     }
   while (child != NULL);
   firstSelectedElem = elem;

   /* If the first leaf of the first selected element is within an element */
   /* of the requested type, the text leaves of selected elements should not */
   /* be any longer within an element of that type */
   /* else, they should all be within an element of that type */
   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = elemtype;
   remove = (TtaGetTypedAncestor (elem, elType) != NULL);

   /* split the last selected elements if it's a text leaf */
   TtaGiveLastSelectedElement (document, &lastEl, &i, &lastSelectedChar);
   TtaUnselect (document);

   TtaOpenUndoSequence (document, selectedEl, lastEl, firstSelectedChar,
			lastSelectedChar);

   selType = TtaGetElementType (lastEl);
   if (selType.ElTypeNum == HTML_EL_TEXT_UNIT)
     /* the last selected element is a text leaf */
     {
     /* is this element within an element of the requested type ? */
     done = FALSE;
     elFont = TtaGetTypedAncestor (lastEl, elType);
     if (remove)
	/* the element has to be removed from an element of type elType */
	/* If it is not within such an element, nothing to do */
	done = (elFont == NULL);
     else
	/* the element should be within an element of type elType */
	/* If it is already within such an element, nothing to do */
	done = (elFont != NULL);
     if (!done)
       {
	  /* split that text leaf if it is not entirely selected */
	  length = TtaGetTextLength (lastEl);
	  if (lastSelectedChar != 0 && lastSelectedChar < length)
	     {
	       TtaRegisterElementReplace (lastEl, document);	     
	       TtaSplitText (lastEl, lastSelectedChar, document);
	       elem = lastEl;
	       TtaNextSibling (&elem);
	       TtaRegisterElementCreate (elem, document);
	     }
       }
     }
   /* get the last leaf in the last selected element */
   elem = lastEl;
   do
     {
	child = TtaGetLastChild (elem);
	if (child != NULL)
	   elem = child;
     }
   while (child != NULL);
   lastSelectedElem = elem;

   /* split the first selected elements if it's a text leaf */
   selType = TtaGetElementType (selectedEl);
   if (selType.ElTypeNum == HTML_EL_TEXT_UNIT)
     /* the first selected element is a text leaf */
     {
     /* is this element within an element of the requested type ? */
     done = FALSE;
     elFont = TtaGetTypedAncestor (selectedEl, elType);
     if (remove)
	/* the element has to be removed from an element of type elType */
	/* If it is not within such an element, nothing to do */
	done = (elFont == NULL);
     else
	/* the element should be within an element of type elType */
	/* If it is already within such an element, nothing to do */
	done = (elFont != NULL);
     if (!done)
       {
	  /* split that text leaf if it is not entirely selected */
	  if (firstSelectedChar > 1)
	    {
	       elem = selectedEl;
	       TtaRegisterElementReplace (selectedEl, document);
	       TtaSplitText (selectedEl, firstSelectedChar - 1, document);
	       TtaNextSibling (&selectedEl);
	       TtaRegisterElementCreate (selectedEl, document);
	       firstSelectedElem = selectedEl;
	       firstSelectedChar = 0;
	       if (elem == lastEl)
		  lastEl = selectedEl;	       
	    }
       }
     }

   /* process all selected elements */
   elem = NULL;
   while (selectedEl != NULL)
     {
	/* get the element to be processed after the current element: the */
	/* current element may change during processing */
	if (selectedEl == lastEl)
	    next = NULL;
	else
	   {
	   next = selectedEl;
	   TtaGiveNextElement (document, &next, lastEl);
	   }
	selType = TtaGetElementType (selectedEl);
	if (!TtaIsLeaf (selType))
	  {
	    /* this selected element is not a leaf. Process all text leaves */
	    /* of that element */
	    SetFontOrPhraseOnElement ((Document) document, selectedEl,
				      elemtype, remove);
	    elem = selectedEl;
	    toset = FALSE;
	  }
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
		/* the element should be within an element of type elType */
		/* If it is already within such an element, nothing to do */
		done = (elFont != NULL);
	     if (!done)
	       {
		  /* process the text leaf */
		  elem = selectedEl;
		  if (remove)
		     ResetFontOrPhraseOnText (document, elem, elemtype);
		  else
		     SetFontOrPhraseOnText (document, elem, elemtype);
	       }
	  }
	/* next selected element */
	selectedEl = next;
     }

   TtaCloseUndoSequence (document);

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
   if (toset)
     {
       switch (elemtype)
	 {
	    case HTML_EL_Emphasis:
	       SelectionInEM = !remove;
	       break;
	    case HTML_EL_Strong:
	       SelectionInSTRONG = !remove;
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
	    case HTML_EL_Cite:
	       SelectionInCITE = !remove;
	       break;
	    case HTML_EL_ABBR:
	       SelectionInABBR = !remove;
	       break;
	    case HTML_EL_ACRONYM:
	       SelectionInACRONYM = !remove;
	       break;
	    case HTML_EL_INS:
	       SelectionInINS = !remove;
	       break;
	    case HTML_EL_DEL:
	       SelectionInDEL = !remove;
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
}
