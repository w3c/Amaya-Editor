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
 *          R.Guetari (W3C/INRIA) - Unicode and Windows version.
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "MathML.h"
#ifdef GRAPHML
#include "GraphML.h"
#endif

#include "css_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "html2thot_f.h"
#include "selection.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "XMLparser_f.h"

#ifdef ANNOTATIONS
#include "ANNOTevent_f.h"
#endif /* ANNOTATIONS */

#ifdef _WINDOWS
#include "wininclude.h"

HWND currentWindow = NULL;
static CHAR_T WIN_buffer [1024];
#endif /* _WINDOWS */

/* info about the last element highlighted when synchronizing with the
   source view */
Document	HighlightDocument = 0;
Element		HighlightElement = NULL;
Attribute	HighLightAttribute = NULL;

/* Some prototypes */
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

   elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), document);
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

   elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), document);
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
   SearchNAMEattribute
   Search in document doc an element having an attribut of type attrType
   whose value is nameVal.
   Return that element or NULL if not found.
   If ignore is not NULL, it is an attribute that should be ignored when
   comparing attributes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element GetElemWithAttr (Document doc, AttributeType attrType,
				STRING nameVal, Attribute ignore)
#else  /* __STDC__ */
static Element GetElemWithAttr (doc, attrType, nameVal, Attribute ignore)
Document            doc;
AttributeType	    attrType;
STRING              nameVal;
Attribute           ignore;

#endif /* __STDC__ */

{
   Element             el, elFound;
   Attribute           nameAttr;
   ThotBool            found;
   int                 length;
   STRING              name;

   elFound = NULL;
   el = TtaGetMainRoot (doc);
   found = FALSE;
   do
     {
	TtaSearchAttribute (attrType, SearchForward, el, &elFound, &nameAttr);
	if (nameAttr != NULL && elFound != NULL)
	   if (nameAttr != ignore)
	     {
		length = TtaGetTextAttributeLength (nameAttr);
		length++;
		name = TtaAllocString (length);
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
      elFound = NULL;
   return elFound;
}

/*----------------------------------------------------------------------
   SearchNAMEattribute
   search in document doc an element having an attribut NAME or ID (defined
   in DTD HTML, MathML or GraphML) whose value is nameVal.         
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
   Element             elFound;
   AttributeType       attrType;

   /* search all elements having an attribute NAME */
   attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
   attrType.AttrTypeNum = HTML_ATTR_NAME;
   elFound = GetElemWithAttr (doc, attrType, nameVal, ignore);

   if (!elFound)
     {
       /* search all elements having an attribute ID */
       attrType.AttrTypeNum = HTML_ATTR_ID;
       elFound = GetElemWithAttr (doc, attrType, nameVal, ignore);
     }
   if (!elFound)
     {
       /* search all elements having an attribute ID (defined in the
	  MathML DTD) */
       attrType.AttrSSchema = TtaGetSSchema (TEXT("MathML"), doc);
       if (attrType.AttrSSchema)
	  /* this document uses the MathML DTD */
	  {
          attrType.AttrTypeNum = MathML_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignore);
	  }
     }
#ifdef GRAPHML
   if (!elFound)
     {
       /* search all elements having an attribute ID (defined in the
	  GraphML DTD) */
       attrType.AttrSSchema = TtaGetSSchema (TEXT("GraphML"), doc);
       if (attrType.AttrSSchema)
	  /* this document uses the GraphML DTD */
	  {
          attrType.AttrTypeNum = GraphML_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignore);
	  }
     }
#endif
   return (elFound);
}


/*----------------------------------------------------------------------
   FollowTheLink_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               FollowTheLink_callback (int targetDocument, int status, 
					   STRING urlName,
					   STRING outputfile, 
					   AHTHeaders *http_headers,
					   void * context)
#else  /* __STDC__ */
void               FollowTheLink_callback (targetDocument, status, urlName,
                                             outputfile, http_headers,
                                             context)
int TargetDocument;
int status;
STRING url, urlName;
STRING outputfile;
AHTHeaders *http_headers;
void *context;

#endif
{
  Element             elFound=NULL;
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
		TtaSetAttributeText (PseudoAttr, TEXT("visited"), anchor, doc);
	    }
	  else
	    TtaSetAttributeText (PseudoAttr, TEXT("visited"), anchor, doc);
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
		  AddDocHistory (doc, DocumentURLs[doc], DocumentMeta[doc]->form_data, DocumentMeta[doc]->method);
		}
	      /* show the target element in all views */
	      for (view = 1; view < 6; view++)
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
static ThotBool  FollowTheLink (Element anchor, Element elSource, Document doc)
#else  /* __STDC__ */
static ThotBool  FollowTheLink (anchor, elSource, doc)
Element          anchor;
Element          elSource;
Document         doc;
#endif /* __STDC__ */
{
   AttributeType          attrType;
   Attribute              HrefAttr, PseudoAttr, attr;
#ifdef ANNOTATIONS
   Attribute              isAnnotLink;
#endif /* ANNOTATIONS */
   ElementType            elType;
   Document               targetDocument;
   SSchema                HTMLSSchema;
   CHAR_T                 documentURL[MAX_LENGTH];
   STRING                 url, info, sourceDocUrl;
   int                    length;
   ThotBool		  isHTML;
   FollowTheLink_context *ctx;

   info = NULL;
   HrefAttr = NULL;
#ifdef ANNOTATIONS
   isAnnotLink = NULL;
#endif /* ANNOTATIONS */
   HTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);

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
	url = TtaAllocString (length);
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
		  TtaSetAttributeText (PseudoAttr, TEXT("active"), anchor, doc);

#ifdef ANNOTATIONS
		  /* see if it's a link to an annotation */
		  attrType.AttrTypeNum = HTML_ATTR_Annotation;
		  isAnnotLink = TtaGetAttribute (anchor, attrType);
#endif /* ANNOTATIONS */
	       }
	     /* get the URL itself */
	     TtaGiveTextAttributeValue (HrefAttr, url, &length);

	     /* suppress white spaces at the end */
	     length--;
	     while (url[length] == ' ')
		url[length--] = EOS;
	     /* save the complete URL of the source document */
	     length = ustrlen (DocumentURLs[doc])+1;
	     sourceDocUrl = TtaAllocString (length);
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
#ifdef ANNOTATIONS
		 targetDocument = GetHTMLDocument (documentURL, NULL,
						   (isAnnotLink) ? 0 : doc,
						   doc,
						   (isAnnotLink) ? CE_ANNOT : CE_RELATIVE, 
						   TRUE, 
						   (void *) FollowTheLink_callback,
						   (void *) ctx);
#else
		 targetDocument = GetHTMLDocument (documentURL, NULL,
						   doc, 
						   doc, 
						   CE_RELATIVE, TRUE, 
						   (void *) FollowTheLink_callback,
						   (void *) ctx);
#endif /* ANNOTATIONS */
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
  ActivateElement    The user has activated an element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ActivateElement (Element element, Document document)
#else  /* __STDC__ */
static ThotBool     ActivateElement (element, document)
Element             element;
Document            document;
#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             anchor, elFound, ancestor;
   ElementType         elType, elType1;
   ThotBool	       ok, isHTML;

   elType = TtaGetElementType (element);
   isHTML = (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0);

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
	    elType.ElTypeNum == HTML_EL_FRAME ||
	    elType.ElTypeNum == HTML_EL_Anchor)
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
	     StopTransfer (document, 1);	   
	     SubmitForm (document, element);
	   }
	else if (elType1.ElTypeNum == HTML_EL_BUTTON)
	   DblClickOnButton (element, document);
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
	  DblClickOnButton (elFound, document);
	  return (TRUE);
	 }
       else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_IsInput;
	   attr = TtaGetAttribute (element, attrType);
	   if (attr)
	     /* it's a input image */
	     {
	       /* interrupt current transfer */
	       StopTransfer (document, 1);	   
	       SubmitForm (document, element);
	       return (TRUE);
	     }
	 }
     }

   if (isHTML && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     {
	/* is it an option menu ? */
	elFound = TtaGetParent (element);
	elType1 = TtaGetElementType (elFound);
	if (elType1.ElTypeNum == HTML_EL_Option)
	  {
	     SelectOneOption (document, elFound);
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
	     SelectOneOption (document, elFound);
	     return (TRUE);
	  }
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_Checkbox_Input)
     {
	SelectCheckbox (document, element);
	return (TRUE);
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_Radio_Input)
     {
	SelectOneRadio (document, element);
	return (TRUE);
     }
   else if (isHTML && elType.ElTypeNum == HTML_EL_File_Input)
     {
	ActivateFileInput (document, element);
	return (TRUE);
     }

   /* Search the anchor or LINK element */
   anchor = SearchAnchor (document, element, TRUE, FALSE);
   if (anchor == NULL)
      if (isHTML && (elType.ElTypeNum == HTML_EL_LINK ||
		     elType.ElTypeNum == HTML_EL_FRAME))
	   anchor = element;
      else
	{
	   elType1.ElTypeNum = HTML_EL_LINK;
	   elType1.ElSSchema = TtaGetSSchema (TEXT("HTML"), document);
	   anchor = TtaGetTypedAncestor (element, elType1);
	}
   /* if not found, search a cite or href attribute on an ancestor */
   if (anchor == NULL)
      {
	ancestor = element;
	attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), document);
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

   return (FollowTheLink (anchor, element, document));
}

/*----------------------------------------------------------------------
  DoAction         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            DoAction (Document doc, View view)
#else  /* __STDC__ */
void            DoAction (doc, view)
Document        doc;
View            view;
#endif /* __STDC__ */
{
   Element             firstSel;
   int                 firstChar, lastChar;

   TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
   if (firstSel)
     ActivateElement (firstSel, doc);
}

/*----------------------------------------------------------------------
  DoubleClick     The user has double-clicked an element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            DoubleClick (NotifyElement *event)
#else  /* __STDC__ */
ThotBool            DoubleClick (event)
NotifyElement      *event;
#endif /* __STDC__ */
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);  
  if (usedouble)
    /* don't let Thot perform normal operation */
    return (ActivateElement (event->element, event->document));
  else
    return TRUE;
}


/*----------------------------------------------------------------------
  SimpleClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            SimpleClick (NotifyElement *event)
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
    return (ActivateElement (event->element, event->document));
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
#ifndef _GTK
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
	text = TtaAllocString (length);
	TtaGiveTextContent (textElem, text, &length, &lang);
	UpdateAtom (doc, DocumentURLs[doc], text);
        TtaChangeWindowTitle (doc, 0, text);
    	if (DocumentSource[doc])
    	   TtaChangeWindowTitle (DocumentSource[doc], 0, text);
	TtaFreeMemory (text);
     }
#endif /* _GTK */
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
  STRING             tempdocument;
  Document	     sourceDoc;
  CHAR_T               htmlErrFile [80];
  int			i;

  if (doc == 0)
    return;

  TtaSetItemOff (doc, 1, Special, BShowLogFile);
  HTMLErrorsFound = FALSE;

  if (DocumentURLs[doc] != NULL)
    {
      /* remove the temporary copy of the file */
      tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
      TtaFileUnlink (tempdocument);
      TtaFreeMemory (tempdocument);

      if (IsHTTPPath (DocumentURLs[doc]))
	{
	  usprintf (htmlErrFile, TEXT("%s%c%d%cHTML.ERR"), TempFileDirectory, DIR_SEP, doc, DIR_SEP);
	  if (TtaFileExist (htmlErrFile))
	    TtaFileUnlink (htmlErrFile);
	}

      if (DocumentTypes[doc] == docImage)
	DocumentTypes[doc] = docHTML;
      else if (DocumentTypes[doc] == docImageRO)
	DocumentTypes[doc] = docHTMLRO;

      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = NULL;
      if (DocumentMeta[doc])
	{
        if (DocumentMeta[doc]->form_data)
	  TtaFreeMemory (DocumentMeta[doc]->form_data);
        TtaFreeMemory (DocumentMeta[doc]);
        DocumentMeta[doc] = NULL;
	}
#ifdef ANNOTATIONS
       LINK_DelMetaFromMemory (doc);
#endif /* ANNOTATIONS */
      if (HighlightDocument == doc)
	ResetHighlightedElement ();
      if (DocumentSource[doc])
	{
	sourceDoc = DocumentSource[doc];
	TtcCloseDocument (sourceDoc, 1);
	FreeDocumentResource (sourceDoc);
        DocumentSource[doc] = 0;
	}
      /* is this document the source of another document? */
      for (i = 1; i < DocumentTableLength; i++)
         if (DocumentURLs[i] != NULL)
	    if (DocumentSource[i] == doc)
	       DocumentSource[i] = 0;
      RemoveDocCSSs (doc);
      /* avoid to free images of backup documents */
      if (BackupDocument != doc)
	RemoveDocumentImages (doc);
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
#ifdef ANNOTATIONS
	/* a quick hack before the commit */
        elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), doc);
#else
	elType.ElSSchema = TtaGetDocumentSSchema (doc);
#endif /* ANNOTATIONS */
	elType.ElTypeNum = HTML_EL_Preformatted;
	NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (NewSelInElem != SelectionInPRE)
     {
	SelectionInPRE = NewSelInElem;
	if (NewSelInElem)
	  {
	     TtaSetItemOff (doc, 1, Types, BImage);
	     TtaSetItemOff (doc, 1, Types, BObject);
	     TtaSetItemOff (doc, 1, Style, TBig);
	     TtaSetItemOff (doc, 1, Style, TSmall);
	     TtaSetItemOff (doc, 1, Style, TSub);
	     TtaSetItemOff (doc, 1, Style, TSup);
	  }
	else
	  {
	     TtaSetItemOn (doc, 1, Types, BImage);
	     TtaSetItemOn (doc, 1, Types, BObject);
	     TtaSetItemOn (doc, 1, Style, TBig);
	     TtaSetItemOn (doc, 1, Style, TSmall);
	     TtaSetItemOn (doc, 1, Style, TSub);
	     TtaSetItemOn (doc, 1, Style, TSup);
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
	TtaSwitchButton (doc, 1, iI);
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
	TtaSwitchButton (doc, 1, iB);
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
	TtaSwitchButton (doc, 1, iT);
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

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Subscript;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSub != NewSelInElem)
     {
	SelectionInSub = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TSub, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Superscript;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSup != NewSelInElem)
     {
	SelectionInSup = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TSup, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Quotation;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInQuote != NewSelInElem)
     {
	SelectionInQuote = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TQuotation, NewSelInElem);
     }

   if (firstSel == NULL)
      NewSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_BDO;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   NewSelInElem = TRUE;
	else
	   NewSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInBDO != NewSelInElem)
     {
	SelectionInBDO = NewSelInElem;
	TtaSetToggleItem (doc, 1, Style, TBDO, NewSelInElem);
     }
}

/*----------------------------------------------------------------------
   LineNumberOfEl
   Returns the line number (position in the source file) of element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int LineNumberOfEl (Element el)
#else  /* __STDC__ */
static int LineNumberOfEl (el)
Element	el;
#endif /* __STDC__ */
{
   int		ln;
   Element	child, sibling, uncle, ancestor, prev, parent;

   ln = TtaGetElementLineNumber (el);
   if (ln == 0)
      /* there is no line number associated with this element: the element
         does not exist in the source file */
      {
      /* get the first line number associated with its descendants */
      child = TtaGetFirstChild (el);
      while (child && ln == 0)
	 {
	 ln = LineNumberOfEl (child);
	 if (ln == 0)
	    TtaNextSibling (&child);
	 }
      if (ln == 0)
         /* Descendants don't have any line number. Get the first line number
	    associated with its following siblings */
	 {
	 sibling = el;
	 do
	    {
	    TtaNextSibling (&sibling);
	    if (sibling)
	       ln = LineNumberOfEl (sibling);
	    }
	 while (sibling && ln == 0);
	 }
      if (ln == 0)
         /* Siblings don't have any line number. Get the first line number
	    associated with the following siblings of its ancestors */
	 {
	 ancestor = el;
	 do
	    {
	    ancestor = TtaGetParent (ancestor);
	    if (ancestor)
	       {
	       uncle = ancestor;
	       do
		  {
	          TtaNextSibling (&uncle);
	          if (uncle)
		     ln = LineNumberOfEl (uncle);
		  }
	       while (uncle && ln == 0);
	       }
	    }
	 while (ancestor && ln == 0);
	 }
      if (ln == 0)
         /* Still no line number. Get the line number of the previous
	    element with a line number */
	 {
	 ancestor = el;
         prev = el;
	 TtaPreviousSibling (&prev);
	 while (prev == NULL && ln == 0 && ancestor != NULL)
	    {
	    ancestor = TtaGetParent (ancestor);
	    ln = TtaGetElementLineNumber (ancestor);
	    if (ln == 0)
	       {
	       prev = ancestor;
	       TtaPreviousSibling (&prev);
	       }
	    }
	 ancestor = prev;

	 while (ancestor && ln == 0)
	    {
	    prev = TtaGetLastLeaf (ancestor);
	    if (prev)
	       {
	       parent = TtaGetParent (prev);
	       while (prev && ln == 0)
		  {
		  ln = TtaGetElementLineNumber (prev);
		  if (ln == 0)
		     TtaPreviousSibling (&prev);
		  }
	       if (ln == 0)
		  ln = TtaGetElementLineNumber (parent);
	       if (ln == 0)
		  ancestor = TtaGetPredecessor (parent);
	       }
	    }
	 }
      }
   return ln;
}

/*----------------------------------------------------------------------
   ResetHighlightedElement
   If an element is currently highlighted, remove its Highlight attribute
  ----------------------------------------------------------------------*/
void                ResetHighlightedElement ()
{
   if (HighlightElement)
      {
      if (TtaGetElementType (HighlightElement).ElSSchema != NULL)
         TtaRemoveAttribute (HighlightElement, HighLightAttribute,
			     HighlightDocument);
      HighlightDocument = 0;
      HighlightElement = NULL;
      HighLightAttribute = NULL;
      }
}

/*----------------------------------------------------------------------
   SynchronizeSourceView
   A new element has been selected. If the Source view is open,
   synchronize it with the new selection.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SynchronizeSourceView (NotifyElement * event)
#else  /* __STDC__ */
void                SynchronizeSourceView (event)
NotifyElement* event;
#endif				/* __STDC__ */
{
   Element             firstSel, el, child, otherEl;
   int                 firstChar, lastChar, line, i, view;
   AttributeType       attrType;
   Attribute	       attr;
   Document	       doc, otherDoc;
   int		       val, x, y, width, height;
   ThotBool	       otherDocIsHTML, done;

   if (!event)
       return;
   doc = event->document;
   done = FALSE;
   /* get the other Thot document to be synchronized with the one where the
      user has just clicked */
   otherDoc = 0;
   otherDocIsHTML = FALSE;
   if (DocumentTypes[doc] == docHTML || DocumentTypes[doc] == docHTMLRO)
      /* the user clicked on a HTML document, the other doc is the
         corresponding source document */
      otherDoc = DocumentSource[doc];
   else if (DocumentTypes[doc] == docSource ||
	    DocumentTypes[doc] == docSourceRO)
      /* the user clicked on a source document, the other doc is the
         corresponding HTML document */
      {
      otherDocIsHTML = TRUE;
      for (i = 1; i < DocumentTableLength; i++)
         if (DocumentURLs[i] != NULL)
	    if (DocumentTypes[i] == docHTML ||
		DocumentTypes[i] == docHTMLRO)
	       if (DocumentSource[i] == doc)
		  {
	          otherDoc = i;
		  i = DocumentTableLength;
		  }
      }
   if (otherDoc)
      /* looks for the element in the other document that corresponds to
         the clicked element */
      {
      TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
      if (firstSel)
	 {
	 otherEl = NULL;
	 /* Get the line number associated with the clicked element */
	 line = LineNumberOfEl (firstSel);
	 if (line == 0)
	    return;
	 /* look for an element with the same line number in the other doc */
	 /* line numbers are increasing in document order */
	 el = TtaGetMainRoot (otherDoc);
	 do
	    {
	    if (TtaGetElementLineNumber (el) >= line)
	       /* that's the right element */
	       otherEl = el;
	    else
	       {
	       child = TtaGetFirstChild (el);
	       if (!child)
		  otherEl = el;
	       else
		  {
		  do
		     {
		     el = child;
		     TtaNextSibling (&child);
		     }
		  while (child && LineNumberOfEl (child) <= line);
	          }
	       }
	    }
	 while (!otherEl && el);

	 if (otherEl && otherEl != HighlightElement)
	    /* element found */
	    {
	    /* If an element is currently highlighted, remove its Highlight
	       attribute */
	    ResetHighlightedElement ();
	    /* Put a Highlight attribute on the element found */
	    if (otherDocIsHTML)
	       {
	       attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), otherDoc);
	       attrType.AttrTypeNum = HTML_ATTR_Highlight;
	       val = HTML_ATTR_Highlight_VAL_Yes_;
	       }
	    else
	       {
	       attrType.AttrSSchema = TtaGetSSchema (TEXT("TextFile"), otherDoc);
	       attrType.AttrTypeNum = TextFile_ATTR_Highlight;
	       val = TextFile_ATTR_Highlight_VAL_Yes_;
	       }
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (otherEl, attr, otherDoc);
	    TtaSetAttributeValue (attr, val, otherEl, otherDoc);
	    /* record the highlighted element */
	    HighlightDocument = otherDoc;
	    HighlightElement = otherEl;
	    HighLightAttribute = attr;
	    /* Scroll all views where the element appears to show it */
	    for (view = 1; view < 6; view++)
	       if (TtaIsViewOpened (otherDoc, view))
		  {
		  TtaGiveBoxAbsPosition (otherEl, otherDoc, view, UnPixel, &x, &y);
		  TtaGiveWindowSize (otherDoc, view, UnPixel, &width, &height);
		  if (y < 0 || y > height - 15)
		     TtaShowElement (otherDoc, view, otherEl, 25);
		  }
	    }
	 done = TRUE;
	 }
      }
   if (!done)
      /* If an element is currently highlighted, remove its Highlight
	 attribute */
      ResetHighlightedElement ();
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
   SynchronizeSourceView (event);
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

   if (!TtaGetDocumentAccessMode (document))
     return;
   TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
				&lastSelectedChar);
   if (selectedEl == NULL)
     /* no selection available */
     return;

   toset = TRUE;
   TtaClearViewSelections ();
   dispMode = TtaGetDisplayMode (document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, DeferredDisplay);
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
	       if (lastSelectedElem == firstSelectedElem)
		 {
		   lastSelectedElem = selectedEl;
		   lastSelectedChar = lastSelectedChar - firstSelectedChar + 1;
		 }
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
	    case HTML_EL_Subscript:
	       SelectionInSub = !remove;
	       break;
	    case HTML_EL_Superscript:
	       SelectionInSup = !remove;
	       break;
	    case HTML_EL_Quotation:
	       SelectionInQuote = !remove;
	       break;
	    case HTML_EL_BDO:
	       SelectionInBDO = !remove;
	       break;
	    default:
	       break;
	 }
     }
}
